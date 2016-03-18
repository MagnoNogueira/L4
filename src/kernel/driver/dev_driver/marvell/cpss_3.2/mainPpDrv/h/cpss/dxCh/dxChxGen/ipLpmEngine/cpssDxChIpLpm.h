/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpLpm.h
*
* DESCRIPTION:
*       the CPSS DXCH LPM Hierarchy manager .
*
* FILE REVISION NUMBER:
*       $Revision: 18 $
*******************************************************************************/

#ifndef __cpssDxChIpLpmh
#define __cpssDxChIpLpmh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>


/*******************************************************************************
* cpssDxChIpLpmDBCreate
*
* DESCRIPTION:
*   This function creates an LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId               - the LPM DB id.
*       shadowType            - the type of shadow to maintain (cheetah/cheetha2)
*       protocolStack         - the type of protocol stack this LPM DB support.
*       indexesRangePtr       - the range of TCAM indexes availble for this
*                               LPM DB (see explanation in
*                               CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC)
*       partitionEnable       - GT_TRUE:  partition the TCAM range according to the
*                                         capacityCfgPtr, any unused TCAM entries will
*                                         be allocated to IPv4 UC entries
*                               GT_FALSE: allocate TCAM entries on demand while
*                                         guarantee entries as specified in capacityCfgPtr
*       tcamLpmManagerCapcityCfgPtr - holds the capacity configuration required
*                               from this TCAM LPM manager
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_SUPPORTED         - request is not supported if partitioning
*                                  is disabled.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       if partitionEnable = GT_FALSE then the TCAM is one big shared resource
*       for all kinds of prefixes. In a shared resource situation a best effort
*       is applied to put the inserted prefixes in a way they wouldn't interfere
*       with other prefixes. But there could be a situation where due to
*       fragmentation a prefix couldn't be inserted, in this situation all
*       prefix insert function has a defragmationEnable to enable a performance
*       costing de-fragmentation process in order to fit the inserted prefix.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCreate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT            shadowType,
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN GT_BOOL                                      partitionEnable,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
);

/*******************************************************************************
* cpssDxChIpLpmDBCheetah2VrSupportCreate
*
* DESCRIPTION:
*   This function creates an LPM DB for a shared LPM managment in Cheetah2
*   devices with support for virtual router.
*
* APPLICABLE DEVICES: All DxCh2 devices
*
* INPUTS:
*       lpmDBId          - the LPM DB id.
*       protocolStack    - the type of protocol stack this LPM DB support.
*       pclTcamCfgPtr    - the pcl tcam configuration
*       routerTcamCfgPtr - the router tcam configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - NULL pointer.
*
* COMMENTS:
*       Since LTT rules are shared between PCL and Router Tcam, several checks
*       are made to supplied parameters to make sure that all prefixes could be
*       inserted.
*
*       Note that LTT entries corresponding to routerTcamCfgPtr->tcamRange
*       are allocated to Lpm Manager.
*
*       Furthermore:
*
*       routerTcamCfgPtr->tcamRange.lastIndex -
*       routerTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Router Tcam lines).
*
*       pclTcamCfgPtr->tcamRange.lastIndex -
*       pclTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Pcl Tcam lines).
*
*       (routerTcamCfgPtr->tcamRange.lastIndex -
*        routerTcamCfgPtr->tcamRange.firstIndex + 1) * 5
*           must be larger or equal to
*       pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes
*
*       Multicast is not supported.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCheetah2VrSupportCreate
(
    IN GT_U32                                                   lpmDBId,
    IN CPSS_IP_PROTOCOL_STACK_ENT                               protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr
);

/*******************************************************************************
* cpssDxChIpLpmDBDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB . this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devListArr     - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDevListAdd
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
);


/*******************************************************************************
* cpssDxChIpLpmDBDevsListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing LPM DB . this remove will
*   invoke a hot sync removal of the devices.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devListArr     - the array of device ids to remove from the
*                        LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDevsListRemove
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
);

/*******************************************************************************
* cpssDxChIpLpmTcamLinesReserve
*
* DESCRIPTION:
*   This function reserves lines in the router TCAM. The LPM DB will not use
*   those lines for prefixes. Those lines will be considered as allocated
*   entries.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       linesToReserveArray       - lines to be reserved
*       numberOfLinesToReserve    - number of lines in linesToReserveArray
*       allocateReserveLinesAs    - relevant only when LPM DB is configured to
*                                   use dynamic TCAM partitioning.
*                                   Indicates as what entry type to treat the
*                                   reserved lines.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - all lines to reserve must reside in the TCAM section allocated
*           to the LPM DB
*         - all lines to reserve must be unique
*       In case the LPM DB is configured to use dynamic partitioning those
*       additional conditions must be matched:
*         - the sum of the lines in allocateReserveLinesAs must be equal to
*           the number of lines to reserve
*         - the lines in allocateReserveLinesAs must comply with the protocol
*           stacks supported by the LPM DB
*         - the LPM DB must have enough space to hold the allocateReserveLinesAs
*           as allocated entires.
*       Note that the parameters in allocateReserveLinesAs are in lines. Meaning
*       that if (allocateReserveLinesAs.numOfIpv4Prefixes == 1) it means that
*       one whole TCAM line will be considered allocated as IPv4 prefixes. In
*       4 columns TCAM it means that 4 IPv4 prefixes will be "allocated" in
*       this line to reserve.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmTcamLinesReserve
(
    IN  GT_U32                                          lpmDBId,
    IN  GT_U32                                          linesToReserveArray[],
    IN  GT_U32                                          numberOfLinesToReserve,
    IN  CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    allocateReserveLinesAs
);

/*******************************************************************************
* cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
*
* DESCRIPTION:
*   The function uses the first available TCAM rule for IPv4/IPv6 prefixes and
*   sets there "do nothing" rule that will make sure that Multicast traffic is
*   matched by this rule and not by the Unicast default rule.
*   When policy based routing Unicast traffic coexists with IP based Multicast
*   (S,G,V) bridging (used in IGMP and MLD protocols), there is a need to add
*   default Multicast rules in order to make sure that the Multicast traffic
*   will not be matched by the Unicast default rule.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_NOT_SUPPORTED          - if the LPM DB doesn't configured to
*                                   operate in policy based routing mode
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - the LPM DB must be configured to use policy based routing
*       One default Multicast rule will be set to each protocol stack supported
*       by the LPM DB.
*       For each protocol stack, the Multicast default rule will use one of the
*       prefixes allocated to the Unicast prefixes for this protocol. For example
*       if the application allocates 10 IPv4 Unicast prefixes, then after setting
*       this default Multicast prefix, only 9 IPv4 Unicast prefixes will be
*       available.
*       The default Multicast rules will capture all IPv4/IPv6 Multicast
*       traffic. As a result all rules configured to match IP Multicast traffic
*       that reside after the Multicast defaults will not be matched.
*       The default Multicast rules can not be deleted after set.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
(
    IN  GT_U32  lpmDBId
);

/*******************************************************************************
* cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
*
* DESCRIPTION:
*   The function gets whether the LPM DB is configured to support default MC
*   rules in policy based routing mode and the rule indexes of those default
*   rules.
*   Refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet for more details.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*
* OUTPUTS:
*       defaultMcUsedPtr          - whether default MC is used for this LPM DB
*       protocolStackPtr          - protocol stack supported by this LPM DB
*                                   relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexPtr   - rule index of the default IPv4 MC (in case
*                                   the LPM DB support IPv4)
*                                   relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv6RuleIndexPtr   - rule index of the default IPv6 MC (in case
*                                   the LPM DB support IPv6)
*                                   relevant only if defaultMcUsedPtr == GT_TRUE
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The rule indexes are according to explanations in cpssDxChPclRuleSet.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
(
    IN  GT_U32                              lpmDBId,
    OUT GT_BOOL                             *defaultMcUsedPtr,
    OUT CPSS_IP_PROTOCOL_STACK_ENT          *protocolStackPtr,
    OUT GT_U32                              *defaultIpv4RuleIndexPtr,
    OUT GT_U32                              *defaultIpv6RuleIndexPtr
);

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*       vrConfigPtr          - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PARAM             - if wrong value in any of the parameters
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*        Refer to cpssDxChIpLpmVirtualRouterSharedAdd for limitation when shared
*        virtual router is used.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC        *vrConfigPtr
);

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterSharedAdd
*
* DESCRIPTION:
*   This function adds a shared virtual router in system for specific LPM DB.
*   Prefixes that reside within shared virtual router will participate in the
*   lookups of all virtual routers.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The  virtual's router ID.
*       vrConfigPtr         - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       1.  Only one shared virtual router can exists at any time within a
*           given LPM DB.
*       2.  Virtual router ID that is used for non-shared virtual router can't
*           be used for the shared virtual router and via versa.
*       3.  Shared prefixes can't overlap non-shared prefixes. If the shared
*           virtual router supports shared prefixes type then adding non-shared
*           virtual router that supports the same prefixes type will fail.
*           Also, if a non-shared virtual router that supports prefixes type
*           exists, then adding a shared virtual router that supports the same
*           prefixes type will fail.
*       4.  When the shared virtual router supports IPv4 UC prefixes, then the
*           the following will apply:
*             - The <match all> default TCAM entry will not be written to TCAM
*               (however TCAM entry will still be allocated to this entry)
*             - The following prefixes will be added to match all non-MC traffic:
*               0x00/1, 0x80/2, 0xC0/3, 0xF0/4
*           Same applies when the shared virtual router supports IPv6 UC
*           prefixes. The prefixes added to match all non-MC traffic are:
*           0x00/1, 0x80/2, 0xC0/3, 0xE0/4, 0xF0/5, 0xF8/6, 0xFC/7, 0xFE/8
*       5.  The application should not delete the non-MC prefixes (when added).
*       6.  Manipulation of the default UC will not be applied on the non-MC
*           prefixes. The application should manipulate those non-MC entries
*           directly when needed.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterSharedAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC        *vrConfigPtr
);

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterGet
*
* DESCRIPTION:
*   This function gets the virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.
*
* OUTPUTS:
*       vrConfigPtr    - Virtual router configuration
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    OUT CPSS_DXCH_IP_LPM_VR_CONFIG_STC          *vrConfigPtr
);

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterDel
*
* DESCRIPTION:
*   This function removes a virtual router in system for a specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterDel
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPADDR                                ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of prefixes' lengths is too big
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
);


/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv4 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPADDR                            ipAddr,
    IN GT_U32                               prefixLen
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDelBulk
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv4 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of the prefixes' lengths is too big
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
);


/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv4 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr       - if found, this is the route entry info.
*                               accosiated with this UC prefix.
*       tcamRowIndexPtr      - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr   - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
);



/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one and prefix Tcam index; it used for iterating over the exisiting
*   prefixes.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - The ip address of the found entry.
*       prefixLenPtr         - The prefix length of the found entry.
*       nextHopInfoPtr       - If found, this is the route entry info accosiated
*                              with this UC prefix.
*       tcamRowIndexPtr      - TCAM row index of this uc prefix.
*       tcamColumnIndexPtr   - TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT GT_U32                                      *tcamRowIndexPtr,
    OUT GT_U32                                      *tcamColumnIndexPtr

);

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*   Add IP multicast route for a particular/all source and a particular
*   group address. this is done for a specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual private network identifier.
*       ipGroup             - The IP multicast group address.
*       ipGroupPrefixLen    - The number of bits that are actual valid in,
*                             the ipGroup.
*       ipSrc               - the root address for source base multi tree
*                             protocol.
*       ipSrcPrefixLen      - The number of bits that are actual valid in,
*                             the ipSrc.
*       mcRouteLttEntryPtr  - the LTT entry pointing to the MC route entry
*                             associated with this MC route.
*       override            - whether to override an mc Route pointer for the
*                             given prefix
*       defragmentationEnable - whether to enable performance costing
*                             de-fragmentation process in the case that there
*                             is no place to insert the prefix. To point of the
*                             process is just to make space for this prefix.
*                             relevant only if the LPM DB was created with
*                             partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - if one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPADDR                    ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPADDR                    ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC   *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      defragmentationEnable
);

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPADDR    ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPADDR    ipSrc,
    IN GT_U32       ipSrcPrefixLen
);

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc,ipGroup TCAM indexes.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr                 - The next ip Group address.
*       ipGroupPrefixLenPtr        - the ipGroup prefix Length.
*       ipSrcPtr                   - The next ip Source address.
*       ipSrcPrefixLenPtr          - ipSrc prefix length.
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route.
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1.  The value of ipGroupPtr must be a valid value, it
*           means that it exists in the IP-Mc Table, unless this is the first
*           call to this function, then it's value is 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryGetNext
(
    IN    GT_U32                        lpmDBId,
    IN    GT_U32                        vrId,
    INOUT GT_IPADDR                     *ipGroupPtr,
    INOUT GT_U32                        *ipGroupPrefixLenPtr,
    INOUT GT_IPADDR                     *ipSrcPtr,
    INOUT GT_U32                        *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    OUT GT_U32                          *tcamGroupRowIndexPtr,
    OUT GT_U32                          *tcamGroupColumnIndexPtr,
    OUT GT_U32                          *tcamSrcRowIndexPtr,
    OUT GT_U32                          *tcamSrcColumnIndexPtr
);


/*******************************************************************************
* cpssDxChIpLpmIpv4McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPADDR                   ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPADDR                   ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamGroupColumnIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr,
    OUT GT_U32                      *tcamSrcColumnIndexPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix in a Virtual Router
*   for the specified LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If prefix length is too big.
*       GT_ERROR                 - If the vrId was not created yet.
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPV6ADDR                              ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override,
    IN GT_BOOL                                  defragmentationEnable
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArrayPtr - The UC prefix array.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArrayPtr,
    IN GT_BOOL                              defragmentationEnable
);


/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv6 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipAddr,
    IN GT_U32                               prefixLen
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDelBulk
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv6 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big, or
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArrayPtr
);


/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv6 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it and TCAM prefix index.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If  found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - if found, TCAM row index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one and prefix Tcam index; it used for iterating over the exisiting
*   prefixes.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - The ip address of the found entry.
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - If found, TCAM row index of this uc prefix.
*
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT GT_U32                                      *tcamRowIndexPtr
);


/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*   To add the multicast routing information for IP datagrams from a particular
*   source and addressed to a particular IP multicast group address for a
*   specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRouteLttEntryPtr - the LTT entry pointing to the MC route entry
*                            associated with this MC route.
*       override        - weather to override the mcRoutePointerPtr for the
*                         given prefix
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented .
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = ipGroupPrefixLen = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPV6ADDR                  ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPV6ADDR                  ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC   *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      defragmentationEnable
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPV6ADDR  ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPV6ADDR  ipSrc,
    IN GT_U32       ipSrcPrefixLen
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc,ipGroup TCAM indexes.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - ipGroup prefix length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr           - The next ip Group address.
*       ipGroupPrefixLenPtr  - ipGroup prefix length.
*       ipSrcPtr             - The next ip Source address.
*       ipSrcPrefixLenPtr    - ipSrc prefix length.
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1.  The values of (ipGroupPtr,ipGroupPrefixLenPtr) must be a valid
*           values, it means that they exist in the IP-Mc Table, unless this is
*           the first call to this function, then the value of (ipGroupPtr,
*           ipSrcPtr) is (0,0).
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryGetNext
(
    IN    GT_U32                      lpmDBId,
    IN    GT_U32                      vrId,
    INOUT GT_IPV6ADDR                 *ipGroupPtr,
    INOUT GT_U32                      *ipGroupPrefixLenPtr,
    INOUT GT_IPV6ADDR                 *ipSrcPtr,
    INOUT GT_U32                      *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                        *tcamGroupRowIndexPtr,
    OUT GT_U32                        *tcamSrcRowIndexPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
*       vrId              - The virtual router Id.
*       ipGroup           - The ip Group address to get the entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPV6ADDR                 ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPV6ADDR                 ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr
);



/*******************************************************************************
* cpssDxChIpLpmDBMemSizeGet
*
* DESCRIPTION:
*   This function gets the memory size needed to export the Lpm DB,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       numOfEntriesInStep - how many entries in each iteration step
*       lpmDbSizePtr       - the table size calculated up to now (start with 0)
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbSizePtr  - the table size calculated (in bytes)
*       iterPtr       - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK on success
*       GT_FAIL otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBMemSizeGet
(
    IN    GT_U32 lpmDBId,
    INOUT GT_U32 *lpmDbSizePtr,
    IN    GT_U32 numOfEntriesInStep,
    INOUT GT_U32 *iterPtr
);

/*******************************************************************************
* cpssDxChIpLpmDBExport
*
* DESCRIPTION:
*   This function exports the Lpm DB into the preallocated memory,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       numOfEntriesInStep - how many entries in each iteration step
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbPtr  - the lpm DB info block allocated
*       iterPtr   - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK on success
*       GT_FAIL otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBExport
(
    IN    GT_U32 lpmDBId,
    OUT   GT_U32 *lpmDbPtr,
    IN    GT_U32 numOfEntriesInStep,
    INOUT GT_U32 *iterPtr
);

/*******************************************************************************
* cpssDxChIpLpmDBImport
*
* DESCRIPTION:
*   This function imports the Lpm DB recived and reconstruct it,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       numOfEntriesInStep - how many entries in each iteration step
*       lpmDbPtr           - the lpm DB info block
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       iterPtr   - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK on success
*       GT_FAIL otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBImport
(
    IN    GT_U32 lpmDBId,
    IN    GT_U32 *lpmDbPtr,
    IN    GT_U32 numOfEntriesInStep,
    INOUT GT_U32 *iterPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChIpLpmh */
