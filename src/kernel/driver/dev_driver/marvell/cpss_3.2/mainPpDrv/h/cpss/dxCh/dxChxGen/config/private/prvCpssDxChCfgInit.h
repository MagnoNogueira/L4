/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCfgInit.h
*
* DESCRIPTION:
*       Includes functions for internal CPSS DxCh table sizes calculation
*       functions.
*
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __prvCpssDxChCfgInith
#define __prvCpssDxChCfgInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>

/*
 * typedef: struct PRV_CPSS_DXCH_PP_CONFIG_INIT_STC
 *
 * Description: DxCh Packet Processor (PP) init configuration Parameters
 *              structure
 *
 * Fields:  
 *    maxNumOfVirtualRouters - max numbers of IPv4 virtual routers.
 *    maxNumOfIpNextHop      - max number of IPv4 and Ipv6 next hop entries in the
 *                             PP.
 *    maxNumOfIpv4Prefixes   - max number of IPv4 prefixes in the PP.
 *    maxNumOfIpv4McEntries  - max number of IPv4 and IPv6 Multicast Entries in the PP.
 *    maxNumOfMll            - max number of Downstream IPv4 and Ipv6 Multicast
 *                             interface entries (MLLs) in the PP.
 *    maxNumOfIpv6Prefixes   - max number of IPv6 prefixes in the PP.
 *    maxNumOfTunnelEntries  - max number of tunnel (of all types) entries
 *    maxNumOfIpv4TunnelTerms - the maximum number of ipv4 tunnel termination
 *                                  entries
 *    ipMemDivisionOn         - if GT_FALSE, no mem devision between IPv4 and IPv6 takes place
 *    ipv6MemShare            - relevant only if ipMemDivisionOn is true, percentage
 *                             of ip memory reserved for IPv6 use.
 *
 *
 *    COMMENTS:
 */
typedef struct
{           
    /*Lib init params. */
     /* IP Unicast     */
    GT_U32          maxNumOfVirtualRouters;
    GT_U32          maxNumOfIpNextHop; 
    /* IPv4 Unicast     */
    GT_U32          maxNumOfIpv4Prefixes;
    /* IP Multicast   */
    GT_U32          maxNumOfMll;
    /* IPv4 Multicast   */
    GT_U32          maxNumOfIpv4McEntries;
    /* IPv6 Unicast     */
    GT_U32          maxNumOfIpv6Prefixes;
    /* Tunnels */
    GT_U32          maxNumOfTunnelEntries;

    /* ipv4 tunnel termination */
    GT_U32          maxNumOfIpv4TunnelTerms;

    /* if GT_FALSE - no division of memory pools for the use of different IP versions */
    GT_BOOL         ipMemDivisionOn;
    /* division of memory between IPv4 and IPv6 prefixes */
    GT_U32          ipv6MemShare;
} PRV_CPSS_DXCH_PP_CONFIG_INIT_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChCfgInith */

