/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwInit.h
*
* DESCRIPTION:
*       Includes Core level basic Hw initialization functions, and data
*       structures.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/
#ifndef __prvCpssDxChHwInith
#define __prvCpssDxChHwInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#define PRV_CPSS_DXCH_PROFILES_NUM_CNS       4
#define PRV_CPSS_DXCH2_PROFILES_NUM_CNS      8

#define PRV_CPSS_XCAT_SERDES_NUM_CNS        22
#define PRV_CPSS_LION_SERDES_NUM_CNS        24

/*******************************************************************************
* prvCpssDxChHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah devices.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrInit
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxCh2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah2 devices.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh2HwRegAddrInit
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxCh3HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah-3 devices.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwRegAddrInit
(
    IN GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChXcatHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh xCat devices.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatHwRegAddrInit
(
    IN GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChLionHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh Lion devices.
*
* APPLICABLE DEVICES:  DxChLion and above
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLionHwRegAddrInit
(
    IN GT_U32 devNum
);



/*******************************************************************************
* prvCpssDxChHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChHwRegAddrInit(...)
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxCh2HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxCh2HwRegAddrInit(...)
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh2HwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxCh3HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxCh3HwRegAddrInit(...)
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChXcatHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChXcatHwRegAddrInit(...)
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatHwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChLionHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChLionHwRegAddrInit(...)
*
* APPLICABLE DEVICES:  DxChLion and above
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLionHwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChXcatHwRegAddrPortMacUpdate
*
* DESCRIPTION:
*       This function updates mac registers for given port.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatHwRegAddrPortMacUpdate
(
    IN GT_U32 devNum,
    IN GT_U32 portNum
);

/*******************************************************************************
* prvCpssDxCh3HwPpPllUnlockWorkaround
*
* DESCRIPTION:
*       Workaround to eliminate the PLL unlocking issue.
*
* APPLICABLE DEVICES:  DxCh3
*
* INPUTS:
*       devNum                  - device number
*       networkSerdesPowerUpBmp - bitmap of network SERDES to be power UP
*       xgDevice                - GT_TRUE for XG device, GT_FALSE for GE device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*       PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC usage has actual influence
*       only for XG devices, where it is used to get the port number from
*       the power up SERDES bitmap (networkSerdesPowerUpBmp).
*       For GE devices the macro does nothing usefull since previouse one
*       (PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC) make all necessary checks.
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwPpPllUnlockWorkaround
(
    IN GT_U8    devNum,
    IN GT_U32   networkSerdesPowerUpBmp,
    IN GT_BOOL  xgDevice
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #define __prvCpssDxChHwInith
 */

