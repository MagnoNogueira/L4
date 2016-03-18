/*
 * $Id: diffserv.h,v 1.6 Broadcom SDK $
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
 * File:	bcmx/diffserv.h
 * Purpose:	BCMX Differentiated Services APIs
 */

#ifndef	_BCMX_DIFFSERV_H
#define	_BCMX_DIFFSERV_H

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/diffserv.h>

extern int bcmx_ds_init(void);
extern int bcmx_ds_datapath_create(int dpid,
				   uint32 flags,
				   bcmx_lplist_t plist);
extern int bcmx_ds_datapath_alloc_create(int *dpid,
					 uint32 flags,
					 bcmx_lplist_t plist);
extern int bcmx_ds_datapath_unit_count(int dpid);

extern int bcmx_ds_datapath_delete(int dpid);
extern int bcmx_ds_datapath_install(int dpid);

extern int bcmx_ds_classifier_create(int dpid,
				     int cfid,
				     bcm_ds_clfr_t *clfr,
				     bcm_ds_inprofile_actn_t *inp_actn,
				     bcm_ds_outprofile_actn_t *outp_actn,
				     bcm_ds_nomatch_actn_t *nm_actn);

extern int bcmx_ds_classifier_alloc_create(int dpid,
					   int *cfid,
					   bcm_ds_clfr_t *clfr,
					   bcm_ds_inprofile_actn_t *inp_actn,
					   bcm_ds_outprofile_actn_t *outp_actn,
					   bcm_ds_nomatch_actn_t *nm_actn);

extern int bcmx_ds_classifier_update(int dpid,
				     int cfid,
				     uint32 flags,
				     bcm_ds_inprofile_actn_t *inp_actn,
				     bcm_ds_outprofile_actn_t *outp_actn);

extern int bcmx_ds_classifier_delete(int dpid, int cfid);

extern int bcmx_ds_classifier_get(int dpid, int cfid, 
                                  bcm_ds_clfr_t *clfr, 
                                  bcm_ds_inprofile_actn_t *inp_actn, 
                                  bcm_ds_outprofile_actn_t *outp_actn, 
                                  bcm_ds_nomatch_actn_t *nm_actn);

extern int bcmx_ds_scheduler_add(int dpid, bcm_ds_scheduler_t *scheduler);
extern int bcmx_ds_counter_get(int dpid,
			       int cfid,
			       bcm_ds_counters_t *counter);

#if defined(BROADCOM_DEBUG)
extern int bcmx_ds_dump(void);
#endif  /* BROADCOM_DEBUG */

#endif	/* _BCMX_DIFFSERV_H */
