#include "utest.h"

#if 0
static BOOL_T RULE_MGR_TEST_Bind_QoS_To_Lookup0()
{
    UI8_T *policy_map_name = (UI8_T*)"policy-map";
    UI8_T *class_map_name = (UI8_T*)"class-map";
    UI32_T uport_ifindex = 1;
    RULE_TYPE_UI_ClassMapElement_T cls_entry;
    UI32_T class_map_index;
    UI32_T policy_map_index;

    RULE_OM_InitiateSystemResources();
    RULE_CTRL_InitiateSystemResources();
    RULE_MGR_InitiateSystemResources();
    RULE_CTRL_EnterMasterMode();

    memset(&cls_entry, 0, sizeof(cls_entry));
    cls_entry.class_type = RULE_TYPE_CLASS_MF;
    cls_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
    cls_entry.element.mf_entry.mf_value = 1;

    if (RULE_TYPE_OK != RULE_MGR_CreatePolicyMap(policy_map_name))
    {
        UT_LOG("RULE_MGR_CreatePolicyMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index))
    {
        UT_LOG("RULE_MGR_GetPolicyMapIdByName failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_CreateClassMap(class_map_name))
    {
        UT_LOG("RULE_MGR_CreateClassMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index))
    {
        UT_LOG("RULE_MGR_GetClassMapIdByName failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_AddElement2ClassMap(class_map_index, &cls_entry))
    {
        UT_LOG("RULE_MGR_AddElement2ClassMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_AddElement2PolicyMap(policy_map_index, class_map_index))
    {
        UT_LOG("RULE_MGR_AddElement2PolicyMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_BindPort2PolicyMap(uport_ifindex, RULE_TYPE_INBOUND, policy_map_index))
    {
        UT_LOG("RULE_MGR_BindPort2PolicyMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_UnBindPortFromPolicyMap(uport_ifindex, RULE_TYPE_INBOUND, policy_map_index))
    {
        UT_LOG("RULE_MGR_UnBindPortFromPolicyMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_BindPort2PolicyMap(uport_ifindex, RULE_TYPE_INBOUND, policy_map_index))
    {
        UT_LOG("RULE_MGR_BindPort2PolicyMap failed");
        return FALSE;
    }

    return TRUE;
}

static BOOL_T RULE_MGR_TEST_SetPolicyMapElement()
{
    /* L4_MGR_QoS_SetDiffServPolicyMapElementClassMapIndex
       L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex
       L4_MGR_QoS_SetDiffServPolicyMapElementActionIndex
     */
    UI32_T  policy_element_index;
    UI32_T  p1_index;
    UI32_T  c1_index;
    UI32_T  c2_index;
    UI32_T  row_status;
    RULE_TYPE_PolicyElement_T       policy_element;
    RULE_TYPE_UI_ClassMapElement_T  cls_entry;
    UI32_T uport_ifindex  = 1;
    UI8_T *p1_name  = (UI8_T*)"p1";
    UI8_T *c1_name  = (UI8_T*)"c1";
    UI8_T *c2_name  = (UI8_T*)"c2";


    enum
    {
       L_RSTATUS_ACTIVE = 1,
       L_RSTATUS_NOTINSERVICE=2,
       L_RSTATUS_NOTREADY=3,
       L_RSTATUS_CREATEANDGO=4,
       L_RSTATUS_CREATEANDWAIT=5,
       L_RSTATUS_DESTROY=6,
       L_RSTATUS_SET_OTHER = L_RSTATUS_SET_OTHER_COLUMN,    /* Not RFC value */
       L_RSTATUS_ACTIVECHECK=8                              /* Not RFC value */
    };


    RULE_OM_InitiateSystemResources();
    RULE_CTRL_InitiateSystemResources();
    RULE_MGR_InitiateSystemResources();
    RULE_CTRL_EnterMasterMode();

    if (RULE_TYPE_OK != RULE_MGR_CreatePolicyMap(p1_name))
    {
        UT_LOG("RULE_MGR_CreatePolicyMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_GetPolicyMapIdByName(p1_name, &p1_index))
    {
        UT_LOG("RULE_MGR_GetPolicyMapIdByName failed");
        return FALSE;
    }

    /* Create class-map c1 and bind it to policy-map p1
     * Then bind p1 to an ingress port
     */
    if ( UT_FAIL(RULE_MGR_CreateClassMap(c1_name)) ||
         UT_FAIL(RULE_MGR_GetClassMapIdByName(c1_name, &c1_index)))
    {
        UT_LOG("RULE_MGR_CreateClassMap failed");
        return FALSE;
    }

    memset(&cls_entry, 0, sizeof(cls_entry));
    cls_entry.class_type = RULE_TYPE_CLASS_MF;
    cls_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
    cls_entry.element.mf_entry.mf_value = 1;

    if (RULE_TYPE_OK != RULE_MGR_AddElement2ClassMap(c1_index, &cls_entry))
    {
        UT_LOG("RULE_MGR_AddElement2ClassMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_AddElement2PolicyMap(p1_index, c1_index))
    {
        UT_LOG("RULE_MGR_AddElement2PolicyMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_BindPort2PolicyMap(uport_ifindex, RULE_TYPE_INBOUND, p1_index))
    {
        UT_LOG("RULE_MGR_BindPort2PolicyMap failed");
        return FALSE;
    }

    /* Create class-map c2
     */
    if ( UT_FAIL(RULE_MGR_CreateClassMap(c2_name)) ||
         UT_FAIL(RULE_MGR_GetClassMapIdByName(c2_name, &c2_index)))
    {
        UT_LOG("RULE_MGR_CreateClassMap failed");
        return FALSE;
    }

    memset(&cls_entry, 0, sizeof(cls_entry));
    cls_entry.class_type = RULE_TYPE_CLASS_MF;
    cls_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
    cls_entry.element.mf_entry.mf_value = 2;

    if (RULE_TYPE_OK != RULE_MGR_AddElement2ClassMap(c2_index, &cls_entry))
    {
        UT_LOG("RULE_MGR_AddElement2ClassMap failed");
        return FALSE;
    }

    /* Replace the class-map in p1
     */

    policy_element_index = 1;
    if ( UT_FAIL(RULE_MGR_GetPolicyMapElement(policy_element_index, &policy_element)) )
    {
        UT_LOG("RULE_MGR_GetPolicyMapElement failed");
        return FALSE;
    }

    policy_element.class_map_index = c2_index;
    if (UT_FAIL(RULE_MGR_SetPolicyMapElement(policy_element_index, &policy_element)) )
    {
        UT_LOG("RULE_MGR_SetPolicyMapElement failed");
        return FALSE;
    }

    return TRUE;
}

static BOOL_T RULE_MGR_TEST_Bind_Qos_To_Engress_Port()
{
    UI8_T *policy_map_name = (UI8_T*)"policy-map";
    UI8_T *class_map_name  = (UI8_T*)"class-map";
    const UI32_T uport_ifindex = 1;
    RULE_TYPE_UI_ClassMapElement_T cls_entry;
    UI32_T class_map_index;
    UI32_T policy_map_index;



    RULE_OM_InitiateSystemResources();
    RULE_CTRL_InitiateSystemResources();
    RULE_MGR_InitiateSystemResources();
    RULE_CTRL_EnterMasterMode();




    memset(&cls_entry, 0, sizeof(cls_entry));
    cls_entry.class_type = RULE_TYPE_CLASS_MF;
    cls_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
    cls_entry.element.mf_entry.mf_value = 1;

    if (RULE_TYPE_OK != RULE_MGR_CreatePolicyMap(policy_map_name))
    {
        UT_LOG("RULE_MGR_CreatePolicyMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index))
    {
        UT_LOG("RULE_MGR_GetPolicyMapIdByName failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_CreateClassMap(class_map_name))
    {
        UT_LOG("RULE_MGR_CreateClassMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index))
    {
        UT_LOG("RULE_MGR_GetClassMapIdByName failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_AddElement2ClassMap(class_map_index, &cls_entry))
    {
        UT_LOG("RULE_MGR_AddElement2ClassMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_AddElement2PolicyMap(policy_map_index, class_map_index))
    {
        UT_LOG("RULE_MGR_AddElement2PolicyMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_BindPort2PolicyMap(uport_ifindex, RULE_TYPE_OUTBOUND, policy_map_index))
    {
        UT_LOG("RULE_MGR_BindPort2PolicyMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_UnBindPortFromPolicyMap(uport_ifindex, RULE_TYPE_OUTBOUND, policy_map_index))
    {
        UT_LOG("RULE_MGR_UnBindPortFromPolicyMap failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_BindPort2PolicyMap(uport_ifindex, RULE_TYPE_OUTBOUND, policy_map_index))
    {
        UT_LOG("RULE_MGR_BindPort2PolicyMap failed");
        return FALSE;
    }

    return TRUE;
}

static BOOL_T RULE_MGR_TEST_Bind_DIP6_Up_ACL()
{
    UI8_T *acl_name = (UI8_T*)"ip6-ext";
    const RULE_TYPE_AclType_T acl_type = RULE_TYPE_IPV6_EXT_ACL;

    const UI32_T port_ifindex = 1;
    const BOOL_T ingress = TRUE;
    const UI16_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;

    UI32_T acl_index;
    RULE_TYPE_Ace_Entry_T ace_entry;

    RULE_OM_InitiateSystemResources();
    RULE_CTRL_InitiateSystemResources();
    RULE_MGR_InitiateSystemResources();
    RULE_CTRL_EnterMasterMode();


    if (RULE_TYPE_OK != RULE_MGR_CreateAcl(acl_name, acl_type) ||
        RULE_TYPE_OK != RULE_MGR_GetAclIdByName(acl_name, &acl_index))
    {
        UT_LOG("RULE_MGR_CreateAcl failed");
        return FALSE;
    }

    ace_entry.ace_type = acl_type;
    RULE_MGR_InitAce(&ace_entry);

    ace_entry.u.ipv6.aceDestIpAddr[0] = 0x12;
    ace_entry.u.ipv6.aceDestIpAddrPrefixLen = 8;

    if (RULE_TYPE_OK != RULE_MGR_SetAce2Acl(acl_index, acl_type, &ace_entry))
    {
        UT_LOG("RULE_MGR_SetAce2Acl failed");
        return FALSE;
    }

    if (RULE_TYPE_OK != RULE_MGR_BindPort2Acl(port_ifindex, acl_index, RULE_TYPE_IPV6_ACL, ingress, time_range_index))
    {
        UT_LOG("RULE_MGR_BindPort2Acl failed");
        return FALSE;
    }

    return TRUE;
}
#endif

BOOL_T RULE_MGR_TEST_Main()
{
    /*
    return RULE_MGR_TEST_Bind_QoS_To_Lookup0()
        && RULE_MGR_TEST_SetPolicyMapElement()
        && RULE_MGR_TEST_Bind_Qos_To_Engress_Port()
        && RULE_MGR_TEST_Bind_DIP6_Up_ACL()
        ;
    */
    return TRUE;
}
