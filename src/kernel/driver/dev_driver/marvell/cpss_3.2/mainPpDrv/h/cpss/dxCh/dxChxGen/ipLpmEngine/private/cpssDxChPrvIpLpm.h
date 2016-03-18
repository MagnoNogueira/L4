/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPrvIpLpm.h
*
* DESCRIPTION:
*       This file includes internal typedefs declarations for controlling the
*       IP tables and structures, and structures definitions for shadow
*       management.
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*
*******************************************************************************/
#ifndef __cpssDxChPrvIpLpmh
#define __cpssDxChPrvIpLpmh

#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpmTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* cpssDxChIpTcamInit
*
* DESCRIPTION:
*       Init the Ip unit
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       shadowType      - the type of shadow we're building (cheetah+ or
*                         cheetah2)
*       rangeIndexsPtr  - the range of TCAM indexes availble for this
*                         LPM DB (see explanation in
*                         CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC)
*       partitionEnable - weather to enable partitioning of the router
*                         TCAM according to the
*                         tcamLpmManagerCapcityCfgPtr.
*       capcityCfgPtr   - The capcity configuration struct.
*       protocolStack - the protocol stack to support
*
* OUTPUTS:
*       ipTcamShadowPtrPtr - the ip shadow we created.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamInit
(
    IN  CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT                shadowType,
    IN  CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC     *rangeIndexsPtr,
    IN  GT_BOOL                                          partitionEnable,
    IN  CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC     *capcityCfgPtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT                       protocolStack,
    OUT PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC                 **ipTcamShadowPtrPtr
);

/*******************************************************************************
* cpssDxChIpTcamCheetah2VrSupportInit
*
* DESCRIPTION:
*       Init the Ip unit for Cheetah2 devices with virtual router support.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       protocolStack    - the type of protocol stack this LPM DB support.
*       pclTcamCfgPtr    - the pcl tcam configuration
*       routerTcamCfgPtr - the router tcam configuration
*
* OUTPUTS:
*       ipTcamShadowPtrPtr  - the ip shadow we created.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamCheetah2VrSupportInit
(
    IN  CPSS_IP_PROTOCOL_STACK_ENT                              protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr,
    OUT PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC                        **ipTcamShadowPtrPtr
);

/*******************************************************************************
* cpssDxChIpTcamAddVirtualRouter
*
* DESCRIPTION:
*       This function adds a new virtual router to the valid virtual routers
*       in system for specific shared shadow devices.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       vrId                        - The virtual's router ID (in cheetah+ this
*                                     will be the pcl id).
*       defIpv4UcNextHopInfoPtr     - The next hop info representing the default IPv4-Uc
*                                     route entry.
*       defIpv6UcNextHopInfoPtr     - The next hop info representing the default IPv6-Uc
*                                     route entry.

*       defUcNextHopRuleIdxArr      - if not NULL, then this is the internal rule idx
*                                     of the defualt NextHop prefix. (array of 2
*                                     for each of the protocol Stacks)
*       defIpv4McRouteLttEntryPtr   - The default IPv4_Mc LTT entry
*       defIpv6McRouteLttEntryPtr   - The default IPv6_Mc LTT entry
*       defMcRouteRuleIdxArr        - if not null then this is the internal Rule idx
*                                     of the defautl mc route (prefix) (array of 2
*                                     for each of the protocol Stacks)
*       protocolStack               - types of IP stack used in this virtual router.
*       ipTcamShadowPtr             - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the given next hop Id is not valid.
*       GT_ALREADY_EXIST         - if the virtual router already exists
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamAddVirtualRouter
(
    IN GT_U32                                 vrId,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defIpv4UcNextHopInfoPtr,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defIpv6UcNextHopInfoPtr,
    IN GT_U32                                 *defUcNextHopRuleIdxArr[],
    IN CPSS_DXCH_IP_LTT_ENTRY_STC             *defIpv4McRouteLttEntryPtr,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC             *defIpv6McRouteLttEntryPtr,
    IN GT_U32                                 *defMcRouteRuleIdxArr[],
    IN CPSS_IP_PROTOCOL_STACK_ENT             protocolStack,
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC       *ipTcamShadowPtr
);

/*******************************************************************************
* cpssDxChIpTcamGetVirtualRouter
*
* DESCRIPTION:
*       This function gets the virtual router configuration
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       vrId                    - The virtual's router ID (in cheetah+ this
*                                 will be the pcl id).
*       ipTcamShadowPtr         - the ip shadow we are working on.
*
* OUTPUTS:
*       supportIpv4UcPtr           - Whether the virtual router supports IPv4 UC
*       defIpv4UcNextHopInfoPtr    - The next hop info representing the default Ipv4 Uc
*                                    route entry.
*                                    Relevant only when <supportIpv4UcPtr> == GT_TRUE
*       supportIpv6UcPtr           - Whether the virtual router supports IPv6 UC
*       defIpv6UcNextHopInfoPtr    - The next hop info representing the default Ipv6 Uc
*                                    route entry.
*                                    Relevant only when <supportIpv6UcPtr> == GT_TRUE

*       defUcNextHopRuleIdxArr     - if not NULL, then this is the internal rule idx
*                                    of the defualt NextHop prefix. (array of 2
*                                    for each of the protocol Stacks)
*                                    Relevant only when <supportIpv4UcPtr> == GT_TRUE
*                                    or <supportIpv6UcPtr> == GT_TRUE
*       supportIpv4McPtr           - Whether the virtual router supports Ipv4 MC
*       defIpv4McRouteLttEntryPtr  - The default Ipv4 MC LTT entry
*                                    Relevant only when <supportIpv4McPtr> == GT_TRUE
*       supportIpv6McPtr           - Whether the virtual router supports Ipv6 MC
*       defIpv6McRouteLttEntryPtr  - The default Ipv6 MC LTT entry
*                                    Relevant only when <supportIpv6McPtr> == GT_TRUE
*       defMcRouteRuleIdxArr       - if not null then this is the internal Rule idx
*                                    of the defautl mc route (prefix) (array of 2
*                                    for each of the protocol Stacks)
*                                    Relevant only when <supportIpv4McPtr> == GT_TRUE
*                                    or <supportIpv6McPtr> == GT_TRUE
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the given next hop Id is not valid.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamGetVirtualRouter
(
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC        *ipTcamShadowPtr,
    OUT GT_BOOL                                 *supportIpv4UcPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *defIpv4UcNextHopInfoPtr,
    OUT GT_BOOL                                 *supportIpv6UcPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *defIpv6UcNextHopInfoPtr,
    OUT GT_U32                                  *defUcNextHopRuleIdxArr[],
    OUT GT_BOOL                                 *supportIpv4McPtr,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC              *defIpv4McRouteLttEntryPtr,
    OUT GT_BOOL                                 *supportIpv6McPtr,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC              *defIpv6McRouteLttEntryPtr,
    OUT GT_U32                                  *defMcRouteRuleIdxArr[]
);

/*******************************************************************************
* cpssDxChIpTcamDeleteVirtualRouter
*
* DESCRIPTION:
*       This function deletes an existing virtual router from the vr table, The
*       IP tables (Uc & Mc) are flushed as a result of deletion.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       vrId          - The virtual's router ID to be deleted.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if the given vrId does not exist.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamDeleteVirtualRouter
(
    IN GT_U32                              vrId,
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC    *ipTcamShadowPtr
);

/*******************************************************************************
* cpssDxChIpTcamAddDevToShadow
*
* DESCRIPTION:
*       add the device to the ip shadow
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devListPtr  - The device list to add.
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamAddDevToShadow
(
    IN  PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STC *devListPtr,
    IN  PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC      *ipTcamShadowPtr
);


/*******************************************************************************
* cpssDxChIpTcamRemoveDevsFromShadow
*
* DESCRIPTION:
*       removes the devices from belonging to the shadow.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devListPtr  - The device list to remove
*       ipTcamShadowPtr - the ip shadow we are working on.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamRemoveDevsFromShadow
(
    IN PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STC *devListPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC      *ipTcamShadowPtr
);


/*******************************************************************************
* cpssDxChIpTcamTrieSetPrefixIpCheetah2
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       vrId        - The virtual's router ID.
*       ipAddrPtr   - IP Uc address
*       prefixLen   - Ip Uc prefix length
*       ecmpRouteNum- the ecmp route number to write
*       gIndex      - the G-index to write
*       tcamRulePtr - the tcam rule to set it in
*       pData       - the data to attach to this prefix.
*       protocolStack- the type of the prefix.
*       devListPtr - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to add a default IP group, use prefixLen = 0.
*       2.  Bit 15 in the vrId indicates whether this is shared virtual router
*           or not (if the bit is 1 this is shared virtual router).
*           For shared virtual router:
*             - <match all> entries (meaning perfix len is 0) are not written to
*               the hardware
*             - all other prefixes are written with <don't care> masking for
*               the virtual router field.
*******************************************************************************/
GT_STATUS cpssDxChIpTcamTrieSetPrefixIpCheetah2
(
    IN  GT_U32                                  vrId,
    IN  GT_U8                                   *ipAddrPtr,
    IN  GT_U32                                  prefixLen,
    IN  GT_U8                                   ecmpRouteNum,
    IN  GT_U16                                  gIndex,
    IN  PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC     *tcamRulePtr,
    IN  GT_PTR                                  pData,
    IN  CPSS_IP_PROTOCOL_STACK_ENT              protocolStack,
    IN  PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STC   *devListPtr
);

/*******************************************************************************
* cpssDxChIpTcamTrieDelPrefixCheetah2
*
* DESCRIPTION:
*       utility function to Delete IP prefix rule from IP table.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       vrId            - the virtual router id.
*       tcamRulePtr     - the tcam rule to delete it from.
*       pData           - the data entry that was attached to this prefix.
*       protocolStack   - the protocol stack of the prefix.
*       devListPtr      - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpTcamTrieDelPrefixCheetah2
(
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC     *tcamRulePtr,
    IN  GT_PTR                                  pData,
    IN  CPSS_IP_PROTOCOL_STACK_ENT              protocolStack,
    IN  PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STC   *devListPtr
);

/*******************************************************************************
* cpssDxChIpTcamTrieDelIpv4PrefixCheetah2
*
* DESCRIPTION:
*       utility function to Delete IP prefix rule from IP table.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       vrId        - the virtual router id.
*       tcamRulePtr - the tcam rule to delete it from.
*       pData       - the data entry that was attached to this prefix.
*       devListPtr  - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpTcamTrieDelIpv4PrefixCheetah2
(
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC     *tcamRulePtr,
    IN  GT_PTR                                  pData,
    IN  PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STC   *devListPtr
);


/*******************************************************************************
* cpssDxChIpTcamTrieDelIpv6PrefixCheetah2
*
* DESCRIPTION:
*       utility function to Delete IP prefix rule from IP table.
*
* APPLICABLE DEVICES: All DxCh2 devices and above
*
* INPUTS:
*       vrId        - the virtual router id.
*       tcamRulePtr - the tcam rule to delete it from.
*       pData       - the data entry that was attached to this prefix.
*       devListPtr  - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChIpTcamTrieDelIpv6PrefixCheetah2
(
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC     *tcamRulePtr,
    IN  GT_PTR                                  pData,
    IN  PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STC   *devListPtr
);


/*******************************************************************************
* cpssDxChIpTcamUpdateHW
*
* DESCRIPTION:
*       Perform the actual HW update at the end of a hot sync operation
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       ipTcamShadowPtr - the shadow to write the HW.
*       updateMode      - the update mode.
*       protocolStack   - which protocol stack to update.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamUpdateHW
(
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *ipTcamShadowPtr,
    IN PRV_DXCH_IP_UPDATE_TYPE_ENT       updateMode,
    IN CPSS_IP_PROTOCOL_STACK_ENT        protocolStack
);


/*******************************************************************************
* prvCpssDxChIpLpmPbrDefaultMcRuleSet
*
* DESCRIPTION:
*       Sets "do-nothing" PCL rule to capture all MC traffic for specific
*       protocol stack.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       ipTcamShadowPtr   - the ip shadow we are working on
*       protocolStack     - protocol stack; valid values:
*                               CPSS_IP_PROTOCOL_IPV4_E
*                               CPSS_IP_PROTOCOL_IPV6_E
*       devListPtr        - list of devices to set the rule at
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - Illegal parameter in function called
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmPbrDefaultMcRuleSet
(
    IN  PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC        *ipTcamShadowPtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT              protocolStack,
    IN  PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STC   *devListPtr
);

/*******************************************************************************
* cpssDxChIpTcamLpmDbGetL3
*
* DESCRIPTION:
*       Retrives a specifc shadow's ip Table memory Size needed and info
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       numOfEntriesInStep - how many entries in each iteration step
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       tableSizePtr  - (pointer to) the table size calculated (in bytes)
*       tablePtr      - (pointer to) the table size info block
*       iterPtr       - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamLpmDbGetL3
(
    IN     GT_U32                       lpmDBId,
    OUT    GT_U32                       *tableSizePtr,
    OUT    GT_VOID                      *tablePtr,
    IN     GT_U32                       numOfEntriesInStep,
    INOUT  GT_U32                       *iterPtr
);

/*******************************************************************************
* cpssDxChIpTcamLpmDbSetL3
*
* DESCRIPTION:
*       Set the data needed for core IP shadow reconstruction used after HSU.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       tablePtr            - (pointer to)the table size info block
*       numOfEntriesInStep  - how many entries in each iteration step
*       iterPtr             - the iterator, to start - set to 0.
*
* OUTPUTS:
*       iterPtr - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamLpmDbSetL3
(
    IN     GT_U32                       lpmDBId,
    IN     GT_VOID                      *tablePtr,
    IN     GT_U32                       numOfEntriesInStep,
    INOUT  GT_U32                       *iterPtr
);

/*******************************************************************************
* prvCpssPpConfigDevDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of LPM.
*       NOTE: function not 'free' the allocated memory. only detach from it ,
*             and restore DB to 'pre-init' state
*
*             The assumption is that the 'cpssOsMalloc' allocations will be not
*             valid any more by the application , so no need to 'free' each and
*             every allocation !
*
*  APPLICABLE DEVICES: ALL DXCH devices
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmDbRelease
(
    void
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPrvIpLpmh */


