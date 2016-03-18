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
 * $Revision: 27486 $
 * $Date: 2012-03-26 08:53:40 +0800 (Mon, 26 Mar 2012) $
 *
 * Purpose : Definition those public LED APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) LED parameter settings
 */

#ifndef __RTK_LED_H__
#define __RTK_LED_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
/* system led type */
typedef enum rtk_led_type_e
{
    LED_TYPE_SYS = 0,
    LED_TYPE_ALARM,
    RTK_LED_TYPE_END
} rtk_led_type_t;

typedef enum rtk_led_swCtrl_mode_e
{
    RTK_LED_SWCTRL_MODE_OFF,
    RTK_LED_SWCTRL_MODE_BLINKING_32MS,
    RTK_LED_SWCTRL_MODE_BLINKING_64MS,
    RTK_LED_SWCTRL_MODE_BLINKING_128MS,
    RTK_LED_SWCTRL_MODE_BLINKING_256MS,
    RTK_LED_SWCTRL_MODE_BLINKING_512MS,
    RTK_LED_SWCTRL_MODE_BLINKING_1024MS,
    RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE,
    RTK_LED_SWCTRL_MODE_BLINKING_END
} rtk_led_swCtrl_mode_t;

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_led_init
 * Description:
 *      Initialize led module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8389, 8328, 8390, 8380
 * Note:
 *      Must initialize led module before calling any led APIs.
 */
extern int32
rtk_led_init(uint32 unit);

/* Function Name:
 *      rtk_led_portEnable_get
 * Description:
 *      Get led status on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the led status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8389
 * Note:
 *      None
 */
extern int32
rtk_led_portEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_led_portEnable_set
 * Description:
 *      Set led status on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - led status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8389
 * Note:
 *      None
 */
extern int32
rtk_led_portEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_led_sysEnable_get
 * Description:
 *      Get led status of specified LED type.
 * Input:
 *      unit    - unit id
 *      type    - system led type
 * Output:
 *      pEnable - pointer to the led status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8389, 8328, 8390, 8380
 * Note:
 *      The type of system LED as following:
 *      - LED_TYPE_SYS
 *      - LED_TYPE_ALARM
 */
extern int32
rtk_led_sysEnable_get(uint32 unit, rtk_led_type_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_led_sysEnable_set
 * Description:
 *      Set led status of specified LED type.
 * Input:
 *      unit   - unit id
 *      type   - system led type
 *      enable - led status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8389, 8328, 8390, 8380
 * Note:
 *      The type of system LED as following:
 *      - LED_TYPE_SYS
 *      - LED_TYPE_ALARM
 */
extern int32
rtk_led_sysEnable_set(uint32 unit, rtk_led_type_t type, rtk_enable_t enable);

/* Function Name:
 *      rtk_led_portLedEntitySwCtrlEnable_get
 * Description:
 *      Get LED software control status on specified LED entity and port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      entity - LED entity id
 * Output:
 *      pEnable - software control mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 8380
 * Note:
 *      (1) Three LED entities (LED0/LED1/LED2) are supported per port. Each LED entity can be switched
 *          to software control mode individually.
 *      (2) rtk_led_portLedEntitySwCtrlEnable_set/rtk_led_portLedEntitySwCtrlMode_set doesn't take effect
 *          unless 'rtk_led_swCtrl_start' is invoked.
 */
extern int32
rtk_led_portLedEntitySwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    uint32          entity,
    rtk_enable_t    *pEnable);

/* Function Name:
 *      rtk_led_portLedEntitySwCtrlEnable_set
 * Description:
 *      Set LED software control status on specified LED entity and port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      entity - LED entity id
 *      enable - software control mode status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 8380
 * Note:
 *      (1) Three LED entities (LED0/LED1/LED2) are supported per port. Each LED entity can be switched
 *          to software control mode individually.
 *      (2) rtk_led_portLedEntitySwCtrlEnable_set/rtk_led_portLedEntitySwCtrlMode_set doesn't take effect
 *          unless 'rtk_led_swCtrl_start' is invoked.
 */
extern int32
rtk_led_portLedEntitySwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    uint32          entity,
    rtk_enable_t    enable);

/* Function Name:
 *      rtk_led_portLedEntitySwCtrlMode_get
 * Description:
 *      Get display mode of software control LED on specified port, LED entity and media.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      entity - LED entity id
 *      media  - media type
 * Output:
 *      pMode  - LED display mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 8380
 * Note:
 *      (1) Media type only supports PORT_MEDIA_COPPER and PORT_MEDIA_FIBER.
 *      (2) System software control mode only support:
 *          - RTK_LED_SWCTRL_MODE_OFF,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_32MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_64MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_128MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_256MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_512MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_1024MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE
 *      (3) rtk_led_portLedEntitySwCtrlEnable_set/rtk_led_portLedEntitySwCtrlMode_set doesn't take effect
 *          unless 'rtk_led_swCtrl_start' is invoked.
 */
extern int32
rtk_led_portLedEntitySwCtrlMode_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  entity,
    rtk_port_media_t        media,
    rtk_led_swCtrl_mode_t   *pMode);

/* Function Name:
 *      rtk_led_portLedEntitySwCtrlMode_set
 * Description:
 *      Set display mode of software control LED on specified port, LED entity and media.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      entity - LED entity id
 *      media  - media type
 *      mode   - LED display mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 8380
 * Note:
 *      (1) Media type only supports PORT_MEDIA_COPPER and PORT_MEDIA_FIBER.
 *      (2) System software control mode only support:
 *          - RTK_LED_SWCTRL_MODE_OFF,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_32MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_64MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_128MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_256MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_512MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_1024MS,
 *          - RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE
 *      (3) rtk_led_portLedEntitySwCtrlEnable_set/rtk_led_portLedEntitySwCtrlMode_set doesn't take effect
 *          unless 'rtk_led_swCtrl_start' is invoked.
 */
extern int32
rtk_led_portLedEntitySwCtrlMode_set(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  entity,
    rtk_port_media_t        media,
    rtk_led_swCtrl_mode_t   mode);

/* Function Name:
 *      rtk_led_swCtrl_start
 * Description:
 *      Apply LED software control configuration.
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8390
 * Note:
 *      The API must be called for software control LED "Enable/Mode" configurations to take effect.
 */
extern int32
rtk_led_swCtrl_start(uint32 unit);

/* Function Name:
 *      rtk_led_sysMode_get
 * Description:
 *      Get system LED display mode.
 * Input:
 *      unit  - unit id
 * Output:
 *      pMode - LED display mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 8380
 * Note:
 *      System software control mode only support:
 *      - RTK_LED_SWCTRL_MODE_OFF
 *      - RTK_LED_SWCTRL_MODE_BLINKING_512MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_1024MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE
 */
extern int32
rtk_led_sysMode_get(uint32 unit, rtk_led_swCtrl_mode_t *pMode);

/* Function Name:
 *      rtk_led_sysMode_set
 * Description:
 *      Set system LED display mode.
 * Input:
 *      unit - unit id
 *      mode - LED display mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 8380
 * Note:
 *      System software control mode only support:
 *      - RTK_LED_SWCTRL_MODE_OFF
 *      - RTK_LED_SWCTRL_MODE_BLINKING_64MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_1024MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE
 */
extern int32
rtk_led_sysMode_set(uint32 unit, rtk_led_swCtrl_mode_t mode);

#endif /* __RTK_LED_H__ */
