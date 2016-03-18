/*
 * $Id: port_ability.h,v 1.8 Broadcom SDK $
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
 * This file defines common network port modes.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define port modes.
 */

#ifndef _SHR_PORTABILITY_H
#define _SHR_PORTABILITY_H

typedef struct _shr_port_ability_s {
    _shr_port_mode_t speed_half_duplex;
    _shr_port_mode_t speed_full_duplex;
    _shr_port_mode_t pause;
    _shr_port_mode_t interface;
    _shr_port_mode_t medium;
    _shr_port_mode_t loopback;
    _shr_port_mode_t flags;
} _shr_port_ability_t;

#define _SHR_PA_ABILITY_ALL     (0xffffffff)
#define _SHR_PA_ABILITY_NONE    (0)
/*
 * Defines:
 *      _SHR_PA_SPEED_*
 * Purpose:
 *      Defines for half duplex port speeds.
 */
#define _SHR_PA_SPEED_10MB         (1 << 0)
#define _SHR_PA_SPEED_100MB        (1 << 1)
#define _SHR_PA_SPEED_1000MB       (1 << 2)
#define _SHR_PA_SPEED_2500MB       (1 << 3)
#define _SHR_PA_SPEED_3000MB       (1 << 4)
#define _SHR_PA_SPEED_5000MB       (1 << 5)
#define _SHR_PA_SPEED_6000MB       (1 << 6)
#define _SHR_PA_SPEED_10GB         (1 << 7)
#define _SHR_PA_SPEED_12GB         (1 << 8)
#define _SHR_PA_SPEED_12P5GB       (1 << 9)
#define _SHR_PA_SPEED_13GB         (1 << 10)
#define _SHR_PA_SPEED_15GB         (1 << 11)
#define _SHR_PA_SPEED_16GB         (1 << 12)
#define _SHR_PA_SPEED_20GB         (1 << 13)
#define _SHR_PA_SPEED_21GB         (1 << 14)
#define _SHR_PA_SPEED_25GB         (1 << 15)

#define _SHR_PA_STRING       {"10MB", \
                                 "100MB", \
                                 "1000MB", \
                                 "2500MB", \
                                 "3000MB", \
                                 "5000MB", \
                                 "6000MB", \
                                 "10GB", \
                                 "12GB", \
                                 "12P5GB", \
                                 "13GB", \
                                 "15GB", \
                                 "16GB", \
                                 "20GB", \
                                 "21GB", \
                                 "25GB"}
                                        
/*
 * Defines:
 *      _SHR_PA_PAUSE_*
 * Purpose:
 *      Defines for flow control abilities.
 */
#define _SHR_PA_PAUSE_TX        (1 << 0)       /* TX pause capable */
#define _SHR_PA_PAUSE_RX        (1 << 1)       /* RX pause capable */
#define _SHR_PA_PAUSE_ASYMM     (1 << 2)       /* Asymm pause capable (R/O) */

#define _SHR_PA_PAUSE_STRING    {"PAUSE_TX", \
                                 "PAUSE_RX", \
                                 "PAUSE_ASYMM"}

/*
 * Defines:
 *      _SHR_PA_INTF_*
 * Purpose:
 *      Defines for port interfaces supported.
 */
#define _SHR_PA_INTF_TBI        (1 << 0)       /* TBI mode supported */
#define _SHR_PA_INTF_MII        (1 << 1)       /* MII mode supported */
#define _SHR_PA_INTF_GMII       (1 << 2)       /* GMII mode supported */
#define _SHR_PA_INTF_RGMII      (1 << 3)       /* RGMII mode supported */
#define _SHR_PA_INTF_SGMII      (1 << 4)       /* SGMII mode supported */
#define _SHR_PA_INTF_XGMII      (1 << 5)       /* XGMII mode supported */
#define _SHR_PA_INTF_QSGMII     (1 << 6)       /* QSGMII mode supported */

#define _SHR_PA_INTF_STRING     {"TBI", \
                                 "MII", \
                                 "GMII", \
                                 "RGMII", \
                                 "SGMII", \
                                 "XGMII"}

/*
 * Defines:
 *      _SHR_PA_MEDIUM_*
 * Purpose:
 *      Defines for port medium modes.
 */
#define _SHR_PA_MEDIUM_COPPER   (1 << 0)
#define _SHR_PA_MEDIUM_FIBER    (1 << 1)

#define _SHR_PA_MEDIUM_STRING   {"COPPER", \
                                 "FIBER"}
/*
 * Defines:
 *      _SHR_PA_LOOPBACK_*
 * Purpose:
 *      Defines for port loopback modes.
 */
#define _SHR_PA_LB_NONE         (1 << 0)       /* Useful for automated test */
#define _SHR_PA_LB_MAC          (1 << 1)       /* MAC loopback supported */
#define _SHR_PA_LB_PHY          (1 << 2)       /* PHY loopback supported */
#define _SHR_PA_LB_LINE         (1 << 3)       /* PHY lineside loopback */

#define _SHR_PA_LB_STRING       {"LB_NONE", \
                                 "LB_MAC", \
                                 "LB_PHY", \
                                 "LB_LINE"}

/*
 * Defines:
 *      _SHR_PA_FLAGS_*
 * Purpose:
 *      Defines for the reest of port ability flags.
 */
#define _SHR_PA_AUTONEG         (1 << 0)       /* Auto-negotiation */
#define _SHR_PA_COMBO           (1 << 1)       /* COMBO ports support both
                                                * copper and fiber interfaces */
#define _SHR_PA_FLAGS_STRING    {"AUTONEG", \
                                 "COMBO"}

#define _SHR_PA_PAUSE           (_SHR_PA_PAUSE_TX  | _SHR_PA_PAUSE_RX)

#define _SHR_PA_SPEED_ALL    (_SHR_PA_SPEED_25GB |         \
                              _SHR_PA_SPEED_21GB |         \
                              _SHR_PA_SPEED_20GB |         \
                              _SHR_PA_SPEED_16GB |         \
                              _SHR_PA_SPEED_15GB |         \
                              _SHR_PA_SPEED_13GB |         \
                              _SHR_PA_SPEED_12P5GB |       \
                              _SHR_PA_SPEED_12GB |         \
                              _SHR_PA_SPEED_10GB |         \
                              _SHR_PA_SPEED_6000MB |       \
                              _SHR_PA_SPEED_5000MB |       \
                              _SHR_PA_SPEED_3000MB |       \
                              _SHR_PA_SPEED_2500MB |       \
                              _SHR_PA_SPEED_1000MB |       \
                              _SHR_PA_SPEED_100MB |        \
                              _SHR_PA_SPEED_10MB)


#define _SHR_PA_SPEED_MAX(m) (((m) & _SHR_PA_SPEED_25GB)     ? 25000 : \
                              ((m) & _SHR_PA_SPEED_21GB)     ? 21000 : \
                              ((m) & _SHR_PA_SPEED_20GB)     ? 20000 : \
                              ((m) & _SHR_PA_SPEED_16GB)     ? 16000 : \
                              ((m) & _SHR_PA_SPEED_15GB)     ? 15000 : \
                              ((m) & _SHR_PA_SPEED_13GB)     ? 13000 : \
                              ((m) & _SHR_PA_SPEED_12P5GB)   ? 12500 : \
                              ((m) & _SHR_PA_SPEED_12GB)     ? 12000 : \
                              ((m) & _SHR_PA_SPEED_10GB)     ? 10000 : \
                              ((m) & _SHR_PA_SPEED_6000MB)   ? 6000 : \
                              ((m) & _SHR_PA_SPEED_5000MB)   ? 5000 : \
                              ((m) & _SHR_PA_SPEED_3000MB)   ? 3000 : \
                              ((m) & _SHR_PA_SPEED_2500MB)   ? 2500 : \
                              ((m) & _SHR_PA_SPEED_1000MB)   ? 1000 : \
                              ((m) & _SHR_PA_SPEED_100MB)    ? 100 : \
                              ((m) & _SHR_PA_SPEED_10MB)     ? 10 : 0)


#define _SHR_PA_SPEED(s)     ((25000 == (s)) ? _SHR_PA_SPEED_25GB : \
                              (21000 == (s)) ? _SHR_PA_SPEED_21GB : \
                              (20000 == (s)) ? _SHR_PA_SPEED_20GB : \
                              (16000 == (s)) ? _SHR_PA_SPEED_16GB : \
                              (15000 == (s)) ? _SHR_PA_SPEED_15GB : \
                              (13000 == (s)) ? _SHR_PA_SPEED_13GB : \
                              (12500 == (s)) ? _SHR_PA_SPEED_12P5GB : \
                              (12000 == (s)) ? _SHR_PA_SPEED_12GB : \
                              (10000 == (s)) ? _SHR_PA_SPEED_10GB : \
                              (6000  == (s)) ? _SHR_PA_SPEED_6000MB : \
                              (5000  == (s)) ? _SHR_PA_SPEED_5000MB : \
                              (3000  == (s)) ? _SHR_PA_SPEED_3000MB : \
                              (2500  == (s)) ? _SHR_PA_SPEED_2500MB : \
                              (1000  == (s)) ? _SHR_PA_SPEED_1000MB : \
                              (100   == (s)) ? _SHR_PA_SPEED_100MB : \
                              (10    == (s)) ? _SHR_PA_SPEED_10MB : 0)

#endif  /* !_SHR_PORTABILITY_H */
