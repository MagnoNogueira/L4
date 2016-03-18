#include "sys_type.h"
#include "sys_adpt.h"

/*--------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRV_Logical2PhyDevicePortID
 *---------------------------------------------------------------------------
 * PURPOSE:  mapping logical user port to phy port
 * INPUT:    unit_id, port,
 * OUTPUT:   module_id, device_id, phy_port
 * RETURN:   TRUE/FALSE
 * NOTE:
 *---------------------------------------------------------------------------
 */
BOOL_T
DEV_SWDRV_Logical2PhyDevicePortID(
    UI32_T unit_id,
    UI32_T port,
    UI32_T *module_id,
    UI32_T *device_id,
    UI32_T *phy_port)
{
    if ((unit_id < 1 || SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK < unit_id)
        || (port < 1 || SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT < port))
    {
        return FALSE;
    }

    *module_id = 0;
    *device_id = ((port-1) / SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP);
    *phy_port  = ((port-1) % SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP);

    assert(*device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT);

    return TRUE;
}

/* -------------------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRV_WorkaroundForTrunkId
 * -------------------------------------------------------------------------------------------
 * PURPOSE : Workaround for trunk id
 * INPUT   : device_id
 *           trunk_id
 *           to_chip   -- TRUE: convert to chip trunk id
 *                        FALSE: convert from chip trunk id
 * OUTPUT  : None
 * RETURN  : new trunk_id
 * NOTE    : None
 * -------------------------------------------------------------------------------------------
 */
UI32_T DEV_SWDRV_WorkaroundForTrunkId(UI32_T device_id, UI32_T trunk_id, BOOL_T to_chip)
{
    UI32_T new_trunk_id;

    new_trunk_id = to_chip ?
        ((0x20 | trunk_id) << 1) :
        ((trunk_id >> 1) & 0x1f);

    return new_trunk_id;
}


BOOL_T
DEV_SWDRV_GetModuleIdFromLocalDeviceId(
    UI32_T unit,
    UI32_T device_id,
    UI32_T *module_id)
{
    *module_id = (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT * (unit - 1)) + device_id;

    return TRUE;
}
