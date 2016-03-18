#include <stdio.h>

#include "rule_om.c"
#include "utest.h"

#ifndef _countof
#define _countof(ary)   (sizeof( (ary) )/sizeof( (*ary) ))
#endif /* _countof */

#ifndef SET_MAC
#define SET_MAC(mac, b1, b2, b3, b4, b5, b6) \
    { mac[0]=b1; mac[1]=b2; mac[2]=b3; mac[3]=b4; mac[4]=b5; mac[5]=b6; }
#endif /* SET_MAC */

#ifndef SET_IP
#define SET_IP(ip, b1, b2, b3, b4)  \
    { ((UI8_T*)&ip)[0]=b1; ((UI8_T*)&ip)[1]=b2; ((UI8_T*)&ip)[2]=b3; ((UI8_T*)&ip)[3]=b4; }
#endif /* SET_IP */

static int RULE_OM_TEST_Setup()
{
    RULE_OM_InitiateSystemResources();
    return 0;
}

static int RULE_OM_TEST_Setdown()
{
    return 0;
}

static int RULE_OM_TEST_1()
{
    RULE_TYPE_Ace_Entry_T ace;

    /* SET IPV6 STD ACL
     */
    ace.ace_type = RULE_TYPE_IPV6_STD_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceSourceIpAddr[0] = 0xfe;
    ace.u.ipv6.aceSourceIpAddrPrefixLen = 128;

    if (RULE_TYPE_OK != RULE_OM_LocalValidateAce(&ace))
    {
        printf("Test failed on %s:%d\n", __FUNCTION__, __LINE__);
        return 1;
    }

    ace.ace_type = RULE_TYPE_IPV6_STD_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceDestIpAddr[0] = 0xfe;
    ace.u.ipv6.aceDestIpAddrPrefixLen = 128;

    if (RULE_TYPE_FAIL != RULE_OM_LocalValidateAce(&ace))
    {
        printf("Test failed on %s:%d\n", __FUNCTION__, __LINE__);
        return 1;
    }

    ace.ace_type = RULE_TYPE_IPV6_STD_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceDscp = 1;
    ace.u.ipv6.aceFlowLabel = 1;
    ace.u.ipv6.aceNextHeader = 1;

    if (RULE_TYPE_FAIL != RULE_OM_LocalValidateAce(&ace))
    {
        printf("Test failed on %s:%d\n", __FUNCTION__, __LINE__);
        return 1;
    }

    /* any
     */
    ace.ace_type = RULE_TYPE_IPV6_STD_ACL;
    RULE_OM_InitAce(&ace);

    if (RULE_TYPE_OK != RULE_OM_LocalValidateAce(&ace))
    {
        printf("Test failed on %s:%d\n", __FUNCTION__, __LINE__);
        return 1;
    }

    /* SET IPV6 EXT ACL
     */
    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceSourceIpAddr[0] = 0xfe;
    ace.u.ipv6.aceSourceIpAddrPrefixLen = 128;

    if (RULE_TYPE_OK != RULE_OM_LocalValidateAce(&ace))
    {
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
        printf("Test failed on %s:%d\n", __FUNCTION__, __LINE__);
        return 1;
#else
        ;
#endif
    }

    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceDestIpAddr[0] = 0xfe;
    ace.u.ipv6.aceDestIpAddrPrefixLen = 128;

    if (RULE_TYPE_OK != RULE_OM_LocalValidateAce(&ace))
    {
#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
        printf("Test failed on %s:%d\n", __FUNCTION__, __LINE__);
        return 1;
#else
        ;
#endif
    }

    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceDscp = 1;

    if (RULE_TYPE_OK != RULE_OM_LocalValidateAce(&ace))
    {
#if (SYS_CPNT_ACL_IPV6_EXT_TRAFFIC_CLASS == TRUE)
        printf("Test failed on %s:%d\n", __FUNCTION__, __LINE__);
        return 1;
#else
        ;
#endif
    }

    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceFlowLabel = 1;

    if (RULE_TYPE_OK != RULE_OM_LocalValidateAce(&ace))
    {
#if (SYS_CPNT_ACL_IPV6_EXT_FLOW_LABEL == TRUE)
        printf("Test failed on %s:%d\n", __FUNCTION__, __LINE__);
        return 1;
#else
        ;
#endif
    }

    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceNextHeader = 1;

    if (RULE_TYPE_OK != RULE_OM_LocalValidateAce(&ace))
    {
#if (SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER == TRUE)
        printf("Test failed on %s:%d\n", __FUNCTION__, __LINE__);
        return FALSE;
#else
        ;
#endif
    }

    /* any any
     */
    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    if (RULE_TYPE_OK != RULE_OM_LocalValidateAce(&ace))
    {
        printf("Test failed on %s:%d\n", __FUNCTION__, __LINE__);
        return 1;
    }

    return 0;
}

static int RULE_OM_TEST_BindPort2PolicyMap()
{
    char                        *policy_map_name = "p1";
    RULE_TYPE_PortEntry_T       *port_entry_p;
    RULE_TYPE_PolicyMap_T       *policy_map_p;
    UI32_T                      uport_ifindex;
    RULE_TYPE_InOutDirection_T  inout_profile;
    UI32_T                      policy_map_index;

    if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
    {
        UT_LOG(" ... failed");
        return 1;
    }

    policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
    if (NULL == policy_map_p)
    {
        UT_LOG(" ... failed");
        return 1;
    }

    uport_ifindex = 2;
    inout_profile = RULE_TYPE_INBOUND;

    if (RULE_TYPE_OK != RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index))
    {
        UT_LOG(" ... failed");
        return 1;
    }

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
    if (policy_map_index != port_entry_p->policy_map_index)
    {
        UT_LOG(" ... failed");
        return 1;
    }

    if (1 != policy_map_p->port_count)
    {
        UT_LOG(" ... failed");
        return 1;
    }

    if (!RULE_OM_IS_BIT_ON(policy_map_p->qos_port_list, (uport_ifindex - 1)))
    {
        UT_LOG(" ... failed");
        return 1;
    }

    if (RULE_TYPE_OK != RULE_OM_UnBindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index))
    {
        UT_LOG(" ... failed");
        return 1;
    }

    if (0 != policy_map_p->port_count)
    {
        UT_LOG(" ... failed");
        return 1;
    }

    if (!RULE_OM_IS_BIT_OFF(policy_map_p->qos_port_list, (uport_ifindex - 1)))
    {
        UT_LOG(" ... failed");
        return 1;
    }

    /**************/

    uport_ifindex = 2;
    inout_profile = RULE_TYPE_OUTBOUND;

    if (RULE_TYPE_OK != RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index))
    {
        UT_LOG(" ... failed");
        return 1;
    }

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
    if (policy_map_index != port_entry_p->policy_map_index)
    {
        UT_LOG(" ... failed");
        return 1;
    }

    if (1 != policy_map_p->port_count)
    {
        UT_LOG(" ... failed");
        return 1;
    }

    if (!RULE_OM_IS_BIT_ON(policy_map_p->egress_qos_port_list, (uport_ifindex - 1)))
    {
        UT_LOG(" ... failed");
        return 1;
    }

    if (RULE_TYPE_OK != RULE_OM_UnBindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index))
    {
        UT_LOG(" ... failed");
        return 1;
    }

    if (0 != policy_map_p->port_count)
    {
        UT_LOG(" ... failed");
        return 1;
    }

    if (!RULE_OM_IS_BIT_OFF(policy_map_p->qos_port_list, (uport_ifindex - 1)))
    {
        UT_LOG(" ... failed");
        return 1;
    }

    return 0;
}

int RULE_OM_TEST_GetPortPolicyMapIndex()
{
    UI32_T                      uport_ifindex;
    UI32_T                      policy_map_index;
    char                        *policy_map_name = "p1";
    RULE_TYPE_InOutDirection_T  inout_profile;

    if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_map_name, &policy_map_index))
    {
        UT_LOG("RULE_OM_Create_PolicyMap failed");
        return 1;
    }

    uport_ifindex = 2;
    inout_profile = RULE_TYPE_INBOUND;

    if (RULE_TYPE_OK != RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index))
    {
        UT_LOG("RULE_OM_BindPort2PolicyMap failed");
        return 1;
    }

    {
        UI32_T old_pmap_idx;
        if (RULE_TYPE_OK != RULE_OM_GetPortPolicyMapIndex(uport_ifindex, inout_profile, &old_pmap_idx))
        {
            UT_LOG("RULE_OM_GetPortPolicyMapIndex failed");
            return 1;
        }

        if (old_pmap_idx !=policy_map_index)
        {
            UT_LOG("RULE_OM_GetPortPolicyMapIndex failed");
            return 1;
        }
    }

    uport_ifindex = 3;
    inout_profile = RULE_TYPE_OUTBOUND;

    if (RULE_TYPE_OK != RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index))
    {
        UT_LOG("RULE_OM_BindPort2PolicyMap failed");
        return 1;
    }

    {
        UI32_T old_pmap_idx;
        if (RULE_TYPE_OK != RULE_OM_GetPortPolicyMapIndex(uport_ifindex, inout_profile, &old_pmap_idx))
        {
            UT_LOG("RULE_OM_GetPortPolicyMapIndex failed");
            return 1;
        }

        if (old_pmap_idx !=policy_map_index)
        {
            UT_LOG("RULE_OM_GetPortPolicyMapIndex failed");
            return 1;
        }
    }

    return 0;
}

static int RULE_OM_TEST_GetNext_PortListByPolicyMap()
{
    struct PolicyMapCfg
    {
        char    policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH];
        UI32_T  policy_map_index;
        UI32_T  ingress_port_tbl[SYS_ADPT_TOTAL_NBR_OF_LPORT];
        UI32_T  egress_port_tbl[SYS_ADPT_TOTAL_NBR_OF_LPORT];
    } policy_map_tbl[] =
    {
        {
            "p1",
            0,
            {1,2,3,4,5,6},
            {2,4,6,8,10},
        },
        {
            "p2",
            0,
            {1,2,3,4,5,6},
            {1,3,5,7,9},
        },
        {
            "p3",
            0,
            {2,5,10},
            {3,6,9},
        },
    };

    UI32_T  i;

    for (i=0; i<sizeof(policy_map_tbl)/sizeof(*policy_map_tbl); i++)
    {
        struct PolicyMapCfg *cfg = &policy_map_tbl[i];
        UI32_T j;

        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)cfg->policy_map_name, &cfg->policy_map_index))
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        for (j=0; j<sizeof(cfg->ingress_port_tbl)/sizeof(*cfg->ingress_port_tbl); j++)
        {
            if (0 == cfg->ingress_port_tbl[j])
            {
                continue;
            }

            if (RULE_TYPE_OK != RULE_OM_BindPort2PolicyMap(cfg->ingress_port_tbl[j],
                                                           RULE_TYPE_INBOUND,
                                                           cfg->policy_map_index))
            {
                UT_LOG("Prepare policy-map failed");
                return 1;
            }
        }

        for (j=0; j<sizeof(cfg->egress_port_tbl)/sizeof(*cfg->egress_port_tbl); j++)
        {
            if (0 == cfg->egress_port_tbl[j])
            {
                continue;
            }

            if (RULE_TYPE_OK != RULE_OM_BindPort2PolicyMap(cfg->egress_port_tbl[j],
                                                           RULE_TYPE_OUTBOUND,
                                                           cfg->policy_map_index))
            {
                UT_LOG("Prepare policy-map failed");
                return 1;
            }
        }
    }

    for (i=0; i<sizeof(policy_map_tbl)/sizeof(*policy_map_tbl); i++)
    {
        struct PolicyMapCfg         *cfg = &policy_map_tbl[i];
        UI32_T                      uport_ifindex = 0;
        RULE_TYPE_InOutDirection_T  inout_profile;
        UI32_T                      in_index = 0;
        UI32_T                      out_index = 0;

        while (RULE_TYPE_OK == RULE_OM_GetNextPort_BindPolicyMap(cfg->policy_map_index, &uport_ifindex, &inout_profile))
        {
            if (inout_profile == RULE_TYPE_INBOUND)
            {
                if (cfg->ingress_port_tbl[in_index++] != uport_ifindex)
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindPolicyMap failed");
                    return 1;
                }
            }
            else
            {
                if (cfg->egress_port_tbl[out_index++] != uport_ifindex)
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindPolicyMap failed");
                    return 1;
                }
            }

            //UT_LOG("ifindex=%lu, inout=%s", uport_ifindex, (inout_profile==RULE_TYPE_INBOUND)?"in":"out");
        }
    }

    return 0;
}

static int RULE_OM_TEST_GetNext_PortListByClassMap()
{
    struct ClassMapCfg
    {
        char        class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH];
        UI32_T      class_map_index;

        struct PolicyMapCfg
        {
            char    policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH];
            UI32_T  policy_map_index;
            UI32_T  ingress_port_tbl[SYS_ADPT_TOTAL_NBR_OF_LPORT];
            UI32_T  egress_port_tbl[SYS_ADPT_TOTAL_NBR_OF_LPORT];
        } policy_map_cfg_tbl[SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_NBR_OF_POLICY_MAP];

    } class_map_cfg_tbl [] =
    {
        {
            "c1",
            0,
            {
                {
                    "p1",
                    0,
                    {1,2,3},
                    {1,2,3},
                },
                {
                    "p2",
                    0,
                    {4,5,6},
                    {4,5,6},
                },
            },
        },
        {
            "c2",
            0,
            {
                {
                    "p1",
                    0,
                    {1,2,3},
                    {1,2,3},
                },
                {
                    "p2",
                    0,
                    {4,5,6},
                    {4,5,6},
                },
            },
        },
        {
            "c3",
            0,
            {
                {
                    "p3",
                    0,
                    {7,10},
                    {8,9},
                },
            },
        },
    };

    UI32_T  i;

    for (i=0; i<sizeof(class_map_cfg_tbl)/sizeof(*class_map_cfg_tbl); i++)
    {
        struct ClassMapCfg *class_cfg = &class_map_cfg_tbl[i];
        UI32_T j;

        if (RULE_TYPE_OK != RULE_OM_Create_ClassMap(class_cfg->class_map_name,
                                                    RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_cfg->class_map_index))
        {
            UT_LOG("Prepare class-map failed");
            return 1;
        }

        for (j=0; j<sizeof(class_cfg->policy_map_cfg_tbl)/sizeof(*class_cfg->policy_map_cfg_tbl);j++)
        {
            struct PolicyMapCfg *policy_cfg = &class_cfg->policy_map_cfg_tbl[j];
            UI32_T k;

            if (policy_cfg->policy_map_name[0] == 0)
            {
                break;
            }

            if (RULE_TYPE_OK != RULE_OM_GetPolicyMapIdByName(policy_cfg->policy_map_name, &policy_cfg->policy_map_index))
            {
                if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap((UI8_T*)policy_cfg->policy_map_name, &policy_cfg->policy_map_index))
                {
                    UT_LOG("Prepare policy-map failed");
                    return 1;
                }
            }

            if (RULE_TYPE_OK != RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_cfg->policy_map_index,
                                                                          class_cfg->class_map_index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            for (k=0; k<sizeof(policy_cfg->ingress_port_tbl)/sizeof(*policy_cfg->ingress_port_tbl); k++)
            {
                UI32_T ifindex = policy_cfg->ingress_port_tbl[k];

                if (0 == ifindex)
                {
                    break;
                }

                if (RULE_TYPE_OK != RULE_OM_BindPort2PolicyMap(ifindex,
                                                               RULE_TYPE_INBOUND,
                                                               policy_cfg->policy_map_index))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

            for (k=0; k<sizeof(policy_cfg->egress_port_tbl)/sizeof(*policy_cfg->egress_port_tbl); k++)
            {
                UI32_T ifindex = policy_cfg->egress_port_tbl[k];

                if (0 == ifindex)
                {
                    break;
                }

                if (RULE_TYPE_OK != RULE_OM_BindPort2PolicyMap(ifindex,
                                                               RULE_TYPE_OUTBOUND,
                                                               policy_cfg->policy_map_index))
                {
                    UT_LOG("Bind policy-map failed");
                    return 1;
                }
            }

        }
    }

    for (i=0; i<sizeof(class_map_cfg_tbl)/sizeof(*class_map_cfg_tbl); i++)
    {
        struct ClassMapCfg *class_cfg = &class_map_cfg_tbl[i];
        UI32_T  policy_map_index;
        UI32_T  uport_ifindex;
        RULE_TYPE_InOutDirection_T inout_profile;

        UI32_T  policy_cfg_ix = 0;
        UI32_T  in_ifindex_ix = 0;
        UI32_T  en_ifindex_ix = 0;

        uport_ifindex = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextPort_BindClassMap(class_cfg->class_map_index,
                                                                &uport_ifindex,
                                                                &inout_profile,
                                                                &policy_map_index
                                                                ))
        {
            struct PolicyMapCfg *policy_cfg = &class_cfg->policy_map_cfg_tbl[ policy_cfg_ix ];

            //UT_LOG("class-map %lu, policy-map %lu, ifindex %2lu, direction %s",
            //                        class_cfg->class_map_index,
            //                        policy_map_index,
            //                        uport_ifindex,
            //                        inout_profile==RULE_TYPE_INBOUND ? "in" : "out");

            if (policy_cfg->policy_map_index != policy_map_index)
            {
                policy_cfg_ix ++;

                policy_cfg = &class_cfg->policy_map_cfg_tbl[ policy_cfg_ix ];
                in_ifindex_ix = 0;
                en_ifindex_ix = 0;

                if (policy_cfg->policy_map_index != policy_map_index)
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindClassMap failed");
                    return FALSE;
                }
            }

            if (RULE_TYPE_INBOUND == inout_profile)
            {
                if (policy_cfg->ingress_port_tbl[ in_ifindex_ix++ ] != uport_ifindex)
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindClassMap failed");
                    return FALSE;
                }
            }
            else
            {
                if (policy_cfg->egress_port_tbl[ en_ifindex_ix++ ] != uport_ifindex)
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindClassMap failed");
                    return FALSE;
                }
            }
        }

        //printf("\n");
    }

    /* RULE_OM_GetNextPort_BindClassMap vs RULE_OM_Get_PortListByClassMap
     * ingress port
     */
    for (i=0; i<sizeof(class_map_cfg_tbl)/sizeof(*class_map_cfg_tbl); i++)
    {
        struct ClassMapCfg *class_cfg = &class_map_cfg_tbl[i];
        UI32_T  policy_map_index;
        UI32_T  uport_ifindex;
        RULE_TYPE_InOutDirection_T inout_profile;

        UI32_T  port_nbr;
        UI8_T   port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST] = {0};

        RULE_OM_Get_PortListByClassMap(class_cfg->class_map_index, RULE_TYPE_INBOUND, port_list, &port_nbr);

        uport_ifindex = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextPort_BindClassMap(class_cfg->class_map_index,
                                                                &uport_ifindex,
                                                                &inout_profile,
                                                                &policy_map_index
                                                                ))
        {
            if (inout_profile == RULE_TYPE_INBOUND)
            {
                RULE_OM_SET_BIT_OFF(port_list, (uport_ifindex-1) );
            }
        }

        {
            UI32_T i;

            for (i=0; i<sizeof(port_list)/sizeof(*port_list); i++)
            {
                if (port_list[i])
                {
                    UT_LOG("Test RULE_OM_Get_PortListByClassMap failed");
                    return 1;
                }
            }
        }
    }

    /* RULE_OM_GetNextPort_BindClassMap vs RULE_OM_Get_PortListByClassMap
     * engress port
     */
    for (i=0; i<sizeof(class_map_cfg_tbl)/sizeof(*class_map_cfg_tbl); i++)
    {
        struct ClassMapCfg *class_cfg = &class_map_cfg_tbl[i];
        UI32_T  policy_map_index;
        UI32_T  uport_ifindex;
        RULE_TYPE_InOutDirection_T inout_profile;

        UI32_T  port_nbr;
        UI8_T   port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST] = {0};

        RULE_OM_Get_PortListByClassMap(class_cfg->class_map_index, RULE_TYPE_OUTBOUND, port_list, &port_nbr);

        uport_ifindex = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextPort_BindClassMap(class_cfg->class_map_index,
                                                                &uport_ifindex,
                                                                &inout_profile,
                                                                &policy_map_index
                                                                ))
        {
            if (inout_profile == RULE_TYPE_OUTBOUND)
            {
                RULE_OM_SET_BIT_OFF(port_list, (uport_ifindex-1) );
            }
        }

        {
            UI32_T i;

            for (i=0; i<sizeof(port_list)/sizeof(*port_list); i++)
            {
                if (port_list[i])
                {
                    UT_LOG("Test RULE_OM_Get_PortListByClassMap failed");
                    return 1;
                }
            }
        }
    }

    return 0;
}

static int RULE_OM_TEST_GetNextPort_BindAcl()
{
    struct AclCfg
    {
        char                name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH];
        RULE_TYPE_AclType_T type;
        UI32_T              index;
        RULE_TYPE_Acl_T     *entry;
        UI32_T              ingress_port_tbl[SYS_ADPT_TOTAL_NBR_OF_LPORT];
        UI32_T              egress_port_tbl[SYS_ADPT_TOTAL_NBR_OF_LPORT];
    } acl_cfg_tbl[] =
    {
        {
            "a1",
            RULE_TYPE_MAC_ACL,
            0,
            NULL,
            {1,2,3},
            {1,2,3},
        },
        {
            "a2",
            RULE_TYPE_MAC_ACL,
            0,
            NULL,
            {1,2,3},
            {1,2,3},
        },
        {
            "a3",
            RULE_TYPE_MAC_ACL,
            0,
            NULL,
            {4,6,8},
            {5,7,9},
        },
    };
    UI32_T i;

    for (i=0; i<sizeof(acl_cfg_tbl)/sizeof(*acl_cfg_tbl);i++)
    {
        struct  AclCfg *acl = &acl_cfg_tbl[i];
        UI32_T  j;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl(acl->name, acl->type))
        {
            UT_LOG("Prepare ACL failed");
            return 1;
        }

        if (RULE_TYPE_OK != RULE_OM_GetAclIdByName(acl->name, &acl->index))
        {
            UT_LOG("Prepare ACL failed");
            return 1;
        }

        acl->entry = RULE_OM_LocalGetAclByIndex(acl->index);
        if (NULL == acl->entry)
        {
            UT_LOG("Prepare ACL failed");
            return 1;
        }

        for (j=0; j<sizeof(acl->ingress_port_tbl)/sizeof(*acl->ingress_port_tbl);j++)
        {
            UI32_T port = acl->ingress_port_tbl[j];

            if (0 == port)
            {
                break;
            }

            RULE_OM_SET_BIT_ON( acl->entry->filtering_in_port_list, (port-1) );
        }

        for (j=0; j<sizeof(acl->egress_port_tbl)/sizeof(*acl->egress_port_tbl);j++)
        {
            UI32_T port = acl->egress_port_tbl[j];

            if (0 == port)
            {
                break;
            }

            RULE_OM_SET_BIT_ON( acl->entry->filtering_out_port_list, (port-1) );
        }
    }

    for (i=0; i<sizeof(acl_cfg_tbl)/sizeof(*acl_cfg_tbl);i++)
    {
        struct  AclCfg *acl = &acl_cfg_tbl[i];

        UI32_T  uport_ifindex;
        RULE_TYPE_InOutDirection_T  inout_profile;

        UI32_T  in_ix   = 0;
        UI32_T  out_ix  = 0;

        uport_ifindex = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextPort_BindAcl(acl->index, &uport_ifindex, &inout_profile))
        {
            //UT_LOG("acl %s index %lu on port %lu direction %s",
            //    acl->name, acl->index,
            //    uport_ifindex, (RULE_TYPE_INBOUND == inout_profile) ? "in" : "out"
            //    );

            if (RULE_TYPE_INBOUND == inout_profile)
            {
                if (acl->ingress_port_tbl[in_ix++] != uport_ifindex)
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindAcl failed");
                    return 1;
                }
            }
            else if (RULE_TYPE_OUTBOUND == inout_profile)
            {
                if (acl->egress_port_tbl[out_ix++] != uport_ifindex)
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindAcl failed");
                    return 1;
                }
            }
            else
            {
                UT_LOG("Test RULE_OM_GetNextPort_BindAcl failed");
                return 1;
            }
        }
    }

    return 0;
}

static int RULE_OM_TEST_GetNext_PortListByAclEx()
{
#define MAX_NAME_LENGTH         10
#define MAX_NBR_OF_POLICY_MAP   5
#define MAX_NBR_OF_CLASS_MAP    5

    struct PolicyMap
    {
        char                    name[MAX_NAME_LENGTH];
        UI32_T                  index;
        RULE_TYPE_PolicyMap_T   *entry;

        UI32_T                  ingress_port_tbl[SYS_ADPT_TOTAL_NBR_OF_LPORT];
        UI32_T                  egress_port_tbl[SYS_ADPT_TOTAL_NBR_OF_LPORT];
    };

    struct ClassMap
    {
        char                    name[MAX_NAME_LENGTH];
        UI32_T                  index;
        RULE_TYPE_ClassMap_T    *entry;

        struct PolicyMap        policy_map_tbl[MAX_NBR_OF_POLICY_MAP];
    };

    struct Acl
    {
        char                name[MAX_NAME_LENGTH];
        RULE_TYPE_AclType_T type;
        UI32_T              index;
        RULE_TYPE_Acl_T     *entry;

        struct ClassMap     class_map_tbl[MAX_NBR_OF_CLASS_MAP];
    };

    struct PolicyMap policy_map_tbl[] =
    {
        {
            "p1",
            0,
            NULL,
            {
                1,2,3
            },
            {
                1,2,3
            },
        },
        {
            "p2",
            0,
            NULL,
            {
                4,5,6
            },
            {
                4,5,6
            },
        },
        {
            "p3",
            0,
            NULL,
            {
                7,9
            },
            {
                8,10
            },
        },
    };

    struct ClassMap class_map_tbl[] =
    {
        {
            "c1",
            0,
            NULL,
            {
                {
                    "p1",
                    0,
                    0,
                    {0},
                    {0},
                },
                {
                    "p2",
                    0,
                    0,
                    {0},
                    {0},
                },
                {
                    "p3",
                    0,
                    0,
                    {0},
                    {0},
                },
            },
        },
        {
            "c2",
            0,
            NULL,
            {
                {
                    "p1",
                    0,
                    0,
                    {0},
                    {0},
                },
                {
                    "p2",
                    0,
                    0,
                    {0},
                    {0},
                },
            },
        },
        {
            "c3",
            0,
            NULL,
            {
                {
                    "p2",
                    0,
                    0,
                    {0},
                    {0},
                },
                {
                    "p3",
                    0,
                    0,
                    {0},
                    {0},
                },
            },
        },
    };

    struct Acl acl_tbl[] =
    {
        {
            "a1",
            RULE_TYPE_MAC_ACL,
            0,
            NULL,
            {
                {
                    "c2",
                    0,
                    NULL,
                    {
                        0
                    },
                },
                {
                    "c3",
                    0,
                    NULL,
                    {
                        0
                    },
                },
            },
        },
        {
            "a2",
            RULE_TYPE_MAC_ACL,
            0,
            NULL,
            {
                {
                    "c1",
                    0,
                    NULL,
                    {
                        0
                    },
                },
                {
                    "c2",
                    0,
                    NULL,
                    {
                        0
                    },
                },
                {
                    "c3",
                    0,
                    NULL,
                    {
                        0
                    },
                },
            },
        },
    };

    UI32_T i;

    /* CREATE POLICY-MAP AND BIND TO PORT
     */
    for (i=0; i<_countof(policy_map_tbl); i++)
    {
        struct PolicyMap *policy_map = &policy_map_tbl[i];
        UI32_T j;

        if (RULE_TYPE_OK != RULE_OM_Create_PolicyMap(policy_map->name, &policy_map->index))
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        policy_map->entry = RULE_OM_LocalGetPolicyMapByIndex(policy_map->index);
        if (NULL == policy_map->entry)
        {
            UT_LOG("Prepare policy-map failed");
            return 1;
        }

        for (j=0; j<sizeof(policy_map->ingress_port_tbl); j++)
        {
            UI32_T port = policy_map->ingress_port_tbl[j];

            if (0 == port)
            {
                break;
            }

            if (RULE_TYPE_OK != RULE_OM_BindPort2PolicyMap(port, RULE_TYPE_INBOUND, policy_map->index))
            {
                UT_LOG("Prepare policy-map failed");
                return 1;
            }
        }

        for (j=0; j<sizeof(policy_map->egress_port_tbl); j++)
        {
            UI32_T port = policy_map->egress_port_tbl[j];

            if (0 == port)
            {
                break;
            }

            if (RULE_TYPE_OK != RULE_OM_BindPort2PolicyMap(port, RULE_TYPE_OUTBOUND, policy_map->index))
            {
                UT_LOG("Prepare policy-map failed");
                return 1;
            }
        }
    }

    /* CREATE CLASS-MAP AND BIND TO POLICY-MAP
     */
    for (i=0; i<sizeof(class_map_tbl)/sizeof(*class_map_tbl); i++)
    {
        struct ClassMap *class_map = &class_map_tbl[i];
        UI32_T j;

        if (RULE_TYPE_OK != RULE_OM_Create_ClassMap(class_map->name,
                                                    RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map->index))
        {
            UT_LOG("Prepare class-map failed");
            return 1;
        }

        class_map->entry = RULE_OM_LocalGetClassMapByIndex(class_map->index);
        if (NULL == class_map->entry)
        {
            UT_LOG("Prepare class-map failed");
            return 1;
        }

        for (j=0; j<sizeof(class_map->policy_map_tbl)/sizeof(*class_map->policy_map_tbl); j++)
        {
            struct PolicyMap *policy_map = &class_map->policy_map_tbl[j];

            if (policy_map->name[0] == 0)
            {
                break;
            }

            if (RULE_TYPE_OK != RULE_OM_GetPolicyMapIdByName(policy_map->name, &policy_map->index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }

            if (RULE_TYPE_OK != RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map->index, class_map->index))
            {
                UT_LOG("Prepare class-map failed");
                return 1;
            }
        }
    }

    for (i=0; i<sizeof(acl_tbl)/sizeof(*acl_tbl); i++)
    {
        struct Acl *acl = &acl_tbl[i];
        UI32_T j;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl(acl->name, acl->type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName(acl->name, &acl->index))
        {
            UT_LOG("Prepare ACL failed");
            return 1;
        }

        acl->entry = RULE_OM_LocalGetAclByIndex(acl->index);
        if (NULL == acl->entry)
        {
            UT_LOG("Prepare ACL failed");
            return 1;
        }

        for (j=0; j<sizeof(acl->class_map_tbl)/sizeof(acl->class_map_tbl); j++)
        {
            struct ClassMap *class_map = &acl->class_map_tbl[j];

            if (class_map->name[0] == 0)
            {
                break;
            }

            if (RULE_TYPE_OK != RULE_OM_GetClassMapIndexByName(class_map->name, &class_map->index))
            {
                UT_LOG("Prepare ACL failed");
                return 1;
            }

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_ACL;
                strcpy((char*)element_entry.element.acl_name, acl->name);

                if (RULE_TYPE_OK != RULE_OM_Add_ClassMapElement_To_ClassMap(class_map->index, &element_entry, &class_index))
                {
                    UT_LOG("Prepare class-map failed");
                    return 1;
                }
            }
        }
    }

    /* merger policy_map_tbl into class_map_tbl
     */
    for (i=0; i<sizeof(class_map_tbl)/sizeof(*class_map_tbl); i++)
    {
        struct ClassMap *class_map = &class_map_tbl[i];
        UI32_T j;

        if (0 == class_map->name[0])
        {
            break;
        }

        for (j=0; j<sizeof(class_map->policy_map_tbl)/sizeof(*class_map->policy_map_tbl); j++)
        {
            struct PolicyMap *policy_map = &class_map->policy_map_tbl[j];

            UI32_T __i;

            if (0 == policy_map->name[0])
            {
                break;
            }

            for (__i=0; __i<sizeof(policy_map_tbl)/sizeof(*policy_map_tbl); __i++)
            {
                if (0 == strcmp(policy_map->name, policy_map_tbl[__i].name))
                {
                    memcpy(policy_map, &policy_map_tbl[__i], sizeof(*policy_map));
                    break;
                }
            }
        }
    }

    for (i=0; i<sizeof(acl_tbl)/sizeof(*acl_tbl); i++)
    {
        struct Acl *acl = &acl_tbl[i];
        UI32_T j;

        for (j=0; j<sizeof(acl->class_map_tbl)/sizeof(*acl->class_map_tbl); j++)
        {
            struct ClassMap *class_map = &acl->class_map_tbl[j];
            UI32_T k;
            UI32_T __i;

            if (0 == class_map->name[0])
            {
                break;
            }

            for (__i=0; __i<sizeof(class_map_tbl)/sizeof(*class_map_tbl); __i++)
            {
                if (0 == strcmp(class_map->name, class_map_tbl[__i].name))
                {
                    memcpy(class_map, &class_map_tbl[__i], sizeof(*class_map));
                    break;
                }
            }
        }
    }

    ///* dump configuration table
    // */
    //for (i=0; i<sizeof(acl_tbl)/sizeof(*acl_tbl); i++)
    //{
    //    struct Acl *acl = &acl_tbl[i];
    //    UI32_T j;

    //    UT_LOG("acl %s", acl->name);

    //    for (j=0; j<sizeof(acl->class_map_tbl)/sizeof(*acl->class_map_tbl); j++)
    //    {
    //        struct ClassMap *class_map = &acl->class_map_tbl[j];
    //        UI32_T k;

    //        if (0 == class_map->name[0])
    //        {
    //            break;
    //        }
    //
    //        UT_LOG("  class-map %s", class_map->name);

    //        for (k=0; k<sizeof(class_map->policy_map_tbl)/sizeof(*class_map->policy_map_tbl); k++)
    //        {
    //            struct PolicyMap *policy_map = &class_map->policy_map_tbl[k];
    //            UI32_T __i;
    //
    //            if (0 == policy_map->name[0])
    //            {
    //                break;
    //            }
    //
    //            UT_LOG("    policy-map %s", policy_map->name);

    //            for (__i=0; __i<sizeof(policy_map->ingress_port_tbl)/sizeof(*policy_map->ingress_port_tbl);__i++)
    //            {
    //                if (0 == policy_map->ingress_port_tbl[__i])
    //                {
    //                    break;
    //                }

    //                UT_LOG("      in %lu", policy_map->ingress_port_tbl[__i]);
    //            }

    //            for (__i=0; __i<sizeof(policy_map->egress_port_tbl)/sizeof(*policy_map->egress_port_tbl);__i++)
    //            {
    //                if (0 == policy_map->egress_port_tbl[__i])
    //                {
    //                    break;
    //                }

    //                UT_LOG("      out %lu", policy_map->egress_port_tbl[__i]);
    //            }
    //        }
    //    }
    //}

    for (i=0; i<_countof(acl_tbl);i++)
    {
        struct Acl  *acl = &acl_tbl[i];

        UI32_T  ifindex;
        RULE_TYPE_InOutDirection_T  inout_profile;
        UI32_T  policy_map_index;
        UI32_T  class_map_index;

        /* check list
         */
        UI32_T  class_map_tbl_ix    = 0;
        UI32_T  policy_map_tbl_ix   = 0;
        UI32_T  in_port_tbl_ix  = 0;
        UI32_T  out_port_tbl_ix = 0;
        struct ClassMap *class_map  = NULL;
        struct PolicyMap *policy_map= NULL;

        ifindex = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextPort_BindClsMapAcl(acl->index,
                                                                 &ifindex,
                                                                 &inout_profile,
                                                                 &policy_map_index,
                                                                 &class_map_index))
        {
            class_map   = &acl->class_map_tbl[ class_map_tbl_ix ];

            if (0 == class_map->name[0])
            {
                class_map = &acl->class_map_tbl[ ++class_map_tbl_ix ];
                policy_map_tbl_ix = 0;
                in_port_tbl_ix  = 0;
                out_port_tbl_ix = 0;
            }

            /* check class_map_index, if not equal, hop to next
             */
            if (class_map_index != class_map->index)
            {
                class_map = &acl->class_map_tbl[ ++class_map_tbl_ix ];
                policy_map_tbl_ix = 0;
                in_port_tbl_ix  = 0;
                out_port_tbl_ix = 0;

                if (class_map_index != class_map->index)
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindClsMapAcl failed");
                    return 1;
                }
            }

            policy_map  = &class_map->policy_map_tbl[ policy_map_tbl_ix ];

            if (0 == policy_map->name[0])
            {
                policy_map = &class_map->policy_map_tbl[ ++policy_map_tbl_ix ];
                in_port_tbl_ix  = 0;
                out_port_tbl_ix = 0;
            }

            if (policy_map_index != policy_map->index)
            {
                policy_map = &class_map->policy_map_tbl[ ++policy_map_tbl_ix ];
                in_port_tbl_ix  = 0;
                out_port_tbl_ix = 0;

                if (policy_map_index != policy_map->index)
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindClsMapAcl failed");
                    return 1;
                }
            }

            //UT_LOG("acl %lu, class-map %lu, policy-map %lu, ifindex %lu, dir %s",
            //    acl->index,
            //    class_map_index,
            //    policy_map_index,
            //    ifindex,
            //    RULE_TYPE_INBOUND==inout_profile?"in":"out");

            if (RULE_TYPE_INBOUND == inout_profile)
            {
                if (ifindex != policy_map->ingress_port_tbl[ in_port_tbl_ix++ ])
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindClsMapAcl failed"
                           "  ifindex = %lu, policy_map->ingress_port = %lu",
                           ifindex,
                           policy_map->ingress_port_tbl[ in_port_tbl_ix-1 ]);
                    return 1;
                }
            }
            else
            {
                if (ifindex != policy_map->egress_port_tbl[ out_port_tbl_ix++ ])
                {
                    UT_LOG("Test RULE_OM_GetNextPort_BindClsMapAcl failed");
                    return 1;
                }
            }
        }
    }

    return 0;
}

int RULE_OM_TEST_MergerMacAce_basic()
{
    RULE_TYPE_Ace_Entry_T ace;
    RULE_TYPE_Ace_Entry_T agg_ace;

    ace.ace_type = RULE_TYPE_MAC_ACL;
    RULE_OM_InitAce(&ace);

    agg_ace.ace_type = RULE_TYPE_MAC_ACL;
    RULE_OM_InitAce(&agg_ace);

    SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x01, 0x01, 0x00);
    SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

    SET_MAC(agg_ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x01, 0x01, 0x01);
    SET_MAC(agg_ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

    if (RULE_TYPE_OK != RULE_OM_LocalCompressAce(&ace, &agg_ace))
    {
        UT_LOG("Test RULE_OM_LocalCompressAce fail");
        return UT_FAIL;
    }

    if (0xFE != agg_ace.u.mac.aceDestMacAddrBitmask[5])
    {
        UT_LOG("Test RULE_OM_LocalCompressAce fail");
        return UT_FAIL;
    }

    SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x01, 0x01, 0x02);
    SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

    SET_MAC(agg_ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x01, 0x01, 0x03);
    SET_MAC(agg_ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

    if (RULE_TYPE_OK != RULE_OM_LocalCompressAce(&ace, &agg_ace))
    {
        UT_LOG("Test RULE_OM_LocalCompressAce fail");
        return UT_FAIL;
    }

    if (0xFE != agg_ace.u.mac.aceDestMacAddrBitmask[5])
    {
        UT_LOG("Test RULE_OM_LocalCompressAce fail");
        return UT_FAIL;
    }

    return UT_OK;
}

int RULE_OM_TEST_MergerMacAce_BMT()
{
    UI32_T total_count = 0;

    {
        UI32_T  i;
        char                *acl_name   = "a1";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, acl_type) ||
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

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("DA\r\n");
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %02x-%02x-%02x-%02x-%02x-%02x %02x-%02x-%02x-%02x-%02x-%02x \r\n",
                    i+1,
                    ace_p->u.mac.aceDestMacAddr[0], ace_p->u.mac.aceDestMacAddr[1], ace_p->u.mac.aceDestMacAddr[2],
                    ace_p->u.mac.aceDestMacAddr[3], ace_p->u.mac.aceDestMacAddr[4], ace_p->u.mac.aceDestMacAddr[5],
                    ace_p->u.mac.aceDestMacAddrBitmask[0], ace_p->u.mac.aceDestMacAddrBitmask[1], ace_p->u.mac.aceDestMacAddrBitmask[2],
                    ace_p->u.mac.aceDestMacAddrBitmask[3], ace_p->u.mac.aceDestMacAddrBitmask[4], ace_p->u.mac.aceDestMacAddrBitmask[5]);
            }
        }
    } /* a1 */

    {
        UI32_T  i;
        char                *acl_name   = "a2";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, acl_type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=51; i<=100; i++)
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

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("DA\r\n");
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %02x-%02x-%02x-%02x-%02x-%02x %02x-%02x-%02x-%02x-%02x-%02x \r\n",
                    i+1,
                    ace_p->u.mac.aceDestMacAddr[0], ace_p->u.mac.aceDestMacAddr[1], ace_p->u.mac.aceDestMacAddr[2],
                    ace_p->u.mac.aceDestMacAddr[3], ace_p->u.mac.aceDestMacAddr[4], ace_p->u.mac.aceDestMacAddr[5],
                    ace_p->u.mac.aceDestMacAddrBitmask[0], ace_p->u.mac.aceDestMacAddrBitmask[1], ace_p->u.mac.aceDestMacAddrBitmask[2],
                    ace_p->u.mac.aceDestMacAddrBitmask[3], ace_p->u.mac.aceDestMacAddrBitmask[4], ace_p->u.mac.aceDestMacAddrBitmask[5]);
            }
        }
    } /* a2 */

    {
        UI32_T  i;
        char                *acl_name   = "a3";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, acl_type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=1; i<=50; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceSourceMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceSourceMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("SA\r\n");
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %02x-%02x-%02x-%02x-%02x-%02x %02x-%02x-%02x-%02x-%02x-%02x \r\n",
                    i+1,
                    ace_p->u.mac.aceSourceMacAddr[0], ace_p->u.mac.aceSourceMacAddr[1], ace_p->u.mac.aceSourceMacAddr[2],
                    ace_p->u.mac.aceSourceMacAddr[3], ace_p->u.mac.aceSourceMacAddr[4], ace_p->u.mac.aceSourceMacAddr[5],
                    ace_p->u.mac.aceSourceMacAddrBitmask[0], ace_p->u.mac.aceSourceMacAddrBitmask[1], ace_p->u.mac.aceSourceMacAddrBitmask[2],
                    ace_p->u.mac.aceSourceMacAddrBitmask[3], ace_p->u.mac.aceSourceMacAddrBitmask[4], ace_p->u.mac.aceSourceMacAddrBitmask[5]);
            }
        }
    } /* a3 */

    {
        UI32_T  i;
        char                *acl_name   = "a4";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, acl_type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=51; i<=100; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceSourceMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceSourceMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("SA\r\n");
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %02x-%02x-%02x-%02x-%02x-%02x %02x-%02x-%02x-%02x-%02x-%02x \r\n",
                    i+1,
                    ace_p->u.mac.aceSourceMacAddr[0], ace_p->u.mac.aceSourceMacAddr[1], ace_p->u.mac.aceSourceMacAddr[2],
                    ace_p->u.mac.aceSourceMacAddr[3], ace_p->u.mac.aceSourceMacAddr[4], ace_p->u.mac.aceSourceMacAddr[5],
                    ace_p->u.mac.aceSourceMacAddrBitmask[0], ace_p->u.mac.aceSourceMacAddrBitmask[1], ace_p->u.mac.aceSourceMacAddrBitmask[2],
                    ace_p->u.mac.aceSourceMacAddrBitmask[3], ace_p->u.mac.aceSourceMacAddrBitmask[4], ace_p->u.mac.aceSourceMacAddrBitmask[5]);
            }
        }
    }

    {
        UI32_T  i;
        char                *acl_name   = "a5";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, acl_type) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=0x8863; i<=0x886f; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            ace.u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_equal;
            ace.u.mac.aceEtherTypeBitmask = 0xffff;
            ace.u.mac.aceMinEtherType = i;
            ace.u.mac.aceMaxEtherType = i;

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("EtherType\r\n");
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %08lx %08lx\r\n",
                    i+1,
                    ace_p->u.mac.aceMinEtherType,
                    ace_p->u.mac.aceEtherTypeBitmask);
            }
        }
    } /* a5 */

    {
        UI32_T  i;
        char                *acl_name   = "a6";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, RULE_TYPE_IP_EXT_ACL) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=1; i<=50; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            SET_IP(ace.u.ip.aceDestIpAddr, 192, 168, 1, i);
            SET_IP(ace.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("DIP\r\n");
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %d.%d.%d.%d %d.%d.%d.%d \r\n",
                    i+1,
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[0],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[1],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[2],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[3],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[0],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[1],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[2],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[3]
                    );
            }
        }
    } /* a6 */

    {
        UI32_T  i;
        char                *acl_name   = "a7";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, RULE_TYPE_IP_EXT_ACL) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=51; i<=100; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            SET_IP(ace.u.ip.aceDestIpAddr, 192, 168, 1, i);
            SET_IP(ace.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("DIP %s\r\n", acl_entry.acl_name);
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %d.%d.%d.%d %d.%d.%d.%d \r\n",
                    i+1,
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[0],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[1],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[2],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[3],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[0],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[1],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[2],
                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[3]
                    );
            }
        }
    } /* a7 */

    {
        UI32_T  i;
        char                *acl_name   = "a8";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, RULE_TYPE_IP_EXT_ACL) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=1; i<=50; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            SET_IP(ace.u.ip.aceSourceIpAddr, 192, 168, 1, i);
            SET_IP(ace.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("SIP %s\r\n", acl_entry.acl_name);
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %d.%d.%d.%d %d.%d.%d.%d \r\n",
                    i+1,
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[0],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[1],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[2],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[3],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[0],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[1],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[2],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[3]
                    );
            }
        }
    } /* a8 */

    {
        UI32_T  i;
        char                *acl_name   = "a9";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, RULE_TYPE_IP_EXT_ACL) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=51; i<=100; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            SET_IP(ace.u.ip.aceSourceIpAddr, 192, 168, 1, i);
            SET_IP(ace.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("SIP %s\r\n", acl_entry.acl_name);
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %d.%d.%d.%d %d.%d.%d.%d \r\n",
                    i+1,
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[0],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[1],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[2],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[3],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[0],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[1],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[2],
                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[3]
                    );
            }
        }
    } /* a9 */

    {
        UI32_T  i;
        char                *acl_name   = "a10";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, RULE_TYPE_IP_EXT_ACL) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=5001; i<=5050; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            ace.u.ip.aceDestPortOp = VAL_diffServIpAceDestPortOp_equal;
            ace.u.ip.aceMinDestPort = i;
            ace.u.ip.aceDestPortBitmask = 0xffff;

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("DPort %s\r\n", acl_entry.acl_name);
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %lu %lu \r\n",
                    i+1,
                    ace_p->u.ip.aceMinDestPort,
                    ace_p->u.ip.aceDestPortBitmask
                    );
            }
        }
    } /* a10 */

    {
        UI32_T  i;
        char                *acl_name   = "a11";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        if (RULE_TYPE_OK != RULE_OM_CreateAcl((UI8_T *) acl_name, RULE_TYPE_IP_EXT_ACL) ||
            RULE_TYPE_OK != RULE_OM_GetAclIdByName((UI8_T *) acl_name, &acl_index))
        {
            UT_LOG("Prepare ACL failed");
            return UT_FAIL;
        }

        for (i=6001; i<=6050; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            ace.u.ip.aceSourcePortOp = VAL_diffServIpAceDestPortOp_equal;
            ace.u.ip.aceMinSourcePort = i;
            ace.u.ip.aceSourcePortBitmask = 0xffff;

            if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, &ace))
            {
                UT_LOG("Prepare ACE failed");
                return UT_FAIL;
            }
        }

        if (RULE_TYPE_OK != RULE_OM_RebuildAggregateAcl(acl_index))
        {
            UT_LOG("Test RULE_OM_RebuildAggregateAcl failed");
            return UT_FAIL;
        }

        {
            UI32_T i;

            RULE_TYPE_Acl_T acl_entry;

            RULE_OM_GetAclByIndex(acl_index, &acl_entry);

            RULE_OM_GetAclByIndex(acl_entry.aggregate_acl_index, &acl_entry);

            total_count += acl_entry.ace_count;

            printf("SPort %s\r\n", acl_entry.acl_name);
            for (i=0; i<acl_entry.ace_count; ++i)
            {
                RULE_TYPE_Ace_Entry_T *ace_p = RULE_OM_LocalGetAceByIndex( acl_entry.ace_index_list[i] );

                printf("[%lu] %lu %lu \r\n",
                    i+1,
                    ace_p->u.ip.aceMinSourcePort,
                    ace_p->u.ip.aceSourcePortBitmask
                    );
            }
        }
    } /* a11 */

    printf("total count = %lu \r\n", total_count);

    return UT_OK;
}

/*
int RULE_OM_TEST_AddCount()
{
    char class_map_name[] = "c1";
    UI32_T class_map_index;
    RULE_TYPE_CounterRec_T rec;
    UI32_T i;

    rec.last_update_tick = 100;
    rec.green_octets = 222;
    rec.yellow_octets = 333;
    rec.red_octets = 444;

    RULE_OM_Create_ClassMap(class_map_name, RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map_index);

    for (i=0; i<100; i++)
    {
        RULE_OM_ClassMapExtAddCounterRec(class_map_index, 0, &rec);

        rec.last_update_tick += 100;
        rec.green_octets += 11;
        rec.yellow_octets += 2;
        rec.red_octets += 1;
    }

    return UT_OK;
}
*/

int main(int argc, const char * argv[])
{
    int res;

    UT_Init();

    UT_REGISTER(RULE_OM_TEST_Setup,
                RULE_OM_TEST_Setdown,
                RULE_OM_TEST_1
                );

    UT_REGISTER(RULE_OM_TEST_Setup,
                RULE_OM_TEST_Setdown,
                RULE_OM_TEST_BindPort2PolicyMap
                );

    UT_REGISTER(RULE_OM_TEST_Setup,
                RULE_OM_TEST_Setdown,
                RULE_OM_TEST_GetPortPolicyMapIndex
                );

    UT_REGISTER(RULE_OM_TEST_Setup,
                RULE_OM_TEST_Setdown,
                RULE_OM_TEST_GetNext_PortListByPolicyMap
                );

    UT_REGISTER(RULE_OM_TEST_Setup,
                RULE_OM_TEST_Setdown,
                RULE_OM_TEST_GetNext_PortListByClassMap
                );

    UT_REGISTER(RULE_OM_TEST_Setup,
                RULE_OM_TEST_Setdown,
                RULE_OM_TEST_GetNextPort_BindAcl
                );

    UT_REGISTER(RULE_OM_TEST_Setup,
                RULE_OM_TEST_Setdown,
                RULE_OM_TEST_GetNext_PortListByAclEx
                );

    UT_REGISTER(RULE_OM_TEST_Setup,
                RULE_OM_TEST_Setdown,
                RULE_OM_TEST_MergerMacAce_basic
                );

    UT_REGISTER(RULE_OM_TEST_Setup,
                RULE_OM_TEST_Setdown,
                RULE_OM_TEST_MergerMacAce_BMT
                );

/*
    UT_REGISTER(RULE_OM_TEST_Setup,
                RULE_OM_TEST_Setdown,
                RULE_OM_TEST_AddCount
                );
*/
    res = UT_Run();

    return res;
}
