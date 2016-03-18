/*
 *  swctrl_pom.h
 *  l4_mgr
 *
 *  Created by yehjunying on 2011/6/17.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef SWCTRL_POM_H
#define SWCTRL_POM_H

#include "swctrl.h"

/*------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_POM_UIGetUnitPortNumber
 *------------------------------------------------------------------------
 * FUNCTION: This API is used to get the number of some unit that caller
 *           want to get and should only be used by  CLI.
 * INPUT   : unit --- which unit caller want to get.
 * OUTPUT  : None.
 * RETURN  : The port number of that unit.
 *           > 0 --- 1) Base on the unit MAC address table given by CLI
 *                      before provision complete.
 *                   2) Output normal ifindex after provision complete.
 *           0   --- Get fail.
 *                   1) Not in master mode.
 *                   2) Argument is invalid.
 *                   3) This unit is not present.
 * NOTE    : Return port number base on the table given by CLI before provision complete.
 *           Return port number normally after provision complete.
 *------------------------------------------------------------------------*/
UI32_T SWCTRL_POM_UIGetUnitPortNumber(UI32_T unit);

/*------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_POM_UIUserPortToIfindex
 *------------------------------------------------------------------------
 * FUNCTION: This API is used to get ifindex of some unit and some port that
 *           caller want to get and should only be used by  CLI.
 * INPUT   : unit --- which unit caller want to get.
 *           port --- which port caller want to get.
 * OUTPUT  : ifindex --- 1) Base on the unit MAC address table given by CLI
 *                          before provision complete.
 *                       2) Output normal ifindex after provision complete.
 *           is_inherit- 1) TRUE  UI should inherit this ifindex's config to provision
 *                       2) FALSE UI should not inherit this ifindex's config to provision
 * RETURN  : SWCTRL_LPORT_UNKNOWN_PORT      --- 1) Not in master mode.
 *                                              2) Argument is invalid.
 *                                              3) This port is not present.
 *           SWCTRL_LPORT_NORMAL_PORT       --- This is a normal port.
 *           SWCTRL_LPORT_TRUNK_PORT_MEMBER --- This port is a member of a trunk.
 * NOTE    : Output "ifindex" base on the table given by CLI before provision complete.
             Output "ifindex" normally after provision complete.
 *------------------------------------------------------------------------*/
SWCTRL_Lport_Type_T SWCTRL_POM_UIUserPortToIfindex (UI32_T unit, UI32_T port, UI32_T *ifindex, BOOL_T *is_inherit);

/*------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_POM_UIUserPortExisting
 *------------------------------------------------------------------------
 * FUNCTION: This API is used to check if a user port exists or not, and
 *           should only be called by CLI.
 * INPUT   : unit --- which unit caller want to get.
 *           port --- which port caller want to get.
 * OUTPUT  : None.
 * RETURN  : TRUE  --- This user port exist.
 *           FALSE --- This user port does not exist.
 * NOTE    : Return TRUE/FALSE base on the table given by CLI before provision complete.
 *           Return TRUE/FALSE normally after provision complete.
 *------------------------------------------------------------------------*/
BOOL_T SWCTRL_POM_UIUserPortExisting (UI32_T unit, UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_POM_LogicalPortExisting
 * -------------------------------------------------------------------------
 * FUNCTION: This function will return if this port is existing
 * INPUT   : l_port -- the key to ask
 * OUTPUT  : None
 * RETURN  : TRUE: Existing, FALSE: Not existing
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T SWCTRL_POM_LogicalPortExisting(UI32_T l_port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_POM_LogicalPortToUserPort
 * -------------------------------------------------------------------------
 * FUNCTION: This function will get a user port mapping from a logical port
 * INPUT   : ifindex  -- which port to map
 * OUTPUT  : unit     -- the unit
 *           port     -- the user port
 *           trunk_id -- trunk ID if it is a trunk port
 * RETURN  : One of SWCTRL_Lport_Type_T
 * NOTE    : None
 * -------------------------------------------------------------------------*/
SWCTRL_Lport_Type_T SWCTRL_POM_LogicalPortToUserPort(UI32_T ifindex,
                                                 UI32_T *unit,
                                                 UI32_T *port,
                                                 UI32_T *trunk_id);

/*------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_POM_UIGetUnitPortNumber
 *------------------------------------------------------------------------
 * FUNCTION: This API is used to get the number of some unit that caller
 *           want to get and should only be used by  CLI.
 * INPUT   : unit --- which unit caller want to get.
 * OUTPUT  : None.
 * RETURN  : The port number of that unit.
 *           > 0 --- 1) Base on the unit MAC address table given by CLI
 *                      before provision complete.
 *                   2) Output normal ifindex after provision complete.
 *           0   --- Get fail.
 *                   1) Not in master mode.
 *                   2) Argument is invalid.
 *                   3) This unit is not present.
 * NOTE    : Return port number base on the table given by CLI before provision complete.
 *           Return port number normally after provision complete.
 *------------------------------------------------------------------------*/
UI32_T SWCTRL_POM_UIGetUnitPortNumber(UI32_T unit);


#endif // SWCTRL_POM_H
