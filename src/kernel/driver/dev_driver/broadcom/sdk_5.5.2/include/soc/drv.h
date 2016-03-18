/*
 * $Id: drv.h,v 1.178.2.11 Broadcom SDK $
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
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */

#ifndef _SOC_DRV_H
#define _SOC_DRV_H

#include <sal/core/time.h>
#include <sal/core/boot.h>

#include <assert.h>

#include <shared/avl.h>
#include <shared/bitop.h>

#include <soc/util.h>
#include <soc/cm.h>
#include <soc/feature.h>
#include <soc/property.h>

#include <sal/core/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>

#include <soc/chip.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/error.h>
#include <soc/dma.h>
#include <soc/enet.h>
#include <soc/counter.h>
#include <soc/mmuerr.h>
#include <soc/drv_if.h>
#include <soc/types.h>
#include <soc/macipadr.h>
#include <soc/ll.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/robo/memregs.h>

/****************************************************************
 * UNIT DRIVER ACCESS MACROS
 *
 *         MACRO                             EVALUATES TO
 *  ________________________________________________________________
 *      SOC_DRIVER(unit)                Chip driver structure
 *      SOC_INFO(unit)                  SOC Info structure
 *      SOC_MEM_INFO(unit,mem)          Memory info structure
 *      SOC_REG_INFO(unit,reg)          Register info structure
 *      SOC_BLOCK_INFO(unit,port)       Block info structure
 *      SOC_PORT_INFO(unit,port)        Port info structure
 *      SOC_BLOCK2SCH(unit,blk)         Integer schan num for block
 *      SOC_BLOCK2OFFSET(unit,blk)      Block to idx for cmic cmds
 *      SOC_HAS_CTR_TYPE(unit, ctype)   Does the device have a given
 *                                      counter map defined?
 *      SOC_CTR_DMA_MAP(unit, ctype)    Return pointer to the counter
 *                                      map of the indicated type.
 *      SOC_CTR_TO_REG(unit, ctype, ind) Return the register index for
 *                                       a given counter index.
 *      SOC_CTR_MAP_SIZE(unit, ctype)   How many entries in a given
 *                                      counter map.
 ****************************************************************/

#define SOC_CONTROL(unit)               (soc_control[unit])
#define SOC_DRIVER(unit)                (SOC_CONTROL(unit)->chip_driver)
#define SOC_FUNCTIONS(unit)             (SOC_CONTROL(unit)->soc_functions)
#define SOC_INFO(unit)                  (SOC_CONTROL(unit)->info)


#define DRV_SERVICES(unit)              (SOC_DRIVER(unit)->services)
#define SOC_MEM_INFO(unit, mem)         (*SOC_DRIVER(unit)->mem_info[mem])
#define SOC_MEM_AGGR(unit, index)       (SOC_DRIVER(unit)->mem_aggr[index])
#define SOC_MEM_PTR(unit, mem)          (SOC_DRIVER(unit)->mem_info[mem])
#define SOC_REG_INFO(unit, reg)         (*SOC_DRIVER(unit)->reg_info[reg])
#define SOC_REG_PTR(unit, reg)          (SOC_DRIVER(unit)->reg_info[reg])
#define SOC_BLOCK_INFO(unit, blk)       (SOC_DRIVER(unit)->block_info[blk])
#define SOC_PORT_INFO(unit, port)       (SOC_DRIVER(unit)->port_info[port])
#define SOC_CHIP_STRING(unit)           (SOC_DRIVER(unit)->chip_string)
#define SOC_ORIGIN(unit)                (SOC_DRIVER(unit)->origin)
#define SOC_PCI_VENDOR(unit)            (SOC_DRIVER(unit)->pci_vendor)
#define SOC_PCI_DEVICE(unit)            (SOC_DRIVER(unit)->pci_device)
#define SOC_PCI_REVISION(unit)          (SOC_DRIVER(unit)->pci_revision)

#define SOC_BLOCK2SCH(unit, blk)        (SOC_BLOCK_INFO(unit,blk).schan)
#define SOC_BLOCK2OFFSET(unit, blk)     (SOC_BLOCK_INFO(unit,blk).cmic)

#define SOC_PERSIST(unit)               (soc_persist[unit])

#define SOC_IRQ_MASK(unit)              (SOC_CONTROL(unit)->irq_mask)

#define SOC_IS_RCPU_UNIT(unit)          (SOC_CONTROL(unit)->remote_cpu)
#define SOC_IS_RCPU_SCHAN(unit)         (SOC_CONTROL(unit)->soc_flags & SOC_F_RCPU_SCHAN)

/* rval must be 64-bit value */
#define SOC_REG_RST_VAL_GET(unit, reg, rval) \
    COMPILER_64_SET(rval, SOC_REG_INFO(unit, reg).rst_val_hi, \
    SOC_REG_INFO(unit, reg).rst_val_lo)
#define SOC_REG_RST_MSK_GET(unit, reg, rmsk) \
    COMPILER_64_SET(rmsk, SOC_REG_INFO(unit, reg).rst_mask_hi, \
    SOC_REG_INFO(unit, reg).rst_mask_lo)

/*
 * Counter map macros
 * Assumes "ctype" is of SOC_CTR_TYPE_xxx
 */
#define SOC_HAS_CTR_TYPE(unit, ctype) \
    (SOC_DRIVER(unit)->counter_maps[ctype].cmap_base != NULL)
/* Reference to array of counters */
#define SOC_CTR_DMA_MAP(unit, ctype)  \
    (SOC_DRIVER(unit)->counter_maps[ctype])
/* Map a counter index to a register index */
#define SOC_CTR_TO_REG(unit, ctype, ind)  \
    (SOC_DRIVER(unit)->counter_maps[ctype].cmap_base[ind].reg)
#define SOC_CTR_TO_REG_IDX(unit, ctype, ind)  \
    (SOC_DRIVER(unit)->counter_maps[ctype].cmap_base[ind].index)
/* Right now, this is really per chip, not per counter type. */
#define SOC_CTR_MAP_SIZE(unit, ctype)   \
    (SOC_DRIVER(unit)->counter_maps[ctype].cmap_size)

#define SOC_REG_CTR_IDX(unit, reg) \
    (SOC_REG_INFO(unit, reg).ctr_idx)

#define SOC_MEM_IS_VALID(unit, mem) \
    ((SOC_MEM_PTR(unit, mem) != NULL) ?\
    (mem >= 0 && mem < (SOC_IS_ROBO(unit) ? NUM_SOC_ROBO_MEM : NUM_SOC_MEM) && \
     (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_VALID)) : 0)

#define SOC_MEM_IS_ENABLED(unit, mem) \
    (SOC_MEM_IS_VALID(unit, mem) && \
     !(SOC_MEM_INFO(unit, mem).flags & \
       SOC_CONTROL(unit)->disabled_mem_flags))

#define SOC_REG_IS_VALID(unit, reg) \
    ((SOC_REG_PTR(unit, reg) != NULL) ?\
    (reg >= 0 && reg < (SOC_IS_ROBO(unit) ? NUM_SOC_ROBO_REG : NUM_SOC_REG))\
    :0)

#define SOC_MAX_COUNTER_NUM  \
        ((IS_FE_PORT(0,0)) ?  SOC_CTR_MAP_SIZE(0,0) : SOC_CTR_MAP_SIZE(0, 1))


#define SOC_REG_IS_ENABLED(unit, reg) \
    (SOC_REG_IS_VALID(unit, reg) && \
     !(SOC_REG_INFO(unit, reg).flags & \
       SOC_CONTROL(unit)->disabled_reg_flags))

#define SOC_MEM_BYTES(unit, mem)        (SOC_MEM_INFO(unit, mem).bytes)
#define SOC_MEM_WORDS(unit, mem)        (BYTES2WORDS(SOC_MEM_BYTES(unit, mem)))

#define SOC_MEM_BASE(unit, mem)     (SOC_MEM_INFO(unit, mem).base)
#define SOC_MEM_SIZE(unit, mem) \
    (SOC_MEM_INFO(unit, mem).index_max - \
     SOC_MEM_INFO(unit, mem).index_min + 1)
#define SOC_MEM_TABLE_BYTES(unit, mem) \
    (4 * SOC_MEM_WORDS(unit, mem) * \
     (SOC_MEM_INFO(unit, mem).index_max - \
      SOC_MEM_INFO(unit, mem).index_min + 1))

#define SOC_REG_TYPE(unit, reg)      \
    (SOC_REG_INFO(unit, reg).regtype)

#define SOC_REG_IS_64(unit, reg)     \
    (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_64_BITS)

#define SOC_REG_IS_32(unit, reg) \
    (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_32_BITS)

#define SOC_REG_IS_16(unit, reg) \
    (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_16_BITS)

#define SOC_REG_IS_8(unit, reg) \
    (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_8_BITS)

/* Register requires special processing */
#define SOC_REG_IS_SPECIAL(unit, reg)     \
    (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_SPECIAL)

#define SOC_REG_ARRAY(unit, reg)     \
    (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_ARRAY)
#define SOC_REG_NUMELS(unit, reg)     \
    (SOC_REG_INFO(unit, reg).numels)
#define SOC_REG_GRAN(unit, reg)     \
    (soc_regtype_gran[SOC_REG_INFO(unit, reg).regtype])
#define SOC_REG_PAGE(unit, reg) \
    (SOC_REG_INFO(unit, reg).page)

#define SOC_CHIP_TYPE(unit)     (SOC_INFO(unit).driver_type)
#define SOC_CHIP_GROUP(unit)    (SOC_INFO(unit).driver_group)

/*
 * SOC_IS_* Macros.  If support for the chip is defined out of the
 * build, they are defined as zero to let the optimizer remove
 * code.
 */
#ifdef  BCM_STRATA_SUPPORT
#define SOC_IS_STRATA1(unit)    (SOC_INFO(unit).chip & SOC_INFO_STRATA1)
#ifdef  BCM_STRATA2_SUPPORT
#define SOC_IS_STRATA2(unit)    (SOC_INFO(unit).chip & SOC_INFO_STRATA2)
#else
#define SOC_IS_STRATA2(unit)    (0)
#endif
#define SOC_IS_STRATA(unit)     (SOC_INFO(unit).chip & SOC_INFO_STRATA)
#else
#define SOC_IS_STRATA1(unit)    (0)
#define SOC_IS_STRATA2(unit)    (0)
#define SOC_IS_STRATA(unit)     (0)
#endif

#ifdef  BCM_DRACO_SUPPORT
#define SOC_IS_DRACO1(unit)     (SOC_INFO(unit).chip & SOC_INFO_DRACO1)
#ifdef  BCM_DRACO15_SUPPORT
#define SOC_IS_DRACO15(unit)    (SOC_INFO(unit).chip & SOC_INFO_DRACO15)
#else
#define SOC_IS_DRACO15(unit)    (0)
#endif
#define SOC_IS_DRACO(unit)      (SOC_INFO(unit).chip & SOC_INFO_DRACO)
#else
#define SOC_IS_DRACO1(unit)     (0)
#define SOC_IS_DRACO15(unit)    (0)
#define SOC_IS_DRACO(unit)      (0)
#endif

#ifdef  BCM_HERCULES_SUPPORT
#define SOC_IS_HERCULES1(unit)  (SOC_INFO(unit).chip & SOC_INFO_HERCULES1)
#ifdef  BCM_HERCULES15_SUPPORT
#define SOC_IS_HERCULES15(unit) (SOC_INFO(unit).chip & SOC_INFO_HERCULES15)
#else
#define SOC_IS_HERCULES15(unit) (0)
#endif
#define SOC_IS_HERCULES(unit)   (SOC_INFO(unit).chip & SOC_INFO_HERCULES)
#else
#define SOC_IS_HERCULES1(unit)  (0)
#define SOC_IS_HERCULES15(unit) (0)
#define SOC_IS_HERCULES(unit)   (0)
#endif

#ifdef  BCM_LYNX_SUPPORT
#define SOC_IS_LYNX(unit)       (SOC_INFO(unit).chip & SOC_INFO_LYNX)
#else
#define SOC_IS_LYNX(unit)       (0)
#endif

#ifdef  BCM_TUCANA_SUPPORT
#define SOC_IS_TUCANA(unit)     (SOC_INFO(unit).chip & SOC_INFO_TUCANA)
#else
#define SOC_IS_TUCANA(unit)     (0)
#endif

#ifdef  BCM_EASYRIDER_SUPPORT
#define SOC_IS_EASYRIDER(unit)  (SOC_INFO(unit).chip & SOC_INFO_EASYRIDER)
#else
#define SOC_IS_EASYRIDER(unit)  (0)
#endif

#ifdef  BCM_FIREBOLT_SUPPORT
#define SOC_IS_FIREBOLT(unit)   (SOC_INFO(unit).chip & SOC_INFO_FIREBOLT)
#define SOC_IS_FB(unit)         (SOC_INFO(unit).chip & SOC_INFO_FB)
#define SOC_IS_FB_FX_HX(unit)   (SOC_INFO(unit).chip & SOC_INFO_FB_FX_HX)
#define SOC_IS_HB_GW(unit)      (SOC_INFO(unit).chip & SOC_INFO_HB_GW)
#define SOC_IS_HBX(unit)        (SOC_INFO(unit).chip & SOC_INFO_HBX)
#define SOC_IS_FBX(unit)        (SOC_INFO(unit).chip & SOC_INFO_FBX)
#else
#define SOC_IS_FIREBOLT(unit)   (0)
#define SOC_IS_FB(unit)         (0)
#define SOC_IS_FB_FX_HX(unit)   (0)
#define SOC_IS_HB_GW(unit)      (0)
#define SOC_IS_HBX(unit)        (0)
#define SOC_IS_FBX(unit)        (0)
#endif

#ifdef  BCM_HELIX_SUPPORT
#define SOC_IS_HELIX1(unit)     (SOC_INFO(unit).chip & SOC_INFO_HELIX)
#define SOC_IS_HELIX15(unit)    (SOC_INFO(unit).chip & SOC_INFO_HELIX15)
#define SOC_IS_HELIX(unit)      (SOC_INFO(unit).chip & SOC_INFO_HELIX_ANY)
#else
#define SOC_IS_HELIX(unit)      (0)
#define SOC_IS_HELIX1(unit)     (0)
#define SOC_IS_HELIX15(unit)    (0)
#endif
#ifdef  BCM_FELIX_SUPPORT
#define SOC_IS_FELIX1(unit)     (SOC_INFO(unit).chip & SOC_INFO_FELIX)
#define SOC_IS_FELIX15(unit)    (SOC_INFO(unit).chip & SOC_INFO_FELIX15)
#define SOC_IS_FELIX(unit)      (SOC_INFO(unit).chip & SOC_INFO_FELIX_ANY)
#else
#define SOC_IS_FELIX(unit)      (0)
#define SOC_IS_FELIX1(unit)     (0)
#define SOC_IS_FELIX15(unit)    (0)
#endif
#ifdef  BCM_RAPTOR_SUPPORT
#define SOC_IS_RAPTOR(unit)     (SOC_INFO(unit).chip & SOC_INFO_RAPTOR)
#define SOC_IS_RAVEN(unit)      (SOC_INFO(unit).chip & SOC_INFO_RAVEN)
#else
#define SOC_IS_RAPTOR(unit)     (0)
#define SOC_IS_RAVEN(unit)      (0)
#endif
#if defined(BCM_HELIX_SUPPORT) || defined(BCM_FELIX_SUPPORT)
#define SOC_IS_FX_HX(unit)      (SOC_INFO(unit).chip & SOC_INFO_FX_HX)
#else
#define SOC_IS_FX_HX(unit)      (0)
#endif

#ifdef  BCM_GOLDWING_SUPPORT
#define SOC_IS_GOLDWING(unit)   (SOC_INFO(unit).chip & SOC_INFO_GOLDWING)
#else
#define SOC_IS_GOLDWING(unit)   (0)
#endif

#ifdef  BCM_HUMV_SUPPORT
#define SOC_IS_HUMV(unit)       (SOC_INFO(unit).chip & SOC_INFO_HUMV)
#else
#define SOC_IS_HUMV(unit)       (0)
#endif

#ifdef  BCM_BRADLEY_SUPPORT
#define SOC_IS_BRADLEY(unit)    (SOC_INFO(unit).chip & SOC_INFO_BRADLEY)
#else
#define SOC_IS_BRADLEY(unit)    (0)
#endif

#ifdef  BCM_FIREBOLT2_SUPPORT
#define SOC_IS_FIREBOLT2(unit)  (SOC_INFO(unit).chip & SOC_INFO_FIREBOLT2)
#else
#define SOC_IS_FIREBOLT2(unit)  (0)
#endif

#ifdef  BCM_TRIUMPH_SUPPORT
#define SOC_IS_TRIUMPH(unit)    (SOC_INFO(unit).chip & SOC_INFO_TRIUMPH)
#define SOC_IS_TR_VL(unit)      (SOC_INFO(unit).chip & SOC_INFO_TR_VL)
#else
#define SOC_IS_TRIUMPH(unit)    (0)
#define SOC_IS_TR_VL(unit)      (0)
#endif

#ifdef  BCM_VALKYRIE_SUPPORT
#define SOC_IS_VALKYRIE(unit)   (SOC_INFO(unit).chip & SOC_INFO_VALKYRIE)
#else
#define SOC_IS_VALKYRIE(unit)   (0)
#endif

#ifdef  BCM_SCORPION_SUPPORT
#define SOC_IS_SCORPION(unit)   (SOC_INFO(unit).chip & SOC_INFO_SCORPION)
#define SOC_IS_SC_CQ(unit)      (SOC_INFO(unit).chip & SOC_INFO_SC_CQ)
#else
#define SOC_IS_SCORPION(unit)   (0)
#define SOC_IS_SC_CQ(unit)      (0)
#endif

#ifdef  BCM_CONQUEROR_SUPPORT
#define SOC_IS_CONQUEROR(unit)  (SOC_INFO(unit).chip & SOC_INFO_CONQUEROR)
#else
#define SOC_IS_CONQUEROR(unit)  (0)
#endif

#ifdef  BCM_APOLLO_SUPPORT
#define SOC_IS_APOLLO(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_APOLLO))
#else
#define SOC_IS_APOLLO(unit)  (0)
#endif

#ifdef BCM_TRX_SUPPORT
#define SOC_IS_TRX(unit)          (SOC_INFO(unit).chip & SOC_INFO_TRX)
#else
#define SOC_IS_TRX(unit)          (0)
#endif

#ifdef  BCM_XGS_SUPPORT
#define SOC_IS_XGS(unit)        (SOC_INFO(unit).chip & SOC_INFO_XGS)
#else
#define SOC_IS_XGS(unit)        (0)
#endif
#ifdef  BCM_XGS_FABRIC_SUPPORT
#define SOC_IS_XGS_FABRIC(unit) (SOC_INFO(unit).chip & SOC_INFO_XGS_FABRIC)
#else
#define SOC_IS_XGS_FABRIC(unit) (0)
#endif
#ifdef  BCM_XGS_SWITCH_SUPPORT
#define SOC_IS_XGS_SWITCH(unit) (SOC_INFO(unit).chip & SOC_INFO_XGS_SWITCH)
#else
#define SOC_IS_XGS_SWITCH(unit) (0)
#endif
#ifdef  BCM_XGS12_FABRIC_SUPPORT
#define SOC_IS_XGS12_FABRIC(unit) (SOC_INFO(unit).chip & SOC_INFO_XGS12_FABRIC)
#else
#define SOC_IS_XGS12_FABRIC(unit) (0)
#endif
#ifdef  BCM_XGS12_SWITCH_SUPPORT
#define SOC_IS_XGS12_SWITCH(unit) (SOC_INFO(unit).chip & SOC_INFO_XGS12_SWITCH)
#else
#define SOC_IS_XGS12_SWITCH(unit) (0)
#endif
#ifdef  BCM_XGS3_SWITCH_SUPPORT
#define SOC_IS_XGS3_SWITCH(unit)  (SOC_INFO(unit).chip & SOC_INFO_XGS3_SWITCH)
#else
#define SOC_IS_XGS3_SWITCH(unit) (0)
#endif
#ifdef  BCM_XGS3_FABRIC_SUPPORT
#define SOC_IS_XGS3_FABRIC(unit)  (SOC_INFO(unit).chip & SOC_INFO_XGS3_FABRIC)
#else
#define SOC_IS_XGS3_FABRIC(unit) (0)
#endif
#ifdef  BCM_ROBO_SUPPORT
#define SOC_IS_ROBO(unit)        (SOC_INFO(unit).chip & SOC_INFO_ROBO)
#define SOC_IS_ROBO5324(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO5324)
#define SOC_IS_ROBO5324_A1(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO5324_A1)
#define SOC_IS_ROBO5396(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO5396)
#define SOC_IS_ROBO5389(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO5389)
#define SOC_IS_ROBO5398(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO5398)
#define SOC_IS_ROBODINO(unit)   (SOC_INFO(unit).chip & (SOC_INFO_ROBO5389|SOC_INFO_ROBO5396))
#define SOC_IS_ROBO5348(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO5348)
#define SOC_IS_ROBO5397(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO5397)
#define SOC_IS_ROBO5347(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO5347)
#define SOC_IS_ROBO5395(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO5395)
#define SOC_IS_ROBO53242(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO53242)
#define SOC_IS_ROBO53262(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO53262)
#define SOC_IS_ROBO53115(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO53115)
#define SOC_IS_ROBO53118(unit)   (SOC_INFO(unit).chip & SOC_INFO_ROBO53118)
#define SOC_IS_ROBO5325(unit) (0)
#define SOC_IS_ROBO_GE_SWITCH(unit) (SOC_INFO(unit).chip & (SOC_INFO_ROBO5389|\
                                                            SOC_INFO_ROBO5396 | SOC_INFO_ROBO5398 |\
                                                            SOC_INFO_ROBO5397 | SOC_INFO_ROBO5395 |\
                                                            SOC_INFO_ROBO53115 | SOC_INFO_ROBO53118))

#else
#define SOC_IS_ROBO(unit)        (0)
#define SOC_IS_ROBO_GE_SWITCH(unit) (0)
#endif

/*
 * Note: ESW and ROBO devices need similar SOC_IS definitions
 *
 * SOC_IS_* macros for SBX devices
 */
typedef enum {
    SOC_INFO_STRATA1,
    SOC_INFO_STRATA2,
    SOC_INFO_DRACO1,
    SOC_INFO_HERCULES1,
    SOC_INFO_LYNX,
    SOC_INFO_TUCANA,
    SOC_INFO_DRACO15,
    SOC_INFO_HERCULES15,
    SOC_INFO_EASYRIDER,
    SOC_INFO_FIREBOLT,
    SOC_INFO_EASYRIDER10,
    SOC_INFO_FELIX,
    SOC_INFO_HELIX,
    SOC_INFO_GOLDWING,
    SOC_INFO_HUMV,
    SOC_INFO_BRADLEY,
    SOC_INFO_RAPTOR,
    SOC_INFO_FELIX15,
    SOC_INFO_HELIX15,
    SOC_INFO_FIREBOLT2,
    SOC_INFO_TRIUMPH,
    SOC_INFO_RAVEN,
    SOC_INFO_QE2000,
    SOC_INFO_BME3200,
    SOC_INFO_FE2000,
    SOC_INFO_BM9600
} soc_chip_e;


#define SOC_IS_ESW(unit) !SOC_IS_ROBO(unit)
/* Some devices need a register updated before starting a DMA TX operation */
#if     defined(BCM_DRACO1_SUPPORT) || \
        defined(BCM_LYNX_SUPPORT) || \
        defined(BCM_TUCANA_SUPPORT)
#define SOC_TX_PKT_PROPERTIES
#endif

#define SOC_UNIT_VALID(unit)            ((unit) >= 0 && (unit) < soc_ndev && \
                                        SOC_CONTROL(unit) != NULL)

/* Indicates port lies in module 1.  Currently applies only to Tucana */
#ifdef  BCM_TUCANA_SUPPORT
#define SOC_PORT_MOD1(unit, port)       ((((port) & 0x20) != 0) && \
                                        (soc_feature(unit, soc_feature_mod1)))

/*
 * This macro returns the memory table (soc_mem_t) associated with
 * the given unit/port for Tucana.  It returns a memory valid for
 * other chips as well.
 */
#define SOC_PORT_MEM_TAB(unit, port)                            \
    (SOC_PORT_MOD1(unit, port) ? PORT_TAB1m : PORT_TABm)


/*
 * Macro:
 *      SOC_PORT_MOD_OFFSET
 * Purpose:
 *      Maps a port to its offset in its module on Tucana.
 *      (Lower 5 bits: port % 32)
 *      Example:   7 -> 7,   32 -> 0,  35 -> 3.
 */
#define SOC_PORT_MOD_OFFSET(unit, port) ((soc_feature(unit, soc_feature_mod1) ? \
                                         (port & 0x1f) : port))

/*
 * Read/write macros for accessing proper port table w/ proper index
 */
#define READ_PORT_TAB_MOD_CHK(unit, block, port, val_p)          \
    (SOC_PORT_MOD1(unit, port) ?                                 \
        READ_PORT_TAB1m(unit, block, (port) & 0x1f, val_p) :     \
        READ_PORT_TABm(unit, block, port, val_p))

#define WRITE_PORT_TAB_MOD_CHK(unit, block, port, val_p)         \
    (SOC_PORT_MOD1(unit, port) ?                                 \
        WRITE_PORT_TAB1m(unit, block, (port) & 0x1f, val_p) :    \
        WRITE_PORT_TABm(unit, block, port, val_p))

/* Forces port to lie in module 1.  Currently applies only to Tucana */
#define SOC_PORT_IN_MOD1(unit, port)    ((port) | 0x20)

#else /* BCM_TUCANA_SUPPORT */
#define SOC_PORT_MOD1(unit, port)       (0)
#define SOC_PORT_MEM_TAB(unit, port)    (PORT_TABm)
#define SOC_PORT_MOD_OFFSET(unit, port) (port)
#define READ_PORT_TAB_MOD_CHK(unit, block, port, val_p)          \
            (READ_PORT_TABm(unit, block, port, val_p))

#define WRITE_PORT_TAB_MOD_CHK(unit, block, port, val_p)         \
            (WRITE_PORT_TABm(unit, block, port, val_p))
#define SOC_PORT_IN_MOD1(unit, port)    (port)

#endif /* BCM_TUCANA_SUPPORT */

#define SOC_PORT_VALID_RANGE(unit,port)      ((port) >= 0 && \
                                         (port) < (SOC_IS_ROBO(unit) ? \
                                             SOC_ROBO_MAX_NUM_PORTS : \
                                                SOC_MAX_NUM_PORTS) )

/* Holes in the port list shall be detected by port type */
#define	SOC_PORT_VALID(unit,port)   ((SOC_PORT_VALID_RANGE(unit, port)) && \
                                     (SOC_PORT_TYPE(unit, port) != 0))

#define SOC_PORT_NAME(unit,port)        (SOC_INFO(unit).port_name[port])
#define SOC_PORT_OFFSET(unit,port)      (SOC_INFO(unit).port_offset[port])
#define SOC_PORT_TYPE(unit,port)        (SOC_INFO(unit).port_type[port])
#define SOC_PORT_BLOCK(unit, port)      (SOC_PORT_INFO(unit, port).blk)
#define SOC_PORT_BINDEX(unit, port)     (SOC_PORT_INFO(unit, port).bindex)

#define SOC_BLOCK_ANY                   -1      /* for reading */
#define SOC_BLOCK_ALL                   -1      /* for writing */
#define SOC_BLOCK_NAME(unit,blk)        ((int)blk == SOC_BLOCK_ALL ? \
                                        "*" : \
                                        SOC_INFO(unit).block_name[blk])

#define SOC_BLOCK_PORT(unit, blk)       (SOC_INFO(unit).block_port[blk])
#define SOC_BLOCK_BITMAP(unit, blk)     (SOC_INFO(unit).block_bitmap[blk])
#define SOC_BLOCK_TYPE(unit, blk)       (SOC_BLOCK_INFO(unit, blk).type)
#define SOC_BLOCK_NUMBER(unit, blk)     (SOC_BLOCK_INFO(unit, blk).number)
#define SOC_BLOCK_IS_TYPE(unit, blk, mask) (SOC_BLOCK_TYPE(unit, blk) & (mask))


/*
 * Macros to get the device block type, number (instance) and index
 * within block for a given port
 */
#define SOC_PORT_BLOCK_TYPE(unit, port)    \
    (SOC_BLOCK_TYPE(unit, SOC_PORT_BLOCK(unit, port)))

#define SOC_PORT_BLOCK_NUMBER(unit, port)    \
    (SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, port)))

#define SOC_PORT_BLOCK_INDEX(unit, port)   (SOC_PORT_BINDEX(unit, port))


#define NUM_COS(unit)                   (SOC_DRIVER(unit)->num_cos)
#define NUM_CPU_COSQ(unit)              (SOC_INFO(unit).num_cpu_cosq)
#define NUM_MODID(unit)                 (SOC_INFO(unit).modid_count)
#define SOC_MODID_MAX(unit)             (SOC_INFO(unit).modid_max)
#define SOC_TRUNK_BIT_POS(unit)         (SOC_INFO(unit).trunk_bit_pos)
#define SOC_PORT_ADDR_MAX(unit)         (SOC_INFO(unit).port_addr_max)
#define SOC_HG_OFFSET(unit)             (SOC_INFO(unit).hg_offset)
#define SOC_PORT_ADDRESSABLE(unit,port) (port >= 0 && port <= \
                                            SOC_INFO(unit).port_addr_max)
#define SOC_MODID_ADDRESSABLE(unit,mod) (mod >= 0 && mod <= \
                                            SOC_INFO(unit).modid_max)

#define SOC_PORT(unit,ptype,pno)        (SOC_INFO(unit).ptype.port[pno])
#define SOC_PORT_NUM(unit,ptype)        (SOC_INFO(unit).ptype.num)
#define SOC_PORT_MIN(unit,ptype)        (SOC_INFO(unit).ptype.min)
#define SOC_PORT_MAX(unit,ptype)        (SOC_INFO(unit).ptype.max)
#define SOC_PORT_BITMAP(unit,ptype)     (SOC_INFO(unit).ptype.bitmap)

#define NUM_FE_PORT(unit)               SOC_PORT_NUM(unit,fe)
#define NUM_GE_PORT(unit)               SOC_PORT_NUM(unit,ge)
#define NUM_XE_PORT(unit)               SOC_PORT_NUM(unit,xe)
#define NUM_HG_PORT(unit)               SOC_PORT_NUM(unit,hg)
#define NUM_HL_PORT(unit)               SOC_PORT_NUM(unit,hl)
#define NUM_ST_PORT(unit)               SOC_PORT_NUM(unit,st)
#define NUM_GX_PORT(unit)               SOC_PORT_NUM(unit,gx)
#define NUM_XG_PORT(unit)               SOC_PORT_NUM(unit,xg)
#define NUM_SPI_PORT(unit)              SOC_PORT_NUM(unit,spi)
#define NUM_SPI_SUBPORT_PORT(unit)      SOC_PORT_NUM(unit,spi_subport)
#define NUM_SCI_PORT(unit)              SOC_PORT_NUM(unit,sci)
#define NUM_SFI_PORT(unit)              SOC_PORT_NUM(unit,sfi)
#define NUM_FAB_PORT(unit)              SOC_PORT_NUM(unit,fab)
#define NUM_E_PORT(unit)                SOC_PORT_NUM(unit,ether)
#define NUM_PORT(unit)                  SOC_PORT_NUM(unit,port)
#define NUM_ALL_PORT(unit)              SOC_PORT_NUM(unit,all)
#define MAX_PORT(unit)                  (SOC_INFO(unit).port_num)
#define IPIC_PORT(unit)                 (SOC_INFO(unit).ipic_port)
#define IPIC_BLOCK(unit)                (SOC_INFO(unit).ipic_block)
#define CMIC_PORT(unit)                 (SOC_INFO(unit).cmic_port)
#define CMIC_BLOCK(unit)                (SOC_INFO(unit).cmic_block)
#define ARL_BLOCK(unit)                 (SOC_INFO(unit).arl_block)
#define MMU_BLOCK(unit)                 (SOC_INFO(unit).mmu_block)
#define MCU_BLOCK(unit)                 (SOC_INFO(unit).mcu_block)
#define IPIPE_BLOCK(unit)               (SOC_INFO(unit).ipipe_block)
#define IPIPE_HI_BLOCK(unit)            (SOC_INFO(unit).ipipe_hi_block)
#define EPIPE_BLOCK(unit)               (SOC_INFO(unit).epipe_block)
#define EPIPE_HI_BLOCK(unit)            (SOC_INFO(unit).epipe_hi_block)
#define IGR_BLOCK(unit)                 (SOC_INFO(unit).igr_block)
#define EGR_BLOCK(unit)                 (SOC_INFO(unit).egr_block)
#define BSE_BLOCK(unit)                 (SOC_INFO(unit).bse_block)
#define CSE_BLOCK(unit)                 (SOC_INFO(unit).cse_block)
#define HSE_BLOCK(unit)                 (SOC_INFO(unit).hse_block)
#define BSAFE_BLOCK(unit)               (SOC_INFO(unit).bsafe_block)
#define OTPC_BLOCK(unit)                (SOC_INFO(unit).otpc_block)
#define ESM_BLOCK(unit)                 (SOC_INFO(unit).esm_block)

/*
 * E is ethernet port (FE|GE|XE)
 * PORT is all net ports (E|HG)
 * ALL is all ports (PORT|CMIC)
 */
#define PBMP_FE_ALL(unit)               SOC_PORT_BITMAP(unit,fe)
#define PBMP_GE_ALL(unit)               SOC_PORT_BITMAP(unit,ge)
#define PBMP_XE_ALL(unit)               SOC_PORT_BITMAP(unit,xe)
#define PBMP_HG_ALL(unit)               SOC_PORT_BITMAP(unit,hg)
#define PBMP_HL_ALL(unit)               SOC_PORT_BITMAP(unit,hl)
#define PBMP_ST_ALL(unit)               SOC_PORT_BITMAP(unit,st)
#define PBMP_GX_ALL(unit)               SOC_PORT_BITMAP(unit,gx)
#define PBMP_XG_ALL(unit)               SOC_PORT_BITMAP(unit,xg)
#define PBMP_SPI_ALL(unit)              SOC_PORT_BITMAP(unit,spi)
#define PBMP_SPI_SUBPORT_ALL(unit)      SOC_PORT_BITMAP(unit,spi_subport)
#define PBMP_SCI_ALL(unit)              SOC_PORT_BITMAP(unit,sci)
#define PBMP_SFI_ALL(unit)              SOC_PORT_BITMAP(unit,sfi)
#define PBMP_FAB_ALL(unit)              SOC_PORT_BITMAP(unit,fab)
#define PBMP_E_ALL(unit)                SOC_PORT_BITMAP(unit,ether)
#define PBMP_PORT_ALL(unit)             SOC_PORT_BITMAP(unit,port)
#define PBMP_CMIC(unit)                 SOC_INFO(unit).cmic_bitmap
#define PBMP_LB(unit)                   SOC_INFO(unit).lb_pbm
#define PBMP_SPI(unit)                  SOC_INFO(unit).spi_bitmap
#define PBMP_ALL(unit)                  SOC_PORT_BITMAP(unit,all)

/* use PORT_MIN/_MAX to be more efficient than PBMP_ITER */
#define _SOC_PBMP_ITER(_u,_pt,_p)       \
        for ((_p) = SOC_PORT_MIN(_u,_pt); \
             (_p) >= 0 && (_p) <= SOC_PORT_MAX(_u,_pt); \
             (_p)++) \
                if (_SHR_PBMP_MEMBER(SOC_PORT_BITMAP(_u,_pt), (_p)))
#define PBMP_FE_ITER(_u, _p)            _SOC_PBMP_ITER(_u,fe,_p)
#define PBMP_GE_ITER(_u, _p)            _SOC_PBMP_ITER(_u,ge,_p)
#define PBMP_XE_ITER(_u, _p)            _SOC_PBMP_ITER(_u,xe,_p)
#define PBMP_HG_ITER(_u, _p)            _SOC_PBMP_ITER(_u,hg,_p)
#define PBMP_HL_ITER(_u, _p)            _SOC_PBMP_ITER(_u,hl,_p)
#define PBMP_ST_ITER(_u, _p)            _SOC_PBMP_ITER(_u,st,_p)
#define PBMP_GX_ITER(_u, _p)            _SOC_PBMP_ITER(_u,gx,_p)
#define PBMP_XG_ITER(_u, _p)            _SOC_PBMP_ITER(_u,xg,_p)
#define PBMP_SPI_ITER(_u, _p)           _SOC_PBMP_ITER(_u,spi,_p)
#define PBMP_SPI_SUBPORT_ITER(_u, _p)   _SOC_PBMP_ITER(_u,spi_subport,_p)
#define PBMP_SCI_ITER(_u, _p)           _SOC_PBMP_ITER(_u,sci,_p)
#define PBMP_SFI_ITER(_u, _p)           _SOC_PBMP_ITER(_u,sfi,_p)
#define PBMP_E_ITER(_u, _p)             _SOC_PBMP_ITER(_u,ether,_p)
#define PBMP_PORT_ITER(_u, _p)          _SOC_PBMP_ITER(_u,port,_p)
#define PBMP_ALL_ITER(_u, _p)           _SOC_PBMP_ITER(_u,all,_p)

#define IS_S_PORT(unit, port)           \
        (SOC_PBMP_MEMBER(SOC_INFO(unit).s_pbm, port))
#define IS_GMII_PORT(unit, port)        \
        (SOC_PBMP_MEMBER(SOC_INFO(unit).gmii_pbm, port))
#define IS_FE_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_FE_ALL(unit), port))
#define IS_GE_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_GE_ALL(unit), port))
#define IS_XE_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_XE_ALL(unit), port))
#define IS_HG_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_HG_ALL(unit), port))
#define IS_HL_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_HL_ALL(unit), port))
#define IS_ST_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_ST_ALL(unit), port))
#define IS_GX_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_GX_ALL(unit), port))
#define IS_XG_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_XG_ALL(unit), port))
#define IS_SPI_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_SPI_ALL(unit), port))
#define IS_SPI_SUBPORT_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_SPI_SUBPORT_ALL(unit), port))
#define IS_SCI_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_SCI_ALL(unit), port))
#define IS_SFI_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port))
#define IS_E_PORT(unit,port)            \
        (SOC_PBMP_MEMBER(PBMP_E_ALL(unit), port))
#define IS_PORT(unit,port)              \
        (SOC_PORT_TYPE(unit, port) & SOC_BLK_NET)
#define IS_LB_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_LB(unit), port))
#define IS_CPU_PORT(unit,port)          \
        (SOC_PORT_TYPE(unit, port) & SOC_BLK_CPU)
#define IS_SPI_BLK(unit,port)      \
    (SOC_PORT_TYPE(unit, port) & SOC_BLK_SPI)
#define IS_EXP_PORT(unit,port)      \
    (SOC_PORT_TYPE(unit, port) & SOC_BLK_EXP)
#define IS_ALL_PORT(unit,port)          \
        (SOC_PORT_TYPE(unit, port) != 0)

/* Stack related macros */

#define SOC_PBMP_STACK_CURRENT(unit)    \
    (SOC_PERSIST(unit)->stack_ports_current)
#define SOC_PBMP_STACK_INACTIVE(unit)   \
    (SOC_PERSIST(unit)->stack_ports_inactive)
#define SOC_PBMP_STACK_PREVIOUS(unit)   \
    (SOC_PERSIST(unit)->stack_ports_previous)

#define SOC_PBMP_STACK_ACTIVE_GET(unit, active) do { \
        SOC_PBMP_ASSIGN(active, SOC_PBMP_STACK_CURRENT(unit)); \
        SOC_PBMP_REMOVE(active, SOC_PBMP_STACK_INACTIVE(unit)); \
    } while (0)

#define SOC_IS_STACK_PORT(unit, port) \
    SOC_PBMP_MEMBER(SOC_PBMP_STACK_CURRENT(unit), port)

#define SOC_IS_INACTIVE_STACK_PORT(unit, port) \
    SOC_PBMP_MEMBER(SOC_PBMP_STACK_INACTIVE(unit), port)

/* SL Mode set/get macros */
#define SOC_SL_MODE(unit) \
    ((SOC_CONTROL(unit)->soc_flags & SOC_F_SL_MODE) != 0)

#define SOC_SL_MODE_SET(unit, sl_mode) \
    if (sl_mode) SOC_CONTROL(unit)->soc_flags |= SOC_F_SL_MODE; \
    else SOC_CONTROL(unit)->soc_flags &= ~SOC_F_SL_MODE

/* URPF on/off set/get macros */
#define SOC_URPF_STATUS_GET(unit) \
    ((SOC_CONTROL(unit)->soc_flags & SOC_F_URPF_ENABLED) != 0)

#define SOC_URPF_STATUS_SET(unit, status) \
    if (status) SOC_CONTROL(unit)->soc_flags |= SOC_F_URPF_ENABLED; \
    else SOC_CONTROL(unit)->soc_flags &= ~SOC_F_URPF_ENABLED

/* URPF on/off set/get macros */
#define SOC_L2X_GROUP_ENABLE_GET(_unit_) (SOC_CONTROL(_unit_)->l2x_group_enable)
#define SOC_L2X_GROUP_ENABLE_SET(_unit_, _status_) \
           (SOC_CONTROL(_unit_)->l2x_group_enable) = (_status_) ?  TRUE : FALSE


/* Device should use memory dma for memory clear macros */
#define SOC_MEM_CLEAR_USE_DMA(unit) \
    ((SOC_CONTROL(unit)->soc_flags & SOC_F_MEM_CLEAR_USE_DMA) != 0)

#define SOC_MEM_CLEAR_USE_DMA_SET(unit, status) \
    if (status) SOC_CONTROL(unit)->soc_flags |= SOC_F_MEM_CLEAR_USE_DMA; \
    else SOC_CONTROL(unit)->soc_flags &= ~SOC_F_MEM_CLEAR_USE_DMA

/* Device should use gport for source/destination notation */
#define SOC_USE_GPORT(unit) \
    ((SOC_CONTROL(unit)->soc_flags & SOC_F_GPORT) != 0)

#define SOC_USE_GPORT_SET(unit, status) \
    if (status) SOC_CONTROL(unit)->soc_flags |= SOC_F_GPORT; \
    else SOC_CONTROL(unit)->soc_flags &= ~SOC_F_GPORT

/* Maximum vrf id for the device set/get macro */
#define SOC_VRF_MAX(unit)  SOC_CONTROL(unit)->max_vrf_id
#define SOC_VRF_MAX_SET(unit, value) \
            SOC_CONTROL(unit)->max_vrf_id = value

/* Maximum address class for the device set/get macro */
#define SOC_ADDR_CLASS_MAX(unit)  SOC_CONTROL(unit)->max_address_class
#define SOC_ADDR_CLASS_MAX_SET(unit, value) \
            SOC_CONTROL(unit)->max_address_class = value

/* Maximum interface class for the device set/get macro */
#define SOC_INTF_CLASS_MAX(unit)  SOC_CONTROL(unit)->max_interface_class
#define SOC_INTF_CLASS_MAX_SET(unit, value) \
            SOC_CONTROL(unit)->max_interface_class = value


/* Device memory clear chunk size set/get macros */
#define SOC_MEM_CLEAR_CHUNK_SIZE_GET(unit)  SOC_CONTROL(unit)->mem_clear_chunk_size
#define SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit, value) \
            SOC_CONTROL(unit)->mem_clear_chunk_size = value

/* IPMC replication sharing set/get macros */
#define SOC_IPMCREPLSHR_GET(unit) \
    ((SOC_CONTROL(unit)->soc_flags & SOC_F_IPMCREPLSHR) != 0)

#define SOC_IPMCREPLSHR_SET(unit, status) \
    if (status) SOC_CONTROL(unit)->soc_flags |= SOC_F_IPMCREPLSHR; \
    else SOC_CONTROL(unit)->soc_flags &= ~SOC_F_IPMCREPLSHR

/* Dual hash maximum recursion level */
#define SOC_DUAL_HASH_MOVE_MAX(unit) \
    SOC_CONTROL(unit)->dual_hash_recurse_depth

/* Dual hash maximum recursion level */
#define SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit) \
    SOC_CONTROL(unit)->mcast_add_all_router_ports

/* Switching logic bypass mode */
#define SOC_SWITCH_BYPASS_MODE_NONE             0
#define SOC_SWITCH_BYPASS_MODE_L3_ONLY          1
#define SOC_SWITCH_BYPASS_MODE_L3_AND_FP        2

#define SOC_SWITCH_BYPASS_MODE(unit) \
    SOC_CONTROL(unit)->switch_bypass_mode

/*
 * LMD enabled ports. 2.5 Gbps stacking support
 */
#define SOC_LMD_PBM(unit) SOC_INFO(unit).lmd_pbm
#define SOC_LMD_ENABLED_PORT_SET(unit, port)     \
    SOC_PBMP_PORT_SET(SOC_INFO(unit).lmd_pbm, port)
#define IS_LMD_ENABLED_PORT(unit, port)          \
    SOC_PBMP_MEMBER(SOC_INFO(unit).lmd_pbm, port)

/*
 * Ports using HiGig2 encapsulation.
 * This is a cached value used for bcm_tx performance reasons.
 */
#define SOC_HG2_PBM(unit) SOC_INFO(unit).hg2_pbm
#define SOC_HG2_ENABLED_PORT_ADD(unit, port)     \
    SOC_PBMP_PORT_ADD(SOC_INFO(unit).hg2_pbm, port)
#define SOC_HG2_ENABLED_PORT_REMOVE(unit, port)     \
    SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).hg2_pbm, port)
#define IS_HG2_ENABLED_PORT(unit, port)          \
    SOC_PBMP_MEMBER(SOC_INFO(unit).hg2_pbm, port)

/* iterate over enabled blocks of type mask */
#define SOC_BLOCK_ITER(unit, var, mask) \
        for ((var) = 0; SOC_BLOCK_INFO(unit, var).type >= 0; (var)++) \
                if (SOC_INFO(unit).block_valid[var] && \
                    SOC_BLOCK_IS_TYPE(unit, var, mask))

#define SOC_MEM_BLOCK_MIN(unit, mem)    SOC_MEM_INFO(unit, mem).minblock
#define SOC_MEM_BLOCK_MAX(unit, mem)    (soc_mem_is_unified(unit, mem) ? \
                        SOC_MEM_INFO(unit, mem).minblock : \
                        SOC_MEM_INFO(unit, mem).maxblock)
#define SOC_MEM_BLOCK_ANY(unit, mem)    (SOC_INFO(unit).mem_block_any[mem])

#define SOC_MEM_BLOCK_VALID(unit, mem, blk)     \
                        ((blk) >= 0 && \
                         (blk) < (SOC_IS_ROBO(unit) ? SOC_ROBO_MAX_NUM_BLKS \
                         : SOC_MAX_NUM_BLKS) && \
                         (SOC_MEM_INFO(unit, mem).blocks & (1<<(blk))) && \
                         (SOC_INFO(unit).block_valid[blk]))

#define SOC_MEM_BLOCK_ITER(unit, mem, var) \
        for ((var) = SOC_MEM_BLOCK_MIN(unit, mem); \
                (var) <= SOC_MEM_BLOCK_MAX(unit, mem); \
                (var)++) \
                if ((SOC_MEM_INFO(unit, mem).blocks & (1 << (var))) && \
                     SOC_INFO(unit).block_valid[var])

/* Default dcb/dma values */
#define SOC_DEFAULT_DMA_SRCMOD_GET(_u)      SOC_PERSIST(_u)->dcb_srcmod
#define SOC_DEFAULT_DMA_SRCPORT_GET(_u)     SOC_PERSIST(_u)->dcb_srcport
#define SOC_DEFAULT_DMA_PFM_GET(_u)         SOC_PERSIST(_u)->dcb_pfm
#define SOC_DEFAULT_DMA_SRCMOD_SET(_u, _v)  SOC_PERSIST(_u)->dcb_srcmod = (_v)
#define SOC_DEFAULT_DMA_SRCPORT_SET(_u, _v) SOC_PERSIST(_u)->dcb_srcport = (_v)
#define SOC_DEFAULT_DMA_PFM_SET(_u, _v)     SOC_PERSIST(_u)->dcb_pfm = (_v)

/* Features cache */
#define SOC_FEATURE_GET(unit, feat)     \
        SHR_BITGET(SOC_CONTROL(unit)->features, feat)
#define SOC_FEATURE_SET(unit, feat)     \
        SHR_BITSET(SOC_CONTROL(unit)->features, feat)

/*
 * Various mutex controls
 */
#define SCHAN_LOCK(unit) \
        sal_mutex_take(SOC_CONTROL(unit)->schanMutex, sal_mutex_FOREVER)
#define SCHAN_UNLOCK(unit) \
        sal_mutex_give(SOC_CONTROL(unit)->schanMutex)

#define MIIM_LOCK(unit) \
        sal_mutex_take(SOC_CONTROL(unit)->miimMutex, sal_mutex_FOREVER)
#define MIIM_UNLOCK(unit) \
        sal_mutex_give(SOC_CONTROL(unit)->miimMutex)

#define TABLE_DMA_LOCK(unit) \
        sal_mutex_take(SOC_CONTROL(unit)->tableDmaMutex, sal_mutex_FOREVER)
#define TABLE_DMA_UNLOCK(unit) \
        sal_mutex_give(SOC_CONTROL(unit)->tableDmaMutex)

#define TSLAM_DMA_LOCK(unit) \
        sal_mutex_take(SOC_CONTROL(unit)->tslamDmaMutex, sal_mutex_FOREVER)
#define TSLAM_DMA_UNLOCK(unit) \
        sal_mutex_give(SOC_CONTROL(unit)->tslamDmaMutex)

#define SOC_CONTROL_LOCK(unit) \
        sal_mutex_take(SOC_CONTROL(unit)->socControlMutex, sal_mutex_FOREVER)
#define SOC_CONTROL_UNLOCK(unit) \
        sal_mutex_give(SOC_CONTROL(unit)->socControlMutex)

#define COUNTER_LOCK(unit) \
        sal_mutex_take(SOC_CONTROL(unit)->counterMutex, sal_mutex_FOREVER)
#define COUNTER_UNLOCK(unit) \
        sal_mutex_give(SOC_CONTROL(unit)->counterMutex)

#define OVERRIDE_LOCK(unit) \
       sal_mutex_take(SOC_CONTROL(unit)->overrideMutex, sal_mutex_FOREVER)
#define OVERRIDE_UNLOCK(unit) \
       sal_mutex_give(SOC_CONTROL(unit)->overrideMutex)

#define SPI_LOCK \
    sal_mutex_take(spiMutex, sal_mutex_FOREVER)
#define SPI_UNLOCK \
    sal_mutex_give(spiMutex)

#define SOC_EGRESS_METERING_LOCK(unit) \
        sal_mutex_take(SOC_CONTROL(unit)->egressMeteringMutex, \
                       sal_mutex_FOREVER)
#define SOC_EGRESS_METERING_UNLOCK(unit) \
        sal_mutex_give(SOC_CONTROL(unit)->egressMeteringMutex)

/* features cache */
#define SOC_FEATURE_GET(unit, feat) \
    SHR_BITGET(SOC_CONTROL(unit)->features, feat)
#define SOC_FEATURE_SET(unit, feat) \
    SHR_BITSET(SOC_CONTROL(unit)->features, feat)

#define ARLDMA_SIZE_DEFAULT     1024    /* Size in 16-byte entries */


/*
 * Define the default number of milliseconds before a BIST operation
 * times out and fails.  Different values apply if the SAL boot flag
 * BOOT_F_QUICKTURN or BOOT_F_PLISIM are set.
 */

#define BIST_TIMEOUT            (1  * SECOND_MSEC)
#define BIST_TIMEOUT_QT         (1  * MINUTE_MSEC)
#define BIST_TIMEOUT_PLI        (20 * MINUTE_MSEC)

/*
 * Typedef: soc_stat_t
 * Purpose: Driver statistics counts (interrupts, errors, etc).
 */
typedef struct soc_stat_s {
    uint32      intr;           /* Total interrupt count */
    uint32      intr_sce;       /* S-Channel error interrupt count */
    uint32      intr_sc;        /* S-Channel interrupt count */
    uint32      intr_ls;        /* Link status interrupt count */
    uint32      intr_gbp;       /* GBP Full interrupt count */
    uint32      intr_pci_fe;    /* PCI Fatal Error interrupt count */
    uint32      intr_pci_pe;    /* PCI Parity Error interrupt count */
    uint32      intr_arl_d;     /* ARL message dropped interrupt count */
    uint32      intr_arl_m;     /* ARL message ready interrupt count */
    uint32      intr_arl_x;     /* ARL DMA xfer done interrupt count */
    uint32      intr_arl_0;     /* ARL DMA count=0 interrupt count */
    uint32      intr_i2c;       /* I2C controller interrupt count */
    uint32      intr_mii;       /* MII interrupt count */
    uint32      intr_stats;     /* Stats DMA interrupt count */
    uint32      intr_desc;      /* DMA desc done interrupt count */
    uint32      intr_chain;     /* DMA chain done interrupt count */
    uint32      intr_mmu;       /* MMU status interrupt count */
    uint32      intr_tdma;      /* Table DMA complete interrupt count */
    uint32      intr_tslam;     /* Table SLAM DMA complete interrupt count */
    uint32      intr_mem_cmd[3]; /* Memory command complete interrupt count */
    uint32      intr_chip_func[5]; /* Chip functional interrupt count */
    uint32      intr_fifo_dma[4];  /* Fifo-dma interrupt count */

    uint32      schan_op;       /* Number of S-Channel operations */
    uint32      mem_cmd_op;     /* Number of memory command operations */

    uint32      err_sdram;      /* SDRAM parity error count */
    uint32      err_cfap;       /* CFAP oversubscribed count */
    uint32      err_fcell;      /* Unexpected First cell count */
    uint32      err_sr;         /* MMU Soft Reset count */
    uint32      err_cellcrc;    /* CBP cell CRC count */
    uint32      err_cbphp;      /* CBP header parity count */
    uint32      err_npcell;     /* MMU cells not in packet count */
    uint32      err_mp;         /* Memory parity error count */
    uint32      err_pdlock;     /* PLL/DLL Lock loss count */
    uint32      err_cpcrc;      /* Cell pointer CRC error count */
    uint32      err_cdcrc;      /* Cell data CRC error count */
    uint32      err_fdcrc;      /* Frame data CRC error count */
    uint32      err_cpbcrc;     /* Cell pointer block CRC error count */
    uint32      err_multi;      /* Multiple error count */
    uint32      err_invalid;    /* Invalid schan error count */
    uint32      err_sc_tmo;     /* S-Channel operation timeout count */
    uint32      err_mii_tmo;    /* MII operation timeout count */
    uint32      err_mc_tmo;     /* Memory command operation timeout count */

    uint32      arl_msg_ins;    /* Count of ARL insert messages processed */
    uint32      arl_msg_del;    /* Count of ARL delete messages processed */
    uint32      arl_msg_bad;    /* Count of bad messages processed */
    uint32      arl_msg_tot;    /* Count of all ARL messages */

    uint32      dma_rpkt;       /* Packets received by CPU */
    uint32      dma_rbyt;       /* Bytes received by CPU */
    uint32      dma_tpkt;       /* Packets transmitted by CPU */
    uint32      dma_tbyt;       /* Bytes transmitted by CPU */

    uint32      dv_alloc;       /* Number of DV alloc's */
    uint32      dv_free;        /* Number of DV free's */
    uint32      dv_alloc_q;     /* Free list satisfied DV allocs */
} soc_stat_t;

/*
 * Typedef: soc_memstate_t
 * Purpose: Maintain per-memory information
 * Notes: To avoid deadlock, do not use MEM_LOCK while holding SCHAN_LOCK.
 *      MEM_LOCK must be held while manipulating memory caches in any way.
 */

#define SOC_MEM_STATE(unit, mem)        SOC_CONTROL(unit)->memState[mem]

#define MEM_LOCK(unit, mem) \
        sal_mutex_take(SOC_MEM_STATE(unit, mem).lock, sal_mutex_FOREVER)
#define MEM_UNLOCK(unit, mem) \
        sal_mutex_give(SOC_MEM_STATE(unit, mem).lock)

#define CACHE_VMAP_OP(vmap, index, BOOL_OP) \
        ((vmap)[(index) / 8] BOOL_OP (1 << ((index) % 8)))

#define CACHE_VMAP_SET(vmap, index)     CACHE_VMAP_OP(vmap, index, |=)
#define CACHE_VMAP_CLR(vmap, index)     CACHE_VMAP_OP(vmap, index, &= ~)
#define CACHE_VMAP_TST(vmap, index)     CACHE_VMAP_OP(vmap, index, &)

typedef struct soc_memstate_s {
    sal_mutex_t lock;                   /* Table update lock */
                                        /*   (sorted tables only) */
    uint32     *cache[SOC_MAX_NUM_BLKS];/* Write-through cache when non-NULL */
    uint8      *vmap[SOC_MAX_NUM_BLKS]; /* Cache entry valid bitmap */
} soc_memstate_t;

/*
 * Typedef: soc_driver_t
 * Purpose: Chip driver.  All info about a particular device type.
 * Notes: These structures are automatically generated by mcm.
 *        Used in soc/mcm/bcm*.c files.
 */
typedef struct soc_driver_s {
    soc_chip_types      type;                   /* the chip type id */
    char                *chip_string;           /* chip string for var defs */
    char                *origin;                /* regsfile origin */
    uint16              pci_vendor;             /* nominal PCI vendor */
    uint16              pci_device;             /* nominal PCI device */
    uint8               pci_revision;           /* nominal PCI revision */
    int                 num_cos;                /* classes of service */
    soc_reg_info_t      **reg_info;             /* register array */
    soc_mem_info_t      **mem_info;             /* memory array */
    soc_mem_t           **mem_aggr;             /* memory aggregate */
    soc_block_info_t    *block_info;            /* block array */
    soc_port_info_t     *port_info;             /* port array */
    soc_cmap_t          *counter_maps;          /* counter map */
                                                /* indexed by soc_ctr_type_t */
    soc_feature_fun_t   feature;                /* boolean feature function */
    soc_init_chip_fun_t init;                   /* chip init function */
    drv_if_t *services;
} soc_driver_t;

typedef int (*soc_misc_init_f)(int);
typedef int (*soc_mmu_init_f)(int);
typedef int (*soc_age_timer_get_f)(int, int *, int *);
typedef int (*soc_age_timer_max_get_f)(int, int *);
typedef int (*soc_age_timer_set_f)(int, int, int);

/*
 * Typedef: soc_functions_t
 * Purpose: Chip driver functions that are not automatically generated.
 */
typedef struct soc_functions_s {
    /* Get/set age timer value and enable in device, if supported */
    soc_misc_init_f                    soc_misc_init;
    soc_mmu_init_f                     soc_mmu_init;
    soc_age_timer_get_f                soc_age_timer_get;
    soc_age_timer_max_get_f            soc_age_timer_max_get;
    soc_age_timer_set_f                soc_age_timer_set;
} soc_functions_t;


/*
 * This define tells whether a driver is active (not null).
 * The null driver is allocated, but is all zeros.  We assume that
 * the block list (block_info) is non-null for non-null drivers.
 */
#define SOC_DRIVER_ACTIVE(i) (soc_base_driver_table[i]->block_info)
#define SOC_ROBO_DRIVER_ACTIVE(i) (soc_robo_base_driver_table[i]->block_info)

/* Find an active chip to get reg info from base driver table. */
#define SOC_FIRST_ACTIVE_DRIVER(chip) \
    do { \
        chip = 0; \
        while (!SOC_DRIVER_ACTIVE(chip) && chip < SOC_NUM_SUPPORTED_CHIPS) { \
            chip++; \
        } \
        assert(chip < SOC_NUM_SUPPORTED_CHIPS); \
    } while (0)

#define SOC_ROBO_DRIVER_ACTIVE(i) (soc_robo_base_driver_table[i]->block_info)

/* Find an active chip to get reg info from base driver table. */
#define SOC_ROBO_FIRST_ACTIVE_DRIVER(chip) \
    do { \
    chip = 0; \
    while (!SOC_ROBO_DRIVER_ACTIVE(chip) && chip < SOC_ROBO_NUM_SUPPORTED_CHIPS) { \
        chip++; \
    } \
    assert(chip < SOC_ROBO_NUM_SUPPORTED_CHIPS); \
    } while (0)

extern soc_driver_t *soc_chip_driver_find(uint16 dev_id, uint8 rev_id);
extern void soc_chip_dump(int unit, soc_driver_t *d);
extern void soc_robo_chip_dump(int unit, soc_driver_t *d);

#define SOC_UNIT_NAME(unit) \
    (SOC_CHIP_NAME(SOC_DRIVER(unit)->type))

#define SOC_UNIT_GROUP(unit) \
    (soc_chip_group_names[soc_chip_type_map[SOC_DRIVER(unit)->type]])


/*
 * Typedef: soc_fm_t
 * Purpose: For each filter mask entry, cache the index of the first rule
 *          using that mask, and the number of rules using that mask.
 *          (All rules that use a given mask are necessarily contiguous.)
 *          These values speed up searches in the FFP, and on 5690 they
 *          are required by the hardware.
 */

#define SOC_FFP_MASK_MAX        24      /* Max # masks of any chip */

typedef struct soc_fm_s {
    uint16      start, count;
} soc_fm_t;

/*
 * Typedef: soc_ipg_t
 * Purpose: IFG register setting for all speed/duplex combinations
 */

typedef struct soc_ipg_s {
    uint32      hd_10;
    uint32      hd_100;
    uint32      hd_1000;
    uint32      hd_2500;
    uint32      fd_10;
    uint32      fd_100;
    uint32      fd_1000;
    uint32      fd_2500;
    uint32      fd_10000;
    uint32      fe_hd_10;
    uint32      fe_hd_100;
    uint32      fe_fd_10;
    uint32      fe_fd_100;
    uint32      gth_hd_10;
    uint32      gth_hd_100;
    uint32      gth_fd_10;
    uint32      gth_fd_100;
    uint32      ge_hd_1000;
    uint32      ge_fd_1000;
    uint32      ge_hd_2500;
    uint32      ge_fd_2500;
    uint32      bm_fd_10000;
} soc_ipg_t;

/*
 * soc_info_t is part of the per-unit soc_control_t.
 * It gets filled in at run time based on the port_info and block_info
 * from the driver structure.
 */
typedef struct {
        int     port[SOC_MAX_NUM_PORTS];
        int     num;                    /* number of entries used in port[] */
        int     min;                    /* minimum bit on */
        int     max;                    /* maximum bit on */
        pbmp_t  bitmap;
} soc_ptype_t;

typedef struct {
        soc_chip_types  driver_type;    /* chip type (with rev) */
        soc_chip_groups_t       driver_group;   /* chip type (no rev) */

        soc_chip_e              chip_type;

        uint32          chip;           /* chip id bits */
#define SOC_INFO_STRATA1        0x000001          /* 5600/80 */
#define SOC_INFO_STRATA2        0x000002          /* 5605/15 */
#define SOC_INFO_DRACO1         0x000004          /* 5690 */
#define SOC_INFO_HERCULES1      0x000008          /* 5670 */
#define SOC_INFO_LYNX           0x000010          /* 5673/74 */
#define SOC_INFO_TUCANA         0x000020          /* 5665 */
#define SOC_INFO_DRACO15        0x000040          /* 5695 */
#define SOC_INFO_HERCULES15     0x000080          /* 5675 */
#define SOC_INFO_EASYRIDER      0x000100          /* 5660x */
#define SOC_INFO_FIREBOLT       0x000200          /* 56504 */
#define SOC_INFO_EASYRIDER10    0x000800          /* 5661x */
#define SOC_INFO_FELIX          0x001000          /* 5610x */
#define SOC_INFO_HELIX          0x002000          /* 5630x */
#define SOC_INFO_GOLDWING       0x004000          /* 56580 */
#define SOC_INFO_HUMV           0x008000          /* 56700 */
#define SOC_INFO_BRADLEY        0x010000          /* 56800 */
#define SOC_INFO_RAPTOR         0x020000          /* 56218 */
#define SOC_INFO_FELIX15        0x040000          /* 5611x */
#define SOC_INFO_HELIX15        0x080000          /* 5631x */
#define SOC_INFO_FIREBOLT2      0x100000          /* 56514 */
#define SOC_INFO_SCORPION       0x200000          /* 56820 */
#define SOC_INFO_TRIUMPH        0x400000          /* 56624 */
#define SOC_INFO_RAVEN          0x800000          /* 56224 */
#define SOC_INFO_VALKYRIE      0x2000000          /* 56680 */
#define SOC_INFO_CONQUEROR     0x4000000          /* 56725 */
#define SOC_INFO_STRATA         (SOC_INFO_STRATA1 | SOC_INFO_STRATA2)
#define SOC_INFO_DRACO          (SOC_INFO_DRACO1 | SOC_INFO_DRACO15)
#define SOC_INFO_HERCULES       (SOC_INFO_HERCULES1 | SOC_INFO_HERCULES15)
#define SOC_INFO_XGS12_SWITCH   (SOC_INFO_DRACO | SOC_INFO_LYNX | \
                                 SOC_INFO_TUCANA)
#define SOC_INFO_FB             (SOC_INFO_FIREBOLT | SOC_INFO_FIREBOLT2)
#define SOC_INFO_HELIX_ANY      (SOC_INFO_HELIX | SOC_INFO_HELIX15)
#define SOC_INFO_FELIX_ANY      (SOC_INFO_FELIX | SOC_INFO_FELIX15)
#define SOC_INFO_FX_HX          (SOC_INFO_FELIX_ANY | SOC_INFO_HELIX_ANY | \
                                 SOC_INFO_RAPTOR | SOC_INFO_RAVEN)
#define SOC_INFO_FB_FX_HX       (SOC_INFO_FB | SOC_INFO_FX_HX)
#define SOC_INFO_HB_GW          (SOC_INFO_HUMV | SOC_INFO_BRADLEY | \
                                 SOC_INFO_GOLDWING)
#define SOC_INFO_TR_VL          (SOC_INFO_TRIUMPH | SOC_INFO_VALKYRIE)
#define SOC_INFO_SC_CQ          (SOC_INFO_SCORPION | SOC_INFO_CONQUEROR)
#define SOC_INFO_HBX            (SOC_INFO_HB_GW | SOC_INFO_SC_CQ)
#define SOC_INFO_TRX            (SOC_INFO_TR_VL | SOC_INFO_SC_CQ)
#define SOC_INFO_FBX            (SOC_INFO_FB_FX_HX | SOC_INFO_HB_GW | SOC_INFO_TRX)
#define SOC_INFO_XGS3_SWITCH    (SOC_INFO_FBX | SOC_INFO_EASYRIDER)
#define SOC_INFO_XGS3_FABRIC    (SOC_INFO_HUMV | SOC_INFO_CONQUEROR)
#define SOC_INFO_XGS_SWITCH     (SOC_INFO_XGS12_SWITCH | SOC_INFO_XGS3_SWITCH)
#define SOC_INFO_XGS12_FABRIC   (SOC_INFO_HERCULES)
#define SOC_INFO_XGS_FABRIC     (SOC_INFO_XGS12_FABRIC | SOC_INFO_XGS3_FABRIC)
#define SOC_INFO_XGS            (SOC_INFO_XGS_FABRIC | SOC_INFO_XGS_SWITCH)
#define SOC_INFO_ROBO5324           0x200000        /* 5324 */
#define SOC_INFO_ROBO5324_A1        0x400000        /* 5324 A1 */
#define SOC_INFO_ROBO5396           0x800000        /* 5396 */
#define SOC_INFO_ROBO5389           0x1000000       /* 5389 */
#define SOC_INFO_ROBO5398           0x2000000       /* 5398 */
#define SOC_INFO_ROBO5348           0x4000000       /* 5348 */
#define SOC_INFO_ROBO53242          0x8000000       /* 53242 */
#define SOC_INFO_ROBO5397           0x10000000      /* 5397 */
#define SOC_INFO_ROBO5347           0x20000000
#define SOC_INFO_ROBO53262          0x40000000      /* 53262 */
#define SOC_INFO_ROBO5395           0x80000000      /* 5395 */
#define SOC_INFO_ROBO53115          0x100000        /* 53115 */
#define SOC_INFO_ROBO53118          0x80000        /* 53118 */
#define SOC_INFO_ROBO      	(SOC_INFO_ROBO5324 | \
    SOC_INFO_ROBO5396 | SOC_INFO_ROBO5389 | SOC_INFO_ROBO5398 | \
    SOC_INFO_ROBO5324_A1 | SOC_INFO_ROBO5348 | SOC_INFO_ROBO5397 | \
    SOC_INFO_ROBO5347 | SOC_INFO_ROBO5395 | SOC_INFO_ROBO53242 | \
    SOC_INFO_ROBO53262 | SOC_INFO_ROBO53115 | SOC_INFO_ROBO53118)
        int             port_num;       /* count of entries in port_info */
        int             block_num;      /* count of entries in block_info */
        soc_ptype_t     fe;
        soc_ptype_t     ge;
        soc_ptype_t     xe;
        soc_ptype_t     hg;
        soc_ptype_t     hl;                     /* Higig Lite */
        soc_ptype_t     st;                     /* hg | ge (Higig Lite) */
        soc_ptype_t     gx;                     /* gx (Unicore) */
        soc_ptype_t     xg;                     /* xg (Unicore + Octal Serdes) */
        soc_ptype_t     lb;                     /* Internal loopback */
        soc_ptype_t     spi;                    /* spi interface port */
        soc_ptype_t     spi_subport;            /* spi subport */
        soc_ptype_t     sci;                    /* fabric control  */
        soc_ptype_t     sfi;                    /* fabric data     */

        soc_ptype_t     port;                   /* fe|ge|xe|hg|spi|fl */
        soc_ptype_t     ether;                  /* fe|ge|xe */
        soc_ptype_t     all;                    /* fe|ge|xe|hg|cmic|fl */
        pbmp_t          cmic_bitmap;            /* cmic bitmap */
        pbmp_t          lmd_pbm;                /* LMD port bitmap */
        pbmp_t          spi_bitmap;             /* spi bitmap */
        pbmp_t          s_pbm;                  /* 2.5 Gpbs Comboserdes ports */
        pbmp_t          gmii_pbm;               /* GMII port bitmap */
        pbmp_t          hg2_pbm;                /* HiGig2 encap port bitmap */
        pbmp_t          lb_pbm;                 /* Internal loopback ports */
        int             ipic_port;              /* ipic port or -1 */
        int             ipic_block;             /* ipic block or -1 */
        int             cmic_port;              /* cmic port */
        int             cmic_block;             /* cmic block */
        int             arl_block;              /* arl block */
        int             mmu_block;              /* mmu block */
        int             mcu_block;              /* mcu block */
        int             ipipe_block;            /* ipipe block (FB) */
        int             ipipe_hi_block;         /* ipipe_hi block (FB) */
        int             epipe_block;            /* epipe block (FB) */
        int             epipe_hi_block;         /* epipe_hi block (FB) */
        int             igr_block;              /* Ingress block (ER) */
        int             egr_block;              /* Egress block (ER) */
        int             bse_block;              /* bse block (ER) */
        int             cse_block;              /* cse block (ER) */
        int             hse_block;              /* hse block (ER) */
        int             bsafe_block;            /* BSAFE block (FB/ER) */
        int             otpc_block;             /* OTPC block (Raven) */
        int             esm_block;              /* External Search Machine block (TR) */
        int             exp_port;               /* expansion port or -1 */
        int             exp_block;              /* expansion block or -1 */
        int             spi_port;               /* spi port */ /* Add for Robo */
        int             spi_block;              /* spi block */
        int             inter_block;            /* internal mii block*/
        int             exter_block;            /* external mii block*/

        soc_block_t     has_block;              /* or of all blocks */
        int             port_type[SOC_MAX_NUM_PORTS];
        char            port_name[SOC_MAX_NUM_PORTS][8];
        int             port_offset[SOC_MAX_NUM_PORTS]; /* 3 for ge3 */
        int             port_speed_max[SOC_MAX_NUM_PORTS]; /* max port speed */
        char            block_valid[SOC_MAX_NUM_BLKS];  /* is block used? */
        pbmp_t          block_bitmap[SOC_MAX_NUM_BLKS];
        int             block_port[SOC_MAX_NUM_BLKS];
        char            block_name[SOC_MAX_NUM_BLKS][8];
        int             mem_block_any[NUM_SOC_MEM];
        int             modid_count;            /* needed modids */
        int             modid_max;              /* maximum modid supported */
        int             trunk_bit_pos;          /* Bit-0 is 0 */
        int             port_addr_max;          /* max addressable ports */
        int             hg_offset;              /* offset of hg ports in pbmp */
        int             num_cpu_cosq;           /* number of cpu cosq */
        char            lock_info[128];         /* lock debug info */
} soc_info_t;


typedef int (*soc_rcpu_schan_op_cb)(
    int unit,
    schan_msg_t *msg,
    int dwc_write,
    int dwc_read);

extern int soc_rcpu_schan_op_register(int unit, soc_rcpu_schan_op_cb f);
extern int soc_rcpu_schan_op_unregister(int unit);


/*
 * Typedef: soc_control_t
 * Purpose: SOC Control Structure.  All info about a device instance.
 */

typedef struct soc_control_s {
    uint32      soc_flags;           /* Flags for this device */
#define   SOC_F_ATTACHED     0x01    /* Device attached */
#define   SOC_F_INITED       0x02    /* Device inited */
#define   SOC_F_LSE          0x04    /* Link SCAN enabled in SW */
#define   SOC_F_SL_MODE      0x08    /* Device is in SL stack mode */
#define   SOC_F_POLLED       0x10    /* Polled IRQ mode */
#define   SOC_F_URPF_ENABLED 0x20    /* Unicast rpf enabled on device.*/
#define   SOC_F_IPMCREPLSHR  0x40    /* IPMC replication with lists.*/
#define   SOC_F_BUSY         0x80    /* Device is not fully out of Reset */
#define   SOC_F_MEM_CLEAR_USE_DMA 0x100 /* Device should use table dma  */
                                        /* for memory clear operations. */
#define   SOC_F_GPORT        0x200   /* Device should use GPORT for  */
                                     /* source/destination notation. */
#define   SOC_F_RCPU_SCHAN   0x400   /* Indicates that SCHAN operations will
                                          be sent over RCPU mechanism */
    /* Mask of flags that are persistent across init */

#   define      SOC_F_RESET     (SOC_F_ATTACHED|SOC_F_INITED|SOC_F_POLLED)

    /* Port and block information filled in when attached or inited */

    soc_info_t  info;

    /* currently used by sbx devices only */
    void        *drv;

    /* Disable subsets of registers and memories */

    uint32      disabled_reg_flags;
    uint32      disabled_mem_flags;

    /* Miscellaneous chip state */

    uint32      gbpMemSize;     /* GBP size in bytes */
    int         gbpBanks;       /* Number of banks, 2 or 4 */
    int         gbpDDR;         /* Boolean for DDR vs non-DDR */
    int         gbpWidth;       /* 128 or 64 bits wide */
    int         gbpFullEnable;  /* GBP Full interrupts (see soc/intr.c) */
    int         gbpFullTime;

    int         pciParityDPC;   /* True if PCI Parity Error DPC queued */
    int         pciFatalDPC;    /* True if PCI Fatal Error DPC queued */

    uint32      irq_mask;       /* Cached copy of CMIC_IRQ_MASK */

    uint32      board_type;     /* From I2C */
    uint32      remote_cpu;
    soc_rcpu_schan_op_cb    soc_rcpu_schan_op; /* RCPU SCHAN function */

    /* Operation Management */

    sal_mutex_t schanMutex;     /* S-Channel mutual exclusion */
    int         schanTimeout;   /* Operation timeout in microseconds */
    sal_sem_t   schanIntr;      /* S-Channel DONE interrupt notification */
    int         schanIntrEnb;   /* Allow interrupts to be used */
    sal_usecs_t schanIntrBlk;   /* Length of time to block the S-channel
                                   error interrupt after one occurs */
    uint32      schan_result;   /* CMIC_SCHAN_CTRL register after
                                   last S-channel operation */
    uint32  schan_override; /* ER cmd memory write override during
                   warm reboot */

    sal_mutex_t miimMutex;      /* MII registers mutual exclusion */
    int         miimTimeout;    /* Operation timeout in microseconds */
    sal_sem_t   miimIntr;       /* MII DONE interrupt notification */
    int         miimIntrEnb;    /* Use interrupts instead of polling */

    int         bistTimeout;    /* Operation timeout in milliseconds */

    sal_mutex_t tableDmaMutex;  /* Table DMA register mutual exclusion */
    int         tableDmaTimeout;/* Operation timeout in microseconds */
    sal_sem_t   tableDmaIntr;   /* Table DMA DONE interrupt FB/ER */
    int         tableDmaIntrEnb;/* Use interrupts instead of polling */

    sal_mutex_t tslamDmaMutex;  /* Table SLAM DMA register mutual exclusion */
    int         tslamDmaTimeout;/* Operation timeout in microseconds */
    sal_sem_t   tslamDmaIntr;   /* Table SLAM DMA DONE interrupt FB/ER */
    int         tslamDmaIntrEnb;/* Use interrupts instead of polling */

    sal_mutex_t socControlMutex;/* SOC control structure mutual exclusion */
    sal_mutex_t counterMutex;   /* Counter activity mutual exclusion */

    int         memCmdTimeout;  /* Operation timeout in microseconds */
    sal_sem_t   memCmdIntr[3];  /* Memory command interrupt ER */
    int         memCmdIntrEnb;  /* Use interrupts instead of polling */

    sal_sem_t   ipfixIntr;      /* IPFIX FIFO DMA interrupt */
    int         ipfixIntrEnb;   /* Use interrupts instead of polling */

    int         l2modDmaIntrEnb; /* Use interrupts instead of polling */

    sal_mutex_t overrideMutex;  /* OVERRIDE registers mutual exclusion */

    /* MAC IPG Configuration */

    soc_ipg_t   ipg;
    /* Statistics counts */

    soc_stat_t  stat;

    /* On-chip memory management */

    soc_memstate_t
                memState[NUM_SOC_MEM];

#ifdef INCLUDE_MEM_SCAN
    VOL sal_thread_t
                mem_scan_pid;
    char        mem_scan_name[16];      /* Memory scan task name */
    VOL int     mem_scan_rate;          /* Max entries to scan per pass */
    VOL sal_usecs_t
                mem_scan_interval;      /* Task wake up interval */
    sal_sem_t   mem_scan_notify;
#endif

    /* Hardware ARL Table */

    VOL sal_thread_t
                arl_pid;        /* Software ARL message task */
    char        arl_name[16];   /* ARL message task name */
    int         arl_exit;       /* Request for ARL thread to exit */
    sal_sem_t   arl_notify;
    VOL sal_usecs_t
        arl_interval;   /* Interval between L2 table sync passes */
    int         arl_notified;   /* TRUE if notify semaphore already given */
    int         arl_dma_cnt0;   /* can only be set in intr; only cleared
                                   when done processing in arl thread. */
    int         arl_dma_xfer;   /* can only be set in intr; only cleared
                                   when done processing in arl thread. */
    int         arl_mbuf_done;  /* can only be set in intr; only cleared
                                   when done processing in arl thread. */
    int         arl_msg_drop;   /* can only be set in intr; only cleared
                                   when processed in arl_mbuf_process. */
    sal_time_t  arlDropWarn;    /* Timestamp of last warning msg printed */
    uint32      arlResyncDelay; /* Time after drop before resync (0=never) */
    int         arlBufSize;     /* Number of entries in arlBuf */
    schan_msg_arlins_t
                *arlBuf;        /* ARL DMA Message Buffer */
    schan_msg_arlins_t
                *arlPtr;        /* Consumer pointer */
    shr_avl_t   *arlShadow;     /* Shadow copy of ARL or L2X table */
    sal_mutex_t arlShadowMutex;
    int         arlMode;        /* Current ARL mode */
    int         arlNextBuf;     /* Poll mode: next ARL Message Buffer (0-3) */
    int         arlRateLimit;   /* Limit in msgs/sec */

    /* robo */
    l2_arl_sw_entry_t *arl_table;
    sal_mutex_t arl_table_Mutex;
    sal_mutex_t arl_mem_search_Mutex;
    sal_mutex_t mem_rwctrl_reg_Mutex;


    /* Hardware L2X Table */

    VOL sal_thread_t
                l2x_pid;        /* Software L2 shadow task */
    char        l2x_name[16];   /* L2X shadow task name */
    VOL sal_usecs_t
                l2x_interval;   /* Interval between L2 table sync passes */
    VOL uint32  l2x_flags;
    sal_sem_t   l2x_notify;
    sal_mutex_t l2x_del_sync;
#define SOC_L2_DEL_SYNC_LOCK(s) \
    sal_mutex_take((s)->l2x_del_sync, sal_mutex_FOREVER)

#define SOC_L2_DEL_SYNC_UNLOCK(s)  sal_mutex_give((s)->l2x_del_sync)

    sal_sem_t   l2x_lock;
    int         l2x_shadow_hit_bits;
    int         l2x_l2mc_in_l2entry;
    int         l2x_external;
    int         l2x_group_enable;

    /* Hash keys configuration */
    uint32      hash_key_config;
#       define  L3X_IPMC_ENABLE  0x0001  /* ipmc is enabled */
#       define  L3X_IPMC_SIP     0x0002  /* lookup includes src ip */
#       define  L3X_IPMC_SIP0    0x0004  /* lookup with src ip set to zero */
#       define  L3X_IPMC_VLAN    0x0008  /* lookup includes vlan */

    /* Hash mask cache */

    uint32      hash_mask_l2x;
    int         hash_bits_l2x;
    uint32      hash_mask_l3x;
    int         hash_bits_l3x;
    uint32      hash_mask_vlan_mac;
    int         hash_bits_vlan_mac;
    uint32      hash_mask_egr_vlan_xlate;
    int         hash_bits_egr_vlan_xlate;
    uint32      hash_mask_mpls;
    int         hash_bits_mpls;
    uint32      hash_mask_l2x_ext;
    int         hash_bits_l2x_ext;
    uint32      hash_mask_l3v6;
    int         hash_bits_l3v6;
    uint32      hash_mask_my_station;
    int         hash_bits_my_station;

    uint32      dual_hash_recurse_depth;

    /* Multicast ranges */

    int mcast_size;
    int ipmc_size;
    int higig2_bcast_size;
    int higig2_mcast_size;
    int higig2_ipmc_size;

    int mcast_add_all_router_ports;

    /* Switching logic bypass mode */
    int switch_bypass_mode;

    /* Link status */

    pbmp_t      link_mask1;     /* Override for 5605 STP workaround */
    pbmp_t      link_mask2;     /* Override for LCC drain */

    int         soc_link_pause; /* Link scan pause count */
    void        (*soc_link_callout)(int);

    pbmp_t     link_fwd;    /* Forwarding ports current value */

    /* DMA Management */
    uint32  soc_robo_dma_lock;  /* Lock for updating DMA operations etc. */
    uint32      soc_dma_lock;   /* Lock for updating DMA operations etc. */
#   define SOC_DMA_LOCK(s)      ((s)->soc_dma_lock = sal_splhi())
#   define SOC_DMA_UNLOCK(s)    (void)sal_spl((s)->soc_dma_lock)
    sdc_t       soc_channels[N_DMA_CHAN];
    int         soc_max_channels;       /* maximum channel count */
    sdc_t       *soc_dma_default_rx;    /* Default RX channel */
    sdc_t       *soc_dma_default_tx;    /* Default TX channel */
    int         dma_droptx;             /* Any channels in drop tx mode */
    dv_t        *soc_dv_free;           /* Available DVs */
    int         soc_dv_free_cnt;        /* # on free list */
    int         soc_dv_cnt;             /* # allowed on free list */
    int         soc_dv_size;            /* Number DCBs in free list entries */
    uint32      *tx_purge_pkt;          /* DMA able buffer for TX Purge */

    dcb_op_t    dcb_op;                 /* DCB operations */

    /*
     * Packet Counter Management
     *
     * The hardware counter buffer is allocated using I/O-safe
     * memory because counter values may be transferred to the buffer
     * via DMA.  The software counter buffer uses regular memory.
     */

    int         counter_portsize;       /* Bytes per port for counters */
    int         counter_bufsize;        /* Total memory allocated for dma */
    int         counter_n32;            /* Number of 32-bit counters */
    int         counter_n64;            /* Number of 64-bit counters */
    int         counter_n64_non_dma;    /* Number of non DMA 64-bit counters */
    int         counter_ports32;        /* Number of 32-bit ports */
    int         counter_ports64;        /* Number of 64-bit ports */
    int         counter_perport;        /* Number of counter per port */
    uint32      *counter_buf32;         /* Hardware counter DMA buffer */
    uint64      *counter_buf64;         /* Hardware counter DMA buffer */
    uint64      *counter_hw_val;        /* Hardware counters */
    uint64      *counter_sw_val;        /* Software accumulated counters */
    uint64      *counter_delta;         /* Deltas over last two collections */
    soc_cmap_t  *counter_map[SOC_MAX_NUM_PORTS];        /* per port cmap */

    VOL sal_thread_t
                counter_pid;            /* Counter DMA emulation task */
    char        counter_name[16];       /* Counter task name */
    VOL int     counter_interval;       /* Update interval in usec */
                                        /*   (Zero when thread not running) */
    sal_usecs_t counter_coll_prev;      /* Timestamp of previous collection */
    sal_usecs_t counter_coll_cur;       /* Timestamp of current collection */
    VOL pbmp_t  counter_pbmp;           /* Ports being scanned */
    VOL uint32  counter_flags;          /* SOC_COUNTER_F_XXX */
    VOL sal_sem_t
                counter_trigger;        /* Trigger counter thread attn */
    VOL sal_sem_t
                counter_intr;           /* Counter H/W interrupt notifier */
    VOL int     counter_sync_req;       /* soc_counter_sync() support */
    VOL int     counter_suspend;        /* Skip update for another interval */
    VOL int     counter_updating;       /* Processing update cycle */
    VOL sal_sem_t
        counter_notify;     /* Scan complete notification */
    int     counter_swap64;     /* DMA buf 64-bit values must
                       be word-swapped */
    soc_counter_non_dma_t *counter_non_dma; /* non counter DMA support
                                             * entries */

    /* Memory correction status */

    soc_mmu_error_t *mmu_errors;        /* Statistics tracking for MMU */
    uint32      *lla_cells_good;        /* LLA/PP entries known good */
    SHR_BITDCL  **lla_map;              /* LLA/PP analysis cache */
    int         *sbe_disable;           /* PP has entries w/ SBE errors */

    int         mmu_error_block;        /* Disable MMU errors */
    /* Chip driver pointers */

    soc_driver_t
                *chip_driver;
    soc_functions_t
                *soc_functions;

    void        *i2c_bus;               /* Cast to soc_i2c_bus_t */
    void        *bsc_bus;               /* Cast to soc_bsc_bus_t */
    uint16      max_vrf_id;             /* Maximum vrf id on the device. */
    uint16      max_address_class;      /* Maximum src/dst class         */
                                        /* on the device.                */
    uint16      max_interface_class;    /* Maximum interface class       */
                                        /* on the device.                */
    uint16      mem_clear_chunk_size;   /* Memory clear chunk size.      */
    sal_mutex_t egressMeteringMutex;    /* Egress metering access mutex. */

    /* Feature cache */

    SHR_BITDCLNAME(features, soc_feature_count);
} soc_control_t;

/*
 * Typedef: sop_memstate_t
 * Purpose: Maintain per-memory persistent information
 */

#define SOP_MEM_STATE(unit, mem)        SOC_PERSIST(unit)->memState[mem]

typedef struct sop_memstate_s {
    uint32      count[SOC_MAX_NUM_BLKS];/* Number of valid table entries */
                                        /*   (sorted tables only) */
    int         index_max;              /* May be updated per unit */
} sop_memstate_t;

typedef struct sop_memcfg_er_s {
    uint32      ext_table_cfg;
    uint32      host_hash_table_cfg;
    uint32      l3v4_search_offset;
    uint32      l3v6_search_offset;
    uint32      mvl_hash_table_cfg;
    uint32      mystation_search_offset;
    int         tcam_select;
} sop_memcfg_er_t;

/*
 * Typedef: soc_persist_t
 * Purpose: SOC Persistent Structure.
 *      All info about a device instance that must be saved across a reload.
 * Notes:
 *      Modifications to this structure will prevent reload.  A system running
 *      a given version would not be able to be upgraded to a modified version.
 */

typedef struct soc_persist_s {
    /* Miscellaneous chip state */

    int         version;        /* Persistent state version */

    int         debugMode;      /* True if MMU is in debug mode */

    /* DCB static configuration */

    uint32      dcb_srcmod;             /* Source module and port for dcbs */
    uint32      dcb_srcport;
    uint32      dcb_pfm;                /* Port Forwarding Mode */

    /* IPG Configuration per port */

    soc_ipg_t   ipg[SOC_MAX_NUM_PORTS];

    /* On-chip memory management */

    sop_memstate_t
                memState[NUM_SOC_MEM];
#ifdef SOC_MEM_L3_DEFIP_WAR
    uint32      l3_defip_map;   /* Unused L3_DEFIP blocks */
#endif

    sop_memcfg_er_t
                er_memcfg;

    /* Fast Filter Processor */

    int         filter_enable;  /* True if filtering enabled for chip */
    int         filter_update;  /* True if filter_masks should be updated */
    soc_fm_t    filter_masks[SOC_MAX_NUM_BLKS][SOC_FFP_MASK_MAX];

    /* Link status */

    pbmp_t      link_fwd;       /* Forwarding ports current value */

    /* Linkscan status (private; may only accessed by link.c) */

    pbmp_t      lc_pbm_link;            /* Ports currently up */
    pbmp_t      lc_pbm_link_change;     /* Ports needed to recognize down */
    pbmp_t      lc_pbm_override_ports;  /* Force up/Down ports */
    pbmp_t      lc_pbm_override_link;   /* Force up/Down status */

    /*
     * Stacking related:
     *     stack_ports_current:   Ports currently engaged in stacking
     *     stack_ports_inactive:  Ports stacked, but explicitly blocked
     *     stack_ports_simplex:   Ports stacked using simplex mode
     *     stack_ports_previous:  Last stack port; to detect changes
     */
    soc_pbmp_t stack_ports_current;
    soc_pbmp_t stack_ports_inactive;
    soc_pbmp_t stack_ports_previous;
} soc_persist_t;

#define SOC_FLAGS_CLR(s, f) {       \
    int _s = sal_splhi();       \
    (s)->soc_flags &= ~(f);     \
    sal_spl(_s);            \
}

/*
 * soc_control: Per-device non-persistent global driver state
 * soc_persist: Per-device persistent global driver state
 * soc_ndev: Number of devices found during probe
 */

extern soc_control_t    *soc_control[SOC_MAX_NUM_DEVICES];
extern soc_persist_t    *soc_persist[SOC_MAX_NUM_DEVICES];
extern int              soc_ndev_attached;

extern int              soc_ndev;
extern int      soc_eth_ndev;
extern int      soc_all_ndev;
extern int      soc_ndev_attached;
extern int      soc_eth_unit;
extern int      soc_mii_unit;
extern char     soc_reg_noinfo[];

extern soc_block_name_t soc_block_port_names[];
extern soc_block_name_t soc_block_names[];
extern sal_mutex_t spiMutex;

#if defined(BCM_WARM_BOOT_SUPPORT) || defined(BCM_EASY_RELOAD_SUPPORT)
extern uint32    soc_state[SOC_MAX_NUM_DEVICES];
#endif /* BCM_WARM_BOOT_SUPPORT || BCM_EASY_RELOAD_SUPPORT */

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * BCM Warm Boot Support
 *
 * Purpose:  Indicates whether the device is currently in reload
 *           state (performing Warm boot) or not.
 *
 *           If finer granularity is needed in the future, additional
 *           flags can be defined to control specific hardware accesses.
 */

#define SOC_ST_F_RELOADING         0x1    /* Warm boot in progress, device is reloading */
#define SOC_WARM_BOOT(unit)       (soc_state[unit] == SOC_ST_F_RELOADING)
#define SOC_WARM_BOOT_START(unit) (soc_state[unit] = SOC_ST_F_RELOADING)
#define SOC_WARM_BOOT_DONE(unit)  (soc_state[unit] = 0x0)

extern int soc_shutdown(int unit);
#else
#define SOC_WARM_BOOT(unit)       (0)
#define soc_shutdown(unit)        (SOC_E_UNAVAIL)
#define SOC_WARM_BOOT_START(unit) \
             do { \
             } while(0)

#define SOC_WARM_BOOT_DONE(unit)  \
             do { \
             } while(0)
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_EASY_RELOAD_SUPPORT)
/*
 * BCM Easy Reload Support
 *
 * Purpose:  Indicates whether the device is currently in reload
 *           state (performing Easy Reload) or not.
 *
 *           If finer granularity is needed in the future, additional
 *           flags can be defined to control specific hardware accesses.
 */

#define SOC_ST_F_EASY_RELOAD       0x2    /* Easy Reload is in progress */
/* Reload mode set/get macros */
#define SOC_IS_RELOADING(unit)    (soc_state[unit] == SOC_ST_F_EASY_RELOAD)
#define SOC_RELOAD_MODE_SET(unit, reload_mode) \
    if (reload_mode) {              \
    soc_state[unit] = SOC_ST_F_EASY_RELOAD; \
    } else {                    \
        soc_state[unit] = 0;            \
    }
#else
#define SOC_IS_RELOADING(unit) (0)
#define SOC_RELOAD_MODE_SET(unit, reload_mode)  \
             do { \
             } while(0)
#endif /* BCM_EASY_RELOAD_SUPPORT */

/*
 * Driver calls.
 */
extern int soc_attach(int unit);
extern int soc_attached(int unit);
extern int soc_reset(int unit);
extern int soc_init(int unit);
extern int soc_reset_init(int unit);
extern int soc_detach(int unit);
extern int soc_bist(int unit, soc_mem_t *mems, int num_mems, int timeout_msec);
extern int soc_bist_all(int unit);

extern int soc_bpdu_addr_set(int unit, int index, sal_mac_addr_t addr);
extern int soc_bpdu_addr_get(int unit, int index, sal_mac_addr_t *addr);

extern int soc_rcpu_schan_enable_set(int unit, int enable);

extern int soc_gbp_size_force(int unit,
                           uint32 size, int banks,
                           int ddr, int width);
extern int soc_gbp_size_probe(int unit);
extern int soc_gbp_init(int unit);
extern int soc_mmu_init(int unit);
extern int soc_misc_init(int unit);
extern const char *soc_dev_name(int unit);
extern void soc_info_config(int unit, soc_control_t *soc);
extern void soc_xgxs_lcpll_lock_check(int unit);
extern void soc_xport_type_update(int unit, soc_port_t port, int to_hg_port);
extern int  soc_robo_attach(int unit);
extern int  soc_robo_reset(int unit);
extern int  soc_robo_init(int unit);
extern int  soc_robo_reset_init(int unit);
extern int  soc_robo_detach(int unit);
extern int  soc_robo_dma_attach(int unit);
extern int  soc_robo_dma_init(int unit);
extern int _soc_robo_device_created(int unit);

#ifdef BCM_LEDPROC_SUPPORT
extern int soc_ledproc_config(int unit, const uint8 *program, int bytes);
#endif

#define SOC_PROPERTY_NAME_MAX   64
#define SOC_PROPERTY_VALUE_MAX  64

extern char *soc_property_get_str(int unit, const char *name);
extern uint32 soc_property_get(int unit, const char *name, uint32 defl);
extern pbmp_t soc_property_get_pbmp(int unit, const char *name,
                                    int defneg);
extern pbmp_t soc_property_get_pbmp_default(int unit, const char *name,
                                            pbmp_t def_pbmp);
extern char *soc_property_port_get_str(int unit, soc_port_t port,
                                       const char *name);
extern uint32 soc_property_port_get(int unit, soc_port_t port,
                                    const char *name, uint32 defl);
extern uint32 soc_property_suffix_num_get(int unit, int tc,
                                          const char *name,
                                          const char *suffix, uint32 defl);

extern char *soc_block_port_name_lookup(soc_block_t);
extern char *soc_block_name_lookup(soc_block_t);
extern soc_block_t soc_block_port_name_match(char *);
extern soc_block_t soc_block_name_match(char *);


#define SOC_PCI_DEV_TYPE      SAL_PCI_DEV_TYPE    /* Type of PCI device */
#define SOC_SPI_DEV_TYPE      SAL_SPI_DEV_TYPE    /* Type of SPI device
                             accessed via SMP */
#define SOC_EB_DEV_TYPE      SAL_EB_DEV_TYPE    /* Type of EB device */
#define SOC_ICS_DEV_TYPE      SAL_ICS_DEV_TYPE    /* Type of ICS device */

#define SOC_SWITCH_DEV_TYPE   SAL_SWITCH_DEV_TYPE /* Type of switch device */
#define SOC_ETHER_DEV_TYPE    SAL_ETHER_DEV_TYPE  /* Type of outband ethernet
                             device */
#define SOC_TIMEOUT_VAL 100 /* Times for retrying */
/* soc/intr.c exported routines */

extern void soc_intr(void *unit);
extern uint32  soc_intr_enable(int unit, uint32 mask);
extern uint32  soc_intr_disable(int unit, uint32 mask);

extern int soc_max_vrf_set(int unit, int value);

#if defined(BROADCOM_DEBUG)                      /* Debug Support */
extern int      soc_dump(int unit, const char *pfx);
extern int  soc_robo_dump(int unit, const char *pfx);
#endif /* defined(BROADCOM_DEBUG) */

/* MAC Core initialization */
extern int soc_xgxs_reset(int unit, soc_port_t port);
extern int soc_fusioncore_reset(int unit, soc_port_t port);
#if defined(BCM_GXPORT_SUPPORT)
extern int soc_unicore_reset(int unit, soc_port_t port);
#endif /* BCM_GXPORT_SUPPORT */

/* Cosq init */
#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
int soc_cosq_stack_port_map_disable(int unit);
#endif

#ifdef BCM_XGS3_SWITCH_SUPPORT
int soc_cpu_priority_mapping_init(int unit);
#endif
#endif  /* !_SOC_DRV_H */
