#ifndef __STKTPLG_OM__
#define __STKTPLG_OM__


/* INCLUDE FILE DECLARATIONS
 */
#include "sys_cpnt.h"
#include "sys_adpt.h"

#define STKTPLG_OM_UPORT_TO_IFINDEX(unit, port)     ( ((unit)-1) * SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT + (port) )
#define STKTPLG_OM_IFINDEX_TO_UNIT(ifindex)         ( ((UI32_T)(((ifindex)-1)/SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT))+1 )
#define STKTPLG_OM_IFINDEX_TO_PORT(ifindex)         ( (ifindex) - (STKTPLG_OM_IFINDEX_TO_UNIT(ifindex)-1)*SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT )

#define STKTPLG_OM_TRUNKID_TO_IFINDEX(trunk_id)     ( (trunk_id) + SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER - 1 )
#define STKTPLG_OM_IFINDEX_TO_TRUNKID(ifindex)      ( (ifindex) - SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER + 1 )

enum
{
    STKTPLG_TYPE_STACKING_PORT_UP_LINK,
    STKTPLG_TYPE_STACKING_PORT_DOWN_LINK
};

BOOL_T STKTPLG_OM_GetStackingPortPhyDevPortId(UI32_T n, UI32_T *dev_id, UI32_T *up_port);

BOOL_T STKPLG_OM_PRIV_SetUnitState(UI32_T unit, BOOL_T is_enable);

/* FUNCTION NAME : STKTPLG_OM_GetMaxPortNumberOnBoard
 * PURPOSE: To get the maximum port number on board .
 * INPUT:   unit             -- unit id. 
 * OUTPUT:  *max_port_number -- maximum port number on board.
 * RETUEN:  TRUE         -- successful.
 *          FALSE        -- unspecified failure.
 * NOTES:   0           -- get local device.
 *          otherwise   -- get this device.
 */
BOOL_T STKTPLG_OM_GetMaxPortNumberOnBoard(UI8_T unit, UI32_T *max_port_number);

/* FUNCTION NAME : STKTPLG_OM_GetLocalUnitBaseMac
 * PURPOSE: To get the base address of the local unit.
 * INPUT:   unit_id         -- unit id.
 * OUTPUT:  base_mac_addr   -- base mac address (6 bytes) of this unit.
 * RETUEN:  TRUE            -- successful.
 *          FALSE           -- unspecified failure.
 * NOTES:
 *          
 */
BOOL_T STKTPLG_OM_GetLocalUnitBaseMac(UI8_T *base_mac_addr);

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
BOOL_T STKTPLG_OM_IsLocalUnit(UI32_T unit,UI32_T port,UI32_T *ext_unit_id);

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
BOOL_T STKTPLG_OM_UnitExist(UI32_T unit_id);

/* FUNCTION NAME : STKTPLG_OM_GetMyUnitID
 * PURPOSE: To get the unit id of myself.
 * INPUT:   None.
 * OUTPUT:  my_unit_id   -- the unit id of myself.
 * RETUEN:  TRUE         -- master/slave.
 *          FALSE        -- transition state.
 * NOTES:
 *          
 */
BOOL_T STKTPLG_OM_GetMyUnitID(UI32_T *my_unit_id);

/* FUNCTION NAME : STKTPLG_OM_GetMyDriverUnit
 * PURPOSE  : To get my driver unit ID.
 * INPUT    : None.
 * OUTPUT   : *my_driver_unit -- my driver ID.
 * RETUEN   : TRUE  -- exist.
 *            FALSE -- not exist.
 */
BOOL_T STKTPLG_OM_GetMyDriverUnit(UI32_T *my_driver_unit);

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
BOOL_T STKTPLG_OM_PortExist(UI32_T unit_id, UI32_T port_id);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - STKTPLG_OM_GetNextUnit
 * -------------------------------------------------------------------------
 * FUNCTION: Get Next Unit ID.
 * INPUT   : unit_id       -- Which Unit
 * OUTPUT  : unit_id       -- Next Unit ID
 * RETURN  : TRUE/FALSE
 * NOTE    :
 * -------------------------------------------------------------------------*/
BOOL_T STKTPLG_OM_GetNextUnit(UI32_T *unit_id);

#endif /* #ifndef __STKTPLG_OM__ */
