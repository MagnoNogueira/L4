#include "stktplg_om.h"

BOOL_T local_unit_state[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK] = {TRUE};

BOOL_T STKPLG_OM_PRIV_SetUnitState(UI32_T unit, BOOL_T is_enable)
{
    if (unit < 1 || SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK+1 < unit)
    {
        return FALSE;
    }

    local_unit_state[unit - 1] = is_enable;
    return TRUE;
}

/* FUNCTION NAME : STKTPLG_OM_GetMaxPortNumberOnBoard
 * PURPOSE: To get the maximum port number on board .
 * INPUT:   unit             -- unit id.
 * OUTPUT:  *max_port_number -- maximum port number on board.
 * RETUEN:  TRUE         -- successful.
 *          FALSE        -- unspecified failure.
 * NOTES:   0           -- get local device.
 *          otherwise   -- get this device.
 */
BOOL_T STKTPLG_OM_GetMaxPortNumberOnBoard(UI8_T unit, UI32_T *max_port_number)
{
    if(unit <1 || unit >SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK)
     return FALSE;

    *max_port_number = SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD;

    return (TRUE);
}

/* FUNCTION NAME : STKTPLG_OM_GetLocalUnitBaseMac
 * PURPOSE: To get the base address of the local unit.
 * INPUT:   unit_id         -- unit id.
 * OUTPUT:  base_mac_addr   -- base mac address (6 bytes) of this unit.
 * RETUEN:  TRUE            -- successful.
 *          FALSE           -- unspecified failure.
 * NOTES:
 *
 */
BOOL_T STKTPLG_OM_GetLocalUnitBaseMac(UI8_T *base_mac_addr)
{
    base_mac_addr[0] = 0x00;
    base_mac_addr[1] = 0x11;
    base_mac_addr[2] = 0x22;
    base_mac_addr[3] = 0x33;
    base_mac_addr[4] = 0x44;
    base_mac_addr[5] = 0x55;
    return TRUE;
}

/* FUNCTION NAME : STKTPLG_OM_PortExist
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
BOOL_T STKTPLG_OM_PortExist(UI32_T unit_id, UI32_T port_id)
{
    if (TRUE != STKTPLG_OM_UnitExist(unit_id))
    {
        return FALSE;
    }

    if (48 < port_id)
    {
        return FALSE;
    }

    return TRUE;
}

 /*--------------------------------------------------------------------------
 * ROUTINE NAME - STKTPLG_OM_IsLocalUnit
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
BOOL_T   STKTPLG_OM_IsLocalUnit(UI32_T unit,UI32_T port,UI32_T *ext_unit_id)
{
    UI32_T  my_unit_id;
    UI32_T  module_x,max_port_number;

    if(unit <1 || unit >SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK)
     return FALSE;

    STKTPLG_OM_GetMyUnitID(&my_unit_id);
    STKTPLG_OM_GetMaxPortNumberOnBoard(unit, &max_port_number);

    if(port>max_port_number)
    {
        /*
         * if((port %SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT_ON_BOARD)!=0)
         *module_x=port/SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT_ON_BOARD;
         */
         module_x=1;
         *ext_unit_id= module_x*SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK + unit ;
         return(FALSE );

    }
    else if(unit!=my_unit_id)
    {
        *ext_unit_id=unit;
        return( FALSE );
    }
    else
    {
        *ext_unit_id=unit;
        return( TRUE );
    }
}

/* FUNCTION NAME: STKTPLG_OM_UnitExist
 * PURPOSE: This function is used to check if the specified unit is
 *          existing or not.
 * INPUT:   unit_id  -- unit id
 * OUTPUT:  None.
 * RETUEN:  TRUE   -- exist
 *          FALSE  -- not exist
 * NOTES:   Use got mac address of each unit to
 *          know if unit exist or not.
 *
 */
BOOL_T STKTPLG_OM_UnitExist(UI32_T unit_id)
{
    if(unit_id <1 || unit_id >SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK)
     return FALSE;

    return local_unit_state[unit_id - 1];
//    return TRUE;
}

/* FUNCTION NAME : STKTPLG_OM_GetMyUnitID
 * PURPOSE: To get the unit id of myself.
 * INPUT:   None.
 * OUTPUT:  my_unit_id   -- the unit id of myself.
 * RETUEN:  TRUE         -- master/slave.
 *          FALSE        -- transition state.
 * NOTES:
 *
 */
BOOL_T STKTPLG_OM_GetMyUnitID(UI32_T *my_unit_id)
{
    *my_unit_id = 1;
    return TRUE;
}

/* FUNCTION NAME : STKTPLG_OM_GetMyDriverUnit
 * PURPOSE  : To get my driver unit ID.
 * INPUT    : None.
 * OUTPUT   : *my_driver_unit -- my driver ID.
 * RETUEN   : TRUE  -- exist.
 *            FALSE -- not exist.
 */
BOOL_T STKTPLG_OM_GetMyDriverUnit(UI32_T *my_driver_unit)
{
    UI32_T my_unit_id;

    if (FALSE == STKTPLG_OM_GetMyUnitID(&my_unit_id))
    {
        return FALSE;
    }

    *my_driver_unit = my_unit_id;
    return TRUE;
}

/* -------------------------------------------------------------------------
 * ROUTINE NAME - STKTPLG_OM_GetNextUnit
 * -------------------------------------------------------------------------
 * FUNCTION: Get Next Unit ID.
 * INPUT   : unit_id       -- Which Unit
 * OUTPUT  : unit_id       -- Next Unit ID
 * RETURN  : TRUE/FALSE
 * NOTE    :
 * -------------------------------------------------------------------------*/
BOOL_T STKTPLG_OM_GetNextUnit(UI32_T *unit_id)
{
    if (*unit_id == 0)
    {
        *unit_id = 1;
        return TRUE;
    }

    for ((*unit_id) ++; *unit_id < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; (*unit_id)++)
    {
        if (local_unit_state[(*unit_id) - 1])
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL_T STKTPLG_OM_GetStackingPortPhyDevPortId(UI32_T n, UI32_T *dev_id, UI32_T *up_port)
{
    return TRUE;
}

