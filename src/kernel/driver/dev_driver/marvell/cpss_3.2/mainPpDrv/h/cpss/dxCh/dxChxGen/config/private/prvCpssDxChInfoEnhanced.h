/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChInfoEnhanced.h
*
* DESCRIPTION:
*       This file includes the declaration of the structure to hold the
*       enhanced parameters that "fine tuning" the PP's capabilities in CPSS
*       point of view.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __prvCpssDxChInfoEnhancedh
#define __prvCpssDxChInfoEnhancedh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>

/*
 * Typedef: struct PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC
 *
 * Description: A Structure including PP's tables sizes and parameters that
 *              define the "fine tuning" , of the specific device.
 * Notes:
 *         the initialization of those fields should be done during "phase 1".
 *
 * Fields:
 *      enhancedInfoPtr - (pointer to) the other enhanced features of the PP.
 *
 *      tableSize - structure that hold the size of tables that the PP hold.
 *                  the values refer to number of entries in the tables.
 *          fdb - the number of entries in bridge FDB table.
 *          vidxNum - number of VIDX (multicast groups).
 *          stgNum - number of MSTP groups . STG is spanning tree groups.
 *          routerAndTunnelTermTcam - the number of rows in router and tunnel termination TCAM table.(number of TTI entries / Ipv6 entries)
 *          routerArpAndTunnelStart - the number of rows in router ARP and tunnel start table.(number of TS entries)
 *          routerNextHop - the number of entries in the router next hop table
 *          mllPairs - the number of mll pairs (multicast linked list)
 *          policyTcamRaws -  actual amount of TCAM rows
 *                            The DxCh3 TCAM has 4 banks multiply to policyTcamRaws of 24 Byte entries.
 *                            The DxCh and DxCh2 TCAM has 2 banks multiply to policyTcamRaws of 24 Byte entries
 *          cncBlocks      -  actual amount of CNC blocks.
 *                           The PPs address space represents the 12 blocks
 *                           but not all blocks present
 *          cncBlockNumEntries - number of entries in each CNC block
 *          policersNum - number of policers (shared for ingress and egress)
 *                        Note: For DxChXcat and above, this is the number of ingress policers
 *                        for both ingress stages.
 *          egressPolicersNum - Number of egress policers.
 *                              Note: Relevant for DxChXcat and above.
 *          trunksNum - number of trunks.
 *          transmitDescr  - Transmit Descriptors number. Supported for DxCh3 and above.
 *          bufferMemory   - Buffer Memory size. Supported for DxCh3 and above.
 *          txQueuesNum    -  Transmit Queues Number
 *          tr101Supported - TR101 feature support. Supported for DxChXcat and above.
 *                              GT_TRUE - supported
 *                              GT_FALSE - not supported
 *          vlanTranslationSupported - VLAN translation feature support. Supported for DxCh3 and above.
 *                              GT_TRUE - supported
 *                              GT_FALSE - not supported
 *          iplrSecondStageSupported -
 *                              GT_TRUE - There are two ingress stages.
 *                              GT_FALSE - There is only one ingress stage.
 *                              Note: Relevant for DxChXcat and above.
*/
typedef struct{
    void*       enhancedInfoPtr;

    struct{
        /* bridge section */
        GT_U32  fdb;
        GT_U32  vidxNum;
        GT_U32  stgNum;

        /* tti section */
        GT_U32 routerAndTunnelTermTcam;

        /* tunnel section*/
        GT_U32  routerArpAndTunnelStart;

        /* ip section */
        GT_U32  routerNextHop;
        GT_U32  mllPairs;

        /* PCL section */
        GT_U32 policyTcamRaws;

        /* CNC section */
        GT_U32 cncBlocks;
        GT_U32 cncBlockNumEntries;

        /* Policer Section */
        GT_U32  policersNum;
        GT_U32  egressPolicersNum;

        /* trunk Section */
        GT_U32  trunksNum;

        /* Transmit Descriptors */
        GT_U32 transmitDescr;

        /* Buffer Memory */
        GT_U32 bufferMemory;

        /*  Transmit Queues */
        GT_U32      txQueuesNum;


        /****************************************************/
        /* add here other table sizes that are "parametric" */
        /****************************************************/
    }tableSize;

    struct
    {
        /* TR101 Feature support */
        GT_BOOL tr101Supported;

        /* VLAN translation support */
        GT_BOOL vlanTranslationSupported;

        /* Policer Ingress second stage support flag */
        GT_BOOL iplrSecondStageSupported;
    }featureInfo;

}PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC;

/* check policer Id , when used as 'key' we return GT_BAD_PARAM on error */
/* note : index is in the range: 0..maxNum-1 */
#define PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(_devNum,_stage,_index)              \
    if((_index) >= PRV_CPSS_DXCH_PP_MAC(_devNum)->policer.memSize[(_stage)])     \
    {                                                                            \
        return GT_BAD_PARAM;                                                     \
    }


/* check vidxId , when used as 'key' we return GT_BAD_PARAM on error */
/* note : index is in the range: 0..maxNum-1 */
#define PRV_CPSS_DXCH_VIDX_NUM_CHECK_MAC(_devNum,_index)                 \
    if((_index) >= PRV_CPSS_DXCH_PP_MAC(_devNum)->fineTuning.tableSize.vidxNum)    \
        return GT_BAD_PARAM


/*******************************************************************************
* PRV_CPSS_DXCH_PP_CONFIG_ENHANCED_INIT_FUNC
*
* DESCRIPTION:
*       do enhanced initialization , on devices that need to.
*       this function sets the fine tuning parameters needed by the CPSS SW.
*       Function may also do HW operations.
*
* APPLICABLE DEVICES:
*       NA
*
* INPUTS:
*       devNum      - The device number
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK         - on success
*       GT_HW_ERROR   - on hardware error.
*       GT_BAD_PARAM  - on bad devNum
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_PP_CONFIG_ENHANCED_INIT_FUNC)
(
    IN      GT_U8                           devNum
);

/* pointer to a function used for DXCH devices that will do enhanced
   initialization of SW and HW parameters --  for FUTURE compatibility */
extern PRV_CPSS_DXCH_PP_CONFIG_ENHANCED_INIT_FUNC prvCpssDxChPpConfigEnhancedInitFuncPtr;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChInfoEnhancedh */

