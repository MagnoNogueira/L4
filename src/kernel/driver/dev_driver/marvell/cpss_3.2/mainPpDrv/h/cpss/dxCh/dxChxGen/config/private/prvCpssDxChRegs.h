/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChRegs.h
*
* DESCRIPTION:
*       This file includes the declaration of the structure to hold the
*       addresses of PP registers for each different PP type.
*
*
* FILE REVISION NUMBER:
*       $Revision: 50$
*
*******************************************************************************/
#ifndef __prvCpssDxChRegsh
#define __prvCpssDxChRegsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>

#define LED_MAX_NUM_OF_INTERFACE 2
#define LED_MAX_NUM_OF_CLASS     7
#define LED_MAX_NUM_OF_GROUP     4

#define MAX_NUM_OF_QOS_PROFILE_CNS 16

#define DEBUG_REG_FIELDS


struct _txPortRegs
{
    GT_U32 txConfig;            /* Port Transmit Configuration  */
    GT_U32 wdTxFlush;           /* Port Watchdog Register       */
    GT_U32 tokenBuck;
    GT_U32 tokenBuckLen;
    GT_U32 prioTokenBuck[8];    /* per priority */
    GT_U32 prioTokenBuckLen[8];    /* per priority */
    GT_U32 wrrWeights0;         /* Port WRR Weights 0 Register  */
    GT_U32 wrrWeights1;         /* Port WRR Weights 1 Register  */
    GT_U32 wrrStrictPrio;       /* Port WRR & Strict Priority   */
    GT_U32 descrLimit;          /* Port descriptor limit        */
    GT_U32 descrCounter;        /* Port Descriptors Counter Register */
    GT_U32 bufferCounter;       /* Port Buffers Counter Register */
    GT_U32 tcDescrCounter[8];   /* Port TC Descriptors Counter Register */
    GT_U32 tcBufferCounter[8];  /* Port TC Buffers Counter Register */

};
/* number of lanes in the XPCS */
#define PRV_CPSS_DXCH_NUM_LANES_CNS 4

/* number of SERDESs for the port */
#define PRV_CPSS_DXCH_NUM_SERDESS_CNS 4
struct _perPortRegs
{
    GT_U32 macCtrl;             /* MAC Control Register or Register0 */
    GT_U32 macStatus;           /* MAC Status Register      */
    GT_U32 macCounters;         /* MAC counters register.   */
    GT_U32 macCaptureCounters;   /* MAC capture port counters register.   */
    GT_U32 autoNegCtrl;         /* AutoNeg Control Register */
    GT_U32 serdesCnfg;          /* Serdes configuration reg */
    /* SERDESs registers */
    GT_U32 serdesCommon[PRV_CPSS_DXCH_NUM_SERDESS_CNS];     /* Serdes Common Module Configuration Register */
    GT_U32 serdesPowerUp1[PRV_CPSS_DXCH_NUM_SERDESS_CNS];   /* Serdes Power Up Register1 */
    GT_U32 serdesPowerUp2[PRV_CPSS_DXCH_NUM_SERDESS_CNS];   /* Serdes Power Up Register2 */
    GT_U32 serdesReset[PRV_CPSS_DXCH_NUM_SERDESS_CNS];      /* Serdes Reset Register */
    GT_U32 serdesTxSyncConf[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Tx Sync Conf Register */
    GT_U32 serdesSpeed1[PRV_CPSS_DXCH_NUM_SERDESS_CNS];     /* Serdes speed register 1 */
    GT_U32 serdesSpeed2[PRV_CPSS_DXCH_NUM_SERDESS_CNS];     /* Serdes speed register 2 */
    GT_U32 serdesSpeed3[PRV_CPSS_DXCH_NUM_SERDESS_CNS];     /* Serdes speed register 3 */
    GT_U32 serdesTxConfReg1[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Tx Configuration Register1 */
    GT_U32 serdesTxConfReg2[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Tx Configuration Register2 */
    GT_U32 serdesRxConfReg1[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Rx Configuration Register1 */
    GT_U32 serdesRxConfReg2[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Rx Configuration Register2 */
    GT_U32 serdesDtlConfReg2[PRV_CPSS_DXCH_NUM_SERDESS_CNS];/* Serdes DTL Conf Register2 */
    GT_U32 serdesDtlConfReg3[PRV_CPSS_DXCH_NUM_SERDESS_CNS];/* Serdes DTL Conf Register3 */
    GT_U32 serdesDtlConfReg5[PRV_CPSS_DXCH_NUM_SERDESS_CNS];/* Serdes DTL Conf Register5 */
    GT_U32 serdesMiscConf[PRV_CPSS_DXCH_NUM_SERDESS_CNS];   /* Serdes Misc. Configuration Register */
    GT_U32 serdesReservConf[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Reserv Conf Register */
    GT_U32 serdesIvrefConf1[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes IVREF Configuration Register1 */
    GT_U32 serdesIvrefConf2[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes IVREF Configuration Register2 */
    GT_U32 serdesProcessMon[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Process Monitor Configuration Register */
    GT_U32 serdesCalibConf1[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Calibration Configuration Register (1) */
    GT_U32 serdesCalibConf2[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Calibration Configuration Register 2 */
    GT_U32 xgGlobalConfReg0;    /* XG Global Configuration Register 0 */
    GT_U32 macCtrl1;            /* MAC Control Register1 - Cheetah only */
    GT_U32 macCtrl2;            /* MAC Control Register2 - Cheetah only */
    GT_U32 macCtrl3;            /* MAC Control Register3 - Cheetah3 only */
    GT_U32 xgMibCountersCtrl;   /* XG MIB Counters Control Register - Cheetah3 only */
    GT_U32 serialParameters;    /* Serial Parameters Register - Cheetah only */
    GT_U32 hxPortConfig0[2];    /* HX Port Configuration Registers 0 -
                                   Cheetah HX ports only */
    GT_U32 hxPortConfig1[2];    /* HX Port Configuration Registers 1 -
                                   Cheetah HX ports only */
    GT_U32 prbsCheckStatus;     /* PRBS Check Status register */
    GT_U32 prbsErrorCounter;    /* PRBS Error Counter Register */
    GT_U32 ccfcFcTimerBaseGig;  /* CCFC FC Timer Register for Gig port*/
    GT_U32 ccfcFcTimerBaseXg;   /* CCFC FC Timer Register for XG port*/
    GT_U32 Config100Fx;         /* 100FX Configuration Register */

    /* lane registers */
    GT_U32 laneConfig0[PRV_CPSS_DXCH_NUM_LANES_CNS];      /* Lane Configuration Register0 */
    GT_U32 laneConfig1[PRV_CPSS_DXCH_NUM_LANES_CNS];      /* Lane Configuration Register1 */
    GT_U32 prbsErrorCounterLane[PRV_CPSS_DXCH_NUM_LANES_CNS];/* PRBS Error Counter Register */
    GT_U32 laneStatus[PRV_CPSS_DXCH_NUM_LANES_CNS];       /* Lane Status Register */
    GT_U32 cyclicData[PRV_CPSS_DXCH_NUM_LANES_CNS];       /* Cyclic Data First Register*/

};

struct _bufMngPerPort
{
    GT_U32 flowCntrlThr;        /* Port Flow Control Threshold Register */
    GT_U32 rxBufLimit;          /* Port Receive Buffer Limit Register */
};


struct _bufLimitsProfile
{
    GT_U32 associationReg0;   /* Buffers Limits Profile Association Register0 */
    GT_U32 associationReg1;   /* Buffers Limits Profile Association Register1 */
};

struct _lpSerdesConfig
{
    GT_U32 pllIntpReg1;   /* PLL/INTP Register 1 */
    GT_U32 pllIntpReg2;   /* PLL/INTP Register 2 */
    GT_U32 pllIntpReg3;   /* PLL/INTP Register 3 */
    GT_U32 pllIntpReg4;   /* PLL/INTP Register 4 */
    GT_U32 pllIntpReg5;   /* PLL/INTP Register 5 */
    GT_U32 calibrationReg0; /* Calibration Register 0 */
    GT_U32 calibrationReg1; /* Calibration Register 1 */
    GT_U32 calibrationReg2; /* Calibration Register 2 */
    GT_U32 calibrationReg3; /* Calibration Register 3 */
    GT_U32 calibrationReg5; /* Calibration Register 5 */
    GT_U32 calibrationReg7; /* Calibration Register 7 */
    GT_U32 transmitterReg0; /* Transmitter Register 0 */
    GT_U32 transmitterReg1; /* Transmitter Register 1 */
    GT_U32 transmitterReg2; /* Transmitter Register 2 */
    GT_U32 receiverReg1;    /* Receiver Register 1 */
    GT_U32 receiverReg2;    /* Receiver Register 2 */
    GT_U32 ffeReg;          /* FFE Register 0 */
    GT_U32 analogReservReg; /* Analog Reserved Register 0 */
    GT_U32 slcReg;          /* SLC Register */
    GT_U32 referenceReg;    /* Reference Register 0 */
    GT_U32 powerReg;        /* Power Register 0 */
    GT_U32 resetReg;        /* Reset Register 0 */
    GT_U32 digitalIfReg0;   /* Digital Interface Register 0 */
    GT_U32 standaloneCntrlReg; /* Standalone Control Register 0 */
    GT_U32 serdesExternalReg1; /* Serdes External Configuration 1 */
    GT_U32 serdesExternalReg2; /* Serdes External Configuration 1 */
    GT_U32 phyTestReg0;        /* PHY Test Register 0 */
    GT_U32 phyTestReg1;        /* PHY Test Register 1 */
    GT_U32 phyTestDataReg5;    /* PHY Test Data Register 5 */
    GT_U32 phyTestPrbsCntReg2; /* PHY Test PRBS Counter Register 2 */
    GT_U32 phyTestPrbsErrCntReg0; /* PHY Test PRBS Error Counter Register 0 */
    GT_U32 phyTestPrbsErrCntReg1; /* PHY Test PRBS Error Counter Register 1 */

};

/*
 * Typedef: struct PRV_CPSS_DXCH_PP_REGS_ADDR_STC
 *
 * Description: A struct including PP registers addresses for register access
 *              by different types of PPs.
 *
 * Fields: The struct includes fields by the names of PP registers, each
 *         field represents the respective register address.
 *
 * Notes:
 *      1.  Groups of registers (e.g: port(0,..9) control) will be represented
 *          by an array of registers addresses, for easy access to these
 *          registers.
 *      2.  Before each field which is write-only registers set and/or a pointer
 *          to a dynamically allocated buffer, bookmark is added to be used for
 *          scanning the PRV_CPSS_DXCH_PP_REGS_ADDR_STC struct linearly, without
 *          depending on the names of the fields.
 *          The fields are:
 *              sw<FieldName>Bookmark - Indicates that a size field is
 *                                      declared two words ahead.
 *              sw<FieldName>Type     - Indicates the type of data following the
 *                                      bookmark:
 *                                      can be write only data and/or pointer
 *                                      field
 *              sw<FieldName>Size     - Indicates the size (in bytes) of the
 *                                      next field (which is a pointer).
 *                                      or/and number of bytes to skip in dump
 *                                      operation
 *                                      (if case then the data is write only)
 *          The above fields must appear immediately before every dynamically
 *          allocated pointer and in the above order.
 *
 */
typedef struct
{

     struct _global
    {
        GT_U32          globalControl;  /* Global Control                      */
        GT_U32          extendedGlobalControl;  /* Extended Global Control */
        GT_U32          extendedGlobalControl2; /* Extended Global Control2 */
        GT_U32          addrCompletion; /* PCI to Internal memory regions map  */
        GT_U32          auQBaseAddr;    /* Addr Update Block Base address.     */
        GT_U32          fuQBaseAddr;    /* Fu Addr Update Block Base address.  */
        GT_U32          auQControl;     /* Addr Update Queue Control.          */
        GT_U32          fuQControl;     /* Fu Addr Update Queue Control.       */
        GT_U32          interruptCoalescing; /* Interrupt Coalesing Conf. Register  */
        GT_U32          addrUpdate[4];  /* address update registers            */
        GT_U32          addrUpdateControlReg;/*address update control registers*/
        GT_U32          dummyReadAfterWriteReg; /* Used for read after write   */
        GT_U32          cpuPortCtrlReg; /* CPU port control register */
        GT_U32          lastReadTimeStampReg; /* Last Read Time Stamp Register */
        GT_U32          sampledAtResetReg;    /* Sampled at Reset Register */
        GT_U32          ftdllReg;             /* Fine tune DLL Register */
        GT_U32          analogCfgReg;         /* analog config register */
    }globalRegs;

    struct _bufferMng
    {
        struct _eqBlkCfgRegs
        {
            GT_U32          trSrcSniff;
            GT_U32          trapSniffed;
            GT_U32          sniffQosCfg; /* Ingress and Egress Monitoring
                                             to Analyzer QoS Configuration
                                             Register*/
            GT_U32          sniffPortsCfg; /* Analyzers Configuration Register*/
            GT_U32          analyzerPortGlobalConfig; /* Analyzer Port Global
                                                     Configuration Register.
                                                     For DxChXcat and above */
            GT_U32          mirrorInterfaceParameterReg[7]; /* Mirror Interface
                                                            Parameter Register.
                                                       For DxChXcat and above */
            GT_U32          portTxMirrorIndex[3]; /* Port Egress Mirror Index
                                                        For DxChXcat and above */
            GT_U32          portRxMirrorIndex[3]; /* Port Ingress Mirror Index
                                                        For DxChXcat and above */
            GT_U32  cpuRateLimiterConfBase; /* TO CPU Rate Limiter Configuration Register */
            GT_U32  preEgrEngineGlobal; /* Pre Egress Engine Global Register */
            GT_U32  dupPktsToCpuConfReg;/*Duplication of Packets to CPU Configuration Register*/
            GT_U32  ieeeResMcstCpuCodeIndexBase; /* IEEE Reserved Multicast
                                                      CPU Code Index Register */
            GT_U32  ipv4McstLinkLocalCpuCodeIndexBase; /* IPv4 Multicast Link Local
                                                             CPU Code Index Register */
            GT_U32  ipv6McstLinkLocalCpuCodeIndexBase; /* IPv6 Multicast Link Local
                                                             CPU Code Index Register */
            GT_U32  tcpUdpDstPortRangeCpuCodeWord0Base; /* TCP/UDP Destination Port
                                                    Range CPU Code Entry Word 0 */
            GT_U32  tcpUdpDstPortRangeCpuCodeWord1Base; /* TCP/UDP Destination Port
                                                    Range CPU Code Entry Word 1 */
            GT_U32  ipProtCpuCodeBase;         /* IP protocol CPU Code Register */
            GT_U32  ipProtCpuCodeValid;    /* IP protocol CPU Code Valid Register */

            GT_U32  cpuCodeAccessControlReg;/*CPU Code Access Control Register*/
            GT_U32  cpuCodeAccessDataReg;   /*  CPU Code Access Data Register */
            GT_U32  cpuStatRateLimitControlReg;/* Statistical Rate Limits Table
                                                  Access Control Register*/
            GT_U32  cpuStatRateLimitDataReg;   /* Statistical Rate Limits Table
                                                  Access Data Register */

            GT_U32  cpuTargDevConfReg0;   /* CPU Code Target device configuration Register */
            GT_U32  cpuTargDevConfReg1;   /* CPU Code Target device configuration Register */
            GT_U32  logicalTargetDeviceMappingConfReg; /*Logical Target Device Mapping Configuration */

        } eqBlkCfgRegs;

        struct _trunkTblCfgRegs
        {

            GT_U32  trunkNumConfigReg[7];/* addresses of trunk numbers of
                                            the ports (4 ports in reg) */
            GT_U32  baseAddrNonTrunk; /* base address of non trunk members table */

            GT_U32  baseAddrTrunkDesig; /* base address of trunk designated port table */

        } trunkTblCfgRegs;

        GT_U32  srcIdEggFltrTbl[32];
        GT_U32  devMapTbl[32];
        GT_U32  swBufMngPerPortBookmark;
        GT_U32  swBufMngPerPortType;
        GT_U32  swBufMngPerPortSize;

        struct _bufMngPerPort *bufMngPerPort;   /* array of per port regs */
        GT_U32  bufMngAgingConfig; /*Buffer Management Aging Configuration Reg*/
        GT_U32  bufMngGlobalLimit; /*Buffer Management Global Limits Reg*/
        GT_U32  cpuRxBufLimit;          /* CPU Receive Buffer Limit Reg */
        GT_U32  cpuRxBufCount;          /* CPU Receive Buffer Limit Cnt */

        GT_U32  bufMngGigaPortGroupLimit; /*Buffer Management Giga Port Group Limits Reg*/
        GT_U32  bufMngHyperGStackPortGroupLimit; /*Buffer Management HyperG.Stack Port Group Limits Reg*/

        struct _bufLimitsProfile bufLimitsProfile;
        GT_U32  bufMngSharedBufConfigReg; /* Buffer Management Shared Buffers Conf. Reg. */

        GT_U32  bufMngMetalFix;     /* Buffer Management Metal Fix Register */

    }bufferMng;

    struct _egrTxQConf              /* Egress and TxQ Configuration */
    {

        struct _setsConfigRegs
        {
            GT_U32 tcDp0Red[8][8];      /* Set Traffic Class DP 0 RED
                                         In 98Dx2x5 devices there 8 Profiles */
                                        /* 8 traffic classes and 4 sets      */
            GT_U32 tcDp1Red[8][8];      /* Set Traffic Class DP 1 RED
                                         In 98Dx2x5 devices there 8 Profiles */
        }setsConfigRegs;

        struct _txQCountSet         /* Transmit Queue Count SET 0,1 */
        {
            GT_U32  txQConfig;         /* Configuration Register             */
            GT_U32  outUcPkts;         /* Outgoing Unicast Packet Count       */
            GT_U32  outMcPkts;         /* Outgoing Multicast Packet Count     */
            GT_U32  outBytePkts;       /* Outgoing Byte Count Packet Count    */
            GT_U32  brgEgrFiltered;    /* Bridge Egress Filtered Packet Count */
            GT_U32  txQFiltered;       /* Transmit Queue Filtered Packet Count*/
            GT_U32  outCtrlPkts;       /*outgoing control (to cpu, from cpu and
                                   to analyzer) packet count.  For Cheetah only*/
            GT_U32  egrFrwDropPkts;   /*egress forwarding restriction dropped
                               Packets Count Register. For 98DX2x5 devices only*/
        }txQCountSet[2];

        GT_U32     trSniffMirrorReg;    /* Transmit Sniffer Register    */
        GT_U32     trStatSniffAndStcReg; /* Cheetah Tx Statistical Mirroring
                                            and STC config */
        GT_U32     txQueueConfig;       /* Transmit Queue Control       */
        GT_U32     txQueueMiscCtrl;     /* Transmit Queue Misc. Control (Metal Fix) Register */
        GT_U32     txQMcFifoEccConfig;  /* Txq Multicast FIFO and ECC Configuration */
        GT_U32     txRouterEgressConfig;    /* Router Egress Configuration*/

        struct _txPortRegs  txCpuRegs;  /* CPU Tx regs                  */
        GT_U32  swTxPortRegsBookmark;
        GT_U32  swTxPortRegsType;
        GT_U32  swTxPortRegsSize;

        struct _txPortRegs *txPortRegs; /* ptr to Tx regs struct for ports */

        GT_U32     cncCountMode;

        GT_U32     sniffTailDropCfgReg;
        GT_U32     txQueueConfigExt;   /* Transmit Queue Control Extended */
        GT_U32     tcDp0CscdDataRemap; /* Data Packets TC and DP0
                                          Remapping Register */
        GT_U32     tcDp1CscdDataRemap; /* Data Packets TC and DP0
                                          Remapping Register */
        GT_U32     txQueueResSharingAndTunnelEgrFltr; /* Transmit Queue Resource
                   Sharing And Tunnnel Egress Filtering */

        GT_U32     txQueueDpToCfiReg;          /* DP to CFI Reg. */
        GT_U32     txQueueDpToCfiPerPortEnReg; /* DP2CFI Enable Per Port Register */
        GT_U32     txQueueSharedPriorityReg[2]; /* Shared priority Reg. */

        /* Cheetah3 registers */
        GT_U32     secondaryTargetPortMapTbl[64]; /* Secondary Target Port Map Table */
        GT_U32     stackRemapPortEnReg; /* Stack remap enable per port register
                                           for fast stack failover */
        GT_U32     stackTcPerProfileRemapTbl[8]; /* Stack TC per Profile remap table */

        /* new xCat and above registers */
        GT_U32     l2PortIsolationTableBase;  /* L2 Port Isolation table base address */
        GT_U32     l3PortIsolationTableBase;  /* L3 Port Isolation table base address */

        GT_U32     totalBufferLimitConfig;    /* Total Buffer Limit Configuration */

    }egrTxQConf;


    struct _interrupts
    {

        GT_U32  intSumCauseBookmark;
        GT_U32  intSumCauseType;
        GT_U32  intSumCauseSize;
        GT_U32  intSumCause;        /* PCI Interrupt Summary Cause             */
        GT_U32  intSumMask;         /* PCI Interrupt Summary Mask              */
        GT_U32  ethBrgCause;        /* Ethernet Bridge Interrupt Cause         */
        GT_U32  ethBrgMask;         /* Ethernet Bridge Interrupt Mask Cause    */
        GT_U32  bmIntCause;         /* EPF Buffer Management Interrupt Cause   */
        GT_U32  bmIntMask;          /* EPF Buffer Management Interrupt Mask    */
        GT_U32  gopInt0Cause;       /* GOP Interrupt Cause Register 0          */
        GT_U32  gopInt0Mask;        /* GOP Interrupt Mask Register 0           */
        GT_U32  gopInt1Cause;       /* GOP Interrupt Cause Register 1          */
        GT_U32  gopInt1Mask;        /* GOP Interrupt Mask Register 1           */
        GT_U32  miscIntCause;       /* Miscellaneous Interrupt Cause           */
        GT_U32  miscIntMask;        /* Miscellaneous Interrupt Mask            */
        GT_U32  txqIntCause;        /* Transmit Queue Interrupt Cause          */
        GT_U32  txqIntMask;         /* Transmit Queue Interrupt Mask           */
        GT_U32  rxDmaIntCause;      /* Receive SDMA Interrupt Cause            */
        GT_U32  rxDmaIntMask;       /* Receive SDMA Interrupt Mask             */
        GT_U32  txDmaIntCause;      /* Transmit SDMA Interrupt Cause           */
        GT_U32  txDmaIntMask;       /* Transmit SDMA Interrupt Mask            */
    }interrupts;

    GT_U32  swi2cRegBookmark;
    GT_U32  swi2cRegType;
    GT_U32  swi2cRegSize;

    struct _i2cReg
    {
          GT_U32  slaveAddr;
          GT_U32  slaveAddrExtend;
          GT_U32  data;
          GT_U32  ctrl;
          GT_U32  status;
          GT_U32  baudRate;
          GT_U32  softReset;
          GT_U32  serInitCtrl;
          GT_U32  serFinishAddr;
    }i2cReg;


    struct _sdmaRegs
    {
        GT_U32  sdmaCfgReg;         /* SDMA configuration register.         */
        GT_U32  rxQCmdReg;          /* Receive Queue Command Reg.           */
        GT_U32  rxDmaCdp[8];        /* Rx SDMA Current Rx Desc. Pointer Reg */
        GT_U32  txQCmdReg;          /* Transmit Queue Command Reg.          */
        GT_U32  txDmaCdp[8];        /* Tx Current Desc Pointer Reg.         */
        GT_U32  rxDmaResErrCnt[2];  /* Rx SDMA resource error count regs0/1.*/
        GT_U32  rxDmaPcktCnt[8];    /* Rx SDMA packet count.                */
        GT_U32  rxDmaByteCnt[8];    /* Rx SDMA byte count.                  */
        GT_U32  txQWrrPrioConfig[8];/* Tx Queue WRR Priority Configuration. */
        GT_U32  txQFixedPrioConfig; /* Tx Queue Fixed Prio Config Register. */
    }sdmaRegs;

    struct _macRegs                 /* mac related */
    {
        GT_U32  srcAddrMiddle;      /* Source Address Middle Register  */
        GT_U32  srcAddrHigh;        /* Source Address High Register    */

        GT_U32  swPerPortRegsBookmark;
        GT_U32  swPerPortRegsType;
        GT_U32  swPerPortRegsSize;
        struct _perPortRegs *perPortRegs; /* MAC regs for ports */
        GT_U32  macCountersCtrl[4];  /* MAC counters control register */
        GT_U32  hgXsmiiCountersCtr; /* HyperG.Stack Ports MIB Counters and
                                       XSMII Configuration Register */

        struct _perPortRegs cpuPortRegs;/* MAC regs for Ethernet CPU port */
        GT_U32  hxPortGlobalConfig;  /* HXPorts Global Configuration - DX269 only */

        struct { /* new xCat and above registers */
            GT_U32 QSGMIIControlRegister0;  /* 0x0a80004c + 4*n*0x400: where n (0-5) represents QSGMII */
            GT_U32 QSGMIIStatusRegister;    /* 0x0a800050 + 4*n*0x400: where n (0-5) represents QSGMII */
            GT_U32 QSGMIIPRBSErrorCounter; /* 0x0a800054 + 4*n*0x400: where n (0-5) represents QSGMII */
            GT_U32 QSGMIIControlRegister1;  /* 0x0a80044c + 4*n*0x400: where n (0-5) represents QSGMII */
            GT_U32 QSGMIIControlRegister2;  /* 0x0a80084c + 4*n*0x400: where n (0-5) represents QSGMII */
        }QSGMIIControlAndStatus[6];

    }macRegs;



    struct _ledRegs
    {
        GT_U32  ledControl[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledClass  [LED_MAX_NUM_OF_INTERFACE][LED_MAX_NUM_OF_CLASS];
        GT_U32  ledGroup  [LED_MAX_NUM_OF_INTERFACE][LED_MAX_NUM_OF_GROUP];
        GT_U32  ledXgClass04Manipulation[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledXgClass59Manipulation[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledXgClass1011Manipulation[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledXgGroup01Configuration[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledHyperGStackDebugSelect[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledHyperGStackDebugSelect1[LED_MAX_NUM_OF_INTERFACE];

    }ledRegs;


    struct _smiRegs
    {
        GT_U32 smi10GeControl;          /* 10GE SMI Control Register */
        GT_U32 smi10GeAddr;             /* 10GE SMI Address Register */
        GT_U32 smiControl;              /* SMI Control Register      */
        GT_U32 smiPhyAddr;              /* PHY Address Register0     */
        /* 98DX260/270 */
        GT_U32 swSmi10GePhyCfg0Bookmark;
        GT_U32 swSmi10GePhyCfg0Type;
        GT_U32 swSmi10GePhyCfg0Size;
        GT_U32 *smi10GePhyConfig0; /* 10GE PHY Configuration Register for ports */
        GT_U32 swSmi10GePhyCfg1Bookmark;
        GT_U32 swSmi10GePhyCfg1Type;
        GT_U32 swSmi10GePhyCfg1Size;
        GT_U32 *smi10GePhyConfig1; /* 10GE PHY Configuration Register for ports */
        GT_U32 lms0MiscConfig; /* LMS0 Misc Configurations */
        GT_U32 lms1MiscConfig; /* LMS1 Misc Configurations */

    }smiRegs;

    struct _bridge
    {
        GT_U32      swPortControlBookmark;
        GT_U32      swPortControlType;
        GT_U32      swPortControlSize;
        GT_U32      *portControl;    /* Bridge Ports Control                 */
        GT_U32      cpuPortControl;  /* Bridge CPU port Control - Salsa only */
        GT_U32      l2IngressControl;   /* Layer 2 Ingress Control           */

        GT_U32      macTblAction0;      /* MAC Table Action0                 */
        GT_U32      macTblAction1;      /* MAC Table Action1  - Salsa only   */
        GT_U32      macTblAction2;      /* MAC Table Action2  - Salsa only   */

        GT_U32      swMacRangeFltrBaseBookmark;
        GT_U32      swMacRangeFltrBaseType;
        GT_U32      swMacRangeFltrBaseSize;
        GT_U32      *macRangeFltrBase;      /* MAC Range Filtering bases */

        GT_U32      ipv6IcmpMsgTypeBase;    /* IPv6 ICMP Message type config */
        GT_U32      ipv6IcmpMsgCmd;         /* IPv6 ICMP Message type trap/mirror command */
        GT_U32      ipv4McastLinkLocalBase; /* IPv4 Mcast Link Local Config */
        GT_U32      ipv6McastLinkLocalBase; /* IPv6 Mcast Link Local Config */
        GT_U32      ipv6McastSolicitNodeAddrBase; /* IPv6 Solicit Node Multicast Address
                                                     Configuration Register 0 */
        GT_U32      ipv6McastSolicitNodeMaskBase; /* IPv6 Solicit Node Multicast Address
                                                     Configuration Register 0 */
        GT_U32      deviceTable;        /* Device Table Register */
        GT_U32      trunkTable;         /* Number of ports in each trunk*/
        GT_U32      dropIngrCntr;       /* Bridge Ingress Drop Counter */
        GT_U32      brgMacCntr0;        /* base address for MAC Addr Count0-2*/
        GT_U32      hostInPckt;         /* Host Incoming Packets Count */
        GT_U32      hostOutPckt;        /* Host Outgoing Packets Count */
        GT_U32      hostOutBrdPckt;     /* Host Outgoing Broadcast Pckt Count */
        GT_U32      hostOutMcPckt;      /* Host Outgoing Multicast Pckt Count */
        GT_U32      matrixPckt;         /* Matrix Src/Dst Packet Count */
        GT_U32      learnedDisc;        /* Learned Entry Discards Count */
        GT_U32      vlan0;              /* VLAN0 Register */
        GT_U32      vlan1;              /* VLAN1 Register */
        GT_U32      egressVlanEtherTypeSel;     /* Egress Vlan EtherType
                                                   Configuration Register
                                                   for tagged packets */
        GT_U32      egressVlanEtherTypeConfig;  /* Egress Vlan EtherType
                                                   Select Register for
                                                   tagged packets */
        GT_U32      macErrorIndicationPortConfReg; /* MAC error indication port conf. register */
        GT_U32      macErrorIndicationStatusReg[2];/* MAC error indication status register */
        GT_U32      ingFltStatus;       /* Ingress Filter */
        GT_U32      egressFilter;       /* Egress Bridging */
        GT_U32      egressFilter2;      /* Egress Bridging 2 register(Twist)*/
        GT_U32      egressFilterUnregBc; /* Egress BC Unreg. packets Filter - for 98DX2x5 devices only*/
        GT_U32      mcastLocalEnableConfig;  /* Malticast Local Enable Config Reg*/
        GT_U32      egressFilterIpMcRouted;/*Egress Filtering for IP Multicat Routed packets*/
        GT_U32      macControlReg;      /* MAC Table Control Register       */
        GT_U32      udpBcDestPortConfigReg[12]; /* UDP BC destination port Config. reg. */
        GT_U32      bridgeIpv6BytesSelection; /* IPv6 MC Bridging Bytes
                                            Selection Configuration Register */
        GT_U32      vlanMruProfilesConfigReg; /* Vlan MPU profiles confog reg -
                                                for 98DX2x5 devices only */
        GT_U32      toAnalyzerVlanAddConfig; /* TO ANALYZER VLAN Adding Configuration Registers */
        GT_U32      ingAnalyzerVlanTagConfig;/* Ingress Analyzer VLAN Tag Configuration Register */
        GT_U32      egrAnalyzerVlanTagConfig;/* Egress Analyzer VLAN Tag Configuration Register */

        struct _brgCntrSet
        {
            GT_U32  cntrSetCfg;         /* Counter Set Configuration      */
            GT_U32  inPckt;             /* Incoming Packet Count          */
            GT_U32  inFltPckt;          /* Ingress Filtered Packet Count  */
            GT_U32  secFltPckt;         /* Security Filtered Packet Count */
            GT_U32  brgFltPckt;         /* Bridge Filtered Packet Count   */
        }brgCntrSet[2];

        struct _nstRegs /* for 98DX2x5 devices only */
        {
            GT_U32  brgAccessMatrix;       /* Bridge Access Matrix */
            GT_U32  brgSecureLevelConfReg; /* Bridge Security Levels configuration Register  */
            GT_U32  cpuIngrFrwFltConfReg; /* To CPU ingress forwarding restrictions Config Reg */
            GT_U32  netIngrFrwFltConfReg; /* To Network ingress forwarding restrictions Config Reg */
            GT_U32  analyzerIngrFrwFltConfReg; /* To Analyzer ingress forwarding restrictions Config Reg */
            GT_U32  cpuPktsFrwFltConfReg; /* From CPU packetsfilter Config Reg */
            GT_U32  brgPktsFrwFltConfReg; /* Bridged packets filter Config Reg */
            GT_U32  routePktsFrwFltConfReg;/* Routed packets filter Config Reg */
            GT_U32  ingressFrwDropCounter;    /* Ingress Forwarding Restrictions Drop counter */
        }nstRegs;

        struct _srcIdReg /*For 98DX2x5 devices only */
        {
                GT_U32  srcIdAssignedModeConfReg; /* Source ID assigment mode Config. Reg.*/
                GT_U32  srcIdUcEgressFilterConfReg; /* Unicast SrcId Egress filtering Config. Reg.*/
        }srcIdReg;

        struct _vntReg /*For Ch3 devices only */
        {
            GT_U32  vntOamLoopbackConfReg;  /* OAM loopback enable per port register .*/
            GT_U32  vntCfmEtherTypeConfReg; /* CFM EtherType Config. Reg.*/
            GT_U32  vntGreEtherTypeConfReg; /* GRE EtherType Config. Reg */
        }vntReg;

        struct _vlanRegs
        {
            GT_U32      swPortPrvVlanBookmark;
            GT_U32      swPortPrvVlanType;
            GT_U32      swPortPrvVlanSize;
            GT_U32      *portPrvVlan;       /* Ports PVLAN */
            GT_U32      protoTypeBased;     /* Protocols               */
            GT_U32      protoEncBased;      /* Protocols Encapsulation */
            GT_U32      protoEncBased1;     /* Protocols Encapsulation1 */
        }vlanRegs;

        struct _qosProfileRegs
        {
            GT_U32  qosProfile2ExpTable;        /* QoSProfile to EXP Table */
            GT_U32  dscp2QosProfileMapTable;    /* DSCP2QoS-Profile Map Table */
            GT_U32  up2QosProfileMapTable;      /* UP2QoS-Profile Map Table */
            GT_U32  exp2QosProfileMapTable;     /* EXP2QoS-Profile Map Table */
            GT_U32  dscp2DscpMapTable;          /* Dscp2Dscp Mutation Table */
            GT_U32  macQosTable;                /* MAC QoS Table*/
            GT_U32  qosProfile2DpTable;         /* QoSProfile to DP Table */
        }qosProfileRegs;

        GT_U32  bridgeRateLimitConfigRegArray[2];     /*Bridge RateLimit Configuration Registers for cheetah*/
        GT_U32  bridgeGlobalConfigRegArray[3];        /* Bridge Global Configuration Registers */
        GT_U32  bridgeGlobalSecurBreachDropCounter;   /* Bridge Global Security Breach DropCounter - Cheetah */
        GT_U32  bridgePortVlanSecurBreachDropCounter; /* Bridge Port/VLAN Security Breach DropCounter - Cheetah */
        GT_U32  bridgeSecurBreachStatusRegArray[3];   /* Bridge Security Breach Status Registers - Cheetah */
        GT_U32  haVlanTransEnReg;                     /* Vlan Translation Enable Register */

        struct _ingressLog /*For Ch3 devices only */
        {
            GT_U32  etherType;      /* Ingress Log EtherType    */
            GT_U32  daLow;          /* Ingress Log DA low       */
            GT_U32  daHigh;         /* Ingress Log DA high      */
            GT_U32  daLowMask;      /* Ingress Log DA low Mask  */
            GT_U32  daHighMask;     /* Ingress Log DA high Mask */
            GT_U32  saLow;          /* Ingress Log SA low       */
            GT_U32  saHigh;         /* Ingress Log SA high      */
            GT_U32  saLowMask;      /* Ingress Log SA low Mask  */
            GT_U32  saHighMask;     /* Ingress Log SA high Mask */
            GT_U32  macDaResult;    /* MAC DA result            */
            GT_U32  macSaResult;    /* MAC SA result            */
        }ingressLog;

        /* DxChXcat and above section */
        GT_U32  bridgeIngressVlanEtherTypeTable[4]; /* Ingress Vlan Ethertype table configuration registers */
        GT_U32  bridgeEgressVlanEtherTypeTable[4];  /* Egress Vlan Ethertype table configuration registers */
        GT_U32  bridgeIngressVlanSelect[32];        /* Ingress Vlan Select registers */
        GT_U32  bridgeEgressPortTag0TpidSelect[4];  /* Egress port Tag0 TPID select register */
        GT_U32  bridgeEgressPortTag1TpidSelect[4];  /* Egress port Tag1 TPID select register */

        GT_U32  ieeeReservedMcastProfileSelect0; /* IEEE Reserved Multicast Profile table select 0 */
        GT_U32  ieeeReservedMcastProfileSelect1; /* IEEE Reserved Multicast Profile table select 1 */
    }bridgeRegs;

    struct _pcl
    {
        GT_U32  PclBookmark;         /* because pcl need special function to read fromwe need book mark*/
        GT_U32  PclType;             /* because pcl need special function to read fromwe need book mark*/
        GT_U32  PclSize;             /* because pcl need special function to read fromwe need book mark*/
        GT_U32  control;            /* PCL control                          */
        /* 98DX250/260/270 */
        /* registers - buffer for triggered operation */
        GT_U32 tcamWriteData[12];
        GT_U32 tcamReadMaskBase;
        GT_U32 tcamReadPatternBase;
        GT_U32 tcamOperationTrigger;
        GT_U32 tcamOperationParam1;
        GT_U32 tcamOperationParam2;

        GT_U32 tcamReadMaskControlBase;
        GT_U32 tcamReadPatternControlBase;
        GT_U32 actionTableBase;

        /* user defined bytes configurations */
        GT_U32 udbConfigKey0bytes0_2;
        GT_U32 udbConfigKey0bytes3_5;
        GT_U32 udbConfigKey1bytes0_2;
        GT_U32 udbConfigKey1bytes3_5;
        GT_U32 udbConfigKey2bytes0_2;
        GT_U32 udbConfigKey3bytes0_2;
        GT_U32 udbConfigKey4bytes0_1;
        GT_U32 udbConfigKey5bytes0_2;
        /* rule match conters base 0x0B801000 */
        GT_U32 ruleMatchCounersBase;

        /*98DX255/265/275/285*/
        /* EPCL - egress PCL engine */
        GT_U32 epclGlobalConfig;
        /* bit per port registers */
        GT_U32 epclCfgTblAccess;
        GT_U32 epclEnPktNonTs;
        GT_U32 epclEnPktTs;
        GT_U32 epclEnPktToCpu;
        GT_U32 epclEnPktFromCpuData;
        GT_U32 epclEnPktFromCpuControl;
        GT_U32 epclEnPktToAnalyzer;
        /* TCP/UDP port comparators (8 register sequences) */
        GT_U32 ipclTcpPortComparator0;
        GT_U32 ipclUdpPortComparator0;
        GT_U32 epclTcpPortComparator0;
        GT_U32 epclUdpPortComparator0;
        /* Ch3 registers */
        GT_U32 fastStack; /* fast stack failover */
        GT_U32 loopPortReg;         /* loop port for fast stack failover */
       /* CNC relted registers */
        GT_U32 cncCountMode;
        GT_U32 cncl2l3IngressVlanCountEnable;
        /* TCAM low level HW configuration - CH3 only */
        GT_U32 pclTcamConfig0;
        GT_U32 pclTcamConfig2;
        GT_U32 pclTcamControl;
        /* DxChXCat and above registers */
        GT_U32 pclUDBConfig;
        GT_U32 pclEngineConfig;
        GT_U32 ttiUnitConfig;
        GT_U32 ttiEngineConfig;
        /* Egress Policer Global Configuration*/
        GT_U32 eplrGlobalConfig;

        struct _policyTcamTest
        {
            GT_U32  pointerRelReg[14];  /* Pointer (to line for effuse replacement) Register */
            GT_U32  configStatusReg;    /* config status Register */
            GT_U32  opcodeExpectedHitReg; /* Opcode Expected Hit Rel */
            GT_U32  opcodeCommandReg;     /* Opcode Command Register */
        }policyTcamTest;

    }pclRegs;

    struct _ipv4Regs
    {
        GT_U32  ipTunnelTerm0;      /* IP tunnel termination register 0.    */
        GT_U32  ipTunnelTerm1;      /* IP tunnel termination register 1.    */
        GT_U32  vrTableBase;        /* Base address of the Vr Table.        */
        GT_U32  mcLpmRootBase;      /* Multicast LPM Root Node Base Address */
        GT_U32  defMcTblBase;       /* Base address of the Default Mc Tbl.  */
        GT_U32  mllBaseAddr;        /* Base address of Mll memory block.    */
        GT_U32  ipv4CtrlReg;        /* IPv4 Global control register.        */
        GT_U32  rxPcktCnt[3];       /* Receive Packet Counter Table Register*/
        GT_U32  rxOctetCntLow[3];   /* Receive Octet Counter Low Register   */
        GT_U32  rxOctetCntHigh[3];  /* Receive Octet Counter High Register  */
        GT_U32  discardCnt[3];      /* Discard Packet Counter Register      */
        GT_U32  discardOctet[3];    /* Discard Octet Counter Register       */
        GT_U32  ipMcPcktCnt[3];     /* Ip-Mc packets Counter Register       */
        GT_U32  trapPcktCnt[3];     /* Trapped Packet Counter Register      */
        GT_U32  tunnelTermPcktCnt[3];/* Tunnel Terminate Packet Counter Reg.*/
        GT_U32  failRpfPcktCnt[3];  /* Failed Reverse Path Forwarding Packet*/
                                    /* Counter Register.                    */
        GT_U32  ipGlobalCntReg0;    /* IPv4 Global Counter 0 Register.      */
        GT_U32  ipGlobalCntReg1;    /* IPv4 Global Counter 1 Register.      */
        GT_U32  ipGlobalCntReg2;    /* IPv4 Global Counter 2 Register.      */
        GT_U32  ipGlobalDiscardPcktCnt; /* IPv4 Discard Packet Counter Reg. */
        GT_U32  ipGlbDiscardOctetCntLow;/* IPv4 Discard Octet Low Counter   */
                                    /* Register.                            */
        GT_U32  ipGlbDiscardOctetCntHigh;/* IPv4 Discard Octet Low Counter  */
                                    /* Register.                            */
        GT_U32  ipv4ShadowCntReg;   /* IPv4 Shadow Counter Register.        */
        GT_U32  dscp2CosTable;      /* dscp to cos table                    */

        /* cheetah2 regs */

        GT_U32 ctrlReg0;    /* IPv4 Control Register0 */

        GT_U32 ctrlReg1;    /* IPv4 Control Register1 */

        GT_U32 ucEnCtrlReg; /* IPv4 Unicast Enable Control Register */

        GT_U32 mcEnCtrlReg; /* IPv4 Multicast Enable Control Register */

    }ipv4Regs;               /* ipRegs is Registers set for IPv4 & IPv6     */

    struct _ipv6Regs
    {
        GT_U32 ipv6CtrlReg;
        GT_U32 erroneousLpmTbl;
        GT_U32 ipv6vrTableBase; /* Base address of the ipv6 Vr Table.        */

        GT_U32 ucScopeTblRegs[4]; /* IPv6 Unicast Scope Command Table
                                     Registers */
        GT_U32 mcScopeTblRegs[4]; /* IPv6 multicast Scope Command Table
                                     Registers */
        GT_U32 ipScopeRegs[4]; /* IPV6 Unicast Scope Prefix<n> Registers */
        GT_U32 ipScopeCmdReg;

        GT_U32 mldSnoopingRegs[3];
        GT_U32 outblockCtrlReg;

        GT_U32  rxPcktCnt[3];       /* Receive Packet Counter Table Register*/
        GT_U32  rxOctetCntLow[3];   /* Receive Octet Counter Low Register   */
        GT_U32  rxOctetCntHigh[3];  /* Receive Octet Counter High Register  */
        GT_U32  discardCnt[3];      /* Discard Packet Counter Register      */
        GT_U32  discardOctet[3];    /* Discard Octet Counter Register       */
        GT_U32  ipMcPcktCnt[3];     /* Ip-Mc packets Counter Register       */
        GT_U32  trapPcktCnt[3];     /* Trapped Packet Counter Register      */
        GT_U32  failRpfPcktCnt[3];  /* Failed Reverse Path Forwarding Packet*/
                                    /* Counter Register.                    */
        GT_U32  ipGlobalCntReg0;    /* IPv4 Global Counter 0 Register.      */
        GT_U32  ipGlobalCntReg1;    /* IPv4 Global Counter 1 Register.      */
        GT_U32  ipGlobalCntReg2;    /* IPv4 Global Counter 2 Register.      */
        GT_U32  ipGlobalDiscardPcktCnt; /* IPv4 Discard Packet Counter Reg. */
        GT_U32  ipGlbDiscardOctetCntLow;/* IPv4 Discard Octet Low Counter   */
                                    /* Register.                            */
        GT_U32  ipGlbDiscardOctetCntHigh;/* IPv4 Discard Octet Low Counter  */
                                    /* Register.                            */
        GT_U32  ipv6ShadowCntReg;   /* IPv4 Shadow Counter Register.        */

        /* cheetah2 regs */

        GT_U32 ctrlReg0;    /* IPv4 Control Register0 */
        GT_U32 ctrlReg1;    /* IPv4 Control Register1 */
        GT_U32 ucEnCtrlReg; /* IPv4 Unicast Enable Control Register */
        GT_U32 mcEnCtrlReg; /* IPv4 Multicast Enable Control Register */

        GT_U32 ipScopeLevel[4]; /* IPV6 Unicast Scope<n> Level Registers */

    }ipv6Regs;

    struct _ipRegs
    {

        GT_U32 ctrlReg0;
        /* cheetah + Ctrl regs */
        GT_U32 routerHdrAltGblCfgReg; /* router Header Alteration Golbal    */
                                      /* configuration                      */
        GT_U32 routerMacSaBaseReg[2]; /* router Mac SA Base Regs 0&1            */

        GT_U32 routerHdrAltEnMacSaModifyReg; /* router Header Alteration enable*/
                                             /* Mac SA modification           */
        GT_U32 routerDevIdModifyEnReg;       /* Device ID Modification Enable */

        GT_U32 routerHdrAltTblAccCtrlReg; /* router Header Alteration Table   */
                                          /* table access control             */
        GT_U32 routerHdrAltTblAccDataReg[2]; /* router Header Alteration Table*/
                                             /* table access Data 0&1         */
        GT_U32 routerHdrAltMacSaModifyMode[2];  /* router Header Alteration MAC SA */
                                                /* Modification Mode               */

        /* cheetah2 regs */
        GT_U32 routerGlobalReg;             /*Router Global Control Register*/

        GT_U32 routerMtuCfgRegs[4];         /*Router MTU Configuration Register*/

        GT_U32 routerTtTcamAccessCtrlReg; /*Router TCAM Access Control
                                            Register*/

        GT_U32 routerTtTcamAccessDataCtrlReg; /*Router and TT TCAM Access Data
                                                Control Register*/

        GT_U32 routerTtTcamAccessDataRegsBase; /*Router and TT TCAM Access Data
                                                Registers base*/

        GT_U32 routerTtTcamBankAddr[5];     /* bank address for direct access to single entry*/
                                            /* in router / tunnel termination TCAM           */

        GT_U32 ucMcRouteEntriesAgeBitsBase; /*Router Next Hop Table Age Bits
                                              Entries Base*/

        GT_U32 routerMngCntSetCfgRegs[4]; /* Router Management Counters Set<n>
                                             Configuration Register*/

        GT_U32 routerMngInUcPktCntSet[4]; /* Router Management In Unicast
                                             Packets Counter Set<n> */
        GT_U32 routerMngMcPktCntSet[4];   /* Router Management In Multicast
                                             Packets Counter Set<n> */
        GT_U32 routerMngInUcNonRoutedNonExcptPktCntSet[4];/*Router Management In
                                                          Unicast Non-Routed
                                                          Non-Exception Packets
                                                          Counter Set<n> */
        GT_U32 routerMngInUcNonRoutedExcptPktCntSet[4];/*Router Management In
                                                       Unicast Non-Routed
                                                       Exception Packets Counter
                                                       Set<n> */

        GT_U32 routerMngInMcNonRoutedNonExcptPktCntSet[4];/*Router Management In
                                                          Multicast Non-Routed
                                                          Non-Exception Packets
                                                          Counter Set<n> */

        GT_U32 routerMngInMcNonRoutedExcptCntSet[4];/*Router Management In
                                                    Multicast Non-Routed
                                                    Exception Packets Counter
                                                    Set<n> */
        GT_U32 routerMngInUcTrapMrrPktCntSet[4]; /* Router Management In Unicast
                                                    Trapped / Mirrored Packet
                                                    Counter Set<n> */
        GT_U32 routerMngInMcTrapMrrPktCntSet[4]; /* Router Management In
                                                    Multicast Trapped / Mirrored
                                                    Packet Counter Set<n> */
        GT_U32 routerMngInMcRPFFailCntSet[4]; /* Router Management In Multicast
                                                 RPF Fail Counter Set<n> */
        GT_U32 routerMngOutUcPktCntSet[4]; /* Router Management Out Unicast
                                              Packet Counter Set<n> */

        GT_U32 routerBridgedPktExcptCnt; /* Router Bridged Pkt Exceptions
                                            Counter */
        GT_U32 routerDropCntCfgReg; /* Router Drop Counter Configuration
                                       Register */
        GT_U32 routerDropCnt; /* Router Drop Counter */

        GT_U32 routerInterruptCauseReg; /* Router Interrupt Cause Register */

        GT_U32 routerInterruptMaskReg; /* Router Interrupt Mask Register */

        GT_U32 routerAdditionalCtrlReg; /* Router Additional Control Register */

        GT_U32 qoSProfile2RouteBlockOffsetMapTable[MAX_NUM_OF_QOS_PROFILE_CNS];   /* QoSProfile to Route Block Offset Table */

        GT_U32 routerAccessMatrixBase; /* router Access matrix Base address*/

        struct _ipHitLog
        {
            GT_U32  dipAddrReg[4];       /* IP Hit-Log dip address */
            GT_U32  dipMaskAddrReg[4];   /* IP Hit-Log dip mask address */
            GT_U32  sipAddrReg[4];       /* IP Hit-Log sip address */
            GT_U32  sipMaskAddrReg[4];   /* IP Hit-Log sip mask address */
            GT_U32  layer4DestPortReg;   /* IP Hit-Log Layer4 Destination Port Register */
            GT_U32  layer4SrcPortReg;    /* IP Hit-Log Layer4 Source Port Register */
            GT_U32  protocolReg;         /* IP Hit-Log Protocol Register */
            GT_U32  dstIpNheEntryReg[3]; /* IP Hit-Log Destination IP NHE Entry Register */
            GT_U32  srcIpNheEntryReg[3]; /* IP Hit-Log Source IP NHE Entry Register */
        }ipHitLog;

        /* TCAM low level HW configuration - CH3 only */
        GT_U32 ipTcamConfig0;
        GT_U32 ipTcamControl;

        struct _routerTcamTest
        {
            GT_U32  pointerRelReg[20];  /* Pointer (to line for effuse replacement) Register */
            GT_U32  configStatusReg;    /* config status Register */
            GT_U32  opcodeExpectedHitReg; /* Opcode Expected Hit Rel */
            GT_U32  opcodeCommandReg;     /* Opcode Command Register */
        }routerTcamTest;

    }ipRegs;               /* ipRegs is Registers set for IPv4 & IPv6     */

    struct _ipMtRegs
    {

        GT_U32 qoSProfile2MTTCQueuesMapTable[8];   /* QoSProfile to Multi Target TC Queues Table */

        GT_U32 mllGlobalReg;            /* MLL Global Control Register */

        GT_U32 multiTargetRateShapingReg;   /* Multi Target Rate Shaping Register */

        GT_U32 mtUcSchedulerModeReg;    /* Multi Target Vs. Unicast SDWRR */
                                        /* and Strict Priority Scheduler */

        GT_U32 mtTcQueuesSpEnableReg;   /* Multi Target TC Queues Strict Priority */
                                        /* Enable Configuration Register*/

        GT_U32 mtTcQueuesWeightReg;     /* Multi Target TC Queues Weight */
                                        /* Configuration Register*/

        GT_U32 mtTcQueuesGlobalReg;     /* Multi Target TC Queues Global */
                                        /* Configuration Register */

        GT_U32 mllMcFifoFullDropCnt;    /* MLL MC Fifo Drop Counter */

        GT_U32 mllTableBase;            /* Router Multicast Linked List (MLL) Table Base */

        GT_U32 mllOutInterfaceCfg[2];   /* MLL Out Interface Counter<n> Configuration Register */

        GT_U32 mllOutMcPktCnt[2];       /* MLL Out Multicat Packets Counter<n> */

        GT_U32 dft1Reg;                 /* DFT1 Register */

        GT_U32 mllMetalFix;             /* MLL Metal Fix */

    }ipMtRegs;             /* IP Multi Traffic Registers (Cheetah 2) */

    struct _PLR /* Policer */
    {
        GT_U32      policerControlReg;          /* PLR Control */
        GT_U32      policerPortMeteringEnReg[2];/* PLR Port Metering Enable */
        GT_U32      policerMRUReg;              /* PLR MRU */
        GT_U32      policerErrorReg;            /* PLR Error */
        GT_U32      policerErrorCntrReg;        /* PLR Error Counter */

        GT_U32      policerTblAccessControlReg; /* PLR Table Access Control */
        GT_U32      policerTblAccessDataReg;    /* PLR Table Access Data */
        GT_U32      policerRefreshScanRateReg;  /* PLR Metering refresh
                                                   Scan Rate Limit */
        GT_U32      policerRefreshScanRangeReg; /* PLR Metering refresh
                                                   Scan Address Range */
        GT_U32      policerInitialDPReg;        /* PLR Initial DP - there   */
                                                /* are 8 registers for DxCh3 */
        GT_U32      policerIntCauseReg;         /* PLR Interrupt Cause */
        GT_U32      policerIntMaskReg;          /* PLR Interrupt Mask */
        GT_U32      policerShadowReg;           /* PLR Shadow */

        /* Xcat and above registers */
        GT_U32      policerControl1Reg;         /* PLR Control 1 */
        GT_U32      policerControl2Reg;         /* PLR Control 2 */
        GT_U32      portMeteringPtrIndexReg;    /* PLR Port metering pointer index */
        GT_U32      ipfixControl;               /* PLR IPFIX Control */
        GT_U32      ipfixNanoTimerStampUpload;  /* PLR IPFIX nano timer stamp upload */
        GT_U32      ipfixSecLsbTimerStampUpload;/* PLR IPFIX Seconds LSb timer stamp upload */
        GT_U32      ipfixSecMsbTimerStampUpload;/* PLR IPFIX Seconds MSb timer stamp upload */
        GT_U32      ipfixDroppedWaThreshold;    /* PLR IPFIX dropped packet count WA threshold */
        GT_U32      ipfixPacketWaThreshold;     /* PLR IPFIX packet count WA threshold */
        GT_U32      ipfixByteWaThresholdLsb;    /* PLR IPFIX byte count WA threshold LSB */
        GT_U32      ipfixByteWaThresholdMsb;    /* PLR IPFIX byte count WA threshold MSB */
        GT_U32      ipfixSampleEntriesLog0;     /* PLR IPFIX Sample Entries Log0 */
        GT_U32      ipfixSampleEntriesLog1;     /* PLR IPFIX Sample Entries Log1 */

        /*Policer Tables */
        GT_U32      policerMeteringCountingTbl; /* PLR Metering and Counting */
        GT_U32      policerQosRemarkingTbl;     /* PLR Relative Qos Remarking */
        GT_U32      policerManagementCntrsTbl;  /* PLR Management Counters */

        /* Xcat and above Tables*/
        GT_U32      policerCountingTbl;         /* PLR Counting */
        GT_U32      policerTimerTbl;            /* PLR Timers   */
        GT_U32      ipfixWrapAroundAlertTbl;    /* PLR IPFIX wrap around alert */
        GT_U32      ipfixAgingAlertTbl;         /* PLR IPFIX aging alert */

    }PLR[3]; /* Policer */


    struct _sFlowRegs
    {
        GT_U32  sFlowControl;           /* sflow control               */

        /* Cheetah ingress sampling to cpu registers */
        GT_U32  ingStcTblAccessCtrlReg;
        GT_U32  ingStcTblWord0ValueReg;
        GT_U32  ingStcTblWord1ValueReg; /* read only */
        GT_U32  ingStcTblWord2ValueReg;
        /* GT_U32  ingStcIntCauseReg; */

        /* Cheetah egress STC table registers (three words per port) */

        GT_U32  swEgrStcTblW0Bookmark;
        GT_U32  swEgrStcTblW0Type;
        GT_U32  swEgrStcTblW0Size;
        GT_U32  *egrStcTblWord0;
        GT_U32  swEgrStcTblW1Bookmark;
        GT_U32  swEgrStcTblW1Type;
        GT_U32  swEgrStcTblW1Size;
        GT_U32  *egrStcTblWord1;
        GT_U32  swEgrStcTblW2Bookmark;
        GT_U32  swEgrStcTblW2Type;
        GT_U32  swEgrStcTblW2Size;
        GT_U32  *egrStcTblWord2;

    }sFlowRegs;


    struct _addrSpace
    {
        GT_U32  cscdHeadrInsrtConf;     /* Cascading and Header Insertion Configuration */
        GT_U32  eggrDSATagTypeConf;     /* Egress DSA Tag Type Configuration */
        GT_U32  buffMemWriteControl;    /* BufMem write control        */
        GT_U32  buffMemBank0Write;      /* bank0 write                 */
        GT_U32  buffMemBank1Write;      /* bank1 write                 */
        GT_U32  buffMemClearType;       /* Buffer Memory Clear Type Register */
        GT_U32  invalidCrcModeConfigReg;/* Invalid CRC Mode Configuration Register
                                           relevant for Cheetah 2 devices */
    }addrSpace;

    struct _cncRegs
    {
        /* global configuration           */
        GT_U32  globalCfg;
        GT_U32  blockUploadTriggers;
        GT_U32  clearByReadValueWord0;
        GT_U32  clearByReadValueWord1;
        /* per couner block configuration */
        GT_U32  blockClientCfgAddrBase;
        GT_U32  blockWraparoundStatusAddrBase;
        /* counters in blocks */
        GT_U32  blockCountersAddrBase;
    }cncRegs;

    struct _ttiRegs
    {
        GT_U32 pclIdConfig0;
        GT_U32 pclIdConfig1;
        GT_U32 ttiIpv4GreEthertype;
        GT_U32 specialEthertypes;
        GT_U32 mplsEthertypes; /* The Ethertype used to identify MPLS
                                   Unicast/Multicast packets */
        GT_U32 trunkHashCfg[3];
        GT_U32 ccfcEthertype;
        GT_U32 globalUnitConfig;
        GT_U32 cmTagEtherType;
    }ttiRegs;

    struct _haRegs
    {

        GT_U32 hdrAltMplsEthertypes; /* The MPLS Ethertype to be used when MPLS
                                       headers need to be added or recognized*/

        GT_U32 hdrAltIEthertype; /* The I-Ethertype to be used when Mac
                                    In Mac headers need to be added */
        GT_U32 hdrAltCcfcEthertype;
        GT_U32 hdrAltGlobalConfig;
        GT_U32 hdrAltCnConfig;
        GT_U32 hdrAltCpidReg0;
        GT_U32 hdrAltCnTagFlowId;
        GT_U32 hdrAltCnmHeaderConfig;

    }haRegs;

    struct _bcnRegs
    {

        GT_U32 bcnControlReg;
        GT_U32 bcnPauseTriggerEnableReg;
        GT_U32 bcnGlobalPauseThresholdsReg;
        GT_U32 portsBcnProfileReg0;
        GT_U32 portsBcnAwarenessTbl;
        GT_U32 portsSpeedIndexesReg0;
        GT_U32 cnGlobalConfigReg;
        GT_U32 cnPrioQueueEnProfile;
        GT_U32 cnEnCnFrameTx;
        GT_U32 fbCalcConfigReg;
        GT_U32 fbMinReg;
        GT_U32 fbMaxReg;
        GT_U32 cnProfileThreshold;
        GT_U32 cnProfileSevereThreshold;
        GT_U32 cnProfileMildThreshold;
        GT_U32 cnProfileSampleScaleThreshold;
        GT_U32 cnSampleTbl;

    }bcnRegs;

    GT_U32  serdesConfigRegsBookmark;
    GT_U32  serdesConfigRegsType;
    GT_U32  serdesConfigRegsSize;
    struct _lpSerdesConfig *serdesConfig; /* array of per SERDES regs */

    struct _cutThroughRegs /* For lion and above. */
    {
        GT_U32 ctEnablePerPortReg;
        GT_U32 ctEthertypeReg;
        GT_U32 ctUpEnableReg;
        GT_U32 ctPacketIndentificationReg;
        GT_U32 ctCascadingPortReg;
        GT_U32 ctPortMemoryRateLimitThresholdReg[6];
        GT_U32 ctCpuPortMemoryRateLimitThresholdReg;
    }cutThroughRegs;

    struct _pfcRegs /* For lion and above. */
    {
        GT_U32 pfcGlobalConfigReg;
        GT_U32 pfcSrcPortFcMode;
        GT_U32 pfcSourcePortProfile;
        GT_U32 pfcGlobaGroupOfPortsThresholds;
        GT_U32 pfcProfileXoffThresholds;
        GT_U32 pfcProfileXonThresholds;

    }pfcRegs;

}PRV_CPSS_DXCH_PP_REGS_ADDR_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChRegsh */

