//
//  dev_rm_lib.c
//  l4_mgr
//
//  Created by yehjunying on 13/3/21.
//
//

#include <memory.h>
#include "dev_rm_lib.h"
#include "l_stdlib.h"

#ifndef ASSERT
#define ASSERT(eq)
#endif

void
DEVRM_LIB_RulePatternInit(
    DEVRM_AceEntry_T *rule_pattern_p)
{
    ASSERT(NULL != rule_pattern_p);

    memset(rule_pattern_p, 0, sizeof(*rule_pattern_p));
}

BOOL_T
DEVRM_LIB_RulePatternQualifyDestMacAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    const UI8_T *data_p,
    const UI8_T *mask_p)
{
    ASSERT(NULL != rule_pattern_p);
    ASSERT(NULL != data_p);
    ASSERT(NULL != mask_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_MACDA;
    memcpy(rule_pattern_p->dstmac_data, data_p, SYS_ADPT_MAC_ADDR_LEN);
    memcpy(rule_pattern_p->dstmac_mask, mask_p, SYS_ADPT_MAC_ADDR_LEN);

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifySourceMacAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    const UI8_T *data_p,
    const UI8_T *mask_p)
{
    ASSERT(NULL != rule_pattern_p);
    ASSERT(NULL != data_p);
    ASSERT(NULL != mask_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_MACSA;
    memcpy(rule_pattern_p->srcmac_data, data_p, SYS_ADPT_MAC_ADDR_LEN);
    memcpy(rule_pattern_p->srcmac_mask, mask_p, SYS_ADPT_MAC_ADDR_LEN);

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyEtherType(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI16_T data,
    UI16_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ETHER;
    rule_pattern_p->ethertype_data = data;
    rule_pattern_p->ethertype_mask = mask;

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIp(
    DEVRM_AceEntry_T *rule_pattern_p)
{
    return DEVRM_LIB_RulePatternQualifyEtherType(rule_pattern_p, 0x0800, 0xffff);
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIpv6(
    DEVRM_AceEntry_T *rule_pattern_p)
{
    return DEVRM_LIB_RulePatternQualifyEtherType(rule_pattern_p, 0x86dd, 0xffff);
}

BOOL_T
DEVRM_LIB_RulePatternQualifyVlan(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI16_T data,
    UI16_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_VID;
    rule_pattern_p->outvlan_data = data;
    rule_pattern_p->outvlan_mask = mask;

    //{{{{{
    rule_pattern_p->invlan_data = data;
    rule_pattern_p->invlan_mask = mask;
    //}}}}}

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyCos(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_VTAGUP;
    rule_pattern_p->vtagup_data = data;
    rule_pattern_p->vtagup_mask = mask;

    rule_pattern_p->is_tagged = 1;

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIpProtocol(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL;
    rule_pattern_p->ipprotocol_data = data;
    rule_pattern_p->ipprotocol_mask = mask;

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIpDscp(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_DSCP;
    rule_pattern_p->dscp_data = data;
    rule_pattern_p->dscp_mask = mask;

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIpv6NextHeader(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask)
{
    return DEVRM_LIB_RulePatternQualifyIpProtocol(rule_pattern_p, data, mask);
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIcmp(
    DEVRM_AceEntry_T *rule_pattern_p)
{
    enum
    {
        RULE_CTRL_PROTOCOL_ICMP = 1,
    };

    return DEVRM_LIB_RulePatternQualifyIpProtocol(rule_pattern_p,
                                          RULE_CTRL_PROTOCOL_ICMP,
                                          0xff);
}

BOOL_T
DEVRM_LIB_RulePatternQualifyUdp(
    DEVRM_AceEntry_T *rule_pattern_p)
{
    return DEVRM_LIB_RulePatternQualifyIpProtocol(rule_pattern_p,
                                          SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_UDP,
                                          0xff);
}

BOOL_T
DEVRM_LIB_RulePatternQualifyTcp(
    DEVRM_AceEntry_T *rule_pattern_p)
{
    return DEVRM_LIB_RulePatternQualifyIpProtocol(rule_pattern_p,
                                          SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_TCP,
                                          0xff);
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIpDestAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI32_T data,
    UI32_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV4DIP;
    rule_pattern_p->dstip_data = data;
    rule_pattern_p->dstip_mask = mask;

    DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p);

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIpSourceAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI32_T data,
    UI32_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV4SIP;
    rule_pattern_p->srcip_data = data;
    rule_pattern_p->srcip_mask = mask;

    DEVRM_LIB_RulePatternQualifyIp(rule_pattern_p);

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIpv6DestAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    const UI8_T *data_p,
    const UI8_T *mask_p)
{
    ASSERT(NULL != rule_pattern_p);
    ASSERT(NULL != data_p);
    ASSERT(NULL != mask_p);

#if (SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN == 8)
    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6DIPUPPER;
#else
    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6DIP;
#endif
    memcpy(rule_pattern_p->dstip6_data, data_p, SYS_ADPT_IPV6_ADDR_LEN);
    memcpy(rule_pattern_p->dstip6_mask, mask_p, SYS_ADPT_IPV6_ADDR_LEN);

    DEVRM_LIB_RulePatternQualifyIpv6(rule_pattern_p);

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIpv6SourceAddress(
    DEVRM_AceEntry_T *rule_pattern_p,
    const UI8_T *data_p,
    const UI8_T *mask_p)
{
    ASSERT(NULL != rule_pattern_p);
    ASSERT(NULL != data_p);
    ASSERT(NULL != mask_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6SIP;
    memcpy(rule_pattern_p->srcip6_data, data_p, SYS_ADPT_IPV6_ADDR_LEN);
    memcpy(rule_pattern_p->srcip6_mask, mask_p, SYS_ADPT_IPV6_ADDR_LEN);

    DEVRM_LIB_RulePatternQualifyIpv6(rule_pattern_p);

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyL4DestPort(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI16_T data,
    UI16_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_L4DSTPORT;
    rule_pattern_p->l4_dstport_data = data;
    rule_pattern_p->l4_dstport_mask = mask;

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyL4SourcePort(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI16_T data,
    UI16_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT;
    rule_pattern_p->l4_srcport_data = data;
    rule_pattern_p->l4_srcport_mask = mask;

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyTcpControlFlags(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL;
    rule_pattern_p->tcpcontrol_data = data;
    rule_pattern_p->tcpcontrol_mask = mask;

    DEVRM_LIB_RulePatternQualifyTcp(rule_pattern_p);

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIcmpType(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE;
    rule_pattern_p->icmptype_data = data;
    rule_pattern_p->icmptype_mask = mask;

    DEVRM_LIB_RulePatternQualifyIcmp(rule_pattern_p);

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIcmpv6(
    DEVRM_AceEntry_T *rule_pattern_p)
{
    enum
    {
        RULE_CTRL_PROT_ICMP6 = 58
    };

    DEVRM_LIB_RulePatternQualifyIpv6NextHeader(rule_pattern_p, RULE_CTRL_PROT_ICMP6, 0xff);
    DEVRM_LIB_RulePatternQualifyIpv6(rule_pattern_p);

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIcmpv6Type(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE;
    rule_pattern_p->icmptype_data = data;
    rule_pattern_p->icmptype_mask = mask;

    DEVRM_LIB_RulePatternQualifyIcmpv6(rule_pattern_p);

    return TRUE;
}

BOOL_T
DEVRM_LIB_RulePatternQualifyIpFragment(
    DEVRM_AceEntry_T *rule_pattern_p,
    UI8_T data,
    UI8_T mask)
{
    ASSERT(NULL != rule_pattern_p);

    rule_pattern_p->entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPFRAG;
    rule_pattern_p->ip_fragment_data = data;
    rule_pattern_p->ip_fragment_mask = mask;
    
    DEVRM_LIB_RulePatternQualifyIcmp(rule_pattern_p);
    
    return TRUE;
}

void
DEVRM_LIB_RulePatternDump(
    const DEVRM_AceEntry_T *rule_pattern_p,
    BOOL_T (*printf_fn)(char*, ...),
    UI32_T indent)
{
    typedef union
    {
        UI32_T  u32Ip;
        UI8_T   arIP[4];
    } IPADDR;

    char *prefix = "pattern";

    ASSERT(NULL != rule_pattern_p);

    printf_fn("%*s" "+-o %s\r\n", indent, "", prefix);

    indent += 4;
    printf_fn("%*s" "{\r\n", indent, "");

    indent += 2;

    if (NULL == rule_pattern_p)
    {
        printf_fn("%*s" "nil\r\n", indent, "");

        indent -= 2;
        printf_fn("%*s" "}\r\n", indent, "");
        return;
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACSA)
    {
        printf_fn("%*s"
                  "patt.sa = 0x%02X%02X%02X%02X%02X%02X\r\n",
                  indent, "",
                  rule_pattern_p->srcmac_data[0],
                  rule_pattern_p->srcmac_data[1],
                  rule_pattern_p->srcmac_data[2],
                  rule_pattern_p->srcmac_data[3],
                  rule_pattern_p->srcmac_data[4],
                  rule_pattern_p->srcmac_data[5]);

        printf_fn("%*s"
                  "mask.sa = 0x%02X%02X%02X%02X%02X%02X\r\n",
                  indent, "",
                  rule_pattern_p->srcmac_mask[0],
                  rule_pattern_p->srcmac_mask[1],
                  rule_pattern_p->srcmac_mask[2],
                  rule_pattern_p->srcmac_mask[3],
                  rule_pattern_p->srcmac_mask[4],
                  rule_pattern_p->srcmac_mask[5]);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACDA)
    {
        printf_fn("%*s"
                  "patt.da = 0x%02X%02X%02X%02X%02X%02X\r\n",
                  indent, "",
                  rule_pattern_p->dstmac_data[0],
                  rule_pattern_p->dstmac_data[1],
                  rule_pattern_p->dstmac_data[2],
                  rule_pattern_p->dstmac_data[3],
                  rule_pattern_p->dstmac_data[4],
                  rule_pattern_p->dstmac_data[5]);

        printf_fn("%*s"
                  "mask.da = 0x%02X%02X%02X%02X%02X%02X\r\n",
                  indent, "",
                  rule_pattern_p->dstmac_mask[0],
                  rule_pattern_p->dstmac_mask[1],
                  rule_pattern_p->dstmac_mask[2],
                  rule_pattern_p->dstmac_mask[3],
                  rule_pattern_p->dstmac_mask[4],
                  rule_pattern_p->dstmac_mask[5]);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_UP ||
        rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP)
    {
        printf_fn("%*s"
                  "patt.vtagup = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->vtagup_data,
                  rule_pattern_p->vtagup_data);

        printf_fn("%*s"
                  "mask.vtagup = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->vtagup_mask,
                  rule_pattern_p->vtagup_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TAGGED)
    {
        printf_fn("%*s"
                  "patt.isTagged = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->is_tagged,
                  rule_pattern_p->is_tagged);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VID)
    {
        printf_fn("%*s"
                  "patt.vid = 0x%04hX(%hu)\r\n",
                  indent, "",
                  rule_pattern_p->outvlan_data,
                  rule_pattern_p->outvlan_data);

        printf_fn("%*s"
                  "mask.vid = 0x%04hX(%hu)\r\n",
                  indent, "",
                  rule_pattern_p->outvlan_mask,
                  rule_pattern_p->outvlan_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ETHER)
    {
        printf_fn("%*s"
                  "patt.ethType = 0x%04hX\r\n",
                  indent, "",
                  rule_pattern_p->ethertype_data);

        printf_fn("%*s"
                  "mask.ethType = 0x%04hX\r\n",
                  indent, "",
                  rule_pattern_p->ethertype_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ENCAP)
    {
        printf_fn("%*s"
                  "patt.encap = 0x%08lX(%s)\r\n",
                  indent, "",
                  rule_pattern_p->packetformat_data,
                  ETHERTYPE_ENCAPS == rule_pattern_p->packetformat_data ? "ENCAPS" : "");

        printf_fn("%*s"
                  "mask.encap = 0x%08lX(%s)\r\n",
                  indent, "",
                  rule_pattern_p->packetformat_mask,
                  ETHERTYPE_ENCAPS == rule_pattern_p->packetformat_mask ? "ENCAPS" : "");
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPFRAG)
    {
        printf_fn("%*s"
                  "patt.ip_fragment = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->ip_fragment_data,
                  rule_pattern_p->ip_fragment_data);

        printf_fn("%*s"
                  "mask.ip_fragment = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->ip_fragment_mask,
                  rule_pattern_p->ip_fragment_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL)
    {
        printf_fn("%*s"
                  "patt.ipProtocol = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->ipprotocol_data,
                  rule_pattern_p->ipprotocol_data);

        printf_fn("%*s"
                  "mask.ipProtocol = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->ipprotocol_mask,
                  rule_pattern_p->ipprotocol_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_DSCP)
    {
        printf_fn("%*s"
                  "patt.dscp = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->dscp_data,
                  rule_pattern_p->dscp_data);

        printf_fn("%*s"
                  "mask.dscp = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->dscp_mask,
                  rule_pattern_p->dscp_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PREC)
    {
        printf_fn("%*s"
                  "patt.precedence = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->prec_data,
                  rule_pattern_p->prec_data);

        printf_fn("%*s"
                  "mask.Precedence = 0x%02X(%u)\r\n",
                  indent, "",
                  rule_pattern_p->prec_mask,
                  rule_pattern_p->prec_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE)
    {
        printf_fn("%*s"
                  "patt.icmp.type = 0x%02X(%u)\r\n",
                  indent, "",
                  (UI16_T)rule_pattern_p->icmptype_data,
                  (UI16_T)rule_pattern_p->icmptype_data);

        printf_fn("%*s"
                  "mask.icmp.type = 0x%02X(%u)\r\n",
                  indent, "",
                  (UI16_T)rule_pattern_p->icmptype_mask,
                  (UI16_T)rule_pattern_p->icmptype_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV4SIP)
    {
        IPADDR sip;

        sip.u32Ip = L_STDLIB_Hton32(rule_pattern_p->srcip_data);

        printf_fn("%*s"
                  "patt.sip = 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n",
                  indent, "",
                  sip.arIP[0],
                  sip.arIP[1],
                  sip.arIP[2],
                  sip.arIP[3],
                  sip.arIP[0],
                  sip.arIP[1],
                  sip.arIP[2],
                  sip.arIP[3]);

        sip.u32Ip = L_STDLIB_Hton32(rule_pattern_p->srcip_mask);

        printf_fn("%*s"
                  "mask.sip = 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n",
                  indent, "",
                  sip.arIP[0],
                  sip.arIP[1],
                  sip.arIP[2],
                  sip.arIP[3],
                  sip.arIP[0],
                  sip.arIP[1],
                  sip.arIP[2],
                  sip.arIP[3]);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV4DIP)
    {
        IPADDR dip;

        dip.u32Ip = L_STDLIB_Hton32(rule_pattern_p->dstip_data);

        printf_fn("%*s"
                  "patt.dip = 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n",
                  indent, "",
                  dip.arIP[0],
                  dip.arIP[1],
                  dip.arIP[2],
                  dip.arIP[3],
                  dip.arIP[0],
                  dip.arIP[1],
                  dip.arIP[2],
                  dip.arIP[3]);

        dip.u32Ip = L_STDLIB_Hton32(rule_pattern_p->dstip_mask);

        printf_fn("%*s"
                  "mask.dip = 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n",
                  indent, "",
                  dip.arIP[0],
                  dip.arIP[1],
                  dip.arIP[2],
                  dip.arIP[3],
                  dip.arIP[0],
                  dip.arIP[1],
                  dip.arIP[2],
                  dip.arIP[3]);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT)
    {
        printf_fn("%*s"
                  "patt.l4.sport = 0x%04hX(%hu)\r\n",
                  indent, "",
                  (UI16_T)rule_pattern_p->l4_srcport_data,
                  (UI16_T)rule_pattern_p->l4_srcport_data);

        printf_fn("%*s"
                  "mask.l4.sport = 0x%04hX(%hu)\r\n",
                  indent, "",
                  (UI16_T)rule_pattern_p->l4_srcport_mask,
                  (UI16_T)rule_pattern_p->l4_srcport_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4DSTPORT)
    {
        printf_fn("%*s"
                  "patt.l4.dport = 0x%04hX(%hu)\r\n",
                  indent, "",
                  (UI16_T)rule_pattern_p->l4_dstport_data,
                  (UI16_T)rule_pattern_p->l4_dstport_data);

        printf_fn("%*s"
                  "mask.l4.dport = 0x%04hX(%hu)\r\n",
                  indent, "",
                  (UI16_T)rule_pattern_p->l4_dstport_mask,
                  (UI16_T)rule_pattern_p->l4_dstport_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL)
    {
        printf_fn("%*s"
                  "patt.tcp.control = 0x%02X(%u)\r\n",
                  indent, "",
                  (UI16_T)rule_pattern_p->tcpcontrol_data,
                  (UI16_T)rule_pattern_p->tcpcontrol_data);

        printf_fn("%*s"
                  "mask.tcp.control = 0x%02X(%u)\r\n",
                  indent, "",
                  (UI16_T)rule_pattern_p->tcpcontrol_mask,
                  (UI16_T)rule_pattern_p->tcpcontrol_mask);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6EHHOPBYHOP)
    {
        printf_fn("%*s"
                  "patt.ipv6.hopByHop = 1\r\n",
                  indent, "");
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6EHEXIST)
    {
        printf_fn("%*s"
                  "patt.ipv6.hdrExist = 1\r\n",
                  indent, "");
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6SIP)
    {
        printf_fn("%*s"
                  "patt.ipv6.sip = 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
                  indent, "",
                  rule_pattern_p->srcip6_data[0],
                  rule_pattern_p->srcip6_data[1],
                  rule_pattern_p->srcip6_data[2],
                  rule_pattern_p->srcip6_data[3],
                  rule_pattern_p->srcip6_data[4],
                  rule_pattern_p->srcip6_data[5],
                  rule_pattern_p->srcip6_data[6],
                  rule_pattern_p->srcip6_data[7],
                  rule_pattern_p->srcip6_data[8],
                  rule_pattern_p->srcip6_data[9],
                  rule_pattern_p->srcip6_data[10],
                  rule_pattern_p->srcip6_data[11],
                  rule_pattern_p->srcip6_data[12],
                  rule_pattern_p->srcip6_data[13],
                  rule_pattern_p->srcip6_data[14],
                  rule_pattern_p->srcip6_data[15]);

        printf_fn("%*s"
                  "mask.ipv6.sip = 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
                  indent, "",
                  rule_pattern_p->srcip6_mask[0],
                  rule_pattern_p->srcip6_mask[1],
                  rule_pattern_p->srcip6_mask[2],
                  rule_pattern_p->srcip6_mask[3],
                  rule_pattern_p->srcip6_mask[4],
                  rule_pattern_p->srcip6_mask[5],
                  rule_pattern_p->srcip6_mask[6],
                  rule_pattern_p->srcip6_mask[7],
                  rule_pattern_p->srcip6_mask[8],
                  rule_pattern_p->srcip6_mask[9],
                  rule_pattern_p->srcip6_mask[10],
                  rule_pattern_p->srcip6_mask[11],
                  rule_pattern_p->srcip6_mask[12],
                  rule_pattern_p->srcip6_mask[13],
                  rule_pattern_p->srcip6_mask[14],
                  rule_pattern_p->srcip6_mask[15]);
    }

    if (rule_pattern_p->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6DIPUPPER)
    {
        printf_fn("%*s"
                  "patt.ipv6.dipUpper = 0x%02X\r\n",
                  indent, "",
                  rule_pattern_p->dstip6_data[0]);

        printf_fn("%*s"
                  "mask.ipv6.dipUpper = 0x%02X\r\n",
                  indent, "",
                  rule_pattern_p->dstip6_mask[0]);
    }

    indent -= 2;
    printf_fn("%*s" "}\r\n", indent, "");
}
