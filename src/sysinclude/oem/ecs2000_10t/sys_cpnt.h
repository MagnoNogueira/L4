#ifndef  SYS_CPNT_H
#define  SYS_CPNT_H


#define BCM_SDK_5_5_2

#define UT_SUPPORT_DEVRM_BACKDOOR TRUE
/*----------------------------------------------------------------------------
 * Package: SYS_CPNT.H
 * Purpose: This package defines the support components/Features for the project.
 * Note: The file should only be used by SYS_CPNT_SUPPORT_.c and stkctrl_task.c.
 *  History
 *
 *   Jason Hsue     07/19/2002      new created
 *   Jason Hsue     10/31/2002      1st revised
 *      Modified 802.1X constant definition
 *   Jason Hsue     11/18/2002      2nd revised
 *      1. Add PVLAN relative constants
 *      2. Add traffic segmentation constant
 *      3. Add Web Compression constant
 *      4. Add watch dog timer and Bstorm lport support constants
 *      5. Add MAU MIB and Protocol VLAN constants
 *   Zhong QY       12-08/2003
 *      1. Synchronisation from ES4649-32 (Hagrid)
 *   wuli           06-15/2004
 *      #define SYS_CPNT_DIFFSERV                   TRUE
 *   wuli    07/30/2004
 *     Diffserv and marker is conflict, change old constant to following
 *     to avoid both set to true
 *      #define SYS_CPNT_QOS_NONE     					0
 *      #define SYS_CPNT_QOS_MARKER   					1
 *      #define SYS_CPNT_QOS_DIFFSERV 					2
 *   Zhong QY       04-30/2007
 *      Moved to be based on ACPv3/main/ES4649.
 *
 * ------------------------------------------------------------------------
 * Copyright(C)							  	Accton Technology Corp. , 2007
 * ------------------------------------------------------------------------
 */

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_hwcfg.h"

/* NAMING CONSTANT DECLARATIONS
 */

#define SYS_CPNT_TIME_RANGE          TRUE

/* define runtime openD_CACHE or not?
 * If runtime not support D_CACHE, we will disable D_CACHE after CLI provision complete.
 * By default, BSP will be always enabled D cache, but after provision complete, project leader
 * can make the decision if he wants to enable or disable D cache during run time based on
 * the fact that if D Cache enable will cause system running abnormal
 */
#define SYS_CPNT_RUNTIME_D_CACHE            TRUE

/* Definition watch dog timer is supported
 */
#define SYS_CPNT_WATCHDOG_TIMER             FALSE


/* Define main components this project supports
 * Note: Project Leader needs to review these components supported or not
 *       based on product spec.
 */

/* This defines whether "SYSFUN_Debug_Printf" calls "printf" do display
   debug messages in the local console.
 */
#define SYS_CPNT_PRINTF                     FALSE


/* no need this cpnt
 *#define SYS_CPNT_TWOCHIPSTACKINGINONEBOARD                          TRUE
 */
#define SYS_CPNT_TWOCHIPSTACKINGINONEBOARD_E2EFLOWCONTROL           FALSE

/* Turn into debug mode, will show debug message in console
 * must set to FALSE when release runtime image
 */
#define SYS_CPNT_DEBUG                      FALSE

/* Definition stackable or not */
#define SYS_CPNT_STACKING                   FALSE

#define SYS_CPNT_BCM_LINKSCAN_ENABLE        TRUE
#define SYS_CPNT_BCM_LINKSCAN_UPDATE_TIME          250000

#define SYS_CPNT_REFINE_ISC_MSG             FALSE

/* Added by fen.wang,2008-8-18,when sender get callback so quickly that receiver
 *  cannot process the msg in time,it will cause the msg queue full,and lost msg
 *  so we need to refine it
 */
#define SYS_CPNT_REFINE_IPC_MSG             FALSE

#define SYS_CPNT_STACKING_BUTTON                FALSE /*eric*/
/* TRUE:  Support module hot swap feature
 * FALSE: 1. If module is hot swap inserted, it is ignored and not enabled.
 *        2. If it is hot swap removed, the master console will print a message
 *            and ask user to reboot device. Otherwise it will lead to unexpected behavior.
 */
#define SYS_CPNT_EXPANSION_MODULE_HOT_SWAP  FALSE

/* TRUE:  Support Unit Hot Swap - Resilient Stacking
 * FALSE: Adding or removing units will result in restacking and disturbance in traffic
 */
#define SYS_CPNT_UNIT_HOT_SWAP              TRUE
#define SYS_CPNT_DEBUG_STK_PERFORMANCE             TRUE

#define SYS_CPNT_STKTPLG_SHMEM              TRUE

/* Driver layer */
#define SYS_CPNT_SWDRV                      FALSE
#define SYS_CPNT_NICDRV                     FALSE
#define SYS_CPNT_MGMT_PORT                  FALSE
#define SYS_CPNT_FLASHDRV                   FALSE
#define SYS_CPNT_FLASH_AUTO_DETECTION       FALSE

#if (SYS_CPNT_MGMT_PORT == TRUE)
#define SYS_CPNT_MGMT_PORT_SHOW_IN_WEB      TRUE
#endif

/* Definition for the FLASH memory protection mechanism using the chipset lockbit.
 * SYS_CPNT_FLASH_LOCKBIT_PROTECTION_NONE       : no lockbit protection.
 * SYS_CPNT_FLASH_LOCKBIT_PROTECTION_SYSDATA    : using lockbit to protect the system data (H/W information and loader)
 * SYS_CPNT_FLASH_LOCKBIT_PROTECTION_WHOLE      : using lockbit to protect the whole flash chip.
 */
#define SYS_CPNT_FLASH_LOCKBIT_PROTECTION_NONE          0
#define SYS_CPNT_FLASH_LOCKBIT_PROTECTION_SYSDATA       1
#define SYS_CPNT_FLASH_LOCKBIT_PROTECTION_WHOLE         2

#define SYS_CPNT_FLASH_LOCKBIT_PROTECTION               SYS_CPNT_FLASH_LOCKBIT_PROTECTION_WHOLE

/* For the backward compatibility: if the next-boot runtime image does not support
 * the lockbit protection (i.e., its version is older than the first version defined)
 * then unlock all the lockbit. Zero is assigned to the 1st version if we never roll
 * back by clearing the lockbit for compatibility.
 */
#define SYS_CPNT_FLASH_LOCKBIT_PROTECTION_1ST_VERSION   0x00000000 /* ex.0x02030306 => version no = 2.3.3.6 */


#define SYS_CPNT_LEDDRV                     FALSE
#define SYS_CPNT_VDSLDRV                    FALSE
#define SYS_CPNT_ISCDRV                     FALSE
#define SYS_CPNT_SYSDRV                     FALSE
#define SYS_CPNT_I2CDRV                     FALSE


/* Apparently that a module with CPU or without CPU will have different design.
 * This constant will differentiate the module with or without CPU.
 */
#define SYS_CPNT_MODULE_WITH_CPU            FALSE

/* Layer 3 driver layer features */
#define SYS_CPNT_SWDRVL3                    FALSE

#define SYS_CPNT_SWDRVL4                    FALSE

/* Core layer */
#define SYS_CPNT_SYSCALLBACK                FALSE /* currently, SYSCALLBACK only used in AMTR refinement */
#define SYS_CPNT_STKMGMT                    TRUE
#define SYS_CPNT_CLUSTER                    FALSE  /* temp FALSE for ASF4506B */
#define SYS_CPNT_CFGDB                      FALSE  /*Eli 2007.08.02: will be TRUE   */
#define SYS_CPNT_SWCTRL                     FALSE
#define SYS_CPNT_BRIDGE                     FALSE
#define SYS_CPNT_VLAN                       FALSE
#define SYS_CPNT_PRIVATE_VLAN               FALSE

/* TRUE if support MAC-based VLAN; FALSE if not. */
#define SYS_CPNT_MAC_VLAN                      TRUE
#define SYS_CPNT_MAC_VLAN_WITH_PRIORITY        TRUE & SYS_CPNT_MAC_VLAN
#define SYS_CPNT_MAC_VLAN_IMPLEMENTED_BY_RULE  TRUE & SYS_CPNT_MAC_VLAN

/* TRUE if support IP subnet-based VLAN; FALSE if not. */
#define SYS_CPNT_IP_SUBNET_VLAN                TRUE
/* TRUE if IP subnet-based VLAN can also assign priority */
#define SYS_CPNT_IP_SUBNET_VLAN_WITH_PRIORITY  TRUE

/* VLAN Trunking on a port allows traffic belonging to unknown VLAN groups to
 * pass through this port. This is a useful if you want to set up VLAN groups
 * on end devices without having to configure the same VLAN groups on intermediary
 * devices.
 */
#define SYS_CPNT_VLAN_TRUNKING              TRUE

/* SYS_CPNT_ISOLATED_MGMT_VLAN define the include/exclude for MGMT-VLAN function
 * When the value is FALSE, no management packet filtering mechanism will not be included.
 * When the value is TRUE, management packet will be checked and decide to drop the packet
 * or not. In current stage, only ES4612-20 and ECN330 should set the value to be TRUE
 */
#define SYS_CPNT_ISOLATED_MGMT_VLAN         FALSE

#define SYS_CPNT_GARP                       TRUE  /* temp FALSE for ASF4506B */

/* Definition for the behavior to process the GVRP PDU when GVRP is disabled.
 * SYS_CPNT_GVRP_ACTION_WHEN_GVRP_DISABLED_NO_FLOOD     : discard the received GVRP PDU.
 * SYS_CPNT_GVRP_ACTION_WHEN_GVRP_DISABLED_FLOOD        : flood the GVRP PDU to all the other ports.
 * SYS_CPNT_GVRP_ACTION_WHEN_GVRP_DISABLED_FLOOD_VLAN   : flood the GVRP PDU to the assigned vlan according to the ingress rule.
 */
#define SYS_CPNT_GVRP_ACTION_WHEN_GVRP_DISABLED_NO_FLOOD    0
#define SYS_CPNT_GVRP_ACTION_WHEN_GVRP_DISABLED_FLOOD       1
#define SYS_CPNT_GVRP_ACTION_WHEN_GVRP_DISABLED_FLOOD_VLAN  2
#define SYS_CPNT_GVRP_ACTION_WHEN_GVRP_DISABLED             SYS_CPNT_GVRP_ACTION_WHEN_GVRP_DISABLED_NO_FLOOD


#define SYS_CPNT_L2MCAST                    TRUE

#define SYS_CPNT_SECURITY                   TRUE
#define SYS_CPNT_PORT_SECURITY              TRUE
#define SYS_CPNT_PORT_SECURITY_TRUNK        FALSE


#if (SYS_CPNT_SECURITY == TRUE)
#define SYS_CPNT_MGMT_IP_FLT                FALSE

#define SYS_CPNT_MGMT_IP_FLT_ENABLE_CHECK_FILTER    FALSE

/* MODE_SUBNET:    configure mgmt ip filter with (ip address, subnet mask)
 * MODE_RANGE:     configure mgmt ip filter with (start ip address, end ip address)
 */
#define SYS_CPNT_MGMT_IP_FLT_MODE_SUBNET    1
#define SYS_CPNT_MGMT_IP_FLT_MODE_RANGE     2
#define SYS_CPNT_MGMT_IP_FLT_MODE           SYS_CPNT_MGMT_IP_FLT_MODE_RANGE

/* This defines whether Management Filters for SSH and HTTPS
 * should be separate from Telnet and HTTP.
 *
 * For a default Accton-brand or platform project, it should be FALSE.
 * For some customers (e.g. 3Com), they are separate and this is TRUE.
 * But they have not considered whether SNMPv3 should be separate from SNMP.
 */
#define SYS_CPNT_MGMT_IP_FLT_SSH_HTTPS      FALSE

#define SYS_CPNT_INTRUSION_MSG_TRAP         TRUE
#endif

#define SYS_CPNT_NETWORK                    TRUE
#define SYS_CPNT_MIB2MGMT                   TRUE
#define SYS_CPNT_SYSMGMT                    TRUE
#define SYS_CPNT_SYSLOG                     TRUE
#define SYS_CPNT_UCMGMT                     TRUE
#define SYS_CPNT_COS                        FALSE //TRUE
#define SYS_CPNT_DIFFSERV                   FALSE
#define SYS_CPNT_L2MUX                      TRUE
#define SYS_CPNT_QOS_V2                     TRUE


#define SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL FALSE
#define SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_COS TRUE

/* Bind rule(ACL, policy-map) on egress port (not include trunk port now).
 * If this constant is FALSE, then only support bind rule on ingress port.
 */
#define SYS_CPNT_QOS_V2_EGRESS_PORT                   TRUE

#define PLATFORM_MAX_NBR_OF_ACL_PER_PORT 3

/* Diffserv and marker is conflict, change old constant to following
 * to avoid both set to true
 */
#define SYS_CPNT_QOS_NONE                                       0
#define SYS_CPNT_QOS_MARKER                                     1
#define SYS_CPNT_QOS_DIFFSERV                                   2

#define SYS_CPNT_QOS                                            SYS_CPNT_QOS_DIFFSERV

/* FALSE: shared meters (default)
 *  TRUE: separate (not shared) meters (special chips)
 */
#define SYS_CPNT_DIFFSERV_SEPARATE_METERS                       FALSE

/* Gordon: For Marvell xCat chipsets, the CONFORM ACTION to modify DSCP
 *         is not supported.
 */
#define SYS_CPNT_QOS_DIFFSERV_CONFORM_ACTION_MODIFY_DSCP        TRUE

/* This definition is used for the set action of policy-map's class
 */
#define SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_COS                1   /* BIT-0 */
#define SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_PHB                2   /* BIT-1 */
#define SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_IP_DSCP            4   /* BIT-2 */
#define SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_IP_PRECEDENCE      8   /* BIT-3 */

#define SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_MODE    (SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_COS     | \
                                                        SYS_CPNT_QOS_DIFFSERV_CLASS_SET_ACTION_PHB)
/* SYS_CPNT_QOS_DIFFSERV_POLICE_MODE
 *  - Define bitmap for supportting police mode on DiffServ
 */
#define SYS_CPNT_QOS_DIFFSERV_POLICE_FLOW                    0x01 /* BIT-0 */
#define SYS_CPNT_QOS_DIFFSERV_POLICE_SRTCM_COLOR_BLIND       0x02 /* BIT-1 */
#define SYS_CPNT_QOS_DIFFSERV_POLICE_SRTCM_COLOR_AWARE       0x04 /* BIT-2 */
#define SYS_CPNT_QOS_DIFFSERV_POLICE_TRTCM_COLOR_BLIND       0x08 /* BIT-3 */
#define SYS_CPNT_QOS_DIFFSERV_POLICE_TRTCM_COLOR_AWARE       0x10 /* BIT-4 */
#define SYS_CPNT_QOS_DIFFSERV_POLICE_RATE                    0x20 /* BIT-5 */

#if (SYS_HWCFG_RTK_CHIP_FAMILY == SYS_HWCFG_RTK_CHIP_RTL8380)
#define SYS_CPNT_QOS_DIFFSERV_POLICE_MODE  (SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
#else
#define SYS_CPNT_QOS_DIFFSERV_POLICE_MODE  (SYS_CPNT_QOS_DIFFSERV_POLICE_FLOW |              \
                                            SYS_CPNT_QOS_DIFFSERV_POLICE_SRTCM_COLOR_BLIND | \
                                            SYS_CPNT_QOS_DIFFSERV_POLICE_SRTCM_COLOR_AWARE | \
                                            SYS_CPNT_QOS_DIFFSERV_POLICE_TRTCM_COLOR_BLIND | \
                                            SYS_CPNT_QOS_DIFFSERV_POLICE_TRTCM_COLOR_AWARE)
#endif /* SYS_HWCFG_RTK_CHIP_FAMILY */


/* inter DSCP = inter priority + color (drop precedence)
 */
#define SYS_CPNT_COS_INTER_DSCP                                 TRUE

#define SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS                   (TRUE && !SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_COS_PER_PORT          TRUE /* Now only support per-port */

#define SYS_CPNT_COS_ING_DSCP_TO_COS                            (TRUE  && !SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_DSCP_TO_COS_PER_PORT                   TRUE /* Now only support per-port */

#define SYS_CPNT_COS_ING_IP_PORT_TO_COS                         (TRUE && !SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_IP_PORT_TO_COS_PER_PORT                TRUE /* Now only support per-port */

#define SYS_CPNT_COS_ING_COS_TO_QUEUE                           (TRUE  && !SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_COS_TO_QUEUE_PER_PORT                  TRUE /* Now only support per-port */


/* Ingress CoS map to internal DSCP
 */
#define SYS_CPNT_COS_ING_COS_TO_INTER_DSCP                      (TRUE  && SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_COS_TO_INTER_DSCP_PER_PORT             TRUE /* TRUE: per-port, FALSE: global */

/* Ingress IP precedence to internal DSCP
 */
#define SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP            (FALSE && SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_IP_PRECEDENCE_TO_INTER_DSCP_PER_PORT   TRUE /* TRUE: per-port, FALSE: global */

/* Ingress DSCP map to internal DSCP
 */
#define SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP                     (TRUE  && SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_DSCP_TO_INTER_DSCP_PER_PORT            FALSE /* TRUE: per-port, FALSE: global */

/* Ingress TCP/UDP port to internal DSCP
 */
#define SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP                  (FALSE  && SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_IP_PORT_TO_INTER_DSCP_PER_PORT         TRUE /* Now only support per-port */

/* Internal DSCP map to queue
 */
#define SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE                    (TRUE  && SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_INTER_DSCP_TO_QUEUE_PER_PORT           TRUE /* TRUE: per-port, FALSE: global */

/* Internal DSCP map to cos */
#define SYS_CPNT_COS_ING_INTER_DSCP_TO_COS                      (FALSE && SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_INTER_DSCP_TO_COS_PER_PORT             TRUE /* Now only support per-port */

/* Internal DSCP map to color */
#define SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR                    (TRUE && SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_ING_INTER_DSCP_TO_COLOR_PER_PORT           TRUE /* Now only support per-port */

/* cos - trust mode */
#define SYS_CPNT_COS_TRUST_MODE_SUPPORT_COS                     (TRUE  && SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_TRUST_MODE_SUPPORT_DSCP                    (TRUE  && SYS_CPNT_COS_INTER_DSCP)
#define SYS_CPNT_COS_TRUST_MODE_SUPPORT_PRECEDENCE              (FALSE && SYS_CPNT_COS_INTER_DSCP)



/* support Per Port Per CoS Rate Limit
 */
#define SYS_CPNT_QOS_PER_PORT_PER_DOT1P_PRIORITY_RATE_LIMIT TRUE
#define SYS_CPNT_VLAN_BASED_PRIORITY		FALSE

/* Defines whether the user can configure the 802.1p mapping.
 * This is related to the design of the CPU interface.
 * For XGS, it should be FALSE.
 */
#define SYS_CPNT_1P_USER_CONFIGUREABLE             TRUE


#if (SYS_CPNT_SYSMGMT == TRUE)
#define SYS_CPNT_SYS_MGR                    FALSE
#define SYS_CPNT_SYS_TIME                   FALSE

#define SYS_CPNT_SYSMGMT_MONITORING_PROCESS_CPU             TRUE
#define SYS_CPNT_SYSMGMT_MONITORING_MEMORY_UTILIZATION      TRUE

/* for sysResourceMgt MIB
 * need set SYS_CPNT_SYSMGMT_MONITORING_PROCESS_CPU and
 *          SYS_CPNT_SYSMGMT_MONITORING_MEMORY_UTILIZATION to TRUE
 */
#define SYS_CPNT_SYSMGMT_RESMIB                             TRUE

#define SYS_CPNT_SYSMGMT_DEFERRED_RELOAD    FALSE

#endif

/* This constant is used trun on the other mechanisms when our system know a plugged
 * in SPF transceiver is invalid or faulty:
 * 1) Shutdown this port
 * 2) Do report link up down of this port
 */
#define SYS_CPNT_INVALID_SFP_SHUTDOWN                      FALSE /* Useless */

/* If value is TRUE, indicate to use strong encryption algorithm for SSH and HTTPS.
 * SSH will use DES(56), 3DES(168), Blowfish(128), and AES(128,192,256) algorithms to encrypt/decrypt data.
 * HTTPS will use DES(56), 3DES(168), and RC4(40,56,128) algorithm to encrypt/decrypt data.
 * Otherwise, if value is FALSE, indicate to use weak encryption algorithm for SSH and HTTPS.
 * SSH will use DES(56) algorithms to encrypt/decrypt data.
 * HTTPS will use DES(56) and RC4(40) algorithm to encrypt/decrypt data.
*/
#define SYS_CPNT_SSL_WITH_STRONG_ENCRYPTION TRUE  /* for 128 bits encryption */

#define SYS_CPNT_SSHD                       FALSE
#define SYS_CPNT_SSH2                       TRUE

/* If this is TRUE, TFTP information and relateds will be stored
 * into the config DB, not CLI config file.
 */
#if (SYS_CPNT_CFGDB == TRUE)
#define SYS_CPNT_SSH2_TFTP_INFO_IN_CFGDB    FALSE
#endif /* SYS_CPNT_CFGDB == TRUE */

#define SYS_CPNT_CPU_PORT_RATE_LIMIT        FALSE /*anzhen.zheng, 8/19/2008*/

#define SYS_CPNT_SOFTBRIDGE                 FALSE
#define SYS_CPNT_SNTP                       TRUE
#define SYS_CPNT_NTP                        FALSE
#define SYS_CPNT_INGRESS_RATE_LIMIT         TRUE
#define SYS_CPNT_EGRESS_RATE_LIMIT          TRUE
#define SYS_CPNT_PORT_TRAFFIC_SEGMENTATION  TRUE    /* H/W support, somebody calls it private vlan  */

#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE)
#define SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_SINGLE_SESSION        1
#define SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION      2
#define SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE     SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_SINGLE_SESSION
#endif /* #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE) */

#define SYS_CPNT_SFLOW  TRUE		/*sFlow*/
/* when SYS_CPNT_FE_PORT_GROUP is TRUE
 * it means that all ports of the same chip need to join together.
 * some ASICs, such as marvell DX107 and 88E6095, need to set it to TRUE
 */
#define SYS_CPNT_FE_PORT_GROUP  FALSE


#define SYS_CPNT_STP_TYPE_STA               1
#define SYS_CPNT_STP_TYPE_RSTP              2
#define SYS_CPNT_STP_TYPE_MSTP              3
#define SYS_CPNT_STP                        SYS_CPNT_STP_TYPE_MSTP

#define SYS_CPNT_STP_LOOPBACK_DETECTION     TRUE
#define SYS_CPNT_STP_AUTO_EDGE_PORT         TRUE
#define SYS_CPNT_STP_ROOT_GUARD             TRUE
#define SYS_CPNT_STP_BPDU_GUARD             TRUE
#define SYS_CPNT_STP_BPDU_FILTER            TRUE

/* FALSE: Trunk will NOT inherit the 1st member STP attribute and member port will NOT
 *        inherit the trunk STP attribute too after leave from trunk.
 *        The STP attribute include path cost, fast start state and per port STP state, etc.
 */
#define SYS_CPNT_STP_INHERIT_TRUNK_STP_ATTR TRUE    /* if not defined, assumed TRUE */

#define SYS_CPNT_MAU_MIB                            TRUE
#define SYS_CPNT_PROTOCOL_VLAN                      TRUE
#define SYS_CPNT_PROTOCOL_VLAN_IMPLEMENTED_BY_RULE  TRUE & SYS_CPNT_PROTOCOL_VLAN
/*a port only can specify one group id*/
#define SYS_CPNT_PROTOCOL_VLAN_ONE_PORT_ONE_GROUP FALSE

/* FALSE: Follow 802.1s standard and not change MTSI Message Format in MSTP BPDU. (16 octets)
          SYS_ADPT_MAX_NBR_OF_MST_INSTANCE shall be less than or egual 65.
   TRUE: Change MTSI Message Format in MSTP BPDU to solve MSTI lopping issue.(24 octets)
         Modify msti_bridge_priority to msti_bridge_id. (add 7 octets)
         Modify msti_port_priority to msti_port_id. (add 1 octest)
         SYS_ADPT_MAX_NBR_OF_MST_INSTANCE shall be less than or egual 58.
*/
#define SYS_CPNT_XSTP_PATCH_BPDU_FORMAT     FALSE

/* Activate the feature that user could configure the system flooding behavior and specify per port flooding.
   Behavior references XSTP_TYPE_FLOODING_BEHAVIOR_E.
 */
#define SYS_CPNT_XSTP_CONFIGURABLE_FLOODING  FALSE

/* Definition for indicating the MST members from the assigned vlan members */
#define SYS_CPNT_MST_VLAN_EGRESS_TOPOLOGY                0

/* Definition for indicating the MST members from all the existing ports */
#define SYS_CPNT_MST_FULL_MEMBER_TOPOLOGY                1

/* Definition for indicating the MST members */
#define SYS_CPNT_MST_TOPOLOGY                            SYS_CPNT_MST_VLAN_EGRESS_TOPOLOGY

/* If this is TRUE, default users cannot be deleted.
 */
#define SYS_CPNT_USERAUTH_PROTECT_DEFAULT_USERS FALSE

/* If this is TRUE, default users privilege cannot be modified.
 * The constant shall be FALSE when SYS_CPNT_USERAUTH_PROTECT_DEFAULT_USERS
 * be defined as FALSE.
 */
#define SYS_CPNT_USERAUTH_PROTECT_DEFAULT_USERS_PRIVILEGE       FALSE


/* If this is TRUE, user names and passwords will be stored
 * into the config DB, not CLI config file.
 */
#if (SYS_CPNT_CFGDB == TRUE)
#define SYS_CPNT_USERAUTH_USER_IN_CFGDB     FALSE
#endif /* SYS_CPNT_CFGDB == TRUE */

/* This defines whether a customer-specific implementation of the "manager" (8)
 * user level between "admin" (15) and "guest" (0) is supported.
 * It involves UserAuth, RADIUS, CLI, WEB, SNMP, and maybe others.
 *
 * This must be added to all projects. Otherwise, missing these definitions
 * would result in #if 0 == 0 during comparison, which means always TRUE.
 */
#define SYS_CPNT_USERAUTH_MANAGER_STYLE_NONE  1
#define SYS_CPNT_USERAUTH_MANAGER_STYLE_3COM  2
#define SYS_CPNT_USERAUTH_MANAGER_STYLE     SYS_CPNT_USERAUTH_MANAGER_STYLE_NONE

/* This symbol will be defined to TRUE when HTTPS or SSH support */
#define SYS_CPNT_KEYGEN                     TRUE

#define SYS_CPNT_DNS                        TRUE

/* If this is TRUE, the IP address and relateds will be stored
 * into the config DB, not CLI config file.
 */
#if (SYS_CPNT_CFGDB == TRUE)
#define SYS_CPNT_NETCFG_IP_ADDRESS_IN_CFGDB FALSE
#define SYS_CPNT_CLI_RUNCFG_APPEND_CFGDB_IP FALSE
#endif /* SYS_CPNT_CFGDB == TRUE */


/* Layer 3 Core layer features */
#define SYS_CPNT_AMTRL3                     TRUE
#define SYS_CPNT_ROUTING                    TRUE  /* To distinguish L2 produce from L3 produce. */
#define SYS_CPNT_IPV6                       TRUE
#define SYS_CPNT_INET6                      TRUE

#define SYS_CPNT_IPV4_ROUTING               TRUE  /* Support IPV4 routing or not for L3 produce. */
#if (SYS_CPNT_IPV6 == TRUE)
#define SYS_CPNT_IPV6_ROUTING               TRUE /* Support IPV6 routing or not for L3 produce. */
#endif
#define SYS_CPNT_IP_TUNNEL  TRUE
#define SYS_CPNT_IPAL                   TRUE /*temp add by simon*/

/* application */
#define SYS_CPNT_TRACEROUTE                 TRUE
#define SYS_CPNT_PING                       TRUE
#define SYS_CPNT_NSLOOKUP                   TRUE

/* IML */
#define SYS_CPNT_IML                        TRUE
#define SYS_CPNT_IML_PRIORITY_Q_ENABLED     FALSE
#define SYS_CPNT_ND                         FALSE


#if (SYS_CPNT_ROUTING == TRUE)
#define SYS_CPNT_NSM                        TRUE
#define SYS_CPNT_ARP                        TRUE
#define SYS_CPNT_ICMP                       TRUE
#define SYS_CPNT_RIP                        TRUE

#if (SYS_CPNT_ND == TRUE)
#define SYS_CPNT_RAADV                      TRUE
#endif /*SYS_CPNT_ND*/

/* This constant define the method to determine the metric of a static route
 * that redistributing to RIP. If you can make sure what you should use.
 * Please check with Layer3 team.
 * SYS_CPNT_REDIST_STATIC_ROUTE_TO_RIP_WITH_ORIG_METRIC:
 *     metric = metric of static route + Interface metric
 *     Currently, 3com's product use this spec.
 * SYS_CPNT_REDIST_STATIC_ROUTE_TO_RIP_WITH_REDIST_METRIC:
 *     metric = redist metric that configure by user. No matter what metric of static
 *     route is configured, all static route will be redistributed with this metric.
 *     All other product use this spec.
 *
 */
enum
{
    SYS_CPNT_REDIST_STATIC_ROUTE_TO_RIP_WITH_STATIC_ROUTE_METRIC_PLUS_INTF_METRIC=0,
    SYS_CPNT_REDIST_STATIC_ROUTE_TO_RIP_WITH_REDIST_METRIC
};
#define SYS_CPNT_REDIST_STATIC_ROUTE_TO_RIP_METRIC_METHOD SYS_CPNT_REDIST_STATIC_ROUTE_TO_RIP_WITH_REDIST_METRIC

#define SYS_CPNT_RIP_ENABLE_RIF_BASE        TRUE
#define SYS_CPNT_OSPF                       TRUE
#define SYS_CPNT_OSPF6                      FALSE
#define SYS_CPNT_VRRP                       TRUE
#define SYS_CPNT_ROUTE_OPTIMIZE_NET_ROUTE   TRUE
#define SYS_CPNT_SUPPORT_HW_NETROUTE        TRUE
#define SYS_CPNT_SUPPORTING_MULTICAST_ROUTING          TRUE
/*define IPv6 multicast routing*/
#define SYS_CPNT_SUPPORTING_IPV6_MULTICAST_ROUTING          TRUE

/* Only for Layer3 product.
For some Broadcom's ASIC, routing will work incorrectly when MSTP is enable.
If enable this feature, user can't enable routing and MSTP at same time for accton product.
For 3com product line, when user enable MSTP, routing will be disabled automatically.

Following Broadcom ASIC have this problem.
- 5690/5691 (all revisions)
- 5695/5696 (revision A, fixed in revision B)
                 note: revision B has a different spanning tree errata as documented
- 565x/566x (revision A, fixed in revision B)
- 5673 (all revisions)
                 note: does not apply to 5674*/
#define SYS_CPNT_ROUTING_MUTUAL_EXCLUSIVE_WITH_MSTP     FALSE


#if (SYS_CPNT_SUPPORTING_MULTICAST_ROUTING == TRUE)
#define SYS_CPNT_IGMP                       TRUE
#define SYS_CPNT_DVMRP                      FALSE
#define SYS_CPNT_PIM                        TRUE
#define SYS_CPNT_IPMCLRN                    FALSE

/* Multicast Balance Method on Trunk Port
 * Note: This compile option is used to define balance port to forward of Mcast data packet.
 *
 *      1. SYS_CPNT_IPMCLRN_BALANCE_METHOD_SOFTWARE
 *          -- mcast data will be forwarded to the new balance port which calculated by balancing
 *             algorithm. The balancing algorithm will pick one tunk member according to the SIP and
 *             Group ID as the balance port to transfer the data packet.
 *             Chips like Strata is suggested to use this method.
 *
 *      2. SYS_CPNT_IPMCLRN_BALANCE_METHOD_HARDWARE
 *          -- This option is reserved for the future if the hardware can support trunk port
 *             balance algorithm.
 *
 *      3. SYS_CPNT_IPMCLRN_BALANCE_METHOD_PRIMARY
 *          -- For chip limitation, mcast data can only be forwarded to primary trunk member port.
 *             It will always use the primary trunk member port as the balance port.
 *             Chips like XGS, used in ES4612_CO is suggested to use this method.
 *
 */
/*NAMING IPMC_LRN BALANCE METHODS
*/

#define SYS_CPNT_IPMCLRN_BALANCE_METHOD_SOFTWARE              1
#define SYS_CPNT_IPMCLRN_BALANCE_METHOD_HARDWARE              2
#define SYS_CPNT_IPMCLRN_BALANCE_METHOD_PRIMARY               3
#define SYS_CPNT_IPMCLRN_BALANCE_METHOD     SYS_CPNT_IPMCLRN_BALANCE_METHOD_PRIMARY

/* SYS_CPNT_IPMCLRN_DOWNSTREAM_MULTICAST_ENTRY_SUPPORT is used for workaround for Strata Layer3 chip.
   Bcz we need to add a multicast to block traffic from downstream interface.
   So we only need to set it to TRUE in strata chip

 */
#define SYS_CPNT_IPMCLRN_DOWNSTREAM_MULTICAST_ENTRY_SUPPORT     FALSE

#endif /* (SYS_CPNT_SUPPORTING_MULTICAST_ROUTING == TRUE) */

#define SYS_CPNT_HSRP                       FALSE
#endif /* (SYS_CPNT_ROUTING == TRUE) */


#if (SYS_CPNT_IPV6_ROUTING ==TRUE)
#if (SYS_CPNT_SUPPORTING_IPV6_MULTICAST_ROUTING== TRUE)

/*
 * Define the PIM support or not.
 */
#define SYS_CPNT_PIM6  TRUE
#define SYS_CPNT_PIM6_SM  FALSE
/*
 * Define the MLD support or not.
 */
#define SYS_CPNT_MLD                          TRUE

#endif /*(#if (SYS_CPNT_SUPPORTING_IPV6_MULTICAST_ROUTING== TRUE))*/

#endif /*(#if (SYS_CPNT_IPV6_ROUTING ==TRUE))*/




/* Application layer */
#define SYS_CPNT_LEDMGMT                    TRUE

/* define the SNMP component,RMON component and TRAP compoment:
 *
 * Note that in here, the RMON/TRAP component represents the Epliogue Elite RMON
 * packet and TRAP packet, so this packet is now auto enable/disable by the SNMP version.
 * We only need to take care if we want to enable the SNMP component and the
 * SNMP version, if the SNMP version equal to 1 or 2, it will auto enable the
 * RMON/TRAP component(Epliogue Elite) and if the SNMP version is equal to 3.
 * The SNMPV3 (Net-Snmp) component have included RMON (Net-snmp)/TRAP inside and
 * thus we will auto disable the RMON/TRAP component (Epligogue Elite).
 */
#define SYS_CPNT_SNMP                                   FALSE

#if (SYS_CPNT_SNMP == TRUE)
#define SYS_CPNT_SNMP_VERSION  3

/* To be compatible with current naming convention -
 * SNMP and RMON/TRAP enabled/disabled at the same time.
 */
#if ((SYS_CPNT_SNMP_VERSION == 1) || (SYS_CPNT_SNMP_VERSION == 2))
#define SYS_CPNT_RMON                                   TRUE
#define SYS_CPNT_TRAPMGMT                               TRUE

/* DO NOT mean RMON disabled, but
   leave RMON/TRAP auto enabled at the SNMPv3 component (net-snmp) */
#elif (SYS_CPNT_SNMP_VERSION == 3)
#define SYS_CPNT_RMON                                   FALSE
#define SYS_CPNT_TRAPMGMT                               FALSE

/* Should not happen
 */
#else
#define SYS_CPNT_RMON                                   FALSE
#define SYS_CPNT_TRAPMGMT                               FALSE
#endif

#else
#define SYS_CPNT_RMON                                   FALSE
#define SYS_CPNT_TRAPMGMT                               FALSE
#endif

/* If this is TRUE, SNMP communities will be stored
 * into the config DB, not CLI config file.
 */
#if (SYS_CPNT_CFGDB == TRUE)
#define SYS_CPNT_SNMP_COMMUNITY_IN_CFGDB    FALSE
#endif /* SYS_CPNT_CFGDB == TRUE */

/* This defines whether an SNMP variable of type BITS
 * has the bits counting from the left (highest bit of first
 * byte). According to RFC1906, this should be TRUE. But old
 * products have it reversed (like some big companies). So,
 * for backward compatibility, we use this SYS_CPNT.
 */
#define SYS_CPNT_SNMP_BITS_FROM_LEFT        TRUE

/* This defines whether default groups and users are created in SNMPv3.
 */
#define SYS_CPNT_SNMPV3_DEFAULT_GROUP       FALSE
#define SYS_CPNT_SNMPV3_DEFAULT_USER        FALSE

/* This defines the engine ID style.
 *
 * This must be added to all projects. Otherwise, missing these definitions
 * would result in #if 0 == 0 during comparison, which means always TRUE.
 *
 * SYS_CPNT_SNMPV3_ENGINE_ID_STYLE_ACCTON: Accton and default customers
 * SYS_CPNT_SNMPV3_ENGINE_ID_STYLE_3COM: 3Com only
 */
#define SYS_CPNT_SNMPV3_ENGINE_ID_STYLE_ACCTON 1
#define SYS_CPNT_SNMPV3_ENGINE_ID_STYLE_3COM   2
#define SYS_CPNT_SNMPV3_ENGINE_ID_STYLE SYS_CPNT_SNMPV3_ENGINE_ID_STYLE_ACCTON

/* This defines the style of the target name for SNMPv3.
 *
 * This must be added to all projects. Otherwise, missing these definitions
 * would result in #if 0 == 0 during comparison, which means always TRUE.
 *
 * SYS_CPNT_SNMPV3_TARGET_NAME_STYLE_ACCTON: fixed text.IP address.user name
 * SYS_CPNT_SNMPV3_TARGET_NAME_STYLE_3COM: target name from user
 */
#define SYS_CPNT_SNMPV3_TARGET_NAME_STYLE_ACCTON 1
#define SYS_CPNT_SNMPV3_TARGET_NAME_STYLE_3COM   2
#define SYS_CPNT_SNMPV3_TARGET_NAME_STYLE SYS_CPNT_SNMPV3_TARGET_NAME_STYLE_ACCTON

/* If this is TRUE, default RMON alarm entries will be created.
 */
#define SYS_CPNT_RMON_ALARM_DEFAULT         TRUE /* if not defined, assumed TRUE */

#define SYS_CPNT_WEB                        FALSE

/* Define WEB format is which style
 */
#define SYS_CPNT_WEB_STYLE_ACCTON           1
#define SYS_CPNT_WEB_STYLE_DELL             2
#define SYS_CPNT_WEB_STYLE_3COM             3

/* Define WEB format is Accton style
 */
#define SYS_CPNT_WEB_STYLE                  SYS_CPNT_WEB_STYLE_ACCTON

/* Define if WEB Compression is implemented
 */
#define SYS_CPNT_WEB_COMPRESSION            FALSE

#define SYS_CPNT_WEB_OEM_LOGIN_SCREEN       TRUE

#define SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP  FALSE /*eric es4626F does not have this cpnt*/

#define SYS_CPNT_TELNET                     TRUE

#define SYS_CPNT_XFER                       TRUE
/* This constant defines to support FTP download or not
 */
#define SYS_CPNT_XFER_FTP                   TRUE

/* This constant defines to support loader download or not
 */
#define SYS_CPNT_XFER_DOWNLOAD_LOADER       FALSE

#define SYS_CPNT_DHCP                       TRUE
#define SYS_CPNT_DHCPSNP                    TRUE  /*fuzhimin, 20090505*/
#define SYS_CPNT_DHCPSNP_INFORMATION_OPTION TRUE  /*fuzhimin, 20090505*/
#define SYS_CPNT_IP_SOURCE_GUARD            TRUE
#define SYS_CPNT_IPV6_SOURCE_GUARD          TRUE
#define SYS_CPNT_IPV6_RA_GUARD              TRUE
#define SYS_CPNT_CLI                        TRUE
#define SYS_CPNT_CLI_BANNER                 FALSE
#define SYS_CPNT_HTTP                       TRUE
#define SYS_CPNT_HTTP_DIGEST_AUTHENTICATION TRUE
#define SYS_CPNT_HTTPS                      TRUE

/* This is define WEB Authentication feature. If this is TRUE,
 * webauth can function after enabling it. All hosts in a webauth
 * enabled port will need authentication if they want use internet resource.
 */
#define SYS_CPNT_WEBAUTH                    TRUE

#define SYS_CPNT_TACACS                     TRUE
#define SYS_CPNT_RADIUS                     TRUE
#define SYS_CPNT_DHCPV6                     TRUE

/* Follow cisco's operation of filtering display in terminal and
   some setting in terminal, such as length, width, terminal type,
   history size and escape character.
 */
#define SYS_CPNT_CLI_TERMINAL                   FALSE /*eric*/

#define SYS_CPNT_AAA                                    TRUE
#define SYS_CPNT_RADIUS_AUTHENTICATION                  TRUE
#define SYS_CPNT_RADIUS_ACCOUNTING                      TRUE
#define SYS_CPNT_RADIUS_ACCOUNTING_COMMAND              (FALSE && SYS_CPNT_RADIUS_ACCOUNTING)

/*maggie liu for authorization*/
#define SYS_CPNT_TACACS_PLUS_GRANT_ADMIN_BY_ENABLE          1
#define SYS_CPNT_TACACS_PLUS_GRANT_ADMIN_BY_AUTHORIZATION   2
#define SYS_CPNT_TACACS_PLUS_GRANT_ADMIN_METHOD             SYS_CPNT_TACACS_PLUS_GRANT_ADMIN_BY_AUTHORIZATION

#define SYS_CPNT_TACACS_PLUS_AUTHENTICATION             TRUE
#define SYS_CPNT_TACACS_PLUS_ACCOUNTING                 TRUE
#define SYS_CPNT_TACACS_PLUS_ACCOUNTING_COMMAND         (FALSE && SYS_CPNT_TACACS_PLUS_ACCOUNTING)
#define SYS_CPNT_AUTHENTICATION                         (SYS_CPNT_RADIUS_AUTHENTICATION || SYS_CPNT_TACACS_PLUS_AUTHENTICATION)
#define SYS_CPNT_ACCOUNTING                             (SYS_CPNT_RADIUS_ACCOUNTING || SYS_CPNT_TACACS_PLUS_ACCOUNTING)
#define SYS_CPNT_ACCOUNTING_COMMAND                     (SYS_CPNT_RADIUS_ACCOUNTING_COMMAND || SYS_CPNT_TACACS_PLUS_ACCOUNTING_COMMAND)

/*maggie liu for authorization*/
#define SYS_CPNT_TACACS_PLUS_GRANT_ADMIN_BY_ENABLE          1
#define SYS_CPNT_TACACS_PLUS_GRANT_ADMIN_BY_AUTHORIZATION   2
#define SYS_CPNT_TACACS_PLUS_GRANT_ADMIN_METHOD             SYS_CPNT_TACACS_PLUS_GRANT_ADMIN_BY_AUTHORIZATION

#define SYS_CPNT_TACACS_PLUS_AUTHORIZATION              TRUE
#define SYS_CPNT_AUTHORIZATION                          SYS_CPNT_TACACS_PLUS_AUTHORIZATION
#define SYS_CPNT_TACACS_PLUS_MULTIPLE_SERVER            TRUE

/* If this is TRUE, the RADIUS secret key will be stored
 * into the config DB, not CLI config file.
 */
#if (SYS_CPNT_CFGDB == TRUE)
#define SYS_CPNT_RADIUS_SECRET_KEY_IN_CFGDB FALSE
#endif /* SYS_CPNT_CFGDB == TRUE */

/* These three constants are defined for 802.1X,
 * SYS_CPNT_DOT1X 			    -- if the project want to include 802.1X
 * SYS_CPNT_DOT1X_AUTHENTICATOR	-- 802.1X authenticator is supported or not
 * SYS_CPNT_DOT1X_TRANSPARENT	-- transparent 802.1X is supported or not
 * SYS_CPNT_DOT1X_OPERATION_MODE-- Identify that 802.1X is supported with transparent or
 *                                  authenticator mode.
 */
#define SYS_CPNT_DOT1X_AUTHENTICATED        1
#define SYS_CPNT_DOT1X_TRANSPARENT          2

#define SYS_CPNT_DOT1X                      TRUE
#define SYS_CPNT_DOT1X_OPERATION_MODE       SYS_CPNT_DOT1X_AUTHENTICATED

/* This defines whether in 802.1X, three encryption methods
 * TTLS, TLS, and PEAP are supports. Old products and products
 * running on smaller memory size may not support these, but
 * the ACP_V3 platform products and products with 128MB of SDRAM are
 * recommended to support these.
 */
#define SYS_CPNT_DOT1X_TTLS_TLS_PEAP        TRUE


/* If true, 802.1X will support MAC-based authentication.
 * Else, 802.1X doesn't support MAC-based authentication.
 */
#define SYS_CPNT_DOT1X_MACBASED_AUTH            TRUE

/* This defines whether support EAPOL frames pass-through.
 * If this is TRUE, 802.1X support EAPOL frames pass-through by
 * user configration.
 * Else, does not support EAPOL frames pass-through.
 */
#define SYS_CPNT_DOT1X_EAPOL_PASS_THROUGH         TRUE
#define SYS_CPNT_DOT1X_EAPOL_PASS_THROUGH_BY_RULE TRUE
/* This defines whether support 802.1X supplicant.
 */
#define SYS_CPNT_DOT1X_SUPPLICANT           FALSE

#define SYS_CPNT_NETWORK_ACCESS                 FALSE

#define SYS_CPNT_NETACCESS                      TRUE
#define SYS_CPNT_NETACCESS_GUEST_VLAN           TRUE
#define SYS_CPNT_NETACCESS_MACAUTH              TRUE
#define SYS_CPNT_NETACCESS_DYNAMIC_VLAN         TRUE
#define SYS_CPNT_NETACCESS_DYNAMIC_QOS          TRUE
#define SYS_CPNT_NETACCESS_MAC_FILTER_TABLE     TRUE
#define SYS_CPNT_NETACCESS_LINK_DETECTION       TRUE

/* This defines the aging attribute of the secure MAC address.
 * SYS_CPNT_NETACCESS_AGING_MODE_STATIC, the secure MAC address shall not be aged out.
 * SYS_CPNT_NETACCESS_AGING_MODE_DYNAMIC, the secure MAC address shall be aged out.
 * SYS_CPNT_NETACCESS_AGING_MODE_CONFIGURABLE, decide by user.
 *
 * This must be added to all projects. Otherwise, missing these definitions
 * would result in #if 0 == 0 during comparison, which means always TRUE.
 */
#define SYS_CPNT_NETACCESS_AGING_MODE_STATIC            1
#define SYS_CPNT_NETACCESS_AGING_MODE_DYNAMIC           2
#define SYS_CPNT_NETACCESS_AGING_MODE_CONFIGURABLE      3  /* user can decide static or dynamic via UI */

#define SYS_CPNT_NETACCESS_AGING_MODE                   SYS_CPNT_NETACCESS_AGING_MODE_CONFIGURABLE   /* one of the above three MAC MODE */

/* Define if this is a VDSL product
 */
#define SYS_CPNT_VDSL                       FALSE

/* Special component for root.c to call */
#define SYS_CPNT_PFU                        FALSE       /* Packet Filtering Unit */


/* Define sub components this project supports
 * Note: Project Leader needs to review these components supported or not
 *       based on product spec.
 */
#if (SYS_CPNT_SWCTRL == TRUE)
#define SYS_CPNT_AMTR                       FALSE
#define SYS_CPNT_NMTR                       FALSE
#define SYS_CPNT_LACP                       FALSE
#define SYS_CPNT_SWCTRL_CABLE_DIAG          FALSE   /* TRUE to support cable diagnostics */
#if (SYS_CPNT_SWCTRL_CABLE_DIAG == TRUE)
#define SYS_CPNT_SWCTRL_CABLE_DIAG_MARVELL    1
#define SYS_CPNT_SWCTRL_CABLE_DIAG_BROADCOM   2
#define SYS_CPNT_SWCTRL_CABLE_DIAG_CHIP SYS_CPNT_SWCTRL_CABLE_DIAG_BROADCOM
#endif
#endif

/* Define the LLDP component
 */
#define SYS_CPNT_LLDP                       FALSE /* temp FALSE for ASF4506B */

#if (SYS_CPNT_LLDP == TRUE)
#define SYS_CPNT_LLDP_EXT                   TRUE
#define SYS_CPNT_LLDP_MED                   FALSE
#endif


#if (SYS_CPNT_SWDRV == TRUE)
#define SYS_CPNT_AMTRDRV                    FALSE
#define SYS_CPNT_NMTRDRV                    FALSE
#define SYS_CPNT_SWDRV_CACHE                FALSE /*eli mark,will be TRUE 20070723 */
#define SYS_CPNT_SWDRVL3_CACHE              FALSE
#endif

/* PORT Shutdown
 * The function is to do port shutdown which it is shutdown of mac or mac-and-phy.
 * SYS_DFLT_DEVSWDRV_DISABLE_PHY_WHEN_PORT_ADMIN_DISABLE : It is defined in sys_dflt.h
 *     TRUE -> The shutdown is shutdown of mac and phy.
 *     FALSE -> The shutdown is only shutdown of mac.
 * SYS_CPNT_SHUTDOWN_ALLOW_IN_TURNK_MEMBER :
 *     TRUE -> Allow the port independently do shutdown when the port is a member of trunk.
 *     FALSE -> Do not allow the port independently do shutdown when the port is a member of trunk.
 */
#define SYS_CPNT_SHUTDOWN_ALLOW_IN_TURNK_MEMBER     FALSE


#if (SYS_CPNT_SYSLOG == TRUE)
#define SYS_CPNT_SYSLOG_BACKUP              TRUE
#define SYS_CPNT_REMOTELOG                  TRUE
#define SYS_CPNT_SMTP                       TRUE /* temp FALSE for ASF4506B */
#endif

/* When this define is set to true, only the specified protocol is allowed to access the switch.
 * eg: management telnet 1.1.1.1
       then user 1.1.1.1 can access the switch by telnet only,
       not by web or snmp.
   when this define is set to false, user (1.1.1.1) can still get access to switch via web/snmp.
   but on telnet, only this user (1.1.1.1) gets access, no one else is allowed to.
 */
#define SYS_CPNT_MANAGEMENT_IP_FILTER_DEFAULT_DENY  FALSE


#if (SYS_CPNT_PRIVATE_VLAN == TRUE)
/* Community private vlan can not be set TRUE if SYS_CPNT_PORT_TRAFFIC_SEGMENTATION
 * is true and vice versa.  You can say that one is S/W PVLAN, and the other is
 * H/W ASIC PVLAN.
 */
#define SYS_CPNT_COMMUNITY_PRIVATE_VLAN         TRUE
#define SYS_CPNT_ISOLATED_PRIVATE_VLAN          FALSE
#define SYS_CPNT_2WAY_COMMUNITY_PRIVATE_VLAN    FALSE
#endif

#define SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_WITHOUT_FFP  TRUE

#if (SYS_CPNT_DHCP == TRUE)
#define SYS_CPNT_DHCP_CLIENT                TRUE
#define SYS_CPNT_DHCP_RELAY                 TRUE
#define SYS_CPNT_DHCP_SERVER                TRUE
#endif

#if (SYS_CPNT_DHCPV6 == TRUE)
#define SYS_CPNT_DHCPV6_RELAY               TRUE
#endif

/* Following is for feature definition of project
 */


/* Naming constant to indicate if ASIC doesn't support per port configuration.
 * Value TRUE  -- support port based priority management
 *       FALSE -- not support
 * 802.1p will not support per port configuration if ASIC not support
 */
#define SYS_CPNT_PRI_MGMT_PORT_BASE         FALSE


/* For COS setting
 */
#define SYS_CPNT_STACK_COS_BY_DEFAULT       TRUE
#define SYS_CPNT_COS_PER_PORT               FALSE

/* For Priority Management
 * Naming constant to indicate if ASIC doesn't support per port configuration.
 * 802.1p will not support per port configuration if ASIC not support
 */
#define SYS_CPNT_PRI_MGMT_PER_PORT          FALSE


/* Define CLI displaying features
 * Note: The following is for CLI to decide what information to display for command:
 *       show version
 */
#define SYS_CPNT_CLI_SHOW_SERVICE_TAG       FALSE
#define SYS_CPNT_CLI_SHOW_MICROCODE_VER     FALSE
#define SYS_CPNT_CLI_SHOW_EXPAN_MODULE      FALSE
#define SYS_CPNT_CLI_SHOW_BACKUP_POWER      TRUE
#define SYS_CPNT_CLI_SHOW_FAN_STATUS        TRUE

#define SYS_CPNT_MULTI_BRIDGE_NONE          1
#define SYS_CPNT_MULTI_BRIDGE_AGENT         2
#define SYS_CPNT_MULTI_BRIDGE_BLADE         3
#define SYS_CPNT_MULTI_BRIDGE_BOTH          4


/* This definition is used for project FTTH.  At FTTH project, there would
 * be agent or bridge for rsapi.
 */
#define SYS_CPNT_MULTI_BRIDGE_TYPE          SYS_CPNT_MULTI_BRIDGE_NONE

/* Egress Packet Block */
/* This definition is used for Egress Packet Block function.
 */
#define SYS_CPNT_EGRESS_PACKET_BLOCK                                      TRUE /* Useless */

#if (SYS_CPNT_EGRESS_PACKET_BLOCK == TRUE)
#define SYS_CPNT_EGRESS_PACKET_BLOCK_UNKNOWN_MULTICAST_SUPPORT_LPORT      TRUE
#define SYS_CPNT_EGRESS_PACKET_BLOCK_UNKNOWN_MULTICAST_ENABLE_PER_PORT    TRUE /* decide to be per port or per system */
#define SYS_CPNT_EGRESS_PACKET_BLOCK_UNKNOWN_UNICAST_SUPPORT_LPORT        TRUE
#define SYS_CPNT_EGRESS_PACKET_BLOCK_UNKNOWN_UNICAST_ENABLE_PER_PORT      TRUE /* decide to be per port or per system */
#endif

/* add compile options for storm  sample type
*/
#define SYS_CPNT_STORM_SAMPLE_TYPE_OCTET     FALSE
#define SYS_CPNT_STORM_SAMPLE_TYPE_PACKET    TRUE
#define SYS_CPNT_STORM_SAMPLE_TYPE_PERCENT   FALSE
#define SYS_CPNT_STORM_SAMPLE_TYPE_KILO_BITS FALSE

#define SYS_CPNT_BSTORM_SUPPORT_LPORT       TRUE
#define SYS_CPNT_BSTORM_RATE_PER_PORT_CTRL  TRUE
#define SYS_CPNT_BSTORM_ENABLE_PER_PORT     TRUE
#define SYS_CPNT_MSTORM_SUPPORT_LPORT       FALSE
#define SYS_CPNT_MSTORM_RATE_PER_PORT_CTRL  TRUE
#define SYS_CPNT_MSTORM_ENABLE_PER_PORT     TRUE
#define SYS_CPNT_MULTI_MIRRORED_PORT        TRUE
#define SYS_CPNT_MULTI_MIRROR_TO_PORT       TRUE

/* Unknown Unicast Storm (DLF)
 */
#define SYS_CPNT_UNKNOWN_USTORM             FALSE /* Should change to TRUE later */

#if (SYS_CPNT_UNKNOWN_USTORM == TRUE)
#define SYS_CPNT_UNKNOWN_USTORM_SUPPORT_LPORT       TRUE
#define SYS_CPNT_UNKNOWN_USTORM_RATE_PER_PORT_CTRL  TRUE
#define SYS_CPNT_UNKNOWN_USTORM_ENABLE_PER_PORT     TRUE
#endif

/*  This feature is only one rate limit share to BC/MC/UUC storm control
 */
#define SYS_CPNT_RATE_BASED_STORM_CONTROL           FALSE


#if (SYS_CPNT_MULTI_MIRROR_TO_PORT == FALSE)
/* this feature make sense only when there is only one mirroring destination port
 */
#define SYS_CPNT_ALLOW_DUMMY_MIRRORING_DEST     FALSE
#endif

/* This definition is used to indicate if WRR Q Mode and Q Weight is supported by per port based
 */
#define SYS_CPNT_WRR_Q_MODE_PER_PORT_CTRL   TRUE	/* Jinfeng.chen in 2008-05-30 , old: FALSE*/
#define SYS_CPNT_WRR_Q_WEIGHT_PER_PORT_CTRL TRUE

#define SYS_CPNT_SWCTRL_Q_MODE_DRR   FALSE

/* These define whether flow control and forced-mode speed-dupex
 * are supported. For some chips, this is not supported,
 * and is not available from CLI, SNMP, or Web.
 */
/* TRUE to include flow control config menu
 */
#define SYS_CPNT_FLOW_CONTROL                           TRUE
#define SYS_CPNT_FLOW_CONTROL_NOT_SUPPORT_ON_10G        FALSE /* Useless */
#define SYS_CPNT_RATE_LIMIT_NOT_SUPPORT_ON_10G          TRUE /* Useless */

/* TRUE to include speed duplex config menu
 */
#define SYS_CPNT_FIXED_SPEED_DUPLEX         TRUE

/* ACPv4 AMTR has the option of using sofware learn or hardware learn on a standalone switch.
 * The following definiation indicates whether the project is to run software learn or hardware learn on a standalone switch.
 * Note: only software learn is supported for stacked switches
 */
#define SYS_CPNT_AMTR_HW_LEARN_ON_STANDALONE    FALSE

/* These three constants define for vlan,
 * They are telling if this project supports vlan hybrid mode, q_trunk mode, access mode
 * separately.
 */
#define SYS_CPNT_VLAN_HYBRID                TRUE
#define SYS_CPNT_VLAN_QTRUNK                TRUE
#define SYS_CPNT_VLAN_ACCESS                FALSE


/* This content define vlan behavior for crossbar chipset (5670/5671)
 * TRUE  : crossbar ports will join all vlans
 * FALSE : crossbar ports will not join all vlans (depend on its front panel chipset vlan set)
 *
 * Note : Per Project Definition as following
 * TRUE  : all projects except 5508
 * FALSE : 5508
 */
#define SYS_CPNT_CROSSBAR_JOIN_ALL_VLANS    TRUE

/* A null trunk without any member is available
 * for the LACP operation and the manual configuration
 * to add the member in when this definition is TRUE.
 * Otherwise, the null trunk accepts only the static
 * members if it is created by the manual configuration,
 * and vice versa.
 */
#define SYS_CPNT_SUPPORT_NULL_TRUNK         TRUE

/* The static trunk configuration is allowed on the port with the LACP enabled */
#define SYS_CPNT_STATIC_TRUNK_CONFIG_ALLOWED_ON_LACP_PORT   TRUE

/* The old spec was to allow the trunk members to join the same trunk if they had the same attribute,
 * else stop the configuring and returned false.
 * And the new spec is to apply the trunk attribute to the trunk members and let them join the trunk.
 */
#define SYS_CPNT_TRUNK_MEMBER_AUTO_ATTRIBUTE                TRUE

/* This constant defines fan, thermal and power status detection
 */
#define SYS_CPNT_ALARM_GROUP_MODE_TER       0
#define SYS_CPNT_ALARM_GROUP_MODE_P5        1
#define SYS_CPNT_ALARM_GROUP_MODE           SYS_CPNT_ALARM_GROUP_MODE_TER
#define SYS_CPNT_POWER_DETECT               TRUE
#define SYS_CPNT_STKTPLG_FAN_DETECT         TRUE
#define SYS_CPNT_THERMAL_DETECT             TRUE
#define SYS_CPNT_THERMAL_DETECT_SHOW_TEMPERATURE TRUE
#define SYS_CPNT_FAN_FAULT_STATE_DETECT     FALSE
#define SYS_CPNT_SEVEN_LED_DETECT           TRUE
#define SYS_CPNT_SUPPORT_ALARM_GROUP        TRUE
#define SYS_CPNT_SUPPORT_POWER_MODULE_TYPE  TRUE
#define SYS_CPNT_POWER_LED_DISPLAY_BY_SW    TRUE

/* This constant defines if the ASIC support NET route
 */
#define SYS_CPNT_NET_ROUTE                  TRUE
#define SYS_CPNT_CHECK_HOST_TABLE_HIT_BIT   TRUE


/* This constant defines if the ASIC supports jumbo frame
 */
#define SYS_CPNT_JUMBO_FRAMES               TRUE

#define SYS_CPNT_ACL                        TRUE
#define SYS_CPNT_ACL_IPV6                   TRUE

#define SYS_CPNT_ACL_IP_EXT_TOS             (TRUE  && (SYS_CPNT_ACL == TRUE))

#define SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR   (TRUE && (SYS_CPNT_ACL_IPV6 == TRUE))
#define SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR   (TRUE  && (SYS_CPNT_ACL_IPV6 == TRUE))
#define SYS_CPNT_ACL_IPV6_EXT_TRAFFIC_CLASS (TRUE  && (SYS_CPNT_ACL_IPV6 == TRUE))
#define SYS_CPNT_ACL_IPV6_EXT_FLOW_LABEL    (TRUE  && (SYS_CPNT_ACL_IPV6 == TRUE))
#define SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER   (TRUE  && (SYS_CPNT_ACL_IPV6 == TRUE))

/* time based acl */
#define SYS_CPNT_TIME_BASED_ACL             (TRUE && SYS_CPNT_TIME_RANGE)

/* Auto aggregate ACE before writting to chip.
 */
#define SYS_CPNT_ACL_AUTO_COMPRESS_ACE      TRUE

/* Show ACL counter on UI.
 * SYS_CPNT_ACL_COUNTER defined whether we have this feature or not.
 * SYS_CPNT_ACL_COUNTER_PER_PORT define this feature is per port or per ACL
 */
#define SYS_CPNT_ACL_COUNTER                TRUE
#define SYS_CPNT_ACL_COUNTER_PER_PORT       (TRUE && (SYS_CPNT_ACL_COUNTER == TRUE))

/* For Broadcom platform(chip) only. Dynamic selector assignment.
 * If this constant is FALSE, means the ACL use the pre-assigned selector.
 */
#define SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT                    FALSE//TRUE

#define SYS_CPNT_HRDRV                      FALSE
#define SYS_CPNT_L4                         TRUE
#define SYS_CPNT_MARKER                     TRUE
#define SYS_CPNT_MARKER_CONTAIN_CLI_SET_DOT1P_PRIORITY              TRUE
#define SYS_CPNT_MARKER_CONTAIN_CLI_SET_IP_DSCP                     TRUE
#define SYS_CPNT_MARKER_CONTAIN_CLI_SET_IP_PRECEDENCE               TRUE

#define SYS_CPNT_INGRESS_RATE_LIMIT_VIA_FILTER_ENGINE               FALSE



/* Define the auto baudrate support or not */
#define	SYS_CPNT_AUTOBAUDRATE 	            FALSE

/* Michael 2004-04-08
 * The detection is for SDRAM size and type (number of banks) as outlined
 * in ES4625 H/W spec.  The purpose of it is to determine the max memory
 * size in order to calculate the location of UC.
 */
/* Define the auto DRAM detect support or not - Michael */
#define SYS_CPNT_SDRAM_AUTO_DETECTION       FALSE

/* Define the combo force mode support or not */
#define SYS_CPNT_COMBO_PORT_FORCE_MODE      TRUE

/* Define 100BASE-FX support or not.
 */
#define SYS_CPNT_SWDRV_100FX_SFP            FALSE

/* This constant defines if "restore config from TFTP" should restart
 */
#define SYS_CPNT_XFER_RESTORE_CFG_RESTART   FALSE

/* This constant defines Database Synchronisation (text form)
 */
#define SYS_CPNT_DBSYNC_TXT                 FALSE

/* Define the LED display by ASIC or not?
 */
#define SYS_CPNT_STKTPLG_LED_DISPLAY_BY_ASIC  TRUE

/* IGMPSNP full querier
 * Note: 1) In original design, device will be Non-Querier when receive a query.
 *       2) In new design, device will check source ip of query. If source ip of
 *          query is lower than ours, device will be Non-Querier.
 *          Otherwise, device is still Querier.
 * Suggestion: Currently, 3Com and Enterasys request this feature.
 *             Other vendor don't have request. So please set to TRUE in project
 *             of 3Com and Enterasys. About other project, depend on your requirement.
 */
#define SYS_CPNT_IGMPSNP_QUERIER_FULL_SUPPORT       TRUE

/* IGMPSNP static system router port
 * Note: When user configure router port, they only need to input port number.
 *       And this port will become router port in each vlan.
 * Suggestion: This feature is requested by 3Com. And it need to modify APP.
 *             Except to project of 3Com, please set to FALSE unless your
 *             customer request this feature.
 */
#define SYS_CPNT_IGMPSNP_STATIC_SYSTEM_ROUTER_PORT  FALSE

/* Unknown multicast
 * Note: This compile option is used to define forwarding behavior of Unknown Mcast data packet.
 *
 *      1. SYS_CPNT_IGMPSNP_SELECT_FLOOD_UNKNOWN_MCAST_DATA_PACKET
 *          -- Unknown mcast data will be flooded to each port in same vlan.
 *
 *      2. SYS_CPNT_IGMPSNP_SELECT_UNKNOWN_MCAST_DATA_PACKET_TO_ROUTER_PORT
 *          -- Unknown mcast data will be forward to router port in same vlan.
 *             Strata and Alllayer chip can't provide this feature.
 *             Currently, projects of 3com, Enterasys and Foundry require this feature.
 *
 *      3. SYS_CPNT_IGMPSNP_SELECT_UNKNOWN_MCAST_DATA_PACKET_TO_ROUTER_PORT_PER_SYSTEM
 *          -- Unknown mcast data will be forward to router port in system.
 *             Due to chip's limitation, strata can't meet "SYS_CPNT_IGMPSNP_SELECT_UNKNOWN_MCAST_DATA_PACKET_TO_ROUTER_PORT". So we use other feature of chip to provide similar function. Unknown mcast data packet will be forwarded to system router port.
 *             If your customer request forward unknown mcast data packet to router port and chip is strata, please use this one.
 *
 */
#define SYS_CPNT_IGMPSNP_SELECT_FLOOD_UNKNOWN_MCAST_DATA_PACKET                     1
#define SYS_CPNT_IGMPSNP_SELECT_UNKNOWN_MCAST_DATA_PACKET_TO_ROUTER_PORT            2
#define SYS_CPNT_IGMPSNP_SELECT_UNKNOWN_MCAST_DATA_PACKET_TO_ROUTER_PORT_PER_SYSTEM 3
#define SYS_CPNT_IGMPSNP_PROCESS_UNKNOWN_MCAST_DATA_PACKET  SYS_CPNT_IGMPSNP_SELECT_UNKNOWN_MCAST_DATA_PACKET_TO_ROUTER_PORT

/* Note: It is used to define how chip handle reserve multicast address(224.0.0.x).
 *       If it is defined, mean chip will trap data packet with reserve multicast address to CPU
 *       and won't forward to other port. In current project, no chip have this behavior.
 *       It is defined for future use. Please don't define it now.
 */
#define SYS_CPNT_IGMPSNP_RESERVE_ADDRESS_PACKET_CHIP_TRAP_TO_CPU_BUT_NOT_FORWARD     FALSE

/* Define chip's capability of Layer2 multicast
 */
/* Trunk reload balance
 * Note: This compile option is used to define how to do load balance in trunk port.
 *
 *      1. SYS_CPNT_L2MCAST_TRUNK_BALANCE_MECHANISM_SELECT_ONE_TRUNK_MEMBER
 *          -- MFLT will choose one trunk member and set it as Mcast forward port in Mcast Entry.
 *             Currently, strata use this one. Other chip may use this one.
 *             But please don't use this one in AllLayer.
 *
 *      2. SYS_CPNT_L2MCAST_TRUNK_BALANCE_MECHANISM_SELECT_ALL_TRUNK_MEMBER
 *          -- MFLT will choose set all trunk members in Mcast Entry. It is XGS's and AllLayer's behavior.
 *
 */
#define SYS_CPNT_L2MCAST_TRUNK_BALANCE_MECHANISM_SELECT_ONE_TRUNK_MEMBER   1
#define SYS_CPNT_L2MCAST_TRUNK_BALANCE_MECHANISM_SELECT_ALL_TRUNK_MEMBER   2
#define SYS_CPNT_L2MCAST_TRUNK_BALANCE_MECHANISM   SYS_CPNT_L2MCAST_TRUNK_BALANCE_MECHANISM_SELECT_ALL_TRUNK_MEMBER

/* Unknown mcast forward
 * Note: Define whether chip support "Unknown Mcast Forward Port" feature. Currently, XGS and Strata support this feature.
 */
#define SYS_CPNT_L2MCAST_SUPPORT_SYSTEM_UNKNOWN_MCAST_FORWARD_PORT                         TRUE

/*
 Define IGMP snooping is supported
*/
#define SYS_CPNT_IGMPSNP                    TRUE
/*Define IGMP snooping leave proxy and report proxy
*/
#define SYS_CPNT_IGMPSNP_PROXY              TRUE

/* Define whether support IGMP snooping Immediate Leave feature or not
 */
#define SYS_CPNT_IGMPSNP_IMMEDIATE_LEAVE    TRUE

/* Define whether support IGMP snooping Leave Proxy feature or not
 */
#define SYS_CPNT_IGMPSNP_LEAVE_PROXY        FALSE  /* Zhong Qiyao 2007.07.19: will be TRUE */
#define SYS_CPNT_IGMPSNP_QUERIER            TRUE

#define SYS_CPNT_MVR                        TRUE
#define SYS_CPNT_FILTER_THROOTTLE           TRUE
/*
 * Define the MLDSNP support or not.
 */
#define SYS_CPNT_MLDSNP                       FALSE

/* Define whether support IGMP snooping version 3 (ASM) or not
 */
#define SYS_CPNT_IGMPSNP_SUPPORT_V3         TRUE
#define SYS_CPNT_IGMPSNP_SUPPORT_V3_ASM     TRUE

/* Added by Steven.Gao in 2008/5/26 */
/*
 * Porting IGMPSNP from IPI nsm source code. All related macro is defined below.
 */
#define SYS_CPNT_IPI_IGMPSNP                TRUE
/* end of add by Steven.Gao in 2008/5/26 */


/* Define Default route Action */
#define SYS_CPNT_DEFAULT_ROUTE_ACTION_TRAP2CPU                                          1
#define SYS_CPNT_DEFAULT_ROUTE_ACTION_ROUTE                                             2
#define SYS_CPNT_DEFAULT_ROUTE_ACTION_DROP                                              3


/*  For unknown destination packet switching action
 *  if there is not defined default gateway.
 *  ref. L3 chip action when use the constant:
 *       1 -- trap to CPU;
 *       2 -- switching to port
 *       3 -- drop the packet
 */
#define SYS_CPNT_L3_SWITCHING_UNKNOW_DST_WITHOUT_DFLT_GW_ACTION SYS_CPNT_DEFAULT_ROUTE_ACTION_TRAP2CPU
#define SYS_CPNT_L3_SWITCHING_NET_ROUTE_ACTION_TO_CPU_NOT_SUPPORT   TRUE


#define SYS_CPNT_RIP_CFG_TABLE_BASE_ON_RIF                      1
#define SYS_CPNT_RIP_CFG_TABLE_BASE_ON_VLAN                     2
#define SYS_CPNT_RIP_CFG_TABLE_BASE             SYS_CPNT_RIP_CFG_TABLE_BASE_ON_VLAN


#define SYS_CPNT_CPU_STORM                      TRUE

/* wuli, 2005-09-05 Add Constant for CPU interface configurations
 */
#define SYS_CPNT_CPU_INTERFACE_CPU_JOIN_VLAN    FALSE

/* wuli 2004-05-21, sync from Hagrid, shall be reviewed */
/* These constant defines 3Com features.
 */
#define SYS_CPNT_3COM_CLI                       FALSE
#define SYS_CPNT_3COM_CLI_ACCTON_CLI_BACKDOOR   FALSE
#define SYS_CPNT_3COM_CLI_PROVISION             FALSE

/* Note: SYS_CPNT_3COM_PASSWORD_RECOVERY is supported only
 * when SYS_CPNT_3COM_CLI_PROVISION is FALSE, *or*
 * SYS_CPNT_CFGDB and SYS_CPNT_USERAUTH_USER_IN_CFGDB is TRUE.
 */
#define SYS_CPNT_3COM_PASSWORD_RECOVERY         FALSE

#define SYS_CPNT_3COM_RMON2_PROBE_CONFIG_MIB    FALSE

#define SYS_CPNT_3COM_TWO_OPCODES               FALSE

#define SYS_CPNT_3COM_LOOPBACK_TEST             FALSE
#define SYS_CPNT_3COM_COS_DEFAULT               FALSE
#define SYS_CPNT_3COM_BASIC_ACL                 FALSE

/* 3Com, ACE in an ACL must be with the same action,
 * this is different from Accton's ACL.
 */
#define SYS_CPNT_ACL_WITH_THE_SAME_ACTION       FALSE

/* 3Com, ACL mask is predeinfed, and can be configured by user,
 * it is not Accton's ACL.
 */
#define SYS_CPNT_3COM_ACL_MASK_PREDEFINED       FALSE

/* This is different from Accton's ACL.
 * In Accton's ACL mask, mask can be added and deleted,
 * but can't be modified.
 */
#define SYS_CPNT_3COM_ACL_MASK_MODIFICATION     FALSE


/* the following constants are used to define the method to trap packet that DA is
 * CPU MAC address.
 */
#define SYS_CPNT_AMTR_CPU_INTERVENTION_CPU_JOIN_VLAN    1
#define SYS_CPNT_AMTR_CPU_INTERVENTION_FFP_TRAP         2

/* the CSC shall use this constant in the code to specify the method
 */
#define SYS_CPNT_AMTR_CPU_INTERVENTION_METHOD           SYS_CPNT_AMTR_CPU_INTERVENTION_FFP_TRAP



/* Constant for the component BUFFERMGMT to manage the landing buffer.
 * TRUE  if the component BUFFERMGMT is defined, else FALSE.
 * purpose of landing beffer: for driver layer(ex.FS) could access a static allocated buffer
 * instead of dynamically allocate huge memory
 * Ericali 2004-09-01   After BLANC v3.0.0.69
 */
#define SYS_CPNT_BUFFERMGMT                             FALSE

#define SYS_CPNT_MAINBOARD                              TRUE

/* the follwing constants are usd to define which mechanism is used for master button */
#define SYS_CPNT_MASTER_BUTTON_NONE                  0
#define SYS_CPNT_MASTER_BUTTON_SOFTWARE              1
#define SYS_CPNT_MASTER_BUTTON_HARDWARE              2

#define SYS_CPNT_MASTER_BUTTON     SYS_CPNT_MASTER_BUTTON_HARDWARE

/* SYS_CPNT_SWDRV_MMU_CFG_OVERSUBSCRIBE_CONFIG :
 * Under FULL MESH testing, if the packet lost is not allowed, each queue needs more packet buffer to
 * hold packets under burst traffic (one of congestion case) condition.
 * To pass mesh testing, we may need to OVERSUBSCRIBE the packet buffer size for each queue if the
 * total number of packet buffer is too small, such as BCM5690 only have 128K bytes packet per port
 * (for 8 queues).
 * This configuration WILL ALLOW a given queue to use all available packet buffer of a given port.
 * This MAY cause high priority packet get drop when all packet buffer are locked by low priority
 * traffic under heavy congestion environment ...
 * Note: 1) For stacking switch, we need to guarantee that High priority control packet must NOT be
 *          dropped under any traffic storm. So, we shall not OVERSUBSCRIBE the MMU configuration for
 *          stackable product line.
 *       2) For 10G switch, we can OVERSUBSCRIBE the MMU configuration if pass the FULL MESH test is a
 *          major requirement.
 *
 * SYS_CPNT_SWDRV_MMU_CFG_NORMAL_CONFIG :
 * This configuration will limit the total number of packet buffer for each queue.
 * The sum of total buffer of each queue will be equal to total number of packet available for a port.
 * Under this configuration, each queue will get guarantee packet buffer for that priority traffic.
 * However, this also limit the packet buffer can be used per each queue.
 * Under this configuration, each queue will not be able to hold too many packets under burst traffic
 * (one of congestion case) condition. Under FULL MESH test, we will see packet lost understand this
 * configuration.
 * Note: For stacking switch, we need to guarantee that High priority control packet must NOT be
 * dropped under any traffic storm. So, we shall use NORMAL the MMU configuration for stackable
 * product line.
 */
#define SYS_CPNT_SWDRV_MMU_OVERSUBSCRIBE_CONFIG     1
#define SYS_CPNT_SWDRV_MMU_NORMAL_CONFIG            2

#define SYS_CPNT_SWDRV_MMU_CFG_MODE                 SYS_CPNT_SWDRV_MMU_NORMAL_CONFIG

/* wuli, 2004-11-05
 * BCM5690 errata:
 * Workaround:
 * usage:
 * define SYS_CPNT_FFP_CPU_PRIORITY_WORKAROUND to TRUE to FFP for Workaround
 * define SYS_CPNT_FFP_CPU_PRIORITY_WORKAROUND_MODE to specify L2 product or L3 product
 */
/* Wingson 2004-11-11, add for dev_hrdrv.c to differ L2 from L3 mode
 */
#define SYS_CPNT_FFP_CPU_PRIORITY_WORKAROUND       FALSE

#if (SYS_CPNT_FFP_CPU_PRIORITY_WORKAROUND == TRUE)
#define SYS_CPNT_FFP_CPU_PRIORITY_L2_MODE           1
#define SYS_CPNT_FFP_CPU_PRIORITY_L3_MODE           2
#define SYS_CPNT_FFP_CPU_PRIORITY_WORKAROUND_MODE   SYS_CPNT_FFP_CPU_PRIORITY_L2_MODE
#endif   /* End of SYS_CPNT_FFP_CPU_PRIORITY_WORKAROUND */

#define SYS_CPNT_PACKET_TOCPU_INTERFACE_XGS3        TRUE

/* Automatic Device Detection (ADD)
 */
#define SYS_CPNT_ADD                            TRUE
#define SYS_CPNT_ADD_MODIFY_PRIORITY_BY_RULE    TRUE

/* This defines whether Automatic Device Detection (ADD)
 * supports detection via Link Layer Discovery Protocol (LLDP)
 * as defined by IEEE 802.1ab, in the UI and the
 * core layer.
 */
#define SYS_CPNT_ADD_SUPPORT_LLDP               TRUE

/* This defines whether Automatic Device Detection (ADD)
 * supports configurable 802.1p priority in the UI and the
 * core layer.
 */
#define SYS_CPNT_ADD_CONFIGURABLE_PRIORTIY      TRUE

/* This is defined the ADD shall detect IP phone traffic on the native VLAN only or
 * for all VLAN. And auto join the monitored port to voice VLAN when detected a IP
 * phone plug in.
 *
 * If it is TRUE, the ADD shall detect IP phone traffic on native VLAN.
 * Else the ADD shall detect IP phone traffic for all VLAN.
 */
#define SYS_CPNT_ADD_DETECT_NATIVE_VLAN_ONLY    FALSE

/* support Q in Q feature
 */
#define SYS_CPNT_QINQ                              TRUE

#if (SYS_CPNT_QINQ==TRUE)
#define SYS_CPNT_QINQ_TPID_GLOBAL                  TRUE

/* To specify chip QinQ mode is global or not.
 * TRUE  - On chip, QinQ mode is global,
 *         all port must be enabled or disabled QinQ at the same time.
 *         That is, when a port change from none mode to access/uplink
 *         mode, all other ports will be also changed to access or uplink
 *         according to SYS_DFLT_DOT1Q_TUNNEL_MODE. If defaule is none mode,
 *         it works as same as access mode.
 *         As above, when a port change from access/uplink to none mode,
 *         all other ports will be also changed to none mode.
 * FALSE - On chip, QinQ mode is not global,
 *         all ports can be enabled or disabled QinQ independently.
 */
#define SYS_CPNT_SWCTRL_QINQ_MODE_GLOBAL        TRUE
#endif

/* 565x support DDR ram test
 */
#define SYS_CPNT_DDR_TEST	                   TRUE


/*support when this is true it will not run diag after reload
 */
#define SYS_CPNT_SUPPORT_QUICKBOOT                 FALSE

/* Zhong Qiyao 2007.07.19:
 * The following does not look like they belong to SYS_CPNT.
 */
#if 0
/* Constant for the component L3 Driver to manage the packets to CPU.
 * 2005-7-1, vai.wang
 */
#define SYS_CPNT_PKT2CPU_L3SLOWPATH             1   /* L3SlowPathToCPU */
#define SYS_CPNT_PKT2CPU_ICMPREDIRECT           2   /* IcmpRedirectToCPU */
#define SYS_CPNT_PKT2CPU_IPMCTTLERROR           3   /* IpmcTtlErrorToCPU */
#define SYS_CPNT_PKT2CPU_L3UCTTLERROR           4   /* L3UCTtlErrorToCPU */
#define SYS_CPNT_PKT2CPU_DOSATTACK              5   /* DosAttackToCPU */
#define SYS_CPNT_PKT2CPU_TUNNELERROR            6   /* TunnelErrorToCPU */
#define SYS_CPNT_PKT2CPU_MARTIANADDR            7   /* MartianAddrToCPU */
#define SYS_CPNT_PKT2CPU_V6L3ERROR              8   /* V6L3ErrorToCPU */
#define SYS_CPNT_PKT2CPU_SRCROUTE               9   /* SrcRteToCPU */
#define SYS_CPNT_PKT2CPU_IPMCPORTMISS           10  /* IpmcPortMissToCPU */
#define SYS_CPNT_PKT2CPU_V4L3DSTMISS            11  /* V4L3DstMissToCPU */
#define SYS_CPNT_PKT2CPU_V6L3DSTMISS            12  /* V6L3DstMissToCPU */
#define SYS_CPNT_PKT2CPU_UNRESOLVEDL3SRC        13  /* UnresolvedL3SrcToCPU */
#define SYS_CPNT_PKT2CPU_UNKNOWNVLAN            14  /* UnknownVlanToCPU */
#define SYS_CPNT_PKT2CPU_IPMCERROR              15  /* IpmcErrorToCPU */
#define SYS_CPNT_PKT2CPU_V4L3ERROR              16  /* V4L3ErrorToCPU */
#define SYS_CPNT_PKT2CPU_UNKNOWNUCAST           17  /* UnknownUcastToCPU */
#define SYS_CPNT_PKT2CPU_UNKNOWNMCAST           18  /* UnknownMcastToCPU */
#define SYS_CPNT_PKT2CPU_UNKNOWNIPMC            19  /* UnknownIpmcToCPU */
#define SYS_CPNT_PKT2CPU_NONSTATICSTATIONMOVE   20  /* NonStaticStationMoveToCPU */
#define SYS_CPNT_PKT2CPU_STATICSTATIONMOVE      21  /* StaticStationMoveToCPU */
#endif

#define SYS_CPNT_EFM_OAM                        FALSE
#if (SYS_CPNT_EFM_OAM == TRUE)
/* perform remote loopback actively, i.e. permit to send loopback control OAMPDUs
 */
#define SYS_CPNT_EFM_OAM_REMOTE_LB_ACTIVELY     FALSE

/* accept remote loopback passively, i.e. react to loopback control OAMPDUs
 */
#define SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY    TRUE

#define SYS_CPNT_EFM_OAM_ORG_SPEC               FALSE
#endif
#if defined(SYS_CPNT_EFM_OAM_ORG_SPEC) && (SYS_CPNT_EFM_OAM_ORG_SPEC == TRUE)
/* OAM organization specific feature
 * Note: OAM org. spec. feature can be extended by vendor.
 *       The org. spec. OAMPDU must be recognized by switch and CPE.
 *       In order to operate with different CPE, switch must be indicated which CPE type used.
 */
#define SYS_CPNT_EFM_OAM_ORG_SPEC_ACCTON_CPE    1
#define SYS_CPNT_EFM_OAM_ORG_SPEC_CPE_STYLE  SYS_CPNT_EFM_OAM_ORG_SPEC_ACCTON_CPE
#endif

#define SYS_CPNT_INTERNAL_LOOPBACK_TEST     TRUE

/* RESETMGMT component deals with the H/W reset button
 * if the time that the reset button is being pressed/held is T seconds,
 * T < SYS_ADPT_RESETMGMT_RESET_TIME --> no reset
 * T >= SYS_ADPT_RESETMGMT_RESET_TIME && T < SYS_ADPT_RESETMGMT_RESET_TO_FACTORY_DEFAULT_TIME --> reset the system
 * T >= SYS_ADPT_RESETMGMT_RESET_TO_FACTORY_DEFAULT_TIME --> reset with factory defaults
 */
#define SYS_CPNT_RESETMGMT 		    FALSE
#define SYS_CPNT_I2C                    TRUE
#if (SYS_CPNT_I2C == TRUE)
#define  SYS_CPNT_SELECT_CHANNEL_MODE_NOCHANNEL               0
#define  SYS_CPNT_SELECT_CHANNEL_MODE__PCA9548                  1
#define  SYS_CPNT_SELECT_CHANNEL_MODE__EPLD                        2

#define SYS_CPNT_SELECT_CHANNEL_MODE        SYS_CPNT_SELECT_CHANNEL_MODE__PCA9548
#endif

/* ES4626F-38 combo port i2c slave address is defined by HW
 * we define a new cnpt to separate the code
 */
#define SYS_CPNT_COMBO_PORT_I2C_BY_HW        TRUE

#define SYS_CPNT_NOTIFICATIONLOG_MIB  TRUE

/*the 802.1ag protocol*/
#define SYS_CPNT_CFM                   FALSE
#define SYS_CPNT_CFM_DELAY_MEASUREMENT TRUE
/*end 802.1ag*/

#define SYS_CPNT_POE                FALSE

#define SYS_CPNT_DOS                TRUE

#define SYS_CPNT_LED                TRUE

/* flow control in experimental which is added by haiqiang
 * you must turn on the flow ctl in linux kernel before using it in user mode applications
 */
#define SYS_CPNT_MSG_FLOW_CTL       FALSE /* flow ctl is turned off */

#define SYS_CPNT_SYSCTRL_XOR        FALSE

#define SYS_CPNT_SUPPORT_FORCED_1000BASE_T_MODE  FALSE

/* This definition is used for STORM support mode
 * If no support any storm, please set to SYS_CPNT_STORM_MODE to SYS_CPNT_STORM_NONE,
 * else please combine your support items with '|' in SYS_CPNT_STORM_MODE
 */
#define SYS_CPNT_STORM_NONE               0   /* BIT-0 */
#define SYS_CPNT_STORM_BSTORM             1   /* BIT-1 */
#define SYS_CPNT_STORM_MSTORM             0   /* BIT-2 */
#define SYS_CPNT_STORM_UNKNOWN_USTORM     0   /* BIT-3 */
#define SYS_CPNT_STORM_MODE               (SYS_CPNT_STORM_BSTORM | SYS_CPNT_STORM_MSTORM | SYS_CPNT_STORM_UNKNOWN_USTORM)


/* Remote Switched Port Analyzer (RSPAN)
 * TRUE, meaning to enable the RSPAN function in the project.
 * This only works for standalone project by now.
 */
#define SYS_CPNT_RSPAN              FALSE
#define SYS_CPNT_RSPAN_FILTER_VLAN  FALSE

/* To avoid making confused, MTU setting is either per system or per port.
 *  if chip can support per port setting, should select SYS_CPNT_SWCTRL_MTU_PER_PORT
 */
#define SYS_CPNT_SWCTRL_MTU_PER_SYSTEM 	1
#define SYS_CPNT_SWCTRL_MTU_PER_PORT		2
#define SYS_CPNT_SWCTRL_MTU_CONFIG_MODE	SYS_CPNT_SWCTRL_MTU_PER_PORT
#define SYS_CPNT_UDP_HELPER        TRUE

#define SYS_CPNT_DAI                     TRUE
#define SYS_CPNT_DAI_RATE_LIMIT_BY_RULE  (TRUE & SYS_CPNT_DAI)


/* To specify combo port media preferred auto is implemented by
 * hardware or software.
 *
 * TRUE  - Auto media selection is performed by hardware.
 *
 * FALSE - Auto media selection is performed by software.
 *         SWDRV always set combo port to force mode according to
 *         SFP transceiver present state when user config media type of
 *         combo port to auto mode.
 *         It also implies SFP transceiver
 *         present state monitoring need to do.
 */
#define SYS_CPNT_SWDRV_COMBO_PORT_MEDIA_PREFERRED_AUTO_BY_HW    FALSE

/* To specify how to trap packets to CPU.
 *
 * TRUE - trap packtes by rule
 * TRUE - trap packets by chip register.
 */
#define SYS_CPNT_SWDRV_TRAP_IGMP_BY_RULE                    TRUE
#define SYS_CPNT_SWDRV_TRAP_MLD_BY_RULE                     TRUE
#define SYS_CPNT_SWDRV_TRAP_UNKNOWN_IPMC_BY_RULE            TRUE
#define SYS_CPNT_SWDRV_TRAP_UNKNOWN_IPV6MC_BY_RULE          TRUE

/* To specifiy the format of packet delivered between
 * LAN and chip SDK.
 *
 * SYS_CPNT_LAN_FIXED_TPID
 *     TRUE  - LAN always receives/sends packets with TPID 0x8100.
 *     FALSE - LAN receives/sends packets with actual TPID.
 */
#define SYS_CPNT_LAN_FIXED_TPID                     FALSE

#define SYS_CPNT_MONITORING_PROCESS_CPU     TRUE

/* Support the system-wise counter, NMTR is responsible for clearing counter behavior.
 */
#define SYS_CPNT_SYSTEM_WISE_COUNTER        TRUE

#define SYS_CPNT_10G_MODULE_SUPPORT            TRUE     /* for support 10G module used */

#if (SYS_CPNT_10G_MODULE_SUPPORT == TRUE)
#define DEV_SWDRV_26PORT_10G_MODULE_DEVID     0
#define DEV_SWDRV_50PORT_10G_MODULE_DEVID     1
#define DEV_SWDRV_26PORT_10G_MODULE1_PHYADDR  (25 + 0x40) /*bcm sdk mdio address starts from 0x40 */
#define DEV_SWDRV_26PORT_10G_MODULE2_PHYADDR  (26 + 0x40)
#define DEV_SWDRV_50PORT_10G_MODULE1_PHYADDR  (27 + 0x40)
#define DEV_SWDRV_50PORT_10G_MODULE2_PHYADDR  (28 + 0x40)

#define DEV_SWDRV_26PORT_10G_XE1_PORT  25
#define DEV_SWDRV_26PORT_10G_XE0_PORT  24

#define DEV_SWDRV_50PORT_10G_XE1_PORT  27
#define DEV_SWDRV_50PORT_10G_XE0_PORT  26
#endif /*end of SYS_CPNT_10G_MODULE_SUPPORT */

/* for UDLD (UniDirectional Link Detection)
 *  SYS_CPNT_UDLD_TRAP_UDLD_BY_GLOBAL_RULE
 *      TRUE    - Packet is trapped by global rule
 *      FALSE   - Packet is trapped by per port rule
 *  SYS_CPNT_UDLD_PDU_FORMAT
 *      ACCTON  - should be used as default if this option is not defined.
 *      CISCO   - refer to RFC 5171.
 */
#define SYS_CPNT_UDLD                               TRUE
#define SYS_CPNT_UDLD_TRAP_UDLD_BY_GLOBAL_RULE      TRUE

#define SYS_CPNT_ERPS   FALSE
#define SYS_CPNT_EFM_OAM_REMOTE_LB_ACTIVELY TRUE
#define SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY TRUE

#endif
/* End of SYS_CPNT.H */

