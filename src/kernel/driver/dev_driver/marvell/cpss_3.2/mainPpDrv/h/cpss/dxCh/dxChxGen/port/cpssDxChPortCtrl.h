/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortCtrl.h
*
* DESCRIPTION:
*       CPSS implementation for Port configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 25 $
*******************************************************************************/

#ifndef __cpssDxChPortCtrlh
#define __cpssDxChPortCtrlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

/* definition for the number of PSC lanes per port */
#define CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS 4

/*
 * typedef: enum CPSS_DXCH_PORT_SERDES_MODE_ENT
 *
 * Description: Enumeration of Serdes modes
 *
 * Enumerations:
 *    CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E - Regular Serdes are active
 *    CPSS_DXCH_PORT_SERDES_MODE_COMBO_E - Combo Serdes are active
 */
typedef enum
{
    CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E,
    CPSS_DXCH_PORT_SERDES_MODE_COMBO_E

}CPSS_DXCH_PORT_SERDES_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT
 *
 * Description: Enumeration of the MAC error types.
 *
 * Enumerations:
 *  CPSS_DXCH_PORT_MAC_ERROR_JABBER_E    - Jabber error type
 *  CPSS_DXCH_PORT_MAC_ERROR_FRAGMENTS_E - Fragment error type
 *  CPSS_DXCH_PORT_MAC_ERROR_UNDERSIZE_E - Undersize error type
 *  CPSS_DXCH_PORT_MAC_ERROR_OVERSIZE_E  - Oversize error type
 *  CPSS_DXCH_PORT_MAC_ERROR_RX_E        - Rx error type
 *  CPSS_DXCH_PORT_MAC_ERROR_CRC_E       - CRC error type
 *  CPSS_DXCH_PORT_MAC_ERROR_OVERRUN_E   - Overrun error type
 *  CPSS_DXCH_PORT_MAC_ERROR_NO_E        - No error
 *
 * Comments:
 *         None.
 */
typedef enum
{
    CPSS_DXCH_PORT_MAC_ERROR_JABBER_E,
    CPSS_DXCH_PORT_MAC_ERROR_FRAGMENTS_E,
    CPSS_DXCH_PORT_MAC_ERROR_UNDERSIZE_E,
    CPSS_DXCH_PORT_MAC_ERROR_OVERSIZE_E,
    CPSS_DXCH_PORT_MAC_ERROR_RX_E,
    CPSS_DXCH_PORT_MAC_ERROR_CRC_E,
    CPSS_DXCH_PORT_MAC_ERROR_OVERRUN_E,
    CPSS_DXCH_PORT_MAC_ERROR_NO_E

}CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT;

/*
 * typedef: enum CPSS_DXCH_PORT_FC_MODE_ENT
 *
 * Description: Enumeration of FC modes.
 *
 * Enumerations:
 *  CPSS_DXCH_PORT_FC_MODE_802_3X_E  - 802.3X
 *  CPSS_DXCH_PORT_FC_MODE_PFC_E     - PFC: Priority Flow Control
 *  CPSS_DXCH_PORT_FC_MODE_LL_FC_E   - LL FC: Link Level Flow control
 *
 * Comments:
 *         None.
 */
typedef enum
{
    CPSS_DXCH_PORT_FC_MODE_802_3X_E,
    CPSS_DXCH_PORT_FC_MODE_PFC_E,
    CPSS_DXCH_PORT_FC_MODE_LL_FC_E
}CPSS_DXCH_PORT_FC_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_PORT_SERDES_CONFIG_STC
 *
 * Description: Port SERDES configuration.
 *
 * Fields:
 *      txAmp       - Tx Driver output amplitude(valid values range 0-31).
 *      txEmphEn    - Pre- or de-emphasis enable signal(GT_TRUE - enable, GT_FALSE - disable).
 *      txEmphAmp   - Emphasis level control bits(valid values range 0-15).
 *      txAmpAdj    - Transmitter Amplitude Adjust.
 *      txEmphLevelAdjEnable - Transmitter Emphasis Level Adjust Enable.
 *      ffeSignalSwingControl - FFE signal swing control.
 *      ffeResistorSelect - Feed Forward Equalization (FFE) Resistor Select.
 *      ffeCapacitorSelect - Feed Forward Equalization (FFE) Capacitor Select.
 * Comments:
 *      1. Relevant fields for DxCh3: txAmp, txEmphEn, txEmphAmp.
 *      2. Relevant fields for DxChXcat and above: txAmp, txEmphAmp, txAmpAdj,
 *         txEmphLevelAdjEnable, ffeSignalSwingControl, ffeResistorSelect,
 *         ffeCapacitorSelect.
 *      3. For further information, contact your Marvell representative.
 */
typedef struct {
    GT_U32 txAmp;
    GT_BOOL txEmphEn;
    GT_U32 txEmphAmp;
    GT_U32 txAmpAdj;
    GT_BOOL txEmphLevelAdjEnable;
    GT_U32 ffeSignalSwingControl;
    GT_U32 ffeResistorSelect;
    GT_U32 ffeCapacitorSelect;
} CPSS_DXCH_PORT_SERDES_CONFIG_STC;

                            
/*******************************************************************************
* cpssDxChPortMacSaLsbSet
*
* DESCRIPTION:
*       Set the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       macSaLsb    - The ls byte of the MAC SA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       see srcAddrMiddle and srcAddrHigh as related parameters.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaLsbSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U8    macSaLsb
);

/*******************************************************************************
* cpssDxChPortMacSaLsbGet
*
* DESCRIPTION:
*       Get the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       macSaLsbPtr - (pointer to) The ls byte of the MAC SA
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       see srcAddrMiddle and srcAddrHigh as related parameters.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaLsbGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U8    *macSaLsbPtr
);

/*******************************************************************************
* cpssDxChPortEnableSet
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status of a specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       statePtr - Pointer to the Get Enable/disable state of the port.
*                  GT_TRUE for enabled port, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *statePtr
);

/*******************************************************************************
* cpssDxChPortDuplexModeSet
*
* DESCRIPTION:
*       Set the port to enable and the port mode to half- or full-duplex mode.
*       When port is disabled it does not transmit pending packets and
*       drops packets received from the line.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       dMode    - 0 = The port operates in half-duplex mode.
*                  1 = The port operates in full-duplex mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or dMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported duplex mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The <SetFullDx> bit is meaningless when <ANDuplex> is set.
*       For CPU port, this field must be set to reflect this port Duplex mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexModeSet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    IN  CPSS_PORT_DUPLEX_ENT  dMode
);

/*******************************************************************************
* cpssDxChPortDuplexModeGet
*
* DESCRIPTION:
*       Gets duplex mode for specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       dModePtr - duplex mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexModeGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_DUPLEX_ENT  *dModePtr
);

/*******************************************************************************
* cpssDxChPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported speed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting.
*                                  (For DxCh3, not XG ports only.)
*
* COMMENTS:
*       1. If the port is enabled then the function disables the port before
*          the operation and re-enables it at the end.
*       2. For Flex-Link ports the interface mode should be
*          configured before port's speed, see cpssDxChPortInterfaceModeSet.
*          The following table summarizes all options:
*  Interface   |  Interface Mode                       |  Port Speed
*  ------------------------------------------------------------------------
*  1G SGMII    | CPSS_PORT_INTERFACE_MODE_SGMII_E      | CPSS_PORT_SPEED_1000_E
*  1000 BASE-X | CPSS_PORT_INTERFACE_MODE_1000BASE_X_E | CPSS_PORT_SPEED_1000_E
*  100 BAS-FX  | CPSS_PORT_INTERFACE_MODE_100BASE_FX_E | CPSS_PORT_SPEED_100_E
*  2.5G SGMII  | CPSS_PORT_INTERFACE_MODE_SGMII_E      | CPSS_PORT_SPEED_2500_E
*  QX          | CPSS_PORT_INTERFACE_MODE_QX_E         | CPSS_PORT_SPEED_2500_E
*  DQX         | CPSS_PORT_INTERFACE_MODE_QX_E         | CPSS_PORT_SPEED_5000_E
*  HX          | CPSS_PORT_INTERFACE_MODE_HX_E         | CPSS_PORT_SPEED_5000_E
*  DHX         | CPSS_PORT_INTERFACE_MODE_HX_E         | CPSS_PORT_SPEED_10000_E
*  XAUI        | CPSS_PORT_INTERFACE_MODE_XGMII_E      | CPSS_PORT_SPEED_10000_E
*  HGS         | CPSS_PORT_INTERFACE_MODE_XGMII_E      | CPSS_PORT_SPEED_12000_E
*
*       3.This API also checks all SERDES per port initialization. If serdes was
*         not initialized, proper init will be done
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

/*******************************************************************************
* cpssDxChPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
);


/*******************************************************************************
* cpssDxChPortDuplexAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for duplex mode on specified port on
*       specified device.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the duplex auto negotiation
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
);
/*******************************************************************************
* cpssDxChPortDuplexAutoNegEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of an Auto-Negotiation for MAC duplex mode
*       per port.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the duplex auto negotiation
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexAutoNegEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL    *statePtr
);

/*******************************************************************************
* cpssDxChPortFlowCntrlAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for Flow Control on
*       specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*       state          - GT_TRUE for enable Auto-Negotiation for Flow Control
*                        GT_FALSE otherwise
*       pauseAdvertise - Advertise symmetric flow control support in
*                        Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the flow control auto negotiation
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowCntrlAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state,
    IN  GT_BOOL   pauseAdvertise
);
/*******************************************************************************
* cpssDxChPortFlowCntrlAutoNegEnableGet
*
* DESCRIPTION:
*       Get Auto-Negotiation enable/disable state for Flow Control per port
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       statePtr       -   GT_TRUE for enable Auto-Negotiation for Flow Control
*                          GT_FALSE otherwise
*       pauseAdvertisePtr - Advertise symmetric flow control support in
*                           Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the flow control auto negotiation
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowCntrlAutoNegEnableGet
(
    IN   GT_U8       devNum,
    IN   GT_U8       portNum,
    OUT  GT_BOOL     *statePtr,
    OUT  GT_BOOL     *pauseAdvertisePtr
);


/*******************************************************************************
* cpssDxChPortSpeedAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation of interface speed on specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the speed auto negotiation
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
);

/*******************************************************************************
* cpssDxChPortSpeedAutoNegEnableGet
*
* DESCRIPTION:
*
*   Get status  of  Auto-Negotiation enable on specified port
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enabled Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the speed auto negotiation
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedAutoNegEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT  GT_BOOL  *statePtr
);

/*******************************************************************************
* cpssDxChPortFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex on specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - Flow Control state: Both disabled, 
*                  Both enabled, Only Rx or Only Tx enabled.
*                  Note: only XG ports can be configured in all 4 options, 
*                        Tri-Speed and FE ports may use only first two.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, state or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state
);

/*******************************************************************************
* cpssDxChPortFlowControlEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x Flow Control on specific logical port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - Pointer to Flow Control state: Both disabled, 
*                  Both enabled, Only Rx or Only Tx enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT      *statePtr
);


/*******************************************************************************
* cpssDxChPortPeriodicFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmits of periodic 802.3x flow control.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       enable   - If GT_TRUE, enable transmit.
*                  If GT_FALSE, disable transmit.
*
* OUTPUTS:
*       None.
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
GT_STATUS cpssDxChPortPeriodicFcEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortPeriodicFcEnableGet
*
* DESCRIPTION:
*       Get status of periodic 802.3x flow control transmition.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - Pointer to the periodic 802.3x flow control
*                    tramsition state.
*                    If GT_TRUE, enable transmit.
*                    If GT_FALSE, disable transmit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFcEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChPortBackPressureEnableSet
*
* DESCRIPTION:
*       Enable/disable of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for enable Back Pressure, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For XGMII (10 Gbps) ports feature is not supported.
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortBackPressureEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
);

/*******************************************************************************
* cpssDxChPortBackPressureEnableGet
*
* DESCRIPTION:
*       Gets the state of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - pointer to back pressure enable/disable state:
*                       - GT_TRUE to enable Back Pressure
*                       - GT_FALSE to disable Back Pressure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For XGMII (10 Gbps) ports feature is not supported.
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortBackPressureEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChPortLinkStatusGet
*
* DESCRIPTION:
*       Gets Link Status of specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       isLinkUpPtr - GT_TRUE for link up, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortLinkStatusGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *isLinkUpPtr
);

/*******************************************************************************
* cpssDxChPortInterfaceModeSet
*
* DESCRIPTION:
*       Sets Interface mode on a specified port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. For CPU port the interface may be one of GMII, RGMII, MII and MII
*        PHY. RGMII and MII PHY port interface modes are not supported for
*        DxCh3 CPU port.
*     2. For Tri-Speed ports the interface may be SGMII or 1000 BASE-X.
*     3. For Flex-Link ports the interface may be SGMII, 1000 BASE-X,
*        100 BASE-FX, QX, HX or XGMII.
*        Depends on capability of the specific device. The port's speed
*        should be set after configuring Interface Mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInterfaceModeSet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
);


/*******************************************************************************
* cpssDxChPortInterfaceModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortInterfaceModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
);

/*******************************************************************************
* cpssDxChPortForceLinkPassEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Pass on specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link pass, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
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
GT_STATUS cpssDxChPortForceLinkPassEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
);

/*******************************************************************************
* cpssDxChPortForceLinkDownEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Down on specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link down, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
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
GT_STATUS cpssDxChPortForceLinkDownEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
);


/*******************************************************************************
* cpssDxChPortMruSet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       mruSize    - max receive packet size in bytes. (0..16382)
*                    value must be even
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                          odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 16382
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruSet
(
    IN  GT_U8  devNum,
    IN  GT_U8  portNum,
    IN  GT_U32 mruSize
);


/*******************************************************************************
* cpssDxChPortMruGet
*
* DESCRIPTION:
*       Gets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       mruSizePtr - (pointer to) max receive packet size in bytes. (0..16382)
*                    value must be even
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                          odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 16382
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruGet
(
    IN   GT_U8  devNum,
    IN   GT_U8  portNum,
    OUT  GT_U32 *mruSizePtr
);


/*******************************************************************************
* cpssDxChPortCrcCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable 32-bit the CRC checking.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       enable     - If GT_TRUE, enable CRC checking
*                    If GT_FALSE, disable CRC checking
*
* OUTPUTS:
*       None.
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
GT_STATUS cpssDxChPortCrcCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChPortCrcCheckEnableGet
*
* DESCRIPTION:
*       Get CRC checking (Enable/Disable) state for received packets.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - Pointer to the CRS checking state :
*                       GT_TRUE  - CRC checking is enable,
*                       GT_FALSE - CRC checking is disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortCrcCheckEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChPortXGmiiModeSet
*
* DESCRIPTION:
*       Sets XGMII mode on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       mode  - XGMII mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXGmiiModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_XGMII_MODE_ENT    mode
);

/*******************************************************************************
* cpssDxChPortXGmiiModeGet
*
* DESCRIPTION:
*       Gets XGMII mode on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
* OUTPUTS:
*       modePtr  - Pointer to XGMII mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXGmiiModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    OUT CPSS_PORT_XGMII_MODE_ENT    *modePtr
);

/*******************************************************************************
* cpssDxChPortIpgBaseSet
*
* DESCRIPTION:
*       Sets IPG base for fixed IPG mode on XG ports.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       ipgBase  - IPG base
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when XG Port is in CPSS_PORT_XGMII_FIXED_E mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgBaseSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_XG_FIXED_IPG_ENT  ipgBase
);

/*******************************************************************************
* cpssDxChPortIpgBaseGet
*
* DESCRIPTION:
*       Gets IPG base for fixed IPG mode on XG ports.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
* OUTPUTS:
*       ipgBasePtr  - pointer to IPG base
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       Relevant only when XG Port is in CPSS_PORT_XGMII_FIXED_E mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgBaseGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    OUT CPSS_PORT_XG_FIXED_IPG_ENT  *ipgBasePtr
);

/****************************************************************************
* cpssDxChPortIpgSet
*
* DESCRIPTION:
*       Sets the Inter-Packet Gap (IPG) interval of a tri-speed physical port.
*       Using this API may be required to enable wire-speed in traffic paths
*       that include cascading ports, where it may not be feasible to reduce
*       the preamble length.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*       ipg         - IPG in bytes, acceptable range:
*                     [0..511] for DxCh3 and above devices.
*                     [0..15] for DxCh1 and DxCh2 devices.
*                     Default HW value is 12 bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on wrong port type
*       GT_OUT_OF_RANGE  - ipg value out of range
*       GT_BAD_PARAM     - on bad parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  ipg
);

/****************************************************************************
* cpssDxChPortIpgGet
*
* DESCRIPTION:
*       Gets the Inter-Packet Gap (IPG) interval of a tri-speed physical port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*
* OUTPUTS:
*       ipgPtr      - (pointer to) IPG value in bytes
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on wrong port type
*       GT_BAD_PARAM     - on bad parameter
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT  GT_U32  *ipgPtr
);

/*******************************************************************************
* cpssDxChPortExtraIpgSet
*
* DESCRIPTION:
*       Sets the number of 32-bit words to add to the 12-byte IPG.
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       number   -  number of words
*                   for DxCh,DxCh2  : number range 0..3
*                   DxCh3 and above : number range 0..127
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - the number is out of range
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortExtraIpgSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       number
);

/*******************************************************************************
* cpssDxChPortExtraIpgGet
*
* DESCRIPTION:
*       Gets the number of 32-bit words to add to the 12-byte IPG.
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       numberPtr -  pointer to number of words
*                   for DxCh,DxCh2  : number range 0..3
*                   DxCh3 and above : number range 0..127
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - the number is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortExtraIpgGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_U8       *numberPtr
);

/*******************************************************************************
* cpssDxChPortXgmiiLocalFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected local
*       fault messages.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       isLocalFaultPtr - GT_TRUE, if bit set or
*                         GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgmiiLocalFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *isLocalFaultPtr
);

/*******************************************************************************
* cpssDxChPortXgmiiRemoteFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected remote
*       fault messages.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number
*
* OUTPUTS:
*       isRemoteFaultPtr  - GT_TRUE, if bit set or
*                           GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgmiiRemoteFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *isRemoteFaultPtr
);

/*******************************************************************************
* cpssDxChPortMacStatusGet
*
* DESCRIPTION:
*       Reads bits that indicate different problems on specified port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portMacStatusPtr  - info about port MAC
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacStatusGet
(
    IN  GT_U8                      devNum,
    IN  GT_U8                      portNum,
    OUT CPSS_PORT_MAC_STATUS_STC   *portMacStatusPtr
);

/*******************************************************************************
* cpssDxChPortInternalLoopbackEnableSet
*
* DESCRIPTION:
*       Set the internal Loopback state in the packet processor MAC port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       LoopBack is enabled only on XG devices.
*       For port 25 in DX269 (XG/HX port) it's the application responsibility to
*       reconfigure the loopback mode after switching XG/HX mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInternalLoopbackEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChPortInternalLoopbackEnableGet
*
* DESCRIPTION:
*       Get Internal Loopback
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       enablePtr  - Pointer to the Loopback state.
*                    If GT_TRUE, loopback is enabled
*                    If GT_FALSE, loopback is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not relevant for the CPU port.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInternalLoopbackEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
);


/*******************************************************************************
* cpssDxChPortInbandAutoNegEnableSet
*
* DESCRIPTION:
*       Configure Auto-Negotiation mode of MAC for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                 GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - The feature is not supported the port/device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not supported for CPU port of DX devices and not supported for XG ports.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*       The change of the Auto-Negotiation causes temporary change of the link
*       to down and up for ports with link up.
*
*       Not relevant for the CPU port
*
*******************************************************************************/
GT_STATUS cpssDxChPortInbandAutoNegEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortInbandAutoNegEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation mode of MAC for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                    GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - The feature is not supported the port/device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not supported for CPU port of DX devices and not supported for XG ports.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*
*       Not relevant for the CPU port
*
*******************************************************************************/
GT_STATUS cpssDxChPortInbandAutoNegEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortAttributesOnPortGet
*
* DESCRIPTION:
*       Gets port attributes for particular logical port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (or CPU port)
*
* OUTPUTS:
*       portAttributSetArrayPtr - Pointer to attributes values array.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortAttributesOnPortGet
(
    IN    GT_U8                     devNum,
    IN    GT_U8                     portNum,
    OUT   CPSS_PORT_ATTRIBUTES_STC  *portAttributSetArrayPtr
);

/*******************************************************************************
* cpssDxChPortPreambleLengthSet
*
* DESCRIPTION:
*       Set the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction and "both directions"
*                      options (GE ports support only Tx direction)
*       length       - length of preamble in bytes
*                      support only values of 4,8
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPreambleLengthSet(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
);

/*******************************************************************************
* cpssDxChPortPreambleLengthGet
*
* DESCRIPTION:
*       Get the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction
*                      GE ports support only Tx direction.
*
*
* OUTPUTS:
*       lengthPtr    - pointer to preamble length in bytes :
*                      supported length values are : 4,8.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPreambleLengthGet(
    IN  GT_U8                    devNum,
    IN  GT_U8                    portNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_U32                   *lengthPtr
);

/*******************************************************************************
* cpssDxChPortMacSaBaseSet
*
* DESCRIPTION:
*       Sets the Source addresses of all of the device's ports.
*       This addresses is used as the MAC SA for Flow Control Packets
*       transmitted by the device.In addition this address is used for
*       reception of Flow Control packets.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum - device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
);

/*******************************************************************************
* cpssDxChPortMacSaBaseGet
*
* DESCRIPTION:
*       Sets the Source addresses of all of the device's ports.
*       This addresses is used as the MAC SA for Flow Control Packets
*       transmitted by the device.In addition this address is used for
*       reception of Flow Control packets.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       macPtr - (pointer to)The system Mac address.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_ETHERADDR    *macPtr
);

/*******************************************************************************
* cpssDxChPortSerdesComboModeSet
*
* DESCRIPTION:
*       Set Serdes mode. If Serdes mode is CPSS_DXCH_PORT_SERDES_MODE_COMBO_E,
*       Combo Serdes are active for the port, else regular Serdes are active for
*       the port.
*
* APPLICABLE DEVICES: DxCh3 Only.
*
* INPUTS:
*       devNum      - device number
*       portNum  - physical port number, values 20..23.
*       mode     - serdes mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum, mode
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesComboModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    IN  CPSS_DXCH_PORT_SERDES_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChPortSerdesComboModeGet
*
* DESCRIPTION:
*       Get Serdes mode. If Serdes mode is CPSS_DXCH_PORT_SERDES_MODE_COMBO_E,
*       Combo Serdes are active for the port, else regular Serdes are active for
*       the port.
*
* APPLICABLE DEVICES: DxCh3 Only.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number, values 20..23.
*
* OUTPUTS:
*       modePtr   -  pointer to Serdes mode.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesComboModeGet
(
    IN  GT_U8                             devNum,
    IN  GT_U8                             portNum,
    OUT CPSS_DXCH_PORT_SERDES_MODE_ENT    *modePtr
);

/*******************************************************************************
* cpssDxChPortSerdesConfigSet
*
* DESCRIPTION:
*       Set SERDES configuration per port
*
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number.
*       serdesCfgPtr    - pointer to port serdes configuration.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on SERDES parameters out of range
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This API also checks all SERDES per port initialization. If serdes was
*       not initialized, proper init will be done
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesConfigSet
(
    IN GT_U8  devNum,
    IN GT_U8  portNum,
    IN CPSS_DXCH_PORT_SERDES_CONFIG_STC  *serdesCfgPtr
);

/*******************************************************************************
* cpssDxChPortSerdesConfigGet
*
* DESCRIPTION:
*       Get SERDES configuration per port.
*
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number.
*
* OUTPUTS:
*       serdesCfgPtr    - pointer to port serdes configuration.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_INITIALIZED - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesConfigGet
(
    IN GT_U8  devNum,
    IN GT_U8  portNum,
    OUT CPSS_DXCH_PORT_SERDES_CONFIG_STC  *serdesCfgPtr
);

/*******************************************************************************
* cpssDxChPortPaddingEnableSet
*
* DESCRIPTION:
*       Enable/Disable padding of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - Pad short packet in Tx.
*                - GT_FALSE - No padding in short packets.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPaddingEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
);

/*******************************************************************************
* cpssDxChPortPaddingEnableGet
*
* DESCRIPTION:
*       Gets padding status of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr   - pointer to packet padding status.
*                     - GT_TRUE  - Pad short packet in Tx.
*                     - GT_FALSE - No padding in short packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPaddingEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
);

/*******************************************************************************
* cpssDxChPortExcessiveCollisionDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - if the number of collisions on the same packet
*                             is 16 the packet is dropped.
*                - GT_FALSE - A collided packet will be retransmitted by device
*                             until it is transmitted without collisions,
*                             regardless of the number of collisions on the packet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       The setting is not relevant in full duplex mode
*
*******************************************************************************/
GT_STATUS cpssDxChPortExcessiveCollisionDropEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
);

/*******************************************************************************
* cpssDxChPortExcessiveCollisionDropEnableGet
*
* DESCRIPTION:
*       Gets status of excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr - pointer to status of excessive collision packets drop.
*                   - GT_TRUE  - if the number of collisions on the same packet
*                               is 16 the packet is dropped.
*                   - GT_FALSE - A collided packet will be retransmitted by
*                                device until it is transmitted
*                                without collisions, regardless of the number
*                                of collisions on the packet.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Not relevant in full duplex mode
*
*******************************************************************************/
GT_STATUS cpssDxChPortExcessiveCollisionDropEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
);

/*******************************************************************************
* cpssDxChPortXgLanesSwapEnableSet
*
* DESCRIPTION:
*       Enable/Disable swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*
*                - GT_FALSE - Normal operation (no swapping)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgLanesSwapEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
);

/*******************************************************************************
* cpssDxChPortXgLanesSwapEnableGet
*
* DESCRIPTION:
*       Gets status of swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*
*
* OUTPUTS:
*       enablePtr - pointer to status of swapping XAUI PHY SERDES Lanes.
*                    - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*                    - GT_FALSE - Normal operation (no swapping)
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgLanesSwapEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
);

/*******************************************************************************
* cpssDxChPortXgPscLanesSwapSet
*
* DESCRIPTION:
*       Set swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES: DxCh3 and above
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*       rxSerdesLaneArr - array for binding port Rx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*       txSerdesLaneArr - array for binding port Tx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - wrong SERDES lane
*       GT_BAD_VALUE    - multiple connection detected
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgPscLanesSwapSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS],
    IN GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS]
);

/*******************************************************************************
* cpssDxChPortXgPscLanesSwapGet
*
* DESCRIPTION:
*       Get swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES: DxCh3 and above
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*
* OUTPUTS:
*       rxSerdesLaneArr - array for binding port Rx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*       txSerdesLaneArr - array for binding port Tx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgPscLanesSwapGet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    OUT GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS],
    OUT GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS]
);

/*******************************************************************************
* cpssDxChPortInBandAutoNegBypassEnableSet
*
* DESCRIPTION:
*       Enable/Disable Auto-Negotiation by pass.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Auto-Negotiation can't be bypassed.
*                - GT_FALSE - Auto-Negotiation is bypassed.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Supported for ports in 1000Base-X mode only.
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssDxChPortInbandAutoNegEnableSet.)
*
*******************************************************************************/
GT_STATUS cpssDxChPortInBandAutoNegBypassEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
);

/*******************************************************************************
* cpssDxChPortInBandAutoNegBypassEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation by pass status.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*
* OUTPUTS:
*       enablePtr - pointer to Auto-Negotiation by pass status.
*                   - GT_TRUE  - Auto-Negotiation can't be bypassed.
*                   - GT_FALSE - Auto-Negotiation is bypassed.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Supported for ports in 1000Base-X mode only.
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssDxChPortInbandAutoNegEnableSet.)
*
*******************************************************************************/
GT_STATUS cpssDxChPortInBandAutoNegBypassEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
);

/*******************************************************************************
* cpssDxChPortMacErrorIndicationPortSet
*
* DESCRIPTION:
*       Sets port monitored for MAC errors.
*
* APPLICABLE DEVICES:  DxCh1, DxCh2
*
* INPUTS:
*       devNum   - device number
*       portNum  - port monitored for MAC errors.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       MAC source addresses and the error type can be read
*       for the port. See cpssDxChPortMacErrorIndicationGet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacErrorIndicationPortSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum
);

/*******************************************************************************
* cpssDxChPortMacErrorIndicationPortGet
*
* DESCRIPTION:
*       Gets port monitored for MAC errors.
*
* APPLICABLE DEVICES:  DxCh1, DxCh2
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       portNumPtr  - pointer to port monitored for MAC errors.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       MAC source addresses and the error type can be read
*       for the port. See cpssDxChPortMacErrorIndicationGet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacErrorIndicationPortGet
(
    IN  GT_U8      devNum,
    OUT GT_U8      *portNumPtr
);

/*******************************************************************************
* cpssDxChPortMacErrorIndicationGet
*
* DESCRIPTION:
*       Gets MAC source addresses and the error type for the monitored port.
*
* APPLICABLE DEVICES:  DxCh1, DxCh2
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       typePtr     - pointer to MAC error type.
*       macPtr      - pointer to MAC SA of the last packet received
*                     with  a MAC error on MAC error indication port.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*      Port monitored for MAC errors should be set.
*      See cpssDxChPortMacErrorIndicationPortSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacErrorIndicationGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT   *typePtr,
    OUT GT_ETHERADDR                        *macPtr
);

/******************************************************************************
* cpssDxChPortSerdesPowerStatusSet
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes according to port capabilities.
*       XG / XAUI ports: All 4 (TX or RX) lanes are set simultanuously.
*       HX / QX ports: lanes 0,1 or TX and RX may be set separately.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       direction - may be either CPSS_PORT_DIRECTION_RX_E,
*                                 CPSS_PORT_DIRECTION_TX_E, or
*                                 CPSS_PORT_DIRECTION_BOTH_E.
*       lanesBmp  - bitmap of SERDES lanes (bit 0-> lane 0, etc.)
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_FAIL          - on error
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     DxCh3 and above devices supports only CPSS_PORT_DIRECTION_BOTH_E.
*
******************************************************************************/
GT_STATUS cpssDxChPortSerdesPowerStatusSet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  lanesBmp,
    IN  GT_BOOL                 powerUp
);

/******************************************************************************
* cpssDxChPortGroupSerdesPowerStatusSet
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes per group
*       according to port capabilities.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum     - physical device number
*       portSerdesGroup  - port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                                 0..3        |      0
*                                 4..7        |      1
*                                 8..11       |      2
*                                 12..15      |      3
*                                 16..19      |      4
*                                 20..23      |      5
*                                 24          |      6
*                                 25          |      7
*                                 26          |      8
*                                 27          |      9
*                DxCh3 XG devices:
*                                 0           |      0
*                                 4           |      1
*                                 10          |      2
*                                 12          |      3
*                                 16          |      4
*                                 22          |      5
*                                 24          |      6
*                                 25          |      7
*                                 26          |      8
*                                 27          |      9
*               xCat FE devices
*                                 24          |      6
*                                 25          |      7
*                                 26          |      8
*                                 27          |      9
*               Lion devices: Port == SERDES Group
*
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong devNum, portSerdesGroup
*     GT_FAIL          - on error
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Get SERDES port group according to port by calling to
*     cpssDxChPortSerdesGroupGet.
*
******************************************************************************/
GT_STATUS cpssDxChPortGroupSerdesPowerStatusSet
(
    IN  GT_U8      devNum,
    IN  GT_U32     portSerdesGroup,
    IN  GT_BOOL    powerUp
);

/******************************************************************************
* cpssDxChPortSerdesGroupGet
*
* DESCRIPTION:
*       Get SERDES port group that may be used in per SERDES group APIs.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portSerdesGroupPtr  - Pointer to port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               Lion devices: Port == SERDES Group
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong devNum, portNum
*     GT_FAIL          - on error
*     GT_BAD_PTR       - on NULL pointer
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
*
******************************************************************************/
GT_STATUS cpssDxChPortSerdesGroupGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U32   *portSerdesGroupPtr
);


/*******************************************************************************
* cpssDxChPortSerdesCalibrationStartSet
*
* DESCRIPTION:
*       Start calibration on all SERDES of given port.
*
* APPLICABLE DEVICES: DxChXcat and above
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesCalibrationStartSet
(
    IN GT_U8  devNum,
    IN GT_U8  portNum
);

/*******************************************************************************
* cpssDxChPortFlowControlModeSet
*
* DESCRIPTION:
*       Sets Flow Control mode on given port.
*
* APPLICABLE DEVICES: Lion and above
*
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       fcMode     - flow control mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlModeSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  CPSS_DXCH_PORT_FC_MODE_ENT fcMode
);

/*******************************************************************************
* cpssDxChPortFlowControlModeGet
*
* DESCRIPTION:
*       Gets Flow Control mode on given port.
*
* APPLICABLE DEVICES: Lion and above
*
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       fcModePtr  - flow control mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlModeGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT CPSS_DXCH_PORT_FC_MODE_ENT   *fcModePtr
);

/*******************************************************************************
* cpssDxChPortMacResetStateSet
*
* DESCRIPTION:
*       Set MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state    - Reset state
*                  GT_TRUE   - Port MAC is under Reset
*                  GT_FALSE - Port MAC is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
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
GT_STATUS cpssDxChPortMacResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
);

/*******************************************************************************
* cpssDxChPortSerdesResetStateSet
*
* DESCRIPTION:
*       Set SERDES Reset state on specified port on specified device.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state  - Reset state
*                GT_TRUE   - Port SERDES is under Reset
*                GT_FALSE - Port SERDES is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
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
GT_STATUS cpssDxChPortSerdesResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortCtrlh */

