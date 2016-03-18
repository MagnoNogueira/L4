/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpPatTrie.h
*
* DESCRIPTION:
*       This file includes implementation of a patricia trie for the purpose
*       of hierarchy managing to the Ip tree structure
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/

#ifndef __cpssDxChIpPatTrieh
#define __cpssDxChIpPatTrieh

#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/prvCpssDxChIpTcamEntriesHandlerTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChIpTcamMgm.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IP_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES 16 /*ecmp - 65*/

struct PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STCT;

/*
 * typedef: enum PRV_PAT_TRIE_PREFIX_FLAG_TYPE
 *
 * Description: type of PREFIX FLAG in the PAT TRIE node
 *
 * Enumerations:
 *  PRV_PAT_TRIE_PREFIX_FLAG_ADDR   - the PAT TRIE node holds a prefix which is
 *                                    part ip address
 *  PRV_PAT_TRIE_PREFIX_FLAG_ECMP   - the PAT TRIE node holds a prefix which is
 *                                    part "ECMP prefix"
 */
typedef enum
{
    PRV_PAT_TRIE_PREFIX_FLAG_ADDR     = 0,
    PRV_PAT_TRIE_PREFIX_FLAG_ECMP     = 1
}PRV_PAT_TRIE_PREFIX_FLAG_TYPE;

/*
 * typedef: enum PRV_PAT_TRIE_NODE_UPDATE_STATUS_ENT
 *
 * Description: the update status of a node in the pat trie.
 *
 * Enumerations:
 *  PRV_PAT_TRIE_NODE_UPDATE_STATUS_OK_E - there is no action needed for this
 *                                         node, or it's children.
 *  PRV_PAT_TRIE_NODE_UPDATE_CHILDREN_E  - some of this node's children need a
 *                                        hw update.
 *  PRV_PAT_TRIE_NODE_UPDATE_HW_E        - this node needs a HW update.
 *  PRV_PAT_TRIE_NODE_INVALIDATE_FIRST_E - this means that there is a need of
 *                                        a hw update which will be over an
 *                                        existing one, which means we need to
 *                                        invlaidate first.
 *  PRV_PAT_TRIE_NODE_NEW_INSERT_E - this is new node create by the insert bulk
 *                                  it should be writen only all the children
 *                                  are in there place.
 */
typedef enum
{
    PRV_PAT_TRIE_NODE_UPDATE_STATUS_OK_E = 0,
    PRV_PAT_TRIE_NODE_UPDATE_CHILDREN_E = 1,
    PRV_PAT_TRIE_NODE_UPDATE_HW_E = 2,
    PRV_PAT_TRIE_NODE_INVALIDATE_FIRST_E = 4,
    PRV_PAT_TRIE_NODE_NEW_INSERT_E = 8
}PRV_PAT_TRIE_NODE_UPDATE_STATUS_ENT;

/****************************************************************************
 * Ip-Patricia Trie related Data structures                                 *
 ****************************************************************************/

/*
 * typedef: struct PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC
 *
 * Description: the Ip patricia trie node structure.
 *
 *
 * fields:
 *      father          - the parent node pointer
 *      leftSon         - the left son (starts with 0) node pointer
 *      rightSon        - the right son (starts with 1) node pointer
 *      ipAddr          - the partial ip address this node holds
 *      updateStatus    - this the update status of this node.
 *                        in the case this is an invalid node then it will
 *                        just indicate weather some of it's sons need update.
 *      size            - the size of the trie this node is a root for.
 *      prefixFlagType  - the type of node this is (ecmp extention or regular
 *                        address)
 *      prefixLen       - out of the ipAddr the relevant prefix length
 *      ruleNode        - the rule that this node is in charge of.
 *      pData           - A pointer to the node's data, also indicates validity
 *                        if != NULL
 *      vrId            - virtual router associated with this entry
 *      entryType       - type of the entry
 *      ipTcamFuncsPtrs - set of TCAM functions used to add this prefix
 */
typedef struct PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STCT
{
   struct PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STCT        *father;
   struct PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STCT        *leftSon;
   struct PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STCT        *rightSon;
   GT_U8                                             ipAddr[IP_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES];
   GT_U8                                             updateStatus;
   GT_U16                                            size;
   GT_U8                                             prefixFlagType;
   GT_U32                                            prefixLen;
   struct PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STCT       *ruleNode;
   GT_PTR                                            pData;
   GT_U32                                            vrId;
   PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT entryType;
   struct PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_FUNCS_OBJ_STCT *ipTcamFuncsPtr;

}PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_IP_PAT_TRIE_OLD_NODE_REC_STC
 *
 * Description: the Ip patricia trie old node record for bulk insertion
 *
 *
 * fields:
 *      oldNode - the pointer for the old node that used to sit here
 *      pData   - the real data of this node.
 */
typedef struct PRV_CPSS_DXCH_IP_PAT_TRIE_OLD_NODE_REC_STCT
{
   PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC  *oldNode;
   GT_PTR                              pData;
}PRV_CPSS_DXCH_IP_PAT_TRIE_OLD_NODE_REC_STC;


/*******************************************************************************
* PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_TRAVERSE_FUN
*
* DESCRIPTION:
*       function protoType to use when traversing.
*
* INPUTS:
*       vrId        - the virtual router id.
*       ipAddr      - IP Uc address
*       prefixLen   - Ip Uc prefix length
*       trieNodePtr - the trie node that we are currently at.
*       extraData   - extra passed data
*       devListPtr  - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_TRAVERSE_FUN)
(
    IN GT_U32                                         vrId,
    IN GT_U8                                          *ipAddr,
    IN GT_U32                                         prefixLen,
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC             *trieNodePtr,
    IN GT_PTR                                         extraData,
    IN struct PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STCT  *devListPtr
);

/*******************************************************************************
* PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_SET_PREFIX_FUN
*
* DESCRIPTION:
*       utility function to Add IP UC prefix.
*
* INPUTS:
*       vrId        - the virtual router id.
*       ipAddr      - IP Uc address
*       prefixLen   - Ip Uc prefix length
*       ecmpRouteNum- the ecmp route number to write
*       tcamRulePtr - the tcam rule to set it in
*       pData       - the data to attach to this prefix.
*       devListPtr - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If we want to add a default IP group, then prefixLen should be 0.
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_SET_PREFIX_FUN)
(
    IN  GT_U32                                        vrId,
    IN  GT_U8                                         *ipAddr,
    IN  GT_U32                                        prefixLen,
    IN  GT_U8                                         ecmpRouteNum,
    IN  PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC           *tcamRulePtr,
    IN  GT_PTR                                        pData,
    IN  struct PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STCT *devListPtr
);

/*******************************************************************************
* PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_DEL_PREFIX_FUN
*
* DESCRIPTION:
*       utility function to Delete IP prefix PCE from IP table.
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
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_DEL_PREFIX_FUN)
(
    IN  GT_U32                                        vrId,
    IN  PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC           *tcamRulePtr,
    IN  GT_PTR                                        pData,
    IN  struct PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STCT *devListPtr
);




/*******************************************************************************
* PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_MOVE_PREFIX_FUN
*
* DESCRIPTION:
*       utility function to Update move PCE entry action in IP pCL table.
*
* INPUTS:
*       vrId                - the virtual router id.
*       moveFromTcamRulePtr - the tcam rule to move from.
*       moveToTcamRulePtr   - the tcam rule to move to .
*       pData               - the data entry that that is attached to this moved
*                           prefix.
*       devListPtr - pointer to the device list to preform the actions on.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_MOVE_PREFIX_FUN)
(
    IN  GT_U32                                        vrId,
    IN  PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC           *moveFromTcamRulePtr,
    IN  PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC           *moveToTcamRulePtr,
    IN  GT_PTR                                        pData,
    IN  struct PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STCT *devListPtr
);

/*******************************************************************************
* PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_UPDATE_DATA_IDX_FUN
*
* DESCRIPTION:
*       utility function to update the tcam rule with the data entry given.
*
* INPUTS:
*       oldDataEntry - the data entry currently in.
*       dataEntry    - the data entry to set.
*       tcamRulePtr  - the tcam rule needs updating
*       devListPtr   - pointer to the device list to preform the actions on.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_UPDATE_DATA_IDX_FUN)
(
    IN  GT_PTR                                        oldDataEntry,
    IN  GT_PTR                                        dataEntry,
    IN  PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC           *tcamRulePtr,
    IN  struct PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STCT *devListPtr
);


/*
 * typedef: struct PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_FUNCS_OBJ_STC
 *
 * Description:
 *      IP Tcam utility functions Object.
 *
 * Fields:
 *      workDevListPtr   - pointer to the device list to preform the actions on.
 *      ipTcamSetPrefix  - utility function to Add IP UC prefix.
 *      ipTcamDelPrefix  - utility function to Delete IP prefix PCE from IP
 *                         table.
 *      ipTcamMovePrefix - utility function to Update move PCE entry action in
 *                         IP pCL table..
 *      ipTcamUpdateDataIdx - utility function to update the offset/pceNum (idx)
 *                            in the data entry given.
 *
 * Comments:
 *
 */

typedef struct PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_FUNCS_OBJ_STCT
{

    struct PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STCT        **workDevListPtr;

    PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_SET_PREFIX_FUN        ipTcamSetPrefix;
    PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_DEL_PREFIX_FUN        ipTcamDelPrefix;
    PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_MOVE_PREFIX_FUN       ipTcamMovePrefix;
    PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_UPDATE_DATA_IDX_FUN   ipTcamUpdateDataIdx;
}PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_FUNCS_OBJ_STC;

/******************************************************************************
* cpssDxChIpPatTrieInsert
*
* DESCRIPTION:
*       Insert a new NEXT_POINTER_PTR to the ip trie structure.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       vrId            - the virtual router id.
*       rootPtr         - Root of the trie to insert the entry.
*       tcamEntriesHandlerPtr - points to TCAM entries handler
*       entryType       - entry type
*       ipAddrPtr       - IP address associated with the given entry.
*       prefixLen       - prefix of ipAddr.
*       ecmpPrefixLen   - the "ECMP prefix" length (the route num of the ecmp next
*                         hop)
*       entry           - The entry to be inserted.
*       isMaxPrefix     - is this prefix is at max prefix legnth for this protocol
*                         stack.
*       isBulkInsert    - the type of insert this is: Bulk or regular.
*       defragEnable    - whether to defrag entries in case there is no
*                         free place for this entry cause of entries
*                         fragmentation; relevant only if the TCAM entries
*                         handler was created with partitionEnable = GT_FALSE
*       ruleNodePtr     - if this isn't NULL then this is the rule node which this
*                         prefix should occupy (HSU use)
*       ipTcamFuncsPtr  - the utility functions to use for tcam access
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if succeeded,
*       GT_OUT_OF_CPU_MEM if failed to allocate memory,
*       GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPatTrieInsert
(
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC           *rootPtr,
    IN PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_STC       *tcamEntriesHandlerPtr,
    IN PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT    entryType,
    IN GT_U8                                        *ipAddrPtr,
    IN GT_U32                                       prefixLen,
    IN GT_U8                                        ecmpPrefixLen,
    IN GT_PTR                                       entry,
    IN GT_BOOL                                      isMaxPrefix,
    IN GT_BOOL                                      isBulkInsert,
    IN  GT_BOOL                                     defragEnable,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC          *ruleNodePtr,
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_FUNCS_OBJ_STC *ipTcamFuncsPtr
);

/*******************************************************************************
* cpssDxChIpPatTrieSearch
*
* DESCRIPTION:
*       Search for the entry which is associated with ipAddr & ipPrefix .
*
* INPUTS:
*       rootPtr     - Root of the trie to search for the entry.
*       ipAddr      - IP address associated with the given entry.
*       ipPrefix    - prefix of ipAddr.
*       entryPtr    - Indicates whether to return the entry in the searched node
*                   or not.
*
* OUTPUTS:
*       entry   - A pointer to the searched entry if found, NULL otherwise.
*       numOfRules - the number of rules this prefix takes in the TCAM (for ecmp
*                    connected to prefixes)
*
* RETURNS:
*       A pointer to the node in the trie including the searched entry.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC* cpssDxChIpPatTrieSearch
(
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC   *rootPtr,
    IN GT_U8                                *ipAddr,
    IN GT_U32                               prefixLen,
    OUT GT_U32                              *numOfRules,
    INOUT GT_PTR                            *entryPtr
);

/*******************************************************************************
* cpssDxChIpPatTrieGetAddrFromNode
*
* DESCRIPTION:
*       construct an ip address from a given node
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       nodePtr       - the node form which to start constructing
*
* OUTPUTS:
*       ipAddrPtr       - IP address associated with the given node.
*       prefixLenPtr    - prefix of ipAddr.
*
* RETURNS:
*       GT_OK
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPatTrieGetAddrFromNode
(
    IN  PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC    *nodePtr,
    OUT GT_U8                                 *ipAddrPtr,
    OUT GT_U32                                *prefixLenPtr
);

/*******************************************************************************
* ipPatTrieGetNext
*
* DESCRIPTION:
*       Search for the entry which is associated with larger ipAddr & ipPrefix .
*
* INPUTS:
*       rootPtr         - Root of the trie to search for the entry.
*       ipAddr          - IP address associated with the given entry.
*       prefixLenPtr    - prefix of ipAddr.
*       entryPtr        - Indicates whether to return the entry in the searched node
*                       or not.
*
* OUTPUTS:
*       ipAddr    - the address of the next prefix
*       prefixLen - the prefix length of the next prefix
*       entryPtr  - A pointer to the searched entry if found, NULL otherwise.
*
* RETURNS:
*       A pointer to the node in the trie including the next entry.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC* cpssDxChIpPatTrieGetNext
(
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC      *rootPtr,
    INOUT GT_U8                                *ipAddr,
    INOUT GT_U32                               *prefixLenPtr,
    INOUT GT_PTR                               *entryPtr
);

/*******************************************************************************
* cpssDxChIpPatTrieWriteToHw
*
* DESCRIPTION:
*       write the whole patricai tree to the HW.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       vrId            - the virtual router id.
*       rootPtr         - Root of the trie to search for the entry.
*       ipTcamFuncsPtr  - the utility functions to use for tcam access
*       isUpdateOnly    - is this an update only or a whole trie overwrite.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPatTrieWriteToHw
(
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC           *rootPtr,
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_FUNCS_OBJ_STC *ipTcamFuncsPtr,
    IN GT_BOOL                                      isUpdateOnly
);

/*******************************************************************************
* cpssDxChIpPatTrieDel
*
* DESCRIPTION:
*       Deletes a previously inserted entry.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       vrId          - the virtual router id.
*       rootPtr       - Root of the trie to delete from.
*       tcamEntriesHandlerPtr - points to TCAM entries handler
*       entryType     - entry type
*       ipAddrPtr     - IP address associated with the entry to be deleted.
*       prefixLen     - prefix of ipAddr.
*       ecmpPrefixLen - the "ECMP prefix" length (the route num of the ecmp next
*                       hop)
*       ipTcamFuncsPtr- the utility functions to use for tcam access
*
* OUTPUTS:
*       entryPtr   - A pointer to the entry of the deleted node if found, NULL
*                 otherwise.
* RETURNS:
*       GT_OK if succeeded, GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function does not free the pointers to the data stored in the
*       trie node, it's the responsibility of the caller to do that.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPatTrieDel
(
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC           *rootPtr,
    IN PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_STC       *tcamEntriesHandlerPtr,
    IN PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT    entryType,
    IN GT_U8                                        *ipAddrPtr,
    IN GT_U32                                       prefixLen,
    IN GT_U8                                        ecmpPrefixLen,
    OUT GT_PTR                                      *entryPtr,
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_FUNCS_OBJ_STC *ipTcamFuncsPtr
);

/*******************************************************************************
* cpssDxChIpPatTrieInit
*
* DESCRIPTION:
*       Initialize a new Pat-Trie for the prefixes
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       None
*
* OUTPUTS:
*       rootPtrPtr - the root of the new trie.
*
* RETURNS:
*       GT_OK if succeeded, GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPatTrieInit
(
    OUT PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC **rootPtrPtr
);

/*******************************************************************************
* cpssDxChIpPatTrieDestroy
*
* DESCRIPTION:
*       Delete a Pat-Trie , only if it is an empty one
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       rootPtr - the root of the trie.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK if succeeded, GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPatTrieDestroy
(
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC *rootPtr
);

/*******************************************************************************
* cpssDxChIpPatTrieFlush
*
* DESCRIPTION:
*       flushes all trie (along with deleting all data nodes).
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       rootPtr         - Root of the trie to delete from.
*       tcamEntriesHandlerPtr - points to TCAM entries handler
*       entryType   - entry type
*       ipTcamFuncsPtr  - the utility functions to use for tcam access
*       dataFreeFunc    - A pointer to a function to be called for deleteing the
*                       user data, or NULL if no free operation should be done
*                       on the user's data.
*       flushDefault    - weather to flush the default rule as well
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpPatTrieFlush
(
    IN  PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC                  *rootPtr,
    IN PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_STC               *tcamEntriesHandlerPtr,
    IN PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT    entryType,
    IN  PRV_CPSS_DXCH_IP_PAT_TRIE_TCAM_FUNCS_OBJ_STC        *ipTcamFuncsPtr,
    IN  GT_VOIDFUNCPTR                                      dataFreeFunc,
    IN  GT_BOOL                                             flushDefault
);

/*******************************************************************************
* cpssDxChIpPatTrieIsEmpty
*
* DESCRIPTION:
*       check weather the trie is empty
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       rootPtr - the root of the trie.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       weather the trie is empty
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL cpssDxChIpPatTrieIsEmpty
(
    IN PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC *rootPtr
);

/*******************************************************************************
* prvCpssDxChIpPatTrieRelocatePrefix
*
* DESCRIPTION:
*       Move a prefix from a specific location in the TCAM to another location
*       in the TCAM.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       tcamHandlerPtr        - points to TCAM entries handler
*       prefixPtr             - the prefix to relocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                 - if success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpPatTrieRelocatePrefix
(
    IN  PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_STC  *tcamHandlerPtr,
    IN  PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC      *prefixPtr
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtIpTrieh */



