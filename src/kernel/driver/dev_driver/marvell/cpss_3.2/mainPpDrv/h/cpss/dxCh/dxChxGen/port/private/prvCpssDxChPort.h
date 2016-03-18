/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPort.h
*
* DESCRIPTION:
*       Includes structures definition for the use of CPSS DxCh Port lib .
*
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*
*******************************************************************************/
#ifndef __prvCpssDxChPorth
#define __prvCpssDxChPorth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/* max number ports in cheetah */
#define PRV_CPSS_CHEETAH_MAX_PORTS_NUM_CNS      27

/* max number ports in cheetah 2 */
#define PRV_CPSS_CHEETAH2_MAX_PORTS_NUM_CNS     28

/* defines maximal number of Ports, depending on the type of the Cheetah */
#define PRV_CPSS_DXCH_CPU_INDEX_MAC(_devNum) \
        ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E) ? \
         PRV_CPSS_CHEETAH2_MAX_PORTS_NUM_CNS : \
         PRV_CPSS_CHEETAH_MAX_PORTS_NUM_CNS)

/* get the register address - port mac control */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macCtrl;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].macCtrl

/* get the register address - port mac control register1 */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macCtrl1;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].macCtrl1

/* get the register address - port mac control register2 */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macCtrl2;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].macCtrl2

/* get the register address - port mac control register3 */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macCtrl3;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].macCtrl3

/* get the register address - Auto-Negotiation Configuration Register */
#define PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.autoNegCtrl;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].autoNegCtrl

/* get the register address - Port<n> Status Register0 */
#define PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macStatus;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].macStatus

/* get the register address - Port<n> Serial Parameters Configuration Register */
#define PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.serialParameters;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].serialParameters

/* get the register address - port TXQ configuration */
#define PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.txConfig;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].txConfig

/* get the register address - port Token bucket configuration */
#define PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.tokenBuck;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].tokenBuck;

/* get the register address - port Token bucket Len configuration */
#define PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_LEN_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.tokenBuckLen;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].tokenBuckLen;


/* get the register address - port , Queue -- Token bucket configuration */
#define PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum,portNum,tcQueue,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.prioTokenBuck[tcQueue];\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].prioTokenBuck[tcQueue];

/* get the register address - port , Queue -- Token bucket Len configuration */
#define PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_LEN_REG_MAC(devNum,portNum,tcQueue,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.prioTokenBuckLen[tcQueue];\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].prioTokenBuckLen[tcQueue];


/* get the register address - port watchdog configuration */
#define PRV_CPSS_DXCH_PORT_WATCHDOG_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.wdTxFlush;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].wdTxFlush;

/* get the register address - port Descriptors Counter Register */
#define PRV_CPSS_DXCH_PORT_DESCR_COUNT_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.descrCounter;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].descrCounter;

/* get the register address - port Buffers Counter Register */
#define PRV_CPSS_DXCH_PORT_BUFFER_COUNT_REG_MAC(devNum,portNum,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.bufferCounter;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].bufferCounter;

/* get the register address - port TC Descriptors Counter Register */
#define PRV_CPSS_DXCH_PORT_TC_DESCR_COUNT_REG_MAC(devNum,portNum,tcQueue,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.tcDescrCounter[tcQueue];\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].tcDescrCounter[tcQueue];

/* get the register address - port TC Buffers Counter Register */
#define PRV_CPSS_DXCH_PORT_TC_BUFFER_COUNT_REG_MAC(devNum,portNum,tcQueue,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.tcBufferCounter[tcQueue];\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].tcBufferCounter[tcQueue];

/*
 * typedef: enum PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT
 *
 * Description: Defines SERDES speed. 
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E  - Serdes speed is 1.25G
 *  PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E - Serdes speed is 3.125G
 *  PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_75_E  - Serdes speed is 3.75G
 *  PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E  - Serdes speed is 6.25G
 *  PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E     - Serdes speed is 5G
 *  PRV_CPSS_DXCH_PORT_SERDES_SPEED_4_25_E  - Serdes speed is 4.25G
 *  PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E    - Last member, the speed is not determind
 */
typedef enum
{
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E,
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E,
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_75_E,
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E,
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E,
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_4_25_E,
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E
} PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT;

/*
 * typedef: enum PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_ENT
 *
 * Description: Defines SERDES reference clock type. 
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_25_SINGLE_ENDED_E - 25 MHz single ended reference clock.
 *  PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_SINGLE_ENDED_E - 125 MHz single ended reference clock.
 *  PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_DIFF_E - 125 MHz differential reference clock.
 *  PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_SINGLE_ENDED_E - 156.25 MHz single ended reference clock.
 *  PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_DIFF_E - 156.25 MHz differential reference clock.
 */
typedef enum
{
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_25_SINGLE_ENDED_E,
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_SINGLE_ENDED_E,
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_DIFF_E,
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_SINGLE_ENDED_E,
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_DIFF_E
} PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_ENT;



/*
 * typedef: enum PRV_CPSS_DXCH_PORT_SERDES_REG_ENT
 *
 * Description: Defines SERDES registers that should be initialized during
 *              serdes init sequence.
 *
 * Enumerations:
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG1_E  - PLL/INTP Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E  - PLL/INTP Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_0_E - PLL/INTP Register 3/0
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_1_E - PLL/INTP Register 3/1
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG4_E  - PLL/INTP Register 4
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG5_E  - PLL/INTP Register 5
 *   PRV_CPSS_DXCH_PORT_SERDES_ANALOG_REG0_E    - Analog Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG1_E - Calibration Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG2_E - Calibration Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_0_E  - Calibration Register 3
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG5_E - Calibration Register 5
 *   PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_E - Transmit Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_E - Transmit Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG2_E - Transmit Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG1_E - Reciever Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG2_E - Reciever Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_E      - FFE Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_SLC_REG_E       - SLC Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_REFERENCE_REG1_E - Reference Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_0_E   - Reset Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_1_E - Callibration Register 3
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG7_E   - Callibration Register 7
 *   PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_1_E - Reset Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_2_E - Reset Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_0_E - Callibration Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_1_E - Callibration Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_2_E - Callibration Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E    - Number of serdes registers
 */
typedef enum
{
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG1_E,        /* 0  */
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E,        /* 1  */
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_0_E,      /* 2  */
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_1_E,      /* 3  */
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG4_E,        /* 4  */
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG5_E,        /* 5  */
    PRV_CPSS_DXCH_PORT_SERDES_ANALOG_REG0_E,          /* 6  */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG1_E,     /* 7  */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG2_E,     /* 8  */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_0_E,   /* 9  */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG5_E,     /* 10 */
    PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_E,        /* 11 */
    PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_E,        /* 12 */
    PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG2_E,        /* 13 */
    PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG1_E,        /* 14 */
    PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG2_E,        /* 15 */
    PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_E,             /* 16 */
    PRV_CPSS_DXCH_PORT_SERDES_SLC_REG_E,              /* 17 */
    PRV_CPSS_DXCH_PORT_SERDES_REFERENCE_REG1_E,       /* 18 */
    PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_0_E,         /* 19 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_1_E,   /* 20 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG7_E,     /* 21 */
    PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_1_E,         /* 22 */
    PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_2_E,         /* 23 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_0_E,   /* 24 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_1_E,   /* 25 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_2_E,   /* 26 */
    PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E               /* 27 */
} PRV_CPSS_DXCH_PORT_SERDES_REG_ENT;

/*
 * typedef: enum PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_ENT
 *
 * Description: Defines SERDES external configuration:
 *              Pre-init and post-init.
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_0_E  - Serdes External Configuration 2/0 Register 
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E    - Serdes External Configuration 1 Register   
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_1_E  - Serdes External Configuration 2/1 Register 
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_2_E  - Serdes External Configuration 2/2 Register 
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_3_E  - Serdes External Configuration 2/3 Register 
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_4_E  - Serdes External Configuration 2/4 Register 
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E     - Number of Serdes External Registers
 */
typedef enum
{
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_0_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_1_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_2_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_3_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_4_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E
} PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_ENT;

extern GT_U32 lpSerdesExtConfig[PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E][2];
extern GT_U32 lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E];
/*******************************************************************************
* prvCpssDxCh3PortSerdesPowerUpDownSet
*
* DESCRIPTION:
*       Set power up or down state to port and serdes.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       powerUp  - power up state:
*                  GT_TRUE  - power up
*                  GT_FALSE - power down
*
* OUTPUTS:
*       oldPowerUpPtr     - (pointer to) old power up state.
*                           GT_TRUE  - power up
*                           GT_FALSE - power down
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3PortSerdesPowerUpDownSet
(
    IN GT_U8         devNum,
    IN GT_U8         portNum,
    IN GT_BOOL       powerUp,
    OUT GT_BOOL      *oldPowerUpPtr
);

/*******************************************************************************
* prvCpssDxChLpPreInitSerdesSequence
*
* DESCRIPTION:
*       Make pre-init sequence for given serdes.
*
* INPUTS:
*       devNum              - physical device number
*       serdesNum           - given SERDES number
*       serdesExtCnfg1Value - value for Serdes External Cnfg 1 register .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpPreInitSerdesSequence
(
    IN  GT_U8                devNum,
    IN  GT_U32               serdesNum,
    IN  GT_U32               serdesExtCnfg1Value
);

/*******************************************************************************
* prvCpssDxChPortMacResetStateSet
*
* DESCRIPTION:
*       Set MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state    - Reset state
*               GT_TRUE   - Port MAC is under Reset
*               GT_FALSE - Port MAC is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
);

/*******************************************************************************
* prvCpssDxChLpCheckAndInitNeededSerdes
*
* DESCRIPTION:
*       Make serdes initialization and enable access to serdes internal registers
*
* INPUTS:
*       devNum             - physical device number
*       startSerdes        - start SERDES number
*       serdesesNum        - number of  SERDES per port
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpCheckAndInitNeededSerdes
(
    IN  GT_U8                devNum,
    IN  GT_U32               startSerdes,
    IN  GT_U32               serdesesNum
);

/*******************************************************************************
* prvCpssDxCh3PortSerdesPowerUpDownGet
*
* DESCRIPTION:
*       Get power up or down state to port and serdes.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       powerUpPtr     - (pointer to) power up state.
*                           GT_TRUE  - power up
*                           GT_FALSE - power down
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3PortSerdesPowerUpDownGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL   *powerUpPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChPorth */

