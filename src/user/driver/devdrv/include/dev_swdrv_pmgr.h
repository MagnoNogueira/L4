#ifndef _DEV_SWDRV_PMGR_H_
#define _DEV_SWDRV_PMGR_H_
#include "sys_type.h"
#include "dev_swdrv.h"

BOOL_T
DEV_SWDRV_PMGR_Logical2PhyDevicePortID(
    UI32_T unit_id,
    UI32_T port,
    UI32_T *module_id,
    UI32_T *device_id,
    UI32_T *phy_port
);

#endif /* #ifndef _DEV_SWDRV_PMGR_H_ */
