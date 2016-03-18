#ifndef  SYS_DFLT_H
#define  SYS_DFLT_H


/*----------------------------------------------------------------------------
 * Package: SYS_DFLT.H
 * Purpose: This package defines the system default configuration values for the system.
 *          The system will be configurued to system default operation state when
 *          1. The system is first shipping to the customer
 *          2. The system is "factory reset command" is executed
 *          3. the system can not retrieve the last system configuration (lost/corrupted)
 * Note: 1. The naming defined for system default configuration value in this package shall
 *          be reused by all the BNBU L2/L3 switch projects.
 *       2. The system default configuration value will be OEM dependent whcih may need to be
 *          customized based on OEM customer requirements.
 *  History
 *
 *   Anderson   07/16/2001      new created
 *   Jason Hsue 10/31/2002      2nd Edition
 *      Add 802.1x default constants (suggested by JJ Young & Kevin Cheng)
 *   Jason Hsue 02/11/2003      3rd Edition
 *      Add SYS_DFLT_ENABLE_PASSWORD_USERNAME for radius
 *   Jason Hsue 02/14/2003
 *      Add SYS_DFLT_COMBO_PORT_FORCED_MODE for supporting combo port forced mode
 *   Zhong QY   12-08/2003
 *      Synchronisation from Hagrid (ES4649-32)
 *   Wuli/Willy    2004-07-13
 *      Add defaults HSRP, VRRP
 *   Zhong QY       04-30/2007
 *      Moved to be based on ACPv3/main/ES4649.
 *
 * ------------------------------------------------------------------------
 * Copyright(C)							  	Accton Technology Corp. , 2007
 * ------------------------------------------------------------------------
 */

#include "sys_adpt.h"
#include "leaf_1213.h"
#include "leaf_1493.h"
#include "leaf_2011.h"
#include "leaf_2618.h"
#include "leaf_2674p.h"
#include "leaf_2674q.h"
#include "leaf_2863.h"
#include "leaf_2933.h"
#include "leaf_es3626a.h"
#include "leaf_ubridge.h"
#include "leaf_ieee8021x.h"
#include "leaf_1850.h"
#include "leaf_2932.h"
#include "leaf_4293.h"


/* NAMING CONSTANT DECLARATIONS
 */

/* The maximum number of member threads for a thread group.
 */
#define SYS_DFLT_NUM_OF_THREADS_IN_THREADGROUP 32


/* Define default configuration value for those MIB-II read-write objects in RFC1213 MIB.
 */
#define SYS_DFLT_SYS_NAME                       ""
#define SYS_DFLT_SYS_CONTACT                    ""
#define SYS_DFLT_SYS_LOCATION                   ""

#define SYS_DFLT_SNMP_ENABLE_AUTHEN_TRAPS       VAL_snmpEnableAuthenTraps_enabled

/* define the SNMPV3 Agent default value */
#define SYS_DFLT_SNMP_AGENT_STATUS              TRUE

/* This defines the default value of snmpTargetAddrTimeout
 * in RFC3413 in the implementation. The unit is 0.01 s.
 * For a default project, the RFC MIB default is 1500 (15 s).
 */
#define SYS_DFLT_SNMP_TARGET_ADDR_TIMEOUT       1500  /* 15 s */

/* define SNMP trap default values */
#define  SYS_DFLT_TRAP_SNMP_VERSION             VAL_trapDestVersion_version1
#define  SYS_DFLT_TRAP_UDP_PORT                 162

/* Define number of buckets for RMON probe (RFC2819).
 * These contants are for RMON's history group.
 * The RMON probe creates two default control-table entries
 * for each port according the interval (in seconds).
 * Unless there are customer preferences, according to the RFC,
 * interval 1 is 30 (30 s), and interval 2 is 1800 (30 min).
 */
#define SYS_DFLT_RMON_HISTORY_INTERVAL_1        30    /* seconds */
#define SYS_DFLT_RMON_HISTORY_INTERVAL_2        1800  /* seconds */

/* Define default thermal parameters.
 */
#if 0 /* obsolete, use SYS_ADPT_THERMAL_<X>_THRESHOLD_UP/DOWN */
#define SYS_DFLT_THERMAL_RISING_THRESHOLD       80
#define SYS_DFLT_THERMAL_FALLING_THRESHOLD      70
#endif

#define SYS_DFLT_THERMAL_ACTION                 VAL_switchThermalActionAction_trap

/* Define default configuration value for thosed Interface related read-write objects
 * in RFC2863 MIB.
 * Note: The SYS_DFLT_IF_ADMIN_STATUS shall also be used as default configuration for
 *       the Port Admin state in SWCRTL subsystem.
 */
#define SYS_DFLT_IF_ADMIN_STATUS                VAL_ifAdminStatus_up
#define SYS_DFLT_IF_LINK_UP_DOWN_TRAP_ENABLE    VAL_ifLinkUpDownTrapEnable_enabled
#define SYS_DFLT_IF_PROMISCUOUS_MODE            VAL_ifPromiscuousMode_false
#define SYS_DFLT_IF_ALIAS                       ""

/* SYS_DFLT_IF_LINK_TRAP_PORT_BASE: the constant to indicate the
 * if_mgr provides the port-base configuration for the link up/down
 * trap of the if_mgr.
 * TRUE     -- port-base configuration for the link up/down trap
 * FALSE    -- global setting for the link up/down trap
 */
#define SYS_DFLT_IF_LINK_TRAP_PORT_BASE         FALSE


/* Define default configuration value for those STP read-write objects in RFC1493 MIB.
 * Note: The value of port_admin_state, if_admin_status, and stp_port_enable state
 *       must be in-sync/consistent each time when one these objects changes its state.
 */
#define SYS_DFLT_STP_BRIDGE_MAX_AGE             20          /* in seconds */
#define SYS_DFLT_STP_BRIDGE_HELLO_TIME          2           /* in seconds */
#define SYS_DFLT_STP_BRIDGE_FORWARD_DELAY       15          /* in seconds */
#define SYS_DFLT_STP_BRIDGE_PRIORITY            32768
#define SYS_DFLT_STP_PORT_PRIORITY              128
#define SYS_DFLT_STP_MODE                       VAL_staSystemStatus_enabled
#define SYS_DFLT_STP_PORT_MODE                  VAL_dot1dStpPortEnable_enabled
#define SYS_DFLT_STP_PORT_FAST_MODE             VAL_staPortFastForward_disabled
#define SYS_DFLT_STP_TRUNK_10M_PORT_PATH_COST   90
#define SYS_DFLT_STP_FDPLX_10M_PORT_PATH_COST   95
#define SYS_DFLT_STP_HDPLX_10M_PORT_PATH_COST   100
#define SYS_DFLT_STP_TRUNK_100M_PORT_PATH_COST  15
#define SYS_DFLT_STP_FDPLX_100M_PORT_PATH_COST  18
#define SYS_DFLT_STP_HDPLX_100M_PORT_PATH_COST  19
#define SYS_DFLT_STP_TRUNK_1GM_PORT_PATH_COST   3
#define SYS_DFLT_STP_FDPLX_1GM_PORT_PATH_COST   4
#define SYS_DFLT_STP_HDPLX_1GM_PORT_PATH_COST   5
#define SYS_DFLT_STP_TRUNK_10GM_PORT_PATH_COST  2
#define SYS_DFLT_STP_FDPLX_10GM_PORT_PATH_COST  2
#define SYS_DFLT_STP_HDPLX_10GM_PORT_PATH_COST  2

#define SYS_DFLT_XSTP_BRIDGE_MAX_AGE            20          /* in seconds */
#define SYS_DFLT_XSTP_BRIDGE_HELLO_TIME         2           /* in seconds */
#define SYS_DFLT_XSTP_BRIDGE_FORWARD_DELAY      15          /* in seconds */
#define SYS_DFLT_XSTP_BRIDGE_PRIORITY           32768
#define SYS_DFLT_XSTP_PORT_PRIORITY             128
#define SYS_DFLT_XSTP_MODE                      VAL_staSystemStatus_enabled
#define SYS_DFLT_XSTP_PORT_MODE                 VAL_dot1dStpPortEnable_enabled
#define SYS_DFLT_XSTP_HDPLX_10M_PORT_PATH_COST  2000000
#define SYS_DFLT_XSTP_FDPLX_10M_PORT_PATH_COST  1000000
#define SYS_DFLT_XSTP_TRUNK_10M_PORT_PATH_COST  500000
#define SYS_DFLT_XSTP_HDPLX_100M_PORT_PATH_COST 200000
#define SYS_DFLT_XSTP_FDPLX_100M_PORT_PATH_COST 100000
#define SYS_DFLT_XSTP_TRUNK_100M_PORT_PATH_COST 50000
#define SYS_DFLT_XSTP_HDPLX_1G_PORT_PATH_COST   20000
#define SYS_DFLT_XSTP_FDPLX_1G_PORT_PATH_COST   10000
#define SYS_DFLT_XSTP_TRUNK_1G_PORT_PATH_COST   5000
#define SYS_DFLT_XSTP_HDPLX_10G_PORT_PATH_COST  2000
#define SYS_DFLT_XSTP_FDPLX_10G_PORT_PATH_COST  1000
#define SYS_DFLT_XSTP_TRUNK_10G_PORT_PATH_COST  500

#define SHORT_FORMAT(_val_)   (((_val_)>65535)?(65535):(_val_))

#define SYS_DFLT_XSTP_HDPLX_10M_PORT_LONG_PATH_COST     SYS_DFLT_XSTP_HDPLX_10M_PORT_PATH_COST
#define SYS_DFLT_XSTP_FDPLX_10M_PORT_LONG_PATH_COST     SYS_DFLT_XSTP_FDPLX_10M_PORT_PATH_COST
#define SYS_DFLT_XSTP_TRUNK_10M_PORT_LONG_PATH_COST     SYS_DFLT_XSTP_TRUNK_10M_PORT_PATH_COST
#define SYS_DFLT_XSTP_HDPLX_100M_PORT_LONG_PATH_COST    SYS_DFLT_XSTP_HDPLX_100M_PORT_PATH_COST
#define SYS_DFLT_XSTP_FDPLX_100M_PORT_LONG_PATH_COST    SYS_DFLT_XSTP_FDPLX_100M_PORT_PATH_COST
#define SYS_DFLT_XSTP_TRUNK_100M_PORT_LONG_PATH_COST    SYS_DFLT_XSTP_TRUNK_100M_PORT_PATH_COST
#define SYS_DFLT_XSTP_HDPLX_1G_PORT_LONG_PATH_COST      SYS_DFLT_XSTP_HDPLX_1G_PORT_PATH_COST
#define SYS_DFLT_XSTP_FDPLX_1G_PORT_LONG_PATH_COST      SYS_DFLT_XSTP_FDPLX_1G_PORT_PATH_COST
#define SYS_DFLT_XSTP_TRUNK_1G_PORT_LONG_PATH_COST      SYS_DFLT_XSTP_TRUNK_1G_PORT_PATH_COST
#define SYS_DFLT_XSTP_HDPLX_10G_PORT_LONG_PATH_COST     SYS_DFLT_XSTP_HDPLX_10G_PORT_PATH_COST
#define SYS_DFLT_XSTP_FDPLX_10G_PORT_LONG_PATH_COST     SYS_DFLT_XSTP_FDPLX_10G_PORT_PATH_COST
#define SYS_DFLT_XSTP_TRUNK_10G_PORT_LONG_PATH_COST     SYS_DFLT_XSTP_TRUNK_10G_PORT_PATH_COST

#define SYS_DFLT_XSTP_HDPLX_10M_PORT_SHORT_PATH_COST    SHORT_FORMAT(SYS_DFLT_XSTP_HDPLX_10M_PORT_PATH_COST)
#define SYS_DFLT_XSTP_FDPLX_10M_PORT_SHORT_PATH_COST    SHORT_FORMAT(SYS_DFLT_XSTP_FDPLX_10M_PORT_PATH_COST)
#define SYS_DFLT_XSTP_TRUNK_10M_PORT_SHORT_PATH_COST    SHORT_FORMAT(SYS_DFLT_XSTP_TRUNK_10M_PORT_PATH_COST)
#define SYS_DFLT_XSTP_HDPLX_100M_PORT_SHORT_PATH_COST   SHORT_FORMAT(SYS_DFLT_XSTP_HDPLX_100M_PORT_PATH_COST)
#define SYS_DFLT_XSTP_FDPLX_100M_PORT_SHORT_PATH_COST   SHORT_FORMAT(SYS_DFLT_XSTP_FDPLX_100M_PORT_PATH_COST)
#define SYS_DFLT_XSTP_TRUNK_100M_PORT_SHORT_PATH_COST   SHORT_FORMAT(SYS_DFLT_XSTP_TRUNK_100M_PORT_PATH_COST)
#define SYS_DFLT_XSTP_HDPLX_1G_PORT_SHORT_PATH_COST     SHORT_FORMAT(SYS_DFLT_XSTP_HDPLX_1G_PORT_PATH_COST)
#define SYS_DFLT_XSTP_FDPLX_1G_PORT_SHORT_PATH_COST     SHORT_FORMAT(SYS_DFLT_XSTP_FDPLX_1G_PORT_PATH_COST)
#define SYS_DFLT_XSTP_TRUNK_1G_PORT_SHORT_PATH_COST     SHORT_FORMAT(SYS_DFLT_XSTP_TRUNK_1G_PORT_PATH_COST)
#define SYS_DFLT_XSTP_HDPLX_10G_PORT_SHORT_PATH_COST    SHORT_FORMAT(SYS_DFLT_XSTP_HDPLX_10G_PORT_PATH_COST)
#define SYS_DFLT_XSTP_FDPLX_10G_PORT_SHORT_PATH_COST    SHORT_FORMAT(SYS_DFLT_XSTP_FDPLX_10G_PORT_PATH_COST)
#define SYS_DFLT_XSTP_TRUNK_10G_PORT_SHORT_PATH_COST    SHORT_FORMAT(SYS_DFLT_XSTP_TRUNK_10G_PORT_PATH_COST)

/* The multiplier of the number of trunk members which determines the path cost for a trunk */
#define SYS_DFLT_XSTP_PATH_COST_TM_FACTOR_10M           1
#define SYS_DFLT_XSTP_PATH_COST_TM_FACTOR_100M          1
#define SYS_DFLT_XSTP_PATH_COST_TM_FACTOR_1G            1
#define SYS_DFLT_XSTP_PATH_COST_TM_FACTOR_10G           1

#define SYS_DFLT_LOOPBACK_DETECTOIN_STATUS      VAL_staLoopbackDetectionPortStatus_enabled
#define SYS_DFLT_LOOPBACK_DETECTOIN_MODE        VAL_staLoopbackDetectionPortReleaseMode_auto
#define SYS_DFLT_LOOPBACK_DETECTOIN_TRAP        VAL_staLoopbackDetectionPortTrapStatus_disabled

#define SYS_DFLT_PORT_ROOT_GUARD_STATUS         VAL_staPortRootGuardAdminStatus_disabled
#define SYS_DFLT_ROOT_GUARD_AGED_TIME           6  /* Wait 6 Seconds */

#define SYS_DFLT_PORT_BPDU_GUARD_STATUS         VAL_staPortBpduGuard_disabled

#define SYS_DFLT_PORT_BPDU_FILTER_STATUS        VAL_staPortBpduFilter_disabled

#if(SYS_CPNT_BPDU_FLOOD_TO_VLAN == TRUE)
#define SYS_DFLT_XSTP_FLOODING_BEHAVIOR                 VAL_staSystemBPDUFlooding_to_vlan
#else
#define SYS_DFLT_XSTP_FLOODING_BEHAVIOR                 VAL_staSystemBPDUFlooding_to_all
#endif
#define SYS_DFLT_XSTP_PORT_FLOODING                     VAL_staPortBpduFlooding_enabled


#define SYS_DFLT_L2_DYNAMIC_ADDR_AGING_TIME     300         /* in seconds */
#define SYS_DFLT_L2_DYNAMIC_ADDR_AGING_STATUS   VAL_amtrMacAddrAgingStatus_enabled
#define SYS_DFLT_L2_ADDR_PSEC_LEARN_COUNT_INCLUDE_CONFIG    FALSE /* Whether auto learning max counter include UI conig or not.*/

/* These define parameters for Clustering
 */
#define SYS_DFLT_CLUSTER_DEFAULT_VLAN                       4093         /* clustering VLAN */
#define SYS_DFLT_CLUSTER_DEFAULT_IP_POOL                    10,254,254,1 /* 10.254.254.1 */

#define SYS_DFLT_CLUSTER_HEARTBEAT_INTERVAL                 30           /* seconds */
#define SYS_DFLT_CLUSTER_HEARTBEAT_LOSS_COUNT               3

#define SYS_DFLT_CLUSTER_STATUS                             VAL_clusterEnable_disabled
#define SYS_DFLT_CLUSTER_ADDR_WORD1                         0x0012
#define SYS_DFLT_CLUSTER_ADDR_WORD2                         0xCF00
#define SYS_DFLT_CLUSTER_ADDR_WORD3                         0x0000


/* Define default configuration value for those 1P/GARP/GMRP/GVRP related read-write objects
 * in RFC2674 MIB.
 */
#define SYS_DFLT_1P_TRAFFIC_CLASSES_ENABLED     VAL_dot1dTrafficClassesEnabled_true

#define SYS_DFLT_1P_PORT_DEFAULT_USER_PRIORITY  0           /* refer to 802.1D 1998 */
#define SYS_DFLT_1P_0_Regen_User_Priority       0           /* regenerated 1P priority for a rcvd priority 0 packet */
#define SYS_DFLT_1P_1_Regen_User_Priority       1           /* regenerated 1P priority for a rcvd priority 1 packet */
#define SYS_DFLT_1P_2_Regen_User_Priority       2           /* regenerated 1P priority for a rcvd priority 2 packet */
#define SYS_DFLT_1P_3_Regen_User_Priority       3           /* regenerated 1P priority for a rcvd priority 3 packet */
#define SYS_DFLT_1P_4_Regen_User_Priority       4           /* regenerated 1P priority for a rcvd priority 4 packet */
#define SYS_DFLT_1P_5_Regen_User_Priority       5           /* regenerated 1P priority for a rcvd priority 5 packet */
#define SYS_DFLT_1P_6_Regen_User_Priority       6           /* regenerated 1P priority for a rcvd priority 6 packet */
#define SYS_DFLT_1P_7_Regen_User_Priority       7           /* regenerated 1P priority for a rcvd priority 7 packet */

#define SYS_DFLT_1P_0_MAPPED_TRAFFIC_CLASS      2           /* priority 0 mapped to traffic class (queue) 1 */
#define SYS_DFLT_1P_1_MAPPED_TRAFFIC_CLASS      0           /* priority 1 mapped to traffic class (queue) 0 */
#define SYS_DFLT_1P_2_MAPPED_TRAFFIC_CLASS      1           /* priority 2 mapped to traffic class (queue) 0 */
#define SYS_DFLT_1P_3_MAPPED_TRAFFIC_CLASS      3           /* priority 3 mapped to traffic class (queue) 1 */
#define SYS_DFLT_1P_4_MAPPED_TRAFFIC_CLASS      4           /* priority 4 mapped to traffic class (queue) 2 */
#define SYS_DFLT_1P_5_MAPPED_TRAFFIC_CLASS      5           /* priority 5 mapped to traffic class (queue) 2 */
#define SYS_DFLT_1P_6_MAPPED_TRAFFIC_CLASS      6           /* priority 6 mapped to traffic class (queue) 3 */
#define SYS_DFLT_1P_7_MAPPED_TRAFFIC_CLASS      7           /* priority 7 mapped to traffic class (queue) 3 */

#define SYS_DFLT_1D_PORT_GARP_JOIN_TIME         20          /* in centiseconds */
#define SYS_DFLT_1D_PORT_GARP_LEAVE_TIME        60          /* in centiseconds */
#define SYS_DFLT_1D_PORT_GARP_LEAVE_ALL_TIME    1000        /* in centiseconds */

#define SYS_DFLT_1D_GMRP_STATUS                 VAL_dot1dGmrpStatus_disabled

#define SYS_DFLT_1Q_GVRP_STATUS                 VAL_dot1qGvrpStatus_disabled
#define SYS_DFLT_1Q_PORT_VLAN_PVID              1
#define SYS_DFLT_1Q_PORT_ACCEPTABLE_FRAME_TYPES VAL_dot1qPortAcceptableFrameTypes_admitAll
#define SYS_DFLT_1Q_PORT_INGRESS_FILTERING      VAL_dot1qPortIngressFiltering_false
#define SYS_DFLT_1Q_PORT_GVRP_STATUS            VAL_dot1qPortGvrpStatus_disabled

/* The value of SYS_DFLT_DOT1QMAXVLANID should be 4089 in 3com projects,
 * 4093 in other stackable projects
 * and 4094 in the non-stackable projects.
 */
#define SYS_DFLT_DOT1QMAXVLANID                 4093

#define SYS_DFLT_DOT1Q_PORT_TPID_FIELD          0x8100	/*Wingson 2005-01-27, for QINQ*/
#define SYS_DFLT_DOT1Q_TUNNEL_PRIORITY_MAP_STATUS   VAL_vlanDot1qTunnelPriorityMapStatus_disabled



/* Definition for the method to configure the traffic segmentation method as the port private mode or the traditional definition.
 * SYS_DFLT_TRAFFIC_SEG_METHOD_TRADITION            : define the port type as normal, uplink and downlink and default is the normal port.
 * SYS_DFLT_TRAFFIC_SEG_METHOD_PORT_PRIVATE_MODE    : define the port type as public (uplink) and private (downlink) and default is the public port.
 */
#define SYS_DFLT_TRAFFIC_SEG_METHOD_TRADITION           1
#define SYS_DFLT_TRAFFIC_SEG_METHOD_PORT_PRIVATE_MODE   2
#define SYS_DFLT_TRAFFIC_SEG_METHOD                     SYS_DFLT_TRAFFIC_SEG_METHOD_TRADITION

/* Definition the mode/behavior for the traffic segmentation cross session between uplink to uplink - blocking or forwarding.
 * SYS_DFLT_TRAFFIC_SEG_UPLINK_TO_UPLINK_MODE_BLOCKING      : blocking between cross session up-link to up-link.
 * SYS_DFLT_TRAFFIC_SEG_UPLINK_TO_UPLINK_MODE_FORWARDING    : forwarding between cross session up-link to up-link.
 */
#define SYS_DFLT_TRAFFIC_SEG_UPLINK_TO_UPLINK_MODE_BLOCKING     1
#define SYS_DFLT_TRAFFIC_SEG_UPLINK_TO_UPLINK_MODE_FORWARDING   2
#define SYS_DFLT_TRAFFIC_SEG_UPLINK_TO_UPLINK_MODE    SYS_DFLT_TRAFFIC_SEG_UPLINK_TO_UPLINK_MODE_BLOCKING

/* Define default configuration value for those GVRP-Extension related read-write objects
 * in 802.1u Bridge MIB.
 */
#define SYS_DFLT_1Q_PORT_Restricted_Registration            VAL_dot1qPortRestrictedRegistration_false


/* Define default configuration value for those IGMP related read-write objects
 * in RFC2933 MIB.
 * Note: The default vaule defined for IGMP protocol shall also be used as the
 *       default value for the IGMP Snooping Querier Function.
 */
#define SYS_DFLT_IGMP_INTERFACE_QUERY_INTERVAL              125     /* in seconds */
#define SYS_DFLT_IGMP_INTERFACE_VERSION                     2       /* IGMPv2 */
#define SYS_DFLT_IGMP_INTERFACE_QUERY_MAX_RESPONSE_TIME     100      /* in seconds */
#define SYS_DFLT_IGMP_INTERFACE_PROXY_IFINDEX               0       /* no proxying is being done */
#define SYS_DFLT_IGMP_INTERFACE_ROBUSTNESS                  2       /* IGMP is robust to (Robustness Variable-1) packet losses. */
#define SYS_DFLT_IGMP_INTERFACE_LAST_MEMBER_QUERY_INTERVAL  10       /* in seconds */
#define SYS_DFLT_IGMP_CACHE_SELF                            VAL_igmpCacheSelf_true

/* 2009-04-14 Jimi Chen */
#define SYS_DFLT_IGMP_INTERFACE_PROXY_UNSOLICITED_REPORT_INTERVAL  400  /* in seconds */
#define SYS_DFLT_IGMP_INTERFACE_STATUS                      FALSE

/* Define default configuration value for those read-write objects in ES3626A Private MIB.
 * Note: 1. The default vaule defined for IGMP protocol shall also be used as the
 *          default value for the IGMP Snooping Querier Function.
 *       2. The resolution of "query max response time" defined for IGNMP Snooping Querier
 *          function does not comply with the standard IGMP RFC2933 MIB.
 *          The resolution of "query max response time" defined for IGNMP Snooping Querier
 *          function is "seconds", but not "tenths of seconds" as defined in IGMP RFC2933 MIB.
 */
#define SYS_DFLT_SWITCH_MANAGEMENT_VLAN                 SYS_DFLT_1Q_PORT_VLAN_PVID
#define SYS_DFLT_VLAN_ADDRESS_METHOD                    VAL_vlanAddressMethod_user
#define SYS_DFLT_PORT_NAME                              SYS_DFLT_IF_ALIAS
#define SYS_DFLT_PORT_FLOW_CONTROL                      VAL_portFlowCtrlCfg_disabled
#define SYS_DFLT_PORT_AUTONEGOTIATION                   VAL_portAutonegotiation_enabled
#define SYS_DFLT_PORT_MTU                               0x5ee /* 1518 */

/*added by jinhua.wei,*/
#define SYS_DFLT_PORT_MAC_LEARNING                      VAL_staPortMacAddrLearning_true
#define SYS_DFLT_MACADDR_LEARNING_COUNT                 VAL_staMacAddrLearning_Count
#define SYS_DFLT_10_PORT_CAPABILITIES                   (SYS_VAL_portCapabilities_portCap10half     |   \
                                                         SYS_VAL_portCapabilities_portCap10full)

#define SYS_DFLT_10_100_PORT_CAPABILITIES               (SYS_VAL_portCapabilities_portCap10half     |   \
                                                         SYS_VAL_portCapabilities_portCap10full     |   \
                                                         SYS_VAL_portCapabilities_portCap100half    |   \
                                                         SYS_VAL_portCapabilities_portCap100full)

#define SYS_DFLT_10_100_1000_PORT_CAPABILITIES          (SYS_VAL_portCapabilities_portCap10half     |   \
                                                         SYS_VAL_portCapabilities_portCap10full     |   \
                                                         SYS_VAL_portCapabilities_portCap100half    |   \
                                                         SYS_VAL_portCapabilities_portCap100full    |   \
                                                         SYS_VAL_portCapabilities_portCap1000full)

#define SYS_DFLT_10_FIBER_PORT_CAPABILITIES             (SYS_VAL_portCapabilities_portCap10full)

#define SYS_DFLT_100_FIBER_PORT_CAPABILITIES            (SYS_VAL_portCapabilities_portCap100full)

#define SYS_DFLT_1000_FIBER_PORT_CAPABILITIES           (SYS_VAL_portCapabilities_portCap1000full)

/* For combo ports, when nothing is plugged, it should be assumed copper.
 * So, SYS_DFLT_COMBO_PORT_FORCED_MODE must be VAL_portComboForcedMode_copperForced.
 * Zhong Qiyao, 2004.07.01.
 * Charles comment: Qiyao's modification is wrong, so change back.
 * 2004.08.04
 */
#define SYS_DFLT_COMBO_PORT_FORCED_MODE                 VAL_portComboForcedMode_sfpPreferredAuto

#define SYS_DFLT_COMBO_PORT_FORCED_MODE_SFP_SPEED       VAL_portType_thousandBaseSfp

/* The definition is used for MDIX mode.
 */
#define SYS_DFLT_MDIX_MODE                              VAL_portMdixMode_auto

#define SYS_DFLT_PORT_LACP_PORT_STATUS                  VAL_lacpPortStatus_disabled

#define SYS_DFLT_STA_SYSTEM_STATUS                      VAL_staSystemStatus_enabled
#define SYS_DFLT_STA_PORT_MGT_FAST_FORWARD              VAL_staPortMgtFastForward_enabled

#define SYS_DFLT_restart_OpCode_File                    "Factory_Default_Run_2_0_0_0.img"
#define SYS_DFLT_restartConfigFile                      "Factory_Default_Config.cfg"

#define SYS_DFLT_MIRROR_TYPE                            VAL_mirrorType_both

#define SYS_DFLT_IGMP_SNOOP_STATUS                      VAL_igmpSnoopStatus_enabled
#define SYS_DFLT_IGMP_SNOOP_QUERIER                     VAL_igmpSnoopQuerier_disabled
#define SYS_DFLT_IGMP_SNOOP_LEAVE_PROXY_STATUS          VAL_igmpSnoopLeaveProxy_disabled
#define SYS_DFLT_IGMP_SNOOP_QUERY_COUNT                 2
#define SYS_DFLT_IGMP_SNOOP_QUERY_INTERVAL              SYS_DFLT_IGMP_INTERFACE_QUERY_INTERVAL
#define SYS_DFLT_IGMP_SNOOP_QUERY_MAX_RESPONSE_TIME     (SYS_DFLT_IGMP_INTERFACE_QUERY_MAX_RESPONSE_TIME/10)
#define SYS_DFLT_IGMP_SNOOP_QUERY_TIMEOUT               300     /* in seconds */
#define SYS_DFLT_IGMP_SNOOP_VERSION                     SYS_DFLT_IGMP_INTERFACE_VERSION
#define SYS_DFLT_IGMP_SNOOP_IMMEDIATE_LEAVE_STATUS      VAL_igmpSnoopCurrentVlanImmediateLeave_disabled     /* 2006-02, Joseph */
#define SYS_DFLT_IGMP_SNOOP_PROXY_REPORTING             FALSE  /*TRUE: default enabled proxy reporting; FALSE: defautl disable proxy reporting*/
#define SYS_DFLT_IGMP_SNOOP_MRD                         VAL_igmpSnoopVlanMrd_disabled

#define SYS_DFLT_IGMP_FILTER_STATUS                     VAL_igmpSnoopFilterStatus_disabled
#define SYS_DFLT_IGMP_PROFILE_ACCESS_MODE               VAL_igmpSnoopProfileAction_deny
#define SYS_DFLT_IGMP_PROFILE_ID_NULL                   0
#define SYS_DFLT_IGMP_THROTTLE_ACTION                   VAL_igmpSnoopThrottlePortAction_deny

/* default igmpsnp group multicast data forwarding pirority
*/
#define SYS_DFLT_IGMP_SNOOP_FORWARD_PRIORITY            SYS_ADPT_MULTICAST_FORWARDING_PRIORITY

/* Define default value of MVR (Multicast Vlan Registration)
 */
#define SYS_DFLT_MVR_MVR_VLAN                           1
#define SYS_DFLT_MVR_COUNT                              1
#define SYS_DFLT_MVR_ADMIN_STATUS                       VAL_mvrStatus_disabled
/* default mvr group multicast data forwarding pirority
*/
#define SYS_DFLT_MVR_FORWARD_PRIORITY                   SYS_ADPT_MULTICAST_FORWARDING_PRIORITY



#define SYS_DFLT_NET_DEFAULT_GATEWAY                    0x00000000       /* 0.0.0.0 for undefined default gateway */

/* This defines default values for HTTP.
 */
#define SYS_DFLT_IP_HTTP_STATE                          VAL_ipHttpState_enabled
#define SYS_DFLT_IP_HTTP_PORT                           80      /* the UDP port number for HTTP */

/* This defines default values for WEBAUTH.
 */
#define SYS_DFLT_WEBAUTH_STATUS                         VAL_webauthEnable_disabled
#define SYS_DFLT_WEBAUTH_PORT_STATUS                    VAL_webAuthPortConfigStatus_disabled
#define SYS_DFLT_WEBAUTH_MAX_LOGIN_ATTEMPTS             3
#define SYS_DFLT_WEBAUTH_SESSION_TIMEOUT                3600
#define SYS_DFLT_WEBAUTH_QUIET_PERIOD                   60

/* This defines default values for telnet.
 */
#define SYS_DFLT_TELNET_SOCKET_PORT                     23
#define SYS_DFLT_TELNET_DEFAULT_MAX_SESSION             8
#define SYS_DFLT_TELNET_DEFAULT_STATE                   VAL_telnetStatus_enabled

/* This defines default values for TFTP.
 */
#define SYS_DFLT_TFTP_NUMBER_OF_RETRIES                 15
#define SYS_DFLT_TFTP_PORT                              5000    /* wuli 20040607 */

#define SYS_DFLT_XFER_AUTO_UPGRADE_OPCODE_STATUS        VAL_fileAutoUpgradeOpCodeStatus_disabled

/* The defalut search path for auto opcode upgrade.
 *
 * If want to assign a default search path, set as the following:
 * tftp://192.168.0.1[/filedir]/ via TFTP
 * ftp://[username[:password@]]192.168.0.1[/filedir]/ via FTP
 *
 * Set this path as "" to not specify.
 */
#define SYS_DFLT_XFER_AUTO_UPGRADE_OPCODE_PATH          ""

/* Egress Packet Block */
/* This defines default values for egress packet block function.
 */
#define SYS_DFLT_EGRESS_PACKET_BLOCK_UNKNOWN_MCAST_STATUS         VAL_egressPacketBlockUnknownMcastStatus_disabled /* default: disabled */
#define SYS_DFLT_EGRESS_PACKET_BLOCK_UNKNOWN_UCAST_STATUS         VAL_egressPacketBlockUnknownUcastStatus_disabled /* default: disabled */

#define SYS_DFLT_BCAST_STORM_STATUS                     VAL_bcastStormStatus_enabled
#define SYS_DFLT_BCAST_STORM_MODE                       VAL_bcastStormSampleType_octet_rate
#define SYS_DFLT_MCAST_STORM_STATUS                     VAL_mcastStormStatus_disabled
#define SYS_DFLT_MCAST_STORM_MODE                       VAL_mcastStormSampleType_octet_rate
#define SYS_DFLT_UNKUCAST_STORM_STATUS                  VAL_unknownUcastStormStatus_disabled
#define SYS_DFLT_UNKUCAST_STORM_MODE                    VAL_unknownUcastStormSampleType_octet_rate


#if (SYS_CPNT_ATC_STORM == TRUE)
/* ATC Broadcast Storm */
/* This defines default values for ATC broadcast storm function.
 */
#define SYS_DFLT_ATC_BSTORM_STATUS                          VAL_atcBcastStormEnable_disabled
#define SYS_DFLT_ATC_BSTORM_TYPE                            VAL_atcBcastStormSampleType_packet_rate
#define SYS_DFLT_ATC_BSTORM_ATC_ON                          VAL_atcBcastStormEnable_disabled
#define SYS_DFLT_ATC_BSTORM_ATC_RELEASE                     VAL_atcBcastStormAutoRelease_disabled
#define SYS_DFLT_ATC_BSTORM_POLLING_TIMER                   1
#define SYS_DFLT_ATC_BSTORM_TC_ON_TIMER                     300
#define SYS_DFLT_ATC_BSTORM_TC_RELEASE_TIMER                900
#define SYS_DFLT_ATC_BSTORM_STORM_ALARM_THRESHOLD           (128 * SYS_ADPT_ATC_STORM_CONTROL_UNIT)
#define SYS_DFLT_ATC_BSTORM_STORM_CLEAR_THRESHOLD           (128 * SYS_ADPT_ATC_STORM_CONTROL_UNIT)
#define SYS_DFLT_ATC_BSTORM_STORM_TRAP_STORM_ALARM          VAL_atcBcastStormAlarmFireTrapStatus_disabled
#define SYS_DFLT_ATC_BSTORM_STORM_TRAP_STORM_CLEAR          VAL_atcBcastStormAlarmClearTrapStatus_disabled
#define SYS_DFLT_ATC_BSTORM_STORM_TRAP_TC_ON                VAL_atcBcastStormTcApplyTrapStatus_disabled
#define SYS_DFLT_ATC_BSTORM_STORM_TRAP_TC_RELEASE           VAL_atcBcastStormTcReleaseTrapStatus_disabled
#define SYS_DFLT_ATC_BSTORM_STORM_ACTION                     VAL_atcBcastStormTcAction_rate_control
#define SYS_DFLT_ATC_BSTORM_STORM_TC_ON                     VAL_atcBcastStormTrafficControlApply_disabled
#define SYS_DFLT_ATC_BSTORM_STORM_TC_RELEASE                VAL_atcBcastStormTrafficControlRelease_disabled
#define SYS_DFLT_ATC_BSTORM_OPEARATION_STATUS               VAL_ifOperStatus_up

/* ATC Multicast Storm */
/* This defines default values for ATC multicast storm function.
 */
#define SYS_DFLT_ATC_MSTORM_STATUS                          VAL_atcMcastStormEnable_disabled
#define SYS_DFLT_ATC_MSTORM_TYPE                            VAL_atcMcastStormSampleType_packet_rate
#define SYS_DFLT_ATC_MSTORM_ATC_ON                          VAL_atcMcastStormEnable_disabled
#define SYS_DFLT_ATC_MSTORM_ATC_RELEASE                     VAL_atcMcastStormAutoRelease_disabled
#define SYS_DFLT_ATC_MSTORM_POLLING_TIMER                   1
#define SYS_DFLT_ATC_MSTORM_TC_ON_TIMER                     300
#define SYS_DFLT_ATC_MSTORM_TC_RELEASE_TIMER                900
#define SYS_DFLT_ATC_MSTORM_STORM_ALARM_THRESHOLD           (128 * SYS_ADPT_ATC_STORM_CONTROL_UNIT)
#define SYS_DFLT_ATC_MSTORM_STORM_CLEAR_THRESHOLD           (128 * SYS_ADPT_ATC_STORM_CONTROL_UNIT)
#define SYS_DFLT_ATC_MSTORM_STORM_TRAP_STORM_ALARM          VAL_atcMcastStormAlarmFireTrapStatus_disabled
#define SYS_DFLT_ATC_MSTORM_STORM_TRAP_STORM_CLEAR          VAL_atcMcastStormAlarmClearTrapStatus_disabled
#define SYS_DFLT_ATC_MSTORM_STORM_TRAP_TC_ON                VAL_atcMcastStormTcApplyTrapStatus_disabled
#define SYS_DFLT_ATC_MSTORM_STORM_TRAP_TC_RELEASE           VAL_atcMcastStormTcReleaseTrapStatus_disabled
#define SYS_DFLT_ATC_MSTORM_STORM_ACTION                    VAL_atcMcastStormTcAction_rate_control
#define SYS_DFLT_ATC_MSTORM_STORM_TC_ON                     VAL_atcMcastStormTrafficControlApply_disabled
#define SYS_DFLT_ATC_MSTORM_STORM_TC_RELEASE                VAL_atcMcastStormTrafficControlRelease_disabled
#define SYS_DFLT_ATC_MSTORM_OPEARATION_STATUS               VAL_ifOperStatus_up
#endif

#define SYS_DFLT_VLAN_PORT_1Q_TRUNK                     VAL_vlanPortDot1qTrunk_disabled

/* Configuring trunk load-balance modes
 */
#define SYS_DFLT_TRUNK_BALANCE_MODE                     VAL_trunkBalanceMode_ipSrcDst

/* define naming constants for default SYSLOG */
#define SYS_DFLT_SYSLOG_STATUS                          VAL_sysLogStatus_enabled
#define SYS_DFLT_SYSLOG_FLASH_LOG_LEVEL                 3
#define SYS_DFLT_SYSLOG_UC_LOG_LEVEL                    7


/* Define default configuration value for the mapping from TOS to COS.
 * Note: 1. The default vaule defined is the user priority, range from 0 to 7.
 *       2. The mapping from user priority to cos queue number is provided in 802.1p module.
 *       3. The default value follow the suggestion of 802.1D
 */

#define SYS_DFLT_IP_PREC_0_MAPPED_COS                   0
#define SYS_DFLT_IP_PREC_1_MAPPED_COS                   1
#define SYS_DFLT_IP_PREC_2_MAPPED_COS                   2
#define SYS_DFLT_IP_PREC_3_MAPPED_COS                   3
#define SYS_DFLT_IP_PREC_4_MAPPED_COS                   4
#define SYS_DFLT_IP_PREC_5_MAPPED_COS                   5
#define SYS_DFLT_IP_PREC_6_MAPPED_COS                   6
#define SYS_DFLT_IP_PREC_7_MAPPED_COS                   7

/* Define default configuration value for the mapping from DSCP to COS.
 * Note: 1. The default vaule defined is the user priority, range from 0 to 7.
 *       2. The mapping from user priority to cos queue number is provided in 802.1p module.
 *       3. The default value follow the suggestion of RFC 2474 (PHB_CS), 2597(PHB_AF), and 2598 (PHB_EF)
 */
#define SYS_DFLT_DSCP_0_MAPPED_COS                      0  /* dscp  0 */
#define SYS_DFLT_DSCP_1_MAPPED_COS                      0  /* dscp  1 */
#define SYS_DFLT_DSCP_2_MAPPED_COS                      0  /* dscp  2 */
#define SYS_DFLT_DSCP_3_MAPPED_COS                      0  /* dscp  3 */
#define SYS_DFLT_DSCP_4_MAPPED_COS                      0  /* dscp  4 */
#define SYS_DFLT_DSCP_5_MAPPED_COS                      0  /* dscp  5 */
#define SYS_DFLT_DSCP_6_MAPPED_COS                      0  /* dscp  6 */
#define SYS_DFLT_DSCP_7_MAPPED_COS                      0  /* dscp  7 */
#define SYS_DFLT_DSCP_8_MAPPED_COS                      1  /* dscp  8 */
#define SYS_DFLT_DSCP_9_MAPPED_COS                      1  /* dscp  9 */
#define SYS_DFLT_DSCP_10_MAPPED_COS                     1  /* dscp  10 */
#define SYS_DFLT_DSCP_11_MAPPED_COS                     1  /* dscp  11 */
#define SYS_DFLT_DSCP_12_MAPPED_COS                     1  /* dscp  12 */
#define SYS_DFLT_DSCP_13_MAPPED_COS                     1  /* dscp  13 */
#define SYS_DFLT_DSCP_14_MAPPED_COS                     1  /* dscp  14 */
#define SYS_DFLT_DSCP_15_MAPPED_COS                     1  /* dscp  15 */
#define SYS_DFLT_DSCP_16_MAPPED_COS                     2  /* dscp  16 */
#define SYS_DFLT_DSCP_17_MAPPED_COS                     2  /* dscp  17 */
#define SYS_DFLT_DSCP_18_MAPPED_COS                     2  /* dscp  18 */
#define SYS_DFLT_DSCP_19_MAPPED_COS                     2  /* dscp  19 */
#define SYS_DFLT_DSCP_20_MAPPED_COS                     2  /* dscp  20 */
#define SYS_DFLT_DSCP_21_MAPPED_COS                     2  /* dscp  21 */
#define SYS_DFLT_DSCP_22_MAPPED_COS                     2  /* dscp  22 */
#define SYS_DFLT_DSCP_23_MAPPED_COS                     2  /* dscp  23 */
#define SYS_DFLT_DSCP_24_MAPPED_COS                     3  /* dscp  24 */
#define SYS_DFLT_DSCP_25_MAPPED_COS                     3  /* dscp  25 */
#define SYS_DFLT_DSCP_26_MAPPED_COS                     3  /* dscp  26 */
#define SYS_DFLT_DSCP_27_MAPPED_COS                     3  /* dscp  27 */
#define SYS_DFLT_DSCP_28_MAPPED_COS                     3  /* dscp  28 */
#define SYS_DFLT_DSCP_29_MAPPED_COS                     3  /* dscp  29 */
#define SYS_DFLT_DSCP_30_MAPPED_COS                     3  /* dscp  30 */
#define SYS_DFLT_DSCP_31_MAPPED_COS                     3  /* dscp  31 */
#define SYS_DFLT_DSCP_32_MAPPED_COS                     4  /* dscp  32 */
#define SYS_DFLT_DSCP_33_MAPPED_COS                     4  /* dscp  33 */
#define SYS_DFLT_DSCP_34_MAPPED_COS                     4  /* dscp  34 */
#define SYS_DFLT_DSCP_35_MAPPED_COS                     4  /* dscp  35 */
#define SYS_DFLT_DSCP_36_MAPPED_COS                     4  /* dscp  36 */
#define SYS_DFLT_DSCP_37_MAPPED_COS                     4  /* dscp  37 */
#define SYS_DFLT_DSCP_38_MAPPED_COS                     4  /* dscp  38 */
#define SYS_DFLT_DSCP_39_MAPPED_COS                     4  /* dscp  39 */
#define SYS_DFLT_DSCP_40_MAPPED_COS                     5  /* dscp  40 */
#define SYS_DFLT_DSCP_41_MAPPED_COS                     5  /* dscp  41 */
#define SYS_DFLT_DSCP_42_MAPPED_COS                     5  /* dscp  42 */
#define SYS_DFLT_DSCP_43_MAPPED_COS                     5  /* dscp  43 */
#define SYS_DFLT_DSCP_44_MAPPED_COS                     5  /* dscp  44 */
#define SYS_DFLT_DSCP_45_MAPPED_COS                     5  /* dscp  45 */
#define SYS_DFLT_DSCP_46_MAPPED_COS                     5  /* dscp  46 */
#define SYS_DFLT_DSCP_47_MAPPED_COS                     5  /* dscp  47 */
#define SYS_DFLT_DSCP_48_MAPPED_COS                     6  /* dscp  48 */
#define SYS_DFLT_DSCP_49_MAPPED_COS                     6  /* dscp  49 */
#define SYS_DFLT_DSCP_50_MAPPED_COS                     6  /* dscp  50 */
#define SYS_DFLT_DSCP_51_MAPPED_COS                     6  /* dscp  51 */
#define SYS_DFLT_DSCP_52_MAPPED_COS                     6  /* dscp  52 */
#define SYS_DFLT_DSCP_53_MAPPED_COS                     6  /* dscp  53 */
#define SYS_DFLT_DSCP_54_MAPPED_COS                     6  /* dscp  54 */
#define SYS_DFLT_DSCP_55_MAPPED_COS                     6  /* dscp  55 */
#define SYS_DFLT_DSCP_56_MAPPED_COS                     7  /* dscp  56 */
#define SYS_DFLT_DSCP_57_MAPPED_COS                     7  /* dscp  57 */
#define SYS_DFLT_DSCP_58_MAPPED_COS                     7  /* dscp  58 */
#define SYS_DFLT_DSCP_59_MAPPED_COS                     7  /* dscp  59 */
#define SYS_DFLT_DSCP_60_MAPPED_COS                     7  /* dscp  60 */
#define SYS_DFLT_DSCP_61_MAPPED_COS                     7  /* dscp  61 */
#define SYS_DFLT_DSCP_62_MAPPED_COS                     7  /* dscp  62 */
#define SYS_DFLT_DSCP_63_MAPPED_COS                     7  /* dscp  63 */



#define SYS_DFLT_DSCP_0_MAPPED_COS_ENABLED              TRUE   /* dscp  0 */
#define SYS_DFLT_DSCP_1_MAPPED_COS_ENABLED              TRUE  /* dscp  1 */
#define SYS_DFLT_DSCP_2_MAPPED_COS_ENABLED              TRUE  /* dscp  2 */
#define SYS_DFLT_DSCP_3_MAPPED_COS_ENABLED              TRUE  /* dscp  3 */
#define SYS_DFLT_DSCP_4_MAPPED_COS_ENABLED              TRUE  /* dscp  4 */
#define SYS_DFLT_DSCP_5_MAPPED_COS_ENABLED              TRUE  /* dscp  5 */
#define SYS_DFLT_DSCP_6_MAPPED_COS_ENABLED              TRUE  /* dscp  6 */
#define SYS_DFLT_DSCP_7_MAPPED_COS_ENABLED              TRUE  /* dscp  7 */
#define SYS_DFLT_DSCP_8_MAPPED_COS_ENABLED              TRUE  /* dscp  8 */
#define SYS_DFLT_DSCP_9_MAPPED_COS_ENABLED              TRUE  /* dscp  9 */
#define SYS_DFLT_DSCP_10_MAPPED_COS_ENABLED             TRUE  /* dscp  10 */
#define SYS_DFLT_DSCP_11_MAPPED_COS_ENABLED             TRUE  /* dscp  11 */
#define SYS_DFLT_DSCP_12_MAPPED_COS_ENABLED             TRUE  /* dscp  12 */
#define SYS_DFLT_DSCP_13_MAPPED_COS_ENABLED             TRUE  /* dscp  13 */
#define SYS_DFLT_DSCP_14_MAPPED_COS_ENABLED             TRUE  /* dscp  14 */
#define SYS_DFLT_DSCP_15_MAPPED_COS_ENABLED             TRUE  /* dscp  15 */
#define SYS_DFLT_DSCP_16_MAPPED_COS_ENABLED             TRUE  /* dscp  16 */
#define SYS_DFLT_DSCP_17_MAPPED_COS_ENABLED             TRUE  /* dscp  17 */
#define SYS_DFLT_DSCP_18_MAPPED_COS_ENABLED             TRUE  /* dscp  18 */
#define SYS_DFLT_DSCP_19_MAPPED_COS_ENABLED             TRUE  /* dscp  19 */
#define SYS_DFLT_DSCP_20_MAPPED_COS_ENABLED             TRUE  /* dscp  20 */
#define SYS_DFLT_DSCP_21_MAPPED_COS_ENABLED             TRUE  /* dscp  21 */
#define SYS_DFLT_DSCP_22_MAPPED_COS_ENABLED             TRUE  /* dscp  22 */
#define SYS_DFLT_DSCP_23_MAPPED_COS_ENABLED             TRUE  /* dscp  23 */
#define SYS_DFLT_DSCP_24_MAPPED_COS_ENABLED             TRUE  /* dscp  24 */
#define SYS_DFLT_DSCP_25_MAPPED_COS_ENABLED             TRUE  /* dscp  25 */
#define SYS_DFLT_DSCP_26_MAPPED_COS_ENABLED             TRUE  /* dscp  26 */
#define SYS_DFLT_DSCP_27_MAPPED_COS_ENABLED             TRUE  /* dscp  27 */
#define SYS_DFLT_DSCP_28_MAPPED_COS_ENABLED             TRUE  /* dscp  28 */
#define SYS_DFLT_DSCP_29_MAPPED_COS_ENABLED             TRUE  /* dscp  29 */
#define SYS_DFLT_DSCP_30_MAPPED_COS_ENABLED             TRUE  /* dscp  30 */
#define SYS_DFLT_DSCP_31_MAPPED_COS_ENABLED             TRUE  /* dscp  31 */
#define SYS_DFLT_DSCP_32_MAPPED_COS_ENABLED             TRUE  /* dscp  32 */
#define SYS_DFLT_DSCP_33_MAPPED_COS_ENABLED             TRUE  /* dscp  33 */
#define SYS_DFLT_DSCP_34_MAPPED_COS_ENABLED             TRUE  /* dscp  34 */
#define SYS_DFLT_DSCP_35_MAPPED_COS_ENABLED             TRUE  /* dscp  35 */
#define SYS_DFLT_DSCP_36_MAPPED_COS_ENABLED             TRUE  /* dscp  36 */
#define SYS_DFLT_DSCP_37_MAPPED_COS_ENABLED             TRUE  /* dscp  37 */
#define SYS_DFLT_DSCP_38_MAPPED_COS_ENABLED             TRUE  /* dscp  38 */
#define SYS_DFLT_DSCP_39_MAPPED_COS_ENABLED             TRUE  /* dscp  39 */
#define SYS_DFLT_DSCP_40_MAPPED_COS_ENABLED             TRUE  /* dscp  40 */
#define SYS_DFLT_DSCP_41_MAPPED_COS_ENABLED             TRUE  /* dscp  41 */
#define SYS_DFLT_DSCP_42_MAPPED_COS_ENABLED             TRUE  /* dscp  42 */
#define SYS_DFLT_DSCP_43_MAPPED_COS_ENABLED             TRUE  /* dscp  43 */
#define SYS_DFLT_DSCP_44_MAPPED_COS_ENABLED             TRUE  /* dscp  44 */
#define SYS_DFLT_DSCP_45_MAPPED_COS_ENABLED             TRUE  /* dscp  45 */
#define SYS_DFLT_DSCP_46_MAPPED_COS_ENABLED             TRUE   /* dscp  46 */
#define SYS_DFLT_DSCP_47_MAPPED_COS_ENABLED             TRUE  /* dscp  47 */
#define SYS_DFLT_DSCP_48_MAPPED_COS_ENABLED             TRUE   /* dscp  48 */
#define SYS_DFLT_DSCP_49_MAPPED_COS_ENABLED             TRUE  /* dscp  49 */
#define SYS_DFLT_DSCP_50_MAPPED_COS_ENABLED             TRUE  /* dscp  50 */
#define SYS_DFLT_DSCP_51_MAPPED_COS_ENABLED             TRUE  /* dscp  51 */
#define SYS_DFLT_DSCP_52_MAPPED_COS_ENABLED             TRUE  /* dscp  52 */
#define SYS_DFLT_DSCP_53_MAPPED_COS_ENABLED             TRUE  /* dscp  53 */
#define SYS_DFLT_DSCP_54_MAPPED_COS_ENABLED             TRUE  /* dscp  54 */
#define SYS_DFLT_DSCP_55_MAPPED_COS_ENABLED             TRUE  /* dscp  55 */
#define SYS_DFLT_DSCP_56_MAPPED_COS_ENABLED             TRUE   /* dscp  56 */
#define SYS_DFLT_DSCP_57_MAPPED_COS_ENABLED             TRUE  /* dscp  57 */
#define SYS_DFLT_DSCP_58_MAPPED_COS_ENABLED             TRUE  /* dscp  58 */
#define SYS_DFLT_DSCP_59_MAPPED_COS_ENABLED             TRUE  /* dscp  59 */
#define SYS_DFLT_DSCP_60_MAPPED_COS_ENABLED             TRUE  /* dscp  60 */
#define SYS_DFLT_DSCP_61_MAPPED_COS_ENABLED             TRUE  /* dscp  61 */
#define SYS_DFLT_DSCP_62_MAPPED_COS_ENABLED             TRUE  /* dscp  62 */
#define SYS_DFLT_DSCP_63_MAPPED_COS_ENABLED             TRUE  /* dscp  63 */


/* MIB object name:     OID: privateMIB.4.1.13.1
 * Possible value: VAL_prioIpPrecDscpStatus_disabled   -- disable both TOS and DSCP mapping
 *                 VAL_prioIpPrecDscpStatus_precedence -- enable TOS mapping, but disable DSCP mapping
 *                 VAL_prioIpPrecDscpStatus_dscp       -- enable DSCP mapping, but disable TOS mapping
 */
#define SYS_DFLT_TOSDSCP_DEFAULT_TABLE_STATUS           VAL_prioIpPrecDscpStatus_disabled


/* MIB object name:     OID: privateMIB.4.1.13.6
 * Possible value: VAL_prioIpPortEnableStatus_disabled   -- disable TcpPort mapping
 *                 VAL_prioIpPortEnableStatus_enabled    -- enable TcpPort mapping
 */
#define SYS_DFLT_TCPPORT_DEFAULT_TABLE_STATUS           VAL_prioIpPortEnableStatus_disabled


/* This following username is for enable command using
 */
#define SYS_DFLT_ENABLE_PASSWORD_USERNAME               "$Enable"


/* The following naming constants define the default values for RADIUS Client
 * It follows the standard MIB of RFC 2618.
 */
#define SYS_DFLT_RADIUS_AUTH_SERVER_ADDRESS                0x0A010001   /* 10.1.0.1 */
#define SYS_DFLT_RADIUS_AUTH_CLIENT_SERVER_PORT_NUMBER     1812         /* UDP port number */
#define SYS_DFLT_RADIUS_AUTH_CLIENT_ACCESS_RETRANSMISSIONS 2            /* 2 times */
#define SYS_DFLT_RADIUS_AUTH_CLIENT_TIMEOUTS               5            /* 5 seconds */

/* netaccess
 */
#define SYS_DFLT_NETACCESS_SECURE_REAUTH_TIME               1800L/* seconds */
#define SYS_DFLT_NETACCESS_DYNAMIC_VLAN_ENABLE              TRUE
#define SYS_DFLT_NETACCESS_DYNAMIC_QOS_ENABLE               FALSE
#define SYS_DFLT_NETACCESS_SECURE_ADDRESSES_PER_PORT        1024/* Accton:no limit,should equal max nbr */
#define SYS_DFLT_NETACCESS_AGING_MODE                       VAL_networkAccessAging_disabled
#define SYS_DFLT_NETACCESS_MACAUTH_SECURE_ADDRESSES_PER_PORT     1024

#define SYS_DFLT_NETACCESS_LINK_DETECTION_STATUS            VAL_networkAccessPortLinkDetectionStatus_disabled
#define SYS_DFLT_NETACCESS_LINK_DETECTION_MODE              VAL_networkAccessPortLinkDetectionMode_linkDown
#define SYS_DFLT_NETACCESS_LINK_DETECTION_ACTION            VAL_networkAccessPortLinkDetectionAciton_trap

/*  Define default configuration value for TACACS */
#define SYS_DFLT_TACACS_AUTH_CLIENT_SERVER_PORT_NUMBER      49
#define SYS_DFLT_TACACS_AUTH_SERVER_ADDRESS                 0x0a010001

/*  Define default configuration value for TACACS
 */
#define SYS_DFLT_TACACS_ACC_CLIENT_SERVER_PORT_NUMBER       49
#define SYS_DFLT_TACACS_AUTH_CLIENT_TIMEOUTS                5

/*If a project doesn't support the TACACS retry/timeout. Set the default value in sys_dflt.h with
 *#define SYS_DFLT_TACACS_AUTH_CLIENT_ACCESS_RETRANSMISSIONS    1
 *#define SYS_DFLT_TACACS_AUTH_CLIENT_TIMEOUTS                  75

 *If a project want to support the TACACS retry/timeout. Set the default value you want. (refer to radius setting)
 *#define SYS_DFLT_TACACS_AUTH_CLIENT_ACCESS_RETRANSMISSIONS    2
 *#define SYS_DFLT_TACACS_AUTH_CLIENT_TIMEOUTS                  5
 */

#define SYS_DFLT_TACACS_AUTH_CLIENT_ACCESS_RETRANSMISSIONS   2            /* 2 times */
#define SYS_DFLT_TACACS_AUTH_CLIENT_TIMEOUTS                 5            /* 5 seconds */


/* The following naming constants define the default values for
 * AAA (Authentication, Authorization, and Accounting
 */
#define SYS_DFLT_AAA_METHOD_LIST_NAME                       "default" /* default method list name */
#define SYS_DFLT_AAA_RADIUS_GROUP_NAME                      "radius" /* default radius group name */
#define SYS_DFLT_AAA_TACACS_PLUS_GROUP_NAME             "tacacs+" /* default tacacs group name */
#define SYS_DFLT_ACCOUNTING_ENABLE                          FALSE /* default accounting status for all l_ports */
#define SYS_DFLT_ACCOUNTING_UPDATE_INTERVAL                 1 /* minute */
#define SYS_DFLT_RADIUS_ACC_CLIENT_SERVER_PORT_NUMBER       1813 /* UDP port number */

/* Define Management IP Filter
 *      SYS_DFLT_MGMT_IP_FLT_STATUS: 1-enabled
 */
#define SYS_DFLT_MGMT_IP_FLT_STATUS                     1

/* The following naming constants define:
 * 1. The ststus of WRR scheduler
 * 2. The default values of weights of WRR queues
 * Notes:
 *    The number of WRR queues is defined in "sys_adpt.h".
 *    The range of weights is defined in private MIB (es3626a.h)
 */

#define SYS_DFLT_WRR_STATUS                             TRUE    /* WRR scheduler enabled */

#define SYS_DFLT_WRR_Q7_WEIGHT      14
#define SYS_DFLT_WRR_Q6_WEIGHT      12
#define SYS_DFLT_WRR_Q5_WEIGHT      10
#define SYS_DFLT_WRR_Q4_WEIGHT      8
#define SYS_DFLT_WRR_Q3_WEIGHT      6
#define SYS_DFLT_WRR_Q2_WEIGHT      4
#define SYS_DFLT_WRR_Q1_WEIGHT      2
#define SYS_DFLT_WRR_Q0_WEIGHT      1

/* 2008-06-02, Jinfeng.Chen:
	The following naming constants define:
	1. The status of DRR scheduler
	2. The default values of weights of DRR queues
	Notes:
		The number of DRR queues is defined in "sys_adpt.h"
 */
#define SYS_DFLT_DRR_STATUS			TRUE
#define SYS_DFLT_DRR_GRANULARITY	2

#define SYS_DFLT_DRR_Q7_WEIGHT		64
#define SYS_DFLT_DRR_Q6_WEIGHT		48
#define SYS_DFLT_DRR_Q5_WEIGHT		32
#define SYS_DFLT_DRR_Q4_WEIGHT		16
#define SYS_DFLT_DRR_Q3_WEIGHT		8
#define SYS_DFLT_DRR_Q2_WEIGHT		4
#define SYS_DFLT_DRR_Q1_WEIGHT		2
#define SYS_DFLT_DRR_Q0_WEIGHT		1

/* The following naming constans define:
 * 1. Threshold of console and telnet
 * 2. inactive time out default value of console and telnet
 * 3. silent time default value of time out
 */
#define SYS_DFLT_SYSMGR_CONSOLE_THRESHOLD_DEF           3       /* 3 times     */
#define SYS_DFLT_SYSMGR_CONSOLE_INACTIVE_TIMEOUT_DEF    600       /* no time out */
#define SYS_DFLT_SYSMGR_SILENT_TIME_DEF                 0       /* no silent time */
#define SYS_DFLT_SYSMGR_TELNET_THRESHOLD_DEF            3       /* 3 times     */
#define SYS_DFLT_SYSMGR_TELNET_INACTIVE_TIMEOUT_DEF     600     /* 600 seconds */

#define SYS_DFLT_SYSMGR_PROMPT_DEF_FOR_BOARD_ID0                      ""
#define SYS_DFLT_SYSMGR_PROMPT_DEF_FOR_BOARD_ID1                      ""
#define SYS_DFLT_SYSMGR_PROMPT_DEF_FOR_BOARD_ID2                      ""
#define SYS_DFLT_SYSMGR_PROMPT_DEF_FOR_BOARD_ID3                      ""
#define SYS_DFLT_SYSMGR_PROMPT_DEF_FOR_BOARD_ID4                      ""
#define SYS_DFLT_SYSMGR_PROMPT_DEF_FOR_BOARD_ID5                      ""
#define SYS_DFLT_SYSMGR_PROMPT_DEF_FOR_BOARD_ID6                      ""
#define SYS_DFLT_SYSMGR_PROMPT_DEF_FOR_BOARD_ID7                      ""

#define SYS_DFLT_SYSMGR_CONSOLE_LOGIN_TIMEOUT           0       /* no time out */
#define SYS_DFLT_SYSMGR_TELNET_LOGIN_TIMEOUT            300     /* 300 seconds */

/* for CPU/memory utilization monitoring
 */
#define SYS_DFLT_SYSMGR_CPU_UTILIZATION_RAISING_THRESHOLD       90
#define SYS_DFLT_SYSMGR_CPU_UTILIZATION_FALLING_THRESHOLD       70
#define SYS_DFLT_SYSMGR_MEMORY_UTILIZATION_RAISING_THRESHOLD    90
#define SYS_DFLT_SYSMGR_MEMORY_UTILIZATION_FALLING_THRESHOLD    70

#define SYS_DFLT_1Q_CONSTRAIN_TYPE                      VAL_dot1qConstraintType_independent
#define SYS_DFLT_VLAN_PORT_MODE                         VAL_vlanPortMode_hybrid

/* Define default configuration value for HTTPS */
#define SYS_DFLT_IP_HTTP_SECURE_PORT                    443
#define SYS_DFLT_SSL_SESSION_CACHE_TIMEOUT              36000
#define SYS_DFLT_IP_SECURE_HTTP_STATE		            VAL_ipHttpsState_enabled
#define SYS_DFLT_IP_REDIRECT_HTTP_STATE                 2L      /* redirect to HTTPS */

/* Define default users for user auth */
/* admin
 */
#define SYS_DFLT_USERAUTH_ADMIN_USERNAME                "admin"
#define SYS_DFLT_USERAUTH_ADMIN_PASSWORD                ""
#define SYS_DFLT_USERAUTH_ADMIN_PRIVILEGE               15

/* manager
 * This is used only if SYS_CPNT_USERAUTH_MANAGER_STYLE is
 * not set to SYS_CPNT_USERAUTH_MANAGER_STYLE_NONE.
 */
#define SYS_DFLT_USERAUTH_MANAGER_USERNAME              "manager"
#define SYS_DFLT_USERAUTH_MANAGER_PASSWORD              "manager"
#define SYS_DFLT_USERAUTH_MANAGER_PRIVILEGE             8

/* guest
 */
#define SYS_DFLT_USERAUTH_GUEST_USERNAME                "guest"
#define SYS_DFLT_USERAUTH_GUEST_PASSWORD                "guest"
#define SYS_DFLT_USERAUTH_GUEST_PRIVILEGE               0

/* define naming constants for default REMOTELOG */
#define SYS_DFLT_REMOTELOG_SERVER_IP_ADDR               0
#define SYS_DFLT_REMOTELOG_FACILITY_TYPE                23
#define SYS_DFLT_REMOTELOG_STATUS                       VAL_remoteLogStatus_disabled
#define SYS_DFLT_REMOTELOG_LEVEL                        7

/* Define default configuration value for SSHD */
#define SYS_DFLT_IP_SSH_PORT                            22
#define SYS_DFLT_IP_SSH_STATE                           2L /*VAL_ipSshdState_disabled*/
#define SYS_DFLT_IP_SSH_AUTHENTICATION_RETRIES          3
#define SYS_DFLT_IP_SSH_TIMEOUT                         120
#define SYS_DFLT_SSH_SERVER_KEY_SIZE                    768
#define SYS_DFLT_PASSWORD_AUTHENTICATION_STATE          1L
#define SYS_DFLT_PUBKEY_AUTHENTICATION_STATE            1L
#define SYS_DFLT_RSA_AUTHENTICATION_STATE               1L


/* for VAL_portType_thousandBaseT
 */
#define SYS_DFLT_FE_COPPER_PORT_FORCE_MODE_SPDDPX       VAL_portSpeedDpxCfg_fullDuplex100

/* for VAL_portType_thousandBaseT
 */
#define SYS_DFLT_GE_COPPER_PORT_FORCE_MODE_SPDDPX       VAL_portSpeedDpxCfg_fullDuplex100

/* for VAL_portType_hundredBaseFX
 *     VAL_portType_hundredBaseFxScSingleMode
 *     VAL_portType_hundredBaseFxScMultiMode
 */
#define SYS_DFLT_FE_FIBER_PORT_FORCE_MODE_SPDDPX        VAL_portSpeedDpxCfg_fullDuplex100

/* for VAL_portType_thousandBaseSX
 *     VAL_portType_thousandBaseLX
 *     VAL_portType_thousandBaseGBIC
 *     VAL_portType_thousandBaseSfp
 */
#define SYS_DFLT_GE_FIBER_PORT_FORCE_MODE_SPDDPX        VAL_portSpeedDpxCfg_fullDuplex1000

/* PORT Shutdown
 * The function is to do port shutdown which it is shutdown of mac or mac-and-phy.
 * SYS_DFLT_DEVSWDRV_DISABLE_PHY_WHEN_PORT_ADMIN_DISABLE : It is defined in sys_dflt.h
 *     TRUE -> The shutdown is shutdown of mac and phy.
 *     FALSE -> The shutdown is only shutdown of mac.
 * SYS_CPNT_SHUTDOWN_ALLOW_IN_TURNK_MEMBER :
 *     TRUE -> Allow the port independently do shutdown when the port is a member of trunk.
 *     FALSE -> Do not allow the port independently do shutdown when the port is a member of trunk.
 */
#define SYS_DFLT_DEVSWDRV_DISABLE_PHY_WHEN_PORT_ADMIN_DISABLE TRUE


/* Those symbol is for rstp using
 */
#define SYS_DFLT_STP_PATH_COST_METHOD                   VAL_xstInstanceCfgPathCostMethod_long

/* SYS_CPNT_STP_TYPE_RSTP : Follow 802.1w
                           We can change SYS_DFLT_STP_PROTOCOL_TYPE to decide default spanning tree mode
                           (stp or rstp)
   SYS_CPNT_STP_TYPE_MSTP: Follow 802.1s
                           We can change SYS_DFLT_STP_PROTOCOL_TYPE to decide default spanning tree mode
                           (stp or rstp or mstp)
*/
#define SYS_DFLT_STP_PROTOCOL_TYPE                      VAL_staProtocolType_rstp
#define SYS_DFLT_STP_PORT_LINK_TYPE_MODE                VAL_staPortAdminPointToPoint_auto
#define SYS_DFLT_STP_CONFIG_REVISION                    0
#define SYS_DFLT_STP_BRIDGE_MAX_HOP                     20
#define SYS_DFLT_STP_BRIDGE_TX_HOLD_COUNT               3
#define SYS_DFLT_STP_MIGRATE_TIME                       3
#define SYS_DFLT_STP_PORT_PROTOCOL_MIGRATION_STATUS     VAL_staPortProtocolMigration_false
#define SYS_DFLT_STP_PORT_ADMIN_EDGE_PORT               VAL_staPortAdminEdgePortWithAuto_auto
#define SYS_DFLT_STP_CONFIG_ID_FORMAT_SELECTOR          0
#define SYS_DFLT_STP_PORT_ROOT_GUARD                    VAL_staPortRootGuardAdminStatus_disabled


#define SYS_DFLT_INGRESS_RATE_LIMIT_STATUS              VAL_rlPortInputStatus_disabled
#define SYS_DFLT_EGRESS_RATE_LIMIT_STATUS               VAL_rlPortOutputStatus_disabled
#define SYS_DFLT_PRIVATE_VLAN_STATUS                    VAL_privateVlanStatus_disabled
#define SYS_DFLT_JUMBO_FRAME_STATUS                     VAL_switchJumboFrameStatus_disabled
#define SYS_DFLT_BSTORM_TYPE                            VAL_bcastStormSampleType_octet_rate
#define SYS_DFLT_MSTORM_TYPE                            VAL_mcastStormSampleType_octet_rate
#define SYS_DFLT_UNKUSTORM_TYPE                         VAL_unknownUcastStormSampleType_octet_rate

#define SYS_DFLT_RATE_LIMIT_DFLT_BURST_KBYTES           16    /* 16 kbytes. To pass jumbo frame, and small burst size make rate limit take affect quickly. */
#define SYS_DFLT_FE_INGRESS_RATE_LIMIT                  SYS_DFLT_BSTORM_RATE_LIMIT
#define SYS_DFLT_GE_INGRESS_RATE_LIMIT                  SYS_DFLT_BSTORM_RATE_LIMIT
#define SYS_DFLT_10GE_INGRESS_RATE_LIMIT                10000000  /* 10000 * 1000kbits  */
#define SYS_DFLT_FE_EGRESS_RATE_LIMIT                   100000    /* 100  * 1000kbits  */
#define SYS_DFLT_GE_EGRESS_RATE_LIMIT                   1000000   /* 1000 * 1000kbits  */
#define SYS_DFLT_10GE_EGRESS_RATE_LIMIT                 10000000  /* 10000 * 1000kbits  */
#define SYS_DFLT_BSTORM_RATE_LIMIT                      64        /* 64 kbits   */
#define SYS_DFLT_MSTORM_RATE_LIMIT                      64        /* 64 kbits   */
#define SYS_DFLT_UNKUSTORM_RATE_LIMIT                   64        /* 64 kbits   */

/* The following naming constants defines the default values
 * of Port Security
 */
/* amtr_port_info[].learn_with_count used this constant to be default vaule.
 * Psec should use this constant instead of "hard code" when PSEC_MGR_GetRunningPortSecurityMacCount().
 * water_huang add ; 94-7-19
 */
#define SYS_DFLT_PORT_SECURITY_MAX_MAC_COUNT			MIN_portSecMaxMacCount
#define SYS_DFLT_PORT_SECURITY_STATUS					VAL_portSecPortStatus_disabled
#define SYS_DFLT_PORT_SECURITY_ACTION_STATUS            VAL_portSecAction_none

/*
 * The following naming constants defines the default values
 * of 802.1X authenticator:
 * SYS_DFLT_EAP_FLOOD_ON_DOT1X_DISABLED -- Support flooding EAP packets when dot1x disabled
 * SYS_DFLT_DOT1X_AUTH_QUIET_PERIOD     -- The initialization value uesed for the quietWhile timer
 *                                          to define periods of time during which it will not attempt
 *                                          to acquire a Supplicant.
 * SYS_DFLT_DOT1X_AUTH_TX_PERIOD        -- The initialization value uesed for the txWhen timer
 *                                          to determine when an EAPOL PDU is to be transmitted.
 * SYS_DFLT_DOT1X_AUTH_SUPP_TIMEOUT     -- The initialization value uesed for the aWhile timer
 *                                          when timing out the Supplicant.
 * SYS_DFLT_DOT1X_AUTH_SERVER_TIMEOUT   -- The initialization value uesed for the aWhile timer
 *                                          when timing out the Authentication Server.
 * SYS_DFLT_DOT1X_AUTH_MAX_REQ          -- The amximum number of times that the state machine will
 *                                          retransmit an EAP Request packet to the Supplicant before
 *                                          it times out the authentication session.
 * SYS_DFLT_DOT1X_AUTH_REAUTH_PERIOD    -- A constant that defines a nonzero number of seconds
 *                                          between periodic reauthentication of the Supplicant.
 * SYS_DFLT_DOT1X_AUTH_CONTROLLED_PORT_CONTROL  -- A constant that defines the port mode of Port.
 *                                          (ForceAuthorized, ForceUnAuthorized, Auto)
 * SYS_DFLT_DOT1X_AUTH_REAUTH_ENABLED   -- A constant that defines whether regular reauthentication
 *                                          will take place on this Port.
 * SYS_DFLT_DOT1X_AUTH_ADMIN_CONTROLLED_DIRECTIONS  -- The current value of the administrative
 *                                          controlled directions parameter for the port
 */
#define SYS_DFLT_EAP_FLOOD_ON_DOT1X_DISABLED            FALSE
#define SYS_DFLT_DOT1X_AUTH_QUIET_PERIOD                60    /* seconds */
#define SYS_DFLT_DOT1X_AUTH_TX_PERIOD                   30    /* seconds */
#define SYS_DFLT_DOT1X_AUTH_SUPP_TIMEOUT                30    /* seconds */
#define SYS_DFLT_DOT1X_AUTH_SERVER_TIMEOUT              30    /* seconds */
#define SYS_DFLT_DOT1X_AUTH_MAX_REAUTH_REQ              2     /* times   */
#define SYS_DFLT_DOT1X_AUTH_MAX_REQ                     2     /* times   */
#define SYS_DFLT_DOT1X_AUTH_REAUTH_PERIOD               3600  /* seconds */
#define SYS_DFLT_DOT1X_AUTH_CONTROLLED_PORT_CONTROL     VAL_dot1xAuthAuthControlledPortControl_forceAuthorized
#define SYS_DFLT_DOT1X_AUTH_REAUTH_ENABLED              VAL_dot1xAuthReAuthEnabled_false
#define SYS_DFLT_DOT1X_AUTH_ADMIN_CONTROLLED_DIRECTIONS VAL_dot1xAuthAdminControlledDirections_both
#define SYS_DFLT_DOT1X_PAE_SYSTEM_AUTH_CONTROL         VAL_dot1xPaeSystemAuthControl_disabled

/* Definition the default baudrate is 115200 */
#define SYS_DFLT_UART_ADMIN_BAUDRATE                    0
#define SYS_DFLT_UART_OPER_BAUDRATE                     115200

/* Define Database Synchronisation file name
 */

/* define naming constants for default SMTP */
#define SYS_DFLT_SMTP_LEVEL                     7
#define SYS_DFLT_SMTP_SOURCE_EMAIL_ADDR         ""
#define SYS_DFLT_SMTP_ADMIN_STATUS              VAL_smtpStatus_enabled
#define SYS_DFLT_SMTP_SERVER_IP_ADDR            0
#define SYS_DFLT_SMTP_DESTINATION_EMAIL_ADDR    ""

/* define DNS default values */
#define SYS_DFLT_DNS_DEFAULT_TIME_OUT           12
#define SYS_DFLT_DNS_DEFAULT_LOCAL_REQUEST      5
#define SYS_DFLT_DNS_DEFAULT_SERVER_REQUEST     10
#define SYS_DFLT_DNS_DEFAULT_CACHE_SIZE         2560
#define SYS_DFLT_IP_DOMAIN_LOOKUP               VAL_dnsDomainLookup_disabled

/* River@May 7, 2008, add nslookup mib */
#define SYS_DFLT_DNS_NSLOOKUP_PTIME             900

/* define LACP default values */
#define SYS_DFLT_LACP_SYSTEM_DEFAULT_PRIORITY   0x8000
#define SYS_DFLT_LACP_PORT_DEFAULT_PRIORITY     0x8000
#define SYS_DFLT_LACP_KEY_NULL                  0
#define SYS_DFLT_LACP_KEY_DEFAULT               1
#define SYS_DFLT_LACP_KEY_10FULL                2
#define SYS_DFLT_LACP_KEY_100FULL               3
#define SYS_DFLT_LACP_KEY_1000FULL              4
#define SYS_DFLT_LACP_KEY_10GFULL               5

/* default LLDP configuration */
#define SYS_DFLT_LLDP_SYS_ADMIN_STATUS          TRUE

/* cpu qeueue mapping
 */
/* 2004-09-09, wuli,
 * CPU interface classification FFP
 * This is the same as 1p priority
 */
/* Wingson 2004-11-12 these priority are only temporary
 */
#define SYS_DFLT_STK_MGMT_PACKET_TO_CPU_PRIORITY          SYS_ADPT_CPU_QUEUE_STKMGMT

/*  2008-11-21, Jinfeng.Chen:
    Used for cpu send packet
 */

#define SYS_DFLT_CPU_SEND_PACKET_PRIORITY                 7

/* use for stacking topology election/discovery  */
#define SYS_DFLT_STK_TPLG_PACKET_TO_CPU_PRIORITY          SYS_ADPT_CPU_QUEUE_STKTPLG

#define SYS_DFLT_IUC_PACKET_TO_CPU_PRIORITY               SYS_ADPT_CPU_QUEUE_IUC/* not used */
#define SYS_DFLT_DOT1X_PACKET_TO_CPU_PRIORITY             SYS_ADPT_CPU_QUEUE_DOT1X/* IFP modify */
#define SYS_DFLT_LLDP_PACKET_TO_CPU_PRIORITY              SYS_ADPT_CPU_QUEUE_LLDP/* not used */
#define SYS_DFLT_LACP_PACKET_TO_CPU_PRIORITY              SYS_ADPT_CPU_QUEUE_LACP/* IFP modify */
#define SYS_DFLT_GVRP_PACKET_TO_CPU_PRIORITY              SYS_ADPT_CPU_QUEUE_GVRP/* IFP modify */
#define SYS_DFLT_BPDU_PACKET_TO_CPU_PRIORITY              SYS_ADPT_CPU_QUEUE_BPDU/* IFP modify */
#define SYS_DFLT_ARP_REPLY_PACKET_TO_CPU_PRIORITY         SYS_ADPT_CPU_QUEUE_ARP_REPLY/* not used */
#define SYS_DFLT_L3_CONTROL_PACKET_TO_CPU_PRIORITY        4/* not used */
#define SYS_DFLT_IGMP_PACKET_TO_CPU_PRIORITY              SYS_ADPT_CPU_QUEUE_IGMP/* IFP modify */
#define SYS_DFLT_CLUSTER_PACKET_TO_CPU_PRIORITY           4/* not used */
#define SYS_DFLT_IPV6_MC_PACKET_TO_CPU_PRIORITY           4/* not used */
#define SYS_DFLT_SWDRV_NOTIFY_PACKET_TO_CPU_PRIORITY      4/* not used */
#define SYS_DFLT_MYMACMYIP_PACKET_TO_CPU_PRIORITY         SYS_ADPT_CPU_QUEUE_MYMAC_MYIP/* IFP modify */
#define SYS_DFLT_DHCP_PACKET_TO_CPU_PRIORITY              SYS_ADPT_CPU_QUEUE_DHCP/* IFP modify */
#define SYS_DFLT_IPV6_UC_PACKET_TO_CPU_PRIORITY           4/* not used */
#define SYS_DFLT_AMTR_PACKET_TO_CPU_PRIORITY              4/* not used */
#define SYS_DFLT_ARP_PACKET_TO_CPU_PRIORITY               SYS_ADPT_CPU_QUEUE_ARP_REQUEST/* IFP modify */
#define SYS_DFLT_MY_MAC_PACKET_TO_CPU_PRIORITY            2/* ?? */
#define SYS_DFLT_UNKNOWN_IPMCAST_PACKET_TO_CPU_PRIORITY   SYS_ADPT_CPU_QUEUE_UNKNOW_IPMC/* not used */
#define SYS_DFLT_UNKNOWN_MCAST_PACKET_TO_CPU_PRIORITY     1/* not used */
#define SYS_DFLT_DEFAULT_PACKET_TO_CPU_PRIORITY           0/* not used */
#define SYS_DFLT_LKUPFAIL_PACKET_TO_CPU_PRIORITY          1/* for port move learning.
By default QoS mapping, 1 is mapped to cpu queue 0;when QoS mapping changed, port move learning will not enter queue 0 */
#define SYS_DFLT_RAPS_PACKET_TO_CPU_PRIORITY              6

#define SYS_DFLT_ARP_BA_RATE                              500
#define SYS_DFLT_ARP_BI_RATE                              300
#define SYS_DFLT_IP_BCAST_BA_RATE                         500
#define SYS_DFLT_IP_BCAST_BI_RATE                         300
#define SYS_DFLT_MY_MAC_BA_RATE                           500
#define SYS_DFLT_MY_MAC_BI_RATE                           300
#define SYS_DFLT_ARP_REPLY_BA_RATE                        500
#define SYS_DFLT_ARP_REPLY_BI_RATE                        300

/* wait for N time to enble trap traffic to CPU */
#define SYS_DFLT_CLASS_G_EVENT_RELEASE_TIME_LIMIT          3

#define SYS_DFLT_LAN_CPU_STORM_RATE_LIMIT                 800
#define SYS_DFLT_PER_PORT_STORM_RATE_LIMIT                500


/*===  Layer 3 system adaptor definition, not including Layer 2 already defined adaptor ===*/
/* Define system default (ip, mask) when the interface do not bind any (ip,mask)
 */
#define SYS_DFLT_DEFAULT_IP                     0x00000000
#define	SYS_DFLT_DEFAULT_MASK					0xff000000

/* Define L3 Egress mode default argument
 * 1 stands for enable and 0 stands for disable
 */
#define SYS_DFLT_L3_EGRESS_MODE                 1

/*---IP group---
 * Define default configuration value for thosed Interface related read-write objects
 * in RFC2011 MIB.
 */
/* Define the ip forwarding
 * L2 ==> VAL_ipForwarding_notForwarding
 * L3 ==> VAL_ipForwarding_forwarding
 */
#define SYS_DFLT_IP_FORWARDING                  VAL_ipForwarding_notForwarding /* layer 3 will be VAL_ipForwarding_forwarding */
#define SYS_DFLT_IP_DEFAULT_TTL                         64

#define SYS_DFLT_DEFAULT_ROUTE_ACTION            SYS_CPNT_DEFAULT_ROUTE_ACTION_DROP  /* drop packet, using AMTRL3 mechanism */


/*   SYS_DFLT_REASM_DEF_LIFE : Reasmbly define life time : 20 min.
 */
#define SYS_DFLT_REASM_DEF_LIFE                         20
/*   SYS_DFLT_DEFAULT_GATEWAY_METRIC : default gateway metric, 5.
 *   because we can set different backup routing. The metric is meanful
 *   when set the route entry.
 */
#define SYS_DFLT_DEFAULT_GATEWAY_METRIC                 1

/*   SYS_DFLT_ARP_DEF_TTL : ARP entry age-out time, 20 min.
 */
#define SYS_DFLT_ARP_DEF_TTL                    20
#define SYS_DFLT_NEIGHBOR_DEF_TTL               20


/*---ARP---
 * Define default configuration value for ARP related read-write objects
 * in arpMgr group of private MIB.
 *
 *  define ARP related system default value.
 *      SYS_DFLT_IP_NET_TO_MEDIA_ENTRY_TIMEOUT (please. Sys_adpt.h)
 *          SYS_ADPT_MAX_ARP_CACHE_TIMEOUT .. SYS_ADPT_MIN_ARP_CACHE_TIMEOUT
 *      SYS_DFLT_ARP_PROXY_ARP_STATUS (ref. leaf_ES3626A.h)
 *          VAL_arpProxyArpStatus_disabled
 *          VAL_arpProxyArpStatus_enabled
 */
#define SYS_DFLT_IP_NET_TO_MEDIA_ENTRY_TIMEOUT  1200
#define SYS_DFLT_ARP_PROXY_ARP_STATUS           FALSE/*Lin.Li, for ARP porting*/

/*---ICMP---
 * Define default configuration value for ICMP Redirect related read-write objects
 * in ipMgr group of private MIB. (ES3626A.h)
 */
#define SYS_DFLT_IP_ICMP_REDIRECT_STATUS        VAL_ipIcmpRedirectStatus_enabled

/*---IRDP (ICMP Router Discover Protocol)---
 * Define default configuration value for IRDP related read-write objects
 * in ipMgr group of private MIB. (leaf_ES3626A.h)
 */
#define SYS_DFLT_IP_IRDP_PROCESS_STATUS              VAL_ipIrdpProcessStatus_disabled
#define SYS_DFLT_IP_IRDP_ADVERTISEMENT_ADDRESS       VAL_ipIrdpAdvertisementAddress_broadcast
#define SYS_DFLT_IP_IRDP_MAX_ADVERTISEMENT_INTERVAL  600
#define SYS_DFLT_IP_IRDP_MIN_ADVERTISEMENT_INTERVAL  (SYS_DFLT_IP_IRDP_MAX_ADVERTISEMENT_INTERVAL *3 /4)
#define SYS_DFLT_IP_IRDP_ADVERTISEMENT_LIFETIME      (3 * SYS_DFLT_IP_IRDP_MAX_ADVERTISEMENT_INTERVAL)
#define SYS_DFLT_IP_IRDP_PREFERENCE_LEVEL            0

/* Default number of ECMP entry per route */
#define SYS_DFLT_ALLOWED_ECMP_NBR_PER_ROUTE     4

/*---RIP---
 * Define default configuration value for RIP related read-write objects
 * in ripMgr group of private MIB. (leaf_ES3626A.h)
 */
#define SYS_DFLT_RIP_UPDATE_TIME                30      /* in seconds */
#define SYS_DFLT_RIP_ROUTING_PROCESS_STATUS     VAL_ripRoutingProcessStatus_disabled
#define SYS_DFLT_RIP_ROUTER_VERSION             VAL_ripRouterVersion_byInterface
#define SYS_DFLT_RIP_POISON_REVERSE_STATUS      VAL_ripSplitHorizonStatus_splitHorizon

/* Define default configuration value for thosed Interface related read-write objects
 * in RFC1724 MIB.
 * Note: 1. The authentication type MD5 is not supported by the device now.
 *       2. If a authentication key string shorter than 16 octets is supplied,
 *          it will be left-justified and padded to 16 octets, on the right, with nulls (0x00).
 */
#define SYS_DFLT_RIP_IF_CONF_DOMAIN_TYPE        '0'
#define SYS_DFLT_RIP_IF_CONF_AUTH_TYPE          VAL_rip2IfConfAuthType_noAuthentication
#define SYS_DFLT_RIP_IF_CONF_AUTH_KEY           "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
#define SYS_DFLT_RIP_IF_CONF_SEND               VAL_rip2IfConfSend_rip1Compatible
#define SYS_DFLT_RIP_IF_CONF_RECEIVE            VAL_rip2IfConfReceive_rip1OrRip2
#define SYS_DFLT_RIP_IF_CONF_DEFAULT_METRIC     5

#define SYS_DFLT_RIP_IMPORT_METRIC              8

#define SYS_DFLT_RIP_STATIC_ROUTE_ADVERTISEMENT 0
/* 0 indicates RIP update packets do not contain Static Route info on the device */
#define SYS_DFLT_RIP_DEFAULT_ROUTE_ADVERTISEMENT 0
/* 0 indicates RIP update packets do not contain default Route info on the devic */

/* ------ OSPF ------
 * Define default configuration value for OSPF related read-write objects
 * in ospfMgr group of private MIB.
 */

/*OSPFNssaTable \ ospfNssaEntry   */
#define SYS_DFLT_OSPF_NSSA_REDISTRIBUTE_STATUS             VAL_ospfNssaRedistributeStatus_enabled
#define SYS_DFLT_OSPF_NSSA_ORIGINATE_DEFAULT_INFO_STATUS   VAL_ospfNssaOriginateDefaultInfoStatus_disabled
#define SYS_DFLT_OSPF_NSSA_STATUS                          L_RSTATUS_NOT_EXIST

/*OSPFRedistributeTable \ ospfRedistributeEntry */
#define SYS_DFLT_OSPF_REDISTRIBUTE_METRIC_TYPE             VAL_ospfRedistributeMetricType_type2
#define SYS_DFLT_OSPF_REDISTRIBUTE_METRIC                  10  // ?
#define SYS_DFLT_OSPF_REDISTRIBUTE_STATUS                  L_RSTATUS_NOT_EXIST


/*OSPFSummaryAddressTable \ OSPF_TYPE_OspfSummaryAddressEntry */
#define SYS_DFLT_OSPF_SUMMARY_ADDRESS_STATUS              L_RSTATUS_NOT_EXIST


/*OSPFNetworkAddressTable \ OSPF_TYPE_OspfNetworkAreaAddressEntry */
#define SYS_DFLT_OSPF_NETWORK_AREA_ID                   0
#define SYS_DFLT_OSPF_NETWORK_AREA_STATUS               L_RSTATUS_NOT_EXIST
#define SYS_DFLT_OSPF_PROC_ID                           1



/*OSPFGeneral Group / OSPFSystem Group  \ private for ospfGeneralGroup */

#define SYS_DFLT_OSPF_ROUTER_ID_TYPE                    VAL_ospfRouterIdType_auto
#define SYS_DFLT_OSPF_RFC1583_COMPATIBLE_STATE          VAL_ospfRfc1583CompatibleState_disabled
#define SYS_DFLT_OSPF_AUTO_COST                         100
#define SYS_DFLT_OSPF_ORIGINATE_DEFAULT_ROUTE           VAL_ospfOriginateDefaultRoute_disabled
#define SYS_DFLT_OSPF_ADVERTISE_DEFAULT_ROUTE           VAL_ospfAdvertiseDefaultRoute_notAlways
#define SYS_DFLT_OSPF_EXTERNAL_METRIC_TYPE              VAL_ospfExternalMetricType_type2
#define SYS_DFLT_OSPF_DEFAULT_EXTERNAL_METRIC           10
#define SYS_DFLT_OSPF_SPF_HOLD_TIME                     10
#define SYS_DFLT_OSPF_SPF_DELAY_TIME                    5
#define SYS_DFLT_OSPF_AREA_NUMBER                       0


/* Define default configuration value for thosed Interface related read-write objects
 * in RFC1850 MIB.
 */
/* OSPFGeneral Group \ ospfGeneralGroup  */
#define SYS_DFLT_OSPF_ADMIN_STAT                    VAL_ospfAdminStat_disabled
#define SYS_DFLT_OSPF_VERSION_NUMBER                VAL_ospfVersionNumber_version2
#define SYS_DFLT_OSPF_AREA_BDR_RTR_STATUS           VAL_ospfAreaBdrRtrStatus_false
#define SYS_DFLT_OSPF_AS_BDR_RTR_STATUS             VAL_ospfASBdrRtrStatus_false
#define SYS_DFLT_OSPF_EXTERN_LSA_COUNT              0
#define SYS_DFLT_OSPF_EXTERN_LSA_CKSUM_SUM          0
#define SYS_DFLT_OSPF_TOS_SUPPORT                   VAL_ospfTOSSupport_false
#define SYS_DFLT_OSPF_ORIGINATE_NEW_LSAS            0
#define SYS_DFLT_OSPF_RX_NEW_LSAS                   0
#define SYS_DFLT_OSPF_EXT_LSDB_LIMIT                MAX_ospfExtLsdbLimit
#define SYS_DFLT_OSPF_MULTICAST_EXTENSIONS          0
#define SYS_DFLT_OSPF_EXIT_OVERFLOW_INTERVAL        0
#define SYS_DFLT_OSPF_DEMAND_EXTENSIONS             VAL_ospfDemandExtensions_false



/* OSPFAreaTable \ ospfAreaEntry         */
#define SYS_DFLT_OSPF_AREA_AUTH_TYPE                0  /* OSPF_TYPE_AUTH_TYPE_NONE */
#define SYS_DFLT_OSPF_AREA_IMPORT_AS_EXTERN         VAL_ospfImportAsExtern_importExternal
#define SYS_DFLT_OSPF_AREA_SPF_RUNS                 0
#define SYS_DFLT_OSPF_AREA_BDR_RTR_COUNT            0
#define SYS_DFLT_OSPF_AREA_ASBDRRTR_COUNT           0
#define SYS_DFLT_OSPF_AREA_LSA_COUNT                0
#define SYS_DFLT_OSPF_AREA_LSA_CKSUM_SUM            0
#define SYS_DFLT_OSPF_AREA_SUMMARY                  VAL_ospfAreaSummary_noAreaSummary
#define SYS_DFLT_OSPF_AREA_STATUS                   L_RSTATUS_NOT_EXIST

/* OSPFStubAreaTable \ ospfStubAreaEntry */
#define SYS_DFLT_OSPF_STUB_METRIC                   1
#define SYS_DFLT_OSPF_STUB_STATUS                   L_RSTATUS_NOT_EXIST
#define SYS_DFLT_OSPF_STUB_METRIC_TYPE              VAL_ospfStubMetricType_ospfMetric
#define SYS_DFLT_OSPF_STUB_TOS                      0


/*OSPFIfTable \ ospfifEntry             */
#define SYS_DFLT_OSPF_IF_AREA_ID                    0
#define SYS_DFLT_OSPF_IF_TYPE                       VAL_ospfIfType_broadcast
#define SYS_DFLT_OSPF_IF_ADMIN_STAT                 VAL_ospfIfAdminStat_enabled
#define SYS_DFLT_OSPF_IF_RTR_PRIORITY               1
#define SYS_DFLT_OSPF_IF_TRANSIT_DELAY              1
#define SYS_DFLT_OSPF_IF_RETRANS_INTERVAL           5
#define SYS_DFLT_OSPF_IF_HELLO_INTERVAL             10
#define SYS_DFLT_OSPF_IF_RTR_DEAD_INTERVAL          40
#define SYS_DFLT_OSPF_IF_POLL_INTERVAL              120
#define SYS_DFLT_OSPF_IF_STATE                      VAL_ospfIfState_down
#define SYS_DFLT_OSPF_IF_DESIGNATED_ROUTER          0
#define SYS_DFLT_OSPF_IF_BACKUP_DESIGNATED_ROUTER   0
#define SYS_DFLT_OSPF_IF_EVENTS                     0
#define SYS_DFLT_OSPF_IF_AUTH_TYPE                  0
#define SYS_DFLT_OSPF_IF_AUTH_KEY                   ""
#define SYS_DFLT_OSPF_IF_AUTH_MD5_KEY               ""
#define SYS_DFLT_OSPF_IF_STATUS                     L_RSTATUS_NOT_EXIST
#define SYS_DFLT_OSPF_IF_MULTICAST_FORWARDING       VAL_ospfIfMulticastForwarding_blocked
#define SYS_DFLT_OSPF_IF_DEMAND                     VAL_ospfIfDemand_false

#define SYS_DFLT_OSPF_IF_AUTH_MD5_KEY_ID            0


/*OSPFIfMetricTable \ ospfifMetricEntry      */
#define SYS_DFLT_OSPF_IFMETRIC_VALUE                1
#define SYS_DFLT_OSPF_IFMETRIC_STATUS               L_RSTATUS_NOT_EXIST


/*OSPFVirtIfTable \ ospfVirtIfEntry          */
#define SYS_DFLT_OSPF_VIRT_IF_TRANSIT_DELAY         1
#define SYS_DFLT_OSPF_VIRT_IF_RETRANS_INTERVAL      5
#define SYS_DFLT_OSPF_VIRT_IF_HELLO_INTERVAL        10
#define SYS_DFLT_OSPF_VIRT_IF_RTR_DEAD_INTERVAL     60 /* RFC 1252,1253 and 1850[OSPF Version 2 Management Information Base] */
#define SYS_DFLT_OSPF_VIRT_IF_STATE                 VAL_ospfVirtIfState_down
#define SYS_DFLT_OSPF_VIRT_IF_EVENTS                0
#define SYS_DFLT_OSPF_VIRT_IF_AUTH_TYPE             0
#define SYS_DFLT_OSPF_VIRT_IF_AUTH_KEY              ""
#define SYS_DFLT_OSPF_VIRT_IF_STATUS                L_RSTATUS_NOT_EXIST


/*OSPFAreaAggregateTable \ ospfAreaAggregateEntry   */
#define SYS_DFLT_OSPF_AREA_AGGREGATE_LSDB_TYPE      VAL_ospfAreaAggregateLsdbType_summaryLink
#define SYS_DFLT_OSPF_AREA_AGGREGATE_STATUS         L_RSTATUS_NOT_EXIST
#define SYS_DFLT_OSPF_AREA_AGGREGATE_EFFECT         VAL_ospfAreaAggregateEffect_advertiseMatching




/* Not Verify Yet */
#define SYS_DFLT_OSPF_AREA_RANGE_EFFECT             VAL_ospfAreaRangeEffect_ospfMetric
#define SYS_DFLT_OSPF_NBR_RTR_ID                                0
#define SYS_DFLT_OSPF_NBR_OPTIONS                               0
#define SYS_DFLT_OSPF_NBR_PRIORITY                              1
#define SYS_DFLT_OSPF_NBR_STATE                                 VAL_ospfNbrState_down
#define SYS_DFLT_OSPF_NBMA_NBR_PERMANENCE                       VAL_ospfNbmaNbrPermanence_permanent

/* OSPF ABR type.  */
#define SYS_DFLT_OSPF_ABR_TYPE_UNKNOWN			0
#define SYS_DFLT_OSPF_ABR_TYPE_STANDARD			1
#define SYS_DFLT_OSPF_ABR_TYPE_CISCO			2
#define SYS_DFLT_OSPF_ABR_TYPE_IBM			3
#define SYS_DFLT_OSPF_ABR_TYPE_SHORTCUT			4
#define SYS_DFLT_OSPF_ABR_TYPE_MAX			5
#define SYS_DFLT_OSPF_ABR_TYPE			SYS_DFLT_OSPF_ABR_TYPE_CISCO

/*Lin.Li add for OSPF*/
#define SYS_DFLT_OSPF_SPF_DELAY_DEFAULT     5
#define SYS_DFLT_OSPF_SPF_HOLDTIME_DEFAULT  10
#define SYS_DFLT_OSPF_STUB_DEFAULT_COST		1
#define SYS_ADPT_OSPF_STUB_DEFAULT_COST_MIN		0
#define SYS_ADPT_OSPF_STUB_DEFAULT_COST_MAX		16777215

/* ------ End of OSPF ------ */


/*---IGMP---
 */

/*---DVMRP---
 * Define DVMRP default configuration value for global based and interface related
 */
#define SYS_DFLT_DVMRP_ADMIN_STATUS             FALSE
#define SYS_DFLT_DVMRP_PROBE_INTERVAL           10      /* seconds */
#define SYS_DFLT_DVMRP_NEIGHBOR_TIMEOUT         35      /* seconds */
#define SYS_DFLT_DVMRP_REPORT_INTERVAL          60      /* seconds */
#define SYS_DFLT_DVMRP_MIN_FLASH_UPDATE_INTERVAL    5   /* seconds */
#define SYS_DFLT_DVMRP_PRUNE_LIFETIME           7200    /* seconds */
#define SYS_DFLT_DVMRP_IF_ADMIN_STATUS          FALSE
#define SYS_DFLT_DVMRP_IF_METRIC                1

/* Define multicast routing configuration */
#define SYS_DFLT_MROUTE_ENABLE_STATUS           VAL_ipMRouteEnable_disabled

/* Define multicast routing configuration */
#define SYS_DFLT_MROUTE_ENABLE_STATUS           VAL_ipMRouteEnable_disabled

/*---PIM---
 * Define PIM default configuration value for global based and interface related
 */
#define SYS_DFLT_PIM_ADMIN_STATUS               FALSE
#define SYS_DFLT_PIM_MODE_DENSE                 1
#define SYS_DFLT_PIM_MODE_VERSION               2
#if 0
#define SYS_DFLT_PIM_HELLO_INTERVAL             30      /* seconds */
#define SYS_DFLT_PIM_HELLO_HOLDTIME             105     /* seconds */
#define SYS_DFLT_PIM_TRIGGER_HELLO_INTERVAL     5       /* seconds */
#define SYS_DFLT_PIM_JOIN_PRUNE_INTERVAL        60      /* seconds */
#define SYS_DFLT_PIM_JOIN_PRUNE_HOLDTIME        210     /* seconds */
#define SYS_DFLT_PIM_PROPAGATIONDELAY           500     /* milliseconds */
#define SYS_DFLT_PIM_DR_PRIORITY                1
#define SYS_DFLT_PIM_GRAFT_RETRY_INTERVAL       3       /* seconds */
#define SYS_DFLT_PIM_MAX_GRAFT_RETRIES          2
#define SYS_DFLT_PIM_STATEREFRESH_ORIGINATIONINTERVAL              60    /* seconds */
#endif
#define SYS_DFLT_PIM_HELLO_INTERVAL             30   /* seconds */
#define SYS_DFLT_PIM_TRIGGERED_HELLO_DELAY      5    /* seconds */
#define SYS_DFLT_PIM_HELLO_HOLD_TIME            105  /* seconds */
#define SYS_DFLT_PIM_OVERRIDE_INTERVAL          2500 /* milliseconds */
#define SYS_DFLT_PIM_PROPAGATION_DELAY          500  /* milliseconds */
#define SYS_DFLT_PIM_JOIN_PRUNE_INTERVAL        60   /* seconds */
#define SYS_DFLT_PIM_JOIN_PRUNE_HOLD_TIME       210  /* seconds */
#define SYS_DFLT_PIM_GRAFT_RETRY_INTERVAL       3    /* seconds */
#define SYS_DFLT_PIM_GRAFT_RETRY_COUNT          3
#define SYS_DFLT_PIM_STATE_REFRESH_INTERVAL     60   /* seconds */
#define SYS_DFLT_PIM_REGISTER_RATE_LIMIT        0    /* no limit */
#define SYS_DFLT_PIM_SWITCHOVER_THRESHOLD       0
#define SYS_DFLT_PIM_DESIGNATED_ROUTER_PRIORITY 1
#define SYS_DFLT_PIM_BSR_HASH_MASK_LEN          1
#define SYS_DFLT_PIM_BSR_PRIORITY               192  /* from cisco */
#define SYS_DFLT_PIM_RP_CANDIDATE_INTERVAL      60   /* seconds, from cisco */
#define SYS_DFLT_PIM_RP_CANDIDATE_PRIORITY      0    /* from cisco */

/*
 *   DHCP client/relay/server default value setting
 *
 */
#define SYS_DFLT_DHCP_CLIENT_PORT               68
#define SYS_DFLT_DHCP_SERVER_PORT               67
#define SYS_DFLT_DHCP_LEASE_TIME                86400
#define SYS_DFLT_DHCP_CID_MODE                  VAL_dhcpPoolOptionCidMode_hex
#define SYS_DFLT_DHCP_NETBIOS_NODE_TYPE         8
#define SYS_DFLT_DHCP_GATEWAY_METRIC            5
#define SYS_DFLT_DHCP_CLASSID_MODE              VAL_dhcpcIfVendorClassIdMode_text


#define SYS_DFLT_DHCP_OPTION82_STATUS           VAL_dhcp_Option82_disabled
#define SYS_DFLT_DHCP_OPTION82_POLICY           VAL_dhcp_Option82_Policy_drop
#define SYS_DFLT_DHCP_OPTION82_RID_MODE         1          /* 2009-05-19, Jimi, default is MAC mode */
#define SYS_DFLT_DHCP_OPTION82_RID_ENCODE_HEX   TRUE       /* Default is encoded hex when user doesn't specify encoded type 
                                                            * Ex: ip dhcp relay information option remote-id mac-address */


/* The Option82 Status of DHCP Snooping, default is disabled
 */
#define SYS_DFLT_DHCPSNP_GLOBAL                     VAL_dhcpSnoopEnable_disabled 
#define SYS_DFLT_DHCPSNP_VLAN                       VAL_dhcpSnoopVlanEnable_disabled
#define SYS_DFLT_DHCPSNP_VERIFY_MAC_ADDRESS         VAL_dhcpSnoopVerifyMacAddressEnable_enabled
#define SYS_DFLT_DHCPSNP_PORT_TRUST_MODE            VAL_dhcpSnoopPortTrustEnable_disabled
#define SYS_DFLT_DHCPSNP_OPTION82_STATUS_DEFAULT    VAL_dhcpSnoopInformationOptionEnable_disabled
#define SYS_DFLT_DHCPSNP_OPTION82_POLICY_DEFAULT    VAL_dhcpSnoopInformationOptionPolicy_replace
#define SYS_DFLT_DHCPSNP_OPTION82_RID_MODE_DEFAULT  1      /* Default is mac-address encoded hex mode, (DHCPSNP_TYPE_OPTION82_RID_MAC_HEX)*/
#define SYS_DFLT_DHCPSNP_OPTION82_CID_MODE_DEFAULT  1      /* Default is received vlan-unit-port mode, (DHCPSNP_TYPE_OPTION82_CID_VLAN_UNIT_PORT)*/
#define SYS_DFLT_DHCPSNP_OPTION82_RID_ENCODE_HEX    TRUE   /* Default is encoded hex when user doesn't specify encoded type 
                                                            * Ex: ip dhcp relay information option remote-id mac-address */


/* ip source guard */
#define SYS_DFLT_IPSRCGUARD_FILTER_MODE             VAL_ipSrcGuardMode_diabled

/* DHCPv6 relay */
#define SYS_DFLT_DHCPv6_CLIENT_PORT             546
#define SYS_DFLT_DHCPv6_SERVER_PORT             547 


/* Defined by Wuli, Checkin by Garfield, 04/09/2004
 * This constant defines the file name for new gererated "start up" configuration file
 * The stack UNIT- MAC information may changed after TCN, CLI needs to re-generate configuration file
 * The new start up file is auto generate by CLI based on new topology and next boot configuration file.
 * after provision complete, and set this file as next boot
 */

#define SYS_DFLT_CLI_AUTO_GENERATE_START_UP_FILENAME "startup1.cfg"


/* HSRP defaults, willy
 */
#define SYS_DFLT_HSRP_CONFIG_TIMEOUT            5        //LEAF_cHsrpConfigTimeout 1
#define SYS_DFLT_HSRP_GRP_PRIORITY              100      //LEAF_cHsrpGrpPriority 3
#define SYS_DFLT_HSRP_GRP_PREEMPT               FALSE    //LEAF_cHsrpGrpPreempt 4
#define SYS_DFLT_HSRP_GRP_PREEMPT_DELAY         0        //LEAF_cHsrpGrpPreemptDelay 5
#define SYS_DFLT_HSRP_GRP_HELLO_TIME            3        //LEAF_cHsrpGrpPriority 7
#define SYS_DFLT_HSRP_GRP_HOLD_TIME             10       //LEAF_cHsrpGrpConfiguredHoldTime 8
#define SYS_DFLT_HSRP_GRP_IFTRACKED_PRIORITY    10       //LEAF_cHsrpExtIfTrackedPriority 2
/* End of HSRP defaults */

/* VRRP defaults, willy
 */
#define SYS_DFLT_VRRP_AUTH_TYPE                 VAL_vrrpOperAuthType_noAuthentication
#define SYS_DFLT_VRRP_PRIORITY                  100 //LEAF_vrrpOperPriority 5
#define SYS_DFLT_VRRP_PREEMPT_MODE              VAL_vrrpOperPreemptMode_true
#define SYS_DFLT_VRRP_PREEMPT_DELAY             0
#define SYS_DFLT_VRRP_ADVER_INTERVAL            1
/* End of VRRP defaults */

/* Zhong Qiyao 2007.07.19:
 * These are for 3Com only and should belong to SYS_ADPT.H
 */
#if 1 // eli 2007.07.26, it may be not need,but it is needed for build.
/* Supported Xenpak Information
 */
#define SYS_DFLT_MODULE_PRODUCT_NUMBER        	"3C17461"
#define SYS_DFLT_XENPAK_PARTNUMBER_LR           "3CXENPAK92"
#define SYS_DFLT_XENPAK_PARTNUMBER_ER           "3CXENPAK96"
#define SYS_DFLT_XENPAK_PARTNUMBER_CX4          "3CXENPAK95"
#endif

/* Voice VLAN defaults (Auto Device Detection)
 */
#define SYS_DFLT_ADD_VOICE_VLAN_ID                  VAL_voiceVlanEnabledId_disabled
#define SYS_DFLT_ADD_VOICE_VLAN_PORT_MODE           VAL_voiceVlanPortMode_none
#define SYS_DFLT_ADD_VOICE_VLAN_PORT_SECURITY_STATE VAL_voiceVlanPortSecurity_disabled

/* Definition for the rule of the voice VLAN.
 * SYS_DFLT_ADD_VOICE_VLAN_PORT_RULE_OUI_STATUS
 *     If enabled, any packet whose OUI is found in the OUI table is considered
 *     as telephony packet.
 * SYS_DFLT_ADD_VOICE_VLAN_PORT_RULE_LLDP_STATUS
 *     If enabled, any LLDP packet with telephone phone bit set is considered
 *     as telephony packet.
 * Note: There shall be at least one port rule specified.
 */
#define SYS_DFLT_ADD_VOICE_VLAN_PORT_RULE_OUI_STATUS    VAL_voiceVlanPortRuleOui_enabled
#define SYS_DFLT_ADD_VOICE_VLAN_PORT_RULE_LLDP_STATUS   VAL_voiceVlanPortRuleLldp_disabled

#define SYS_DFLT_ADD_VOICE_VLAN_PORT_PRIORITY           (6)
#define SYS_DFLT_ADD_VOICE_VLAN_TIMEOUT_MINUTE      (24*60)

/* If set true, avoid deleting the active voice vlan by any method.
 */
#define SYS_DFLT_VLAN_AVOID_DELETING_VOICE_VLAN  TRUE


#define SYS_DFLT_DOT1Q_TUNNEL_STATUS            VAL_vlanDot1qTunnelStatus_disabled
#define SYS_DFLT_DOT1Q_TUNNEL_MODE              VAL_vlanDot1qTunnelPortMode_none
#define SYS_DFLT_DOT1Q_TUNNEL_TPID              0x8100

/* Defines default destination multicast address for PDU tunneling
 * performed by L2PT.
 *
 * 01-00-0c-cd-cd-d0  Generic Bridge PDU Tunneling (GBPT)
 * 01-12-cf-00-00-02  Accton tunnel address
 */
#define SYS_DFLT_L2PT_TUNNEL_DA                 { 0x01, 0x12, 0xcf, 0x00, 0x00, 0x02 }

#define SYS_DFLT_ORG_SPEC1_DA                   { 0x00, 0x12, 0xcf, 0x00, 0x00, 0x01}

/* DA for Accton's UDLD format(, PDU will be redirected to CPU)
 */
#define SYS_DFLT_ORG_SPEC2_DA                   { 0x01, 0x12, 0xcf, 0x00, 0x00, 0x03 }

#define SYS_DFLT_ORG_SPEC3_DA                   { 0x01, 0x12, 0xcf, 0x00, 0x00, 0x04 }

/* for 802.3ah OAM
 */
#define SYS_DFLT_EFM_OAM_LOOPBACK_SENDING_RATE  100     /* 100 packets*/
#define SYS_DFLT_EFM_OAM_LOOPBACK_FRAME_SIZE    64      /* 64 bytes*/
#define SYS_DFLT_EFM_OAM_LOOPBACK_FRAME_NUMBER  10000   /* 10000 packets*/

#define SYS_DFLT_EFM_OAM_MAX_LOGDB_SIZE         100        /* 100 log entries */
#define SYS_DFLT_EFM_OAM_MAX_INDEX              0xffffffff

#define SYS_DFLT_EFM_OAM_ERROR_FRAME_WINDOW              10 /* 10(100m second) = 1 seconds*/
#define SYS_DFLT_EFM_OAM_ERROR_FRAME_THRESHOLD           1
#define SYS_DFLT_EFM_OAM_MAX_FW_UPGRADE_RETRY_TIMES      3
#define SYS_DFLT_EFM_OAM_MAX_FW_UPGRADE_WAIT_TIME        2
#define SYS_DFLT_EFM_OAM_MAX_FW_UPGRADE_RESULT_WAIT_TIME 180 /*3 mins=60*3=180 seconds */


/* Trace Route
 */
#define SYS_DFLT_TRACEROUTE_MAX_TTL             30   /* max hop count allowed */
#define SYS_DFLT_TRACEROUTE_MAX_PROBE           3    /* max probe for each hop */
#define SYS_DFLT_TRACEROUTE_MAX_WAITTIME        3    /* response waiting time: 3 sec */
#define SYS_DFLT_UDP_MAX_LEN                    512  /* UDP receiving buffer size */

/* Define Trace Route used default constants
 */
#define SYS_DFLT_TRACEROUTE_CTL_IP_ADDRESS_TYPE         VAL_traceRouteCtlTargetAddressType_ipv4
#define SYS_DFLT_TRACEROUTE_CTL_BY_PASS_ROUTE_TABLE     VAL_traceRouteCtlByPassRouteTable_false
#define SYS_DFLT_TRACEROUTE_CTL_DATA_SIZE               0
#define SYS_DFLT_TRACEROUTE_CTL_TIME_OUT                (3 * SYS_BLD_TICKS_PER_SECOND)
#define SYS_DFLT_TRACEROUTE_CTL_PROBES_PER_HOP          3
#define SYS_DFLT_TRACEROUTE_CTL_PORT                    33434
#define SYS_DFLT_TRACEROUTE_CTL_MAX_TTL                 30
#define SYS_DFLT_TRACEROUTE_CTL_DS_FIELD                MIN_traceRouteCtlDSField
#define SYS_DFLT_TRACEROUTE_CTL_SOURCE_ADDRESS_TYPE     VAL_traceRouteCtlSourceAddressType_unknown
#define SYS_DFLT_TRACEROUTE_CTL_IFINDEX                 MIN_traceRouteCtlIfIndex
#define SYS_DFLT_TRACEROUTE_CTL_MAX_FAILURE             5
#define SYS_DFLT_TRACEROUTE_CTL_DONT_FRAGMENT           VAL_traceRouteCtlDontFragment_false
#define SYS_DFLT_TRACEROUTE_CTL_INITIAL_TTL             1
#define SYS_DFLT_TRACEROUTE_CTL_FREQUENCY               MIN_traceRouteCtlInitialTtl
#define SYS_DFLT_TRACEROUTE_CTL_STORAGE_TYPE            VAL_traceRouteCtlStorageType_nonVolatile
#define SYS_DFLT_TRACEROUTE_CTL_ADMIN_STATUS            VAL_traceRouteCtlAdminStatus_disabled
#define SYS_DFLT_TRACEROUTE_CTL_MAX_ROWS                90
#define SYS_DFLT_TRACEROUTE_CTL_TRAP_GENERATION         0
#define SYS_DFLT_TRACEROUTE_CTL_CREATE_HOP_ENTRY        VAL_traceRouteCtlCreateHopsEntries_false
/* Define Ping used default constants
 */
#define SYS_DFLT_PING_CTL_IP_ADDRESS_TYPE         VAL_pingCtlTargetAddressType_ipv4
#define SYS_DFLT_PING_CTL_BY_PASS_ROUTE_TABLE     VAL_pingCtlByPassRouteTable_false
#define SYS_DFLT_PING_CTL_DATA_SIZE               LEAF_pingCtlDataSize
/* shumin.wang fix bug ES3628BT-FLF-ZZ-00613 */
#define SYS_DFLT_PING_CTL_TIME_OUT                (3 * SYS_BLD_TICKS_PER_SECOND)
#define SYS_DFLT_PING_CTL_DS_FIELD                MIN_pingCtlDSField
#define SYS_DFLT_PING_CTL_SOURCE_ADDRESS_TYPE     VAL_pingCtlSourceAddressType_unknown
#define SYS_DFLT_PING_CTL_IFINDEX                 MIN_pingCtlIfIndex
#define SYS_DFLT_PING_CTL_FREQUENCY               0 //MIN_pingCtlInitialTtl
#define SYS_DFLT_PING_CTL_STORAGE_TYPE            VAL_pingCtlStorageType_nonVolatile
#define SYS_DFLT_PING_CTL_ADMIN_STATUS            VAL_pingCtlAdminStatus_disabled
#define SYS_DFLT_PING_CTL_MAX_ROWS                90
#define SYS_DFLT_PING_CTL_TRAP_GENERATION         0
#define SYS_DFLT_PING_PRIVATE_MIB_ID              "privateMIB"
#define SYS_DFLT_PING_PRIVATE_MIB_ID_LEN          10            /* Length of SYS_DFLT_PING_PRIVATE_MIB_ID */

/* For SYS_CPNT_RATE_BASED_STORM_CONTROL use
 */
#define SYS_DFLT_RATE_BASED_STORM_CONTROL_RATE	  10000000

/*Lin.Li add*/
#define SYS_DFLT_VR_ID      0
#define SYS_DFLT_VRF_ID     0
#define SYS_DFLT_RIP_DEFAULT_METRIC    1
#define SYS_DFLT_RIP_DEFAULT_ROUTE_ORIGINATE    FALSE
#define SYS_DFLT_RIP_TIMEOUT_TIME    180
#define SYS_DFLT_RIP_GARBAGE_TIME    120
#define SYS_DFLT_RIP_DISTANCE        120
#define SYS_DFLT_RIP_PREFIX_MAX      SYS_ADPT_MAX_NBR_OF_RIP_ROUTE_ENTRY
#define SYS_DFLT_RIP_RECV_BUFF_SIZE  8192
#define SYS_DFLT_RIP_GLOBAL_VERSION                         NETCFG_TYPE_RIP_GLOBAL_VERSION_BY_INTERFACE
#define SYS_DFLT_RIP_AUTH_MODE                              NETCFG_TYPE_RIP_NO_AUTH
#define SYS_DFLT_RIP_SEND_PACKET_VERSION                    NETCFG_TYPE_RIP_VERSION_VERSION_1_COMPATIBLE
#define SYS_DFLT_RIP_RECEIVE_PACKET_VERSION                 NETCFG_TYPE_RIP_VERSION_VERSION_1_AND_2
#define SYS_DFLT_RIP_SPLIT_HORIZON_TYPE                     NETCFG_TYPE_RIP_SPLIT_HORIZON_POISONED

/*Lin.Li add end */
/* DAI */ /* 2007-10, Joseph */
/* This defines default values for DAI.
 */
#define SYS_DFLT_DAI_GLOBAL_STATUS	                VAL_daiGlobalStatus_disabled       /* disabled */
#define SYS_DFLT_DAI_VLAN_STATUS	                VAL_daiVlanStatus_disabled         /* disabled */
#define SYS_DFLT_DAI_LOG_MSG_NBR_PER_INTERVAL       5

#define SYS_DFLT_DAI_ADDITIONAL_CHECK_STATUS        VAL_daiGlobalAdditionalValidStatus_disabled
#define SYS_DFLT_DAI_PORT_TRUST_STATUS              VAL_daiGlobalAdditionalValidStatus_disabled

#define SYS_DFLT_DAI_LOG_MSG_INTERVAL	            1
#define SYS_DFLT_DAI_PORT_RATE_LIMIT	            15      /* pps */

#define SYS_ADPT_DAI_MAX_NBR_OF_LOG_ENTRY	            256
#define SYS_ADPT_DAI_MAX_NBR_OF_LOG_MSG_PER_INTERVAL	SYS_ADPT_DAI_MAX_NBR_OF_LOG_ENTRY
#define SYS_ADPT_DAI_MIN_NBR_OF_LOG_MSG_PER_INTERVAL	0
#define SYS_ADPT_DAI_LOG_MSG_MAX_INTERVAL	            86400
#define SYS_ADPT_DAI_LOG_MSG_MIN_INTERVAL	            0
#define SYS_ADPT_DAI_MAX_PORT_RATE_LIMIT	            2048        /* pps */
#define SYS_ADPT_DAI_MIN_PORT_RATE_LIMIT	            0           /* pps */

#define SYS_DFLT_IPV6_ENABLE            FALSE  // 1: enable, 2: disable, peter, need to defined by mib value
#define SYS_DFLT_IPV6_ADDR_AUTOCONFIG   FALSE  // 1: enable, 2: disable, peter, need to defined by mib value
#define SYS_DFLT_IPV6_INTERFACE_MTU     1500 // peter
#define SYS_DFLT_IPV6_FORWARDING        VAL_ipv6IpForwarding_notForwarding

/*add bys simonm*/
#define SYS_DFLT_ND_DUPLICATE_ADDRESS_DETECTION_ATTEMPTS  3/*rfc 4862: Duplicate Address Detection*/
#define SYS_DFLT_ND_NEIGHBOR_SOLICITATION_RETRANSMISSIONS_INTERVAL 1000 /*command nd ns-interval, "Retrans Timer" in RFC, RTADV_DEFAULT_RETRANS_TIMER	0  in NSM setting*/
#define SYS_DFLT_ND_NEIGHBOR_SOLICITATION_RETRANSMISSIONS_INTERVAL_ADVERTISED 0 /*if 1, RETRANSMISSIONS_INTERVAL are advised, otherwise 0 will be used(no specify)*/
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_HOPLIMIT 255
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_HOPLIMIT_ADVERTISED 0
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_MANAGED_ADDRESS 0
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_OTHER_CONFIG    0
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_REACHABLE_TIME    30000
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_REACHABLE_TIME_ADVERTISED    0
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_SUPPRESS    0
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_TRANSMISSIONS_INTERVAL  600 /*RFC 4861: MaxRtrAdvInterval , must 4~1800sec*/
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_ROUTER_LIFETIME  (3 * SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_TRANSMISSIONS_INTERVAL) /*RFC 4861: AdvDefaultLifetime*/
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_ROUTER_LIFETIME_ADVERTISED 0 /*attention: haven't done yet!*/
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_DEFAULT_ROUTER_PREFERENCE  0 /*NETCFG_TYPE_ND_ROUTER_PERFERENCE_HIGH , expected MEDIUM*/
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_VALID_LIFETIME	2592000 /*RTADV_DEFAULT_VALID_LIFETIME*/
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_PREFERRED_LIFETIME  604800 /*RTADV_DEFAULT_PREFERRED_LIFETIME*/
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_FLAG_ON_LINK   1 /*L bit , default off*/
#define SYS_DFLT_ND_ROUTER_ADVERTISEMENTS_FLAG_AUTO_ADDRESS   1 /*A bit , default off*/

/*end simon*/

#if (SYS_CPNT_CFM == TRUE)
#define SYS_DFLT_CFM_GLOBAL_STATUS           1  /*need private mib define this value, enabled =1, disabled=2*/
#define SYS_DFLT_CFM_PORT_STATUS             1
#define SYS_DFLT_CFM_LINKTRACE_HOLD_TIME         100 /*in minutes*/
#define SYS_DFLT_CFM_LINKTRACE_CACHE_SIZE        100
#define SYS_DFLT_CFM_LINKTRACE_CACHE_STATUS      1  /*need private mib define this value, enabled =1, disabled=2*/

#define SYS_DFLT_CFM_SNMP_TRAP_CC_ALL            FALSE
#define SYS_DFLT_CFM_SNMP_TRAP_CC_MEP_UP         FALSE
#define SYS_DFLT_CFM_SNMP_TRAP_CC_MEP_DOWN       FALSE
#define SYS_DFLT_CFM_SNMP_TRAP_CC_CONFIG         FALSE
#define SYS_DFLT_CFM_SNMP_TRAP_CC_LOOP           FALSE

#define SYS_DFLT_CFM_SNMP_TRAP_CROSS_CHECK_ALL            FALSE
#define SYS_DFLT_CFM_SNMP_TRAP_CROSS_CHECK_UNKNOWN        FALSE
#define SYS_DFLT_CFM_SNMP_TRAP_CROSS_CHECK_MEP_MISSING    FALSE
#define SYS_DFLT_CFM_SNMP_TRAP_CROSS_CHECK_MA_UP          FALSE


#define SYS_DFLT_CFM_FNG_LOWEST_ALARM_PRI            VAL_dot1agCfmMepLowPrDef_macRemErrXcon
#define SYS_DFLT_CFM_FNG_LOWEST_ALARM_TIME           3
#define SYS_DFLT_CFM_FNG_LOWEST_RESET_TIME           10

#define SYS_DFLT_CFM_CCM_STATUS                      VAL_dot1agCfmMepCciEnabled_true
#define SYS_DFLT_CFM_CCM_INTERVAL                    VAL_dot1agCfmMaCcmInterval_interval1s
#define SYS_DFLT_CFM_ARCHIVE_HOLD_TIME               100       /*minitues*/
#define SYS_DFLT_CFM_CROSSCHECK_START_DELAY          10        /*10 sec.*/
#define SYS_DFLT_CFM_CROSSCHECK_STATUS               1         /*enable*/

#define SYS_DFLT_CFM_MIP_CREATE                   VAL_dot1agCfmMaMhfCreation_defMHFdefault
#define SYS_DFLT_CFM_MD_LEVEL                         MIN_dot1agCfmMdMdLevel
#define SYS_DFLT_CFM_MHF_ID_PERMISSION                VAL_dot1agCfmMaIdPermission_sendIdDefer

#define SYS_DFLT_CFM_MEP_ACTIVE                      TRUE
#define SYS_DFLT_CFM_MEP_CCI_STATUS                  VAL_dot1agCfmMepCciEnabled_true
#define SYS_DFLT_CFM_LTM_TTL                         64

#define SYS_DFLT_CFM_AIS_SUPRESS_STATUS   1/*0:CFM_TYPE_AIS_STATUS_ENABLE, 1:CFM_TYPE_AIS_STATUS_DISABLE*/
#define SYS_DFLT_CFM_AIS_STATUS                  0 /*0:CFM_TYPE_AIS_STATUS_ENABLE, 1:CFM_TYPE_AIS_STATUS_DISABLE*/
#define SYS_DFLT_CFM_AIS_LEVEL                     0 /*0 default*/
#define SYS_DFLT_CFM_AIS_PERIOD                  1
#endif
/*below define the MLDSNP default value*/
#define SYS_DFLT_MLDSNP_STATUS                 VAL_mldSnoopStatus_disabled
#define SYS_DFLT_MLDSNP_VERSION                2
#define SYS_DFLT_MLDSNP_QUERIER_STATUS         VAL_mldSnoopQuerier_disabled
#define SYS_DFLT_MLDSNP_ROBUST_VALUE           2
#define SYS_DFLT_MLSDNP_QUERY_INTERVAL         125 /*sec*/
#define SYS_DFLT_MLDSNP_UNKNOWN_MULTICAST_MOD  VAL_mldSnoopUnknownMcastMode_toRouterPort
#define SYS_DFLT_MLDSNP_IMMEIDATE_STATUS       VAL_mldSnoopCurrentVlanImmediateLeave_disabled

/* for Power-save */
#define SYS_DFLT_POWER_SAVE_STATUS             VAL_portPowerSave_enabled

/* for POE */
#define SYS_DFLT_NOTIFICATION_CONTROL                    VAL_pethNotificationControlEnable_true
#define SYS_DFLT_MAIN_PSE_USAGE_THRESHOLD                95 //MAX_pethMainPseUsageThreshold
#define SYS_DFLT_MAIN_PSE_POWER_MAX_ALLOCATION           195
#define SYS_DFLT_PSE_PORT_POWER_MAX_ALLOCATION           15400
#define SYS_DFLT_PSE_PORT_POWER_PRIORITY                 VAL_pethPsePortPowerPriority_low
#define SYS_DFLT_PSE_PORT_POWER_PAIRS_CONTROL_ABILITY    VAL_pethPsePortPowerPairsControlAbility_false
#define SYS_DFLT_PSE_PORT_ADMIN  		         VAL_pethPsePortAdminEnable_true
#define SYS_DFLT_PSE_PORT_POWER_PAIRS                    VAL_pethPsePortPowerPairs_spare
#define SYS_DFLT_PSE_PORT_TYPE                           ""
#define SYS_DFLT_PSE_PORT_DETECTION_STATUS               VAL_pethPsePortDetectionStatus_disabled
#define SYS_DFLT_PSE_PORT_MPSABSENT_COUNT                0
#define SYS_DFLT_PSE_PORT_OVERCURRENT_COUNT              0
#define SYS_DFLT_PSE_PORT_POWER_CLASSIFICATION           VAL_pethPsePortPowerClassifications_class0
#define SYS_DFLT_PSE_PORT_POWER_HIGHPOWER_MODE           0
#define SYS_DFLT_MAIN_PSE_POWER                          195
#define SYS_DFLT_MAIN_PSE_OPER_STATUS                    VAL_pethMainPseOperStatus_off        /* off */
#define SYS_DFLT_MAIN_PSE_CONSUMPTION_POWER              0
#define SYS_DFLT_CAPACITOR_DETECTION                     TRUE


/*default ingress CoS to DSCP mapping*/

#define SYS_DFLT_INGRESS_COS_0_TO_PHB_0        0
#define SYS_DFLT_INGRESS_COS_0_TO_COLOR_0      0

#define SYS_DFLT_INGRESS_COS_0_TO_PHB_1        0
#define SYS_DFLT_INGRESS_COS_0_TO_COLOR_1      0

#define SYS_DFLT_INGRESS_COS_1_TO_PHB_0        1
#define SYS_DFLT_INGRESS_COS_1_TO_COLOR_0      0

#define SYS_DFLT_INGRESS_COS_1_TO_PHB_1        1
#define SYS_DFLT_INGRESS_COS_1_TO_COLOR_1      0

#define SYS_DFLT_INGRESS_COS_2_TO_PHB_0        2
#define SYS_DFLT_INGRESS_COS_2_TO_COLOR_0      0

#define SYS_DFLT_INGRESS_COS_2_TO_PHB_1        2
#define SYS_DFLT_INGRESS_COS_2_TO_COLOR_1      0

#define SYS_DFLT_INGRESS_COS_3_TO_PHB_0        3
#define SYS_DFLT_INGRESS_COS_3_TO_COLOR_0      0

#define SYS_DFLT_INGRESS_COS_3_TO_PHB_1        3
#define SYS_DFLT_INGRESS_COS_3_TO_COLOR_1      0

#define SYS_DFLT_INGRESS_COS_4_TO_PHB_0        4
#define SYS_DFLT_INGRESS_COS_4_TO_COLOR_0      0

#define SYS_DFLT_INGRESS_COS_4_TO_PHB_1        4
#define SYS_DFLT_INGRESS_COS_4_TO_COLOR_1      0

#define SYS_DFLT_INGRESS_COS_5_TO_PHB_0        5
#define SYS_DFLT_INGRESS_COS_5_TO_COLOR_0      0

#define SYS_DFLT_INGRESS_COS_5_TO_PHB_1        5
#define SYS_DFLT_INGRESS_COS_5_TO_COLOR_1      0

#define SYS_DFLT_INGRESS_COS_6_TO_PHB_0        6
#define SYS_DFLT_INGRESS_COS_6_TO_COLOR_0      0

#define SYS_DFLT_INGRESS_COS_6_TO_PHB_1        6
#define SYS_DFLT_INGRESS_COS_6_TO_COLOR_1      0

#define SYS_DFLT_INGRESS_COS_7_TO_PHB_0        7
#define SYS_DFLT_INGRESS_COS_7_TO_COLOR_0      0

#define SYS_DFLT_INGRESS_COS_7_TO_PHB_1        7
#define SYS_DFLT_INGRESS_COS_7_TO_COLOR_1      0

/*default ingress IP precedence to DSCP mapping*/

#define SYS_DFLT_INGRESS_PRE_0_TO_PHB          0
#define SYS_DFLT_INGRESS_PRE_0_TO_COLOR        0

#define SYS_DFLT_INGRESS_PRE_1_TO_PHB          1
#define SYS_DFLT_INGRESS_PRE_1_TO_COLOR        0

#define SYS_DFLT_INGRESS_PRE_2_TO_PHB          2
#define SYS_DFLT_INGRESS_PRE_2_TO_COLOR        0

#define SYS_DFLT_INGRESS_PRE_3_TO_PHB          3
#define SYS_DFLT_INGRESS_PRE_3_TO_COLOR        0

#define SYS_DFLT_INGRESS_PRE_4_TO_PHB          4
#define SYS_DFLT_INGRESS_PRE_4_TO_COLOR        0

#define SYS_DFLT_INGRESS_PRE_5_TO_PHB          5
#define SYS_DFLT_INGRESS_PRE_5_TO_COLOR        0

#define SYS_DFLT_INGRESS_PRE_6_TO_PHB          6
#define SYS_DFLT_INGRESS_PRE_6_TO_COLOR        0

#define SYS_DFLT_INGRESS_PRE_7_TO_PHB          7
#define SYS_DFLT_INGRESS_PRE_7_TO_COLOR        0


/*default ingress DSCP to DSCP mapping*/
#define SYS_DFLT_INGRESS_DSCP_0_TO_PHB         0
#define SYS_DFLT_INGRESS_DSCP_0_TO_COLOR       0
#define SYS_DFLT_INGRESS_DSCP_1_TO_PHB         0
#define SYS_DFLT_INGRESS_DSCP_1_TO_COLOR       1
#define SYS_DFLT_INGRESS_DSCP_2_TO_PHB         0
#define SYS_DFLT_INGRESS_DSCP_2_TO_COLOR       0
#define SYS_DFLT_INGRESS_DSCP_3_TO_PHB         0
#define SYS_DFLT_INGRESS_DSCP_3_TO_COLOR       3
#define SYS_DFLT_INGRESS_DSCP_4_TO_PHB         0
#define SYS_DFLT_INGRESS_DSCP_4_TO_COLOR       0
#define SYS_DFLT_INGRESS_DSCP_5_TO_PHB         0
#define SYS_DFLT_INGRESS_DSCP_5_TO_COLOR       1
#define SYS_DFLT_INGRESS_DSCP_6_TO_PHB         0
#define SYS_DFLT_INGRESS_DSCP_6_TO_COLOR       0
#define SYS_DFLT_INGRESS_DSCP_7_TO_PHB         0
#define SYS_DFLT_INGRESS_DSCP_7_TO_COLOR       3

#define SYS_DFLT_INGRESS_DSCP_8_TO_PHB         1
#define SYS_DFLT_INGRESS_DSCP_8_TO_COLOR       0
#define SYS_DFLT_INGRESS_DSCP_9_TO_PHB         1
#define SYS_DFLT_INGRESS_DSCP_9_TO_COLOR       1
#define SYS_DFLT_INGRESS_DSCP_10_TO_PHB        1
#define SYS_DFLT_INGRESS_DSCP_10_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_11_TO_PHB        1
#define SYS_DFLT_INGRESS_DSCP_11_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_12_TO_PHB        1
#define SYS_DFLT_INGRESS_DSCP_12_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_13_TO_PHB        1
#define SYS_DFLT_INGRESS_DSCP_13_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_14_TO_PHB        1
#define SYS_DFLT_INGRESS_DSCP_14_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_15_TO_PHB        1
#define SYS_DFLT_INGRESS_DSCP_15_TO_COLOR      3

#define SYS_DFLT_INGRESS_DSCP_16_TO_PHB        2
#define SYS_DFLT_INGRESS_DSCP_16_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_17_TO_PHB        2
#define SYS_DFLT_INGRESS_DSCP_17_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_18_TO_PHB        2
#define SYS_DFLT_INGRESS_DSCP_18_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_19_TO_PHB        2
#define SYS_DFLT_INGRESS_DSCP_19_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_20_TO_PHB        2
#define SYS_DFLT_INGRESS_DSCP_20_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_21_TO_PHB        2
#define SYS_DFLT_INGRESS_DSCP_21_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_22_TO_PHB        2
#define SYS_DFLT_INGRESS_DSCP_22_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_23_TO_PHB        2
#define SYS_DFLT_INGRESS_DSCP_23_TO_COLOR      3

#define SYS_DFLT_INGRESS_DSCP_24_TO_PHB        3
#define SYS_DFLT_INGRESS_DSCP_24_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_25_TO_PHB        3
#define SYS_DFLT_INGRESS_DSCP_25_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_26_TO_PHB        3
#define SYS_DFLT_INGRESS_DSCP_26_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_27_TO_PHB        3
#define SYS_DFLT_INGRESS_DSCP_27_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_28_TO_PHB        3
#define SYS_DFLT_INGRESS_DSCP_28_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_29_TO_PHB        3
#define SYS_DFLT_INGRESS_DSCP_29_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_30_TO_PHB        3
#define SYS_DFLT_INGRESS_DSCP_30_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_31_TO_PHB        3
#define SYS_DFLT_INGRESS_DSCP_31_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_32_TO_PHB        4
#define SYS_DFLT_INGRESS_DSCP_32_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_33_TO_PHB        4
#define SYS_DFLT_INGRESS_DSCP_33_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_34_TO_PHB        4
#define SYS_DFLT_INGRESS_DSCP_34_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_35_TO_PHB        4
#define SYS_DFLT_INGRESS_DSCP_35_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_36_TO_PHB        4
#define SYS_DFLT_INGRESS_DSCP_36_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_37_TO_PHB        4
#define SYS_DFLT_INGRESS_DSCP_37_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_38_TO_PHB        4
#define SYS_DFLT_INGRESS_DSCP_38_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_39_TO_PHB        4
#define SYS_DFLT_INGRESS_DSCP_39_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_40_TO_PHB        5
#define SYS_DFLT_INGRESS_DSCP_40_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_41_TO_PHB        5
#define SYS_DFLT_INGRESS_DSCP_41_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_42_TO_PHB        5
#define SYS_DFLT_INGRESS_DSCP_42_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_43_TO_PHB        5
#define SYS_DFLT_INGRESS_DSCP_43_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_44_TO_PHB        5
#define SYS_DFLT_INGRESS_DSCP_44_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_45_TO_PHB        5
#define SYS_DFLT_INGRESS_DSCP_45_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_46_TO_PHB        5
#define SYS_DFLT_INGRESS_DSCP_46_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_47_TO_PHB        5
#define SYS_DFLT_INGRESS_DSCP_47_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_48_TO_PHB        6
#define SYS_DFLT_INGRESS_DSCP_48_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_49_TO_PHB        6
#define SYS_DFLT_INGRESS_DSCP_49_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_50_TO_PHB        6
#define SYS_DFLT_INGRESS_DSCP_50_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_51_TO_PHB        6
#define SYS_DFLT_INGRESS_DSCP_51_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_52_TO_PHB        6
#define SYS_DFLT_INGRESS_DSCP_52_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_53_TO_PHB        6
#define SYS_DFLT_INGRESS_DSCP_53_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_54_TO_PHB        6
#define SYS_DFLT_INGRESS_DSCP_54_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_55_TO_PHB        6
#define SYS_DFLT_INGRESS_DSCP_55_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_56_TO_PHB        7
#define SYS_DFLT_INGRESS_DSCP_56_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_57_TO_PHB        7
#define SYS_DFLT_INGRESS_DSCP_57_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_58_TO_PHB        7
#define SYS_DFLT_INGRESS_DSCP_58_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_59_TO_PHB        7
#define SYS_DFLT_INGRESS_DSCP_59_TO_COLOR      3
#define SYS_DFLT_INGRESS_DSCP_60_TO_PHB        7
#define SYS_DFLT_INGRESS_DSCP_60_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_61_TO_PHB        7
#define SYS_DFLT_INGRESS_DSCP_61_TO_COLOR      1
#define SYS_DFLT_INGRESS_DSCP_62_TO_PHB        7
#define SYS_DFLT_INGRESS_DSCP_62_TO_COLOR      0
#define SYS_DFLT_INGRESS_DSCP_63_TO_PHB        7
#define SYS_DFLT_INGRESS_DSCP_63_TO_COLOR      3

/*default egress DSCP to CoS mapping*/
#define SYS_DFLT_EGRESS_DSCP_0_TO_COS          0
#define SYS_DFLT_EGRESS_DSCP_0_TO_CFI          0
#define SYS_DFLT_EGRESS_DSCP_1_TO_COS          0
#define SYS_DFLT_EGRESS_DSCP_1_TO_CFI          0
#define SYS_DFLT_EGRESS_DSCP_2_TO_COS          0
#define SYS_DFLT_EGRESS_DSCP_2_TO_CFI          0
#define SYS_DFLT_EGRESS_DSCP_3_TO_COS          0
#define SYS_DFLT_EGRESS_DSCP_3_TO_CFI          0
#define SYS_DFLT_EGRESS_DSCP_4_TO_COS          0
#define SYS_DFLT_EGRESS_DSCP_4_TO_CFI          0
#define SYS_DFLT_EGRESS_DSCP_5_TO_COS          0
#define SYS_DFLT_EGRESS_DSCP_5_TO_CFI          0
#define SYS_DFLT_EGRESS_DSCP_6_TO_COS          0
#define SYS_DFLT_EGRESS_DSCP_6_TO_CFI          0
#define SYS_DFLT_EGRESS_DSCP_7_TO_COS          0
#define SYS_DFLT_EGRESS_DSCP_7_TO_CFI          0
#define SYS_DFLT_EGRESS_DSCP_8_TO_COS          1
#define SYS_DFLT_EGRESS_DSCP_8_TO_CFI          0
#define SYS_DFLT_EGRESS_DSCP_9_TO_COS          1
#define SYS_DFLT_EGRESS_DSCP_9_TO_CFI          0
#define SYS_DFLT_EGRESS_DSCP_10_TO_COS         1
#define SYS_DFLT_EGRESS_DSCP_10_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_11_TO_COS         1
#define SYS_DFLT_EGRESS_DSCP_11_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_12_TO_COS         1
#define SYS_DFLT_EGRESS_DSCP_12_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_13_TO_COS         1
#define SYS_DFLT_EGRESS_DSCP_13_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_14_TO_COS         1
#define SYS_DFLT_EGRESS_DSCP_14_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_15_TO_COS         1
#define SYS_DFLT_EGRESS_DSCP_15_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_16_TO_COS         2
#define SYS_DFLT_EGRESS_DSCP_16_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_17_TO_COS         2
#define SYS_DFLT_EGRESS_DSCP_17_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_18_TO_COS         2
#define SYS_DFLT_EGRESS_DSCP_18_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_19_TO_COS         2
#define SYS_DFLT_EGRESS_DSCP_19_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_20_TO_COS         2
#define SYS_DFLT_EGRESS_DSCP_20_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_21_TO_COS         2
#define SYS_DFLT_EGRESS_DSCP_21_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_22_TO_COS         2
#define SYS_DFLT_EGRESS_DSCP_22_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_23_TO_COS         2
#define SYS_DFLT_EGRESS_DSCP_23_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_24_TO_COS         3
#define SYS_DFLT_EGRESS_DSCP_24_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_25_TO_COS         3
#define SYS_DFLT_EGRESS_DSCP_25_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_26_TO_COS         3
#define SYS_DFLT_EGRESS_DSCP_26_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_27_TO_COS         3
#define SYS_DFLT_EGRESS_DSCP_27_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_28_TO_COS         3
#define SYS_DFLT_EGRESS_DSCP_28_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_29_TO_COS         3
#define SYS_DFLT_EGRESS_DSCP_29_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_30_TO_COS         3
#define SYS_DFLT_EGRESS_DSCP_30_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_31_TO_COS         3
#define SYS_DFLT_EGRESS_DSCP_31_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_32_TO_COS         4
#define SYS_DFLT_EGRESS_DSCP_32_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_33_TO_COS         4
#define SYS_DFLT_EGRESS_DSCP_33_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_34_TO_COS         4
#define SYS_DFLT_EGRESS_DSCP_34_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_35_TO_COS         4
#define SYS_DFLT_EGRESS_DSCP_35_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_36_TO_COS         4
#define SYS_DFLT_EGRESS_DSCP_36_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_37_TO_COS         4
#define SYS_DFLT_EGRESS_DSCP_37_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_38_TO_COS         4
#define SYS_DFLT_EGRESS_DSCP_38_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_39_TO_COS         4
#define SYS_DFLT_EGRESS_DSCP_39_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_40_TO_COS         5
#define SYS_DFLT_EGRESS_DSCP_40_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_41_TO_COS         5
#define SYS_DFLT_EGRESS_DSCP_41_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_42_TO_COS         5
#define SYS_DFLT_EGRESS_DSCP_42_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_43_TO_COS         5
#define SYS_DFLT_EGRESS_DSCP_43_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_44_TO_COS         5
#define SYS_DFLT_EGRESS_DSCP_44_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_45_TO_COS         5
#define SYS_DFLT_EGRESS_DSCP_45_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_46_TO_COS         5
#define SYS_DFLT_EGRESS_DSCP_46_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_47_TO_COS         5
#define SYS_DFLT_EGRESS_DSCP_47_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_48_TO_COS         6
#define SYS_DFLT_EGRESS_DSCP_48_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_49_TO_COS         6
#define SYS_DFLT_EGRESS_DSCP_49_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_50_TO_COS         6
#define SYS_DFLT_EGRESS_DSCP_50_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_51_TO_COS         6
#define SYS_DFLT_EGRESS_DSCP_51_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_52_TO_COS         6
#define SYS_DFLT_EGRESS_DSCP_52_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_53_TO_COS         6
#define SYS_DFLT_EGRESS_DSCP_53_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_54_TO_COS         6
#define SYS_DFLT_EGRESS_DSCP_54_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_55_TO_COS         6
#define SYS_DFLT_EGRESS_DSCP_55_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_56_TO_COS         7
#define SYS_DFLT_EGRESS_DSCP_56_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_57_TO_COS         7
#define SYS_DFLT_EGRESS_DSCP_57_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_58_TO_COS         7
#define SYS_DFLT_EGRESS_DSCP_58_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_59_TO_COS         7
#define SYS_DFLT_EGRESS_DSCP_59_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_60_TO_COS         7
#define SYS_DFLT_EGRESS_DSCP_60_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_61_TO_COS         7
#define SYS_DFLT_EGRESS_DSCP_61_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_62_TO_COS         7
#define SYS_DFLT_EGRESS_DSCP_62_TO_CFI         0
#define SYS_DFLT_EGRESS_DSCP_63_TO_COS         7
#define SYS_DFLT_EGRESS_DSCP_63_TO_CFI         0

/*2, 0, 1, 3, 4, 5, 6 ,7 */
/* Priority to queue mapping*/
#define SYS_DFLT_PRI_0_TO_QUEUE       1
#define SYS_DFLT_PRI_1_TO_QUEUE       0
#define SYS_DFLT_PRI_2_TO_QUEUE       0
#define SYS_DFLT_PRI_3_TO_QUEUE       1
#define SYS_DFLT_PRI_4_TO_QUEUE       2
#define SYS_DFLT_PRI_5_TO_QUEUE       2
#define SYS_DFLT_PRI_6_TO_QUEUE       3
#define SYS_DFLT_PRI_7_TO_QUEUE       3

/*default Priority to color mapping*/
#define SYS_DFLT_PRI_0_TO_COLOR      0
#define SYS_DFLT_PRI_1_TO_COLOR      0
#define SYS_DFLT_PRI_2_TO_COLOR      0
#define SYS_DFLT_PRI_3_TO_COLOR      0
#define SYS_DFLT_PRI_4_TO_COLOR      0
#define SYS_DFLT_PRI_5_TO_COLOR      0
#define SYS_DFLT_PRI_6_TO_COLOR      0
#define SYS_DFLT_PRI_7_TO_COLOR      0

/* Define default SFP media type when the transceiver does not
 * contains necessary information and can not identify the type of
 * the transceiver.
 */
#define SYS_DFLT_STKTPLG_MEDIA_TYPE_FOR_UNIDENTIFIABLE_SFP      VAL_portType_hundredBaseFX

/* for G.8032 (ERPS)
 */
#define SYS_DFLT_ERPS_GLOBAL_STATUS     FALSE
#define SYS_DFLT_ERPS_DOMAIN_STATUS     FALSE
#define SYS_DFLT_ERPS_TIMER_WTR         5    /* min  */
#define SYS_DFLT_ERPS_TIMER_GUARD       500  /* ms   */
#define SYS_DFLT_ERPS_TIMER_HOLDOFF     0    /* ms   */
#define SYS_DFLT_ERPS_TIMER_RAPSTX      5000 /* ms   */
#define SYS_DFLT_ERPS_MEG_LEVEL         1

/* For selective QinQ,
 */
#define SYS_DFLT_QINQ_SERVICE_TAG_INFO_SRV_ACTION \
            ( SYS_VAL_vlanDot1qTunnelSrvAction_assignSvid )

#define SYS_DFLT_QINQ_SERVICE_ASSIGN_PRI    0
#define SYS_DFLT_QINQ_SERVICE_ASSIGN_SVID   1
#define SYS_DFLT_QINQ_SERVICE_ASSIGN_CVID   1

#endif
/* End of SYS_DFLT.H */

