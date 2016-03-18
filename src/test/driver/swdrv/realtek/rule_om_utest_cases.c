/*
 *  rule_om_utest_cases.c
 *  rule_ctrl
 *
 *  Created by yehjunying on 2011/4/29.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "utest.h"
#include "l_inet.h"

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
#include "sys_time.h"
#endif

/* -----------------------------------------------------------------------------
 * Help Functions
 * -----------------------------------------------------------------------------
 */
void
RULE_OM_UT_Proc_SetAceCos(
    RULE_TYPE_Ace_Entry_T *ace,
    UI32_T cos)
{
    switch (ace->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            ace->u.mac.aceCoS = cos;
            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            assert(0);
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            assert(0);
            break;

        default:
            assert(0);
            break;
    }
}

void
RULE_OM_UT_Proc_SetAceL4SrcPort(
    RULE_TYPE_Ace_Entry_T *ace,
    UI32_T port)
{
    switch (ace->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            assert(0);
            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            ace->u.ip.aceMinSourcePort = port;
            break;

        default:
            assert(0);
            break;
    }
}

void
RULE_OM_UT_Proc_SetAceDstIpAddress(
    RULE_TYPE_Ace_Entry_T *ace,
    UI8_T addr[SYS_TYPE_IPV4_ADDR_LEN],
    UI8_T mask[SYS_TYPE_IPV4_ADDR_LEN])
{
    switch (ace->ace_type)
    {
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            memcpy(&ace->u.ip.aceDestIpAddr, addr,
                                             sizeof(ace->u.ip.aceDestIpAddr));
            memcpy(&ace->u.ip.aceDestIpAddrBitmask, mask,
                                      sizeof(ace->u.ip.aceDestIpAddrBitmask));
            break;

        default:
            assert(0);
            break;
    }
}

void
RULE_OM_UT_Proc_SetAceSrcIpAddress(
    RULE_TYPE_Ace_Entry_T *ace,
    UI8_T src_addr[SYS_TYPE_IPV4_ADDR_LEN],
    UI8_T src_addr_bitmask[SYS_TYPE_IPV4_ADDR_LEN])
{
    switch (ace->ace_type)
    {
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            memcpy(&ace->u.ip.aceSourceIpAddr, src_addr,
                                             sizeof(ace->u.ip.aceSourceIpAddr));
            memcpy(&ace->u.ip.aceSourceIpAddrBitmask, src_addr_bitmask,
                                      sizeof(ace->u.ip.aceSourceIpAddrBitmask));
            break;

        default:
            assert(0);
            break;
    }
}

void
RULE_OM_UT_Proc_SetAceDstMacAddress(
    RULE_TYPE_Ace_Entry_T *ace,
    UI8_T mac[SYS_ADPT_MAC_ADDR_LEN],
    UI8_T mask[SYS_ADPT_MAC_ADDR_LEN])
{
    switch (ace->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            memcpy(&ace->u.mac.aceDestMacAddr, mac, SYS_ADPT_MAC_ADDR_LEN);
            memcpy(&ace->u.mac.aceDestMacAddrBitmask, mask, SYS_ADPT_MAC_ADDR_LEN);
            break;

        default:
            assert(0);
            break;
    }
}

void
RULE_OM_UT_Proc_SetAceSrcMacAddress(
    RULE_TYPE_Ace_Entry_T *ace,
    UI8_T mac[SYS_ADPT_MAC_ADDR_LEN],
    UI8_T mask[SYS_ADPT_MAC_ADDR_LEN])
{
    switch (ace->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            memcpy(&ace->u.mac.aceSourceMacAddr, mac, SYS_ADPT_MAC_ADDR_LEN);
            memcpy(&ace->u.mac.aceSourceMacAddrBitmask, mask, SYS_ADPT_MAC_ADDR_LEN);
            break;

        default:
            assert(0);
            break;
    }
}

void
RULE_OM_UT_Proc_SetAceSrcIpv6Address(
    RULE_TYPE_Ace_Entry_T *ace,
    char *ipv6_addr)
{
    L_INET_AddrIp_T inet;
    L_INET_RETURN_VALUE_T l_inet_result;

    l_inet_result = L_INET_StringToInaddr(L_INET_FORMAT_IPV6_ADDR_RAW, ipv6_addr,
                                          (L_INET_Addr_T *)&inet, sizeof(inet));
    
    assert(L_INET_RETURN_SUCCESS == l_inet_result);
    
    switch (ace->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            memcpy(&ace->u.ipv6.aceSourceIpAddr, inet.addr, SYS_TYPE_IPV6_ADDR_LEN);
            RULE_OM_LocalConvertIpv6Prefix2Mask(inet.preflen,
                                                ace->u.ipv6.aceSourceIpAddrBitmask);
            //ace->u.ipv6.aceSourceIpAddrPrefixLen = inet.preflen;
            break;

        default:
            assert(0);
            break;
    }
}

void
RULE_OM_UT_Proc_SetAceDstIpv6Address(
    RULE_TYPE_Ace_Entry_T *ace,
    char *ipv6_addr)
{
    L_INET_AddrIp_T inet;
    L_INET_RETURN_VALUE_T l_inet_result;

    l_inet_result = L_INET_StringToInaddr(L_INET_FORMAT_IPV6_ADDR_RAW, ipv6_addr,
                                          (L_INET_Addr_T *)&inet, sizeof(inet));

    assert(L_INET_RETURN_SUCCESS == l_inet_result);

    switch (ace->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            memcpy(&ace->u.ipv6.aceDestIpAddr, inet.addr, SYS_TYPE_IPV6_ADDR_LEN);
            RULE_OM_LocalConvertIpv6Prefix2Mask(inet.preflen,
                                                ace->u.ipv6.aceDestIpAddrBitmask);
            //ace->u.ipv6.aceDestIpAddrPrefixLen = inet.preflen;
            break;

        default:
            assert(0);
            break;
    }
}

void
RULE_OM_UT_Proc_SetAceFlowLabel(
    RULE_TYPE_Ace_Entry_T *ace,
    UI32_T flow_label)
{
    switch (ace->ace_type)
    {
        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            ace->u.ipv6.aceFlowLabel = flow_label;
            break;

        default:
            assert(0);
            break;
    }
}

#if (SYS_CPNT_DAI == TRUE)
void
RULE_OM_UT_Proc_SetAceSenderIpAddress(
    RULE_TYPE_Ace_Entry_T *ace,
    UI8_T src_addr[SYS_TYPE_IPV4_ADDR_LEN],
    UI8_T src_addr_bitmask[SYS_TYPE_IPV4_ADDR_LEN])
{
    switch (ace->ace_type)
    {
        case RULE_TYPE_ARP_ACL:
            memcpy(&ace->u.arp.aceSenderIpAddr, src_addr,
                   sizeof(ace->u.arp.aceSenderIpAddr));
            memcpy(&ace->u.arp.aceSenderIpAddrBitmask, src_addr_bitmask,
                   sizeof(ace->u.arp.aceSenderIpAddrBitmask));
            break;

        default:
            assert(0);
            break;
    }
}
#endif // SYS_CPNT_DAI

void
RULE_OM_UT_Proc_Validate_Leaks_ACE()
{
    RULE_TYPE_Ace_Entry_T *dup_ace_table = malloc(sizeof(shmem_data_p->ace_table));

    UI32_T i;

    assert(NULL != dup_ace_table);

    memcpy(dup_ace_table, shmem_data_p->ace_table, sizeof(shmem_data_p->ace_table));

    //
    // Referred by ACL
    //
    for (i = 0; i < _countof(shmem_data_p->acl_table); ++i)
    {
        RULE_TYPE_Acl_T *acl_p = &shmem_data_p->acl_table[i];
        RULE_TYPE_Ace_Entry_T *ace_entry_p;

        UI32_T ace_index;
        UI32_T ace_count;

        if (0 == acl_p->row_status)
        {
            continue;
        }

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_entry_p);
             ++ace_count)
        {
            assert(0 != ace_entry_p->row_status);

            assert(0 != dup_ace_table[ ace_index - 1 ].row_status);

            dup_ace_table[ ace_index - 1 ].row_status = 0;
        }

        assert(ace_count == acl_p->ace_count);
    }

    //
    // Referred by Class Map
    //
    for (i = 0; i < _countof(shmem_data_p->class_map_table); ++i)
    {
        RULE_TYPE_ClassMap_T *class_map_p = &shmem_data_p->class_map_table[i];

        UI32_T j;

        if (0 == class_map_p->row_status)
        {
            continue;
        }

        for (j = 0; j < class_map_p->class_element_count; ++j)
        {
            UI32_T class_type = class_map_p->class_type[j];
            UI32_T class_id   = class_map_p->class_index_list[j];

            if (RULE_TYPE_CLASS_MF != class_type)
            {
                continue;
            }

            assert(0 != shmem_data_p->ace_table[ class_id - 1 ].row_status);

            assert(0 != dup_ace_table[ class_id - 1 ].row_status);

            dup_ace_table[ class_id - 1 ].row_status = 0;
        }
    }

    for (i = 0; i < _countof(shmem_data_p->ace_table); ++i)
    {
        RULE_TYPE_Ace_Entry_T *ace_entry_p;

        ace_entry_p = &dup_ace_table[i];

        assert(0 == ace_entry_p->row_status);
    }

    free(dup_ace_table);
}

void
RULE_OM_UT_Proc_Validate_Leaks()
{
    RULE_OM_UT_Proc_Validate_Leaks_ACE();
}

void
RULE_OM_UT_Proc_Validate_Rule()
{
    RULE_OM_UT_Proc_Validate_Leaks();
}

/* -----------------------------------------------------------------------------
 * Test Functions
 * -----------------------------------------------------------------------------
 */

int
RULE_OM_UT_Setup()
{
    RULE_OM_InitiateSystemResources();

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    SYS_TIME_InitiateSystemResources();
#endif // SYS_CPNT_TIME_BASED_ACL
    return 0;
}

int
RULE_OM_UT_Teardown()
{
    RULE_OM_UT_Proc_Validate_Rule();
    return 0;
}

int
RULE_OM_UT_Add_And_Remove_Ace_2_Acl()
{
    enum
    {
        MAX_RETRY_COUNT = 2,
        MAX_ACE         = 10,
    };

    typedef struct
    {
        UI32_T ace_index;
        RULE_TYPE_Ace_Entry_T   ace_entry;
    } ace_entry_t;

    const char *acl_name = "a1";

    int retry_count = MAX_RETRY_COUNT;

    UI32_T acl_index;
    UI32_T ace_index;
    UI8_T  src_addr[SYS_TYPE_IPV4_ADDR_LEN];
    UI8_T  src_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

    UI32_T i;

    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_STD_ACL;
    RULE_TYPE_AclType_T ace_type;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_Acl_T         acl_entry;
    RULE_TYPE_Ace_Entry_T   ace_entry;

    ace_entry_t             ace_list[MAX_ACE];

    memset(src_addr, 0, sizeof(src_addr));
    memset(src_addr_mask, 0xff, sizeof(src_addr_mask));

    result = RULE_OM_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    assert(RULE_TYPE_OK == result);

    //
    // Empty
    //
    for (ace_index = 0, i = 0;
         RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
         ++i)
    {
    }

    assert(acl_entry.ace_count == 0);


retry:
    for (i = 0; i < _countof(ace_list); ++i, src_addr[3] += 2)
    {
        ace_list[i].ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_list[i].ace_entry);
        assert(RULE_TYPE_OK == result);

        RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_list[i].ace_entry, src_addr, src_addr_mask);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_list[i].ace_entry);
        assert(RULE_TYPE_OK == result);
    }

    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    assert(RULE_TYPE_OK == result);

    for (ace_index = 0, i = 0;
         RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
         ++i)
    {
        assert(i < _countof(ace_list));

        assert(0 == memcmp(&ace_list[i].ace_entry.u, &ace_entry.u, sizeof(ace_entry.u)));

        ace_list[i].ace_index = ace_index;
//        memcpy(&ace_list[i].ace_entry, &ace_entry, sizeof(ace_entry));
//        memset(&ace_list[i].ace_entry.super, 0, sizeof(ace_list[i].ace_entry.super));
    }

    assert(acl_entry.ace_count == i);

    for (i = 0; i < _countof(ace_list); ++i)
    {
        result = RULE_OM_DelAceFromAcl(acl_index, ace_list[i].ace_entry.ace_type, &ace_list[i].ace_entry);
        assert(RULE_TYPE_OK == result);

        memset(&ace_list[i], 0, sizeof(ace_list[i]));
    }

    RULE_OM_UT_Proc_Validate_Rule();

    for (ace_index = 0, i = 0;
         RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
         ++i)
    {
    }

    assert(0 == i);

    if (0 < retry_count --)
    {
        goto retry;
    }

    return 0;
}

int
RULE_OM_UT_Bind_Unbind_Element_2_Acl()
{
    enum
    {
        MAX_ACL  = 5,
        MAX_ACE  = 5,
    };

    typedef struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;

        UI32_T ace_type_list[MAX_ACE];
        UI32_T ace_index_list[MAX_ACE];
    } acl_entry_t;

    RULE_TYPE_AclType_T acl_type_ar[] =
    {
        RULE_TYPE_MAC_ACL,
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IP_EXT_ACL,
        RULE_TYPE_IPV6_STD_ACL,
        RULE_TYPE_IPV6_EXT_ACL,
    };

    UI32_T i;
    UI32_T g_ace_index = 1;

    RULE_TYPE_RETURN_TYPE_T result;

    acl_entry_t acl_list[MAX_ACL];

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_ar[ i%_countof(acl_type_ar) ];
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        acl_list[i].acl_type = acl_type;

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T field_index;
            UI32_T ace_index;

            UI8_T  src_ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  src_ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  src_mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  src_mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   src_ipv6_addr[L_INET_MAX_IPADDR_STR_LEN+1];

            //
            // Create ACE
            //
            result = RULE_OM_CreateAce(g_ace_index, acl_type, TRUE);
            assert(RULE_TYPE_OK == result);

            acl_list[i].ace_type_list[ ace_count ]  = acl_type;
            acl_list[i].ace_index_list[ ace_count ] = g_ace_index;

            g_ace_index ++;

            ace_index = acl_list[i].ace_index_list[ ace_count ];
            //
            //

            result = RULE_OM_BindElement2Acl(acl_list[i].acl_index, acl_type, ace_index);
            assert(RULE_TYPE_OK == result);

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_ipv4_addr, 0, sizeof(src_ipv4_addr));
            memset(src_ipv4_addr_mask, 0xff, sizeof(src_ipv4_addr_mask));

            memset(src_mac_addr, 0, sizeof(src_mac_addr));
            memset(src_mac_addr_mask, 0xff, sizeof(src_mac_addr_mask));

            src_ipv4_addr[3] = ace_count;
            src_mac_addr[5] = ace_count;

            assert(ace_count <= 255);

            snprintf(src_ipv6_addr, sizeof(src_ipv6_addr), "fe%02x::", (UI8_T)ace_count);
            src_ipv6_addr[ sizeof(src_ipv6_addr)-1 ] = '\0';

            switch (ace_entry.ace_type)
            {
                case RULE_TYPE_MAC_ACL:
                    field_index = LEAF_diffServMacAceSourceMacAddr;
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac_addr, src_mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    field_index = LEAF_diffServIpAceSourceIpAddr;
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, src_ipv4_addr, src_ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    field_index = LEAF_diffServIpv6AceSourceIpAddr;
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    field_index = LEAF_diffServIpv6AceSourceIpAddr;
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    field_index = LEAF_diffServIpv6AceDestIpAddr;
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, src_ipv6_addr);
#endif
                    break;

                default:
                    assert(0);
                    break;
            }

            result = RULE_OM_SetAceFieldByIndex(ace_index, &ace_entry, field_index, RULE_TYPE_ACE_OWNED_BY_ACL);
            assert(RULE_TYPE_OK == result);
        }
    }

    RULE_OM_UT_Proc_Validate_Rule();

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(acl_list[i].ace_index_list));

            assert(acl_list[i].ace_index_list[ ace_count ] == ace_index);
            assert(acl_list[i].ace_type_list[ ace_count ] == ace_type);
            assert(acl_list[i].ace_type_list[ ace_count ] == acl_list[i].acl_type);
        }

        assert(ace_count == _countof(acl_list[i].ace_index_list));
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        UI32_T ace_count;
        UI32_T acl_index = acl_list[i].acl_index;

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T ace_index = acl_list[i].ace_index_list[ace_count];
            UI32_T ace_type  = acl_list[i].ace_type_list[ace_count];

            result = RULE_OM_UnbindElementFromAcl(acl_index, ace_type, ace_index);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_DestroyAce(ace_index);
            assert(RULE_TYPE_OK == result);
        }

        RULE_OM_UT_Proc_Validate_Rule();
    }

    return 0;
}

int
RULE_OM_UT_Bind_Unbind_Element_2_Acl_Use_Wrong_Ace_Index()
{
    enum
    {
        MAX_ACL  = 5,
        MAX_ACE  = 5,
    };

    typedef struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;

        UI32_T ace_type_list[MAX_ACE];
        UI32_T ace_index_list[MAX_ACE];
    } acl_entry_t;

    RULE_TYPE_AclType_T acl_type_ar[] =
    {
        RULE_TYPE_MAC_ACL,
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IP_EXT_ACL,
        RULE_TYPE_IPV6_STD_ACL,
        RULE_TYPE_IPV6_EXT_ACL,
    };

    UI32_T i;
    UI32_T g_ace_index = 1;

    RULE_TYPE_RETURN_TYPE_T result;

    acl_entry_t acl_list[MAX_ACL];

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_ar[ i%_countof(acl_type_ar) ];
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        acl_list[i].acl_type = acl_type;

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T field_index;
            UI32_T ace_index;

            UI8_T  src_ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  src_ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  src_mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  src_mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   src_ipv6_addr[L_INET_MAX_IPADDR_STR_LEN+1];

            //
            // Create ACE
            //
            result = RULE_OM_CreateAce(g_ace_index, acl_type, TRUE);
            assert(RULE_TYPE_OK == result);

            acl_list[i].ace_type_list[ ace_count ]  = acl_type;
            acl_list[i].ace_index_list[ ace_count ] = g_ace_index;

            g_ace_index ++;

            ace_index = acl_list[i].ace_index_list[ ace_count ];
            //
            //

            result = RULE_OM_BindElement2Acl(acl_list[i].acl_index, acl_type, ace_index);
            assert(RULE_TYPE_OK == result);

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_ipv4_addr, 0, sizeof(src_ipv4_addr));
            memset(src_ipv4_addr_mask, 0xff, sizeof(src_ipv4_addr_mask));

            memset(src_mac_addr, 0, sizeof(src_mac_addr));
            memset(src_mac_addr_mask, 0xff, sizeof(src_mac_addr_mask));

            src_ipv4_addr[3] = ace_count;
            src_mac_addr[5] = ace_count;

            assert(ace_count <= 255);

            snprintf(src_ipv6_addr, sizeof(src_ipv6_addr), "fe%02x::", (UI8_T)ace_count);
            src_ipv6_addr[ sizeof(src_ipv6_addr)-1 ] = '\0';

            switch (ace_entry.ace_type)
            {
                case RULE_TYPE_MAC_ACL:
                    field_index = LEAF_diffServMacAceSourceMacAddr;
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac_addr, src_mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    field_index = LEAF_diffServIpAceSourceIpAddr;
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, src_ipv4_addr, src_ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    field_index = LEAF_diffServIpv6AceSourceIpAddr;
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    field_index = LEAF_diffServIpv6AceSourceIpAddr;
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    field_index = LEAF_diffServIpv6AceDestIpAddr;
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, src_ipv6_addr);
#endif
                    break;

                default:
                    assert(0);
                    break;
            }

            result = RULE_OM_SetAceFieldByIndex(ace_index, &ace_entry, field_index, RULE_TYPE_ACE_OWNED_BY_ACL);
            assert(RULE_TYPE_OK == result);
        }
    }

    RULE_OM_UT_Proc_Validate_Rule();

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(acl_list[i].ace_index_list));

            assert(acl_list[i].ace_index_list[ ace_count ] == ace_index);
            assert(acl_list[i].ace_type_list[ ace_count ] == ace_type);
            assert(acl_list[i].ace_type_list[ ace_count ] == acl_list[i].acl_type);
        }

        assert(ace_count == _countof(acl_list[i].ace_index_list));
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        UI32_T ace_count;
        UI32_T acl_index = acl_list[i].acl_index;

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T wrong_ace_index = acl_list[(i+1) % MAX_ACL].ace_index_list[ace_count];
            UI32_T wrong_ace_type  = acl_list[(i+1) % MAX_ACL].ace_type_list[ace_count];

            result = RULE_OM_UnbindElementFromAcl(acl_index, wrong_ace_type, wrong_ace_index);
            assert(RULE_TYPE_OK != result);
        }

        RULE_OM_UT_Proc_Validate_Rule();
    }

    return 0;
}

int
RULE_OM_UT_Get_Acl_Index_And_Type_By_Ace()
{
    enum
    {
        MAX_ACL  = 5,
        MAX_ACE  = 5,
    };

    typedef struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
        UI32_T ace_index_list[MAX_ACE];
    } acl_entry_t;

    RULE_TYPE_AclType_T acl_type_ar[] =
    {
        RULE_TYPE_MAC_ACL,
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IP_EXT_ACL,
        RULE_TYPE_IPV6_STD_ACL,
        RULE_TYPE_IPV6_EXT_ACL,
    };

    UI32_T i;

    RULE_TYPE_RETURN_TYPE_T result;

    acl_entry_t acl_list[MAX_ACL];

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_ar[ i%_countof(acl_type_ar) ];
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        acl_list[i].acl_type = acl_type;

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            UI8_T  src_ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  src_ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  src_mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  src_mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   src_ipv6_addr[L_INET_MAX_IPADDR_STR_LEN+1];

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_ipv4_addr, 0, sizeof(src_ipv4_addr));
            memset(src_ipv4_addr_mask, 0xff, sizeof(src_ipv4_addr_mask));

            memset(src_mac_addr, 0, sizeof(src_mac_addr));
            memset(src_mac_addr_mask, 0xff, sizeof(src_mac_addr_mask));

            src_ipv4_addr[3] = ace_count;
            src_mac_addr[5] = ace_count;

            assert(ace_count <= 255);

            snprintf(src_ipv6_addr, sizeof(src_ipv6_addr), "fe%02x::", (UI8_T)ace_count);
            src_ipv6_addr[ sizeof(src_ipv6_addr)-1 ] = '\0';

            switch (ace_entry.ace_type)
            {
                case RULE_TYPE_MAC_ACL:
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac_addr, src_mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, src_ipv4_addr, src_ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, src_ipv6_addr);
#endif
                    break;

                default:
                    assert(0);
                    break;
            }

            result = RULE_OM_AddAce2Acl(acl_list[i].acl_index, ace_entry.ace_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(acl_list[i].ace_index_list));

            acl_list[i].ace_index_list[ ace_count ] = ace_index;
        }

        assert(ace_count == _countof(acl_list[i].ace_index_list));
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        UI32_T ace_count;

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            RULE_TYPE_AclType_T result_acl_type;

            UI32_T ace_index = acl_list[i].ace_index_list[ace_count];
            UI32_T result_acl_index;

            result = RULE_OM_GetAclIndexByAce(ace_index, &result_acl_index);
            assert(RULE_TYPE_OK == result);

            assert(acl_list[i].acl_index == result_acl_index);

            result = RULE_OM_GetAclTypeByAce(ace_index, &result_acl_type);
            assert(RULE_TYPE_OK == result);

            assert(acl_list[i].acl_type == result_acl_type);
        }
    }

    return 0;
}

int
RULE_OM_UT_Del_Acl()
{
    enum
    {
        MAX_ACL  = 5,
        MAX_ACE  = 5,
        ACL_TYPE = RULE_TYPE_IP_STD_ACL,
    };

    typedef struct
    {
        UI32_T acl_index;
    } acl_entry_t;

    UI32_T i;

    RULE_TYPE_RETURN_TYPE_T result;

    acl_entry_t acl_list[MAX_ACL];

    for (i = 0; i < _countof(acl_list); ++i)
    {
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        UI8_T  src_addr[SYS_TYPE_IPV4_ADDR_LEN];
        UI8_T  src_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, ACL_TYPE);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        for (ace_count = 0; ace_count < MAX_ACE; ++ace_count)
        {
            ace_entry.ace_type = ACL_TYPE;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_addr, 0, sizeof(src_addr));
            memset(src_addr_mask, 0xff, sizeof(src_addr_mask));

            src_addr[3] = ace_count;

            RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, src_addr, src_addr_mask);

            result = RULE_OM_AddAce2Acl(acl_list[i].acl_index, ace_entry.ace_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        result = RULE_OM_DelAcl(acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);
    }

    return 0;
}

int
RULE_OM_UT_Get_Number_Of_Permit_Access_By_Acl_Index()
{
    enum
    {
        MAX_ACL  = 5,
        MAX_ACE  = 10,
    };

    typedef struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
        UI32_T ace_index_list[MAX_ACE];
        UI32_T permit_ace_count;
    } acl_entry_t;

    RULE_TYPE_AclType_T acl_type_ar[] =
    {
        RULE_TYPE_MAC_ACL,
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IP_EXT_ACL,
        RULE_TYPE_IPV6_STD_ACL,
        RULE_TYPE_IPV6_EXT_ACL,
    };

    UI32_T i;

    RULE_TYPE_RETURN_TYPE_T result;

    acl_entry_t acl_list[MAX_ACL];

    memset(acl_list, 0, sizeof(acl_list));

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_ar[ i%_countof(acl_type_ar) ];
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        acl_list[i].acl_type = acl_type;

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T access;

            UI8_T  src_ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  src_ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  src_mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  src_mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   src_ipv6_addr[L_INET_MAX_IPADDR_STR_LEN+1];

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_ipv4_addr, 0, sizeof(src_ipv4_addr));
            memset(src_ipv4_addr_mask, 0xff, sizeof(src_ipv4_addr_mask));

            memset(src_mac_addr, 0, sizeof(src_mac_addr));
            memset(src_mac_addr_mask, 0xff, sizeof(src_mac_addr_mask));

            src_ipv4_addr[3] = ace_count;
            src_mac_addr[5] = ace_count;

            assert(ace_count <= 255);

            snprintf(src_ipv6_addr, sizeof(src_ipv6_addr), "fe%02x::", (UI8_T)ace_count);
            src_ipv6_addr[ sizeof(src_ipv6_addr)-1 ] = '\0';

            switch (ace_entry.ace_type)
            {
                case RULE_TYPE_MAC_ACL:
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac_addr, src_mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, src_ipv4_addr, src_ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, src_ipv6_addr);
#endif
                    break;

                default:
                    assert(0);
                    break;
            }

            access = (0 == ace_count % 2) ? RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;
            ace_entry.access = access;

            result = RULE_OM_AddAce2Acl(acl_list[i].acl_index, ace_entry.ace_type, &ace_entry);
            assert(RULE_TYPE_OK == result);

            if (RULE_TYPE_ACE_PERMIT == access)
            {
                acl_list[i].permit_ace_count ++;
            }
        }
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(acl_list[i].ace_index_list));

            acl_list[i].ace_index_list[ ace_count ] = ace_index;
        }

        assert(ace_count == _countof(acl_list[i].ace_index_list));
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        UI32_T result_ace_count;

        result = RULE_OM_GetNumberOfPermitAcesByAclIndex(acl_list[i].acl_index, &result_ace_count);
        assert(RULE_TYPE_OK == result);

        assert(acl_list[i].permit_ace_count == result_ace_count);
    }

    return 0;
}

int
RULE_OM_UT_Arp_ACL_Check_For_Dai()
{
#if (SYS_CPNT_DAI == TRUE)
    enum
    {
        MAX_ACL  = 2,
        MAX_ACE  = 10,
    };

    typedef struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
        UI32_T ace_index_list[MAX_ACE];
        UI32_T ace_access[MAX_ACE];
        UI8_T  ace_sender_ipv4_addr[MAX_ACE][SYS_TYPE_IPV4_ADDR_LEN];
        UI8_T  ace_sender_ipv4_addr_mask[MAX_ACE][SYS_TYPE_IPV4_ADDR_LEN];
    } acl_entry_t;

    RULE_TYPE_AclType_T acl_type_ar[] =
    {
        RULE_TYPE_ARP_ACL,
    };

    UI32_T i;

    RULE_TYPE_RETURN_TYPE_T result;

    acl_entry_t acl_list[MAX_ACL];

    memset(acl_list, 0, sizeof(acl_list));

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_ar[ i%_countof(acl_type_ar) ];
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        acl_list[i].acl_type = acl_type;

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T access;

            UI8_T  src_ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  src_ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_ipv4_addr, 0, sizeof(src_ipv4_addr));
            memset(src_ipv4_addr_mask, 0xff, sizeof(src_ipv4_addr_mask));

            src_ipv4_addr[3] = ace_count;

            switch (ace_entry.ace_type)
            {
                case RULE_TYPE_ARP_ACL:
                    RULE_OM_UT_Proc_SetAceSenderIpAddress(&ace_entry, src_ipv4_addr, src_ipv4_addr_mask);
                    break;

                default:
                    assert(0);
                    break;
            }

            access = (0 == ace_count % 2) ? RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;
            ace_entry.access = access;

            result = RULE_OM_AddAce2Acl(acl_list[i].acl_index, ace_entry.ace_type, &ace_entry);
            assert(RULE_TYPE_OK == result);

            acl_list[i].ace_access[ace_count] = access;
            memcpy(acl_list[i].ace_sender_ipv4_addr[ace_count], src_ipv4_addr,
                           sizeof(acl_list[i].ace_sender_ipv4_addr[ace_count]));
            memcpy(acl_list[i].ace_sender_ipv4_addr_mask[ace_count], src_ipv4_addr_mask,
                      sizeof(acl_list[i].ace_sender_ipv4_addr_mask[ace_count]));
        }
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(acl_list[i].ace_index_list));

            acl_list[i].ace_index_list[ ace_count ] = ace_index;
        }

        assert(ace_count == _countof(acl_list[i].ace_index_list));
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        UI32_T ace_count;

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            RULE_TYPE_ArpPktFormat_T arp_pkt;

            char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1];

            BOOL_T result_drop;

            memset(&arp_pkt, 0, sizeof(arp_pkt));

            memcpy(&arp_pkt.srcIp, &acl_list[i].ace_sender_ipv4_addr[ace_count], sizeof(arp_pkt.srcIp));

            result = RULE_OM_GetAclNameById(acl_list[i].acl_index, acl_name);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_ArpACLCheckForDai(acl_name, &arp_pkt, &result_drop);
            assert(RULE_TYPE_OK == result);

            if (RULE_TYPE_ACE_PERMIT == acl_list[i].ace_access[ace_count])
            {
                assert(FALSE == result_drop);
            }
            else
            {
                assert(TRUE == result_drop);
            }

        }
    }
#endif // SYS_CPNT_DAI

    return 0;
}

int
RULE_OM_UT_Copy_ACL()
{
    enum
    {
        MAX_ACL      = 10,
        MAX_SRC_ACE  = SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM / MAX_ACL / 2,
        MAX_DST_ACE  = SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM / MAX_ACL / 2,
    };

    typedef struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
        UI32_T ace_index_list[MAX_SRC_ACE];
    } src_acl_entry_t;

    typedef struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
        UI32_T ace_index_list[MAX_DST_ACE];
    } dst_acl_entry_t;


    RULE_TYPE_AclType_T acl_type_ar[] =
    {
        RULE_TYPE_MAC_ACL,
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IP_EXT_ACL,
        RULE_TYPE_IPV6_STD_ACL,
        RULE_TYPE_IPV6_EXT_ACL,
    };

    UI32_T i;

    RULE_TYPE_RETURN_TYPE_T result;

    src_acl_entry_t src_acl_list[MAX_ACL];
    dst_acl_entry_t dst_acl_list[MAX_ACL];

    memset(src_acl_list, 0, sizeof(src_acl_list));
    memset(dst_acl_list, 0, sizeof(dst_acl_list));

    for (i = 0; i < _countof(src_acl_list); ++i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_ar[ i%_countof(acl_type_ar) ];
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "src-%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &src_acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        src_acl_list[i].acl_type = acl_type;

        for (ace_count = 0; ace_count < _countof(src_acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T access;

            UI8_T  src_ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  src_ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  src_mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  src_mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   src_ipv6_addr[L_INET_MAX_IPADDR_STR_LEN+1];

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_ipv4_addr, 0, sizeof(src_ipv4_addr));
            memset(src_ipv4_addr_mask, 0xff, sizeof(src_ipv4_addr_mask));

            memset(src_mac_addr, 0, sizeof(src_mac_addr));
            memset(src_mac_addr_mask, 0xff, sizeof(src_mac_addr_mask));

            src_ipv4_addr[3] = ace_count;
            src_mac_addr[5] = ace_count;

            assert(ace_count <= 255);

            snprintf(src_ipv6_addr, sizeof(src_ipv6_addr), "fe%02x::", (UI8_T)ace_count);
            src_ipv6_addr[ sizeof(src_ipv6_addr)-1 ] = '\0';

            switch (ace_entry.ace_type)
            {
                case RULE_TYPE_MAC_ACL:
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac_addr, src_mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, src_ipv4_addr, src_ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, src_ipv6_addr);
#endif
                    break;

                default:
                    assert(0);
                    break;
            }

            access = (0 == ace_count % 2) ? RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;
            ace_entry.access = access;

            result = RULE_OM_AddAce2Acl(src_acl_list[i].acl_index, ace_entry.ace_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (i = 0; i < _countof(src_acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(src_acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(src_acl_list[i].ace_index_list));

            src_acl_list[i].ace_index_list[ ace_count ] = ace_index;
        }

        assert(ace_count == _countof(src_acl_list[i].ace_index_list));
    }

    for (i = 0; i < _countof(dst_acl_list); ++i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_ar[ (i+1)%_countof(acl_type_ar) ];
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "dst-%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &dst_acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        dst_acl_list[i].acl_type = acl_type;

        for (ace_count = 0; ace_count < _countof(dst_acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T access;

            UI8_T  src_ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  src_ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  src_mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  src_mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   src_ipv6_addr[L_INET_MAX_IPADDR_STR_LEN+1];

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_ipv4_addr, 0, sizeof(src_ipv4_addr));
            memset(src_ipv4_addr_mask, 0xff, sizeof(src_ipv4_addr_mask));

            memset(src_mac_addr, 0, sizeof(src_mac_addr));
            memset(src_mac_addr_mask, 0xff, sizeof(src_mac_addr_mask));

            src_ipv4_addr[3] = ace_count;
            src_mac_addr[5] = ace_count;

            assert(ace_count <= 255);

            snprintf(src_ipv6_addr, sizeof(src_ipv6_addr), "fe%02x::", (UI8_T)ace_count);
            src_ipv6_addr[ sizeof(src_ipv6_addr)-1 ] = '\0';

            switch (ace_entry.ace_type)
            {
                case RULE_TYPE_MAC_ACL:
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac_addr, src_mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, src_ipv4_addr, src_ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, src_ipv6_addr);
#endif
                    break;

                default:
                    assert(0);
                    break;
            }

            access = (0 == ace_count % 2) ? RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;
            ace_entry.access = access;

            result = RULE_OM_AddAce2Acl(dst_acl_list[i].acl_index, ace_entry.ace_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (i = 0; i < _countof(dst_acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(dst_acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(dst_acl_list[i].ace_index_list));

            dst_acl_list[i].ace_index_list[ ace_count ] = ace_index;
        }

        assert(ace_count == _countof(dst_acl_list[i].ace_index_list));
    }

    for (i = 0; i < _countof(dst_acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T src_acl_index = src_acl_list[i].acl_index;
        UI32_T dst_acl_index = dst_acl_list[i].acl_index;
        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T src_ace_entry;
        RULE_TYPE_Ace_Entry_T dst_ace_entry;

        result = RULE_OM_LocalCopyAcl(src_acl_index, dst_acl_index);
        assert(RULE_TYPE_OK == result);

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(dst_acl_list[i].acl_index, &ace_type, &ace_index, &dst_ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(src_acl_list[i].ace_index_list));

            result = RULE_OM_GetAceByIndex(src_acl_list[i].ace_index_list[ace_count], &src_ace_entry);
            assert(RULE_TYPE_OK == result);

            assert(src_ace_entry.ace_type == dst_ace_entry.ace_type);

            switch (dst_ace_entry.ace_type)
            {
                case RULE_TYPE_MAC_ACL:
                    assert(0 == memcmp(&src_ace_entry.u.mac, &dst_ace_entry.u.mac, sizeof(src_ace_entry.u.mac)));
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    assert(0 == memcmp(&src_ace_entry.u.ip, &dst_ace_entry.u.ip, sizeof(src_ace_entry.u.ip)));
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                case RULE_TYPE_IPV6_EXT_ACL:
                    assert(0 == memcmp(&src_ace_entry.u.ipv6, &dst_ace_entry.u.ipv6, sizeof(src_ace_entry.u.ipv6)));
                    break;

                default:
                    assert(0);
                    break;
            }
        }

        assert(ace_count == _countof(src_acl_list[i].ace_index_list));

        RULE_OM_UT_Proc_Validate_Rule();
    }

    return 0;
}

int
RULE_OM_UT_Copy_ACL_Undo()
{
    enum
    {
        MAX_ACL      = 20,
        MAX_SRC_ACE  = (SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM / MAX_ACL) / 3 +
                       (SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM / MAX_ACL) / 3,
        MAX_DST_ACE  = (SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM / MAX_ACL) / 3,
    };

    typedef struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
        UI32_T ace_index_list[MAX_SRC_ACE];
    } src_acl_entry_t;

    typedef struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
        UI32_T ace_index_list[MAX_DST_ACE];
    } dst_acl_entry_t;


    RULE_TYPE_AclType_T acl_type_ar[] =
    {
        RULE_TYPE_MAC_ACL,
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IP_EXT_ACL,
        RULE_TYPE_IPV6_STD_ACL,
        RULE_TYPE_IPV6_EXT_ACL,
    };

    UI32_T i;

    RULE_TYPE_RETURN_TYPE_T result;

    src_acl_entry_t src_acl_list[MAX_ACL];
    dst_acl_entry_t dst_acl_list[MAX_ACL];

    memset(src_acl_list, 0, sizeof(src_acl_list));
    memset(dst_acl_list, 0, sizeof(dst_acl_list));

    for (i = 0; i < _countof(src_acl_list); ++i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_ar[ i%_countof(acl_type_ar) ];
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "src-%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &src_acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        src_acl_list[i].acl_type = acl_type;

        for (ace_count = 0; ace_count < _countof(src_acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T access;

            UI8_T  src_ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  src_ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  src_mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  src_mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   src_ipv6_addr[L_INET_MAX_IPADDR_STR_LEN+1];

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_ipv4_addr, 0, sizeof(src_ipv4_addr));
            memset(src_ipv4_addr_mask, 0xff, sizeof(src_ipv4_addr_mask));

            memset(src_mac_addr, 0, sizeof(src_mac_addr));
            memset(src_mac_addr_mask, 0xff, sizeof(src_mac_addr_mask));

            src_ipv4_addr[3] = ace_count;
            src_mac_addr[5] = ace_count;

            assert(ace_count <= 255);

            snprintf(src_ipv6_addr, sizeof(src_ipv6_addr), "fe%02x::", (UI8_T)ace_count);
            src_ipv6_addr[ sizeof(src_ipv6_addr)-1 ] = '\0';

            switch (ace_entry.ace_type)
            {
                case RULE_TYPE_MAC_ACL:
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac_addr, src_mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, src_ipv4_addr, src_ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, src_ipv6_addr);
#endif
                    break;

                default:
                    assert(0);
                    break;
            }

            access = (0 == ace_count % 2) ? RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;
            ace_entry.access = access;

            result = RULE_OM_AddAce2Acl(src_acl_list[i].acl_index, ace_entry.ace_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (i = 0; i < _countof(src_acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(src_acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(src_acl_list[i].ace_index_list));

            src_acl_list[i].ace_index_list[ ace_count ] = ace_index;
        }

        assert(ace_count == _countof(src_acl_list[i].ace_index_list));
    }

    for (i = 0; i < _countof(dst_acl_list); ++i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_ar[ (i+1)%_countof(acl_type_ar) ];
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "dst-%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &dst_acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        dst_acl_list[i].acl_type = acl_type;

        for (ace_count = 0; ace_count < _countof(dst_acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T access;

            UI8_T  src_ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  src_ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  src_mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  src_mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   src_ipv6_addr[L_INET_MAX_IPADDR_STR_LEN+1];

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_ipv4_addr, 0, sizeof(src_ipv4_addr));
            memset(src_ipv4_addr_mask, 0xff, sizeof(src_ipv4_addr_mask));

            memset(src_mac_addr, 0, sizeof(src_mac_addr));
            memset(src_mac_addr_mask, 0xff, sizeof(src_mac_addr_mask));

            src_ipv4_addr[3] = ace_count;
            src_mac_addr[5] = ace_count;

            assert(ace_count <= 255);

            snprintf(src_ipv6_addr, sizeof(src_ipv6_addr), "fe%02x::", (UI8_T)ace_count);
            src_ipv6_addr[ sizeof(src_ipv6_addr)-1 ] = '\0';

            switch (ace_entry.ace_type)
            {
                case RULE_TYPE_MAC_ACL:
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac_addr, src_mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, src_ipv4_addr, src_ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, src_ipv6_addr);
#endif
                    break;

                default:
                    assert(0);
                    break;
            }

            access = (0 == ace_count % 2) ? RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;
            ace_entry.access = access;

            result = RULE_OM_AddAce2Acl(dst_acl_list[i].acl_index, ace_entry.ace_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (i = 0; i < _countof(dst_acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(dst_acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(dst_acl_list[i].ace_index_list));

            dst_acl_list[i].ace_index_list[ ace_count ] = ace_index;
        }

        assert(ace_count == _countof(dst_acl_list[i].ace_index_list));
    }

    for (i = 0; i < _countof(dst_acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T src_acl_index = src_acl_list[i].acl_index;
        UI32_T dst_acl_index = dst_acl_list[i].acl_index;
        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T src_ace_entry;
        RULE_TYPE_Ace_Entry_T dst_ace_entry;

        result = RULE_OM_LocalCopyAcl(src_acl_index, dst_acl_index);

        if (RULE_TYPE_OK == result)
        {
            for (ace_index = 0, ace_count = 0;
                 RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(dst_acl_list[i].acl_index, &ace_type, &ace_index, &dst_ace_entry);
                 ++ace_count)
            {
                assert(ace_count < _countof(src_acl_list[i].ace_index_list));

                result = RULE_OM_GetAceByIndex(src_acl_list[i].ace_index_list[ace_count], &src_ace_entry);
                assert(RULE_TYPE_OK == result);

                assert(src_ace_entry.ace_type == dst_ace_entry.ace_type);

                switch (dst_ace_entry.ace_type)
                {
                    case RULE_TYPE_MAC_ACL:
                        assert(0 == memcmp(&src_ace_entry.u.mac, &dst_ace_entry.u.mac, sizeof(src_ace_entry.u.mac)));
                        break;

                    case RULE_TYPE_IP_STD_ACL:
                    case RULE_TYPE_IP_EXT_ACL:
                        assert(0 == memcmp(&src_ace_entry.u.ip, &dst_ace_entry.u.ip, sizeof(src_ace_entry.u.ip)));
                        break;

                    case RULE_TYPE_IPV6_STD_ACL:
                    case RULE_TYPE_IPV6_EXT_ACL:
                        assert(0 == memcmp(&src_ace_entry.u.ipv6, &dst_ace_entry.u.ipv6, sizeof(src_ace_entry.u.ipv6)));
                        break;

                    default:
                        assert(0);
                        break;
                }
            }

            assert(ace_count == _countof(src_acl_list[i].ace_index_list));
        }
        else
        {
            for (ace_index = 0, ace_count = 0;
                 RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(dst_acl_list[i].acl_index, &ace_type, &ace_index, &dst_ace_entry);
                 ++ace_count)
            {
            }

            assert(ace_count == 0);
        }


        RULE_OM_UT_Proc_Validate_Rule();
    }

    return 0;
}

int
RULE_OM_UT_Del_All_Ace_From_Acl()
{
    enum
    {
        MAX_ACL  = 5,
        MAX_ACE  = 10,
    };

    typedef struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
        UI32_T ace_index_list[MAX_ACE];
    } acl_entry_t;

    RULE_TYPE_AclType_T acl_type_ar[] =
    {
        RULE_TYPE_MAC_ACL,
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IP_EXT_ACL,
        RULE_TYPE_IPV6_STD_ACL,
        RULE_TYPE_IPV6_EXT_ACL,
    };

    UI32_T i;

    RULE_TYPE_RETURN_TYPE_T result;

    acl_entry_t acl_list[MAX_ACL];

    memset(acl_list, 0, sizeof(acl_list));

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_ar[ i%_countof(acl_type_ar) ];
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_list[i].acl_index);
        assert(RULE_TYPE_OK == result);

        acl_list[i].acl_type = acl_type;

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            UI32_T access;

            UI8_T  src_ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  src_ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  src_mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  src_mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   src_ipv6_addr[L_INET_MAX_IPADDR_STR_LEN+1];

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            memset(src_ipv4_addr, 0, sizeof(src_ipv4_addr));
            memset(src_ipv4_addr_mask, 0xff, sizeof(src_ipv4_addr_mask));

            memset(src_mac_addr, 0, sizeof(src_mac_addr));
            memset(src_mac_addr_mask, 0xff, sizeof(src_mac_addr_mask));

            src_ipv4_addr[3] = ace_count;
            src_mac_addr[5] = ace_count;

            assert(ace_count <= 255);

            snprintf(src_ipv6_addr, sizeof(src_ipv6_addr), "fe%02x::", (UI8_T)ace_count);
            src_ipv6_addr[ sizeof(src_ipv6_addr)-1 ] = '\0';

            switch (ace_entry.ace_type)
            {
                case RULE_TYPE_MAC_ACL:
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac_addr, src_mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, src_ipv4_addr, src_ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, src_ipv6_addr);
#endif
                    break;

                default:
                    assert(0);
                    break;
            }

            access = (0 == ace_count % 2) ? RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;
            ace_entry.access = access;

            result = RULE_OM_AddAce2Acl(acl_list[i].acl_index, ace_entry.ace_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_AclType_T ace_type;

        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
            assert(ace_count < _countof(acl_list[i].ace_index_list));

            acl_list[i].ace_index_list[ ace_count ] = ace_index;
        }

        assert(ace_count == _countof(acl_list[i].ace_index_list));
    }

    for (i = 0; i < _countof(acl_list); ++i)
    {
        RULE_TYPE_Acl_T *acl_p;

        RULE_TYPE_AclType_T ace_type;

        UI32_T acl_index = acl_list[i].acl_index;
        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_Ace_Entry_T ace_entry;

        acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
        assert(NULL != acl_p);

        result = RULE_OM_LocalDelAllAceFromAcl(/*acl_index*/ acl_p);
        assert(RULE_TYPE_OK == result);

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
        }

        assert(0 == ace_count);

        for (ace_count = 0; ace_count < _countof(acl_list[i].ace_index_list); ++ace_count)
        {
            ace_index = acl_list[i].ace_index_list[ace_count];

            result = RULE_OM_GetAceByIndex(ace_index, &ace_entry);
            assert(RULE_TYPE_OK != result);
        }

        RULE_OM_UT_Proc_Validate_Rule();
    }

    return 0;
}

int
RULE_OM_UT_IsSameAceWithoutAccessField()
{
    RULE_TYPE_Ace_Entry_T ace_entry_1;
    RULE_TYPE_Ace_Entry_T ace_entry_2;
    BOOL_T b_result;

    UI32_T i;

    struct experimental_group {
        RULE_TYPE_AclType_T ace_type_1;
        RULE_TYPE_AclType_T ace_type_2;
        BOOL_T b_result;
    } exp_grp[] =
    {
        {RULE_TYPE_MAC_ACL, RULE_TYPE_MAC_ACL,              TRUE},

        {RULE_TYPE_IP_ACL, RULE_TYPE_IP_ACL,                TRUE},
        {RULE_TYPE_IP_STD_ACL, RULE_TYPE_IP_STD_ACL,        TRUE},
        {RULE_TYPE_IP_EXT_ACL, RULE_TYPE_IP_EXT_ACL,        TRUE},

        {RULE_TYPE_IPV6_ACL, RULE_TYPE_IPV6_ACL,            TRUE},
        {RULE_TYPE_IPV6_STD_ACL, RULE_TYPE_IPV6_STD_ACL,    TRUE},
        {RULE_TYPE_IPV6_EXT_ACL, RULE_TYPE_IPV6_EXT_ACL,    TRUE},

        {RULE_TYPE_IP_STD_ACL, RULE_TYPE_IP_EXT_ACL,        FALSE},
        {RULE_TYPE_IP_ACL, RULE_TYPE_IP_STD_ACL,            FALSE},
        {RULE_TYPE_IP_ACL, RULE_TYPE_IP_EXT_ACL,            FALSE},

        {RULE_TYPE_IPV6_STD_ACL, RULE_TYPE_IPV6_EXT_ACL,    FALSE},
        {RULE_TYPE_IPV6_ACL, RULE_TYPE_IPV6_EXT_ACL,        FALSE},
        {RULE_TYPE_IPV6_EXT_ACL, RULE_TYPE_IPV6_STD_ACL,    FALSE},

        {RULE_TYPE_MAC_ACL, RULE_TYPE_IP_ACL,               FALSE},
        {RULE_TYPE_MAC_ACL, RULE_TYPE_IP_STD_ACL,           FALSE},
        {RULE_TYPE_MAC_ACL, RULE_TYPE_IP_EXT_ACL,           FALSE},
        {RULE_TYPE_MAC_ACL, RULE_TYPE_IPV6_ACL,             FALSE},
        {RULE_TYPE_MAC_ACL, RULE_TYPE_IPV6_STD_ACL,         FALSE},
        {RULE_TYPE_MAC_ACL, RULE_TYPE_IPV6_EXT_ACL,         FALSE},

        {RULE_TYPE_IP_ACL, RULE_TYPE_IPV6_ACL,              FALSE},
        {RULE_TYPE_IP_ACL, RULE_TYPE_IPV6_STD_ACL,          FALSE},
        {RULE_TYPE_IP_ACL, RULE_TYPE_IPV6_EXT_ACL,          FALSE},

        {RULE_TYPE_IP_STD_ACL, RULE_TYPE_IPV6_ACL,          FALSE},
        {RULE_TYPE_IP_STD_ACL, RULE_TYPE_IPV6_STD_ACL,      FALSE},
        {RULE_TYPE_IP_STD_ACL, RULE_TYPE_IPV6_EXT_ACL,      FALSE},

        {RULE_TYPE_IP_EXT_ACL, RULE_TYPE_IPV6_ACL,          FALSE},
        {RULE_TYPE_IP_EXT_ACL, RULE_TYPE_IPV6_STD_ACL,      FALSE},
        {RULE_TYPE_IP_EXT_ACL, RULE_TYPE_IPV6_EXT_ACL,      FALSE},
    };

    for (i=0; i < _countof(exp_grp); ++i)
    {
        struct experimental_group *eg_p = &exp_grp[i];

        ace_entry_1.ace_type = eg_p->ace_type_1;
        RULE_OM_InitAce(&ace_entry_1);

        ace_entry_2.ace_type = eg_p->ace_type_2;
        RULE_OM_InitAce(&ace_entry_2);

        b_result = RULE_OM_LocalIsSameAceWithoutAccessField(&ace_entry_1, &ace_entry_2);
        assert(b_result == eg_p->b_result);
    }

    for (i=0; i < _countof(exp_grp); ++i)
    {
        struct experimental_group *eg_p = &exp_grp[i];

        if (eg_p->b_result == FALSE) {
            continue;
        }

        ace_entry_1.ace_type = eg_p->ace_type_1;
        RULE_OM_InitAce(&ace_entry_1);

        ace_entry_2.ace_type = eg_p->ace_type_2;
        RULE_OM_InitAce(&ace_entry_2);

        ace_entry_1.access = RULE_TYPE_ACE_DENY;
        ace_entry_2.access = RULE_TYPE_ACE_PERMIT;

        b_result = RULE_OM_LocalIsSameAceWithoutAccessField(&ace_entry_1, &ace_entry_2);
        assert(b_result == eg_p->b_result);
    }

    for (i=0; i < _countof(exp_grp); ++i)
    {
        struct experimental_group *eg_p = &exp_grp[i];

        if (eg_p->b_result == FALSE) {
            continue;
        }

        ace_entry_1.ace_type = eg_p->ace_type_1;
        RULE_OM_InitAce(&ace_entry_1);

        ace_entry_2.ace_type = eg_p->ace_type_2;
        RULE_OM_InitAce(&ace_entry_2);

        if (ace_entry_1.ace_type == RULE_TYPE_MAC_ACL)
        {
            RULE_OM_UT_Proc_SetAceCos(&ace_entry_1, 2);
            RULE_OM_UT_Proc_SetAceCos(&ace_entry_1, 3);
        }
        else if (ace_entry_1.ace_type == RULE_TYPE_IP_ACL ||
                 ace_entry_1.ace_type == RULE_TYPE_IP_STD_ACL ||
                 ace_entry_1.ace_type == RULE_TYPE_IP_EXT_ACL)
        {
            RULE_OM_UT_Proc_SetAceL4SrcPort(&ace_entry_1, 100);
            RULE_OM_UT_Proc_SetAceL4SrcPort(&ace_entry_2, 200);
        }
        else
        {
            RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry_1, "2013:3:22::1");
            RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry_2, "2013:3:22::2");
        }

        b_result = RULE_OM_LocalIsSameAceWithoutAccessField(&ace_entry_1, &ace_entry_2);
        assert(FALSE == b_result);
    }

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    //
    // specific case for ipv6
    //
    {
        ace_entry_1.ace_type = RULE_TYPE_IPV6_STD_ACL;
        RULE_OM_InitAce(&ace_entry_1);

        ace_entry_2.ace_type = RULE_TYPE_IPV6_STD_ACL;
        RULE_OM_InitAce(&ace_entry_2);

        if (ace_entry_1.ace_type == RULE_TYPE_IPV6_ACL ||
            ace_entry_1.ace_type == RULE_TYPE_IPV6_STD_ACL ||
            ace_entry_1.ace_type == RULE_TYPE_IPV6_EXT_ACL)
        {
            RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry_1, "2000::1/64");
            RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry_2, "2000::1/64");
        }

        ace_entry_1.access = RULE_TYPE_ACE_DENY;
        ace_entry_2.access = RULE_TYPE_ACE_PERMIT;

        b_result = RULE_OM_LocalIsSameAceWithoutAccessField(&ace_entry_1, &ace_entry_2);
        assert(TRUE == b_result);
    }

    {
        ace_entry_1.ace_type = RULE_TYPE_IPV6_STD_ACL;
        RULE_OM_InitAce(&ace_entry_1);

        ace_entry_2.ace_type = RULE_TYPE_IPV6_STD_ACL;
        RULE_OM_InitAce(&ace_entry_2);

        if (ace_entry_1.ace_type == RULE_TYPE_IPV6_ACL ||
            ace_entry_1.ace_type == RULE_TYPE_IPV6_STD_ACL ||
            ace_entry_1.ace_type == RULE_TYPE_IPV6_EXT_ACL)
        {
            RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry_1, "2000::1/64");
            RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry_2, "2000::1/64");
        }

        ace_entry_1.access = RULE_TYPE_ACE_DENY;
        ace_entry_2.access = RULE_TYPE_ACE_PERMIT;

        b_result = RULE_OM_LocalIsSameAceWithoutAccessField(&ace_entry_1, &ace_entry_2);
        assert(TRUE == b_result);
    }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

    return 0;
}

int
RULE_OM_UT_IsMatchAnyAnyAce()
{
    RULE_TYPE_Ace_Entry_T ace_entry;
    UI32_T ui32_result;
    BOOL_T b_result;

    struct experimental_group
    {
        RULE_TYPE_AclType_T ace_type;
    }exp_grp[] =
    {
        {RULE_TYPE_MAC_ACL},
        {RULE_TYPE_IP_STD_ACL},
        {RULE_TYPE_IP_EXT_ACL},

        {RULE_TYPE_IPV6_STD_ACL},
        {RULE_TYPE_IPV6_EXT_ACL},
    };

    UI32_T i;

    for (i=0; i < _countof(exp_grp); i++)
    {
        struct experimental_group *eg_p = &exp_grp[i];

        char *acl_name = "a1";

        UI32_T acl_index;
        UI32_T ace_index;

        RULE_TYPE_Acl_T acl;
        RULE_TYPE_Acl_T *acl_p;
        RULE_TYPE_Ace_Entry_T *ace_p;

        ui32_result = RULE_OM_CreateAcl(acl_name, eg_p->ace_type);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == ui32_result);

        ace_entry.ace_type = eg_p->ace_type;
        RULE_OM_InitAce(&ace_entry);

        ui32_result = RULE_OM_AddAce2Acl(acl_index, eg_p->ace_type, &ace_entry);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_GetAclByIndex(acl_index, &acl);
        assert(RULE_TYPE_OK == ui32_result);

        assert(1 == acl.ace_count);

        acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
        assert(NULL != acl_p);

        ace_index = 0;
        ui32_result = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == ui32_result);

        b_result = RULE_OM_IsMatchAnyAnyAce(ace_index);
//        b_result = RULE_OM_IsMatchAnyAnyAce(acl.ace_index_list[0]);
        assert(TRUE == b_result);

        ui32_result = RULE_OM_DelAcl(acl_index);
        assert(RULE_TYPE_OK == ui32_result);
    }

    for (i=0; i < _countof(exp_grp); i++)
    {
        struct experimental_group *eg_p = &exp_grp[i];

        char *acl_name = "a1";

        UI32_T acl_index;
        UI32_T ace_index;

        RULE_TYPE_Acl_T acl;
        RULE_TYPE_Acl_T *acl_p;
        RULE_TYPE_Ace_Entry_T *ace_p;

        ui32_result = RULE_OM_CreateAcl(acl_name, eg_p->ace_type);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == ui32_result);

        ace_entry.ace_type = eg_p->ace_type;
        RULE_OM_InitAce(&ace_entry);

        ace_entry.access = RULE_TYPE_ACE_DENY;

        ui32_result = RULE_OM_AddAce2Acl(acl_index, eg_p->ace_type, &ace_entry);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_GetAclByIndex(acl_index, &acl);
        assert(RULE_TYPE_OK == ui32_result);

        assert(1 == acl.ace_count);

        acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
        assert(NULL != acl_p);

        ace_index = 0;
        ui32_result = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == ui32_result);

        b_result = RULE_OM_IsMatchAnyAnyAce(ace_index);
//        b_result = RULE_OM_IsMatchAnyAnyAce(acl.ace_index_list[0]);
        assert(TRUE == b_result);

        ui32_result = RULE_OM_DelAcl(acl_index);
        assert(RULE_TYPE_OK == ui32_result);
    }

    for (i=0; i < _countof(exp_grp); i++)
    {
        struct experimental_group *eg_p = &exp_grp[i];

        char *acl_name = "a1";

        UI32_T acl_index;
        UI32_T ace_index;

        RULE_TYPE_Acl_T acl;
        RULE_TYPE_Acl_T *acl_p;
        RULE_TYPE_Ace_Entry_T *ace_p;

        ui32_result = RULE_OM_CreateAcl(acl_name, eg_p->ace_type);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == ui32_result);

        ace_entry.ace_type = eg_p->ace_type;
        RULE_OM_InitAce(&ace_entry);

        ace_entry.access = RULE_TYPE_ACE_PERMIT;

        ui32_result = RULE_OM_AddAce2Acl(acl_index, eg_p->ace_type, &ace_entry);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_GetAclByIndex(acl_index, &acl);
        assert(RULE_TYPE_OK == ui32_result);

        assert(1 == acl.ace_count);

        acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
        assert(NULL != acl_p);

        ace_index = 0;
        ui32_result = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == ui32_result);

//        b_result = RULE_OM_IsMatchAnyAnyAce(acl.ace_index_list[0]);
        b_result = RULE_OM_IsMatchAnyAnyAce(ace_index);
        assert(TRUE == b_result);

        ui32_result = RULE_OM_DelAcl(acl_index);
        assert(RULE_TYPE_OK == ui32_result);
    }


    return 0;
}

int
RULE_OM_UT_GetNextAceByAcl()
{
    enum
    {
        ACL_TYPE = RULE_TYPE_MAC_ACL,
        ACE_TYPE = RULE_TYPE_MAC_ACL,
    };

    const char              *acl_name = "a1";
    RULE_TYPE_AclType_T     acl_type = ACL_TYPE;
    RULE_TYPE_AclType_T     ace_type = ACE_TYPE;

    RULE_TYPE_Ace_Entry_T   ace_entry;

    UI32_T                  ace_count;
    UI32_T                  acl_index;
    UI32_T                  ace_index;

    UI32_T                  result;

    result = RULE_OM_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);


    //
    // validate by invalid ACL index
    //
    ace_index = 0;
    result = RULE_OM_GetNextAceByAcl(99, &ace_type, &ace_index, &ace_entry);
    assert(result != RULE_TYPE_OK);


    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    //
    // empty ACL
    //
    ace_index = 0;
    result = RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
    assert(result != RULE_TYPE_OK);


    ace_count = 0;

    while (TRUE)
    {
        UI8_T src_mac[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T src_mask[SYS_ADPT_MAC_ADDR_LEN];

        int i, j;

        ace_entry.ace_type = ace_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(result == RULE_TYPE_OK);

        memset(src_mac, 0, sizeof(src_mac));
        memset(src_mask, 0xff, sizeof(src_mask));

        assert(SYS_ADPT_MAX_NBRS_OF_ACE < (255*255));

        i = ace_count / 255;
        j = ace_count % 255;

        src_mac[ 4 ] = i;
        src_mac[ 5 ] = j;

        RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac, src_mask);

        result = RULE_OM_AddAce2Acl(acl_index, ace_type, &ace_entry);
        if (result != RULE_TYPE_OK)
        {
            break;
        }

        ace_count ++;
    }

    assert(ace_count == SYS_ADPT_MAX_NBRS_OF_ACE);

    ace_index = 0;
    ace_type = 99;
    while (RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry))
    {
        assert(ace_type == ACE_TYPE);
        assert(ace_entry.u.mac.aceSourceMacAddrBitmask[0] == 0xff);
        assert(ace_entry.u.mac.aceSourceMacAddrBitmask[5] == 0xff);

        ace_count --;

        memset(&ace_entry, 0, sizeof(ace_entry));
    }

    assert(ace_count == 0);

    return 0;
}

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
static int
RULE_OM_TEST_Compress_ACE_Basic()
{
    RULE_TYPE_RETURN_TYPE_T result;

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

    result = RULE_OM_LocalCompressAce(&ace, &agg_ace);
    assert(RULE_TYPE_OK == result);

    assert(0xFE == agg_ace.u.mac.aceDestMacAddrBitmask[5]);

    SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x01, 0x01, 0x02);
    SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

    SET_MAC(agg_ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x01, 0x01, 0x03);
    SET_MAC(agg_ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

    result = RULE_OM_LocalCompressAce(&ace, &agg_ace);
    assert(RULE_TYPE_OK == result);

    assert(0xFE == agg_ace.u.mac.aceDestMacAddrBitmask[5]);

    return UT_OK;
}

static int
RULE_OM_TEST_Compress_ACL_1()
{
#define ETH_ACE(ace_entry, eth)                                                 \
    {                                                                           \
        ace_entry.ace_type = RULE_TYPE_MAC_ACL;                                 \
        RULE_OM_InitAce(&ace_entry);                                            \
        ace_entry.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;  \
        ace_entry.u.mac.aceMinEtherType = eth;                                  \
        ace_entry.u.mac.aceEtherTypeBitmask = 0xffff;                           \
    }

    UI32_T i;
    const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    UI32_T acl_index;
    RULE_TYPE_Ace_Entry_T ace;
    UI32_T res;


    res = RULE_OM_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == res);

    res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == res);

    // group 1
    ETH_ACE(ace, 0);
    res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
    assert(res == RULE_TYPE_OK);

    ETH_ACE(ace, 1);
    res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
    assert(res == RULE_TYPE_OK);

    // group 2
    ETH_ACE(ace, 4);
    res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
    assert(res == RULE_TYPE_OK);

    RULE_OM_LocalCompressAcl(acl_index);

    {
        RULE_TYPE_Acl_T *acl_p;
        RULE_TYPE_Ace_Entry_T *ace_p;
        UI32_T ace_index;
        UI32_T ace_count = 0;

        for (i=0; i<RULE_TYPE_MAX_NBRS_OF_ACE_OF_SYSTEM; ++i)
        {
            ace_p = RULE_OM_LocalGetAceByIndex(i);
            if (ace_p)
            {
                ++ace_count;
            }
        }

        assert(ace_count == 2);

        acl_p = RULE_OM_LocalGetAclByIndex(acl_index);

        assert(acl_p->ace_count == ace_count);

        ace_index = 0;
        res = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == res);

        assert(ace_p->u.mac.aceMinEtherType == 0 &&
               ace_p->u.mac.aceEtherTypeBitmask == 0x0000fffe);

        res = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == res);

        assert(ace_p->u.mac.aceMinEtherType == 4 &&
               ace_p->u.mac.aceEtherTypeBitmask == 0x0000ffff);

    }

    return UT_OK;
#undef ETH_ACE
}

static int
RULE_OM_TEST_Compress_ACL_2()
{
#define ETH_ACE(ace_entry, eth)                                                 \
    {                                                                           \
        ace_entry.ace_type = RULE_TYPE_MAC_ACL;                                 \
        RULE_OM_InitAce(&ace_entry);                                            \
        ace_entry.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;  \
        ace_entry.u.mac.aceMinEtherType = eth;                                  \
        ace_entry.u.mac.aceEtherTypeBitmask = 0xffff;                           \
    }

    UI32_T i;
    const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    UI32_T acl_index;
    RULE_TYPE_Ace_Entry_T ace;
    UI32_T res;


    res = RULE_OM_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == res);

    res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == res);

    // group 1
    ETH_ACE(ace, 0);
    res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
    assert(res == RULE_TYPE_OK);

    ETH_ACE(ace, 1);
    res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
    assert(res == RULE_TYPE_OK);

    // group 2
    ETH_ACE(ace, 4);
    res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
    assert(res == RULE_TYPE_OK);

    ETH_ACE(ace, 6);
    res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
    assert(res == RULE_TYPE_OK);

    RULE_OM_LocalCompressAcl(acl_index);

    {
        RULE_TYPE_Acl_T *acl_p;
        RULE_TYPE_Ace_Entry_T *ace_p;

        UI32_T ace_index;
        UI32_T ace_count = 0;

        for (i=0; i<RULE_TYPE_MAX_NBRS_OF_ACE_OF_SYSTEM; ++i)
        {
            ace_p = RULE_OM_LocalGetAceByIndex(i);
            if (ace_p)
            {
                ++ace_count;
            }
        }

        assert(ace_count == 2);

        acl_p = RULE_OM_LocalGetAclByIndex(acl_index);

        assert(acl_p->ace_count == ace_count);

        ace_index = 0;
        res = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == res);

        assert(ace_p->u.mac.aceMinEtherType == 0 &&
               ace_p->u.mac.aceEtherTypeBitmask == 0x0000fffe);

        res = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == res);

        assert(ace_p->u.mac.aceMinEtherType == 4 &&
               ace_p->u.mac.aceEtherTypeBitmask == 0x0000fffd);

    }

    return UT_OK;
#undef ETH_ACE
}

static int
RULE_OM_TEST_Compress_ACL_Best_Case_Inc()
{
#define ETH_ACE(ace_entry, eth)                                                 \
    {                                                                           \
        ace_entry.ace_type = RULE_TYPE_MAC_ACL;                                 \
        RULE_OM_InitAce(&ace_entry);                                            \
        ace_entry.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;  \
        ace_entry.u.mac.aceMinEtherType = eth;                                  \
        ace_entry.u.mac.aceEtherTypeBitmask = 0xffff;                           \
    }

    UI32_T i;
    const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    UI32_T acl_index;
    RULE_TYPE_Ace_Entry_T ace;
    UI32_T res;


    res = RULE_OM_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == res);

    res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == res);

    for (i=0; i<16; ++i)
    {
        ETH_ACE(ace, i);
        res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
        assert(res == RULE_TYPE_OK);
    }

    // EthType => 0 1 2 3 4 ...

    RULE_OM_LocalCompressAcl(acl_index);

    {
        RULE_TYPE_Acl_T *acl_p;
        RULE_TYPE_Ace_Entry_T *ace_p;

        UI32_T ace_index;
        UI32_T ace_count = 0;

        for (i=0; i<RULE_TYPE_MAX_NBRS_OF_ACE_OF_SYSTEM; ++i)
        {
            ace_p = RULE_OM_LocalGetAceByIndex(i);
            if (ace_p)
            {
                ++ace_count;
            }
        }

        assert(ace_count == 1);

        acl_p = RULE_OM_LocalGetAclByIndex(acl_index);

        assert(acl_p->ace_count == ace_count);

        ace_index = 0;
        res = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == res);

        assert(ace_p->u.mac.aceMinEtherType == 0 &&
               ace_p->u.mac.aceEtherTypeBitmask == 0x0000fff0);
    }

    return UT_OK;
#undef ETH_ACE
}

static int
RULE_OM_TEST_Compress_ACL_Best_Case_Dec()
{
#define ETH_ACE(ace_entry, eth)                                                 \
    {                                                                           \
        ace_entry.ace_type = RULE_TYPE_MAC_ACL;                                 \
        RULE_OM_InitAce(&ace_entry);                                            \
        ace_entry.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;  \
        ace_entry.u.mac.aceMinEtherType = eth;                                  \
        ace_entry.u.mac.aceEtherTypeBitmask = 0xffff;                           \
    }

    UI32_T i;
    const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    UI32_T acl_index;
    RULE_TYPE_Ace_Entry_T ace;
    UI32_T res;

    res = RULE_OM_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == res);

    res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == res);

    for (i=15; 0 <= (int)i; --i)
    {
        ETH_ACE(ace, i);
        res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
        assert(res == RULE_TYPE_OK);
    }

    // EthType => 15 14 13 12 ...

    RULE_OM_LocalCompressAcl(acl_index);

    {
        RULE_TYPE_Acl_T *acl_p;
        RULE_TYPE_Ace_Entry_T *ace_p;

        UI32_T ace_index;
        UI32_T ace_count = 0;

        for (i=0; i<RULE_TYPE_MAX_NBRS_OF_ACE_OF_SYSTEM; ++i)
        {
            ace_p = RULE_OM_LocalGetAceByIndex(i);
            if (ace_p)
            {
                ++ace_count;
            }
        }

        assert(ace_count == 1);

        acl_p = RULE_OM_LocalGetAclByIndex(acl_index);

        assert(acl_p->ace_count == ace_count);

        ace_index = 0;
        res = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == res);

        assert(ace_p->u.mac.aceMinEtherType == 0 &&
               ace_p->u.mac.aceEtherTypeBitmask == 0x0000fff0);
    }

    return UT_OK;
#undef ETH_ACE
}

static int
RULE_OM_TEST_Compress_ACL_Best_Case_Interlace()
{
#define ETH_ACE(ace_entry, eth)                                                 \
    {                                                                           \
        ace_entry.ace_type = RULE_TYPE_MAC_ACL;                                 \
        RULE_OM_InitAce(&ace_entry);                                            \
        ace_entry.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;  \
        ace_entry.u.mac.aceMinEtherType = eth;                                  \
        ace_entry.u.mac.aceEtherTypeBitmask = 0xffff;                           \
    }

    UI32_T i;
    const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    UI32_T acl_index;
    RULE_TYPE_Ace_Entry_T ace;
    UI32_T res;

    res = RULE_OM_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == res);

    res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == res);

    for (i=0; i < 16; ++i)
    {
        if (i%2) continue;

        ETH_ACE(ace, i);
        res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
        assert(res == RULE_TYPE_OK);
    }

    for (i=0; i < 16; ++i)
    {
        if (i%2==0) continue;

        ETH_ACE(ace, i);
        res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
        assert(res == RULE_TYPE_OK);
    }

    // EthType => 0 2 4 6 8 10 ... 1 3 5 7 9 ...

    RULE_OM_LocalCompressAcl(acl_index);

    {
        RULE_TYPE_Acl_T *acl_p;
        RULE_TYPE_Ace_Entry_T *ace_p;

        UI32_T ace_index;
        UI32_T ace_count = 0;

        for (i=0; i<RULE_TYPE_MAX_NBRS_OF_ACE_OF_SYSTEM; ++i)
        {
            ace_p = RULE_OM_LocalGetAceByIndex(i);
            if (ace_p)
            {
                ++ace_count;
            }
        }

        assert(ace_count == 1);

        acl_p = RULE_OM_LocalGetAclByIndex(acl_index);

        assert(acl_p->ace_count == ace_count);

        ace_index = 0;
        res = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == res);

        assert(ace_p->u.mac.aceMinEtherType == 0 &&
               ace_p->u.mac.aceEtherTypeBitmask == 0x0000fff0);
    }

    return UT_OK;
#undef ETH_ACE
}
#endif // #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)

static int
RULE_OM_TEST_Max_ACE_Of_ACL()
{
    UI32_T i;
    UI32_T res;
    RULE_TYPE_Ace_Entry_T ace;

    // experimental group
    struct
    {
        const char *acl_name;
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
    } eg[] =
    {
        {"a1", RULE_TYPE_MAC_ACL,       0},
        {"a2", RULE_TYPE_IP_STD_ACL,    0},
        {"a3", RULE_TYPE_IP_EXT_ACL,    0},
        {"a4", RULE_TYPE_IPV6_STD_ACL,  0},
        {"a5", RULE_TYPE_IPV6_EXT_ACL,  0}
    };

    for (i=0; i < _countof(eg); ++i)
    {
        res = RULE_OM_CreateAcl(eg[i].acl_name, eg[i].acl_type);
        assert(RULE_TYPE_OK == res);

        res = RULE_OM_GetAclIdByName(eg[i].acl_name, &eg[i].acl_index);
        assert(RULE_TYPE_OK == res);
    }

    for (i=0; i < _countof(eg); ++i)
    {
        UI32_T j;

        // MUST success when j < MaxNumberofACEPerACL, otherwise will fail
        for (j = 0; j < SYS_ADPT_MAX_NBRS_OF_ACE + 1; ++j)
        {
            ace.ace_type = eg[i].acl_type;
            RULE_OM_InitAce(&ace);

            switch (eg[i].acl_type)
            {
                case RULE_TYPE_MAC_ACL:
                    ace.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;
                    ace.u.mac.aceMinEtherType = j;
                    ace.u.mac.aceEtherTypeBitmask = 0xffff;
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    ace.u.ip.aceSourceIpAddr = j;
                    ace.u.ip.aceSourceIpAddrBitmask = 0xffffffff;
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    ace.u.ipv6.aceSourceIpAddr[0] = (UI8_T)j;
                    ace.u.ipv6.aceSourceIpAddr[1] = (UI8_T)(j / 255);
                    //ace.u.ipv6.aceSourceIpAddrPrefixLen = RULE_TYPE_MAX_OF_SRC_IPV6_PREFIX_LEN;
                    RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_SRC_IPV6_PREFIX_LEN,
                                                        ace.u.ipv6.aceSourceIpAddrBitmask);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    ace.u.ipv6.aceSourceIpAddr[0] = (UI8_T)j;
                    ace.u.ipv6.aceSourceIpAddr[1] = (UI8_T)(j / 255);
                    RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_SRC_IPV6_PREFIX_LEN,
                                                        ace.u.ipv6.aceSourceIpAddrBitmask);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    ace.u.ipv6.aceDestIpAddr[0] = (UI8_T)j;
                    ace.u.ipv6.aceDestIpAddr[1] = (UI8_T)(j / 255);
                    //ace.u.ipv6.aceDestIpAddrPrefixLen = RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN;
                    RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN,
                                                        ace.u.ipv6.aceDestIpAddrBitmask);
#endif
                    break;
            }

            res = RULE_OM_AddAce2Acl(eg[i].acl_index, ace.ace_type, &ace);

            // MUST success
            if (j < SYS_ADPT_MAX_NBRS_OF_ACE)
            {
                assert(res == RULE_TYPE_OK);
            }
            else
            {
                assert(res != RULE_TYPE_OK);
            }
        }

        RULE_OM_DelAcl(eg[i].acl_index);
        RULE_OM_UT_Proc_Validate_Rule();
    }

    return UT_OK;
}

static int
RULE_OM_TEST_Max_ACE_Of_System()
{
    UI32_T acl_id = 0;
    UI32_T ace_count = 0;
    BOOL_T stop_and_check_result = FALSE;

    while (1)
    {
        UI32_T i;
        char acl_name[32];
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
        UI32_T acl_index;
        RULE_TYPE_Ace_Entry_T ace;
        UI32_T res;

        sprintf(acl_name, "a%lu", acl_id++);

        res = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == res);

        res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == res);

        for (i = 0; i < SYS_ADPT_MAX_NBRS_OF_ACE; ++i)
        {
            ace.ace_type = acl_type;
            RULE_OM_InitAce(&ace);

            ace.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;
            ace.u.mac.aceMinEtherType = i;
            ace.u.mac.aceEtherTypeBitmask = 0xffff;

            res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            if (!(res == RULE_TYPE_OK))
            {
                stop_and_check_result = TRUE;
                break;
            }

            ++ ace_count;
        }

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        res = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(res == RULE_TYPE_OK);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        if (TRUE == stop_and_check_result)
        {
            break;
        }

    }

    assert(ace_count == SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM);

    return UT_OK;
}

static int
RULE_OM_TEST_Create_ACL_By_Index()
{
    UI32_T res;
    UI32_T acl_index = 1;
    BOOL_T active = TRUE;

    res = RULE_OM_CreateAclByIndex(acl_index, active);
    assert(res == RULE_TYPE_OK);

    // class-map already exist
    res = RULE_OM_CreateAclByIndex(acl_index, active);
    assert(res != RULE_TYPE_OK);

    res = RULE_OM_SetAclNameByIndex(acl_index, "2");
    assert(res == RULE_TYPE_OK);

    acl_index = 2;

    // duplicate name
    res = RULE_OM_CreateAclByIndex(acl_index, active);
    assert(res != RULE_TYPE_OK);

    // semaphore relase check
    res = RULE_OM_CreateAclByIndex(acl_index, active);
    assert(res != RULE_TYPE_OK);

    return UT_OK;
}

static int
RULE_OM_TEST_Max_ACL_Of_System()
{
    UI32_T res;
    UI32_T i;

    for (i = 0; ; ++i)
    {
        char acl_name[32];
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
        UI32_T acl_index;
        RULE_TYPE_Ace_Entry_T ace_entry;

        sprintf(acl_name, "a%lu", i);

        res = RULE_OM_CreateAcl(acl_name, acl_type);
        if (!(res == RULE_TYPE_OK))
        {
            break;
        }

        res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(res == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;
        RULE_OM_InitAce(&ace_entry);

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(res == RULE_TYPE_OK);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        res = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(res == RULE_TYPE_OK);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE
    }

    assert(i == SYS_ADPT_MAX_NBRS_OF_ACL);

    return UT_OK;
}

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
static int
RULE_OM_TEST_MergerMacAce_IPv6_Standard()
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T total_count = 0;

    {
        UI32_T  i;
        const char          *acl_name   = "a1";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IPV6_STD_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i = 1; i<= 3*16; i++)
        {
            ace.ace_type = RULE_TYPE_IPV6_STD_ACL;
            RULE_OM_InitAce(&ace);

            memset(ace.u.ipv6.aceSourceIpAddr,0, 16);

            ace.u.ipv6.aceSourceIpAddr[0] = 0x3f;
            ace.u.ipv6.aceSourceIpAddr[1] = 0xfe;
            ace.u.ipv6.aceSourceIpAddr[2] = 0x00;
            ace.u.ipv6.aceSourceIpAddr[3] = 0x01;
            ace.u.ipv6.aceSourceIpAddr[14]= 0x12;
            ace.u.ipv6.aceSourceIpAddr[15]= i;

            //ace.u.ipv6.aceSourceIpAddrPrefixLen = 128;
            RULE_OM_LocalConvertIpv6Prefix2Mask(128,
                                                ace.u.ipv6.aceSourceIpAddrBitmask);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
            RULE_TYPE_Ace_Entry_T   *ace_p;

            UI32_T i;
            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("SIP6\r\n");
            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
            {
                //
                // FIXME: Add code to validate result here
                //

//                printf("[%02lu] %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n"
//                       "     %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
//                    i+1,
//                    ace_p->u.ipv6.aceSourceIpAddr[0],
//                    ace_p->u.ipv6.aceSourceIpAddr[1],
//                    ace_p->u.ipv6.aceSourceIpAddr[2],
//                    ace_p->u.ipv6.aceSourceIpAddr[3],
//                    ace_p->u.ipv6.aceSourceIpAddr[4],
//                    ace_p->u.ipv6.aceSourceIpAddr[5],
//                    ace_p->u.ipv6.aceSourceIpAddr[6],
//                    ace_p->u.ipv6.aceSourceIpAddr[7],
//                    ace_p->u.ipv6.aceSourceIpAddr[8],
//                    ace_p->u.ipv6.aceSourceIpAddr[9],
//                    ace_p->u.ipv6.aceSourceIpAddr[10],
//                    ace_p->u.ipv6.aceSourceIpAddr[11],
//                    ace_p->u.ipv6.aceSourceIpAddr[12],
//                    ace_p->u.ipv6.aceSourceIpAddr[13],
//                    ace_p->u.ipv6.aceSourceIpAddr[14],
//                    ace_p->u.ipv6.aceSourceIpAddr[15],
//
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[0],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[1],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[2],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[3],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[4],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[5],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[6],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[7],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[8],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[9],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[10],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[11],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[12],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[13],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[14],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[15]);
            }
        }
    } /* a1 */

    return UT_OK;
}

static int
RULE_OM_TEST_MergerMacAce_IPv6_Extended()
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T total_count = 0;

    {
        UI32_T  i;
        const char          *acl_name   = "sip";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IPV6_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=1; i<= 3*16; i++)
        {
            ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
            RULE_OM_InitAce(&ace);

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
            memset(ace.u.ipv6.aceSourceIpAddr,0, 16);

            ace.u.ipv6.aceSourceIpAddr[0] = 0x3f;
            ace.u.ipv6.aceSourceIpAddr[1] = 0xfe;
            ace.u.ipv6.aceSourceIpAddr[2] = 0x00;
            ace.u.ipv6.aceSourceIpAddr[3] = 0x01;
            ace.u.ipv6.aceSourceIpAddr[14]= 0x12;
            ace.u.ipv6.aceSourceIpAddr[15]= i;

            //ace.u.ipv6.aceSourceIpAddrPrefixLen = 128;
            RULE_OM_LocalConvertIpv6Prefix2Mask(128,
                                                ace.u.ipv6.aceSourceIpAddrBitmask);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            memset(ace.u.ipv6.aceDestIpAddr,0, 16);

            ace.u.ipv6.aceDestIpAddr[0] = 0x3f;
            ace.u.ipv6.aceDestIpAddr[1] = 0xfe;
            ace.u.ipv6.aceDestIpAddr[2] = 0x00;
            ace.u.ipv6.aceDestIpAddr[3] = 0x01;
            ace.u.ipv6.aceDestIpAddr[14]= 0x12;
            ace.u.ipv6.aceDestIpAddr[15]= i;

            //ace.u.ipv6.aceDestIpAddrPrefixLen = SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN;
            RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN,
                                                ace.u.ipv6.aceDestIpAddrBitmask);
#endif

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
            RULE_TYPE_Ace_Entry_T   *ace_p;

            UI32_T i;
            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("SIP6\r\n");
            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
            {
                //
                // FIXME: Add code to validate result here
                //

//                printf("[%02lu] %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n"
//                       "     %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
//                    i+1,
//                    ace_p->u.ipv6.aceSourceIpAddr[0],
//                    ace_p->u.ipv6.aceSourceIpAddr[1],
//                    ace_p->u.ipv6.aceSourceIpAddr[2],
//                    ace_p->u.ipv6.aceSourceIpAddr[3],
//                    ace_p->u.ipv6.aceSourceIpAddr[4],
//                    ace_p->u.ipv6.aceSourceIpAddr[5],
//                    ace_p->u.ipv6.aceSourceIpAddr[6],
//                    ace_p->u.ipv6.aceSourceIpAddr[7],
//                    ace_p->u.ipv6.aceSourceIpAddr[8],
//                    ace_p->u.ipv6.aceSourceIpAddr[9],
//                    ace_p->u.ipv6.aceSourceIpAddr[10],
//                    ace_p->u.ipv6.aceSourceIpAddr[11],
//                    ace_p->u.ipv6.aceSourceIpAddr[12],
//                    ace_p->u.ipv6.aceSourceIpAddr[13],
//                    ace_p->u.ipv6.aceSourceIpAddr[14],
//                    ace_p->u.ipv6.aceSourceIpAddr[15],
//
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[0],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[1],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[2],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[3],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[4],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[5],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[6],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[7],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[8],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[9],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[10],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[11],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[12],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[13],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[14],
//                    ace_p->u.ipv6.aceSourceIpAddrBitmask[15]);
            }
        }
    } /* sip */

    {
        UI32_T  i;
        const char          *acl_name   = "dip";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IPV6_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i = 1; i <= 3*16; i++)
        {
            ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
            RULE_OM_InitAce(&ace);

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
            memset(ace.u.ipv6.aceSourceIpAddr,0, 16);

            ace.u.ipv6.aceSourceIpAddr[0] = 0x3f;
            ace.u.ipv6.aceSourceIpAddr[1] = 0xfe;
            ace.u.ipv6.aceSourceIpAddr[2] = i / 255;
            ace.u.ipv6.aceSourceIpAddr[3] = i % 255;

            //ace.u.ipv6.aceSourceIpAddrPrefixLen = RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN;
            RULE_OM_LocalConvertIpv6Prefix2Mask(128,
                                                ace.u.ipv6.aceSourceIpAddrBitmask);
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            memset(ace.u.ipv6.aceDestIpAddr,0, 16);

            ace.u.ipv6.aceDestIpAddr[0] = 0x3f;
            ace.u.ipv6.aceDestIpAddr[1] = 0xfe;
            ace.u.ipv6.aceDestIpAddr[2] = i / 255;
            ace.u.ipv6.aceDestIpAddr[3] = i % 255;

            //ace.u.ipv6.aceDestIpAddrPrefixLen = RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN;
            RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN,
                                                ace.u.ipv6.aceDestIpAddrBitmask);
#endif

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
            RULE_TYPE_Ace_Entry_T   *ace_p;

            UI32_T i;
            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
//            printf("SIP6\r\n");
//#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
//            printf("DIP6\r\n");
//#endif

            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
            {
                //
                // FIXME: Add code to validate result here
                //

//                printf("[%02lu] %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n"
//                       "     %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
//                    i+1,
//#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
//
//                   ace_p->u.ipv6.aceSourceIpAddr[0],
//                   ace_p->u.ipv6.aceSourceIpAddr[1],
//                   ace_p->u.ipv6.aceSourceIpAddr[2],
//                   ace_p->u.ipv6.aceSourceIpAddr[3],
//                   ace_p->u.ipv6.aceSourceIpAddr[4],
//                   ace_p->u.ipv6.aceSourceIpAddr[5],
//                   ace_p->u.ipv6.aceSourceIpAddr[6],
//                   ace_p->u.ipv6.aceSourceIpAddr[7],
//                   ace_p->u.ipv6.aceSourceIpAddr[8],
//                   ace_p->u.ipv6.aceSourceIpAddr[9],
//                   ace_p->u.ipv6.aceSourceIpAddr[10],
//                   ace_p->u.ipv6.aceSourceIpAddr[11],
//                   ace_p->u.ipv6.aceSourceIpAddr[12],
//                   ace_p->u.ipv6.aceSourceIpAddr[13],
//                   ace_p->u.ipv6.aceSourceIpAddr[14],
//                   ace_p->u.ipv6.aceSourceIpAddr[15],
//
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[0],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[1],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[2],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[3],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[4],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[5],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[6],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[7],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[8],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[9],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[10],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[11],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[12],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[13],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[14],
//                   ace_p->u.ipv6.aceSourceIpAddrBitmask[15]
//
//#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
//
//                    ace_p->u.ipv6.aceDestIpAddr[0],
//                    ace_p->u.ipv6.aceDestIpAddr[1],
//                    ace_p->u.ipv6.aceDestIpAddr[2],
//                    ace_p->u.ipv6.aceDestIpAddr[3],
//                    ace_p->u.ipv6.aceDestIpAddr[4],
//                    ace_p->u.ipv6.aceDestIpAddr[5],
//                    ace_p->u.ipv6.aceDestIpAddr[6],
//                    ace_p->u.ipv6.aceDestIpAddr[7],
//                    ace_p->u.ipv6.aceDestIpAddr[8],
//                    ace_p->u.ipv6.aceDestIpAddr[9],
//                    ace_p->u.ipv6.aceDestIpAddr[10],
//                    ace_p->u.ipv6.aceDestIpAddr[11],
//                    ace_p->u.ipv6.aceDestIpAddr[12],
//                    ace_p->u.ipv6.aceDestIpAddr[13],
//                    ace_p->u.ipv6.aceDestIpAddr[14],
//                    ace_p->u.ipv6.aceDestIpAddr[15],
//
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[0],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[1],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[2],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[3],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[4],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[5],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[6],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[7],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[8],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[9],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[10],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[11],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[12],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[13],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[14],
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[15]
//#endif
//                    );
            }
        }
    } /* dip */

    /* DSCP have not compress. Check no lost ACE
     */
    {
        UI32_T  i;
        const char          *acl_name   = "dscp";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IPV6_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i = 1; i <= 3*16; i++)
        {
            ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
            RULE_OM_InitAce(&ace);

            ace.u.ipv6.aceDscp = i;

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
            RULE_TYPE_Ace_Entry_T   *ace_p;

            UI32_T i;
            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("DSCP\r\n");
            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
            {
                //
                // FIXME: Add code to validate result here
                //

//                printf("[%02lu] %08lx\r\n",
//                    i+1,
//                    ace_p->u.ipv6.aceDscp);
            }
        }
    } /* dscp */

    /* FlowLabel have not compress. Check no lost ACE
     */
    {
        UI32_T  i;
        const char          *acl_name   = "flowlabel";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IPV6_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i = 1; i <= 3*16; i++)
        {
            ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
            RULE_OM_InitAce(&ace);

            ace.u.ipv6.aceFlowLabel = i;

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
            RULE_TYPE_Ace_Entry_T   *ace_p;

            UI32_T i;
            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("FlowLabel\r\n");
            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
            {
                //
                // FIXME: Add code to validate result here
                //

//                printf("[%02lu] %08lx\r\n",
//                    i+1,
//                    ace_p->u.ipv6.aceFlowLabel);
            }
        }
    } /* flowlabel */

    return UT_OK;
}
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) */

int
RULE_OM_UT_Get_Port_Acl_By_Type()
{
    enum
    {
        MIN_IFINDEX = 1,
        MAX_IFINDEX = 5,

        MIN_DIRECTION = RULE_TYPE_INBOUND,
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
    };

    struct experimental_group
    {
        RULE_TYPE_AclType_T acl_type;
        RULE_TYPE_InOutDirection_T direction;
        RULE_TYPE_AclMemento_T bind_info;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_INBOUND,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_INBOUND,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_INBOUND,
            {0}
        },
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_OUTBOUND,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            {0}
        },
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    UI32_T pl_idx;
    UI32_T ifindex;

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);
        result = RULE_OM_CreateAcl(acl_name, acls[i].acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &acls[i].bind_info.acl_index);
        assert(RULE_TYPE_OK == result);

        acls[i].bind_info.time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        acls[i].bind_info.counter_enable = RULE_TYPE_COUNTER_DISABLE;
        RULE_OM_SET_ENTRY_ACTIVE(acls[i].bind_info.row_status);
    }

    for (i = 0; i < _countof(acls); ++ i)
    {
        for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
        {
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex, acls[i].direction, acls[i].acl_type);
            assert(NULL == bind_acl_p);
        }
    }

    //
    // SetAcl
    //
    for (i = 0; i < _countof(acls); ++ i)
    {
        for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
                continue;

            result = RULE_OM_SetAcl(ifindex,
                                    acls[i].direction,
                                    acls[i].bind_info.acl_index,
                                    acls[i].bind_info.time_range_index,
                                    acls[i].bind_info.time_range_name,
                                    acls[i].bind_info.counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    //
    // Verification
    //
    for (i = 0; i < _countof(acls); ++ i)
    {
        for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
        {
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex,acls[i].direction))
                continue;

            bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex,
                                                       acls[i].direction,
                                                       acls[i].acl_type);
            assert(NULL != bind_acl_p);
            
            assert(0 == memcmp(bind_acl_p, &acls[i].bind_info, sizeof(*bind_acl_p)));
        }
    }

    return 0;
}

int
RULE_OM_UT_Get_Port_Acl_By_Precedence()
{
    enum
    {
        MIN_IFINDEX = 1,
        MAX_IFINDEX = 5,

        MIN_DIRECTION = RULE_TYPE_INBOUND,
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
    };

    //
    // Sort by precedence
    //
    struct experimental_group
    {
        RULE_TYPE_AclType_T acl_type;
        RULE_TYPE_InOutDirection_T direction;
        UI32_T precedence;
        RULE_TYPE_AclMemento_T bind_info;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    UI32_T pl_idx;
    UI32_T ifindex;

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);
        result = RULE_OM_CreateAcl(acl_name, acls[i].acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &acls[i].bind_info.acl_index);
        assert(RULE_TYPE_OK == result);

        acls[i].bind_info.time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        acls[i].bind_info.counter_enable = RULE_TYPE_COUNTER_DISABLE;
        RULE_OM_SET_ENTRY_ACTIVE(acls[i].bind_info.row_status);
    }

    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex, acls[i].direction, acls[i].acl_type);
            assert(NULL == bind_acl_p);
        }
    }

    //
    // SetAcl
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex,acls[i].direction))
                continue;

            result = RULE_OM_SetAcl(ifindex,
                                    acls[i].direction,
                                    acls[i].bind_info.acl_index,
                                    acls[i].bind_info.time_range_index,
                                    acls[i].bind_info.time_range_name,
                                    acls[i].bind_info.counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    //
    // Verification
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
        RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex,acls[i].direction))
                continue;

            bind_acl_p = RULE_OM_LocalGetNextPortAclByPrecedence(ifindex, &precedence);
            assert(NULL != bind_acl_p);
            assert(0 == memcmp(bind_acl_p, &acls[i].bind_info, sizeof(*bind_acl_p)));

            bind_acl_p = RULE_OM_LocalGetPortAclByPrecedence(ifindex, precedence);
            assert(NULL != bind_acl_p);
            assert(0 == memcmp(bind_acl_p, &acls[i].bind_info, sizeof(*bind_acl_p)));
        }

        bind_acl_p = RULE_OM_LocalGetNextPortAclByPrecedence(ifindex, &precedence);
        assert(NULL == bind_acl_p);
    }

    return 0;
}

int
RULE_OM_UT_Get_Port_Acl_By_Precedence_When_Apply_Partial_Acl()
{
    enum
    {
        MIN_IFINDEX = 1,
        MAX_IFINDEX = 5,

        MIN_DIRECTION = RULE_TYPE_INBOUND,
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
    };

    //
    // Sort by precedence
    //
    struct experimental_group
    {
        RULE_TYPE_AclType_T acl_type;
        RULE_TYPE_InOutDirection_T direction;
        UI32_T precedence;
        RULE_TYPE_AclMemento_T bind_info;
    } acls[] =
    {
        // Remove INGRESS_MAC_ACL_PRECEDENCE
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        // Remove INGRESS_IP_ACL_PRECEDENCE
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
        // Remove EGRESS_IPV6_ACL_PRECEDENCE
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    UI32_T pl_idx;
    UI32_T ifindex;

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);
        result = RULE_OM_CreateAcl(acl_name, acls[i].acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &acls[i].bind_info.acl_index);
        assert(RULE_TYPE_OK == result);

        acls[i].bind_info.time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        acls[i].bind_info.counter_enable = RULE_TYPE_COUNTER_DISABLE;
        RULE_OM_SET_ENTRY_ACTIVE(acls[i].bind_info.row_status);
    }

    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex, acls[i].direction, acls[i].acl_type);
            assert(NULL == bind_acl_p);
        }
    }

    //
    // SetAcl
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex,acls[i].direction))
                continue;

            result = RULE_OM_SetAcl(ifindex,
                                    acls[i].direction,
                                    acls[i].bind_info.acl_index,
                                    acls[i].bind_info.time_range_index,
                                    acls[i].bind_info.time_range_name,
                                    acls[i].bind_info.counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    //
    // Verification
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
        RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
                continue;

            bind_acl_p = RULE_OM_LocalGetNextPortAclByPrecedence(ifindex, &precedence);
            assert(NULL != bind_acl_p);
            assert(0 == memcmp(bind_acl_p, &acls[i].bind_info, sizeof(*bind_acl_p)));

            bind_acl_p = RULE_OM_LocalGetPortAclByPrecedence(ifindex, precedence);
            assert(NULL != bind_acl_p);
            assert(0 == memcmp(bind_acl_p, &acls[i].bind_info, sizeof(*bind_acl_p)));
        }

        bind_acl_p = RULE_OM_LocalGetNextPortAclByPrecedence(ifindex, &precedence);
        assert(NULL == bind_acl_p);
    }

    return 0;
}

int
RULE_OM_UT_Get_Port_Acl_By_Direction_And_Precedence()
{
    enum
    {
        MIN_IFINDEX = 1,
        MAX_IFINDEX = 5,

        MIN_DIRECTION = RULE_TYPE_INBOUND,
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
    };

    //
    // Sort by precedence
    //
    struct experimental_group
    {
        RULE_TYPE_AclType_T acl_type;
        RULE_TYPE_InOutDirection_T direction;
        UI32_T precedence;
        RULE_TYPE_AclMemento_T bind_info;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    UI32_T pl_idx;
    UI32_T ifindex;

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);
        result = RULE_OM_CreateAcl(acl_name, acls[i].acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &acls[i].bind_info.acl_index);
        assert(RULE_TYPE_OK == result);

        acls[i].bind_info.time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        acls[i].bind_info.counter_enable = RULE_TYPE_COUNTER_DISABLE;
        RULE_OM_SET_ENTRY_ACTIVE(acls[i].bind_info.row_status);
    }

    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);
    
        for (i = 0; i < _countof(acls); ++ i)
        {
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex, acls[i].direction, acls[i].acl_type);
            assert(NULL == bind_acl_p);
        }
    }

    //
    // SetAcl
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
                continue;

            result = RULE_OM_SetAcl(ifindex,
                                    acls[i].direction,
                                    acls[i].bind_info.acl_index,
                                    acls[i].bind_info.time_range_index,
                                    acls[i].bind_info.time_range_name,
                                    acls[i].bind_info.counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    //
    // Verification
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        RULE_TYPE_InOutDirection_T direction;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
        {
            UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
                continue;

            for (i = 0; i < _countof(acls); ++ i)
            {
                if (acls[i].direction != direction)
                {
                    continue;
                }

                bind_acl_p = RULE_OM_LocalGetNextPortAclByDirectionAndPrecedence(ifindex, direction, &precedence);
                assert(NULL != bind_acl_p);
                assert(0 == memcmp(bind_acl_p, &acls[i].bind_info, sizeof(*bind_acl_p)));

                bind_acl_p = RULE_OM_LocalGetPortAclByPrecedence(ifindex, precedence);
                assert(NULL != bind_acl_p);
                assert(0 == memcmp(bind_acl_p, &acls[i].bind_info, sizeof(*bind_acl_p)));
            }

            bind_acl_p = RULE_OM_LocalGetNextPortAclByDirectionAndPrecedence(ifindex, direction, &precedence);
            assert(NULL == bind_acl_p);
        }
    }

    return 0;
}

static int
RULE_OM_TEST_GetNextAclByPort()
{
    enum
    {
        MIN_DIRECTION = RULE_TYPE_INBOUND,
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
    };

    //
    // Sort by precedence
    //
    struct experimental_group
    {
        RULE_TYPE_AclType_T acl_type;
        RULE_TYPE_InOutDirection_T direction;
        UI32_T precedence;
        RULE_TYPE_AclMemento_T bind_info;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    UI32_T pl_idx;
    UI32_T ifindex;

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);
        result = RULE_OM_CreateAcl(acl_name, acls[i].acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &acls[i].bind_info.acl_index);
        assert(RULE_TYPE_OK == result);

        acls[i].bind_info.time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        acls[i].bind_info.counter_enable = RULE_TYPE_COUNTER_DISABLE;
        RULE_OM_SET_ENTRY_ACTIVE(acls[i].bind_info.row_status);
    }

    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);
        
        for (i = 0; i < _countof(acls); ++ i)
        {
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex, acls[i].direction, acls[i].acl_type);
            assert(NULL == bind_acl_p);
        }
    }

    //
    // SetAcl
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
                continue;

            result = RULE_OM_SetAcl(ifindex,
                                    acls[i].direction,
                                    acls[i].bind_info.acl_index,
                                    acls[i].bind_info.time_range_index,
                                    acls[i].bind_info.time_range_name,
                                    acls[i].bind_info.counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    //
    // Verification
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
        RULE_TYPE_AclMemento_T bind_acl;
        RULE_TYPE_Acl_T acl_entry;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        memset(&bind_acl, 0, sizeof(bind_acl));
        RULE_OM_SET_ENTRY_ACTIVE(bind_acl.row_status);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
                continue;

            result = RULE_OM_GetNextAclByPort(ifindex, &bind_acl.acl_index,
                                              &acl_entry, &precedence,
                                              bind_acl.time_range_name,
                                              &bind_acl.counter_enable);
            assert(RULE_TYPE_OK == result);

            // FIXME: should get time range index
            if ('\0' == bind_acl.time_range_name[0])
            {
                bind_acl.time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            }

            assert(0 == memcmp(&bind_acl, &acls[i].bind_info, sizeof(bind_acl)));

            {
                RULE_TYPE_AclType_T port_acl_type;
                RULE_TYPE_InOutDirection_T direction;

                result = RULE_OM_LocalAclTypeToPortAclType(acl_entry.acl_type, &port_acl_type);
                assert(RULE_TYPE_OK == result);

                result = RULE_OM_LocalPrecedenceToDirection(precedence, &direction);
                assert(RULE_TYPE_OK == result);

                result = RULE_OM_GetPortAclIndex(ifindex, port_acl_type,
                                                 direction == RULE_TYPE_INBOUND ? TRUE : FALSE,
                                                 &bind_acl.acl_index,
                                                 bind_acl.time_range_name,
                                                 &bind_acl.counter_enable);
                assert(RULE_TYPE_OK == result);
                assert(0 == memcmp(&bind_acl, &acls[i].bind_info, sizeof(bind_acl)));
            }
        }

        result = RULE_OM_GetNextAclByPort(ifindex, &bind_acl.acl_index,
                                          &acl_entry, &precedence,
                                          bind_acl.time_range_name,
                                          &bind_acl.counter_enable);
        assert(RULE_TYPE_OK != result);
    }
    
    return 0;
}

static int
RULE_OM_TEST_GetNextUIAclByPort()
{
    enum
    {
        MIN_DIRECTION = RULE_TYPE_INBOUND,
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
    };

    //
    // Sort by precedence
    //
    struct experimental_group
    {
        RULE_TYPE_AclType_T acl_type;
        RULE_TYPE_InOutDirection_T direction;
        UI32_T precedence;
        RULE_TYPE_AclMemento_T bind_info;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    UI32_T pl_idx;
    UI32_T ifindex;

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);
        result = RULE_OM_CreateAcl(acl_name, acls[i].acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &acls[i].bind_info.acl_index);
        assert(RULE_TYPE_OK == result);

        acls[i].bind_info.time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        acls[i].bind_info.counter_enable = RULE_TYPE_COUNTER_DISABLE;
        RULE_OM_SET_ENTRY_ACTIVE(acls[i].bind_info.row_status);
    }

    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);
        
        for (i = 0; i < _countof(acls); ++ i)
        {
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex, acls[i].direction, acls[i].acl_type);
            assert(NULL == bind_acl_p);
        }
    }

    //
    // SetAcl
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
                continue;

            result = RULE_OM_SetAcl(ifindex,
                                    acls[i].direction,
                                    acls[i].bind_info.acl_index,
                                    acls[i].bind_info.time_range_index,
                                    acls[i].bind_info.time_range_name,
                                    acls[i].bind_info.counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    //
    // Verification
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        RULE_TYPE_InOutDirection_T direction;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
        {
            UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
            RULE_TYPE_AclMemento_T bind_acl;
            RULE_TYPE_UI_AclEntry_T acl_entry;

            memset(&bind_acl, 0, sizeof(bind_acl));

            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
                continue;

            for (i = 0; i < _countof(acls); ++ i)
            {
                if (acls[i].direction != direction)
                {
                    continue;
                }

                // For testing by memcmp
                memset(&acl_entry, 0, sizeof(acl_entry));

                result = RULE_OM_GetNextUIAclByPort(ifindex, direction,
                                                    &bind_acl.acl_index,
                                                    &acl_entry, &precedence,
                                                    bind_acl.time_range_name,
                                                    &bind_acl.counter_enable);
                assert(RULE_TYPE_OK == result);
                assert(0 == memcmp(&bind_acl, &acls[i].bind_info, sizeof(bind_acl)));

                {
                    RULE_TYPE_AclType_T port_acl_type;

                    result = RULE_OM_LocalAclTypeToPortAclType(acl_entry.acl_type, &port_acl_type);
                    assert(RULE_TYPE_OK == result);

                    result = RULE_OM_GetPortAclIndex(ifindex, port_acl_type,
                                                     direction == RULE_TYPE_INBOUND ? TRUE : FALSE,
                                                     &bind_acl.acl_index,
                                                     bind_acl.time_range_name,
                                                     &bind_acl.counter_enable);
                    assert(RULE_TYPE_OK == result);
                    assert(0 == memcmp(&bind_acl, &acls[i].bind_info, sizeof(bind_acl)));
                }

                {
                    RULE_TYPE_UI_AclEntry_T acl_entry_2;

                    memset(&acl_entry_2, 0, sizeof(acl_entry_2));

                    acl_entry_2.acl_type = acl_entry.acl_type;

                    result = RULE_OM_GetUIAclByTypeAndDirection(ifindex, &acl_entry_2, direction == RULE_TYPE_INBOUND ? TRUE : FALSE);
                    assert(RULE_TYPE_OK == result);

                    assert(0 == memcmp(&acl_entry, &acl_entry_2, sizeof(acl_entry)));
                }
            }

            result = RULE_OM_GetNextUIAclByPort(ifindex, direction,
                                                &bind_acl.acl_index,
                                                &acl_entry, &precedence,
                                                bind_acl.time_range_name,
                                                &bind_acl.counter_enable);
            assert(RULE_TYPE_OK != result);
        }
    }
    
    return 0;
}

static int
RULE_OM_TEST_Get_Next_UIAcl_By_Port_And_Binding_Type()
{
    enum
    {
        MIN_DIRECTION = RULE_TYPE_INBOUND,
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
    };

    //
    // Sort by precedence
    //
    struct experimental_group
    {
        RULE_TYPE_AclType_T acl_type;
        RULE_TYPE_InOutDirection_T direction;
        UI32_T precedence;
        RULE_TYPE_AclMemento_T bind_info;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_MAC_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_MAC_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IP_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_INBOUND,
            INGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            RULE_TYPE_OUTBOUND,
            EGRESS_IPV6_ACL_PRECEDENCE,
            {0}
        },
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    UI32_T pl_idx;
    UI32_T ifindex;

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);
        result = RULE_OM_CreateAcl(acl_name, acls[i].acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &acls[i].bind_info.acl_index);
        assert(RULE_TYPE_OK == result);

        acls[i].bind_info.time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        acls[i].bind_info.counter_enable = RULE_TYPE_COUNTER_DISABLE;
        RULE_OM_SET_ENTRY_ACTIVE(acls[i].bind_info.row_status);
    }

    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            bind_acl_p = RULE_OM_LocalGetPortAclByType(ifindex, acls[i].direction, acls[i].acl_type);
            assert(NULL == bind_acl_p);
        }
    }

    //
    // SetAcl
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
                continue;

            result = RULE_OM_SetAcl(ifindex,
                                    acls[i].direction,
                                    acls[i].bind_info.acl_index,
                                    acls[i].bind_info.time_range_index,
                                    acls[i].bind_info.time_range_name,
                                    acls[i].bind_info.counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    //
    // Verification
    //
    for (i = 0; i < _countof(acls); ++ i)
    {
        RULE_TYPE_InOutDirection_T direction;

        UI32_T precedence = acls[i].precedence;

        UI32_T bind_ifindex;
        RULE_TYPE_UI_AclEntry_T acl_entry;

        result = RULE_OM_LocalPrecedenceToDirection(precedence, &direction);
        assert(RULE_TYPE_OK == result);

        for (pl_idx = 0, bind_ifindex = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);
            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, direction))
                continue;

            // For testing by memcmp
            memset(&acl_entry, 0, sizeof(acl_entry));

            result = RULE_OM_GetNextUIAclByPortAndBindingType(precedence, &bind_ifindex, &acl_entry, NULL);
            assert(RULE_TYPE_OK == result);

            assert(ifindex == bind_ifindex);

            {
                RULE_TYPE_UI_AclEntry_T acl_entry_2;

                memset(&acl_entry_2, 0, sizeof(acl_entry_2));

                acl_entry_2.acl_type = acl_entry.acl_type;

                result = RULE_OM_GetUIAclByTypeAndDirection(ifindex, &acl_entry_2, direction == RULE_TYPE_INBOUND ? TRUE : FALSE);
                assert(RULE_TYPE_OK == result);

                assert(0 == memcmp(&acl_entry, &acl_entry_2, sizeof(acl_entry)));
            }
        }

        result = RULE_OM_GetNextUIAclByPortAndBindingType(precedence, &bind_ifindex, &acl_entry, NULL);
        assert(RULE_TYPE_OK != result);
        
    }
    
    return 0;
}

static int
RULE_OM_TEST_Get_UI_Port_Entry()
{
    enum
    {
        MIN_DIRECTION = RULE_TYPE_INBOUND,
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    UI32_T pl_idx;
    UI32_T ifindex;

    RULE_TYPE_PortEntry_T port_entry;

    RULE_TYPE_AclType_T acl_type_array[] =
    {
        RULE_TYPE_MAC_ACL,
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IPV6_STD_ACL
    };

    memset(&port_entry, 0, sizeof(port_entry));

    for (i = 0; i < _countof(acl_type_array); ++ i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_array[i];
        RULE_TYPE_AclType_T port_acl_type;

        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalAclTypeToPortAclType(acl_type, &port_acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &port_entry.bind_acl[port_acl_type].acl_index);
        assert(RULE_TYPE_OK == result);

        for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
        {
            RULE_TYPE_InOutDirection_T direction;
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
            {
                UI16_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

                if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, direction))
                    continue;

                result = RULE_OM_SetAcl(ifindex,
                                        direction,
                                        port_entry.bind_acl[port_acl_type].acl_index,
                                        time_range_index,
                                        time_range_name,
                                        counter_enable);
                assert(RULE_TYPE_OK == result);
            }
        }
    }

    // policy-map
    {
        const char                  *policy_map_name = "p1";
        RULE_TYPE_PortEntry_T       *port_entry_p;
        RULE_TYPE_PolicyMap_T       *policy_map_p;
        UI32_T                      uport_ifindex;
        RULE_TYPE_InOutDirection_T  inout_profile;
        UI32_T                      policy_map_index;
        RULE_TYPE_RETURN_TYPE_T     result;

        result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);

        policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
        assert(NULL != policy_map_p);

        for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
        {
            uport_ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            for (inout_profile = MIN_DIRECTION; inout_profile <= MAX_DIRECTION; ++ inout_profile)
            {
                if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, inout_profile))
                    continue;

                port_entry.policy_map_index = policy_map_index;

                result = RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
                assert(RULE_TYPE_OK == result);

                port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
                assert(policy_map_index == port_entry_p->policy_map_index);

                assert((pl_idx) * (MAX_DIRECTION+1) + (inout_profile+1) == policy_map_p->port_count);

                assert(RULE_OM_IS_BIT_ON(policy_map_p->qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(uport_ifindex)));
            }
        }
    }

    //
    // Verification
    //
    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        RULE_TYPE_InOutDirection_T direction;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
        {
            RULE_TYPE_UI_PortEntry_T ui_port_entry;

            if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, direction))
                continue;

            result = RULE_OM_GetUIPortEntry(ifindex, direction, &ui_port_entry);
            assert(RULE_TYPE_OK == result);

            assert(ui_port_entry.ingress_mac_acl_index == port_entry.bind_acl[RULE_TYPE_MAC_ACL].acl_index);
            assert(ui_port_entry.ingress_ip_acl_index == port_entry.bind_acl[RULE_TYPE_IP_ACL].acl_index);
            assert(ui_port_entry.ingress_ipv6_acl_index == port_entry.bind_acl[RULE_TYPE_IPV6_ACL].acl_index);
            assert(ui_port_entry.policy_map_index == port_entry.policy_map_index);
            
        }
    }
    
    return 0;
}

static int
RULE_OM_TEST_Get_Next_UI_Port_Entry()
{
    enum
    {
        MIN_DIRECTION = RULE_TYPE_INBOUND,
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    UI32_T pl_idx;
    UI32_T ifindex;

    RULE_TYPE_PortEntry_T port_entry;

    RULE_TYPE_AclType_T acl_type_array[] =
    {
        RULE_TYPE_MAC_ACL,
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IPV6_STD_ACL
    };

    memset(&port_entry, 0, sizeof(port_entry));

    for (i = 0; i < _countof(acl_type_array); ++ i)
    {
        RULE_TYPE_AclType_T acl_type = acl_type_array[i];
        RULE_TYPE_AclType_T port_acl_type;

        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalAclTypeToPortAclType(acl_type, &port_acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &port_entry.bind_acl[port_acl_type].acl_index);
        assert(RULE_TYPE_OK == result);

        for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
        {
            RULE_TYPE_InOutDirection_T direction;
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
            {
                UI16_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

                if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, direction))
                    continue;

                result = RULE_OM_SetAcl(ifindex,
                                        direction,
                                        port_entry.bind_acl[port_acl_type].acl_index,
                                        time_range_index,
                                        time_range_name,
                                        counter_enable);
                assert(RULE_TYPE_OK == result);
            }
        }
    }

    // policy-map
    {
        const char                  *policy_map_name = "p1";
        RULE_TYPE_PortEntry_T       *port_entry_p;
        RULE_TYPE_PolicyMap_T       *policy_map_p;
        UI32_T                      uport_ifindex;
        RULE_TYPE_InOutDirection_T  inout_profile;
        UI32_T                      policy_map_index;
        RULE_TYPE_RETURN_TYPE_T     result;

        result = RULE_OM_Create_PolicyMap("aa", &policy_map_index);
        assert(RULE_TYPE_OK == result);
        result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);

        policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
        assert(NULL != policy_map_p);

        for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
        {
            uport_ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            for (inout_profile = MIN_DIRECTION; inout_profile <= MAX_DIRECTION; ++ inout_profile)
            {
                if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, inout_profile))
                    continue;

                port_entry.policy_map_index = policy_map_index;

                result = RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
                assert(RULE_TYPE_OK == result);

                port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
                assert(policy_map_index == port_entry_p->policy_map_index);

                assert((pl_idx) * (MAX_DIRECTION+1) + (inout_profile+1) == policy_map_p->port_count);

                assert(RULE_OM_IS_BIT_ON(policy_map_p->qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(uport_ifindex)));
            }
        }
    }

    /*
     * Verification
     */
    {
        RULE_TYPE_InOutDirection_T direction;

        UI32_T test_ifindex = 0;
        RULE_TYPE_InOutDirection_T test_direction = RULE_TYPE_INBOUND;

        RULE_TYPE_UI_PortEntry_T test_ui_port_entry;

        for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            for (direction = RULE_TYPE_INBOUND; direction <= RULE_TYPE_OUTBOUND; ++ direction)
            {
                if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, direction))
                    continue;

                memset(&test_ui_port_entry, 0, sizeof(test_ui_port_entry));

                result = RULE_OM_GetNextUIPortEntry(&test_ifindex, &test_direction, &test_ui_port_entry);
                assert(RULE_TYPE_OK == result);

                assert(test_ifindex == ifindex);
                assert(test_direction == direction);

                assert(test_ui_port_entry.ingress_mac_acl_index == port_entry.bind_acl[RULE_TYPE_MAC_ACL].acl_index);
                assert(test_ui_port_entry.ingress_ip_acl_index == port_entry.bind_acl[RULE_TYPE_IP_ACL].acl_index);
                assert(test_ui_port_entry.ingress_ipv6_acl_index == port_entry.bind_acl[RULE_TYPE_IPV6_ACL].acl_index);
                assert(test_ui_port_entry.policy_map_index == port_entry.policy_map_index);

                {
                    RULE_TYPE_UI_PortEntry_T tmp_ui_port_entry;

                    memset(&tmp_ui_port_entry, 0, sizeof(tmp_ui_port_entry));
                    result = RULE_OM_GetUIPortEntry(ifindex, direction, &tmp_ui_port_entry);
                    assert(RULE_TYPE_OK == result);

                    assert(0 == memcmp(&test_ui_port_entry, &tmp_ui_port_entry, sizeof(test_ui_port_entry)));
                }
            }
        }
        
        result = RULE_OM_GetNextUIPortEntry(&test_ifindex, &test_direction, &test_ui_port_entry);
        assert(RULE_TYPE_OK != result);
    }
    
    return 0;
}

static int
RULE_OM_UT_Get_Next_Mib_Ace_ByPort()
{
//    ASSERT(0);
    return 0;
}

int
RULE_OM_UT_PolicyMapPtrToIndex()
{
    assert(1 == RULE_OM_LocalPolicyMapPtrToIndex(&shmem_data_p->policy_map_table[0]));
    assert(2 == RULE_OM_LocalPolicyMapPtrToIndex(&shmem_data_p->policy_map_table[1]));
    assert(3 == RULE_OM_LocalPolicyMapPtrToIndex(&shmem_data_p->policy_map_table[2]));
    assert(4 == RULE_OM_LocalPolicyMapPtrToIndex(&shmem_data_p->policy_map_table[3]));
    assert(5 == RULE_OM_LocalPolicyMapPtrToIndex(&shmem_data_p->policy_map_table[4]));
    return 0;
}

int
RULE_OM_UT_Get_Next_PolicyMap()
{
    struct input_s
    {
        const char *policy_map_name;
        UI32_T policy_map_index;
    } input[] =
    {
        {"p1", 0},
        {"p2", 0},
        {"p3", 0},
        {"p4", 0},
        {"p5", 0},
    };

    RULE_TYPE_PolicyMap_T   *policy_map_p;
    UI32_T                  policy_map_index;
    UI32_T                  i;
    UI32_T                  result;

    for (i = 0; i < _countof(input); ++i)
    {
        result = RULE_OM_Create_PolicyMap(input[i].policy_map_name, &input[i].policy_map_index);
        assert(RULE_TYPE_OK == result);
    }

    policy_map_index = 0;
    for (i = 0; i < _countof(input); ++i)
    {
        policy_map_p = RULE_OM_LocalGetNextPolicyMap(&policy_map_index);
        assert(NULL != policy_map_p);
        assert(input[i].policy_map_index == policy_map_index);
    }

    policy_map_p = RULE_OM_LocalGetNextPolicyMap(&policy_map_index);
    assert(NULL == policy_map_p);

    return 0;
}

int
RULE_OM_UT_Get_Next_PolicyMap_By_ClassMap_Index()
{
    struct class_map_s
    {
        const char *class_map_name;
        UI32_T class_map_index;
    } class_map[] =
    {
        {"c1", 0},
        {"c2", 0},
        {"c3", 0},
        {"c4", 0},
        {"c5", 0},
        {"c6", 0},
        {"c7", 0},
        {"c8", 0},
    };

    struct policy_map_s
    {
        const char *policy_map_name;
        UI32_T policy_map_index;
        const char *class_map_name[10];
    } policy_map[] =
    {
        {"p1", 0, {"c1", "c2", "c3", "c4", NULL},},
        {"p2", 0, {"c5", "c6", "c7", "c8", NULL},},
        {"p3", 0, {"c1", "c3", "c5", "c7", NULL},},
        {"p4", 0, {"c2", "c4", "c6", "c8", NULL},},
        {"p5", 0, {"c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", NULL},},
    };

    struct get_result_s
    {
        const char *class_map_name;
        const char *policy_map_name[10];
    } get_result[] =
    {
        {"c1", {"p1", "p3", "p5", NULL},},
        {"c2", {"p1", "p4", "p5", NULL},},
        {"c3", {"p1", "p3", "p5", NULL},},
        {"c4", {"p1", "p4", "p5", NULL},},
        {"c5", {"p2", "p3", "p5", NULL},},
        {"c6", {"p2", "p4", "p5", NULL},},
        {"c7", {"p2", "p3", "p5", NULL},},
        {"c8", {"p2", "p4", "p5", NULL},},
    };

    UI32_T                  i;
    UI32_T                  result;
    UI32_T                  result_policy_map_index;
    RULE_TYPE_PolicyMap_T   *result_policy_map;

    for (i = 0; i < _countof(class_map); ++i)
    {
        result = RULE_OM_Create_ClassMap(class_map[i].class_map_name, RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map[i].class_map_index);
        assert(RULE_TYPE_OK == result);
    }

    for (i = 0; i < _countof(policy_map); ++i)
    {
        UI32_T j;
        UI32_T class_map_index;

        result = RULE_OM_Create_PolicyMap(policy_map[i].policy_map_name, &policy_map[i].policy_map_index);
        assert(RULE_TYPE_OK == result);

        for (j = 0; j < _countof(policy_map[i].class_map_name); ++j)
        {
            if (NULL == policy_map[i].class_map_name[j])
            {
                break;
            }

            result = RULE_OM_GetClassMapIndexByName(policy_map[i].class_map_name[j], &class_map_index);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map[i].policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (i = 0; i < _countof(get_result); ++i)
    {
        UI32_T j;
        UI32_T class_map_index;
        UI32_T policy_map_index;

        result = RULE_OM_GetClassMapIndexByName(get_result[i].class_map_name, &class_map_index);
        assert(RULE_TYPE_OK == result);

        result_policy_map_index = 0;
        for (j = 0; j < _countof(get_result[i].policy_map_name); ++j)
        {
            if (NULL == get_result[i].policy_map_name[j])
            {
                break;
            }

            result = RULE_OM_GetPolicyMapIdByName(get_result[i].policy_map_name[j], &policy_map_index);
            assert(RULE_TYPE_OK == result);

            result_policy_map = RULE_OM_LocalGetNextPolicyMapByClassMapIndex(class_map_index, &result_policy_map_index);
            assert(NULL != result_policy_map);
            assert(result_policy_map_index == policy_map_index);
            assert(result_policy_map_index == RULE_OM_LocalPolicyMapPtrToIndex(result_policy_map));
        }

        result_policy_map = RULE_OM_LocalGetNextPolicyMapByClassMapIndex(class_map_index, &result_policy_map_index);
        assert(NULL == result_policy_map);

    }

    return 0;
}

int
RULE_OM_UT_Get_Next_PolicyMap_By_ClassMap_Action_Meter_Index()
{
    struct class_map_s
    {
        const char *class_map_name;
        UI32_T class_map_index;
    } class_map[] =
    {
        {"c1", 0},
        {"c2", 0},
        {"c3", 0},
        {"c4", 0},
        {"c5", 0},
        {"c6", 0},
        {"c7", 0},
        {"c8", 0},
    };

    struct policy_map_s
    {
        const char *policy_map_name;
        UI32_T policy_map_index;
        const char *class_map_name[10];
        UI32_T meter_index[10];
        UI32_T action_index[10];
    } policy_map[] =
    {
        {"p1", 0, {"c1", "c2", "c3", "c4", NULL}, {0}, {0}},
        {"p2", 0, {"c5", "c6", "c7", "c8", NULL}, {0}, {0}},
        {"p3", 0, {"c1", "c3", "c5", "c7", NULL}, {0}, {0}},
        {"p4", 0, {"c2", "c4", "c6", "c8", NULL}, {0}, {0}},
        {"p5", 0, {"c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", NULL},  {0}, {0}},
    };

    struct class_map_result_s
    {
        BOOL_T valid_status;
        UI32_T policy_map_index[10];
        UI32_T policy_map_count;
    } class_map_result[SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP+1];

    struct meter_result_s
    {
        BOOL_T valid_status;
        UI32_T policy_map_index[10];
        UI32_T policy_map_count;
    } meter_result[SYS_ADPT_DIFFSERV_MAX_NBR_OF_METER+1];

    struct action_result_s
    {
        BOOL_T valid_status;
        UI32_T policy_map_index[10];
        UI32_T policy_map_count;
    } action_result[SYS_ADPT_DIFFSERV_MAX_NBR_OF_ACTIONS+1];

    UI32_T                  i;
    UI32_T                  result;
    UI32_T                  result_policy_map_index;
    RULE_TYPE_PolicyMap_T   *result_policy_map;

    memset(class_map_result, 0, sizeof(class_map_result));
    memset(meter_result, 0, sizeof(meter_result));
    memset(action_result, 0, sizeof(action_result));

    for (i = 0; i < _countof(class_map); ++i)
    {
        result = RULE_OM_Create_ClassMap(class_map[i].class_map_name, RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map[i].class_map_index);
        assert(RULE_TYPE_OK == result);
    }

    for (i = 0; i < _countof(policy_map); ++i)
    {
        UI32_T j;
        UI32_T policy_map_index;
        UI32_T class_map_index;

        result = RULE_OM_Create_PolicyMap(policy_map[i].policy_map_name, &policy_map[i].policy_map_index);
        assert(RULE_TYPE_OK == result);

        policy_map_index = policy_map[i].policy_map_index;

        for (j = 0; j < _countof(policy_map[i].class_map_name); ++j)
        {
            RULE_TYPE_UI_Action_T           ui_action;
            RULE_TYPE_TBParamEntry_T        meter;
            RULE_TYPE_PolicyElement_T       policy_element;

            if (NULL == policy_map[i].class_map_name[j])
            {
                break;
            }

            result = RULE_OM_GetClassMapIndexByName(policy_map[i].class_map_name[j], &class_map_index);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            ui_action.in_action_type        = IN_ACTION_INVALID;
            ui_action.out_action_type       = OUT_ACTION_INVALID;
            ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
            ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
            ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

            ui_action.class_action_type     = CLASS_ACTION_SET_PRIORITY;
            ui_action.class_action_value    = 1000;

            meter.meter_model = RULE_TYPE_METER_MODE_FLOW;
            meter.rate = 5000;
            meter.burst_size = 4000;

            result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_GetPolicyMapElementEntry(policy_map_index, class_map_index, &policy_element);
            assert(RULE_TYPE_OK == result);

            policy_map[i].meter_index[j]    = policy_element.meter_index;
            policy_map[i].action_index[j]   = policy_element.action_index;

            {
                UI32_T count = class_map_result[ policy_element.class_map_index ].policy_map_count;

                class_map_result[ policy_element.class_map_index ].valid_status = TRUE;
                class_map_result[ policy_element.class_map_index ].policy_map_index[count++] = policy_map[i].policy_map_index;
                class_map_result[ policy_element.class_map_index ].policy_map_count = count;
            }

            {
                UI32_T count = meter_result[ policy_element.meter_index ].policy_map_count;

                meter_result[ policy_element.meter_index ].valid_status = TRUE;
                meter_result[ policy_element.meter_index ].policy_map_index[count++] = policy_map[i].policy_map_index;
                meter_result[ policy_element.meter_index ].policy_map_count = count;
            }

            {
                UI32_T count = action_result[ policy_element.action_index ].policy_map_count;

                action_result[ policy_element.action_index ].valid_status = TRUE;
                action_result[ policy_element.action_index ].policy_map_index[count++] = policy_map[i].policy_map_index;
                action_result[ policy_element.action_index ].policy_map_count = count;
            }
        }
    }

    for (i = 0; i < _countof(class_map_result); ++i)
    {
        UI32_T j;
        UI32_T class_map_index;
        UI32_T policy_map_index;

        if (TRUE != class_map_result[i].valid_status)
        {
            continue;
        }

        class_map_index = i;

        result_policy_map_index = 0;
        for (j = 0; j < _countof(class_map_result[i].policy_map_index); ++j)
        {
            if (NULL == class_map_result[i].policy_map_index[j])
            {
                break;
            }

            policy_map_index = class_map_result[i].policy_map_index[j];

            result_policy_map = RULE_OM_LocalGetNextPolicyMapByClassMapIndex(class_map_index, &result_policy_map_index);
            assert(NULL != result_policy_map);
            assert(result_policy_map_index == policy_map_index);
            assert(result_policy_map_index == RULE_OM_LocalPolicyMapPtrToIndex(result_policy_map));
        }

        result_policy_map = RULE_OM_LocalGetNextPolicyMapByClassMapIndex(class_map_index, &result_policy_map_index);
        assert(NULL == result_policy_map);

    }

    for (i = 0; i < _countof(meter_result); ++i)
    {
        UI32_T j;
        UI32_T meter_index;
        UI32_T policy_map_index;

        if (TRUE != meter_result[i].valid_status)
        {
            continue;
        }

        meter_index = i;

        result_policy_map_index = 0;
        for (j = 0; j < _countof(meter_result[i].policy_map_index); ++j)
        {
            if (NULL == meter_result[i].policy_map_index[j])
            {
                break;
            }

            policy_map_index = meter_result[i].policy_map_index[j];

            result_policy_map = RULE_OM_LocalGetNextPolicyMapByMeterIndex(meter_index, &result_policy_map_index);
            assert(NULL != result_policy_map);
            assert(result_policy_map_index == policy_map_index);
            assert(result_policy_map_index == RULE_OM_LocalPolicyMapPtrToIndex(result_policy_map));
        }

        result_policy_map = RULE_OM_LocalGetNextPolicyMapByMeterIndex(meter_index, &result_policy_map_index);
        assert(NULL == result_policy_map);

    }

    for (i = 0; i < _countof(action_result); ++i)
    {
        UI32_T j;
        UI32_T action_index;
        UI32_T policy_map_index;

        if (TRUE != action_result[i].valid_status)
        {
            continue;
        }

        action_index = i;

        result_policy_map_index = 0;
        for (j = 0; j < _countof(action_result[i].policy_map_index); ++j)
        {
            if (NULL == action_result[i].policy_map_index[j])
            {
                break;
            }

            policy_map_index = action_result[i].policy_map_index[j];

            result_policy_map = RULE_OM_LocalGetNextPolicyMapByActionIndex(action_index, &result_policy_map_index);
            assert(NULL != result_policy_map);
            assert(result_policy_map_index == policy_map_index);
            assert(result_policy_map_index == RULE_OM_LocalPolicyMapPtrToIndex(result_policy_map));
        }

        result_policy_map = RULE_OM_LocalGetNextPolicyMapByActionIndex(action_index, &result_policy_map_index);
        assert(NULL == result_policy_map);

    }

    return 0;
}

int
RULE_OM_UT_RunTestCaese()
{
#define RULE_OM_UT_TEST(func) \
    UT_Test(RULE_OM_UT_Setup, RULE_OM_UT_Teardown, func)

    if (l4_options.run_rule_om)
    {
        RULE_OM_UT_TEST(RULE_OM_UT_Add_And_Remove_Ace_2_Acl);
        RULE_OM_UT_TEST(RULE_OM_UT_Bind_Unbind_Element_2_Acl);
        RULE_OM_UT_TEST(RULE_OM_UT_Bind_Unbind_Element_2_Acl_Use_Wrong_Ace_Index);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Acl_Index_And_Type_By_Ace);
        RULE_OM_UT_TEST(RULE_OM_UT_Del_Acl);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Number_Of_Permit_Access_By_Acl_Index);
        RULE_OM_UT_TEST(RULE_OM_UT_Arp_ACL_Check_For_Dai);
        RULE_OM_UT_TEST(RULE_OM_UT_Copy_ACL);
        RULE_OM_UT_TEST(RULE_OM_UT_Copy_ACL_Undo);
        RULE_OM_UT_TEST(RULE_OM_UT_Del_All_Ace_From_Acl);

        RULE_OM_UT_TEST(RULE_OM_UT_IsSameAceWithoutAccessField);
        RULE_OM_UT_TEST(RULE_OM_UT_IsMatchAnyAnyAce);
        RULE_OM_UT_TEST(RULE_OM_UT_GetNextAceByAcl);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACE_Basic);
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_1);
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_2);
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_Best_Case_Inc);
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_Best_Case_Dec);
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_Best_Case_Interlace);
#endif // #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)

        RULE_OM_UT_TEST(RULE_OM_TEST_Max_ACE_Of_ACL);
        RULE_OM_UT_TEST(RULE_OM_TEST_Max_ACE_Of_System);
        RULE_OM_UT_TEST(RULE_OM_TEST_Create_ACL_By_Index);
        RULE_OM_UT_TEST(RULE_OM_TEST_Max_ACL_Of_System);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        RULE_OM_UT_TEST(RULE_OM_TEST_MergerMacAce_IPv6_Standard);
        RULE_OM_UT_TEST(RULE_OM_TEST_MergerMacAce_IPv6_Extended);
#endif // #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)

        RULE_OM_UT_TEST(RULE_OM_UT_Get_Port_Acl_By_Type);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Port_Acl_By_Precedence);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Port_Acl_By_Precedence_When_Apply_Partial_Acl);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Port_Acl_By_Direction_And_Precedence);

        RULE_OM_UT_TEST(RULE_OM_TEST_GetNextAclByPort);
        RULE_OM_UT_TEST(RULE_OM_TEST_GetNextUIAclByPort);
        RULE_OM_UT_TEST(RULE_OM_TEST_Get_Next_UIAcl_By_Port_And_Binding_Type);
        RULE_OM_UT_TEST(RULE_OM_TEST_Get_UI_Port_Entry);
        RULE_OM_UT_TEST(RULE_OM_TEST_Get_Next_UI_Port_Entry);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Next_Mib_Ace_ByPort);

        RULE_OM_UT_TEST(RULE_OM_UT_PolicyMapPtrToIndex);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Next_PolicyMap);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Next_PolicyMap_By_ClassMap_Index);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Next_PolicyMap_By_ClassMap_Action_Meter_Index);
    }
    else
    {
        printf("%s (%d): Skip all rule_om test case!!\r\n", __FUNCTION__, __LINE__);
    }
    return 0;

#undef RULE_OM_UT_TEST
}


