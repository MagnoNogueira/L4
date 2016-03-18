/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDiag.h
*
* DESCRIPTION:
*       General diagnostic definitions and data structures for PP.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __cpssDiagh
#define __cpssDiagh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Typedef: CPSS_DIAG_TEST_PROFILE_ENT
 *
 * Description: Test profile.
 *
 * Enumerations:
 *      CPSS_DIAG_TEST_RANDOM_E            - write random values
 *      CPSS_DIAG_TEST_INCREMENTAL_E       - write incremental values
 *      CPSS_DIAG_TEST_BIT_TOGGLE_E        - toggles all the bits
 *      CPSS_DIAG_TEST_AA_55_E             - write 0xAAAAAAAA/0x55555555
 */
typedef enum
{
    CPSS_DIAG_TEST_RANDOM_E,
    CPSS_DIAG_TEST_INCREMENTAL_E,
    CPSS_DIAG_TEST_BIT_TOGGLE_E,
    CPSS_DIAG_TEST_AA_55_E
}CPSS_DIAG_TEST_PROFILE_ENT;

/*
 * Typedef: CPSS_DIAG_PP_MEM_TYPE_ENT
 *
 * Description: Packet Process memory type.
 *
 * Enumerations:
 *      CPSS_DIAG_PP_MEM_BUFFER_DRAM_E            - The external packet buffer
 *                                                  SDRAM.
 *      CPSS_DIAG_PP_MEM_WIDE_SRAM_E              - The external Wide SRAM, or
 *                                                  internal Control SRAM if the
 *                                                  device was configured 
 *                                                  without external wide-SRAM.
 *      CPSS_DIAG_PP_MEM_NARROW_SRAM_E            - the external Narrow SRAM, or
 *                                                  internal IP SRAM if the
 *                                                  device was configured
 *                                                  without external wide-SRAM.
 *                                                  Not relevant for EX1x6.
 *      CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_0_E - Internal Narrow SRAM 0
 *                                                  (EX1x6 only)
 *      CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_1_E - Internal Narrow SRAM 1
 *                                                  (EX1x6 only)
 *      CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_0_E - External Narrow SRAM 0
 *                                                  (EX1x6 only)
 *      CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_1_E - External Narrow SRAM 1
 *                                                  (EX1x6 only)
 *      CPSS_DIAG_PP_MEM_FLOW_DRAM_E              - The external flow DRAM.
 *      CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E            - Internal MAC table memory.
 *      CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E           - Internal VLAN table memory.
 */
typedef enum
{
    CPSS_DIAG_PP_MEM_BUFFER_DRAM_E,
    CPSS_DIAG_PP_MEM_WIDE_SRAM_E,
    CPSS_DIAG_PP_MEM_NARROW_SRAM_E,
    CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_0_E,
    CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_1_E,
    CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_0_E,
    CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_1_E,
    CPSS_DIAG_PP_MEM_FLOW_DRAM_E,
    CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E,
    CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E
}CPSS_DIAG_PP_MEM_TYPE_ENT;

/*
 * Typedef: CPSS_DIAG_PP_REG_TYPE_ENT
 *
 * Description: Packet Process registers type.
 *
 * Enumerations:
 *      CPSS_DIAG_PP_REG_INTERNAL_E - PP internal register space.
 *      CPSS_DIAG_PP_REG_PCI_CFG_E  - PP pci config and PEX register space.
 */
typedef enum
{
    CPSS_DIAG_PP_REG_INTERNAL_E,
    CPSS_DIAG_PP_REG_PCI_CFG_E
}CPSS_DIAG_PP_REG_TYPE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDiagh */

