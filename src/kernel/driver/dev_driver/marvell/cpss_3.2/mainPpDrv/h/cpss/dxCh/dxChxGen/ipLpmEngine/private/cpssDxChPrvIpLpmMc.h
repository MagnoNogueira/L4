/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPrvIpLpmMc.h
*
* DESCRIPTION:
*       This file includes functions declarations for controlling the IP (v6/v4)
*       MC tables and structures, and structures definitions for shadow
*       management.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/
#ifndef __cpssDxChIpLpmMch
#define __cpssDxChIpLpmMch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* cpssDxChIpTcamAddIpMcEntry
*
* DESCRIPTION:
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address.
*
* APPLICABLE DEVICES: All DxCh2 devices and above.
*
* INPUTS:
*       vrId            - The virtual private network identifier.
*       ipGroupPtr      - (pointer to) The IP multicast group address.
*       ipGroupPrefix   - The number of bits that are actual valid in,
*                         the ipGroup. (CIDR like)
*       ipSrcPtr        - (pointer to) the root address for source base multi tree protocol.
*       ipSrcPrefix     - The number of bits that are actual valid in,
*                         the ipSrc. (CIDR like)
*       ipLttEntryPtr   - The Ltt entry pointer to write for this mc route
*       ruleIdxArr      - if not null then this is the internal rule indexes
*                         (G,S) for this prefix (Hsu use)
*       overwrite       - update or override an existing entry, or create a new one.
*       defragEnable    - whether to defrag entries in case there is no
*                         free place for this entry cause of entries
*                         fragmentation; relevant only if the TCAM entries
*                         handler was created with partitionEnable = GT_FALSE
*       protocolStack   - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR - if the virtual router does not exist, or ipGroupPrefix == 0,
*       GT_FAIL                  - otherwise.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If tries to override a non existing entry - return error.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamAddIpMcEntry
(
    IN GT_U32                           vrId,
    IN GT_U8                            *ipGroupPtr,
    IN GT_U32                           ipGroupPrefix,
    IN GT_U8                            *ipSrcPtr,
    IN GT_U32                           ipSrcPrefix,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC       *ipLttEntryPtr,
    IN GT_U32                           *ruleIdxArr[],
    IN GT_BOOL                          overwrite,
    IN GT_BOOL                          defragEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *ipTcamShadowPtr
);

/*******************************************************************************
* cpssDxChIpTcamDeleteIpMcEntry
*
* DESCRIPTION:
*       To delete a particular mc route entry and all its MLLs.
*
* APPLICABLE DEVICES: All DxCh2 devices and above.
*
* INPUTS:
*       vrId            - The virtual private network identifier.
*       ipGroupPtr      - (pointer to)The IP multicast group address.
*       ipGroupPrefix   - The number of bits that are actual valid in,
*                         the ipGroup. (CIDR like)
*       ipSrcPtr        - (pointer to)the root address for source base multi tree protocol.
*       ipSrcPrefix     - The number of bits that are actual valid in,
*                         the ipSrc. (CIDR like)
*       protocolStack  - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_OUT_OF_CPU_MEM if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM if failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamDeleteIpMcEntry
(
    IN GT_U32                           vrId,
    IN GT_U8                            *ipGroupPtr,
    IN GT_U32                           ipGroupPrefix,
    IN GT_U8                            *ipSrcPtr,
    IN GT_U32                           ipSrcPrefix,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *ipTcamShadowPtr
);

/*******************************************************************************
* cpssDxChIpTcamFlushIpMcEntries
*
* DESCRIPTION:
*       flushes the multicast IP Routing table and stays with the default entry
*       only.
*
* APPLICABLE DEVICES: All DxCh2 devices and above.
*
* INPUTS:
*       vrId           - The virtual private network identifier.
*       protocolStack  - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on.
*       flushDefault   - weather to flush the default as well
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamFlushIpMcEntries
(
    IN GT_U32                           vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *ipTcamShadowPtr,
    IN GT_BOOL                          flushDefault
);


/*******************************************************************************
* cpssDxChIpTcamIpMcEntriesWriteToHw
*
* DESCRIPTION:
*       writes to the HW the multicast IP Routing table
*
* APPLICABLE DEVICES: All DxCh2 devices and above.
*
* INPUTS:
*       vrEntryPtr      - The virtual router entry pointer
*       protocolStack   - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on
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
GT_STATUS cpssDxChIpTcamIpMcEntriesWriteToHw
(
    IN PRV_CPSS_DXCH_VR_TCAM_TBL_BLOCK_STC *vrEntryPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT          protocolStack,
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC    *ipTcamShadowPtr
);


/*******************************************************************************
* cpssDxChIpTcamSearchMcEntry
*
* DESCRIPTION:
*       This function searches for the route entries associates with a given
*       (S,G), prefixes.
*
* APPLICABLE DEVICES: All DxCh2 devices and above.
*
* INPUTS:
*       vrId            - The virtual private network identifier.
*       ipGroupArr      - The ip multicast addr. associated with the given mll node.
*       ipGroupPrefix   - ipGroup prefix.
*       ipSrcArr        - The source ip address. associated with the given mll node.
*       srcPrefix       - ipSrc prefix.
*       protocolStack   - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on
*
* OUTPUTS:
*       lttEntryPtr         - pointer to ltt entry struct
*       internalRuleIdxPtr  - if not null then it is filled with internal mangment
*                           pool rule index (Hsu use)
*
* RETURNS:
*       GT_OK if successful, or
*       GT_NOT_FOUND if the given prefix was not found.
*       GT_NOT_SUPPORTED          - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       1.  Calling this function with ipGroupPrefix = 0, will return the
*           default Mc route entries.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamSearchMcEntry
(
    IN GT_U32                             vrId,
    IN GT_U8                              ipGroupArr[],
    IN GT_U32                             ipGroupPrefix,
    IN GT_U8                              ipSrcArr[],
    IN GT_U32                             srcPrefix,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC        *lttEntryPtr,
    OUT GT_U32                            *internalRuleIdxPtr,
    IN  CPSS_IP_PROTOCOL_STACK_ENT        protocolStack,
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC   *ipTcamShadowPtr
);

/*******************************************************************************
* cpssDxChIpTcamMcRuleOffsetGet
*
* DESCRIPTION:
*       This function searches for the  given (S,G) prefixes and returns the
*       Ip Tcam multicast rules offsets associated with it.
*
* APPLICABLE DEVICES: All DxCh2 devices and above.
*
* INPUTS:
*       vrId            - The virtual private network identifier.
*       ipGroupPtr      - (pointer to)The ip multicast addr. associated with the given mll node.
*       ipGroupPrefixLen - ipGroup prefix length.
*       ipSrcPtr        - (pointer to)The source ip address. associated with the given mll node.
*       ipSrcPrefixLen  - ipSrc prefix length.
*       protocolStack   - type of ip protocol stack to work on.
*       ipTcamShadowPtr - the shadow pointer we are working on
*
* OUTPUTS:
*
*       ipGroupTcamRuleOffsetPtr  - pointer to group TCAM rule offset
*       ipSrcTcamRuleOffsetPtr    - pointer to source TCAM rule offset
*
* RETURNS:
*       GT_OK                     -  if successful, or
*       GT_NOT_FOUND              -  if the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE  -  on not applicable device
*       GT_NOT_SUPPORTED          - the request is not supported.
*       GT_BAD_VALUE              -  on bad value
*
* COMMENTS:
*
*        None
*******************************************************************************/
GT_STATUS cpssDxChIpTcamMcRuleOffsetGet
(
    IN GT_U32                             vrId,
    IN GT_U8                              *ipGroupPtr,
    IN GT_U32                             ipGroupPrefixLen,
    IN GT_U8                              *ipSrcPtr,
    IN GT_U32                             ipSrcPrefixLen,
    IN  CPSS_IP_PROTOCOL_STACK_ENT        protocolStack,
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC   *ipTcamShadowPtr,
    OUT GT_U32                            *ipGroupTcamRuleOffsetPtr,
    OUT GT_U32                            *ipSrcTcamRuleOffsetPtr
);

/*******************************************************************************
* cpssDxChIpTcamGetMcEntry
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh2 devices and above.
*
* INPUTS:
*       vrId                - The virtual router Id.
*       ipGroupArr          - The ip Group address to get the next entry for.
*       groupPrefixPtr      - the ipGroup prefix Length.
*       ipSrcArr            - The ip Source address to get the next entry for.
*       srcPrefixPtr        - ipSrc prefix length.
*       protocolStack       - type of ip protocol stack to work on.
*       ipTcamShadowPtr     - the shadow pointer we are working on
*
* OUTPUTS:
*       ipGroupArr          - The next ip Group address.
*       groupPrefixPtr      - the ipGroup prefix Length.
*       ipSrcArr            - The next ip Source address.
*       srcPrefixPtr        - ipSrc prefix length.
*       lttEntryPtr         - the LTT entry pointing to the MC route entry
*                             associated with this MC route.
*       internalRuleIdxArr  - if not null then it is filled with internal mangment
*                             pool rule indexes (G,S) (Hsu use)
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*       GT_NOT_SUPPORTED          - the request is not supported.
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       1.  The value of ipGroupPtr must be a valid value, it
*           means that it exists in the IP-Mc Table, unless this is the first
*           call to this function, then it's value is 0/0
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamGetMcEntry
(
    IN GT_U32                             vrId,
    INOUT GT_U8                           ipGroupArr[],
    INOUT GT_U32                          *groupPrefixPtr,
    INOUT GT_U8                           ipSrcArr[],
    INOUT GT_U32                          *srcPrefixPtr,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC        *lttEntryPtr,
    OUT GT_U32                            *internalRuleIdxArr[],
    IN  CPSS_IP_PROTOCOL_STACK_ENT        protocolStack,
    IN  PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC  *ipTcamShadowPtr
);

/*******************************************************************************
* cpssDxChIpTcamRegisterIpMcFuncs
*
* DESCRIPTION:
*       register the relevant ip functions for that ip protocol
*
* APPLICABLE DEVICES: All DxCh2 devices and above.
*
* INPUTS:
*       shadowType      - the shadow type (cheetah+ or cheetah2)
*       protocolStack   - types of IP stack used in the future virtual routers.
*       ipTcamShadowPtr - the shadow pointer we are working on
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamRegisterIpMcFuncs
(
    IN CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT       shadowType,
    IN CPSS_IP_PROTOCOL_STACK_ENT              protocolStack,
    IN PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC        *ipTcamShadowPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChIpLpmMch */
