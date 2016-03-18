//
//  rule_config.h
//  l4_mgr
//
//  Created by yehjunying on 12/10/2.
//
//

#ifndef _RULE_CONFIG_H_
#define _RULE_CONFIG_H_

#define RULE_TYPE_LST_ENUM(a,b)         a,
#define RULE_TYPE_LST_NAME(a,b)         #a,
#define RULE_TYPE_LST_RULE_CNT(a,b)     b,
#define RULE_TYPE_LST_RULE_CNT_SUM(a,b) b+

/*  global ace:  enum name, rule count
 */
#define RULE_TYPE_GLBL_ACE_LST(_)                                                                       \
    _(RULE_TYPE_GLOBAL_ACE_ARP,                     RULE_TYPE_GLOBAL_ACE_ARP_RULE_CNT)                  \
    _(RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC,            RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC_RULE_CNT)         \
    _(RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC1,           RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC1_RULE_CNT)        \
    _(RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC2,           RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC2_RULE_CNT)        \
    _(RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC3,           RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC3_RULE_CNT)        \
    _(RULE_TYPE_GLOBAL_ACE_DHCP_SERVER,             RULE_TYPE_GLOBAL_ACE_DHCP_SERVER_RULE_CNT)          \
    _(RULE_TYPE_GLOBAL_ACE_DHCP_CLIENT,             RULE_TYPE_GLOBAL_ACE_DHCP_CLIENT_RULE_CNT)          \
    _(RULE_TYPE_GLOBAL_ACE_DHCP6_SERVER,            RULE_TYPE_GLOBAL_ACE_DHCP6_SERVER_RULE_CNT)         \
    _(RULE_TYPE_GLOBAL_ACE_DHCP6_CLIENT,            RULE_TYPE_GLOBAL_ACE_DHCP6_CLIENT_RULE_CNT)         \
    _(RULE_TYPE_GLOBAL_ACE_MAC_BASED_VLAN,          RULE_TYPE_GLOBAL_ACE_MAC_BASED_VLAN_RULE_CNT)       \
    _(RULE_TYPE_GLOBAL_ACE_IP_SUBNET_BASED_VLAN,    RULE_TYPE_GLOBAL_ACE_IP_SUBNET_BASED_VLAN_RULE_CNT) \
    _(RULE_TYPE_GLOBAL_ACE_IGMP,                    RULE_TYPE_GLOBAL_ACE_IGMP_RULE_CNT)                 \
    _(RULE_TYPE_GLOBAL_ACE_MVR_REC_VLAN,            RULE_TYPE_GLOBAL_ACE_MVR_REC_VLAN_RULE_CNT)         \
    _(RULE_TYPE_GLOBAL_ACE_MAC_BASED_MIRROR,        RULE_TYPE_GLOBAL_ACE_MAC_BASED_MIRROR_RULE_CNT)     \
    _(RULE_TYPE_GLOBAL_ACE_PPPOED,                  RULE_TYPE_GLOBAL_ACE_PPPOED_RULE_CNT)               \
    _(RULE_TYPE_GLOBAL_ACE_CFM,                     RULE_TYPE_GLOBAL_ACE_CFM_RULE_CNT)                  \
    _(RULE_TYPE_GLOBAL_ACE_RAPS,                    RULE_TYPE_GLOBAL_ACE_RAPS_RULE_CNT)                 \
    _(RULE_TYPE_GLOBAL_ACE_L2PT,                    RULE_TYPE_GLOBAL_ACE_L2PT_RULE_CNT)                 \
    _(RULE_TYPE_GLOBAL_ACE_CDP,                     RULE_TYPE_GLOBAL_ACE_CDP_RULE_CNT)                  \
    _(RULE_TYPE_GLOBAL_ACE_PVST,                    RULE_TYPE_GLOBAL_ACE_PVST_RULE_CNT)                 \
    _(RULE_TYPE_GLOBAL_ACE_MY_MAC_ARP,              RULE_TYPE_GLOBAL_ACE_MY_MAC_ARP_RULE_CNT)           \
    _(RULE_TYPE_GLOBAL_ACE_RIP,                     RULE_TYPE_GLOBAL_ACE_RIP_RULE_CNT)                  \
    _(RULE_TYPE_GLOBAL_ACE_PTP_EVENT_ETH,           RULE_TYPE_GLOBAL_ACE_PTP_EVENT_ETH_RULE_CNT)        \
    _(RULE_TYPE_GLOBAL_ACE_PTP_EVENT_UDP,           RULE_TYPE_GLOBAL_ACE_PTP_EVENT_UDP_RULE_CNT)        \
    _(RULE_TYPE_GLOBAL_ACE_PTP_GENERAL_UDP,         RULE_TYPE_GLOBAL_ACE_PTP_GENERAL_UDP_RULE_CNT)      \
    _(RULE_TYPE_GLOBAL_ACE_IP6_ND_RS,               RULE_TYPE_GLOBAL_ACE_IP6_ND_RS_RULE_CNT)            \
    _(RULE_TYPE_GLOBAL_ACE_IP6_ND_RA,               RULE_TYPE_GLOBAL_ACE_IP6_ND_RA_RULE_CNT)            \
    _(RULE_TYPE_GLOBAL_ACE_IP6_ND_NS,               RULE_TYPE_GLOBAL_ACE_IP6_ND_NS_RULE_CNT)            \
    _(RULE_TYPE_GLOBAL_ACE_IP6_ND_NA,               RULE_TYPE_GLOBAL_ACE_IP6_ND_NA_RULE_CNT)            \
    _(RULE_TYPE_GLOBAL_ACE_IP6_ND_REDIRECT,         RULE_TYPE_GLOBAL_ACE_IP6_ND_REDIRECT_RULE_CNT)      \
    _(RULE_TYPE_GLOBAL_ACE_DOS_CHARGEN,             RULE_TYPE_GLOBAL_ACE_DOS_CHARGEN_RULE_CNT)          \
    _(RULE_TYPE_GLOBAL_ACE_DOS_ECHO,                RULE_TYPE_GLOBAL_ACE_DOS_ECHO_RULE_CNT)             \
    _(RULE_TYPE_GLOBAL_ACE_DOS_TCP_FLOODING,        RULE_TYPE_GLOBAL_ACE_DOS_TCP_FLOODING_RULE_CNT)     \
    _(RULE_TYPE_GLOBAL_ACE_DOS_UDP_FLOODING,        RULE_TYPE_GLOBAL_ACE_DOS_UDP_FLOODING_RULE_CNT)     \
    _(RULE_TYPE_GLOBAL_ACE_DOS_WIN_NUKE,            RULE_TYPE_GLOBAL_ACE_DOS_WIN_NUKE_RULE_CNT)         \
    _(RULE_TYPE_GLOBAL_ACE_DOS_SMURF,               RULE_TYPE_GLOBAL_ACE_DOS_SMURF_RULE_CNT)            \
    _(RULE_TYPE_GLOBAL_ACE_L3_LITE_ROUTING,         RULE_TYPE_GLOBAL_ACE_L3_LITE_ROUTING_RULE_CNT)      \
    _(RULE_TYPE_GLOBAL_ACE_IP_BCAST,                RULE_TYPE_GLOBAL_ACE_IP_BCAST_RULE_CNT)             \
    _(RULE_TYPE_GLOBAL_ACE_VRRP,                    RULE_TYPE_GLOBAL_ACE_VRRP_RULE_CNT)                 \
    _(RULE_TYPE_GLOBAL_ACE_BPDU,                    RULE_TYPE_GLOBAL_ACE_BPDU_RULE_CNT)                 \
    _(RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL,        RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_RULE_CNT)        \
    _(RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_STRICT, RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_STRICT_RULE_CNT) \
    _(RULE_TYPE_GLOBAL_ACE_DAI_ARP_DROP,            RULE_TYPE_GLOBAL_ACE_DAI_ARP_DROP_RULE_CNT)         \
    _(RULE_TYPE_GLOBAL_ACE_PBR,                     RULE_TYPE_GLOBAL_ACE_PBR_RULE_CNT)

#define RULE_TYPE_CRAFT_PORT_ACE_LST(_)                                                                 \
    _(RULE_TYPE_CRAFT_PORT_MY_MAC,                  RULE_TYPE_CRAFT_PORT_MY_MAC_RULE_CNT)

/*  default ace:  enum name, rule count
 */
#define RULE_TYPE_DFLT_ACE_LST(_)                                                                           \
    _(RULE_TYPE_DFLT_ACE_AF_DENY_CDP,               RULE_TYPE_DFLT_ACE_AF_DENY_CDP_RULE_CNT)                \
    _(RULE_TYPE_DFLT_ACE_AF_DENY_PVST,              RULE_TYPE_DFLT_ACE_AF_DENY_PVST_RULE_CNT)               \
    _(RULE_TYPE_DFLT_ACE_OAM_LBK,                   RULE_TYPE_DFLT_ACE_OAM_LBK_RULE_CNT)                    \
    _(RULE_TYPE_DFLT_ACE_L2PT_CUSTOM_PDU,           RULE_TYPE_DFLT_ACE_L2PT_CUSTOM_PDU_RULE_CNT)            \
    _(RULE_TYPE_DFLT_ACE_DHCP_CLIENT,               RULE_TYPE_DFLT_ACE_DHCP_CLIENT_RULE_CNT)                \
    _(RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA,         RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA_RULE_CNT)          \
    _(RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA_DROP,    RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA_DROP_RULE_CNT)     \
    _(RULE_TYPE_DFLT_ACE_IP6_MULTICAST_DATA_DROP,   RULE_TYPE_DFLT_ACE_IP6_MULTICAST_DATA_DROP_RULE_CNT)    \
    _(RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_PORT,  RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_PORT_RULE_CNT)   \
    _(RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_TRUNK, RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_TRUNK_RULE_CNT)  \
    _(RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_PORT_RATE_LIMIT,  RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_PORT_RATE_LIMIT_RULE_CNT)  \
    _(RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_TRUNK_RATE_LIMIT, RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_TRUNK_RATE_LIMIT_RULE_CNT) \
    _(RULE_TYPE_DFLT_ACE_SQINQ,                     RULE_TYPE_DFLT_ACE_SQINQ_RULE_CNT)                      \
    _(RULE_TYPE_DFLT_ACE_DNS,                       RULE_TYPE_DFLT_ACE_DNS_RULE_CNT)                        \
    _(RULE_TYPE_DFLT_ACE_IP_SUBNET_BASED_VLAN,      RULE_TYPE_DFLT_ACE_IP_SUBNET_BASED_VLAN_RULE_CNT)       \
    _(RULE_TYPE_DFLT_ACE_PPPOED,                    RULE_TYPE_DFLT_ACE_PPPOED_RULE_CNT)                     \
    _(RULE_TYPE_DFLT_ACE_TRUNK_VLAN_TRANSLATION,    RULE_TYPE_DFLT_ACE_TRUNK_VLAN_TRANSLATION_RULE_CNT)     \
    _(RULE_TYPE_DFLT_ACE_VLAN_TRANSLATION,          RULE_TYPE_DFLT_ACE_VLAN_TRANSLATION_RULE_CNT)           \
    _(RULE_TYPE_DFLT_ACE_ACL,                       RULE_TYPE_DFLT_ACE_ACL_RULE_CNT)                        \
    _(RULE_TYPE_DFLT_ACE_WEB_AUTH_PERMIT_HOST,      RULE_TYPE_DFLT_ACE_WEB_AUTH_PERMIT_HOST_RULE_CNT)       \
    _(RULE_TYPE_DFLT_ACE_IP_SRC_GRD_PERMIT_HOST,    RULE_TYPE_DFLT_ACE_IP_SRC_GRD_PERMIT_HOST_RULE_CNT)     \
    _(RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_PERMIT_HOST,   RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_PERMIT_HOST_RULE_CNT)    \
    _(RULE_TYPE_DFLT_ACE_WEB_AUTH_REDIR_HTTP,       RULE_TYPE_DFLT_ACE_WEB_AUTH_REDIR_HTTP_RULE_CNT)        \
    _(RULE_TYPE_DFLT_ACE_IP_SRC_GRD_DENY_IP_ANY,    RULE_TYPE_DFLT_ACE_IP_SRC_GRD_DENY_IP_ANY_RULE_CNT)     \
    _(RULE_TYPE_DFLT_ACE_WEB_AUTH_DENY_IP_ANY,      RULE_TYPE_DFLT_ACE_WEB_AUTH_DENY_IP_ANY_RULE_CNT)       \
    _(RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_DENY_IP6_ANY,  RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_DENY_IP6_ANY_RULE_CNT)   \
    _(RULE_TYPE_DFLT_ACE_ACL_MATCH_ANY_ANY,         RULE_TYPE_DFLT_ACE_ACL_MATCH_ANY_ANY_RULE_CNT)          \
    _(RULE_TYPE_DFLT_ACE_QOS,                       RULE_TYPE_DFLT_ACE_QOS_RULE_CNT)                        \
    _(RULE_TYPE_DFLT_ACE_RATE_LIMIT,                RULE_TYPE_DFLT_ACE_RATE_LIMIT_RULE_CNT)                 \
    _(RULE_TYPE_DFLT_ACE_WA_PERMIT_HOST_STRICT,     RULE_TYPE_DFLT_ACE_WA_PERMIT_HOST_STRICT_RULE_CNT)      \
    _(RULE_TYPE_DFLT_ACE_IP_SG_PERMIT_HOST_STRICT,  RULE_TYPE_DFLT_ACE_IP_SG_PERMIT_HOST_STRICT_RULE_CNT)   \
    _(RULE_TYPE_DFLT_ACE_IP6_SG_PERMIT_HOST_STRICT, RULE_TYPE_DFLT_ACE_IP6_SG_PERMIT_HOST_STRICT_RULE_CNT)  \
    _(RULE_TYPE_DFLT_ACE_WA_REDIR_HTTP_STRICT,      RULE_TYPE_DFLT_ACE_WA_REDIR_HTTP_STRICT_RULE_CNT)       \
    _(RULE_TYPE_DFLT_ACE_IP_SG_DENY_IP_ANY_STRICT,  RULE_TYPE_DFLT_ACE_IP_SG_DENY_IP_ANY_STRICT_RULE_CNT)   \
    _(RULE_TYPE_DFLT_ACE_WA_DENY_IP_ANY_STRICT,     RULE_TYPE_DFLT_ACE_WA_DENY_IP_ANY_STRICT_RULE_CNT)      \
    _(RULE_TYPE_DFLT_ACE_IP6_SG_DENY_IP6_ANY_STRICT,RULE_TYPE_DFLT_ACE_IP6_SG_DENY_IP6_ANY_STRICT_RULE_CNT) \
    _(RULE_TYPE_DFLT_ACE_EGRESS_VLAN_TRANSLATION,   RULE_TYPE_DFLT_ACE_EGRESS_VLAN_TRANSLATION_RULE_CNT)    \
    _(RULE_TYPE_DFLT_ACE_EGRESS_ACL,                RULE_TYPE_DFLT_ACE_EGRESS_ACL_RULE_CNT)                 \
    _(RULE_TYPE_DFLT_ACE_EGRESS_ACL_MATCH_ANY_ANY,  RULE_TYPE_DFLT_ACE_EGRESS_ACL_MATCH_ANY_ANY_RULE_CNT)   \
    _(RULE_TYPE_DFLT_ACE_EGRESS_QOS,                RULE_TYPE_DFLT_ACE_EGRESS_QOS_RULE_CNT)

#define QUOTA(min, max)     min, max
#define RESERVE_QUOTOA(x)   QUOTA(x, x)

#define RULE_CONFIG_PACKET_TYPE_TABLE                                           \
    PACKET_TYPE(RULE_TYPE_PACKET_NIL,                                           \
                0xffffffff,                                                     \
                RULE_TYPE_PACKET_INGRESS_0_0_BEGIN,                             \
                QUOTA(0, 0))                                                    \
                                                                                \
    /* Ingress Lookup 0-0 Begin */                                              \
    PACKET_TYPE(RULE_TYPE_PACKET_INGRESS_0_0_BEGIN,                             \
                0xffffffff,                                                     \
                RULE_TYPE_PACKET_INGRESS_0_0_BEGIN,                             \
                QUOTA(0, 0))                                                    \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_GLOBAL_BPDU,                                   \
               RULE_TYPE_INTERFACE_ALL_UNIT,                                    \
               RULE_TYPE_GLOBAL_ACE_BPDU,                                       \
               QUOTA(0,                                                         \
                      RULE_TYPE_GLOBAL_ACE_BPDU_RULE_CNT))                      \
    PACKET_TYPE(RULE_TYPE_PACKET_LOOP_DENY_ANY_PER_PORT,                        \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_PORT_RATE_LIMIT,           \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_PORT_RATE_LIMIT_RULE_CNT))  \
    PACKET_TYPE(RULE_TYPE_PACKET_LOOP_DENY_ANY_PER_TRUNK,                       \
                RULE_TYPE_INTERFACE_TRUNK,                                      \
                RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_TRUNK_RATE_LIMIT,          \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_TRUNK_RATE_LIMIT_RULE_CNT)) \
    PACKET_TYPE(RULE_TYPE_PACKET_DAI_ARP_DROP,                                  \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DAI_ARP_DROP,                              \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DAI_ARP_DROP_RULE_CNT,               \
                      RULE_TYPE_GLOBAL_ACE_DAI_ARP_DROP_RULE_CNT))              \
    PACKET_TYPE(RULE_TYPE_PACKET_ARP,                                           \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_ARP,                                       \
                QUOTA(RULE_TYPE_GLOBAL_ACE_ARP_RULE_CNT,                        \
                      RULE_TYPE_GLOBAL_ACE_ARP_RULE_CNT))                       \
    PACKET_TYPE(RULE_TYPE_PACKET_ORG_SPECIFIC,                                  \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC,                              \
                QUOTA(RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC_RULE_CNT,               \
                      RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC_RULE_CNT))              \
    PACKET_TYPE(RULE_TYPE_PACKET_ORG_SPECIFIC1,                                 \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC1,                             \
                QUOTA(RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC1_RULE_CNT,              \
                      RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC1_RULE_CNT))             \
    PACKET_TYPE(RULE_TYPE_PACKET_ORG_SPECIFIC2,                                 \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC2,                             \
                QUOTA(RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC2_RULE_CNT,              \
                      RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC2_RULE_CNT))             \
    PACKET_TYPE(RULE_TYPE_PACKET_ORG_SPECIFIC3,                                 \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC3,                             \
                QUOTA(RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC3_RULE_CNT,              \
                      RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC3_RULE_CNT))             \
    PACKET_TYPE(RULE_TYPE_PACKET_DHCP_SERVER,                                   \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DHCP_SERVER,                               \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DHCP_SERVER_RULE_CNT,                \
                      RULE_TYPE_GLOBAL_ACE_DHCP_SERVER_RULE_CNT))               \
    PACKET_TYPE(RULE_TYPE_PACKET_DHCP_CLIENT,                                   \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DHCP_CLIENT,                               \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DHCP_CLIENT_RULE_CNT,                \
                      RULE_TYPE_GLOBAL_ACE_DHCP_CLIENT_RULE_CNT))               \
    PACKET_TYPE(RULE_TYPE_PACKET_DHCP6_SERVER,                                  \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DHCP6_SERVER,                              \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DHCP6_SERVER_RULE_CNT,               \
                      RULE_TYPE_GLOBAL_ACE_DHCP6_SERVER_RULE_CNT))              \
    PACKET_TYPE(RULE_TYPE_PACKET_DHCP6_CLIENT,                                  \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DHCP6_CLIENT,                              \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DHCP6_CLIENT_RULE_CNT,               \
                      RULE_TYPE_GLOBAL_ACE_DHCP6_CLIENT_RULE_CNT))              \
    PACKET_TYPE(RULE_TYPE_PACKET_MAC_BASED_VLAN,                                \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_MAC_BASED_VLAN,                            \
                QUOTA(0,                                                        \
                      RULE_TYPE_GLOBAL_ACE_MAC_BASED_VLAN_RULE_CNT))            \
    PACKET_TYPE(RULE_TYPE_PACKET_IP_SUBNET_BASED_VLAN,                          \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_IP_SUBNET_BASED_VLAN,                      \
                QUOTA(0,                                                        \
                      RULE_TYPE_GLOBAL_ACE_IP_SUBNET_BASED_VLAN_RULE_CNT))      \
    PACKET_TYPE(RULE_TYPE_PACKET_IGMP,                                          \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_IGMP,                                      \
                QUOTA(RULE_TYPE_GLOBAL_ACE_IGMP_RULE_CNT,                       \
                      RULE_TYPE_GLOBAL_ACE_IGMP_RULE_CNT))                      \
    PACKET_TYPE(RULE_TYPE_PACKET_MVR_REC_VLAN,                                  \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_MVR_REC_VLAN,                              \
                QUOTA(RULE_TYPE_GLOBAL_ACE_MVR_REC_VLAN_RULE_CNT,               \
                      RULE_TYPE_GLOBAL_ACE_MVR_REC_VLAN_RULE_CNT))              \
    PACKET_TYPE(RULE_TYPE_PACKET_MAC_BASED_MIRROR,                              \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_MAC_BASED_MIRROR,                          \
                QUOTA(0,                                                        \
                      RULE_TYPE_GLOBAL_ACE_MAC_BASED_MIRROR_RULE_CNT))          \
    PACKET_TYPE(RULE_TYPE_PACKET_PPPOED,                                        \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_PPPOED,                                    \
                QUOTA(RULE_TYPE_GLOBAL_ACE_PPPOED_RULE_CNT,                     \
                      RULE_TYPE_GLOBAL_ACE_PPPOED_RULE_CNT))                    \
    PACKET_TYPE(RULE_TYPE_PACKET_CFM,                                           \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_CFM,                                       \
                QUOTA(RULE_TYPE_GLOBAL_ACE_CFM_RULE_CNT,                        \
                      RULE_TYPE_GLOBAL_ACE_CFM_RULE_CNT))                       \
    PACKET_TYPE(RULE_TYPE_PACKET_RAPS,                                          \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_RAPS,                                      \
                QUOTA(RULE_TYPE_GLOBAL_ACE_RAPS_RULE_CNT,                       \
                      RULE_TYPE_GLOBAL_ACE_RAPS_RULE_CNT))                      \
    PACKET_TYPE(RULE_TYPE_PACKET_L2PT,                                          \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_L2PT,                                      \
                QUOTA(RULE_TYPE_GLOBAL_ACE_L2PT_RULE_CNT,                       \
                      RULE_TYPE_GLOBAL_ACE_L2PT_RULE_CNT))                      \
    PACKET_TYPE(RULE_TYPE_PACKET_CDP,                                           \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_CDP,                                       \
                QUOTA(RULE_TYPE_GLOBAL_ACE_CDP_RULE_CNT,                        \
                      RULE_TYPE_GLOBAL_ACE_CDP_RULE_CNT))                       \
    PACKET_TYPE(RULE_TYPE_PACKET_PVST,                                          \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_PVST,                                      \
                QUOTA(RULE_TYPE_GLOBAL_ACE_PVST_RULE_CNT,                       \
                      RULE_TYPE_GLOBAL_ACE_PVST_RULE_CNT))                      \
    PACKET_TYPE(RULE_TYPE_PACKET_MY_MAC_ARP,                                    \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_MY_MAC_ARP,                                \
                QUOTA(RULE_TYPE_GLOBAL_ACE_MY_MAC_ARP_RULE_CNT,                 \
                      RULE_TYPE_GLOBAL_ACE_MY_MAC_ARP_RULE_CNT))                \
    PACKET_TYPE(RULE_TYPE_PACKET_RIP,                                           \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_RIP,                                       \
                QUOTA(RULE_TYPE_GLOBAL_ACE_RIP_RULE_CNT,                        \
                      RULE_TYPE_GLOBAL_ACE_RIP_RULE_CNT))                       \
    PACKET_TYPE(RULE_TYPE_PACKET_PTP_EVENT_ETH,                                 \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_PTP_EVENT_ETH,                             \
                QUOTA(RULE_TYPE_GLOBAL_ACE_PTP_EVENT_ETH_RULE_CNT,              \
                      RULE_TYPE_GLOBAL_ACE_PTP_EVENT_ETH_RULE_CNT))             \
    PACKET_TYPE(RULE_TYPE_PACKET_PTP_EVENT_UDP,                                 \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_PTP_EVENT_UDP,                             \
                QUOTA(RULE_TYPE_GLOBAL_ACE_PTP_EVENT_UDP_RULE_CNT,              \
                      RULE_TYPE_GLOBAL_ACE_PTP_EVENT_UDP_RULE_CNT))             \
    PACKET_TYPE(RULE_TYPE_PACKET_PTP_GENERAL_UDP,                               \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_PTP_GENERAL_UDP,                           \
                QUOTA(RULE_TYPE_GLOBAL_ACE_PTP_GENERAL_UDP_RULE_CNT,            \
                      RULE_TYPE_GLOBAL_ACE_PTP_GENERAL_UDP_RULE_CNT))           \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6_ND_RS,                                     \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_IP6_ND_RS,                                 \
                QUOTA(RULE_TYPE_GLOBAL_ACE_IP6_ND_RS_RULE_CNT,                  \
                      RULE_TYPE_GLOBAL_ACE_IP6_ND_RS_RULE_CNT))                 \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6_ND_RA,                                     \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_IP6_ND_RA,                                 \
                QUOTA(RULE_TYPE_GLOBAL_ACE_IP6_ND_RA_RULE_CNT,                  \
                      RULE_TYPE_GLOBAL_ACE_IP6_ND_RA_RULE_CNT))                 \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6_ND_NS,                                     \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_IP6_ND_NS,                                 \
                QUOTA(RULE_TYPE_GLOBAL_ACE_IP6_ND_NS_RULE_CNT,                  \
                      RULE_TYPE_GLOBAL_ACE_IP6_ND_NS_RULE_CNT))                 \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6_ND_NA,                                     \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_IP6_ND_NA,                                 \
                QUOTA(RULE_TYPE_GLOBAL_ACE_IP6_ND_NA_RULE_CNT,                  \
                      RULE_TYPE_GLOBAL_ACE_IP6_ND_NA_RULE_CNT))                 \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6_ND_REDIRECT,                               \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_IP6_ND_REDIRECT,                           \
                QUOTA(RULE_TYPE_GLOBAL_ACE_IP6_ND_REDIRECT_RULE_CNT,            \
                      RULE_TYPE_GLOBAL_ACE_IP6_ND_REDIRECT_RULE_CNT))           \
    PACKET_TYPE(RULE_TYPE_PACKET_DOS_CHARGEN,                                   \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DOS_CHARGEN,                               \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DOS_CHARGEN_RULE_CNT,                \
                      RULE_TYPE_GLOBAL_ACE_DOS_CHARGEN_RULE_CNT))               \
    PACKET_TYPE(RULE_TYPE_PACKET_DOS_ECHO,                                      \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DOS_ECHO,                                  \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DOS_ECHO_RULE_CNT,                   \
                      RULE_TYPE_GLOBAL_ACE_DOS_ECHO_RULE_CNT))                  \
    PACKET_TYPE(RULE_TYPE_PACKET_DOS_TCP_FLOODING,                              \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DOS_TCP_FLOODING,                          \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DOS_TCP_FLOODING_RULE_CNT,           \
                      RULE_TYPE_GLOBAL_ACE_DOS_TCP_FLOODING_RULE_CNT))          \
    PACKET_TYPE(RULE_TYPE_PACKET_DOS_UDP_FLOODING,                              \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DOS_UDP_FLOODING,                          \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DOS_UDP_FLOODING_RULE_CNT,           \
                      RULE_TYPE_GLOBAL_ACE_DOS_UDP_FLOODING_RULE_CNT))          \
    PACKET_TYPE(RULE_TYPE_PACKET_DOS_WIN_NUKE,                                  \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DOS_WIN_NUKE,                              \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DOS_WIN_NUKE_RULE_CNT,               \
                      RULE_TYPE_GLOBAL_ACE_DOS_WIN_NUKE_RULE_CNT))              \
    PACKET_TYPE(RULE_TYPE_PACKET_DOS_SMURF,                                     \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_DOS_SMURF,                                 \
                QUOTA(RULE_TYPE_GLOBAL_ACE_DOS_SMURF_RULE_CNT,                  \
                      RULE_TYPE_GLOBAL_ACE_DOS_SMURF_RULE_CNT))                 \
    PACKET_TYPE(RULE_TYPE_PACKET_VRRP,                                          \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_VRRP,                                      \
                QUOTA(RULE_TYPE_GLOBAL_ACE_VRRP_RULE_CNT,                       \
                      RULE_TYPE_GLOBAL_ACE_VRRP_RULE_CNT))                      \
    PACKET_TYPE(RULE_TYPE_PACKET_IP_BCAST,                                      \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_IP_BCAST,                                  \
                QUOTA(RULE_TYPE_GLOBAL_ACE_IP_BCAST_RULE_CNT,                   \
                      RULE_TYPE_GLOBAL_ACE_IP_BCAST_RULE_CNT))                  \
    PACKET_TYPE(RULE_TYPE_PACKET_AF_DENY_CDP,                                   \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_AF_DENY_CDP,                                 \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_AF_DENY_CDP_RULE_CNT))                 \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_AF_DENY_PVST,                                  \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_AF_DENY_PVST,                                \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_AF_DENY_PVST_RULE_CNT))                \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_MY_MAC_CRAFT_PORT,                             \
                RULE_TYPE_INTERFACE_CRAFT_PORT,                                 \
                RULE_TYPE_CRAFT_PORT_MY_MAC,                                    \
                QUOTA(RULE_TYPE_CRAFT_PORT_MY_MAC_RULE_CNT,                     \
                      RULE_TYPE_CRAFT_PORT_MY_MAC_RULE_CNT))                    \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_OAM_LBK,                                       \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_OAM_LBK,                                     \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_OAM_LBK_RULE_CNT))                     \
    PACKET_TYPE(RULE_TYPE_PACKET_L2PT_CUSTOM_PDU,                               \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_L2PT_CUSTOM_PDU,                             \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_L2PT_CUSTOM_PDU_RULE_CNT))             \
    PACKET_TYPE(RULE_TYPE_PACKET_WA_PERMIT_DHCP_CLIENT,                         \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_DHCP_CLIENT,                                 \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_DHCP_CLIENT_RULE_CNT))                 \
    PACKET_TYPE(RULE_TYPE_PACKET_IP_MULTICAST_DATA,                             \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA,                           \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA_RULE_CNT))           \
    PACKET_TYPE(RULE_TYPE_PACKET_IP_MULTICAST_DATA_DROP,                        \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA_DROP,                      \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA_DROP_RULE_CNT))      \
    PACKET_TYPE(RULE_TYPE_PACKET_TRUNK_IP_MULTICAST_DATA_DROP,                  \
                RULE_TYPE_INTERFACE_TRUNK,                                      \
                RULE_CONFIG_TRUNK_PORT_IP_MULTICAST_DATA_DROP,                  \
                QUOTA(0,                                                        \
                      RULE_CONFIG_TRUNK_PORT_IP_MULTICAST_DATA_DROP_RULE_CNT))  \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6_MULTICAST_DATA_DROP,                       \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP6_MULTICAST_DATA_DROP,                     \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP6_MULTICAST_DATA_DROP_RULE_CNT))     \
    PACKET_TYPE(RULE_TYPE_PACKET_TRUNK_IP6_MULTICAST_DATA_DROP,                 \
                RULE_TYPE_INTERFACE_TRUNK,                                      \
                RULE_CONFIG_TRUNK_PORT_IP6_MULTICAST_DATA_DROP,                 \
                QUOTA(0,                                                        \
                      RULE_CONFIG_TRUNK_PORT_IP6_MULTICAST_DATA_DROP_RULE_CNT)) \
    PACKET_TYPE(RULE_TYPE_PACKET_UP_VID,                                        \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_SQINQ,                                       \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_SQINQ_RULE_CNT))                       \
    PACKET_TYPE(RULE_TYPE_PACKET_WA_PERMIT_DNS_CLIENT,                          \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_DNS,                                         \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_DNS_RULE_CNT))                         \
    PACKET_TYPE(RULE_TYPE_PACKET_IP_SUBNET_BASED_VLAN_PER_PORT,                 \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP_SUBNET_BASED_VLAN,                        \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP_SUBNET_BASED_VLAN_RULE_CNT))        \
    PACKET_TYPE(RULE_TYPE_PACKET_TRUNK_VLAN_TRANSLATION,                        \
                RULE_TYPE_INTERFACE_TRUNK,                                      \
                RULE_TYPE_DFLT_ACE_TRUNK_VLAN_TRANSLATION,                      \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_TRUNK_VLAN_TRANSLATION_RULE_CNT))      \
    PACKET_TYPE(RULE_TYPE_PACKET_VLAN_TRANSLATION,                              \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_VLAN_TRANSLATION,                            \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_VLAN_TRANSLATION_RULE_CNT))            \
    PACKET_TYPE(RULE_TYPE_PACKET_ACL,                                           \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_ACL,                                         \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_ACL_RULE_CNT))                         \
    PACKET_TYPE(RULE_TYPE_PACKET_SIP,                                           \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_WEB_AUTH_PERMIT_HOST,                        \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_WEB_AUTH_PERMIT_HOST_RULE_CNT))        \
    PACKET_TYPE(RULE_TYPE_PACKET_SA_SIP_VID,                                    \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP_SRC_GRD_PERMIT_HOST,                      \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP_SRC_GRD_PERMIT_HOST_RULE_CNT))      \
    PACKET_TYPE(RULE_TYPE_PACKET_SIP6_VID,                                      \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_PERMIT_HOST,                     \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_PERMIT_HOST_RULE_CNT))     \
    PACKET_TYPE(RULE_TYPE_PACKET_WA_REDIR_HTTP_CLIENT,                          \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_WEB_AUTH_REDIR_HTTP,                         \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_WEB_AUTH_REDIR_HTTP_RULE_CNT))         \
    PACKET_TYPE(RULE_TYPE_PACKET_IPSG_DENY_IP_ANY,                              \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP_SRC_GRD_DENY_IP_ANY,                      \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP_SRC_GRD_DENY_IP_ANY_RULE_CNT))      \
    PACKET_TYPE(RULE_TYPE_PACKET_WA_DENY_IP_ANY,                                \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_WEB_AUTH_DENY_IP_ANY,                        \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_WEB_AUTH_DENY_IP_ANY_RULE_CNT))        \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6SG_PERMIT_LINKLOCAL,                        \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL,                    \
                QUOTA(RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_RULE_CNT,     \
                      RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_RULE_CNT))    \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6_ANY,                                       \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_DENY_IP6_ANY,                    \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_DENY_IP6_ANY_RULE_CNT))    \
    PACKET_TYPE(RULE_TYPE_PACKET_PPPOED_PER_PORT,                               \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_PPPOED,                                      \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_PPPOED_RULE_CNT))                      \
    PACKET_TYPE(RULE_TYPE_PACKET_ACL_MATCH_ANY_ANY,                             \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_ACL_MATCH_ANY_ANY,                           \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_ACL_MATCH_ANY_ANY_RULE_CNT))           \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_TRUNK_ACL,                                     \
                RULE_TYPE_INTERFACE_TRUNK,                                      \
                RULE_CONFIG_TRUNK_PORT_ACL,                                     \
                QUOTA(0,                                                        \
                      RULE_CONFIG_TRUNK_PORT_ACL_RULE_CNT))                     \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_TRUNK_ACL_MATCH_ANY_ANY,                       \
                RULE_TYPE_INTERFACE_TRUNK,                                      \
                RULE_CONFIG_TRUNK_PORT_ACL_MATCH_ANY_ANY,                       \
                QUOTA(0,                                                        \
                      RULE_CONFIG_TRUNK_PORT_ACL_MATCH_ANY_ANY_RULE_CNT))       \
                                                                                \
    /* Ingress Lookup 0-1 Begin */                                              \
    PACKET_TYPE(RULE_TYPE_PACKET_INGRESS_0_1_BEGIN,                             \
                0xffffffff,                                                     \
                RULE_TYPE_PACKET_INGRESS_0_1_BEGIN,                             \
                QUOTA(0, 0))                                                    \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_ALL_PORTS_ACL_0_1,                             \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_CONFIG_ALL_PORTS_ACL_0_1,                                  \
                QUOTA(0,                                                        \
                      RULE_CONFIG_ALL_PORTS_ACL_0_1_RULE_CNT))                  \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_BPDU_RATE_LIMIT_PER_PORT,                      \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_PORT,                    \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_PORT_RULE_CNT))    \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_BPDU_RATE_LIMIT_PER_TRUNK,                     \
                RULE_TYPE_INTERFACE_TRUNK,                                      \
                RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_TRUNK,                   \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_TRUNK_RULE_CNT))   \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_QOS,                                           \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_QOS,                                         \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_QOS_RULE_CNT))                         \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_RATE_LIMIT,                                    \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_RATE_LIMIT,                                  \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_RATE_LIMIT_RULE_CNT))                  \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_TRUNK_RATE_LIMIT,                              \
                RULE_TYPE_INTERFACE_TRUNK,                                      \
                RULE_TYPE_DFLT_ACE_RATE_LIMIT,                                  \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_RATE_LIMIT_RULE_CNT))                  \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_TRUNK_QOS,                                     \
                RULE_TYPE_INTERFACE_TRUNK,                                      \
                RULE_CONFIG_TRUNK_PORT_QOS,                                     \
                QUOTA(0,                                                        \
                      RULE_CONFIG_TRUNK_PORT_QOS_RULE_CNT))                     \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_ALL_PORTS_ACL_MATCH_ANY_ANY_0_1,               \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_CONFIG_ALL_PORTS_ACL_MATCH_ANY_ANY_0_1,                    \
                QUOTA(0,                                                        \
                      RULE_CONFIG_ALL_PORTS_ACL_MATCH_ANY_ANY_0_1_RULE_CNT))    \
                                                                                \
    /* Ingress Lookup 1 Begin */                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_INGRESS_1_BEGIN,                               \
                0xffffffff,                                                     \
                RULE_TYPE_PACKET_INGRESS_1_BEGIN,                               \
                QUOTA(0, 0))                                                    \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_ALL_PORTS_ACL,                                 \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_CONFIG_ALL_PORTS_ACL,                                      \
                QUOTA(0,                                                        \
                      RULE_CONFIG_ALL_PORTS_ACL_RULE_CNT))                      \
    PACKET_TYPE(RULE_TYPE_PACKET_PBR,                                           \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_PBR,                                       \
                QUOTA(0,                                                        \
                      RULE_TYPE_GLOBAL_ACE_PBR_RULE_CNT))                       \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_L3_LITE_ROUTING,                               \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_L3_LITE_ROUTING,                           \
                QUOTA(RULE_TYPE_GLOBAL_ACE_L3_LITE_ROUTING_RULE_CNT,            \
                      RULE_TYPE_GLOBAL_ACE_L3_LITE_ROUTING_RULE_CNT))           \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_WA_PERMIT_HOST_STRICT,                         \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_WA_PERMIT_HOST_STRICT,                       \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_WA_PERMIT_HOST_STRICT_RULE_CNT))       \
    PACKET_TYPE(RULE_TYPE_PACKET_IP_SG_PERMIT_HOST_STRICT,                      \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP_SG_PERMIT_HOST_STRICT,                    \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP_SG_PERMIT_HOST_STRICT_RULE_CNT))    \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6_SG_PERMIT_HOST_STRICT,                     \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP6_SG_PERMIT_HOST_STRICT,                   \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP6_SG_PERMIT_HOST_STRICT_RULE_CNT))   \
    PACKET_TYPE(RULE_TYPE_PACKET_WA_REDIR_HTTP_STRICT,                          \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_WA_REDIR_HTTP_STRICT,                        \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_WA_REDIR_HTTP_STRICT_RULE_CNT))        \
    PACKET_TYPE(RULE_TYPE_PACKET_IP_SG_DENY_IP_ANY_STRICT,                      \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP_SG_DENY_IP_ANY_STRICT,                    \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP_SG_DENY_IP_ANY_STRICT_RULE_CNT))    \
    PACKET_TYPE(RULE_TYPE_PACKET_WA_DENY_IP_ANY_STRICT,                         \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_WA_DENY_IP_ANY_STRICT,                       \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_WA_DENY_IP_ANY_STRICT_RULE_CNT))       \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6SG_PERMIT_LINKLOCAL_STRICT,                 \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_STRICT,                 \
                QUOTA(RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_STRICT_RULE_CNT,  \
                      RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_STRICT_RULE_CNT)) \
    PACKET_TYPE(RULE_TYPE_PACKET_IP6_SG_DENY_IP6_ANY_STRICT,                    \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_IP6_SG_DENY_IP6_ANY_STRICT,                  \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_IP6_SG_DENY_IP6_ANY_STRICT_RULE_CNT))  \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_ALL_PORTS_ACL_MATCH_ANY_ANY,                   \
                RULE_TYPE_INTERFACE_ALL_UNIT,                                   \
                RULE_CONFIG_ALL_PORTS_ACL_MATCH_ANY_ANY,                        \
                QUOTA(0,                                                        \
                      RULE_CONFIG_ALL_PORTS_ACL_MATCH_ANY_ANY_RULE_CNT))        \
                                                                                \
    /* Egress Begin */                                                          \
    PACKET_TYPE(RULE_TYPE_PACKET_EGRESS_BEGIN,                                  \
                0xffffffff,                                                     \
                RULE_TYPE_PACKET_EGRESS_BEGIN,                                  \
                QUOTA(0, 0))                                                    \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_EGRESS_VLAN_TRANSLATION,                       \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_EGRESS_VLAN_TRANSLATION,                     \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_EGRESS_VLAN_TRANSLATION_RULE_CNT))     \
    PACKET_TYPE(RULE_TYPE_PACKET_EGRESS_ACL,                                    \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_EGRESS_ACL,                                  \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_EGRESS_ACL_RULE_CNT))                  \
    PACKET_TYPE(RULE_TYPE_PACKET_EGRESS_ACL_MATCH_ANY_ANY,                      \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_EGRESS_ACL_MATCH_ANY_ANY,                    \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_EGRESS_ACL_MATCH_ANY_ANY_RULE_CNT))    \
    PACKET_TYPE(RULE_TYPE_PACKET_EGRESS_QOS,                                    \
                RULE_TYPE_INTERFACE_UPORT,                                      \
                RULE_TYPE_DFLT_ACE_EGRESS_QOS,                                  \
                QUOTA(0,                                                        \
                      RULE_TYPE_DFLT_ACE_EGRESS_QOS_RULE_CNT))                  \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_CPU_PORT_EGRESS_ACL,                           \
                RULE_TYPE_INTERFACE_CPU_PORT,                                   \
                RULE_CONFIG_CPU_PORT_EGRESS_ACL,                                \
                QUOTA(0,                                                        \
                      RULE_CONFIG_CPU_PORT_EGRESS_ACL_RULE_CNT))                \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_CPU_PORT_EGRESS_ACL_MATCH_ANY_ANY,             \
                RULE_TYPE_INTERFACE_CPU_PORT,                                   \
                RULE_CONFIG_CPU_PORT_EGRESS_ACL_MATCH_ANY_ANY,                  \
                QUOTA(0,                                                        \
                      RULE_CONFIG_CPU_PORT_EGRESS_ACL_MATCH_ANY_ANY_RULE_CNT))  \
                                                                                \
    PACKET_TYPE(RULE_TYPE_PACKET_CPU_PORT_EGRESS_QOS,                           \
                RULE_TYPE_INTERFACE_CPU_PORT,                                   \
                RULE_CONFIG_CPU_PORT_EGRESS_QOS,                                \
                QUOTA(0,                                                        \
                      RULE_CONFIG_CPU_PORT_EGRESS_QOS_RULE_CNT))

enum RULE_TYPE_GLOBAL_ACE
{
    RULE_TYPE_GLBL_ACE_LST(RULE_TYPE_LST_ENUM)
    RULE_TYPE_GLOBAL_ACE_MAX_TYPE
};  /* used for global acl */

typedef enum RULE_TYPE_GLOBAL_ACE   RULE_TYPE_GLOBAL_ACE_T;

enum RULE_TYPE_CRAFT_PORT_ACE
{
    RULE_TYPE_CRAFT_PORT_ACE_LST(RULE_TYPE_LST_ENUM)
    RULE_TYPE_CRAFT_PORT_ACE_MAX_TYPE
};

typedef enum RULE_TYPE_CRAFT_PORT_ACE   RULE_TYPE_CRAFT_PORT_ACE_T;

enum RULE_TYPE_DEFAULT_ACE
{
    RULE_TYPE_DFLT_ACE_LST(RULE_TYPE_LST_ENUM)
    RULE_TYPE_DFLT_ACE_MAX_TYPE,
}; /* used for per port default ace */

typedef enum RULE_TYPE_DEFAULT_ACE  RULE_TYPE_DEFAULT_ACE_T;

enum RULE_CONFIG_TRUNK_PORT_RULE_KIND_E
{
#define TRUNK_PORT_RULE(name) RULE_CONFIG_TRUNK_PORT_##name,
#include "rule_config.def"
    RULE_CONFIG_NUM_TRUNK_PORT_RULE_KIND
};

typedef enum RULE_CONFIG_TRUNK_PORT_RULE_KIND_E   RULE_CONFIG_TRUNK_PORT_RULE_KIND_T;

enum RULE_CONFIG_CPU_PORT_RULE_KIND_E
{
#define CPU_PORT_RULE(name) RULE_CONFIG_CPU_PORT_##name,
#include "rule_config.def"
    RULE_CONFIG_NUM_CPU_PORT_RULE_KIND
};

typedef enum RULE_CONFIG_CPU_PORT_RULE_KIND_E    RULE_CONFIG_CPU_PORT_RULE_KIND_T;

enum RULE_CONFIG_ALL_PORTS_RULE_KIND_E
{
#define ALL_PORTS_RULE(name) RULE_CONFIG_ALL_PORTS_##name,
#include "rule_config.def"
    RULE_CONFIG_NUM_ALL_PORTS_RULE_KIND
};

typedef enum RULE_CONFIG_ALL_PORTS_RULE_KIND_E    RULE_CONFIG_ALL_PORTS_RULE_KIND_T;

enum RULE_CONFIG_MVL_PACKET_TYPE_E
{
#define PACKET_TYPE(pktType, ifType, remapAceType, quota) pktType,

    RULE_CONFIG_PACKET_TYPE_TABLE
    RULE_TYPE_PACKET_MAX_TYPE

#undef PACKET_TYPE
};

typedef enum RULE_CONFIG_MVL_PACKET_TYPE_E RULE_CONFIG_MVL_PACKET_TYPE_T;

#define RULE_TYPE_PACKET_INGRESS_0_0_BEGIN_RULE_CNT                 0
#define RULE_TYPE_PACKET_INGRESS_0_1_BEGIN_RULE_CNT                 0
#define RULE_TYPE_PACKET_INGRESS_1_BEGIN_RULE_CNT                   0
#define RULE_TYPE_PACKET_EGRESS_BEGIN_RULE_CNT                      0

/*---------------------------------------------------------
 * Reserved rule counts for each CSC
 *---------------------------------------------------------
 */

/** The following rule count is used to create PCL only.
 */
#define RULE_TYPE_DFLT_ACE_ACL_RULE_CNT                             0xffff
#define RULE_TYPE_DFLT_ACE_ACL_MATCH_ANY_ANY_RULE_CNT               0xffff
#define RULE_TYPE_DFLT_ACE_QOS_RULE_CNT                             0xffff

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
    #define RULE_TYPE_DFLT_ACE_EGRESS_ACL_RULE_CNT                  0xffff
    #define RULE_TYPE_DFLT_ACE_EGRESS_ACL_MATCH_ANY_ANY_RULE_CNT    0xffff
    #define RULE_TYPE_DFLT_ACE_EGRESS_QOS_RULE_CNT                  0xffff
#else
    #define RULE_TYPE_DFLT_ACE_EGRESS_ACL_RULE_CNT                  0
    #define RULE_TYPE_DFLT_ACE_EGRESS_ACL_MATCH_ANY_ANY_RULE_CNT    0
    #define RULE_TYPE_DFLT_ACE_EGRESS_QOS_RULE_CNT                  0
#endif

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
    #define RULE_CONFIG_CPU_PORT_EGRESS_ACL_RULE_CNT                0xffff
    #define RULE_CONFIG_CPU_PORT_EGRESS_ACL_MATCH_ANY_ANY_RULE_CNT  0xffff
    #define RULE_CONFIG_CPU_PORT_EGRESS_QOS_RULE_CNT                0xffff
#else
    #define RULE_CONFIG_CPU_PORT_EGRESS_ACL_RULE_CNT                0
    #define RULE_CONFIG_CPU_PORT_EGRESS_ACL_MATCH_ANY_ANY_RULE_CNT  0
    #define RULE_CONFIG_CPU_PORT_EGRESS_QOS_RULE_CNT                0
#endif

#if (SYS_CPNT_QOS_V2_TRUNK_PORT == TRUE)
    #define RULE_CONFIG_TRUNK_PORT_ACL_RULE_CNT                     0xffff
    #define RULE_CONFIG_TRUNK_PORT_ACL_MATCH_ANY_ANY_RULE_CNT       0xffff
    #define RULE_CONFIG_TRUNK_PORT_QOS_RULE_CNT                     0xffff
#else
    #define RULE_CONFIG_TRUNK_PORT_ACL_RULE_CNT                     0
    #define RULE_CONFIG_TRUNK_PORT_ACL_MATCH_ANY_ANY_RULE_CNT       0
    #define RULE_CONFIG_TRUNK_PORT_QOS_RULE_CNT                     0
#endif

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
    #define RULE_CONFIG_ALL_PORTS_ACL_RULE_CNT                      0xffff
    #define RULE_CONFIG_ALL_PORTS_ACL_MATCH_ANY_ANY_RULE_CNT        0xffff
#else
    #define RULE_CONFIG_ALL_PORTS_ACL_RULE_CNT                      0
    #define RULE_CONFIG_ALL_PORTS_ACL_MATCH_ANY_ANY_RULE_CNT        0
#endif

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE && SYS_CPNT_QOS_V2_ALL_PORTS_RULE_AT_LOOKUP_0_1 == TRUE)
    #undef RULE_CONFIG_ALL_PORTS_ACL_RULE_CNT
    #undef RULE_CONFIG_ALL_PORTS_ACL_MATCH_ANY_ANY_RULE_CNT

    #define RULE_CONFIG_ALL_PORTS_ACL_RULE_CNT                          0
    #define RULE_CONFIG_ALL_PORTS_ACL_MATCH_ANY_ANY_RULE_CNT            0

    #define RULE_CONFIG_ALL_PORTS_ACL_0_1_RULE_CNT                      0xffff
    #define RULE_CONFIG_ALL_PORTS_ACL_MATCH_ANY_ANY_0_1_RULE_CNT        0xffff
#else
    #define RULE_CONFIG_ALL_PORTS_ACL_0_1_RULE_CNT                      0
    #define RULE_CONFIG_ALL_PORTS_ACL_MATCH_ANY_ANY_0_1_RULE_CNT        0
#endif /* SYS_CPNT_QOS_V2_ALL_PORTS && SYS_CPNT_QOS_V2_ALL_PORTS_RULE_AT_LOOKUP_0_1 */

#if (SYS_CPNT_DHCPSNP == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_DHCP_CLIENT_RULE_CNT               1 /* DPORT == SERVER */
    #define RULE_TYPE_GLOBAL_ACE_DHCP_SERVER_RULE_CNT               1 /* DPORT == CLIENT */
#else
    #define RULE_TYPE_GLOBAL_ACE_DHCP_CLIENT_RULE_CNT               0 /* DPORT == SERVER */
    #define RULE_TYPE_GLOBAL_ACE_DHCP_SERVER_RULE_CNT               0 /* DPORT == CLIENT */
#endif

#if (SYS_CPNT_DHCPV6SNP == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_DHCP6_CLIENT_RULE_CNT              1
    #define RULE_TYPE_GLOBAL_ACE_DHCP6_SERVER_RULE_CNT              1
#else
    #define RULE_TYPE_GLOBAL_ACE_DHCP6_CLIENT_RULE_CNT              0
    #define RULE_TYPE_GLOBAL_ACE_DHCP6_SERVER_RULE_CNT              0
#endif

#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
    #define RULE_TYPE_DFLT_ACE_IP_SRC_GRD_PERMIT_HOST_RULE_CNT      SYS_ADPT_DHCPSNP_MAX_NBR_OF_CLIENT_PER_PORT
    #define RULE_TYPE_DFLT_ACE_IP_SRC_GRD_DENY_IP_ANY_RULE_CNT      1
#else
    #define RULE_TYPE_DFLT_ACE_IP_SRC_GRD_PERMIT_HOST_RULE_CNT      0
    #define RULE_TYPE_DFLT_ACE_IP_SRC_GRD_DENY_IP_ANY_RULE_CNT      0
#endif /* SYS_CPNT_IP_SOURCE_GUARD */

#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE && SYS_CPNT_IPSG_STRICT_MODE == TRUE)
    #undef RULE_TYPE_DFLT_ACE_IP_SRC_GRD_PERMIT_HOST_RULE_CNT
    #undef RULE_TYPE_DFLT_ACE_IP_SRC_GRD_DENY_IP_ANY_RULE_CNT

    #define RULE_TYPE_DFLT_ACE_IP_SRC_GRD_PERMIT_HOST_RULE_CNT      0
    #define RULE_TYPE_DFLT_ACE_IP_SRC_GRD_DENY_IP_ANY_RULE_CNT      0

    #define RULE_TYPE_DFLT_ACE_IP_SG_PERMIT_HOST_STRICT_RULE_CNT    SYS_ADPT_DHCPSNP_MAX_NBR_OF_CLIENT_PER_PORT
    #define RULE_TYPE_DFLT_ACE_IP_SG_DENY_IP_ANY_STRICT_RULE_CNT    1
#else
    #define RULE_TYPE_DFLT_ACE_IP_SG_PERMIT_HOST_STRICT_RULE_CNT    0
    #define RULE_TYPE_DFLT_ACE_IP_SG_DENY_IP_ANY_STRICT_RULE_CNT    0
#endif /* SYS_CPNT_IP_SOURCE_GUARD && SYS_CPNT_IPSG_STRICT_MODE */

#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_RULE_CNT    1
    #define RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_PERMIT_HOST_RULE_CNT     SYS_ADPT_DHCPSNP_MAX_NBR_OF_CLIENT_PER_PORT
    #define RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_DENY_IP6_ANY_RULE_CNT    1
#else
    #define RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_RULE_CNT    0
    #define RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_PERMIT_HOST_RULE_CNT     0
    #define RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_DENY_IP6_ANY_RULE_CNT    0
#endif /* SYS_CPNT_IPV6_SOURCE_GUARD */

#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE && SYS_CPNT_IPSG_STRICT_MODE == TRUE)
    #undef RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_RULE_CNT    
    #undef RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_PERMIT_HOST_RULE_CNT
    #undef RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_DENY_IP6_ANY_RULE_CNT

    #define RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_RULE_CNT    0
    #define RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_PERMIT_HOST_RULE_CNT     0
    #define RULE_TYPE_DFLT_ACE_IP6_SRC_GRD_DENY_IP6_ANY_RULE_CNT    0

    #define RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_STRICT_RULE_CNT    1
    #define RULE_TYPE_DFLT_ACE_IP6_SG_PERMIT_HOST_STRICT_RULE_CNT          SYS_ADPT_DHCPSNP_MAX_NBR_OF_CLIENT_PER_PORT
    #define RULE_TYPE_DFLT_ACE_IP6_SG_DENY_IP6_ANY_STRICT_RULE_CNT         1
#else
    #define RULE_TYPE_GLOBAL_ACE_IP6SG_PERMIT_LINKLOCAL_STRICT_RULE_CNT    0
    #define RULE_TYPE_DFLT_ACE_IP6_SG_PERMIT_HOST_STRICT_RULE_CNT          0
    #define RULE_TYPE_DFLT_ACE_IP6_SG_DENY_IP6_ANY_STRICT_RULE_CNT         0
#endif /* SYS_CPNT_IPV6_SOURCE_GUARD && SYS_CPNT_IPSG_STRICT_MODE */

#if (SYS_CPNT_CLUSTER == TRUE || SYS_CPNT_ERPS_NON_ERPS_DEV_PROTECT == TRUE || SYS_CPNT_LBD == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC_RULE_CNT              1
#else
    #define RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC_RULE_CNT              0
#endif

#if (SYS_CPNT_LBD == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC1_RULE_CNT              1
#else
    #define RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC1_RULE_CNT              0
#endif

#if ((SYS_CPNT_UDLD == TRUE) && (SYS_CPNT_UDLD_PDU_FORMAT == SYS_CPNT_UDLD_PDU_FORMAT_ACCTON))
    #define RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC2_RULE_CNT             1
#else
    #define RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC2_RULE_CNT             0
#endif

#if (SYS_CPNT_MLAG == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC3_RULE_CNT             1
#else
    #define RULE_TYPE_GLOBAL_ACE_ORG_SPECIFIC3_RULE_CNT             0
#endif

#if (SYS_CPNT_DAI == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_ARP_RULE_CNT                       1
#else
    #define RULE_TYPE_GLOBAL_ACE_ARP_RULE_CNT                       0
#endif

#if (SYS_CPNT_MAC_VLAN == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_MAC_BASED_VLAN_RULE_CNT            SYS_ADPT_MAX_NBR_OF_MAC_VLAN_ENTRY
#else
    #define RULE_TYPE_GLOBAL_ACE_MAC_BASED_VLAN_RULE_CNT            0
#endif

#if (SYS_CPNT_IP_SUBNET_VLAN_IMPLEMENTED_BY_RULE == TRUE)
  #if (SYS_CPNT_IP_SUBNET_VLAN_BIND_PORT == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_IP_SUBNET_BASED_VLAN_RULE_CNT      0
    #define RULE_TYPE_DFLT_ACE_IP_SUBNET_BASED_VLAN_RULE_CNT        SYS_ADPT_MAX_NBR_OF_IP_SUBNET_VLAN_ENTRY
  #else
    #define RULE_TYPE_GLOBAL_ACE_IP_SUBNET_BASED_VLAN_RULE_CNT      SYS_ADPT_MAX_NBR_OF_IP_SUBNET_VLAN_ENTRY
    #define RULE_TYPE_DFLT_ACE_IP_SUBNET_BASED_VLAN_RULE_CNT        0
  #endif
#else
    #define RULE_TYPE_GLOBAL_ACE_IP_SUBNET_BASED_VLAN_RULE_CNT      0
    #define RULE_TYPE_DFLT_ACE_IP_SUBNET_BASED_VLAN_RULE_CNT        0
#endif

#if (SYS_CPNT_SWDRV_TRAP_IGMP_BY_RULE == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_IGMP_RULE_CNT                      1
    #define RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA_RULE_CNT           1
#else
    #define RULE_TYPE_GLOBAL_ACE_IGMP_RULE_CNT                      0
    #define RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA_RULE_CNT           0
#endif

#if (SYS_CPNT_VRRP == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_VRRP_RULE_CNT                      1
#else
    #define RULE_TYPE_GLOBAL_ACE_VRRP_RULE_CNT                      0
#endif

#if (SYS_CPNT_IP_MULTICAST_DATA_DROP == TRUE) || (SYS_CPNT_MVR == TRUE)
    #define RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA_DROP_RULE_CNT      1
    #define RULE_CONFIG_TRUNK_PORT_IP_MULTICAST_DATA_DROP_RULE_CNT  0xffff
#else
    #define RULE_TYPE_DFLT_ACE_IP_MULTICAST_DATA_DROP_RULE_CNT      0
    #define RULE_CONFIG_TRUNK_PORT_IP_MULTICAST_DATA_DROP_RULE_CNT  0
#endif

#if (SYS_CPNT_IPV6_MULTICAST_DATA_DROP == TRUE) || (SYS_CPNT_MVR6 == TRUE)
    #define RULE_TYPE_DFLT_ACE_IP6_MULTICAST_DATA_DROP_RULE_CNT     1
    #define RULE_CONFIG_TRUNK_PORT_IP6_MULTICAST_DATA_DROP_RULE_CNT 0xffff
#else
    #define RULE_TYPE_DFLT_ACE_IP6_MULTICAST_DATA_DROP_RULE_CNT     0
    #define RULE_CONFIG_TRUNK_PORT_IP6_MULTICAST_DATA_DROP_RULE_CNT 0
#endif

#if (SYS_CPNT_PPPOE_IA_TRAP_PPPOED_BY_GLOBAL_RULE == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_PPPOED_RULE_CNT                    1
    #define RULE_TYPE_DFLT_ACE_PPPOED_RULE_CNT                      0
#else
    #define RULE_TYPE_GLOBAL_ACE_PPPOED_RULE_CNT                    0
    #define RULE_TYPE_DFLT_ACE_PPPOED_RULE_CNT                      1
#endif

#if (SYS_CPNT_BPDU_LOOPBACK_RATELIMIT == TRUE)
    #define RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_PORT_RULE_CNT      1
    #define RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_TRUNK_RULE_CNT     1
    #define RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_PORT_RATE_LIMIT_RULE_CNT        1
    #define RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_TRUNK_RATE_LIMIT_RULE_CNT       1
    #define RULE_TYPE_GLOBAL_ACE_BPDU_RULE_CNT                        1
#else
    #define RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_PORT_RULE_CNT      0
    #define RULE_TYPE_DFLT_ACE_BPDU_RATE_LIMIT_PER_TRUNK_RULE_CNT     0
    #define RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_PORT_RATE_LIMIT_RULE_CNT        0
    #define RULE_TYPE_DFLT_ACE_LOOP_DENY_ANY_PER_TRUNK_RATE_LIMIT_RULE_CNT       0
    #define RULE_TYPE_GLOBAL_ACE_BPDU_RULE_CNT                        0
#endif

#if (SYS_CPNT_CFM == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_CFM_RULE_CNT                       1
#else
    #define RULE_TYPE_GLOBAL_ACE_CFM_RULE_CNT                       0
#endif

#if (SYS_CPNT_MVR_RECEIVER_VLAN_SUPPORT == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_MVR_REC_VLAN_RULE_CNT              SYS_ADPT_MVR_MAX_NBR_OF_RECEIVER_GROUPS
#else
    #define RULE_TYPE_GLOBAL_ACE_MVR_REC_VLAN_RULE_CNT              0
#endif

#if (SYS_CPNT_WEBAUTH == TRUE)
    #define RULE_TYPE_DFLT_ACE_WEB_AUTH_REDIR_HTTP_RULE_CNT         1
    #define RULE_TYPE_DFLT_ACE_WEB_AUTH_PERMIT_HOST_RULE_CNT        SYS_ADPT_WEBAUTH_MAX_NBR_OF_HOSTS_PER_PORT
    #define RULE_TYPE_DFLT_ACE_DNS_RULE_CNT                         1
    #define RULE_TYPE_DFLT_ACE_DHCP_CLIENT_RULE_CNT                 1
    #define RULE_TYPE_DFLT_ACE_WEB_AUTH_DENY_IP_ANY_RULE_CNT        1
#else
    #define RULE_TYPE_DFLT_ACE_WEB_AUTH_REDIR_HTTP_RULE_CNT         0
    #define RULE_TYPE_DFLT_ACE_WEB_AUTH_PERMIT_HOST_RULE_CNT        0
    #define RULE_TYPE_DFLT_ACE_DNS_RULE_CNT                         0
    #define RULE_TYPE_DFLT_ACE_DHCP_CLIENT_RULE_CNT                 0
    #define RULE_TYPE_DFLT_ACE_WEB_AUTH_DENY_IP_ANY_RULE_CNT        0
#endif

#if (SYS_CPNT_WEBAUTH_STRICT == TRUE)
    #undef RULE_TYPE_DFLT_ACE_WEB_AUTH_REDIR_HTTP_RULE_CNT
    #undef RULE_TYPE_DFLT_ACE_WEB_AUTH_PERMIT_HOST_RULE_CNT
    #undef RULE_TYPE_DFLT_ACE_WEB_AUTH_DENY_IP_ANY_RULE_CNT

    #define RULE_TYPE_DFLT_ACE_WEB_AUTH_REDIR_HTTP_RULE_CNT         0
    #define RULE_TYPE_DFLT_ACE_WEB_AUTH_PERMIT_HOST_RULE_CNT        0
    #define RULE_TYPE_DFLT_ACE_WEB_AUTH_DENY_IP_ANY_RULE_CNT        0

    #define RULE_TYPE_DFLT_ACE_WA_REDIR_HTTP_STRICT_RULE_CNT        1
    #define RULE_TYPE_DFLT_ACE_WA_PERMIT_HOST_STRICT_RULE_CNT       SYS_ADPT_WEBAUTH_MAX_NBR_OF_HOSTS_PER_PORT
    #define RULE_TYPE_DFLT_ACE_WA_DENY_IP_ANY_STRICT_RULE_CNT       1
#else
    #define RULE_TYPE_DFLT_ACE_WA_REDIR_HTTP_STRICT_RULE_CNT        0
    #define RULE_TYPE_DFLT_ACE_WA_PERMIT_HOST_STRICT_RULE_CNT       0
    #define RULE_TYPE_DFLT_ACE_WA_DENY_IP_ANY_STRICT_RULE_CNT       0
#endif /* SYS_CPNT_WEBAUTH_STRICT */

#if (SYS_CPNT_MAC_BASED_MIRROR == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_MAC_BASED_MIRROR_RULE_CNT          SYS_ADPT_MAX_NBR_OF_MAC_BASED_MIRROR_ENTRY
#else
    #define RULE_TYPE_GLOBAL_ACE_MAC_BASED_MIRROR_RULE_CNT          0
#endif

#if (SYS_CPNT_SWCTRL_QINQ_SERVICE == TRUE)
    #define RULE_TYPE_DFLT_ACE_SQINQ_RULE_CNT                       SYS_ADPT_MAX_NBR_OF_QINQ_SERVICE_SUBSCRIPTION_PER_PORT
#else
    #define RULE_TYPE_DFLT_ACE_SQINQ_RULE_CNT                       0
#endif

#if (SYS_CPNT_ERPS == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_RAPS_RULE_CNT                      1
#else
    #define RULE_TYPE_GLOBAL_ACE_RAPS_RULE_CNT                      0
#endif

#if (SYS_CPNT_EFM_OAM_REMOTE_LB_ACTIVELY == TRUE)
    #define RULE_TYPE_DFLT_ACE_OAM_LBK_RULE_CNT                     1
#else
    #define RULE_TYPE_DFLT_ACE_OAM_LBK_RULE_CNT                     0
#endif

#if (SYS_CPNT_QINQ_L2PT_DA_REMARK == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_L2PT_RULE_CNT                      1
#else
    #define RULE_TYPE_GLOBAL_ACE_L2PT_RULE_CNT                      0
#endif

#if (SYS_CPNT_QINQ_L2PT_CUSTOM_PDU == TRUE)
    #define RULE_TYPE_DFLT_ACE_L2PT_CUSTOM_PDU_RULE_CNT             SYS_ADPT_QINQ_L2PT_MAX_NBR_OF_CUSTOM_PDU_PER_PORT
#else
    #define RULE_TYPE_DFLT_ACE_L2PT_CUSTOM_PDU_RULE_CNT             0
#endif

#if (SYS_CPNT_QINQ_L2PT_CDP == TRUE) || (SYS_CPNT_QINQ_L2PT_VTP == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_CDP_RULE_CNT                       1
#else
    #define RULE_TYPE_GLOBAL_ACE_CDP_RULE_CNT                       0
#endif

#if (SYS_CPNT_QINQ_L2PT_PVST == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_PVST_RULE_CNT                      1
#else
    #define RULE_TYPE_GLOBAL_ACE_PVST_RULE_CNT                      0
#endif

#if (SYS_CPNT_ROUTING == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_MY_MAC_ARP_RULE_CNT                1
#else
    #define RULE_TYPE_GLOBAL_ACE_MY_MAC_ARP_RULE_CNT                0
#endif /* SYS_CPNT_ROUTING */

#if (SYS_CPNT_CRAFT_PORT == TRUE) && (SYS_CPNT_CRAFT_PORT_MODE == SYS_CPNT_CRAFT_PORT_MODE_FRONT_PORT_CRAFT_PORT)
    #define RULE_TYPE_CRAFT_PORT_MY_MAC_RULE_CNT                    1
#else
    #define RULE_TYPE_CRAFT_PORT_MY_MAC_RULE_CNT                    0
#endif /* SYS_CPNT_CRAFT_PORT */

#if (SYS_CPNT_RIP == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_RIP_RULE_CNT                       1
#else
    #define RULE_TYPE_GLOBAL_ACE_RIP_RULE_CNT                       0
#endif

#if (SYS_CPNT_PTP == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_PTP_EVENT_ETH_RULE_CNT             1
    #define RULE_TYPE_GLOBAL_ACE_PTP_EVENT_UDP_RULE_CNT             1
    #define RULE_TYPE_GLOBAL_ACE_PTP_GENERAL_UDP_RULE_CNT           1
#else
    #define RULE_TYPE_GLOBAL_ACE_PTP_EVENT_ETH_RULE_CNT             0
    #define RULE_TYPE_GLOBAL_ACE_PTP_EVENT_UDP_RULE_CNT             0
    #define RULE_TYPE_GLOBAL_ACE_PTP_GENERAL_UDP_RULE_CNT           0
#endif  /* SYS_CPNT_PTP */

#if (SYS_CPNT_ND_BY_RULE == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_IP6_ND_RS_RULE_CNT                 1
    #define RULE_TYPE_GLOBAL_ACE_IP6_ND_RA_RULE_CNT                 1
    #define RULE_TYPE_GLOBAL_ACE_IP6_ND_NS_RULE_CNT                 1
    #define RULE_TYPE_GLOBAL_ACE_IP6_ND_NA_RULE_CNT                 1
    #define RULE_TYPE_GLOBAL_ACE_IP6_ND_REDIRECT_RULE_CNT           1
#else
    #define RULE_TYPE_GLOBAL_ACE_IP6_ND_RS_RULE_CNT                 0
    #define RULE_TYPE_GLOBAL_ACE_IP6_ND_RA_RULE_CNT                 0
    #define RULE_TYPE_GLOBAL_ACE_IP6_ND_NS_RULE_CNT                 0
    #define RULE_TYPE_GLOBAL_ACE_IP6_ND_NA_RULE_CNT                 0
    #define RULE_TYPE_GLOBAL_ACE_IP6_ND_REDIRECT_RULE_CNT           0
#endif

/* for DoS Protection
 */
#if (SYS_CPNT_DOS_ECHO_CHARGEN == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_DOS_CHARGEN_RULE_CNT               1
    #define RULE_TYPE_GLOBAL_ACE_DOS_ECHO_RULE_CNT                  1
#else
    #define RULE_TYPE_GLOBAL_ACE_DOS_CHARGEN_RULE_CNT               0
    #define RULE_TYPE_GLOBAL_ACE_DOS_ECHO_RULE_CNT                  0
#endif

#if (SYS_CPNT_DOS_SMURF == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_DOS_SMURF_RULE_CNT                 1
#else
    #define RULE_TYPE_GLOBAL_ACE_DOS_SMURF_RULE_CNT                 0
#endif

#if (SYS_CPNT_UDP_HELPER == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_IP_BCAST_RULE_CNT                  1
#else
    #define RULE_TYPE_GLOBAL_ACE_IP_BCAST_RULE_CNT                  0
#endif

#if (SYS_CPNT_DOS_TCP_FLOODING == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_DOS_TCP_FLOODING_RULE_CNT          1
#else
    #define RULE_TYPE_GLOBAL_ACE_DOS_TCP_FLOODING_RULE_CNT          0
#endif

#if (SYS_CPNT_DOS_UDP_FLOODING == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_DOS_UDP_FLOODING_RULE_CNT          1
#else
    #define RULE_TYPE_GLOBAL_ACE_DOS_UDP_FLOODING_RULE_CNT          0
#endif

#if (SYS_CPNT_DOS_WIN_NUKE == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_DOS_WIN_NUKE_RULE_CNT              1
#else
    #define RULE_TYPE_GLOBAL_ACE_DOS_WIN_NUKE_RULE_CNT              0
#endif

#if (SYS_CPNT_ROUTING == TRUE)
    #if (0 < SYS_ADPT_PE_MAX_NBR_OF_RULE_FOR_L3_LITE_ROUTING)
        #define RULE_TYPE_GLOBAL_ACE_L3_LITE_ROUTING_RULE_CNT       1
    #else
        #define RULE_TYPE_GLOBAL_ACE_L3_LITE_ROUTING_RULE_CNT       0
    #endif
#else
    #define RULE_TYPE_GLOBAL_ACE_L3_LITE_ROUTING_RULE_CNT           0
#endif /* SYS_CPNT_ROUTING */

#if (SYS_CPNT_INGRESS_RATE_LIMIT_VIA_FILTER_ENGINE == TRUE)
    #define RULE_TYPE_DFLT_ACE_RATE_LIMIT_RULE_CNT                  1
#else
    #define RULE_TYPE_DFLT_ACE_RATE_LIMIT_RULE_CNT                  0
#endif /* SYS_CPNT_INGRESS_RATE_LIMIT_VIA_FILTER_ENGINE */

#if (SYS_CPNT_VLAN_XLATE == TRUE && SYS_CPNT_VLAN_XLATE_BY_RULE == TRUE)
    #define RULE_TYPE_DFLT_ACE_TRUNK_VLAN_TRANSLATION_RULE_CNT             SYS_ADPT_MAX_NBR_OF_VLAN_XLATE_PER_PORT

    #define RULE_TYPE_DFLT_ACE_VLAN_TRANSLATION_RULE_CNT                   SYS_ADPT_MAX_NBR_OF_VLAN_XLATE_PER_PORT
    #define RULE_TYPE_DFLT_ACE_EGRESS_VLAN_TRANSLATION_RULE_CNT            SYS_ADPT_MAX_NBR_OF_VLAN_XLATE_PER_PORT
#else
    #define RULE_TYPE_DFLT_ACE_TRUNK_VLAN_TRANSLATION_RULE_CNT             0

    #define RULE_TYPE_DFLT_ACE_VLAN_TRANSLATION_RULE_CNT                   0
    #define RULE_TYPE_DFLT_ACE_EGRESS_VLAN_TRANSLATION_RULE_CNT            0
#endif /* SYS_CPNT_VLAN_XLATE */

#if (SYS_CPNT_APP_FILTER_CDP == TRUE)
    #define RULE_TYPE_DFLT_ACE_AF_DENY_CDP_RULE_CNT                 1
#else
    #define RULE_TYPE_DFLT_ACE_AF_DENY_CDP_RULE_CNT                 0
#endif /* SYS_CPNT_APP_FILTER_CDP */

#if (SYS_CPNT_APP_FILTER_PVST == TRUE)
    #define RULE_TYPE_DFLT_ACE_AF_DENY_PVST_RULE_CNT                1
#else
    #define RULE_TYPE_DFLT_ACE_AF_DENY_PVST_RULE_CNT                0
#endif /* SYS_CPNT_APP_FILTER_PVST */

#if (SYS_CPNT_DAI_ARP_ACL == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_DAI_ARP_DROP_RULE_CNT              SYS_ADPT_ARP_INSPECTION_MAX_NBR_OF_DENY_RULE
#else
    #define RULE_TYPE_GLOBAL_ACE_DAI_ARP_DROP_RULE_CNT              0
#endif /* SYS_CPNT_DAI_ARP_ACL */

#if (SYS_CPNT_PBR == TRUE)
    #define RULE_TYPE_GLOBAL_ACE_PBR_RULE_CNT                       SYS_ADPT_MAX_NBR_OF_PBR_RULE
#else
    #define RULE_TYPE_GLOBAL_ACE_PBR_RULE_CNT                       0
#endif /* SYS_CPNT_PBR */

//enum
//{
//#define PACKET_TYPE(pktType, ifType, remapAceType)  (ifType == RULE_TYPE_INTERFACE_ALL_UNIT ? remapAceType##_RULE_CNT : 0) +
//
//    RULE_CONFIG_TOTAL_GLOBAL_RULE_CNT = RULE_CONFIG_PACKET_TYPE_TABLE 0
//
//#undef PACKET_TYPE
//};
//
//enum
//{
//#define PACKET_TYPE(pktType, ifType, remapAceType)  (ifType == RULE_TYPE_INTERFACE_CRAFT_PORT ? remapAceType##_RULE_CNT : 0) +
//
//    RULE_CONFIG_TOTAL_CRAFT_PORT_RULE_CNT = RULE_CONFIG_PACKET_TYPE_TABLE 0
//
//#undef PACKET_TYPE
//};
//
//enum
//{
//#define PACKET_TYPE(pktType, ifType, remapAceType)  (ifType == RULE_TYPE_INTERFACE_UPORT ? remapAceType##_RULE_CNT : 0) +
//
//    RULE_CONFIG_TOTAL_NORMAL_PORT_RULE_CNT = RULE_CONFIG_PACKET_TYPE_TABLE 0
//
//#undef PACKET_TYPE
//};


#endif /* _RULE_CONFIG_H_ */
