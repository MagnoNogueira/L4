/*
 * $Id: debug.h,v 1.12 Broadcom SDK $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */

#ifndef _SOC_DEBUG_H
#define _SOC_DEBUG_H

/*
 * These are the possible debug types/flags used by the driver output
 * routine soc_cm_debug().
 */

#define DK_PCI         (1 << 0)    /* PCI reads/writes */
#define DK_SCHAN       (1 << 1)    /* S-Channel operations */
#define DK_SOCMEM      (1 << 2)    /* Memory table operations */
#define DK_SYMTAB      (1 << 3)    /* Symbol parsing routines */
#define DK_VERINET     (1 << 4)    /* Verilog PLI transactions */
#define DK_L3          (1 << 5)    /* L3, DefIP, IPMC and MPLS */
#define DK_INTR        (1 << 6)    /* Interrupt processing */
#define DK_ARL         (1 << 7)    /* ARL DMA and poll routines */
#define DK_ARLMON      (1 << 8)    /* Monitor ARL ins/del */
#define DK_L2TABLE     (1 << 9)    /* Debug software L2 table */
#define DK_DMA         (1 << 10)   /* DMA operations */
#define DK_PORT        (1 << 11)   /* Port operations */
#define DK_PACKET      (1 << 12)   /* Loopback packet data */
#define DK_TX          (1 << 13)   /* Packet transmit */
#define DK_RCLOAD      (1 << 14)   /* Echo cmds before exec */
#define DK_TESTS       (1 << 15)   /* Verbose during tests */
#define DK_VERBOSE     (1 << 16)   /* General verbose output */
#define DK_MIIM        (1 << 17)   /* MIIM register operations */
#define DK_PHY         (1 << 18)   /* PHY operations */
#define DK_END         (1 << 19)   /* Show END driver output */
#define DK_LINK        (1 << 20)   /* Show Link status changes */
#define DK_ERR         (1 << 21)   /* Print errors */
#define DK_COUNTER     (1 << 22)   /* Counter DMA, etc */
#define DK_IP          (1 << 23)   /* IP/Ethernet Stack */
#define DK_STP         (1 << 24)   /* 802.1D Spanning Tree */
#define DK_VLAN        (1 << 25)   /* VLAN gateway */
#define DK_RX          (1 << 26)   /* Packet Receive */
#define DK_WARN        (1 << 27)   /* Print warnings */
#define DK_I2C         (1 << 28)   /* I2C driver */
#define DK_REG         (1 << 29)   /* I2C driver */
#define DK_MEM         (1 << 30)   /* Memory Driver */
#define DK_STK         (1 << 31)   /* Stacking */

/*
 * SOC_DEBUG_PRINT - Debug messages, compiled conditionally under DEBUG_PRINT
 * SOC_ERROR_PRINT - Error messages, compiled conditionally under DEBUG_ERROR
 * SOC_DEBUG_PRINT_IF_ERROR - conditionally compiled in under DEBUG_PRINT, 
 *		calls debug print routine if first parameter if a BCM/SOC 
 *		error.
 */

#define	_SOC_DEBUG_IF(_x, _s)	if ((_x) < 0) soc_cm_debug _s

#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
#   define	SOC_DEBUG_PRINT(_x)		soc_cm_debug _x
#   define	SOC_IF_ERROR_DEBUG_PRINT(_x, _s) _SOC_DEBUG_IF(_x, _s)
#else
#   define	SOC_DEBUG_PRINT(_x)	((void)0)
#   define	SOC_IF_ERROR_DEBUG_PRINT(_x, _s) ((void)(_x))
#endif

#if defined(BROADCOM_DEBUG) || defined(DEBUG_ERROR)
#   define	SOC_ERROR_PRINT(_x)	soc_cm_debug _x
#else
#   define	SOC_ERROR_PRINT(_x)	((void)0)
#endif

/****************************************************************
 *
 * SOC specific debugging flags
 *
 ****************************************************************/

#include <soc/types.h>

#if defined(BROADCOM_DEBUG)

/*
 * These are debug flags to be used with soc_debug_level and
 * the SOC_DEBUG() output macro, below.
 */

#define SOC_DBG_NORMAL     (1 << 0)   /* Normal output */
#define SOC_DBG_ERR        (1 << 1)   /* Print errors */
#define SOC_DBG_WARN       (1 << 2)   /* Print warnings */
#define SOC_DBG_VERBOSE    (1 << 3)   /* General verbose output */
#define SOC_DBG_VVERBOSE   (1 << 4)   /* General verbose output */
#define SOC_DBG_PORT       (1 << 5)   /* Port operations */
#define SOC_DBG_PKT_DATA   (1 << 6)   /* Loopback packet data */
#define SOC_DBG_LINK       (1 << 7)   /* Show Link status changes */
#define SOC_DBG_PCI        (1 << 8)   /* PCI reads/writes */
#define SOC_DBG_SCHAN      (1 << 9)   /* S-Channel operations */
#define SOC_DBG_INTR       (1 << 10)  /* Interrupt processing */
#define SOC_DBG_ARL        (1 << 11)  /* ARL DMA and poll routines */
#define SOC_DBG_DMA        (1 << 12)  /* DMA operations */
#define SOC_DBG_MIIM       (1 << 13)  /* MIIM register operations */
#define SOC_DBG_PHY        (1 << 14)  /* PHY operations */
#define SOC_DBG_COUNTER    (1 << 15)  /* Counter DMA, etc */
#define SOC_DBG_I2C        (1 << 16)  /* I2C driver */
#define SOC_DBG_REG        (1 << 17)  /* Register access */
#define SOC_DBG_MEM        (1 << 18)  /* Memory Driver */
#define SOC_DBG_10G        (1 << 19)  /* 10G port related */
#define SOC_DBG_GE         (1 << 20)  /* FE or GE port related */
#define SOC_DBG_TCAM       (1 << 21)  /* TCAM */

#define SOC_DBG_COUNT      22

#define SOC_DBG_NAMES    \
    "NORmal",            \
    "ERRor",             \
    "WARN",              \
    "VERbose",           \
    "VVERbose",          \
    "POrt",              \
    "PKTdata",           \
    "LInk",              \
    "PCI",               \
    "SChan",             \
    "INTr",              \
    "ARL",               \
    "DMA",               \
    "MIIm",              \
    "PHY",               \
    "CounTeR",           \
    "I2C",               \
    "REG",               \
    "MEM",               \
    "10G",               \
    "GE",                \
    "TCAM"

#if defined(NO_DEBUG_OUTPUT_DEFAULT)
#define SOC_DBG_DEFAULT 0
#else
#define SOC_DBG_DEFAULT (SOC_DBG_ERR | SOC_DBG_WARN | SOC_DBG_NORMAL)
#endif

extern uint32 soc_debug_level;
extern char *soc_debug_names[];

#define SOC_DEBUG_CHECK(flags) (((flags) & soc_debug_level) == (flags))
#define SOC_DEBUG(flags, stuff) \
    if (SOC_DEBUG_CHECK(flags)) soc_cm_print stuff

#else

#define SOC_DEBUG_CHECK(flags) 0
#define SOC_DEBUG(flags, stuff)

#endif   /* defined(BROADCOM_DEBUG) */


#endif	/* !_SOC_DEBUG_H */
