/*
 * $Id: stg.h,v 1.6 Broadcom SDK $
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
 * File:	bcmx/stg.h
 * Purpose:	BCMX Spanning Tree Group APIs
 */

#ifndef	_BCMX_STG_H
#define	_BCMX_STG_H

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/stg.h>

extern int bcmx_stg_init(void);
extern int bcmx_stg_default_get(bcm_stg_t *stg_ptr);
extern int bcmx_stg_default_set(bcm_stg_t stg);
extern int bcmx_stg_vlan_add(bcm_stg_t stg, bcm_vlan_t vid);
extern int bcmx_stg_vlan_remove(bcm_stg_t stg, bcm_vlan_t vid);
extern int bcmx_stg_vlan_remove_all(bcm_stg_t stg);
extern int bcmx_stg_vlan_list(bcm_stg_t stg, bcm_vlan_t **list, int *count);
extern int bcmx_stg_vlan_list_destroy(bcm_vlan_t *list, int count);
extern int bcmx_stg_create(bcm_stg_t *stg_ptr);
extern int bcmx_stg_create_id(bcm_stg_t stg);
extern int bcmx_stg_destroy(bcm_stg_t stg);
extern int bcmx_stg_list(bcm_stg_t **list, int *count);
extern int bcmx_stg_list_destroy(bcm_stg_t *list, int count);
extern int bcmx_stg_stp_set(bcm_stg_t stg, bcmx_lport_t port, int stp_state);
extern int bcmx_stg_stp_get(bcm_stg_t stg, bcmx_lport_t port, int *stp_state);
extern int bcmx_stg_count_get(int *max_stg);

#endif	/* _BCMX_STG_H */
