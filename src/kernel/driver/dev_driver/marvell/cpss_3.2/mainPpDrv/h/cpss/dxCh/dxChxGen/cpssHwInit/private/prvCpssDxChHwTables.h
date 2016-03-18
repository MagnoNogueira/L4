/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwTables.h
*
* DESCRIPTION:
*       Private API definition for tables access of the Cheetah and Cheetah 2.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*
*******************************************************************************/

#ifndef __prvCpssDxChHwTablesh
#define __prvCpssDxChHwTablesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwMultiPortGroups.h>

/*
 * Typedef: enum PRV_CPSS_DXCH_ACCESS_ACTION_ENT
 *
 * Description:
 *      access action for non-direct memory of cheetah
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_ACCESS_ACTION_READ_E - read action
 *      PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E - write action
 *
 */
typedef enum
{
    PRV_CPSS_DXCH_ACCESS_ACTION_READ_E = 0,
    PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E
} PRV_CPSS_DXCH_ACCESS_ACTION_ENT;


/* common used types */
/*
 * Typedef: enum PRV_CPSS_DXCH_TABLE_ACCESS_TYPE_ENT
 *
 * Description:
 *      type of access to cheetah's table
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_DIRECT_ACCESS_E   - direct access
 *      PRV_CPSS_DXCH_INDIRECT_ACCESS_E - indirect access
 *
 */

typedef enum
{
    PRV_CPSS_DXCH_DIRECT_ACCESS_E,
    PRV_CPSS_DXCH_INDIRECT_ACCESS_E
} PRV_CPSS_DXCH_TABLE_ACCESS_TYPE_ENT;


/* common used types
 *
 * Typedef: enum PRV_CPSS_DXCH_TABLE_ENT
 *
 * Description:
 *      names of the tables that are non-direct accessed
 *
 *     !!!! Values of enum linked with tables DB. Tables DB must be updated
 *          in any cases of changes in the enum !!!!!!
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E  - VLAN_PORT_PROTOCOL table
 *  PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E - PORT_VLAN_QOS table
 *  PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E - TABLE_TRUNK_MEMBERS.
 *  PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E - STATISTICAL_RATE_LIMIT table.
 *  PRV_CPSS_DXCH_TABLE_CPU_CODE_E -  CPU_CODE table.
 *  PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E - PCL_CONFIG table
 *  PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E - QOS_PROFILE table.
 *  PRV_CPSS_DXCH_TABLE_REMARKING_E - REMARKING table.
 *  PRV_CPSS_DXCH_TABLE_STG_E - STG table.
 *  PRV_CPSS_DXCH_TABLE_VLAN_E -  VLAN table.
 *  PRV_CPSS_DXCH_TABLE_MULTICAST_E - MULTICAST table
 *  PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E - ROUTE_HA_MAC_SA table.
 *  PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E - ROUTE_HA_ARP_DA table.
 *  PRV_CPSS_DXCH_TABLE_FDB_E - FDB table.
 *  PRV_CPSS_DXCH_TABLE_POLICER_E - POLICER table
 *  PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E - POLICER_COUNTERS table.
 *  PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E -  EGRESS_PCL_CONFIG table.
 *  PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E -  TUNNEL_START table.
 *  PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E - QOS_PROFILE_TO_ROUTE table.
 *  PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E - ROUTE_ACCESS_MATRIX table.
 *  PRV_CPSS_DXCH2_LTT_TT_ACTION_E - LTT_TT_ACTION table.
 *  PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E - UC_MC_ROUTE_NEXT_HOP table.
 *  PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E - ROUTE_NEXT_HOP_AGE table.
 *  PRV_CPSS_DXCH3_TABLE_MAC2ME_E - MAC2ME table.
 *  PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E - INGRESS_VLAN_TRANSLATION table.
 *  PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E - EGRESS_VLAN_TRANSLATION table.
 *  PRV_CPSS_DXCH3_TABLE_VRF_ID_E - VRF_ID table.
 *  PRV_CPSS_DXCH3_LTT_TT_ACTION_E - LTT_TT_ACTION table.
 *  PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E - INGRESS_PCL_LOOKUP1 table.
 *  PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E - INGRESS_PCL_UDB table.
 *  PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E -LOGICAL_TARGET_MAPPING table.
 *  PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E -BCN_PROFILES table.
 *  PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E - Egress Policer Remarking table.
 *  PRV_CPSS_DXCH_XCAT_TABLE_LAST_E - last for XCAT supported tables.
 *  PRV_CPSS_DXCH_TABLE_LAST_E - PRV_CPSS_DXCH_XCAT_TABLE_LAST_E - must be maximal - now as XCAT_LAST
 *
 ******************************************************************************
 *  Table Name                  index                          entry
 ******************************************************************************
 *  VLAN_PORT_PROTOCOL        port num                  8 words, word# is protocol,
 *                                                      1 word of data per protocol
 * PORT_VLAN_QOS             port num                        2 words of data
 *  TABLE_TRUNK_MEMBERS     (trunk_id << 3) + member#         1 word   of data
 *  STATISTICAL_RATE_LIMIT  limiter index, 0..32              1 word   of data
 *  CPU_CODE                  cpu code, 0..255                1 word  of data
 *  PCL_CONFIG                table index, 0..1152 Ch         1 word of data
 *                            0 .. 4224 Ch2                   1 word of data
 *  QOS_PROFILE &            profile index, 0..72 Ch          1 word of data
 *  REMARKING                0..128 Ch2                       1 word of data
 *  STG                     STP Group Index, 0..255           2 words of data
 *  VLAN                    vid, 0..4095                      3 words of data, Ch
 *                                                            4 words of data, Ch2
 *  MULTICAST               vidx, 0..4095                     1 words of data
 *  ROUTE_HA_MAC_SA         port or vid, 0..4095              1 word of data
 *  ROUTE_HA_ARP_DA        Ch, MAC DA index, 0...1023         2 words of data
 *                         Ch2, MAC DA/Tunnel row, 0..1023,   6 words of data,4 MAC DA per row.
 *  FDB                    FDB entry #, 0..(16K -1)           4 words of DATA
 *  POLICER                policer index, 0.. 255             2 words of DATA
 *  POLICER_COUNTERS       counter index, 0.. 15              2 words of DATA
 *
 *  Cheetah 2 only tables
 *
 *  EGRESS_PCL_CONFIG      table index, 0..4159               1 word of data
 *  TUNNEL_START           table index, 0..1023               6 words of data
 *  QOS_PROFILE_TO_ROUTE   table index, 0..15                 1 words of data
 *  ROUTE_ACCESS_MATRIX    table index, 0..7                  1 words of data
 *  LTT_TT_ACTION          table index, 0..1023               4 words of data
 *   UC_MC_ROUTE_NEXT_HOP   table index, 0..4095               4 words of data
 *   ROUTE_NEXT_HOP_AGE     table index, 0..125                1 words of data
 *
 *   Cheetah 3 only tables
 *
 *   MAC2ME                 table index, 0..8                  4 words of data
 *   LTT_TT_ACTION          table index, 0..5119               4 words of data
 *
 *   xCat and above
 *   LTT_TT_ACTION          table index, 0..(13K/4 - 1)        4 words of data
 *   VLAN                   vid, 0..4095                       6 words of data, Xcat
 *
 *   XCat and above tables:
 *   INGRESS_PCL_LOOKUP1    table index, 0..4223    1 word of data, 8 bytes aligned
 *  INGRESS_PCL_UDB        table index, 0..7       8 word of data, 32 bytes aligned
 *  LOGICAL_TARGET_MAPPING table index, 0.. 511    1 word of data, 4 bytes alligned
 *  EGRESS_POLICER_REMARKING table index 0..51     2 words of data, 8 bytes alligned
 *
 *  NOTE:
 *
*/

typedef enum
{
    PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E = 0,
    PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
    PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
    PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
    PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,
    PRV_CPSS_DXCH_TABLE_REMARKING_E,
    PRV_CPSS_DXCH_TABLE_STG_E,
    PRV_CPSS_DXCH_TABLE_VLAN_E,
    PRV_CPSS_DXCH_TABLE_MULTICAST_E,
    PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
    PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
    PRV_CPSS_DXCH_TABLE_FDB_E,
    PRV_CPSS_DXCH_TABLE_POLICER_E,
    PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
    PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E,
    PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
    PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E,
    PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E,
    PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
    PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
    PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E,
    PRV_CPSS_DXCH3_TABLE_MAC2ME_E,
    PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E,
    PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E,
    PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
    PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
    PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E,
    PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E,
    PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E,
    PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
    PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,
    /* must be last for XCAT supported tables */
    PRV_CPSS_DXCH_XCAT_TABLE_LAST_E,

    /* must be maximal - now as XCAT_LAST */
    PRV_CPSS_DXCH_TABLE_LAST_E = PRV_CPSS_DXCH_XCAT_TABLE_LAST_E

} PRV_CPSS_DXCH_TABLE_ENT;

/*
 * Typedef: struct PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC
 *
 * Description: A structure to hold Cheetah's indirect access table entry info.
 *
 * Fields:
 *      controlReg   - address of the control register
 *      dataReg      - address of the data register of the entry
 *      trigBit      - the bit num that trig the action in the control register
 *      indexBit     - bit where to write the index (of entry)
 *      specificTableValue - some tables share the same control registers ,
 *                           this value is the specific table
 *      specificTableBit   - start bit for the specificTableValue
 *      actionBit          - the bit number where to specify the action (read/write)
 */


typedef struct
{
    GT_U32  controlReg;
    GT_U32  dataReg;
    GT_U32  trigBit;
    GT_U32  indexBit;
    GT_U32  specificTableValue;
    GT_U32  specificTableBit;
    GT_U32  actionBit;
} PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC
 *
 * Description: A structure to hold Cheetah's direct access table entry info.
 *
 * Fields:
 *      baseAddress    - base address
 *      step           - step in entry promotion
 *      nextWordOffset - next word offest value in bytes.
 */

typedef struct
{
    GT_U32  baseAddress;
    GT_U32  step;
    GT_U32  nextWordOffset;
} PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC;


/*
 * Typedef: struct PRV_CPSS_DXCH_TABLES_INFO_STC
 *
 * Description: A structure to hold Cheetah's direct access table entry info.
 *
 * Fields:
 *      maxNumOfEntries  - number of entries
 *      entrySize        - size in words
 *      readAccessType   - direct/indirect access for read action
 *      readTablePtr     - address of table for read action
 *      writeAccessType  - direct/indirect access for write action
 *      writeTablePtr    - address of table for write action
 */

typedef struct
{
    GT_U32                              maxNumOfEntries;
    GT_U32                              entrySize;
    PRV_CPSS_DXCH_TABLE_ACCESS_TYPE_ENT readAccessType;
    GT_VOID                            *readTablePtr;
    PRV_CPSS_DXCH_TABLE_ACCESS_TYPE_ENT writeAccessType;
    GT_VOID                            *writeTablePtr;

} PRV_CPSS_DXCH_TABLES_INFO_STC;


/*******************************************************************************
* prvCpssDxChReadTableEntry
*
* DESCRIPTION:
*       Read a whole entry from the table.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*
* OUTPUTS:
*       entryValuePtr - (pointer to) the data read from the table
*                       may be NULL in the case of indirect table.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send read entry command to device. And entry is stored in the
*      data registers of a indirect table
*******************************************************************************/
GT_STATUS prvCpssDxChReadTableEntry
(
    IN GT_U8                    devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    OUT GT_U32                 *entryValuePtr
);

/*******************************************************************************
* prvCpssDxChWriteTableEntry
*
* DESCRIPTION:
*       Write a whole entry to the table.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       entryValuePtr - (pointer to) the data that will be written to the table
*                       may be NULL in the case of indirect table.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send write entry command to device. And entry is taken from the
*      data registers of a indirect table.
*******************************************************************************/
GT_STATUS prvCpssDxChWriteTableEntry
(
    IN GT_U8                    devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    IN GT_U32                  *entryValuePtr
);

/*******************************************************************************
* prvCpssDxChReadTableEntryField
*
* DESCRIPTION:
*       Read a field from the table.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*       fieldOffset  - field offset
*       fieldLength - field length
*
* OUTPUTS:
*       fieldValuePtr - (pointer to) the data read from the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChReadTableEntryField
(
    IN GT_U8                    devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    IN GT_U32                   fieldWordNum,
    IN GT_U32                   fieldOffset,
    IN GT_U32                   fieldLength,
    OUT GT_U32                 *fieldValuePtr
);

/*******************************************************************************
* prvCpssDxChWriteTableEntryField
*
* DESCRIPTION:
*       Write a field to the table.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*       fieldOffset  - field offset
*       fieldLength - field length
*       fieldValue - the data write to the table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChWriteTableEntryField
(
    IN GT_U8                    devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    IN GT_U32                   fieldWordNum,
    IN GT_U32                   fieldOffset,
    IN GT_U32                   fieldLength,
    IN GT_U32                   fieldValue
);


/*******************************************************************************
* prvCpssDxChTableNumEntriesGet
*
* DESCRIPTION:
*       get the number of entries in a table
*       needed for debug purpose
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries in the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTableNumEntriesGet
(
    IN GT_U8                    devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    OUT GT_U32                  *numEntriesPtr
);


/*******************************************************************************
* prvCpssDxChTablesAccessInit
*
* DESCRIPTION:
*       Initializes all structures for table access.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  prvCpssDxChTablesAccessInit
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntryFieldList
*
* DESCRIPTION:
*       Write a list of fields to the table.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum            - device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType         - HW table Id
*       entryIndex        - entry Index
*       entryMemoBufArr   - the work memory for read, update and write the entry
*       fieldsAmount      - amount of updated fields in the entry
*       fieldOffsetArr    - (array) the offset of the field in bits
*                           from the entry origin
*       fieldLengthArr    - (array) the length of the field in bits,
*                           0 - means to skip this subfield
*       fieldValueArr     - (array) the value of the field
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_TIMEOUT         - after max number of retries checking if PP ready
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupWriteTableEntryFieldList
(
    IN GT_U8                                  devNum,
    IN GT_U32                                 portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT                tableType,
    IN GT_U32                                 entryIndex,
    IN GT_U32                                 entryMemoBufArr[],
    IN GT_U32                                 fieldsAmount,
    IN GT_U32                                 fieldOffsetArr[],
    IN GT_U32                                 fieldLengthArr[],
    IN GT_U32                                 fieldValueArr[]
);

/*******************************************************************************
* prvCpssDxChReadTableEntryFieldList
*
* DESCRIPTION:
*       Read a list of fields from the table.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum            - device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableId           - HW table Id
*       entryIndex        - entry Index
*       entryMemoBufArr   - the work memory for read, update and write the entry
*       fieldsAmount      - amount of updated fields in the entry
*       fieldOffsetArr    - (array) the offset of the field in bits
*                           from the entry origin
*       fieldLengthArr    - (array) the length of the field in bits,
*                           0 - means to skip this subfield

* OUTPUTS:
*       fieldValueArr     - (array) the value of the field
*
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_TIMEOUT         - after max number of retries checking if PP ready
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupReadTableEntryFieldList
(
    IN  GT_U8                                  devNum,
    IN GT_U32                                 portGroupId,
    IN  PRV_CPSS_DXCH_TABLE_ENT                tableId,
    IN  GT_U32                                 entryIndex,
    IN  GT_U32                                 entryMemoBufArr[],
    IN  GT_U32                                 fieldsAmount,
    IN  GT_U32                                 fieldOffsetArr[],
    IN  GT_U32                                 fieldLengthArr[],
    OUT GT_U32                                 fieldValueArr[]
);


/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntry
*
* DESCRIPTION:
*       Write a whole entry to the table. - for specific portGroupId
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*       entryValuePtr - (pointer to) the data that will be written to the table
*                       may be NULL in the case of indirect table.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send write entry command to device. And entry is taken from the
*      data registers of a indirect table.
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupWriteTableEntry
(
    IN GT_U8                    devNum,
    IN GT_U32                   portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    IN GT_U32                  *entryValuePtr
);

/*******************************************************************************
* prvCpssDxChPortGroupReadTableEntry
*
* DESCRIPTION:
*       Read a whole entry from the table. - for specific portGroupId
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*
* OUTPUTS:
*       entryValuePtr - (pointer to) the data read from the table
*                       may be NULL in the case of indirect table.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send read entry command to device. And entry is stored in the
*      data registers of a indirect table
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupReadTableEntry
(
    IN GT_U8                   devNum,
    IN GT_U32                  portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    OUT GT_U32                 *entryValuePtr
);

/*******************************************************************************
* prvCpssDxChPortGroupReadTableEntryField
*
* DESCRIPTION:
*       Read a field from the table. - for specific portGroupId
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*       fieldOffset  - field offset
*       fieldLength - field length
*
* OUTPUTS:
*       fieldValuePtr - (pointer to) the data read from the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupReadTableEntryField
(
    IN GT_U8                  devNum,
    IN GT_U32                  portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                 entryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    OUT GT_U32                *fieldValuePtr
);

/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntryField
*
* DESCRIPTION:
*       Write a field to the table.  - for specific portGroupId
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*       fieldOffset  - field offset
*       fieldLength - field length
*       fieldValue - the data write to the table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat A1 and above devices the data is updated only when the last
*       word in the entry was written.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupWriteTableEntryField
(
    IN GT_U8                  devNum,
    IN GT_U32                 portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                 entryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    IN GT_U32                 fieldValue
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChHwTablesh */
