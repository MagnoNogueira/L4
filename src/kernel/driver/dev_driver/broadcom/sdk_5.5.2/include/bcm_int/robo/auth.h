/*
 * $Id: auth.h,v 1.15 Broadcom SDK $
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
 * This file contains Auth definitions internal to the BCM library.
 */
#ifndef _BCM_INT_AUTH_H
#define _BCM_INT_AUTH_H

#include <soc/drv.h>
#define BCM_AUTH_SEC_NONE		    0x00010000
#define BCM_AUTH_SEC_STATIC_ACCEPT	0x00020000
#define BCM_AUTH_SEC_STATIC_REJECT	0x00040000
#define BCM_AUTH_SEC_SA_NUM		    0x00080000
#define BCM_AUTH_SEC_SA_MATCH		0x00100000
#define BCM_AUTH_SEC_EXTEND_MODE    0x00200000
#define BCM_AUTH_SEC_SIMPLIFY_MODE  0x00400000

#define STATIC_MAC_WRITE 0  /* For Static Mac Security Write operateion */
#define STATIC_MAC_READ  1  /* For Static Mac Security Read operateion */

#define MAX_SEC_MAC	16 /* Sec Macs per port */



        
typedef struct robo_static_mac_s
{
#ifdef LE_HOST
    uint8    mac[3];  /* Low/Hight part of Static Security Mac */
    uint8    valid:1; /* Valid bit */
    uint8    rsvd:7;  /* reserved */
#else
    uint8    rsvd:7;  /* reserved */
    uint8    valid:1; /* Valid bit */
    uint8    mac[3];  /* Low/Hight part of Static Security Mac */
#endif
} robo_static_mac_t;

typedef struct robo_eap_port_conf_s
{
#ifdef LE_HOST
    uint8    eapUniDa[6];  /* EAP frame Unicast DA */
    uint16   enEapUniDa:1; /* Enable EAP frame with unicast DA */
    uint16   eapBlkMode:1; /* When set, only special frame will be
                    received; other frame will be dropped */
    uint16   EapEn:1;            /* Enable EAP (802.1x) function */
    uint16   rsvd:13;      /* reserved */
#else
    uint16   rsvd:13;      /* reserved */
    uint16   EapEn:1;            /* Enable EAP (802.1x) function */
    uint16   eapBlkMode:1; /* When set, only special frame will be
                    received; other frame will be dropped */
    uint16   enEapUniDa:1; /* Enable EAP frame with unicast DA */
    uint8    eapUniDa[6];  /* EAP frame Unicast DA */
#endif
} robo_eap_port_conf_t;

typedef struct robo_eap_gbl_conf_s
{
#ifdef LE_HOST
    uint32    enEapChk:1; /* Enable EAP Frame Packet Type Check */
    uint32    spFrame:22; /* Special Frame, ex. 01-80-C2-00-00-01 */
    uint32    rsvd:9;     /* reserved */
#else
    uint32    rsvd:9;     /* reserved */
    uint32    spFrame:22; /* Special Frame, ex. 01-80-C2-00-00-01 */
    uint32    enEapChk:1; /* Enable EAP Frame Packet Type Check */
#endif
} robo_eap_gbl_conf_t;

typedef struct robo_8021x_ctrl_1_s
{
#ifdef LE_HOST
    uint32    rsvd0:1;         /* reserved */
    uint32    BPDU_EAPOL_en:1; /* BPDU and EAPOL enable */
    uint32    rsvd:14;         /* reserved */
#else
    uint32    rsvd:14;         /* reserved */
    uint32    BPDU_EAPOL_en:1; /* BPDU and EAPOL enable */
    uint32    rsvd0:1;         /* reserved */
#endif
} robo_8021x_ctrl_1_t;

typedef struct robo_8021x_ctrl_2_s
{
#ifdef LE_HOST
    uint32    EapEn:9;        /* Enable EAP (802.1x) function */
    uint32    rsvd:7;         /* reserved */
#else
    uint32    rsvd:7;         /* reserved */
    uint32    EapEn:9;        /* Enable EAP (802.1x) function */
#endif
} robo_8021x_ctrl_2_t;

extern int bcm_robo_auth_sec_mac_add(int unit, 
                bcm_port_t port, bcm_mac_t mac);
extern int bcm_robo_auth_sec_mac_delete(int unit, 
                bcm_port_t port, bcm_mac_t mac);
extern int bcm_robo_auth_sec_mac_get(int unit, 
                bcm_port_t port,bcm_mac_t *mac, int *num);
extern int bcm_robo_auth_sec_mode_set(int unit, 
                bcm_port_t port, int mode,int mac_num);
extern int bcm_robo_auth_sec_mode_get(int unit, 
                bcm_port_t port, int *mode, int *mac_num);
extern int 
_bcm_robo_auth_sec_mode_set(int unit, bcm_port_t port, int mode,
                 int mac_num);


#endif
