/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortSyncEther.h
*
* DESCRIPTION:
*       CPSS definition for Sync-E (Synchronious Ethernet)
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#ifndef __cpssDxChPortSyncEther
#define __cpssDxChPortSyncEther

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
* typedef: enum CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT
*
* Description: Enumeration of PLL reference clock.
*
* Enumerations:
*  CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_OSCILLATOR_E - Oscillator clock
*  CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_RECOVERED_E  - Recovered clock
*
* Comments:
*         None.
*/ 
typedef enum
{
    CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_OSCILLATOR_E,
    CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_RECOVERED_E
}CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT;

/*
* typedef: enum CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT
*
* Description: Enumeration of physical recovery clock output pins.
*
* Enumerations:
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E - clock output pin0 or
*                                              clock output for port group 0. 
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E - clock output pin1 or 
*                                              clock output for port group 1.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E - clock output for port group 2.
*                                              relevant only for Lion.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E - clock output for port group 3.
*                                              relevant only for Lion.
*
* Comments:
*         None.
*/ 
typedef enum
{
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E
}CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT;

/*
* typedef: enum CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT
*
* Description: Enumeration of recovery clock divider values.
*
* Enumerations:
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_1_E - clock divided by 1.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_E - clock divided by 2.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_3_E - clock divided by 3.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_4_E - clock divided by 4.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_5_E - clock divided by 5.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_8_E - clock divided by 8.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_E - clock divided by 16.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_5_E - clock divided by 2.5.
*
* Comments:
*         None.
*/ 
typedef enum
{
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_1_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_3_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_4_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_5_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_8_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_5_E
}CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT;

/*******************************************************************************
* cpssDxChPortSyncEtherSecondRecoveryClkEnableSet
*
* DESCRIPTION:
*       Function enables/disables second recovery clock output pin.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum  - device number.
*       enable  - enable/disable second clock output pin
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Function performs all SERDES power down and restore original SERDES 
*       power state.
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherSecondRecoveryClkEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortSyncEtherSecondRecoveryClkEnableGet
*
* DESCRIPTION:
*       Function gets status (enabled/disabled) of 
*       second recovery clock output pin.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       enablePtr - (pointer to) second clock output pin state
*                    GT_TRUE  - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - on bad state
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherSecondRecoveryClkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortSyncEtherPllRefClkSelectSet
*
* DESCRIPTION:
*       Function sets PLL reference clock select:
*       oscillator clock or recovered clock.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*                      For xCat & xCat2 valid ports for configuration:
*                      Giga Ports: 0,4,8,12,16,20
*                      Stacking Ports: 24,25,26,27
*       pllClkSelect - clock source: recovered or oscillator
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or pllClkSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Function performs SERDES power down and restore
*       original SERDES power state.
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherPllRefClkSelectSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT pllClkSelect
);

/*******************************************************************************
* cpssDxChPortSyncEtherPllRefClkSelectGet
*
* DESCRIPTION:
*       Function gets PLL reference clock select: 
*       oscillator clock or recovered clock.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number.
*       portNum   - port number. 
*                   For xCat & xCat2 valid ports for configuration:
*                   Giga Ports: 0,4,8,12,16,20
*                   Stacking Ports: 24,25,26,27
*
* OUTPUTS:
*       pllClkSelectPtr - (pointer to) clock source: recovered or oscillator
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherPllRefClkSelectGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT *pllClkSelectPtr
);

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkConfigSet
*
* DESCRIPTION:
*       Function configures the recovery clock enable/disable state and sets 
*       its source portNum.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type.
*       enable          - enable/disable recovered clock1/clock2
*                         GT_TRUE - enable
*                         GT_FALSE - disable
*       portNum         - port number.
*                         For xCat & xCat2 valid ports for configuration:
*                         Giga Ports: 0,4,8,12,16,20
*                         Stacking Ports: 24,25,26,27
*       laneNum         - selected SERDES lane number within port.
*                         relevant only for Lion.
*                         Ranges according to port interface:
*                         SGMII/1000BaseX - 0
*                         RXAUI/HX        - 0..1
*                         XAUII - 0..3
*                         XLG   - 0..7 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType, portNum
*                                  or laneNum.
*       GT_BAD_STATE             - another SERDES already enabled
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkConfigSet
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    IN  GT_BOOL enable,
    IN  GT_U8   portNum,
    IN  GT_U32  laneNum
);

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkConfigGet
*
* DESCRIPTION:
*       Function gets the recovery clock enable/disable state and its source 
*       portNum.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                         GT_TRUE -  enabled
*                         GT_FALSE - disbled
*       portNumPtr      - (pointer to) port number.
*       laneNumPtr      - (pointer to) selected SERDES lane number within port.
*                         relevant only for Lion.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on bad state of register
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkConfigGet
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U8    *portNumPtr,
    OUT GT_U32   *laneNumPtr
);

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
*
* DESCRIPTION:
*       Function sets recovery clock divider bypass enable.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number.
*       enable      - enable/disable recovery clock divider bypass enable
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, enable
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
(
    IN  GT_U8  devNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet
*
* DESCRIPTION:
*       Function gets recovery clock divider bypass status.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   - (pointer to) recovery clock divider bypass state
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet
(
    IN  GT_U8  devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerValueSet
*
* DESCRIPTION:
*       Function sets recovery clock divider value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     relevant only for Lion.
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*       value       - recovery clock divider value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerValueSet
(
    IN  GT_U8                                               devNum,
    IN  GT_U8                                               portNum,
    IN  GT_U32                                              laneNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT  value
);

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerValueGet
*
* DESCRIPTION:
*       Function gets recovery clock divider value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     relevant only for Lion.
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*
* OUTPUTS:
*       valuePtr   - (pointer to) recovery clock divider value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerValueGet
(
    IN  GT_U8                                               devNum,
    IN  GT_U8                                               portNum,
    IN  GT_U32                                              laneNum,
    OUT CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT  *valuePtr
);

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet
*
* DESCRIPTION:
*       Function sets Recovered Clock Automatic Masking enabling.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     relevant only for Lion.
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*       enable      - enable/disable Recovered Clock Automatic Masking
*                     GT_TRUE - enable
*                     GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  laneNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet
*
* DESCRIPTION:
*       Function gets Recovered Clock Automatic Masking status.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     relevant only for Lion.
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*
* OUTPUTS:
*       enablePtr   - (pointer to) Recovered Clock Automatic Masking state
*                     GT_TRUE - enable
*                     GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  laneNum,
    OUT GT_BOOL *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortSyncEther */

