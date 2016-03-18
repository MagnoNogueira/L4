/*
 * $Id: phyctrl.h,v 1.23 Broadcom SDK $
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
 *
 * Data structures and function prototypes for PHY register access.
 */

#ifndef _SOC_PHYCTRL_H_
#define _SOC_PHYCTRL_H_
#include <shared/port.h>
#include <sal/core/time.h>
#include <soc/portmode.h>
#include <soc/port_ability.h>

/* PHY specific control settings */
typedef enum soc_phy_control_e {
    SOC_PHY_CONTROL_WAN            = _SHR_PORT_PHY_CONTROL_WAN,
    SOC_PHY_CONTROL_PREEMPHASIS    = _SHR_PORT_PHY_CONTROL_PREEMPHASIS,
    SOC_PHY_CONTROL_DRIVER_CURRENT = _SHR_PORT_PHY_CONTROL_DRIVER_CURRENT,
    SOC_PHY_CONTROL_PRE_DRIVER_CURRENT =
                          _SHR_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT,
    SOC_PHY_CONTROL_COUNT           /* always last */
} soc_phy_control_t;

typedef enum soc_phy_event_e {
    phyEventInterface,
    phyEventDuplex,
    phyEventSpeed,
    phyEventStop,
    phyEventResume,
    phyEventAutoneg,
    phyEventCount            /* Always last */
} soc_phy_event_t;

#define PHY_EVENT_STRING {"Interface", \
                          "Duplex", \
                          "Speed", \
                          "Stop", \
                          "Resume", \
                          "Autoneg"}

/* PHY_STOP flag to use with phyEventStop and phyEventResume */
#define PHY_STOP_MAC_DIS        (0x01)
#define PHY_STOP_PHY_DIS        (0x02)
#define PHY_STOP_DRAIN          (0x04)
#define PHY_STOP_DUPLEX_CHG     (0x08)
#define PHY_STOP_SPEED_CHG      (0x10)
#define PHY_STOP_COPPER         (0x20)

/* Put extra PHY information for each port here. */
typedef struct soc_phy_info_s {
    uint16      phy_id0;        /* PHY ID0 Register */
    uint16      phy_id1;        /* PHY ID1 Register */
    uint8       phy_addr;       /* PHY address to use for MII ops */
    uint8       phy_addr_int;   /* Internal PHY address if applicable */
    char        *phy_name;      /* Short name of PHY */
    uint32      phy_flags;      /* Logical OR among PHY_FLAGS_xxx */
    sal_usecs_t an_timeout;     /* Autoneg timeout (from property) */
} soc_phy_info_t;

extern soc_phy_info_t *phy_port_info[SOC_MAX_NUM_DEVICES];
extern soc_port_t *phy_rmap[SOC_MAX_NUM_DEVICES];

#define PHY_MIN_REG             0
#define PHY_MAX_REG             0x1f

/* PHY address per MDIO Bus */
#define PHY_ADDR_MIN            0
#define PHY_ADDR_MAX            31

#define EXT_PHY_ADDR_MAX        256

#define PHY_ADDR_TO_PORT(unit, phy_addr) (phy_rmap[unit][phy_addr])
#define SOC_PHY_INFO(unit, port)     (phy_port_info[unit][port])
#define PHY_ID0_REG(unit, port)      (SOC_PHY_INFO(unit, port).phy_id0) 
#define PHY_ID1_REG(unit, port)      (SOC_PHY_INFO(unit, port).phy_id1) 
#define PHY_ADDR(unit, port)         (SOC_PHY_INFO(unit, port).phy_addr)
#define PHY_ADDR_INT(unit, port)     (SOC_PHY_INFO(unit, port).phy_addr_int)
#define PHY_NAME(unit, port)         (SOC_PHY_INFO(unit, port).phy_name)
#define PHY_FLAGS(unit, port)        (SOC_PHY_INFO(unit, port).phy_flags)
#define PHY_AN_TIMEOUT(unit, port)   (SOC_PHY_INFO(unit, port).an_timeout)

#define PHY_FLAGS_COPPER           (1 << 0)  /* copper medium */
#define PHY_FLAGS_FIBER            (1 << 1)  /* fiber medium */
#define PHY_FLAGS_PASSTHRU         (1 << 2)  /* serdes passthru (5690) */
#define PHY_FLAGS_10B              (1 << 3)  /* ten bit interface (TBI) */
#define PHY_FLAGS_5421S            (1 << 4)  /* True if PHY is a 5421S */
#define PHY_FLAGS_DISABLE          (1 << 5)  /* True if PHY is disabled */
#define PHY_FLAGS_C45              (1 << 6)  /* True if PHY uses clause 45
                                              * MIIM */
#define PHY_FLAGS_100FX            (1 << 7)  /* True if PHY at 100FX
                                              * (for 5482S) */
#define PHY_FLAGS_MEDIUM_CHANGE    (1 << 8)  /* True if PHY medium changed
                                              * between copper and fiber */
#define PHY_FLAGS_SERDES_FIBER     (1 << 9)  /* True if use internal serdes
                                              * phy */
#define PHY_FLAGS_WAN              (1 << 10) /* WAN mode */
#define PHY_FLAGS_SGMII_AUTONEG    (1 << 11) /* Use SGMII autoneg between
                                              * internal SerDes and external
                                              * PHY */
#define PHY_FLAGS_EXTERNAL_PHY     (1 << 12) /* Port has external PHY */
#define PHY_FLAGS_FORCED_SGMII     (1 << 13) /* Interface between internal and
                                              * external PHY is always SGMII */ 
#define PHY_FLAGS_FORCED_COPPER    (1 << 14) /* Forced media to copper */
#define PHY_FLAGS_C73              (1 << 15) /* Auto-negotiation for Backplane
                                              * Ethernet (clause 73) */
#define PHY_FLAGS_INDEPENDENT_LANE (1 << 16) /* Treat each XGXS lane as 
                                              * independent lane. */
#define PHY_FLAGS_SINGLE_LANE      (1 << 17) /* Use only lane 0 of XGXS core. */

#define PHY_FLAGS_PRIMARY_SERDES    (1<<18)

#define PHY_FLAGS_SECONDARY_SERDES (1<<19)

#define PHY_FLAGS_SET(unit, port, flags) \
            (PHY_FLAGS(unit, port) |= (flags))
#define PHY_FLAGS_CLR(unit, port, flags) \
            (PHY_FLAGS(unit, port) &= ~(flags))
#define PHY_FLAGS_TST(unit, port, flags) \
            ((PHY_FLAGS(unit, port) & (flags)) == (flags))
#define PHY_FLAGS_CLR_ALL(unit, port) \
            (PHY_FLAGS(unit, port) = 0)
#define PHY_COPPER_MODE(unit, port)        PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_COPPER)
#define PHY_FIBER_MODE(unit, port)         PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_FIBER)
#define PHY_PASSTHRU_MODE(unit, port)      PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_PASSTHRU)
#define PHY_TBI_MODE(unit, port)           PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_10B)
#define PHY_5421S_MODE(unit, port)         PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_5421S)
#define PHY_DISABLED_MODE(unit, port)      PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_DISABLE)
#define PHY_CLAUSE45_MODE(unit, port)      PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_C45)
#define PHY_FIBER_100FX_MODE(unit, port)   PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_100FX)
#define PHY_MEDIUM_CHANGED(unit, port)     PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_MEDIUM_CHANGE)
#define PHY_SERDES_FIBER_MODE(unit, port)  PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_SERDES_FIBER)
#define PHY_WAN_MODE(unit, port)           PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_WAN)
#define PHY_SGMII_AUTONEG_MODE(unit, port) PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_SGMII_AUTONEG)
#define PHY_EXTERNAL_MODE(unit, port)      PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_EXTERNAL_PHY)
#define PHY_FORCED_SGMII_MODE(unit, port)  PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_FORCED_SGMII)
#define PHY_FORCED_COPPER_MODE(unit, port) PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_FORCED_COPPER)
#define PHY_CLAUSE73_MODE(unit, port)      PHY_FLAGS_TST(unit, port, \
                                                         PHY_FLAGS_C73)
#define PHY_INDEPENDENT_LANE_MODE(unit, port) PHY_FLAGS_TST(unit, port, \
                                                     PHY_FLAGS_INDEPENDENT_LANE)
#define PHY_SINGLE_LANE_MODE(unit, port)   PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_SINGLE_LANE)
#define PHY_PRIMARY_SERDES_MODE(unit, port)  PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_PRIMARY_SERDES)
#define PHY_SECONDARY_SERDES_MODE(unit, port)  PHY_FLAGS_TST(unit, port, \
                                                        PHY_FLAGS_SECONDARY_SERDES)
/* PHY 100FX support
  * - Currently, the driver supports phy5482s only. Early chip like phy5461, 
  *     phy5464 or other should be very different on 100FX specification.
  * - PHY5482s 100FX in bcm95324_a1 board :
  *     a. 100FX(SFP)-to-fiber wire, means wired at SGOUT+/- and SGIN+/-.
  *     b. This driver is designed for the 100FX/1000X fiber auto-detect mode.
  *     c. both 100FX/1000X use the same signal detecting for awaring fiber 
  *         link.
  * - Extern definition for explain the 100FX property, PHY_100FX_PROPERTY.
  *     a. Fiber 100FX          0x1
  *     b. Copper 100FX     0x2
  *     c. 100FX only       0x10
  *     d. 100FX/1000X auto-detect      0x20
  */
#define PHY_100FX_SUPPORT   1
#if PHY_100FX_SUPPORT
    #define PROP_100FX_FIBER        0x1
    #define PROP_100FX_COPPER       0x2
    #define PROP_100FX_ONLY         0x10
    #define PROP_100FX_1000X_BOTH   0x20

    /* above property-bit mask */
    #define PHY_100FX_PROPERTY      (PROP_100FX_FIBER |PROP_100FX_1000X_BOTH)
#else
    #undef PHY_100FX_SUPPORT
#endif

extern int
soc_phyctrl_init_phy_id_map(int unit, int map[]);

extern int
soc_phyctrl_software_init(int unit);

extern int
soc_phyctrl_init(int unit, soc_port_t port);

extern int
soc_phyctrl_probe(int unit, soc_port_t port);

extern int
soc_phyctrl_drv_name_get(int unit, soc_port_t port, char *name, int len);

extern char * 
soc_phyctrl_drv_name(int unit, soc_port_t port);

extern int
soc_phyctrl_init(int unit, soc_port_t port);

extern int
soc_phyctrl_reset(int unit, soc_port_t port, void *user_arg);

extern int
soc_phyctrl_link_get(int unit, soc_port_t port, int *link);

extern int
soc_phyctrl_enable_set(int unit, soc_port_t port, int enable);

extern int
soc_phyctrl_enable_get(int unit, soc_port_t port, int *enable);

extern int
soc_phyctrl_duplex_set(int unit, soc_port_t port, int duplex);

extern int
soc_phyctrl_duplex_get(int unit, soc_port_t port, int *duplex);

extern int
soc_phyctrl_speed_set(int unit, soc_port_t port, int speed);

extern int
soc_phyctrl_speed_get(int unit, soc_port_t port, int *speed);

extern int
soc_phyctrl_master_set(int unit, soc_port_t port, int master);

extern int
soc_phyctrl_master_get(int unit, soc_port_t port, int *master);

extern int
soc_phyctrl_auto_negotiate_set(int unit, soc_port_t port, int an);

extern int
soc_phyctrl_auto_negotiate_get(int unit, soc_port_t port, int *an,
                               int *an_done);

extern int
soc_phyctrl_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode);

extern int
soc_phyctrl_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode);

extern int
soc_phyctrl_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode);

extern int
soc_phyctrl_loopback_set(int unit, soc_port_t port, int enable);

extern int
soc_phyctrl_loopback_get(int unit, soc_port_t port, int *enable);

extern int
soc_phyctrl_interface_set(int unit, soc_port_t port, soc_port_if_t pif);

extern int
soc_phyctrl_interface_get(int unit, soc_port_t port, soc_port_if_t *pif);

extern int
soc_phyctrl_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode);

extern int
soc_phyctrl_linkup_evt(int unit, soc_port_t port);

extern int
soc_phyctrl_linkdn_evt(int unit, soc_port_t port);

extern int
soc_phyctrl_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mdix);

extern int
soc_phyctrl_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mdix);

extern int
soc_phyctrl_mdix_status_get(int unit, soc_port_t port,
                            soc_port_mdix_status_t *status);

extern int
soc_phyctrl_medium_config_set(int unit, soc_port_t port,
                              soc_port_medium_t medium,
                              soc_phy_config_t *cfg);

extern int
soc_phyctrl_medium_config_get(int unit, soc_port_t port,
                              soc_port_medium_t medium,
                              soc_phy_config_t *cfg);

extern int
soc_phyctrl_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium);

extern int
soc_phyctrl_cable_diag(int unit, soc_port_t port,
                       soc_port_cable_diag_t *status);

extern int
soc_phyctrl_link_change(int unit, soc_port_t port, int *link);

extern int
soc_phyctrl_control_set(int unit, soc_port_t port,
                        soc_phy_control_t ctrl_type, uint32 value);

extern int
soc_phyctrl_control_get(int unit, soc_port_t port,
                        soc_phy_control_t ctrl_type, uint32 *value);

extern int
soc_phyctrl_reg_read(int unit, soc_port_t port, uint32 flags,
                     uint32 addr, uint32 *data);

extern int
soc_phyctrl_reg_write(int unit, soc_port_t port, uint32 flags,
                      uint32 addr, uint32 data);

extern int
soc_phyctrl_reg_modify(int unit, soc_port_t port, uint32 flags,
                       uint32 addr, uint32 data, uint32 mask);

extern int
soc_phyctrl_ability_advert_get(int unit, soc_port_t port,
                            soc_port_ability_t * ability);

extern int
soc_phyctrl_ability_advert_set(int unit, soc_port_t port,
                            soc_port_ability_t * ability);

extern int
soc_phyctrl_ability_remote_get(int unit, soc_port_t port,
                                   soc_port_ability_t * ability);

extern int
soc_phyctrl_ability_local_get(int unit, soc_port_t port,
                               soc_port_ability_t * ability);

extern int soc_phyctrl_ioctl(int unit,soc_port_t port,
								unsigned int cmd,void * io_req);

extern int
soc_phyctrl_detach(int unit, soc_port_t port);

extern int
soc_phyctrl_notify(int unit, soc_port_t port,
                   soc_phy_event_t event, uint32 data);

extern int
soc_phy_init(int unit);

extern int
soc_phy_info_get(int unit, soc_port_t port, soc_phy_info_t *pi);

extern uint8
soc_phy_addr_of_port(int unit, soc_port_t port);

extern uint8
soc_phy_addr_int_of_port(int unit, soc_port_t port);
#define PORT_TO_PHY_ADDR_INT(_u, _p) PHY_ADDR_INT((_u), (_p))

extern soc_port_t
soc_phy_addr_to_port(int unit, uint8 phy_id);
#define PORT_TO_PHY_ADDR(_u, _p) PHY_ADDR((_u), (_p)) 

extern uint16
soc_phy_id0reg_get(int unit, soc_port_t port);

extern uint16
soc_phy_id1reg_get(int unit, soc_port_t port);

extern int
soc_phy_is_c45_miim(int unit, soc_port_t port);

extern int
soc_phy_port_offset_get(int unit, soc_port_t port);

extern char*
soc_phy_name_get(int unit, soc_port_t port);

extern sal_usecs_t
soc_phy_an_timeout_get(int unit, soc_port_t port);

/*
 * Phy reset method overloading support
 */
typedef _shr_port_phy_reset_cb_t soc_port_phy_reset_cb_t;

extern int
soc_phy_reset_register(int                      unit,
                       soc_port_t               port,
                       soc_port_phy_reset_cb_t  callback,
                       void                    *user_data,
                       int                      reg_default);

extern int
soc_phy_reset_unregister(int                      unit,
                         soc_port_t               port,
                         soc_port_phy_reset_cb_t  callback,
                         void                    *user_data);

extern int
soc_phy_reset(int unit, soc_port_t port);

/*
 * Medium configuration and control
 */
typedef _shr_port_medium_status_cb_t soc_port_medium_status_cb_t;


extern int
soc_phy_medium_status_register(int unit,
                               soc_port_t port,
                               soc_port_medium_status_cb_t cb,
                               void  *user_data);

extern int
soc_phy_medium_status_unregister(int unit,
                                 soc_port_t port,
                                 soc_port_medium_status_cb_t cb,
                                 void  *user_data);

extern void
soc_phy_medium_status_notify(int unit, soc_port_t port,
                             soc_port_medium_t medium);

/*
 * Initialization of the features above
 */
extern int
soc_phy_common_init(int unit);

#endif /* _PHYCTRL_H_ */
