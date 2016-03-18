/*
 * $Id: scorpion.h,v 1.9 Broadcom SDK $
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
 * File:        scorpion.h
 * Purpose:     Function declarations for Scorpion bcm functions
 */

#ifndef _BCM_INT_SCORPION_H_
#define _BCM_INT_SCORPION_H_
#if defined(BCM_SCORPION_SUPPORT)
#include <bcm_int/esw/mbcm.h>

/****************************************************************
 *
 * Scorpion functions
 *
 ****************************************************************/
#if defined(INCLUDE_L3)
extern int _bcm_sc_defip_init(int unit);
extern int _bcm_sc_defip_deinit(int unit);
#endif /* INCLUDE_L3 */

#if defined(BCM_FIELD_SUPPORT)
#define _BCM_FIELD_SC_INGRESS_SINGLE_WIDE_SLICE_NUM (8)
#define _BCM_FIELD_SC_INGRESS_SINGLE_WIDE_SLICE_SZ  (128)
#define _BCM_FIELD_SC_INGRESS_DOUBLE_WIDE_SLICE_NUM (4)
#define _BCM_FIELD_SC_INGRESS_DOUBLE_WIDE_SLICE_SZ  (256)
#define _BCM_FIELD_SC_INGRESS_DOUBLE_WIDE_SLICE0 (8) 
#define _BCM_FIELD_SC_INGRESS_DOUBLE_WIDE_SLICE1 (9) 
#define _BCM_FIELD_SC_INGRESS_DOUBLE_WIDE_SLICE2 (10) 
#define _BCM_FIELD_SC_INGRESS_DOUBLE_WIDE_SLICE3 (11) 


extern int _bcm_sc_field_init(int unit, _field_control_t *fc);

#endif /* BCM_FIELD_SUPPORT */


extern int _bcm_sc_port_control_bridge_set(int unit, bcm_port_t port, 
                                           int value);

extern int _bcm_sc_field_entry_slice_idx_change(int unit, 
                                                _field_entry_t *f_ent,
                                                int amount);
#endif  /* BCM_SCORPION_SUPPORT */

extern int bcm_sc_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 kbits_sec_min,
                                          uint32 kbits_sec_max,
                                          uint32 flags);
extern int bcm_sc_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 *kbits_sec_min,
                                          uint32 *kbits_sec_max,
                                          uint32 *flags);

#endif  /* !_BCM_INT_SCORPION_H_ */
