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
    return TRUE;
}
