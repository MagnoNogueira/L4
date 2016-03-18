#include "sys_adpt.h"
#include "swctrl.h"


/* define the macros about the index translation
 */
#define SWCTRL_UPORT_TO_IFINDEX(unit, port)         ( ((unit)-1) * (SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + (port)  )
#define SWCTRL_IFINDEX_TO_UNIT(ifindex)             ( ((UI32_T)(((ifindex)-1)/(SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT)))+1 )
#define SWCTRL_IFINDEX_TO_PORT(ifindex)             ( (ifindex) - (SWCTRL_IFINDEX_TO_UNIT(ifindex)-1)*(SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) )


#define SWCTRL_TRUNKID_TO_IFINDEX(trunk_id)         ( (trunk_id) + (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER) - 1 )
#define SWCTRL_IFINDEX_TO_TRUNKID(ifindex)          ( (ifindex)  - (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER) + 1 )

/* The MSB of byte 0 is index 1, the LSB of byte 0 is index 8,
 * the MSB of byte 1 is index 9, the LSB of byte 1 is index 16,
 *  ...
 *  and so on.
 */
#define SWCTRL_BYTE_IN_BITMAP(INDEX)   ((int)(((INDEX)-1)/8))
#define SWCTRL_BIT_IN_BITMAP(INDEX)    (1 << (7 - (((INDEX)-1) - (SWCTRL_BYTE_IN_BITMAP((INDEX))*8))))

/* -------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_GetCpuMac
 * -------------------------------------------------------------------------
 * FUNCTION: This function will get the MAC address of CPU
 * INPUT   : None
 * OUTPUT  : mac -- the buffer to put MAC address
 * RETURN  : TRUE: Successfully, FALSE: If not available
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T SWCTRL_GetCpuMac(UI8_T *mac)
{
    mac[0] = 0x00;
    mac[1] = 0x01;
    mac[2] = 0x02;
    mac[3] = 0x03;
    mac[4] = 0x04;
    mac[5] = 0x05;
    return TRUE;
} /* End of SWCTRL_GetCpuMac() */

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
SWCTRL_Lport_Type_T SWCTRL_LogicalPortToUserPort(UI32_T ifindex,
                                                 UI32_T *unit,
                                                 UI32_T *port,
                                                 UI32_T *trunk_id)
{
    if( ifindex == 0 || ifindex > SYS_ADPT_TOTAL_NBR_OF_LPORT)
    {
        return SWCTRL_LPORT_UNKNOWN_PORT;
    }

    if( unit == 0 || port == 0 || trunk_id == 0)
    {
        return SWCTRL_LPORT_UNKNOWN_PORT;
    }

    if (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER <= ifindex)
    {
        *trunk_id = ifindex - SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER + 1;
        return SWCTRL_LPORT_TRUNK_PORT;
    }

    *unit = SWCTRL_IFINDEX_TO_UNIT(ifindex);
    *port = SWCTRL_IFINDEX_TO_PORT(ifindex);
    *trunk_id = 0;

    return SWCTRL_LPORT_NORMAL_PORT;
} /* End of SWCTRL_LogicalPortToUserPort() */


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
BOOL_T SWCTRL_SetAclMirrorDestPort(UI32_T ifindex_dest, BOOL_T enable)
{
    return TRUE;
}
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */


BOOL_T SWCTRL_LogicalPortExisting(UI32_T l_port)
{
    // if (l_port ==2) return FALSE;

    return TRUE;
}

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
UI32_T SWCTRL_POM_UIGetUnitPortNumber(UI32_T unit)
{
    return SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT;
}

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
BOOL_T SWCTRL_POM_UserPortExisting(UI32_T unit, UI32_T port)
{
    UI32_T ifindex;

    ifindex = SWCTRL_UPORT_TO_IFINDEX(unit, port);

    if (ifindex == 0 || ifindex > SYS_ADPT_TOTAL_NBR_OF_LPORT)
    {
        return FALSE;
    }

    return TRUE;
}

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
SWCTRL_Lport_Type_T SWCTRL_POM_UIUserPortToIfindex (UI32_T unit,
                                                    UI32_T port,
                                                    UI32_T *ifindex,
                                                    BOOL_T *is_inherit)
{
    *ifindex = SWCTRL_UPORT_TO_IFINDEX(unit, port);

    if (*ifindex == 0 || *ifindex > SYS_ADPT_TOTAL_NBR_OF_LPORT)
    {
        return SWCTRL_LPORT_UNKNOWN_PORT;
    }

    return SWCTRL_LPORT_NORMAL_PORT;
}

/* -------------------------------------------------------------------------
 * ROUTINE NAME - SWCTRL_POM_LogicalPortExisting
 * -------------------------------------------------------------------------
 * FUNCTION: This function will return if this port is existing
 * INPUT   : l_port -- the key to ask
 * OUTPUT  : None
 * RETURN  : TRUE: Existing, FALSE: Not existing
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T SWCTRL_POM_LogicalPortExisting(UI32_T lport)
{
    return TRUE;
}

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
BOOL_T SWCTRL_POM_UIUserPortExisting (UI32_T unit, UI32_T port)
{
    return TRUE;
}

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
                                                     UI32_T *trunk_id)
{
    return SWCTRL_LogicalPortToUserPort(ifindex, unit, port, trunk_id);
}



