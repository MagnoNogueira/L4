/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssExMxPhy.h
*
* DESCRIPTION:
*        Definitions and enumerators for port Core Serial Management
*        Interface facility.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#ifndef __prvCpssPhyh
#define __prvCpssPhyh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


/* use in accessing Twist-D-XG's SMI control Reg */
#define PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS   0x05 /* DTE device address */
#define PRV_CPSS_PHY_XENPAK_PORT_ADDRESS_CNS  0x01 /* DTE port address   */

/* default values to indicate no SMI configuration */
#define PRV_CPSS_PHY_SMI_NO_PORT_ADDR_CNS   0xFFFF
#define PRV_CPSS_PHY_SMI_NO_CTRL_ADDR_CNS   0xFFFFFFFF

/* macro to get the port's PHY SMI address -- not relevant to XG ports */
#define PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portNum)  \
    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].smiIfInfo.smiPortAddr

/* macro to get the port's SMI control register address --
   not relevant to XG ports */
#define PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum)  \
    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].smiIfInfo.smiCtrlRegAddr

/*
 * Typedef: GT_PORT_SMI_IF_INFO
 *
 * Description: Holds information about the Smi interface to be used for a given
 *              port.
 *
 * Fields:
 *      smiPortAddr     - SMI port address.
 *      smiCtrlRegAddr  - Address of the Smi control register to be used for the
 *                        Port
 *
 */
typedef struct
{
    GT_U16  smiPortAddr;
    GT_U32  smiCtrlRegAddr;
}PRV_CPSS_PHY_PORT_SMI_IF_INFO_STC;

/*******************************************************************************
* PRV_CPSS_VCT_PHY_READ_PHY_REGISTER_FUN
*
* DESCRIPTION:
*       Read specified SMI Register on a specified port on specified device.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       phyReg - SMI register
*
* OUTPUTS:
*       data  - data read.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_VCT_PHY_READ_PHY_REGISTER_FUN)
(
     IN  GT_U8     dev,
     IN  GT_U8     port,
     IN  GT_U8     phyReg,
     OUT GT_U16    *data
);
/*******************************************************************************
* PRV_CPSS_VCT_PHY_WRITE_PHY_REGISTER_FUN
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       phyReg - SMI register
*       data   - value to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_VCT_PHY_WRITE_PHY_REGISTER_FUN)
(
     IN  GT_U8     dev,
     IN  GT_U8     port,
     IN  GT_U8     phyReg,
     IN  GT_U16    data
);

/*******************************************************************************
* PRV_CPSS_VCT_PHY_AUTONEG_SET_FUN
*
* DESCRIPTION:
*       This function sets the auto negotiation process between the PP and
*       Phy status.
* APPLICABLE DEVICES:   All devices
*
* INPUTS:
*       devNum          - the device number
*       port            - port number
*       enable          - enable/disable Auto Negotiation status
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*

*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_VCT_PHY_AUTONEG_SET_FUN)
(
     IN  GT_U8     devNum,
     IN  GT_U8     port,
     IN  GT_BOOL   enable
);

/*******************************************************************************
* PRV_CPSS_VCT_PHY_AUTONEG_GET_FUN
*
* DESCRIPTION:
*       This function sets the auto negotiation process between the PP and
*       Phy status.
* APPLICABLE DEVICES:   All devices
*
* INPUTS:
*       devNum          - the device number
*       port            - port number
*       enablePtr       - read Auto Negotiation status
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*

*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_VCT_PHY_AUTONEG_GET_FUN)
(
     IN  GT_U8     devNum,
     IN  GT_U8     port,
     OUT GT_BOOL   *enablePtr
);

/*
 * Typedef: struct PRV_CPSS_VCT_GEN_BIND_FUNC_STC
 *
 * Description: A structure to hold common VCT functions for PP Family needed
 *              in CPSS
 *      INFO "PER DEVICE FAMILY"
 *
 * Fields:
 *

 *
*/
typedef struct{
    PRV_CPSS_VCT_PHY_READ_PHY_REGISTER_FUN     cpssPhyRegisterRead;
    PRV_CPSS_VCT_PHY_WRITE_PHY_REGISTER_FUN    cpssPhyRegisterWrite;
    PRV_CPSS_VCT_PHY_AUTONEG_SET_FUN           cpssPhyAutoNegotiationSet;
    PRV_CPSS_VCT_PHY_AUTONEG_GET_FUN           cpssPhyAutoNegotiationGet;

}PRV_CPSS_VCT_GEN_BIND_FUNC_STC;




/*
 * Typedef: PRV_CPSS_PHY_INFO_STC
 *
 * Description: Holds info regarding virtual functions
 *              port.
 *
 * Fields:
 *      genVctBindFunc  -   holds common VCT functions PP Family
 *
 */
typedef struct
{
   PRV_CPSS_VCT_GEN_BIND_FUNC_STC genVctBindFunc;
}PRV_CPSS_PHY_INFO_STC;

/*******************************************************************************
* prvCpssVctDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of the VCT.
*       NOTE: function not 'free' the allocated memory. only detach from it ,
*             and restore DB to 'pre-init' state
*
*             The assumption is that the 'cpssOsMalloc' allocations will be not
*             valid any more by the application , so no need to 'free' each and
*             every allocation !
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       None
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssVctDbRelease
(
    void
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssPhyh */

