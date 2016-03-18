/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvExMxUtilLion.h
*
* DESCRIPTION:
*       This file define useful defines for Lion (A,B..) devices
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssDrvExMxUtilLionh
#define __prvCpssDrvExMxUtilLionh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* remove 'const' from (const PRV_CPSS_DRV_INTERRUPT_SCAN_STC *) */
#define REMOVE_CONST_IN_INT_SCAN_TREE_MAC (PRV_CPSS_DRV_INTERRUPT_SCAN_STC *)

/* set values that will be updated/filled during runtime (initialization stage)*/
#define FILLED_IN_RUNTIME_CNS   0

#define INTERRUPT_NUM_MAC(firstIntInReg,bit)           (firstIntInReg + bit)

/* interrupt register for the GE MAC port */
#define GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port)      (0x0A800020  + (port)*0x400)

/* XPCS offset for port 0 */
#define XG_PORT_XPCS_REGS_OFFSET_CNS    0x08800200

/* distance between interrupts of port 0 and 1 --> same distance between any 2 ports */
#define DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)   \
    (PRV_CPSS_##_devType##_XG_PORT1_SUM_XG_XPCS_SUMMARY_E - PRV_CPSS_##_devType##_XG_PORT0_SUM_XG_XPCS_SUMMARY_E)

/* distance between interrupts of lanes in a port --> same distance between any 2 lanes (of a port) */
#define DISTANCE_BETWEEN_LANES_INTERRUPTS_CNS(_devType)   \
    (PRV_CPSS_##_devType##_XG_PORT1_LANE1_SUM_SUM_E - PRV_CPSS_##_devType##_XG_PORT1_LANE0_SUM_SUM_E)

/* GE MAC port leaf  -- overlap bit 1 with XG_MAC_PORT_INTERRUPT_LEAF_MAC */
#define EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(_devType,index,port) \
            { index ,GT_FALSE,0, NULL,GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port) , GE_MAC_PORT_INTERRUPT_CAUSE_MAC(port) + 4,\
             prvCpssDrvHwPpPortGroupIsrRead,                          \
             prvCpssDrvHwPpPortGroupIsrWrite,                         \
             ((port) * DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)) + PRV_CPSS_##_devType##_PORT0_LINK_STATUS_CHANGED_E,  \
             ((port) * DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)) + PRV_CPSS_##_devType##_PORT0_TX_FIFO_UNDERRUN_E,     \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

#define XG_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port)      (0x08800058  + (port)*0x400)


#define XG_PORT_SUMMARY_INTERRUPT_MASK_MAC(port)      (XG_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port) + 4)

/* XG port summary leaf , for ports 1..11 */
#define XG_PORT_1_11_SUMMARY_LEAF_MAC(_devType,port)    \
            {(port), GT_FALSE,0, NULL, XG_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(port) , XG_PORT_SUMMARY_INTERRUPT_MASK_MAC(port),                    \
              prvCpssDrvHwPpPortGroupIsrRead,                                                                     \
              prvCpssDrvHwPpPortGroupIsrWrite,                                                                   \
             ((port) * DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)) + PRV_CPSS_##_devType##_XG_PORT0_SUM_XG_E,          \
             ((port) * DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)) + PRV_CPSS_##_devType##_XG_PORT0_SUM_XPCS_SUMMARY_E,\
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL}

/* XG port summary leaf , for port 0 */
#define XG_PORT_0_SUMMARY_LEAF_MAC(_devType)    \
            {(31), GT_FALSE,0, NULL, XG_PORT_SUMMARY_INTERRUPT_CAUSE_MAC(0) , XG_PORT_SUMMARY_INTERRUPT_MASK_MAC(0),\
              prvCpssDrvHwPpPortGroupIsrRead,   \
              prvCpssDrvHwPpPortGroupIsrWrite, \
             PRV_CPSS_##_devType##_XG_PORT0_SUM_XG_E,          \
             PRV_CPSS_##_devType##_XG_PORT0_SUM_XPCS_SUMMARY_E,\
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 2,NULL, NULL}

/* interrupt register for the XG MAC port */
#define XG_MAC_PORT_INTERRUPT_CAUSE_MAC(port)      (0x08800014  + (port)*0x400)

/* XG MAC port leaf -- overlap bit 1 with EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC */
#define XG_MAC_PORT_INTERRUPT_LEAF_MAC(_devType,port) \
            { 1 ,GT_FALSE,0, NULL,XG_MAC_PORT_INTERRUPT_CAUSE_MAC(port) , XG_MAC_PORT_INTERRUPT_CAUSE_MAC(port) + 4,\
             prvCpssDrvHwPpPortGroupIsrRead,                          \
             prvCpssDrvHwPpPortGroupIsrWrite,                         \
             ((port) * DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)) + PRV_CPSS_##_devType##_XG_PORT0_LINK_STATUS_CHANGED_E,  \
             ((port) * DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)) + PRV_CPSS_##_devType##_XG_PORT0_XG_COUNT_EXPIRED_E + 1, \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}

/* interrupt register for the XG port XPCS interrupts */
#define XG_PORT_XPCS_INTERRUPT_CAUSE_MAC(port)      (XG_PORT_XPCS_REGS_OFFSET_CNS + 0x14  + (port)*0x400)

/* XG port XPCS leaf */
#define XG_PORT_XPCS_INTERRUPT_LEAF_MAC(_devType,port) \
            { 2 ,GT_FALSE,0, NULL,XG_PORT_XPCS_INTERRUPT_CAUSE_MAC(port) , XG_PORT_XPCS_INTERRUPT_CAUSE_MAC(port) + 4,\
             prvCpssDrvHwPpPortGroupIsrRead,                          \
             prvCpssDrvHwPpPortGroupIsrWrite,                         \
             ((port) * DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)) + PRV_CPSS_##_devType##_XPCS_PORT0_LINK_STATUS_CHANGED_E,  \
             ((port) * DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)) + PRV_CPSS_##_devType##_XPCS_PORT0_LANE5_E,     \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 6,NULL, NULL}

/* interrupt register for the XG port XPCS lane interrupts */
#define XG_PORT_XPCS_LANE_INTERRUPT_CAUSE_MAC(port,lane)      (XG_PORT_XPCS_REGS_OFFSET_CNS + 0x60 + (port)*0x400 + (lane)*0x44)

/* XG port XPCS lane leaf */
#define XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(_devType,port,lane) \
            { 7 + (lane) ,GT_FALSE,0, NULL,XG_PORT_XPCS_LANE_INTERRUPT_CAUSE_MAC(port,lane) , XG_PORT_XPCS_LANE_INTERRUPT_CAUSE_MAC(port,lane) + 4,\
             prvCpssDrvHwPpPortGroupIsrRead,                          \
             prvCpssDrvHwPpPortGroupIsrWrite,                         \
             ((port) * DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)) + ((lane) * DISTANCE_BETWEEN_LANES_INTERRUPTS_CNS(_devType)) + PRV_CPSS_##_devType##_XG_PORT0_LANE0_SUM_PRBS_ERROR_E,  \
             ((port) * DISTANCE_BETWEEN_PORTS_INTERRUPTS_CNS(_devType)) + ((lane) * DISTANCE_BETWEEN_LANES_INTERRUPTS_CNS(_devType)) + PRV_CPSS_##_devType##_XG_PORT0_LANE0_SUM_DETECTED_IIAII_E,     \
             FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0,NULL, NULL}


/* the sub tree of interrupts that relate to the XG port , without the summary register */
#define XG_PORT_SUB_TREE_MAC(_devType,port)                          \
/*      EMULATED_GE_MAC_PORT_INTERRUPT_LEAF_MAC(port), bit 1 - hold both registers for XG,GE interrupts */        \
        XG_MAC_PORT_INTERRUPT_LEAF_MAC(_devType,port),  /* bit 1 - hold both registers for XG,GE interrupts */    \
        XG_PORT_XPCS_INTERRUPT_LEAF_MAC(_devType,port), /* bit 2 , has 6 'childs' --> per lane interrupts */      \
            XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(_devType,port,0), /* lane 0 */  \
            XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(_devType,port,1), /* lane 1 */  \
            XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(_devType,port,2), /* lane 2 */  \
            XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(_devType,port,3), /* lane 3 */  \
            XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(_devType,port,4), /* lane 4 */  \
            XG_PORT_XPCS_LANE_INTERRUPT_LEAF_MAC(_devType,port,5)  /* lane 5 */

/* all the sub tree of interrupts that relate to the XG port 0 */
#define XG_PORT_0_SUB_TREE_MAC(_devType)            \
        XG_PORT_0_SUMMARY_LEAF_MAC(_devType),       \
            XG_PORT_SUB_TREE_MAC(_devType,0)

/* all the sub tree of interrupts that relate to the XG port 1..11 */
#define XG_PORT_1_11_SUB_TREE_MAC(_devType,port)      \
        XG_PORT_1_11_SUMMARY_LEAF_MAC(_devType,port), \
            XG_PORT_SUB_TREE_MAC(_devType,port)


/* XLG registers match addresses of port 12 according to 0x400 offset from 0x08800000
   0x08800000 + (12*0x400)  = 0x08803000 */
#define LION_B_XLG_PORT_NUM_CNS    12

/* interrupt register for the XLG port XPCS interrupts */
#define XLG_PORT_XPCS_INTERRUPT_CAUSE_MAC(port)     (XG_PORT_XPCS_REGS_OFFSET_CNS  + (port)*0x400)
#define XLG_PORT_XPCS_INTERRUPT_MASK_MAC(port)      (XLG_PORT_XPCS_INTERRUPT_CAUSE_MAC(port) + 4)

/* interrupt register for the XLG port XPCS GEN interrupts */
#define XLG_PORT_XPCS_GEN_INTERRUPT_CAUSE_MAC(port)   (XG_PORT_XPCS_REGS_OFFSET_CNS + 0x8  + (port)*0x400 )
#define XLG_PORT_XPCS_GEN_INTERRUPT_MASK_MAC(port)    (XLG_PORT_XPCS_GEN_INTERRUPT_CAUSE_MAC(port) + 4)

/* interrupt register for the XLG port XPCS lane interrupts */
#define XLG_PORT_XPCS_LANE_INTERRUPT_CAUSE_MAC(port,lane)      (XG_PORT_XPCS_REGS_OFFSET_CNS + 0xD0 + (port)*0x400 + (lane)*0x8)
/* mask interrupt register for the XLG port XPCS lane interrupts */
#define XLG_PORT_XPCS_LANE_INTERRUPT_MASK_MAC(port,lane)      (XLG_PORT_XPCS_LANE_INTERRUPT_CAUSE_MAC(port,lane) + 0x4)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvExMxUtilLionh */

