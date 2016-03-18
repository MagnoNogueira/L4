/*-----------------------------------------------------------------------------
 * Module   : dai_mgr.c
 *-----------------------------------------------------------------------------
 * PURPOSE  : Provide APIs to access DAI.
 *-----------------------------------------------------------------------------
 * NOTES    : Dynamic ARP Inspection (DAI) is a security feature that
 *            validates Address Resolution Protocol (ARP) packets in a
 *            network. DAI allows a network administrator to intercept,
 *            log, and discard ARP packets with invalid MAC address to
 *            IP address bindings. This capability protects the network
 *            from certain "man-in-the-middle" attacks.
 *
 *-----------------------------------------------------------------------------
 * HISTORY  : 2007.11  -  Joseph Lin, Create
 *
 *-----------------------------------------------------------------------------
 * Copyright(C)                               Accton Corporation, 2007
 *-----------------------------------------------------------------------------
 */

#include "sys_type.h"
#include "dai_pmgr.h"

/* FUNCTION NAME : DAI_PMGR_ClearVlanFilterByAclName
 * PURPOSE:
 *      To clear ACL rule on vlan.
 *
 * INPUT:
 *      acl_name  --  ACL rule name need to be cleared
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      DAI_TYPE_OK
 *      DAI_TYPE_FAIL
 *
 * NOTES:
 *      1.In Global CLI: "no access-list arp "
 */
UI32_T DAI_PMGR_ClearVlanFilterByAclName(UI8_T *acl_name)
{
    return DAI_TYPE_OK;
}
