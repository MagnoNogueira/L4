/*
 * $Id: phyctrl.h,v 1.4.6.1 Broadcom SDK $
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
 * File:        phyctrl.h
 * Purpose:     Data structures and function prototypes for PHY configuration.
 * Note:        This header file is for PHY driver module. Should not include
 *              this header file in SOC or BCM layers except
 *              soc/common/phyctrl.c
 */

#ifndef _PHY_PHYCTRL_H_
#define _PHY_PHYCTRL_H_

/* Data Structures */
typedef struct phy_ctrl_s {
    int                 unit;               /* BCM unit number */
    soc_port_t          port;               /* BCM port number */
    uint16              phy_id0;            /* PHY ID0 register */
    uint16              phy_id1;            /* PHY ID1 register */
    uint16              phy_model;          /* PHY model from ID reg */
    uint16              phy_rev;            /* PHY revision from ID reg */
    uint16              phy_oui;            /* PHY oui from ID reg */
    uint8               phy_id;             /* PHY ID on MDIO bus */
    uint8               lane_num;           /* Lane number in a device */ 
    int                 speed_max;          /* Restrict support to speed max */ 
    uint8               ledmode[4];         /* LED1..4 selectors */
    uint16              ledctrl;            /* LED control */
    uint16              ledselect;          /* LED select */
    int                 automedium;         /* Medium auto-select active */
    int                 tbi_capable;        /* PHY & SOC both support TBI */
    int                 medium;             /* Last active medium type */
    int                 fiber_detect;       /* Fiber Signal Detection */
    int                 halfout;            /* Transmit half amplitude */
    int                 fiber_plug_dely;    /* De-glitch time from property */
    sal_usecs_t         fiber_plug_time;    /* Time de-glitch started */
    int                 fiber_plug_wait;    /* De-glitch timer is running */
    int                 fiber_plug_done;    /* De-glitch period satisfied */
    sal_usecs_t         an_state_time;
    int                 an_state_valid;
    uint16              sgmii_an_state;
    soc_port_if_t       interface;          /* Operating mode of PHY */
    soc_phy_config_t    copper;             /* Current copper config */
    soc_phy_config_t    fiber;              /* Current fiber config */
    uint32              stop;               /* Power up/down state */
    uint32              flags;              /* PHY_STOP flags */ 
    struct phy_driver_s *pd;                /* PHY driver functions */
    int                 (*read)(int, uint32, uint32, uint16*); /* HW read */
    int                 (*write)(int, uint32, uint32, uint16); /* HW write */
    void                *driver_data;       /* driver private data */
} phy_ctrl_t;

extern phy_ctrl_t **int_phy_ctrl[SOC_MAX_NUM_DEVICES];
extern phy_ctrl_t **ext_phy_ctrl[SOC_MAX_NUM_DEVICES];

#define INT_PHY_SW_STATE(unit, port)  (int_phy_ctrl[unit][port])
#define EXT_PHY_SW_STATE(unit, port)  (ext_phy_ctrl[unit][port])

/* Param check macros */
#define EXT_PHY_INIT_CHECK(unit, port) \
    if (EXT_PHY_SW_STATE(unit, port) == NULL) { return SOC_E_INIT; }
#define INT_PHY_INIT_CHECK(unit, port) \
    if (INT_PHY_SW_STATE(unit, port) == NULL) { return SOC_E_INIT; }

extern  int
soc_phy_probe(int unit, soc_port_t port, phy_ctrl_t *ext_pc,
                              phy_ctrl_t *int_pc);

#define _PHYCTRL_CALL(_pc, _pf, _pa) \
        ((_pc) == 0 ? SOC_E_INIT : \
         (((_pc)->pd)->_pf == 0 ? SOC_E_UNAVAIL : ((_pc)->pd)->_pf _pa))

#define PHYCTRL_INIT(_pc, _u, _p) \
        _PHYCTRL_CALL((_pc), pd_init, ((_u), (_p)))

#define PHYCTRL_RESET(_pc, _u, _p) \
        _PHYCTRL_CALL((_pc), pd_reset, ((_u), (_p), NULL))

#define PHYCTRL_LINK_GET(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_link_get, ((_u), (_p), (_l)))

#define PHYCTRL_ENABLE_SET(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_enable_set, ((_u), (_p), (_l)))

#define PHYCTRL_ENABLE_GET(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_enable_get, ((_u), (_p), (_l)))

#define PHYCTRL_DUPLEX_SET(_pc, _u, _p, _d) \
        _PHYCTRL_CALL((_pc), pd_duplex_set, ((_u), (_p), (_d)))

#define PHYCTRL_DUPLEX_GET(_pc, _u, _p, _d) \
        _PHYCTRL_CALL((_pc), pd_duplex_get, ((_u), (_p), (_d)))

#define PHYCTRL_SPEED_SET(_pc, _u, _p, _s) \
        _PHYCTRL_CALL((_pc), pd_speed_set, ((_u), (_p), (_s)))

#define PHYCTRL_SPEED_GET(_pc, _u, _p, _s) \
        _PHYCTRL_CALL((_pc), pd_speed_get, ((_u), (_p), (_s)))

#define PHYCTRL_MASTER_SET(_pc, _u, _p, _s) \
        _PHYCTRL_CALL((_pc), pd_master_set, ((_u), (_p), (_s)))

#define PHYCTRL_MASTER_GET(_pc, _u, _p, _s) \
        _PHYCTRL_CALL((_pc), pd_master_get, ((_u), (_p), (_s)))

#define PHYCTRL_AUTO_NEGOTIATE_SET(_pc, _u, _p, _a) \
        _PHYCTRL_CALL((_pc), pd_an_set, ((_u), (_p), (_a)))

#define PHYCTRL_AUTO_NEGOTIATE_GET(_pc, _u, _p, _a, _b) \
        _PHYCTRL_CALL((_pc), pd_an_get, ((_u), (_p), (_a), (_b)))

#define PHYCTRL_ADV_SET(_pc, _u, _p, _a) \
        _PHYCTRL_CALL((_pc), pd_adv_local_set, ((_u), (_p), (_a)))

#define PHYCTRL_ADV_GET(_pc, _u, _p, _a) \
        _PHYCTRL_CALL((_pc), pd_adv_local_get, ((_u), (_p), (_a)))

#define PHYCTRL_LOOPBACK_SET(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_lb_set, ((_u), (_p), (_l)))

#define PHYCTRL_LOOPBACK_GET(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_lb_get, ((_u), (_p), (_l)))

#define PHYCTRL_INTERFACE_SET(_pc, _u, _p, _i) \
        _PHYCTRL_CALL((_pc), pd_interface_set, ((_u), (_p), (_i)))

#define PHYCTRL_INTERFACE_GET(_pc, _u, _p, _i) \
        _PHYCTRL_CALL((_pc), pd_interface_get, ((_u), (_p), (_i)))

#define PHYCTRL_ABILITY_GET(_pc, _u, _p, _m) \
        _PHYCTRL_CALL((_pc), pd_ability, ((_u), (_p), (_m)))

#define PHYCTRL_LINKUP_EVT(_pc, _u, _p) \
        _PHYCTRL_CALL((_pc), pd_linkup_evt, ((_u), (_p)))

#define PHYCTRL_LINKDN_EVT(_pc, _u, _p) \
        _PHYCTRL_CALL((_pc), pd_linkdn_evt, ((_u), (_p)))

#define PHYCTRL_ADV_REMOTE_GET(_pc, _u, _p, _m) \
        _PHYCTRL_CALL((_pc), pd_adv_remote_get, ((_u), (_p), (_m)))

#define PHYCTRL_MDIX_SET(_pc, _u, _p, _m) \
        _PHYCTRL_CALL((_pc), pd_mdix_set, ((_u), (_p), (_m)))

#define PHYCTRL_MDIX_GET(_pc, _u, _p, _m) \
        _PHYCTRL_CALL((_pc), pd_mdix_get, ((_u), (_p), (_m)))

#define PHYCTRL_MDIX_STATUS_GET(_pc, _u, _p, _s) \
        _PHYCTRL_CALL((_pc), pd_mdix_status_get, ((_u), (_p), (_s)))

#define PHYCTRL_MEDIUM_CONFIG_SET(_pc, _u, _p, _m, _c) \
        _PHYCTRL_CALL((_pc), pd_medium_config_set, ((_u), (_p), (_m), (_c)))

#define PHYCTRL_MEDIUM_CONFIG_GET(_pc, _u, _p, _m, _c) \
        _PHYCTRL_CALL((_pc), pd_medium_config_get, ((_u), (_p), (_m), (_c)))

#define PHYCTRL_MEDIUM_GET(_pc, _u, _p, _m) \
        _PHYCTRL_CALL((_pc), pd_medium_get, ((_u), (_p), (_m)))

#define PHYCTRL_CABLE_DIAG(_pc, _u, _p, _status) \
        _PHYCTRL_CALL((_pc), pd_cable_diag, ((_u), (_p), (_status)))

#define PHYCTRL_LINK_CHANGE(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_link_change, ((_u), (_p), (_l)))

#define PHYCTRL_CONTROL_SET(_pc, _u, _p, _t, _v) \
        _PHYCTRL_CALL((_pc), pd_control_set, ((_u), (_p), (_t), (_v)))

#define PHYCTRL_CONTROL_GET(_pc, _u, _p, _t, _v) \
        _PHYCTRL_CALL((_pc), pd_control_get, ((_u), (_p), (_t), (_v)))

#define PHYCTRL_REG_READ(_pc, _u, _p, _f, _a, _v) \
        _PHYCTRL_CALL((_pc), pd_reg_read, ((_u), (_p), (_f), (_a), (_v)))

#define PHYCTRL_REG_WRITE(_pc, _u, _p, _f, _a, _v) \
        _PHYCTRL_CALL((_pc), pd_reg_write, ((_u), (_p), (_f), (_a), (_v)))

#define PHYCTRL_REG_MODIFY(_pc, _u, _p, _f, _a, _v, _m) \
        _PHYCTRL_CALL((_pc), pd_reg_modify, ((_u), (_p), (_f), (_a), (_v), (_m)))

#define PHYCTRL_NOTIFY(_pc, _u, _p, _e, _v) \
        _PHYCTRL_CALL((_pc), pd_notify, ((_u), (_p), (_e), (_v)))

#endif /* _PHY_PHYCTRL_H_ */
