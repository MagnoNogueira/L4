/*
 * $Id: vlan.h,v 1.10 Broadcom SDK $
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
 */
#ifndef _BCM_INT_VLAN_H
#define _BCM_INT_VLAN_H

#include <sal/types.h>

/* QVLAN_UTBMP_BACKUP :
 *  - for keeping correct untag bitmp when turn on/off device double tagging mode.
 *  - ROBO chips (bcm5347/5348/53242) use 1Q_VLAN untag bitmap to apply to the 
 *      NNI/UNI port setting about untagging SP_TAG action.
 *      
 */
#define QVLAN_UTBMP_BACKUP      0


/*
 * VLAN internal struct used for book keeping
 */
typedef struct vlist_s {
    bcm_vlan_t          vid;

#if QVLAN_UTBMP_BACKUP
    /* ReadMe : ubmp 
     *   - This value should be maintained in realtime VLAN configuration
     *   - When Double tagging mode is enabled, this port bitmap might be 
     *      quite different with the untag_bitmap field in 1Q_VLAN table.
     *      The port's NNI/UNI setting causes such differnece.
     *   - If the double tagging mode is disabled again, the untag-bitmap in 
     *      all valid VLAN should re-programmed by this value(ubmp).
     *   - Each time to set any port's NNI/UNI mode will causes the untag 
     *      bitmap in all valid VLAN entries been reprogrammed. 
     */
    bcm_pbmp_t ubmp;    /* for the reference by port_dtag_mode setting */
#endif

    struct vlist_s      *next;
} vlist_t;

typedef struct bcm_vlan_prio_info_s {
    int     enable;				/* ??? */
    int     flag;				/* New re-mapping or Default mapping*/
    uint32  default_priority;	/* Default priority mapping */
    uint32  old_priority;		/* Previous priority mapping */
    uint32  new_priority;		/* New priority mapping */
} bcm_vlan_prio_info_t[BCM_LOCAL_UNITS_MAX];

#if QVLAN_UTBMP_BACKUP
    extern int _bcm_robo_vlan_utbmp_dt_rebuild(int unit, int mode);
#endif


#ifdef	BCM_53115
/* CFP may be used for supporting VLAN translation 
 *  - bcm53115 use CFP to implement the VLAN translation feature on 
 *      Mapping/Transparent mode.
 */
extern int flag_vt_cfp_init;  /* flag for the init status */ 

/* init check on VLAN translation CFP entries */
#define IS_VT_CFP_INIT          (flag_vt_cfp_init == TRUE)

/* VTCFP_SUPPORT_VLAN_CNT is a predefined number to represent the number 
 * of the VLAN on supporting VLAN translation.
 */
#define VTCFP_SUPPORT_VLAN_CNT    128

typedef struct drv_vt_cfp_entry_db_s {
    bcm_vlan_t  vid;
    int     vt_mode;        /* 0: transparent; 1: mapping */
    int     nni_field_id;   /* field entry id for NNI */
    int     uni_field_id;   /* field entry id for UNI */
    
    int     prev;       /* init as -1 */
    int     next;       /* init as -1 */
}drv_vt_cfp_entry_db_t;

typedef struct drv_vt_cfp_db_s {
    int     vt_cfp_db_valid_count;
    int     vt_cfp_db_start_id;     /* init as -1 */
    drv_vt_cfp_entry_db_t     vt_cfp_entry_db[VTCFP_SUPPORT_VLAN_CNT];
}drv_vt_cfp_db_t;

#define VTCFP_NULL_INDEX    -1

#define IS_VALID_CTCFP_DB_ENTRY_ID(_id)    \
                ((_id) >= 0 && (_id) < VTCFP_SUPPORT_VLAN_CNT)
#define IS_CTCFP_DB_FULL    \
            (vt_cfp_db.vt_cfp_db_valid_count >= VTCFP_SUPPORT_VLAN_CNT)

#define VTCFP_SWDB_OP_FLAG_INSERT   1
#define VTCFP_SWDB_OP_FLAG_DELETE   2
#define VTCFP_SWDB_OP_FLAG_RESET    3

/* global variable for keeping the cfp information for iDT_Mode */
typedef struct drv_idt_mode_cfp_info_s {
    /* cfp for untag action at none-isp port */
    int     none_isp_cfp_group_id;  /* field group id */
    int     none_isp_filter_id;     /* field entry id */
    int     none_isp_evr_id;        /* EVR table entry id */
    
    /* ---- cfp for vlan translation ---- */
    int     vt_cfp_group_id;    /* field group id */
    
    /* add more below if any cfp related group id have to create for APIs */
    
}drv_idt_mode_cfp_info_t;

extern drv_idt_mode_cfp_info_t  idt_mode_cfp_info;
/* macro define to get the pre-created CFP group id for egress vlan 
 *  translation usage.
 */
#define FIELD_GPID_FOR_VT   (idt_mode_cfp_info.vt_cfp_group_id)

extern int _bcm_robo_vlan_vtcfp_init(int unit);
extern int _bcm_robo_vlan_vtcfp_isp_change(int unit);
extern int _bcm_robo_vlan_vtevr_isp_change(int unit, pbmp_t changed_pbm);

#else	/* BCM_53115 */

#define IS_VT_CFP_INIT	0
#endif	/* BCM_53115 */


#endif

