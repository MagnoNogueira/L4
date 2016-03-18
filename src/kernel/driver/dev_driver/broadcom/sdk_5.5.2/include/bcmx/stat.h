/*
 * $Id: stat.h,v 1.8 Broadcom SDK $
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
 * File:    bcmx/stat.h
 * Purpose: BCMX Port Statistics APIs
 */

#ifndef _BCMX_STAT_H
#define _BCMX_STAT_H

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcm/stat.h>

extern int bcmx_stat_init(void);

extern int bcmx_stat_sync(void);

extern int bcmx_stat_clear(bcmx_lport_t port);

extern int bcmx_stat_get(bcmx_lport_t port, bcm_stat_val_t type,
                         uint64 *value);

extern int bcmx_stat_get32(bcmx_lport_t port, bcm_stat_val_t type,
                           uint32 *value);

extern int bcmx_stat_custom_set(bcmx_lport_t port, bcm_stat_val_t type,
                                uint32 flags);

extern int bcmx_stat_custom_get(bcmx_lport_t port, bcm_stat_val_t type,
                                uint32 *flags);

extern int bcmx_stat_custom_add(bcmx_lport_t port, bcm_stat_val_t type, 
                                bcm_custom_stat_trigger_t trigger);

extern int bcmx_stat_custom_delete(bcmx_lport_t port, bcm_stat_val_t type, 
                                   bcm_custom_stat_trigger_t trigger);

extern int bcmx_stat_custom_delete_all(bcmx_lport_t port, bcm_stat_val_t type);

extern int bcmx_stat_custom_check(bcmx_lport_t port, bcm_stat_val_t type, 
                                  bcm_custom_stat_trigger_t trigger,
                                  int *result);

#endif /* _BCMX_STAT_H */
