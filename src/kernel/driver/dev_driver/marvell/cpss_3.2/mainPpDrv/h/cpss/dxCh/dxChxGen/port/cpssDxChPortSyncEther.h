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
*       $Revision: 2 $
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
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E - clock output pin0.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E - clock output pin1.
*
* Comments:
*         None.
*/ 
typedef enum
{
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E
}CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT;

/*******************************************************************************
* cpssDxChPortSyncEtherSecondRecoveryClkEnableSet
*
* DESCRIPTION:
*       Function enables/disables second recovery clock output pin.
*
* APPLICABLE DEVICES:  DxChXcat and above
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
*       1. Function performs all SERDES power down and restore 
*          original SERDES power state.
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
* APPLICABLE DEVICES:  DxChXcat and above
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
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number. Valid ports for configuration:
*                       Giga Ports: 0,4,8,12,16,20
*                       XG  Ports: 24,25,26,27
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
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number.
*       portNum   - port number. Valid ports for configuration:
*                   Giga Ports: 0,4,8,12,16,20
*                   XG Ports: 24,25,26,27
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
*       Function configures the recovery clock (0 or 1) enable/disable 
*       state and sets its source portNum.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*       enable          - enable/disable recovered clock1/clock2
*                          GT_TRUE - enable
*                          GT_FALSE - disable
*       portNum         - port number. Valid ports for configuration:
*                          Giga Ports: 0,4,8,12,16,20
*                          XG Ports: 24,25,26,27
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType or portNum
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
    IN  GT_U8   portNum
);

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkConfigGet
*
* DESCRIPTION:
*       Function gets the recovery clock (0 or 1) enable/disable
*       state and sets its source portNum.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                           GT_TRUE - reference port
*                           GT_FALSE - regular port
*       portNumPtr      - (pointer to) port number. Valid ports for configuration:
*                           Giga Ports: 0,4,8,12,16,20
*                           XG Ports: 24,25,26,27
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
    OUT GT_U8    *portNumPtr
);

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
*
* DESCRIPTION:
*       Function sets recovery clock divider bypass enable.
*
* APPLICABLE DEVICES:  DxChXcat and above
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
* APPLICABLE DEVICES:  DxChXcat and above
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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortSyncEther */

