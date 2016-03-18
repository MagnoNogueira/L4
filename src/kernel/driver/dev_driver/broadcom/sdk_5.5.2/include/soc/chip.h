/*
 * $Id: chip.h,v 1.18 Broadcom SDK $
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
 * File:        chip.h
 * Purpose:     Defines for chip types, etc.
 * Requires:    soctypes.h, memory, register and feature defs.
 *
 * System on Chip (SOC) basic structures and typedefs
 * Each chip consists of a number of blocks.  Each block can be
 * a port interface controller (PIC) that contains ports.
 * The mcm/bcm*.i files contain definitions of each block and port
 * in the associated chip.  The structures used in those files
 * are defined here.  They are used to build internal use data
 * structures defined in soc/drv.h
 */

#ifndef _SOC_CHIP_H
#define _SOC_CHIP_H

#include <soc/types.h>
#include <soc/memory.h>
#include <soc/register.h>
#include <soc/feature.h>

/*
 * Arrays of soc_block_info_t are built by the registers program.
 * Each array is indexed by block number (per chip).
 * The last entry has type -1.
 * Used in bcm*.i files.
 */
typedef struct {
    int                 type;		/* SOC_BLK_* */
    int			number;		/* instance of type */
    int			schan;		/* pic number for schan commands */
    int			cmic;		/* pic number for cmic r/w commands */
} soc_block_info_t;

/*
 * Arrays of soc_port_info_t are built by the registers program.
 * Each array is indexed by port number (per chip).
 * Unused ports have blk -1 and bindex 0
 * The last entry has blk -1 and bindex -1
 * Used in bcm*.i files.
 */
typedef struct {
    int			blk;		/* index into soc_block_info array */
    int			bindex;		/* index of port within block */
} soc_port_info_t;

/*
 * Block types
 */
#define SOC_BLK_NONE      0x00000000  /* no port (used for empty port numbers) */
#define SOC_BLK_EPIC      0x00000001  /* 10/100M ethernet ports */
#define SOC_BLK_GPIC      0x00000002  /* 1G ethernet ports */
#define SOC_BLK_HPIC      0x00000004  /* 10G higig ports (herc style) */
#define SOC_BLK_IPIC      0x00000008  /* 10G higig ports (draco/lynx style) */
#define SOC_BLK_XPIC      0x00000010  /* 10G ethernet ports */
#define SOC_BLK_CMIC      0x00000020  /* CPU (XGS CMIC or SBX NIC) */
#define SOC_BLK_CPIC      0x00000040  /* CPU (herc style) */
#define SOC_BLK_ARL       0x00000080  /* Address Resolution Logic */
#define SOC_BLK_MMU       0x00000100  /* Memory Management Unit */
#define SOC_BLK_MCU       0x00000200  /* Memory Control Unit */
#define SOC_BLK_GPORT     0x00000400  /* 1G Ethernet ports (FB/ER) */
#define SOC_BLK_XPORT     0x00000800  /* 10G Ethernet/higig port(FB/ER)*/
#define SOC_BLK_IPIPE     0x00001000  /* Ingress Pipeline (firebolt) */
#define SOC_BLK_IPIPE_HI  0x00002000  /* Ingress Pipeline (Higig only) (FB) */
#define SOC_BLK_EPIPE     0x00004000  /* Egress Pipeline (FB) */
#define SOC_BLK_EPIPE_HI  0x00008000  /* Egress Pipeline (Higig only) (FB) */
#define SOC_BLK_IGR       0x00010000  /* Ingress (ER) */
#define SOC_BLK_EGR       0x00020000  /* Egress (ER) */
#define SOC_BLK_BSE       0x00040000  /* Binary Search Engine (ER) */
#define SOC_BLK_CSE       0x00080000  /* CAM Search Engine (ER) */
#define SOC_BLK_HSE       0x00100000  /* Hash Search Engine (ER) */
#define SOC_BLK_BSAFE     0x00200000  /* Broadsafe */
#define SOC_BLK_GXPORT    0x00400000  /* 10/2.5/1G port */
#define SOC_BLK_SPI       0x00800000  /* SPI port for ROBO or SBX */
#define SOC_BLK_EXP       0x01000000  /* Expansion port for ROBO */
#define SOC_BLK_SYS       0x02000000  /* SYS for ROBO */
#define SOC_BLK_XGPORT    0x04000000  /* 10/100/1G/2.5/10G port (TR) */
#define SOC_BLK_SPORT     0x08000000  /* 1G SGMII port (TR) */
#define SOC_BLK_INTER     0x10000000  /* Internal MII for ROBO*/
#define SOC_BLK_EXTER     0x20000000  /* External MII for ROBO*/
#define SOC_BLK_ESM       0x40000000  /* External Search Engine (TR) */

#define SOC_BLK_OTPC      SOC_BLK_BSAFE /* OTP Controller (to be removed before release) */
#define SOC_BLK_QGPORT    SOC_BLK_GPORT /* 10/100/1G/2.5 port (SC) Alias */
#define SOC_BLK_XQPORT    SOC_BLK_XGPORT

#define SOC_BLK_PORT    (SOC_BLK_EPIC | \
                         SOC_BLK_GPIC | \
                         SOC_BLK_HPIC | \
                         SOC_BLK_IPIC | \
                         SOC_BLK_XPIC | \
                         SOC_BLK_GPORT | \
                         SOC_BLK_XPORT | \
                         SOC_BLK_GXPORT | \
                         SOC_BLK_XGPORT | \
                         SOC_BLK_QGPORT | \
                         SOC_BLK_SPORT | \
                         SOC_BLK_XQPORT | \
                         SOC_BLK_CPIC)
#define SOC_BLK_CPU     (SOC_BLK_CMIC | \
                         SOC_BLK_CPIC)
#define SOC_BLK_ETHER   (SOC_BLK_EPIC | \
                         SOC_BLK_GPIC | \
                         SOC_BLK_GPORT | \
                         SOC_BLK_XPORT | \
                         SOC_BLK_GXPORT | \
                         SOC_BLK_XGPORT | \
                         SOC_BLK_QGPORT | \
                         SOC_BLK_SPORT | \
                         SOC_BLK_XPIC)
#define SOC_BLK_HIGIG   (SOC_BLK_HPIC | \
                         SOC_BLK_XPORT | \
                         SOC_BLK_GXPORT | \
                         SOC_BLK_XGPORT | \
                         SOC_BLK_QGPORT | \
                         SOC_BLK_IPIC)
#define SOC_BLK_NET     (SOC_BLK_ETHER | \
                         SOC_BLK_HIGIG | \
                         SOC_BLK_SPI)
#define SOC_BLK_HGPORT  (SOC_BLK_IPIPE_HI)

/*
 * Naming of blocks (there are two such arrays, one for
 * block based naming and one for port based naming)
 * Last entry has blk of SOC_BLK_NONE.
 */
typedef struct {
    soc_block_t     blk;        /* block bits to match */
    int             isalias;    /* this name is an alias */
    char            *name;      /* printable name */
} soc_block_name_t;

/* used to intialize soc_block_name_t soc_block_port_names[] */
#define SOC_BLOCK_PORT_NAMES_INITIALIZER    {  \
            /*    blk  , isalias, name */      \
            { SOC_BLK_EPIC,     0,  "fe"    }, \
            { SOC_BLK_GPIC,     0,  "ge"    }, \
            { SOC_BLK_GPORT,    0,  "ge"    }, \
            { SOC_BLK_GXPORT,   0,  "hg"    }, \
            { SOC_BLK_XGPORT,   0,  "ge"    }, \
            { SOC_BLK_QGPORT,   0,  "ge"    }, \
            { SOC_BLK_XQPORT,   0,  "ge"    }, \
            { SOC_BLK_SPORT,    0,  "ge"    }, \
            { SOC_BLK_HPIC,     0,  "hg"    }, \
            { SOC_BLK_IPIC,     0,  "hg"    }, \
            { SOC_BLK_XPIC,     0,  "xe"    }, \
            { SOC_BLK_XPORT,    0,  "hg"    }, \
            { SOC_BLK_CMIC,     0,  "cpu"   }, \
            { SOC_BLK_CPIC,     0,  "cpu"   }, \
            { SOC_BLK_SPI,      0,  "spi"   }, \
            { SOC_BLK_EXP,      0,  "exp"   }, \
            { SOC_BLK_CPU,      0,  "cpu"   }, \
            { SOC_BLK_NONE,     0,  ""  } }

/* used to intialize soc_block_name_t soc_block_names[] */
#define SOC_BLOCK_NAMES_INITIALIZER {  \
    /*    blk  , isalias, name */      \
    { SOC_BLK_EPIC,     0,  "epic"  }, \
    { SOC_BLK_GPIC,     0,  "gpic"  }, \
    { SOC_BLK_HPIC,     0,  "hpic"  }, \
    { SOC_BLK_IPIC,     0,  "ipic"  }, \
    { SOC_BLK_XPIC,     0,  "xpic"  }, \
    { SOC_BLK_CMIC,     0,  "cmic"  }, \
    { SOC_BLK_CPIC,     0,  "cpic"  }, \
    { SOC_BLK_ARL,      0,  "arl"   }, \
    { SOC_BLK_MMU,      0,  "mmu"   }, \
    { SOC_BLK_MCU,      0,  "mcu"   }, \
    { SOC_BLK_GPORT,    0,  "gport" }, \
    { SOC_BLK_XPORT,    0,  "xport" }, \
    { SOC_BLK_GXPORT,   0,  "gxport" }, \
    { SOC_BLK_XGPORT,   0,  "xgport" }, \
    { SOC_BLK_QGPORT,   0,  "qgport" }, \
    { SOC_BLK_XQPORT,   0,  "xqport" }, \
    { SOC_BLK_SPORT,    0,  "sport" }, \
    { SOC_BLK_IPIPE,    0,  "ipipe" }, \
    { SOC_BLK_IPIPE_HI, 0,  "ipipe_hi" }, \
    { SOC_BLK_EPIPE,    0,  "epipe" }, \
    { SOC_BLK_EPIPE_HI, 0,  "epipe_hi" }, \
    { SOC_BLK_IGR,      0,  "igr"  }, \
    { SOC_BLK_EGR,      0,  "egr"  }, \
    { SOC_BLK_BSE,      0,  "bse"  }, \
    { SOC_BLK_CSE,      0,  "cse"  }, \
    { SOC_BLK_HSE,      0,  "hse"  }, \
    { SOC_BLK_BSAFE,    0,  "bsafe" }, \
    { SOC_BLK_ESM,      0,  "esm"}, \
    { SOC_BLK_EPIC,     1,  "e"     }, \
    { SOC_BLK_GPIC,     1,  "g"     }, \
    { SOC_BLK_HPIC,     1,  "h"     }, \
    { SOC_BLK_IPIC,     1,  "i"     }, \
    { SOC_BLK_XPIC,     1,  "x"     }, \
    { SOC_BLK_CMIC,     1,  "cpu"   }, \
    { SOC_BLK_OTPC,     1,  "otpc"  }, \
    { SOC_BLK_SPI,      0,  "spi"   }, \
    { SOC_BLK_CPU,      0,  "cpu"   }, \
    { SOC_BLK_NONE,     0,  ""      } }

/*
 * soc_feature_fun_t: boolean function indicating if feature is supported
 * Used in bcm*.i files.
 */
typedef int (*soc_feature_fun_t) (int unit, soc_feature_t feature);

/*
 * soc_init_chip_fun_t: chip initialization function
 * Used in bcm*.i files.
 */
typedef void (*soc_init_chip_fun_t) (void);

/* Use macros to access */
extern soc_chip_types soc_chip_type_map[SOC_CHIP_TYPES_COUNT];
extern char *soc_chip_type_names[SOC_CHIP_TYPES_COUNT];
extern char *soc_chip_group_names[SOC_CHIP_GROUPS_COUNT];

#define SOC_CHIP_NAME(type)	(soc_chip_type_names[(type)])

#endif	/* !_SOC_CHIP_H */
