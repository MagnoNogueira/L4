/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTypes.h
*
* DESCRIPTION:
*       CPSS DXCH Generic types.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __cpssDxChTypesh
#define __cpssDxChTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnelTypes.h>

/*
 * typedef: struct CPSS_DXCH_OUTPUT_INTERFACE_STC
 *
 * Description: Defines the interface info
 *
 * Fields:
 *   isTunnelStart      - if set to GT_TRUE, packet is to be tunneled.
 *   tunnelStartInfo    - Tunnel Start information (Relevant if isTunnelStart== GT_TRUE)
 *              passengerPacketType - Type of passenger packet going to be
 *                                    encapsulated.
 *              ptr                 - A pointer to a tunnel entry
 *   physicalInterface   - defines the phisical interface info. Reffer to
 *                         CPSS_INTERFACE_INFO_STC. The vidx and vlanId fields
 *                         are not in use in this case.
 *
 * Notes:
 *      1. The structure is used in following cases:
 *          a. Logical Target Mapping
 *
 */

typedef struct
{
    GT_BOOL                 isTunnelStart;

    struct{
        CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT passengerPacketType;
        GT_U32                              ptr;
    }tunnelStartInfo;

    CPSS_INTERFACE_INFO_STC physicalInterface;
} CPSS_DXCH_OUTPUT_INTERFACE_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTypesh */
