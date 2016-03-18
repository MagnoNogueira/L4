/*
 * $Id: filter.h,v 1.7 Broadcom SDK $
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
 * File:    bcmx/filter.h
 * Purpose: BCMX Filter Processor APIs
 */

#ifndef _BCMX_FILTER_H
#define _BCMX_FILTER_H

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/filter.h>

extern int bcmx_filter_init(void);
extern int bcmx_filter_create(bcm_filterid_t * f_return);
extern int bcmx_filter_create_id(bcm_filterid_t f);
extern int bcmx_filter_destroy(bcm_filterid_t f);
extern int bcmx_filter_copy(bcm_filterid_t f_src, bcm_filterid_t * f_return);
extern int bcmx_filter_qualify_priority(bcm_filterid_t f, int prio);
extern int bcmx_filter_qualify_ingress(bcm_filterid_t f, bcmx_lplist_t lplist);
extern int bcmx_filter_qualify_egress(bcm_filterid_t f, bcmx_lplist_t lplist);
extern int bcmx_filter_qualify_egress_modid(bcm_filterid_t f, int module_id);
extern int bcmx_filter_qualify_unknown_ucast(bcm_filterid_t f);
extern int bcmx_filter_qualify_unknown_mcast(bcm_filterid_t f);
extern int bcmx_filter_qualify_known_ucast(bcm_filterid_t f);
extern int bcmx_filter_qualify_known_mcast(bcm_filterid_t f);
extern int bcmx_filter_qualify_broadcast(bcm_filterid_t f);
extern int bcmx_filter_qualify_stop(bcm_filterid_t f, int partial_match);
extern int bcmx_filter_qualify_format(bcm_filterid_t f, bcm_filter_format_t format);
extern int bcmx_filter_qualify_data(bcm_filterid_t f, int offset, int len, const uint8 * data, const uint8 * mask);
extern int bcmx_filter_qualify_data8(bcm_filterid_t f, int offset, uint8 val, uint8 mask);
extern int bcmx_filter_qualify_data16(bcm_filterid_t f, int offset, uint16 val, uint16 mask);
extern int bcmx_filter_qualify_data32(bcm_filterid_t f, int offset, uint32 val, uint32 mask);
extern int bcmx_filter_qualify_data_cvt(bcm_filterid_t f);
extern int bcmx_filter_action_match(bcm_filterid_t f, bcm_filter_action_t action, uint32 param);
extern int bcmx_filter_action_no_match(bcm_filterid_t f, bcm_filter_action_t action, uint32 param);
extern int bcmx_filter_action_out_profile(bcm_filterid_t f, bcm_filter_action_t action, uint32 param, int meter_id);
extern int bcmx_filter_install(bcm_filterid_t f);
extern int bcmx_filter_reinstall(bcm_filterid_t f);
extern int bcmx_filter_remove(bcm_filterid_t f);
extern int bcmx_filter_remove_all(void);

#endif /* _BCMX_FILTER_H */
