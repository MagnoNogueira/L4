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
*       $Revision: 31 $
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

/* The size of XLG ports MAC MIB counters entry in words */
#define PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS 32

/* defines the maximal number of Port Drop Profiles according
   to Cheetah device type */
#define PRV_CPSS_DXCH_PORT_TX_DROP_PROFILE_MAX_MAC(_devNum)                    \
   (((PRV_CPSS_PP_MAC(_devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E )? \
        CPSS_PORT_TX_DROP_PROFILE_8_E : CPSS_PORT_TX_DROP_PROFILE_4_E)

/* check the Drop Profile set range */
#define PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet,devNum) \
        if ((GT_U32)(profileSet) >                                      \
            (GT_U32)PRV_CPSS_DXCH_PORT_TX_DROP_PROFILE_MAX_MAC(devNum)) \
             return GT_BAD_PARAM

/* defines maximal number of Ports, depending on the type of the Cheetah */
#define PRV_CPSS_DXCH_CPU_INDEX_MAC(_devNum) \
        ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E) ? \
         PRV_CPSS_CHEETAH2_MAX_PORTS_NUM_CNS : \
         PRV_CPSS_CHEETAH_MAX_PORTS_NUM_CNS)

/* get the register address - port mac control */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.\
                        macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].\
                            macRegsPerType[portMacType].macCtrl

/* get the register address - port mac control register1 */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs. \
            macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].\
                            macRegsPerType[portMacType].macCtrl1

/* get the register address - port mac control register2 */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.  \
            macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2; \
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum]. \
        macRegsPerType[portMacType].macCtrl2

/* get the register address - port mac control register3 */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,portMacType,regAddrPtr)\
    if((portNum) == CPSS_CPU_PORT_NUM_CNS)              \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.  \
             macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3; \
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum]. \
        macRegsPerType[portMacType].macCtrl3

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
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,0,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TX_CONFIG_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - port Token bucket configuration */
#define PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum,portNum,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,0,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TOKEN_BUCK_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - port , Queue -- Token bucket configuration */
#define PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum,portNum,tcQueue,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,tcQueue,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_PRIO_TOKEN_BUCK_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - port Descriptors Counter Register */
#define PRV_CPSS_DXCH_PORT_DESCR_COUNT_REG_MAC(devNum,portNum,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,0,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_DESCR_COUNTER_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - port Buffers Counter Register */
#define PRV_CPSS_DXCH_PORT_BUFFER_COUNT_REG_MAC(devNum,portNum,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,0,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_BUFFER_COUNTER_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - port TC Descriptors Counter Register */
#define PRV_CPSS_DXCH_PORT_TC_DESCR_COUNT_REG_MAC(devNum,portNum,tcQueue,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,tcQueue,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_DESCR_COUNTER_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - port TC Buffers Counter Register */
#define PRV_CPSS_DXCH_PORT_TC_BUFFER_COUNT_REG_MAC(devNum,portNum,tcQueue,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,tcQueue,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_BUFFER_COUNTER_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - WRR_WEIGHTS_0 Register */
#define PRV_CPSS_DXCH_PORT_WRR_WEIGHTS_0_REG_MAC(devNum,profile,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,profile,0,PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_0_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - WRR_WEIGHTS_1 Register */
#define PRV_CPSS_DXCH_PORT_WRR_WEIGHTS_1_REG_MAC(devNum,profile,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,profile,0,PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_1_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - DESCR_LIMIT Register */
#define PRV_CPSS_DXCH_PORT_WRR_STRICT_PRIO_REG_MAC(devNum,profile,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,profile,0,PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - DESCR_LIMIT_EN Register */
#define PRV_CPSS_DXCH_PORT_WRR_STRICT_PRIO_EN_REG_MAC(devNum,profile,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,profile,0,PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_EN_E,regAddrPtr))\
        return GT_FAIL

/* get the register address - DESCR_LIMIT Register */
#define PRV_CPSS_DXCH_PORT_DESCR_LIMIT_REG_MAC(devNum,profile,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,profile,0,PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_DESCR_LIMIT_E,regAddrPtr))\
        return GT_FAIL

/* defines the maximal number of Scheduler Profiles according
   to Cheetah device type */
#define PRV_CPSS_DXCH_PORT_TX_SCHEDULER_PROFILE_MAX_MAC(_devNum)                \
    (((PRV_CPSS_PP_MAC(_devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E) ? \
     CPSS_PORT_TX_SCHEDULER_PROFILE_8_E : CPSS_PORT_TX_SCHEDULER_PROFILE_4_E)

/* check the Tx Queue scheduler profile set range */
#define PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet,devNum) \
        if ((GT_U32)(profileSet) >                                           \
            (GT_U32)PRV_CPSS_DXCH_PORT_TX_SCHEDULER_PROFILE_MAX_MAC(devNum)) \
             return GT_BAD_PARAM

typedef enum{
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TX_CONFIG_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TOKEN_BUCK_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TOKEN_BUCK_LEN_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_PRIO_TOKEN_BUCK_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_PRIO_TOKEN_BUCK_LEN_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_DESCR_COUNTER_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_BUFFER_COUNTER_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_DESCR_COUNTER_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_BUFFER_COUNTER_E,

    PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_0_E,
    PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_1_E,
    PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_E,
    PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_EN_E,
    PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_DESCR_LIMIT_E
}PRV_CPSS_DXCH_PORT_TX_REG_TYPE_ENT;

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
 *  PRV_CPSS_DXCH_PORT_SERDES_SPEED_2_5_E   - Serdes speed is 2.5G
 *  PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_156_E - Serdes speed is 5.156G (x8 serdes = 40G data speed)
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
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_2_5_E,
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_156_E,
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
 *  PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_INTERNAL_125_E - 125 MHz internal reference clock
 */
typedef enum
{
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_25_SINGLE_ENDED_E,
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_SINGLE_ENDED_E,
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_DIFF_E,
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_SINGLE_ENDED_E,
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_DIFF_E,
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_INTERNAL_125_E
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
 *   PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_1_E - Transmit Register 0 (added for Lion B)
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
    PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_1_E,      /* 11 */
    PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_E,        /* 12 */
    PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_E,        /* 13 */
    PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG2_E,        /* 14 */
    PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG1_E,        /* 15 */
    PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG2_E,        /* 16 */
    PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_E,             /* 17 */
    PRV_CPSS_DXCH_PORT_SERDES_SLC_REG_E,              /* 18 */
    PRV_CPSS_DXCH_PORT_SERDES_REFERENCE_REG1_E,       /* 19 */
    PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_0_E,         /* 20 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_1_E,   /* 21 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG7_E,     /* 22 */
    PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_1_E,         /* 23 */
    PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_2_E,         /* 24 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_0_E,   /* 25 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_1_E,   /* 26 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_2_E,   /* 27 */
    PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E               /* 28 */
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
extern GT_U32 lpSerdesExtConfig_Lion_156Clk[PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E][2];
extern GT_U32 lpSerdesExtConfig_xCat2_125Clk[PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E][2];

extern GT_U32 lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E];
extern GT_U32 lpSerdesConfig_Lion_156Clk[PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E];
extern GT_U32 lpSerdesConfig_xCat2[PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E];

/*******************************************************************************
* prvCpssDxCh3PortSerdesPowerUpDownSet
*
* DESCRIPTION:
*       Set power up or down state to port and serdes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
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
*       portGroupId         - ports group number
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
    IN  GT_U32               portGroupId,
    IN  GT_U32               serdesNum,
    IN  GT_U32               serdesExtCnfg1Value
);

/*******************************************************************************
* prvCpssDxChPortMacResetStateSet
*
* DESCRIPTION:
*       Set MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       portGroupId        - ports group number
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
    IN  GT_U32               portGroupId,
    IN  GT_U32               startSerdes,
    IN  GT_U32               serdesesNum
);

/*******************************************************************************
* prvCpssDxCh3PortSerdesPowerUpDownGet
*
* DESCRIPTION:
*       Get power up or down state to port and serdes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
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

/*******************************************************************************
* prvCpssDxChPortTxRegisterAddrGet
*
* DESCRIPTION:
*       get register address for 'txq per port' .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number.
*       primaryIndex - primary index , can be used as :
*               port number (also CPU port)
*               or as profile number
*       secondaryIndex - secondary index , can be used as TC
*       registerType - register type
* OUTPUTS:
*       regAddrPtr - (pointer to) register address
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTxRegisterAddrGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   primaryIndex,
    IN  GT_U32   secondaryIndex,
    IN  PRV_CPSS_DXCH_PORT_TX_REG_TYPE_ENT  registerType,
    OUT GT_U32   *regAddrPtr
);

/*******************************************************************************
* prvCpssDxChPortPfcCascadeEnableSet
*
* DESCRIPTION:
*       Enable/Disable insertion of DSA tag for PFC frames 
*       transmitted on given port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number, CPU port number.
*       enable     - GT_TRUE - PFC packets are transmitted with a DSA tag.
*                    GT_FALSE - PFC packets are transmitted without DSA tag.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPfcCascadeEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* prvCpssDxChPortNumberOfSerdesLanesGet
*
* DESCRIPTION:
*       Get number of first SERDES and quantity of SERDESes occupied by given
*       port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       firstSerdesPtr         - (pointer to) first used SERDES number
*       numOfSerdesLanesPtr    - (pointer to) quantity of SERDESes occupied
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_SUPPORTED         - wrong device family
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortNumberOfSerdesLanesGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT GT_U32                          *firstSerdesPtr,
    OUT GT_U32                          *numOfSerdesLanesPtr
);

/*******************************************************************************
* prvCpssDxChPortTxQueueingEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this device (0..7)
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTxQueueingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* prvCpssDxChPortEnableSet        
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
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
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* prvCpssDxChPortPcsLoopbackEnableSet
*
* DESCRIPTION:
*       Set the PCS Loopback state in the packet processor MAC port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPcsLoopbackEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* prvCpssDxChPortEnableGet         
*
* DESCRIPTION:
*       Get the Enable/Disable status of a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       statePtr - Pointer to the Get Enable/disable state of the port.
*                  GT_TRUE for enabled port, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *statePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChPorth */

