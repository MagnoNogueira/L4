/*system*/
#define RUNCFG_DEVELOP 0
/*syb-systems*/
#include "sys_type.h"
#include "sys_cpnt.h"
#include "sys_hwcfg.h"
#include "sys_module.h"
#include "l_mm.h"
#include "cli_type.h"
#include "l_inet.h"
#include "l_cvrt.h"
//#if 0 //rich
//#include "sock_port.h"
//#include "netif_om.h"
//#if (SYS_CPNT_COS == TRUE)
//#include "l4_cos_mgr.h"
//#include "cos_vm.h"
//#endif
//#include "L_MD5.h"
//#if (SYS_CPNT_SMTP == TRUE)
//#include "smtp_mgr.h"
//#endif
//#endif
#if (SYS_CPNT_SMTP == TRUE)
//    #include "smtp_pmgr.h"
#endif

#if (SYS_CPNT_SNTP == TRUE)
    //#include "sntp_pmgr.h"
#endif

#if (SYS_CPNT_NTP == TRUE)
#include "ntp_pmgr.h"
#include "ntp_type.h"
#endif

#include "swctrl.h"
#include "leaf_sys.h"
//#include "stktplg_task.h"
//#include "vlan_mgr.h"
#include "sys_time.h"
#include "leaf_2674p.h"
#include "leaf_2674q.h"
//#include "igv3snp_mgr.h"
//#include "trk_mgr.h"
//#include "fs.h"
#include "cli_api.h"
#include "userauth.h"
//#include "syslog_type.h"
//#include "sys_mgr.h"
#include "sys_dflt.h"
//#include "netcfg_pmgr_rip.h"
#if (SYS_CPNT_OSPF == TRUE)
//#include "ospf_type.h"
//#include "ospf_pmgr.h"
#endif
//#include "igv3snp_macros.h"
#include "l_md5.h"
//#include "dhcp_pmgr.h"
//#include "dhcp_mgr.h"
//#include "tacacs_pom.h"
//#include "netcfg_type.h"
//#include "netcfg_pom_nd.h"
//#include "netcfg_pmgr_nd.h"

//#include "netcfg_pmgr_route.h"
//#include "ip_lib.h"
//#include "vlan_lib.h"

#if (SYS_CPNT_COS == TRUE)
    #if (SYS_CPNT_QOS_V2 != TRUE)
        #include "l4_cos_mgr.h"
    #endif

    #include "cos_vm.h"
#endif
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
#include <net/if.h>/*fuzhimin,20090212*/
#endif
#include "sys_adpt.h"
#include "l_inet.h"
#include <string.h>
//#include "stktplg_mgr.h"
//#include "mib2_mgr.h"
//#include <stdio.h>
//#include "syslog_mgr.h"
//#include "amtr_mgr.h"
#include "leaf_2863.h"
//#include "netcfg.h"
//#include "iml.h"
//#include "pri_mgr.h"
//#include "lacp_mgr.h"
//#include "extbrg_mgr.h"
//#include "smtp_om.h"
//#if (SYS_CPNT_NSM == TRUE)
//#include "nsm_pmgr.h"
//#endif
//#if(SYS_CPNT_GARP==TRUE)
//#include "garp_mgr.h"
//#endif
//
//#include "trap_mgr.h"
//#include "vlan_om_1v.h"
//
//#include "userauth_pmgr.h"
/*cli internal*/
#include "cli_def.h"
#include "cli_lib.h"
#include "cli_runcfg.h"
//#include "cli_api.h"
#include "cli_msg.h"
//#if (SYS_CPNT_STP == SYS_CPNT_STP_TYPE_RSTP || SYS_CPNT_STP == SYS_CPNT_STP_TYPE_MSTP)
//#include "xstp_pmgr.h"
//#include "xstp_pom.h"
//#include "xstp_type.h"
//#endif
//#include "cli_api_vlan.h"

//#if (CLI_SUPPORT_L2_DHCP_RELAY == 1)
//#include "brg_mgr.h"
//#endif

//#include "if_mgr.h"

//#if (SYS_CPNT_VDSL == TRUE)
//#include "vdsl_drv.h"
//#endif

//#if (SYS_CPNT_PORT_SECURITY == TRUE)
//#include "psec_mgr.h"
//#include "psec_pmgr.h"
//#endif
//
//#if (SYS_CPNT_PFU == TRUE)
//#include "pfu_type.h"
//#include "pfu_mgr.h"
//#endif
//
//#if (SYS_CPNT_COMMUNITY_PRIVATE_VLAN == TRUE)
//#include "pvlan_pmgr.h"
//#endif
//
//#if (SYS_CPNT_TACACS == TRUE )
//#include "tacacs_mgr.h"
//#endif
//
//#if (SYS_CPNT_SSHD == TRUE || SYS_CPNT_SSH2 == TRUE)
//#include "sshd_type.h"
//#include "sshd_mgr.h"
//#endif
//
//#if (SYS_CPNT_HTTP == TRUE || SYS_CPNT_HTTPS == TRUE)
//#include "http_pmgr.h"
//#include "http_type.h"
//#endif  /* #if (SYS_CPNT_HTTP == TRUE || SYS_CPNT_HTTPS == TRUE) */
//
//#if ((CLI_SUPPORT_DHCP_CLIENTID == 1) || (CLI_SUPPORT_L3_FEATURE == 1))
//#include "dhcp_mgr.h"
//#endif
//
//#include "dhcp_mgr.h"
//#include "dhcp_pmgr.h"
//#include "dhcp_pom.h"
//#include "dhcp_om.h"
//#if (SYS_CPNT_DHCP_CLIENT_CLASSID == TRUE)
//#include "dhcp_type.h"
//#endif
//
//#if (SYS_CPNT_AAA == TRUE)
//#include "aaa_mgr.h"
//#include "aaa_pom.h"
//#endif
//
//#if (SYS_CPNT_DOT1X == TRUE)
//#include "1x_mgr.h"
//#include "1x_pom.h"
//#include "leaf_ieee8021x.h"
//#endif
//
//#if (SYS_CPNT_RADIUS == TRUE)
//#include "radius_pmgr.h"
//#include "radius_pom.h"
//#endif
//
//#if (SYS_CPNT_TACACS == TRUE )
//#include "tacacs_pmgr.h"
//#include "tacacs_pom.h"
//#endif
//
//#if 0 //rich
//#if (CLI_SUPPORT_L3_FEATURE == 1)
//#include "netcfg_type.h"
//#include "netcfg_mgr.h"
//#else
//#include "netcfg.h"
//#endif
//#endif

//#include "netcfg_type.h"
#if (SYS_CPNT_ACL == TRUE)
#if (SYS_CPNT_QOS_V2 == TRUE)
    #include "l4_pmgr.h"
    #include "cli_api_acl.h"
#else
    #include "acl_type.h"
    #include "l4_acl_mgr.h"
#endif /* #if (SYS_CPNT_QOS_V2 == TRUE) */

    #include "cli_api_acl.h"
#endif /* #if (SYS_CPNT_ACL == TRUE) */

#if ((SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER) && (SYS_CPNT_QOS_V2 != TRUE))
#include "l4_marker_mgr.h"
#endif


//#if (SYS_CPNT_PROTOCOL_VLAN == TRUE)
//#include "vlan_mgr.h"
//#include "leaf_vbridge.h"
//#include "cli_api_1v.h"
//#endif
//
//
//#if (SYS_CPNT_DNS == TRUE)
//#include "dns_mgr.h"
//#include "dns_pmgr.h"
//#include "dns_type.h"
//#endif

#if 0//rich

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
#include "l4_ds_mgr.h"
#include "l4_ds_type.h"
#endif

#endif
//#include "l_base64.h"
//
//#if (SYS_CPNT_MGMT_IP_FLT == TRUE)
//#include "mgmt_ip_flt.h"
//#endif
//
//#if (SYS_CPNT_SNMP_VERSION == 3)
//#include "snmp_mgr.h"
//#include "leaf_3411.h"
//#endif
//
//#if (SYS_CPNT_HSRP == TRUE)
//#include "hsrp_mgr.h"
//#include "hsrp_sys_adpt.h"
//#endif
//
//
//#if (SYS_CPNT_VRRP == TRUE)
//#include "vrrp_mgr.h"
//#include "vrrp_pmgr.h"
//#include "vrrp_type.h"
//#include "vlan_lib.h"
//#endif

//#include "telnet_mgr.h"

//#if (SYS_CPNT_POE == TRUE)
//#include "poe_pom.h"
//#endif
//
//#if (SYS_CPNT_MOTD == TRUE)
//#include "sys_bnr_mgr.h"
//#endif
//
//#if (SYS_CPNT_LACP == TRUE)
//#include "lacp_pom.h"
//#endif
//#if (SYS_CPNT_LLDP == TRUE)
//#include "lldp_mgr.h"
//#include "lldp_pom.h"
//#include "lldp_type.h"
//#include "lldp_pmgr.h"
//#endif
//
//#if(SYS_CPNT_CLUSTER==TRUE)
//#include "cluster_mgr.h"
//#include "cluster_type.h"
//#include "cluster_pom.h"
//#endif
//
//#if (SYS_CPNT_CLI_BANNER == TRUE)
//#include "cli_banner.h"
//#endif
//
//#if (SYS_CPNT_L2MCAST == TRUE)
//#include "igv3snp_pmgr.h"
//#endif
//
//#include "swctrl_pom.h"
//#include "temp.h"
//
///* for OAM */
//#if (SYS_CPNT_EFM_OAM == TRUE)
//#include "oam_mgr.h"
//#include "oam_pom.h"
//#include "oam_type.h"
//#include "leaf_4878.h"
//#endif
//
///*for DHCPSNP*/
//#if (SYS_CPNT_DHCPSNP == TRUE)
//#include "dhcpsnp_mgr.h"
//#include "dhcpsnp_pmgr.h"
//#include "dhcpsnp_type.h"
//#endif
//
///*for DHCP relay information option 82 */
//#if (SYS_CPNT_DHCP_RELAY_OPTION82 == TRUE)
//#include "dhcp_pom.h"
//#endif
//
///*for DHCPv6 */
//#if (SYS_CPNT_DHCPV6 == TRUE)
//#include "dhcpv6_pmgr.h"
//#include "cfg.h"
//#endif
//
///*for DHCPv6 Relay*/
//#if (SYS_CPNT_DHCPV6_RELAY == TRUE)
//#include "dhcpv6_pom.h"
//#endif
//
///*for MLDSNP*/
//#if(SYS_CPNT_MLDSNP==TRUE)
//#include "mldsnp_pom.h"
//#include "mldsnp_type.h"
//#endif
//
///*Net Access */
//#if(SYS_CPNT_NETACCESS == TRUE)
//#include "netaccess_pmgr.h"
//#include "netaccess_type.h"
//#endif  /* #if(SYS_CPNT_NETACCESS == TRUE) */
//
///*for cwmp*/
//#if (SYS_CPNT_CWMP == TRUE)
//#include "cwmp_type.h"
//#include "cwmp_pom.h"
//#endif
//
//#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
//#include "rule_type.h"
//#endif
//
//#if(SYS_CPNT_OSPF == TRUE)
//#include "netcfg_mgr_ospf.h"
//#include "netcfg_pmgr_ospf.h"
//#include "ospf_const.h"
//#endif
///* cfm */
//#if(SYS_CPNT_CFM == TRUE)
//#include "cfm_pmgr.h"
//#endif
//
//#if (SYS_CPNT_PPPOE_IA == TRUE)
//#include "pppoe_ia_pmgr.h"
//#endif /* #if (SYS_CPNT_PPPOE_IA == TRUE) */
//
//#if (SYS_CPNT_ADD == TRUE)
//#include "add_pmgr.h"
//#endif
//#if (SYS_CPNT_UDP_HELPER == TRUE)
//#include "udphelper_type.h"
//#include "udphelper_pmgr.h"
//#endif
//#if defined(SYS_CPNT_IGMP) && (SYS_CPNT_IGMP == TRUE)
//#include "netcfg_om_igmp.h"
//#include "netcfg_pom_igmp.h"
//#endif
//#if defined(SYS_CPNT_MLD) && (SYS_CPNT_MLD == TRUE)
//#include "netcfg_om_mld.h"
//#include "netcfg_pom_mld.h"
//#endif
//
//#include "netcfg_pom_ip.h"
//#include "netcfg_pmgr_ip.h"
//
//#if defined(SYS_CPNT_PIM) && (SYS_CPNT_PIM == TRUE)
//#include "pim_type.h"
//#include "pim_pmgr.h"
//#include "pim_pom.h"
//#endif
//#if (SYS_CPNT_PIM6 == TRUE)
//#include "pim6_type.h"
//#include "pim6_pmgr.h"
//#include "pim6_pom.h"
//#endif
//#if (SYS_CPNT_DAI == TRUE)
//#include "dai_pmgr.h"
//#endif/*#if (SYS_CPNT_DAI == TRUE)*/
//
//#if(SYS_CPNT_MAC_VLAN == TRUE)
//#include "cli_api_mac_vlan.h"
//#endif
//#if (SYS_CPNT_IP_SUBNET_VLAN == TRUE)
//#include "cli_api_subnet_vlan.h"
//#endif
//
//#if (SYS_CPNT_SYSMGMT == TRUE)
//#include "sys_pmgr.h"
//#endif
//
//#if (SYS_CPNT_OSPF6 == TRUE)
//#include "ospf6_type.h"
//#include "ospf6_pmgr.h"
//#endif
//
//#if (SYS_CPNT_RSPAN == TRUE)
//#include "leaf_es3626a.h"
//#include "rspan_om.h"
//#include "rspan_pmgr.h"
//#include "cli_api_rspan.h"
//#endif
//
//#if (SYS_CPNT_WEBAUTH == TRUE)
//#include "webauth_pmgr.h"
//#include "webauth_type.h"
//#endif /* #if(SYS_CPNT_WEBAUTH == TRUE) */
//
//#if (SYS_CPNT_XFER_AUTO_UPGRADE == TRUE)
//#include "xfer_pmgr.h"
//#endif
//
//#if (SYS_CPNT_ERPS == TRUE)
//#include "erps_pmgr.h"
//#endif /* #if (SYS_CPNT_ERPS == TRUE) */
//#if (SYS_CPNT_MVR6 == TRUE)
//#include "mvr6_pmgr.h"
//#endif
//
//#if (SYS_CPNT_DOS == TRUE)
//#include "dos_pmgr.h"
//#endif
//
//#if (SYS_CPNT_LBD == TRUE)
//#include "lbd_om.h"
//#include "lbd_type.h"
//#endif
//
//#if (SYS_CPNT_DHCPV6SNP == TRUE)
//#include "dhcpv6snp_pom.h"
//#include "dhcpv6snp_pmgr.h"
//#endif
//
//#if (SYS_CPNT_BGP == TRUE)
//#include "bgp_type.h"
//#include "bgp_pmgr.h"
//#include "bgp_pom.h"
//#endif
//
//#if (SYS_CPNT_NSM_POLICY == TRUE)
//#include "nsm_policy_type.h"
//#include "nsm_policy_pmgr.h"
//#endif
//
//#if (SYS_CPNT_UDLD == TRUE)
//#include "udld_pmgr.h"
//#endif /* #if (SYS_CPNT_UDLD == TRUE) */
//
//#if (SYS_CPNT_PTP == TRUE)
//#include "ptp_pmgr.h"
//#endif
//#if (SYS_CPNT_ELPS == TRUE)
//#include "elps_pmgr.h"
//#endif
//#include "leaf_3019.h"
//
//#if (SYS_CPNT_NDSNP == TRUE)
//#include "ndsnp_pom.h"
//#endif
//
//#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
//#include "ip6sg_pom.h"
//#endif
//
//#if (SYS_CPNT_SYNCE==TRUE)
//#include "sync_e_pmgr.h"
//#include "syncedrv_type.h"
//#include "l_pbmp.h"
//#endif
//
//#if (SYS_CPNT_PFC == TRUE)
//#include "pfc_pmgr.h"
//#endif
//#if (SYS_CPNT_ETS==TRUE)
//#include "ets_pmgr.h"
//#endif
//#if (SYS_CPNT_DCBX == TRUE)
//#include "dcbx_pmgr.h"
//#endif
//
//#if (SYS_CPNT_CN == TRUE)
//#include "cn_pom.h"
//#include "cn_type.h"
//#endif
//
//#if (SYS_CPNT_APP_FILTER == TRUE)
//#include "af_type.h"
//#include "af_pmgr.h"
//#endif /* #if (SYS_CPNT_APP_FILTER == TRUE) */

#ifndef ASSERT
#define ASSERT(e)
#endif

#define EXPAND_MAC(mac) ((UI8_T*)mac)[0],((UI8_T*)mac)[1],((UI8_T*)mac)[2],((UI8_T*)mac)[3],((UI8_T*)mac)[4],((UI8_T*)mac)[5]
#define EXPAND_IP(ip)  ((UI8_T *)&(ip))[0],((UI8_T *)&(ip))[1],((UI8_T *)&(ip))[2],((UI8_T *)&(ip))[3]
#define EXPAND_IPV4(ip) ((UI8_T*)(ip))[0],((UI8_T*)(ip))[1],((UI8_T*)(ip))[2],((UI8_T*)(ip))[3]
#define EXPAND_IPV6(ip) ((UI32_T*)(ip))[0],((UI32_T*)(ip))[1],((UI32_T*)(ip))[2],((UI32_T*)(ip))[3],((UI32_T*)(ip))[4]


typedef struct
{
    UI32_T value;   /*the value of this variable*/
    UI32_T type;    /*which type of variable  maxage = 1 , hello = 2 , forward = 3*/
    BOOL_T valid;   /*is valid to use*/
}CLI_RUNCFG_Sequence_T;


typedef struct
{
    UI32_T starting_port_ifindex;
    UI32_T number_of_port;
}CLI_RUNCFG_ModuleInfo_T;


typedef UI32_T (* Func_T)    (UI32_T unit_id, char *buffer, char *temp_buffer, UI32_T buffer_size, UI32_T b_count);


/*macros*/
#define APPEND_RUNNING_CFG(FORMAT, ARGS...) do{                                 \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
		SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT, ##ARGS);  \
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }while(0)

#define APPEND_RUNNING_CFG_0(FORMAT) {                                          \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
		SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT);      \
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }

#define APPEND_RUNNING_CFG_1(FORMAT, ARG1) {                                    \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
		SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT, ARG1);\
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }

#define APPEND_RUNNING_CFG_2(FORMAT, ARG1, ARG2) {                              \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
        SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT, ARG1, ARG2);  \
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }

#define APPEND_RUNNING_CFG_3(FORMAT, ARG1, ARG2, ARG3) {                        \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
        SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT, ARG1, ARG2, ARG3);       \
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }

#define APPEND_RUNNING_CFG_4(FORMAT, ARG1, ARG2, ARG3, ARG4) {                  \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
        SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT, ARG1, ARG2, ARG3, ARG4);     \
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }

#define APPEND_RUNNING_CFG_5(FORMAT, ARG1, ARG2, ARG3, ARG4, ARG5) {            \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
        SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT, ARG1, ARG2, ARG3, ARG4, ARG5);     \
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }

#define APPEND_RUNNING_CFG_6(FORMAT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) {      \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
        SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);     \
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }



#define APPEND_RUNNING_CFG_7(FORMAT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) { \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
        SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);     \
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }

#define APPEND_RUNNING_CFG_8(FORMAT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8) { \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
        SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);     \
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }
#define APPEND_RUNNING_CFG_9(FORMAT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9) { \
        memset(temp_buffer, 0, sizeof(temp_buffer));                            \
        SYSFUN_Snprintf((char *)temp_buffer, CLI_DEF_MAX_BUFSIZE, FORMAT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9);     \
        if(buffer_count + strlen((char *)temp_buffer) >= buffer_size)           \
        {                                                                       \
            return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                          \
        }                                                                       \
        else                                                                    \
            buffer_count += strlen((char *)temp_buffer);                        \
        memcpy(&(buffer[buffer_count-strlen((char *)temp_buffer)]), temp_buffer , strlen((char *)temp_buffer)+1);   \
    }

#define COLLECT_ENCRTPT_TEXT_TO_PRINT(ENCRY, PLAIN)                             \
    SYSFUN_Sprintf(ENCRY, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",   \
    PLAIN[0], PLAIN[1], PLAIN[2],  PLAIN[3],  PLAIN[4],  PLAIN[5],  PLAIN[6],  PLAIN[7],        \
    PLAIN[8], PLAIN[9], PLAIN[10], PLAIN[11], PLAIN[12], PLAIN[13], PLAIN[14], PLAIN[15]);

#define COLLECT_ENCRTPT_TEXT_TO_PRINT_LENGTH(ENCRY, PLAIN, BYTE_LEN)          { \
        UI32_T i;                                                               \
        char buff[10] = {0};                                                    \
        for(i=0;i<BYTE_LEN;i++)                                                 \
        {                                                                       \
            SYSFUN_Sprintf(buff, "%02x", PLAIN[i]);                             \
            strcat(ENCRY,buff);                                                 \
        }                                                                       \
    }

#define CHECK_BUFF_RETURN_VALUE(buffer_count)                                   \
    if (buffer_count == CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY)                     \
        return CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY;                              \



static UI32_T
CLI_RUNCFG_Get_ACL_Parameter(
    UI32_T unit_id,
    char *buffer,
    char *temp_buffer,
    UI32_T buffer_size,
    UI32_T b_count)
{
    /*show running*/
    UI32_T buffer_count = b_count;

    UI32_T  i;
    UI32_T  acl_index;
    UI32_T  ace_index;

    RULE_TYPE_AclType_T acl_type_ary[] =
    {
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IP_EXT_ACL,
#if (SYS_CPNT_ACL_IPV6 == TRUE)
        RULE_TYPE_IPV6_STD_ACL,
        RULE_TYPE_IPV6_EXT_ACL,
#endif /* #if (SYS_CPNT_ACL_IPV6 == TRUE) */
        RULE_TYPE_MAC_ACL,
#if (SYS_CPNT_DAI == TRUE)
        RULE_TYPE_ARP_ACL
#endif /* #if (SYS_CPNT_DAI == TRUE) */
    };

    RULE_TYPE_UI_AclEntry_T     acl_entry;
    RULE_TYPE_UI_Ace_Entry_T    ui_ace;

#define CLI_RUNCFG_ACL_PRIV_STRCAT(dest, source) \
    strncat(dest, source, sizeof(dest) - strlen(dest) - 1)

    for (i = 0; i < (sizeof(acl_type_ary)/sizeof(acl_type_ary[0])); i++)
    {
        memset(&acl_entry, 0, sizeof(acl_entry));
        acl_index = 0;
        acl_entry.acl_type = acl_type_ary [i];

        while (L4_PMGR_ACL_GetNextRunningAcl( &acl_index, &acl_entry) == SYS_TYPE_GET_RUNNING_CFG_SUCCESS)
        {
            /* ACL name
             */
            {
                char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + 1];
#if (CLI_SUPPORT_QUOTE_STRING_FEATURE == TRUE)
                CLI_LIB_Str_Add_Quote ( (char *)acl_entry.acl_name, acl_name);
#else
                SYSFUN_Snprintf(acl_name, sizeof(acl_name), "%s", acl_entry.acl_name);
#endif

                switch ( acl_entry.acl_type )
                {
                    case RULE_TYPE_IP_STD_ACL:
                        APPEND_RUNNING_CFG_0("access-list IP standard");
                        break;

                    case RULE_TYPE_IP_EXT_ACL:
                        APPEND_RUNNING_CFG_0("access-list IP extended");
                        break;

                    case RULE_TYPE_MAC_ACL:
                        APPEND_RUNNING_CFG_0("access-list MAC");
                        break;

#if (SYS_CPNT_ACL_IPV6 == TRUE)
                    case RULE_TYPE_IPV6_EXT_ACL:
                        APPEND_RUNNING_CFG_0("access-list ipv6 extended");
                        break;

                    case RULE_TYPE_IPV6_STD_ACL:
                        APPEND_RUNNING_CFG_0("access-list ipv6 standard");
                        break;
#endif /* #if (SYS_CPNT_ACL_IPV6 == TRUE) */

#if (SYS_CPNT_DAI == TRUE)
                    case RULE_TYPE_ARP_ACL:
                        APPEND_RUNNING_CFG_0("access-list arp");
                        break;
#endif/*#if (SYS_CPNT_DAI == TRUE)*/

                    default:
                        continue;
                        break;
                }

                APPEND_RUNNING_CFG_1(" %s\n", acl_name);
            }

            ace_index = 0;

            while(SYS_TYPE_GET_RUNNING_CFG_SUCCESS == L4_PMGR_ACL_GetNextRunningUIAceByAcl(acl_index,
                                                                                           &ace_index,
                                                                                           &ui_ace))
            {
                char  local_format_str[L_INET_MAX_IPADDR_STR_LEN+1] = {0}; /* cache for sprintf */
                char  local_buf[CLI_DEF_MAX_BUFSIZE] = {0}; /* cache per ace string */
                UI32_T ace_action;

                ace_action = ui_ace.access;
                switch (ace_action)
                {
                    case RULE_TYPE_ACE_PERMIT:
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " permit");
                        break;

                    case RULE_TYPE_ACE_DENY:
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " deny");
                        break;

                    default:
                        break;
                }

                switch ( acl_entry.acl_type )
                {
                    case RULE_TYPE_IP_STD_ACL:
                        CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.ipv4.sip.addr,
                                                         ui_ace.ipv4.sip.mask,
                                                         local_format_str);
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                        break;

                    case RULE_TYPE_IP_EXT_ACL:
                        if (ui_ace.ipv4.protocol.op == RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL)
                        {
                            switch (ui_ace.ipv4.protocol.u.s.data)
                            {
                                case RULE_TYPE_ACL_TCP_PROTOCOL:
                                    CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " TCP");
                                    break;
                                case RULE_TYPE_ACL_UDP_PROTOCOL:
                                    CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " UDP");
                                    break;
#if (SYS_CPNT_ACL_IP_EXT_ICMP == TRUE)
                                case RULE_TYPE_ACL_ICMP_PROTOCOL:
                                    CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " ICMP");
                                    break;
#endif
                                default:
                                    SYSFUN_Sprintf(local_format_str," %u",
                                                   ui_ace.ipv4.protocol.u.s.data);
                                    CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                    break;
                            }
                        }

                        CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.ipv4.sip.addr,
                                                         ui_ace.ipv4.sip.mask,
                                                         local_format_str);
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                        CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.ipv4.dip.addr,
                                                         ui_ace.ipv4.dip.mask,
                                                         local_format_str);
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                        switch (ui_ace.ipv4.dscp.op)
                        {
                            case RULE_TYPE_IPV4_DSCP_OP_DSCP:
                                SYSFUN_Sprintf(local_format_str, " DSCP %u",
                                               ui_ace.ipv4.dscp.u.ds);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                break;

                            case RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE:
                                SYSFUN_Sprintf(local_format_str," precedence %u",
                                               ui_ace.ipv4.dscp.u.tos.precedence);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                break;

                            case RULE_TYPE_IPV4_DSCP_OP_TOS:
                                SYSFUN_Sprintf(local_format_str," tos %u",
                                               ui_ace.ipv4.dscp.u.tos.tos);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                break;

                            case RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE_AND_TOS:
                                SYSFUN_Sprintf(local_format_str," precedence %u tos %u",
                                               ui_ace.ipv4.dscp.u.tos.precedence, ui_ace.ipv4.dscp.u.tos.tos);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                break;

                            case RULE_TYPE_IPV4_DSCP_OP_NO_OPERATION:
                                break;

                            default:
                                ASSERT(0);
                                break;
                        }

                        if (ui_ace.l4_common.sport.op == VAL_diffServIpAceSourcePortOp_equal)
                        {
                            SYSFUN_Sprintf(local_format_str, " source-port %lu",
                                           ui_ace.l4_common.sport.u.s.data);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                            if (ui_ace.l4_common.sport.u.s.mask != MAX_diffServIpAceSourcePortBitmask)
                            {
                                SYSFUN_Sprintf(local_format_str, " %lu",
                                               ui_ace.l4_common.sport.u.s.mask);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                            }
                        }

                        if (ui_ace.l4_common.dport.op == VAL_diffServIpAceDestPortOp_equal)
                        {
                            SYSFUN_Sprintf(local_format_str, " destination-port %lu",
                                           ui_ace.l4_common.dport.u.s.data);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                            if (ui_ace.l4_common.dport.u.s.mask != MAX_diffServIpAceDestPortBitmask)
                            {
                                SYSFUN_Sprintf(local_format_str, " %lu",
                                               ui_ace.l4_common.dport.u.s.mask);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                            }
                        }

#if (SYS_CPNT_ACL_IP_EXT_ICMP == TRUE)
                        if (ui_ace.icmp.icmp_type.op == RULE_TYPE_ICMP_ICMPTYPE_OP_EQUAL)
                        {
                            SYSFUN_Sprintf(local_format_str, " icmp-type %u",
                                           ui_ace.icmp.icmp_type.u.s.data);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                        }
#endif /* SYS_CPNT_ACL_IP_EXT_ICMP */

                        if (ui_ace.tcp.flags.mask.u.code != MIN_diffServIpAceControlCodeBitmask)
                        {
                            SYSFUN_Sprintf(local_format_str, " control-flag %u",
                                           ui_ace.tcp.flags.data.u.code);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                            if (ui_ace.tcp.flags.mask.u.code != MAX_diffServIpAceControlCodeBitmask)
                            {
                                SYSFUN_Sprintf(local_format_str," %u",
                                               ui_ace.tcp.flags.mask.u.code);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                            }
                        }
                        break;

                    case RULE_TYPE_MAC_ACL:
                        switch (ui_ace.pkt_format)
                        {
                            case VAL_diffServMacAcePktformat_untagged_Eth2:
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf," untagged-eth2");
                                break;
                            case VAL_diffServMacAcePktformat_untagged802Dot3:
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf," untagged-802.3");
                                break;
                            case VAL_diffServMacAcePktformat_tagggedEth2:
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf," tagged-eth2");
                                break;
                            case VAL_diffServMacAcePktformat_tagged802Dot3:
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf," tagged-802.3");
                                break;

                            case VAL_diffServMacAcePktformat_any:
                                break;

                            default:
                                ASSERT(0);
                                break;
                        }

                        memset(local_format_str, 0, sizeof(local_format_str));
                        CLI_ACL_Check_AnyMAC(ui_ace.ether.sa.addr,
                                             ui_ace.ether.sa.mask,
                                             local_format_str);
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                        memset(local_format_str, 0, sizeof(local_format_str));
                        CLI_ACL_Check_AnyMAC(ui_ace.ether.da.addr,
                                             ui_ace.ether.da.mask,
                                             local_format_str);
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                        if (ui_ace.ether.vid.op == VAL_diffServMacAceVidOp_equal)
                        {
                            SYSFUN_Sprintf(local_format_str, " VID %lu",
                                           ui_ace.ether.vid.u.s.data);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                            if (ui_ace.ether.vid.u.s.mask != MAX_diffServMacAceVidBitmask)
                            {
                                SYSFUN_Sprintf(local_format_str, " %lu",
                                               ui_ace.ether.vid.u.s.mask);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                            }
                        }

                        /* ethertype | ip | ipv6
                         */
                        if (ui_ace.ether.ethertype.op == VAL_diffServMacAceEtherTypeOp_equal)
                        {
                            if (ui_ace.ether.ethertype.u.s.mask == MAX_diffServMacAceEtherTypeBitmask)
                            {
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
                                RULE_TYPE_UI_Ace_Entry_T default_ui_ace;

                                L4_PMGR_ACL_InitUIAce(&default_ui_ace);

                                if(0 != memcmp(&ui_ace.ipv4, &default_ui_ace.ipv4, sizeof(ui_ace.ipv4)) ||
                                   0 != memcmp(&ui_ace.ipv6, &default_ui_ace.ipv6, sizeof(ui_ace.ipv6)) ||
                                   0 != memcmp(&ui_ace.l4_common, &default_ui_ace.l4_common, sizeof(ui_ace.l4_common)))
                                {
                                    switch (ui_ace.ether.ethertype.u.s.data)
                                    {
                                        case RULE_TYPE_ETHERTYPE_IP:
                                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " ip");

                                            CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.ipv4.sip.addr,
                                                                             ui_ace.ipv4.sip.mask,
                                                                             local_format_str);
                                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                                            CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.ipv4.dip.addr,
                                                                             ui_ace.ipv4.dip.mask,
                                                                             local_format_str);
                                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                                            if (ui_ace.ipv4.protocol.op == RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL)
                                            {
                                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " protocol");
                                                SYSFUN_Sprintf(local_format_str," %u", ui_ace.ipv4.protocol.u.s.data);
                                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                            }
                                            break;

                                        case RULE_TYPE_ETHERTYPE_IPV6:
                                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " ipv6");

                                            CLI_ACL_Check_AnyIPV6(local_format_str,
                                                                  ui_ace.ipv6.sip.addr,
                                                                  ui_ace.ipv6.sip.prefix_len);
                                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                                            CLI_ACL_Check_AnyIPV6(local_format_str,
                                                                  ui_ace.ipv6.dip.addr,
                                                                  ui_ace.ipv6.dip.prefix_len);
                                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                                            memset(local_format_str, 0, sizeof(local_format_str));
                                            if (ui_ace.ipv6.next_header.op == RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL)
                                            {
                                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " protocol");
                                                SYSFUN_Sprintf(local_format_str," %u",
                                                               ui_ace.ipv6.next_header.u.s.data);
                                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                            }
                                            break;

                                        default:
                                            ASSERT(0);
                                            break;
                                    }

                                    if (ui_ace.l4_common.sport.op == VAL_diffServIpAceSourcePortOp_equal)
                                    {
                                        SYSFUN_Sprintf(local_format_str, " l4-source-port %lu",
                                                       ui_ace.l4_common.sport.u.s.data);
                                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                                        if (ui_ace.l4_common.sport.u.s.mask != MAX_diffServIpAceSourcePortBitmask )
                                        {
                                            sprintf(local_format_str," %lu",
                                                    ui_ace.l4_common.sport.u.s.mask);
                                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                        }
                                    }

                                    if (ui_ace.l4_common.dport.op == VAL_diffServIpAceDestPortOp_equal)
                                    {
                                        SYSFUN_Sprintf(local_format_str, " l4-destination-port %lu",
                                                       ui_ace.l4_common.dport.u.s.data);
                                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                                        if (ui_ace.l4_common.dport.u.s.mask != MAX_diffServIpAceDestPortBitmask )
                                        {
                                            sprintf(local_format_str," %lu",
                                                    ui_ace.l4_common.dport.u.s.mask);
                                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                        }
                                    }
                                }
                                else
#endif /* SYS_CPNT_ACL_MAC_IP_MIX */
                                {
                                    SYSFUN_Sprintf(local_format_str," ethertype %04lx",
                                                   ui_ace.ether.ethertype.u.s.data);
                                    CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                }
                            }
                            else
                            {
                                SYSFUN_Sprintf(local_format_str," ethertype %04lx %04lx",
                                               ui_ace.ether.ethertype.u.s.data,
                                               ui_ace.ether.ethertype.u.s.mask);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                            }
                        }
                        if (ui_ace.ether.cos.op == VAL_diffServMacAceCosOp_equal)
                        {
                            memset(local_format_str, 0, sizeof(local_format_str));
                            SYSFUN_Sprintf(local_format_str," cos %u %u",ui_ace.ether.cos.u.s.data, ui_ace.ether.cos.u.s.mask);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                        }

                        break;

#if (SYS_CPNT_ACL_IPV6 == TRUE)
                    case RULE_TYPE_IPV6_STD_ACL:
                        memset(local_format_str, 0, sizeof(local_format_str));
                        CLI_ACL_Check_AnyIPV6(local_format_str,
                                              ui_ace.ipv6.sip.addr,
                                              ui_ace.ipv6.sip.prefix_len);
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                        break;

                    case RULE_TYPE_IPV6_EXT_ACL:
                        /*next-header*/
#if (SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER_NEW_FORMAT == TRUE)
                        if (ui_ace.ipv6.next_header.op == RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL)
                        {
                            switch (ui_ace.ipv6.next_header.u.s.data)
                            {
                                case RULE_TYPE_ACL_IPV6_TCP_NEXT_HEADER:
                                    CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " TCP");
                                    break;
                                case RULE_TYPE_ACL_IPV6_UDP_NEXT_HEADER:
                                    CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " UDP");
                                    break;
#if (SYS_CPNT_ACL_IPV6_EXT_ICMP == TRUE)
                                case RULE_TYPE_ACL_IPV6_ICMP_NEXT_HEADER:
                                    CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, " ICMP");
                                    break;
#endif /* SYS_CPNT_ACL_IPV6_EXT_ICMP */
                                default:
                                    SYSFUN_Sprintf(local_format_str," %u",
                                                   ui_ace.ipv6.next_header.u.s.data);
                                    CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                                    break;
                            }
                        }
#endif /* SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER_NEW_FORMAT */

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                        memset(local_format_str, 0, sizeof(local_format_str));
                        CLI_ACL_Check_AnyIPV6(local_format_str,
                                              ui_ace.ipv6.sip.addr,
                                              ui_ace.ipv6.sip.prefix_len);
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
#endif /* SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR */

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                        memset(local_format_str, 0, sizeof(local_format_str));
                        CLI_ACL_Check_AnyIPV6(local_format_str,
                                              ui_ace.ipv6.dip.addr,
                                              ui_ace.ipv6.dip.prefix_len);
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
#endif /* SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR */

#if (SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER == TRUE)
                        if (ui_ace.ipv6.next_header.op == RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL)
                        {
                            SYSFUN_Sprintf(local_format_str," next-header %u",
                                            ui_ace.ipv6.next_header.u.s.data);
                            CLI_RUNCFG_ACL_PRIV_STRCAT (local_buf, local_format_str);
                        }
#endif /* SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER */

                        if (ui_ace.ipv6.traffic_class.op == RULE_TYPE_IPV6_TRAFFIC_CLASS_OP_EQUAL)
                        {
                            /* traffic class is DS(6 bits) + ignore(2 bits)
                             */
                            SYSFUN_Sprintf(local_format_str," DSCP %u",
                                            ui_ace.ipv6.traffic_class.u.s.data >> 2);
                            CLI_RUNCFG_ACL_PRIV_STRCAT (local_buf, local_format_str);
                        }

                        if (ui_ace.ipv6.flow_label.op == RULE_TYPE_IPV6_FLOW_LABEL_OP_EQUAL)
                        {
                            SYSFUN_Sprintf(local_format_str," flow-label %lu",
                                           ui_ace.ipv6.flow_label.u.s.data);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                        }

#if (SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT == TRUE)
                        if (ui_ace.l4_common.sport.op == VAL_diffServIpAceSourcePortOp_equal)
                        {
                            SYSFUN_Sprintf(local_format_str, " source-port %lu",
                                           ui_ace.l4_common.sport.u.s.data);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                            if (ui_ace.l4_common.sport.u.s.mask != MAX_diffServIpAceSourcePortBitmask)
                            {
                                SYSFUN_Sprintf(local_format_str, " %lu",
                                               ui_ace.l4_common.sport.u.s.mask);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                            }
                        }

                        if (ui_ace.l4_common.dport.op == VAL_diffServIpAceDestPortOp_equal)
                        {
                            SYSFUN_Sprintf(local_format_str, " destination-port %lu",
                                           ui_ace.l4_common.dport.u.s.data);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                            if (ui_ace.l4_common.dport.u.s.mask != MAX_diffServIpAceDestPortBitmask)
                            {
                                SYSFUN_Sprintf(local_format_str, " %lu",
                                               ui_ace.l4_common.dport.u.s.mask);
                                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                            }
                        }
#endif /* SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT */

#if (SYS_CPNT_ACL_IPV6_EXT_ICMP == TRUE)
                        if (ui_ace.icmp.icmp_type.op == RULE_TYPE_ICMP_ICMPTYPE_OP_EQUAL)
                        {
                            SYSFUN_Sprintf(local_format_str, " icmp-type %u",
                                           ui_ace.icmp.icmp_type.u.s.data);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                        }
#endif /* SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT */

                        break;
#endif /* #if (SYS_CPNT_ACL_IPV6 == TRUE) */

#if (SYS_CPNT_DAI == TRUE)
                    case RULE_TYPE_ARP_ACL:
                        switch (ui_ace.arp.arp_type)
                        {
                            case RULE_TYPE_ARP_REQUEST:
                                CLI_RUNCFG_ACL_PRIV_STRCAT (local_buf," request");
                                break;
                            case RULE_TYPE_ARP_REPLY:
                                CLI_RUNCFG_ACL_PRIV_STRCAT (local_buf," response");
                                break;
                            default:
                                break;
                        }

                        CLI_RUNCFG_ACL_PRIV_STRCAT (local_buf," ip");

                        CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.arp.sender.ip.addr,
                                                         ui_ace.arp.sender.ip.mask,
                                                         local_format_str);
                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);

                        if (ui_ace.arp.arp_type == RULE_TYPE_ARP_REPLY)
                        {
                            CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.arp.target.ip.addr,
                                                             ui_ace.arp.target.ip.mask,
                                                             local_format_str);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                        }

                        CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf," mac");

                        memset (local_format_str, 0, sizeof(local_format_str));
                        CLI_ACL_Check_AnyMAC (ui_ace.arp.sender.mac.addr,
                                              ui_ace.arp.sender.mac.mask,
                                              local_format_str);
                        CLI_RUNCFG_ACL_PRIV_STRCAT (local_buf, local_format_str);

                        if (ui_ace.arp.arp_type == RULE_TYPE_ARP_REPLY)
                        {
                            memset (local_format_str, 0, sizeof(local_format_str));
                            CLI_ACL_Check_AnyMAC(ui_ace.arp.target.mac.addr,
                                                 ui_ace.arp.target.mac.mask,
                                                 local_format_str);
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, local_format_str);
                        }

                        if (ui_ace.arp.is_log == TRUE)
                        {
                            CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf," log");
                        }

                        break;
#endif/*#if (SYS_CPNT_DAI == TRUE)*/
                    default:
                        break;
                }

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                if (RULE_TYPE_UNDEF_TIME_RANGE != ui_ace.time_range_index)
                {
                    enum
                    {
                        extra_time_range_string_size = sizeof("time-range ") + 1
                    };

                    char time_range_string[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+extra_time_range_string_size] = {0};

                    SYSFUN_Sprintf(time_range_string, " time-range %s", ui_ace.time_range_name);
                    CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, time_range_string);
                }
#endif /* #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */
                CLI_RUNCFG_ACL_PRIV_STRCAT(local_buf, "\n");
                APPEND_RUNNING_CFG_0(local_buf);
            }/* ace */
            APPEND_RUNNING_CFG_0("!\n");
        }/* acl */
    }/* for loop of acl type */

#undef CLI_RUNCFG_ACL_PRIV_STRCAT
    return buffer_count;
}
