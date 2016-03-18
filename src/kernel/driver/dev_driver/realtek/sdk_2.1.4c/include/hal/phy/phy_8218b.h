/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 28540 $
 * $Date: 2012-05-03 14:59:59 +0800 (Thu, 03 May 2012) $
 *
 * Purpose : PHY 8218B/8218FB/8214FC Driver APIs.
 *
 * Feature : PHY 8218B/8218FB/8214FC Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_8218B_H__
#define __HAL_PHY_PHY_8218B_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */
#define PORT_NUM_IN_8218B    (8)
#define PORT_NUM_IN_8218FB   (8)
#define PORT_NUM_IN_8214FC   (4)

#define GIGA_LITE_CTRL_REG	 0xa10

#define CFG_MODE_CHIP_OFFSET 0x4

/*
 * Data Declaration
 */
extern rt_phydrv_t phy_8218Bdrv_ge;
extern rt_phydrv_t phy_8218FBdrv_ge;
extern rt_phydrv_t phy_8214FCdrv_ge;
extern rt_phydrv_t phy_8218FBdrv_MP_ge;
extern rt_phydrv_t phy_8214FCdrv_MP_ge;
extern rt_phydrv_t phy_8380drv_int_ge;

extern rt_phyInfo_t phy_8218FB_info;
extern rt_phyInfo_t phy_8214FC_info;

/* Function Name:
 *      phy_8218b_init
 * Description:
 *      Initialize PHY 8218b.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_init(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8214fc_init
 * Description:
 *      Initialize PHY 8214fc.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_init(uint32 unit, rtk_port_t port);

/*
 * Function Declaration
 */
/* Function Name:
 *      phy_8218fb_init
 * Description:
 *      Initialize PHY 8218fb.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_init(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8218b_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8218b_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8218b_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_speed_get(uint32 unit, rtk_port_t port, uint32 *pSpeed);

/* Function Name:
 *      phy_8218b_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
extern int32
phy_8218b_speed_set(uint32 unit, rtk_port_t port, uint32 speed);

/* Function Name:
 *      phy_8218fb_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
extern int32
phy_8218fb_speed_set(uint32 unit, rtk_port_t port, uint32 speed);

/* Function Name:
 *      phy_8214fc_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
extern int32
phy_8214fc_speed_set(uint32 unit, rtk_port_t port, uint32 speed);



/* Function Name:
 *      phy_8380_int_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8380_int_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);



/* Function Name:
 *      phy_8218b_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218b_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult);

/* Function Name:
 *      phy_8218b_rtct_start
 * Description:
 *      Start PHY interface RTCT test of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not supported
 * Note:
 *      None
 */
extern int32
phy_8218b_rtct_start(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8218b_greenEnable_get
 * Description:
 *      Get the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to status of link-up green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218 is supported the per-port link-up green feature.
 */
extern int32
phy_8218b_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218b_greenEnable_set
 * Description:
 *      Set the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-up green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218 is supported the per-port link-up green feature.
 */
extern int32
phy_8218b_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_eeeEnable_get
 * Description:
 *      Get enable status of EEE function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218b_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218b_eeeEnable_set
 * Description:
 *      Set enable status of EEE function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_crossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
extern int32
phy_8218b_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode);

/* Function Name:
 *      phy_8218_crossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
extern int32
phy_8218b_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode);

/* Function Name:
 *      phy_8218b_crossOverStatus_get
 * Description:
 *      Get cross over status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
extern int32
phy_8218b_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus);

/* Function Name:
 *      phy_8218_linkDownPowerSavingEnable_get
 * Description:
 *      Get the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of link-down power saving
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218 is supported the per-port link-down power saving
 */
extern int32
phy_8218b_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218b_linkDownPowerSavingEnable_set
 * Description:
 *      Set the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218 is supported the per-port link-down power saving
 */
extern int32
phy_8218b_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      phy_8218b_gigaLiteEnable_get
 * Description:
 *      Get the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of Giga Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218b is supported the per-port Giga Lite feature.
 */
extern int32
phy_8218b_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);


/* Function Name:
 *      phy_8218b_gigaLiteEnable_set
 * Description:
 *      Set the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8218b is supported the per-port Giga Lite feature.
 */
extern int32
phy_8218b_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_media_get
 * Description:
 *      Get PHY 8218B media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The media type is return PORT_MEDIA_COPPER.
 */
extern int32
phy_8218b_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia);

/* Function Name:
 *      phy_8218fb_media_get
 * Description:
 *      Get PHY 8218FB media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 *          - PORT_MEDIA_COPPER_AUTO
 *          - PORT_MEDIA_FIBER_AUTO
 */
extern int32
phy_8218fb_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia);

/* Function Name:
 *      phy_8218fb_media_set
 * Description:
 *      Get PHY 8218FB media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 *          - PORT_MEDIA_COPPER_AUTO
 *          - PORT_MEDIA_FIBER_AUTO
 */
extern int32
phy_8218fb_media_set(uint32 unit, rtk_port_t port, rtk_port_media_t media);

/* Function Name:
 *      phy_8214fc_media_get
 * Description:
 *      Get PHY 8214FC media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 *          - PORT_MEDIA_COPPER_AUTO
 *          - PORT_MEDIA_FIBER_AUTO
 */
extern int32
phy_8214fc_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia);

/* Function Name:
 *      phy_8214fc_media_set
 * Description:
 *      Get PHY 8214FC media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      (1) media type is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 *          - PORT_MEDIA_COPPER_AUTO
 *          - PORT_MEDIA_FIBER_AUTO
 */
extern int32
phy_8214fc_media_set(uint32 unit, rtk_port_t port, rtk_port_media_t media);

/* Function Name:
 *      phy_8218b_broadcastEnable_set
 * Description:
 *      Set enable status of broadcast mode
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - broadcast enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_broadcastEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_broadcastID_set
 * Description:
 *      Set broadcast ID
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      broadcastID   - broadcast ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_broadcastID_set(uint32 unit, rtk_port_t port, uint32 broadcastID);

/* Function Name:
 *      phy_8214fc_broadcastEnable_set
 * Description:
 *      Set enable status of broadcast mode
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - broadcast enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_broadcastEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8214fc_broadcastID_set
 * Description:
 *      Set broadcast ID
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      broadcastID   - broadcast ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_broadcastID_set(uint32 unit, rtk_port_t port, uint32 broadcastID);

/* Function Name:
 *      phy_8218fb_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8218fb_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8218b_auto_1000f_get
 * Description:
 *      Get PHY 8218B/8218FB/8214FC copper 1000f ability from shadow.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pAbility - pointer to copper 1000f ability
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. copper 1000f ability value is as following:
 *      - 0: not the ability
 *      - 1: have the ability
 */
extern int32
phy_8218b_auto_1000f_get(uint32 unit, rtk_port_t port, uint32 *pAbility);

/* Function Name:
 *      phy_8218b_auto_1000f_set
 * Description:
 *      Set PHY 8218B/8218FB/8214FC copper 1000f ability to shadow.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      ability  - copper 1000f ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. copper 1000f ability value is as following:
 *      - 0: not the ability
 *      - 1: have the ability
 */
extern int32
phy_8218b_auto_1000f_set(uint32 unit, rtk_port_t port, uint32 ability);

/* Function Name:
 *      phy_8218b_eeepEnable_get
 * Description:
 *      Get enable status of EEEP function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEEP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218b_eeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218_eeepEnable_set
 * Description:
 *      Set enable status of EEEP function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEEP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_eeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218fb_fiber_media_get
 * Description:
 *      Get PHY 8218FB fiber media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy fiber media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_AUTO
 */
extern int32
phy_8218fb_fiber_media_get(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t *pMedia);

/* Function Name:
 *      phy_8218fb_fiber_media_set
 * Description:
 *      Get PHY 8218FB fiber media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media fiber type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_AUTO
 */
extern int32
phy_8218fb_fiber_media_set(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t media);

/* Function Name:
 *      phy_8214fc_fiber_media_get
 * Description:
 *      Get PHY 8214FC fiber media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy fiber media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_AUTO
 */
extern int32
phy_8214fc_fiber_media_get(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t *pMedia);

/* Function Name:
 *      phy_8214fc_fiber_media_set
 * Description:
 *      Get PHY 8214FC fiber media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media fiber type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_AUTO
 */
extern int32
phy_8214fc_fiber_media_set(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t media);

/* Function Name:
 *      phy_8214fc_autoNegoEnable_get
 * Description:
 *      Get 8214fc autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8214fc_autoNegoEnable_set
 * Description:
 *      Set 8214FC autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8214fc_autoNegoAbility_get
 * Description:
 *      Get 8214FC ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_autoNegoAbility_get(uint32 unit, rtk_port_t port,
    rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8214fc_autoNegoAbility_set
 * Description:
 *      Set 8214FC ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_autoNegoAbility_set(uint32 unit, rtk_port_t port,
    rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8214fc_duplex_get
 * Description:
 *      Get 8214FC duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex);

/* Function Name:
 *      phy_8214fc_duplex_set
 * Description:
 *      Set 8214FC duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex);

/* Function Name:
 *      phy_8214fc_speed_get
 * Description:
 *      Get 8214FClink speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_speed_get(uint32 unit, rtk_port_t port, uint32 *pSpeed);

/* Function Name:
 *      phy_8214fc_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);
/* Function Name:
 *      phy_8214fc_MP_speed_set
 * Description:
 *      Set 8214FC MP speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
extern int32
phy_8214fc_MP_speed_set(uint32 unit, rtk_port_t port, uint32 speed);

/* Function Name:
 *      phy_8218fb_autoNegoEnable_get
 * Description:
 *      Get 8218FB autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218fb_autoNegoEnable_set
 * Description:
 *      Set 8218FB autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218fb_MP_autoNegoAbility_get
 * Description:
 *      Get 8218FB ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_MP_autoNegoAbility_get(uint32 unit, rtk_port_t port,
    rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8218fb_MP_autoNegoAbility_set
 * Description:
 *      Set 8218FB ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_MP_autoNegoAbility_set(uint32 unit, rtk_port_t port,
    rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      phy_8218fb_duplex_get
 * Description:
 *      Get 8218FB duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex);

/* Function Name:
 *      phy_8218fb_duplex_set
 * Description:
 *      Set 8218FB duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex);

/* Function Name:
 *      phy_8218fb_speed_get
 * Description:
 *      Get 8218FB MP link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_speed_get(uint32 unit, rtk_port_t port, uint32 *pSpeed);

/* Function Name:
 *      phy_8218fb_MP_speed_set
 * Description:
 *      Set 8218FB MP speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
extern int32
phy_8218fb_MP_speed_set(uint32 unit, rtk_port_t port, uint32 speed);

/* Function Name:
 *      phy_8218fb_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_int8380_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The result unit is cm
 */
extern int32
phy_int8380_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult);


/* Function Name:
 *      phy_8218_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_patch_set(uint32 unit, rtk_port_t port);


/* Function Name:
 *      phy_8214fc_mp_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_mp_patch_set(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8218fb_mp_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_mp_patch_set(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8214fc_fiberDownSpeedEnable_get
 * Description:
 *      Get fiber down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of fiber down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8214fc_fiberDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8214fc_fiberDownSpeedEnable_set
 * Description:
 *      Set fiber down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of fiber down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8214fc_fiberDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      phy_8218fb_fiberDownSpeedEnable_get
 * Description:
 *      Get fiber down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of fiber down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8218fb_fiberDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218fb_fiberDownSpeedEnable_set
 * Description:
 *      Set fiber down speed status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of fiber down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8218fb_fiberDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_utpDownSpeedEnable_get
 * Description:
 *      Get UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of fiber down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8218b_utpDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218b_utpDownSpeedEnable_set
 * Description:
 *      Set UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of UTP down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8218b_utpDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_downSpeedEnable_get
 * Description:
 *      Get UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8218b_downSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218b_downSpeedEnable_set
 * Description:
 *      Set UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
extern int32
phy_8218b_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      phy_8214fc_fiberNwayForceLink_get
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pEnable - fiber Nway force links status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_fiberNwayForceLink_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8214fc_fiberNwayForceLink_set
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - fiber Nway force links status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_fiberNwayForceLink_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      phy_8218fb_fiberNwayForceLink_get
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pEnable - fiber Nway force links status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_fiberNwayForceLink_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218fb_fiberNwayForceLink_set
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - fiber Nway force links status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_fiberNwayForceLink_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);


/* Function Name:
 *      phy_8214fc_fiberOAMLoopBack_set
 * Description:
 *      Set Fiber-Port OAM Loopback feature,
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - Fiber-Port OAM Loopback feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_fiberOAMLoopBack_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);




/* Function Name:
 *      phy_8218b_ptpSwitchMacAddr_get
 * Description:
 *      Get the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      pSwitchMacAddr - point to the Switch MAC Address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpSwitchMacAddr_get(uint32 unit, rtk_port_t port,
    rtk_mac_t *pSwitchMacAddr);

/* Function Name:
 *      phy_8218b_ptpSwitchMacAddr_set
 * Description:
 *      Set the Switch MAC address setting of PHY of the specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      pSwitchMacAddr - point to the Switch MAC Address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpSwitchMacAddr_set(uint32 unit, rtk_port_t port,
    rtk_mac_t *pSwitchMacAddr);

/* Function Name:
 *      phy_8218b_ptpRefTime_get
 * Description:
 *      Get the reference time of PHY of the specified port.
 * Input:
 *      unit       - unit id
 * Output:
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpRefTime_get(uint32 unit, rtk_port_t port,
    rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      phy_8218b_ptpRefTime_set
 * Description:
 *      Set the reference time of PHY of the specified port.
 * Input:
 *      unit      - unit id
 *      timeStamp - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpRefTime_set(uint32 unit, rtk_port_t port,
    rtk_time_timeStamp_t timeStamp);

/* Function Name:
 *      phy_8218b_ptpRefTimeAdjust_set
 * Description:
 *      Adjust the reference time of PHY of the specified port.
 * Input:
 *      unit      - unit id
 *      port    - port id
 *      sign      - significant
 *      timeStamp - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
extern int32
phy_8218b_ptpRefTimeAdjust_set(uint32 unit, rtk_port_t port,
    uint32 sign, rtk_time_timeStamp_t timeStamp);

/* Function Name:
 *      phy_8218b_ptpRefTimeEnable_get
 * Description:
 *      Get the enable state of reference time of PHY of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218b_ptpRefTimeEnable_set
 * Description:
 *      Set the enable state of reference time of PHY of the specified port.
 * Input:
 *      unit   - unit id
 *      port    - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpRefTimeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_ptpEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_8218b_ptpEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_8218b_ptpRxTimestamp_get
 * Description:
 *      Get PTP Rx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpRxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      phy_8218b_ptpTxTimestamp_get
 * Description:
 *      Get PTP Tx timstamp according to the PTP identifier on the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_8218b_ptpTxTimestamp_get(uint32 unit, rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier, rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      phy_8214fc_MP_masterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_MP_masterSlave_set(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t masterSlave);

/* Function Name:
 *      phy_8214fc_MP_masterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pMasterSlaveCfg     - pointer to the PHY master slave configuration
 *      pMasterSlaveActual  - pointer to the PHY master slave actual link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
extern int32
phy_8214fc_MP_masterSlave_get(uint32 unit, rtk_port_t port,
	rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual);

/* Function Name:
 *      phy_8218fb_MP_masterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      None
 */
extern int32
phy_8218fb_MP_masterSlave_set(uint32 unit, rtk_port_t port, rtk_port_masterSlave_t masterSlave);

/* Function Name:
 *      phy_8218fb_MP_masterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pMasterSlaveCfg     - pointer to the PHY master slave configuration
 *      pMasterSlaveActual  - pointer to the PHY master slave actual link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
extern int32
phy_8218fb_MP_masterSlave_get(uint32 unit, rtk_port_t port,
	rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual);

/* Function Name:
 *      phy_8214fc_fiberInternalLoopBack_set
 * Description:
 *      Set Fiber-Port Internal Loopback feature,
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - Fiber-Port Internal Loopback feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_8214fc_fiberInternalLoopBack_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);


#endif /* __HAL_PHY_PHY_8218_H__ */
