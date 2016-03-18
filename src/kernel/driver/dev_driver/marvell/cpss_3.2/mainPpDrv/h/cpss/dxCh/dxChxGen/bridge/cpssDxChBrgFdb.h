/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgFdb.h
*
* DESCRIPTION:
*       FDB tables facility CPSS DxCh implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*
*******************************************************************************/
#ifndef __cpssDxChBrgFdbh
#define __cpssDxChBrgFdbh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>

/*
 * typedef: enum CPSS_DXCH_FDB_QUEUE_TYPE_ENT
 *
 * Description: Enumeration of FDB message queues (AU/FU)
 *
 * Enumerations:
 *      CPSS_DXCH_FDB_QUEUE_TYPE_AU_E - type for AUQ (address update queue)
 *      CPSS_DXCH_FDB_QUEUE_TYPE_FU_E - type for FUQ (FDB upload queue)
 */
typedef enum
{
    CPSS_DXCH_FDB_QUEUE_TYPE_AU_E,
    CPSS_DXCH_FDB_QUEUE_TYPE_FU_E
} CPSS_DXCH_FDB_QUEUE_TYPE_ENT;

/*******************************************************************************
* cpssDxChBrgFdbNaToCpuPerPortSet
*
* DESCRIPTION:
*       Enable/disable forwarding a new mac address message to CPU --per port
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       port    - port number, CPU port
*       enable  - If GT_TRUE, forward NA message to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaToCpuPerPortSet
(
    IN GT_U8   devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbPortLearnStatusGet
*
* DESCRIPTION:
*       Get state of new source MAC addresses learning on packets received
*       on specified port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - device number
*       port   - port number
*
* OUTPUTS:
*       statusPtr - (pointer to) GT_TRUE for enable  or GT_FALSE otherwise
*       cmdPtr    - (pointer to) how to forward packets with unknown/changed SA,
*                   when (*statusPtr) is GT_FALSE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortLearnStatusGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_BOOL *statusPtr,
    OUT CPSS_PORT_LOCK_CMD_ENT *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgFdbPortLearnStatusSet
*
* DESCRIPTION:
*       Enable/disable learning of new source MAC addresses for packets received
*       on specified port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - device number
*       port   - port number
*       status - GT_TRUE for enable  or GT_FALSE otherwise
*       cmd    - how to forward packets with unknown/changed SA,
*                 if status is GT_FALSE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on unknown command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortLearnStatusSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    port,
    IN GT_BOOL                  status,
    IN CPSS_PORT_LOCK_CMD_ENT   cmd
);

/*******************************************************************************
* cpssDxChBrgFdbNaToCpuPerPortGet
*
* DESCRIPTION:
*       Get Enable/disable forwarding a new mac address message to CPU --
*       per port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       port    - port number, CPU port
*
* OUTPUTS:
*       enablePtr  - If GT_TRUE, NA message is forwarded to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaToCpuPerPortGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbNaStormPreventSet
*
* DESCRIPTION:
*   Enable/Disable New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:  All DxCh Devices
* INPUTS:
*       devNum     - device number
*       port       - port number
*       enable     - GT_TRUE - enable NA Storm Prevention (SP),
*                    GT_FALSE - disable NA Storm Prevention
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/portNum/auMsgType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaStormPreventSet
(
    IN GT_U8                       devNum,
    IN GT_U8                       port,
    IN GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChBrgFdbNaStormPreventGet
*
* DESCRIPTION:
*   Get status of New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:  All DxCh Devices
* INPUTS:
*       devNum     - device number
*       port       - port number
*
* OUTPUTS:
*       enablePtr     - pointer to the status of repeated NA CPU messages
*                       GT_TRUE - NA Storm Prevention (SP) is enabled,
*                       GT_FALSE - NA Storm Prevention is disabled
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/portNum/auMsgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaStormPreventGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
);


/*******************************************************************************
* cpssDxChBrgFdbDeviceTableSet
*
* DESCRIPTION:
*       This function sets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*       So for proper work of PP the application must set the relevant bits of
*       all devices in the system prior to inserting FDB entries associated with
*       them
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*       devTableBmp - bmp of devices to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDeviceTableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  devTableBmp
);

/*******************************************************************************
* cpssDxChBrgFdbDeviceTableGet
*
* DESCRIPTION:
*       This function gets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       devTableBmpPtr - pointer to bmp of devices to set.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDeviceTableGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *devTableBmpPtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*       macEntryPtr     - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend and cpssDxChBrgFdbMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/

GT_STATUS cpssDxChBrgFdbMacEntrySet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
);

/*******************************************************************************
* cpssDxChBrgFdbQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware MAC address
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to mac entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/vlan
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend and cpssDxChBrgFdbMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the QA message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQaSend
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
);


/*******************************************************************************
* cpssDxChBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table through Address Update
*       message.(AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend and cpssDxChBrgFdbMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryDelete
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr

);

/*******************************************************************************
* cpssDxChBrgFdbMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
);
/*******************************************************************************
* cpssDxChBrgFdbMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedDevNumPtr = (pointer to) is device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Address entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_U8                   *associatedDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_U32        index
);

/*******************************************************************************
* cpssDxChBrgFdbMaxLookupLenSet
*
* DESCRIPTION:
*       Set the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*       lookupLen   - The maximal length of MAC table lookup, this must be
*                     value divided by 4 with no left over.
*                     in range 4..32. (4,8,12..32)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_OUT_OF_RANGE          - lookupLen > 32 or lookupLen < 4
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Maximal Hash chain length in HW calculated as follows:
*       Maximal Hash chain length = (regVal + 1) * 4
*
*******************************************************************************/
 GT_STATUS cpssDxChBrgFdbMaxLookupLenSet
(
    IN GT_U8    devNum,
    IN GT_U32   lookupLen
);

/*******************************************************************************
* cpssDxChBrgFdbMaxLookupLenGet
*
* DESCRIPTION:
*       Get the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       lookupLenPtr   - The maximal length of MAC table lookup, this must be
*                        value divided by 4 with no left over.
*                        in range 4..32. (4,8,12..32)
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Maximal Hash chain length in HW calculated as follows:
*       Maximal Hash chain length = (regVal + 1) * 4
*
*******************************************************************************/
 GT_STATUS cpssDxChBrgFdbMaxLookupLenGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *lookupLenPtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacVlanLookupModeSet
*
* DESCRIPTION:
*       Sets the VLAN Lookup mode.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacVlanLookupModeSet
(
    IN GT_U8            devNum,
    IN CPSS_MAC_VL_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgFdbMacVlanLookupModeGet
*
* DESCRIPTION:
*       Get VLAN Lookup mode.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacVlanLookupModeGet
(
    IN  GT_U8            devNum,
    OUT CPSS_MAC_VL_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChBrgFdbAuMsgRateLimitSet
*
* DESCRIPTION:
*     Set Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - device number
*       msgRate    - maximal rate of AU messages per second
*       enable     - enable/disable message limit
*                    GT_TRUE - message rate is limitted with msgRate
*                    GT_FALSE - there is no limit, WA to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_OUT_OF_RANGE          - out of range.
*       GT_BAD_PARAM             - on bad parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when DxCh Clock 200 MHz(DxCh2 220 MHz Clock) ,
*          granularity and rate changed lineary depending on clock.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgRateLimitSet
(
    IN GT_U8                       devNum,
    IN GT_U32                      msgRate,
    IN GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChBrgFdbAuMsgRateLimitGet
*
* DESCRIPTION:
*     Get Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       msgRatePtr - (pointer to) maximal rate of AU messages per second
*       enablePtr  - (pointer to) GT_TRUE - message rate is limited with
*                                           msgRatePtr
*                                 GT_FALSE - there is no limit, WA to CPU
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_OUT_OF_RANGE          - out of range.
*       GT_BAD_PARAM             - on bad parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when DxCh Clock 200 MHz(DxCh2 220 MHz Clock) ,
*          granularity and rate changed lineary depending on clock.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgRateLimitGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *msgRatePtr,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbNaMsgOnChainTooLongSet
*
* DESCRIPTION:
*       Enable/Disable sending NA messages to the CPU indicating that the device
*       cannot learn a new SA . It has reached its max hop (bucket is full).
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgOnChainTooLongSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbNaMsgOnChainTooLongGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending NA messages to the CPU
*       indicating that the device cannot learn a new SA. It has reached its
*       max hop (bucket is full).
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the NA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgOnChainTooLongGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbSpAaMsgToCpuSet
*
* DESCRIPTION:
*       Enabled/Disabled sending Aged Address (AA) messages to the CPU for the
*       Storm Prevention (SP) entries when those entries reach an aged out status.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the SP AA message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSpAaMsgToCpuSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbSpAaMsgToCpuGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending Aged Address (AA) messages
*       to the CPU for the Storm Prevention (SP) entries when those entries reach
*       an aged out status.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the SP AA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSpAaMsgToCpuGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbAAandTAToCpuSet
*
* DESCRIPTION:
*       Enable/Disable the PP to/from sending an AA and TA address
*       update messages to the CPU.
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable the message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or auMsgType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAAandTAToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
);

/*******************************************************************************
* cpssDxChBrgFdbAAandTAToCpuGet
*
* DESCRIPTION:
*       Get state of sending an AA and TA address update messages to the CPU
*       as configured to PP.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - the messages to CPU enabled
*                                  GT_FALSE - the messages to CPU disabled
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or auMsgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAAandTAToCpuGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbHashModeSet
*
* DESCRIPTION:
*       Sets the FDB hash function mode.
*       The CRC based hash function provides the best hash index destribution
*       for random addresses and vlans.
*       The XOR based hash function provides optimal hash index destribution
*       for controlled testing scenarios, where sequential addresses and vlans
*       are often used.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - hash function based mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_HASH_FUNC_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgFdbHashModeGet
*
* DESCRIPTION:
*       Gets the FDB hash function mode.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - hash function based mode:
*                 CPSS_FDB_HASH_FUNC_XOR_E - XOR based hash function mode
*                 CPSS_FDB_HASH_FUNC_CRC_E - CRC based hash function mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashModeGet
(
    IN  GT_U8                         devNum,
    OUT CPSS_MAC_HASH_FUNC_MODE_ENT   *modePtr
);

/******************************************************************************
* cpssDxChBrgFdbAgingTimeoutSet
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE :
*       For DxCh3 the device support aging time with granularity of 16 seconds
*       (16,32..1008) . So value of 226 will be round down to 224 , and value of
*       255 will be round up to 256.
*       For others the device support aging time with
*       granularity of 10 seconds (10,20..630) .
*       So value of 234 will be round down to 230 ,
*       and value of 255 will be round up to 260.
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       timeout - aging time in seconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   For the 270MHz range from 16..1008 (but due to rounding we support values 8..636)
*   HW supports steps 16
*   For the 220MHz range from 9..573 (but due to rounding we support values 5..577)
*   HW supports steps 9 (9.09)
*   For the 200MHz range from 10..630 (but due to rounding we support values 5..634)
*   HW supports steps 10
*   For the 144 MHz range from 14..875 (but due to rounding we support values 7..882)
*   HW supports steps 13.88
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgingTimeoutSet
(
    IN GT_U8  devNum,
    IN GT_U32 timeout
);

/******************************************************************************
* cpssDxChBrgFdbAgingTimeoutGet
*
* DESCRIPTION:
*       Gets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE :
*       For DxCh3 the device support aging time with granularity of 16 seconds
*       (16,32..1008).
*       For others the device support aging time with
*       granularity of 10 seconds (10,20..630) .
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       timeoutPtr - pointer to aging time in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For core clock of 270MHz The value ranges from 16 seconds to 1008
*       seconds in steps of 16 seconds.
*       For core clock of 220MHz The value ranges from 9 seconds to 573
*       seconds in steps of 9 seconds.
*       For core clock of 200 MHZ The value ranges from 10 seconds to 630
*       seconds in steps of 10 seconds.
*       For core clock of 144MHZ the aging timeout ranges from 14 (13.8) to 875
*       seconds in steps of 14 seconds.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgingTimeoutGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *timeoutPtr
);

/*******************************************************************************
* cpssDxChBrgFdbSecureAgingSet
*
* DESCRIPTION:
*      Enable or disable secure aging. This mode is relevant to automatic or
*      triggered aging with removal of aged out FDB entries. This mode is
*      applicable together with secure auto learning mode see
*      cpssDxChBrgFdbSecureAutoLearnSet. In the secure aging mode the device
*      sets aged out unicast FDB entry with <Multiple> =  1 and VIDX = 0xfff but
*      not invalidates entry. This causes packets destined to this FDB entry MAC
*      address to be flooded to the VLAN.  But such packets are known ones.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable secure aging
*                 GT_FALSE - disable secure aging
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAgingSet
(
    IN GT_U8          devNum,
    IN GT_BOOL        enable
);

/*******************************************************************************
* cpssDxChBrgFdbSecureAgingGet
*
* DESCRIPTION:
*      Get the status (enabled/disabled) of secure aging.
*      This mode is relevant to automatic or triggered aging with removal of
*      aged out FDB entries. This mode is applicable together with secure auto
*      learning mode see cpssDxChBrgFdbSecureAutoLearnGet. In the secure aging
*      mode  the device sets aged out unicast FDB entry with <Multiple> =  1 and
*      VIDX = 0xfff but not invalidates entry. This causes packets destined to
*      this FDB entry MAC address to be flooded to the VLAN.
*      But such packets are known ones.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable secure aging
*                    GT_FALSE - disable secure aging
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAgingGet
(
    IN  GT_U8          devNum,
    OUT GT_BOOL        *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbSecureAutoLearnSet
*
* DESCRIPTION:
*        Configure secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum  - device number
*       mode    - Enable/Disable secure automatic learning.
*                 The command assigned to packet with new SA not found in FDB
*                 when secure automatic learning enable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAutoLearnSet
(
    IN GT_U8                                devNum,
    IN CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgFdbSecureAutoLearnGet
*
* DESCRIPTION:
*        Get secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr  - Enabled/Disabled secure automatic learning.
*                  The command assigned to packet with new SA not found in FDB
*                  when secure automatic learning enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAutoLearnGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChBrgFdbStaticTransEnable
*
* DESCRIPTION:
*       This routine determines whether the transplanting operate on static
*       entries.
*       When the PP will do the transplanting , it will/won't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it will/won't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE transplanting is enabled on static entries,
*                GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticTransEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbStaticTransEnableGet
*
* DESCRIPTION:
*       Get whether the transplanting enabled to operate on static entries.
*       when the PP do the transplanting , it do/don't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it do/don't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE transplanting is enabled on static entries,
*                   GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticTransEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbStaticDelEnable
*
* DESCRIPTION:
*       This routine determines whether flush delete operates on static entries.
*       When the PP will do the Flush , it will/won't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE delete is enabled on static entries, GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticDelEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbStaticDelEnableGet
*
* DESCRIPTION:
*       Get whether flush delete operates on static entries.
*       When the PP do the Flush , it do/don't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE delete is enabled on static entries,
*                    GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticDelEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionsEnableSet
*
* DESCRIPTION:
*       Enables/Disables FDB actions.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE -  Actions are enabled
*                 GT_FALSE -  Actions are disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbActionsEnableGet
*
* DESCRIPTION:
*       Get the status of FDB actions.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - actions are enabled
*                    GT_FALSE - actions are disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbTrigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       actFinishedPtr  - GT_TRUE  - triggered action completed
*                         GT_FALSE - triggered action is not completed yet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrigActionStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *actFinishedPtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerModeSet
*
* DESCRIPTION:
*       Sets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ACTION_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerModeGet
*
* DESCRIPTION:
*       Gets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_MAC_ACTION_MODE_ENT     *modePtr
);

/*******************************************************************************
* cpssDxChBrgFdbStaticOfNonExistDevRemove
*
* DESCRIPTION:
*       enable the ability to delete static mac entries when the aging daemon in
*       PP encounter with entry registered on non-exist device in the
*       "device table" the PP removes it ---> this flag regard the static
*       entries
*
* APPLICABLE DEVICES:  All DxCh Devices
* INPUTS:
*       devNum       - device number
*       deleteStatic -  GT_TRUE - Action will delete static entries as well as
*                                 dynamic
*                       GT_FALSE - only dynamic entries will be deleted.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It's application responsibility to check the status of Aging Trigger by
*       busy wait (use cpssDxChBrgFdbTrigActionStatusGet API), in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticOfNonExistDevRemove
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     deleteStatic
);

/*******************************************************************************
* cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
*
* DESCRIPTION:
*       Get whether enabled/disabled the ability to delete static mac entries
*       when the aging daemon in PP encounter with entry registered on non-exist
*       device in the "device table".
*       entries
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       deleteStaticPtr -  GT_TRUE - Action deletes entries if there associated
*                                    device number don't exist (static etries
*                                                                  as well)
*                          GT_FALSE - Action don't delete entries if there
*                                     associated device number don't exist
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *deleteStaticPtr
);

/*******************************************************************************
* cpssDxChBrgFdbDropAuEnableSet
*
* DESCRIPTION:
*       Enable/Disable dropping the Address Update messages when the queue is
*       full .
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - Drop the address update messages
*                 GT_FALSE - Do not drop the address update messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDropAuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbDropAuEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of dropping the Address Update messages.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - The address update messages are dropped
*                    GT_FALSE - The address update messages are not dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDropAuEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with trunk,
*       regardless of the device to wich they are associated.
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum                         - device number
*       ageOutAllDevOnTrunkEnable   - GT_TRUE : All entries associated with a
*                                     trunk, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     trunk on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnTrunkEnable
);

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with trunk, regardless of
*       the device to wich they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                      trunk, regardless of the device they
*                                      are associated with are aged out.
*                                      GT_FALSE: Only entries associated with a
*                                      trunk on the local device are aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnTrunkEnablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with port,
*       regardless of the device to wich they are associated.
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum                       - device number
*       ageOutAllDevOnNonTrunkEnable - GT_TRUE : All entries associated with a
*                                     port, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     port on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnNonTrunkEnable
);

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with port, regardless of
*       the device to wich they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnNonTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                         port, regardless of the device they
*                                         are associated with are aged out.
*                                         GT_FALSE: Only entries associated with a
*                                         port on the local device are aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnNonTrunkEnablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionTransplantDataSet
*
* DESCRIPTION:
*       Prepares the entry for transplanting (old and new interface parameters).
*       VLAN and VLAN mask for transplanting is set by
*       cpssDxChBrgFdbActionActiveVlanSet. Execution of transplanting is done
*       by cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*       oldInterfacePtr - pointer to old Interface parameters
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters
*                         (it may be only port or trunk).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionTransplantDataSet
(
    IN GT_U8                    devNum,
    IN CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    IN CPSS_INTERFACE_INFO_STC  *newInterfacePtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionTransplantDataGet
*
* DESCRIPTION:
*       Get transplant data: old interface parameters and the new ones.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*
*
* OUTPUTS:
*       oldInterfacePtr - pointer to old Interface parameters.
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters.
*                         (it may be only port or trunk).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionTransplantDataGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    OUT CPSS_INTERFACE_INFO_STC  *newInterfacePtr
);

/*******************************************************************************
* cpssDxChBrgFdbFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       completedPtr - pointer to AU message processing completion.
*                      GT_TRUE - AU message was proceesed by PP.
*                      GT_FALSE - AU message proceesing is not completed yet by PP.
*       succeededPtr - pointer to a success of AU operation.
*                      GT_TRUE  - the AU action succeeded.
*                      GT_FALSE - the AU action has failed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DXCH devices the succeed status doesn't relevant for query
*       address messages (message type = CPSS_QA_E) -  FEr#93.
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbFromCpuAuMsgStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *completedPtr,
    OUT GT_BOOL  *succeededPtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveVlanSet
*
* DESCRIPTION:
*       Set action active vlan and vlan mask.
*       All actions will be taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan Id
*       vlanMask    - vlan mask filter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveVlanSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   vlanMask
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveVlanGet
*
* DESCRIPTION:
*       Get action active vlan and vlan mask.
*       All actions are taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       vlanIdPtr      - pointer to Vlan Id
*       vlanMaskPtr    - pointer to vlan mask filter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveVlanGet
(
    IN  GT_U8    devNum,
    OUT GT_U16   *vlanIdPtr,
    OUT GT_U16   *vlanMaskPtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveDevSet
*
* DESCRIPTION:
*       Set the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*       actDev      - Action active device (0..31)
*       actDevMask  - Action active mask (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actDev > 31 or actDevMask > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveDevSet
(
    IN GT_U8    devNum,
    IN GT_U32   actDev,
    IN GT_U32   actDevMask
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveDevGet
*
* DESCRIPTION:
*       Get the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       actDevPtr      - pointer to action active device (0..31)
*       actDevMaskPtr  - pointer to action active mask (0..31)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actDev > 31 or actDevMask > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveDevGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *actDevPtr,
    OUT GT_U32   *actDevMaskPtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveInterfaceSet
*
* DESCRIPTION:
*       Set the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of : Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*       A FDB entry will be treated by the FDB action only if the following
*       cases are true:
*       1. The interface is trunk and "active trunk mask" is set and
*       "associated trunkId" masked by the "active trunk/port mask" equal to
*       "Active trunkId".
*       2. The interface is port and "associated portNumber" masked by the
*       "active trunk/port mask" equal to "Active portNumber".
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum           - device number
*       actIsTrunk       - determines if the interface is port or trunk
*       actIsTrunkMask   - action active trunk mask.
*       actTrunkPort     - action active interface (port/trunk)
*       actTrunkPortMask - action active port/trunk mask.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To enable Trunk-ID filter set actIsTrunk and actIsTrunkMask to 0x1,
*       set actTrunkPort to trunkID and actTrunkPortMask to 0x7F. To disable
*       Trunk-ID filter set all those parameters to 0x0.
*       To enable Port/device filter set actIsTrunk to 0x0, actTrunkPort to
*       portNum, actTrunkPortMask to 0x7F, actDev to device number and
*       actDevMask to 0x1F (all ones) by cpssDxChBrgFdbActionActiveDevSet.
*       To disable port/device filter set all those parameters to 0x0.
*
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveInterfaceSet
(
    IN GT_U8   devNum,
    IN GT_U32  actIsTrunk,
    IN GT_U32  actIsTrunkMask,
    IN GT_U32  actTrunkPort,
    IN GT_U32  actTrunkPortMask
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveInterfaceGet
*
* DESCRIPTION:
*       Get the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of: Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       actIsTrunkPtr       - determines if the interface is port or trunk
*       actIsTrunkMaskPtr   - action active trunk mask.
*       actTrunkPortPtr     - action active interface (port/trunk)
*       actTrunkPortMaskPtr - action active port/trunk mask.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveInterfaceGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *actIsTrunkPtr,
    OUT GT_U32  *actIsTrunkMaskPtr,
    OUT GT_U32  *actTrunkPortPtr,
    OUT GT_U32  *actTrunkPortMaskPtr
);

/*******************************************************************************
* cpssDxChBrgFdbUploadEnableSet
*
* DESCRIPTION:
*       Enable/Disable reading FDB entries via AU messages to the CPU.
*       The API only configures mode of triggered action.
*       To execute upload use the cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE - enable FDB Upload
*                  GT_FALSE - disable FDB upload
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUploadEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChBrgFdbUploadEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of uploading FDB entries via AU messages to the CPU.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - FDB Upload is enabled
*                     GT_FALSE - FDB upload is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUploadEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbTrigActionStart
*
* DESCRIPTION:
*       Enable actions, sets action type, action mode to CPSS_ACT_TRIG_E and
*       starts triggered action by setting Aging Trigger.
*       This API may be used to start one of triggered actions: Aging, Deleting,
*       Transplanting and FDB Upload.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE    - when previous FDB triggered action is not completed yet
*                         or CNC block upload not finished (or not all of it's
*                         results retrieved from the common used FU and CNC
*                         upload queue)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Deleting and Transplanting actions can be done only by CPU triggered
*       FDB action that activated by the function.
*       See below sequence of CPSS API calls for Deleting and
*       Transplanting actions.
*
*       Before making Delete or Transplant sequence the application need to
*       disable Actions to avoid wrong automatic aging.
*
*       It is Application responsibility to get and store all parameters
*       that are changed during those actions.
*       1. AA message to CPU status by cpssDxChBrgFdbAAandTAToCpuGet.
*       2. Action Active configuration by
*          cpssDxChBrgFdbActionActiveInterfaceGet,
*          cpssDxChBrgFdbActionActiveDevGet,
*          cpssDxChBrgFdbActionActiveVlanGet.
*       3. Action mode and trigger mode by cpssDxChBrgFdbActionModeGet
*          cpssDxChBrgFdbActionTriggerModeGet.
*
*       The AA and TA messages may be disabled before the FDB action
*       4. Disable AA and TA messages to CPU by cpssDxChBrgFdbAAandTAToCpuSet.
*
*       5. Set Active configuration by: cpssDxChBrgFdbActionActiveInterfaceSet,
*       cpssDxChBrgFdbActionActiveVlanSet and cpssDxChBrgFdbActionActiveDevSet.
*
*       6. Start triggered action by cpssDxChBrgFdbTrigActionStart
*
*       7. Wait that triggered action is completed by:
*           -  Busy-wait poling of status - cpssDxChBrgFdbTrigActionStatusGet
*           -  Wait of event CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E.
*              This wait may be done in context of dedicated task to restore
*              Active configuration and AA messages configuration.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrigActionStart
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChBrgFdbActionModeSet
*
* DESCRIPTION:
*       Sets FDB action mode without setting Action Trigger
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - FDB action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChBrgFdbActionModeGet
*
* DESCRIPTION:
*       Gets FDB action mode.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr    - FDB action mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_FDB_ACTION_MODE_ENT     *modePtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerToggle
*
* DESCRIPTION:
*       Toggle Aging Trigger and cause the device to scan its MAC address table.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerToggle
(
    IN GT_U8  devNum
);

/*******************************************************************************
* cpssDxChBrgFdbTrunkAgingModeSet
*
* DESCRIPTION:
*       Sets bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*       trunkAgingMode - FDB aging mode for trunk entries.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E:
*                 In Controlled Aging mode, the device ages Trunk entries
*                 whether it is the owner or not. When a Trunk entry is aged,
*                 it is not deleted from the MAC table, but an Aged
*                 Address (AA) message is sent to the CPU. This allows
*                 the CPU to track whether the MAC has been aged on all
*                 devices that have port members for the Trunk. Only then
*                 the CPU explicitly deletes the Trunk entry on all
*                 devices.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E:
*                 This is Automatic Trunk Entry Aging mode, which treats
*                 Trunk entries in the MAC table just like non-Trunk entries.
*                 That is, aging is performed only on entries owned by the
*                 device. The advantage of this mode is that it does not
*                 require any CPU intervention. This mode has the
*                 disadvantage of possibly aging a Trunk entry on all devices
*                 when there still may be active traffic for the MAC on a
*                 different device, where packets were sent over another
*                 link of the Trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or aging mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrunkAgingModeSet
(
    IN GT_U8                         devNum,
    IN CPSS_FDB_AGE_TRUNK_MODE_ENT  trunkAgingMode
);

/*******************************************************************************
* cpssDxChBrgFdbTrunkAgingModeGet
*
* DESCRIPTION:
*       Get bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       trunkAgingModePtr - (pointer to) FDB aging mode for trunk entries.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E:
*                 In Controlled Aging mode, the device ages Trunk entries
*                 whether it is the owner or not. When a Trunk entry is aged,
*                 it is not deleted from the MAC table, but an Aged
*                 Address (AA) message is sent to the CPU. This allows
*                 the CPU to track whether the MAC has been aged on all
*                 devices that have port members for the Trunk. Only then
*                 the CPU explicitly deletes the Trunk entry on all
*                 devices.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E:
*                 This is Automatic Trunk Entry Aging mode, which treats
*                 Trunk entries in the MAC table just like non-Trunk entries.
*                 That is, aging is performed only on entries owned by the
*                 device. The advantage of this mode is that it does not
*                 require any CPU intervention. This mode has the
*                 disadvantage of possibly aging a Trunk entry on all devices
*                 when there still may be active traffic for the MAC on a
*                 different device, where packets were sent over another
*                 link of the Trunk.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or aging mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrunkAgingModeGet
(
    IN GT_U8                        devNum,
    OUT CPSS_FDB_AGE_TRUNK_MODE_ENT *trunkAgingModePtr
);

/*******************************************************************************
* cpssDxChBrgFdbInit
*
* DESCRIPTION:
*       Init FDB system facility for a device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum         - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - on not supported FDB table size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbInit
(
    IN  GT_U8   devNum
);

/*******************************************************************************
* cpssDxChBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller
*
*  APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfAuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr
);

/*******************************************************************************
* cpssDxChBrgFdbFuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller.
*       The PP may place FU messages in common FDB Address Update (AU) messages
*       queue or in separate queue only for FU messages. The function
*       cpssDxChHwPpPhase2Init configures queue for FU messages by the
*       fuqUseSeparate parameter. If common AU queue is used then function
*       returns all AU messages in the queue including FU ones.
*       If separate FU queue is used then function returns only FU messages.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum     - the device number from which FU are taken
*       numOfFuPtr - (pointer to)max number of FU messages to get
*
* OUTPUTS:
*       numOfFuPtr - (pointer to)actual number of FU messages that were received
*       fuMessagesPtr - array that holds received FU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfFuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - not all results of the CNC block upload
*                          retrieved from the common used FU and CNC
*                          upload queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbFuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfFuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *fuMessagesPtr
);

/************************************************************************
* cpssDxChBrgFdbAuqFuqMessagesNumberGet
*
* DESCRIPTION:
*       The function scan the AU/FU queues and returns the number of
*       AU/FU messages in the queue.
*
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - the device number on which AU are counted
*       queueType - AUQ or FUQ. FUQ valid for DxCh2 and above.
*
* OUTPUTS:
*       numOfAuPtr - (pointer to) number of AU messages in the specified queue.
*       endOfQueueReachedPtr - (pointer to) GT_TRUE: The queue reached to the end.
*                                        GT_FALSE: else
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, queueType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to have the accurate number of entries  application should
*       protect Mutual exclusion between HW access to the AUQ/FUQ
*       SW access to the AUQ/FUQ and calling to this API.
*       i.e. application should stop the PP from sending AU messages to CPU.
*       and should not call  the api's
*       cpssDxChBrgFdbFuMsgBlockGet, cpssDxChBrgFdbFuMsgBlockGet
*
***********************************************************************/
GT_STATUS cpssDxChBrgFdbAuqFuqMessagesNumberGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_U32                       *numOfAuPtr,
    OUT GT_BOOL                      *endOfQueueReachedPtr
);

/*******************************************************************************
* cpssDxChBrgFdbAgeBitDaRefreshEnableSet
*
* DESCRIPTION:
*       Enables/disables destination address-based aging. When this bit is
*       set, the aging process is done both on the source and the destination
*       address (i.e. the age bit will be refresh when MAC DA hit occurs, as
*       well as MAC SA hit).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable refreshing
*                 GT_FALSE - disable refreshing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeBitDaRefreshEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChBrgFdbAgeBitDaRefreshEnableGet
*
* DESCRIPTION:
*       Gets status (enabled/disabled) of destination address-based aging bit.
*       When this bit is set, the aging process is done both on the source and
*       the destination address (i.e. the age bit will be refresh when MAC DA
*       hit occurs, as well as MAC SA hit).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) status DA refreshing of aged bit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeBitDaRefreshEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbRoutedLearningEnableSet
*
* DESCRIPTION:
*       Enable or disable learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet’s MAC SA to the router’s MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers’ Source Addresses.
*
*
* APPLICABLE DEVICES:  DxCh3 and above devices
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutedLearningEnableSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
);

/*******************************************************************************
* cpssDxChBrgFdbRoutedLearningEnableGet
*
* DESCRIPTION:
*       Get Enable or Disable state of learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet’s MAC SA to the router’s MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers’ Source Addresses.
*
* APPLICABLE DEVICES:  DxCh3 and above devices
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutedLearningEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);


/*******************************************************************************
* cpssDxChBrgFdbTriggerAuQueueWa
*
* DESCRIPTION:
*       Trigger the Address Update (AU) queue workaround for case when learning
*       from CPU is stopped due to full Address Update (AU) fifo queue
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_STATE             - the secondary AUQ not reached the end of
*                                  queue , meaning no need to set the secondary
*                                  base address again.
*                                  this protect the SW from losing unread messages
*                                  in the secondary queue (otherwise the PP may
*                                  override them with new messages).
*                                  caller may use function
*                                  cpssDxChBrgFdbSecondaryAuMsgBlockGet(...)
*                                  to get messages from the secondary queue.
*       GT_NOT_SUPPORTED         - the device not need / not support the WA
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTriggerAuQueueWa
(
    IN  GT_U8 devNum
);

/*******************************************************************************
* cpssDxChBrgFdbSecondaryAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller --> from the secondary AUQ.
*       The function cpssDxChHwPpPhase2Init configures queue for secondary AU
*       messages. If common FU queue is used then function returns all AU
*       messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecondaryAuMsgBlockGet
(
    IN      GT_U8                                    devNum,
    INOUT   GT_U32                                   *numOfAuPtr,
    OUT     CPSS_MAC_UPDATE_MSG_EXT_STC              *auMessagesPtr
);


/*******************************************************************************
* cpssDxChBrgFdbQueueFullGet
*
* DESCRIPTION:
*       The function returns the state that the AUQ/FUQ - is full/not full
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - the device number
*       queueType - queue type AU/FU queue
*                   NOTE: when using shared queue for AU and FU messages, this
*                         parameter is ignored (and the AUQ is queried)
*                         meaning that this parameter always ignored for DxCh1
*                         devices.
*
*
* OUTPUTS:
*       isFullPtr - (pointer to) is the relevant queue full
*                   GT_TRUE  - the queue is full
*                   GT_FALSE - the queue is not full
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum , queueType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQueueFullGet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_BOOL *isFullPtr
);

/*******************************************************************************
* cpssDxChBrgFdbQueueRewindStatusGet
*
* DESCRIPTION:
*       function check if the specific AUQ was 'rewind' since the last time
*       this function was called for that AUQ
*       this information allow the application to know when to finish processing
*       of the WA relate to cpssDxChBrgFdbTriggerAuQueueWa(...)
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       rewindPtr       - (pointer to) was the AUQ rewind since last call
*                           GT_TRUE - AUQ was rewind since last call
*                           GT_FALSE - AUQ wasn't rewind since last call
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQueueRewindStatusGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *rewindPtr
);


/*******************************************************************************
* cpssDxChBrgFdbMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry. 
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       age            - Age flag that is used for the two-step Aging process.
*                        GT_FALSE - The entry will be aged out in the next pass.
*                        GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_U32       index, 
    IN GT_BOOL      age
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgFdbh */
