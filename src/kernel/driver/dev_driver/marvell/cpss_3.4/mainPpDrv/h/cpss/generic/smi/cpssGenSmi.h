/*******************************************************************************
*              Copyright 2006, MARVELL TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* cpssGenPpSmi.h
*
* DESCRIPTION:
*       API implementation for ASIC SMI controller
*
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#ifndef __cpssGenPpSmih
#define __cpssGenPpSmih

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>

/*******************************************************************************
* cpssSmiRegisterRead
*
* DESCRIPTION:
*      The function reads register of a Marvell device, which connected to 
*           SMI master controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       dataPtr - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       Function specific for Marvell devices with 32-bit registers
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterRead
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr, 
    IN  GT_U32  regAddr,
    OUT GT_U32  *dataPtr
);

/*******************************************************************************
* cpssSmiRegisterWrite
*
* DESCRIPTION:
*      The function writes register of a Marvell device, which connected to SMI master 
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - address of register of configurated device
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       Function specific for Marvell devices with 32-bit registers
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterWrite
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr, 
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);

/*******************************************************************************
* cpssSmiRegisterReadShort
*
* DESCRIPTION:
*      The function reads register of a device, which connected to SMI master 
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       dataPtr - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterReadShort
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr, 
    IN  GT_U32  regAddr,
    OUT GT_U16  *dataPtr
);

/*******************************************************************************
* cpssSmiRegisterWriteShort
*
* DESCRIPTION:
*      The function writes register of a device, which connected to SMI master 
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - address of register of configurated device
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterWriteShort
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr, 
    IN  GT_U32  regAddr,
    IN  GT_U16  data
);

/*******************************************************************************
* cpssXsmiPortGroupRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiAddr    - address of configurated device on XSMI (range 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to write to (value of 0..31).
*       data        - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiAddr, phyDev.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiPortGroupRegisterWrite
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  xsmiAddr, 
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    IN  GT_U16  data
);


/*******************************************************************************
* cpssXsmiPortGroupRegisterRead
*
* DESCRIPTION:
*       Read value of a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiAddr    - address of configurated device on XSMI (range 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to read from (value of 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiAddr, phyDev.
*       GT_BAD_PTR               - pointer to place data is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiPortGroupRegisterRead
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  xsmiAddr, 
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    OUT GT_U16  *dataPtr
);

/*******************************************************************************
* cpssXsmiRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       xsmiAddr    - address of configurated device on XSMI (range 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to write to (value of 0..31).
*       data        - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiAddr, phyDev.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiRegisterWrite
(
    IN  GT_U8   devNum,
    IN  GT_U32  xsmiAddr, 
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    IN  GT_U16  data
);


/*******************************************************************************
* cpssXsmiRegisterRead
*
* DESCRIPTION:
*       Read value of a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       xsmiAddr    - address of configurated device on XSMI (range 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to read from (value of 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiAddr, phyDev.
*       GT_BAD_PTR               - pointer to place data is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiRegisterRead
(
    IN  GT_U8   devNum,
    IN  GT_U32  xsmiAddr, 
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    OUT GT_U16  *dataPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenPpSmih */

