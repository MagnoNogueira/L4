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
 * $Revision: 39383 $
 * $Date: 2013-05-13 15:51:17 +0800 (Mon, 13 May 2013) $
 *
 * Purpose : Definition those public global APIs and its data type in the SDK.
 *
 * Feature :  Parameter settings for the system-wise view
 *
 */
#ifndef __DAL_CYPRESS_COMMON_H__
#define __DAL_CYPRESS_COMMON_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/trap.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef enum rtk_q_empty_e
{
    Q_EMPTY_SCHED_ALGO = 0,
    Q_EMPTY_RATE,
    Q_EMPTY_END,
} rtk_q_empty_t;

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_cypress_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
extern int32
dal_cypress_common_init(uint32 unit);

/* Function Name:
 *      dal_cypress_common_portEgrQueueEmpty_start
 * Description:
 *      Empty egress queues of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID               - Invalid port id
 * Note:
 *      After using the API, dal_cypress_common_portEgrQueueEmpty_end must 
 *      be called to free semaphore.
 */
extern int32
dal_cypress_common_portEgrQueueEmpty_start(
    uint32          unit, 
    rtk_port_t      port, 
    uint32          *pState, 
    uint32          *pRate, 
    rtk_trap_oam_action_t *pTrap_action, 
    rtk_action_t    *pAction);

/* Function Name:
 *      dal_cypress_common_portEgrQueueEmpty_end
 * Description:
 *      Restore egress queues of specified port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID               - Invalid port id
 * Note:
 *      Before using the API, dal_cypress_common_portEgrQueueEmpty_start must 
 *      be called to lock semaphore.
 */
extern int32
dal_cypress_common_portEgrQueueEmpty_end(
    uint32          unit, 
    rtk_port_t      port, 
    rtk_q_empty_t   type, 
    uint32          state, 
    uint32          rate, 
    rtk_trap_oam_action_t trap_action, 
    rtk_action_t    action);

#endif /* __DAL_CYPRESS_COMMON_H__ */
