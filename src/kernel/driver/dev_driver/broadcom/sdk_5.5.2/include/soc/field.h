/*
 * $Id: field.h,v 1.9 Broadcom SDK $
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
 * File:        field.h
 * Purpose:     Register/memory field descriptions
 */

#ifndef _SOC_FIELD_H
#define _SOC_FIELD_H

#include <soc/types.h>
#include <soc/mcm/allenum.h>
#include <soc/schanmsg.h>
#include <soc/types.h>
#include <soc/mcm/robo/allenum.h>

/* Values for flags */

#define SOCF_LE		0x01	/* little endian */
#define SOCF_RO		0x02	/* read only */
#define SOCF_WO		0x04	/* write only */

#define SOCF_LSB	0x05	/* Used for define the MAC Address format */
#define SOCF_MSB	0x06	/* Used for define the MAC Address format */

#define SOCF_SC		0x08       
#define SOCF_RES        0x10    /* reserved (do not test, etc.) */

typedef struct soc_field_info_s {
	soc_field_t	field;
	uint16		len;	/* Bits in field */
	uint16		bp;	/* Least bit position of the field */
	unsigned char	flags;	/* Logical OR of SOCF_* */
} soc_field_info_t;

/*
 * Find field _fsrch in the field list _flist (with _fnum entries)
 * Sets _infop to the field_info of _fsrch or NULL if not found
 */
#define	SOC_FIND_FIELD(_fsrch, _flist, _fnum, _infop) do { \
		soc_field_info_t *__f, *__e; \
		__f = _flist; \
		__e = &__f[_fnum]; \
		_infop = NULL; \
		while (__f < __e) { \
			if (__f->field == _fsrch) { \
				_infop = __f; \
				break; \
			} \
			__f++; \
		} \
	} while (0)

EXTERN int soc_mem_field_length(int unit, soc_mem_t mem, soc_field_t field);

#define SOC_MEM_FIELD32_VALUE_MAX(_unit, _mem, _field)         \
    (((uint32)(0xFFFFFFFF)) >>                                 \
     (32 - soc_mem_field_length((_unit), (_mem), (_field))))      

#define SOC_MEM_FIELD32_VALUE_FIT(_unit, _mem, _field, _value) \
    (((uint32)(_value)) <= SOC_MEM_FIELD32_VALUE_MAX((_unit), (_mem), (_field)))

#if !defined(SOC_NO_NAMES)
EXTERN char *soc_fieldnames[];
EXTERN char *soc_robo_fieldnames[];
#define SOC_FIELD_NAME(unit, field)		soc_fieldnames[field]
#else
#define SOC_FIELD_NAME(unit, field)		""
#endif

#endif	/* !_SOC_FIELD_H */
