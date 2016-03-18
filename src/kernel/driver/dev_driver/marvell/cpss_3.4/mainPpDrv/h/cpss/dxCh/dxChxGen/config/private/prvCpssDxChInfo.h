/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChInfo.h
*
* DESCRIPTION:
*       Includes structures definition for the use of Dx Cheetah Prestera SW.
*
*
* FILE REVISION NUMBER:
*       $Revision: 37$
*
*******************************************************************************/

#ifndef __prvCpssDxChInfoh
#define __prvCpssDxChInfoh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* get private types */
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
/* get registers structure */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChRegs.h>
/* get module config structure */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChModuleConfig.h>
/* get errata structure */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChErrataMng.h>
/* get CPSS definitions for port configurations */
#include <cpss/generic/port/cpssPortCtrl.h>
/* get tables structure */
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
/* get DXCH registers access APIs */
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwRegisters.h>
/* get the fine tuning parameters structure */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfoEnhanced.h>

/* get CPSS definitions for private port configurations */
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>

/* get CPSS definitions for NotworkIf configurations */
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetIf.h>
/* FDB hash config */
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgFdbHash.h>
/* get CPSS definitions for policer */
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
/* generic bridge security breach types */
#include <cpss/generic/bridge/cpssGenBrgSecurityBreachTypes.h>

/* multi-port-groups debug info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChMultiPortGroupsDebug.h>

/* CPU PORT NUMBER Definition */
#define PRV_CPSS_DXCH_CPU_PORT_NUM_CNS            63

/* NULL Port */
#define PRV_CPSS_DXCH_NULL_PORT_NUM_CNS           62

/* Some features like ingress bridge rate limit and egress shaper take care of
   ports 0..23 as ports those cannot work in XG modes.
   Only ports 24..27 are XG modes capable ports for such features. */
#define PRV_DXCH_FIRST_XG_CAPABLE_PORT_CNS        24

/* macro to get a pointer on the DxCh device
    devNum - the device id of the DxCh device

    NOTE : the macro do NO validly checks !!!!
           (caller responsible for checking with other means/macro)
*/
#define PRV_CPSS_DXCH_PP_MAC(devNum) \
    ((PRV_CPSS_DXCH_PP_CONFIG_STC*)prvCpssPpConfig[devNum])

/* check that the devFamily is one of Dx Cheetah */
#define PRV_CPSS_DXCH_FAMILY_CHECK_MAC(devNum)              \
   (PRV_CPSS_PP_MAC(devNum)->functionsSupportedBmp &        \
    PRV_CPSS_DXCH_FUNCTIONS_SUPPORT_CNS)


/* check that the devFamily is one of Dx Cheetah 2 and above */
#define PRV_CPSS_DXCH2_FAMILY_CHECK_MAC(devNum)              \
   (PRV_CPSS_PP_MAC(devNum)->functionsSupportedBmp &         \
    PRV_CPSS_DXCH2_FUNCTIONS_SUPPORT_CNS)

/* check that the devFamily is one of Dx Cheetah 3 and above */
#define PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum)              \
   (PRV_CPSS_PP_MAC(devNum)->functionsSupportedBmp &         \
    PRV_CPSS_DXCH3_FUNCTIONS_SUPPORT_CNS)


/* check that the devFamily is one of DxCh xCat and above */
#define PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)       \
   (PRV_CPSS_PP_MAC(devNum)->functionsSupportedBmp &         \
    PRV_CPSS_DXCH_XCAT_FUNCTIONS_SUPPORT_CNS)


/* check that the devFamily is one of DxCh Lion and above */
#define PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum)       \
   (PRV_CPSS_PP_MAC(devNum)->functionsSupportedBmp &         \
    PRV_CPSS_DXCH_LION_FUNCTIONS_SUPPORT_CNS)

/* check that the device is on of EXISTING DxCh
    return GT_BAD_PARAM or GT_NOT_APPLICABLE_DEVICE on error
*/
#define PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum)                   \
    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))               \
        return GT_BAD_PARAM;                                  \
    if(0 == PRV_CPSS_DXCH_FAMILY_CHECK_MAC(devNum))           \
        return GT_NOT_APPLICABLE_DEVICE

/* check if the DxCh XCAT device is DxCh xCat_B1(A3B) device */
#define PRV_CPSS_DXCH_XCAT_B1_ONLY_CHECK_MAC(_devNum)                    \
    ((PRV_CPSS_PP_MAC(_devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) \
       &&                                                                \
    ((PRV_CPSS_DXCH_PP_HW_INFO_MG_MAC(_devNum).metalFix & 8) != 0))

/* check if the DxCh XCAT device is DxCh xCat_A3 device */
#define PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(_devNum)                    \
    ((PRV_CPSS_PP_MAC(_devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) \
       &&                                                                \
    ((PRV_CPSS_DXCH_PP_HW_INFO_MG_MAC(_devNum).metalFix & 4) != 0))

/* check that device is DxCh xCat_A3, otherwize return GT_BAD_PARAM or
   GT_NOT_APPLICABLE_DEVICE on error */
#define PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(_devNum)    \
    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))             \
        return GT_BAD_PARAM;                                 \
    if (! PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(_devNum))     \
        return GT_NOT_APPLICABLE_DEVICE;

/* check that the devFamily is one of DxCh xCat2 and above */
#define PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum)         \
   (PRV_CPSS_PP_MAC(devNum)->functionsSupportedBmp &         \
    PRV_CPSS_DXCH_XCAT2_FUNCTIONS_SUPPORT_CNS)

/* get the port type */
#define PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) \
    (((portNum) == CPSS_CPU_PORT_NUM_CNS)? \
         PRV_CPSS_PORT_GE_E:PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType)

 

/* get the port interface mode */
#define PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum) \
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portIfMode

/* get XG port type */
#define PRV_CPSS_DXCH_XG_PORT_TYPE_MAC(devNum,portNum) \
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portTypeOptions

/* get isFlexLink */
#define PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) \
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].isFlexLink

/* check physical port -- use the generic macro */
#define PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC \
        PRV_CPSS_PHY_PORT_CHECK_MAC

/* check physical port or CPU port -- use the generic macro */
#define PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC \
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC

/* access to the registers addresses of the DxCh device */
#define PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)  \
    (&(PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr))

/* access to the config module of the DxCh device */
#define PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum)  \
    (&(PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg))

/* number of QoS Profiles */
#define PRV_CPSS_DXCH_QOS_PROFILE_NUM_MAX_CNS    72

/* number of QoS Profiles for Cheetah2 */
#define PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS    128

/* DxCh support 256 STG entries */
#define PRV_CPSS_DXCH_STG_MAX_NUM_CNS        256

/* check stp range */
#define PRV_CPSS_DXCH_STG_RANGE_CHECK_MAC(_devNum,_stpId)    \
    if((_stpId) >= PRV_CPSS_DXCH_PP_MAC(_devNum)->fineTuning.tableSize.stgNum)     \
        return GT_BAD_PARAM

/* Maximal number of QoS Profiles  for a device */
#define PRV_CPSS_DXCH_QOS_PROFILE_MAX_MAC(_devNum)                           \
    (((PRV_CPSS_PP_MAC(_devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E) ?   \
     PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS : PRV_CPSS_DXCH_QOS_PROFILE_NUM_MAX_CNS)

/* check CH QoS Profile Index */
#define PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(_devNum, _profileId)     \
        if((_profileId) >= (GT_U32)(PRV_CPSS_DXCH_QOS_PROFILE_MAX_MAC(_devNum)))   \
            return GT_BAD_PARAM


/* macro to check if current iteration is not valid any more
    devNum - the device number to check with it it's max number
             the device number is controlling the "HW interface bus" that allow
             us to tune the needed value for max number of iterations
    currIter - the current iteration number , should be 0...max in increasing
               manner
*/
#define  PRV_CPSS_DXCH_MAX_NUM_ITERATIONS_CHECK_CNS(devNum,currIter)    \
    if((currIter) >= PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum)->miscellaneous.maxIterationsOfBusyWait) \
        return GT_TIMEOUT

/* macro to check if sdma interface used for cpu traffic by current device */
#if 1 /* EdgeCore Patch */
#define  PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(__devNum)    \
    if((PRV_CPSS_PP_MAC(__devNum)->cpuPortMode != PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)   \
    && (PRV_CPSS_PP_MAC(__devNum)->cpuPortMode != PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E)) \
        return GT_BAD_STATE
#else /* EdgeCore Patch */
#define  PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(__devNum)    \
    if(PRV_CPSS_PP_MAC(__devNum)->cpuPortMode != PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)   \
        return GT_BAD_STATE
#endif /* EdgeCore Patch */

/* macro to set where the Lion sim will have different configuration */
#define PRV_CPSS_DXCH_LION_BOOKMARK  CPSS_TBD_BOOKMARK

/*
 * Typedef: struct PRV_CPSS_DXCH_BRIDGE_INFO_STC
 *
 * Description: A structure to hold all PP data needed on bridge DxCh layer
 *
 * Fields:
 *      devTable    - bmp of HW device Ids that the FDB age daemon in PP
 *                     should know about needed as part of Erratum WA.
 *      fdbHashParams - FDB hash parameters.
 */
typedef struct
{
    GT_U32                      devTable;
    PRV_CPSS_DXCH_MAC_HASH_STC  fdbHashParams;
} PRV_CPSS_DXCH_BRIDGE_INFO_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_POLICER_INFO_STC
 *
 * Description: A structure to hold all PP data needed on policer DxCh layer
 *
 * Fields:
 *          memSize       - Memory size per stage.
 *                          Memory size is the maximal number of entries in
 *                          the policer tables.
 *          meteringCalcMethod      - selecting calculation of HW metering
 *                                    parameters between CIR\PIR or CBS\PBS
 *                                    orientation.
 *          cirPirAllowedDeviation  - the allowed deviation in percentage from
 *                                    the requested CBS\PBS. Relevant only for
 *                        CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E.
 *          cbsPbsCalcOnFail        - GT_TRUE: If CBS\PBS constraints cannot be
 *                                    matched return to CIR\PIR oriented
 *                                    calculation.
 *                                    GT_FALSE: If CBS\PBS constraints cannot
 *                                    be matched return error.
 *                                    Relevant only for
 *                        CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E.
 *
 */
typedef struct{
    GT_U32                                      memSize[3];
    CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT  meteringCalcMethod;
    GT_U32                                      cirPirAllowedDeviation;
    GT_BOOL                                     cbsPbsCalcOnFail;
}PRV_CPSS_DXCH_POLICER_INFO_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_PORT_XLG_MIB_SHADOW_STC
 *
 * Description: A structure to hold for XLG MIB counters support DxCh layer
 *
 * Fields:
 *       mibShadow     -  XLG MIB counters entry shadow.
 *       captureMibShadow - XLG MIB counters entry caputer shadow.
 *       clearOnReadEnable - clear on read enable
 */
typedef struct{
    GT_U32 mibShadow[PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS] ;
    GT_U32 captureMibShadow[PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS];
    GT_BOOL clearOnReadEnable;
}PRV_CPSS_DXCH_PORT_XLG_MIB_SHADOW_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_PORT_INFO_STC
 *
 * Description: A structure to hold all PP data needed on port DxCh layer
 *
 * Fields:
 *       serdesRefClock     - SERDES reference clock type.
 *       xlgPortsMibShadow  - XLG ports MAC MIB counters shadow;
 *                            there is one XLG port per port group.
 */
typedef struct{
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_ENT    serdesRefClock ;
    PRV_CPSS_DXCH_PORT_XLG_MIB_SHADOW_STC  xlgPortsMibShadow[CPSS_MAX_PORT_GROUPS_CNS];
}PRV_CPSS_DXCH_PORT_INFO_STC;

/*
 * typedef: enum CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_G003_ENT
 *
 * Description: Enumeration of the FDB modes
 *
 * Enumerations:
 *    PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_NON_MULTI_PORT_GROUP_DEVICE_E -
 *      value for 'non-multi port groups device'
 *
 *    PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E  - all port groups have the
 *          same FDB entries. (unified tables) (except SP entries)
 *    PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E  - the port groups of
 *          the device are split to 2 groups .
 *          Each port group in those groups may hold different FDB entries.
 *          But the 2 groups hold the same entries. (except SP entries)
 *    PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E - each port group of
 *          the device may hold different FDB entries.
 *
 */
typedef enum{
    PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_NON_MULTI_PORT_GROUP_DEVICE_E = 0,

    PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E,
    PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E,
    PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E
}PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT;

/*
 * Typedef: struct PRV_CPSS_DXCH_PP_PORT_GROUPS_INFO_STC
 *
 * Description: A structure to hold info about the port-group of a device
 *          the info used ALSO by the non multi-port-groups device
 *
 * Fields:
 *      debugInfo - debug info for the 'multi-port-groups' device.
 *                  this info used for internal behavior tests (debug) and
 *                  configuration.
 *
 *      fdbMode - FDBs mode. the mode influence the way the FDB APIs behave.
 *                  see also description of PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT
 *
 *     securBreachDropCounterInfo - info about the configuration of security
 *                          breach drop counters .
 *          .counterMode - counter mode - vlan/port
 *          .portGroupId - port-group-id of the security breach drop counters.
 *                  when the security counters are set 'for port' this port-group
 *                  state the port-group that actually do the drop ,and counts.
 *                  -- all other port-groups get 'NULL port' filter
 *                  when the counter is in 'for vlan' --> CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *                  to indicate that configuration is on all port-groups.
 *
 *      portEgressCntrModeInfo - info about the configuration of the 2 egress
 *                          counter sets.
 *          .portGroupId - the port-group-id of the port egress counters counters.
 *                  when the counters are set 'port filter' this port-group
 *                  state the port-group that actually do the drop ,and counts.
 *                  -- all other port-groups get 'NULL port' filter
 *                  when the counter is without 'port filter' --> CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *                  to indicate that configuration is on all port-groups.
 *
 *      bridgeIngressCntrMode - info about the configuration of the 2 ingress
 *                          counter sets.
 *          .portGroupId - the port-group-id of the port ingress counters counters.
 *                  when the counters are set 'port filter' this port-group
 *                  state the port-group that actually do the drop ,and counts.
 *                  -- all other port-groups get 'NULL port' filter
 *                  when the counter is without 'port filter' --> CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *                  to indicate that configuration is on all port-groups.
 *
 *
 */
typedef struct
{
    PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_STC   debugInfo;

    PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT fdbMode;

    struct
    {
        CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT    counterMode;
        GT_U32      portGroupId;
    } securBreachDropCounterInfo;

    struct
    {
        GT_U32      portGroupId;
    } portEgressCntrModeInfo[2];

    struct
    {
        GT_U32      portGroupId;
    } bridgeIngressCntrMode[2];

} PRV_CPSS_DXCH_PP_PORT_GROUPS_INFO_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_PP_CONFIG_STC
 *
 * Description: A structure to hold all PP data needed in CPSS DxCh
 *
 * Fields:
 *      genInfo     - generic info structure
 *      moduleCfg   - Module configuration parameters such as tables addresses ,
 *                    num entries , entries size , entries offset
 *      regsAddr    - registers addresses of the device
 *                    (relevant for this device type).
 *      errata  - info about what Errata should be fixed in the CPSS for the
 *              device (depend on : revision , devType)
 *      fineTuning -  tables sizes and parameters that define the "fine tuning"
 *                    of the specific device.
 *      bridge  - needed bridge info on the device
 *      accessTableInfoPtr - pointer direct access table entry info.
 *      accessTableInfoSize - table size.
 *      netIf   - info needed by the network interface.
 *      policer - info needed by the policer
 *      port    -  needed port info on the device
 *      portGroupsExtraInfo - extra info needed by several libraries but concern
 *                  'multi-port-groups' device support
 *      hwInfo - info about PP (HW parameters)
 *
 */
typedef struct
{
    PRV_CPSS_GEN_PP_CONFIG_STC      genInfo;

    PRV_CPSS_DXCH_MODULE_CONFIG_STC moduleCfg;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC  regsAddr;
    PRV_CPSS_DXCH_ERRATA_STC        errata;

    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC fineTuning;

    PRV_CPSS_DXCH_BRIDGE_INFO_STC      bridge;
    PRV_CPSS_DXCH_TABLES_INFO_STC      *accessTableInfoPtr;
    GT_U32                             accessTableInfoSize;
    PRV_CPSS_DXCH_NET_INFO_STC         netIf;
    PRV_CPSS_DXCH_POLICER_INFO_STC     policer;
    PRV_CPSS_DXCH_PORT_INFO_STC        port;

    PRV_CPSS_DXCH_PP_PORT_GROUPS_INFO_STC    portGroupsExtraInfo;

    PRV_CPSS_DXCH_PP_HW_INFO_STC       hwInfo;

} PRV_CPSS_DXCH_PP_CONFIG_STC;

/* array of pointers to the valid devices */
extern void* prvCpssPpConfig[PRV_CPSS_MAX_PP_DEVICES_CNS];


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChInfoh */


