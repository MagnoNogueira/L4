#ifndef _SWCTRL_H_
#define _SWCTRL_H_

#include "sys_type.h"

typedef enum SWCTRL_Lport_Type_E
{
    SWCTRL_LPORT_UNKNOWN_PORT = 0,
    SWCTRL_LPORT_NORMAL_PORT,                   /* the normal port */
    SWCTRL_LPORT_TRUNK_PORT,                    /* the trunk port */
    SWCTRL_LPORT_TRUNK_PORT_MEMBER,             /* a member of a trunk */
} SWCTRL_Lport_Type_T;

/* -------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_GetCpuMac
 * -------------------------------------------------------------------------
 * FUNCTION: This function will get the MAC address of CPU
 * INPUT   : None
 * OUTPUT  : mac -- the buffer to put MAC address
 * RETURN  : TRUE: Successfully, FALSE: If not available
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T SWCTRL_GetCpuMac(UI8_T *mac);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_LogicalPortToUserPort
 * -------------------------------------------------------------------------
 * FUNCTION: This function will get a user port mapping from a logical port
 * INPUT   : ifindex  -- which port to map
 * OUTPUT  : unit     -- the unit
 *           port     -- the user port
 *           trunk_id -- trunk ID if it is a trunk port
 * RETURN  : One of SWCTRL_Lport_Type_T
 * NOTE    : None
 * -------------------------------------------------------------------------*/
#define SWCTRL_PMGR_LogicalPortToUserPort SWCTRL_LogicalPortToUserPort
SWCTRL_Lport_Type_T SWCTRL_LogicalPortToUserPort(UI32_T ifindex,
                                                 UI32_T *unit,
                                                 UI32_T *port,
                                                 UI32_T *trunk_id);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_POM_UserPortExisting
 * -------------------------------------------------------------------------
 * FUNCTION: This function will return if this user port is existing
 * INPUT   : unit -- which unit
 *           port -- the key to ask
 * OUTPUT  : None
 * RETURN  : TRUE: Existing, FALSE: Not existing
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T SWCTRL_POM_UserPortExisting(UI32_T unit,
                                   UI32_T port);

#if (SYS_CPNT_ACL_MIRROR == TRUE)
/*------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_SetAclMirrorDestPort
 *------------------------------------------------------------------------
 * FUNCTION: This function will setup dest port for  ACL-based mirror
 * INPUT   : ifindex_dest  -- which ifindex-port received mirror packets
 *           enable        -- TRUE to set, FALSE to remove
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 * NOTE    :
 *------------------------------------------------------------------------*/
BOOL_T SWCTRL_SetAclMirrorDestPort(UI32_T ifindex_dest, BOOL_T enable);
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

BOOL_T SWCTRL_LogicalPortExisting(UI32_T l_port);

#endif /* #ifndef _SWCTRL_H_ */
