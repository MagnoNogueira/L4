/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssPortTypes.h
*
* DESCRIPTION:
*       CPSS port related definitions
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
******************************************************************************/

#ifndef __prvCpssPortTypesh
#define __prvCpssPortTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
/* get CPSS definitions for port configurations */
#include <cpss/generic/port/cpssPortCtrl.h>
/* include the phy types Info */
#include <cpss/generic/phy/private/prvCpssPhy.h>
#include <cpss/generic/extMac/cpssExtMacDrv.h> 



/* check if the erratum need to be WA by PSS
    devNum  - the device id of the cpss device
    portNum - port number
    FErNum  - erratum number
*/
#define PRV_CPSS_PHY_ERRATA_GET(devNum, portNum, FErNum) \
  ((PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].phyErrataWa >> FErNum) & 0x1)? \
      GT_TRUE : GT_FALSE)

/*  set that the erratum need to be WA by PSS
    devNum  - the device id of the cpss device
    portNum - port number
    FErNum  - erratum number
*/
#define PRV_CPSS_PHY_ERRATA_SET(devNum, portNum, FErNum) \
    (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].phyErrataWa |= (1 << FErNum))

/*
 * typedef: enum PRV_CPSS_PORT_TYPE_ENT
 *
 * Description: Port Type speed enumeration
 *
 * Enumerations:
 *      PRV_CPSS_PORT_NOT_EXISTS_E - port not exists in the device.
 *      PRV_CPSS_PORT_FE_E         - 10/100 Mbps
 *      PRV_CPSS_PORT_GE_E         - 10/100/1000 Mbps
 *      PRV_CPSS_PORT_XG_E         - 10/20 Gbps
 *      PRV_CPSS_PORT_XLG_E        - 40 Gbps
 *      PRV_CPSS_PORT_NOT_APPLICABLE_E - not applicable mode, for 
 *                                      validity checks and loops
 *
 */
typedef enum
{
    PRV_CPSS_PORT_NOT_EXISTS_E,
    PRV_CPSS_PORT_FE_E,
    PRV_CPSS_PORT_GE_E,
    PRV_CPSS_PORT_XG_E,
    PRV_CPSS_PORT_XLG_E,
    PRV_CPSS_PORT_NOT_APPLICABLE_E
} PRV_CPSS_PORT_TYPE_ENT;

/*
 * typedef: enum  PRV_CPSS_PORT_TYPE_OPTIONS_ENT
 *
 * Description: Port type options
 *
 * Enumerations:
 *      PRV_CPSS_XG_PORT_XG_ONLY_E    - The port works as XG port only
 *      PRV_CPSS_XG_PORT_QX_ONLY_E    - The port works as QX port only
 *      PRV_CPSS_XG_PORT_HX_QX_ONLY_E - The port works as HX/QX port only
 *      PRV_CPSS_XG_PORT_XG_HX_QX_E   - The port can work as either XG or HX/QX
 *      PRV_CPSS_GE_PORT_GE_ONLY_E    - The port works as GE port only
 *      PRV_CPSS_XG_PORT_XLG_SGMII_E  - The port supporting modes from SGMII up to XLG
 *
 *      PRV_CPSS_XG_PORT_OPTIONS_MAX_E - for validity checks etc.
 *
 */
typedef enum
{
    PRV_CPSS_XG_PORT_XG_ONLY_E,
    PRV_CPSS_XG_PORT_QX_ONLY_E,
    PRV_CPSS_XG_PORT_HX_QX_ONLY_E,
    PRV_CPSS_XG_PORT_XG_HX_QX_E,
    PRV_CPSS_GE_PORT_GE_ONLY_E,
    PRV_CPSS_XG_PORT_XLG_SGMII_E,

    PRV_CPSS_XG_PORT_OPTIONS_MAX_E

} PRV_CPSS_PORT_TYPE_OPTIONS_ENT;

/*
 * Typedef: struct PRV_CPSS_PORT_FC_THRESHOLD_STC
 *
 * Description:
 *      Port Flow Control Threshold parameters
 *
 * Fields:
 *      xonLimit  - current port X-On limit value
 *      xoffLimit - current port X-Off limit value
*/
typedef struct
{
    GT_U16 xonLimit;
    GT_U16 xoffLimit;
} PRV_CPSS_PORT_FC_THRESHOLD_STC;
/*
 * Typedef: enum PRV_CPSS_PHY_ERRATA_WA_ENT
 *
 * Description: enum for Work Arounds of PHY erratum regarding IEEE test modes
 *
 * Fields:
 *      PRV_CPSS_PHY_ERRATA_1149_REG_9_WRITE_WA_E - WA of PHY 88E1149 C1 in the
 *                                            Giga mode. Allows hiddden
 *                                            register's update
 *
 *      PRV_CPSS_PHY_ERRATA_MAX_NUM_E - last errata
 */
typedef enum{
    PRV_CPSS_PHY_ERRATA_1149_REG_9_WRITE_WA_E ,

    /* the last one */
    PRV_CPSS_PHY_ERRATA_MAX_NUM_E
}PRV_CPSS_PHY_ERRATA_WA_ENT;

/*
 * Typedef: struct PRV_CPSS_PORT_INFO_ARRAY_STC
 *
 * Description:
 *      Port information structure
 *
 * Fields:
 *      portFcParams   - port flow control threshold parameters
 *      portType       - port type speed
 *      smiIfInfo      - port PHY SMI access related info
 *      portTypeOptions - possible types of GE/XG port.
 *                       XG port can be XG,XG/HX/QX or HX/QX.
 *                       GE port can be GE.
 *      phyErrataWa - erratum bitmap: holds an information of relevant erratums
 *                    per specific port
 *      isFlexLink    - GT_TRUE - port is Flex-Link Port; GT_FALSE - port isn't
 *                      Flex-Link Port
 *      vctLengthOffset - offset value used for fine tuning of VCT length 
 *                        calculations. 
 *      portIfMode - current port interface mode.
 *      portMacObjPtr - pointer that can be bind and reference 
 *                      to PHYMAC MACSEC object
 *                      
 */
typedef struct
{
    PRV_CPSS_PORT_FC_THRESHOLD_STC      portFcParams;
    PRV_CPSS_PORT_TYPE_ENT              portType;
    PRV_CPSS_PHY_PORT_SMI_IF_INFO_STC   smiIfInfo;
    PRV_CPSS_PORT_TYPE_OPTIONS_ENT      portTypeOptions;
    GT_U32                              phyErrataWa;
    GT_BOOL                             isFlexLink;
    GT_32                               vctLengthOffset;
    CPSS_PORT_INTERFACE_MODE_ENT        portIfMode;
    CPSS_MACDRV_OBJ_STC  *              portMacObjPtr;

} PRV_CPSS_PORT_INFO_ARRAY_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssPortTypesh */

