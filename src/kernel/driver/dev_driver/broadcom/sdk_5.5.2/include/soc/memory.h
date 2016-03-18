/*
 * $Id: memory.h,v 1.18.6.2 Broadcom SDK $
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
 * File:        memory.h
 * Purpose:     Base definitions for memory types
 * Requires:    
 */

#ifndef _SOC_MEMORY_H
#define _SOC_MEMORY_H

#include <soc/defs.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/robo/allenum.h>
#include <soc/field.h>
#include <soc/macipadr.h>

#define SOC_MEM_FLAG_READONLY	0x00000001 /* True if table is read-only */
#define SOC_MEM_FLAG_VALID	0x00000002 /* Some tables don't really exist */
#define SOC_MEM_FLAG_DEBUG	0x00000004 /* Access requires CMIC DebugMode */
#define SOC_MEM_FLAG_SORTED	0x00000008 /* Table is kept in sorted order */
#define SOC_MEM_FLAG_CBP	0x00000010 /* Table is part of CBP */
#define SOC_MEM_FLAG_CACHABLE	0x00000020 /* Reasonable to cache in S/W */
#define SOC_MEM_FLAG_BISTCBP	0x00000040 /* CBP has BIST for this memory */
#define SOC_MEM_FLAG_BISTEPIC	0x00000080 /* EPIC has BIST for this memory */
#define SOC_MEM_FLAG_BISTBIT	0x00003f00 /* Bit pos for BIST reg (0-63) */
#define SOC_MEM_FLAG_BISTBSHFT	8	   /* Shift corresponding to BISTBIT */
#define SOC_MEM_FLAG_BISTFFP	0x00004000 /* Use FFPBIST for this memory */
#define SOC_MEM_FLAG_UNIFIED	0x00010000 /* Only one copy of table */
#define SOC_MEM_FLAG_HASHED	0x00020000 /* Hashed table */
#define SOC_MEM_FLAG_WORDADR	0x00040000 /* Requires special addressing */
#define SOC_MEM_FLAG_CAM        0x00080000 /* Memory is a CAM */
#define SOC_MEM_FLAG_AGGR       0x00100000 /* Memory is a AGGREGATE */
#define SOC_MEM_FLAG_CMD        0x00200000 /* Memory allows CMDMEM access */
#define SOC_MEM_FLAG_MONOLITH	0x04000000 /* no block in addressing */
#define SOC_MEM_FLAG_BE		0x08000000 /* Big endian */
#define SOC_MEM_FLAG_MULTIVIEW  0x10000000 /* Multiview memories */

#define SOC_MEM_FLAG_GMEMORY    0x10000000 /* Generic Memory */
#define SOC_MEM_FLAG_VMEMORY    0x20000000 /* VLAN Memory */

/*
 * Declarations for null table entries.
 *
 * A null table entry is the data normally written to each entry when a
 * memory is cleared.  This is generally all zeroes, except for sorted
 * tables fields which make up the search key are generally all f's.
 */

extern uint32		_soc_mem_entry_null_zeroes[SOC_MAX_MEM_WORDS];
extern uint32		_soc_mem_entry_null_word0[SOC_MAX_MEM_WORDS];
#if defined(BCM_ROBO_SUPPORT)
extern uint32		_soc_robo_mem_entry_null_zeroes[SOC_ROBO_MAX_MEM_WORDS];
#endif /* BCM_ROBO_SUPPORT */
typedef int (*soc_mem_cmp_t)(int, void *, void *);

/* New unified soc memories structure */
typedef struct soc_mem_info_s {
    uint32		flags;		/* Logical OR of SOC_MEM_FLAG_xxx */
    soc_mem_cmp_t	cmp_fn;		/* Function to compare two entries */
    void		*null_entry;	/* Value when table entry is empty */
    int			index_min;	/* Minimum table index */
    int			index_max;	/* Maximum table index */
    uint16		minblock;	/* first blocks entry */
    uint16		maxblock;	/* last blocks entry */
    uint32		blocks;		/* bitmask of valid blocks */
    uint32		base;		/* Includes region offset */
    uint32              gran;           /* Phys addr granularity by index */
    uint16		bytes;
    uint16		nFields;
    soc_field_info_t	*fields;
    char                **views;
} soc_mem_info_t;

#define SOC_MEM_IP6_FULL_ADDR       0   /* IPv6 address all 128 bits  */
#define SOC_MEM_IP6_UPPER_ONLY      1   /* IPv6 address upper 64 bits */
#define SOC_MEM_IP6_LOWER_ONLY      2   /* IPv6 address lower 64 bits */

#define SOC_MEM_MAC_FULL_ADDR       0   /* Mac address all 48 bits  */
#define SOC_MEM_MAC_UPPER_ONLY      1   /* Mac address upper 24 bits */
#define SOC_MEM_MAC_LOWER_ONLY      2   /* Mac address lower 24 bits */

uint32 soc_mem_addr(int unit, soc_mem_t mem, int block, int index);

uint32 *soc_mem_field_get(int unit, soc_mem_t mem, const uint32 *entbuf,
                          soc_field_t field, uint32 *fldbuf);
void soc_mem_field_set(int unit, soc_mem_t mem, uint32 *entbuf,
                       soc_field_t field, uint32 *fldbuf);
void soc_mem_field_width_fit_set(int unit, soc_mem_t mem, uint32 *entbuf,
                                 soc_field_t field, uint32 *fldbuf);

extern int soc_mem_field_pbmp_fit(int unit, soc_mem_t mem, 
                                  soc_field_t field, uint32 *value);
uint32 soc_mem_field32_get(int unit, soc_mem_t mem, const void *entbuf,
                           soc_field_t field);
void soc_mem_field32_set(int unit, soc_mem_t mem, void *entbuf,
                         soc_field_t field, uint32 value);
void soc_mem_field32_force(int unit, soc_mem_t mem, void *entbuf,
                           soc_field_t field, uint32 value);
uint32 *soc_mem_mask_field_get(int unit, soc_mem_t mem, const uint32 *entbuf,
                               soc_field_t field, uint32 *fldbuf);
void soc_mem_mask_field_set(int unit, soc_mem_t mem, uint32 *entbuf,
                            soc_field_t field, uint32 *fldbuf);
uint32 soc_mem_mask_field32_get(int unit, soc_mem_t mem, const void *entbuf,
                                soc_field_t field);
void soc_mem_mask_field32_set(int unit, soc_mem_t mem, void *entbuf,
                              soc_field_t field, uint32 value);
void soc_mem_mac_addr_set(int unit, soc_mem_t mem, void *entry,
                          soc_field_t field, const sal_mac_addr_t mac);
void soc_mem_mac_addr_get(int unit, soc_mem_t mem, const void *entry,
                          soc_field_t field, sal_mac_addr_t mac);
void soc_mem_ip6_addr_set(int unit, soc_mem_t mem, void *entry, 
                          soc_field_t field, const ip6_addr_t ip6, int flags);
void soc_mem_ip6_addr_get(int unit, soc_mem_t mem, const void *entry, 
                          soc_field_t field, ip6_addr_t ip6, int flags);
void soc_mem_ip6_addr_mask_set(int unit, soc_mem_t mem, void *entry, 
                               soc_field_t field, const ip6_addr_t ip6,
                               int flags);
void soc_mem_ip6_addr_mask_get(int unit, soc_mem_t mem, const void *entry, 
                               soc_field_t field, ip6_addr_t ip6, int flags);
uint32 *soc_mem_datamask_get(int unit, soc_mem_t mem, uint32 *buf);
uint32 *soc_mem_tcammask_get(int unit, soc_mem_t mem, uint32 *buf);
uint32 *soc_mem_eccmask_get(int unit, soc_mem_t mem, uint32 *buf);

/* These functions do not require a unit number to manipulate mem vars */
uint32 *soc_meminfo_field_get(soc_mem_info_t *meminfo, const uint32 *entbuf,
                              soc_field_t field, uint32 *fldbuf);
void soc_meminfo_mac_addr_set(soc_mem_info_t *meminfo, void *entry,
                              soc_field_t field, const sal_mac_addr_t mac);
void soc_meminfo_field_set(soc_mem_info_t *meminfo, uint32 *entbuf,
                           soc_field_t field, uint32 *fldbuf);
void soc_meminfo_field32_force(soc_mem_info_t *meminfo, void *entry,
                               soc_field_t field, uint32 value);
void soc_mem_mac_address_set(int unit, soc_mem_t mem, void *entry,
                        soc_field_t field, const sal_mac_addr_t mac, int flags);
void soc_mem_mac_address_get(int unit, soc_mem_t mem, const void *entry,
                        soc_field_t field, sal_mac_addr_t mac, int flags);

#endif	/* !_SOC_MEMORY_H */
