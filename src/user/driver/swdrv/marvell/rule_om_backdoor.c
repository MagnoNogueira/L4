#include "rule_om_backdoor.h"
#include "backdoor_mgr.h"

static BOOL_T L4_Backdoor_IsNullMac(const UI8_T *mac)
{
    #define CHECK_MAC_IS_NULL(mac) ((mac[0]|mac[1]|mac[2]|mac[3]|mac[4]|mac[5])==0)

    return (CHECK_MAC_IS_NULL(mac));
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_DumpRawDataInHex
 *-------------------------------------------------------------------------
 * PURPOSE  : dump data in HEX format
 * INPUT    : data, size
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *-------------------------------------------------------------------------*/
static void L4_Backdoor_DumpRawDataInHex(const UI8_T *data, UI32_T size, BOOL_T dont_dump_if_empty)
{
    UI32_T  col_idx, index;

    if (NULL == data)
        return;

    if (TRUE == dont_dump_if_empty)
    {
        for (index = 0; size > index; ++index, ++data)
        {
            if (0 != *data)
                break;
        }

        if (size <= index)
        {
            BACKDOOR_MGR_Printf("\r\n\t<< EMPTY >>");
            return;
        }
    }

    for (col_idx = 0, index = 0; size > index; ++col_idx, ++index, ++data)
    {
        if (16 <= col_idx)
            col_idx = 0;

        if (0 == col_idx)
            BACKDOOR_MGR_Printf("\r\n");

        BACKDOOR_MGR_Printf("%02X ", *data);
    }
}

/*--------------------------------------------------------------------------
 * ROUTINE NAME - L4_Backdoor_ShowMacAddr
 *---------------------------------------------------------------------------
 * PURPOSE  : Show MAC address
 * INPUT    : addr  - MAC address
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *---------------------------------------------------------------------------
 */
static void L4_Backdoor_ShowMacAddr(UI8_T *addr)
{
    if (NULL == addr)
        return;

    BACKDOOR_MGR_Printf("%02x-%02x-%02x-%02x-%02x-%02x",
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

static void L4_Backdoor_ShowIpAddr(UI8_T *addr)
{
    if (NULL == addr)
        return;

    BACKDOOR_MGR_Printf("%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
}

static void L4_Backdoor_ShowRowStatus(UI32_T row_status)
{
    BACKDOOR_MGR_Printf("%s",
        (1 == row_status) ? "active" :
        (2 == row_status) ? "notInService" :
        (3 == row_status) ? "notReady" :
        (4 == row_status) ? "createAndGo" :
        (5 == row_status) ? "createAndWait" :
        (6 == row_status) ? "destroy" :
        "unknown");
}

static void L4_Backdoor_DiffServ_DumpAcl(const RULE_TYPE_Acl_T *acl_entry)
{
    UI32_T  ace_cnt;

    if (NULL == acl_entry)
        return;

    BACKDOOR_MGR_Printf("\r\n        acl type: %s",
        (RULE_TYPE_MAC_ACL == acl_entry->acl_type) ? "mac-acl" :
        (RULE_TYPE_IP_ACL == acl_entry->acl_type) ? "ip-acl" :
        (RULE_TYPE_IP_STD_ACL == acl_entry->acl_type) ? "ip-std-acl" :
        (RULE_TYPE_IP_EXT_ACL == acl_entry->acl_type) ? "ip-ext-acl" :
        (RULE_TYPE_MF == acl_entry->acl_type) ? "mf" :
        (RULE_TYPE_IPV6_ACL == acl_entry->acl_type) ? "ipv6-acl" :
        (RULE_TYPE_IPV6_STD_ACL == acl_entry->acl_type) ? "ipv6-std-acl" :
        (RULE_TYPE_IPV6_EXT_ACL == acl_entry->acl_type) ? "ipv6-ext-acl" :
        "unknown");

    BACKDOOR_MGR_Printf("\r\n            name: %s", acl_entry->acl_name);
    BACKDOOR_MGR_Printf("\r\n       row tatus: ");
    L4_Backdoor_ShowRowStatus(acl_entry->row_status);

    BACKDOOR_MGR_Printf("\r\n------- dump ingress port list -------");
    L4_Backdoor_DumpRawDataInHex(acl_entry->filtering_in_port_list, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST, TRUE);
    BACKDOOR_MGR_Printf("\r\n------- dump egress port list --------");
    L4_Backdoor_DumpRawDataInHex(acl_entry->filtering_out_port_list, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST, TRUE);

    BACKDOOR_MGR_Printf("\r\n class-map count: %lu", acl_entry->class_map_count);
    BACKDOOR_MGR_Printf("\r\n------- dump class-map list -------");
    L4_Backdoor_DumpRawDataInHex(acl_entry->class_map_bit_list, RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_CLASS_MAP_LIST, TRUE);

    BACKDOOR_MGR_Printf("\r\n       ace count: %lu", acl_entry->ace_count);
    BACKDOOR_MGR_Printf("\r\n----------------- ace index list -----------------");

    if (0 == acl_entry->ace_count)
    {
        BACKDOOR_MGR_Printf("\r\n\t<< EMPTY >>");
    }
    else
    {
        for (ace_cnt = 0; ace_cnt < acl_entry->ace_count; ++ace_cnt)
        {
            if ((ace_cnt % 8) == 0)
                BACKDOOR_MGR_Printf("\r\n");
            BACKDOOR_MGR_Printf("(%03lu) ", acl_entry->ace_index_list[ace_cnt]);
        }
    }

#if 0
    BACKDOOR_MGR_Printf("\r\n------------------- instance ---------------------");
    for (unit_cnt = 0; SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK > unit_cnt; ++unit_cnt)
    {
        for (chip_cnt = 0; SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > chip_cnt; ++chip_cnt)
        {
            if (acl_entry->filtering_acl_instance_ref_count[unit_cnt][chip_cnt])
            {
                BACKDOOR_MGR_Printf("\r\nunit(%lu) chip(%lu)", unit_cnt + 1, chip_cnt);
                BACKDOOR_MGR_Printf("\r\nreference count: %u", acl_entry->filtering_acl_instance_ref_count[unit_cnt][chip_cnt]);
            }
            L4_Backdoor_DiffServ_DumpClassInstance(acl_entry->filtering_acl_instance[unit_cnt][chip_cnt]);
        }
    }
#endif

    BACKDOOR_MGR_Printf("\r\n\r\n");

}

static void L4_Backdoor_ShowAceAction (UI8_T access)
{
    if (RULE_TYPE_ACE_PERMIT == access)
        BACKDOOR_MGR_Printf ("/ permit");
    else
        BACKDOOR_MGR_Printf ("/ deny");
}

static void L4_Backdoor_DiffServ_DumpMacAce(const RULE_TYPE_MacAce_Entry_T *mac_ace)
{
    L4_Backdoor_ShowAceAction(mac_ace->access);

    switch (mac_ace->acePktformat)
    {
    case VAL_aclMacAcePktformat_untagged_Eth2:
        BACKDOOR_MGR_Printf("/ untagged-eth2");
        break;

    case VAL_aclMacAcePktformat_untagged802Dot3:
        BACKDOOR_MGR_Printf("/ untagged-802.3");
        break;

    case VAL_aclMacAcePktformat_tagggedEth2:
        BACKDOOR_MGR_Printf("/ tagged-eth2");
        break;

    case VAL_aclMacAcePktformat_tagged802Dot3:
        BACKDOOR_MGR_Printf("/ tagged-802.3");
        break;

    case VAL_aclMacAcePktformat_any:
    default:
        break; /* ignore this field */
    }

    if (FALSE == L4_Backdoor_IsNullMac((UI8_T *)mac_ace->aceSourceMacAddr))
    {
        BACKDOOR_MGR_Printf ("/ sa:");
        L4_Backdoor_ShowMacAddr ((UI8_T *)mac_ace->aceSourceMacAddr);
        BACKDOOR_MGR_Printf (" sa-mask:");
        L4_Backdoor_ShowMacAddr ((UI8_T *)mac_ace->aceSourceMacAddrBitmask);
    }

    if (FALSE == L4_Backdoor_IsNullMac((UI8_T *)mac_ace->aceDestMacAddr))
    {
        BACKDOOR_MGR_Printf ("/ da:");
        L4_Backdoor_ShowMacAddr ((UI8_T *)mac_ace->aceDestMacAddr);
        BACKDOOR_MGR_Printf (" da-mask:");
        L4_Backdoor_ShowMacAddr ((UI8_T *)mac_ace->aceDestMacAddrBitmask);
    }

    if (mac_ace->aceVidOp == VAL_diffServMacAceVidOp_equal)
    {
        BACKDOOR_MGR_Printf ("/ vid %04lx vid-mask %04lx", mac_ace->aceMinVid, mac_ace->aceVidBitmask);
    }

    if (mac_ace->aceEtherTypeOp == VAL_diffServMacAceEtherTypeOp_equal)
    {
        BACKDOOR_MGR_Printf ("/ eth %04lx eth-mask %04lx", mac_ace->aceMinEtherType, mac_ace->aceEtherTypeBitmask);
    }
}

static void L4_Backdoor_DiffServ_DumpIpAce(const RULE_TYPE_IpAce_Entry_T *ip_ace)
{
    L4_Backdoor_ShowAceAction(ip_ace->access);

    if(ip_ace->aceSourceIpAddrBitmask != RULE_TYPE_ACL_IP_BITMASK_ANY)
    {
        BACKDOOR_MGR_Printf ("/ sip:");
        L4_Backdoor_ShowIpAddr ((UI8_T *)&ip_ace->aceSourceIpAddr);
        BACKDOOR_MGR_Printf (" sip-mask:");
        L4_Backdoor_ShowIpAddr ((UI8_T *)&ip_ace->aceSourceIpAddrBitmask);
    }

    if(ip_ace->aceDestIpAddrBitmask != RULE_TYPE_ACL_IP_BITMASK_ANY)
    {
        BACKDOOR_MGR_Printf ("/ dip:");
        L4_Backdoor_ShowIpAddr ((UI8_T *)&ip_ace->aceDestIpAddr);
        BACKDOOR_MGR_Printf (" dip-mask:");
        L4_Backdoor_ShowIpAddr ((UI8_T *)&ip_ace->aceDestIpAddrBitmask);
    }

    if (ip_ace->aceProtocol != RULE_TYPE_UNDEF_IP_PROTOCOL)
    {
        BACKDOOR_MGR_Printf ("/ pro %02lx pro-mask %02x", ip_ace->aceProtocol, 0xFF);
    }

    if (ip_ace->acePrec != RULE_TYPE_UNDEF_IP_PRECEDENCE)
    {
        BACKDOOR_MGR_Printf ("/ prec %02lx prec-mask %02x", ip_ace->acePrec, 0x7);
    }

    if (ip_ace->aceTos != RULE_TYPE_UNDEF_IP_TOS)
    {
        BACKDOOR_MGR_Printf ("/ prec %02lx prec-mask %02x", ip_ace->aceTos, 0xf);
    }

    if (ip_ace->aceDscp != RULE_TYPE_UNDEF_IP_DSCP)
    {
        BACKDOOR_MGR_Printf ("/ dscp %02lx dscp-mask %02x", ip_ace->aceDscp, 63);
    }

    if (ip_ace->aceSourcePortOp == VAL_diffServIpAceSourcePortOp_equal) /* currently only support this operator */
    {
        BACKDOOR_MGR_Printf ("/ sport %04lx sport-mask %04lx", ip_ace->aceMinSourcePort, ip_ace->aceSourcePortBitmask);
    }

    if (ip_ace->aceDestPortOp == VAL_diffServIpAceDestPortOp_equal) /* currently only support this operator */
    {
        BACKDOOR_MGR_Printf ("/ dport %04lx dport-mask %04lx", ip_ace->aceMinDestPort, ip_ace->aceDestPortBitmask);
    }

    if (ip_ace->aceControlCode != RULE_TYPE_UNDEF_IP_CONTROL_CODE)
    {
        BACKDOOR_MGR_Printf ("/ ctrl %02lx ctrl-mask %02lx", ip_ace->aceControlCode, ip_ace->aceControlCodeBitmask);
    }

    {
        /* BCM_FIELD_PKT_FMT_IPV4 */
        BACKDOOR_MGR_Printf ("/ [pkformat %02x pktformat-mask %02x]", 0x0010, 0x0010);
    }
}

static BOOL_T L4_Backdoor_IsNullIpv6Addr(const UI8_T *ipv6_addr)
{
    UI8_T   null_ipv6[SYS_ADPT_IPV6_ADDR_LEN];

    memset(null_ipv6, 0, SYS_ADPT_IPV6_ADDR_LEN);

    return (0 == memcmp(ipv6_addr, null_ipv6, SYS_ADPT_IPV6_ADDR_LEN));
}

static void L4_Backdoor_ShowIpv6Addr(UI8_T *addr, UI8_T pfxlen)
{
    UI32_T idx;

    if (NULL == addr)
        return;

    for (idx =0; idx <SYS_ADPT_IPV6_ADDR_LEN; idx++)
    {
      BACKDOOR_MGR_Printf("%02x", addr[idx]);

      if (((idx %4)==3) && (idx != SYS_ADPT_IPV6_ADDR_LEN-1))
        BACKDOOR_MGR_Printf ("-");
    }

    BACKDOOR_MGR_Printf(" /0x%02x", pfxlen);
}

static void L4_Backdoor_DiffServ_DumpIpv6Ace(const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace)
{
    L4_Backdoor_ShowAceAction(ipv6_ace->access);

    if (FALSE == L4_Backdoor_IsNullIpv6Addr(ipv6_ace->aceSourceIpAddr))
    {
        BACKDOOR_MGR_Printf ("/ sipv6:");
        L4_Backdoor_ShowIpv6Addr ((UI8_T *) ipv6_ace->aceSourceIpAddr, ipv6_ace->aceSourceIpAddrPrefixLen);
    }

    if (FALSE == L4_Backdoor_IsNullIpv6Addr(ipv6_ace->aceDestIpAddr))
    {
        BACKDOOR_MGR_Printf ("/ dipv6:");
        L4_Backdoor_ShowIpv6Addr ((UI8_T *) ipv6_ace->aceDestIpAddr, ipv6_ace->aceDestIpAddrPrefixLen);
    }

    if (ipv6_ace->aceNextHeader != RULE_TYPE_UNDEF_IPV6_NEXT_HEADER)
    {
        BACKDOOR_MGR_Printf ("/ nh %04x nh-mask %04x", ipv6_ace->aceNextHeader, 0xffff);
    }

    if (ipv6_ace->aceDscp != RULE_TYPE_UNDEF_IPV6_DSCP)
    {
        BACKDOOR_MGR_Printf ("/ tc %02lx tc-mask %02x", ipv6_ace->aceDscp, 63);
    }

    if (ipv6_ace->aceFlowLabel != RULE_TYPE_UNDEF_IPV6_FLOW_LABEL)
    {
        /* flow-label only has 20 bits */
        BACKDOOR_MGR_Printf ("/ fl %05lx fl-mask %05x", ipv6_ace->aceFlowLabel, 0xFFFFF);
    }

    {
        /* BCM_FIELD_PKT_FMT_IPV6 */
        BACKDOOR_MGR_Printf ("/ [pkformat %02x pktformat-mask %02x]", 0x0020, 0x0020);
    }
}

static void L4_Backdoor_DiffServ_DumpAce(const RULE_TYPE_Ace_Entry_T *ace_entry)
{
    char   *tag_ary[] = {"mac", "ip", "ip-std", "ip-ext", "mf",
                          "ipv6", "ipv6-std", "ipv6-ext"};

    if (ace_entry->ace_type <= RULE_TYPE_IPV6_EXT_ACL)
    {
        BACKDOOR_MGR_Printf ("\r\n ace_type: %s-ace\r\n", tag_ary[ace_entry->ace_type]);

        switch (ace_entry->ace_type)
        {
        case RULE_TYPE_MAC_ACL:
            L4_Backdoor_DiffServ_DumpMacAce (&ace_entry->u.mac);
            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            L4_Backdoor_DiffServ_DumpIpAce (&ace_entry->u.ip);
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            L4_Backdoor_DiffServ_DumpIpv6Ace (&ace_entry->u.ipv6);
            break;

        case RULE_TYPE_MF:
        default:
            BACKDOOR_MGR_Printf ("NA");
            break;
        }
    }

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    BACKDOOR_MGR_Printf ("/ time range index : %d ", ace_entry->time_range_index);
#endif /* #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */


    BACKDOOR_MGR_Printf ("\r\n");
}

static void L4_Backdoor_DiffServ_DumpRuleInstance(BOOL_T show_title, const RULE_TYPE_RuleInstance_T *rule_instance)
{
    const char title_str[] = "\r\nrule      meter     action    ace ";
    const char edge_line[] = "\r\n--------- --------- --------- ---------";
    const char data_fmt[] = "\r\n%9u%10u%10u%10u";

    if (TRUE == show_title)
    {
        BACKDOOR_MGR_Printf((char *)title_str);
        BACKDOOR_MGR_Printf((char *)edge_line);
    }

    if (NULL == rule_instance)
        return;

    BACKDOOR_MGR_Printf((char *)data_fmt, rule_instance->dev_rule_id, rule_instance->meter_id,
        rule_instance->action_id, rule_instance->ace_id);
}

enum
{
    RULE_TYPE_GETNEXT_GETTING_ACL,
    RULE_TYPE_GETNEXT_GETTING_POLICY_MAP
} RULE_TYPE_GetNext_ClassInstanceStatus;

typedef struct
{
    UI32_T  uport_ifindex;                      /* input */
    RULE_TYPE_InOutDirection_T inout_profile;   /* input */

    RULE_TYPE_ClassInstance_T  *class_instance; /* result */

    UI32_T  __status;
    union
    {
        UI32_T  __precedence;

        struct
        {
            UI16_T  __policy_index;
            UI16_T  __class_index;
        };
    };
} RULE_TYPE_GetNext_ClassInstance_T;

static UI32_T RULE_OM_GetNext_ClassInstanceInit(
    RULE_TYPE_GetNext_ClassInstance_T *ci,
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile
    )
{
    ci->uport_ifindex = ifindex;
    ci->inout_profile = inout_profile;
    ci->__status = RULE_TYPE_GETNEXT_GETTING_ACL;
    ci->__precedence = 0;
    return RULE_TYPE_OK;
}

static UI32_T RULE_OM_GetNext_ClassInstance(
    RULE_TYPE_GetNext_ClassInstance_T *ci
    )
{
    RULE_TYPE_PortEntry_T *port_entry =
        RULE_OM_LocalGetPortEntryByIfIndex(ci->uport_ifindex, ci->inout_profile);

    if (NULL == port_entry)
    {
        return RULE_TYPE_FAIL;
    }

    if (RULE_TYPE_GETNEXT_GETTING_ACL != ci->__status &&
        RULE_TYPE_GETNEXT_GETTING_POLICY_MAP != ci->__status)
    {
        return RULE_TYPE_FAIL;
    }

    if (RULE_TYPE_GETNEXT_GETTING_ACL == ci->__status)
    {
        do
        {
            ci->class_instance =
                RULE_OM_LocalGetNextPortAclInstance(port_entry, ci->__precedence);

            ci->__precedence ++;

            if (EGRESS_IPV6_ACL_PRECEDENCE < ci->__precedence)
            {
                ci->__status = RULE_TYPE_GETNEXT_GETTING_POLICY_MAP;
                ci->__policy_index = 0;
                ci->__class_index  = 0;
                break;
            }

        } while (NULL == ci->class_instance);

        if (ci->class_instance)
        {
            return RULE_TYPE_OK;
        }
    }

    if (RULE_TYPE_GETNEXT_GETTING_POLICY_MAP == ci->__status)
    {
        for (; ci->__policy_index<SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP; ci->__policy_index++)
        {
            for(; ci->__class_index<SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_NBR_OF_POLICY_MAP; ci->__class_index++)
            {
                ci->class_instance =
                    RULE_OM_GetPolicyMapInstance(port_entry, ci->__policy_index, ci->__class_index);

                if (ci->class_instance)
                {
                    ci->__class_index ++;
                    return RULE_TYPE_OK;
                }
            }
        }
    }

    return RULE_TYPE_FAIL;
}

static RULE_OM_BACKDOOR_ShowClassInstance()
{
    UI32_T ifindex;
    RULE_TYPE_InOutDirection_T inout_profile;

    BACKDOOR_MGR_Printf("\r\n");

    for (ifindex = 1; ifindex < SYS_ADPT_TOTAL_NBR_OF_LPORT; ifindex++)
    {
        for (inout_profile = RULE_TYPE_INBOUND; inout_profile <= RULE_TYPE_OUTBOUND; inout_profile++)
        {
            RULE_TYPE_Acl_T *acl = NULL;
            RULE_TYPE_Ace_Entry_T *mf_ace = NULL;

            RULE_TYPE_GetNext_ClassInstance_T ci;

            RULE_OM_GetNext_ClassInstanceInit(&ci, ifindex, inout_profile);

            while (RULE_TYPE_OK == RULE_OM_GetNext_ClassInstance(&ci))
            {
                BACKDOOR_MGR_Printf("         ifindex: %lu \r\n", ci.uport_ifindex);
                BACKDOOR_MGR_Printf("       direction: %s \r\n",
                    RULE_TYPE_INBOUND == ci.inout_profile ? "Ingress" : "Egress");

                BACKDOOR_MGR_Printf("      class_type: %s  \r\n",
                    ci.class_instance->class_template.class_type == RULE_TYPE_CLASS_ACL ? "ACL" : "MF");
                BACKDOOR_MGR_Printf("     class_index: %lu \r\n",
                    ci.class_instance->class_template.class_id);

                if (RULE_TYPE_CLASS_ACL == ci.class_instance->class_template.class_type)
                {
                    UI32_T  ace_cnt;

                    for (ace_cnt = 0; ace_cnt < ci.class_instance->rule_num; ++ace_cnt)
                        L4_Backdoor_DiffServ_DumpRuleInstance((0 == ace_cnt), &ci.class_instance->instance.acl_instance[ace_cnt]);
                }
                else
                {
                    L4_Backdoor_DiffServ_DumpRuleInstance(TRUE, &ci.class_instance->instance.mf_rule_id);
                }

                if (RULE_TYPE_CLASS_ACL == ci.class_instance->class_template.class_type)
                {
                    acl = RULE_OM_LocalGetAclByIndex(
                        ci.class_instance->class_template.class_id);
                }
                else
                {
                    mf_ace = RULE_OM_LocalGetAceByIndex(
                        ci.class_instance->class_template.class_id);
                }

                if (NULL != acl)
                {
                    L4_Backdoor_DiffServ_DumpAcl(acl);
                }

                if (NULL != mf_ace)
                {
                    L4_Backdoor_DiffServ_DumpAce(mf_ace);
                }
            }

        } // for dir
    } // for ifindex

    BACKDOOR_MGR_Printf("\r\n");
}

void RULE_OM_BACKDOOR_Main()
{
    RULE_OM_BACKDOOR_ShowClassInstance();
}

