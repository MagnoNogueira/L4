/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenNetIfTypes.h
*
* DESCRIPTION:    generic network interface MII types definitions
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __cpssGenNetIfMiih
#define __cpssGenNetIfMiih

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>

/* size of single internal TX buffer in Mii CPU mode */
#define CPSS_GEN_NETIF_MII_TX_INTERNAL_BUFF_SIZE_CNS 32

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIfTypes.h>
#endif /*EXMXPM_FAMILY*/

/*
 * typedef: structure CPSS_GEN_NETIF_MII_RX_PARAMS_STC
 *
 * Description: --- get packets from the PP to CPU via ethernet port (NIC)
 *              structure of Rx parameters , that contain full information on
 *              how packet was received from PP to CPU port.
 *
 * fields:
 *      dsaParam - the DSA parameters
 *
*/
typedef struct{
    union{
#if defined CHX_FAMILY || defined EXMXPM_FAMILY
#ifdef CHX_FAMILY
        CPSS_DXCH_NET_DSA_PARAMS_STC   dxChDsaParam;
#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
        CPSS_EXMXPM_NET_DSA_PARAMS_STC   exMxPmDsaParam;
#endif /*EXMXPM_FAMILY*/
#else
       GT_U32    dummy;
#endif
    }dsa;
}CPSS_GEN_NETIF_MII_RX_PARAMS_STC;


/*
 * typedef: structure CPSS_GEN_NETIF_MII_TX_PARAMS_STC
 *
 * Description: --- sending packet from the CPU to PP via ethernet port (NIC)
 *              structure of Tx parameters , that contain full information on
 *              how to send TX packet from CPU to PP.
 *
 * fields:
 *      packetIsTagged - the packet (buffers) contain the vlan TAG inside.
 *                  GT_TRUE  - the packet contain the vlan TAG inside.
 *                  GT_FALSE - the packet NOT contain the vlan TAG inside.
 *
 *      cookie  - The user's data to be returned to the Application when in
 *                the Application's buffers can be re-used .
 *
 *      txQueue - the queue that packet should be sent to CPU port. (0..7)
 *
 *      evReqHndl - The application handle got from cpssEventBind for
 *                  CPSS_PP_TX_BUFFER_QUEUE_E events.
 *
 *      dsa - the DSA parameters
 *          dxChDsaParam - the DXCH DSA parameters
 *          exMxPmDsaParam - the EXMXPM DSA parameters
 *
*/
typedef struct{
    GT_BOOL                        packetIsTagged;
    GT_PTR                         cookie;

    GT_U8                          txQueue;
    GT_U32                         evReqHndl;

    union{
#if defined CHX_FAMILY || defined EXMXPM_FAMILY
#ifdef CHX_FAMILY
        CPSS_DXCH_NET_DSA_PARAMS_STC   dxChDsaParam;
#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
        CPSS_EXMXPM_NET_DSA_PARAMS_STC   exMxPmDsaParam;
#endif /*EXMXPM_FAMILY*/
#else
       GT_U32    dummy;
#endif
    }dsa;
}CPSS_GEN_NETIF_MII_TX_PARAMS_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __cpssGenNetIfTypesh */


