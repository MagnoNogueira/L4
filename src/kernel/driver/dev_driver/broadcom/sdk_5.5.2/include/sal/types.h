/*
 * $Id: types.h,v 1.9 Broadcom SDK $
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
 * File: 	types.h
 * Purpose: 	SAL type definitions
 */

#ifndef _SAL_TYPES_H
#define _SAL_TYPES_H

#include <sal/compiler.h>

/*
 * Define platform-independent types
 */

#ifndef TRUE
#define TRUE			1
#endif

#ifndef FALSE
#define FALSE			0
#endif

#ifndef NULL
#define NULL			0
#endif

#ifndef DONT_CARE
#define DONT_CARE		0
#endif

#define VOL			volatile

/*
 * Define unsigned and signed integers with guaranteed sizes.
 * Adjust if your compiler uses different sizes for short or int.
 */

typedef unsigned char		uint8;		/* 8-bit quantity  */
typedef unsigned short		uint16;		/* 16-bit quantity */
typedef unsigned int		uint32;		/* 32-bit quantity */
typedef COMPILER_UINT64		uint64;		/* 64-bit quantity */

typedef signed char		int8;		/* 8-bit quantity  */
typedef signed short		int16;		/* 16-bit quantity */
typedef signed int		int32;		/* 32-bit quantity */
typedef COMPILER_INT64          int64;          /* 64-bit quantity */

#define BITS2BYTES(x)		(((x) + 7) / 8)
#define BITS2WORDS(x)		(((x) + 31) / 32)

#define BYTES2BITS(x)		((x) * 8)
#define BYTES2WORDS(x)		(((x) + 3) / 4)

#define WORDS2BITS(x)		((x) * 32)
#define WORDS2BYTES(x)		((x) * 4)

#define COUNTOF(ary)		((int) (sizeof (ary) / sizeof ((ary)[0])))

typedef uint32	sal_paddr_t;		/* Physical address (PCI address) */
#ifdef PTRS_ARE_64BITS
typedef uint64	sal_vaddr_t;		/* Virtual address (Host address) */
#define PTR_TO_INT(x)		((uint32)(((sal_vaddr_t)(x))&0xFFFFFFFF))
#define INT_TO_PTR(x)		((void *)(sal_vaddr_t)(x))
#else
typedef uint32	sal_vaddr_t;		/* Virtual address (Host address) */
#define PTR_TO_INT(x)		((sal_vaddr_t)(x))
#define INT_TO_PTR(x)		((void *)(uint32)(x))
#endif

typedef union
{
	uint8			u8;
	uint16			u16;
	uint32			u32;
	uint64			u64;
	sal_paddr_t		paddr;
	sal_vaddr_t		vaddr;
	void			*ptr;
}				any_t;

typedef uint8   sal_mac_addr_t[6];      /* MAC address */
typedef uint32  sal_ip_addr_t;          /* IP Address */

/* mac_addr_t mac;  Just generate a list of the macs for display */
#define SAL_MAC_ADDR_LIST(mac) \
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

/* mac_addr_t m1, m2;  True if equal */
#define SAL_MAC_ADDR_EQUAL(m1, m2) (!sal_memcmp(m1, m2, sizeof(mac_addr_t)))

/* mac_addr_t m1, m2;  like memcmp, returns -1, 0, or 1 */
#define SAL_MAC_ADDR_CMP(m1, m2) sal_memcmp(m1, m2, sizeof(mac_addr_t))

#define SAL_MACADDR_STR_LEN     18              /* Formatted MAC address */
#define SAL_IPADDR_STR_LEN      16              /* Formatted IP address */


/* Adjust justification for uint32 writes to fields */
/* dst is an array name of type uint32 [] */
#define SAL_MAC_ADDR_TO_UINT32(mac, dst) do {\
        dst[0] = (((uint32)mac[2]) << 24 | \
                  ((uint32)mac[3]) << 16 | \
                  ((uint32)mac[4]) << 8 | \
                  ((uint32)mac[5])); \
        dst[1] = (((uint32)mac[0]) << 8 | \
                  ((uint32)mac[1])); \
    } while (0)

/* Adjust justification for uint32 writes to fields */
/* src is an array name of type uint32 [] */
#define SAL_MAC_ADDR_FROM_UINT32(mac, src) do {\
        mac[0] = (uint8) (src[1] >> 8 & 0xff); \
        mac[1] = (uint8) (src[1] & 0xff); \
        mac[2] = (uint8) (src[0] >> 24); \
        mac[3] = (uint8) (src[0] >> 16 & 0xff); \
        mac[4] = (uint8) (src[0] >> 8 & 0xff); \
        mac[5] = (uint8) (src[0] & 0xff); \
    } while (0)


/* dst is a uint64 */
#define SAL_MAC_ADDR_TO_UINT64(mac, dst) do {        \
        uint32 _val[2];                              \
        SAL_MAC_ADDR_TO_UINT32(mac, _val);           \
        COMPILER_64_SET(dst, _val[1], _val[0]);      \
    } while (0)

/* src is a uint64 */
#define SAL_MAC_ADDR_FROM_UINT64(mac, src) do {      \
        uint32 _val[2];                              \
        COMPILER_64_TO_32_LO(_val[0], src);          \
        COMPILER_64_TO_32_HI(_val[1], src);          \
        SAL_MAC_ADDR_FROM_UINT32(mac, _val);         \
    } while (0)


/* Adjust IP6 justification for uint32 field accesses */
/* 
 * These macros are used on IP6 "half addresses", being
 * either the "upper" 64 bits or the "lower" 64 bits of
 * an IPv6 address.
 */

/* dst is an array name of type uint32 [] */
#define SAL_IP6_ADDR_HALF_TO_UINT32(ip6, dst) do {\
        (dst)[1] = (((uint32)(ip6)[0]) << 24 | \
                   ((uint32)(ip6)[1]) << 16 | \
                   ((uint32)(ip6)[2]) << 8 | \
                   ((uint32)(ip6)[3])); \
        (dst)[0] = (((uint32)(ip6)[4]) << 24 | \
                   ((uint32)(ip6)[5]) << 16 | \
                   ((uint32)(ip6)[6]) << 8 | \
                   ((uint32)(ip6)[7])); \
    } while (0)

/* src is an array name of type uint32 [] */
#define SAL_IP6_ADDR_HALF_FROM_UINT32(ip6, src) do {\
        (ip6)[0] = (uint8) ((src)[1] >> 24); \
        (ip6)[1] = (uint8) ((src)[1] >> 16 & 0xff); \
        (ip6)[2] = (uint8) ((src)[1] >> 8 & 0xff); \
        (ip6)[3] = (uint8) ((src)[1] & 0xff); \
        (ip6)[4] = (uint8) ((src)[0] >> 24); \
        (ip6)[5] = (uint8) ((src)[0] >> 16 & 0xff); \
        (ip6)[6] = (uint8) ((src)[0] >> 8 & 0xff); \
        (ip6)[7] = (uint8) ((src)[0] & 0xff); \
    } while (0)

/* 
 * These macros are used on full 128-bit IP6 addresses.
 */

/* dst is an array name of type uint32 [] */
#define SAL_IP6_ADDR_TO_UINT32(ip6, dst) do {\
        SAL_IP6_ADDR_HALF_TO_UINT32(&((ip6)[8]), (dst)); \
        SAL_IP6_ADDR_HALF_TO_UINT32((ip6), &((dst)[2])); \
    } while (0)

/* src is an array name of type uint32 [] */
#define SAL_IP6_ADDR_FROM_UINT32(ip6, src) do {\
        SAL_IP6_ADDR_HALF_FROM_UINT32(&((ip6)[8]), (src)); \
        SAL_IP6_ADDR_HALF_FROM_UINT32((ip6), &((src)[2])); \
    } while (0)


/* For RoboSwitch */
#define SAL_PCI_DEV_TYPE          0x01   /* Type of PCI device */
#define SAL_SPI_DEV_TYPE          0x02   /* Type of SPI device accessed via
					    SMP */
#define SAL_EB_DEV_TYPE           0x04   /* Type of EB device */
#define SAL_ICS_DEV_TYPE          0x08   /* Type of ICS device */

#define SAL_DEV_BUS_TYPE_MASK     0x0f

#define SAL_SWITCH_DEV_TYPE       0x10   /* Type of switch device */
#define SAL_ETHER_DEV_TYPE        0x20   /* Type of outband ethernet device */

#define SAL_DEV_BUS_RD_16BIT      0x100  /* 16 bit reads on bus */
#define SAL_DEV_BUS_WR_16BIT      0x200  /* 16 bit writes on bus */

#endif	/* !_SAL_TYPES_H */
