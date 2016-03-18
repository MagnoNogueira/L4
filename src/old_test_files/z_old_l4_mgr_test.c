#include <stdio.h>

#ifdef MARVELL_CPSS
  #include "cpss.h"
  #include "dev_rm.h"
#endif

#include "rule_ctrl.h"
#include "utest.h"

#define SET_MAC(mac, b1, b2, b3, b4, b5, b6) \
    { mac[0]=b1; mac[1]=b2; mac[2]=b3; mac[3]=b4; mac[4]=b5; mac[5]=b6; }
#define SET_IP(ip, b1, b2, b3, b4)  \
    { ((UI8_T*)&ip)[0]=b1; ((UI8_T*)&ip)[1]=b2; ((UI8_T*)&ip)[2]=b3; ((UI8_T*)&ip)[3]=b4; }

static int L4_MGR_TEST_Setup()
{
#ifdef MARVELL_CPSS
    _cpssInit();
    DEVRM_Initial();
    DEVRM_InitiateSystemResources();
#endif // MARVELL_CPSS

    RULE_OM_InitiateSystemResources();
    RULE_CTRL_InitiateSystemResources();

    RULE_CTRL_EnterMasterMode();

    return 0;
}

static int L4_MGR_TEST_Setdown()
{
    return 0;
}

static int L4_MGR_TEST_Bind_PolicyMap_To_EngressPort_one_class_one_rule()
{
    {
        char    *policy_map_name = "p1";
        UI32_T  policy_map_index;

        /* policy-map p1
         */
        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        /* class-map c1
         *    match dscp 1
         */
        {
            char    *class_map_name = "c1";
            UI32_T  class_map_index;

            if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                        RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_MF;
                element_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
                element_entry.element.mf_entry.mf_value = 1;

                if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }
            }

            RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
        }

        /* service in p1
         */
        {
            UI32_T uport_ifindex = 2;

            if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
            {
                UT_LOG("Bind policy-map failed");
                return 1;
            }
        }

        /* service out p1
         */
        {
            UI32_T uport_ifindex = 2;

            if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
            {
                UT_LOG("Bind policy-map failed");
                return 1;
            }
        }
    }

    return 0;
}

#if (SYS_CPNT_QOS_V2_POLICY_MAP_MODIFY_ON_FLY == TRUE) || 1
static int L4_MGR_TEST_Change_ClassMap_For_PolicyMap_OnFly()
{
    {
        char    *policy_map_name = "p1";
        UI32_T  policy_map_index;
        char    *acl_name = "a1";

        /* policy-map p1
         */
        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        /* access mac a1
         *    permit any host 00-00-00-xx-xx-xx
         *    permit any host 00-00-00-yy-yy-yy
         *    deny any any
         */
        {
            //char                *acl_name = "a1";
            RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
            UI32_T              acl_index;

            if ((RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T*)acl_name, acl_type)) ||
                (RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T*)acl_name, &acl_index))
                )
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            {
                RULE_TYPE_Ace_Entry_T ace_entry;

                ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACL failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
                ace_entry.u.mac.aceDestMacAddr[3] = 0x11;
                ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
                ace_entry.u.mac.aceDestMacAddr[5] = 0x33;
                memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

                if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }
            }

            {
                RULE_TYPE_Ace_Entry_T ace_entry;

                ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACL failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
                ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
                ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
                ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
                memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

                if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }
            }

            {
                RULE_TYPE_Ace_Entry_T ace_entry = {0};

                ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACL failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_DENY;

                if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }
            }
        }

        /* class-map c1
         *    match access-list a1
         * service in p1
         */
        {
            char    *class_map_name = "c1";
            UI32_T  class_map_index;

            if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                        RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_ACL;
                strcpy((char*)element_entry.element.acl_name, acl_name);

                if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }
            }

            if (RULE_TYPE_OK != RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            /* service in p1
             */
            {
                UI32_T uport_ifindex = 2;

                if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

            /* service out p1
             * we should create another PCL for engress policy-map
             */
            {
                UI32_T uport_ifindex = 2;

                if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

            /* class-map c2
             *     match dscp 1
             */
            {
                char    *class_map_name = "c2";
                UI32_T  class_map_index;

                if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                            RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }

                {
                    RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                    UI32_T                         class_index;

                    element_entry.class_type = RULE_TYPE_CLASS_MF;
                    element_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
                    element_entry.element.mf_entry.mf_value = 1;

                    if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
                    {
                        UT_LOG("Prepare class-map failed");
                        return 1;
                    }
                }

                /* replace old class-map
                 */
                {
                    UI32_T policy_element_index = 1;

                    if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementClassMapIndex(policy_element_index, class_map_index))
                    {
                        UT_LOG("Set policy-map on fly failed");
                        return 1;
                    }
                }
            }
        } /* class c1 */

    }

    return 0;
}

static int L4_MGR_TEST_Change_Meter_For_MF_PolicyMap_OnFly_part1()
{
    {
        char    *policy_map_name = "p1";
        UI32_T  policy_map_index;

        /* policy-map p1
         */
        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        /* class-map c1
         *    match dscp 1
         * service in p1
         */
        {
            char    *class_map_name = "c1";
            UI32_T  class_map_index;

            if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                        RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_MF;
                element_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
                element_entry.element.mf_entry.mf_value = 1;

                if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }
            }

            RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);

            /* service in p1
             */
            {
                UI32_T uport_ifindex = 2;

                if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

            /* service out p1
             * we should create another PCL for engress policy-map
             */
            {
                UI32_T uport_ifindex = 2;

                if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

            /* we should have two meter, one is for ingress, other is for engress
             * test RULE_MGR_SetPolicyMapElement and
             * RULE_CTRL_AddMeterConfig/RULE_CTRL_UpdateMeterConfig/RULE_CTRL_RemoveMeterConfig
             */
            {
                UI32_T meter_index;
                RULE_TYPE_TBParamEntry_T meter_entry = {0};

                UI32_T policy_element_index = 1;

                meter_index = 1;
                meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
                meter_entry.rate            = 100;
                meter_entry.burst_size      = 20000;
                meter_entry.peak_rate       = 10000;
                meter_entry.peak_burst_size = 20000;
                meter_entry.row_status = VAL_diffServMeterStatus_active;

                if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_createAndWait) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterRate(meter_index, meter_entry.rate) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterBurstSize(meter_index, meter_entry.burst_size) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakRate(meter_index, meter_entry.peak_rate) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakBurstSize(meter_index, meter_entry.peak_burst_size) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_active))
                {
                    UT_LOG("Prepare meter falied");
                    return 1;
                }

                if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                {
                    UT_LOG("Set policy-map on fly failed");
                    return 1;
                }

                //meter_index = 2;
                //meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
                //meter_entry.rate            = 199;
                //meter_entry.burst_size      = 29999;
                //meter_entry.peak_rate       = 19999;
                //meter_entry.peak_burst_size = 29999;
                //meter_entry.row_status = VAL_diffServMeterStatus_active;

                //if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_createAndWait) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterRate(meter_index, meter_entry.rate) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterBurstSize(meter_index, meter_entry.burst_size) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakRate(meter_index, meter_entry.peak_rate) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakBurstSize(meter_index, meter_entry.peak_burst_size) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_active))
                //{
                //    UT_LOG("Prepare meter falied");
                //    return 1;
                //}

                //if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                //{
                //    UT_LOG("Set policy-map on fly failed");
                //    return 1;
                //}

                //meter_index = 0;
                //if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                //{
                //    UT_LOG("Set policy-map on fly failed");
                //    return 1;
                //}
            }

        } /* class c1 */

    }

    return 0;
}

static int L4_MGR_TEST_Change_Meter_For_MF_PolicyMap_OnFly_part2()
{
    {
        //char    *policy_map_name = "p1";
        //UI32_T  policy_map_index;

        /* policy-map p1
         */
        //if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
        //{
        //    UT_LOG("Prepare policy-map failed");
        //    return 1;
        //}

        /* class-map c1
         *    match dscp 1
         * service in p1
         */
        {
            //char    *class_map_name = "c1";
            //UI32_T  class_map_index;

            //if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name, &class_map_index))
            //{
            //    UT_LOG("Prepare class-map failed");
            //    return 1;
            //}

            //{
            //    RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
            //    UI32_T                         class_index;

            //    element_entry.class_type = RULE_TYPE_CLASS_MF;
            //    element_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
            //    element_entry.element.mf_entry.mf_value = 1;
            //
            //    if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
            //    {
            //        UT_LOG("Prepare class-map failed");
            //        return 1;
            //    }
            //}

            //RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);

            ///* service in p1
            // */
            //{
            //    UI32_T uport_ifindex = 2;

            //    if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
            //    {
            //        UT_LOG("Bind policy-map failed");
            //        return 1;
            //    }
            //}

            ///* service out p1
            // * we should create another PCL for engress policy-map
            // */
            //{
            //    UI32_T uport_ifindex = 2;

            //    if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
            //    {
            //        UT_LOG("Bind policy-map failed");
            //        return 1;
            //    }
            //}

            /* we should have two meter, one is for ingress, other is for engress
             * test RULE_MGR_SetPolicyMapElement and
             * RULE_CTRL_AddMeterConfig/RULE_CTRL_UpdateMeterConfig/RULE_CTRL_RemoveMeterConfig
             */
            {
                UI32_T meter_index;
                RULE_TYPE_TBParamEntry_T meter_entry = {0};

                UI32_T policy_element_index = 1;

                //meter_index = 1;
                //meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
                //meter_entry.rate            = 100;
                //meter_entry.burst_size      = 20000;
                //meter_entry.peak_rate       = 10000;
                //meter_entry.peak_burst_size = 20000;
                //meter_entry.row_status = VAL_diffServMeterStatus_active;

                //if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_createAndWait) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterRate(meter_index, meter_entry.rate) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterBurstSize(meter_index, meter_entry.burst_size) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakRate(meter_index, meter_entry.peak_rate) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakBurstSize(meter_index, meter_entry.peak_burst_size) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_active))
                //{
                //    UT_LOG("Prepare meter falied");
                //    return 1;
                //}

                //if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                //{
                //    UT_LOG("Set policy-map on fly failed");
                //    return 1;
                //}

                meter_index = 2;
                meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
                meter_entry.rate            = 199;
                meter_entry.burst_size      = 29999;
                meter_entry.peak_rate       = 19999;
                meter_entry.peak_burst_size = 29999;
                meter_entry.row_status = VAL_diffServMeterStatus_active;

                if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_createAndWait) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterRate(meter_index, meter_entry.rate) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterBurstSize(meter_index, meter_entry.burst_size) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakRate(meter_index, meter_entry.peak_rate) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakBurstSize(meter_index, meter_entry.peak_burst_size) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_active))
                {
                    UT_LOG("Prepare meter falied");
                    return 1;
                }

                if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                {
                    UT_LOG("Set policy-map on fly failed");
                    return 1;
                }

                //meter_index = 0;
                //if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                //{
                //    UT_LOG("Set policy-map on fly failed");
                //    return 1;
                //}
            }

        } /* class c1 */

    }

    return 0;
}

static int L4_MGR_TEST_Change_Meter_For_MF_PolicyMap_OnFly_part3()
{
    {
        //char    *policy_map_name = "p1";
        //UI32_T  policy_map_index;

        /* policy-map p1
         */
        //if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
        //{
        //    UT_LOG("Prepare policy-map failed");
        //    return 1;
        //}

        /* class-map c1
         *    match dscp 1
         * service in p1
         */
        {
            //char    *class_map_name = "c1";
            //UI32_T  class_map_index;

            //if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name, &class_map_index))
            //{
            //    UT_LOG("Prepare class-map failed");
            //    return 1;
            //}

            //{
            //    RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
            //    UI32_T                         class_index;

            //    element_entry.class_type = RULE_TYPE_CLASS_MF;
            //    element_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
            //    element_entry.element.mf_entry.mf_value = 1;
            //
            //    if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
            //    {
            //        UT_LOG("Prepare class-map failed");
            //        return 1;
            //    }
            //}

            //RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);

            ///* service in p1
            // */
            //{
            //    UI32_T uport_ifindex = 2;

            //    if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
            //    {
            //        UT_LOG("Bind policy-map failed");
            //        return 1;
            //    }
            //}

            ///* service out p1
            // * we should create another PCL for engress policy-map
            // */
            //{
            //    UI32_T uport_ifindex = 2;

            //    if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
            //    {
            //        UT_LOG("Bind policy-map failed");
            //        return 1;
            //    }
            //}

            /* we should have two meter, one is for ingress, other is for engress
             * test RULE_MGR_SetPolicyMapElement and
             * RULE_CTRL_AddMeterConfig/RULE_CTRL_UpdateMeterConfig/RULE_CTRL_RemoveMeterConfig
             */
            {
                UI32_T meter_index;
                //RULE_TYPE_TBParamEntry_T meter_entry = {0};

                UI32_T policy_element_index = 1;

                //meter_index = 1;
                //meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
                //meter_entry.rate            = 100;
                //meter_entry.burst_size      = 20000;
                //meter_entry.peak_rate       = 10000;
                //meter_entry.peak_burst_size = 20000;
                //meter_entry.row_status = VAL_diffServMeterStatus_active;

                //if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_createAndWait) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterRate(meter_index, meter_entry.rate) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterBurstSize(meter_index, meter_entry.burst_size) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakRate(meter_index, meter_entry.peak_rate) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakBurstSize(meter_index, meter_entry.peak_burst_size) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_active))
                //{
                //    UT_LOG("Prepare meter falied");
                //    return 1;
                //}

                //if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                //{
                //    UT_LOG("Set policy-map on fly failed");
                //    return 1;
                //}

                //meter_index = 2;
                //meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
                //meter_entry.rate            = 199;
                //meter_entry.burst_size      = 29999;
                //meter_entry.peak_rate       = 19999;
                //meter_entry.peak_burst_size = 29999;
                //meter_entry.row_status = VAL_diffServMeterStatus_active;

                //if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_createAndWait) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterRate(meter_index, meter_entry.rate) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterBurstSize(meter_index, meter_entry.burst_size) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakRate(meter_index, meter_entry.peak_rate) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakBurstSize(meter_index, meter_entry.peak_burst_size) ||
                //    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_active))
                //{
                //    UT_LOG("Prepare meter falied");
                //    return 1;
                //}

                //if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                //{
                //    UT_LOG("Set policy-map on fly failed");
                //    return 1;
                //}

                meter_index = 0;
                if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                {
                    UT_LOG("Set policy-map on fly failed");
                    return 1;
                }
            }

        } /* class c1 */

    }

    return 0;
}

static int L4_MGR_TEST_Change_Meter_For_ACL_PolicyMap_OnFly()
{
    {
        char    *policy_map_name = "p1";
        UI32_T  policy_map_index;
        char    *acl_name = "a1";

        /* policy-map p1
         */
        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        /* access mac a1
         *    permit any host 00-00-00-xx-xx-xx
         *    permit any host 00-00-00-yy-yy-yy
         *    deny any any
         */
        {
            //char                *acl_name = "a1";
            RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
            UI32_T              acl_index;

            if ((RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T*)acl_name, acl_type)) ||
                (RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T*)acl_name, &acl_index))
                )
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            {
                RULE_TYPE_Ace_Entry_T ace_entry;

                ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACL failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
                ace_entry.u.mac.aceDestMacAddr[3] = 0x11;
                ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
                ace_entry.u.mac.aceDestMacAddr[5] = 0x33;
                memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

                if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }
            }

            {
                RULE_TYPE_Ace_Entry_T ace_entry;

                ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACL failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
                ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
                ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
                ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
                memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

                if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }
            }

            {
                RULE_TYPE_Ace_Entry_T ace_entry = {0};

                ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACL failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_DENY;

                if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }
            }
        }

        /* class-map c1
         *    match access-list a1
         * service in p1
         */
        {
            char    *class_map_name = "c1";
            UI32_T  class_map_index;

            if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                        RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_ACL;
                strcpy((char*)element_entry.element.acl_name, acl_name);

                if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }
            }

            if (RULE_TYPE_OK != RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            /* service in p1
             */
            {
                UI32_T uport_ifindex = 2;

                if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

            /* service out p1
             * we should create another PCL for engress policy-map
             */
            {
                UI32_T uport_ifindex = 2;

                if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

            /* we should have two meter, one is for ingress, other is for engress
             * test RULE_MGR_SetPolicyMapElement and
             * RULE_CTRL_AddMeterConfig/RULE_CTRL_UpdateMeterConfig/RULE_CTRL_RemoveMeterConfig
             */
            {
                UI32_T meter_index;
                RULE_TYPE_TBParamEntry_T meter_entry = {0};

                UI32_T policy_element_index = 1;

                meter_index = 1;
                meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
                meter_entry.rate            = 100;
                meter_entry.burst_size      = 20000;
                meter_entry.peak_rate       = 10000;
                meter_entry.peak_burst_size = 20000;
                meter_entry.row_status = VAL_diffServMeterStatus_active;

                if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_createAndWait) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterRate(meter_index, meter_entry.rate) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterBurstSize(meter_index, meter_entry.burst_size) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakRate(meter_index, meter_entry.peak_rate) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakBurstSize(meter_index, meter_entry.peak_burst_size) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_active))
                {
                    UT_LOG("Prepare meter falied");
                    return 1;
                }

                if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                {
                    UT_LOG("Set policy-map on fly failed");
                    return 1;
                }

                meter_index = 2;
                meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
                meter_entry.rate            = 199;
                meter_entry.burst_size      = 29999;
                meter_entry.peak_rate       = 19999;
                meter_entry.peak_burst_size = 29999;
                meter_entry.row_status = VAL_diffServMeterStatus_active;

                if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_createAndWait) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterRate(meter_index, meter_entry.rate) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterBurstSize(meter_index, meter_entry.burst_size) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakRate(meter_index, meter_entry.peak_rate) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterPeakBurstSize(meter_index, meter_entry.peak_burst_size) ||
                    RULE_TYPE_OK != L4_MGR_QoS_SetDiffServMeterStatus(meter_index, VAL_diffServMeterStatus_active))
                {
                    UT_LOG("Prepare meter falied");
                    return 1;
                }

                if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                {
                    UT_LOG("Set policy-map on fly failed");
                    return 1;
                }

                meter_index = 0;
                if (RULE_TYPE_OK != L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(policy_element_index, meter_index))
                {
                    UT_LOG("Set policy-map on fly failed");
                    return 1;
                }
            }

        } /* class c1 */

    }

    return 0;
}
#endif /* SYS_CPNT_QOS_V2_POLICY_MAP_MODIFY_ON_FLY */

static int L4_MGR_TEST_Change_ACE_For_ACL_OnFly_part1()
{
    /* access-list mac a1
     * mac access-group a1 input
     * mac access-group a1 output
     * permit any host 00-00-00-xx-xx-xx
     * no permit any host 00-00-00-xx-xx-xx
     */
    {
        char                *acl_name = "a1";
        RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T*)acl_name, acl_type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T*)acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return 1;
        }

        {
            UI32_T uport_ifindex = 2;
            BOOL_T ingress_flag;

            ingress_flag = TRUE;
            if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag, NULL))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            ingress_flag = FALSE;
            if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag, NULL))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }
        }

        {
            UI32_T uport_ifindex = 3;
            BOOL_T ingress_flag;

            ingress_flag = TRUE;
            if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag, NULL))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            ingress_flag = FALSE;
            if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag, NULL))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }
        }

        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

            if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }

            ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
            ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
            ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
            ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
            memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

            if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T*)acl_name, acl_type, &ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }
        }

        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

            if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }

            ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
            ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
            ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
            ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
            memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

            if (RULE_TYPE_OK != L4_MGR_ACL_DelAceFromAcl((UI8_T*)acl_name, acl_type, &ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }
        }

        /*
         * Test path to RULE_CTRL_LocalRemoveAceFromDEV
         *
         * The test step will be
         * add ACE, then remove it from SNMP API
         *    L4_MGR_QoS_SetDiffServAclAttachCtlAction or
         *    RULE_MGR_SetAceFieldByIndex <- L4_MGR_QoS_SetDiffServXXX (e.g., L4_MGR_QoS_SetDiffServMacAceMinVid)
         *
         */
        {
            RULE_TYPE_Ace_Entry_T ace_entry;
            UI32_T                ace_index;

            ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

            if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }

            ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
            ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
            ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
            ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
            memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

            if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T*)acl_name, acl_type, &ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }

            if (RULE_TYPE_OK != RULE_OM_GetAceIndexFromAclByAceEntry(acl_index, &ace_entry, &ace_index))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }

            {
                UI32_T min_vid = 2;

                if (TRUE != L4_MGR_QoS_SetDiffServMacAceMinVid(ace_index, min_vid))
                {
                    UT_LOG("Test L4_MGR_QoS_SetDiffServMacAceMinVid failed");
                    return 1;
                }
            }
        }
    }

    return 0;
}

static int L4_MGR_TEST_Change_ACE_For_ACL_OnFly_part2()
{
    /* clear port 2
     */
    {
        char                *acl_name = "a1";
        RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;


        {
            UI32_T uport_ifindex = 2;
            BOOL_T ingress_flag;

            //ingress_flag = TRUE;
            //if (RULE_TYPE_OK != L4_MGR_ACL_UnBindPortFromAcl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag))
            //{
            //    UT_LOG("Prepare ACL failed");
            //    return 1;
            //}

            ingress_flag = FALSE;
            if (RULE_TYPE_OK != L4_MGR_ACL_UnBindPortFromAcl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }
        }
    }

    return 0;
}

static int L4_MGR_TEST_Change_ACE_For_ACL_OnFly_part3()
{
    /* clear port 3
     */
    {
        char                *acl_name = "a1";
        RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;

        {
            UI32_T uport_ifindex = 3;
            BOOL_T ingress_flag;

            ingress_flag = TRUE;
            if (RULE_TYPE_OK != L4_MGR_ACL_UnBindPortFromAcl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            ingress_flag = FALSE;
            if (RULE_TYPE_OK != L4_MGR_ACL_UnBindPortFromAcl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }
        }
    }

    return 0;
}

static int L4_MGR_TEST_Change_ACE_For_ACL_PolicyMap_OnFly_part1()
{
    {
        char    *policy_map_name = "p1";
        UI32_T  policy_map_index;

        char                *acl_name = "a1";
        RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;

        /* policy-map p1
         */
        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        /* access mac a1
         *    permit any host 00-00-00-11-22-33
         *    deny any any
         */
        {
            //char                *acl_name = "a1";
            //RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
            //UI32_T              acl_index;

            if ((RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T*)acl_name, acl_type)) ||
                (RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T*)acl_name, &acl_index))
                )
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            {
                RULE_TYPE_Ace_Entry_T ace_entry;

                ace_entry.ace_type    = acl_type;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACL failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
                ace_entry.u.mac.aceDestMacAddr[3] = 0x11;
                ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
                ace_entry.u.mac.aceDestMacAddr[5] = 0x33;
                memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

                if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }
            }

            {
                RULE_TYPE_Ace_Entry_T ace_entry = {0};

                ace_entry.ace_type    = acl_type;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACL failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_DENY;

                if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }
            }
        }

        /* class-map c1
         *    match access-list a1
         *
         * in et 1/2
         * service input p1
         * service output p1
         */
        {
            char    *class_map_name = "c1";
            UI32_T  class_map_index;

            if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                        RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                                        &class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_ACL;
                strcpy((char*)element_entry.element.acl_name, acl_name);

                if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }
            }

            if (RULE_TYPE_OK != RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            /* service in p1
             */
            {
                UI32_T uport_ifindex = 2;

                if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

            /* service out p1
             * we should create another PCL for engress policy-map
             */
            {
                UI32_T uport_ifindex = 2;

                if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

            /*
             * Test path to RULE_CTRL_LocalAddAce2DevForQosPurpose and
             *              RULE_CTRL_LocalRemoveAceFromDevForQosPurpose
             *
             */
            {
                RULE_TYPE_Ace_Entry_T ace_entry;
                //UI32_T                ace_index;

                ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
                ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
                ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
                ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
                memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

                if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T*)acl_name, acl_type, &ace_entry))
                {
                    UT_LOG("Test RULE_CTRL_LocalAddAce2DevForQosPurpose failed");
                    return 1;
                }

                if (RULE_TYPE_OK != L4_MGR_ACL_DelAceFromAcl((UI8_T*)acl_name, acl_type, &ace_entry))
                {
                    UT_LOG("Test RULE_CTRL_LocalRemoveAceFromDevForQosPurpose failed");
                    return 1;
                }
            }

        } /* class c1 */

    }

    return 0;
}

static int L4_MGR_TEST_Change_ACE_For_ACL_PolicyMap_OnFly_part2()
{
    {
        char    *policy_map_name = "p1";
        UI32_T  uport_ifindex;
        RULE_TYPE_InOutDirection_T inout_profile;

        uport_ifindex = 2;
        inout_profile = RULE_TYPE_INBOUND;
        if (RULE_TYPE_OK != L4_MGR_QoS_UnBindPortFromPolicyMap(uport_ifindex, (UI8_T*)policy_map_name, inout_profile))
        {
            UT_LOG("Clear resource failed");
            return 1;
        }

        uport_ifindex = 2;
        inout_profile = RULE_TYPE_OUTBOUND;
        if (RULE_TYPE_OK != L4_MGR_QoS_UnBindPortFromPolicyMap(uport_ifindex, (UI8_T*)policy_map_name, inout_profile))
        {
            UT_LOG("Clear resource failed");
            return 1;
        }
    }

    return 0;
}

static int L4_MGR_TEST_Bind_ACL_PolicyMap_On_Same_Port()
{
    char    *policy_map_name = "p1";
    UI32_T  policy_map_index;

    char                *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
    UI32_T              acl_index;

    UI32_T              uport_ifindex = 3;

    if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index) ||
        RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T*)acl_name, acl_type) ||
        RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T*)acl_name, &acl_index))
    {
        UT_LOG("Prepare policy-map and ACL failed");
        return 1;
    }

    {
        char            *class_map_name = "c1";
        UI32_T          class_map_index;

        if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                    RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                                    &class_map_index))
        {
            UT_LOG("Prepare class-map failed");
            return 1;
        }

        if (RULE_TYPE_OK != RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index))
        {
            UT_LOG("Prepare class-map failed");
            return 1;
        }
    }

    if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex, (UI8_T*)acl_name, acl_type, FALSE, NULL))
    {
        UT_LOG("Bind ACL to port failed");
        return 1;
    }

    if (RULE_TYPE_OK == L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
    {
        UT_LOG("Test bind ACL and PolicyMap on the same port(egress) failed");
        return 1;
    }

    if (RULE_TYPE_OK != L4_MGR_ACL_UnBindPortFromAcl(uport_ifindex, (UI8_T*)acl_name, FALSE, NULL))
    {
        UT_LOG("Bind ACL to port failed");
        return 1;
    }

    if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
    {
        UT_LOG("Test bind ACL and PolicyMap on the same port(egress) failed");
        return 1;
    }

    if (RULE_TYPE_OK == L4_MGR_ACL_BindPort2Acl(uport_ifindex, (UI8_T*)acl_name, acl_type, FALSE, NULL))
    {
        UT_LOG("Test bind ACL and PolicyMap on the same port(egress) failed");
        return 1;
    }

    return 0;
}

static int L4_MGR_TEST_Bind_PolicyMap_To_EngressPort_one_class_one_rule_and_meter()
{
    {
        char    *policy_map_name = "p1";
        UI32_T  policy_map_index;

        /* policy-map p1
         */
        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        /* class-map c1
         *    match dscp 1
         *    police trtcm xxx
         */
        {
            char    *class_map_name = "c1";
            UI32_T  class_map_index;

            if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                        RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                                        &class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_MF;
                element_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
                element_entry.element.mf_entry.mf_value = 1;

                if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }
            }

            RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};

                meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
                meter_entry.rate            = 100;
                meter_entry.burst_size      = 20000;
                meter_entry.peak_rate       = 10000;
                meter_entry.peak_burst_size = 20000;
                meter_entry.row_status = VAL_diffServMeterStatus_active;

                if (RULE_TYPE_OK != RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter_entry))
                {
                    UT_LOG("Prepare meter failed");
                    return 1;
                }
            }

            {
                RULE_TYPE_UI_Action_T    action = {0};

                action.in_action_type       = IN_ACTION_INVALID;
                action.in_action_value      = 8;

                action.confirm_action_type  = CONFIRM_ACTION_REMARK_DSCP_TRANSMIT;
                action.confirm_action_value = 10;

                action.exceed_action_type   = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
                action.exceed_action_value  = 20;

                action.violate_action_type  = VIOLATE_ACTION_DROP;
                action.violate_action_value = 64;

                if (RULE_TYPE_OK != RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &action))
                {
                    UT_LOG("Prepare action failed");
                    return 1;
                }
            }
        }


        /* service in p1
         */
        {
            UI32_T uport_ifindex = 2;

            if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
            {
                UT_LOG("Bind policy-map failed");
                return 1;
            }
        }

        /* service out p1
         */
        {
            UI32_T uport_ifindex = 2;

            if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
            {
                UT_LOG("Bind policy-map failed");
                return 1;
            }
        }
    }

    return 0;
}

static int L4_MGR_TEST_Check_Class_Instance_part1()
{
    /* access-list mac a1
     * mac access-group a1 input
     * mac access-group a1 output
     * permit any host 00-00-00-xx-xx-xx
     * no permit any host 00-00-00-xx-xx-xx
     */
    char                *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type  = RULE_TYPE_IP_ACL;
    UI32_T              acl_index;

    if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T*)acl_name, RULE_TYPE_IP_EXT_ACL) ||
        RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T*)acl_name, &acl_index))
    {
        UT_LOG("Prepare ACL failed");
        return UT_FAIL;
    }

    {
        UI32_T uport_ifindex = 2;
        BOOL_T ingress_flag;

        ingress_flag = TRUE;
        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag, NULL))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        ingress_flag = FALSE;
        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag, NULL))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }
    }

    {
        char    *policy_map_name = "p1";
        UI32_T  policy_map_index;

        /* policy-map p1
         */
        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        /* class-map c1
         *    match dscp 1
         *    match access-list a1
         *    police trtcm xxx
         */
        {
            char    *class_map_name = "c1";
            UI32_T  class_map_index;

            if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                        RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_MF;
                element_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
                element_entry.element.mf_entry.mf_value = 1;

                if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }

                if (RULE_TYPE_OK != RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_ACL;
                strcpy(element_entry.element.acl_name, acl_name);

                if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }
            }

            RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};

                meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
                meter_entry.rate            = 100;
                meter_entry.burst_size      = 20000;
                meter_entry.peak_rate       = 10000;
                meter_entry.peak_burst_size = 20000;
                meter_entry.row_status = VAL_diffServMeterStatus_active;

                if (RULE_TYPE_OK != RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter_entry))
                {
                    UT_LOG("Prepare meter failed");
                    return 1;
                }
            }

            {
                RULE_TYPE_UI_Action_T    action = {0};

                action.in_action_type       = IN_ACTION_INVALID;
                action.in_action_value      = 8;

                action.confirm_action_type  = CONFIRM_ACTION_REMARK_DSCP_TRANSMIT;
                action.confirm_action_value = 10;

                action.exceed_action_type   = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
                action.exceed_action_value  = 20;

                action.violate_action_type  = VIOLATE_ACTION_DROP;
                action.violate_action_value = 64;

                if (RULE_TYPE_OK != RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &action))
                {
                    UT_LOG("Prepare action failed");
                    return 1;
                }
            }
        }

        /* service in p1
         */
        {
            UI32_T uport_ifindex = 2;

            if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
            {
                UT_LOG("Bind policy-map failed");
                return 1;
            }
        }
    }

    return UT_OK;
}

static int L4_MGR_TEST_Check_Class_Instance_part2()
{
    /* access-list mac a1
     * mac access-group a1 input
     * mac access-group a1 output
     * permit any host 00-00-00-xx-xx-xx
     * no permit any host 00-00-00-xx-xx-xx
     */
    char                *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
    UI32_T              acl_index;

    //if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T*)acl_name, acl_type) ||
    //    RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T*)acl_name, &acl_index))
    //{
    //    UT_LOG("Prepare ACL failed");
    //    return UT_FAIL;
    //}

    //{
    //    UI32_T uport_ifindex = 2;
    //    BOOL_T ingress_flag;

    //    ingress_flag = TRUE;
    //    if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag, NULL))
    //    {
    //        UT_LOG("Prepare ACL failed");
    //        return UT_FAIL;
    //    }

    //    ingress_flag = FALSE;
    //    if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex, (UI8_T*)acl_name, acl_type, ingress_flag, NULL))
    //    {
    //        UT_LOG("Prepare ACL failed");
    //        return UT_FAIL;
    //    }
    //}

    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

        if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
        {
            UT_LOG("Prepare ACE failed");
            return UT_FAIL;
        }

        ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
        ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
        ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
        ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
        memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

        if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T*)acl_name, acl_type, &ace_entry))
        {
            UT_LOG("Prepare ACE failed");
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

        if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
        {
            UT_LOG("Prepare ACE failed");
            return UT_FAIL;
        }

        ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
        ace_entry.u.mac.aceDestMacAddr[3] = 0x99;
        ace_entry.u.mac.aceDestMacAddr[4] = 0x99;
        ace_entry.u.mac.aceDestMacAddr[5] = 0x99;
        memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

        if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T*)acl_name, acl_type, &ace_entry))
        {
            UT_LOG("Prepare ACE failed");
            return UT_FAIL;
        }
    }

    return UT_OK;
}

static int L4_MGR_TEST_Check_Class_Instance_part3()
{
    char    *policy_map_name = "p1";
    UI32_T  policy_map_index;

    /* policy-map p1
     */
    if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
    {
        UT_LOG("Prepare policy-map failed");
        return 1;
    }

    /* class-map c1
     *    match dscp 1
     *    police trtcm xxx
     */
    {
        char    *class_map_name = "c1";
        UI32_T  class_map_index;

        if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                    RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map_index))
        {
            UT_LOG("Prepare class-map failed");
            return 1;
        }

        {
            RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
            UI32_T                         class_index;

            element_entry.class_type = RULE_TYPE_CLASS_MF;
            element_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
            element_entry.element.mf_entry.mf_value = 1;

            if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }
        }

        RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);

        {
            RULE_TYPE_TBParamEntry_T meter_entry = {0};

            meter_entry.meter_model     = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
            meter_entry.rate            = 100;
            meter_entry.burst_size      = 20000;
            meter_entry.peak_rate       = 10000;
            meter_entry.peak_burst_size = 20000;
            meter_entry.row_status = VAL_diffServMeterStatus_active;

            if (RULE_TYPE_OK != RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter_entry))
            {
                UT_LOG("Prepare meter failed");
                return 1;
            }
        }

        {
            RULE_TYPE_UI_Action_T    action = {0};

            action.in_action_type       = IN_ACTION_INVALID;
            action.in_action_value      = 8;

            action.confirm_action_type  = CONFIRM_ACTION_REMARK_DSCP_TRANSMIT;
            action.confirm_action_value = 10;

            action.exceed_action_type   = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
            action.exceed_action_value  = 20;

            action.violate_action_type  = VIOLATE_ACTION_DROP;
            action.violate_action_value = 64;

            if (RULE_TYPE_OK != RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &action))
            {
                UT_LOG("Prepare action failed");
                return 1;
            }
        }
    }

    /* service in p1
     */
    {
        UI32_T uport_ifindex = 2;

        if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
        {
            UT_LOG("Bind policy-map failed");
            return 1;
        }
    }

    ///* service out p1
    // */
    //{
    //    UI32_T uport_ifindex = 2;

    //    if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
    //    {
    //        UT_LOG("Bind policy-map failed");
    //        return 1;
    //    }
    //}

    return UT_OK;
}

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
static int L4_MGR_TEST_Auto_Aggregate_BindAcl()
{
    UI32_T  i;
    RULE_TYPE_Ace_Entry_T ace;

    {
        char                *acl_name   = "a1";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        UI32_T              uport_ifindex = 2;

        if (RULE_TYPE_OK != L4_MGR_ACL_CreateAcl((UI8_T *) acl_name, acl_type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=1; i<=50; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                                    (UI8_T *)acl_name,
                                                    acl_type,
                                                    TRUE,
                                                    NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                                    (UI8_T *)acl_name,
                                                    acl_type,
                                                    FALSE,
                                                    NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }
    }

    return UT_OK;
}

static int L4_MGR_TEST_Auto_Aggregate_UnBindAcl()
{
    UI32_T  i;
    RULE_TYPE_Ace_Entry_T ace;

    {
        char                *acl_name   = "a1";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        UI32_T              uport_ifindex = 2;

        if (/*RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, acl_type) ||*/
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }
/*
        for (i=1; i<=50; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }
*/
        if (RULE_TYPE_OK != L4_MGR_ACL_UnBindPortFromAcl(uport_ifindex,
                                                         (UI8_T *)acl_name,
                                                         acl_type,
                                                         TRUE,
                                                         NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_UnBindPortFromAcl(uport_ifindex,
                                                         (UI8_T *)acl_name,
                                                         acl_type,
                                                         FALSE,
                                                         NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }
    }

    return UT_OK;
}

int L4_MGR_TEST_Auto_Aggregate_AddACE_onFly()
{
    UI32_T  i;
    RULE_TYPE_Ace_Entry_T ace;

    {
        char                *acl_name   = "a1";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        UI32_T              uport_ifindex = 2;

        if (RULE_TYPE_OK != L4_MGR_ACL_CreateAcl((UI8_T *) acl_name, acl_type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                                    (UI8_T *)acl_name,
                                                    acl_type,
                                                    TRUE,
                                                    NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                                    (UI8_T *)acl_name,
                                                    acl_type,
                                                    FALSE,
                                                    NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }

        for (i=1; i<=50; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }
    }

    return UT_OK;
}

int L4_MGR_TEST_Auto_Aggregate_AddACE_Fail_onFly()
{
    UI32_T  i;
    RULE_TYPE_Ace_Entry_T ace;

    {
        char                *acl_name   = "a1";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        UI32_T              uport_ifindex = 2;

        if (RULE_TYPE_OK != L4_MGR_ACL_CreateAcl((UI8_T *) acl_name, acl_type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                                    (UI8_T *)acl_name,
                                                    acl_type,
                                                    TRUE,
                                                    NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                                    (UI8_T *)acl_name,
                                                    acl_type,
                                                    FALSE,
                                                    NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }

        for (i=1; i<=50; i+=2)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }
    }

    return UT_OK;
}

int L4_MGR_TEST_Auto_Aggregate_DelACE_onFly()
{
    UI32_T  i;
    RULE_TYPE_Ace_Entry_T ace;

    {
        char                *acl_name   = "a1";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        UI32_T              uport_ifindex = 2;

        if (RULE_TYPE_OK != L4_MGR_ACL_CreateAcl((UI8_T *) acl_name, acl_type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                                    (UI8_T *)acl_name,
                                                    acl_type,
                                                    TRUE,
                                                    NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                                    (UI8_T *)acl_name,
                                                    acl_type,
                                                    FALSE,
                                                    NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }

        for (i=1; i<=50; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        for (i=1; i<=50; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            if (RULE_TYPE_OK != L4_MGR_ACL_DelAceFromAcl((UI8_T *)acl_name, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }
    }

    return UT_OK;
}

int L4_MGR_TEST_Auto_Aggregate_DelACL_onFly()
{
    UI32_T  i;
    RULE_TYPE_Ace_Entry_T ace;

    {
        char                *acl_name   = "a1";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        UI32_T              uport_ifindex = 2;

        if (RULE_TYPE_OK != L4_MGR_ACL_CreateAcl((UI8_T *) acl_name, acl_type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                                    (UI8_T *)acl_name,
                                                    acl_type,
                                                    TRUE,
                                                    NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                                    (UI8_T *)acl_name,
                                                    acl_type,
                                                    FALSE,
                                                    NULL))
        {
            UT_LOG("Test Aggregate failed");
            return UT_FAIL;
        }

        for (i=1; i<=50; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        /*
        for (i=1; i<=50; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            if (RULE_TYPE_OK != L4_MGR_ACL_DelAceFromAcl((UI8_T *)acl_name, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }
        */
        if (RULE_TYPE_OK != L4_MGR_ACL_DelAcl((UI8_T *)acl_name))
        {
            UT_LOG("no access-list %s failed", acl_name);
            return UT_FAIL;
        }
    }

    return UT_OK;
}
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
int L4_MGR_TEST_MatchAllClassMap_part1()
{
    char    *policy_map_name = "p1";
    UI32_T  policy_map_index;

    char    *acl_name = "a1";

    if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
    {
        UT_LOG("Prepare policy-map failed");
        return UT_FAIL;
    }

    /* access mac a1
     *    permit any host 00-00-00-xx-xx-xx
     *    permit any host 00-00-00-yy-yy-yy
     *    deny any any
     */
    {
        //char                *acl_name = "a1";
        RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;

        if ((RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T*)acl_name, acl_type)) ||
            (RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T*)acl_name, &acl_index))
            )
        {
            UT_LOG("Prepare ACL failed");
            return 1;
        }

        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

            if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
            ace_entry.u.mac.aceDestMacAddr[3] = 0x11;
            ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
            ace_entry.u.mac.aceDestMacAddr[5] = 0x33;
            memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }
        }

        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

            if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
            ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
            ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
            ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
            memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }
        }

        {
            RULE_TYPE_Ace_Entry_T ace_entry = {0};

            ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

            if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            ace_entry.u.mac.access            = RULE_TYPE_ACE_DENY;

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }
        }
    }

    /* class-map c1
     *     match access-list a1
     *     match ip dscp 5
     */
    {
        char    *class_map_name = "c1";
        UI32_T  class_map_index;

        if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                    RULE_TYPE_CLASS_MAP_MATCH_ALL,
                                                    &class_map_index))
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        {
            RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
            UI32_T                         class_index;

            element_entry.class_type = RULE_TYPE_CLASS_ACL;
            strcpy((char*)element_entry.element.acl_name, acl_name);

            if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }
        }

        {
            RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
            UI32_T                         class_index;

            element_entry.class_type = RULE_TYPE_CLASS_MF;
            element_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
            element_entry.element.mf_entry.mf_value = 5;

            if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }
        }

        /* policy-map p1
         *     class c1
         */
        if (RULE_TYPE_OK != RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index))
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }
    }

    /* service out p1
     */
    {
        UI32_T uport_ifindex = 2;

        if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex,
                                                          (UI8_T*)policy_map_name,
                                                          RULE_TYPE_INBOUND))
        {
            UT_LOG("Bind policy-map failed");
            return 1;
        }
    }

    /* policy-map p2
     *     class c1
     * in et 1/3
     * service in p2
     * service out p2
     */
    {
        char    *policy_map_name = "p2";
        UI32_T  policy_map_index;
        char    *class_map_name = "c1";
        UI32_T  class_map_index;
        UI32_T  uport_ifindex = 3;

        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
        {
            UT_LOG("Prepare policy-map failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != RULE_OM_GetClassMapIndexByName((UI8_T *) class_map_name, &class_map_index))
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index))
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex,
                                                          (UI8_T*)policy_map_name,
                                                          RULE_TYPE_INBOUND))
        {
            UT_LOG("Bind policy-map failed");
            return 1;
        }

        if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex,
                                                          (UI8_T*)policy_map_name,
                                                          RULE_TYPE_OUTBOUND))
        {
            UT_LOG("Bind policy-map failed");
            return 1;
        }
    }

    return UT_OK;
}

int L4_MGR_TEST_MatchAllClassMap_part2()
{
    {
        char    *policy_map_name = "p1";
        UI32_T  policy_map_index;
        UI32_T  uport_ifindex = 2;

        if (RULE_TYPE_OK != RULE_OM_GetPolicyMapIdByName((UI8_T *)policy_map_name, &policy_map_index))
        {
            UT_LOG("Prepare Policy-map failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_QoS_UnBindPortFromPolicyMap(uport_ifindex, (UI8_T *)policy_map_name, RULE_TYPE_INBOUND))
        {
            UT_LOG("Test Unbind policy-map failed");
            return UT_FAIL;
        }
    }

    {
        char    *policy_map_name = "p2";
        UI32_T  policy_map_index;
        UI32_T  uport_ifindex = 3;

        if (RULE_TYPE_OK != RULE_OM_GetPolicyMapIdByName((UI8_T *)policy_map_name, &policy_map_index))
        {
            UT_LOG("Prepare Policy-map failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_QoS_UnBindPortFromPolicyMap(uport_ifindex, (UI8_T *)policy_map_name, RULE_TYPE_INBOUND))
        {
            UT_LOG("Test Unbind policy-map failed");
            return UT_FAIL;
        }

        if (RULE_TYPE_OK != L4_MGR_QoS_UnBindPortFromPolicyMap(uport_ifindex, (UI8_T *)policy_map_name, RULE_TYPE_OUTBOUND))
        {
            UT_LOG("Test Unbind policy-map failed");
            return UT_FAIL;
        }
    }

    return UT_OK;
}

static int L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_part1()
{
    {
        char    *policy_map_name = "p1";
        UI32_T  policy_map_index;

        char                *acl_name = "a1";
        RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;

        /* policy-map p1
         */
        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name,
                                                     &policy_map_index))
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        /* access mac a1
         *    permit any host 00-00-00-11-22-33
         *    deny any any
         */
        {
            //char                *acl_name = "a1";
            //RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
            //UI32_T              acl_index;

            if ((RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T*)acl_name, acl_type)) ||
                (RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T*)acl_name, &acl_index))
                )
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            {
                RULE_TYPE_Ace_Entry_T ace_entry;

                ace_entry.ace_type    = acl_type;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACL failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
                ace_entry.u.mac.aceDestMacAddr[3] = 0x11;
                ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
                ace_entry.u.mac.aceDestMacAddr[5] = 0x33;
                memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

                if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }
            }

            {
                RULE_TYPE_Ace_Entry_T ace_entry = {0};

                ace_entry.ace_type    = acl_type;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACL failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_DENY;

                if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }
            }
        }

        /* class-map c1
         *    match access-list a1
         *
         * in et 1/2
         * service input p1
         * service output p1
         */
        {
            char    *class_map_name = "c1";
            UI32_T  class_map_index;

            if (RULE_TYPE_OK != RULE_OM_Create_ClassMap((UI8_T*)class_map_name,
                                                        RULE_TYPE_CLASS_MAP_MATCH_ALL,
                                                        &class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_ACL;
                strcpy((char*)element_entry.element.acl_name, acl_name);

                if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &element_entry, &class_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }
            }

            if (RULE_TYPE_OK != RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            /* service in p1
             */
            {
                UI32_T uport_ifindex = 2;

                if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_INBOUND))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

            /* service out p1
             * we should create another PCL for engress policy-map
             */
            {
                UI32_T uport_ifindex = 2;

                if (RULE_TYPE_OK != L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, (UI8_T*)policy_map_name, RULE_TYPE_OUTBOUND))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

#if 0
            /*
             * Test path to RULE_CTRL_LocalAddAce2DevForQosPurpose and
             *              RULE_CTRL_LocalRemoveAceFromDevForQosPurpose
             *
             */
            {
                RULE_TYPE_Ace_Entry_T ace_entry;
                //UI32_T                ace_index;

                ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

                if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
                {
                    UT_LOG("Prepare ACE failed");
                    return 1;
                }

                ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
                ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
                ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
                ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
                memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

                if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T*)acl_name, acl_type, &ace_entry))
                {
                    UT_LOG("Test RULE_CTRL_LocalAddAce2DevForQosPurpose failed");
                    return 1;
                }

                if (RULE_TYPE_OK != L4_MGR_ACL_DelAceFromAcl((UI8_T*)acl_name, acl_type, &ace_entry))
                {
                    UT_LOG("Test RULE_CTRL_LocalRemoveAceFromDevForQosPurpose failed");
                    return 1;
                }
            }
#endif

        } /* class c1 */

    }

    return 0;
}

static int L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_part2()
{
    {
        char                *acl_name = "a1";
        RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;

        /*
         * Test path to RULE_CTRL_LocalAddAce2DevForQosPurpose and
         *              RULE_CTRL_LocalRemoveAceFromDevForQosPurpose
         *
         */
        {
            RULE_TYPE_Ace_Entry_T ace_entry;
            //UI32_T                ace_index;

            ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

            if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return 1;
            }

            ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
            ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
            ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
            ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
            memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

            if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T*)acl_name, acl_type, &ace_entry))
            {
                UT_LOG("Test RULE_CTRL_LocalAddAce2DevForQosPurpose failed");
                return 1;
            }

            //if (RULE_TYPE_OK != L4_MGR_ACL_DelAceFromAcl((UI8_T*)acl_name, acl_type, &ace_entry))
            //{
            //    UT_LOG("Test RULE_CTRL_LocalRemoveAceFromDevForQosPurpose failed");
            //    return 1;
            //}
        }
    }

    return 0;
}

static int L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_part2_1()
{
    /* class-map c1
     *     match ip dscp 5
     */
    {
        char *class_map_name = "c1";
        RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
        UI32_T                         class_index;

        element_entry.class_type = RULE_TYPE_CLASS_MF;
        element_entry.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
        element_entry.element.mf_entry.mf_value = 5;

        if (RULE_TYPE_OK != L4_MGR_QoS_AddElement2ClassMap((UI8_T *) class_map_name, &element_entry))
        {
            UT_LOG("Prepare class-map failed");
            return 1;
        }
    }

    return 0;
}

static int L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_part3()
{
    {
        char                *acl_name = "a1";
        RULE_TYPE_AclType_T acl_type  = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;

        /*
         * Test path to RULE_CTRL_LocalAddAce2DevForQosPurpose and
         *              RULE_CTRL_LocalRemoveAceFromDevForQosPurpose
         *
         */
        {
            RULE_TYPE_Ace_Entry_T ace_entry;
            //UI32_T                ace_index;

            ace_entry.ace_type    = RULE_TYPE_MAC_ACL;

            if (RULE_TYPE_OK != RULE_OM_InitAce(&ace_entry))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }

            ace_entry.u.mac.access            = RULE_TYPE_ACE_PERMIT;
            ace_entry.u.mac.aceDestMacAddr[3] = 0x33;
            ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
            ace_entry.u.mac.aceDestMacAddr[5] = 0x11;
            memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

//            if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl((UI8_T*)acl_name, acl_type, &ace_entry))
//            {
//                UT_LOG("Test RULE_CTRL_LocalAddAce2DevForQosPurpose failed");
//                return 1;
//            }

            if (RULE_TYPE_OK != L4_MGR_ACL_DelAceFromAcl((UI8_T*)acl_name, acl_type, &ace_entry))
            {
                UT_LOG("Test RULE_CTRL_LocalRemoveAceFromDevForQosPurpose failed");
                return UT_FAIL;
            }
        }
    }

    return 0;
}
#endif /* SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL */

static int L4_MGR_TEST_MatchAllClassMap_Setup()
{
    char *policy_map_name = "p1";
    char *class_map_name = "c1";
    char *acl_name = "a1";
    UI32_T  uport_ifindex;
    int res;

    L4_MGR_TEST_Setup();

    res = L4_MGR_QoS_CreatePolicyMapByName((UI8_T*)policy_map_name);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create policy-map %s failed", policy_map_name);
        return UT_FAIL;
    }

    res = L4_MGR_QoS_CreateClassMapByName((UI8_T *)class_map_name,
                                          RULE_TYPE_CLASS_MAP_MATCH_ALL);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create class-map %s failed", class_map_name);
        return UT_FAIL;
    }

    res = L4_MGR_ACL_CreateAcl((UI8_T *)acl_name,
                               RULE_TYPE_IP_EXT_ACL);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create access-list mac %s failed", acl_name);
        return UT_FAIL;
    }

    {
        RULE_TYPE_UI_ClassMapElement_T class_map_element;

        class_map_element.class_type = RULE_TYPE_CLASS_ACL;
        strcpy(class_map_element.element.acl_name, acl_name);

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name,
                                             &class_map_element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Match access-list %s failed", acl_name);
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T class_map_element;

        class_map_element.class_type = RULE_TYPE_CLASS_MF;
        class_map_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;
        class_map_element.element.mf_entry.mf_value = 1001;

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name,
                                             &class_map_element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Match access-list %s failed", acl_name);
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T class_map_element;

        class_map_element.class_type = RULE_TYPE_CLASS_MF;
        class_map_element.element.mf_entry.mf_type  = RULE_TYPE_MF_COS;
        class_map_element.element.mf_entry.mf_value = 4;

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name,
                                             &class_map_element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Match access-list %s failed", acl_name);
            return UT_FAIL;
        }
    }

    {
        res = L4_MGR_QoS_AddElement2PolicyMap((UI8_T *)policy_map_name,
                                              (UI8_T *)class_map_name,
                                              NULL,
                                              NULL);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Class %s failed", class_map_name);
            return UT_FAIL;
        }
    }

    return UT_OK;
}

static int L4_MGR_TEST_MatchAllClassMap_Mix_MF_ACL_Setup()
{
    char *policy_map_name = "p1";
    char *class_map_name = "c1";
    char *acl_name = "a1";
    UI32_T  uport_ifindex;
    int res;

    L4_MGR_TEST_Setup();

    res = L4_MGR_QoS_CreatePolicyMapByName((UI8_T*)policy_map_name);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create policy-map %s failed", policy_map_name);
        return UT_FAIL;
    }

    res = L4_MGR_QoS_CreateClassMapByName((UI8_T *)class_map_name,
                                          RULE_TYPE_CLASS_MAP_MATCH_ALL);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create class-map %s failed", class_map_name);
        return UT_FAIL;
    }

    res = L4_MGR_ACL_CreateAcl((UI8_T *)acl_name,
                               RULE_TYPE_IP_EXT_ACL);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create access-list mac %s failed", acl_name);
        return UT_FAIL;
    }

    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
        RULE_OM_InitAce(&ace_entry);

        SET_IP(ace_entry.u.ip.aceSourceIpAddr, 192, 168, 1, 1);
        SET_IP(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

        SET_IP(ace_entry.u.ip.aceDestIpAddr, 192, 168, 1, 10);
        SET_IP(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

        res = L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name,
                                    ace_entry.ace_type,
                                    &ace_entry);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Create ace failed");
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
        RULE_OM_InitAce(&ace_entry);

        SET_IP(ace_entry.u.ip.aceSourceIpAddr, 192, 168, 1, 2);
        SET_IP(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

        SET_IP(ace_entry.u.ip.aceDestIpAddr, 192, 168, 1, 11);
        SET_IP(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

        res = L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name,
                                    ace_entry.ace_type,
                                    &ace_entry);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Create ace failed");
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T class_map_element;

        class_map_element.class_type = RULE_TYPE_CLASS_ACL;
        strcpy(class_map_element.element.acl_name, acl_name);

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name,
                                             &class_map_element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Match access-list %s failed", acl_name);
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T class_map_element;

        class_map_element.class_type = RULE_TYPE_CLASS_MF;
        class_map_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;
        class_map_element.element.mf_entry.mf_value = 1001;

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name,
                                             &class_map_element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Match access-list %s failed", acl_name);
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T class_map_element;

        class_map_element.class_type = RULE_TYPE_CLASS_MF;
        class_map_element.element.mf_entry.mf_type  = RULE_TYPE_MF_COS;
        class_map_element.element.mf_entry.mf_value = 4;

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name,
                                             &class_map_element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Match access-list %s failed", acl_name);
            return UT_FAIL;
        }
    }

    {
        res = L4_MGR_QoS_AddElement2PolicyMap((UI8_T *)policy_map_name,
                                              (UI8_T *)class_map_name,
                                              NULL,
                                              NULL);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Class %s failed", class_map_name);
            return UT_FAIL;
        }
    }

    return UT_OK;
}

static int L4_MGR_TEST_MatchAllClassMap_BindPort()
{
    char *policy_map_name = "p1";
    char *class_map_name = "c1";
    char *acl_name = "a1";
    UI32_T  uport_ifindex;
    int res;

    uport_ifindex = 2;
    res = L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex,
                                        (UI8_T *)policy_map_name,
                                        RULE_TYPE_INBOUND);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Bind policy-map %s failed", policy_map_name);
        return UT_FAIL;
    }

    uport_ifindex = 3;
    res = L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex,
                                        (UI8_T *)policy_map_name,
                                        RULE_TYPE_OUTBOUND);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Bind policy-map %s failed", policy_map_name);
        return UT_FAIL;
    }
    return UT_OK;
}

static int L4_MGR_TEST_MatchAllClassMap_UnBindPort()
{
    char *policy_map_name = "p1";
    char *class_map_name = "c1";
    char *acl_name = "a1";
    UI32_T  uport_ifindex;
    int res;

    uport_ifindex = 2;
    res = L4_MGR_QoS_UnBindPortFromPolicyMap(uport_ifindex,
                                             (UI8_T *)policy_map_name,
                                             RULE_TYPE_INBOUND);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Bind policy-map %s failed", policy_map_name);
        return UT_FAIL;
    }

    uport_ifindex = 3;
    res = L4_MGR_QoS_UnBindPortFromPolicyMap(uport_ifindex,
                                             (UI8_T *)policy_map_name,
                                             RULE_TYPE_OUTBOUND);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Bind policy-map %s failed", policy_map_name);
        return UT_FAIL;
    }
    return UT_OK;
}

static int L4_MGR_TEST_BMT_TC03_Step3()
{
    char *policy_map_name = "p1";
    char *class_map_name = "c1";
    char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;
    UI32_T res;
    int i;

    res = L4_MGR_ACL_CreateAcl((UI8_T *)acl_name, acl_type);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("");
        return UT_FAIL;
    }

    for (i=1; i<=50; ++i)
    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
        RULE_OM_InitAce(&ace_entry);

        SET_IP(ace_entry.u.ip.aceDestIpAddr, 192, 168, 1, i);
        SET_IP(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);
        res = L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name, ace_entry.ace_type, &ace_entry);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("");
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
        RULE_OM_InitAce(&ace_entry);

        ace_entry.u.ip.access = RULE_TYPE_ACE_DENY;

        res = L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name, ace_entry.ace_type, &ace_entry);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("");
            return UT_FAIL;
        }
    }

    res = L4_MGR_QoS_CreateClassMapByName((UI8_T *)class_map_name, RULE_TYPE_CLASS_MAP_MATCH_ALL);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("");
        return UT_FAIL;
    }

    {
        RULE_TYPE_UI_ClassMapElement_T element;

        element.class_type = RULE_TYPE_CLASS_ACL;
        strcpy(element.element.acl_name, acl_name);

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name, &element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("");
            return UT_FAIL;
        }
    }

    res = L4_MGR_QoS_CreatePolicyMapByName((UI8_T *)policy_map_name);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("");
        return UT_FAIL;
    }

    res = L4_MGR_QoS_AddElement2PolicyMap((UI8_T *)policy_map_name, (UI8_T *) class_map_name,
                                          NULL, NULL);

    if (RULE_TYPE_OK != res)
    {
        UT_LOG("");
        return UT_FAIL;
    }

    {
        char *class_map_name = "cc1";
        char *acl_name = "v1";

        res = L4_MGR_ACL_CreateAcl((UI8_T *)acl_name, acl_type);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("");
            return UT_FAIL;
        }

        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
            RULE_OM_InitAce(&ace_entry);

            ace_entry.u.ip.access = RULE_TYPE_ACE_DENY;

            res = L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name, ace_entry.ace_type, &ace_entry);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("");
                return UT_FAIL;
            }
        }

        res = L4_MGR_QoS_CreateClassMapByName((UI8_T *)class_map_name, RULE_TYPE_CLASS_MAP_MATCH_ALL);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("");
            return UT_FAIL;
        }

        {
            RULE_TYPE_UI_ClassMapElement_T element;

            element.class_type = RULE_TYPE_CLASS_ACL;
            strcpy(element.element.acl_name, acl_name);

            res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name, &element);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("");
                return UT_FAIL;
            }
        }

        res = L4_MGR_QoS_AddElement2PolicyMap((UI8_T *)policy_map_name, (UI8_T *) class_map_name,
                                              NULL, NULL);

        if (RULE_TYPE_OK != res)
        {
            UT_LOG("");
            return UT_FAIL;
        }

    }

    res = L4_MGR_QoS_BindPort2PolicyMap(2, (UI8_T *) policy_map_name, RULE_TYPE_INBOUND);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("");
        return UT_FAIL;
    }

    return UT_OK;
}

static int L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_Setup()
{
    char *policy_map_name = "p1";
    char *class_map_name = "c1";
    char *acl_name = "a1";
    UI32_T  uport_ifindex;
    int res;

    L4_MGR_TEST_Setup();

    res = L4_MGR_QoS_CreatePolicyMapByName((UI8_T*)policy_map_name);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create policy-map %s failed", policy_map_name);
        return UT_FAIL;
    }

    res = L4_MGR_QoS_CreateClassMapByName((UI8_T *)class_map_name,
                                          RULE_TYPE_CLASS_MAP_MATCH_ALL);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create class-map %s failed", class_map_name);
        return UT_FAIL;
    }

    res = L4_MGR_ACL_CreateAcl((UI8_T *)acl_name,
                               RULE_TYPE_IP_EXT_ACL);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create access-list mac %s failed", acl_name);
        return UT_FAIL;
    }

    {
        RULE_TYPE_UI_ClassMapElement_T class_map_element;

        class_map_element.class_type = RULE_TYPE_CLASS_ACL;
        strcpy(class_map_element.element.acl_name, acl_name);

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name,
                                             &class_map_element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Match access-list %s failed", acl_name);
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T class_map_element;

        class_map_element.class_type = RULE_TYPE_CLASS_MF;
        class_map_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;
        class_map_element.element.mf_entry.mf_value = 1001;

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name,
                                             &class_map_element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Match access-list %s failed", acl_name);
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T class_map_element;

        class_map_element.class_type = RULE_TYPE_CLASS_MF;
        class_map_element.element.mf_entry.mf_type  = RULE_TYPE_MF_COS;
        class_map_element.element.mf_entry.mf_value = 4;

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *)class_map_name,
                                             &class_map_element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Match access-list %s failed", acl_name);
            return UT_FAIL;
        }
    }

    {
        res = L4_MGR_QoS_AddElement2PolicyMap((UI8_T *)policy_map_name,
                                              (UI8_T *)class_map_name,
                                              NULL,
                                              NULL);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Class %s failed", class_map_name);
            return UT_FAIL;
        }
    }

    uport_ifindex = 2;
    res = L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex,
                                        (UI8_T *)policy_map_name,
                                        RULE_TYPE_INBOUND);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Bind policy-map %s failed", policy_map_name);
        return UT_FAIL;
    }

    uport_ifindex = 3;
    res = L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex,
                                        (UI8_T *)policy_map_name,
                                        RULE_TYPE_OUTBOUND);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Bind policy-map %s failed", policy_map_name);
        return UT_FAIL;
    }

    return UT_OK;
}

int L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_AddAce()
{
    char *acl_name = "a1";
    UI32_T uport_ifindex = 2;
    UI32_T res;
    RULE_TYPE_Ace_Entry_T ace_entry;

    ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
    RULE_OM_InitAce(&ace_entry);

    SET_IP(ace_entry.u.ip.aceDestIpAddr, 192, 168, 1, 1);
    SET_IP(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

    res = L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name,
                                ace_entry.ace_type,
                                &ace_entry);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Add Ace to ACL failed");
        return UT_FAIL;
    }

    /* It will fail if add the ACE have in the ACL.
     * To check the rule will not effect by this failure.
     */
    res = L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name,
                                ace_entry.ace_type,
                                &ace_entry);
    if (RULE_TYPE_FAIL != res)
    {
        UT_LOG("Add Ace to ACL failed");
        return UT_FAIL;
    }

    return UT_OK;
}

int L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_AddAce2()
{
    char *acl_name = "a1";
    UI32_T uport_ifindex = 2;
    UI32_T res;
    RULE_TYPE_Ace_Entry_T ace_entry;

    ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
    RULE_OM_InitAce(&ace_entry);

    SET_IP(ace_entry.u.ip.aceDestIpAddr, 192, 168, 1, 2);
    SET_IP(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

    res = L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name,
                                ace_entry.ace_type,
                                &ace_entry);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Add Ace to ACL failed");
        return UT_FAIL;
    }

    /* rule will be compressed on chip
     */
    ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
    RULE_OM_InitAce(&ace_entry);

    SET_IP(ace_entry.u.ip.aceDestIpAddr, 192, 168, 1, 3);
    SET_IP(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

    res = L4_MGR_ACL_SetAce2Acl((UI8_T *)acl_name,
                                ace_entry.ace_type,
                                &ace_entry);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Add Ace to ACL failed");
        return UT_FAIL;
    }

    return UT_OK;
}

int L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_RemoveAce1()
{
    char *acl_name = "a1";
    UI32_T uport_ifindex = 2;
    UI32_T res;
    RULE_TYPE_Ace_Entry_T ace_entry;

    ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
    RULE_OM_InitAce(&ace_entry);

    SET_IP(ace_entry.u.ip.aceDestIpAddr, 192, 168, 1, 2);
    SET_IP(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

    res = L4_MGR_ACL_DelAceFromAcl((UI8_T *)acl_name,
                                   ace_entry.ace_type,
                                   &ace_entry);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Add Ace to ACL failed");
        return UT_FAIL;
    }

    /* After remove this ACE, remain ACE can be compressed
     */

    return UT_OK;
}

int L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_RemoveAce2()
{
    char *acl_name = "a1";
    UI32_T uport_ifindex = 2;
    UI32_T res;
    RULE_TYPE_Ace_Entry_T ace_entry;

    ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
    RULE_OM_InitAce(&ace_entry);

    SET_IP(ace_entry.u.ip.aceDestIpAddr, 192, 168, 1, 3);
    SET_IP(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

    res = L4_MGR_ACL_DelAceFromAcl((UI8_T *)acl_name,
                                   ace_entry.ace_type,
                                   &ace_entry);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Add Ace to ACL failed");
        return UT_FAIL;
    }

    /* After remove this ACE, remain ACE can be compressed
     */

    return UT_OK;
}

int L4_MGR_TEST_MatchAllClassMap_RemoveACL_onFly()
{
    char *acl_name = "a1";
    UI32_T res;

    res = L4_MGR_ACL_DelAcl((UI8_T *)acl_name);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("no access-list %s faile", acl_name);
        return UT_FAIL;
    }

    return UT_OK;
}

int L4_MGR_TEST_IP_Access_Group_Disappear()
{
    char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_STD_ACL;
    UI32_T res;

    res = L4_MGR_ACL_CreateAcl((UI8_T *) acl_name, acl_type);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Prepare ACL failed");
        return UT_FAIL;
    }

    {
        RULE_TYPE_Ace_Entry_T ace_entry;
        ace_entry.ace_type = acl_type;
        L4_MGR_ACL_InitAce(&ace_entry);

        ace_entry.u.ip.access = RULE_TYPE_ACE_DENY;

        res = L4_MGR_ACL_SetAce2Acl((UI8_T *) acl_name, acl_type, &ace_entry);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare ACE failed");
            return UT_FAIL;
        }
    }

    {
        UI32_T uport_ifindex = 1;
        BOOL_T ingress_flag = TRUE;
        res = L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                      (UI8_T *) acl_name,
                                      RULE_TYPE_IP_ACL,
                                      ingress_flag,
                                      NULL);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }
    }

    {
        UI32_T uport_ifindex;
        RULE_TYPE_InOutDirection_T inout_profile;
        UI32_T acl_index;
        UI32_T precedence;
        RULE_TYPE_UI_AclEntry_T acl_entry;
        UI16_T time_range_index;

        uport_ifindex = 1;
        inout_profile = RULE_TYPE_INBOUND;
        acl_index = 0;
        precedence = 0;
        while (RULE_TYPE_OK == L4_MGR_ACL_GetNextAclByPort(uport_ifindex, inout_profile,
                            &acl_index, &acl_entry, &precedence, &time_range_index))
        {
            UT_LOG("name=%s", acl_entry.acl_name);
        }
    }

    return UT_OK;
}

int L4_MGR_TEST_IP_Access_Group_Disappear2()
{
    char *acl_name = "101";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;
    UI32_T res;

    res = L4_MGR_ACL_CreateAcl((UI8_T *) acl_name, acl_type);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Prepare ACL failed");
        return UT_FAIL;
    }

    if (0) {
        RULE_TYPE_Ace_Entry_T ace_entry;
        ace_entry.ace_type = acl_type;
        L4_MGR_ACL_InitAce(&ace_entry);

        ace_entry.u.ip.access = RULE_TYPE_ACE_DENY;

        res = L4_MGR_ACL_SetAce2Acl((UI8_T *) acl_name, acl_type, &ace_entry);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare ACE failed");
            return UT_FAIL;
        }
    }

    {
        UI32_T uport_ifindex = 1;
        BOOL_T ingress_flag = TRUE;
        res = L4_MGR_ACL_BindPort2Acl(uport_ifindex,
                                      (UI8_T *) acl_name,
                                      RULE_TYPE_IP_ACL,
                                      ingress_flag,
                                      NULL);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }
    }

    {
        UI32_T uport_ifindex;
        RULE_TYPE_InOutDirection_T inout_profile;
        UI32_T acl_index;
        UI32_T precedence;
        RULE_TYPE_UI_AclEntry_T acl_entry;
        UI16_T time_range_index;

        uport_ifindex = 1;
        inout_profile = RULE_TYPE_INBOUND;
        acl_index = 0;
        precedence = 0;
        while (RULE_TYPE_OK == L4_MGR_ACL_GetNextAclByPort(uport_ifindex, inout_profile,
                            &acl_index, &acl_entry, &precedence, &time_range_index))
        {
            UT_LOG("name=%s", acl_entry.acl_name);
        }
    }

    return UT_OK;
}

int L4_MGR_TEST_No_Match_Cos()
{
    char *class_map_name = "c1";
    UI32_T res;

    res = L4_MGR_QoS_CreateClassMapByName((UI8_T *) class_map_name);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Prepare class-map failed");
        return UT_FAIL;
    }

    {
        RULE_TYPE_UI_ClassMapElement_T element;

        element.class_type = RULE_TYPE_CLASS_MF;
        element.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
        element.element.mf_entry.mf_value = 5;

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *) class_map_name,  &element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        res = L4_MGR_QoS_DelElementFromClassMap((UI8_T *) class_map_name,  &element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

    }

    return UT_OK;
}

int L4_MGR_TEST_No_Match_Source_Port()
{
    char *class_map_name = "c1";
    UI32_T res;

    res = L4_MGR_QoS_CreateClassMapByName((UI8_T *) class_map_name);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Prepare class-map failed");
        return UT_FAIL;
    }

    {
        RULE_TYPE_UI_ClassMapElement_T element;

        element.class_type = RULE_TYPE_CLASS_MF;
        element.element.mf_entry.mf_type = RULE_TYPE_MF_SOURCE_PORT;
        element.element.mf_entry.mf_value = 5;

        res = L4_MGR_QoS_AddElement2ClassMap((UI8_T *) class_map_name,  &element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        //res = L4_MGR_QoS_DelElementFromClassMap((UI8_T *) class_map_name,  &element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

    }

    return UT_OK;
}

static int L4_MGR_TEST_No_IP_Access_Group()
{
    char *acl_name = "201";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;
    char *policy_map_name = "201";
    char *class_map_name = "201";
    UI32_T res;

    res = L4_MGR_ACL_CreateAcl((char *)acl_name, acl_type);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Prepare acl failed");
        return UT_FAIL;
    }

    {
    #define SET_IP(ip, b1, b2, b3, b4)  \
        { ((UI8_T*)&ip)[0]=b1; ((UI8_T*)&ip)[1]=b2; ((UI8_T*)&ip)[2]=b3; ((UI8_T*)&ip)[3]=b4; }

        RULE_TYPE_Ace_Entry_T ace_entry;
        UI32_T i;

        for (i=1; i<= 50; i++)
        {
            ace_entry.ace_type = acl_type;
            L4_MGR_ACL_InitAce(&ace_entry);

            SET_IP(ace_entry.u.ip.aceDestIpAddr, 192, 168, 1, i);
            SET_IP(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

            res = L4_MGR_ACL_SetAce2Acl((char *) acl_name, acl_type, &ace_entry);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare ace failed");
                return UT_FAIL;
            }
        }

        ace_entry.ace_type = acl_type;
        L4_MGR_ACL_InitAce(&ace_entry);

        ace_entry.u.ip.access = RULE_TYPE_ACE_DENY;

        res = L4_MGR_ACL_SetAce2Acl((char *) acl_name, acl_type, &ace_entry);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare ace failed");
            return UT_FAIL;
        }

    #undef SET_IP
    }

    res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create class-map failed");
        return UT_FAIL;
    }

    {
        RULE_TYPE_UI_ClassMapElement_T element;

        element.class_type = RULE_TYPE_CLASS_ACL;
        strcpy(element.element.acl_name, acl_name);

        res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }
    }

    res = L4_MGR_QoS_CreatePolicyMapByName(policy_map_name);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Create policy-map failed");
        return UT_FAIL;
    }

    {
        //RULE_TYPE_UI_Action_T action = {0};

        //action.in_action_type =

        res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name, class_map_name, NULL, NULL);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare policy-map failed");
            return UT_FAIL;
        }
    }

    res = L4_MGR_ACL_BindPort2Acl(1, (char *) acl_name, RULE_TYPE_IP_ACL, TRUE, NULL);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Bind acl failed");
        return UT_FAIL;
    }

    res = L4_MGR_QoS_BindPort2PolicyMap(1, policy_map_name, RULE_TYPE_INBOUND);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Bind policy-map failed");
        return UT_FAIL;
    }

    res = L4_MGR_ACL_UnBindPortFromAcl(1, (char *) acl_name, RULE_TYPE_IP_ACL, TRUE);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Unbind acl failed");
        return UT_FAIL;
    }

    res = L4_MGR_QoS_UnBindPortFromPolicyMap(1, policy_map_name, RULE_TYPE_INBOUND);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Unbind acl failed");
        return UT_FAIL;
    }

    res = L4_MGR_QoS_BindPort2PolicyMap(1, policy_map_name, RULE_TYPE_INBOUND);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Bind policy-map failed");
        return UT_FAIL;
    }

    /*
    res = L4_MGR_ACL_BindPort2Acl(1, (char *) acl_name, RULE_TYPE_IP_ACL, TRUE, NULL);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Bind acl failed");
        return UT_FAIL;
    }
    */

    return UT_OK;
}

static int L4_MGR_TEST_Match_Cos_Failed()
{
    char *class_map_name = "c1";
    UI32_T res;

    res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Prepare class-map failed");
        return UT_FAIL;
    }

    {
        UI32_T class_map_index;

        res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index, VAL_diffServClassMapMatchType_matchAll);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T element;

        element.class_type = RULE_TYPE_CLASS_MF;
        element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
        element.element.mf_entry.mf_value = 1001;
        res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T element;

        element.class_type = RULE_TYPE_CLASS_MF;
        element.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
        element.element.mf_entry.mf_value = 0;
        res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T element;

        element.class_type = RULE_TYPE_CLASS_MF;
        element.element.mf_entry.mf_type = RULE_TYPE_MF_SOURCE_PORT;
        element.element.mf_entry.mf_value = 0;
        res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }
    }

    {
        RULE_TYPE_UI_ClassMapElement_T element;

        element.class_type = RULE_TYPE_CLASS_MF;
        element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
        element.element.mf_entry.mf_value = 0;
        res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }
    }

    return UT_OK;
}

#if 0
static int L4_MGR_TEST_GetMatchAnyClassMapInstance_1()
{
    UI32_T res;

    /*
        access-list mac a1 <-- first class instance
            permit xx      <-- first rule instance
            deny yy

        class-map c1 match-any
            match cos 0

        policy-map p1
            class c1
                police fl 50M
     */
    {
        char *policy_map_name = "p1";
        RULE_TYPE_InOutDirection_T  inout_profile = RULE_TYPE_INBOUND;
        UI32_T uport_ifindex = 2;
        UI32_T class_map_index;
        UI32_T dev_rule_id;
        UI32_T dev_rule_id_2;

        res = L4_MGR_QoS_CreatePolicyMapByName(policy_map_name);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        {
            char *class_map_name = "c1";
            RULE_TYPE_UI_ClassMapElement_T element;

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAny);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                char                    *acl_name   = "a1";
                UI32_T                  acl_index;
                RULE_TYPE_AclType_T     acl_type    = RULE_TYPE_MAC_ACL;
                RULE_TYPE_Ace_Entry_T   ace;

                if (RULE_TYPE_OK != L4_MGR_ACL_CreateAcl((UI8_T *) acl_name, acl_type) ||
                    RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
                {
                    UT_LOG("Prepare ACL failed");
                    return UT_FAIL;
                }

                ace.ace_type = RULE_TYPE_MAC_ACL;
                RULE_OM_InitAce(&ace);

                ace.u.mac.access = VAL_diffServMacAceAccess_permit;
                SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, 0x01);
                SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

                if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace))
                {
                    UT_LOG("Prepare ACE failed");
                    return UT_FAIL;
                }

                ace.ace_type = RULE_TYPE_MAC_ACL;
                RULE_OM_InitAce(&ace);

                ace.u.mac.access = VAL_diffServMacAceAccess_deny;
                SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, 0x00);
                SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

                if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace))
                {
                    UT_LOG("Prepare ACE failed");
                    return UT_FAIL;
                }

                element.class_type = RULE_TYPE_CLASS_ACL;
                strcpy(element.element.acl_name, acl_name);
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
                element.element.mf_entry.mf_value = 0;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 50000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

        }

        res = L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex,
                                            policy_map_name,
                                            inout_profile);
        if (res != RULE_TYPE_OK)
        {
            UT_LOG("Bind policy-map failed");
            return UT_FAIL;
        }

        dev_rule_id = RULE_OM_GetFirstDevRuleIDofOneClassMap(uport_ifindex,
                                                             class_map_index,
                                                             inout_profile);
        //UT_LOG("dev_rule_id=%lu", dev_rule_id);

        dev_rule_id_2 = RULE_OM_GetFirstDevRuleIDofOneClassMap_2(uport_ifindex,
                                                                 class_map_index,
                                                                 inout_profile);
        if (dev_rule_id != dev_rule_id_2)
        {
            UT_LOG("RULE_OM_GetFirstDevRuleIDofOneClassMap_2 (dev_rule_id=%lu) failed",
                dev_rule_id_2);
            return UT_FAIL;
        }

    }

    return UT_OK;
}

static int L4_MGR_TEST_GetMatchAnyClassMapInstance_2()
{
    UI32_T res;

    /*
        access-list mac a1 <-- first class instance
            deny xx        <-- first rule instance (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL == TRUE)
            permit yy      <-- first rule instance (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL == FALSE)

        class-map c1 match-any
            match cos 0

        policy-map p1
            class c1
                police fl 50M
     */
    {
        char *policy_map_name = "p1";
        RULE_TYPE_InOutDirection_T  inout_profile = RULE_TYPE_INBOUND;
        UI32_T uport_ifindex = 2;
        UI32_T class_map_index;
        UI32_T dev_rule_id;
        UI32_T dev_rule_id_2;

        res = L4_MGR_QoS_CreatePolicyMapByName(policy_map_name);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        {
            char *class_map_name = "c1";
            RULE_TYPE_UI_ClassMapElement_T element;

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAny);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                char                    *acl_name   = "a1";
                UI32_T                  acl_index;
                RULE_TYPE_AclType_T     acl_type    = RULE_TYPE_MAC_ACL;
                RULE_TYPE_Ace_Entry_T   ace;

                if (RULE_TYPE_OK != L4_MGR_ACL_CreateAcl((UI8_T *) acl_name, acl_type) ||
                    RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
                {
                    UT_LOG("Prepare ACL failed");
                    return UT_FAIL;
                }

                ace.ace_type = RULE_TYPE_MAC_ACL;
                RULE_OM_InitAce(&ace);

                ace.u.mac.access = VAL_diffServMacAceAccess_deny;
                SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, 0x01);
                SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

                if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace))
                {
                    UT_LOG("Prepare ACE failed");
                    return UT_FAIL;
                }

                ace.ace_type = RULE_TYPE_MAC_ACL;
                RULE_OM_InitAce(&ace);

                ace.u.mac.access = VAL_diffServMacAceAccess_permit;
                SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, 0x00);
                SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

                if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace))
                {
                    UT_LOG("Prepare ACE failed");
                    return UT_FAIL;
                }

                element.class_type = RULE_TYPE_CLASS_ACL;
                strcpy(element.element.acl_name, acl_name);
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
                element.element.mf_entry.mf_value = 0;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 50000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

        }

        res = L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex,
                                            policy_map_name,
                                            inout_profile);
        if (res != RULE_TYPE_OK)
        {
            UT_LOG("Bind policy-map failed");
            return UT_FAIL;
        }

        dev_rule_id = RULE_OM_GetFirstDevRuleIDofOneClassMap(uport_ifindex,
                                                             class_map_index,
                                                             inout_profile);
        //UT_LOG("dev_rule_id=%lu", dev_rule_id);

        dev_rule_id_2 = RULE_OM_GetFirstDevRuleIDofOneClassMap_2(uport_ifindex,
                                                                 class_map_index,
                                                                 inout_profile);
        if (dev_rule_id != dev_rule_id_2)
        {
            UT_LOG("RULE_OM_GetFirstDevRuleIDofOneClassMap_2 (dev_rule_id=%lu) failed",
                dev_rule_id_2);
            return UT_FAIL;
        }

    }

    return UT_OK;
}

static int L4_MGR_TEST_GetMatchAnyClassMapInstance_3()
{
    UI32_T res;

    /*
        access-list mac a1 <-- first class instance
            deny xx        <-- first rule instance (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL == TRUE)
            permit yy

        class-map c1 match-any
            match cos 0    <-- first rule instance (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL == FALSE)

        policy-map p1
            class c1
                police fl 50M
     */
    {
        char *policy_map_name = "p1";
        RULE_TYPE_InOutDirection_T  inout_profile = RULE_TYPE_INBOUND;
        UI32_T uport_ifindex = 2;
        UI32_T class_map_index;
        UI32_T dev_rule_id;
        UI32_T dev_rule_id_2;

        res = L4_MGR_QoS_CreatePolicyMapByName(policy_map_name);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        {
            char *class_map_name = "c1";
            RULE_TYPE_UI_ClassMapElement_T element;

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAny);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                char                    *acl_name   = "a1";
                UI32_T                  acl_index;
                RULE_TYPE_AclType_T     acl_type    = RULE_TYPE_MAC_ACL;
                RULE_TYPE_Ace_Entry_T   ace;

                if (RULE_TYPE_OK != L4_MGR_ACL_CreateAcl((UI8_T *) acl_name, acl_type) ||
                    RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
                {
                    UT_LOG("Prepare ACL failed");
                    return UT_FAIL;
                }

                ace.ace_type = RULE_TYPE_MAC_ACL;
                RULE_OM_InitAce(&ace);

                ace.u.mac.access = VAL_diffServMacAceAccess_deny;
                SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, 0x01);
                SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

                if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace))
                {
                    UT_LOG("Prepare ACE failed");
                    return UT_FAIL;
                }

                ace.ace_type = RULE_TYPE_MAC_ACL;
                RULE_OM_InitAce(&ace);

                ace.u.mac.access = VAL_diffServMacAceAccess_deny;
                SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, 0x00);
                SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

                if (RULE_TYPE_OK != L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace))
                {
                    UT_LOG("Prepare ACE failed");
                    return UT_FAIL;
                }

                element.class_type = RULE_TYPE_CLASS_ACL;
                strcpy(element.element.acl_name, acl_name);
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
                element.element.mf_entry.mf_value = 0;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 50000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

        }

        res = L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex,
                                            policy_map_name,
                                            inout_profile);
        if (res != RULE_TYPE_OK)
        {
            UT_LOG("Bind policy-map failed");
            return UT_FAIL;
        }

        dev_rule_id = RULE_OM_GetFirstDevRuleIDofOneClassMap(uport_ifindex,
                                                             class_map_index,
                                                             inout_profile);
        //UT_LOG("dev_rule_id=%lu", dev_rule_id);

        dev_rule_id_2 = RULE_OM_GetFirstDevRuleIDofOneClassMap_2(uport_ifindex,
                                                                 class_map_index,
                                                                 inout_profile);
        if (dev_rule_id != dev_rule_id_2)
        {
            UT_LOG("RULE_OM_GetFirstDevRuleIDofOneClassMap_2 (dev_rule_id=%lu) failed",
                dev_rule_id_2);
            return UT_FAIL;
        }

    }

    return UT_OK;
}
#endif

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
static int L4_MGR_TEST_BundleRateControl()
{
    UI32_T res;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    //
    // Downstream Bundle Rate Control
    //
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    {
        char *policy_map_name = "p_mix";
        UI32_T uport_ifinex = 10;
        RULE_TYPE_InOutDirection_T  inout_profile = RULE_TYPE_INBOUND;
        RULE_TYPE_UI_BundleGroup_T  bundle_group = {0};
        UI32_T class_cnt = 0;

        res = L4_MGR_QoS_CreatePolicyMapByName(policy_map_name);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        {
            char *class_map_name = "c_match_cos_0_vlan_1005";

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                UI32_T class_map_index;

                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAll);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
                element.element.mf_entry.mf_value = 0;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
                element.element.mf_entry.mf_value = 1005;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 50000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            strncpy(bundle_group.class_map_name_list[class_cnt],
                    class_map_name,
                    SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
            bundle_group.class_map_name_list[class_cnt][SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';
            bundle_group.class_map_count = ++class_cnt;

        } // class-map c_match_cos_0_vlan_1005

        {
            char *class_map_name = "c_match_cos_4_vlan_1005";

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                UI32_T class_map_index;

                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAll);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
                element.element.mf_entry.mf_value = 4;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
                element.element.mf_entry.mf_value = 43;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 15000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            strncpy(bundle_group.class_map_name_list[class_cnt],
                    class_map_name,
                    SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
            bundle_group.class_map_name_list[class_cnt][SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';
            bundle_group.class_map_count = ++class_cnt;

        } // class-map c_match_cos_4_vlan_43

        {
            char *class_map_name = "c_match_cos_4_vlan_4001";

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                UI32_T class_map_index;

                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAll);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
                element.element.mf_entry.mf_value = 4;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
                element.element.mf_entry.mf_value = 4001;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 15000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            strncpy(bundle_group.class_map_name_list[class_cnt],
                    class_map_name,
                    SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
            bundle_group.class_map_name_list[class_cnt][SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';
            bundle_group.class_map_count = ++class_cnt;

            bundle_group.bundle_rate_kbps = 50000;

            res = L4_MGR_QoS_SetBundleGroup(policy_map_name,
                                            1,
                                            &bundle_group);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Set bundle group failed");
                return UT_FAIL;
            }

            memset(&bundle_group, 0, sizeof(bundle_group));
            class_cnt = 0;

        } // class-map c_match_cos_4_vlan_4001

        {
            char *class_map_name = "c_match_cos_0";

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                UI32_T class_map_index;

                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAll);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
                element.element.mf_entry.mf_value = 0;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 50000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            strncpy(bundle_group.class_map_name_list[class_cnt],
                    class_map_name,
                    SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
            bundle_group.class_map_name_list[class_cnt][SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';
            bundle_group.class_map_count = ++class_cnt;

        } // class-map c_match_cos_0

        {
            char *class_map_name = "c_match_cos_4";

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                UI32_T class_map_index;

                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAll);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_COS;
                element.element.mf_entry.mf_value = 4;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 15000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            strncpy(bundle_group.class_map_name_list[class_cnt],
                    class_map_name,
                    SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
            bundle_group.class_map_name_list[class_cnt][SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';
            bundle_group.class_map_count = ++class_cnt;

            bundle_group.bundle_rate_kbps = 50000;

            res = L4_MGR_QoS_SetBundleGroup(policy_map_name,
                                            2,
                                            &bundle_group);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Set bundle group failed");
                return UT_FAIL;
            }

            memset(&bundle_group, 0, sizeof(bundle_group));
            class_cnt = 0;

        } // class-map c_match_cos_4

#if 0
        {
            char *class_map_name = "c_match_vlan_1001";

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                UI32_T class_map_index;

                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAll);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
                element.element.mf_entry.mf_value = 1001;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 50000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            strncpy(bundle_group.class_map_name_list[class_cnt],
                    class_map_name,
                    SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
            bundle_group.class_map_name_list[class_cnt][SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';
            bundle_group.class_map_count = ++class_cnt;

        } // class-map c_match_vlan_1001

        {
            char *class_map_name = "c_match_vlan_41";

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                UI32_T class_map_index;

                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAll);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
                element.element.mf_entry.mf_value = 43;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 15000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            strncpy(bundle_group.class_map_name_list[class_cnt],
                    class_map_name,
                    SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
            bundle_group.class_map_name_list[class_cnt][SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';
            bundle_group.class_map_count = ++class_cnt;

        } // class-map c_match_vlan_43

        {
            char *class_map_name = "c_match_vlan_4001";

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                UI32_T class_map_index;

                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAll);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
                element.element.mf_entry.mf_value = 4001;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 15000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            strncpy(bundle_group.class_map_name_list[class_cnt],
                    class_map_name,
                    SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
            bundle_group.class_map_name_list[class_cnt][SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';
            bundle_group.class_map_count = ++class_cnt;

            bundle_group.bundle_rate_kbps = 50000;

            res = L4_MGR_QoS_SetBundleGroup(policy_map_name,
                                            3,
                                            &bundle_group);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Set bundle group failed");
                return UT_FAIL;
            }

            memset(&bundle_group, 0, sizeof(bundle_group));
            class_cnt = 0;

        } // class-map c_match_vlan_4001
#endif

        res = L4_MGR_QoS_BindPort2PolicyMap(uport_ifinex,
                                            policy_map_name,
                                            inout_profile);
        if (res != RULE_TYPE_OK)
        {
            UT_LOG("Bind policy-map failed");
            return UT_FAIL;
        }

    } // policy-map p_mix

#if 0
    {
        char *policy_map_name = "p_b";

        res = L4_MGR_QoS_CreatePolicyMapByName(policy_map_name);
        if (RULE_TYPE_OK != res)
        {
            UT_LOG("Prepare class-map failed");
            return UT_FAIL;
        }

        {
            char *class_map_name = "c_match_src_port_10";

            res = L4_MGR_QoS_CreateClassMapByName(class_map_name);
            if (RULE_TYPE_OK != res)
            {
                UT_LOG("Prepare class-map failed");
                return UT_FAIL;
            }

            {
                UI32_T class_map_index;

                res = L4_MGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }

                res = L4_MGR_QoS_SetDiffServClassMapMatchType(class_map_index,
                                                VAL_diffServClassMapMatchType_matchAll);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element;

                element.class_type = RULE_TYPE_CLASS_MF;
                element.element.mf_entry.mf_type = RULE_TYPE_MF_SOURCE_PORT;
                element.element.mf_entry.mf_value = 10;
                res = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &element);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }

            {
                RULE_TYPE_TBParamEntry_T meter_entry = {0};
                RULE_TYPE_UI_Action_T    action_entry = {0};

                action_entry.in_action_type = IN_ACTION_INVALID;
                action_entry.in_action_value = 8;
                action_entry.confirm_action_type = CONFIRM_ACTION_INVALID;
                action_entry.confirm_action_value = 64;
                action_entry.exceed_action_type = EXCEED_ACTION_INVALID;
                action_entry.exceed_action_value = 64;
                action_entry.violate_action_type = VIOLATE_ACTION_INVALID;
                action_entry.violate_action_value = 64;

                meter_entry.row_status = VAL_diffServMeterStatus_active;
                meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
                meter_entry.rate = 50000;
                meter_entry.burst_size = 4000;

                action_entry.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
                action_entry.violate_action_type = VIOLATE_ACTION_DROP;

                res = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name,
                                                      class_map_name,
                                                      &meter_entry,
                                                      &action_entry);
                if (RULE_TYPE_OK != res)
                {
                    UT_LOG("Prepare class-map failed");
                    return UT_FAIL;
                }
            }
        } // class-map c_match_src_port_10

        res = L4_MGR_QoS_BindPort2PolicyMap(1, policy_map_name,
                                            RULE_TYPE_OUTBOUND);
        if (res != RULE_TYPE_OK)
        {
            UT_LOG("Bind policy-map failed");
            return UT_FAIL;
        }

        res = L4_MGR_QoS_BindPort2PolicyMap(2, policy_map_name,
                                            RULE_TYPE_OUTBOUND);
        if (res != RULE_TYPE_OK)
        {
            UT_LOG("Bind policy-map failed");
            return UT_FAIL;
        }

        res = L4_MGR_QoS_BindPort2PolicyMap(3, policy_map_name,
                                            RULE_TYPE_OUTBOUND);
        if (res != RULE_TYPE_OK)
        {
            UT_LOG("Bind policy-map failed");
            return UT_FAIL;
        }
    } // policy-map p_b
#endif

    if (0)
    {
        UI32_T green = 5000;
        UI32_T last_g= 4000;
        UI32_T tick  = 3100;
        UI32_T last_t= 3000;

        UI32_T r1 =          (green-last_g) * 8 /  1000  /  (tick - last_t) / 100;
        UI32_T r2 =         ((green-last_g) * 8) / 1000  /  (tick - last_t) / 100;
        UI32_T r3 =        (((green-last_g) * 8) / 1000) / ((tick - last_t) / 100);
        UI32_T r4 = (float)(((green-last_g) * 8) / 1000) / ((tick - last_t) / 100);
        float  r5 = (float)(((green-last_g) * 8) / 1000) / ((tick - last_t) / 100);

        printf("r1=%lu KB \r\n", r1);
        printf("r2=%lu KB \r\n", r2);
        printf("r3=%lu KB \r\n", r3);
        printf("r4=%lu KB \r\n", r4);
        printf("r5=%f  KB \r\n", r5);
    }

    if (0)
    {
        UI32_T green = 6000;
        UI32_T last_g= 4000;
        UI32_T tick  = 3500;
        UI32_T last_t= 3000;

        UI32_T r1 =          (green-last_g) * 8 /  1000  /  (tick - last_t) / 100;
        UI32_T r2 =         ((green-last_g) * 8) / 1000  /  (tick - last_t) / 100;
        UI32_T r3 =        (((green-last_g) * 8) / 1000) / ((tick - last_t) / 100);
        UI32_T r4 = (float)(((green-last_g) * 8) / 1000) / ((tick - last_t) / 100);
        float  r5 = (float)(((green-last_g) * 8) / 1000) / ((tick - last_t) / 100);

        printf("r1=%lu KB \r\n", r1);
        printf("r2=%lu KB \r\n", r2);
        printf("r3=%lu KB \r\n", r3);
        printf("r4=%lu KB \r\n", r4);
        printf("r5=%f  KB \r\n", r5);
    }


    {
        UI32_T i;
        for (i=0; i < 100 ; i++)
        L4_MGR_ACL_ProcessTimerEvent();
    }

    return UT_OK;
}
#endif /* #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) */

static int L4_MGR_TEST_SetRule_TrapMyMacArp_ToCPU()
{
    if (RULE_TYPE_OK != L4_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_MY_MAC_ARP, NULL))
    {
        UT_LOG("Set rule for Trap MyMAC and MyIP failed");
        return UT_FAIL;
    }

    return UT_OK;
}

static int L4_MGR_TEST_SetRule_TrapRip_ToCPU()
{
    if (RULE_TYPE_OK != L4_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_RIP, NULL))
    {
        UT_LOG("Set rule for Trap RIP failed");
        return UT_FAIL;
    }

    return UT_OK;
}

int L4_MGR_TEST_SetRule_SetToHiQueue_For_PTP()
{
    //if (RULE_TYPE_OK != L4_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_PTP_L2, NULL))
    //{
    //    UT_LOG("Set rule for Trap RIP failed");
    //    return 1;
    //}

    if (RULE_TYPE_OK != L4_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_PTP_EVENT, NULL))
    {
        UT_LOG("Set rule for Trap RIP failed");
        return UT_FAIL;
    }

    return UT_OK;
}

int L4_MGR_TEST_SetRule_IgmpToCpu()
{
    if (RULE_TYPE_OK != L4_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IGMP, NULL))
    {
        UT_LOG("Trap IGMP to CPU failed");
        return UT_FAIL;
    }

    return UT_OK;
}

#if 0
int L4_MGR_TEST_SetRule_IpSubnetVlanPerPort()
{
#define SET_IP(ip, b1, b2, b3, b4)  \
    { ((UI8_T*)&ip)[0]=b1; ((UI8_T*)&ip)[1]=b2; ((UI8_T*)&ip)[2]=b3; ((UI8_T*)&ip)[3]=b4; }

    UI32_T ip;
    UI32_T mask;
    BOOL_T res;

    SET_IP(ip, 192, 168, 1, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_SetIpSubnetVlanEntryPerPort(1, 2, ip, mask, 101, 99);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }

    SET_IP(ip, 192, 168, 1, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_SetIpSubnetVlanEntryPerPort(1, 3, ip, mask, 101, 99);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }

    SET_IP(ip, 192, 168, 2, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_SetIpSubnetVlanEntryPerPort(1, 3, ip, mask, 102, 99);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }

    SET_IP(ip, 192, 168, 3, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_SetIpSubnetVlanEntryPerPort(1, 3, ip, mask, 102, 99);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }

    return UT_OK;
}

int L4_MGR_TEST_SetRule_RemoveIpSubnetVlanPerPort_1()
{
#define SET_IP(ip, b1, b2, b3, b4)  \
    { ((UI8_T*)&ip)[0]=b1; ((UI8_T*)&ip)[1]=b2; ((UI8_T*)&ip)[2]=b3; ((UI8_T*)&ip)[3]=b4; }

    UI32_T ip;
    UI32_T mask;
    BOOL_T res;

    SET_IP(ip, 192, 168, 1, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_DeleteIpSubnetVlanEntryPerPort(1, 2, ip, mask);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }
/*
    SET_IP(ip, 192, 168, 1, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_DeleteIpSubnetVlanEntryPerPort(1, 3, ip, mask);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }

    SET_IP(ip, 192, 168, 2, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_DeleteIpSubnetVlanEntryPerPort(1, 3, ip, mask);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }

    SET_IP(ip, 192, 168, 2, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_DeleteIpSubnetVlanEntryPerPort(1, 3, ip, mask);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }
*/
    return UT_OK;
}

int L4_MGR_TEST_SetRule_RemoveIpSubnetVlanPerPort_2()
{
#define SET_IP(ip, b1, b2, b3, b4)  \
    { ((UI8_T*)&ip)[0]=b1; ((UI8_T*)&ip)[1]=b2; ((UI8_T*)&ip)[2]=b3; ((UI8_T*)&ip)[3]=b4; }

    UI32_T ip;
    UI32_T mask;
    BOOL_T res;
/*
    SET_IP(ip, 192, 168, 1, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_DeleteIpSubnetVlanEntryPerPort(1, 2, ip, mask);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }
*/
    SET_IP(ip, 192, 168, 1, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_DeleteIpSubnetVlanEntryPerPort(1, 3, ip, mask);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }

    SET_IP(ip, 192, 168, 2, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_DeleteIpSubnetVlanEntryPerPort(1, 3, ip, mask);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }

    SET_IP(ip, 192, 168, 2, 0);
    SET_IP(mask, 255, 255, 255, 0);

    res = RULE_CTRL_DeleteIpSubnetVlanEntryPerPort(1, 3, ip, mask);
    if (TRUE != res)
    {
        UT_LOG("Set IP Subnet VLAN Per Port failed");
        return UT_FAIL;
    }

    return UT_OK;
}

int L4_MGR_TEST_SetRule_SetIcmpv6Type()
{
    UI32_T res;

    res = RULE_CTRL_DenyIcmpv6Type(TRUE, 1, 2);
    if (RULE_TYPE_OK != res)
    {
        UT_LOG("Set ICMPv6 type failed");
        return UT_FAIL;
    }

    return UT_OK;
}
#endif

#include "l4_backdoor.h"
BOOL_T L4_MGR_TEST_Main()
{
    int res;

    UT_Init();

#ifdef MARVELL_CPSS
    DEVRM_Initial();

    BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack("l4",
                                                      SYS_BLD_DRIVER_GROUP_IPCMSGQ_KEY,
                                                      L4_Backdoor_CallBack
                                                      );

    BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack("rule_om",
                                                      SYS_BLD_DRIVER_GROUP_IPCMSGQ_KEY,
                                                      RULE_OM_BACKDOOR_Main
                                                      );
#endif // MARVELL_CPSS

/*
    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_Bind_PolicyMap_To_EngressPort_one_class_one_rule
                );
*/

#if (SYS_CPNT_QOS_V2_POLICY_MAP_MODIFY_ON_FLY == TRUE)
    //UT_REGISTER(L4_MGR_TEST_Setup,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Change_ClassMap_For_PolicyMap_OnFly
    //            );

    //UT_REGISTER(L4_MGR_TEST_Setup,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Change_Meter_For_MF_PolicyMap_OnFly_part1
    //            );

    //UT_REGISTER(NULL,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Change_Meter_For_MF_PolicyMap_OnFly_part2
    //            );

    //UT_REGISTER(NULL,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Change_Meter_For_MF_PolicyMap_OnFly_part3
    //            );

    //UT_REGISTER(L4_MGR_TEST_Setup,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Change_Meter_For_ACL_PolicyMap_OnFly
    //            );
#endif // SYS_CPNT_QOS_V2_POLICY_MAP_MODIFY_ON_FLY

    //UT_REGISTER(L4_MGR_TEST_Setup,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Change_ACE_For_ACL_OnFly_part1
    //            );

    //UT_REGISTER(NULL,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Change_ACE_For_ACL_OnFly_part2
    //            );

    //UT_REGISTER(NULL,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Change_ACE_For_ACL_OnFly_part3
    //            );

    //UT_REGISTER(L4_MGR_TEST_Setup,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Change_ACE_For_ACL_PolicyMap_OnFly_part1
    //            );

    //UT_REGISTER(NULL,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Change_ACE_For_ACL_PolicyMap_OnFly_part2
    //            );

    //UT_REGISTER(L4_MGR_TEST_Setup,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Bind_ACL_PolicyMap_On_Same_Port
    //            );

    //UT_REGISTER(L4_MGR_TEST_Setup,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Bind_PolicyMap_To_EngressPort_one_class_one_rule_and_meter
    //            );

    //UT_REGISTER(L4_MGR_TEST_Setup,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Check_Class_Instance_part1
    //            );

    //UT_REGISTER(NULL,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Check_Class_Instance_part2
    //            );

    //UT_REGISTER(NULL,
    //            L4_MGR_TEST_Setdown,
    //            L4_MGR_TEST_Check_Class_Instance_part3
    //            );

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
/*
    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_Auto_Aggregate_BindAcl
                );

    UT_REGISTER(NULL,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_Auto_Aggregate_UnBindAcl
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_Auto_Aggregate_AddACE_onFly
                );

//    UT_REGISTER(L4_MGR_TEST_Setup,
//                L4_MGR_TEST_Setdown,
//                L4_MGR_TEST_Auto_Aggregate_AddACE_Fail_onFly
//                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_Auto_Aggregate_DelACE_onFly
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_Auto_Aggregate_DelACL_onFly
                );
*/
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
/*
    UT_REGISTER(L4_MGR_TEST_MatchAllClassMap_Setup,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_BindPort
                );

    UT_REGISTER(NULL,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_UnBindPort
                );

    UT_REGISTER(L4_MGR_TEST_MatchAllClassMap_Mix_MF_ACL_Setup,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_BindPort
                );

    UT_REGISTER(NULL,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_UnBindPort
                );
*/
/*
    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_BMT_TC03_Step3
                );
*/
#endif

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) && (SYS_CPNT_QOS_V2_POLICY_MAP_MODIFY_ON_FLY == TRUE)
/*
    UT_REGISTER(L4_MGR_TEST_Setup,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_part1
                );

    UT_REGISTER(NULL,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_part2
                );
*/
/*
    UT_REGISTER(L4_MGR_TEST_Setup,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_part1
                );

    UT_REGISTER(NULL,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_part2
                );

    UT_REGISTER(NULL,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_part3
                );
*/

/*
    UT_REGISTER(L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_Setup,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_AddAce
                );

    UT_REGISTER(NULL,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_AddAce2
                );

    UT_REGISTER(NULL,
                NULL,
                L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_RemoveAce1
                );

   UT_REGISTER(NULL,
               NULL,
               L4_MGR_TEST_MatchAllClassMap_ChangeACE_onFly_RemoveAce2
               );

   UT_REGISTER(NULL,
               NULL,
               L4_MGR_TEST_MatchAllClassMap_RemoveACL_onFly
               );
*/
#endif /* SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL */

/*
    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_IP_Access_Group_Disappear
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_IP_Access_Group_Disappear2
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_No_Match_Cos
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_No_Match_Source_Port
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_No_IP_Access_Group
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_Match_Cos_Failed
                );
*/

#if 0
    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_GetMatchAnyClassMapInstance_1
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_GetMatchAnyClassMapInstance_2
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_GetMatchAnyClassMapInstance_3
                );
#endif

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_BundleRateControl
                );
#endif /* #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) */

/*
    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_SetRule_TrapMyMacArp_ToCPU
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_SetRule_TrapRip_ToCPU
                );

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_SetRule_SetToHiQueue_For_PTP
                );
*/

/*
#if (SYS_CPNT_SWDRV_TRAP_IGMP_BY_RULE == TRUE)
    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_SetRule_IgmpToCpu
                );
#endif

#if (SYS_CPNT_IP_SUBNET_VLAN_BIND_PORT == TRUE)
    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_SetRule_IpSubnetVlanPerPort
                );
    UT_REGISTER(NULL,
                NULL,
                L4_MGR_TEST_SetRule_RemoveIpSubnetVlanPerPort_1
                );
    UT_REGISTER(NULL,
                NULL,
                L4_MGR_TEST_SetRule_RemoveIpSubnetVlanPerPort_2
                );
#endif

    UT_REGISTER(L4_MGR_TEST_Setup,
                L4_MGR_TEST_Setdown,
                L4_MGR_TEST_SetRule_SetIcmpv6Type
                );
*/

#ifdef MARVELL_CPSS
    res = UT_Shell();
#else
    res = UT_Run();
#endif // MARVELL_CPSS

    return (res==0) ? TRUE : FALSE;
}
