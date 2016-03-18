/*
 * $Id: trunk.h,v 1.20.2.2 Broadcom SDK $
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
 * This file contains TRUNK definitions internal to the BCM library.
 */

#ifndef _BCM_INT_TRUNK_H
#define _BCM_INT_TRUNK_H

#include <bcm/trunk.h>

/* 6-bit field, MSB indicates trunk */
#define BCM_TGID_TRUNK_INDICATOR(unit)     (1 << SOC_TRUNK_BIT_POS(unit))
#define BCM_TGID_PORT_TRUNK_MASK(unit)     ((1 << SOC_TRUNK_BIT_POS(unit)) - 1)
#define BCM_TGID_PORT_TRUNK_MASK_HI(unit)  (0x3 << SOC_TRUNK_BIT_POS(unit))
#define BCM_TGID_PORT_TRUNK_MASK_HI_SHIFT(unit)  (SOC_TRUNK_BIT_POS(unit))

#define BCM_TRUNK_TO_MODIDf(unit, tid) \
 (((tid) & BCM_TGID_PORT_TRUNK_MASK_HI(unit)) >> BCM_TGID_PORT_TRUNK_MASK_HI_SHIFT(unit))
#define BCM_TRUNK_TO_TGIDf(unit, tid) \
 (((tid) & BCM_TGID_PORT_TRUNK_MASK(unit)) | BCM_TGID_TRUNK_INDICATOR(unit))
#define BCM_MODIDf_TGIDf_TO_TRUNK(unit,mod,tgid) \
 ((((mod)<<BCM_TGID_PORT_TRUNK_MASK_HI_SHIFT(unit))&BCM_TGID_PORT_TRUNK_MASK_HI(unit)) | \
 ((tgid) & BCM_TGID_PORT_TRUNK_MASK(unit)))

/*
 * Define:
 *	TRUNK_CHK_TGID32
 * Purpose:
 *	Causes a routine to return BCM_E_PARAM if the specified
 *	trunk group ID is out of range 0-31.
 */

#define TRUNK_CHK_TGID32(unit, tgid) do { \
	if (tgid < 0 || tgid > 31) return BCM_E_PARAM; \
	} while (0);

/*
 * Define:
 *	TRUNK_CHK_TGID128
 * Purpose:
 *	Causes a routine to return BCM_E_PARAM if the specified
 *	trunk group ID is out of range 0-127.
 */

#define TRUNK_CHK_TGID128(unit, tgid) do { \
	if (tgid < 0 || tgid > 127) return BCM_E_PARAM; \
	} while (0);

typedef struct trunk_private_s {
    int		tid;			/* trunk group ID */
    int		in_use;
    int		psc;			/* port spec criterion */
    int		rtag;			/* hardware rtag value */
    int		dlf_index_spec;		/* requested by user */
    int		dlf_index_used;		/* actually used */
    int		dlf_port_used;		/* actually used or -1 */
    int		mc_index_spec;		/* requested by user */
    int		mc_index_used;		/* actually used */
    int		mc_port_used;		/* actually used or -1 */
    int		ipmc_index_spec;	/* requested by user */
    int		ipmc_index_used;	/* actually used */
    int		ipmc_port_used;		/* actually used or -1 */
} trunk_private_t;

#define BCM_SWITCH_TRUNK_MAX_PORTCNT   8       
            /* Max # front panel ports in trunk */
#define BCM_XGS1_FABRIC_TRUNK_MAX_PORTCNT    4       
#define BCM_XGS23_FABRIC_TRUNK_MAX_PORTCNT   8       
            /* Max # front panel ports in trunk */


extern int _bcm_esw_trunk_gport_construct(int unit, bcm_trunk_add_info_t *t_data);
extern int _bcm_esw_trunk_gport_resolve(int unit, bcm_trunk_add_info_t *t_data);

extern int _bcm_nuc_tpbm_get(int unit,
                             int num_ports,
                             bcm_module_t tm[BCM_TRUNK_MAX_PORTCNT],
                             uint32 *nuc_tpbm);

#if defined (BCM_XGS3_SWITCH_SUPPORT)
#define _BCM_XGS3_TRUNK_MOD_PORT_MAP_IDX_COUNT(_unit_)     \
      (SOC_MAX_NUM_PORTS * ((SOC_MODID_MAX(_unit_) > 0) ?  \
                            (1 + SOC_MODID_MAX(_unit_)) : 1))
extern int _bcm_xgs3_trunk_mod_port_map_init(int unit);
extern int _bcm_xgs3_trunk_mod_port_map_deinit(int unit);
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_trunk_reinit(int unit);
extern int _bcm_trunk_cleanup(int unit);
#else
#define _bcm_trunk_reinit(unit)     (BCM_E_NONE)
#define _bcm_trunk_cleanup(u)       (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_trunk_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern int _bcm_xgs3_trunk_get_port_property(int unit, bcm_module_t mod, 
                                             bcm_port_t, int *tid);
extern int _bcm_trunk_id_validate(int unit, bcm_trunk_t tid);
#endif	/* !_BCM_INT_TRUNK_H */
