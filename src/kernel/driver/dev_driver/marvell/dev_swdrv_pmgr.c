#include "sys_adpt.h"
#include "dev_swdrv_pmgr.h"

/*--------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRV_PMGR_Logical2PhyDevicePortID
 *---------------------------------------------------------------------------
 * PURPOSE:  mapping logical user port to phy port
 * INPUT:    unit_id, port,
 * OUTPUT:   module_id, device_id, phy_port
 * RETURN:   TRUE/FALSE
 * NOTE:
 *---------------------------------------------------------------------------
 */
BOOL_T
DEV_SWDRV_PMGR_Logical2PhyDevicePortID(
    UI32_T unit_id,
    UI32_T port,
    UI32_T *module_id,
    UI32_T *device_id,
    UI32_T *phy_port)
{
    return DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, module_id, device_id, phy_port);
}
