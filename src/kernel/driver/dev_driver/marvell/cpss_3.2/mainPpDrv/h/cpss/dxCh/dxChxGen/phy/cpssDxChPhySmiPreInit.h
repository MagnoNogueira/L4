/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPhySmiPreInit.h
*
* DESCRIPTION:
*        API implementation for PHY SMI pre init configuration.
*        All functions should be called after cpssDxChHwPpPhase1Init() and  
*        before cpssDxChPhyPortSmiInit().
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#ifndef __cpssDxChPhySmiPreInith
#define __cpssDxChPhySmiPreInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>

/*
 * typedef: enum CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT
 *
 * Description: Enumeration of Auto Poll numbers of port 
 *
 * Enumerations:
 *      CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E  - 8 ports polling
 *      CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E - 12 ports polling
 *      CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E - 16 ports polling
 */
typedef enum
{
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E,
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E,
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E
} CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT;


/*******************************************************************************
* cpssDxChPhyPortAddrSet
*
* DESCRIPTION:
*       Configure the port's default phy address, this function should be
*       used to change the default port's phy address.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number.
*       phyAddr     - The new phy address, (value 0...31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For flex ports this API and cpssDxChPhyPortSmiInterfaceSet() must be 
*       used before any access to flex port phy related registers.
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortAddrSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U8   phyAddr
);

/*******************************************************************************
* cpssDxChPhyPortAddrGet
*
* DESCRIPTION:
*       Gets port's phy address from hardware and from database.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number.
*
* OUTPUTS:
*       phyAddFromHwPtr - (pointer to) phy address in HW.
*       phyAddFromDbPtr - (pointer to) phy address in DB.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For flex ports no hardware value exists and this field should be 
*       ignored.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortAddrGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U8   *phyAddFromHwPtr,
    OUT GT_U8   *phyAddFromDbPtr
);

/*******************************************************************************
* cpssDxChPhyAutoPollNumOfPortsSet
*
* DESCRIPTION:
*       Configure number of auto poll ports for SMI0 and SMI1.
*
* APPLICABLE DEVICES:  DxChXCat only
*
* INPUTS:
*       devNum                  - physical device number.
*       autoPollNumOfPortsSmi0  - number of ports for SMI0.
*       autoPollNumOfPortsSmi1  - number of ports for SMI1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, autoPollNumOfPortsSmi0 or 
*                                                   autoPollNumOfPortsSmi1.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable combinations for Auto Poll number of ports are:
*       |-----------------------|
*       |   SMI 0   |   SMI 1   |
*       |-----------|-----------|
*       |     8     |    16     |
*       |     8     |    12     |
*       |    12     |    12     |
*       |    16     |    8      |
*       |-----------------------|
*
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
*******************************************************************************/
GT_STATUS cpssDxChPhyAutoPollNumOfPortsSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi0,
    IN  CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi1
);


/*******************************************************************************
* cpssDxChPhyAutoPollNumOfPortsGet
*
* DESCRIPTION:
*       Get number of auto poll ports for SMI0 and SMI1.
*
* APPLICABLE DEVICES:  DxChXCat only
*
* INPUTS:
*       devNum                  - physical device number.
*
* OUTPUTS:
*       autoPollNumOfPortsSmi0Ptr  - number of ports for SMI0.
*       autoPollNumOfPortsSmi1Ptr  - number of ports for SMI1.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, autoPollNumOfPortsSmi0 or 
*                                                   autoPollNumOfPortsSmi1.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPhyAutoPollNumOfPortsGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi0Ptr,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi1Ptr
);
 
/*******************************************************************************
* cpssDxChPhyPortSmiInterfaceSet
*
* DESCRIPTION:
*       Configure port SMI interface.
*
* APPLICABLE DEVICES:  DxChXCat and above
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*       smiInterface    - port SMI interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum or SMI interface.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For flex ports this API and cpssDxChPhyPortAddrSet() must be
*       used before any access to flex port PHY related registers.
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiInterfaceSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface
);

/*******************************************************************************
* cpssDxChPhyPortSmiInterfaceGet
*
* DESCRIPTION:
*       Get port SMI interface.
*
* APPLICABLE DEVICES:  DxChXCat and above
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*
* OUTPUTS:
*       smiInterfacePtr - (pointer to) port SMI interface.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - port SMI interface was not initialized
*                                  correctly.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiInterfaceGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT CPSS_PHY_SMI_INTERFACE_ENT  *smiInterfacePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPhySmiPreInith */

