/*
 * $Id: defs.h,v 1.94.6.8.2.1 Broadcom SDK $
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
 *
 * File:        defs.h
 * Purpose:     Basic defines for system and chips
 *              This should be included before all other SOC include
 *              files to make sure BCM_xxxx is defined for them.
 */

#ifndef _SOC_DEFS_H
#define _SOC_DEFS_H

#include <soc/mcm/allenum.h>
#include <soc/mcm/robo/allenum.h>
#include <soc/sbx/allenum.h>
#include <soc/maxdef.h>

/* max number of switch devices */
#define SOC_MAX_NUM_SWITCH_DEVICES  16
/* max number of ethernet devices */
#define SOC_MAX_NUM_ETHER_DEVICES   2
/* max number of all kinds of devices */
#define SOC_MAX_NUM_DEVICES         (SOC_MAX_NUM_SWITCH_DEVICES +\
                     SOC_MAX_NUM_ETHER_DEVICES)

/*
 * BCM Chip Support
 *
 * There are a few classes of defines used throughout the code:
 * - chip number defines with revsion numbers (from soc/allenum.h,
 *   but it can be modified from $SDK/make/Make.local)
 * - chip number defines without revision numbers
 * - chip type or group defines (such as BCM_*_SUPPORT)
 * - component support defines (GBP or FILTER)
 */
#if /* defined(BCM_5600_B0) || */ defined(BCM_5600_C0)
#define BCM_5600
#endif

#if /* defined(BCM_5680_A0) || */ defined(BCM_5680_B0)
#define BCM_5680
#endif

#if defined(BCM_5605_A0)
#define BCM_5605
#endif

#if defined(BCM_5615_A0)
#define BCM_5615
#endif

#if defined(BCM_5665_A0)
#define BCM_5665
#define BCM_TUCANA_SUPPORT
#endif

#if defined(BCM_5665_B0)
#define BCM_5665
#define BCM_TUCANA_SUPPORT
#endif

#if defined(BCM_5650_C0)
#define BCM_5665
#define BCM_TUCANA_SUPPORT

#endif

#if defined(BCM_5670_A0)
#define BCM_5670
#define BCM_HERCULES1_SUPPORT
#define BCM_HERCULES_SUPPORT
#endif

#if defined(BCM_5673_A0)
#define BCM_5673
#define BCM_LYNX1_SUPPORT
#define BCM_LYNX_SUPPORT
#endif

#if defined(BCM_5674_A0)
#define BCM_5674
#define BCM_LYNX15_SUPPORT
#define BCM_LYNX_SUPPORT
#endif

#if defined(BCM_5690_A0)
#define BCM_5690
#define BCM_DRACO1_SUPPORT
#define BCM_DRACO_SUPPORT
#endif

#if defined(BCM_5695_A0)
#define BCM_5695
#define BCM_5690
#define BCM_DRACO15_SUPPORT
#define BCM_DRACO_SUPPORT
#endif

#if defined(BCM_5675_A0)
#define BCM_5675
#define BCM_5670
#define BCM_HERCULES15_SUPPORT
#define BCM_HERCULES_SUPPORT
#endif

#if defined(BCM_56601_A0) || defined(BCM_56601_B0) || defined(BCM_56601_C0)
#define BCM_56601
#define BCM_EASYRIDER_SUPPORT
#define BCM_MPLS_SUPPORT
#endif

#if defined(BCM_56602_A0) || defined(BCM_56602_B0) || defined(BCM_56602_C0)
#define BCM_56602
#define BCM_EASYRIDER_SUPPORT
#define BCM_MPLS_SUPPORT
#endif

#if defined(BCM_56504_A0) || defined(BCM_56504_B0)
#define BCM_56504
#define BCM_FIREBOLT_SUPPORT
#endif

#if defined(BCM_56102_A0)
#define BCM_56102
#define BCM_FIREBOLT_SUPPORT
#define BCM_FELIX_SUPPORT
#define BCM_FELIX1_SUPPORT
#endif

#if defined(BCM_56304_B0)
#define BCM_56304
#define BCM_FIREBOLT_SUPPORT
#define BCM_HELIX_SUPPORT
#define BCM_HELIX1_SUPPORT
#define BCM_MIRAGE_SUPPORT
#endif

#if defined(BCM_56112_A0)
#define BCM_56112
#define BCM_FIREBOLT_SUPPORT
#define BCM_FELIX_SUPPORT
#define BCM_FELIX15_SUPPORT
#endif

#if defined(BCM_56314_A0)
#define BCM_56314
#define BCM_FIREBOLT_SUPPORT
#define BCM_HELIX_SUPPORT
#define BCM_HELIX15_SUPPORT
#define BCM_MIRAGE_SUPPORT
#endif

#if defined(BCM_56580_A0)
#define BCM_56580
#define BCM_FIREBOLT_SUPPORT
#define BCM_BRADLEY_SUPPORT
#define BCM_GOLDWING_SUPPORT
#endif

#if defined(BCM_56700_A0)
#define BCM_56700
#define BCM_FIREBOLT_SUPPORT
#define BCM_BRADLEY_SUPPORT
#define BCM_HUMV_SUPPORT
#endif

#if defined(BCM_56800_A0)
#define BCM_56800
#define BCM_56304
#define BCM_FIREBOLT_SUPPORT
#define BCM_BRADLEY_SUPPORT
#endif

#if defined(BCM_56218_A0)
#define BCM_56218
#define BCM_FIREBOLT_SUPPORT
#define BCM_HELIX_SUPPORT
#define BCM_RAPTOR_SUPPORT
#endif

#if defined(BCM_56514_A0)
#define BCM_56514
#define BCM_FIREBOLT_SUPPORT
#define BCM_FIREBOLT2_SUPPORT
#endif

#if defined(BCM_56624_A0)
#define BCM_56624
#define BCM_56304
#define BCM_56800
#define BCM_FIREBOLT_SUPPORT
#define BCM_BRADLEY_SUPPORT
#define BCM_TRIUMPH_SUPPORT
#define BCM_TRX_SUPPORT
#endif

#if defined(BCM_56680_A0)
#define BCM_56680
#define BCM_56624
#define BCM_56304
#define BCM_56800
#define BCM_FIREBOLT_SUPPORT
#define BCM_BRADLEY_SUPPORT
#define BCM_TRIUMPH_SUPPORT
#define BCM_VALKYRIE_SUPPORT
#define BCM_TRX_SUPPORT
#endif

#if defined(BCM_56224_A0) || defined(BCM_56224_B0)
#define BCM_56224
#define BCM_FIREBOLT_SUPPORT
#define BCM_HELIX_SUPPORT
#define BCM_RAPTOR_SUPPORT
#define BCM_RAVEN_SUPPORT
#endif

#if defined(BCM_QE2000_A1) || defined(BCM_QE2000_A2) \
  || defined(BCM_QE2000_A3) || defined(BCM_QE2000_A4)
#define BCM_QE2000
#define BCM_QE2000_SUPPORT
#endif

#if defined(BCM_BME3200_B0) || defined(BCM_BME3200_A0)
#define BCM_BME3200
#define BCM_BME3200_SUPPORT
#endif

#if defined(BCM_BM9600_B0) || defined(BCM_BM9600_A0)
#define BCM_BM9600
#define BCM_BM9600_SUPPORT
#endif

#if defined(BCM_FE2000_A0)
#define BCM_FE2000
#define BCM_FIELD_SUPPORT
#if !defined(BCM_FE2000_SUPPORT)
#define BCM_FE2000_SUPPORT
#endif
#endif

#if defined(BCM_56820_A0)
#define BCM_56820
#define BCM_56800
#define BCM_56304
#define BCM_FIREBOLT_SUPPORT
#define BCM_BRADLEY_SUPPORT
#define BCM_SCORPION_SUPPORT
#define BCM_TRX_SUPPORT
#endif

#if defined(BCM_56725_A0)
#define BCM_56725
#define BCM_56820
#define BCM_56800
#define BCM_56304
#define BCM_FIREBOLT_SUPPORT
#define BCM_BRADLEY_SUPPORT
#define BCM_HUMV_SUPPORT
#define BCM_SCORPION_SUPPORT
#define BCM_CONQUEROR_SUPPORT
#define BCM_TRX_SUPPORT
#endif

#if defined(BCM_56520_A0)
#define BCM_56520
#define BCM_56624
#define BCM_56304
#define BCM_56800
#define BCM_FIREBOLT_SUPPORT
#define BCM_BRADLEY_SUPPORT
#define BCM_TRIUMPH_SUPPORT
#define BCM_CHARIOT_SUPPORT
#define BCM_TRX_SUPPORT
#endif

/*
 * Strata 1 or Strata 2
 */
#if defined(BCM_5600) || defined(BCM_5680)
#define BCM_STRATA1_SUPPORT
#endif

#if defined(BCM_5605) || defined(BCM_5615)
#define BCM_STRATA2_SUPPORT
#endif

#if defined(BCM_STRATA1_SUPPORT) || defined(BCM_STRATA2_SUPPORT)
#define BCM_STRATA_SUPPORT
#endif

/*
 * Strata XGS
 */
#if defined(BCM_5673) || defined(BCM_5690)
#define BCM_XGS1_SWITCH_SUPPORT
#endif

#if defined(BCM_5665) || defined(BCM_5695) || defined(BCM_5674)
#define BCM_XGS2_SWITCH_SUPPORT
#endif

#if defined(BCM_56601) || defined(BCM_56602) || defined(BCM_56504) || \
    defined(BCM_56102) || defined(BCM_56304) || defined(BCM_56218) || \
    defined(BCM_56112) || defined(BCM_56314) || defined(BCM_56514) || \
    defined(BCM_56580) || defined(BCM_56700) || defined(BCM_56800) || \
    defined(BCM_56624) || defined(BCM_56224) || defined(BCM_56820) || \
    defined(BCM_56680) || defined(BCM_56725) || \
    defined(BCM_56520)
#define BCM_XGS3_SWITCH_SUPPORT
#endif

#if defined(BCM_5670)
#define BCM_XGS1_FABRIC_SUPPORT
#endif

#if defined(BCM_5675)
#define BCM_XGS2_FABRIC_SUPPORT
#endif

#if defined(BCM_56700) || defined(BCM_56701) || defined(BCM_56725)
#define BCM_XGS3_FABRIC_SUPPORT
#endif

#if defined(BCM_XGS1_SWITCH_SUPPORT) || defined(BCM_XGS2_SWITCH_SUPPORT)
#define BCM_XGS12_SWITCH_SUPPORT
#endif

#if defined(BCM_XGS12_SWITCH_SUPPORT) || defined(BCM_XGS3_SWITCH_SUPPORT)
#define BCM_XGS_SWITCH_SUPPORT
#endif

#if defined(BCM_XGS1_FABRIC_SUPPORT) || defined(BCM_XGS2_FABRIC_SUPPORT)
#define BCM_XGS12_FABRIC_SUPPORT
#endif

#if defined(BCM_XGS12_SWITCH_SUPPORT) || defined(BCM_XGS12_FABRIC_SUPPORT)
#define BCM_XGS12_SUPPORT
#endif

#if defined(BCM_XGS12_FABRIC_SUPPORT) || defined(BCM_XGS3_FABRIC_SUPPORT)
#define BCM_XGS_FABRIC_SUPPORT
#endif

#if defined(BCM_XGS_SWITCH_SUPPORT) || defined(BCM_XGS_FABRIC_SUPPORT)
#ifdef BCM_XGS_SUPPORT
#undef BCM_XGS_SUPPORT
#endif
#define BCM_XGS_SUPPORT
#endif

#if defined(BCM_XGS_SWITCH_SUPPORT) || defined(BCM_STRATA_SUPPORT)
#define BCM_SWITCH_SUPPORT
#endif

/*
 * SBX
 */
#if defined(BCM_QE2000) || defined(BCM_BME3200) || defined(BCM_BM9600)
#ifndef BCM_SBX_SUPPORT
#define BCM_SBX_SUPPORT
#endif
#endif

/*
 * If any supported chip includes a Global Buffer Pool (GBP)
 */
#if defined(BCM_5600) || defined(BCM_5605) || defined(BCM_5615)
#define BCM_GBP_SUPPORT
#endif


#if defined(BCM_XGS3_SWITCH_SUPPORT)
#define BCM_FIELD_SUPPORT
#endif

/*
 * If any supported chip includes a Fast Filter Processor (FFP)
 * or Field Processor (FP)
 */
#if defined(BCM_STRATA_SUPPORT) || defined(BCM_XGS1_SWITCH_SUPPORT) || \
    defined(BCM_XGS2_SWITCH_SUPPORT)
#define BCM_FILTER_SUPPORT 
#endif

/*
 * If any supported chip includes Broadsafe core
 */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_BSAFE)
#define BCM_BSAFE_SUPPORT
#endif

#if defined(INCLUDE_ACL)
#define BCM_ACL_SUPPORT
#endif

/*
 * If any supported chip supports Higig2
 */
#if defined(BCM_56580) || defined(BCM_56700) || defined(BCM_56800) || \
    defined(BCM_56218) || defined(BCM_56624) || defined(BCM_56224) || \
    defined(BCM_56820) || defined(BCM_56680) || defined(BCM_56725) || \
    defined(BCM_53314) || defined(BCM_56520)
#define BCM_HIGIG2_SUPPORT
#endif

/*
 * If any supported chip has GXPORTS
 */
#if defined(BCM_56580) || defined(BCM_56700) || defined(BCM_56800) || \
    defined(BCM_56624) || defined(BCM_56820) || defined(BCM_56680) || \
    defined(BCM_56725) || defined(BCM_56520)
#define BCM_GXPORT_SUPPORT
#endif

/*
 * If any supported chip includes metering capability
 */
#if defined(BCM_STRATA2_SUPPORT) || defined(BCM_XGS_SWITCH_SUPPORT)
#define BCM_METER_SUPPORT        /* Metering support */
#endif

/*
 * If any supported chip includes storm control (bandwidth rate) capability
 */
#if defined(BCM_56218) || defined(BCM_56224)
#define BCM_BANDWIDTH_RATE_METER
#endif

#if defined(BCM_LYNX_SUPPORT)
#define BCM_DMUX_SUPPORT
#endif

/*
 * If any supported chip has BigMAC ports
 */
#if defined(BCM_XGS12_SUPPORT) || \
    defined(BCM_56601) || defined(BCM_56602) || defined(BCM_56504) || \
    defined(BCM_56102) || defined(BCM_56304) || defined(BCM_56112) || \
    defined(BCM_56314) || defined(BCM_56580) || defined(BCM_56700) || \
    defined(BCM_56800) || defined(BCM_56514) || defined(BCM_56624) || \
    defined(BCM_56680) || defined(BCM_56820) || defined(BCM_56725)
#define BCM_BIGMAC_SUPPORT
#endif

/*
 * If any supported chip has UniMAC ports
 */
#if defined(BCM_56224) || defined(BCM_56624) || defined(BCM_56820) || \
    defined(BCM_56680) || defined(BCM_56725) || \
    defined(BCM_56520) || defined(BCM_FE2000)
#define BCM_UNIMAC_SUPPORT
#endif

/*
 * If any supported chip includes an LED Processor
 */

#if defined(BCM_5605)  || defined(BCM_5615)  || \
    defined(BCM_5665)  || defined(BCM_5670)  || \
    defined(BCM_5673)  || defined(BCM_5675)  || \
    defined(BCM_5690)  || defined(BCM_5695)  || \
    defined(BCM_56601) || defined(BCM_56602) || \
    defined(BCM_5674)  || defined(BCM_56504) || \
    defined(BCM_56102) || defined(BCM_56304) || \
    defined(BCM_56112) || defined(BCM_56314) || \
    defined(BCM_56580) || defined(BCM_56700) || \
    defined(BCM_56800) || defined(BCM_56218) || \
    defined(BCM_56514) || defined(BCM_56624) || \
    defined(BCM_56224) || defined(BCM_56820) || \
    defined(BCM_56680) || \
    defined(BCM_56725) || defined(BCM_56520)
#define BCM_LEDPROC_SUPPORT
#endif
#if defined(BCM_5324_A0)
#define BCM_5324
#endif

#if defined(BCM_5324_A1)
#define BCM_5324
#endif


#if defined(BCM_5396_A0)
#define BCM_5396
#endif

#if defined(BCM_5389_A0)
#define BCM_5389
#endif

#if defined(BCM_5398_A0)
#define BCM_5398
#endif

#if defined(BCM_5348_A0)
#define BCM_5348
#define BCM_FIELD_SUPPORT
#endif

#if defined(BCM_5397_A0)
#define BCM_5397
#endif

#if defined(BCM_5347_A0)
#define BCM_5347
#define BCM_FIELD_SUPPORT
#endif
#if defined(BCM_5395_A0)
#define BCM_5395
#define BCM_FIELD_SUPPORT
#define BCM_MAC2V_SUPPORT
#define BCM_EAV_SUPPORT
#endif

#if defined(BCM_53242_A0)
#define BCM_53242
#define BCM_FIELD_SUPPORT
/* Below used in ROBO chip only. No exist in ESW */
#define BCM_V2V_SUPPORT
#define BCM_MAC2V_SUPPORT
#define BCM_PROTOCOL2V_SUPPORT
/* not supported yet. 
#define BCM_EGRV2V_SUPPORT 
*/
#endif

#if defined(BCM_53262_A0)
#define BCM_53262
#define BCM_FIELD_SUPPORT
/* Below used in ROBO chip only. No exist in ESW */
#define BCM_V2V_SUPPORT
#define BCM_MAC2V_SUPPORT
#define BCM_PROTOCOL2V_SUPPORT
/* not supported yet. 
#define BCM_EGRV2V_SUPPORT 
*/
#endif

#if defined(BCM_53115_A0)
#define BCM_53115
#define BCM_FIELD_SUPPORT
#define BCM_V2V_SUPPORT
#endif

#if defined(BCM_53118_A0)
#define BCM_53118
#define BCM_EAV_SUPPORT
#endif

#if defined(INCLUDE_ACL)
#define BCM_ACL_SUPPORT
#endif

#if defined(BCM_XGS_SUPPORT) || defined(BCM_TUCANA_SUPPORT)
#ifndef BCM_ESW_SUPPORT
#define BCM_ESW_SUPPORT
#endif
#endif

#if defined(INCLUDE_RCPU)
#define BCM_RCPU_SUPPORT
#endif

#if defined(INCLUDE_OOB_RCPU)
#define BCM_OOB_RCPU_SUPPORT
#endif

    
/*
 * ROBO chips
 */
#if defined(BCM_5324) || defined(BCM_5396) || defined(BCM_5389) || defined(BCM_5398) || \
     defined(BCM_5348) || defined(BCM_5397) || defined(BCM5347) ||  defined(BCM_5395) ||\
     defined(BCM_53242) || defined (BCM_53262) || defined (BCM_53115) || defined (BCM_53118)
#ifndef BCM_ROBO_SUPPORT
#define BCM_ROBO_SUPPORT
#endif
#endif




/****************************************************************
 *
 * This is a list of all known chips which may or may not be supported
 * by a given image.
 *
 * Use soc_chip_supported() for which are supported
 * CHANGE soc_chip_type_map if you change this
 *
 * See also socchip.h
 *
 * Enumerated types include:
 *   Unit numbers
 *   Chip type numbers (specific chip references w/ rev id)
 *   Chip group numbers (groups of chips, usually dropping revision
 *        number)
 *
 * All of the above are 0-based.
 *
 ****************************************************************/

typedef enum soc_chip_types_e {
    SOC_CHIP_BCM5600_C0,
    SOC_CHIP_BCM5680_B0,
    SOC_CHIP_BCM5605_A0,
    SOC_CHIP_BCM5615_A0,
    SOC_CHIP_BCM5670_A0,
    SOC_CHIP_BCM5673_A0,
    SOC_CHIP_BCM5690_A0,
    SOC_CHIP_BCM5665_A0,
    SOC_CHIP_BCM5695_A0,
    SOC_CHIP_BCM5675_A0,
    SOC_CHIP_BCM5674_A0,
    SOC_CHIP_BCM5665_B0,
    SOC_CHIP_BCM56601_A0,
    SOC_CHIP_BCM56601_B0,
    SOC_CHIP_BCM56601_C0,
    SOC_CHIP_BCM56602_A0,
    SOC_CHIP_BCM56602_B0,
    SOC_CHIP_BCM56602_C0,
    SOC_CHIP_BCM56504_A0,
    SOC_CHIP_BCM56504_B0,
    SOC_CHIP_BCM56102_A0,
    SOC_CHIP_BCM56304_B0,
    SOC_CHIP_BCM56112_A0,
    SOC_CHIP_BCM56314_A0,
    SOC_CHIP_BCM5650_C0,
    SOC_CHIP_BCM56800_A0,
    SOC_CHIP_BCM56218_A0,
    SOC_CHIP_BCM56514_A0,
    SOC_CHIP_BCM56624_A0,
    SOC_CHIP_BCM56680_A0,
    SOC_CHIP_BCM56224_A0,
    SOC_CHIP_BCM56224_B0,
    SOC_CHIP_BCM56725_A0,
    SOC_CHIP_BCM56820_A0,
    SOC_CHIP_BCM56520_A0,
    SOC_CHIP_BCM5324_A0,
    SOC_CHIP_BCM5324_A1,
    SOC_CHIP_BCM5324_A2,
    SOC_CHIP_BCM5396_A0,
    SOC_CHIP_BCM5389_A0,
    SOC_CHIP_BCM5398_A0,
    SOC_CHIP_BCM5348_A0,
    SOC_CHIP_BCM5397_A0,
    SOC_CHIP_BCM5347_A0,
    SOC_CHIP_BCM5395_A0,
    SOC_CHIP_BCM53242_A0,
    SOC_CHIP_BCM53262_A0,
    SOC_CHIP_BCM53115_A0,
    SOC_CHIP_BCM53118_A0,
    SOC_CHIP_BCM4713_A0,
    SOC_CHIP_QE2000_A4,
    SOC_CHIP_BME3200_A0,
    SOC_CHIP_BME3200_B0,
    SOC_CHIP_BM9600_A0,
    SOC_CHIP_BM9600_B0,
    SOC_CHIP_BCM88020_A0, /* FE2000_A0 */
    SOC_CHIP_BCM88020_A1, /* FE2000_A1 */
    SOC_CHIP_TYPES_COUNT
} soc_chip_types;

/****************************************************************
 * 
 * NB: Order of this array must match soc_chip_types_e above.
 *
 ****************************************************************/
#define SOC_CHIP_TYPE_MAP_INIT { \
    /* SOC_CHIP_BCM5600_C0,  */ SOC_CHIP_BCM5600,  \
    /* SOC_CHIP_BCM5680_B0,  */ SOC_CHIP_BCM5680,  \
    /* SOC_CHIP_BCM5605_A0,  */ SOC_CHIP_BCM5605,  \
    /* SOC_CHIP_BCM5615_A0,  */ SOC_CHIP_BCM5615,  \
    /* SOC_CHIP_BCM5670_A0,  */ SOC_CHIP_BCM5670,  \
    /* SOC_CHIP_BCM5673_A0,  */ SOC_CHIP_BCM5673,  \
    /* SOC_CHIP_BCM5690_A0,  */ SOC_CHIP_BCM5690,  \
    /* SOC_CHIP_BCM5665_A0,  */ SOC_CHIP_BCM5665,  \
    /* SOC_CHIP_BCM5695_A0,  */ SOC_CHIP_BCM5695,  \
    /* SOC_CHIP_BCM5675_A0,  */ SOC_CHIP_BCM5675,  \
    /* SOC_CHIP_BCM5674_A0,  */ SOC_CHIP_BCM5674,  \
    /* SOC_CHIP_BCM5665_B0,  */ SOC_CHIP_BCM5665,  \
    /* SOC_CHIP_BCM56601_A0, */ SOC_CHIP_BCM56601, \
    /* SOC_CHIP_BCM56601_B0, */ SOC_CHIP_BCM56601, \
    /* SOC_CHIP_BCM56601_C0, */ SOC_CHIP_BCM56601, \
    /* SOC_CHIP_BCM56602_A0, */ SOC_CHIP_BCM56602, \
    /* SOC_CHIP_BCM56602_B0, */ SOC_CHIP_BCM56602, \
    /* SOC_CHIP_BCM56602_C0, */ SOC_CHIP_BCM56602, \
    /* SOC_CHIP_BCM56504_A0, */ SOC_CHIP_BCM56504, \
    /* SOC_CHIP_BCM56504_B0, */ SOC_CHIP_BCM56504, \
    /* SOC_CHIP_BCM56102_A0, */ SOC_CHIP_BCM56102, \
    /* SOC_CHIP_BCM56304_B0, */ SOC_CHIP_BCM56304, \
    /* SOC_CHIP_BCM56112_A0, */ SOC_CHIP_BCM56112, \
    /* SOC_CHIP_BCM56314_A0, */ SOC_CHIP_BCM56314, \
    /* SOC_CHIP_BCM5650_C0,  */ SOC_CHIP_BCM5650,  \
    /* SOC_CHIP_BCM56800_A0, */ SOC_CHIP_BCM56800, \
    /* SOC_CHIP_BCM56218_A0, */ SOC_CHIP_BCM56218, \
    /* SOC_CHIP_BCM56514_A0, */ SOC_CHIP_BCM56514,\
    /* SOC_CHIP_BCM56624_A0, */ SOC_CHIP_BCM56624, \
    /* SOC_CHIP_BCM56680_A0, */ SOC_CHIP_BCM56680, \
    /* SOC_CHIP_BCM56224_A0, */ SOC_CHIP_BCM56224, \
    /* SOC_CHIP_BCM56224_B0, */ SOC_CHIP_BCM56224, \
    /* SOC_CHIP_BCM56725_A0, */ SOC_CHIP_BCM56725, \
    /* SOC_CHIP_BCM56820_A0, */ SOC_CHIP_BCM56820, \
    /* SOC_CHIP_BCM56520_A0, */ SOC_CHIP_BCM56520, \
    /* SOC_CHIP_BCM5324_A0,  */ SOC_CHIP_BCM5324,  \
    /* SOC_CHIP_BCM5324_A1,  */ SOC_CHIP_BCM5324,  \
    /* SOC_CHIP_BCM5324_A2,  */ SOC_CHIP_BCM5324,  \
    /* SOC_CHIP_BCM5396_A0,  */ SOC_CHIP_BCM5396,  \
    /* SOC_CHIP_BCM5389_A0,  */ SOC_CHIP_BCM5389,  \
    /* SOC_CHIP_BCM5398_A0,  */ SOC_CHIP_BCM5398,  \
    /* SOC_CHIP_BCM5348_A0,  */ SOC_CHIP_BCM5348,  \
    /* SOC_CHIP_BCM5397_A0,  */ SOC_CHIP_BCM5397,  \
    /* SOC_CHIP_BCM5347_A0,  */ SOC_CHIP_BCM5347,  \
    /* SOC_CHIP_BCM5395_A0,  */ SOC_CHIP_BCM5395,  \
    /* SOC_CHIP_BCM53242_A0, */ SOC_CHIP_BCM53242, \
    /* SOC_CHIP_BCM53262_A0, */ SOC_CHIP_BCM53262, \
    /* SOC_CHIP_BCM53115_A0, */ SOC_CHIP_BCM53115, \
    /* SOC_CHIP_BCM53118_A0, */ SOC_CHIP_BCM53118, \
    /* SOC_CHIP_BCM4713_A0,  */ SOC_CHIP_BCM4713,  \
    /* SOC_CHIP_QE2000_A4,   */ SOC_CHIP_QE2000,   \
    /* SOC_CHIP_BME3200_A0,  */ SOC_CHIP_BME3200,  \
    /* SOC_CHIP_BME3200_B0,  */ SOC_CHIP_BME3200,  \
    /* SOC_CHIP_BM9600_A0,   */ SOC_CHIP_BM9600,  \
    /* SOC_CHIP_BM9600_B0,   */ SOC_CHIP_BM9600,  \
    /* SOC_CHIP_FE2000_A0,   */ SOC_CHIP_BCM88020, \
    /* SOC_CHIP_FE2000_A1,   */ SOC_CHIP_BCM88020  \
}

#define SOC_CHIP_TYPE_NAMES_INIT { \
    "BCM5600_C0",  \
    "BCM5680_B0",  \
    "BCM5605_A0",  \
    "BCM5615_A0",  \
    "BCM5670_A0",  \
    "BCM5673_A0",  \
    "BCM5690_A0",  \
    "BCM5665_A0",  \
    "BCM5695_A0",  \
    "BCM5675_A0",  \
    "BCM5674_A0",  \
    "BCM5665_B0",  \
    "BCM56601_A0", \
    "BCM56601_B0", \
    "BCM56601_C0", \
    "BCM56602_A0", \
    "BCM56602_B0", \
    "BCM56602_C0", \
    "BCM56504_A0", \
    "BCM56504_B0", \
    "BCM56102_A0", \
    "BCM56304_B0", \
    "BCM56112_A0", \
    "BCM56314_A0", \
    "BCM5650_C0",  \
    "BCM56800_A0", \
    "BCM56218_A0", \
    "BCM56514_A0", \
    "BCM56624_A0", \
    "BCM56680_A0", \
    "BCM56224_A0", \
    "BCM56224_B0", \
    "BCM56725_A0", \
    "BCM56820_A0", \
    "BCM56520_A0", \
    "BCM5324_A0",  \
    "BCM5324_A1",  \
    "BCM5324_A2",  \
    "BCM5396_A0",  \
    "BCM5389_A0",  \
    "BCM5398_A0",  \
    "BCM5348_A0",  \
    "BCM5397_A0",  \
    "BCM5347_A0",  \
    "BCM5395_A0",  \
    "BCM53242_A0", \
    "BCM53262_A0", \
    "BCM53115_A0", \
    "BCM53118_A0", \
    "BCM4713_A0",  \
    "QE2000_A0",   \
    "BME3200_A0",  \
    "BME3200_B0",  \
    "BM9600_A0",  \
    "BM9600_B0",  \
    "BCM88020_A0",  \
    "BCM88020_A1"  \
}

typedef enum soc_chip_groups_e {
    /* Chip names w/o revision */
    SOC_CHIP_BCM5600,
    SOC_CHIP_BCM5680,
    SOC_CHIP_BCM5605,
    SOC_CHIP_BCM5615,
    SOC_CHIP_BCM5670,
    SOC_CHIP_BCM5673,
    SOC_CHIP_BCM5690,
    SOC_CHIP_BCM5665,
    SOC_CHIP_BCM5695,
    SOC_CHIP_BCM5675,
    SOC_CHIP_BCM5674,
    SOC_CHIP_BCM56601,
    SOC_CHIP_BCM56602,
    SOC_CHIP_BCM56504,
    SOC_CHIP_BCM56102,
    SOC_CHIP_BCM56304,
    SOC_CHIP_BCM5650,
    SOC_CHIP_BCM56800,
    SOC_CHIP_BCM56218,
    SOC_CHIP_BCM56112,
    SOC_CHIP_BCM56314,
    SOC_CHIP_BCM56514,
    SOC_CHIP_BCM56624,
    SOC_CHIP_BCM56680,
    SOC_CHIP_BCM56224,
    SOC_CHIP_BCM56725,
    SOC_CHIP_BCM56820,
    SOC_CHIP_BCM56520,
    SOC_CHIP_BCM5324,
    SOC_CHIP_BCM5396,
    SOC_CHIP_BCM5389,
    SOC_CHIP_BCM5398,
    SOC_CHIP_BCM5348,
    SOC_CHIP_BCM5397,
    SOC_CHIP_BCM5347,
    SOC_CHIP_BCM5395,
    SOC_CHIP_BCM53242,
    SOC_CHIP_BCM53262,
    SOC_CHIP_BCM53115,
    SOC_CHIP_BCM53118,
    SOC_CHIP_BCM4713,
    SOC_CHIP_QE2000,
    SOC_CHIP_BME3200,
    SOC_CHIP_BM9600,
    SOC_CHIP_BCM88020,
    SOC_CHIP_GROUPS_COUNT
} soc_chip_groups_t;

#define SOC_CHIP_GROUP_NAMES_INIT { \
    "BCM5600",  \
    "BCM5680",  \
    "BCM5605",  \
    "BCM5615",  \
    "BCM5670",  \
    "BCM5673",  \
    "BCM5690",  \
    "BCM5665",  \
    "BCM5695",  \
    "BCM5675",  \
    "BCM5674",  \
    "BCM56601", \
    "BCM56602", \
    "BCM56504", \
    "BCM56102", \
    "BCM56304", \
    "BCM5650",  \
    "BCM56800", \
    "BCM56218", \
    "BCM56112", \
    "BCM56314", \
    "BCM56514", \
    "BCM56624", \
    "BCM56680", \
    "BCM56224", \
    "BCM56725", \
    "BCM56820", \
    "BCM56520", \
    "BCM5324",  \
    "BCM5396",  \
    "BCM5389",  \
    "BCM5398",  \
    "BCM5348",  \
    "BCM5397",\
    "BCM5347",\
    "BCM5395",\
    "BCM53242",\
    "BCM53262",\
    "BCM53115", \
    "BCM53118", \
    "BCM4713",  \
    "QE2000",   \
    "BME3200",  \
    "BM9600",  \
    "BCM88020"  \
}

/*
 * If you add anything here, check soc/common.c for arrays
 * indexed by soc_regtype_t.
 */
typedef enum soc_regtype_t {
    soc_schan_reg,      /* Generic register read thru SCHAN */
    soc_genreg,         /* General soc registers */
    soc_portreg,        /* Port soc registers */
    soc_cosreg,         /* COS soc registers */
    soc_cpureg,         /* AKA PCI memory */
    soc_pci_cfg_reg,    /* PCI configuration space register */
    soc_phy_reg,        /* PHY register, access thru mii */
    soc_spi_reg,        /* SPI relevant Registers*/
    soc_hostmem_w,      /* word */
    soc_hostmem_h,      /* half word */
    soc_hostmem_b,      /* byte */
    soc_invalidreg
} soc_regtype_t;

#endif  /* !_SOC_DEFS_H */

