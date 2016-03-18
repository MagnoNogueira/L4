/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtAppDemoSysConfig.h
*
* DESCRIPTION:
*       System configuration and initialization control.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 38 $
*
*******************************************************************************/
#ifndef __gtAppDemoSysConfigh
#define __gtAppDemoSysConfigh


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoPciConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoSmiConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoTwsiConfig.h>

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h> /* this is not ok - just need for CPSS_PP_FAMILY_TYPE_ENT to compile*/

#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>

/* for cascadig required */
#include <cpss/generic/cscd/cpssGenCscd.h>

/* Defines the max number LPM sram in EXMXPM devices
*/
#define APPDEMO_EXMXPM_MAX_NUMBER_OF_EXTERNAL_LPM_SRAMS_CNS 3
#define APPDEMO_EXMXPM_MAX_NUMBER_OF_EXTERNAL_MEMORY_SRAMS_CNS 2

/* constant for FX950 family type */
#define APPDEMO_FX950_FAMILY_TYPE   0x95

/* constant for FX950 device ID and vendor ID */
#define APPDEMO_FX950_DEV_ID_AND_VENDOR_ID   CPSS_98FX950_CNS

#define APP_DEMO_CPSS_NO_PP_CONNECTED_CNS 0xFFFFFFFF

#define APP_DEMO_CPSS_MAX_NUMBER_OF_SRAMS_CNS      10

/* Use this constant to retrieve core clock value from HW.
   Currently supported only for DxCh and ExMxPm devices*/
#define APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS 0xFFFFFFFF

/* 4 port-groups for multi-port-group device */
#define APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS 4

/* User configured bitmap for SERDES power down */
extern GT_U32 appDemoDxChPowerDownBmp;

/*
 * typedef: enum APPDEMO_EXMXPM_RAM_TYPE_ENT
 *
 * Description: Defines ram types in EXMXPM devices.
 *
 * Enumerations:
 *  APPDEMO_EXMXPM_RAM_TYPE_SRAM_E     - SRAM is used.
 *  APPDEMO_EXMXPM_RAM_TYPE_RLDRAM_E   - RLDRAM is used.
 *
 */
typedef enum
{
    APPDEMO_EXMXPM_RAM_TYPE_SRAM_E,
    APPDEMO_EXMXPM_RAM_TYPE_RLDRAM_E
}
APPDEMO_EXMXPM_RAM_TYPE_ENT;

/*
 * typedef: enum APPDEMO_EXMXPM_CSU_PROTECT_ENT
 *
 * Description: Defines protection mode for Control Memory (CSU) in EXMXPN devices.
 *
 * Enumerations:
 *  APPDEMO_EXMXPM_CSU_PROTECT_PARITY_E - Control Memory data is protected by parity.
 *  APPDEMO_EXMXPM_CSU_PROTECT_ECC_E    - Control Memory data is protected by ECC.
 *
 */
typedef enum
{
    APPDEMO_EXMXPM_CSU_PROTECT_PARITY_E,
    APPDEMO_EXMXPM_CSU_PROTECT_ECC_E
}
APPDEMO_EXMXPM_CSU_PROTECT_ENT;

/*
 * typedef: enum APPDEMO_EXMXPM_BUFFER_SIZE_ENT
 *
 * Description: Defines buffer size (bytes) used in Ingress/Egress pipe.
 *
 * Enumerations:
 *  APPDEMO_EXMXPM_BUFFER_SIZE_256_E - Buffer size used is 256 bytes.
 *  APPDEMO_EXMXPM_BUFFER_SIZE_512_E - Buffer size used is 512 bytes.
 *  APPDEMO_EXMXPM_BUFFER_SIZE_1K_E  - Buffer size used is 1K bytes.
 *  APPDEMO_EXMXPM_BUFFER_SIZE_2K_E  - Buffer size used is 2K bytes.
 *  APPDEMO_EXMXPM_BUFFER_SIZE_4K_E  - Buffer size used is 4K bytes.
 *  APPDEMO_EXMXPM_BUFFER_SIZE_8K_E  - Buffer size used is 8K bytes.
 *
 */
typedef enum
{
    APPDEMO_EXMXPM_BUFFER_SIZE_256_E,
    APPDEMO_EXMXPM_BUFFER_SIZE_512_E,
    APPDEMO_EXMXPM_BUFFER_SIZE_1K_E,
    APPDEMO_EXMXPM_BUFFER_SIZE_2K_E,
    APPDEMO_EXMXPM_BUFFER_SIZE_4K_E,
    APPDEMO_EXMXPM_BUFFER_SIZE_8K_E
}
APPDEMO_EXMXPM_BUFFER_SIZE_ENT;

/*
 * typedef: enum APPDEMO_SERDES_REF_CLOCK_ENT
 *
 * Description: Defines SERDES reference clock type.
 *
 * Enumerations:
 *  APPDEMO_SERDES_REF_CLOCK_EXTERNAL_25_SINGLE_ENDED_E        - 25 MHz external
 *                                                               single ended reference clock.
 *  APPDEMO_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E       - 125 MHz external
 *                                                               single ended reference clock.
 *  APPDEMO_SERDES_REF_CLOCK_EXTERNAL_125_DIFF_E               - 125 MHz external
 *                                                               differential reference clock.
 *  APPDEMO_SERDES_REF_CLOCK_EXTERNAL_156_25_SINGLE_ENDED_E    - 156.25 MHz external
 *                                                               single ended reference clock.
 *  APPDEMO_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E            - 156.25 MHz external
 *                                                               differential reference clock.
 *  APPDEMO_SERDES_REF_CLOCK_INTERNAL_125_E                    - 125 MHz internal reference clock.
 */
typedef enum
{
    APPDEMO_SERDES_REF_CLOCK_EXTERNAL_25_SINGLE_ENDED_E,
    APPDEMO_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E,
    APPDEMO_SERDES_REF_CLOCK_EXTERNAL_125_DIFF_E,
    APPDEMO_SERDES_REF_CLOCK_EXTERNAL_156_25_SINGLE_ENDED_E,
    APPDEMO_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E,
    APPDEMO_SERDES_REF_CLOCK_INTERNAL_125_E
}
APPDEMO_SERDES_REF_CLOCK_ENT;

/*
 * Typedef: struct APPDEMO_EXMXPM_SRAM_CFG_STC
 *
 * Description: Includes EXMXPM devices external Sram configuration values
 *
 * Fields:
 *      sramSize        - The external Sram size.
 *
 */
typedef struct
{
    CPSS_SRAM_SIZE_ENT        sramSize;
}APPDEMO_EXMXPM_SRAM_CFG_STC;


/*
 * typedef: struct CPSS_PP_PHASE1_INIT_PARAMS
 *
 * Description:
 *      This struct defines specific hardware parameters, those parameters are
 *      determined according to the board specific hardware components.
 *
 * Fields:
 *    devNum            - Temporary device number to allow basic communication.
 *    baseAddr          - The packet process PCI base address for the internal
 *                        Prestera address space.
 *                        in case of SMI channel : it take the SmiIdSel.
 *    internalPciBase   - The packet process PCI base address for the internal
 *                        PCI address space.
 *    deviceId          - The unique Prestera Packet Processor device ID,
 *                        as read from its PCI register during PCI scan phase.
 *                        (Including the vendor Id).
 *    revision          - The device revision number,
 *                        as read from its PCI register during PCI scan phase.
 *    intVecNum         - The interrupt vector number this PP is connected to.
 *    intMask           - The interrupt mask to enable PP interrupts.
 *    coreClk           - The PP core clock in MHz.
 *                        APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS may be used for
 *                        core clock auto detection.
 *    flowDramCfg       - External flow dram configuration parameters.
 *    bufferDramCfg     - The buffers dram configuration parameters.
 *    bufferMgmtCfg     - Buffer management configuration parameters.
 *    uplinkCfg         - determine the uplink connection status.
 *    bufferDram        - type of buffer DRAM, according to this field the DRAM
 *                          parameters changed in setPpRamConfigPhase1().
 *    flowDram          - type of flow DRAM, according to this field the DRAM
 *                          parameters changed in setPpRamConfigPhase1().
 *    mngInterfaceType  - Management interface type (SMI/PCI)
 *    routingSramCfgType- Determines what's the routing memory configuration.
 *    sramsCfg                - External SRAMs configuration parameters.It includes
 *                        both External Control Sram configuration parameters
 *                        (aka Wide Sram) and External Routing Sram
 *                        configuration parameters (aka Narrow Sram).
 *    numOfSramsCfg     - Number of valid entries in sramsCfg.
 *    hwDevNum          - HW device number. The PSS writes this device number in the
 *                        PP instead of devNum. The field is used only for systems whith
 *                        Coexistance Manager lib included.
 *                        For other system hwDevNum is ignored
 *   maxBufSize         - System maximum buffer size.
 *   eventsTaskPrio     - Default RTOS Priority to be assigned to the
 *                        Interrupt task.
 *   eventsTaskStackSize- Stack size (in bytes) needed for user
 *                        functions invoked from within the user exits
 *                        context.
 *                        The actual interrupt task stack size equals:
 *                        eventsTaskStackSize + <internal stack size>,
 *                        where <internal stack size> is the stack size
 *                        needed for the interrupt task.
 *   sysHAState         - system HA mode, e.g. active or standby
 *   powerDownPortsBmp  - force SERDES power down bmp - support for CH3 and ExMxPm,
 *                        for CH3 - not include Combo SERDES.
 *   serdesRefClock      - SERDES reference clock type.
 *   initSerdesDefaults  - GT_TRUE - cpss performs SERDES initialization.
 *                         GT_FALSE- cpss doesn't perform SERDES initialization.
 *                         It is supported for dxCh3 and above devices.
 *   isExternalCpuConnected - it's ignored for all not xCat devices
 *                            it's ignored for xCat devices without internal CPU
 *                            it's relevant only for xCat devices with internal CPU
 *                            GT_TRUE - External CPU is connected to the PP. CPSS configures PP to work with external CPU
 *                                      even if internal CPU is active
 *                            GT_FALSE- External CPU is not connected to the PP (only internal CPU active)
 *   ingressBufferMemoryEnableScrambling - GT_TRUE for enable scrambling on ingress buffer SRAM.
 *                                         GT_FALSE for disable scrambling on ingress buffer SRAM.
 *   ingressBufferSramCfg                - The ingress buffers SRAM configuration parameters.
 *   ingressBufferSize                   - The ingress buffer size.
 *   egressBufferMemoryEnableScrambling  - GT_TRUE for enable scrambling on egress buffer SRAM.
 *                                         GT_FALSE for disable scrambling on egress buffer SRAM.
 *   egressBufferRamType                 - Egress Memory Ram type.
 *   egressBufferSramCfg                 - Egress Memory SRAM configuration (relevant only if
 *                                         egressBufferRamType is APPDEMO_EXMXPM_RAM_TYPE_SRAM_E).
 *   egressBufferRldramCfg               - Egress Memory RLDRAM configuration (relevant only if
 *                                         egressBufferRamType is APPDEMO_EXMXPM_RAM_TYPE_RLDRAM_E).
 *   egressBufferSize                    - The egress buffer size.
 *                                         APPDEMO_EXMXPM_BUFFER_SIZE_256_E is not supported for egress.
 *   controlSramUsedArray - array of booleans that indicate if there is external
 *                          control SRAM (access to array is pointed by
 *                          number of external memory:CSU0-index 0, CSU1-index 1)
 *   controlSramCfgArray  - array of info about the external control SRAM.
 *                          (access to array is pointed by number of external
 *                          memory: CSU0 - index 0, CSU1 - index 1)
 *   controlSramProtectArray - array of info about the external control SRAM protection mode
 *                             (access to array is pointed by number of external
 *                             memory: CSU0 - index 0, CSU1 - index 1)
 *
 *   numOfPortGroups - NOT relevant to 'non-multi-port-groups' device.
 *                           (relevant only when busBaseAddr, internalPciBase, intVecNum , intMask
 *                            All equal to CPSS_PARAM_NOT_USED_CNS)
 *                             Use 4 for Lion device.
 *   portGroupsInfo[]  -   info needed per port group.
 *      portGroupsInfo[].busBaseAddr - (per port group) the unique bus Base address (PCI base address /   SMI id select / TWSI id select...) that the device connected to on the management interface bus (PCI/SMI/TWSI)
 *      portGroupsInfo[].internalPciBase - (per port group)Base address to which the internal PCI registers are mapped to. relevant only when mngInterfaceType = CPSS_CHANNEL_PCI_E
 *      portGroupsInfo[].intVecNum - The interrupt vector number this PP is connected to. may use value PSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS when NO ISR bind needed for this device.
 *      portGroupsInfo[].intMask = The interrupt mask to enable PP interrupts
 *
 */
typedef struct
{
    GT_U8                           devNum;
    GT_UINTPTR                      baseAddr;
    GT_UINTPTR                      internalPciBase;
    GT_U32                          deviceId;
    GT_U32                          revision;
    GT_U32                          intVecNum;
    GT_U32                          intMask;
    GT_U32                          coreClk;
    CPSS_DRAM_CFG_STC               flowDramCfg;
    CPSS_DRAM_CFG_STC               bufferDramCfg;
    CPSS_BUFF_MGMT_CFG_STC          bufferMgmtCfg;
    CPSS_PP_UPLINK_CONFIG_ENT       uplinkCfg;
    CPSS_BUFFER_DRAM_PARAM_ENT      bufferDram;
    CPSS_FLOW_DRAM_PARAM_ENT        flowDram;
    CPSS_PP_INTERFACE_CHANNEL_ENT   mngInterfaceType;
    CPSS_NARROW_SRAM_CFG_ENT        routingSramCfgType;
    GT_U32                          numOfSramsCfg;
    CPSS_SRAM_CFG_STC               sramsCfg[CPSS_MAX_NUMBER_OF_EXTERNAL_SRAMS_CNS];
    CPSS_BUF_MODE_ENT               maxBufSize;
    CPSS_SYS_HA_MODE_ENT            sysHAState;
    CPSS_PORTS_BMP_STC              powerDownPortsBmp;
    APPDEMO_SERDES_REF_CLOCK_ENT    serdesRefClock;
    GT_BOOL                         initSerdesDefaults;
    GT_BOOL                         isExternalCpuConnected;
    /*******************/
    /* start Puma info */
    /*******************/
    /* ingress buffer SRAM */
    GT_BOOL                         ingressBufferMemoryEnableScrambling;
    APPDEMO_EXMXPM_SRAM_CFG_STC     ingressBufferSramCfg;
    APPDEMO_EXMXPM_BUFFER_SIZE_ENT  ingressBufferSize;

    /* egress buffer RAM */
    GT_BOOL                         egressBufferMemoryEnableScrambling;
    APPDEMO_EXMXPM_RAM_TYPE_ENT     egressBufferRamType;
    APPDEMO_EXMXPM_SRAM_CFG_STC     egressBufferSramCfg;
    CPSS_RLDRAM_CFG_STC             egressBufferRldramCfg;
    APPDEMO_EXMXPM_BUFFER_SIZE_ENT  egressBufferSize;

    /* external LPM sram */
    GT_BOOL                         lpmSramUsedArray[APPDEMO_EXMXPM_MAX_NUMBER_OF_EXTERNAL_LPM_SRAMS_CNS];
    CPSS_SRAM_CFG_STC               lpmSramCfgArray[APPDEMO_EXMXPM_MAX_NUMBER_OF_EXTERNAL_LPM_SRAMS_CNS];

    /* external CSU */
    GT_BOOL                         controlSramUsedArray[APPDEMO_EXMXPM_MAX_NUMBER_OF_EXTERNAL_MEMORY_SRAMS_CNS];
    APPDEMO_EXMXPM_SRAM_CFG_STC     controlSramCfgArray[APPDEMO_EXMXPM_MAX_NUMBER_OF_EXTERNAL_MEMORY_SRAMS_CNS];
    APPDEMO_EXMXPM_CSU_PROTECT_ENT  controlSramProtectArray[APPDEMO_EXMXPM_MAX_NUMBER_OF_EXTERNAL_MEMORY_SRAMS_CNS];

    /* external TCAM */
    GT_BOOL                         externalTcamUsed;
    /*****************/
    /* end Puma info */
    /*****************/

    /*****************************/
    /* start multi-port-groups support */
    /*****************************/
    GT_U32                          numOfPortGroups;
    struct{
        GT_UINTPTR  busBaseAddr;
        GT_UINTPTR  internalPciBase;
        GT_U32      intVecNum;
        GT_U32      intMask;
    }portGroupsInfo[APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS];

    /***************************/
    /* end multi-port-groups support */
    /***************************/
}CPSS_PP_PHASE1_INIT_PARAMS;


/*
 * typedef: struct GT_PP_PHASE2_INIT_PARAMS
 *
 * Description: PP Hw phase2 init Parameters.
 *
 * Fields:
 *    devNum         - The device number to assign to this PP. Range is
 *                     0-127.
 *    baseAddr       - The packet process PCI base address for the internal
 *                     Prestera address space.
 *    deviceId       - The unique Prestera Packet Processor device ID,
 *                      as returned from corePpHwPhase1Init().
 *    netIfCfg       - Network interface configuration parameters:
 *    auqCfg         - Address Update Queue configuration parameters:
 *    hwDevNum       - HW device number. The PSS writes this device number in the
 *                     PP instead of devNum. The field is used only for systems with
 *                     Coexistance Manager lib included.
 *                     For other system hwDevNum is ignored
 *    fuqCfg         - FDB Upload Queue configuration parameters.The parameter
 *                     used only for 98DX2x5 devices and ignored for another devices
 *    fuqUseSeparate - GT_TRUE  - use separate queue  FDB Upload messages ,
 *                   - GT_FALSE - use Address Update queue for FDB Upload messages
 *                     The parameter used only for 98DX2x5 devices and ignored
 *                     for another devices
 *    rxBufSize      - The size of the SDMA Rx data buffer. If the Data
 *                     buffer size is smaller than received packet
 *                     size, the packet is "chained" over multiple
 *                     buffers. This value is used by the CORE when
 *                     initializing the RX descriptor rings.
 *    headerOffset   - The number of bytes before the start of the Rx
 *                     buffer to reserve for the applicaion use.
 *                     This value is used by the CORE when
 *                     initializing the RX descriptor rings.
 *                     NOTE: This parameter must be synchronized among
 *                     all system CPUs.
 *    netifSdmaPortGroupId  - value of port group ID used for CPU network 
 *                            interface (relevant only for multi-port group devices while
 *                            working in SDMA mode).
 *
 *
 * Comments:
 *    1.    The Driver allocates Tx descriptors, Rx Descriptors, and Rx data
 *          buffers out of the respective blocks of memory provided by the user.
 *
 *          The number of allocated items is a function of the block size, item
 *          structure size, and the HW alignment requirement for the specific
 *          item (and the Rx Buffer headerOffset as defined in sysConfig.h)
 *
 *    2.    The Prestera PP PCI Host Interface directly updates the Rx/Tx
 *          descriptors, Rx data buffers, and the Address  Update Queue.
 *          It is the user's responsibility to determine whether these memory
 *          blocks (txDescBlock, rxDescBlock, rxbufBlock, AUQ) are from
 *          cachable. non-cachable memory regions. For example if the
 *          system supports cach-coherency between the CPU & the
 *          PCI, those blocks can be in a cachable area with no
 *          special handling
 *
 *          If there is no cache coherency, then:
 *              1)  the Rx/Tx descriptors must be allocated from non-cachable
 *                  memory.
 *              2)  the Rx data buffers and AUQ may be from cachable memory,
 *                  if the user flushes the buffer memory cache prior to
 *                  returning the buffer to the Rx pool, and cache-line
 *                  reads do not cross data buffer boundaries.
 *
 *    3.    The provided auDescBlock will be divided into AU descriptors. The
 *          size of each AU descriptor can be obtained via coreGetAuDescSize(),
 *          The number of address update descriptors is calculated by:
 *          (auDescBlockSize / <size of a single descriptor> ).
 *
 *
 *    4.    The number of Rx descriptors to be allocated per Rx queue will be
 *          calculated as follows:
 *              -   if <allocation method> = STATIC_ALLOC, then the number of
 *                  Rx descs. per Rx queue will be:
 *                  (rxBufBlockSize / rxBufSize) / <number of Rx queues>
 *                  while The Rx buffer size must be a multiple of 8.
 *                  In this case the user must take care to provide an Rx Desc.
 *                  block which is large enough to allocate an Rx desc. per
 *                  allocated Rx buffer.
 *              -   if <allocation method> = DYNAMIC_ALLOC, then the number of
 *                  Rx descriptors per queue will be:
 *                  (rxDescBlockSize / <size of a single Rx desc.>) /
 *                  <number of Rx queues>.
 *                  The size of a single Rx descriptor can be obtained by
 *                  calling coreGetRxDescSize().
 *          rxBufSize in the above calculations is received via TAPI
 *          sysConfig.h sysConfig() API.
 *
 *    5.    The number of Tx descriptors to be allocated per Tx queue will be
 *          calculated as follows:
 *          (txDescBlockSize / <size of a single Tx desc.>) /
 *          <number of Rx queues>.
 *          The size of a single Tx descriptor can be obtained by calling
 *          coreGetTxDescSize().
 *    6.    The provided auDescBlock for FDB Upload (FU) messages will be divided
 *          into FU descriptors. The size of each FU descriptor is same as AU one
 *          and can be obtained via coreGetAuDescSize(). The number of
 *          FU descriptors is calculated by:
 *          (auDescBlockSize / <size of a single descriptor> ).
 *
 */
typedef struct
{
    GT_U8                   devNum;
    GT_UINTPTR              baseAddr;
    CPSS_PP_DEVICE_TYPE     deviceId;
    CPSS_NET_IF_CFG_STC     netIfCfg;
    CPSS_AUQ_CFG_STC        auqCfg;
    GT_U8                   hwDevNum;
    CPSS_AUQ_CFG_STC        fuqCfg;
    GT_BOOL                 fuqUseSeparate;
    GT_U32                  rxBufSize;
    GT_U32                  headerOffset;
    GT_U32                  netifSdmaPortGroupId;
}CPSS_PP_PHASE2_INIT_PARAMS;

/*
 * typedef: struct CPSS_PP_CONFIG_FDB_STC
 *
 * Description: Packet Processor (PP) initialization configuration
 *              Parameters structure --> relevant to the FDB .
 *
 * Fields:
 *      fdbTableMode - FDB table location(internal(shared/split) or external memory)
 *      fdbToLutRatio - the FDB to LUT num of entries ratio.
 *                   NOTE : Relevant only when fdbTableMode = "external memory"
 *      fdbHashSize - the FDB hash size.
 *                   NOTE : Always Relevant : "external memory" , "internal memory"
 *
 */
typedef struct
{
    GT_U32    fdbTableMode; /* one of CPSS_EXMXPM_PP_CONFIG_FDB_AND_ARP_TABLE_MODE_ENT    */
    GT_U32    fdbToLutRatio;/* one of CPSS_EXMXPM_PP_CONFIG_FDB_TO_LUT_RATIO_ENT          */
    GT_U32    fdbHashSize;  /* one of CPSS_EXMXPM_PP_CONFIG_FDB_HASH_SIZE_ENT             */
}CPSS_PP_CONFIG_FDB_STC;

 /*
 * typedef: struct APP_DEMO_CPSS_PP_CONFIG_EXTERNAL_TCAM_STC
 *
 * Description: Packet Processor (PP) initialization configuration
 *              Parameters structure --> relevant to the external TCAM .
 *
 * Fields:
 *
 */
typedef struct
{
    GT_U32  actionLinesInMemoryControl0;
    GT_U32  actionLinesInMemoryControl1;
    void *extTcamRuleWriteFuncPtr;
    void *extTcamRuleReadFuncPtr;
    void *extTcamActionWriteFuncPtr;
    void *extTcamActionReadFuncPtr;
}APP_DEMO_CPSS_PP_CONFIG_EXTERNAL_TCAM_STC;

/*
 * typedef: enum APP_DEMO_CPSS_LPM_MEM_CFG_ENT
 *
 * Description: Defines the routing lpm memory configuration.
 *
 * Enumerations:
 *
 *  ------ configurations for ExMx devices ---------------
 *
 *  APP_DEMO_CPSS_EXMX_TWIST_INTERNAL_E -
 *          Internal memory for TWIST type asic - backward compatibility
 *
 *  APP_DEMO_CPSS_EXMX_TWIST_EXTERNAL_E -
 *          Only external memory for TWIST type asic - backward compatibility
 *
 *  APP_DEMO_CPSS_EXMX_TWO_EXTERNAL_250MHZ_E -
 *          Two external memories that work in 250 MHz and therefore in order
 *          to support wire speed the internal memories are used as well.
 *
 *  APP_DEMO_CPSS_EXMX_TWO_EXTERNAL_300MHZ_E -
 *          Two external narrow srams each working in 300MHz; It supports IPv4
 *          and IPv6 routing in full wire speed.
 *
 *  APP_DEMO_CPSS_EXMX_DROP_IN_CAPACITY_E -
 *          Support taking the Tiger as a drop in to TwistD boards. With
 *          emphasis on capacity (more IPv4 mc groups, less wire speed)
 *
 *  APP_DEMO_CPSS_EXMX_DROP_IN_SPEED_E -
 *          Support taking the Tiger as a drop in to TwistD boards. With
 *          emphasis on speed (more wire speed, less IPv4 mc groups)
 *
 *  APP_DEMO_CPSS_EXMX_TWO_INTERNAL_E -
 *          Two internal narrow srams 32Kbyte each working 300 MHz. It
 *          supports IPv4 and IPv6 routing in full wire speed.
 *
 *  APP_DEMO_CPSS_EXMX_ONE_INTERNAL_E -
 *          One 64 Kbyte internal narrow sram for IPv4 only backward compatible mode.
 *
 *  APP_DEMO_CPSS_EXMX_NRAM_NOT_EXISTS_E -
 *          No nram configuration
 *
 *  ----- configurations for ExMxMx devices --------------
 *
 *  APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E -
 *          LPM algorithm uses internal tables
 *
 *  APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E -
 *          LPM algorithm uses three external tables
 *
 *  APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_EXTERNAL_E -
 *          LPM algorithm uses a single external memory. The other three
 *          tables are internal.
 *
 *  APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_INTERNAL_E -
 *          LPM algorithm uses a single internal memory. The other three
 *          tables are external.
 *
 *  APP_DEMO_CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E -
 *          Invalid LPM SRAM configuration
 *
 */
typedef enum
{
    APP_DEMO_CPSS_EXMX_TWIST_INTERNAL_E,
    APP_DEMO_CPSS_EXMX_TWIST_EXTERNAL_E,
    APP_DEMO_CPSS_EXMX_TWO_EXTERNAL_250MHZ_E,
    APP_DEMO_CPSS_EXMX_TWO_EXTERNAL_300MHZ_E,
    APP_DEMO_CPSS_EXMX_DROP_IN_CAPACITY_E,
    APP_DEMO_CPSS_EXMX_DROP_IN_SPEED_E,
    APP_DEMO_CPSS_EXMX_TWO_INTERNAL_E,
    APP_DEMO_CPSS_EXMX_ONE_INTERNAL_E,
    APP_DEMO_CPSS_EXMX_NRAM_NOT_EXISTS_E,
    APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E,
    APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E,
    APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_EXTERNAL_E,
    APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_INTERNAL_E,
    APP_DEMO_CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E

} APP_DEMO_CPSS_LPM_MEM_CFG_ENT;

/*
 * typedef: struct APP_DEMO_CPSS_IP_MEM_CFG_STC
 *
 * Description:
 *      This struct hold all the info needed to config a specific IP memory
 *      configuarion as part of LPM DB.
 *
 * Fields:
 *    routingSramCfgType- Determines what's the routing memory configuration.
 *    sramsSizeArray    - The External Routing Sram sizes (aka Narrow Sram).
 *    numOfSramsSizes   - Number of valid entries in sramsSizeArray.
 */
typedef struct APP_DEMO_CPSS_IP_MEM_CFG_STCT
{
    APP_DEMO_CPSS_LPM_MEM_CFG_ENT   routingSramCfgType;
    CPSS_SRAM_SIZE_ENT              sramsSizeArray[APP_DEMO_CPSS_MAX_NUMBER_OF_SRAMS_CNS];
    GT_U32                          numOfSramsSizes;
}APP_DEMO_CPSS_IP_MEM_CFG_STC;

/*
 * typedef: enum APP_DEMO_CPSS_DIT_MEMORY_MODE_ENT
 *
 * Description: Defines DIT memory mode
 *
 * Enumerations:
 *  APP_DEMO_CPSS_DIT_SHARED_MEM_E
 *        - all different DIT types share the same memory (meaning same
 *          base address and same size)
 *
 *  APP_DEMO_CPSS_DIT_SEPARATE_MEM_E
 *        - each DIT type has separate memory block; this mode is supported
 *          only on external memory
 *
 */
typedef enum
{
    APP_DEMO_CPSS_DIT_SHARED_MEM_E,
    APP_DEMO_CPSS_DIT_SEPARATE_MEM_E

} APP_DEMO_CPSS_DIT_MEMORY_MODE_ENT;

/*
 * typedef: enum APP_DEMO_CPSS_DIT_IP_MC_MODE_ENT
 *
 * Description: Defines IP MC DIT type mode
 *
 * Enumerations:
 *  APP_DEMO_CPSS_DIT_IP_MC_DUAL_MODE_E
 *        - Each DIT table line contains two, reduced format, IP MC entries
 *
 *  APP_DEMO_CPSS_DIT_IP_MC_SINGLE_MODE_E
 *        - IP MC entry has an extended format and uses a full line of the DIT table
 *
 */
typedef enum
{
    APP_DEMO_CPSS_DIT_IP_MC_DUAL_MODE_E,
    APP_DEMO_CPSS_DIT_IP_MC_SINGLE_MODE_E

} APP_DEMO_CPSS_DIT_IP_MC_MODE_ENT;

/*
 * typedef: enum  APP_DEMO_CPSS_INLIF_PORT_MODE_ENT
 *
 * Description: Port InLIF Mode enumeration
 *
 * Fields:
 *       APP_DEMO_CPSS_INLIF_PORT_MODE_PORT_E - The InLIF-ID is set to be equal
 *                          to the per-port configured default InLIF-ID.
 *       APP_DEMO_CPSS_INLIF_PORT_MODE_VLAN_E - The InLIF-ID is set to be equal
 *                          to the VLAN-ID.
 */
typedef enum
{
    APP_DEMO_CPSS_INLIF_PORT_MODE_PORT_E,
    APP_DEMO_CPSS_INLIF_PORT_MODE_VLAN_E
} APP_DEMO_CPSS_INLIF_PORT_MODE_ENT;

/*
 * typedef: struct APP_DEMO_CPSS_DXCH_ROUTING_MODE_ENT
 *
 * Description: Routing mode
 *
 * Enumerations:
 *  APP_DEMO_CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E  - PBR (Policy based routing)
 *                                                    using PCL action as next hop.
 *  APP_DEMO_CPSS_DXCH_TCAM_ROUTER_BASED_E - Regular routing using router engine
 *                                           or PBR using next hop table.
*/
typedef enum
{
    APP_DEMO_CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E = 0,
    APP_DEMO_CPSS_DXCH_TCAM_ROUTER_BASED_E         = 1
}APP_DEMO_CPSS_DXCH_ROUTING_MODE_ENT;

/****************************************************************************************************/
/*
 * typedef: struct CPSS_PP_CONFIG_INIT_STC
 *
 * Description: System configuration Parameters struct.
 *
 * Fields:
 *    devNum           - The Prestera device number to assign to
 *                                this PP.  Range is 0-127.
 *    maxNumOfInlifs   - max number of available input logical interfaces
 *                        on the PP's Sram memory.
 *
 *                        NOTE: setting "maxNumOfInlifs" as 0, means no InLIf's
 *                              SRAM allocation.
 *                              In this case, the only possible working mode is
 *                              "inlif per port" , so the API's for "inlif per
 *                              vlan" are not valid.
 *
 *    inlifPortMode          - default inlif mode (per port / per vlan).
 *
 *    maxNumOfPolicerEntries - max number of Policer entries in the
 *                        device.
 *    policerConformCountEn  - When enabled, the Policer Entry contains
 *                             conformance counters (i.e. green, yellow, red
 *                             counters), increasing the size of the Policer
 *                             Entry. If conformance counters are not required,
 *                             the entry size is reduced.
 *                             (Allowing 50% more Policer Entries).
 *
 *
 *    maxNumOfNhlfe     - max number of NHLFE entries in the PP :
 *                        includes 16 reserved entries.
 *    maxNumOfMplsIfs   - max number of Mpls interfaces in PP.
 *
 *
 *    maxNumOfPclAction - max number of PCL action in PP.
 *    pclActionSize     - defines size (8/16 bytes)of PCL action entry
 *                        in the Control SRAM
 *
 *    maxVid                 - max VLAN ID in the device.
 *    maxMcGroups            - max number of Multicast group need to be
 *                             supported.
 *
 *    maxNumOfVirtualRouters - max numbers of IPv4 virtual routers.
 *    maxNumOfIpNextHop      - max number of IPv4 and Ipv6 next hop entries in the
 *                             PP.
 *    maxNumOfIpv4Prefixes   - max number of IPv4 prefixes in the PP.
 *                             on TCAM based PP with LPM DB, this value is
 *                             the number of IPv4 prefixes the LPM DB supports
 *                             (refer to LPM DB creation for more details)
 *    maxNumOfIpv4McEntries  - max number of IPv4 and IPv6 Multicast Entries in the PP.
 *                             on TCAM based PP with LPM DB, this value is
 *                             the number of IPv4 multicast sources the LPM DB supports
 *                             (refer to LPM DB creation for more details)
 *    maxNumOfMll            - max number of Downstream IPv4 and Ipv6 Multicast
 *                             interface entries (MLLs) in the PP.
 *    maxNumOfIpv6Prefixes   - max number of IPv6 prefixes in the PP.
 *                             on TCAM based PP with LPM DB, this value is
 *                             the number of IPv6 prefixes the LPM DB supports
 *                             (refer to LPM DB creation for more details)
 *    maxNumOfIpv6McGroups   - max number of IPv6 Multicast groups.
 *    maxNumOfPceForIpPrefixes- max number of Pcl entries reserved for IP
 *                              prefixes in the Pcl Tcam.
 *    usePolicyBasedRouting  - whether to use policy based routing
 *    usePolicyBasedDefaultMc- whether to set default rules to match all MC traffic
 *                             when working in policy based routing mode; for details
 *                             refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
 *
 *
 *    maxNumOfTunnelEntries  - max number of tunnel (of all types) entries
 *    maxNumOfIpv4TunnelTerms - the maximum number of ipv4 tunnel termination
 *                                  entries
 *
 *    maxNumOfTunnelTerm     - number of tunnel termination entries (or tunnel
 *                             termination interface entries in case of Cheetah3)
 *
 *    vlanInlifEntryType     - Determines the type of the vlan inlif entry
 *                             (128 bits long or 42 bits long).
 *                             1. GT_VLAN_INLIF_ENTRY_TYPE_REGULAR_E: It should be
 *                                used when working in:
 *                                    A.  INLIF_PER_LPORT_MODE.
 *                                    B.INLIF_PER_VLAN_MODE.
 *                                    C.INLIF_MIXED_MODE
 *                             2. GT_VLAN_INLIF_ENTRY_TYPE_REDUCED_E: It enables
 *                                using around 1/3 of the memory needed for vlan
 *                                regular inlif entries. It should be used when
 *                                working in:
 *                                    A. INLIF_MIXED_MODE
 *
 *    ipMemDivisionOn        - if GT_FALSE, no mem devision between IPv4 and IPv6 takes place
 *    routingMode            - PP routing mode: Policy based routing Or TCAM based Routing
 *    ipv6MemShare           - relevant only if ipMemDivisionOn is true, percentage
 *                             of ip memory reserved for IPv6 use.
 *
 *    numOfTrunks            - Number of trunks.
 *
 *    defIpv6McPclRuleIndex  - PCL rule index for default IPv6 MC
 *    vrIpv6McPclId          - PCL ID to use for IPv6 MC
 *
 *    lpmDbPartitionEnable   - whether static/dynamic partition of TCAM based LPM DB
 *    lpmDbFirstTcamLine     - first TCAM line allocated for TCAM based LPM DB
 *    lpmDbLastTcamLine      - last TCAM line allocated for TCAM based LPM DB
 *    lpmDbSupportIpv4       - whether the LPM DB supports IPv4
 *    lpmDbSupportIpv6       - whether the LPM DB supports IPv6
 *
 *    lpmDbNumOfMemCfg       - number of memory configuration supported by the lpm db
 *    lpmDbMemCfgArray       - array of memory configuration supported by the lpm db
 *
 *    ditMemoryMode               - The DIT memory configuration (shared memory
 *                                  or separate memory)
 *    ditIpMcMode                 - IP MC DIT type mode (dual or single)
 *    maxNumOfDitUcIpEntries      - Number of dit (downstream interface table)
 *                                  entries needed for UC IP.
 *    maxNumOfDitUcMplsEntries    - Number of dit (downstream interface table)
 *                                  entries needed for UC MPLS.
 *    maxNumOfDitUcOtherEntries   - Number of dit (downstream interface table)
 *                                  entries needed for UC other than IP or MPLS.
 *    maxNumOfDitMcIpEntries      - Number of dit (downstream interface table)
 *                                  entries needed for MC IP.
 *    maxNumOfDitMcMplsEntries    - Number of dit (downstream interface table)
 *                                  entries needed for MC MPLS.
 *    maxNumOfDitMcOtherEntries   - Number of dit (downstream interface table)
 *                                  entries needed for MC other than IP or MPLS.
 *
 *    fdbTableInfo           - FDB table info.
 *
 *    miiNumOfTxDesc              - Number of requested TX descriptors.
 *    miiTxInternalBufBlockSize   - The raw size in bytes of host memory to be used
 *                                  for internal TX buffers.
 *    miiBufferPercentage         - A table (entry per queue) that describes the buffer
 *                                  dispersal among all Rx queues.
 *    miiRxBufSize                - The size of the Rx data buffer.
 *    miiHeaderOffset             - Packet header offset size.
 *    miiRxBufBlockSize           - The raw size in byte of memory to be used for
 *                                  allocating Rx packet data buffers.
 *
 *    COMMENTS:
 *          The maximum of the (VLAN table size + maximum number of MAC
 *          multicast entries) is specific to each packet processor type.
 *          Please refer to the specific packet process datasheet for details.
 *
 *          The actual number of tunnels that can be configured is a trade of between
 *          the number of mc groups, the number of vlan entries and the number of tunnels.
 *          See the particular packet processor datasheet for details on the
 *          relationship between maxVid, maxMcGroups and the number of Tunnel
 *          entries available.
 */
typedef struct
{
    /* Input Logical interfaces        */
    GT_U32          maxNumOfLifs;

    APP_DEMO_CPSS_INLIF_PORT_MODE_ENT inlifPortMode;

    /* Policer */
    GT_U32          maxNumOfPolicerEntries;
    GT_BOOL         policerConformCountEn;

    /* MPLS    */
    GT_U32          maxNumOfNhlfe;
    GT_U32          maxNumOfMplsIfs;

    /* PCL     */
    GT_U32          maxNumOfPclAction;
    GT_U32          pclActionSize;

    /* Policy-based routing  */
    GT_U32          maxNumOfPceForIpPrefixes;
    GT_BOOL         usePolicyBasedRouting;
    GT_BOOL         usePolicyBasedDefaultMc;

    /* Bridge           */
    GT_U16          maxVid;
    GT_U16          maxMcGroups;

    /* IP Unicast     */
    GT_U32          maxNumOfVirtualRouters;
    GT_U32          maxNumOfIpNextHop;

    /* IPv4 Unicast     */
    GT_U32          maxNumOfIpv4Prefixes;

    /* IPv4/IPv6 Multicast   */
    GT_U32          maxNumOfIpv4McEntries;
    GT_U32          maxNumOfMll;

    /* IPv6 Unicast     */
    GT_U32          maxNumOfIpv6Prefixes;
    /* IPv6 Multicast   */
    GT_U32          maxNumOfIpv6McGroups;


    /* Tunnels */
    GT_U32          maxNumOfTunnelEntries;
    GT_U32          maxNumOfIpv4TunnelTerms;

    /* tunnel termination (of all types) / TTI entries - for Dx devices */
    GT_U32          maxNumOfTunnelTerm;

    /* inlifs */
    GT_U32          vlanInlifEntryType; /* Will be assigned with the correct
                                           enumeration in the family specific function.*/

    /* if GT_FALSE - no division of memory pools for the use of different IP versions */
    GT_BOOL         ipMemDivisionOn;

    /* IP routing mode*/
    APP_DEMO_CPSS_DXCH_ROUTING_MODE_ENT routingMode;

    /* devision of memory between IPv4 and IPv6 prefixes*/
    GT_U32          ipv6MemShare;

    /* Number of trunks */
    GT_U32          numOfTrunks;

    /* PCL rule index and PCL ID for defualt IPv6 MC entry for devices */
    /* where IPv6 MC group search is implemented in PCL                */
    GT_U32          defIpv6McPclRuleIndex;
    GT_U32          vrIpv6McPclId;

    /* TCAM LPM DB configurations */
    GT_BOOL         lpmDbPartitionEnable;
    GT_U32          lpmDbFirstTcamLine;
    GT_U32          lpmDbLastTcamLine;
    GT_BOOL         lpmDbSupportIpv4;
    GT_BOOL         lpmDbSupportIpv6;

    /* number of memory configurations supported by the lpm db */
    GT_U32          lpmDbNumOfMemCfg;

    /* array of memory configurations supported by the lpm db */
    APP_DEMO_CPSS_IP_MEM_CFG_STC    *lpmDbMemCfgArray;

    /* DIT configuration */
    APP_DEMO_CPSS_DIT_MEMORY_MODE_ENT   ditMemoryMode;
    APP_DEMO_CPSS_DIT_IP_MC_MODE_ENT    ditIpMcMode;
    GT_U32                              maxNumOfDitUcIpEntries;
    GT_U32                              maxNumOfDitUcMplsEntries;
    GT_U32                              maxNumOfDitUcOtherEntries;
    GT_U32                              maxNumOfDitMcIpEntries;
    GT_U32                              maxNumOfDitMcMplsEntries;
    GT_U32                              maxNumOfDitMcOtherEntries;

    /*********************/
    /*start of Puma info */
    /*********************/
    CPSS_PP_CONFIG_FDB_STC   fdbTableInfo;

    APP_DEMO_CPSS_PP_CONFIG_EXTERNAL_TCAM_STC  externalTcamInfo;

    /*******************/
    /*end of Puma info */
    /*******************/

    /* MII */
    GT_U32       miiNumOfTxDesc;
    GT_U32       miiTxInternalBufBlockSize;
    GT_U32       miiBufferPercentage[CPSS_MAX_RX_QUEUE_CNS];
    GT_U32       miiRxBufSize;
    GT_U32       miiHeaderOffset;
    GT_U32       miiRxBufBlockSize;

} CPSS_PP_CONFIG_INIT_STC;



/****************************************************************************************************/

/*
 * Typedef: struct APP_DEMO_LIB_INIT_PARAMS
 *
 * Description: Includes tapi libraries initialization parameters.
 *
 * Fields:
 *
 */
typedef struct
{
    GT_BOOL                 initBridge;
    GT_BOOL                 initClassifier;
    GT_BOOL                 initCos;
    GT_BOOL                 initIpv4;
    GT_BOOL                 initIpv6;
    GT_BOOL                 initIpv4Filter;
    GT_BOOL                 initIpv4Tunnel;
    GT_BOOL                 initLif;
    GT_BOOL                 initMirror;
    GT_BOOL                 initMpls;
    GT_BOOL                 initMplsTunnel;
    GT_BOOL                 initNetworkIf;
    GT_BOOL                 initI2c;
    GT_BOOL                 initPhy;
    GT_BOOL                 initPort;
    GT_BOOL                 initTc;
    GT_BOOL                 initTrunk;
    GT_BOOL                 initPcl;
    GT_BOOL                 initPolicer;
    GT_BOOL                 initAction;
    GT_BOOL                 initSflow;
    GT_BOOL                 initNst;
}APP_DEMO_LIB_INIT_PARAMS;


/*
 * Typedef: FUNCP_CPSS_PP_LOGICAL_INIT
 *
 * Description:
 *      Implements logical initialization for specific device.
 *      Device number is a part of system configuration parameters.
 *
 * Fields:
 *      devNum               -   device number
 *      logicalConfigParams  -   PP logical configuration parameters
 */
typedef GT_STATUS (*FUNCP_CPSS_PP_LOGICAL_INIT)
(
   IN  GT_U8                        devNum,
   IN  CPSS_PP_CONFIG_INIT_STC     *logicalConfigParams
);

/*
 * Typedef: FUNCP_CPSS_LIBRARIES_INIT
 *
 * Description:
 *      Initialize all CPSS libraries for specific device.
 *      It's entry point where all required CPSS libraries are initialized.
 *
 * Fields:
 *      dev                 -   device number
 *      libInitParamsPtr    -   initialization parameters
 *      sysConfigParamPtr   -   system configuration parameters
 */
typedef GT_STATUS (*FUNCP_CPSS_LIBRARIES_INIT)
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
);

/*
 * Typedef: FUNCP_CPSS_NETWORK_IF_INIT
 *
 * Description:
 *      Initialize network interface for specific device.
 *
 * Fields:
 *      dev     -   device number
 */
typedef GT_STATUS (*FUNCP_CPSS_NETWORK_IF_INIT)
(
    IN  GT_U8   dev
);

/*
 * Typedef: FUNCP_CPSS_TRAFFIC_ENABLE
 *
 * Description:
 *      Enable traffic for specific device.
 *      Enable ports and device.
 *
 * Fields:
 *      dev     -   device number
 */
typedef GT_STATUS (*FUNCP_CPSS_TRAFFIC_ENABLE)
(
    IN  GT_U8   dev
);

/*
 * Typedef: FUNCP_CPSS_HW_PP_PHASE1_INIT
 *
 * Description:
 *      Implements HW phase 1 initialization.
 *
 * Fields:
 *      dev                         -   temp device id, could be changed later
 *      appCpssPpPhase1ParamsPtr    -   PP phase 1 init parameters
 *      ppDeviceTypePtr             -   output correct device type
 */
typedef GT_STATUS (*FUNCP_CPSS_HW_PP_PHASE1_INIT)
(
    IN  GT_U8                       dev,
    IN  CPSS_PP_PHASE1_INIT_PARAMS  *appCpssPpPhase1ParamsPtr,
    OUT CPSS_PP_DEVICE_TYPE         *ppDevTypePtr
);

/*
 * Typedef: FUNCP_CPSS_HW_PP_START_INIT
 *
 * Description:
 *      Implements PP start initialization.
 *
 * Fields:
 *      dev             -   device number
 *      fReloadEeprom   -   reload Eeprom flag
 *      initDataListPtr -   list of registers to init
 *      initDataListLen -   length of registers' list
 */
typedef GT_STATUS (*FUNCP_CPSS_HW_PP_START_INIT)
(
    IN  GT_U8                       dev,
    IN  GT_BOOL                     fReloadEeprom,
    IN  CPSS_REG_VALUE_INFO_STC     *initDataListPtr,
    IN  GT_U32                      initDataListLen
);

/*
 * Typedef: FUNCP_CPSS_HW_PP_PHASE2_INIT
 *
 * Description:
 *      Implements HW phase 2 initialization.
 *
 * Fields:
 *      oldDev                      -   old device id, used for phase 1
 *      appCpssPpPhase2ParamsPtr    -   PP phase 2 init parameters
 */
typedef GT_STATUS (*FUNCP_CPSS_HW_PP_PHASE2_INIT)
(
    IN  GT_U8                       oldDev,
    IN  CPSS_PP_PHASE2_INIT_PARAMS  *appCpssPpPhase2ParamsPtr
);

/*
 * Typedef: FUNCP_CPSS_PP_GENERAL_INIT
 *
 * Description:
 *      Implements general initialization for specific device.
 *
 * Fields:
 *      dev                  -   device id
 */
typedef GT_STATUS (*FUNCP_CPSS_PP_GENERAL_INIT)
(
    IN  GT_U8   dev
);

/*
 * Typedef: struct APP_DEMO_SYS_CONFIG_FUNCS
 *
 * Description:
 *      Contains functions for general CPSS init flow.
 *
 * Fields:
 *      cpssHwPpPhase1Init  -   HW phase 1 init
 *      cpssHwPpStartInit   -   PP start init
 *      cpssHwPpPhase2Init  -   HW phase 2 init
 *      cpssLibrariesInit   -   All CPSS libraries init
 *      cpssPpLogicalInit   -   Logical init for device
 *      cpssNetworkIfInit   -   network interface init
 *      cpssTrafficEnable   -   enable ports and devices
 *      cpssPpGeneralInit   -   General initialization
 *
 * Comments:
 *      Since there is more than API could be used this
 *      structure is used to provide general initialization
 *      flow. In order to extend general initialization flow
 *      new function should be added here and implemented for
 *      supported API - ExMx or Dx.
 */
typedef struct
{
    FUNCP_CPSS_HW_PP_PHASE1_INIT    cpssHwPpPhase1Init;
    FUNCP_CPSS_HW_PP_START_INIT     cpssHwPpStartInit;
    FUNCP_CPSS_HW_PP_PHASE2_INIT    cpssHwPpPhase2Init;
    FUNCP_CPSS_PP_LOGICAL_INIT      cpssPpLogicalInit;
    FUNCP_CPSS_LIBRARIES_INIT       cpssLibrariesInit;
    FUNCP_CPSS_TRAFFIC_ENABLE       cpssTrafficEnable;
    FUNCP_CPSS_PP_GENERAL_INIT      cpssPpGeneralInit;

} APP_DEMO_SYS_CONFIG_FUNCS;

/*******************************************************************************
* APP_DEMO_SYS_CONFIG_FUNCS_EXTENTION_GET_FUNC
*
* DESCRIPTION:
*       Gets the system configuration functions accordingly to given device type.
*       allow override of previos setting done by function appDemoSysConfigFuncsGet
*
* INPUTS:
*       deviceType  - device type.
*
* OUTPUTS:
*       sysConfigFuncsPtr  - system configuration functions.
*       apiSupportedBmpPtr - (pointer to)bmp of supported type of API to call
*
* RETURNS:
*       GT_OK           -   on success
*       GT_BAD_PTR      -   bad pointer
*       GT_NOT_FOUND    -   device type is unknown
*       GT_FAIL         -   on failure
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*APP_DEMO_SYS_CONFIG_FUNCS_EXTENTION_GET_FUNC)
(
    IN  GT_U32                      deviceType,
    OUT APP_DEMO_SYS_CONFIG_FUNCS   *sysConfigFuncsPtr,
    OUT GT_U32                      *apiSupportedBmpPtr
);

/* pointer to the function that allow override of setting done by appDemoSysConfigFuncsGet */
extern APP_DEMO_SYS_CONFIG_FUNCS_EXTENTION_GET_FUNC appDemoSysConfigFuncsExtentionGetPtr;

/* XG and Cascade ports support*/
/*******************************************************************************
* internalPortConfig
*
* DESCRIPTION:
*       Internal function performs board-specific configurations of 10G
*       or cascade ports.
*
* INPUTS:
*       devNum     - Device Id.
*       portNum    - Port Number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*FUNCP_PORT_CONFIG)
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum
);

typedef struct
{
    CPSS_CSCD_PORT_TYPE_ENT     cscdPortType;
    GT_TRUNK_ID                 trunkId;
    GT_U8                       portNum;
} CSCD_INIT_PORT_DESC_STC;

typedef struct
{
    CPSS_CSCD_LINK_TYPE_STC   linkToTargetDev;
    GT_U8                     targetDevNum;
} CSCD_INIT_UC_TBL_STC;

/*******************************************************************************
* APP_DEMO_ETH_PRT_INIT_NETIF_FUNC
*
* DESCRIPTION:
*       Initialize the network interface structures, Rx buffers and Tx header
*       buffers (For a single device).
*
* INPUTS:
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*APP_DEMO_ETH_PRT_INIT_NETIF_FUNC)
(
    void
);

/* Cascading support - end */

/* used by apiSupportedBmp */
#define APP_DEMO_EXMX_FUNCTIONS_SUPPORT_CNS         BIT_0
#define APP_DEMO_EXMXTG_FUNCTIONS_SUPPORT_CNS       BIT_1
#define APP_DEMO_DXCH_FUNCTIONS_SUPPORT_CNS         BIT_2
#define APP_DEMO_DXCH2_FUNCTIONS_SUPPORT_CNS        BIT_3
#define APP_DEMO_DXSAL_FUNCTIONS_SUPPORT_CNS        BIT_4
#define APP_DEMO_EXMXPM_FUNCTIONS_SUPPORT_CNS       BIT_5
#define APP_DEMO_DXCH3_FUNCTIONS_SUPPORT_CNS        BIT_6
#define APP_DEMO_DXCH_XCAT_FUNCTIONS_SUPPORT_CNS    BIT_7
#define APP_DEMO_DXCH_LION_FUNCTIONS_SUPPORT_CNS    BIT_8

#define API_SUPPORTED_BMP_MAC(devNum)\
    appDemoPpConfigList[devNum].apiSupportedBmp

/* check if device support API of exmx */
#define IS_API_EXMX_DEV_MAC(devNum) \
    (API_SUPPORTED_BMP_MAC(devNum) & APP_DEMO_EXMX_FUNCTIONS_SUPPORT_CNS)

/* check if device support API of exmxTg */
#define IS_API_TG_DEV_MAC(devNum) \
    (API_SUPPORTED_BMP_MAC(devNum) & APP_DEMO_EXMXTG_FUNCTIONS_SUPPORT_CNS)

/* check if device support API of dxch */
#define IS_API_CH_DEV_MAC(devNum) \
    (API_SUPPORTED_BMP_MAC(devNum) & APP_DEMO_DXCH_FUNCTIONS_SUPPORT_CNS)

/* check if device support API of dxch2 */
#define IS_API_CH2_DEV_MAC(devNum) \
    (API_SUPPORTED_BMP_MAC(devNum) & APP_DEMO_DXCH2_FUNCTIONS_SUPPORT_CNS)

/* check if device support API of dxch3 */
#define IS_API_CH3_DEV_MAC(devNum) \
    (API_SUPPORTED_BMP_MAC(devNum) & APP_DEMO_DXCH3_FUNCTIONS_SUPPORT_CNS)

/* check if device support API of dxsal */
#define IS_API_SAL_DEV_MAC(devNum) \
    (API_SUPPORTED_BMP_MAC(devNum) & APP_DEMO_SAL_FUNCTIONS_SUPPORT_CNS)

/* check if device support API of exmx */
#define IS_API_EXMXPM_DEV_MAC(devNum) \
    (API_SUPPORTED_BMP_MAC(devNum) & APP_DEMO_EXMXPM_FUNCTIONS_SUPPORT_CNS)

struct APP_DEMO_EXTERNAL_TCAM_KEY_LOOKUP_STCT;

/*
 * typedef: enum CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_G003_ENT
 *
 * Description: Enumeration of the FDB modes
 *
 * Enumerations:
 *    APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E  - all port groups have the
 *          same FDB entries. (unified tables)
 *    APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKE_E  - the port groups of
 *          the device are split to 2 groups .
 *          Each port group in those groups may hold different FDB entries.
 *          But the 2 groups hold the same entries.
 *    APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_LINKED_E - each port group of
 *          the device may hold different FDB entries.
 */
typedef enum{
    APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E,
    APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E,
    APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_LINKED_E
}APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_ENT;


/*
 * typedef: struct APP_DEMO_CPSS_HW_ACCESS_DB_STC
 *
 * Description:
 *      This struct defines HW Access parameters and holds the data.
 *
 * Fields:
 *    accessParamsBmp   - Holds the follwing access parameters in a bitmap:
 *                        byte[0]: device number
 *                        byte[1]: port group Id.
 *                        byte[2]: 
 *                          bit0: ISR Context (1 - callback called from ISR context,
 *                                             0 - callback called not from ISR)
 *                          bit1: PCI PEX Space (1 - callback called for PCI/PEX registers address,
 *                                               0 - callback called not for PCI/PEX registers address)
 *                        byte[3]:
 *                          bit0: Is the access for memory (1 - the access is for Memory,
 *                                                          0 - the access is for Register)
 *                                Note: Valid in case of bit0 is 1.
 *                          bit1: Memory end flag (1 - this access entry is memory end,
 *                                                 0 - this access entry is not memory end)
 *      addr            - HW access address
 *      data            - The HW access read/written data
 *
 */
typedef struct
{
    GT_U32      accessParamsBmp;
    GT_U32      addr;
    GT_U32      data;
}APP_DEMO_CPSS_HW_ACCESS_DB_STC;


/*
 * typedef: struct APP_DEMO_CPSS_HW_ACCESS_INFO_STC
 *
 * Description:
 *      This struct defines HW Access Info.
 *
 * Fields:
 *    maxSize -   The Max size allocated for HW Access DB in words.
 *    curSize -   Current HW Access DB size in words 
 *    curState -  Current DB state. 
 *                  GT_TRUE: trace HW Access enabled
 *                  GT_FALSE: trace HW Access disabled
 */
typedef struct
{
    GT_U32      maxSize;
    GT_U32      curSize;
    GT_BOOL     curState;
}APP_DEMO_CPSS_HW_ACCESS_INFO_STC;


/*
 * Typedef: struct APP_DEMO_PP_CONFIG
 *
 * Description: Holds Pp configuration parameters for initialization.
 *
 * Fields:
 *      deviceId            -  HW device ID of the PP
 *      apiSupportedBmp     - bmp of supported type of API to call
 *      ppPhase1Done        - Indicates whether the corePpHwPhase1Init() function was
 *                            already called.
 *      ppPhase2Done        - Indicates whether the corePpHwPhase2Init() function was
 *                            already called.
 *      ppLogicalInitDone   - Indicates whether the sysPpLogicalInit() function was
 *                            already called.
 *      ppGeneralInitDone   - Indicates whether the general initialization was done.
 *      channel     - smi / pci channel to the PP.
 *      pciInfo     - Device pci mapping info.
 *      smiInfo     - Device smi mapping info.
 *      twsiInfo    - Device twsi mapping info.
 *      devNum      - The device's software device number.
 *      oldDevNum   - The old device number that was assigned to the device in
 *                    phase1.
 *      hwDevNum    - The device's hardware device number.
 *      flowGroup   - The flow group to which this device belongs.
 *      intLine     - The interrupt line on which the device is connected.
 *      valid       - If this entry holds valid data.
 *      active      - Is the device active.
 *  XG support:
 *      internal10GPortConfigFuncPtr - Board-specific configuration for 10G ports
 *                                      (if not required - NULL)
 *      ports10GToConfigureArr      - Array of numbers of XG ports to configure
 *      numberOf10GPortsToConfigure - number of valid elements in ports10GToConfigureArr
 *                                      (id of the first invalid)
 *  Cascading support:
 *      numberOfCscdTrunks      - (should be lower than number of trunks supported in HW)
 *      numberOfCscdTargetDevs  -  Number of valid elements in cscdTargetDevsArr
 *                                  (id of the first invalid)
 *      numberOfCscdPorts       - Number of valid elements in cscdPortsArr
 *                                  (id of the first invalid)
 *      cscdTargetDevsArr       - Information required to fill cascade map table
 *                                  (target device numbers, link type and number)
 *      cscdPortsArr            - Cascade ports info (port numbers, cascade type, trunkId)
 *      internalCscdPortConfigFuncPtr   - board-specific additional configuration
 *                                          for cascade ports (if not required - NULL)
 *      externalTcamUsed        - indicate whether external Tcam is used.
 *      extTcamMngPtr           - (pointer to) external TCAM management database.
 *      flowControlDisable      - Defines system Flow Control settings of
 *                                Tail Drop and Buffer Management to be:
 *                                GT_TRUE  - per ingress port disable FC
 *                                           and enable HOL system mode.
 *                                GT_FALSE - per ingress port enable FC
 *                                           and enable Flow Control system mode
 *      numOfPortGroups              - The number of port-groupss in the device.
 *                                0 --> a non multi-port-groups device .
 *                                1,2..   --> multi-port-groups device with 1,2.. cores.
 *                                Set by getBoardInfo -->  set 4 for Lion
 *      portGroupsInfo[]          -   info needed per port group.
 *                                Set by getBoardInfo
 *          portGroupActive           - is the port group active
 *          portGroupPciInfo         - port group PCI info. ('per port group' PCI info)
 *                                    For non active port group set all parameters with CPSS_PARAM_NOT_USED_CNS
 *          unifiedFdbPortGroupsBmp - bitmap of port groups that need to be accessed
 *                                    when set/get fdb entry in current port group.
 *      multiPortGroupRingPortsBmp - bmp of ring ports . -- debug info --
 *                                this is needed for auto add of those ports to
 *                                vlan (used by UT enhanced and can be used by Galtis wrappers )
 *                                (relevant to multi port groups device)
 *      fdbMode                   - fdb mode - unified / linked unified / linked
 *                                  (relevant to multi port groups device)
 *      cpuPortMode         - which way CPU TX/RX goes - MII, SDMA, NONE(current ASIC isn't connected to CPU)
 *      wa - info about WA (workarounds used)
 *          trunkToCpuPortNum - do we use the trunk WA : traffic to CPU from
 *              trunk member get wrong port num indication
 *
 */
typedef struct
{
    CPSS_PP_DEVICE_TYPE             deviceId;
    GT_U32                          apiSupportedBmp;
    CPSS_PP_FAMILY_TYPE_ENT         devFamily;  /*added*/
    GT_BOOL                         ppPhase1Done;
    GT_BOOL                         ppPhase2Done;
    GT_BOOL                         ppLogicalInitDone;
    GT_BOOL                         ppGeneralInitDone;
    CPSS_PP_INTERFACE_CHANNEL_ENT   channel;
    GT_PCI_INFO                     pciInfo;
    GT_SMI_INFO                     smiInfo;
    GT_TWSI_INFO                    twsiInfo;
    GT_U8                           devNum;
    GT_U8                           oldDevNum;
    GT_U8                           hwDevNum;
    GT_PCI_INT                      intLine;
    GT_BOOL                         valid;
    APP_DEMO_SYS_CONFIG_FUNCS       sysConfigFuncs;
    /*XG support*/
    FUNCP_PORT_CONFIG               internal10GPortConfigFuncPtr;
    GT_U8                           ports10GToConfigureArr[PRV_CPSS_MAX_PP_PORTS_NUM_CNS];
    GT_U8                           numberOf10GPortsToConfigure;
    /* Cascading support */
    GT_U8                           numberOfCscdTrunks;
    GT_U8                           numberOfCscdTargetDevs;
    GT_U8                           numberOfCscdPorts;
    CSCD_INIT_UC_TBL_STC            cscdTargetDevsArr[CPSS_CSCD_MAX_DEV_IN_CSCD_CNS];
    CSCD_INIT_PORT_DESC_STC         cscdPortsArr[PRV_CPSS_MAX_PP_PORTS_NUM_CNS];
    FUNCP_PORT_CONFIG               internalCscdPortConfigFuncPtr;

    GT_BOOL                         externalTcamUsed;
    struct APP_DEMO_EXTERNAL_TCAM_KEY_LOOKUP_STCT   *extTcamMngPtr;
    GT_BOOL                         flowControlDisable;

    /* start - multi-port-groups support */
    GT_U32                          numOfPortGroups;
    struct{
        GT_BOOL                     portGroupActive;
        GT_PCI_INFO                 portGroupPciInfo;

        GT_PORT_GROUPS_BMP          unifiedFdbPortGroupsBmp;
    } portGroupsInfo[APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS];

    CPSS_PORTS_BMP_STC               multiPortGroupRingPortsBmp;/* debug info */
    APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_ENT fdbMode;
    /* end - multi-port-groups support */

    PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_ENT    cpuPortMode;

    struct{
        GT_BOOL trunkToCpuPortNum;
    }wa;
}APP_DEMO_PP_CONFIG;

extern GT_U32             appDemoPpConfigDevAmount;
extern APP_DEMO_PP_CONFIG appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];

/* dummy devNum for non initialized value for "CPU Ethernet port" device */
#define NOT_INIT_CPU_ETHER_DEV  0xff

/*
 * Typedef: struct APP_DEMO_CPU_ETHER_PORT_INFO_STC
 *
 * Description: Holds CPU Ether port configuration parameters for initialization.
 *                         relevant to system with CPU Ethernet port.
 *                         (in board with multi devices , only 1 device
 *                          connected to CPU)
 *
 * Fields:
 *      cpuEtherDevNum - the device number on which the CPU Ethernet port.
 *      rxBufInfo        - Rx buffers allocation information.
 *      txHdrBlock_PTR  - ptr to the Tx header buffer block.
 *      txHdrBlockSize  - Tx header buffer block size.
 *      initFunc        - init function
*/
typedef struct{
    GT_U8                           devNum;
    CPSS_RX_BUF_INFO_STC            rxBufInfo;
    GT_U32                          *txHdrBufBlock_PTR;
    GT_U32                          txHdrBufBlockSize;
    APP_DEMO_ETH_PRT_INIT_NETIF_FUNC    initFunc;
}APP_DEMO_CPU_ETHER_PORT_INFO_STC;

/*
 * Typedef: struct APP_DEMO_SYS_CONFIG_STC
 *
 * Description: Holds system configuration parameters for initialization.
 *
 * Fields:
 *      cpuEtherPortUsed - is the system use the cpu Ethernet port.
 *                         (in board with multi devices , only 1 device
 *                          connected to CPU)
 *      cpuEtherInfo - info about the cpu Ether port
 *                     relevant to system with CPU Ethernet port.
 *      firstDevNum - device number for the first device on board.
 *                    ability to set different device numbers for stacking
 *      supportAaMessage - the appDemo support/not support the AA messages to CPU
 *                      GT_FALSE - not support processing of AA messages
 *                      GT_TRUE - support processing of AA messages
 *      forceAutoLearn - indication that the system force to be in auto learning
*/
typedef struct{
    GT_BOOL                             cpuEtherPortUsed;
    APP_DEMO_CPU_ETHER_PORT_INFO_STC    cpuEtherInfo;
    GT_U8                               firstDevNum;
    GT_BOOL                             supportAaMessage;
    GT_BOOL                             forceAutoLearn;
}APP_DEMO_SYS_CONFIG_STC;

extern APP_DEMO_SYS_CONFIG_STC appDemoSysConfig;

/* convert local devNum got system devNum
 --> ability to set different device numbers for stacking */
#define SYSTEM_DEV_NUM_MAC(_devNum) (GT_U8)((_devNum)+appDemoSysConfig.firstDevNum)

/*
 * Typedef: FUNCP_CPSS_MODULE_INIT
 *
 * Description:
 *      Specific CPSS module initialization routine.
 *
 * Fields:
 *      dev                 -   device number
 *      libInitParamsPtr    -   module initialization parameters
 *      sysConfigParamsPtr  -   system configuration parameters
 *
 */
typedef GT_STATUS (*FUNCP_CPSS_MODULE_INIT)
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
);


/*
 * Typedef: APP_DEMO_BRG_FDB_AUQ_FUNC
 *
 * Description:
 *      prototype for function that will be called when appDemo receive FDB AUQ
 *      message
 *
 * Fields:
 *      dev              -   device number
 *      auMessagesPtr    -   pointer to the AUQ message
 *
 */
typedef GT_STATUS (*APP_DEMO_BRG_FDB_AUQ_FUNC)
(
    IN  GT_U8                       dev,
    IN  CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr
);

/*******************************************************************************
* appDemoBrgFdbAuqCbRegister
*
* DESCRIPTION:
*       register a CB (callback) function for a specific AUQ message type.
*       the registered function (cbFuncPtr) will be called for every AUQ message
*       that match the messageType.
*
* INPUTS:
*       messageType - AUQ message type
*       cbFuncPtr   - (pointer to) the CB function
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS   appDemoBrgFdbAuqCbRegister
(
    IN CPSS_UPD_MSG_TYPE_ENT       messageType,
    IN APP_DEMO_BRG_FDB_AUQ_FUNC   cbFuncPtr
);



/*******************************************************************************
* appDemoSysGetPciInfo
*
* DESCRIPTION:
*       Gets the Pci info for the mini-application usage.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoSysGetPciInfo
(
    GT_VOID
);

/*******************************************************************************
* appDemoSysGetSmiInfo
*
* DESCRIPTION:
*       Gets the SMI info for the mini-application usage.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoSysGetSmiInfo
(
    GT_VOID
);

/*******************************************************************************
* gtPresteraIsPexSwitchOnBoard
*
* DESCRIPTION:
*       This routine search for Prestera Devices Over the PCI.
*
* INPUTS:
*
* OUTPUTS:
*       pexOnBoardPtr - GT_TRUE  - pex switch is on board
*                       GT_FALSE - pex switch is absent
*
* RETURNS:
*       GT_OK      - on success.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtPresteraIsPexSwitchOnBoard
(
    OUT  GT_BOOL   *pexOnBoardPtr
);

/*******************************************************************************
* appDemoSysGetTwsiInfo
*
* DESCRIPTION:
*       Gets the SMI info for the mini-application usage.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoSysGetTwsiInfo
(
    GT_VOID
);

/*******************************************************************************
* appDemoSysConfigFuncsGet
*
* DESCRIPTION:
*       Gets the system configuration functions accordingly to given device type.
*
* INPUTS:
*       deviceType  - device type.
*
* OUTPUTS:
*       sysConfigFuncsPtr  - system configuration functions.
*       apiSupportedBmpPtr - (pointer to)bmp of supported type of API to call
*
* RETURNS:
*       GT_OK           -   on success
*       GT_BAD_PTR      -   bad pointer
*       GT_NOT_FOUND    -   device type is unknown
*       GT_FAIL         -   on failure
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoSysConfigFuncsGet
(
    IN  GT_U32                      deviceType,
    OUT APP_DEMO_SYS_CONFIG_FUNCS   *sysConfigFuncsPtr,
    OUT GT_U32                      *apiSupportedBmpPtr
);

/*******************************************************************************
* appDemoDxChFdbFlush
*
* DESCRIPTION:
*       Deletes all addresses from FDB table.
*
*
* INPUTS:
*       dev            - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDxChFdbFlush
(
    IN GT_U8   dev
);

/*******************************************************************************
* appDemoDxChXcatSgmii2_5GbNetworkPortWa
*
* DESCRIPTION:
*       The workaround for SGMII 2.5GB
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum     - physical device number
*       portsBmp   - physical ports bitmap
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NO_RESOURCE  - memory resource not available.
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDxChXcatSgmii2_5GbNetworkPortWa
(
    GT_U8   devNum,
    GT_U32  portsBmp
);

/*******************************************************************************
* appDemoDxChPowerDownBmpSet
*
* DESCRIPTION:
*       Sets AppDemo power down SERDES bitmask.
*
* INPUTS:
*       powerDownBmp  - the value to be written.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxChPowerDownBmpSet
(
    IN GT_U32   powerDownBmp
);

/*******************************************************************************
* appDemoHwDevNumOffsetSet
*
* DESCRIPTION:
*       Sets value of appDemoHwDevNumOffset used during device HW ID calculation
*       based on device SW ID..
*
* INPUTS:
*       hwDevNumOffset - value to save into appDemoHwDevNumOffset.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoHwDevNumOffsetSet
(
    IN GT_U8 hwDevNumOffset
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtAppDemoSysConfigh */
