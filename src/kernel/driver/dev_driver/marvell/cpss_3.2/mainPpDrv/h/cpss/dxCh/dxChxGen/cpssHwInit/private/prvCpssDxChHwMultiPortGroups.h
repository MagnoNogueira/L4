/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwMultiPortGroups.h
*
* DESCRIPTION:
*       Private API definition for multi-port-groups devices utilities.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

#ifndef __prvCpssDxChHwMultiPortGroupsh
#define __prvCpssDxChHwMultiPortGroupsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssDxChPortGroupBusyWait
*
* DESCRIPTION:
*       function do 'Busy wait' on a bit ,until cleared.
*       function wait on the register in the specified port group.
*       if portGroupId = 'unaware' the function loop on all active
*       port groups and wait for all of them to finish
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - the device number
*       portGroupId  - The port group id.
*                      relevant only to 'multi-port-group' devices.
*                      supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr      - the register address to query
*       selfClearBit - the bit to query
*       onlyCheck    - do we want only to check the current status , or to wait
*                      until ready
*                      GT_TRUE - check status , without busy wait
*                      GT_FALSE - loop on the bit until cleared , on all port-groups
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success.
*       GT_TIMEOUT  - when  onlyCheck == GT_FALSE and the busy wait time expired
*       GT_BAD_STATE - when onlyCheck == GT_TRUE and the bit was not cleared
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupBusyWait
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   selfClearBit,
    IN GT_BOOL  onlyCheck
);

/*******************************************************************************
* prvCpssDxChPortGroupBusyWaitByMask
*
* DESCRIPTION:
*       function do 'Busy wait' on specific mask of the register.
*       function wait on the register in the specified port group.
*       if portGroupId = 'unaware' the function loop on all active
*       port groups and wait for all of them to finish
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       portGroupId   - The port group id.
*                      relevant only to 'multi-port-group' devices.
*                      supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - the register address to query
*       regMask - the mask of the register that we wait for to clear
*       onlyCheck - do we want only to check the current status , or to wait
*                   until ready
*                   GT_TRUE - check status , without busy wait
*                   GT_FALSE - loop on the value until value reached , on all port-groups
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success.
*       GT_TIMEOUT  - when  onlyCheck == GT_FALSE and the busy wait time expired
*       GT_BAD_STATE - when onlyCheck == GT_TRUE and the value was not reached
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupBusyWaitByMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   regMask,
    IN GT_BOOL  onlyCheck
);


/*******************************************************************************
* prvCpssDxChPortGroupsCounterSummary
*
* DESCRIPTION:
*       summary the counters from all the port groups , from the input register
*
* APPLICABLE DEVICES:  All DxCh Devices
*                      -- multi-port-group device and non multi-port-groups device
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to read the counter value from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*
* OUTPUTS:
*       counter32bitValuePtr - (pointer to) the summary counter from the port groups, in 32 bits
*                               when NULL --> ignored
*       counter64bitValuePtr - (pointer to) the summary counter from the port groups, in 64 bits
*                               when NULL --> ignored
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupsCounterSummary
(
    IN GT_U8   devNum,
    IN GT_U32  regAddr,
    IN GT_U32  fieldOffset,
    IN GT_U32  fieldLength,
    OUT GT_U32 *counter32bitValuePtr,
    OUT GT_U64 *counter64bitValuePtr
);

/*******************************************************************************
* prvCpssDxChPortGroupsMultiCounterSummary
*
* DESCRIPTION:
*       summary the counters from all the port groups , from the input register
*       the register hold several (2 or 3) counters
*
* APPLICABLE DEVICES:  All DxCh Devices
*                      -- multi-port-group device and non multi-port-groups device
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to read the counter value from.
*       fieldOffset1 - The start bit number in the register - counter 1
*       fieldLength1 - The number of bits to be written to register - counter 1
*       fieldOffset2 - The start bit number in the register - counter 2
*                   when 0 --> counter ignored
*       fieldLength2 - The number of bits to be written to register - counter 2
*                   when 0 --> counter ignored
*       fieldOffset3 - The start bit number in the register - counter 3
*                   when 0 --> counter ignored
*       fieldLength3 - The number of bits to be written to register - counter 3
*                   when 0 --> counter ignored
*
* OUTPUTS:
*       counterValue1Ptr - (pointer to) the summary counter from the
*                           port groups , in 32 bits - counter 1
*       counterValue2Ptr - (pointer to) the summary counter from the
*                           port groups , in 32 bits - counter 2
*       counterValue3Ptr - (pointer to) the summary counter from the
*                           port groups , in 32 bits - counter 3
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupsMultiCounterSummary
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset1,
    IN GT_U32 fieldLength1,
    OUT GT_U32 *counterValue1Ptr,
    IN GT_U32 fieldOffset2,
    IN GT_U32 fieldLength2,
    OUT GT_U32 *counterValue2Ptr,
    IN GT_U32 fieldOffset3,
    IN GT_U32 fieldLength3,
    OUT GT_U32 *counterValue3Ptr
);

/*******************************************************************************
* prvCpssDxChPortGroupsCounterSet
*
* DESCRIPTION:
*       set the counter on all the port groups , from the input register
*       the function set first active port group with the needed value , and set other
*       port groups with value 0 --> so the summary will not count multi-values
*
* APPLICABLE DEVICES:  All DxCh Devices
*                      -- multi-port-group device and non multi-port-groups device
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to set the counter value to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       counterValue - the counter value to set
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupsCounterSet
(
    IN GT_U8  devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 counterValue
);

/*******************************************************************************
* prvCpssDxChPortGroupsCounterArraySet
*
* DESCRIPTION:
*       set the counters on all the port groups , from the input register
*       the function set first active port group with the needed values , and set other
*       port groups with value 0 --> so the summary will not count multi-values
*
* APPLICABLE DEVICES:  All DxCh Devices
*                      -- multi-port-group device and non multi-portgroups device
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       startAddr - The register's start address to set the counters values to.
*       numberOfWords - number of words to set (registers)
*       counterValueArr[] - (array of)the counters values to set
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - when  numberOfWords = 0
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupsCounterArraySet
(
    IN GT_U8 devNum,
    IN GT_U32 startAddr,
    IN GT_U32 numberOfWords,
    IN GT_U32 counterValueArr[]
);

/*******************************************************************************
* prvCpssDxChPortGroupsNextPortGroupGet
*
* DESCRIPTION:
*       get the next active port group.
*       NOTE: the function 'wraparound' the port group meaning that when
*           currentPortGroupId == 3 --> the  *nextPortGroupPtr will be 0
*
* APPLICABLE DEVICES:  DxChLion devices
*                      --ONLY for multi-port-group device
*                      (and more then single port group)
*
* INPUTS:
*       devNum             - The PP device number
*       currentPortGroupId - the current port group id
*
* OUTPUTS:
*       nextPortGroupIdPtr  - (pointer to) the next port group id(may 'wraparound')
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - currentPortGroupId >= CPSS_MAX_PORT_GROUPS_CNS
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_NOT_APPLICABLE_DEVICE - non multi port groups device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupsNextPortGroupGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 currentPortGroupId,
    OUT GT_U32 *nextPortGroupIdPtr
);

/*******************************************************************************
* prvCpssDxChPortGroupsPrevPortGroupGet
*
* DESCRIPTION:
*       get the portGroupId of the previous active port group.
*       NOTE: the function 'wraparound' the portGroupIds meaning that when
*             currentPortGroupId == 0 --> the  *prevPortGroupIdPtr will be 3
*
* APPLICABLE DEVICES:  DxChLion devices
*                      --ONLY for multi-port-group device (and more then single port-group)
*
* INPUTS:
*       devNum             - The PP device number
*       currentPortGroupId - the current port group Id
*
* OUTPUTS:
*      prevPortGroupIdPtr - (pointer to) the previous portGroupId (may 'wraparound')
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - currentPortGroupId >= CPSS_MAX_PORT_GROUPS_CNS
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_NOT_APPLICABLE_DEVICE - non multi port groups device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupsPrevPortGroupGet
(
    IN GT_U8   devNum,
    IN GT_U32  currentPortGroupId,
    OUT GT_U32 *prevPortGroupIdPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChHwMultiPortGroupsh */
