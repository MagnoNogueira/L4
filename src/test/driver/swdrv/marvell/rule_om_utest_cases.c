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
RULE_OM_UT_Proc_SetAceIpProtocol(
    RULE_TYPE_Ace_Entry_T *ace,
    UI8_T protocol)
{
    switch (ace->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            assert(0);
            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            ace->u.ip.aceProtocol = protocol;
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            ace->u.ipv6.aceNextHeader = protocol;
            break;

        default:
            assert(0);
            break;
    }
}

void
RULE_OM_UT_Proc_SetAceIpDscp(
    RULE_TYPE_Ace_Entry_T *ace,
    UI8_T dscp)
{
    switch (ace->ace_type)
    {
        case RULE_TYPE_MAC_ACL:
            assert(0);
            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            ace->u.ip.aceDscp = dscp;
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            ace->u.ipv6.aceDscp = dscp;
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

    l_inet_result = L_INET_StringToInaddr(L_INET_FORMAT_IPV6_ADDR_RAW,
                                          ipv6_addr,
                                          (L_INET_Addr_T *)&inet,
                                          sizeof(inet));
    assert(l_inet_result == L_INET_RETURN_SUCCESS);

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
            RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_SRC_IPV6_PREFIX_LEN,
                                                ace->u.ipv6.aceSourceIpAddrBitmask);
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

    assert(l_inet_result == L_INET_RETURN_SUCCESS);

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
            RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN,
                                                ace->u.ipv6.aceDestIpAddrBitmask);
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

#pragma mark - Test function
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
RULE_OM_UT_Validate_Ifindex()
{
    UI32_T ifindex;

    ifindex = RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER;

    assert(0 == RULE_TYPE_IS_UPORT(ifindex));
    assert(0 == RULE_TYPE_IS_TRUNK(ifindex));
    assert(0 == RULE_TYPE_IS_CPU_PORT(ifindex));
    assert(0 != RULE_TYPE_IS_ALL_PORTS(ifindex));

    assert(RULE_TYPE_INTERFACE_ALL_UNIT == RULE_TYPE_IFINDEX_TO_INTERFACE_TYPE(ifindex));

    assert(RULE_TYPE_OK == RULE_TYPE_VALIDATE_IFINDEX(ifindex));

    for (ifindex = 1;
         ifindex <= (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK * SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT);
         ++ ifindex)
    {
        assert(0 != RULE_TYPE_IS_UPORT(ifindex));
        assert(0 == RULE_TYPE_IS_TRUNK(ifindex));
        assert(0 == RULE_TYPE_IS_CPU_PORT(ifindex));
        assert(0 == RULE_TYPE_IS_ALL_PORTS(ifindex));

        assert(RULE_TYPE_INTERFACE_UPORT == RULE_TYPE_IFINDEX_TO_INTERFACE_TYPE(ifindex));

        assert(RULE_TYPE_OK == RULE_TYPE_VALIDATE_IFINDEX(ifindex));
    }

    for (ifindex = SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER;
         ifindex <= (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER + SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM - 1);
         ++ ifindex)
    {
        assert(0 == RULE_TYPE_IS_UPORT(ifindex));
        assert(0 != RULE_TYPE_IS_TRUNK(ifindex));
        assert(0 == RULE_TYPE_IS_CPU_PORT(ifindex));
        assert(0 == RULE_TYPE_IS_ALL_PORTS(ifindex));

        assert(RULE_TYPE_INTERFACE_TRUNK == RULE_TYPE_IFINDEX_TO_INTERFACE_TYPE(ifindex));

        assert(RULE_TYPE_OK == RULE_TYPE_VALIDATE_IFINDEX(ifindex));
    }

    ifindex = SYS_ADPT_CPU_1_IF_INDEX_NUMBER;

    assert(0 == RULE_TYPE_IS_UPORT(ifindex));
    assert(0 == RULE_TYPE_IS_TRUNK(ifindex));
    assert(0 != RULE_TYPE_IS_CPU_PORT(ifindex));
    assert(0 == RULE_TYPE_IS_ALL_PORTS(ifindex));

    assert(RULE_TYPE_INTERFACE_CPU_PORT == RULE_TYPE_IFINDEX_TO_INTERFACE_TYPE(ifindex));

    assert(RULE_TYPE_OK == RULE_TYPE_VALIDATE_IFINDEX(ifindex));

    {
        UI32_T ar_index;

        for (ar_index = 0; ar_index < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ ar_index)
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(ar_index);

            assert(RULE_TYPE_OK == RULE_TYPE_VALIDATE_IFINDEX(ifindex));
        }
    }

    return 0;
}

int
RULE_OM_UT_Ifindex_To_Array_Index()
{
    UI32_T ar_index;

    for (ar_index = 0; ar_index < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ ar_index)
    {
        UI32_T ifindex;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(ar_index);

        if (1 <= ifindex && ifindex <= SYS_ADPT_TOTAL_NBR_OF_LPORT)
        {
            assert(ar_index == (ifindex - 1));
        }
        else if (SYS_ADPT_CPU_1_IF_INDEX_NUMBER == ifindex)
        {
            assert(ar_index == SYS_ADPT_TOTAL_NBR_OF_LPORT);
        }

        {
            UI32_T temp_ar_index;

            temp_ar_index = RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex);

            assert(ar_index == temp_ar_index);
        }
    }

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
        MAX_ACL      = 10,
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
RULE_OM_UT_Convert_2_Mib_Acl()
{
    enum
    {
        MAX_ACL  = 5,
        MAX_ACE  = SYS_ADPT_MAX_NBRS_OF_ACE,
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
                case RULE_TYPE_IPV6_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
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
        RULE_TYPE_AclType_T ace_type;

        UI32_T acl_index = acl_list[i].acl_index;
        UI32_T ace_index;
        UI32_T ace_count;

        RULE_TYPE_MIB_Acl_T result_mib_acl;
        RULE_TYPE_Ace_Entry_T ace_entry;

        result = RULE_OM_GetMibAclByIndex(acl_index, &result_mib_acl);
        assert(RULE_TYPE_OK == result);

        for (ace_index = 0, ace_count = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_list[i].acl_index, &ace_type, &ace_index, &ace_entry);
             ++ace_count)
        {
#define MAX_BYTES_PER_ACE_INDEX 2

            UI16_T result_ace_index;

            assert(ace_count < _countof(acl_list[i].ace_index_list));

            memcpy(&result_ace_index, &result_mib_acl.ace_index_list[ ace_count * MAX_BYTES_PER_ACE_INDEX], sizeof(result_ace_index));

            result_ace_index = L_STDLIB_Hton16(result_ace_index);

            assert(acl_list[i].ace_index_list[ ace_count ] == result_ace_index);

#undef MAX_BYTES_PER_ACE_INDEX
        }

        assert(ace_count == _countof(acl_list[i].ace_index_list));
    }

    return 0;
}

int
RULE_OM_UT_Get_Last_ACE()
{
    enum
    {
        MAX_ACL  = 5,
        MAX_ACE  = 30,
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
                case RULE_TYPE_IPV6_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, src_ipv6_addr);
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
        RULE_TYPE_AclType_T ace_type;

        UI32_T acl_index = acl_list[i].acl_index;
        UI32_T result_ace_index;

        RULE_TYPE_Ace_Entry_T result_ace_entry;

        result = RULE_OM_GetLastAceByAcl(acl_index, &ace_type, &result_ace_index, &result_ace_entry);
        assert(RULE_TYPE_OK == result);

        assert(result_ace_index == acl_list[i].ace_index_list[ _countof(acl_list[i].ace_index_list)-1 ]);
    }

    return 0;
}

#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
int
RULE_OM_UT_Conver_Ace_Between_OM_UI_With_Mix_MAC_Ace()
{
    typedef struct
    {
        BOOL_T mac;
        BOOL_T ipv4;
        BOOL_T ipv6;
    } Ace_Entry_Pattern_T;

    Ace_Entry_Pattern_T ace_ar[] =
    {
        {TRUE,  FALSE, FALSE},        // mac
        {TRUE,  TRUE,  FALSE},        // mac + ipv4
        {TRUE,  FALSE, TRUE},         // mac + ipv6
        {FALSE, TRUE,  FALSE},        // ipv4
        {FALSE, FALSE, TRUE}          // ipv6
    };

    UI32_T i;

    for (i = 0; i < _countof(ace_ar); i++)
    {
        RULE_TYPE_Ace_Entry_T ace;
        RULE_TYPE_RETURN_TYPE_T result;

        ace.ace_type = RULE_TYPE_MAC_ACL;
        RULE_OM_InitAce(&ace);

        if (TRUE == ace_ar[i].mac)
        {
            ace.access = RULE_TYPE_ACE_DENY;
            ace.u.mac.acePktformat = VAL_diffServMacAcePktformat_untagged_Eth2;
            ace.u.mac.aceSourceMacAddr[0] = 1;
            ace.u.mac.aceSourceMacAddrBitmask[0] = 1;
            ace.u.mac.aceDestMacAddr[0] = 1;
            ace.u.mac.aceDestMacAddrBitmask[0] = 1;
            ace.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;
            ace.u.mac.aceMinVid = 12;
            ace.u.mac.aceVidBitmask = MAX_diffServMacAceVidBitmask;
            ace.u.mac.aceCosOp = VAL_diffServMacAceSourcePortOp_equal;
            ace.u.mac.aceMinCos = 2;

            ace.u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_equal;
            ace.u.mac.aceEtherTypeBitmask = 0xfff0;
            ace.u.mac.aceMinEtherType = 0x0800;
        }

        if (TRUE == ace_ar[i].ipv4)
        {
            ace.u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_equal;
            ace.u.mac.aceEtherTypeBitmask = MAX_diffServMacAceEtherTypeBitmask;
            ace.u.mac.aceMinEtherType = RULE_TYPE_ETHERTYPE_IP;

            RULE_OM_SetAcePatternToDefaultByType(&ace, RULE_TYPE_IP_ACL);

            ace.u.ip.aceSourceIpAddr = 0x12345678;
            ace.u.ip.aceSourceIpAddrBitmask = 0xffffffff;
            ace.u.ip.aceDestIpAddr = 0x90abcdef;
            ace.u.ip.aceDestIpAddrBitmask = 0xffffffff;
            ace.u.ip.aceDscp = 123;

            ace.u.ip.aceProtocol = 17;
            ace.u.ip.aceControlCode = 1;
            ace.u.ip.aceControlCodeBitmask = 64;

            ace.u.ip.aceSourcePortOp = VAL_diffServIpAceSourcePortOp_equal;
            ace.u.ip.aceMinSourcePort = 1;
            ace.u.ip.aceSourcePortBitmask = MAX_diffServIpAceSourcePortBitmask;

            ace.u.ip.aceDestPortOp = VAL_diffServIpAceDestPortOp_equal;
            ace.u.ip.aceMinDestPort = 2;
            ace.u.ip.aceDestPortBitmask = MAX_diffServIpAceDestPortBitmask;
        }

        if (TRUE == ace_ar[i].ipv6)
        {
            ace.u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_equal;
            ace.u.mac.aceEtherTypeBitmask = MAX_diffServMacAceEtherTypeBitmask;
            ace.u.mac.aceMinEtherType = RULE_TYPE_ETHERTYPE_IPV6;

            RULE_OM_SetAcePatternToDefaultByType(&ace, RULE_TYPE_IPV6_ACL);

            ace.u.ipv6.aceSourceIpAddr[0] = 0x12;
            ace.u.ipv6.aceSourceIpAddr[1] = 0x34;
            ace.u.ipv6.aceSourceIpAddr[2] = 0x56;
            ace.u.ipv6.aceSourceIpAddr[3] = 0x78;
            ace.u.ipv6.aceSourceIpAddr[4] = 0x99;
            ace.u.ipv6.aceSourceIpAddr[5] = 0x3;

            memset(ace.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_TYPE_IPV6_ADDR_LEN);

            ace.u.ipv6.aceDestIpAddr[0] = 0xff;
            ace.u.ipv6.aceDestIpAddr[1] = 0xff;
            ace.u.ipv6.aceDestIpAddr[2] = 0xff;
            ace.u.ipv6.aceDestIpAddr[3] = 0xff;
            ace.u.ipv6.aceDestIpAddr[4] = 0xff;
            ace.u.ipv6.aceDestIpAddr[5] = 0x8;

            memset(ace.u.ipv6.aceDestIpAddrBitmask, 0xff, SYS_TYPE_IPV6_ADDR_LEN);

            ace.u.ipv6.aceNextHeader = 17;
            ace.u.ipv6.aceFlowLabel = 1;
            ace.u.ipv6.aceDscp = 12;

            ace.u.ipv6.aceSourcePortOp = VAL_diffServIpAceSourcePortOp_equal;
            ace.u.ipv6.aceMinSourcePort = 1;
            ace.u.ipv6.aceSourcePortBitmask = MAX_diffServIpAceSourcePortBitmask;

            ace.u.ipv6.aceDestPortOp = VAL_diffServIpAceDestPortOp_equal;
            ace.u.ipv6.aceMinDestPort = 2;
            ace.u.ipv6.aceDestPortBitmask = MAX_diffServIpAceDestPortBitmask;
        }

        {
            RULE_TYPE_Ace_Entry_T tmp_ace;
            RULE_TYPE_UI_Ace_Entry_T ui_ace;

            result = RULE_OM_ConvertAce2UIAce(&ace, &ui_ace);
            assert(RULE_TYPE_OK == result);

            assert(ui_ace.ace_type == ace.ace_type);

            result = RULE_OM_ConvertUIAce2Ace(&ui_ace, &tmp_ace);
            assert(RULE_TYPE_OK == result);

            assert(0 == memcmp(&ace, &tmp_ace, sizeof(ace)));
//            assert(ace.ace_type == tmp_ace.ace_type);
//            assert(0 == memcmp(&ace.u.mac, &tmp_ace.u.mac, sizeof(ace.u.mac)));
//            assert(0 == memcmp(&ace.u.ip, &tmp_ace.u.ip, sizeof(ace.u.ip)));
//            assert(0 == memcmp(&ace.u.ipv6, &tmp_ace.u.ipv6, sizeof(ace.u.ipv6)));
        }
    }
    return 0;
}

int
RULE_OM_UT_Convert_Ipv6_PrefixLen_2_Mask_On_Add_Mace_Ace()
{
    char *acl_name = "M1";

    UI32_T acl_index;
    UI32_T ace_index;
    RULE_TYPE_RETURN_TYPE_T res;

    RULE_TYPE_AclType_T ace_typ;
    RULE_TYPE_Ace_Entry_T ace_entry;
    RULE_TYPE_Ace_Entry_T tmp_ace_entry;
    RULE_TYPE_UI_Ace_Entry_T ui_ace;

    res = RULE_OM_CreateAcl(acl_name, RULE_TYPE_MAC_ACL);
    assert(RULE_TYPE_OK == res);

    res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == res);

    memset(&ace_entry, 0, sizeof(ace_entry));
    ace_entry.ace_type = RULE_TYPE_MAC_ACL;
    RULE_OM_InitAce(&ace_entry);

    ace_entry.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;
    ace_entry.u.mac.aceMinEtherType = RULE_TYPE_ETHERTYPE_IPV6;
    ace_entry.u.mac.aceMaxEtherType = RULE_TYPE_ETHERTYPE_IPV6;
    ace_entry.u.mac.aceEtherTypeBitmask = 0xffff;
    RULE_OM_SetAcePatternToDefaultByType(&ace_entry, RULE_TYPE_IPV6_EXT_ACL);

    ace_entry.u.ipv6.aceSourceIpAddr[0] = 1;
    /* prefix_len = 120
     */
    memset(ace_entry.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_TYPE_IPV6_ADDR_LEN);
    ace_entry.u.ipv6.aceSourceIpAddrBitmask[15] = 0;

    ace_entry.u.ipv6.aceDestIpAddr[0] = 2;
    /* prefix_len = 8
     */
    ace_entry.u.ipv6.aceDestIpAddrBitmask[0] = 0xff;

    res = RULE_OM_ConvertAce2UIAce(&ace_entry, &ui_ace);
    assert(RULE_TYPE_OK == res);

    res = RULE_OM_ConvertUIAce2Ace(&ui_ace, &tmp_ace_entry);
    assert(RULE_TYPE_OK == res);

    res = RULE_OM_AddAce2Acl(acl_index, RULE_TYPE_MAC_ACL, &tmp_ace_entry);
    assert(RULE_TYPE_OK == res);

    ace_index = 0;
    res = RULE_OM_GetNextAceByAcl(acl_index, &ace_typ, &ace_index, &tmp_ace_entry);
    assert(RULE_TYPE_OK == res);

    ASSERT(0 == memcmp(&ace_entry.u.mac, &tmp_ace_entry.u.mac, sizeof(ace_entry.u.mac)));
    ASSERT(0 == memcmp(&ace_entry.u.ip, &tmp_ace_entry.u.ip, sizeof(ace_entry.u.ip)));
    ASSERT(0 == memcmp(&ace_entry.u.ipv6, &tmp_ace_entry.u.ipv6, sizeof(ace_entry.u.ipv6)));

    return 0;
}

#endif /* SYS_CPNT_ACL_MAC_IP_MIX */

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

static int
RULE_OM_TEST_Validate_Ace()
{
    RULE_TYPE_Ace_Entry_T ace;
    UI32_T res;

    /* SET IP EXT ACL
     */
    ace.ace_type = RULE_TYPE_IP_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ip.aceTos = 1;

    res = RULE_OM_LocalValidateAce(&ace);
#if (SYS_CPNT_ACL_IP_EXT_TOS == TRUE)
    assert(res == RULE_TYPE_OK);
#else
    assert(res != RULE_TYPE_OK);
#endif // SYS_CPNT_ACL_IP_EXT_TOS

    /* SET IPV6 STD ACL
     */
    ace.ace_type = RULE_TYPE_IPV6_STD_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceSourceIpAddr[0] = 0xfe;
    memset(ace.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_ADPT_IPV6_ADDR_LEN);
    //ace.u.ipv6.aceSourceIpAddrPrefixLen = 128;

    res = RULE_OM_LocalValidateAce(&ace);
    assert(res == RULE_TYPE_OK);

    /* IPV6 Standard should not support the following field
     */
    ace.ace_type = RULE_TYPE_IPV6_STD_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceDscp = 1;
    ace.u.ipv6.aceFlowLabel = 1;
    ace.u.ipv6.aceNextHeader = 1;

    res = RULE_OM_LocalValidateAce(&ace);
    assert(res != RULE_TYPE_OK);

    /* any
     */
    ace.ace_type = RULE_TYPE_IPV6_STD_ACL;
    RULE_OM_InitAce(&ace);

    res = RULE_OM_LocalValidateAce(&ace);
    assert(res == RULE_TYPE_OK);

    /* SET IPV6 EXT ACL
     */
    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceSourceIpAddr[0] = 0xfe;
    memset(ace.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_ADPT_IPV6_ADDR_LEN);
    //ace.u.ipv6.aceSourceIpAddrPrefixLen = RULE_TYPE_MAX_OF_SRC_IPV6_PREFIX_LEN;

    res = RULE_OM_LocalValidateAce(&ace);
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
    assert(res == RULE_TYPE_OK);
#else
    assert(res != RULE_TYPE_OK);
#endif // SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR

    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceDestIpAddr[0] = 0xfe;
    RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN,
                                        ace.u.ipv6.aceDestIpAddrBitmask);
    //ace.u.ipv6.aceDestIpAddrPrefixLen = RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN;

    res = RULE_OM_LocalValidateAce(&ace);
#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
    assert(res == RULE_TYPE_OK);
#else
    assert(res != RULE_TYPE_OK);
#endif // SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR

    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceDscp = 1;

    res = RULE_OM_LocalValidateAce(&ace);
#if (SYS_CPNT_ACL_IPV6_EXT_TRAFFIC_CLASS == TRUE)
    assert(res == RULE_TYPE_OK);
#else
    assert(res != RULE_TYPE_OK);
#endif // SYS_CPNT_ACL_IPV6_EXT_TRAFFIC_CLASS

    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceFlowLabel = 1;

    res = RULE_OM_LocalValidateAce(&ace);
#if (SYS_CPNT_ACL_IPV6_EXT_FLOW_LABEL == TRUE)
    assert(res == RULE_TYPE_OK);
#else
    assert(res != RULE_TYPE_OK);
#endif // SYS_CPNT_ACL_IPV6_EXT_FLOW_LABEL

    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    ace.u.ipv6.aceNextHeader = 1;

    res = RULE_OM_LocalValidateAce(&ace);
#if (SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER == TRUE)
    assert(res == RULE_TYPE_OK);
#else
    assert(res != RULE_TYPE_OK);
#endif // SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER

    /* any any
     */
    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    RULE_OM_InitAce(&ace);

    res = RULE_OM_LocalValidateAce(&ace);
    assert(res == RULE_TYPE_OK);

    return 0;
}

static int
RULE_OM_TEST_Validate_Mac_Plus_Ipv6_Ace()
{
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
#define INIT_MIX_IPV6_ACE(ace_entry)                                            \
    {                                                                           \
        memset(&ace_entry, 0, sizeof(ace_entry));                               \
        ace_entry.ace_type = RULE_TYPE_MAC_ACL;                                 \
        RULE_OM_InitAce(&ace_entry);                                            \
        ace_entry.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;  \
        ace_entry.u.mac.aceMinEtherType = RULE_TYPE_ETHERTYPE_IPV6;             \
        ace_entry.u.mac.aceMaxEtherType = RULE_TYPE_ETHERTYPE_IPV6;             \
        ace_entry.u.mac.aceEtherTypeBitmask = 0xffff;                           \
        RULE_OM_SetAcePatternToDefaultByType(&ace_entry, RULE_TYPE_IPV6_EXT_ACL); \
    }

    typedef struct
    {
        UI32_T src_prefix_len;
        UI32_T dst_prefix_len;
        UI32_T protocol;
        UI32_T l4_src_port;
        UI32_T l4_src_mask;
        UI32_T l4_dst_port;
        UI32_T l4_dst_mask;
    } MAC_ACE_IPV6_T;

    MAC_ACE_IPV6_T success_case[] =
    {
        {
            MIN_diffServIpv6AceSourceIpAddrPrefixLen,
            MIN_diffServIpv6AceDestIpAddrPrefixLen,
            MIN_diffServIpv6AceNextHeader,
            MIN_diffServMacAceL4SourcePort,
            MIN_diffServMacAceL4SourcePortBitmask,
            MIN_diffServMacAceL4DestPort,
            MIN_diffServMacAceL4DestPortBitmask
        },
        {
            MAX_diffServIpv6AceSourceIpAddrPrefixLen,
            SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN,
            (MAX_diffServIpv6AceNextHeader - 1),
            MAX_diffServMacAceL4SourcePort,
            MAX_diffServMacAceL4SourcePortBitmask,
            MAX_diffServMacAceL4DestPort,
            MAX_diffServMacAceL4DestPortBitmask
        },
    };

    RULE_TYPE_Ace_Entry_T ace_entry;
    RULE_TYPE_RETURN_TYPE_T result;
    char *acl_name = "m1";
    UI32_T acl_index;
    UI8_T  mac_addr[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T  mac_mask[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T  ipv6_src_addr[SYS_ADPT_IPV6_ADDR_LEN];
    UI8_T  ipv6_dst_addr[SYS_ADPT_IPV6_ADDR_LEN];
    UI32_T i;

    result = RULE_OM_CreateAcl(acl_name, RULE_TYPE_MAC_ACL);
    assert(result == RULE_TYPE_OK);

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(result == RULE_TYPE_OK);

    for (i = 0; i < _countof(success_case); i++)
    {
        MAC_ACE_IPV6_T *input_p = &success_case[i];

        INIT_MIX_IPV6_ACE(ace_entry);

        memset(&mac_addr, 0, sizeof(mac_addr));
        memset(&mac_mask, 0xff, sizeof(mac_mask));

        memset(&ipv6_src_addr, 0, sizeof(ipv6_src_addr));
        memset(&ipv6_dst_addr, 0, sizeof(ipv6_dst_addr));

        mac_addr[sizeof(mac_addr) - 1] = i;
        ipv6_src_addr[sizeof(ipv6_src_addr) - 1] = i;
        ipv6_dst_addr[sizeof(ipv6_dst_addr) - 1] = i;

        memcpy(ace_entry.u.ipv6.aceSourceIpAddr, ipv6_src_addr, SYS_ADPT_IPV6_ADDR_LEN);
        RULE_OM_LocalConvertIpv6Prefix2Mask(input_p->src_prefix_len,
                                            ace_entry.u.ipv6.aceSourceIpAddrBitmask);

        memcpy(ace_entry.u.ipv6.aceDestIpAddr, ipv6_dst_addr, SYS_ADPT_IPV6_ADDR_LEN);
        RULE_OM_LocalConvertIpv6Prefix2Mask(input_p->dst_prefix_len,
                                            ace_entry.u.ipv6.aceDestIpAddrBitmask);

        ace_entry.u.ipv6.aceNextHeader = input_p->protocol;

        ace_entry.u.ipv6.aceSourcePortOp = VAL_diffServMacAceSourcePortOp_equal;
        ace_entry.u.ipv6.aceMinSourcePort = ace_entry.u.ipv6.aceMaxSourcePort = input_p->l4_src_port;
        ace_entry.u.ipv6.aceSourcePortBitmask = input_p->l4_src_mask;

        ace_entry.u.ipv6.aceDestPortOp = VAL_diffServMacAceDestPortOp_equal;
        ace_entry.u.ipv6.aceMinDestPort = ace_entry.u.ipv6.aceMaxDestPort = input_p->l4_dst_port;
        ace_entry.u.ipv6.aceDestPortBitmask = input_p->l4_dst_mask;

        RULE_OM_SetDebugFlag(7);
        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        RULE_OM_SetDebugFlag(0);
        assert(result == RULE_TYPE_OK);
    }

#undef INIT_MIX_IPV6_ACE
#endif /* SYS_CPNT_ACL_MAC_IP_MIX == TRUE */
    return 0;
}

static int
RULE_OM_TEST_Validate_Mac_Plus_Ipv6_Ace_Fail()
{
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
#define INIT_MIX_IPV6_ACE(ace_entry)                                            \
    {                                                                           \
        memset(&ace_entry, 0, sizeof(ace_entry));                               \
        ace_entry.ace_type = RULE_TYPE_MAC_ACL;                                 \
        RULE_OM_InitAce(&ace_entry);                                            \
        ace_entry.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;  \
        ace_entry.u.mac.aceMinEtherType = RULE_TYPE_ETHERTYPE_IPV6;             \
        ace_entry.u.mac.aceMaxEtherType = RULE_TYPE_ETHERTYPE_IPV6;             \
        ace_entry.u.mac.aceEtherTypeBitmask = 0xffff;                           \
        RULE_OM_SetAcePatternToDefaultByType(&ace_entry, RULE_TYPE_IPV6_EXT_ACL); \
    }

    typedef struct
    {
        UI32_T src_prefix_len;
        UI32_T dst_prefix_len;
        UI32_T protocol;
        UI32_T l4_src_port;
        UI32_T l4_src_mask;
        UI32_T l4_dst_port;
        UI32_T l4_dst_mask;
    } MAC_ACE_IPV6_T;

    MAC_ACE_IPV6_T fail_case[] =
    {
        {
            MAX_diffServIpv6AceSourceIpAddrPrefixLen,
            SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN,
            MAX_diffServIpv6AceNextHeader + 1,
            MAX_diffServMacAceL4SourcePort,
            MAX_diffServMacAceL4SourcePortBitmask,
            MAX_diffServMacAceL4DestPort,
            MAX_diffServMacAceL4DestPortBitmask
        },
        {
            MAX_diffServIpv6AceSourceIpAddrPrefixLen,
            SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN + 1,
            MAX_diffServIpv6AceNextHeader,
            MAX_diffServMacAceL4SourcePort,
            MAX_diffServMacAceL4SourcePortBitmask,
            MAX_diffServMacAceL4DestPort,
            MAX_diffServMacAceL4DestPortBitmask
        },
    };

    RULE_TYPE_Ace_Entry_T ace_entry;
    RULE_TYPE_RETURN_TYPE_T result;
    char *acl_name = "m1";
    UI32_T acl_index;
    UI8_T  mac_addr[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T  mac_mask[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T  ipv6_src_addr[SYS_ADPT_IPV6_ADDR_LEN];
    UI8_T  ipv6_dst_addr[SYS_ADPT_IPV6_ADDR_LEN];
    UI32_T i;

    result = RULE_OM_CreateAcl(acl_name, RULE_TYPE_MAC_ACL);
    assert(result == RULE_TYPE_OK);

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(result == RULE_TYPE_OK);

    for (i = 0; i < _countof(fail_case); i++)
    {
        MAC_ACE_IPV6_T *input_p = &fail_case[i];

        INIT_MIX_IPV6_ACE(ace_entry);

        memset(&mac_addr, 0, sizeof(mac_addr));
        memset(&mac_mask, 0xff, sizeof(mac_mask));

        memset(&ipv6_src_addr, 0, sizeof(ipv6_src_addr));
        memset(&ipv6_dst_addr, 0, sizeof(ipv6_dst_addr));

        mac_addr[sizeof(mac_addr) - 1] = i;
        ipv6_src_addr[sizeof(ipv6_src_addr) - 1] = i;
        ipv6_dst_addr[sizeof(ipv6_dst_addr) - 1] = i;

        memcpy(ace_entry.u.ipv6.aceSourceIpAddr, ipv6_src_addr, SYS_ADPT_IPV6_ADDR_LEN);
        RULE_OM_LocalConvertIpv6Prefix2Mask(input_p->src_prefix_len,
                                            ace_entry.u.ipv6.aceSourceIpAddrBitmask);

        memcpy(ace_entry.u.ipv6.aceDestIpAddr, ipv6_dst_addr, SYS_ADPT_IPV6_ADDR_LEN);
        RULE_OM_LocalConvertIpv6Prefix2Mask(input_p->dst_prefix_len,
                                            ace_entry.u.ipv6.aceDestIpAddrBitmask);

        ace_entry.u.ipv6.aceNextHeader = input_p->protocol;

        ace_entry.u.ipv6.aceSourcePortOp = VAL_diffServMacAceSourcePortOp_equal;
        ace_entry.u.ipv6.aceMinSourcePort = ace_entry.u.ipv6.aceMaxSourcePort = input_p->l4_src_port;
        ace_entry.u.ipv6.aceSourcePortBitmask = input_p->l4_src_mask;

        ace_entry.u.ipv6.aceDestPortOp = VAL_diffServMacAceDestPortOp_equal;
        ace_entry.u.ipv6.aceMinDestPort = ace_entry.u.ipv6.aceMaxDestPort = input_p->l4_dst_port;
        ace_entry.u.ipv6.aceDestPortBitmask = input_p->l4_dst_mask;

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(result == RULE_TYPE_FAIL);
    }

#undef INIT_MIX_IPV6_ACE
#endif /* SYS_CPNT_ACL_MAC_IP_MIX == TRUE */
    return 0;
}


static int
RULE_OM_TEST_Add_ACE_2_ACL_With_Wrong_Type()
{
    const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    UI32_T      acl_index;
    UI32_T res;
    UI32_T i;

    struct
    {
        RULE_TYPE_AclType_T ace_type;
        BOOL_T              exp_res;
    } cg[] =
        {
            {RULE_TYPE_MAC_ACL,         TRUE},
            {RULE_TYPE_IP_STD_ACL,      FALSE},
            {RULE_TYPE_IP_EXT_ACL,      FALSE},
            {RULE_TYPE_IPV6_STD_ACL,    FALSE},
            {RULE_TYPE_IPV6_EXT_ACL,    FALSE},
        #if (SYS_CPNT_DAI == TRUE)
            {RULE_TYPE_ARP_ACL,         FALSE}
        #endif
        };

    res = RULE_OM_CreateAcl(acl_name, acl_type);
    assert(res == RULE_TYPE_OK);

    res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(res == RULE_TYPE_OK);

    for (i=0; i<_countof(cg); ++i)
    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_entry.ace_type = cg[i].ace_type;

        RULE_OM_InitAce(&ace_entry);

        res = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);

        assert(cg[i].exp_res == (res==RULE_TYPE_OK?TRUE:FALSE));
    }

    return UT_OK;
}

static int
RULE_OM_TEST_Add_ACE_2_ACL_With_Wrong_ACE()
{
    UI32_T res;

    {
        const char *acl_name = "a1";
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
        UI32_T acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        res = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == res);

        res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == res);

        ace.ace_type = acl_type;
        RULE_OM_InitAce(&ace);

        ace.u.mac.acePktformat = VAL_diffServMacAcePktformat_untagged802Dot3;
        ace.u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_equal;

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
        assert(res != RULE_TYPE_OK);

        ace.ace_type = acl_type;
        RULE_OM_InitAce(&ace);

        ace.u.mac.acePktformat = VAL_diffServMacAcePktformat_untagged_Eth2;
        ace.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;
        ace.u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_equal;

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
        assert(res != RULE_TYPE_OK);

        ace.ace_type = acl_type;
        RULE_OM_InitAce(&ace);

        ace.u.mac.acePktformat = VAL_diffServMacAcePktformat_tagged802Dot3;
        ace.u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_equal;

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
        assert(res != RULE_TYPE_OK);

        ace.ace_type = acl_type;
        RULE_OM_InitAce(&ace);

        ace.u.mac.aceVidOp = VAL_diffServMacAceVidOp_noOperator;
        ace.u.mac.aceVidBitmask = 0x123;

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
        assert(res != RULE_TYPE_OK);

        ace.ace_type = acl_type;
        RULE_OM_InitAce(&ace);

        ace.u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_noOperator;
        ace.u.mac.aceEtherTypeBitmask = 0x1234;

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
        assert(res != RULE_TYPE_OK);

        ace.ace_type = acl_type;
        RULE_OM_InitAce(&ace);

        ace.u.mac.aceCosOp = VAL_diffServMacAceCosOp_noOperator;
        ace.u.mac.aceMinCos = 2;
        ace.u.mac.aceMaxCos = 6;

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
        assert(res != RULE_TYPE_OK);

        ace.ace_type = acl_type;
        RULE_OM_InitAce(&ace);

        ace.u.mac.aceSourcePortOp = VAL_diffServMacAceSourcePortOp_noOperator;
        ace.u.mac.aceMinSourcePort = 2;
        ace.u.mac.aceMaxSourcePort = 200;

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
        assert(res != RULE_TYPE_OK);

        RULE_OM_DelAcl(acl_index);
    }

    {
        const char *acl_name = "a1";
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;
        UI32_T acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        res = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == res);

        res = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == res);

        ace.ace_type = acl_type;
        RULE_OM_InitAce(&ace);

        ace.u.ip.aceSourcePortOp = VAL_diffServIpAceSourcePortOp_noOperator;
        ace.u.ip.aceMinSourcePort = 2;
        ace.u.ip.aceMaxSourcePort = 65500;

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
        assert(res != RULE_TYPE_OK);

        ace.u.ip.aceDestPortOp = VAL_diffServIpAceDestPortOp_noOperator;
        ace.u.ip.aceMinDestPort = 2;
        ace.u.ip.aceMaxDestPort = 65500;

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
        assert(res != RULE_TYPE_OK);

        ace.u.ip.aceProtocol = 1;
        ace.u.ip.aceControlCodeBitmask = 0x1234;

        res = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
        assert(res != RULE_TYPE_OK);

        RULE_OM_DelAcl(acl_index);
    }

    return UT_OK;
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

#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
static int
RULE_OM_TEST_Compress_ACL_MIX_TYPE_ACE()
{
#define INIT_MIX_IP_ACE(ace_entry)                                              \
    {                                                                           \
        ace_entry.ace_type = RULE_TYPE_MAC_ACL;                                 \
        RULE_OM_InitAce(&ace_entry);                                            \
        ace_entry.u.mac.aceEtherTypeOp  = VAL_diffServMacAceEtherTypeOp_equal;  \
        ace_entry.u.mac.aceMinEtherType = 0x0800;                               \
        ace_entry.u.mac.aceMaxEtherType = 0x0800;                               \
        ace_entry.u.mac.aceEtherTypeBitmask = 0xffff;                           \
        RULE_OM_SetAcePatternToDefaultByType(&ace_entry, RULE_TYPE_IP_EXT_ACL); \
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
    INIT_MIX_IP_ACE(ace);
    ace.u.mac.aceMinVid = 1;
    ace.u.mac.aceMaxVid = 1;
    ace.u.mac.aceVidBitmask = 0x0fff;
    ace.u.mac.aceVidOp = 2;

    ace.u.ip.aceProtocol = 2;
    res = RULE_OM_AddAce2Acl(acl_index, ace.ace_type, &ace);
    assert(res == RULE_TYPE_OK);

    // group 2
    INIT_MIX_IP_ACE(ace);
    ace.u.mac.aceMinVid = 1;
    ace.u.mac.aceMaxVid = 1;
    ace.u.mac.aceVidBitmask = 0x0fff;
    ace.u.mac.aceVidOp = 2;

    ace.u.ip.aceProtocol = 17;
    ace.u.ip.aceMinDestPort =1234;
    ace.u.ip.aceMaxDestPort=1234;
    ace.u.ip.aceDestPortOp =VAL_aclIpAceDestPortOp_equal;
    ace.u.ip.aceDestPortBitmask = 0xffff;
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

        assert(ace_p->u.mac.aceMinEtherType == 0x0800 &&
               ace_p->u.mac.aceEtherTypeBitmask == 0x0000ffff);
        assert(ace_p->u.ip.aceProtocol == 2);

        res = RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p);
        assert(RULE_TYPE_OK == res);

        assert(ace_p->u.mac.aceMinEtherType == 0x0800 &&
               ace_p->u.mac.aceEtherTypeBitmask == 0x0000ffff);
        assert(ace.u.ip.aceMinDestPort == 1234 &&
               ace.u.ip.aceDestPortOp == VAL_aclIpAceDestPortOp_equal &&
               ace.u.ip.aceDestPortBitmask == 0xffff);

    }

    return UT_OK;
#undef INIT_MIX_IP_ACE
}

#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
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
        for (j=0; j < /*RULE_OM_LocalGetMaxNumberOfACEPerACL(eg[i].acl_type)*/SYS_ADPT_MAX_NBRS_OF_ACE + 1; ++j)
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
                    memset(ace.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_ADPT_IPV6_ADDR_LEN);
                    //ace.u.ipv6.aceSourceIpAddrPrefixLen = RULE_TYPE_MAX_OF_SRC_IPV6_PREFIX_LEN;
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    ace.u.ipv6.aceSourceIpAddr[0] = (UI8_T)j;
                    ace.u.ipv6.aceSourceIpAddr[1] = (UI8_T)(j / 255);
                    memset(ace.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_ADPT_IPV6_ADDR_LEN);
                    //ace.u.ipv6.aceSourceIpAddrPrefixLen = RULE_TYPE_MAX_OF_SRC_IPV6_PREFIX_LEN;
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    ace.u.ipv6.aceDestIpAddr[0] = (UI8_T)j;
                    ace.u.ipv6.aceDestIpAddr[1] = (UI8_T)(j / 255);
                    RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN,
                                                        ace.u.ipv6.aceDestIpAddrBitmask);
                    //ace.u.ipv6.aceDestIpAddrPrefixLen = RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN;
#endif
                    break;
            }

            res = RULE_OM_AddAce2Acl(eg[i].acl_index, ace.ace_type, &ace);

            // MUST success
            if (j < /*RULE_OM_LocalGetMaxNumberOfACEPerACL(eg[i].acl_type)*/ SYS_ADPT_MAX_NBRS_OF_ACE)
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

        for (i = 0; i < /*RULE_OM_LocalGetMaxNumberOfACEPerACL(acl_type)*/ SYS_ADPT_MAX_NBRS_OF_ACE; ++i)
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
RULE_OM_TEST_MergerMacAce_BMT()
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T total_count = 0;

    {
        UI32_T  i;
        const char          *acl_name   = "a1";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=1; i<=50; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("DA\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %02x-%02x-%02x-%02x-%02x-%02x %02x-%02x-%02x-%02x-%02x-%02x \r\n",
//                    i+1,
//                    ace_p->u.mac.aceDestMacAddr[0], ace_p->u.mac.aceDestMacAddr[1], ace_p->u.mac.aceDestMacAddr[2],
//                    ace_p->u.mac.aceDestMacAddr[3], ace_p->u.mac.aceDestMacAddr[4], ace_p->u.mac.aceDestMacAddr[5],
//                    ace_p->u.mac.aceDestMacAddrBitmask[0], ace_p->u.mac.aceDestMacAddrBitmask[1], ace_p->u.mac.aceDestMacAddrBitmask[2],
//                    ace_p->u.mac.aceDestMacAddrBitmask[3], ace_p->u.mac.aceDestMacAddrBitmask[4], ace_p->u.mac.aceDestMacAddrBitmask[5]);
//            }
        }
    } /* a1 */

    {
        UI32_T  i;
        const char          *acl_name   = "a2";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=51; i<=100; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceDestMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceDestMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("DA\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %02x-%02x-%02x-%02x-%02x-%02x %02x-%02x-%02x-%02x-%02x-%02x \r\n",
//                    i+1,
//                    ace_p->u.mac.aceDestMacAddr[0], ace_p->u.mac.aceDestMacAddr[1], ace_p->u.mac.aceDestMacAddr[2],
//                    ace_p->u.mac.aceDestMacAddr[3], ace_p->u.mac.aceDestMacAddr[4], ace_p->u.mac.aceDestMacAddr[5],
//                    ace_p->u.mac.aceDestMacAddrBitmask[0], ace_p->u.mac.aceDestMacAddrBitmask[1], ace_p->u.mac.aceDestMacAddrBitmask[2],
//                    ace_p->u.mac.aceDestMacAddrBitmask[3], ace_p->u.mac.aceDestMacAddrBitmask[4], ace_p->u.mac.aceDestMacAddrBitmask[5]);
//            }
        }
    } /* a2 */

    {
        UI32_T  i;
        const char          *acl_name   = "a3";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=1; i<=50; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceSourceMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceSourceMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("SA\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %02x-%02x-%02x-%02x-%02x-%02x %02x-%02x-%02x-%02x-%02x-%02x \r\n",
//                    i+1,
//                    ace_p->u.mac.aceSourceMacAddr[0], ace_p->u.mac.aceSourceMacAddr[1], ace_p->u.mac.aceSourceMacAddr[2],
//                    ace_p->u.mac.aceSourceMacAddr[3], ace_p->u.mac.aceSourceMacAddr[4], ace_p->u.mac.aceSourceMacAddr[5],
//                    ace_p->u.mac.aceSourceMacAddrBitmask[0], ace_p->u.mac.aceSourceMacAddrBitmask[1], ace_p->u.mac.aceSourceMacAddrBitmask[2],
//                    ace_p->u.mac.aceSourceMacAddrBitmask[3], ace_p->u.mac.aceSourceMacAddrBitmask[4], ace_p->u.mac.aceSourceMacAddrBitmask[5]);
//            }
        }
    } /* a3 */

    {
        UI32_T  i;
        const char          *acl_name   = "a4";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=51; i<=100; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            SET_MAC(ace.u.mac.aceSourceMacAddr, 0x00, 0x01, 0x6C, 0x02, 0x01, i);
            SET_MAC(ace.u.mac.aceSourceMacAddrBitmask, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("SA\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %02x-%02x-%02x-%02x-%02x-%02x %02x-%02x-%02x-%02x-%02x-%02x \r\n",
//                    i+1,
//                    ace_p->u.mac.aceSourceMacAddr[0], ace_p->u.mac.aceSourceMacAddr[1], ace_p->u.mac.aceSourceMacAddr[2],
//                    ace_p->u.mac.aceSourceMacAddr[3], ace_p->u.mac.aceSourceMacAddr[4], ace_p->u.mac.aceSourceMacAddr[5],
//                    ace_p->u.mac.aceSourceMacAddrBitmask[0], ace_p->u.mac.aceSourceMacAddrBitmask[1], ace_p->u.mac.aceSourceMacAddrBitmask[2],
//                    ace_p->u.mac.aceSourceMacAddrBitmask[3], ace_p->u.mac.aceSourceMacAddrBitmask[4], ace_p->u.mac.aceSourceMacAddrBitmask[5]);
//            }
        }
    }

    {
        UI32_T  i;
        const char          *acl_name   = "a5";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_MAC_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=0x8863; i<=0x886f; i++)
        {
            ace.ace_type = RULE_TYPE_MAC_ACL;
            RULE_OM_InitAce(&ace);

            ace.u.mac.aceEtherTypeOp = VAL_diffServMacAceEtherTypeOp_equal;
            ace.u.mac.aceEtherTypeBitmask = 0xffff;
            ace.u.mac.aceMinEtherType = i;
            ace.u.mac.aceMaxEtherType = i;

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("EtherType\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %08lx %08lx\r\n",
//                    i+1,
//                    ace_p->u.mac.aceMinEtherType,
//                    ace_p->u.mac.aceEtherTypeBitmask);
//            }
        }
    } /* a5 */

    {
        UI32_T  i;
        const char          *acl_name   = "a6";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=1; i<=50; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            SET_IPV4(ace.u.ip.aceDestIpAddr, 192, 168, 1, i);
            SET_IPV4(ace.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("DIP\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %d.%d.%d.%d %d.%d.%d.%d \r\n",
//                    i+1,
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[0],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[1],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[2],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[3],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[0],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[1],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[2],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[3]
//                    );
//            }
        }
    } /* a6 */

    {
        UI32_T  i;
        const char          *acl_name   = "a7";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=51; i<=100; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            SET_IPV4(ace.u.ip.aceDestIpAddr, 192, 168, 1, i);
            SET_IPV4(ace.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("DIP\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %d.%d.%d.%d %d.%d.%d.%d \r\n",
//                    i+1,
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[0],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[1],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[2],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddr)[3],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[0],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[1],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[2],
//                    ((UI8_T*)&ace_p->u.ip.aceDestIpAddrBitmask)[3]
//                    );
//            }
        }
    } /* a7 */

    {
        UI32_T  i;
        const char          *acl_name   = "a8";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=1; i<=50; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            SET_IPV4(ace.u.ip.aceSourceIpAddr, 192, 168, 1, i);
            SET_IPV4(ace.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("SIP\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %d.%d.%d.%d %d.%d.%d.%d \r\n",
//                    i+1,
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[0],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[1],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[2],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[3],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[0],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[1],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[2],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[3]
//                    );
//            }
        }
    } /* a8 */

    {
        UI32_T  i;
        const char          *acl_name   = "a9";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=51; i<=100; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            SET_IPV4(ace.u.ip.aceSourceIpAddr, 192, 168, 1, i);
            SET_IPV4(ace.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("SIP\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %d.%d.%d.%d %d.%d.%d.%d \r\n",
//                    i+1,
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[0],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[1],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[2],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[3],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[0],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[1],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[2],
//                    ((UI8_T*)&ace_p->u.ip.aceSourceIpAddr)[3]
//                    );
//            }
        }
    } /* a9 */

    {
        UI32_T  i;
        const char          *acl_name   = "a10";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=5001; i<=5050; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            ace.u.ip.aceDestPortOp = VAL_diffServIpAceDestPortOp_equal;
            ace.u.ip.aceMinDestPort = i;
            ace.u.ip.aceDestPortBitmask = 0xffff;

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("L4DstPort\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %lu %lu \r\n",
//                    i+1,
//                    ace_p->u.ip.aceMinDestPort,
//                    ace_p->u.ip.aceDestPortBitmask
//                    );
//            }
        }
    } /* a10 */

    {
        UI32_T  i;
        const char          *acl_name   = "a11";
        RULE_TYPE_AclType_T acl_type    = RULE_TYPE_IP_EXT_ACL;
        RULE_TYPE_AclType_T ace_type    = RULE_TYPE_IP_EXT_ACL;
        UI32_T              acl_index;
        RULE_TYPE_Ace_Entry_T ace;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        for (i=6001; i<=6050; i++)
        {
            ace.ace_type = ace_type;
            RULE_OM_InitAce(&ace);

            ace.u.ip.aceSourcePortOp = VAL_diffServIpAceDestPortOp_equal;
            ace.u.ip.aceMinSourcePort = i;
            ace.u.ip.aceSourcePortBitmask = 0xffff;

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("L4SourcePort\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%lu] %lu %lu \r\n",
//                    i+1,
//                    ace_p->u.ip.aceMinSourcePort,
//                    ace_p->u.ip.aceSourcePortBitmask
//                    );
//            }
        }
    } /* a11 */

//    printf("total count = %lu \r\n", total_count);

    return UT_OK;
}

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

        for (i=1; i<= 3*16; i++)
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

            memset(ace.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_ADPT_IPV6_ADDR_LEN);
            //ace.u.ipv6.aceSourceIpAddrPrefixLen = 128;

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("SIP6\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
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
//            }
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

            memset(ace.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_ADPT_IPV6_ADDR_LEN);
            //ace.u.ipv6.aceSourceIpAddrPrefixLen = 128;
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            memset(ace.u.ipv6.aceDestIpAddr,0, 16);

            ace.u.ipv6.aceDestIpAddr[0] = 0x3f;
            ace.u.ipv6.aceDestIpAddr[1] = 0xfe;
            ace.u.ipv6.aceDestIpAddr[2] = 0x00;
            ace.u.ipv6.aceDestIpAddr[3] = 0x01;
            ace.u.ipv6.aceDestIpAddr[14]= 0x12;
            ace.u.ipv6.aceDestIpAddr[15]= i;

            memset(ace.u.ipv6.aceDestIpAddrBitmask, 0xff, SYS_ADPT_IPV6_ADDR_LEN);
            //ace.u.ipv6.aceDestIpAddrPrefixLen = SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN;
#endif

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("SIP6\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
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
//            }
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

        for (i=1; i<= 3*16; i++)
        {
            ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
            RULE_OM_InitAce(&ace);

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
            memset(ace.u.ipv6.aceSourceIpAddr,0, 16);

            ace.u.ipv6.aceSourceIpAddr[0] = 0x3f;
            ace.u.ipv6.aceSourceIpAddr[1] = 0xfe;
            ace.u.ipv6.aceSourceIpAddr[2] = i / 255;
            ace.u.ipv6.aceSourceIpAddr[3] = i % 255;

            RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN,
                                                ace.u.ipv6.aceSourceIpAddrBitmask);
            //ace.u.ipv6.aceSourceIpAddrPrefixLen = RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN;
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            memset(ace.u.ipv6.aceDestIpAddr,0, 16);

            ace.u.ipv6.aceDestIpAddr[0] = 0x3f;
            ace.u.ipv6.aceDestIpAddr[1] = 0xfe;
            ace.u.ipv6.aceDestIpAddr[2] = i / 255;
            ace.u.ipv6.aceDestIpAddr[3] = i % 255;

            RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN,
                                                ace.u.ipv6.aceSourceIpAddrBitmask);
            //ace.u.ipv6.aceDestIpAddrPrefixLen = RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN;
#endif

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace);
            assert(RULE_TYPE_OK == result);
        }

        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T         *acl_p;
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
//            printf("SIP6\r\n");
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
//            printf("DIP6\r\n");
#endif
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%02lu] %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n"
//                       "     %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
//                    i+1,
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
//                    ace_p->u.ipv6.aceDestIpAddrBitmask[15]);
//            }
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

        for (i=1; i<= 3*16; i++)
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
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("DSCP\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%02lu] %08lx\r\n",
//                    i+1,
//                    ace_p->u.ipv6.aceDscp);
//            }
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

        for (i=1; i<= 3*16; i++)
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
//            RULE_TYPE_Ace_Entry_T   *ace_p;
//
//            UI32_T i;
//            UI32_T ace_index;

            acl_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_p);

            acl_p = RULE_OM_LocalGetAclByIndex(acl_p->aggregate_acl_index);
            assert(NULL != acl_p);

            total_count += acl_p->ace_count;

//            printf("FlowLabel\r\n");
//            for (ace_index = 0, i = 0;
//                 RULE_TYPE_OK == RULE_OM_LocalGetNextAceByAcl(acl_p, &ace_index, &ace_p); ++i)
//            {
//                printf("[%02lu] %08lx\r\n",
//                    i+1,
//                    ace_p->u.ipv6.aceFlowLabel);
//            }
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
        for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
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
        for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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
        for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
        {
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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

    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
        RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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

    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
        RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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

    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        RULE_TYPE_InOutDirection_T direction;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
        {
            UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
            RULE_TYPE_ACL_MEMENTO_PTR_T bind_acl_p;

            for (i = 0; i < _countof(acls); ++ i)
            {
                if (acls[i].direction != direction)
                {
                    continue;
                }

                if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
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
    }

    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
        RULE_TYPE_AclMemento_T bind_acl;
        RULE_TYPE_Acl_T acl_entry;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        memset(&bind_acl, 0, sizeof(bind_acl));

        for (i = 0; i < _countof(acls); ++ i)
        {
            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

            result = RULE_OM_GetNextAclByPort(ifindex, &bind_acl.acl_index,
                                              &acl_entry, &precedence,
                                              bind_acl.time_range_name,
                                              &bind_acl.counter_enable);
            assert(RULE_TYPE_OK == result);

            // FIXME: should get index
            acls[i].bind_info.time_range_index = 0;

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

                // FIXME: should get index
                acls[i].bind_info.time_range_index = 0;

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
    }

    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        RULE_TYPE_InOutDirection_T direction;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
        {
            UI32_T precedence = RULE_TYPE_PRECEDENCE_FIRST;
            RULE_TYPE_AclMemento_T bind_acl;
            RULE_TYPE_UI_AclEntry_T acl_entry;

            memset(&bind_acl, 0, sizeof(bind_acl));

            for (i = 0; i < _countof(acls); ++ i)
            {
                if (acls[i].direction != direction)
                {
                    continue;
                }

                if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
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

                // FIXME: should get index
                acls[i].bind_info.time_range_index = 0;

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

                    // FIXME: should get index
                    acls[i].bind_info.time_range_index = 0;

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

    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
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
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        for (i = 0; i < _countof(acls); ++ i)
        {
            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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

        for (pl_idx = 0, bind_ifindex = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, acls[i].direction))
            {
                continue;
            }

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

        for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
        {
            RULE_TYPE_InOutDirection_T direction;
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
            {
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

                if(FALSE == RULE_OM_LocalIsValidIfindex(ifindex, direction))
                {
                    continue;
                }

                result = RULE_OM_SetAcl(ifindex,
                                        direction,
                                        port_entry.bind_acl[port_acl_type].acl_index,
                                        port_entry.bind_acl[port_acl_type].time_range_index,
                                        port_entry.bind_acl[port_acl_type].time_range_name,
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

        for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
        {
            for (inout_profile = MIN_DIRECTION; inout_profile <= MAX_DIRECTION; ++ inout_profile)
            {
                uport_ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

                if(FALSE == RULE_OM_LocalIsValidIfindex(uport_ifindex, inout_profile))
                {
                    continue;
                }

                port_entry.policy_map_index = policy_map_index;

                result = RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
                assert(RULE_TYPE_OK == result);

                port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
                assert(policy_map_index == port_entry_p->policy_map_index);

                //assert((pl_idx) * (MAX_DIRECTION+1) + (inout_profile+1) == policy_map_p->port_count);

                assert(RULE_OM_IS_BIT_ON(policy_map_p->qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(uport_ifindex)));
            }
        }
    }

    //
    // Verification
    //
    for (pl_idx = 0; pl_idx < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ pl_idx)
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
                if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, direction))
                    continue;

                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

                result = RULE_OM_SetAcl(ifindex,
                                        direction,
                                        port_entry.bind_acl[port_acl_type].acl_index,
                                        RULE_TYPE_UNDEF_TIME_RANGE,
                                        (UI8_T *)"",
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
            for (inout_profile = MIN_DIRECTION; inout_profile <= MAX_DIRECTION; ++ inout_profile)
            {
                UI8_T *qos_port_list;

                uport_ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

                if (FALSE == RULE_OM_LocalIsValidIfindex(uport_ifindex, inout_profile))
                    continue;

                port_entry.policy_map_index = policy_map_index;

                result = RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
                assert(RULE_TYPE_OK == result);

                port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
                assert(policy_map_index == port_entry_p->policy_map_index);

                //assert((pl_idx) * (MAX_DIRECTION+1) + (inout_profile+1) == policy_map_p->port_count);

                qos_port_list = (RULE_TYPE_INBOUND == inout_profile)
                                  ? &policy_map_p->qos_port_list[0]
                                  : &policy_map_p->egress_qos_port_list[0];
                assert(RULE_OM_IS_BIT_ON(qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(uport_ifindex)));
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
            for (direction = RULE_TYPE_INBOUND; direction <= RULE_TYPE_OUTBOUND; ++ direction)
            {
                ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

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

static int
RULE_OM_TEST_CreateClassMap()
{
    UI32_T res;
    UI32_T class_map_index = 1;
    BOOL_T active = TRUE;

    res = RULE_OM_CreateClassMap(class_map_index, active);
    assert(res == RULE_TYPE_OK);

    // class-map already exist
    res = RULE_OM_CreateClassMap(class_map_index, active);
    assert(res != RULE_TYPE_OK);

    res = RULE_OM_SetClassMapNameById(class_map_index, "2");
    assert(res == RULE_TYPE_OK);

    class_map_index = 2;

    // duplicate name
    res = RULE_OM_CreateClassMap(class_map_index, active);
    assert(res != RULE_TYPE_OK);

    // semaphore relase check
    res = RULE_OM_CreateClassMap(class_map_index, active);
    assert(res != RULE_TYPE_OK);

    return UT_OK;
}

static int
RULE_OM_TEST_Max_ClassMap_Of_System()
{
    UI32_T res;
    UI32_T i;

    for (i=0; i<SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP; i++)
    {
        UI32_T class_map_index = i+1;
        BOOL_T active = TRUE;

        res = RULE_OM_CreateClassMap(class_map_index, active);
        assert(res == RULE_TYPE_OK);
    }

    return UT_OK;
}

static int
RULE_OM_TEST_CreatePolicyMap()
{
    UI32_T res;
    UI32_T policy_map_index = 1;
    BOOL_T active = TRUE;

    res = RULE_OM_CreatePolicyMap(policy_map_index, active);
    assert(res == RULE_TYPE_OK);

    // class-map already exist
    res = RULE_OM_CreatePolicyMap(policy_map_index, active);
    assert(res != RULE_TYPE_OK);

    res = RULE_OM_SetPolicyMapNameById(policy_map_index, "2");
    assert(res == RULE_TYPE_OK);

    policy_map_index = 2;

    // duplicate name
    res = RULE_OM_CreatePolicyMap(policy_map_index, active);
    assert(res != RULE_TYPE_OK);

    // semaphore relase check
    res = RULE_OM_CreatePolicyMap(policy_map_index, active);
    assert(res != RULE_TYPE_OK);

    return UT_OK;
}

static int
RULE_OM_TEST_BindPort2PolicyMap()
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

    uport_ifindex = 2;
    inout_profile = RULE_TYPE_INBOUND;

    result = RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
    assert(RULE_TYPE_OK == result);

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
    assert(policy_map_index == port_entry_p->policy_map_index);

    assert(1 == policy_map_p->port_count);

    assert(RULE_OM_IS_BIT_ON(policy_map_p->qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(uport_ifindex)));

    result = RULE_OM_UnBindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
    assert(RULE_TYPE_OK == result);

    assert(0 == policy_map_p->port_count);

    assert(RULE_OM_IS_BIT_OFF(policy_map_p->qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(uport_ifindex)));

    /**************/

    uport_ifindex = 2;
    inout_profile = RULE_TYPE_OUTBOUND;

    result = RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
    assert(RULE_TYPE_OK == result);

    port_entry_p = RULE_OM_LocalGetPortEntryByIfIndex(uport_ifindex, inout_profile);
    assert(policy_map_index == port_entry_p->policy_map_index);

    assert(1 == policy_map_p->port_count);

    assert(RULE_OM_IS_BIT_ON(policy_map_p->egress_qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(uport_ifindex)));

    result = RULE_OM_UnBindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
    assert(RULE_TYPE_OK == result);

    assert(0 == policy_map_p->port_count);

    assert(RULE_OM_IS_BIT_OFF(policy_map_p->qos_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(uport_ifindex)));

    return 0;
}

static int
RULE_OM_TEST_GetPortPolicyMapIndex()
{
    UI32_T                      uport_ifindex;
    UI32_T                      policy_map_index;
    const char                  *policy_map_name = "p1";
    RULE_TYPE_InOutDirection_T  inout_profile;
    RULE_TYPE_RETURN_TYPE_T     result;

    result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    uport_ifindex = 2;
    inout_profile = RULE_TYPE_INBOUND;

    result = RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
    assert(RULE_TYPE_OK == result);

    {
        UI32_T old_pmap_idx;

        result = RULE_OM_GetPortPolicyMapIndex(uport_ifindex, inout_profile, &old_pmap_idx);
        assert(RULE_TYPE_OK == result);

        assert(old_pmap_idx == policy_map_index);
    }

    uport_ifindex = 3;
    inout_profile = RULE_TYPE_OUTBOUND;

    result = RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
    assert(RULE_TYPE_OK == result);

    {
        UI32_T old_pmap_idx;

        result = RULE_OM_GetPortPolicyMapIndex(uport_ifindex, inout_profile, &old_pmap_idx);
        assert(RULE_TYPE_OK == result);

        assert(old_pmap_idx == policy_map_index);
    }

    return 0;
}

static int
RULE_OM_TEST_GetNext_PortListByPolicyMap()
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
    RULE_TYPE_RETURN_TYPE_T result;

    for (i=0; i<sizeof(policy_map_tbl)/sizeof(*policy_map_tbl); i++)
    {
        struct PolicyMapCfg *cfg = &policy_map_tbl[i];
        UI32_T j;

        result = RULE_OM_Create_PolicyMap(cfg->policy_map_name, &cfg->policy_map_index);
        assert(RULE_TYPE_OK == result);

        for (j=0; j<sizeof(cfg->ingress_port_tbl)/sizeof(*cfg->ingress_port_tbl); j++)
        {
            if (0 == cfg->ingress_port_tbl[j])
            {
                continue;
            }

            result = RULE_OM_BindPort2PolicyMap(cfg->ingress_port_tbl[j],
                                                RULE_TYPE_INBOUND,
                                                cfg->policy_map_index);
            assert(RULE_TYPE_OK == result);
        }

        for (j=0; j<sizeof(cfg->egress_port_tbl)/sizeof(*cfg->egress_port_tbl); j++)
        {
            if (0 == cfg->egress_port_tbl[j])
            {
                continue;
            }

            result = RULE_OM_BindPort2PolicyMap(cfg->egress_port_tbl[j],
                                                RULE_TYPE_OUTBOUND,
                                                cfg->policy_map_index);
            assert(RULE_TYPE_OK == result);
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
                assert(cfg->ingress_port_tbl[in_index++] == uport_ifindex);
            }
            else
            {
                assert(cfg->egress_port_tbl[out_index++] == uport_ifindex);
            }
        }
    }

    return 0;
}

static int
RULE_OM_TEST_GetNext_PortListByClassMap()
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
    RULE_TYPE_RETURN_TYPE_T result;

    for (i=0; i<sizeof(class_map_cfg_tbl)/sizeof(*class_map_cfg_tbl); i++)
    {
        struct ClassMapCfg *class_cfg = &class_map_cfg_tbl[i];
        UI32_T j;

        result = RULE_OM_Create_ClassMap(class_cfg->class_map_name,
                                         RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                         &class_cfg->class_map_index);
        assert(RULE_TYPE_OK == result);

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
                result = RULE_OM_Create_PolicyMap(policy_cfg->policy_map_name, &policy_cfg->policy_map_index);
                assert(RULE_TYPE_OK == result);
            }

            result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_cfg->policy_map_index,
                                                               class_cfg->class_map_index);
            assert(RULE_TYPE_OK == result);

            for (k=0; k<sizeof(policy_cfg->ingress_port_tbl)/sizeof(*policy_cfg->ingress_port_tbl); k++)
            {
                UI32_T ifindex = policy_cfg->ingress_port_tbl[k];

                if (0 == ifindex)
                {
                    break;
                }

                result = RULE_OM_BindPort2PolicyMap(ifindex,
                                                    RULE_TYPE_INBOUND,
                                                    policy_cfg->policy_map_index);
                assert(RULE_TYPE_OK == result);
            }

            for (k=0; k<sizeof(policy_cfg->egress_port_tbl)/sizeof(*policy_cfg->egress_port_tbl); k++)
            {
                UI32_T ifindex = policy_cfg->egress_port_tbl[k];

                if (0 == ifindex)
                {
                    break;
                }

                result = RULE_OM_BindPort2PolicyMap(ifindex,
                                                    RULE_TYPE_OUTBOUND,
                                                    policy_cfg->policy_map_index);
                assert(RULE_TYPE_OK == result);
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
                                                                &policy_map_index))
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

                assert(policy_cfg->policy_map_index == policy_map_index);
            }

            if (RULE_TYPE_INBOUND == inout_profile)
            {
                assert(policy_cfg->ingress_port_tbl[ in_ifindex_ix++ ] == uport_ifindex);
            }
            else
            {
                assert(policy_cfg->egress_port_tbl[ en_ifindex_ix++ ] == uport_ifindex);
            }
        }

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
        UI8_T   port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST] = {0};

        RULE_OM_Get_PortListByClassMap(class_cfg->class_map_index, RULE_TYPE_INBOUND, port_list, &port_nbr);

        uport_ifindex = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextPort_BindClassMap(class_cfg->class_map_index,
                                                                &uport_ifindex,
                                                                &inout_profile,
                                                                &policy_map_index))
        {
            if (inout_profile == RULE_TYPE_INBOUND)
            {
                RULE_OM_SET_BIT_OFF(port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(uport_ifindex));
            }
        }

        {
            UI32_T i;

            for (i=0; i<sizeof(port_list)/sizeof(*port_list); i++)
            {
                assert(!port_list[i]);
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
        UI8_T   port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST] = {0};

        RULE_OM_Get_PortListByClassMap(class_cfg->class_map_index, RULE_TYPE_OUTBOUND, port_list, &port_nbr);

        uport_ifindex = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextPort_BindClassMap(class_cfg->class_map_index,
                                                                &uport_ifindex,
                                                                &inout_profile,
                                                                &policy_map_index))
        {
            if (inout_profile == RULE_TYPE_OUTBOUND)
            {
                RULE_OM_SET_BIT_OFF(port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(uport_ifindex));
            }
        }

        {
            UI32_T i;

            for (i=0; i<sizeof(port_list)/sizeof(*port_list); i++)
            {
                assert(!port_list[i]);
            }
        }
    }

    return 0;
}

static int
RULE_OM_TEST_GetNextPort_BindAcl()
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
    RULE_TYPE_RETURN_TYPE_T result;

    for (i=0; i<sizeof(acl_cfg_tbl)/sizeof(*acl_cfg_tbl);i++)
    {
        struct  AclCfg *acl = &acl_cfg_tbl[i];
        UI32_T  j;

        result = RULE_OM_CreateAcl(acl->name, acl->type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl->name, &acl->index);
        assert(RULE_TYPE_OK == result);

        acl->entry = RULE_OM_LocalGetAclByIndex(acl->index);
        assert(NULL != acl->entry);

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
                assert(acl->ingress_port_tbl[in_ix++] == uport_ifindex);
            }
            else if (RULE_TYPE_OUTBOUND == inout_profile)
            {
                assert(acl->egress_port_tbl[out_ix++] == uport_ifindex);
            }
            else
            {
                assert(0);
            }
        }
    }

    //
    // Reset the port list of ACL
    //
    for (i=0; i<sizeof(acl_cfg_tbl)/sizeof(*acl_cfg_tbl);i++)
    {
        struct  AclCfg *acl = &acl_cfg_tbl[i];

        memset(acl->entry->filtering_in_port_list, 0, sizeof(acl->entry->filtering_in_port_list));
        memset(acl->entry->filtering_out_port_list, 0, sizeof(acl->entry->filtering_out_port_list));
    }

    return 0;
}

static int
RULE_OM_TEST_GetNext_PortListByAclEx()
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
    RULE_TYPE_RETURN_TYPE_T result;

    /* CREATE POLICY-MAP AND BIND TO PORT
     */
    for (i=0; i<_countof(policy_map_tbl); i++)
    {
        struct PolicyMap *policy_map = &policy_map_tbl[i];
        UI32_T j;

        result = RULE_OM_Create_PolicyMap(policy_map->name, &policy_map->index);
        assert(RULE_TYPE_OK == result);

        policy_map->entry = RULE_OM_LocalGetPolicyMapByIndex(policy_map->index);
        assert(NULL != policy_map->entry);

        for (j=0; j<sizeof(policy_map->ingress_port_tbl); j++)
        {
            UI32_T port = policy_map->ingress_port_tbl[j];

            if (0 == port)
            {
                break;
            }

            result = RULE_OM_BindPort2PolicyMap(port, RULE_TYPE_INBOUND, policy_map->index);
            assert(RULE_TYPE_OK == result);
        }

        for (j=0; j<sizeof(policy_map->egress_port_tbl); j++)
        {
            UI32_T port = policy_map->egress_port_tbl[j];

            if (0 == port)
            {
                break;
            }

            result = RULE_OM_BindPort2PolicyMap(port, RULE_TYPE_OUTBOUND, policy_map->index);
            assert(RULE_TYPE_OK == result);
        }
    }

    /* CREATE CLASS-MAP AND BIND TO POLICY-MAP
     */
    for (i=0; i<sizeof(class_map_tbl)/sizeof(*class_map_tbl); i++)
    {
        struct ClassMap *class_map = &class_map_tbl[i];
        UI32_T j;

        result = RULE_OM_Create_ClassMap(class_map->name,
                                         RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                         &class_map->index);
        assert(RULE_TYPE_OK == result);

        class_map->entry = RULE_OM_LocalGetClassMapByIndex(class_map->index);
        assert(NULL != class_map->entry);

        for (j=0; j<sizeof(class_map->policy_map_tbl)/sizeof(*class_map->policy_map_tbl); j++)
        {
            struct PolicyMap *policy_map = &class_map->policy_map_tbl[j];

            if (policy_map->name[0] == 0)
            {
                break;
            }

            result = RULE_OM_GetPolicyMapIdByName(policy_map->name, &policy_map->index);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map->index, class_map->index);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (i=0; i<sizeof(acl_tbl)/sizeof(*acl_tbl); i++)
    {
        struct Acl *acl = &acl_tbl[i];
        UI32_T j;

        result = RULE_OM_CreateAcl(acl->name, acl->type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl->name, &acl->index);
        assert(RULE_TYPE_OK == result);

        acl->entry = RULE_OM_LocalGetAclByIndex(acl->index);
        assert(NULL != acl->entry);

        for (j=0; j<sizeof(acl->class_map_tbl)/sizeof(acl->class_map_tbl); j++)
        {
            struct ClassMap *class_map = &acl->class_map_tbl[j];

            if (class_map->name[0] == 0)
            {
                break;
            }

            result = RULE_OM_GetClassMapIndexByName(class_map->name, &class_map->index);
            assert(RULE_TYPE_OK == result);

            {
                RULE_TYPE_UI_ClassMapElement_T element_entry = {0};
                UI32_T                         class_index;

                element_entry.class_type = RULE_TYPE_CLASS_ACL;
                strcpy((char*)element_entry.element.acl_name, acl->name);

                result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map->index, &element_entry, &class_index);
                assert(RULE_TYPE_OK == result);
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
//            UI32_T k;
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

                assert(class_map_index == class_map->index);
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

                assert(policy_map_index == policy_map->index);
            }

            //UT_LOG("acl %lu, class-map %lu, policy-map %lu, ifindex %lu, dir %s",
            //    acl->index,
            //    class_map_index,
            //    policy_map_index,
            //    ifindex,
            //    RULE_TYPE_INBOUND==inout_profile?"in":"out");

            if (RULE_TYPE_INBOUND == inout_profile)
            {
                assert(ifindex == policy_map->ingress_port_tbl[ in_port_tbl_ix++ ]);
            }
            else
            {
                assert(ifindex == policy_map->egress_port_tbl[ out_port_tbl_ix++ ]);
            }
        }
    }

    return 0;
}

int
RULE_OM_UT_BuildMatchAllClassMapRule_ACL_plus_MF()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    UI32_T working_acl_index;

    RULE_TYPE_RETURN_TYPE_T result;

    {
        const char *acl_name = "mac-vid-sa";
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL; // L2
        UI32_T acl_index;

        RULE_TYPE_Ace_Entry_T ace_entry;

        const char *class_map_name = "class-mac-vid-sa";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st ace
        //
        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.u.mac.aceMinVid = 1000;
        ace_entry.u.mac.aceVidBitmask = 0xfff;
        ace_entry.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        //
        // 2nd ace
        //
        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        SET_MAC(ace_entry.u.mac.aceSourceMacAddr, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x11);
        SET_MAC(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);


        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        ui_class_element.class_type = RULE_TYPE_CLASS_ACL;
        strncpy(ui_class_element.element.acl_name, acl_name, sizeof(ui_class_element.element.acl_name)-1);
        ui_class_element.element.acl_name[ sizeof(ui_class_element.element.acl_name)-1 ] = '\0';

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_COS;   // L2
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &working_acl_index);
        assert(RULE_TYPE_OK == result);

        {
            UI32_T  i;
            UI32_T  ace_index;

            RULE_TYPE_AclType_T     ace_type;

            RULE_TYPE_Acl_T         acl_entry;

            result = RULE_OM_GetAclByIndex(working_acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(working_acl_index, &ace_type, &ace_index, &ace_entry);
                 ++i)
            {
                switch (i)
                {
                    case 0:
                        assert(VAL_diffServMacAceVidOp_equal == ace_entry.u.mac.aceVidOp);
                        assert(1000 == ace_entry.u.mac.aceMinVid);
                        break;

                    case 1:
                        assert(0x0c == ace_entry.u.mac.aceSourceMacAddr[4] &&
                               0x11 == ace_entry.u.mac.aceSourceMacAddr[5]);

                        assert(0xff == ace_entry.u.mac.aceSourceMacAddrBitmask[4] &&
                               0xff == ace_entry.u.mac.aceSourceMacAddrBitmask[5]);
                        break;

                    default:
                        assert(0);
                        break;
                }

                assert(VAL_diffServMacAceCosOp_equal == ace_entry.u.mac.aceCosOp);
                assert(5 == ace_entry.u.mac.aceMinCos);
            }

            assert(acl_entry.ace_count == i);
        }
    }

    {
        const char *acl_name = "ip-sip-l4port";
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL; // L3
        UI32_T acl_index;

        RULE_TYPE_Ace_Entry_T ace_entry;

        const char *class_map_name = "class-ip-sip-l4port";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st ace
        //
        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        SET_IPV4(ace_entry.u.ip.aceSourceIpAddr, 192, 168, 1, 1);
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        //
        // 2nd ace
        //
        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.u.ip.aceMinDestPort = 80;
        ace_entry.u.ip.aceDestPortBitmask = 0xffff;
        ace_entry.u.ip.aceDestPortOp = VAL_diffServIpAceDestPortOp_equal;

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        ui_class_element.class_type = RULE_TYPE_CLASS_ACL;
        strncpy(ui_class_element.element.acl_name, acl_name, sizeof(ui_class_element.element.acl_name)-1);
        ui_class_element.element.acl_name[ sizeof(ui_class_element.element.acl_name)-1 ] = '\0';

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_COS;   // L2
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &working_acl_index);
        assert(RULE_TYPE_OK == result);

        {
            UI32_T  ace_index;
            UI32_T  i;

            RULE_TYPE_AclType_T         ace_type;

            RULE_TYPE_Acl_T acl_entry;

            result = RULE_OM_GetAclByIndex(working_acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(working_acl_index, &ace_type, &ace_index, &ace_entry);
                 ++i)
            {
                switch (i)
                {
                    case 0:
                    {
                        UI32_T tmp_ip_addr;

                        SET_IPV4(tmp_ip_addr, 192, 168, 1, 1);
                        assert(tmp_ip_addr == ace_entry.u.ip.aceSourceIpAddr);

                        SET_IPV4(tmp_ip_addr, 255, 255, 255, 255);
                        assert(tmp_ip_addr == ace_entry.u.ip.aceSourceIpAddrBitmask);
                        break;
                    }

                    case 1:
                        assert(80 == ace_entry.u.ip.aceMinDestPort);
                        assert(0xffff == ace_entry.u.ip.aceDestPortBitmask);
                        assert(VAL_diffServIpAceDestPortOp_equal == ace_entry.u.ip.aceDestPortOp);
                        break;

                    default:
                        assert(0);
                        break;
                }

                assert(VAL_diffServMacAceCosOp_equal == ace_entry.u.mac.aceCosOp);
                assert(5 == ace_entry.u.mac.aceMinCos);
            }

            assert(acl_entry.ace_count == i);
        }
    }

#if (SYS_CPNT_ACL_IPV6 == TRUE)
    {
        const char *acl_name = "ipv6-sip-dscp";
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_IPV6_EXT_ACL; // L3
        UI32_T acl_index;

        RULE_TYPE_Ace_Entry_T ace_entry;

        const char *class_map_name = "class-ipv6-sip-dscp";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st ace
        //
        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        SET_IPV6(ace_entry.u.ipv6.aceSourceIpAddr, 0x12345678, 0x12345678, 0x12345678, 0x12345678);
        memset(ace_entry.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_ADPT_IPV6_ADDR_LEN);
        //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 128;

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        //
        // 2nd ace
        //
        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.u.ipv6.aceDscp = 15;

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        ui_class_element.class_type = RULE_TYPE_CLASS_ACL;
        strncpy(ui_class_element.element.acl_name, acl_name, sizeof(ui_class_element.element.acl_name)-1);
        ui_class_element.element.acl_name[ sizeof(ui_class_element.element.acl_name)-1 ] = '\0';

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_COS;   // L2
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &working_acl_index);
        assert(RULE_TYPE_OK == result);

        {
            UI32_T i;
            UI32_T ace_index;
            RULE_TYPE_AclType_T         ace_type;

            RULE_TYPE_Acl_T acl_entry;

            result = RULE_OM_GetAclByIndex(working_acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

#define IS_FULL_IPV6_MASK(addr)                                                                    \
    (0xff == (addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5] & addr[6] & addr[7] &       \
              addr[8] & addr[9] & addr[10] & addr[11] & addr[12] & addr[13] & addr[14] & addr[15]))

            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(working_acl_index, &ace_type, &ace_index, &ace_entry);
                 ++i)
            {
                switch (i)
                {
                    case 0:
                    {
                        UI8_T tmp_ip_addr[16];

                        SET_IPV6(tmp_ip_addr, 0x12345678, 0x12345678, 0x12345678, 0x12345678);
                        assert(0 == memcmp(tmp_ip_addr, ace_entry.u.ipv6.aceSourceIpAddr, sizeof(tmp_ip_addr)));

                        assert(TRUE == IS_FULL_IPV6_MASK(ace_entry.u.ipv6.aceSourceIpAddrBitmask));
                        //assert(128 == ace_entry.u.ipv6.aceSourceIpAddrPrefixLen);
                        break;
                    }

                    case 1:
                        assert(15 == ace_entry.u.ipv6.aceDscp);
                        break;

                    default:
                        assert(0);
                        break;
                }

                assert(VAL_diffServMacAceCosOp_equal == ace_entry.u.mac.aceCosOp);
                assert(5 == ace_entry.u.mac.aceMinCos);
            }
#undef IS_FULL_IPV6_MASK
            assert(acl_entry.ace_count == i);
        }
    }
#endif // SYS_CPNT_ACL_IPV6

#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL

    return 0;
}

int
RULE_OM_UT_BuildMatchAllClassMapRule_ACL_plus_MF_Conflict()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    UI32_T working_acl_index;

    RULE_TYPE_RETURN_TYPE_T result;

#if (SYS_CPNT_ACL_IPV6 == TRUE)
    {
        const char *acl_name = "ipv6-sip-dscp";
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_IPV6_EXT_ACL; // L3
        UI32_T acl_index;

        RULE_TYPE_Ace_Entry_T ace_entry;

        const char *class_map_name = "class-ipv6-sip-dscp";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st ace
        //
        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        SET_IPV6(ace_entry.u.ipv6.aceSourceIpAddr, 0x12345678, 0x12345678, 0x12345678, 0x12345678);
        memset(ace_entry.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_ADPT_IPV6_ADDR_LEN);
        //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 128;

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        //
        // 2nd ace
        //
        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.u.ipv6.aceDscp = 15;

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        ui_class_element.class_type = RULE_TYPE_CLASS_ACL;
        strncpy(ui_class_element.element.acl_name, acl_name, sizeof(ui_class_element.element.acl_name)-1);
        ui_class_element.element.acl_name[ sizeof(ui_class_element.element.acl_name)-1 ] = '\0';

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;   // L3.
        ui_class_element.element.mf_entry.mf_value = 17;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &working_acl_index);
        assert(RULE_TYPE_OK != result); // ip and ipv6 are overlap on OM

    }
#endif // SYS_CPNT_ACL_IPV6
#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL

    return 0;
}

int
RULE_OM_UT_BuildMatchAllClassMapRule_MF()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    UI32_T working_acl_index;

    RULE_TYPE_RETURN_TYPE_T result;

    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        const char *class_map_name = "class-mf-cos-vlan";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st MF
        //

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_COS;   // L2
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // 2nd MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;   // L2
        ui_class_element.element.mf_entry.mf_value = 1000;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &working_acl_index);
        assert(RULE_TYPE_OK == result);

        {
            UI32_T i;
            UI32_T  ace_index;

            RULE_TYPE_AclType_T ace_type;

            RULE_TYPE_Acl_T     acl_entry;

            result = RULE_OM_GetAclByIndex(working_acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(working_acl_index, &ace_type, &ace_index, &ace_entry);
                 ++i)
//            for (i=0; i < acl_entry.ace_count; ++i)
            {
//                UI32_T ace_index = acl_entry.ace_index_list[i];
//
//                result = RULE_OM_GetAceByIndex(ace_index, &ace_entry);
//                assert(RULE_TYPE_OK == result);

//                assert(ace_index == acl_entry.ace_index_list[i]);

                assert(VAL_diffServMacAceVidOp_equal == ace_entry.u.mac.aceVidOp);
                assert(1000 == ace_entry.u.mac.aceMinVid);
                assert(0xfff == ace_entry.u.mac.aceVidBitmask);

                assert(VAL_diffServMacAceCosOp_equal == ace_entry.u.mac.aceCosOp);
                assert(5 == ace_entry.u.mac.aceMinCos);
                assert(0x07 == ace_entry.u.mac.aceCosBitmask);
            }

            assert(acl_entry.ace_count == i);
        }

    }

    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        const char *class_map_name = "class-mf-dscp";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st MF
        //

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;   // L2
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &working_acl_index);
        assert(RULE_TYPE_OK == result);

        {
            UI32_T i;
            UI32_T  ace_index;

            RULE_TYPE_AclType_T ace_type;

            RULE_TYPE_Acl_T     acl_entry;

            result = RULE_OM_GetAclByIndex(working_acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(working_acl_index, &ace_type, &ace_index, &ace_entry);
                 ++i)
//            for (i=0; i < acl_entry.ace_count; ++i)
            {
//                UI32_T ace_index = acl_entry.ace_index_list[i];
//
//                result = RULE_OM_GetAceByIndex(ace_index, &ace_entry);
//                assert(RULE_TYPE_OK == result);

//                assert(ace_index == acl_entry.ace_index_list[i]);

                assert(5 == ace_entry.u.ip.aceDscp);
            }

            assert(acl_entry.ace_count == i);
        }

    }

    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        const char *class_map_name = "class-mf-dscp-cos";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st MF
        //

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;   // L3
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // 2nd MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_COS;  // L2
        ui_class_element.element.mf_entry.mf_value = 2;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &working_acl_index);
        assert(RULE_TYPE_OK == result);

        {
            UI32_T  i;
            UI32_T  ace_index;

            RULE_TYPE_AclType_T     ace_type;
            RULE_TYPE_Acl_T         acl_entry;

            result = RULE_OM_GetAclByIndex(working_acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(working_acl_index, &ace_type, &ace_index, &ace_entry);
                 ++i)
//            for (i=0; i < acl_entry.ace_count; ++i)
            {
//                UI32_T ace_index = acl_entry.ace_index_list[i];
//
//                result = RULE_OM_GetAceByIndex(ace_index, &ace_entry);
//                assert(RULE_TYPE_OK == result);

//                assert(ace_index == acl_entry.ace_index_list[i]);

                assert(5 == ace_entry.u.ip.aceDscp);

                assert(VAL_diffServMacAceCosOp_equal == ace_entry.u.mac.aceCosOp);
                assert(2 == ace_entry.u.mac.aceMinCos);
                assert(0x07 == ace_entry.u.mac.aceCosBitmask);
            }

            assert(acl_entry.ace_count == i);
        }

    }
#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL

    return 0;
}

int
RULE_OM_UT_BuildMatchAllClassMapRule_MF_All()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)

    UI32_T working_acl_index;

    RULE_TYPE_RETURN_TYPE_T result;

    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        const char *class_map_name = "class-mf-all";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st MF
        //

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_COS;   // L2
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // 2nd MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;   // L2
        ui_class_element.element.mf_entry.mf_value = 1000;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // 3nd MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;   // L2
        ui_class_element.element.mf_entry.mf_value = 20;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // 4th MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_SOURCE_PORT;
        ui_class_element.element.mf_entry.mf_value = 1;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &working_acl_index);
        assert(RULE_TYPE_OK == result);

        {
            UI32_T  i;
            UI32_T  ace_index;

            RULE_TYPE_AclType_T     ace_type;

            RULE_TYPE_Acl_T         acl_entry;

            result = RULE_OM_GetAclByIndex(working_acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(working_acl_index, &ace_type, &ace_index, &ace_entry);
                 ++i)
//            for (i=0; i < acl_entry.ace_count; ++i)
            {
//                UI32_T ace_index = acl_entry.ace_index_list[i];
//
//                result = RULE_OM_GetAceByIndex(ace_index, &ace_entry);
//                assert(RULE_TYPE_OK == result);

//                assert(ace_index == acl_entry.ace_index_list[i]);

                assert(VAL_diffServMacAceSourcePortOp_equal == ace_entry.u.mac.aceSourcePortOp);
                assert(1 == ace_entry.u.mac.aceMinSourcePort);

                assert(VAL_diffServMacAceVidOp_equal == ace_entry.u.mac.aceVidOp);
                assert(1000 == ace_entry.u.mac.aceMinVid);
                assert(0xfff == ace_entry.u.mac.aceVidBitmask);

                assert(VAL_diffServMacAceCosOp_equal == ace_entry.u.mac.aceCosOp);
                assert(5 == ace_entry.u.mac.aceMinCos);
                assert(0x07 == ace_entry.u.mac.aceCosBitmask);

                assert(20 == ace_entry.u.ip.aceDscp);

            }

            assert(acl_entry.ace_count == i);
        }
    }

#if (SYS_CPNT_ACL_IPV6 == TRUE)
    {
        RULE_TYPE_Ace_Entry_T ace_entry;

        const char *class_map_name = "class-mf-ip6-all";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st MF
        //

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_COS;   // L2
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // 2nd MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;   // L2
        ui_class_element.element.mf_entry.mf_value = 1000;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // 3nd MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP_IPV6; // L2
        ui_class_element.element.mf_entry.mf_value = 20;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // 4th MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_SOURCE_PORT;
        ui_class_element.element.mf_entry.mf_value = 1;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &working_acl_index);
        assert(RULE_TYPE_OK == result);

        {
            UI32_T  i;
            UI32_T  ace_index;

            RULE_TYPE_AclType_T     ace_type;

            RULE_TYPE_Acl_T         acl_entry;

            result = RULE_OM_GetAclByIndex(working_acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            for (ace_index = 0, i = 0;
                 RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(working_acl_index, &ace_type, &ace_index, &ace_entry);
                 ++i)
//            for (i=0; i < acl_entry.ace_count; ++i)
            {
//                UI32_T ace_index = acl_entry.ace_index_list[i];
//
//                result = RULE_OM_GetAceByIndex(ace_index, &ace_entry);
//                assert(RULE_TYPE_OK == result);

//                assert(ace_index == acl_entry.ace_index_list[i]);

                assert(VAL_diffServMacAceSourcePortOp_equal == ace_entry.u.mac.aceSourcePortOp);
                assert(1 == ace_entry.u.mac.aceMinSourcePort);

                assert(VAL_diffServMacAceVidOp_equal == ace_entry.u.mac.aceVidOp);
                assert(1000 == ace_entry.u.mac.aceMinVid);
                assert(0xfff == ace_entry.u.mac.aceVidBitmask);

                assert(VAL_diffServMacAceCosOp_equal == ace_entry.u.mac.aceCosOp);
                assert(5 == ace_entry.u.mac.aceMinCos);
                assert(0x07 == ace_entry.u.mac.aceCosBitmask);

                assert(20 == ace_entry.u.ipv6.aceDscp);

            }

            assert(acl_entry.ace_count == i);
        }
    }
#endif // SYS_CPNT_ACL_IPV6

#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL

    return 0;
}

int
RULE_OM_UT_BuildMatchAllClassMapRule_MF_Conflit()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    UI32_T working_acl_index;

    RULE_TYPE_RETURN_TYPE_T result;

    {
        const char *class_map_name = "class-mf-dscp-dscp";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st MF
        //

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP; // L3
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // 2nd MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;    // L3
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK != result);

        //
        // 2nd MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;    // L3
        ui_class_element.element.mf_entry.mf_value = 6;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK != result);
    }

#if (SYS_CPNT_ACL_IPV6 == TRUE)
    {
        const char *class_map_name = "class-mf-dscp-dscp6";
        RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;
        UI32_T class_map_index;

        RULE_TYPE_UI_ClassMapElement_T ui_class_element;

        UI32_T class_index;

        result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // 1st MF
        //

        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP; // L3
        ui_class_element.element.mf_entry.mf_value = 5;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // 2nd MF
        //
        ui_class_element.class_type = RULE_TYPE_CLASS_MF;
        ui_class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP_IPV6;    // L3
        ui_class_element.element.mf_entry.mf_value = 6;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &ui_class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_BuildMatchAllClassMapRule(class_map_index, &working_acl_index);
        assert(RULE_TYPE_OK != result);

    }
#endif // SYS_CPNT_ACL_IPV6
#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL

    return 0;
}


#if 0 /* FIXME: TIME RANGE TEST */
int
RULE_OM_UT_TimeRangeActiveTest_For_Periodic()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;
    const char *time_range_name = "t1";
    UI16_T time_range_index;

    RULE_TYPE_TIME_ACL_PERIODIC_TIME_T periodic;

    result = RULE_OM_CreateTimeRangeEntry(time_range_name, &time_range_index);
    assert(RULE_TYPE_OK == result);

    memset(&periodic, 0, sizeof(periodic));
    periodic.start_type_of_wk = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
    periodic.start_hour = 16;
    periodic.start_minute = 25;
    periodic.end_type_of_wk = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
    periodic.end_hour = 16;
    periodic.end_minute = 28;

    result = RULE_OM_SetTimeRangePeriodic(time_range_index, &periodic);
    assert(RULE_TYPE_OK == result);

    memset(&periodic, 0, sizeof(periodic));
    periodic.start_type_of_wk = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
    periodic.start_hour = 16;
    periodic.start_minute = 26;
    periodic.end_type_of_wk = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
    periodic.end_hour = 16;
    periodic.end_minute = 30;

    result = RULE_OM_SetTimeRangePeriodic(time_range_index, &periodic);
    assert(RULE_TYPE_OK == result);

    {
        RULE_TYPE_TIME_ACL_CONVERT_TIME_T current_time;
        UI8_T                             state_change_list[SYS_ADPT_MAX_NBRS_OF_TIME_ACL_TABLE] = {0};

        SYS_TIME_SetRealTimeClock(2011, 1, 1, 16, 26, 0);

        SYS_TIME_GetRealTimeBySec(&current_time.time_by_sec);
        SYS_TIME_ConvertSecondsToDateTime(current_time.time_by_sec,
                                          (int *)&current_time.year,
                                          (int *)&current_time.month,
                                          (int *)&current_time.day,
                                          (int *)&current_time.hour,
                                          (int *)&current_time.minute,
                                          (int *)&current_time.second);
        SYS_TIME_GetDayOfWeek(current_time.time_by_sec, &current_time.day_of_week);

        RULE_OM_UpdateTimeRangeTableByTimer(&current_time, state_change_list);

        assert(state_change_list[time_range_index] == 1);
    }

#endif // SYS_CPNT_TIME_BASED_ACL
    return 0;
}
#endif /* 0 */

int
RULE_OM_UT_ConvertFromUIBundleGroup()
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    const char *policy_map_name = "p1";
    UI32_T policy_map_index;

    struct class_map_s
    {
        char    *class_map_name;
        UI32_T  bandwidth;
        UI32_T  priority;
    } class_map_list[] =
    {
        {
            "match-any", 5000, 0
        },
        {
            "c31", 5000, 10
        },
        {
            "c32", 1000, 400
        },
        {
            "c33", 1000, 400
        },
        {
            "c34", 1000, 400
        },
    };

    struct bundle_s
    {
        UI32_T  index;
        UI32_T  bandwidth;
        char    *class_map_list[SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_OF_BUNDLE_GROUP];
    } bundle_list[] =
    {
        {
            1,
            5000,
            {"match-any", "c31", "c31", "c32", "c33", "c34"},
        },
        {
            1,
            5000,
            {"c31", "c32", "c33"},
        },
        {
            2,
            1000,
            {"match-any", "c34"},
        },

        /* failed case
         */
        {
            2,
            1000,
            {"c33"},
        },
    };

    UI32_T i;
    UI32_T result;

    result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(class_map_list); ++i)
    {
        const char  *class_map_name = class_map_list[i].class_map_name;
        UI32_T      bandwidth       = class_map_list[i].bandwidth;
        UI32_T      priority        = class_map_list[i].priority;

        UI32_T      class_map_index;
        UI32_T      class_index;

        RULE_TYPE_UI_ClassMapElement_T  class_element;
        RULE_TYPE_UI_Action_T           ui_action;
        RULE_TYPE_TBParamEntry_T        meter;

        result = RULE_OM_Create_ClassMap(class_map_name, RULE_TYPE_CLASS_MAP_MATCH_ALL, &class_map_index);
        assert(RULE_TYPE_OK == result);

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 1000 + i;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index,
                                                         &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
        ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
        ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

        ui_action.class_action_type     = CLASS_ACTION_SET_PRIORITY;
        ui_action.class_action_value    = priority;

        meter.meter_model = RULE_TYPE_METER_MODE_FLOW;
        meter.rate = bandwidth;
        meter.burst_size = 4000;

        result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == result);
    }

    for (i = 0; i < _countof(bundle_list); ++i)
    {
        UI32_T idx = bundle_list[i].index;
        UI32_T bundle_rate = bundle_list[i].bandwidth;

        RULE_TYPE_UI_BundleGroup_T ui_bundle_group;
        UI32_T ci;

        memset(&ui_bundle_group, 0, sizeof(ui_bundle_group));

        ui_bundle_group.index = idx;
        ui_bundle_group.bundle_rate_kbps = bundle_rate;
        ui_bundle_group.valid_status = TRUE;
        for (ci = 0; ci < _countof(bundle_list[i].class_map_list); ++ci)
        {
            char *name = bundle_list[i].class_map_list[ci];

            if (NULL == name)
            {
                break;
            }

            strcpy(ui_bundle_group.class_map_name_list[ci], name);
        }

        ui_bundle_group.class_map_count = ci;

        result = RULE_OM_SetUIBundleGroup(policy_map_index, &ui_bundle_group);

        {
            RULE_TYPE_PolicyMap_T *policy_map_p;

            policy_map_p = RULE_OM_LocalGetPolicyMapByIndex(policy_map_index);
            assert(NULL != policy_map_p);

            switch (i)
            {
                case 0:
                    assert(RULE_TYPE_OK == result);
                    assert(TRUE == policy_map_p->bundle_group[0].valid_status);
                    assert(5 == policy_map_p->bundle_group[0].class_map_count);
                    assert(3 == policy_map_p->bundle_group[0].child_bundle_count);

                    assert(FALSE == policy_map_p->bundle_group[1].valid_status);
                    break;

                case 1:
                    assert(RULE_TYPE_OK == result);
                    assert(TRUE == policy_map_p->bundle_group[0].valid_status);
                    assert(3 == policy_map_p->bundle_group[0].class_map_count);
                    assert(2 == policy_map_p->bundle_group[0].child_bundle_count);

                    assert(FALSE == policy_map_p->bundle_group[1].valid_status);
                    break;

                case 2:
                    assert(RULE_TYPE_OK == result);
                    assert(TRUE == policy_map_p->bundle_group[0].valid_status);
                    assert(3 == policy_map_p->bundle_group[0].class_map_count);
                    assert(2 == policy_map_p->bundle_group[0].child_bundle_count);

                    assert(TRUE == policy_map_p->bundle_group[1].valid_status);
                    assert(2 == policy_map_p->bundle_group[1].class_map_count);
                    assert(2 == policy_map_p->bundle_group[1].child_bundle_count);

                    assert(FALSE == policy_map_p->bundle_group[2].valid_status);
                    break;

                case 3:
                    assert(RULE_TYPE_OK != result);
                    assert(TRUE == policy_map_p->bundle_group[0].valid_status);
                    assert(3 == policy_map_p->bundle_group[0].class_map_count);
                    assert(2 == policy_map_p->bundle_group[0].child_bundle_count);

                    assert(TRUE == policy_map_p->bundle_group[1].valid_status);
                    assert(2 == policy_map_p->bundle_group[1].class_map_count);
                    assert(2 == policy_map_p->bundle_group[1].child_bundle_count);

                    assert(FALSE == policy_map_p->bundle_group[2].valid_status);
                    break;

                default:
                    break;
            }
        }
    }

#endif // SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL
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
RULE_OM_UT_BundleGroup_Validate_Sync_Meter_Action()
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    const char *policy_map_name = "p1";
    UI32_T policy_map_index;

    const char *class_map_name = "c1";
    UI32_T class_map_index;

    RULE_TYPE_PolicyMap_T           policy_map;
    RULE_TYPE_UI_BundleGroup_T      ui_boundle_group;
    RULE_TYPE_UI_Action_T           ui_action;
    RULE_TYPE_TBParamEntry_T        meter;
    RULE_TYPE_PolicyElement_T       policy_element;

    UI32_T result;

    memset(&ui_boundle_group, 0, sizeof(ui_boundle_group));

    ui_boundle_group.class_map_count = 1;
    strcpy(ui_boundle_group.class_map_name_list[0], class_map_name);
    ui_boundle_group.index = 1;
    ui_boundle_group.valid_status = TRUE;

    ui_action.in_action_type        = IN_ACTION_INVALID;
    ui_action.out_action_type       = OUT_ACTION_INVALID;
    ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
    ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
    ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

    ui_action.class_action_type     = CLASS_ACTION_SET_PRIORITY;
    ui_action.class_action_value    = 1000;

    meter.meter_model   = RULE_TYPE_METER_MODE_FLOW;
    meter.rate          = 5000;
    meter.burst_size    = 4000;

    result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_Create_ClassMap(class_map_name, RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_SetUIBundleGroup(policy_map_index, &ui_boundle_group);
    assert(RULE_TYPE_OK == result);

    //
    // validate sync bundle
    //
    {
        result = RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map);
        assert(RULE_TYPE_OK == result);

        assert(TRUE == policy_map.bundle_group[ui_boundle_group.index-1].valid_status);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].priority);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].priority);
        assert(meter.rate == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].bandwidth_kbps);
        assert(meter.burst_size == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].burst_size_byte);
    }

    result = RULE_OM_GetPolicyMapElementEntry(policy_map_index, class_map_index, &policy_element);
    assert(RULE_TYPE_OK == result);


    //
    // change meter
    //
    meter.meter_model   = RULE_TYPE_METER_MODE_FLOW;
    meter.rate          = 5500;
    meter.burst_size    = 4500;
    result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
    assert(RULE_TYPE_OK == result);

    //
    // validate sync bundle
    //
    {
        result = RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map);
        assert(RULE_TYPE_OK == result);

        assert(TRUE == policy_map.bundle_group[ui_boundle_group.index-1].valid_status);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].priority);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].priority);
        assert(5500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].bandwidth_kbps);
        assert(4500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].burst_size_byte);
    }

    meter.meter_model   = RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE;
    meter.rate          = 5500;
    meter.burst_size    = 4500;
    result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
    assert(RULE_TYPE_OK != result);

    //
    // validate sync bundle
    //
    {
        result = RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map);
        assert(RULE_TYPE_OK == result);

        assert(TRUE == policy_map.bundle_group[ui_boundle_group.index-1].valid_status);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].priority);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].priority);
        assert(5500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].bandwidth_kbps);
        assert(4500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].burst_size_byte);
    }


    meter.meter_model   = RULE_TYPE_METER_MODE_SRTCM_COLOR_BLIND;
    meter.rate          = 5500;
    meter.burst_size    = 4500;
    result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
    assert(RULE_TYPE_OK != result);

    //
    // validate sync bundle
    //
    {
        result = RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map);
        assert(RULE_TYPE_OK == result);

        assert(TRUE == policy_map.bundle_group[ui_boundle_group.index-1].valid_status);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].priority);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].priority);
        assert(5500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].bandwidth_kbps);
        assert(4500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].burst_size_byte);
    }

    //
    // change action
    //
    ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

    result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
    assert(RULE_TYPE_OK == result);

    //
    // validate sync bundle
    //
    {
        result = RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map);
        assert(RULE_TYPE_OK == result);

        assert(TRUE == policy_map.bundle_group[ui_boundle_group.index-1].valid_status);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].priority);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].priority);
        assert(5500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].bandwidth_kbps);
        assert(4500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].burst_size_byte);
    }

    ui_action.class_action_type   = CLASS_ACTION_SET_PRIORITY;
    ui_action.class_action_value  = 2000;

    result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
    assert(RULE_TYPE_OK == result);

    //
    // validate sync bundle
    //
    {
        result = RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map);
        assert(RULE_TYPE_OK == result);

        assert(TRUE == policy_map.bundle_group[ui_boundle_group.index-1].valid_status);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].priority);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].priority);
        assert(5500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].bandwidth_kbps);
        assert(4500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].burst_size_byte);
    }

    ui_action.violate_action_type   = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
    ui_action.violate_action_value  = 40;

    result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
    assert(RULE_TYPE_OK != result);

    //
    // validate sync bundle
    //
    {
        result = RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map);
        assert(RULE_TYPE_OK == result);

        assert(TRUE == policy_map.bundle_group[ui_boundle_group.index-1].valid_status);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].priority);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].priority);
        assert(5500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].bandwidth_kbps);
        assert(4500 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].burst_size_byte);
    }
#endif // SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL
    return 0;
}

int
RULE_OM_UT_Undo_Set_Action_To_Default()
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    const char *policy_map_name = "p1";
    UI32_T policy_map_index;

    const char *class_map_name = "c1";
    UI32_T class_map_index;

    RULE_TYPE_PolicyMap_T           policy_map;
    RULE_TYPE_UI_BundleGroup_T      ui_boundle_group;
    RULE_TYPE_UI_Action_T           ui_action;
    RULE_TYPE_TBParamEntry_T        meter;
    RULE_TYPE_PolicyElement_T       policy_element;

    UI32_T result;

    memset(&ui_boundle_group, 0, sizeof(ui_boundle_group));

    ui_boundle_group.class_map_count = 1;
    strcpy(ui_boundle_group.class_map_name_list[0], class_map_name);
    ui_boundle_group.index = 1;
    ui_boundle_group.valid_status = TRUE;

    ui_action.in_action_type        = IN_ACTION_INVALID;
    ui_action.out_action_type       = OUT_ACTION_INVALID;
    ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
    ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
    ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

    ui_action.class_action_type     = CLASS_ACTION_SET_PRIORITY;
    ui_action.class_action_value    = 1000;

    meter.meter_model   = RULE_TYPE_METER_MODE_FLOW;
    meter.rate          = 5000;
    meter.burst_size    = 4000;

    result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_Create_ClassMap(class_map_name, RULE_TYPE_CLASS_MAP_MATCH_ANY, &class_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_SetUIBundleGroup(policy_map_index, &ui_boundle_group);
    assert(RULE_TYPE_OK == result);

    //
    // validate sync bundle
    //
    {
        result = RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map);
        assert(RULE_TYPE_OK == result);

        assert(TRUE == policy_map.bundle_group[ui_boundle_group.index-1].valid_status);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].priority);
        assert(1 == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_count);
        assert(ui_action.class_action_value == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].priority);
        assert(meter.rate == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].bandwidth_kbps);
        assert(meter.burst_size == policy_map.bundle_group[ui_boundle_group.index-1].child_bundle_list[0].class_list[0].burst_size_byte);
    }

    {
        RULE_TYPE_Action_T          old_action;
        RULE_TYPE_Action_T          new_action;


        result = RULE_OM_GetPolicyMapElementEntry(policy_map_index, class_map_index, &policy_element);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetActionEntryByIndex(policy_element.action_index, &old_action);
        assert(RULE_TYPE_OK == result);

        //
        // start - case 1
        //
        ui_action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;

        result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK != result);

        result = RULE_OM_GetActionEntryByIndex(policy_element.action_index, &new_action);
        assert(RULE_TYPE_OK == result);

        assert(0 == memcmp(&old_action, &new_action, sizeof(old_action)));

        //
        // case 2
        //
        result = RULE_OM_LocalSetOutOfProfileActionToNull(policy_element.action_index);

        assert(RULE_TYPE_OK != result);

        result = RULE_OM_GetActionEntryByIndex(policy_element.action_index, &new_action);
        assert(RULE_TYPE_OK == result);

        assert(0 == memcmp(&old_action, &new_action, sizeof(old_action)));
    }
#endif // SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL

    return 0;
}

int
RULE_OM_UT_RunTestCaese()
{
#define RULE_OM_UT_TEST(func) \
    UT_Test(RULE_OM_UT_Setup, RULE_OM_UT_Teardown, func)

//    RULE_OM_UT_TEST(RULE_OM_UT_ACE_Entry_Instance);
    if (l4_options.run_rule_om)
    {
        /* priority test
         */
        if (1)
        {
        }

        RULE_OM_UT_TEST(RULE_OM_UT_Validate_Ifindex);
        RULE_OM_UT_TEST(RULE_OM_UT_Ifindex_To_Array_Index);
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
        RULE_OM_UT_TEST(RULE_OM_UT_Convert_2_Mib_Acl);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Last_ACE);

#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
        RULE_OM_UT_TEST(RULE_OM_UT_Conver_Ace_Between_OM_UI_With_Mix_MAC_Ace);
#endif  /* SYS_CPNT_ACL_MAC_IP_MIX == TRUE */

        RULE_OM_UT_TEST(RULE_OM_UT_IsSameAceWithoutAccessField);
        RULE_OM_UT_TEST(RULE_OM_UT_IsMatchAnyAnyAce);
        RULE_OM_UT_TEST(RULE_OM_UT_GetNextAceByAcl);

        RULE_OM_UT_TEST(RULE_OM_TEST_Validate_Ace);
        RULE_OM_UT_TEST(RULE_OM_TEST_Validate_Mac_Plus_Ipv6_Ace);
        RULE_OM_UT_TEST(RULE_OM_TEST_Validate_Mac_Plus_Ipv6_Ace_Fail);
        RULE_OM_UT_TEST(RULE_OM_TEST_Add_ACE_2_ACL_With_Wrong_Type);
        RULE_OM_UT_TEST(RULE_OM_TEST_Add_ACE_2_ACL_With_Wrong_ACE);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACE_Basic);
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_1);
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_2);
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_Best_Case_Inc);
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_Best_Case_Dec);
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_Best_Case_Interlace);
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
        RULE_OM_UT_TEST(RULE_OM_TEST_Compress_ACL_MIX_TYPE_ACE);
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
#endif // #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)

        RULE_OM_UT_TEST(RULE_OM_TEST_Max_ACE_Of_ACL);
        RULE_OM_UT_TEST(RULE_OM_TEST_Max_ACE_Of_System);
        RULE_OM_UT_TEST(RULE_OM_TEST_Create_ACL_By_Index);
        RULE_OM_UT_TEST(RULE_OM_TEST_Max_ACL_Of_System);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        RULE_OM_UT_TEST(RULE_OM_TEST_MergerMacAce_BMT);
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

        RULE_OM_UT_TEST(RULE_OM_TEST_CreateClassMap);
        RULE_OM_UT_TEST(RULE_OM_TEST_Max_ClassMap_Of_System);
        RULE_OM_UT_TEST(RULE_OM_TEST_CreatePolicyMap);
        RULE_OM_UT_TEST(RULE_OM_TEST_BindPort2PolicyMap);
        RULE_OM_UT_TEST(RULE_OM_TEST_GetPortPolicyMapIndex);
        RULE_OM_UT_TEST(RULE_OM_TEST_GetNext_PortListByPolicyMap);
        RULE_OM_UT_TEST(RULE_OM_TEST_GetNext_PortListByClassMap);
        RULE_OM_UT_TEST(RULE_OM_TEST_GetNextPort_BindAcl);
        RULE_OM_UT_TEST(RULE_OM_TEST_GetNext_PortListByAclEx);

        RULE_OM_UT_TEST(RULE_OM_UT_BuildMatchAllClassMapRule_ACL_plus_MF);
        RULE_OM_UT_TEST(RULE_OM_UT_BuildMatchAllClassMapRule_ACL_plus_MF_Conflict);
        RULE_OM_UT_TEST(RULE_OM_UT_BuildMatchAllClassMapRule_MF);
        RULE_OM_UT_TEST(RULE_OM_UT_BuildMatchAllClassMapRule_MF_All);
        RULE_OM_UT_TEST(RULE_OM_UT_BuildMatchAllClassMapRule_MF_Conflit);

        RULE_OM_UT_TEST(RULE_OM_UT_ConvertFromUIBundleGroup);
        RULE_OM_UT_TEST(RULE_OM_UT_PolicyMapPtrToIndex);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Next_PolicyMap);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Next_PolicyMap_By_ClassMap_Index);
        RULE_OM_UT_TEST(RULE_OM_UT_Get_Next_PolicyMap_By_ClassMap_Action_Meter_Index);
        RULE_OM_UT_TEST(RULE_OM_UT_BundleGroup_Validate_Sync_Meter_Action);
        RULE_OM_UT_TEST(RULE_OM_UT_Undo_Set_Action_To_Default);
    }
    else
    {
        printf("%s (%d): Skip all rule_om test case!!\r\n", __FUNCTION__, __LINE__);
    }
    return 0;

#undef RULE_OM_UT_TEST
}


