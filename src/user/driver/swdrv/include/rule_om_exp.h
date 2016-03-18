/* MODULE NAME: rule_om_exp.h
 * PURPOSE:
 *   Declarations of Exported OM APIs for ACL (,etc...).
 *
 * NOTES:
 *   None
 *
 * HISTORY:
 *   mm/dd/yy (A.D.)
 *   10/14/11    -- yehjunying, Create
 *
 * Copyright(C)      Accton Corporation, 2011
 */
#ifndef _RULE_OM_EXP_H_
#define _RULE_OM_EXP_H_

/* INCLUDE FILE DECLARATIONS
 */
#include "rule_om.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
typedef struct RULE_OM_EXP_BgpFilterData_S
{
    UI32_T  ip_addr;    /* IP Address   (Host order) */
    UI32_T  ip_pfx;     /* IP Prefix    (Host order) */
}   RULE_OM_EXP_BgpFilterData_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * FUNCTION NAME: RULE_OM_EXP_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for RULE_CTRL in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void
RULE_OM_EXP_AttachSystemResources(
    void
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_EXP_GetAclIdByNameAndType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name and type.
 * INPUT:    acl_name_p, acl_type
 * OUTPUT:   acl_index_p.
 * RETURN:   RULE_TYPE_OK/RULE_TYPE_FAIL
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_EXP_GetAclIdByNameAndType(
    char *acl_name_p,
    RULE_TYPE_AclType_T acl_type,
    UI32_T *acl_index_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_EXP_SwAclCheckForBgpDistrbLst
 *------------------------------------------------------------------------------
 * PURPOSE: To do software filtering with provided ACL name, and data for BGP
 *          distribute list.
 * INPUT  : acl_name_p - pointer to acl name required to do filtering
 *          data_p     - pointer to data for filtering
 * OUTPUT : None
 * RETURN : RULE_TYPE_FAIL           - unexpected failure
 *          RULE_TYPE_ACL_NONEXISTED - required ACL does not exist
 *          RULE_TYPE_MATCH_PERMIT   - permit ace matched
 *          RULE_TYPE_MATCH_DENY     - deny   ace matched
 *          RULE_TYPE_NO_MATCH       - no     ace matched
 * NOTE   : None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_EXP_SwAclCheckForBgpDistrbLst(
    const char *acl_name_p,
    RULE_OM_EXP_BgpFilterData_T *data_p
);

#endif /* _RULE_OM_EXP_H_ */

