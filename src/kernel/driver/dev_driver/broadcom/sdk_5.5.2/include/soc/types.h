/*
 * $Id: types.h,v 1.14.6.1 Broadcom SDK $
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

#ifndef _SOC_TYPES_H
#define _SOC_TYPES_H

#include <sal/types.h>

#include <soc/defs.h>

#include <shared/pbmp.h>
#include <shared/gport.h>

/*
 * Port Bitmap Macro Interface
 * See shared/pbmp.h for more details.
 */

typedef _shr_pbmp_t	soc_pbmp_t;
#define	pbmp_t		soc_pbmp_t

#define	SOC_PBMP_PORT_MAX		_SHR_PBMP_PORT_MAX

/* used for hw registers */
#define	SOC_PBMP_WORD_MAX		_SHR_PBMP_WORD_MAX
#define	SOC_PBMP_WORD_GET(bmp, word)	_SHR_PBMP_WORD_GET((bmp), (word))
#define	SOC_PBMP_WORD_SET(bmp, word, val)	\
	_SHR_PBMP_WORD_SET((bmp), (word), (val))

#define SOC_PBMP_CLEAR(pbm)             _SHR_PBMP_CLEAR(pbm)
#define SOC_PBMP_MEMBER(bmp, port)      _SHR_PBMP_MEMBER((bmp), (port))
#define SOC_PBMP_ITER(bmp, port)        _SHR_PBMP_ITER((bmp), (port))
#define	SOC_PBMP_COUNT(pbm, count)	_SHR_PBMP_COUNT(pbm, count)

#define SOC_PBMP_IS_NULL(pbm)           _SHR_PBMP_IS_NULL(pbm)
#define SOC_PBMP_NOT_NULL(pbm)          _SHR_PBMP_NOT_NULL(pbm)
#define SOC_PBMP_EQ(pbm_a, pbm_b)       _SHR_PBMP_EQ(pbm_a, pbm_b)
#define SOC_PBMP_NEQ(pbm_a, pbm_b)      _SHR_PBMP_NEQ(pbm_a, pbm_b)

/* Assignment operators */
#define SOC_PBMP_ASSIGN(dst, src)       _SHR_PBMP_ASSIGN(dst, src)
#define SOC_PBMP_AND(pbm_a, pbm_b)      _SHR_PBMP_AND(pbm_a, pbm_b)
#define SOC_PBMP_OR(pbm_a, pbm_b)       _SHR_PBMP_OR(pbm_a, pbm_b)
#define SOC_PBMP_XOR(pbm_a, pbm_b)      _SHR_PBMP_XOR(pbm_a, pbm_b)
#define SOC_PBMP_REMOVE(pbm_a, pbm_b)   _SHR_PBMP_REMOVE(pbm_a, pbm_b)
#define SOC_PBMP_NEGATE(pbm_a, pbm_b)   _SHR_PBMP_NEGATE(pbm_a, pbm_b)

/* Port PBMP operators */
#define SOC_PBMP_PORT_SET(pbm, port)    _SHR_PBMP_PORT_SET(pbm, port)
#define SOC_PBMP_PORT_ADD(pbm, port)    _SHR_PBMP_PORT_ADD(pbm, port)
#define SOC_PBMP_PORT_REMOVE(pbm, port) _SHR_PBMP_PORT_REMOVE(pbm, port)
#define SOC_PBMP_PORT_FLIP(pbm, port)	_SHR_PBMP_PORT_FLIP(pbm, port)
#define SOC_PBMP_PORT_VALID(port)	_SHR_PBMP_PORT_VALID(port)

#define	SOC_PBMP_FMT_LEN		_SHR_PBMP_FMT_LEN
#define	SOC_PBMP_FMT(pbm, buf)		_SHR_PBMP_FMT(pbm, buf)

/* backwards compatibility */
#define PBMP_MEMBER(bmp, port)          SOC_PBMP_MEMBER((bmp), (port))
#define PBMP_ITER(bmp, port)            SOC_PBMP_ITER((bmp), (port))

#define KILO(x)                 ((x) * 1024)
#define MEGA(x)                 ((x) * 1024 * 1024)
#define GIGA(x)                 ((x) * 1024 * 1024 * 1024)
#define TERA(x)                 ((x) * 1024 * 1024 * 1024 * 1024)

typedef int     soc_cos_t;
typedef int     soc_port_t;
typedef uint32	soc_block_t;		/* see soc/chip.h */

typedef uint32	soc_page_t;
/*
 * VLAN Types
 */

typedef uint16  vlan_id_t;

#define VLAN_ID_NONE                    ((vlan_id_t) 0x0000)
#define VLAN_ID_DEFAULT                 ((vlan_id_t) 0x0001)
#define VLAN_ID_INVALID                 ((vlan_id_t) 0x0fff)
#define VLAN_ID_VALID(id)               ((id) >= VLAN_ID_DEFAULT && \
                                         (id) < VLAN_ID_INVALID)

#define VLAN_CTRL(prio, cfi, id)        (((prio) & 0x007) << 13 | \
                                         ((cfi ) & 0x001) << 12 | \
                                         ((id  ) & 0xfff) << 0)

#define VLAN_CTRL_PRIO(c)               ((c) >> 13 & 0x007)
#define VLAN_CTRL_CFI(c)                ((c) >> 12 & 0x001)
#define VLAN_CTRL_ID(c)                 ((c) >>  0 & 0xfff)

/*
 * GPORT (generic port) definitions
 * See shared/gport.h for more details.
 */

typedef _shr_gport_t    soc_gport_t;

#define SOC_GPORT_NONE        _SHR_GPORT_NONE
#define SOC_GPORT_INVALID     _SHR_GPORT_INVALID
#define SOC_GPORT_BLACK_HOLE  _SHR_GPORT_BLACK_HOLE
#define SOC_GPORT_LOCAL_CPU   _SHR_GPORT_LOCAL_CPU

#define SOC_GPORT_IS_LOCAL(_gport)       _SHR_GPORT_IS_LOCAL(_gport)
#define SOC_GPORT_IS_MODPORT(_gport)     _SHR_GPORT_IS_MODPORT(_gport)
#define SOC_GPORT_IS_TRUNK(_gport)       _SHR_GPORT_IS_TRUNK(_gport)
#define SOC_GPORT_IS_BLACK_HOLE(_gport)  _SHR_GPORT_IS_BLACK_HOLE(_gport)
#define SOC_GPORT_IS_LOCAL_CPU(_gport)   _SHR_GPORT_IS_LOCAL_CPU(_gport)
#define SOC_GPORT_IS_MPLS_PORT(_gport)   _SHR_GPORT_IS_MPLS_PORT(_gport)
#define SOC_GPORT_IS_SUBPORT_GROUP(_gport) _SHR_GPORT_IS_SUBPORT_GROUP(_gport)
#define SOC_GPORT_IS_SUBPORT_PORT(_gport) _SHR_GPORT_IS_SUBPORT_PORT(_gport)
#define SOC_GPORT_IS_UCAST_QUEUE_GROUP(_gport) _SHR_GPORT_IS_UCAST_QUEUE_GROUP(_gport)
#define SOC_GPORT_IS_SCHEDULER(_gport)   _SHR_GPORT_IS_SCHEDULER(_gport)
#define SOC_GPORT_IS_DEVPORT(_gport)     _SHR_GPORT_IS_DEVPORT(_gport)

#define SOC_GPORT_LOCAL_SET(_gport, _port) _SHR_GPORT_LOCAL_SET(_gport, _port)
#define SOC_GPORT_LOCAL_GET(_gport)        _SHR_GPORT_LOCAL_GET(_gport)

#define SOC_GPORT_LOCAL_CPU_SET(_gport, _port)  \
        _SHR_GPORT_LOCAL_CPU_SET(_gport, _port)

#define SOC_GPORT_MODPORT_SET(_gport, _module, _port)    \
        _SHR_GPORT_MODPORT_SET(_gport, _module, _port)
#define SOC_GPORT_MODPORT_MODID_GET(_gport)    \
        _SHR_GPORT_MODPORT_MODID_GET(_gport)
#define SOC_GPORT_MODPORT_PORT_GET(_gport)    \
        _SHR_GPORT_MODPORT_PORT_GET(_gport)

#define SOC_GPORT_TRUNK_SET(_gport, _trunk_id)    \
        _SHR_GPORT_TRUNK_SET(_gport, _trunk_id)
#define SOC_GPORT_TRUNK_GET(_gport)    \
        _SHR_GPORT_TRUNK_GET(_gport)

#define SOC_GPORT_MPLS_PORT_ID_SET(_gport, _id)    \
        _SHR_GPORT_MPLS_PORT_ID_SET(_gport, _id)
#define SOC_GPORT_MPLS_PORT_ID_GET(_gport)           \
        _SHR_GPORT_MPLS_PORT_ID_GET(_gport))

#define SOC_GPORT_SUBPORT_GROUP_SET(_gport, _id)    \
        _SHR_GPORT_SUBPORT_GROUP_SET(_gport, _id)
#define SOC_GPORT_SUBPORT_GROUP_GET(_gport)           \
        _SHR_GPORT_SUBPORT_GROUP_GET(_gport))

#define SOC_GPORT_SUBPORT_PORT_SET(_gport, _id)    \
        _SHR_GPORT_SUBPORT_PORT_SET(_gport, _id)
#define SOC_GPORT_SUBPORT_PORT_GET(_gport)           \
        _SHR_GPORT_SUBPORT_PORT_GET(_gport))

#define SOC_GPORT_UCAST_QUEUE_GROUP_SET(_gport, _id)    \
        _SHR_GPORT_UCAST_QUEUE_GROUP_SET(_gport, _id)
#define SOC_GPORT_UCAST_QUEUE_GROUP_GET(_gport)           \
        _SHR_GPORT_UCAST_QUEUE_GROUP_GET(_gport))

#define SOC_GPORT_SCHEDULER_SET(_gport, _id)    \
        _SHR_GPORT_SCHEDULER_SET(_gport, _id)
#define SOC_GPORT_SCHEDULER_GET(_gport)           \
        _SHR_GPORT_SCHEDULER_GET(_gport))

#define SOC_GPORT_DEVPORT_SET(_gport, _device, _port)    \
        _SHR_GPORT_DEVPORT_SET(_gport, _device, _port)
#define SOC_GPORT_DEVPORT_DEVID_GET(_gport)    \
        _SHR_GPORT_DEVPORT_DEVID_GET(_gport)
#define SOC_GPORT_DEVPORT_PORT_GET(_gport)    \
        _SHR_GPORT_DEVPORT_PORT_GET(_gport)

#endif  /* !_SOC_TYPES_H */
