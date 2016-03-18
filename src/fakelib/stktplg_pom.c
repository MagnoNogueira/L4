#include "sys_adpt.h"
#include "stktplg_pom.h"

/* FUNCTION NAME : STKTPLG_POM_GetLocalUnitBaseMac
 * PURPOSE: To get the base address of the local unit.
 * INPUT:   unit_id         -- unit id.
 * OUTPUT:  base_mac_addr   -- base mac address (6 bytes) of this unit.
 * RETUEN:  TRUE            -- successful.
 *          FALSE           -- unspecified failure.
 * NOTES:
 *
 */
BOOL_T STKTPLG_POM_GetLocalUnitBaseMac(UI8_T *base_mac_addr)
{
    return STKTPLG_OM_GetLocalUnitBaseMac(base_mac_addr);
}

/* FUNCTION NAME : STKTPLG_POM_PortExist
 * PURPOSE: This function is used to check if the specified port is
 *          existing or not.
 * INPUT:   logical_unit_id -- Logical unit ID
 *          logical_port_id -- logical port ID
 * OUTPUT:  None.
 * RETUEN:  TRUE         -- successful.
 *          FALSE        -- unspecified failure.
 * NOTES:
 *
 */
BOOL_T STKTPLG_POM_PortExist(UI32_T unit_id, UI32_T port_id)
{
    return STKTPLG_OM_PortExist(unit_id, port_id);
}

 /*--------------------------------------------------------------------------
 * ROUTINE NAME - STKTPLG_POM_IsLocalUnit
 *---------------------------------------------------------------------------
 * PURPOSE:  to  the port of  the specify unit  is belong to local or remote unit
 * INPUT:    unit :  destination unit
 *           port :
 * OUTPUT:   ext_unit_id  :  unit id
 * RETURN:   TRUE   : is local
 *           FALSE   : is remote
 * NOTE:     this function is for (unit,port)
 *           x*n+y :
 *           its meaning the option module x  of unit #y
 *           n: SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK
 *           when x=0 , y  is mainborad

 *
 *----------------------------------------------------------------
 */
BOOL_T STKTPLG_POM_IsLocalUnit(UI32_T unit,UI32_T port,UI32_T *ext_unit_id)
{
    return STKTPLG_OM_IsLocalUnit(unit,port,ext_unit_id);
}

/* FUNCTION NAME : STKTPLG_POM_GetMyDriverUnit
 * PURPOSE  : To get my driver unit ID.
 * INPUT    : None.
 * OUTPUT   : *my_driver_unit -- my driver ID.
 * RETUEN   : TRUE  -- exist.
 *            FALSE -- not exist.
 */
BOOL_T STKTPLG_POM_GetMyDriverUnit(UI32_T *my_driver_unit)
{
    return STKTPLG_OM_GetMyDriverUnit(my_driver_unit);
}

/* FUNCTION NAME : STKTPLG_POM_GetMyUnitID
 * PURPOSE: To get the unit id of myself.
 * INPUT:   None.
 * OUTPUT:  my_unit_id   -- the unit id of myself.
 * RETUEN:  TRUE         -- master/slave.
 *          FALSE        -- transition state.
 * NOTES:
 *
 */
BOOL_T STKTPLG_POM_GetMyUnitID(UI32_T *my_unit_id)
{
    return STKTPLG_OM_GetMyUnitID(my_unit_id);
}

/* -------------------------------------------------------------------------
 * ROUTINE NAME - STKTPLG_POM_GetNextUnit
 * -------------------------------------------------------------------------
 * FUNCTION: Get Next Unit ID.
 * INPUT   : unit_id       -- Which Unit
 * OUTPUT  : unit_id       -- Next Unit ID
 * RETURN  : TRUE/FALSE
 * NOTE    :
 * -------------------------------------------------------------------------*/
BOOL_T STKTPLG_POM_GetNextUnit(UI32_T *unit_id)
{
    return STKTPLG_OM_GetNextUnit(unit_id);
}

/* FUNCTION NAME : STKTPLG_POM_GetMaxPortNumberOnBoard
 * PURPOSE: To get the maximum port number on board .
 * INPUT:   unit             -- unit id.
 * OUTPUT:  *max_port_number -- maximum port number on board.
 * RETUEN:  TRUE         -- successful.
 *          FALSE        -- unspecified failure.
 * NOTES:   0           -- get local device.
 *          otherwise   -- get this device.
 */
BOOL_T STKTPLG_POM_GetMaxPortNumberOnBoard(UI8_T unit, UI32_T *max_port_number)
{
    return STKTPLG_OM_GetMaxPortNumberOnBoard(unit, max_port_number);
}

BOOL_T STKTPLG_POM_GetNextDriverUnit(UI32_T *driver_unit)
{
    return STKTPLG_OM_GetNextUnit(driver_unit);;
}



