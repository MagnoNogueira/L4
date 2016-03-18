/*
 * Bsafe uHSM OSL definitions.
 * bsafe OS Independent Layer 
 * any os specific calls should be in bsafe_<os>.c  and defined here as macros
 *
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
 * $Id: bsafe_osl.h,v 1.1 Broadcom SDK $
 */

#ifndef __BSAFE_OSL_H_
#define __BSAFE_OSL_H_

#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <sal/appl/io.h>

typedef uint8 BYTE;		/* 1 byte */
#ifndef __INCvxTypesOldh
typedef uint16 UINT16;		/* 2 bytes */
typedef uint32 UINT32;		/* 4 bytes */
#endif
typedef int32 INT;		/* 4 bytes */


#define OS_DMA_ALLOC(dev,size) 		sal_alloc((size), "bsafe")
#define OS_DMA_FREE(dev,ptr,size) 	sal_free(ptr)
#define OS_ALLOC(size) 			sal_alloc((size), "bsafe")
#define OS_FREE(ptr,size) 	sal_free(ptr)
#define OS_COPY_FROM_USER	sal_memcpy
#define OS_COPY_TO_USER		sal_memcpy
#define OS_MEMCPY		sal_memcpy
#define OS_MEMSET		sal_memset


/* debuging logging capabilities */
#ifdef DEBUGxxx
#define PRINTF(arg)			printk arg
#else
#define PRINTF(arg)
#endif


#define MAX_TIMEOUT	500000
#define DELAY_US	10
#define DELAY		delay


#ifdef DEBUGxxx
#define CMD_PRINTF(type,var)		PRINTF (("%s [",#var);printk(type,var);printk("]\n"));
#define KEY_PRINTF			PRINTF
#define D_PRINTF			PRINTF
#else
#define CMD_PRINTF(type,var)
#define KEY_PRINTF(arg)			
#define D_PRINTF(arg)			
#endif

/* some utils */
#ifdef LE_HOST
#define SWAP2BYTES(sval) ((((sval)&0xff00)>>8)+(((sval)&0x00ff)<<8))
#define SWAP4BYTES(lval) (((SWAP2BYTES((lval)>>16)))+((SWAP2BYTES((lval)&0xffff)<<16)))
#else
#define SWAP2BYTES(sval) sval
#define SWAP4BYTES(lval) lval
#endif


#endif /* __BSAFE_OSL_H_ */
