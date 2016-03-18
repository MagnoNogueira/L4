/*
 *  rule_om_exp_utest_cases.c
 *  l4_mgr
 *
 *  Created by yehjunying on 2011/10/14.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "rule_om_exp.h"

/*------------------------------------------------------------------------------
 * Test Functions
 *------------------------------------------------------------------------------
 */

int
RULE_OM_EXP_UT_Setup()
{
    RULE_OM_InitiateSystemResources();
    RULE_OM_EXP_AttachSystemResources();

    return 0;
}

int
RULE_OM_EXP_UT_Teardown()
{
    return 0;
}

int
RULE_OM_EXP_UT_Hello()
{
    const char *acl_name = "a1";
    UI32_T acl_index;

    RULE_OM_EXP_BgpFilterData_T bgp_filter;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_Ace_Entry_T ace;

    acl_index = RULE_OM_CreateAcl(acl_name, RULE_TYPE_IP_STD_ACL);

    SET_IPV4(bgp_filter.ip_addr, 192, 168, 1, 1);

    result = RULE_OM_EXP_SwAclCheckForBgpDistrbLst(acl_name, &bgp_filter);
    assert(RULE_TYPE_NO_MATCH == result);

    ace.ace_type = RULE_TYPE_IP_STD_ACL;
    RULE_OM_InitAce(&ace);

    SET_IPV4(ace.u.ip.aceSourceIpAddr,        192, 168, 1,   0);
    SET_IPV4(ace.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 0);

    result = RULE_OM_AddAce2Acl(acl_index, RULE_TYPE_IP_STD_ACL, &ace);

    ace.ace_type = RULE_TYPE_IP_STD_ACL;
    RULE_OM_InitAce(&ace);

    SET_IPV4(ace.u.ip.aceSourceIpAddr,        192, 168, 2,   0);
    SET_IPV4(ace.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 0);

    ace.access = RULE_TYPE_ACE_DENY;
    
    result = RULE_OM_AddAce2Acl(acl_index, RULE_TYPE_IP_STD_ACL, &ace);

    ////////////////////////////////////////////////////////////////////////////


    SET_IPV4(bgp_filter.ip_addr, 192, 168, 1, 1);

    result = RULE_OM_EXP_SwAclCheckForBgpDistrbLst(acl_name, &bgp_filter);
    assert(RULE_TYPE_MATCH_PERMIT == result);

    SET_IPV4(bgp_filter.ip_addr, 192, 168, 1, 2);

    result = RULE_OM_EXP_SwAclCheckForBgpDistrbLst(acl_name, &bgp_filter);
    assert(RULE_TYPE_MATCH_PERMIT == result);

    SET_IPV4(bgp_filter.ip_addr, 192, 168, 2, 2);

    result = RULE_OM_EXP_SwAclCheckForBgpDistrbLst(acl_name, &bgp_filter);
    assert(RULE_TYPE_MATCH_DENY == result);

    SET_IPV4(bgp_filter.ip_addr, 192, 168, 3, 2);

    result = RULE_OM_EXP_SwAclCheckForBgpDistrbLst(acl_name, &bgp_filter);
    assert(RULE_TYPE_NO_MATCH == result);


    return 0;
}

/*------------------------------------------------------------------------------
 * Entry Pointer
 *------------------------------------------------------------------------------
 */
int
RULE_OM_EXP_UT_RunTestCases()
{
#define RULE_OM_EXP_TEST(func) \
UT_Test(RULE_OM_EXP_UT_Setup, RULE_OM_EXP_UT_Teardown, func)

    RULE_OM_EXP_TEST(RULE_OM_EXP_UT_Hello);
    return 0;

#undef RULE_OM_EXP_TEST
}

