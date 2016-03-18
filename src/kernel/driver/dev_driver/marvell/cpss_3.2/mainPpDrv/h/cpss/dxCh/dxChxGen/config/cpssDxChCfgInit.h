/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCfgInit.h
*
* DESCRIPTION:
*       CPSS DxCh initialization of PPs and shadow data structures, and
*       declarations of global variables.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/
#ifndef __cpssDxChCfgInith
#define __cpssDxChCfgInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssHwInit/cpssHwInit.h>

/*
 * typedef: enum CPSS_DXCH_CFG_TABLES_ENT
 *
 * Description:
 *      the type of tables that the DXCH devices hold.
 *      NOTE: some tables may shared on same memory space (like router and IPCL)
 *
 *
 * Enumerations:
 *  CPSS_DXCH_CFG_TABLE_VLAN_E - table type represent the VLAN table
 *  CPSS_DXCH_CFG_TABLE_FDB_E  - table type represent the FDB table
 *  CPSS_DXCH_CFG_TABLE_PCL_ACTION_E - table type represent the PCL action table
 *  CPSS_DXCH_CFG_TABLE_PCL_TCAM_E - table type represent the PCL Tcam table
 *  CPSS_DXCH_CFG_TABLE_ROUTER_NEXT_HOP_E - table type represent the router next hop table
 *  CPSS_DXCH_CFG_TABLE_ROUTER_LTT_E - table type represent the router lookup translation table (LTT)
 *  CPSS_DXCH_CFG_TABLE_ROUTER_TCAM_E - table type represent the Router Tcam table
 *  CPSS_DXCH_CFG_TABLE_MLL_PAIR_E - table type represent the MLL pair table
 *                                      (the MLLs reside as pair in each entry)
 *  CPSS_DXCH_CFG_TABLE_POLICER_METERS_E - table type represent the policer metering table
 *  CPSS_DXCH_CFG_TABLE_POLICER_BILLING_COUNTERS_E - table type represent the policer billing counters table
 *  CPSS_DXCH_CFG_TABLE_VIDX_E - table type represent the VIDX (multicast groups) table
 *  CPSS_DXCH_CFG_TABLE_ARP_TUNNEL_START_E - table type represent the ARP/Tunnel Start table
 *  CPSS_DXCH_CFG_TABLE_STG_E - table type represent the STG (spanning tree groups) table
 *  CPSS_DXCH_CFG_TABLE_QOS_PROFILE_E - table type represent the QOS profile table
 *  CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E - table type represent the Mac to me table
 *  CPSS_DXCH_CFG_TABLE_CNC_E - table type represent the centralized counters (CNC) table
 *                              (the number of counters (X per block) , so 8 blocks means 8*X counters).
 *                              Refer to device datasheet to see number of CNC blocks and the number of
 *                              counters per CNC block.
 *  CPSS_DXCH_CFG_TABLE_CNC_BLOCK_E - table type represent CNC block (the number of conters per CNC block).
 *                                    Refer to device datasheet to see number of counters per CNC block.
 *
 *  CPSS_DXCH_CFG_TABLE_TRUNK_E - table type represent trunk table.
 *
 *  CPSS_DXCH_CFG_TABLE_LAST_E - indication of the last table (not represents a table)
 */
typedef enum
{
    CPSS_DXCH_CFG_TABLE_VLAN_E,
    CPSS_DXCH_CFG_TABLE_FDB_E,
    CPSS_DXCH_CFG_TABLE_PCL_ACTION_E,
    CPSS_DXCH_CFG_TABLE_PCL_TCAM_E,
    CPSS_DXCH_CFG_TABLE_ROUTER_NEXT_HOP_E,
    CPSS_DXCH_CFG_TABLE_ROUTER_LTT_E,
    CPSS_DXCH_CFG_TABLE_ROUTER_TCAM_E,
    CPSS_DXCH_CFG_TABLE_MLL_PAIR_E,
    CPSS_DXCH_CFG_TABLE_POLICER_METERS_E,
    CPSS_DXCH_CFG_TABLE_POLICER_BILLING_COUNTERS_E,
    CPSS_DXCH_CFG_TABLE_VIDX_E,
    CPSS_DXCH_CFG_TABLE_ARP_TUNNEL_START_E,
    CPSS_DXCH_CFG_TABLE_STG_E,
    CPSS_DXCH_CFG_TABLE_QOS_PROFILE_E,
    CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E,
    CPSS_DXCH_CFG_TABLE_CNC_E,
    CPSS_DXCH_CFG_TABLE_CNC_BLOCK_E,
    CPSS_DXCH_CFG_TABLE_TRUNK_E,

    CPSS_DXCH_CFG_TABLE_LAST_E /* mast be last */
} CPSS_DXCH_CFG_TABLES_ENT;

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

} CPSS_DXCH_PP_CONFIG_INIT_STC;

/*******************************************************************************
* cpssDxChCfgDevRemove
*
* DESCRIPTION:
*
*       Remove the device from the CPSS.
*       This function we release the DB of CPSS that associated with the PP ,
*       and will "know" that the device is not longer valid.
*       This function will not do any HW actions , and therefore can be called
*       before or after the HW was actually removed.
*
*       After this function is called the devNum is no longer valid for any API
*       until this devNum is used in the initialization sequence for a "new" PP.
*
*       NOTE: 1. Application may want to disable the Traffic to the CPU , and
*             messages (AUQ) , prior to this operation (if the device still exists).
*             2. Application may want to a reset the HW PP , and there for need
*             to call the "hw reset" function (before calling this function)
*             and after calling this function to do the initialization
*             sequence all over again for the device.
*
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum - device number to remove.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevRemove
(
    IN GT_U8   devNum
);

/*******************************************************************************
* cpssDxChCfgDevEnable
*
* DESCRIPTION:
*       This routine sets the device state.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum - device number to set.
*       enable - GT_TRUE device enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChCfgPpLogicalInit
*
* DESCRIPTION:
*       This function Performs PP RAMs divisions to memory pools, according to
*       the supported modules in system.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum      - The device number
*       ppConfigPtr - (pointer to)includes PP specific logical initialization
*                     params.
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_PP_MEM -  If the given configuration can't fit into the given
*                           PP's memory capabilities, or
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  This function should perform all PP initializations, which
*           includes:
*               -   Init the Driver's part (by calling driverPpInit()).
*               -   Ram structs init, and Ram size check.
*               -   Initialization of the interrupt mechanism data structure.
*       2.  When done, the function should leave the PP in disable state, and
*           forbid any packets forwarding, and any interrupts generation.
*       3.  The execution flow for preparing for the initialization of core
*           modules is described below:
*
*
*             +-----------------+
*             | Init the 3 RAMs |   This initialization includes all structs
*             | conf. structs   |   fields but the base address field.
*             |                 |
*             +-----------------+
*                     |
*   (Ram is big       |                 +-------------------------+
*    enough to hold   +--(No)-----------| return GT_OUT_OF_PP_MEM |
*    all tables?)     |                 +-------------------------+
*                     |
*                   (Yes)
*                     |
*                     V
*             +-------------------+
*             | Set the Rams base |
*             | addr. according   |
*             | to the location   |
*             | fields.           |
*             +-------------------+
*                     |
*                     |
*                     V
*             +----------------------------------+
*             | Init the                         |
*             | PRV_CPSS_DXCH_MODULE_CONFIG_STC  |
*             | struct, according                |
*             | to the info in RAM               |
*             | conf. struct.                    |
*             +----------------------------------+
*                     |
*                     |
*                     |
*                     V
*             +---------------------------------+
*             | set the prepared                |
*             | module Cfg struct               |
*             | in the appropriate              |
*             | PRV_CPSS_DXCH_PP_CONFIG_STC     |
*             | struct.                         |
*             +---------------------------------+
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgPpLogicalInit

(
    IN      GT_U8                           devNum,
    IN      CPSS_DXCH_PP_CONFIG_INIT_STC    *ppConfigPtr
);

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
*
* DESCRIPTION:
*       Sets device ID modification for Routed packets.
*       Enables/Disables FORWARD DSA tag modification of the <source device>
*       and <source port> fields of packets routed by the local device.
*       The <source device> is set to the local device ID and the <source port>
*       is set to 61 (the virtual router port).
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       devNum          - Physical device number.
*       portNum         - Physical port number (or CPU port)
*       modifyEnable    - Boolean value of the FORWARD DSA tag modification:
*                             GT_TRUE  -  Device ID Modification is Enabled.
*                             GT_FALSE -  Device ID Modification is Disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     modifyEnable
);

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortModifySet
*
* DESCRIPTION:
*       Enables/Disables Device ID Modification upon packet sending to another
*       stack unit.
*
*       When Connecting DxCh Devices to SOHO in a Value Blade model to enable
*       DxCh grade Capabilities for FE Ports, in a staking system, we must be
*       able to overcome the 32 devices limitation.
*       To do that, SOHO Device Numbers are not unique and packets received
*       from the SOHO by the DxCh and are relayed to other stack units
*       have their Device ID changed to the DxCh Device ID.
*       On Upstream (packet from SOHO to DxCh):
*           The SOHO Sends a packet to the DxCh and the packet is sent back
*           to the SOHO. In this case we don't want to change the Device ID in
*           the DSA Tag so that the SOHO is able to filter the Src Port and is
*           able to send back the packet to its source when doing cross chip
*           flow control.
*       On Downsteam (packet from SOHO to SOHO):
*           The SOHO receives a packet from another SOHO in this case we
*           want to change the Device ID so that the packet is not filtered.
*
*       On top of the above, for forwarding packets between the DxCh devices
*       and for Auto Learning, the Port Number must also be changed.
*       In addition Changing the Device ID is needed only for FORWARD DSA Tag.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       devNum              - Physical device number.
*       modifedDsaSrcDev    - Boolean value of Enables/Disables Device ID
*                             Modification:
*                             GT_TRUE  -  DSA<SrcDev> is modified to the
*                                         DxCh Device ID and DSA<SrcPort>
*                                         is modified to the DxCh Ingress
*                                         port if all of the following are
*                                         conditions are met:
*                                           - packet is received with a
*                                           non-extended DSA Tag FORWARD    and
*                                           - DSA<SrcIsTrunk> = 0           and
*                                           - packet is transmitted with an
*                                           extended DSA Tag FORWARD.
*
*                             GT_FALSE -  DSA<SrcDev> is not modified when the
*                                         packet is sent to another stac unit
*                                         via an uplink.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This field is relevant for GigE Ports Only.
*       This change is in Addition to the SrcDev Change for Routed packets.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortModifySet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     modifedDsaSrcDev
);

/*******************************************************************************
* cpssDxChCfgTableNumEntriesGet
*
*
* DESCRIPTION:
*       the function return the number of entries of each individual table in
*       the HW
*
*       when several type of entries like ARP and tunnelStart resize in the same
*       table (memory) , the function returns the number of entries for the least
*       denominator type of entry --> in this case number of ARP entries.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - physical device number
*       table        - type of the specific table
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgTableNumEntriesGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_CFG_TABLES_ENT    table,
    OUT GT_U32                      *numEntriesPtr
);

/*******************************************************************************
* cpssDxChCfgReNumberDevNum
*
* DESCRIPTION:
*       function allow the caller to modify the DB of the cpss ,
*       so all info that was 'attached' to 'oldDevNum' will be moved to 'newDevNum'.
*
*       NOTE:
*       1. it is the responsibility of application to update the 'devNum' of
*       HW entries from 'oldDevNum' to 'newDevNum' using appropriate API's
*       such as 'Trunk members','Fdb entries','NH entries','PCE entries'...
*       2. it's application responsibility to update the HW device number !
*          see API cpssDxChCfgHwDevNumSet
*       3. no HW operations involved in this API
*
*       NOTE:
*       this function MUST be called under 'Interrupts are locked' and under
*       'Tasks lock'
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       oldDevNum  - old device number
*       newDevNum  - new device number (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - the device oldDevNum not exist
*       GT_OUT_OF_RANGE - the device newDevNum value > 0x1f (0..31)
*       GT_ALREADY_EXIST - the new device number is already used
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgReNumberDevNum(
    IN GT_U8   oldDevNum,
    IN GT_U8   newDevNum
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChCfgInith */

