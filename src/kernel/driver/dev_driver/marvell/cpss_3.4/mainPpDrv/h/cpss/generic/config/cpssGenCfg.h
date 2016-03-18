/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenCfg.h
*
* DESCRIPTION:
*       CPSS generic configuration types.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __cpssGenCfgh
#define __cpssGenCfgh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
 * typedef: struct CPSS_GEN_CFG_DEV_INFO_STC
 *
 * Description: Generic device info structure
 *
 * Fields:
 *      devType        - device type of the PP.
 *      revision       - the device's revision number.
 *      devFamily      - CPSS's device family that device belongs to.
 *      maxPortNum     - maximal port's number not including CPU one.
 *      numOfVirtPorts - number of virtual ports.
 *                       Relevant only for devices with virtual ports support.
 *      existingPorts  - bitmap of actual exiting ports not including CPU one.
 *
 */
typedef struct
{
    CPSS_PP_DEVICE_TYPE             devType;
    GT_U8                           revision;
    CPSS_PP_FAMILY_TYPE_ENT         devFamily;
    GT_U8                           maxPortNum;
    GT_U8                           numOfVirtPorts;
    CPSS_PORTS_BMP_STC              existingPorts;
}CPSS_GEN_CFG_DEV_INFO_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenCfgh */

