#ifndef _STKTPLG_POM_
#define _STKTPLG_POM_

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_cpnt.h"
#include "stktplg_om.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */
#define STKTPLG_POM_UPORT_TO_IFINDEX STKTPLG_OM_UPORT_TO_IFINDEX
#define STKTPLG_POM_IFINDEX_TO_UNIT  STKTPLG_OM_IFINDEX_TO_UNIT
#define STKTPLG_POM_IFINDEX_TO_PORT(ifindex) ( (ifindex) - (STKTPLG_POM_IFINDEX_TO_UNIT(ifindex)-1)*SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT )
#define STKTPLG_POM_IFINDEX_TO_DRIVERUNIT(ifindex)   ( STKTPLG_POM_IsModulePort(STKTPLG_POM_IFINDEX_TO_UNIT(ifindex),STKTPLG_POM_IFINDEX_TO_PORT(ifindex))?STKTPLG_POM_IFINDEX_TO_UNIT(ifindex)+SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK:STKTPLG_POM_IFINDEX_TO_UNIT(ifindex))

#define STKTPLG_POM_TRUNKID_TO_IFINDEX STKTPLG_OM_TRUNKID_TO_IFINDEX
#define STKTPLG_POM_IFINDEX_TO_TRUNKID STKTPLG_OM_IFINDEX_TO_TRUNKID

/* FUNCTION NAME : STKTPLG_POM_GetMaxPortNumberOnBoard
 * PURPOSE: To get the maximum port number on board .
 * INPUT:   unit             -- unit id.
 * OUTPUT:  *max_port_number -- maximum port number on board.
 * RETUEN:  TRUE         -- successful.
 *          FALSE        -- unspecified failure.
 * NOTES:   0           -- get local device.
 *          otherwise   -- get this device.
 */
BOOL_T STKTPLG_POM_GetMaxPortNumberOnBoard(UI8_T unit, UI32_T *max_port_number);

/* FUNCTION NAME : STKTPLG_POM_GetMyUnitID
 * PURPOSE: To get the unit id of myself.
 * INPUT:   None.
 * OUTPUT:  my_unit_id   -- the unit id of myself.
 * RETUEN:  TRUE         -- master/slave.
 *          FALSE        -- transition state.
 * NOTES:
 *
 */
BOOL_T STKTPLG_POM_GetMyUnitID(UI32_T *my_unit_id);

/* FUNCTION NAME : STKTPLG_POM_GetLocalUnitBaseMac
 * PURPOSE: To get the base address of the local unit.
 * INPUT:   unit_id         -- unit id.
 * OUTPUT:  base_mac_addr   -- base mac address (6 bytes) of this unit.
 * RETUEN:  TRUE            -- successful.
 *          FALSE           -- unspecified failure.
 * NOTES:
 *
 */
BOOL_T STKTPLG_POM_GetLocalUnitBaseMac(UI8_T *base_mac_addr);

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
BOOL_T STKTPLG_POM_PortExist(UI32_T unit_id, UI32_T port_id);

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
BOOL_T   STKTPLG_POM_IsLocalUnit(UI32_T unit,UI32_T port,UI32_T *ext_unit_id);

/* FUNCTION NAME : STKTPLG_POM_GetMyDriverUnit
 * PURPOSE  : To get my driver unit ID.
 * INPUT    : None.
 * OUTPUT   : *my_driver_unit -- my driver ID.
 * RETUEN   : TRUE  -- exist.
 *            FALSE -- not exist.
 */
BOOL_T STKTPLG_POM_GetMyDriverUnit(UI32_T *my_driver_unit);

/* FUNCTION NAME : STKTPLG_POM_GetNextDriverUnit
 * PURPOSE  : To get next driver unit.
 * INPUT    : Next to which driver unit.
 * OUTPUT   : Next driver is which.
 * RETUEN   : TRUE/FALSE
 */
BOOL_T STKTPLG_POM_GetNextDriverUnit(UI32_T *driver_unit);


/* -------------------------------------------------------------------------
 * ROUTINE NAME - STKTPLG_POM_GetNextUnit
 * -------------------------------------------------------------------------
 * FUNCTION: Get Next Unit ID.
 * INPUT   : unit_id       -- Which Unit
 * OUTPUT  : unit_id       -- Next Unit ID
 * RETURN  : TRUE/FALSE
 * NOTE    :
 * -------------------------------------------------------------------------*/
BOOL_T STKTPLG_POM_GetNextUnit(UI32_T *unit_id);

#endif /* #ifndef _STKTPLG_POM_ */
