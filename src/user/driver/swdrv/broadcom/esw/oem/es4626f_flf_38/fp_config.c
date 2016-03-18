/* FUNCTION NAME: FP_CONFIG.C
* PURPOSE:
*   manage Logical Rule(om) and Physical Rule(Chip) mapping
*
* NOTES:
*
*REASON:
*    DESCRIPTION:
*    CREATOR:       Jinfeng Chen
*    Date:          2008-10-08
*
* Copyright(C)      Accton Corporation, 2008
*/
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "sys_cpnt.h"
#include "sys_adpt.h"
#include "fp_config.h"

#if (DUMP_TABLE == TRUE)
#include "cli_tbl.h"
#endif

/* These definitions are used for short name for preprocessor constant
 * and not chagne the orig value
 */
#define RT_IP_ACL                       RULE_TYPE_INGRESS_IP_ACL
#define RT_MAC_ACL                      RULE_TYPE_INGRESS_MAC_ACL
#define RT_IP_QOS                       RULE_TYPE_IP_DIFFSERV
#define RT_MAC_QOS                      RULE_TYPE_MAC_DIFFSERV
#define RT_QOS_MAP                      RULE_TYPE_TCP_UDP_PORT_MAPPING
#define RT_IFP_PBF                      RULE_TYPE_IFP_PBF
#define RT_PBR                          RULE_TYPE_PBR
#define RT_PPORT_RATELIMIT              RULE_TYPE_INGRESS_PPORT_RATELIMIT
#define RT_IP6_STD_ACL                  RULE_TYPE_INGRESS_IPV6_STD_ACL
#define RT_IP6_EXT_ACL                  RULE_TYPE_INGRESS_IPV6_EXT_ACL
#define RT_IP6_STD_QOS                  RULE_TYPE_INGRESS_IPV6_STD_DIFFSERV
#define RT_IP6_EXT_QOS                  RULE_TYPE_INGRESS_IPV6_EXT_DIFFSERV

#define RT_EGRESS_IP6_STD_ACL           RULE_TYPE_EGRESS_IPV6_STD_ACL
#define RT_EGRESS_IP6_EXT_ACL           RULE_TYPE_EGRESS_IPV6_EXT_ACL
#define RT_EGRESS_IP_ACL                RULE_TYPE_EGRESS_IP_ACL
#define RT_EGRESS_MAC_ACL               RULE_TYPE_EGRESS_MAC_ACL

#define RT_EGRESS_IP6_STD_QOS           RULE_TYPE_EGRESS_IPV6_STD_DIFFSERV
#define RT_EGRESS_IP6_EXT_QOS           RULE_TYPE_EGRESS_IPV6_EXT_DIFFSERV
#define RT_EGRESS_IP_QOS                RULE_TYPE_EGRESS_IP_DIFFSERV
#define RT_EGRESS_MAC_QOS               RULE_TYPE_EGRESS_MAC_DIFFSERV

#define RT_WEBAUTH_H                    RULE_TYPE_WEBAUTH_PRIO_HIGH
#define RT_WEBAUTH_M                    RULE_TYPE_WEBAUTH_PRIO_MED
#define RT_WEBAUTH_L                    RULE_TYPE_WEBAUTH_PRIO_LOW
#define RT_IP_SRC_GUARD_H               RULE_TYPE_IP_SOURCE_GUARD_PRIO_HIGH
#define RT_IP_SRC_GUARD_L               RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW
#define RT_IPV6_SG_PERMIT_HOST          RULE_TYPE_IPV6_SG_PERMIT_HOST
#define RT_IPV6_SG_DENY_ALL             RULE_TYPE_IPV6_SG_DENY_ALL
#define RT_CPU_LINK_LOCAL               RULE_TYPE_PACKET_TO_CPU_LINK_LOCAL_ADDR

#if (SYS_CPNT_IPV6_DENY_LINK_LOCAL_ROUTING_BY_RULE_AS_TEMPORARY_SOLUTION == TRUE)
#define RT_LINK_LOCAL_L3_ROUTABLE       RULE_TYPE_LINK_LOCAL_ADDR_L3_ROUTABLE
#endif /* #if (SYS_CPNT_IPV6_DENY_LINK_LOCAL_ROUTING_BY_RULE_AS_TEMPORARY_SOLUTION == TRUE) */

#define RT_CPU_1                        RULE_TYPE_PACKET_TO_CPU_PRIO_1
#define RT_CPU_2                        RULE_TYPE_PACKET_TO_CPU_PRIO_2
#define RT_CPU_3                        RULE_TYPE_PACKET_TO_CPU_PRIO_3
#define RT_CPU_4                        RULE_TYPE_PACKET_TO_CPU_PRIO_4
#define RT_CPU_5                        RULE_TYPE_PACKET_TO_CPU_PRIO_5
#define RT_CPU_6                        RULE_TYPE_PACKET_TO_CPU_PRIO_6
#define RT_CPU_7                        RULE_TYPE_PACKET_TO_CPU_PRIO_7
#define RT_CPU_8                        RULE_TYPE_PACKET_TO_CPU_PRIO_8
#define RT_CPU_MLD                      RULE_TYPE_PACKET_TO_CPU_MLD
#define RT_PIM6                         RULE_TYPE_PACKET_TO_CPU_PIM6

#define InPort                          DEVRM_FIELD_QUALIFY_InPort
#define InPorts                         DEVRM_FIELD_QUALIFY_InPorts
#define OutPort                         DEVRM_FIELD_QUALIFY_OutPort
#define HiGig                           DEVRM_FIELD_QUALIFY_HiGig

#define OuterVlan                       DEVRM_FIELD_QUALIFY_OuterVlan
#define InnerVlan                       DEVRM_FIELD_QUALIFY_InnerVlan
#define OuterVlanPri                    DEVRM_FIELD_QUALIFY_OuterVlanPri
#define DstMac                          DEVRM_FIELD_QUALIFY_DstMac
#define SrcMac                          DEVRM_FIELD_QUALIFY_SrcMac
#define EtherType                       DEVRM_FIELD_QUALIFY_EtherType
#define IpType                          DEVRM_FIELD_QUALIFY_IpType

#define MHOpcode                        DEVRM_FIELD_QUALIFY_MHOpcode
#define PacketFormat                    DEVRM_FIELD_QUALIFY_PacketFormat
#define VlanFormat                      DEVRM_FIELD_QUALIFY_VlanFormat
#define L2Format                        DEVRM_FIELD_QUALIFY_L2Format
#define StageIngress                    DEVRM_FIELD_QUALIFY_StageIngress
#define StageEgress                     DEVRM_FIELD_QUALIFY_StageEgress

#define LookupStatus                    DEVRM_FIELD_QUALIFY_LookupStatus
#define L3Routable                      DEVRM_FIELD_QUALIFY_L3Routable
#define TunnelTerminated                DEVRM_FIELD_QUALIFY_TunnelTerminated
#define VlanTranslationHit              DEVRM_FIELD_QUALIFY_VlanTranslationHit
#define ForwardingVlanValid             DEVRM_FIELD_QUALIFY_ForwardingVlanValid
#define IngressStpState                 DEVRM_FIELD_QUALIFY_IngressStpState
#define L2SrcHit                        DEVRM_FIELD_QUALIFY_L2SrcHit
#define L2SrcStatic                     DEVRM_FIELD_QUALIFY_L2SrcStatic
#define L2DestHit                       DEVRM_FIELD_QUALIFY_L2DestHit
#define L2StationMove                   DEVRM_FIELD_QUALIFY_L2StationMove
#define L2CacheHit                      DEVRM_FIELD_QUALIFY_L2CacheHit
#define L3SrcHostHit                    DEVRM_FIELD_QUALIFY_L3SrcHostHit
#define L3DestHostHit                   DEVRM_FIELD_QUALIFY_L3DestHostHit
#define L3DestRouteHit                  DEVRM_FIELD_QUALIFY_L3DestRouteHit
#define IpmcStarGroupHit                DEVRM_FIELD_QUALIFY_IpmcStarGroupHit
#define DosAttack                       DEVRM_FIELD_QUALIFY_DosAttack

#define SrcIp                           DEVRM_FIELD_QUALIFY_SrcIp
#define DstIp                           DEVRM_FIELD_QUALIFY_DstIp
#define IpProtocol                      DEVRM_FIELD_QUALIFY_IpProtocol
#define L4SrcPort                       DEVRM_FIELD_QUALIFY_L4SrcPort
#define L4DstPort                       DEVRM_FIELD_QUALIFY_L4DstPort
#define DSCP                            DEVRM_FIELD_QUALIFY_DSCP
#define TcpControl                      DEVRM_FIELD_QUALIFY_TcpControl
#define Ttl                             DEVRM_FIELD_QUALIFY_Ttl

#define IpType                          DEVRM_FIELD_QUALIFY_IpType
#define PacketFormat                    DEVRM_FIELD_QUALIFY_PacketFormat
#define SrcIp6                          DEVRM_FIELD_QUALIFY_SrcIp6
#define DstIp6                          DEVRM_FIELD_QUALIFY_DstIp6
#define DstIp6High                      DEVRM_FIELD_QUALIFY_DstIp6High
#define Ip6TrafficClass                 DEVRM_FIELD_QUALIFY_Ip6TrafficClass
#define Ip6FlowLabel                    DEVRM_FIELD_QUALIFY_Ip6FlowLabel
#define Ip6NextHeader                   DEVRM_FIELD_QUALIFY_Ip6NextHeader
#define DstPortTgid                     DEVRM_FIELD_QUALIFY_DstPortTgid

#define SINGLE                          DEVRM_GROUP_MODE_SINGLE
#define DOUBLE                          DEVRM_GROUP_MODE_DOUBLE
#define TRIPLE                          DEVRM_GROUP_MODE_TRIPLE

#define VFP                             DEVRM_STAGE_VFP
#define IFP                             DEVRM_STAGE_IFP
#define EFP                             DEVRM_STAGE_EFP

#define SELECTOR_0                      DEVRM_IFP_SELECTOR_0
#define SELECTOR_1                      DEVRM_IFP_SELECTOR_1
#define SELECTOR_2                      DEVRM_IFP_SELECTOR_2
#define SELECTOR_3                      DEVRM_IFP_SELECTOR_3
#define SELECTOR_4                      DEVRM_IFP_SELECTOR_4
#define SELECTOR_5                      DEVRM_IFP_SELECTOR_5
#define SELECTOR_6                      DEVRM_IFP_SELECTOR_6
#define SELECTOR_7                      DEVRM_IFP_SELECTOR_7
#define SELECTOR_8                      DEVRM_IFP_SELECTOR_8
#define SELECTOR_9                      DEVRM_IFP_SELECTOR_9
#define SELECTOR_10                     DEVRM_IFP_SELECTOR_10
#define SELECTOR_11                     DEVRM_IFP_SELECTOR_11
#define SELECTOR_12                     DEVRM_IFP_SELECTOR_12
#define SELECTOR_13                     DEVRM_IFP_SELECTOR_13
#define SELECTOR_14                     DEVRM_IFP_SELECTOR_14
#define SELECTOR_15                     DEVRM_IFP_SELECTOR_15

#define PR_1                            BCM_FIELD_ENTRY_PRIO_HIGHEST
#define PR_2                            7
#define PR_3                            6
#define PR_4                            5
#define PR_5                            4
#define PR_6                            3
#define PR_7                            2
#define PR_8                            BCM_FIELD_ENTRY_PRIO_LOWEST

#define PR_HIGHEST                      BCM_FIELD_ENTRY_PRIO_HIGHEST
#define PR_LOWEST                       BCM_FIELD_ENTRY_PRIO_LOWEST

#define PR_DEFAULT                      BCM_FIELD_ENTRY_PRIO_DEFAULT

#if 1400 <= _MSC_VER /* VC2005 */

#if _DEBUG
#define ERR(fmt, ...)                       \
    {                                       \
        printf("ERR Line(%d): ", __LINE__); \
        printf(fmt, __VA_ARGS__);           \
        printf("\n\n");                     \
    }
#else
#define ERR(fmt, ...)                       ;
#endif /* _DEBUG */

#else
#if _DEBUG
#define ERR(fmt, args...)                   \
    {                                       \
        printf("ERR Line(%d): ", __LINE__); \
        printf(fmt, args);                  \
        printf("\n");                       \
    }
#else
#define ERR(fmt, args...)                   ;
#endif /* _DEBUG */
#endif

#define FP_CONFIG_FQ(fq)                {fq, #fq}

/* FP_CONFIG_FUNC_DESC
 * type         - the function type
 * pri          - priority of the function type
 * req_rule_num - rule number of the function type. 0 means don't care.
 */
#define FP_CONFIG_FUNC_DESC(type, pri, req_rule_num)    \
{                                                       \
    type,   pri,    req_rule_num,                       \
    #type,  #pri                                        \
}

#define FP_CONFIG_VECTOR(ary)           {sizeof(ary)/sizeof(ary[0]), ary}
#define FP_CONFIG_NULL_VECTOR()         {0, NULL}
#define FP_CONFIG_STRUCT(gid, gmode, sel_stg, sel_num, fq_vector, udf_vector, func_vector)  \
{                                                                                           \
    gid, gmode, sel_stg, sel_num, fq_vector, udf_vector, func_vector,                       \
    #gmode, #sel_stg, #sel_num                                                              \
}

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_ingress_ipv6_std_acl_diffserv[] = {
    FP_CONFIG_FQ(InPorts),
    FP_CONFIG_FQ(IpType),
    FP_CONFIG_FQ(SrcIp6),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_egress_ipv6_std_acl_diffserv[] = {
    FP_CONFIG_FQ(OutPort),
    FP_CONFIG_FQ(IpType),
    FP_CONFIG_FQ(SrcIp6),
    FP_CONFIG_FQ(StageEgress),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_ingress_ipv6_ext_acl_diffserv[] = {
    FP_CONFIG_FQ(InPorts),
    FP_CONFIG_FQ(IpType),
    FP_CONFIG_FQ(DstIp6High),
    FP_CONFIG_FQ(Ip6TrafficClass),
    FP_CONFIG_FQ(Ip6FlowLabel),
    FP_CONFIG_FQ(Ip6NextHeader),

#if (SYS_CPNT_IPV6_DENY_LINK_LOCAL_ROUTING_BY_RULE_AS_TEMPORARY_SOLUTION == TRUE)
    FP_CONFIG_FQ(LookupStatus),
    FP_CONFIG_FQ(L3Routable),
#endif /* #if (SYS_CPNT_IPV6_DENY_LINK_LOCAL_ROUTING_BY_RULE_AS_TEMPORARY_SOLUTION == TRUE) */
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_egress_ipv6_ext_acl_diffserv[] = {
    FP_CONFIG_FQ(OutPort),
    FP_CONFIG_FQ(IpType),
//    FP_CONFIG_FQ(DstIp6),
    FP_CONFIG_FQ(DstIp6High),
    FP_CONFIG_FQ(StageEgress),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_ingress_ip_acl_diffserv[] = {
    FP_CONFIG_FQ(InPorts),
    FP_CONFIG_FQ(SrcIp),
    FP_CONFIG_FQ(DstIp),
    FP_CONFIG_FQ(IpProtocol),
    FP_CONFIG_FQ(L4SrcPort),
    FP_CONFIG_FQ(L4DstPort),
    FP_CONFIG_FQ(DSCP),
    FP_CONFIG_FQ(TcpControl),
    FP_CONFIG_FQ(PacketFormat),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_egress_ip_acl_diffserv[] = {
    FP_CONFIG_FQ(OutPort),
    FP_CONFIG_FQ(IpType),
    FP_CONFIG_FQ(SrcIp),
    FP_CONFIG_FQ(DstIp),
    FP_CONFIG_FQ(IpProtocol),
    FP_CONFIG_FQ(L4SrcPort),
    FP_CONFIG_FQ(L4DstPort),
    FP_CONFIG_FQ(DSCP),
    FP_CONFIG_FQ(TcpControl),
//    FP_CONFIG_FQ(PacketFormat),
    FP_CONFIG_FQ(StageEgress),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_ingress_mac_acl_diffserv[] = {
    FP_CONFIG_FQ(InPorts),
    FP_CONFIG_FQ(OuterVlan),
    FP_CONFIG_FQ(OuterVlanPri),
    FP_CONFIG_FQ(DstMac),
    FP_CONFIG_FQ(SrcMac),
    FP_CONFIG_FQ(EtherType),

    FP_CONFIG_FQ(MHOpcode),
    FP_CONFIG_FQ(PacketFormat),
    FP_CONFIG_FQ(VlanFormat),
    FP_CONFIG_FQ(L2Format),
    FP_CONFIG_FQ(StageIngress),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_egress_mac_acl_diffserv[] = {
    FP_CONFIG_FQ(OutPort),
    FP_CONFIG_FQ(OuterVlan),
    FP_CONFIG_FQ(OuterVlanPri),
    FP_CONFIG_FQ(DstMac),
    FP_CONFIG_FQ(SrcMac),
    FP_CONFIG_FQ(EtherType),

    FP_CONFIG_FQ(PacketFormat),
    FP_CONFIG_FQ(L2Format),

    FP_CONFIG_FQ(StageEgress),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_link_local[] = {
    FP_CONFIG_FQ(InPorts),
    FP_CONFIG_FQ(HiGig),
    FP_CONFIG_FQ(DstIp6),
    FP_CONFIG_FQ(OuterVlan),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_cpu_if[] = {
    FP_CONFIG_FQ(InPorts),
    FP_CONFIG_FQ(OuterVlan),
    FP_CONFIG_FQ(SrcMac),
    FP_CONFIG_FQ(DstMac),
    FP_CONFIG_FQ(EtherType),
    FP_CONFIG_FQ(MHOpcode),
    FP_CONFIG_FQ(IpType),
    FP_CONFIG_FQ(LookupStatus),
    FP_CONFIG_FQ(HiGig),
    FP_CONFIG_FQ(L3Routable),
    FP_CONFIG_FQ(TunnelTerminated),
    FP_CONFIG_FQ(VlanTranslationHit),
    FP_CONFIG_FQ(ForwardingVlanValid),
    FP_CONFIG_FQ(IngressStpState),
    FP_CONFIG_FQ(L2SrcHit),
    FP_CONFIG_FQ(L2SrcStatic),
    FP_CONFIG_FQ(L2DestHit),
    FP_CONFIG_FQ(L2StationMove),
    FP_CONFIG_FQ(L2CacheHit),
    FP_CONFIG_FQ(L3SrcHostHit),
    FP_CONFIG_FQ(L3DestHostHit),
    FP_CONFIG_FQ(L3DestRouteHit),
    FP_CONFIG_FQ(IpmcStarGroupHit),
    FP_CONFIG_FQ(DosAttack),
    FP_CONFIG_FQ(DstPortTgid),

    FP_CONFIG_FQ(SrcIp),
    FP_CONFIG_FQ(DstIp),
    FP_CONFIG_FQ(IpProtocol),
    FP_CONFIG_FQ(L4SrcPort),
    FP_CONFIG_FQ(L4DstPort),
    FP_CONFIG_FQ(Ttl),
    FP_CONFIG_FQ(StageIngress),
    FP_CONFIG_FQ(PacketFormat),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_pport_rate_limit[] = {
    FP_CONFIG_FQ(InPorts),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_ip6_source_guard[] = {
    FP_CONFIG_FQ(InPorts),
    FP_CONFIG_FQ(IpType),
    FP_CONFIG_FQ(HiGig),
    FP_CONFIG_FQ(SrcIp6),
    FP_CONFIG_FQ(OuterVlan),
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_ifp_pbf_ip_source_guard[] = {
    FP_CONFIG_FQ(InPort),
    FP_CONFIG_FQ(OuterVlan),
    FP_CONFIG_FQ(InnerVlan),
    FP_CONFIG_FQ(SrcMac),
    FP_CONFIG_FQ(DstMac),
    FP_CONFIG_FQ(EtherType),

    FP_CONFIG_FQ(SrcIp),
    FP_CONFIG_FQ(DstIp),
    FP_CONFIG_FQ(IpProtocol),
    FP_CONFIG_FQ(L4SrcPort),
    FP_CONFIG_FQ(L4DstPort),
    FP_CONFIG_FQ(Ttl),
    FP_CONFIG_FQ(StageIngress),

#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
    FP_CONFIG_FQ(InPorts),
    FP_CONFIG_FQ(SrcIp),
    FP_CONFIG_FQ(SrcMac),
    FP_CONFIG_FQ(PacketFormat),
#endif /* SYS_CPNT_IP_SOURCE_GUARD */
};

static FP_CONFIG_FieldQualifyDescriptor_T fp_config_fq_tcp_udp_port_mapping_web_auth[] = {
    FP_CONFIG_FQ(InPorts),
    FP_CONFIG_FQ(IpProtocol),

#if (SYS_CPNT_WEBAUTH == TRUE)
    FP_CONFIG_FQ(SrcIp),
#endif /* SYS_CPNT_WEBAUTH */

    FP_CONFIG_FQ(L4SrcPort),
    FP_CONFIG_FQ(L4DstPort),

    FP_CONFIG_FQ(MHOpcode),
    FP_CONFIG_FQ(IpType),
    FP_CONFIG_FQ(PacketFormat),
    FP_CONFIG_FQ(StageIngress),
};

//#define FP_CONFIG_UDF(chunk, flags) {chunk, flags}
//static FP_CONFIG_FieldUDFDescriptor_T fp_config_fq_mld[] =
//{
//    FP_CONFIG_UDF(14, BCM_FIELD_USER_L2_ETHERNET2
//                      | BCM_FIELD_USER_VLAN_NOTAG
//                      | BCM_FIELD_USER_IP6_HDR_ONLY),
//    FP_CONFIG_UDF(15, BCM_FIELD_USER_L2_ETHERNET2
//                      | BCM_FIELD_USER_VLAN_NOTAG
//                      | BCM_FIELD_USER_IP6_HDR_ONLY),
//};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_ip6_std_acl[] =
{
    FP_CONFIG_FUNC_DESC(RT_IP6_STD_ACL, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_IPV6_ACE)
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_ip6_ext_acl[] =
{
#if (SYS_CPNT_IPV6_DENY_LINK_LOCAL_ROUTING_BY_RULE_AS_TEMPORARY_SOLUTION == TRUE)
    FP_CONFIG_FUNC_DESC(RT_LINK_LOCAL_L3_ROUTABLE, PR_DEFAULT, 1),
#endif /* #if (SYS_CPNT_IPV6_DENY_LINK_LOCAL_ROUTING_BY_RULE_AS_TEMPORARY_SOLUTION == TRUE) */

    FP_CONFIG_FUNC_DESC(RT_IP6_EXT_ACL, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_IPV6_ACE),
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_ip6_std_diffserv[] =
{
    FP_CONFIG_FUNC_DESC(RT_IP6_STD_QOS, PR_DEFAULT, SYS_ADPT_DIFFSERV_MAX_NBR_OF_IPV6_RULE)
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_ip6_ext_diffserv[] =
{
    FP_CONFIG_FUNC_DESC(RT_IP6_EXT_QOS, PR_DEFAULT, SYS_ADPT_DIFFSERV_MAX_NBR_OF_IPV6_RULE)
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_ip_acl[] =
{
    FP_CONFIG_FUNC_DESC(RT_IP_ACL, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_IP_ACE)
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_mac_acl[] =
{
    FP_CONFIG_FUNC_DESC(RT_MAC_ACL, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_MAC_ACE)
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_ip_diffserv[] =
{
    FP_CONFIG_FUNC_DESC(RT_IP_QOS, PR_DEFAULT, SYS_ADPT_DIFFSERV_MAX_NBR_OF_IP_RULE)
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_mac_diffserv[] =
{
    FP_CONFIG_FUNC_DESC(RT_MAC_QOS, PR_DEFAULT, SYS_ADPT_DIFFSERV_MAX_NBR_OF_IP_RULE)
};

/**
 * Egress ACL / diffserv
 */
static FP_CONFIG_FunctionDescriptor_T fp_config_fd_egress_ip6_std_acl_diffserv[] =
{
    FP_CONFIG_FUNC_DESC(RT_EGRESS_IP6_STD_ACL, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_IPV6_ACE),
    FP_CONFIG_FUNC_DESC(RT_EGRESS_IP6_STD_QOS, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_IPV6_ACE),
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_egress_ip6_ext_acl_diffserv[] =
{
    FP_CONFIG_FUNC_DESC(RT_EGRESS_IP6_EXT_ACL, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_IPV6_ACE),
    FP_CONFIG_FUNC_DESC(RT_EGRESS_IP6_EXT_QOS, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_IPV6_ACE),
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_egress_ip_acl_diffserv[] =
{
    FP_CONFIG_FUNC_DESC(RT_EGRESS_IP_ACL, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_IP_ACE),
    FP_CONFIG_FUNC_DESC(RT_EGRESS_IP_QOS, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_IP_ACE),
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_egress_mac_acl_diffserv[] =
{
    FP_CONFIG_FUNC_DESC(RT_EGRESS_MAC_ACL, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_MAC_ACE),
    FP_CONFIG_FUNC_DESC(RT_EGRESS_MAC_QOS, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_MAC_ACE),
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_tcp_udp_port_mapping_web_auth[] =
{
    FP_CONFIG_FUNC_DESC(RT_QOS_MAP, PR_DEFAULT, SYS_ADPT_MAX_NBRS_OF_TCPUDP_ACE),

#if (SYS_CPNT_WEBAUTH == TRUE)
    FP_CONFIG_FUNC_DESC(RT_WEBAUTH_H, PR_HIGHEST, 0),
    FP_CONFIG_FUNC_DESC(RT_WEBAUTH_M, PR_DEFAULT, 0),
    FP_CONFIG_FUNC_DESC(RT_WEBAUTH_L, PR_LOWEST, 0),
#endif /* SYS_CPNT_WEBAUTH */
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_ifp_pbf_ip_source_guard[] =
{
    FP_CONFIG_FUNC_DESC(RT_IFP_PBF, BCM_FIELD_ENTRY_PRIO_HIGHEST, 0),
#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
    FP_CONFIG_FUNC_DESC(RT_IP_SRC_GUARD_H, PR_HIGHEST, 0),
    FP_CONFIG_FUNC_DESC(RT_IP_SRC_GUARD_L, PR_LOWEST, 0),
#endif /* IP_SOURCE_GUARD */
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_ip6_source_guard[] =
{
    FP_CONFIG_FUNC_DESC(RT_PBR, PR_HIGHEST, 0),
#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
    FP_CONFIG_FUNC_DESC(RULE_TYPE_IPV6_SG_PERMIT_HOST, PR_HIGHEST, 0),
    FP_CONFIG_FUNC_DESC(RULE_TYPE_IPV6_SG_DENY_ALL, PR_LOWEST, 0),
#endif /* IP_SOURCE_GUARD */
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_pport_rate_limit[] =
{
#if (SYS_CPNT_INGRESS_RATE_LIMIT_VIA_FILTER_ENGINE == TRUE)
    FP_CONFIG_FUNC_DESC(RT_PPORT_RATELIMIT, PR_HIGHEST, 0),
#endif /* SYS_CPNT_INGRESS_RATE_LIMIT_VIA_FILTER_ENGINE */
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_cpu_if[] =
{
    FP_CONFIG_FUNC_DESC(RT_CPU_1, PR_1, 0),
    FP_CONFIG_FUNC_DESC(RT_CPU_2, PR_2, 0),
    FP_CONFIG_FUNC_DESC(RT_CPU_3, PR_3, 0),
    FP_CONFIG_FUNC_DESC(RT_CPU_4, PR_4, 0),
    FP_CONFIG_FUNC_DESC(RT_CPU_5, PR_5, 0),
    FP_CONFIG_FUNC_DESC(RT_CPU_6, PR_6, 0),
    FP_CONFIG_FUNC_DESC(RT_CPU_7, PR_7, 0),
    FP_CONFIG_FUNC_DESC(RT_CPU_8, PR_8, 0),
    FP_CONFIG_FUNC_DESC(RT_CPU_MLD, PR_DEFAULT, 0),
};

static FP_CONFIG_FunctionDescriptor_T fp_config_fd_link_local[] =
{
    FP_CONFIG_FUNC_DESC(RT_CPU_LINK_LOCAL, PR_DEFAULT, 0),
#if (SYS_CPNT_PIM6 == TRUE)
    FP_CONFIG_FUNC_DESC(RT_PIM6, PR_DEFAULT, 0)
#endif /* SYS_CPNT_PIM6 */
};

static FP_CONFIG_STRUCT_T fp_config_struct[] =
{
#if (SYS_CPNT_ACL_IPV6 == TRUE)
    FP_CONFIG_STRUCT
    (
        0,                                                              /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_0,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_ingress_ipv6_std_acl_diffserv),   /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_ip6_std_acl)                      /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        1,                                                              /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_1,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_ingress_ipv6_ext_acl_diffserv),   /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_ip6_ext_acl)                      /* func_vector */
    ),
#endif /* SYS_CPNT_ACL_IPV6 */

    FP_CONFIG_STRUCT
    (
        2,                                                              /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_2,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_ingress_ip_acl_diffserv),         /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_ip_acl)                           /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        3,                                                              /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_3,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_ingress_mac_acl_diffserv),        /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_mac_acl)                          /* func_vector */
    ),

#if (SYS_CPNT_ACL_IPV6 == TRUE)
    FP_CONFIG_STRUCT
    (
        4,                                                              /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_4,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_ingress_ipv6_std_acl_diffserv),   /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_ip6_std_diffserv)                 /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        5,                                                              /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_5,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_ingress_ipv6_ext_acl_diffserv),   /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_ip6_ext_diffserv)                 /* func_vector */
    ),
#endif /* SYS_CPNT_ACL_IPV6 */

    FP_CONFIG_STRUCT
    (
        6,                                                              /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_6,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_ingress_ip_acl_diffserv),         /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_ip_diffserv)                      /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        7,                                                              /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_7,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_ingress_mac_acl_diffserv),        /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_mac_diffserv)                     /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        8,                                                              /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_8,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_tcp_udp_port_mapping_web_auth),   /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_tcp_udp_port_mapping_web_auth)    /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        9,                                                              /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_9,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_ip6_source_guard),                /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_ip6_source_guard)                 /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        10,                                                             /* group_id */
        DOUBLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_10,                                                    /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_ifp_pbf_ip_source_guard),         /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_ifp_pbf_ip_source_guard)          /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        11,                                                             /* group_id */
        DOUBLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_12,                                                    /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_cpu_if),                          /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_cpu_if)                           /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        12,                                                             /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_14,                                                    /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_pport_rate_limit),                /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                         /* udf_vector */
        //FP_CONFIG_VECTOR(fp_config_fq_mld),                             /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_pport_rate_limit)                 /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        13,                                                             /* group_id */
        SINGLE,                                                         /* group_mode */
        IFP,                                                            /* selector_stage */
        SELECTOR_15,                                                    /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_link_local),                      /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_link_local)                       /* func_vector */
    ),

#if (SYS_CPNT_ACL_IPV6 == TRUE)
    FP_CONFIG_STRUCT
    (
        14,                                                             /* group_id */
        SINGLE,                                                         /* group_mode */
        EFP,                                                            /* selector_stage */
        SELECTOR_0,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_egress_ipv6_std_acl_diffserv),    /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_egress_ip6_std_acl_diffserv)      /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        15,                                                             /* group_id */
        SINGLE,                                                         /* group_mode */
        EFP,                                                            /* selector_stage */
        SELECTOR_1,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_egress_ipv6_ext_acl_diffserv),    /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_egress_ip6_ext_acl_diffserv)      /* func_vector */
    ),
#endif /* SYS_CPNT_ACL_IPV6 */

    FP_CONFIG_STRUCT
    (
        16,                                                             /* group_id */
        SINGLE,                                                         /* group_mode */
        EFP,                                                            /* selector_stage */
        SELECTOR_2,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_egress_ip_acl_diffserv),          /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_egress_ip_acl_diffserv)           /* func_vector */
    ),

    FP_CONFIG_STRUCT
    (
        17,                                                             /* group_id */
        SINGLE,                                                         /* group_mode */
        EFP,                                                            /* selector_stage */
        SELECTOR_3,                                                     /* first_selector_num */
        FP_CONFIG_VECTOR(fp_config_fq_egress_mac_acl_diffserv),         /* fq_vector */
        FP_CONFIG_NULL_VECTOR(),                                        /* udf_vector */
        FP_CONFIG_VECTOR(fp_config_fd_egress_mac_acl_diffserv)          /* func_vector */
    ),
};


static FP_CONFIG_FunctionInfo_T *fp_config_shm_func_info_p; /* function information list */
static FP_CONFIG_GroupInfo_T *fp_config_shm_grp_info_p;     /* group information list */


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_GetShMemDataSize
 *------------------------------------------------------------------------------
 * PURPOSE: Get shared memory size of fp_config
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
UI32_T FP_CONFIG_GetShMemDataSize()
{
    return FP_CONFIG_NumberOfFunctionType() * sizeof(FP_CONFIG_FunctionInfo_T)
        +  FP_CONFIG_NumberOfGroup() * sizeof(FP_CONFIG_GroupInfo_T);
}


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for FP_CONFIG in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void FP_CONFIG_AttachSystemResources(void *shmem_start_p)
{
    fp_config_shm_func_info_p = shmem_start_p;
    fp_config_shm_grp_info_p  = (FP_CONFIG_GroupInfo_T*)
        ((UI8_T*)shmem_start_p
        + FP_CONFIG_NumberOfFunctionType() * sizeof(FP_CONFIG_FunctionInfo_T));
}


UI32_T FP_CONFIG_CountBits(UI32_T bitmap)
{
    UI32_T temp = bitmap;
    UI32_T res  = 0;

    while (temp) {
        res  += 1;
        temp &= temp - 1;
    }

    return res;
}


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_Init
 *------------------------------------------------------------------------------
 * PURPOSE: Set OM of FP_CONFIG to default value.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void FP_CONFIG_Init()
{
    FP_CONFIG_FunctionInfo_T *func_info_p;
    FP_CONFIG_GroupInfo_T *grp_info_p;
    size_t size = sizeof(fp_config_struct)/sizeof(fp_config_struct[0]);
    size_t i;
    UI32_T tmp;

    if (0)
    {
#ifdef DUMP_TABLE
    FP_CONFIG_DumpTbl();
#else
    FP_CONFIG_Dump();
#endif
    }

    if (NULL == fp_config_shm_func_info_p || NULL == fp_config_shm_grp_info_p)
        return;

    /* Convert fp_config_struct to shared memory
     */

    func_info_p = fp_config_shm_func_info_p;
    grp_info_p  = fp_config_shm_grp_info_p;

    for (i=0; i<size; i++, grp_info_p++)
    {
        FP_CONFIG_STRUCT_T *struct_p = &fp_config_struct[i];

        memset(grp_info_p, 0, sizeof(FP_CONFIG_GroupInfo_T));

        grp_info_p->group_id        = struct_p->group_id;
        grp_info_p->group_mode      = struct_p->group_mode;
        grp_info_p->selector_stage  = struct_p->selector_stage;
        grp_info_p->selector_bitmap = struct_p->first_selector_num;
        grp_info_p->selector_count  = FP_CONFIG_CountBits(struct_p->first_selector_num);

        for (tmp=0; tmp<struct_p->fq_vector.count; tmp++)
        {
            DEVRM_FIELD_QSET_ADD_INTERNAL(*grp_info_p, struct_p->fq_vector.fq[tmp].fq);
        }

        for (tmp=0; tmp<struct_p->udf_vector.count; tmp++)
        {
            grp_info_p->udf[tmp].udf_id  = tmp+1;
            grp_info_p->udf[tmp].chunk   = struct_p->udf_vector.udf[tmp].chunk;
            grp_info_p->udf[tmp].flags   = struct_p->udf_vector.udf[tmp].flags;
            grp_info_p->udf[tmp].enabled = TRUE;
        }

        for (tmp=0; tmp<struct_p->func_vector.count; tmp++, func_info_p++)
        {
            func_info_p->function_type   = struct_p->func_vector.fd[tmp].func_type;
            func_info_p->rule_pri        = struct_p->func_vector.fd[tmp].rule_pri;
            func_info_p->rule_quota      = struct_p->func_vector.fd[tmp].rule_quota;
            func_info_p->group_id        = i;
        }
    }
}

void FP_CONFIG_DumpFieldQualifyVector(FP_CONFIG_STRUCT_T *struct_p)
{
    UI32_T i;

    for (i=0; i<struct_p->fq_vector.count; i++)
    {
        printf("%d %s%s",
            struct_p->fq_vector.fq[i].fq,
            struct_p->fq_vector.fq[i].fq_str,
            (i==struct_p->fq_vector.count-1) ? "" : ", ");
    }
}


void FP_CONFIG_DumpFieldUDFVector(FP_CONFIG_STRUCT_T *struct_p)
{
    UI32_T i;
    UI32_T tmp;
    char   chunk_offset_str[20];

    if (struct_p->udf_vector.count)
    {
        for (i=0; i<struct_p->udf_vector.count; i++)
        {
            if (struct_p->udf_vector.udf[i].chunk == 0)
            {
                strcpy(chunk_offset_str, "B126, B127, B0, B1");
            }
            else
            {
                tmp = struct_p->udf_vector.udf[i].chunk*4-2;
                sprintf(chunk_offset_str, "B%lu~B%lu",
                    tmp,
                    tmp+3);
            }

            printf("flags=%08X chunk=%lu %s%s",
                (unsigned int)struct_p->udf_vector.udf[i].flags,
                struct_p->udf_vector.udf[i].chunk,
                chunk_offset_str,
                (i==struct_p->func_vector.count-1) ? "" : ", ");
        }
    }
    else
    {
        printf("%s", "NULL");
    }
}


void FP_CONFIG_DumpFuncDescVector(FP_CONFIG_STRUCT_T *struct_p)
{
    UI32_T i;

    for (i=0; i<struct_p->func_vector.count; i++)
    {
        printf("%d %s %d %s%s",
            struct_p->func_vector.fd[i].func_type,
            struct_p->func_vector.fd[i].func_type_str,
            struct_p->func_vector.fd[i].rule_pri,
            struct_p->func_vector.fd[i].rule_pri_str,
            (i==struct_p->func_vector.count-1) ? "" : ", ");
    }
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_Dump
 *------------------------------------------------------------------------------
 * PURPOSE: Dump fp config information
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void FP_CONFIG_Dump()
{
/* end of column
 */
#define EOC "\n"

    size_t size = sizeof(fp_config_struct)/sizeof(fp_config_struct[0]);
    size_t i;
    int    selector_size;

    //printf("Group ID;Application Function;Group Mode;Selector Stage;Selector Num;Selector Size;Rule Size;Field Qualify;Field UDF\n");

    for (i=0; i<size; i++)
    {
        FP_CONFIG_STRUCT_T *struct_p = &fp_config_struct[i];

        printf("%s", (i!=0)?"\n":"");

        printf("Group ID      : %lu", struct_p->group_id); printf(EOC);

        printf("Function      : ");
        FP_CONFIG_DumpFuncDescVector(struct_p); printf(EOC);

        printf("Group Mode    : ");
        printf("%lu %s",
            struct_p->group_mode,
            struct_p->group_mode_str); printf(EOC);

        printf("Selector Stage: ");
        printf("%lu %s",
            struct_p->selector_stage,
            struct_p->selector_stage_str); printf(EOC);

        printf("Selector Num  : ");
        printf("%08X %s",
            (unsigned int)struct_p->first_selector_num,
            struct_p->first_selector_num_str); printf(EOC);

        selector_size = (struct_p->group_mode == SINGLE) ? 1 :
            (struct_p->group_mode == DOUBLE) ? 2 :
            (struct_p->group_mode == TRIPLE) ? 3 :
                                               -1;
        printf("Selector Size : ");
        printf("%d", selector_size); printf(EOC);

        printf("Rule Size     : ");
        printf("%d", SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR); printf(EOC);

        printf("Field Qualify : ");
        FP_CONFIG_DumpFieldQualifyVector(struct_p); printf(EOC);

        printf("Field UDF     : ");
        FP_CONFIG_DumpFieldUDFVector(struct_p); printf(EOC);
    }

#undef EOC
}

#ifdef DUMP_TABLE
/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_DumpTbl
 *------------------------------------------------------------------------------
 * PURPOSE: Dump fp config information
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void FP_CONFIG_DumpTbl()
{
#define PAGE_WIDTH  80

    size_t size = sizeof(fp_config_struct)/sizeof(fp_config_struct[0]);
    size_t i;
    int    selector_size;

    enum
    {
        GROUP_ID = 0,
        APP_FUNC,
        GROUP_MODE,
        SEL_STAGE,
        SEL_NUM,
        SEL_SIZE,
        RULE_SIZE,
        FIELD_QUAL,
    };

    CLI_TBL_Temp_T tbl_temp[] =
    {
        {GROUP_ID,       2, CLI_TBL_ALIGN_RIGHT},
        {APP_FUNC,      16, CLI_TBL_ALIGN_LEFT},
        {GROUP_MODE,     6, CLI_TBL_ALIGN_LEFT},
        {SEL_STAGE,      3, CLI_TBL_ALIGN_LEFT},
        {SEL_NUM,       11, CLI_TBL_ALIGN_LEFT},
        {FIELD_QUAL,    35, CLI_TBL_ALIGN_LEFT},
    };

    CLI_TBL_Object_T tb;
    char             buf[PAGE_WIDTH*15+1] = {0};
    char             tmp_str1[PAGE_WIDTH*10+1];
    char             tmp_str2[PAGE_WIDTH*10+1];
    char             chunk_offset_str[20];

    CLI_TBL_InitWithBuf(&tb, buf, sizeof(buf));
    CLI_TBL_SetColIndirect(&tb, tbl_temp, sizeof(tbl_temp)/sizeof(tbl_temp[0]));

    CLI_TBL_SetColTitle(&tb, GROUP_ID,      "ID");
    CLI_TBL_SetColTitle(&tb, APP_FUNC,      "App Func");
    CLI_TBL_SetColTitle(&tb, GROUP_MODE,    "Mode");
    CLI_TBL_SetColTitle(&tb, SEL_STAGE,     "Stg");
    CLI_TBL_SetColTitle(&tb, SEL_NUM,       "SelNum");
    CLI_TBL_SetColTitle(&tb, FIELD_QUAL,    "FieldQual");
    CLI_TBL_Print(&tb);

    CLI_TBL_SetLine(&tb);
    CLI_TBL_Print(&tb);

    for (i=0; i<size; i++)
    {
        FP_CONFIG_STRUCT_T *struct_p = &fp_config_struct[i];

        CLI_TBL_SetColInt(&tb, GROUP_ID, struct_p->group_id);

        {
            UI32_T i;

            tmp_str2[0] = tmp_str1[0] = '\0';
            for (i=0; i<struct_p->func_vector.count; i++)
            {
                sprintf(tmp_str1, "%s%s",
                    //struct_p->func_vector.fd[i].func_type,
                    struct_p->func_vector.fd[i].func_type_str,
                    /*struct_p->func_vector.fd[i].rule_pri,
                    struct_p->func_vector.fd[i].rule_pri_str*/
                    (i==struct_p->func_vector.count-1) ? "" : ", ");
                strcat(tmp_str2, tmp_str1);
            }
            CLI_TBL_SetColText(&tb, APP_FUNC, tmp_str2);
        }

        CLI_TBL_SetColText(&tb, GROUP_MODE, struct_p->group_mode_str);
        CLI_TBL_SetColText(&tb, SEL_STAGE, struct_p->selector_stage_str);
        CLI_TBL_SetColText(&tb, SEL_NUM, struct_p->first_selector_num_str);

        {
            UI32_T i;

            tmp_str2[0] = tmp_str1[0] = '\0';

            if (struct_p->fq_vector.count)
            {
                strcat(tmp_str2, "fq={");
            }

            for (i=0; i<struct_p->fq_vector.count; i++)
            {
                sprintf(tmp_str1, "%d %s%s",
                    struct_p->fq_vector.fq[i].fq,
                    struct_p->fq_vector.fq[i].fq_str,
                    (i==struct_p->fq_vector.count-1) ? "" : ", ");
                strcat(tmp_str2, tmp_str1);
            }

            if (struct_p->fq_vector.count)
            {
                strcat(tmp_str2, "}");
            }

            if (struct_p->udf_vector.count)
            {
                strcat(tmp_str2, "udf={");
            }

            for (i=0; i<struct_p->udf_vector.count; i++)
            {
                if (struct_p->udf_vector.udf[i].chunk == 0)
                {
                    strcpy(chunk_offset_str, "B126, B127, B0, B1");
                }
                else
                {
                    int tmp = struct_p->udf_vector.udf[i].chunk*4-2;
                    sprintf(chunk_offset_str, "B%d~B%d",
                        tmp,
                        tmp+3);
                }

                sprintf(tmp_str1, "flags=%08X chunk=%d %s%s",
                    struct_p->udf_vector.udf[i].flags,
                    struct_p->udf_vector.udf[i].chunk,
                    chunk_offset_str,
                    (i==struct_p->func_vector.count-1) ? "" : ", ");

                strcat(tmp_str2, tmp_str1);
            }

            if (struct_p->udf_vector.count)
            {
                strcat(tmp_str2, "}");
            }


            CLI_TBL_SetColText(&tb, FIELD_QUAL, tmp_str2);
        }

        CLI_TBL_Print(&tb);
    }

    return;
#undef PAGE_WIDTH
}
#endif /* DUMP_TABLE */


UI32_T FP_CONFIG_NumberOfFunctionType()
{
    size_t size = sizeof(fp_config_struct)/sizeof(fp_config_struct[0]);
    size_t i;
    UI32_T ret = 0;

    for (i=0; i<size; i++)
    {
        FP_CONFIG_STRUCT_T *struct_p = &fp_config_struct[i];
        ret += struct_p->func_vector.count;
    }

    return ret;
}


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_GetFunctionInfo
 *------------------------------------------------------------------------------
 * PURPOSE: Get function information by array index
 * INPUT:   index   - array index
 * OUTPUT:  None
 * RETUEN:  function infomation pointer
 * NOTES:
 *------------------------------------------------------------------------------
 */
FP_CONFIG_FunctionInfo_T * FP_CONFIG_GetFunctionInfo(UI32_T index)
{
    if(FP_CONFIG_NumberOfFunctionType() < index || NULL == fp_config_shm_func_info_p)
        return NULL;

    return &fp_config_shm_func_info_p[index];
}


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_get_function_info_by_type
 *------------------------------------------------------------------------------
 * PURPOSE: Get function information by type
 * INPUT:   fun_type   - function type
 * OUTPUT:  None
 * RETUEN:  function infomation pointer
 * NOTES:
 *------------------------------------------------------------------------------
 */
FP_CONFIG_FunctionInfo_T * FP_CONFIG_get_function_info_by_type(RULE_TYPE_FunctionType_T fun_type)
{
    UI32_T i;

    if (NULL == fp_config_shm_func_info_p)
    {
        return NULL;
    }

    for (i=0; i < FP_CONFIG_NumberOfFunctionType(); i++)
    {
        if (fp_config_shm_func_info_p[i].function_type == fun_type)
            return &fp_config_shm_func_info_p[i];
    }

    return NULL;
}


UI32_T FP_CONFIG_NumberOfGroup()
{
    return sizeof(fp_config_struct)/sizeof(fp_config_struct[0]);
}


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_get_group_info_by_id
 *------------------------------------------------------------------------------
 * PURPOSE: Get group information by array index
 * INPUT:   group_id   - array index
 * OUTPUT:  None
 * RETUEN:  group infomation pointer
 * NOTES:
 *------------------------------------------------------------------------------
 */
FP_CONFIG_GroupInfo_T * FP_CONFIG_get_group_info_by_id(UI32_T group_id)
{
    if (NULL == fp_config_shm_grp_info_p
        || FP_CONFIG_NumberOfGroup() <= group_id)
    {
        return NULL;
    }

    return &fp_config_shm_grp_info_p[group_id];
}

UI32_T FP_CONFIG_GetConfigSize()
{
    return sizeof(fp_config_struct) / sizeof(*fp_config_struct);
}

// TODO: Add an API to convert function type to string name

// TODO: Add an API to dump group configure from Rule Control,
//       so we do not need to add this API in Marvell platform.
//       For BCM prject only.

FP_CONFIG_STRUCT_T * FP_CONFIG_GetConfig(UI32_T idx)
{
    return &fp_config_struct[idx];
}

