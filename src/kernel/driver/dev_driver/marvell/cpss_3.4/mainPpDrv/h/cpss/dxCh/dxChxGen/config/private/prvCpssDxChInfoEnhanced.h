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
*       $Revision: 15 $
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
 *          tunnelStart - the number of rows in router ARP and tunnel start table.(number of TS entries)
 *          routerArp - the number of ARP entries in router ARP and tunnel start table.
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
 *                        Note: For xCat and above, this is the number of ingress policers
 *                        for both ingress stages.
 *          egressPolicersNum - Number of egress policers.
 *                              Note: Relevant for xCat and above.
 *          trunksNum - number of trunks.
 *          transmitDescr  - Transmit Descriptors number. Supported for DxCh3 and above.
 *          bufferMemory   - Buffer Memory size. Supported for DxCh3 and above.
 *          txQueuesNum    -  Transmit Queues Number
 *
 *      featureInfo - structure that hold the features that the PP may support/not support.
 *          tr101Supported - TR101 feature support. Supported for xCat and above.
 *                              GT_TRUE - supported
 *                              GT_FALSE - not supported
 *          vlanTranslationSupported - VLAN translation feature support. Supported for DxCh3 and above.
 *                              GT_TRUE - supported
 *                              GT_FALSE - not supported
 *          iplrSecondStageSupported -
 *                              GT_TRUE - There are two ingress stages.
 *                              GT_FALSE - There is only one ingress stage.
 *                              Note: Relevant for xCat and above.
 *          trunkCrcHashSupported - Enhanced Hash for load balancing (CRC based)
 *                              GT_TRUE - supported
 *                              GT_FALSE - not supported
 *                              Note: Relevant for Lion and above.
 *
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
        GT_U32  tunnelStart;
        GT_U32  routerArp;

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

        /* trunk CRC hash support flag */
        GT_BOOL trunkCrcHashSupported;
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


/*
 * Typedef: struct PRV_CPSS_DXCH_PP_HW_INFO_STC
 *
 * Description: A Structure including PP's HW major differences between devices.
 * Notes:
 *         the initialization of those fields should be done during "phase 1".
 *
 * Fields:
 *      l2i - info about the bridge unit
 *              supportIngressRateLimiter - is the device support the
 *                  'ingress rate limiter' (in the L2i unit)
 *      cnc - info about the CNC unit
 *              cncBlocks          - number of max CNC blocks
 *              cncBlockNumEntries - number of entries in CNC block
 *      txq - info about the TXQ
 *          revision - the revision of the TXQ
 *
 *      ha - info about the Header alteration
 *          cpuPortBitIndex - CPU port index for traffic that access the HA unit .
 *                    ch1 :
 *                          CPU port --> bit 27
 *                          cpuPortBitIndex = 27
 *                    ch2,3,xcat :
 *                          CPU port --> bit 31
 *                          cpuPortBitIndex = 27
 *                    Lion A:
 *                          CPU port --> bit 31
 *                          cpuPortBitIndex = 27
 *                    Lion B:
 *                          cpuPortBitIndex = 63
 *      mg  - management unit
 *          metalFix        - the contents of metal Fix register
 *                            at power on time.
 *                            relevant for all DxCh devices:
 *                            For xCat: MG metal Fix register value.
 *                            For dxCh3 B2: 1 (lsb set).
 *                            Otherwise: 0 (zero).
 *
*/
typedef struct{
    struct{
        GT_BOOL supportIngressRateLimiter;
    }l2i;
    struct{
        GT_U32 cncBlocks;
        GT_U32 cncBlockNumEntries;
    }cnc;
    struct{
        GT_U32  revision;
    }txq;
    struct{
        GT_U32  cpuPortBitIndex;
    }ha;
    struct{
        GT_U32  metalFix;
    }mg;
}PRV_CPSS_DXCH_PP_HW_INFO_STC;

/* macro to get pointer to the hwInfo.txq in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_TXQ_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.txq

/* macro to get pointer to the hwInfo.ha in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_HA_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.ha

/* macro to get pointer to the hwInfo.cnc in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.cnc

/* check if txq revision is 1 or above */
#define PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(_devNum)    \
    ((PRV_CPSS_DXCH_PP_HW_INFO_TXQ_MAC(_devNum).revision) ? 1 : 0)

/* get the HA egress cpu port bit index */
#define PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(_devNum) \
    PRV_CPSS_DXCH_PP_HW_INFO_HA_MAC(_devNum).cpuPortBitIndex

/*  ch1 special macro support : cpu port in bit 27 and not 31 in the HA register */
#define PRV_CPSS_DXCH_CH1_SPECIAL_BIT_HA_CPU_PORT_BIT_INDEX_MAC(_devNum , ch1CpuPort) \
    ((PRV_CPSS_PP_MAC(_devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E) ?       \
    PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(_devNum) : ch1CpuPort)

/* macro to get pointer to the hwInfo.l2i in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_L2I_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.l2i

/* macro to get pointer to the hwInfo.mg in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_MG_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.mg

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChInfoEnhancedh */

