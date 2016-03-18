/*
 * $Id: triumph.h,v 1.9.2.3 Broadcom SDK $
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
 * File:        triumph.h
 */

#ifndef _SOC_TRIUMPH_H_
#define _SOC_TRIUMPH_H_

#include <soc/drv.h>
#include <shared/sram.h>

typedef _shr_ext_sram_entry_t tr_ext_sram_entry_t;
 
extern int soc_triumph_misc_init(int);
extern int soc_triumph_mmu_init(int);
extern int soc_triumph_age_timer_get(int, int *, int *);
extern int soc_triumph_age_timer_max_get(int, int *);
extern int soc_triumph_age_timer_set(int, int, int);
extern void soc_triumph_mem_config(int unit);
extern void soc_triumph_parity_error(void *unit_vp, void *d1, void *d2,
                                     void *d3, void *d4);
extern int soc_triumph_esm_intr_status(int unit);
extern int soc_triumph_esm_init_set_tcam_freq(int unit, int freq);
extern int soc_triumph_esm_init_set_sram_freq(int unit, int freq);
extern int soc_tr_tcam_access(int unit, int op_type, int num_inst,
                              int num_pre_nop, int num_post_nop,
                              uint32 *dbus, int *ibus);
extern int soc_triumph_ext_sram_enable_set(int unit, soc_reg_t mode,
                                           int enable, int clr);
extern int soc_triumph_ext_sram_op(int unit, soc_reg_t mode,
                                   tr_ext_sram_entry_t *entry);
extern int soc_triumph_l2x_to_ext_l2(int unit, l2x_entry_t *l2x_entry,
                                     ext_l2_entry_entry_t *ext_l2_entry);
extern int soc_triumph_ext_l2_to_l2x(int unit,
                                     ext_l2_entry_entry_t *ext_l2_entry,
                                     l2x_entry_t *l2x_entry);
extern soc_functions_t soc_triumph_drv_funs;

#endif	/* !_SOC_TRIUMPH_H_ */
