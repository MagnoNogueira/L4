/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCnc.h
*
* DESCRIPTION:
*       CPSS DxCh Centralized Counters (CNC) API.
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*******************************************************************************/

#ifndef __cpssDxChCnch
#define __cpssDxChCnch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* get public types */
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/cpssTypes.h>

/*
 * typedef: CPSS_DXCH_CNC_CLIENT_ENT
 *
 * Description:
 *      CNC clients.
 * Enumerations:
 *   CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E       - L2/L3 Ingress VLAN
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E    - Ingress PCL0 lookup 0
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E  - Ingress PCL0 lookup 1
 *                                                    (APPLICABLE DEVICES:
 *                                                     DxChXcat and above)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E    - Ingress PCL1 lookup
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E  - Ingress VLAN Pass/Drop
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E   - Egress VLAN Pass/Drop
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E  - Egress Queue Pass/Drop
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E              - Egress PCL
 *   CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E        - ARP Table access
 *   CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E            - Tunnel Start
 *   CPSS_DXCH_CNC_CLIENT_TTI_E                     - TTI
 *   CPSS_DXCH_CNC_CLIENT_LAST_E                    - amount of enum members
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E,
    CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E,
    CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E,
    CPSS_DXCH_CNC_CLIENT_TTI_E,
    CPSS_DXCH_CNC_CLIENT_LAST_E
} CPSS_DXCH_CNC_CLIENT_ENT;

/*
 * typedef: CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT
 *
 * Description:
 *      CNC modes for byte count counters.
 *
 * Enumerations:
 *   CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E - The Byte Count counter counts the
 *                 entire packet byte count for all packet type
 *   CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E - Byte Count counters counts the
 *                 packet L3 fields (the entire packet
 *                 minus the L3 offset) and only
 *                 the passenger part for tunnel-terminated
 *                 packets or tunnel-start packets.
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E,
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E
} CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT;

/*
 * typedef: CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT
 *
 * Description:
 *      CNC modes Egress VLAN Drop counting.
 *
 * Enumerations:
 *      CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E -
 *          Drop-counter counts egress-filtered and tail-dropped traffic
 *      CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E          -
 *          Drop-counter counts egress filtered traffic only
 *      CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E              -
 *          Drop-counter counts tail-drop only
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E,
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E,
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E
} CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT
 *
 * Description:
 *      CNC counting enabled unit
 *
 * Enumerations:
 *      CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E - enable counting for TTI
 *                                                 client.
 *      CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E - enable counting for extended
 *                                                 PCL unit (this unit includes
 *                                                 PCL0_0, PCL0_1, PCL1 & L2/L3
 *                                                 Ingress VLAN clients).
 *
 * Comment:
 *      Other clients are implicitly enabled for counting and binding to
 *      CNC block operation is needed to perform counting.
 */
typedef enum
{
    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E,
    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E
} CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT;

/*
 * typedef: struct CPSS_DXCH_CNC_COUNTER_STC
 *
 * Description:
 *      The counter entry contents.
 *
 * Fields:
 *      byteCount      - byte count 35 bits
 *      packetCount    - packets count - 29 bits
 *
 * Comment:
 */
typedef struct
{
    GT_U64     byteCount;
    GT_U32     packetCount;
} CPSS_DXCH_CNC_COUNTER_STC;

/*******************************************************************************
* cpssDxChCncBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum         - device number
*       blockNum       - CNC block number
*                        valid range:
*                        DxCh3 - 0..7
*                        DxChXCat,DxChLion - 0,1
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device or
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
);

/*******************************************************************************
* cpssDxChCncBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range:
*                         DxCh3 - 0..7
*                         DxChXCat,DxChLion - 0,1
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device or
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
);

/*******************************************************************************
* cpssDxChCncBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range:
*                          DxCh3 - 0..7
*                          DxChXCat,DxChLion - 0,1
*       client           - CNC client
*       indexRangesBmp   - there are 8 ranges of index counters, each
*                         2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device or
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U32                      indexRangesBmp
);

/*******************************************************************************
* cpssDxChCncBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range:
*                          DxCh3 - 0..7
*                          DxChXCat,DxChLion - 0,1
*       client           - CNC client
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) bitmap
*                         there are 8 ranges of index counters, each
*                         2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device or
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U32                    *indexRangesBmpPtr
);

/*******************************************************************************
* cpssDxChCncPortClientEnableSet
*
* DESCRIPTION:
*   The function sets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_U8                     portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   enable
);

/*******************************************************************************
* cpssDxChCncPortClientEnableGet
*
* DESCRIPTION:
*   The function gets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_U8                     portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *enablePtr
);

/*******************************************************************************
* cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
*
* DESCRIPTION:
*   The function enables or disables counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
*
* DESCRIPTION:
*   The function gets status of counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChCncClientByteCountModeSet
*
* DESCRIPTION:
*   The function sets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*       countMode        - count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device or
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncClientByteCountModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    IN  CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countMode
);

/*******************************************************************************
* cpssDxChCncClientByteCountModeGet
*
* DESCRIPTION:
*   The function gets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*
* OUTPUTS:
*       countModePtr     - (pointer to) count mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device or
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncClientByteCountModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    OUT CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT *countModePtr
);

/*******************************************************************************
* cpssDxChCncEgressVlanDropCountModeSet
*
* DESCRIPTION:
*   The function sets Egress VLAN Drop counting mode.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       mode             - Egress VLAN Drop counting mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressVlanDropCountModeSet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT mode
);

/*******************************************************************************
* cpssDxChCncEgressVlanDropCountModeGet
*
* DESCRIPTION:
*   The function gets Egress VLAN Drop counting mode.
*
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       modePtr             - (pointer to) Egress VLAN Drop counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - on reserved value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressVlanDropCountModeGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT *modePtr
);

/*******************************************************************************
* cpssDxChCncCounterClearByReadEnableSet
*
* DESCRIPTION:
*   The function enable/disables clear by read mode of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChCncCounterClearByReadEnableGet
*
* DESCRIPTION:
*   The function gets clear by read mode status of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueSet
*
* DESCRIPTION:
*   The function sets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadValueSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_CNC_COUNTER_STC        *counterPtr
);

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueGet
*
* DESCRIPTION:
*   The function gets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadValueGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_CNC_COUNTER_STC       *counterPtr
);

/*******************************************************************************
* cpssDxChCncCounterWraparoundEnableSet
*
* DESCRIPTION:
*   The function enables/disables wraparound for all CNC counters
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChCncCounterWraparoundEnableGet
*
* DESCRIPTION:
*   The function gets status of wraparound for all CNC counters
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChCncCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*       blockNum     - number of 2K-counters block
*       indexNumPtr  - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr  - (pointer to) actual size of array of indexes
*       indexesArr[] - (pointer to) array of indexes of counters wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundIndexesGet
(
    IN    GT_U8    devNum,
    IN    GT_U32   blockNum,
    INOUT GT_U32   *indexNumPtr,
    OUT   GT_U32   indexesArr[]
);

/*******************************************************************************
* cpssDxChCncCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range:
*                          DxCh3 - 0..7
*                          DxChXCat,DxChLion - 0,1
*       index            - counter index in the block 0..2047
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          blockNum,
    IN  GT_U32                          index,
    IN  CPSS_DXCH_CNC_COUNTER_STC       *counterPtr
);

/*******************************************************************************
* cpssDxChCncCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range:
*                          DxCh3 - 0..7
*                          DxChXCat,DxChLion - 0,1
*       index            - counter index in the block 0..2047
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          blockNum,
    IN  GT_U32                          index,
    OUT CPSS_DXCH_CNC_COUNTER_STC       *counterPtr
);

/*******************************************************************************
* cpssDxChCncBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   cpssDxChCncBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range:
*                          DxCh3 - 0..7
*                          DxChXCat,DxChLion - 0,1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum
);

/*******************************************************************************
* cpssDxChCncBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                     *inProcessBlocksBmpPtr
);

/*******************************************************************************
* cpssDxChCncUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncUploadedBlockGet one more time to get rest of the block.
*
*  APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          CNC counter value
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                          retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncUploadedBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfCounterValuesPtr,
    OUT    CPSS_DXCH_CNC_COUNTER_STC   *counterValuesPtr
);

/*******************************************************************************
* cpssDxChCncCountingEnableSet
*
* DESCRIPTION:
*   The function enables counting on selected cnc unit.
*
* APPLICABLE DEVICES:  DxChXcat and above.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*       enable  - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCountingEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    IN  GT_BOOL                                 enable
);

/*******************************************************************************
* cpssDxChCncCountingEnableGet
*
* DESCRIPTION:
*   The function gets enable counting status on selected cnc unit.
*
* APPLICABLE DEVICES:  DxChXcat and above.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable
*                     GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCountingEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    OUT GT_BOOL                                 *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChCnch */


