/*---------------------------------------------------------------------
 * Package: SYS_BLD.H
 * Purpose: This package defines the task name, queue name, and
 *      semaphore name for the project. This package also defines
 *    the priority of each task.
 * Note: The naming constant defined in this package shall be reused
 *   by the other project.
 *---------------------------------------------------------------------
 *
 * History:
 *          Date      -- Modifier,        Reason
 *  V1.0  2001.10.18  -- Jason Hsue,      Creation
 *  V2.0  2002.09.05  -- Jason Hsue,      Add SNTP task priority and task name
 *  V2.1  2003.01.06  -- Jason Hsue,      Modify StkCtrl Task priority to 104 and StkTplg Task to 52
 *  V3.0  2004.02.12  -- wuli             clean up and adjust task priority
 *  V3.1  2004.03.03  -- wuli
 *                           Remove SYS_BLD_3RD_HIGH_TASK_PRIORITY ( used by isc.c)
 *                           add define SYS_BLD_RAISE_TO_HIGH_PRIORITY ( 3rd high)
 */
#ifndef SYS_BLD_H
#define SYS_BLD_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_adpt.h"


#define SYS_BLD_PROCESS_DEFAULT_PRIORITY        135     /* System default priority */
#define SYS_BLD_LOWEST_TASK_PRIORITY            139     /* System lowest prioirty  */
#define SYS_BLD_PROCESS_DEFAULT_SCHED_POLICY    SYSFUN_SCHED_DEFAULT

#define SYS_BLD_REALTIMEPROCESS_DEFAULT_PRIORITY        90
#define SYS_BLD_REALTIMEPROCESS_HIGHEST_PRIORITY        99
#define SYS_BLD_REALTIMEPROCESS_2ND_HIGH_PRIORITY       98

/* Task Working Environment Definitions
 */
#define SYS_BLD_KERNEL_TIME_SLICE_TICKS         5       /* 5 ticks for round-robin */
#define SYS_BLD_MAX_TIMER_IN_SYSTEM             256     /* Max timer allowed in system  */
#define SYS_BLD_COMMON_MSG_Q_LENGTH             256     /* default message Q length     */
#define SYS_BLD_TELNET_SERVICE_SOCKET_PORT      5151

/* Common stack sizes and attributes
 */
#define SYS_BLD_TASK_COMMON_STACK_SIZE          (4 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE)     /* User stack size  in byte */
#define SYS_BLD_TASK_LARGE_STACK_SIZE           (8 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE)     /* User stack size  in byte */
#define SYS_BLD_TASK_COMMON_ATTRIBUTE           (PRE_EMPTIVE | TIME_SLICING)

/* Special stack sizes
 */
#define SYS_BLD_SNMP_V3_TASK_STACK_SIZE             (16 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* SNMPv3 task (not SNMPv2) */
#define SYS_BLD_CLI_TASK_STACK_SIZE                 (16 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* CLI task */

#define SYS_BLD_HTTP_TASK_STACK_SIZE                (16 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* HTTP task */
#define SYS_BLD_HTTP_ASYNC_GET_CERT_TASK_STACK_SIZE (16 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* HTTP Async-get-cert task */
#define SYS_BLD_SNTP_TASK_STACK_SIZE                (6 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE)  /* SNTP task */
#define SYS_BLD_TELNET_SHELL_CHILD_TASK_STACK_SIZE  (16 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* Telnet shell daemon task */
#define SYS_BLD_GARP_TASK_STACK_SIZE                (12 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* GARP task */

#define SYS_BLD_SSHD_ASYNC_GEN_HOST_TASK_STACK_SIZE (16 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* SSHD Async-gen-host task */
#define SYS_BLD_SSHD_CHILD_TASK_STACK_SIZE          (20 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* SSHD child task */
#define SYS_BLD_RADIUS_TASK_LARGE_STACK_SIZE        (16 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* RADIUS task when large */
#define SYS_BLD_NSM_GROUP_TASK_STACK_SIZE           (24 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* NSM Group task */
/* There are many local variables declared on the functions
 * called by NMTRDRV. Enlarge the stack size to 64 KB temporaily (no harm except there is more area for memroy corruption)
 * This value should be tuned after resonable stack size can be defined
 * charlie_chen 2009/9/30
 */
#define SYS_BLD_NMTRDRV_TASK_STACK_SIZE             (64 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* NMTRDRV task */

/* Idle task just contains an infinite while loop and will never
 * call other functions. Thus it just requires a very small stack size
 * If set SYS_BLD_SYSFUN_IDLE_TASK_STACK_SIZE as 256 bytes, stack overflow
 * will occur. After testing, it's no problem to set it as 1K.
 */
#define SYS_BLD_SYSFUN_IDLE_TASK_STACK_SIZE         (SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE)

/* to avoid stack overflow, stack size of backdoor thread should be the
 * maximum of all thread.
 */
#define SYS_BLD_BACKDOOR_THREAD_STACK_SIZE          (64 * SYS_TYPE_1K_BYTES + SYS_ADPT_EXTRA_TASK_STACK_SIZE) /* Backdoor */

/* Task Priority Definitions
 * Note: Priority level ranges from 1 ~ 255, where priority 1 is the highest priority in SYS_BLD.H.
 */
#define SYS_BLD_DEFAULT_TRANSLATED_TASK_PRIORITY          SYS_BLD_PROCESS_DEFAULT_PRIORITY

#define SYS_BLD_OM_CRITICAL_SECTION_PRIORITY    39      /* raise task priority to protect OM        */

#define SYS_BLD_HIGHTEST_TASK_PRIORITY          SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Usable hightest priority for Phase II, 44 --> 40 Anderson comment */
#define SYS_BLD_2ND_HIGH_TASK_PRIORITY          SYS_BLD_HIGHTEST_TASK_PRIORITY
#define SYS_BLD_RAISE_TO_HIGH_PRIORITY          SYS_BLD_2ND_HIGH_TASK_PRIORITY

/* Task priorities
 */
#define SYS_BLD_SYS_TIMER_TASK_PRIORITY         SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* Timer task priority                      */

/* Following symbol is defined task priority for bcm SDK in RX task
 * please reference orion\include\bcm_int\rx.h
 * #define RX_THREAD_PRI_DFLT          99 (Priority for all front panel chip types, like 569x/5673)
 * #define RX_THREAD_PRI_CROSSBAR      44 (Priority for crossbar chip, like 5670/5671)
 */

/* Following symbol is defined task priority for bcm SDK in TX task
 * please reference orion\src\bcm\tx.c
 *   tid = sal_thread_create("bcmTX", SAL_THREAD_STKSZ, 44,
 *                           _bcm_tx_callback_thread, NULL);
 */

/* Following symbol is defined as task priority for bcmDPC in bcm SDK
 * please refer to orion\src\sal\kern\dpc.c
 * #define SAL_DPC_THREAD_PRIO  43
 */

#define SYS_BLD_NIC_TASK_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* NIC task priority, must greater than RX and protocol task */
#define SYS_BLD_TPLG_DISCOVERY_TASK_PRIORITY    SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* Topology dicovery task priority          */
#define SYS_BLD_LAN_ADM_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* 46 --> 48 Anderson comment */

#define SYS_BLD_LACP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* LACP task priority, 100 --> 50 Anderson comment */
#define SYS_BLD_STAP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* Spanning Tree task priority, org = 140 -> 255 -> 145 */
#define SYS_BLD_DOT1X_TASK_PRIORITY             SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* DOT1X task priority                      */

#define SYS_BLD_SYS_TASK_MON_TASK_PRIORITY      SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* wuli, TBD */

#define SYS_BLD_MIDDLE_HIGH_TASK_PRIORITY       SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* wuli, new define */

#define SYS_BLD_IMC_TASK_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* Remote Poll task priority                */
#define SYS_BLD_ISC_AGENT_LOW_SPEED_TASK_PRIORITY      SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* ISC_AGENT low speed task priority */
#define SYS_BLD_ISC_AGENT_HIGH_SPEED_TASK_PRIORITY     SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* ISC_AGENT high speed task priority */

#define SYS_BLD_SWDRV_TASK_PRIORITY             SYS_BLD_REALTIMEPROCESS_DEFAULT_PRIORITY      /* Switch control driver task priority      */
#define SYS_BLD_SWCTRL_TASK_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY      /* Switch control task priority             */

/* Zhong Qiyao, 2004.04.19
 * Mercury_V3-00438: 150 BPDU's locks up CLI
 * problems: CLI is temporarily suspended
 * cause: RX task (99) has a higher priority than CLI (105)
 * solution: move CLI up to 98
 * files: sys_bld.h
 */
#define SYS_BLD_CLI_TASK_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Console task priority                    */

#define SYS_BLD_SYS_CALLBACK_TASK_PRIORITY      SYS_BLD_CLI_TASK_PRIORITY

#define SYS_BLD_VLANMGR_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* GARP/GVRP/GMRP task priority             */
#define SYS_BLD_CALLBACK_TASK_PRIORITY          SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* COS task priority                        */
#define SYS_BLD_STACK_CTRL_TASK_PRIORITY        SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Stack control task priority              */

#define SYS_BLD_BCMDRV_COUNTER_TASK_PRIORITY    SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* BCMDRV task*/
#define SYS_BLD_BCMDRV_BACKDOOR_TASK_PRIORITY   SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* BCMDRV task*/

#define SYS_BLD_POEDRV_TASK_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* PoE driver task priority                 */

#define SYS_BLD_LEDDRV_TASK_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* LED driver task priority  org = 95       */
#define SYS_BLD_LED_MGR_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* LED task priority       org = 95         */

#define SYS_BLD_PHASE2_IP_TASK_PRIORITY         SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* phase2 ip task, higher than timer        */

#define SYS_BLD_RADIUS_TASK_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* RADIUS task priority sync from 1.0       */
#define SYS_BLD_TACACS_TASK_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* TACACS task priority sync from 1.0       */
#define SYS_BLD_FXFER_TASK_PRIORITY             SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* File Transfer task priority, higher than gvrp/garp             */
#define SYS_BLD_DNS_PROXY_TASK_PRIORITY         SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* DNS proxy task */
#define SYS_BLD_DNS_RESOLVER_TASK_PRIORITY      SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* DNS resolver task */

#define SYS_BLD_NETWORKACCESS_TASK_PRIORITY     SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Network Access task priority  */

#define SYS_BLD_AMTRL3_TASK_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* AMTRL3 task priority                 */
#define SYS_BLD_AMTRDRV_ADDRESS_TASK_PRIORITY        SYS_BLD_AMTRL3_TASK_PRIORITY      /* task to process NA and ageout. used in Software Learn */
#define SYS_BLD_AMTRDRV_ASIC_COMMAND_TASK_PRIORITY   SYS_BLD_AMTRL3_TASK_PRIORITY      /* task to program ASIC's ARL table and process command block. used in Software Learn */

#define SYS_BLD_CLUSTER_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Cluster task priority                    */

#if defined (SYS_ADPT_TELNET_08_REQUIREMENT) && (SYS_ADPT_TELNET_08_REQUIREMENT == TRUE)
#define SYS_BLD_TELNET_D_TASK_PRIORITY          SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Telnet daemon task priority , org = 90, 110->90 again   */
#define SYS_BLD_TELNET_SHELL_D_TASK_PRIORITY    SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Telnet shell daemon task priority , org = 90, 110->90 again */
#else
#define SYS_BLD_TELNET_D_TASK_PRIORITY          SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Telnet daemon task priority , org = 90   */
#define SYS_BLD_TELNET_SHELL_D_TASK_PRIORITY    SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Telnet shell daemon task priority , org = 90 */
#endif

#define SYS_BLD_SNMP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* SNMP task priority                       */
#define SYS_BLD_TRAPMGR_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Trap management task priority            */
#define SYS_BLD_HTTP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* HTTP (Web) task priority                 */

#define SYS_BLD_SSHD_MAIN_TASK_PRIORITY         SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* SSHD main task priority                  */
#define SYS_BLD_SSHD_SERVER_TASK_PRIORITY       SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* SSHD server task priority                */
#define SYS_BLD_SSHD_ASYNC_DEL_USER_KEY_TASK_PRIORITY SYS_BLD_SSHD_MAIN_TASK_PRIORITY /* SSHD async del-user-key task priority */
#define SYS_BLD_SSHD_ASYNC_WRITE_HOST_KEY_TASK_PRIORITY SYS_BLD_SSHD_MAIN_TASK_PRIORITY /* SSHD async write-host-key task priority */
#define SYS_BLD_SMTP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* SMTP task priority                       */
#define SYS_BLD_GVRP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY  /*org=110, allenc agree to change it, hans*/     /* GARP/GVRP/GMRP task priority             */
#define SYS_BLD_GARP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY  /*org=110 allenc agree to change it, hans */     /* GARP/GVRP/GMRP task priority             */
#define SYS_BLD_TELNET_CHILD_TASK_PRIORITY      (SYS_BLD_TELNET_D_TASK_PRIORITY+1)     /* Telnet child task priority */
#define SYS_BLD_TELNET_SHELL_CHILD_TASK_PRIORITY (SYS_BLD_TELNET_SHELL_D_TASK_PRIORITY+1) /* Telnet child task priority */
#define SYS_BLD_SSHD_CHILD_TASK_PRIORITY        (SYS_BLD_SSHD_SERVER_TASK_PRIORITY+1) /* SSHD child task priority */

#define SYS_BLD_PHASE2_MSG_TASK_PRIORITY        SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* task to process phase2 message, higher than http.. */

#define SYS_BLD_VDSLDRV_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* VDSL task priority                       */
#define SYS_BLD_WEBAUTH_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* WEBAUTH task priority                    */

#define SYS_BLD_IGMP_SNOOPING_TASK_PRIORITY     SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* IGMP task priority                       */
#define SYS_BLD_SYSLOG_TASK_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* SYSLOG task priority                     */
#define SYS_BLD_L2MCAST_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* L2MCAST task priority                    */
#define SYS_BLD_PORT_SECURITY_TASK_PRIORITY     SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Port security (intrusion) task priority  */
#define SYS_BLD_SWDRV_CACHE_TASK_PRIORITY       SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Switch Driver Cache task priority */
#define SYS_BLD_UDPHELPER_TASK_PRIORITY         SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* UDP Helper priority  */


#define SYS_BLD_SYSDRV_TASK_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* SYSDRV detection task priority. Org=133, kinghong modified for I2C bus busy issue.  */
#define SYS_BLD_IML_TASK_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* IML event handler task, to process nessary event */
#define SYS_BLD_TRACEROUTE_TASK_PRIORITY        SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Trace Route task priority */
#define SYS_BLD_PING_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Ping task priority */

#define SYS_BLD_SWDRVL3_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_HTTP_ASYNC_GET_CERT_TASK_PRIORITY SYS_BLD_PROCESS_DEFAULT_PRIORITY   /* HTTP async get-cert task priority        */
#define SYS_BLD_SSHD_ASYNC_GEN_HOST_KEY_TASK_PRIORITY SYS_BLD_PROCESS_DEFAULT_PRIORITY /* SSHD async gen-host-key task priority */
#define SYS_BLD_DB_SYN_TASK_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* E2prom data sync. task priority          */

#define SYS_BLD_DHCP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* DHCP task priority                       */
#define SYS_BLD_DHCPSNP_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* DHCPSNP task priority  same as DHCP */

#define SYS_BLD_BOOTP_TASK_PRIORITY             SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* BootP download task priority             */
#define SYS_BLD_TFTP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* TFTP download or upload task priority    */
#define SYS_BLD_HSRP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* HSRP task priority               */

#define SYS_BLD_LLDP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* LLDP task priority */

/* VRRP's priority must higher than IML. Otherwise, it will cause serious problem, please reference ES4649-ZZ-00622 */
#define SYS_BLD_VRRP_TASK_PRIORITY              SYS_BLD_IML_TASK_PRIORITY     /* VRRP task priority               */

#define SYS_BLD_RMON_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* RMON task priority                       */

#define SYS_BLD_NMTR_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* NMTR task priority                       */

#define SYS_BLD_AMTR_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* AMTR task priority                       */
#define SYS_BLD_AMTRDRV_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* task to sync to HISAM and delete group of addresses. used in Hardware Learn */

#define SYS_BLD_SNTP_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* SNTP task priority   skyang 2002.08.16   */

/* DbSync_Txt task priority shall be better than or equal to AMTR's to prevent the starvation.
 * This was 200, but it may not be the final solution.
 * So we keep its original position in this file.
 */
#define SYS_BLD_DBSYNC_TXT_TASK_PRIORITY        SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* dbsync_txt task priority */

#define SYS_BLD_BPROTECT_TASK_PRIORITY          SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* wuli, ver 00.13.00                       */

#define SYS_BLD_PHASE2_PING_TASK_PRIORITY       SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* phase2 ping client task                  */

/* CFGDB task priority shall be better than or equal to AMTR's to prevent the starvation.
 * This was 235, but it may not be the final solution.
 * So we keep its original position in this file.
 */
#define SYS_BLD_CFGDB_TASK_PRIORITY             SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* For CFGDB task priority                  */

/* FS task priority shall be better than or equal to AMTR's to prevent the starvation.
 * This was 240, but it may not be the final solution.
 * So we keep its original position in this file.
 */
#define SYS_BLD_FLSHDRV_TASK_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* E2prom programming task priority         */

#define SYS_BLD_SERIAL_DOWNLOAD_TASK_PRIORITY   SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Serial download task priority            */

#define SYS_BLD_KEYGEN_MAIN_TASK_PRIORITY       SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* Key generation task priority */

#define SYS_BLD_IDLE_TASK_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* System Idle task priority                */

/* Task Name Definitions
 */
#define SYS_BLD_SYS_TIMER_TASK          "SYS_TIMER"    /* System timer task            */
#define SYS_BLD_TASK_MON_TASK           "TASK_MON"     /* Task monitor task            */
#define SYS_BLD_FS_TASK                 "FS"            /* File system task           */

#define SYS_BLD_P2IP_TASK               "P2IP"
#define SYS_BLD_P2MG_TASK               "P2MG"
#define SYS_BLD_P2TM_TASK               "P2TM"
#define SYS_BLD_P2LC_TASK               "P2LC"
#define SYS_BLD_LAN_ADM_TASK            "LANADM"
#define SYS_BLD_STACK_CTRL_TASK         "STACK_CTRL"   /* Stack management (i.e. system main) task */
#define SYS_BLD_CLUSTER_TASK            "CLUSTER"      /* Cluster task                 */
#define SYS_BLD_CONSOLE_TASK            "CLIX"         /* Console task                 */
#define SYS_BLD_TPLGY_DISCOVERY_TASK    "TPLG"         /* Topology dicovery task       */
#define SYS_BLD_LED_MGR_TASK            "LED_MGR"      /* LED Manager task             */
#define SYS_BLD_NIC_TASK                "NIC"          /* NIC driver task              */
#define SYS_BLD_SWCTRL_TASK             "SWCTRL"       /* Switch control task          */
#define SYS_BLD_SWDRV_TASK              "SWDRV_MONITOR"        /* Switch Driver task           */
#define SYS_BLD_SWDRV_CACHE_TASK        "SWDRV_CACHE"  /* Switch Driver Cache task     */

#if 0 /* obselete, not used for XGS, Zhong Qiyao, 2004.02.19 */
#define SYS_BLD_SWDRVL3_CACHE_TASK      "SWDRVL3_CACHE"  /* Switch Driver L3 Cache task     */
#endif

#define SYS_BLD_SWDRVL3_TASK            "SWDRVL3_CACHE"  /* Switch Driver L3 Cache task     */
#define SYS_BLD_SWDRVL3_BACKDOOR_TASK   "SD3_BCKD"       /* Switch Driver L3 Cache task     */

#define SYS_BLD_VDSLDRV_TASK            "VDSLDRV"      /* VDSL driver task */
#define SYS_BLD_MAU_CTRL_TASK           "MAU_CTRL"          /* MAU control task             */

#define SYS_BLD_SSHD_MAIN_TASK          "SSHD_MAIN"    /* SSHD main task               */
#define SYS_BLD_SSHD_CHILD_TASK         "SSH%02d"      /* SSHD main task               */
#define SYS_BLD_SSHD_ASYNC_GEN_HOST_KEY_TASK "SSH_GEN" /* SSHD async gen-host-key task */
#define SYS_BLD_SSHD_ASYNC_DEL_USER_KEY_TASK "SSH_DEL" /* SSHD async del-user-key task */
#define SYS_BLD_SSHD_ASYNC_WRITE_HOST_KEY_TASK "SSHD_WRITE" /* SSHD async write-host-key task */

#define SYS_BLD_SYSDRV_TASK             "SYSDRV"       /* SYSDRV detecetion task       */
#define SYS_BLD_STAP_TASK               "STA"          /* Spanning Tree task           */
#define SYS_BLD_GARP_TASK               "GARP"         /* GARP task                    */
#define SYS_BLD_IGMP_TASK               "IGMP"         /* GARP task                    */
#define SYS_BLD_LACP_TASK               "LACP"         /* LACP task                    */
#define SYS_BLD_DOT1X_TASK              "DOT1X"        /* DOT1X task                   */
#define SYS_BLD_NETWORKACCESS_TASK      "NETWORK_ACCESS" /* Network Access task        */
#define SYS_BLD_RADIUS_TASK             "RADIUS"       /* RADIUS task name sync from 1.0 */
#define SYS_BLD_TACACS_TASK             "TACACS_TASK"       /* TACACS task                  */
#define SYS_BLD_SYSLOG_TASK             "SYSLOG"       /* SYSLOG task                  */
#define SYS_BLD_SMTP_TASK               "SMTP"         /* SMTP task                    */
#define SYS_BLD_ISC_AGENT_LOW_SPEED_TASK               "ISC_AG_LS"     /* ISC_AGENT low speed task */
#define SYS_BLD_ISC_AGENT_HIGH_SPEED_TASK              "ISC_AG_HS"     /* ISC_AGENT high speed task */
#define SYS_BLD_PORT_SECURITY_TASK      "IMTR"         /* Port security (intrusion) task */
#define SYS_BLD_RMON_TASK               "RMON"         /* RMON task                    */
#define SYS_BLD_SAS_TASK                "SAS"          /* Smart auto-sensing task      */
#define SYS_BLD_RESILIENT_LINK_TASK     "RES"          /* Resilient link control task  */
#define SYS_BLD_TELNET_D_TASK           "TELNET_D"     /* Telnet task                  */
#define SYS_BLD_TELNET_SHELL_D_TASK     "TELNET_S"     /* telnet shell deamon        */
#define SYS_BLD_TELNET_CHILD_TASK       "TN%02d"       /* Telnet child task            */
#define SYS_BLD_TELNET_SHELL_CHILD_TASK "UI%02d"       /* Telnet shell child task      */
#define SYS_BLD_SNMP_TASK               "SNMP"         /* SNMP task                    */
#define SYS_BLD_HTTP_TASK               "HTTP"         /* HTTP task                    */
#define SYS_BLD_HTTP_ASYNC_GET_CERT_TASK "HTTP_GET"    /* HTTP async get-cert task */
#define SYS_BLD_WEBAUTH_TASK            "WEBAUTH"      /* WEBAUTH task                */
#define SYS_BLD_PING_TASK               "PING"         /* Ping task                    */
#define SYS_BLD_TRAPMGR_TASK            "TRAPMGR"      /* Trap management task         */
#define SYS_BLD_NMTR_TASK               "NMTR"         /* Network monitor task         */
#define SYS_BLD_AMTR_TASK               "AMTR"         /* Address monitor task         */
#define SYS_BLD_AMTRDRV_TASK            "AMTRDRV"      /* task to sync to HISAM and delete group of addresses. used in Hardware Learn */
#define SYS_BLD_AMTRDRV_ADDRESS_TASK       "AMTR_ADDRESS"      /* task to process NA and ageout. used in Software Learn */
#define SYS_BLD_AMTRDRV_ASIC_COMMAND_TASK  "AMTR_ASIC_COM" /* task to program ASIC's ARL table and process command block. used in Software Learn */
#define SYS_BLD_AMTRL3_TASK             "AMTRL3"       /* L3 host/net route task    */
#define SYS_BLD_LEDMGR_TASK             "LEDMGR"       /* LED Manager task             */
#define SYS_BLD_LEDDRV_TASK             "LEDDRV"       /* LED driver task              */
#define SYS_BLD_TFTP_DOWNLOAD_TASK      "DNLD"         /* TFTP download task           */
#define SYS_BLD_FXFER_TASK              "FXFER"        /* File Transfer task           */
#define SYS_BLD_EE_DRV_TASK             "EE_DRV"       /* E2prom programming task      */
#define SYS_BLD_EE_SYN_TASK             "EE_SYN"       /* E2prom data sync. task       */
#define SYS_BLD_SYS_SYN_TASK            "SYS_SYN"      /* image file data sync. task   */
#define SYS_BLD_BOOTP_TASK              "BOOTP"        /* BootP task                   */
#define SYS_BLD_DHCP_TASK               "DHCP"         /* DHCP task                    */
#define SYS_BLD_DHCPSNP_TASK            "DHCPSNP"      /* DHCPSNP task */
#define SYS_BLD_UDPHELPER_TASK          "UDPHELPER"    /* UDP Helper */
#define SYS_BLD_SNTP_TASK               "SNTP"         /* SNTP task   2002.08.16       */

#if 0 /* name changed to KEYGEN, Zhong Qiyao, 2004.02.19 */
#define SYS_BLD_HTTP_PKEYGEN_TASK       "HTTPS_PKEYGEN"/* HTTPS Generate private key task */
#endif

#define SYS_BLD_KEYGEN_MAIN_TASK        "KEYGEN"       /* Key generation task priority */

#define SYS_BLD_L2MCAST_TASK            "L2MCAST"      /* L2MCAST task                 */
#define SYS_BLD_CALLBACK_TASK           "CALLBACK"     /* CALLBACK task                */
#define SYS_BLD_VLAN_TASK               "VLAN"         /* VLAN task                    */
#define SYS_BLD_CFGDB_TASK              "CFGDB"        /* Config Database task         */

#define SYS_BLD_LLDP_TASK               "LLDP"         /* LLDP task */

/* BCMDRV task*/
#define SYS_BLD_ADMDRV_TASK             "ADMDRV_TASK"
#define SYS_BLD_BCMDRV_COUNTER_TASK1    "BCM_CNT1"
#define SYS_BLD_BCMDRV_COUNTER_TASK2    "BCM_CNT2"
#define SYS_BLD_BCMDRV_BACKDOOR_TASK    "BCM_DBG"

#define SYS_BLD_DNS_PROXY_TASK          "DNS_PROXY"
#define SYS_BLD_DNS_RESOLVER_TASK       "DNS_RES"

/* DBSYNC_TXT task */
#define SYS_BLD_DBSYNC_TXT_TASK         "DBSYNC_TXT"

/* Trace Route Task */
#define SYS_BLD_TRACEROUTE_TASK         "TRACEROUTE"

#define SYS_BLD_POEDRV_TASK             "POEDRV_TASK"
#define SYS_BLD_POE_PROCESS_NAME        "POE_PROC"
#define SYS_BLD_POE_TASK                "POE_TASK"

#define SYS_BLD_HSRP_TASK               "HSRP"         /* HSRP task                 */
#define SYS_BLD_VRRP_TASK               "VRRP"         /* VRRP task                 */

#define SYS_BLD_EFM_OAM_TASK		"EFM_OAM"

/* System performance parameters
 * Notes: 1. The system performance parameters, such as time ticks to update statistics
 *           will affect other component (for example: LED) has to be defined here.
 *        2. One system tick is 10 msec.
 */
#define SYS_BLD_TICKS_PER_SECOND                                100

#define SYS_BLD_AMTR_UPDATE_ADDR_TABLE_TICKS                    1  /*Change by Tony.Lei*/
/* Yongxin.Zhao added, temp use, change from 1 to 100, avoid system disaster, 2009-06-25, 18:14:47 */
#define SYS_BLD_AMTR_UPDATE_ADDR_TABLE_TICKS_FOR_ES4308MA                    100

#define SYS_BLD_AMTR_MGR_SYNC_HASH2HISAM_TICKS                  100
#define SYS_BLD_AMTR_NBR_OF_ADDR_TO_READ_IN_ONE_SCAN            50
#define SYS_BLD_AMTR_NBR_OF_ADDR_TO_SYNC_IN_ONE_SCAN            120
#define SYS_BLD_AMTR_NBR_OF_ITERATION_TO_START_HW_ARL_SYNC      300
#define SYS_BLD_AMTR_NBR_OF_ITERATION_FOR_SYNC_INTERVAL         5
#define SYS_BLD_AMTR_MAX_ADDR_AGE_DISTANCE                      4
#define SYS_BLD_NMTR_UPDATE_STATISTICS_TICKS                    100

#define SYS_BLD_AMTRL3_UPDATE_ADDR_TABLE_TICKS                  32
#define SYS_BLD_PSEC_LOOKUP_INTERVAL_TICKS                      50
#define SYS_BLD_SWDRV_UPDATE_PORT_STATE_TICKS                   50          /* the interval for Switch Driver  update port state */
#define SYS_BLD_SWDRV_UPDATE_PORT_STATE_INTERVAL                  1         /*Unit:second, the interval for Switch Driver  update port state */
#define SYS_BLD_IDLE_TASK_SLEEP_PERIOD                          100         /* ten ticks for 100ms              */
#define SYS_BLD_CFGDB_PERIODIC_POLLING_TICKS                    500         /* Config Database periodic polling            */
#define SYS_BLD_SYSLOG_LOOKUP_INTERVAL_TICKS                    500         /* per 500 ticks to lookup table full or not */
#define SYS_BLD_SMTP_LOOKUP_INTERVAL_TICKS                      3000        /* per 3000 ticks to lookup queue empty or not */
#define SYS_BLD_STKTPLG_UPDATE_MODULE_TICKS                     200         /* STKTPLG detect hot insert/remove tcicks */
#define SYS_BLD_SYSLOG_MAX_UPDATE_LOGFILE_TIME_SEC              (60*60)     /* Maximum 60 minutes to update the UC log information
                                                                             * to logfile in file system even the UC database not full
                                                                             */
#define SYS_BLD_POEDRV_UPDATE_POE_STATS_TICKS                   150         /* 10ms per tick   */
#define SYS_BLD_DEBUG_TASK_SLEEP_PERIOD                         500

/*      2001.11.05, Modified by Jason.  Suggested by William.
 *  Why 64?  For ip reassembly packet, since Phase 2 will queue fragment packet
 *  until collect all of the fragment packets.  We need to prepare enough rx
 *  packet buffer size for this special purpose.
 */
#define SYS_BLD_MAX_LAN_RX_BUF_SIZE_PER_PACKET          1600


/***************************************
 *  key definition for message queues  *
 ***************************************
 */

/* base for key definition of CSC Group message queues
 */
#define SYS_BLD_CSCGROUP_IPCMSGQ_KEY_BASE   1

/* base for key definition of OM message queues
 */
#define SYS_BLD_OM_IPCMSGQ_KEY_BASE                       1000

#define SYS_BLD_CSC_MSGQ_KEY_BASE                       2000

/* key definitions for OM message queues will start from SYS_BLD_OM_IPCMSGQ_KEY_BASE
 */
enum
{
    SYS_BLD_STKCTRL_PROC_OM_IPCMSGQ_KEY = SYS_BLD_OM_IPCMSGQ_KEY_BASE,
    SYS_BLD_STKTPLG_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_AUTH_PROTOCOL_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_L2_L4_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_IP_SERVICE_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_NETCFG_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_CLI_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_WEB_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_DRIVER_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_SYS_MGMT_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_CORE_UTIL_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_XFER_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_NSM_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_APP_PROTOCOL_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_RIP_PROC_OM_IPCMSGQ_KEY,/*Lin.Li, for RIP porting*/
    SYS_BLD_OSPF_PROC_OM_IPCMSGQ_KEY,/*Lin.Li, for OSPF porting*/
    SYS_BLD_OSPF6_PROC_OM_IPCMSGQ_KEY,/* added by steven.gao, for OSPF6 porting*/
    SYS_BLD_PIM_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_VRRP_PROC_OM_IPCMSGQ_KEY, /*Donny.Li for vrrp*/
    SYS_BLD_POE_PROC_OM_IPCMSGQ_KEY,
    SYS_BLD_SFLOW_PROC_OM_IPCMSGQ_KEY, /* Nelson Dai, for sFlow */
    SYS_BLD_DOT1X_SUP_PROC_OM_IPCMSGQ_KEY,
};

/* key definitions for CSC Group message queues will start from SYS_BLD_CSCGROUP_IPCMSGQ_KEY_BASE
 */
enum
{
    SYS_BLD_NSM_GROUP_IPCMSGQ_KEY = SYS_BLD_CSCGROUP_IPCMSGQ_KEY_BASE,
    SYS_BLD_STKCTRL_GROUP_IPCMSGQ_KEY,
    SYS_BLD_STKTPLG_GROUP_IPCMSGQ_KEY,
    SYS_BLD_AUTH_PROTOCOL_GROUP_IPCMSGQ_KEY,
    SYS_BLD_SNMP_GROUP_IPCMSGQ_KEY,
    SYS_BLD_GVRP_GROUP_IPCMSGQ_KEY,
    SYS_BLD_L2MCAST_GROUP_IPCMSGQ_KEY,
    SYS_BLD_IP_SERVICE_GROUP_IPCMSGQ_KEY,
    SYS_BLD_LACP_GROUP_IPCMSGQ_KEY,
    SYS_BLD_OAM_GROUP_IPCMSGQ_KEY,
    SYS_BLD_OAM_GROUP_LBK_IPCMSGQ_KEY,
    SYS_BLD_NETACCESS_GROUP_IPCMSGQ_KEY,
    SYS_BLD_NETACCESS_NMTR_IPCMSGQ_KEY,
    SYS_BLD_NETCFG_GROUP_IPCMSGQ_KEY,
    SYS_BLD_STA_GROUP_IPCMSGQ_KEY,
    SYS_BLD_SWCTRL_GROUP_IPCMSGQ_KEY,
    SYS_BLD_WEB_GROUP_IPCMSGQ_KEY,
    SYS_BLD_TELNET_GROUP_IPCMSGQ_KEY,
    SYS_BLD_UTILITY_GROUP_IPCMSGQ_KEY,
    SYS_BLD_SSH_GROUP_IPCMSGQ_KEY,
    SYS_BLD_UI_MGR_GROUP_IPCMSGQ_KEY,
    SYS_BLD_XFER_GROUP_IPCMSGQ_KEY,
    SYS_BLD_DRIVER_GROUP_IPCMSGQ_KEY,
    SYS_BLD_DRIVER_GROUP_SENDPACKET_IPCMSGQ_KEY,
    SYS_BLD_L2MUX_GROUP_IPCMSGQ_KEY,
    SYS_BLD_SYSMGMT_GROUP_IPCMSGQ_KEY,
    SYS_BLD_CLI_GROUP_IPCMSGQ_KEY,
    SYS_BLD_L4_GROUP_IPCMSGQ_KEY,
    SYS_BLD_CLUSTER_GROUP_IPCMSGQ_KEY,
    SYS_BLD_LEDMGMT_GROUP_IPCMSGQ_KEY,
    SYS_BLD_CFGDB_GROUP_IPCMSGQ_KEY,
    SYS_BLD_AMTRL3_GROUP_IPCMSGQ_KEY,
    SYS_BLD_APP_PROTOCOL_GROUP_IPCMSGQ_KEY,
    SYS_BLD_DHCPSNP_GROUP_IPCMSGQ_KEY,
    SYS_BLD_PIM_GROUP_IPCMSGQ_KEY,
    SYS_BLD_RIP_GROUP_IPCMSGQ_KEY,/*Lin.Li, for RIP porting*/
    SYS_BLD_OSPF_GROUP_IPCMSGQ_KEY,/*Lin.Li, for OSPF porting*/
    SYS_BLD_OSPF6_GROUP_IPCMSGQ_KEY,/* added by steven.gao, for OSPF6 porting*/
    SYS_BLD_VRRP_GROUP_IPCMSGQ_KEY,/*wang.tong, for VRRP porting*/
    SYS_BLD_UDPHELPER_GROUP_IPCMSGQ_KEY,/*Lin.Li for udp helper*/
    SYS_BLD_DAI_GROUP_IPCMSGQ_KEY,
    SYS_BLD_CFM_GROUP_IPCMSGQ_KEY,
    SYS_BLD_IGMP_GROUP_IPCMSGQ_KEY,/* Jimi Chen, for IGMP */
    SYS_BLD_WEBAUTH_GROUP_IPCMSGQ_KEY,
    SYS_BLD_POE_GROUP_IPCMSGQ_KEY,
    SYS_BLD_SFLOW_GROUP_IPCMSGQ_KEY, /* Nelson Dai, for sFlow */
    SYS_BLD_DOT1X_SUP_GROUP_IPCMSGQ_KEY,
    SYS_BLD_PPPOE_IA_GROUP_IPCMSGQ_KEY,
    SYS_BLD_ERPS_GROUP_IPCMSGQ_KEY,
    SYS_BLD_CMGR_GROUP_IPCMSGQ_KEY,
    SYS_BLD_DRIVER_GROUP_FREEPACKET_IPCMSGQ_KEY,
    SYS_BLD_CSCGROUP_IPCMSGQ_KEY_MAX
};

/* key definitions for CSC specific message queues will start from SYS_BLD_CSC_MSGQ_KEY_BASE
 */
enum
{
    SYS_BLD_CSC_DOT1X_TASK_MSGQ_KEY= SYS_BLD_CSC_MSGQ_KEY_BASE,
    SYS_BLD_CSC_DOT1X_RADIUS_MSGQ_KEY,
    SYS_BLD_CSC_DOT1X_ASYNC_MSGQ_KEY,
    SYS_BLD_CSC_PSEC_TASK_MSGQ_KEY,
    SYS_BLD_CSC_KEYGEN_TASK_MSGQ_KEY,
    SYS_BLD_CSC_XFER_MSGQ_KEY,
    SYS_BLD_CSC_XFER_DNLD_MSGQ_KEY,
    SYS_BLD_CSC_TACACS_TASK_EVENT_ACC_REQ,
    SYS_BLD_CSC_NETACCESS_TASK_NEW_MAC_MSGQ_KEY,
    SYS_BLD_CSC_NETACCESS_TASK_DOT1X_MSGQ_KEY,
    SYS_BLD_CSC_NETACCESS_TASK_RADIUS_MAC_MSGQ_KEY,
    SYS_BLD_CSC_NETACCESS_TASK_LINK_STCNG_MSGQ_KEY,
    SYS_BLD_CSC_NETACCESS_TASK_VLAN_CNG_MSGQ_KEY,
    SYS_BLD_CSC_NETACCESS_TASK_MAC_AGE_OUT_MSGQ_KEY,
    SYS_BLD_CSC_DHCPSNP_TASK_MSGQ_KEY,
    SYS_BLD_CSC_MSL_TASK_MSGK_KEY,
    SYS_BLD_CSC_STKTPLG_TASK_MSGK_KEY,
    SYS_BLD_CSC_IPCMSGQ_KEY_MAX
};

/***********************************************************
 ** Process/Thread priority and sched policy definitions  **
 ***********************************************************
 */
/* Note: Priority level ranges from 1 ~ 255
 * where priority 1 is the highest priority in SYS_BLD.H.
 */

/* Common Definitions for OM thread
 */
#define SYS_BLD_OM_THREAD_PRIORITY                          SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_OM_THREAD_SCHED_POLICY                      SYSFUN_SCHED_DEFAULT

/* Commmon Definitions for Backdoor thread
 */
#define SYS_BLD_BACKDOOR_THREAD_PRIORITY                    SYS_BLD_CLI_TASK_PRIORITY
#define SYS_BLD_BACKDOOR_THREAD_SCHED_POLICY                SYSFUN_SCHED_DEFAULT

#define SYS_BLD_STKTPLG_GROUP_MGR_THREAD_PRIORITY           (SYS_BLD_PROCESS_DEFAULT_PRIORITY - 10)     /* must be higher than all thread that use default schedule */
#define SYS_BLD_STKTPLG_GROUP_MGR_THREAD_SCHED_POLICY       SYSFUN_SCHED_DEFAULT

#define SYS_BLD_LACP_GROUP_MGR_THREAD_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_LACP_GROUP_MGR_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_STA_GROUP_MGR_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_STA_GROUP_MGR_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_NETACCESS_GROUP_MGR_THREAD_PRIORITY         SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_NETACCESS_GROUP_MGR_SCHED_POLICY            SYSFUN_SCHED_DEFAULT
/*add by Tony.Lei*/
#define SYS_BLD_NETACCESS_GROUP_MGR_THREAD_PRIORITY1        SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_NETACCESS_GROUP_MGR_SCHED_POLICY1           SYSFUN_SCHED_DEFAULT
/*Fix Bug ES4827G-FLF-ZZ-00464*/
#define SYS_BLD_NETACCESS_GROUP_MGR_THREAD_HASH2HSIAM_PRIORITY 138
#define SYS_BLD_NETACCESS_GROUP_MGR_HASH2HSIAM_SCHED_POLICY    SYSFUN_SCHED_DEFAULT
/*end Tony.Lei*/
#define SYS_BLD_DOT1X_CSC_THREAD_PRIORITY                   SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_DOT1X_CSC_THREAD_SCHED_POLICY               SYSFUN_SCHED_DEFAULT

#define SYS_BLD_L4_GROUP_MGR_THREAD_PRIORITY                SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_L4_GROUP_MGR_SCHED_POLICY                   SYSFUN_SCHED_DEFAULT

#define SYS_BLD_AMTRL3_GROUP_MGR_THREAD_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_AMTRL3_GROUP_MGR_SCHED_POLICY               SYSFUN_SCHED_DEFAULT
#define SYS_BLD_AMTRL3_CSC_THREAD_PRIORITY                  SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* AMTRL3 task priority                 */
#define SYS_BLD_AMTRL3_CSC_THREAD_SCHED_POLICY              SYSFUN_SCHED_DEFAULT

#define SYS_BLD_UDPHELPER_GROUP_MGR_THREAD_PRIORITY         SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_UDPHELPER_GROUP_MGR_SCHED_POLICY            SYSFUN_SCHED_DEFAULT
#define SYS_BLD_UDPHELPER_CSC_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* AMTRL3 task priority                 */
#define SYS_BLD_UDPHELPER_CSC_THREAD_SCHED_POLICY           SYSFUN_SCHED_DEFAULT

#define SYS_BLD_DAI_GROUP_MGR_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_DAI_GROUP_MGR_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT
#define SYS_BLD_DAI_CSC_THREAD_PRIORITY                     SYS_BLD_PROCESS_DEFAULT_PRIORITY     /* AMTRL3 task priority                 */
#define SYS_BLD_DAI_CSC_THREAD_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_STKCTRL_GROUP_MGR_THREAD_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_STKCTRL_GROUP_MGR_THREAD_SCHED_POLICY       SYSFUN_SCHED_DEFAULT
#define SYS_BLD_CFM_GROUP_MGR_THREAD_PRIORITY               108
#define SYS_BLD_CFM_GROUP_MGR_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_PPPOE_IA_GROUP_MGR_THREAD_PRIORITY          108
#define SYS_BLD_PPPOE_IA_GROUP_MGR_SCHED_POLICY             SYSFUN_SCHED_DEFAULT

#define SYS_BLD_ERPS_GROUP_MGR_THREAD_PRIORITY              108
#define SYS_BLD_ERPS_GROUP_MGR_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_AUTH_PROTOCOL_GROUP_MGR_THREAD_PRIORITY     SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_AUTH_PROTOCOL_GROUP_MGR_SCHED_POLICY        SYSFUN_SCHED_DEFAULT

#define SYS_BLD_TACACS_CSC_THREAD_PRIORITY                  SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_TACACS_CSC_THREAD_SCHED_POLICY              SYSFUN_SCHED_DEFAULT

#define SYS_BLD_XFER_GROUP_MGR_THREAD_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_XFER_GROUP_MGR_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_DBSYNC_TXT_CSC_THREAD_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_DBSYNC_TXT_CSC_THREAD_SCHED_POLICY          SYSFUN_SCHED_DEFAULT

#define SYS_BLD_UTILITY_GROUP_MGR_THREAD_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_UTILITY_GROUP_MGR_SCHED_POLICY              SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SYSLOG_THREAD_PRIORITY                      SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SYSLOG_THREAD_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_CFGDB_GROUP_MGR_THREAD_PRIORITY             SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_CFGDB_GROUP_MGR_SCHED_POLICY                SYSFUN_SCHED_DEFAULT


#define SYS_BLD_TELNET_GROUP_MGR_THREAD_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_TELNET_GROUP_MGR_SCHED_POLICY               SYSFUN_SCHED_DEFAULT

#define SYS_BLD_TELNET_SHELL_THREAD_PRIORITY                SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_TELNET_THREAD_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT
#define SYS_BLD_TELNET_DAEMON_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
/* Add telnet client, Jinnee.Jin, 2008, 3, 5 */
#define SYS_BLD_TELNET_CLIENT_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY

#define SYS_BLD_SSH_GROUP_MGR_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SSH_GROUP_MGR_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SSHD_CSC_THREAD_PRIORITY                    SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SSHD_CSC_THREAD_SCHED_POLICY                SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SSHD_CSC_CHILD_THREAD_PRIORITY              SYS_BLD_SSHD_CSC_THREAD_PRIORITY
#define SYS_BLD_SSHD_CSC_CHILD_THREAD_SCHED_POLICY          SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SSHD_CSC_ASYNC_GEN_HOST_KEY_THREAD_PRIORITY     SYS_BLD_SSHD_ASYNC_GEN_HOST_KEY_TASK_PRIORITY
#define SYS_BLD_SSHD_CSC_ASYNC_GEN_HOST_KEY_SCHED_POLICY        SYSFUN_SCHED_DEFAULT
#define SYS_BLD_SSHD_CSC_ASYNC_DEL_USER_KEY_THREAD_PRIORITY     SYS_BLD_SSHD_MAIN_TASK_PRIORITY /* SSHD async del-user-key task priority */
#define SYS_BLD_SSHD_CSC_ASYNC_DEL_USER_KEY_SCHED_POLICY        SYSFUN_SCHED_DEFAULT
#define SYS_BLD_SSHD_CSC_ASYNC_WRITE_HOST_KEY_THREAD_PRIORITY   SYS_BLD_SSHD_MAIN_TASK_PRIORITY /* SSHD async write-host-key task priority */
#define SYS_BLD_SSHD_CSC_ASYNC_WRITE_HOST_KEY_SCHED_POLICY      SYSFUN_SCHED_DEFAULT

#define SYS_BLD_KEYGEN_CSC_THREAD_PRIORITY                  SYS_BLD_LOWEST_TASK_PRIORITY
#define SYS_BLD_KEYGEN_CSC_THREAD_SCHED_POLICY              SYSFUN_SCHED_DEFAULT

#define SYS_BLD_WEBGROUP_MGR_THREAD_PRIORITY                SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_WEBGROUP_MGR_SCHED_POLICY                   SYSFUN_SCHED_DEFAULT

#define SYS_BLD_HTTP_CSC_THREAD_PRIORITY                    SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_HTTP_CSC_THREAD_SCHED_POLICY                SYSFUN_SCHED_DEFAULT
#define SYS_BLD_HTTP_ASYNC_GET_CERT_THREAD_PRIORITY         SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_HTTP_ASYNC_GET_CERT_THREAD_SCHED_POLICY     SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SNMP_GROUP_MGR_THREAD_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SNMP_GROUP_MGR_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_CLUSTER_GROUP_MGR_THREAD_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_CLUSTER_GROUP_MGR_SCHED_POLICY              SYSFUN_SCHED_DEFAULT

#define SYS_BLD_CLUSTER_CSC_THREAD_PRIORITY                 SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_CLUSTER_CSC_THREAD_SCHED_POLICY             SYSFUN_SCHED_DEFAULT

#define SYS_BLD_NETCFG_GROUP_MGR_THREAD_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY /* we do not have NETCFG thread before, just set 130 temporarily */
#define SYS_BLD_NETCFG_GROUP_MGR_SCHED_POLICY               SYSFUN_SCHED_DEFAULT

#define SYS_BLD_NSM_GROUP_MGR_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_NSM_GROUP_MGR_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_NSM_CSC_THREAD_PRIORITY                     SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_NSM_CSC_THREAD_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT
/*Lin.Li, for RIP porting, modify start*/
#define SYS_BLD_RIP_GROUP_MGR_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_RIP_GROUP_MGR_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_RIP_CSC_THREAD_PRIORITY                     SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_RIP_CSC_THREAD_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT
/*Lin.Li, for RIP porting, modify end*/

/*Lin.Li, for OSPF porting, modify start*/
#define SYS_BLD_OSPF_GROUP_MGR_THREAD_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_OSPF_GROUP_MGR_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_OSPF_CSC_THREAD_PRIORITY                    SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_OSPF_CSC_THREAD_SCHED_POLICY                SYSFUN_SCHED_DEFAULT
/*Lin.Li, for OSPF porting, modify end*/

/* added by steven.gao, for OSPF6 porting */
#define SYS_BLD_OSPF6_GROUP_MGR_THREAD_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_OSPF6_GROUP_MGR_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_OSPF6_CSC_THREAD_PRIORITY                    SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_OSPF6_CSC_THREAD_SCHED_POLICY                SYSFUN_SCHED_DEFAULT
/* end of added by steven.gao, for OSPF6 porting */


/*Donny.Li, for VRRP porting, modify start*/
#define SYS_BLD_VRRP_GROUP_MGR_THREAD_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_VRRP_GROUP_MGR_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_VRRP_CSC_THREAD_PRIORITY                    SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_VRRP_CSC_THREAD_SCHED_POLICY                SYSFUN_SCHED_DEFAULT
/*Donny.Li, for VRRP porting, modify end*/


#define SYS_BLD_PIM_GROUP_MGR_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_PIM_GROUP_MGR_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_PIM_CSC_THREAD_PRIORITY                     SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_PIM_CSC_THREAD_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_PIM6_CSC_THREAD_PRIORITY                     SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_PIM6_CSC_THREAD_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

/*changed by fen.wang,when 1-23 ports added to 4kvlan, gvrp and l2mcast will cannot receive msg in time
so the queue is easy to queue-full*/
#define SYS_BLD_GVRP_GROUP_MGR_THREAD_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_GVRP_GROUP_MGR_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_L2MCAST_GROUP_MGR_THREAD_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_L2MCAST_GROUP_MGR_SCHED_POLICY              SYSFUN_SCHED_DEFAULT

#define SYS_BLD_MSL_THREAD_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_MSL_SCHED_POLICY              SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SWCTRL_GROUP_MGR_THREAD_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SWCTRL_GROUP_MGR_SCHED_POLICY               SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SWCTRL_CSC_THREAD_PRIORITY                  SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SWCTRL_CSC_THREAD_SCHED_POLICY              SYSFUN_SCHED_DEFAULT

#define SYS_BLD_CLI_THREAD_PRIORITY                         SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_CLI_THREAD_SCHED_POLICY                     SYSFUN_SCHED_DEFAULT

#define SYS_BLD_PORT_SECURITY_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_PORT_SECURITY_THREAD_SCHED_POLICY           SYSFUN_SCHED_DEFAULT

#define SYS_BLD_RADIUS_THREAD_PRIORITY                      SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_RADIUS_THREAD_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_NIC_THREAD_PRIORITY                         SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_NIC_THREAD_SCHED_POLICY                     SYSFUN_SCHED_DEFAULT

#define SYS_BLD_ISC_AGENT_HIGH_SPEED_THREAD_PRIORITY        SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_ISC_AGENT_HIGH_SPEED_THREAD_SCHED_POLICY    SYSFUN_SCHED_DEFAULT

#define SYS_BLD_ISC_AGENT_LOW_SPEED_THREAD_PRIORITY         SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_ISC_AGENT_LOW_SPEED_THREAD_SCHED_POLICY     SYSFUN_SCHED_DEFAULT

#define SYS_BLD_GARP_THREAD_PRIORITY                        SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_GARP_THREAD_SCHED_POLICY                    SYSFUN_SCHED_DEFAULT

#define SYS_BLD_LLDP_THREAD_PRIORITY                        SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_LLDP_THREAD_SCHED_POLICY                    SYSFUN_SCHED_DEFAULT

#define SYS_BLD_L2MUX_GROUP_MGR_THREAD_PRIORITY             SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_L2MUX_GROUP_MGR_SCHED_POLICY                SYSFUN_SCHED_DEFAULT

#define SYS_BLD_IML_RX_THREAD_PRIORITY                      SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_IML_RX_THREAD_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_IML_TX_THREAD_PRIORITY                      SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_IML_TX_THREAD_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_IGMP_SNOOPING_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_IGMP_SNOOPING_THREAD_SCHED_POLICY           SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SYSMGMT_GROUP_MGR_THREAD_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SYSMGMT_GROUP_MGR_SCHED_POLICY              SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SYSMGMT_THREAD_PRIORITY                     SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SYSMGMT_THREAD_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_AMTRDRV_ADDRESS_THREAD_PRIORITY             SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_AMTRDRV_ADDRESS_THREAD_SCHED_POLICY         SYSFUN_SCHED_DEFAULT

#define SYS_BLD_AMTRDRV_ASIC_COMMAND_THREAD_PRIORITY        SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_AMTRDRV_ASIC_COMMAND_THREAD_SCHED_POLICY    SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SNMP_CSC_THREAD_PRIORITY                    SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SNMP_CSC_THREAD_SCHED_POLICY                SYSFUN_SCHED_DEFAULT

#define SYS_BLD_DRIVER_GROUP_MGR_THREAD_PRIORITY            SYS_BLD_REALTIMEPROCESS_DEFAULT_PRIORITY
#define SYS_BLD_DRIVER_GROUP_MGR_SCHED_POLICY               SYSFUN_SCHED_RR

#define SYS_BLD_IP_SERVICE_GROUP_MGR_THREAD_PRIORITY        SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_IP_SERVICE_GROUP_MGR_SCHED_POLICY           SYSFUN_SCHED_DEFAULT

#define SYS_BLD_NMTRDRV_THREAD_PRIORITY                     SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_NMTRDRV_THREAD_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SYSDRV_THREAD_PRIORITY                      SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SYSDRV_THREAD_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_TFTP_DOWNLOAD_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_TFTP_DOWNLOAD_THREAD_SCHED_POLICY           SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SMTP_CSC_THREAD_PRIORITY                    SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SMTP_CSC_THREAD_SCHED_POLICY                SYSFUN_SCHED_DEFAULT

#define SYS_BLD_SNTP_CSC_THREAD_PRIORITY                    SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SNTP_CSC_THREAD_SCHED_POLICY                SYSFUN_SCHED_DEFAULT

#define SYS_BLD_DNS_RESOLVER_CSC_THREAD_PRIORITY            SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_DNS_RESOLVER_CSC_THREAD_SCHED_POLICY        SYSFUN_SCHED_DEFAULT

#define SYS_BLD_DNS_PROXY_CSC_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_DNS_PROXY_CSC_THREAD_SCHED_POLICY           SYSFUN_SCHED_DEFAULT

#define SYS_BLD_APP_PROTOCOL_GROUP_MGR_THREAD_PRIORITY      SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_APP_PROTOCOL_GROUP_MGR_SCHED_POLICY         SYSFUN_SCHED_DEFAULT

#define SYS_BLD_OAM_GROUP_MGR_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY /* TBD */
#define SYS_BLD_OAM_GROUP_MGR_SCHED_POLICY                  SYSFUN_SCHED_DEFAULT

#define SYS_BLD_OAM_XMITLBK_THREAD_PRIORITY                 SYS_BLD_PROCESS_DEFAULT_PRIORITY /* TBD */
#define SYS_BLD_OAM_XMITLBK_SCHED_POLICY                    SYSFUN_SCHED_DEFAULT

#define SYS_BLD_NETACCESS_CSC_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_NETACCESS_CSC_THREAD_SCHED_POLICY           SYSFUN_SCHED_DEFAULT

#define SYS_BLD_DHCPSNP_GROUP_MGR_THREAD_PRIORITY           SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_DHCPSNP_GROUP_MGR_SCHED_POLICY              SYSFUN_SCHED_DEFAULT

#define SYS_BLD_DHCPSNP_CSC_THREAD_PRIORITY                 SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_DHCPSNP_CSC_THREAD_SCHED_POLICY             SYSFUN_SCHED_DEFAULT

#define SYS_BLD_DHCPV6SNP_CSC_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_DHCPV6SNP_CSC_THREAD_SCHED_POLICY           SYSFUN_SCHED_DEFAULT

/* Joeanne add for LED */
#define SYS_BLD_LED_MGR_THREAD_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_LED_MGR_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_WEBAUTH_CSC_THREAD_PRIORITY                 SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_WEBAUTH_CSC_SCHED_POLICY                    SYSFUN_SCHED_DEFAULT

/* Nelson Dai add for sFlow */
#define SYS_BLD_SFLOW_GROUP_MGR_THREAD_PRIORITY             SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SFLOW_GROUP_MGR_SCHED_POLICY                SYSFUN_SCHED_DEFAULT
#define SYS_BLD_SFLOW_CSC_THREAD_PRIORITY                   SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_SFLOW_CSC_SCHED_POLICY                      SYSFUN_SCHED_DEFAULT

#define SYS_BLD_DOT1X_SUP_GROUP_MGR_THREAD_PRIORITY         SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_DOT1X_SUP_GROUP_MGR_SCHED_POLICY            SYSFUN_SCHED_DEFAULT

#define SYS_BLD_DYING_GASP_THREAD_PRIORITY                  SYS_BLD_REALTIMEPROCESS_HIGHEST_PRIORITY
#define SYS_BLD_DYING_GASP_THREAD_SCHED_POLICY              SYSFUN_SCHED_FIFO

#define SYS_BLD_CRAFT_PORT_TX_THREAD_PRIORITY               SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_CRAFT_PORT_TX_THREAD_SCHED_POLICY           SYSFUN_SCHED_DEFAULT

#define SYS_BLD_CMGR_GROUP_MGR_THREAD_PRIORITY              SYS_BLD_PROCESS_DEFAULT_PRIORITY
#define SYS_BLD_CMGR_GROUP_MGR_SCHED_POLICY                 SYSFUN_SCHED_DEFAULT

#define SYS_BLD_WTDOG_PROCESS_PRIORITY                      SYS_BLD_REALTIMEPROCESS_2ND_HIGH_PRIORITY

/***************************************
 **  Process/Thread Name Definitions  **
 ***************************************
 */
#define SYS_BLD_NSM_CSC_THREAD_NAME                 "NSM_TD"
#define SYS_BLD_NSM_GROUP_MGR_THREAD_NAME           "NSM_GROUP"

#define SYS_BLD_BACKDOOR_MGR_THREAD_NAME            "BACKDOOR_MGR"
#define SYS_BLD_BACKDOOR_CSC_THREAD_NAME            "BACKDOOR_TD"

#define SYS_BLD_CLI_PROCES_NAME                     "CLI_PROC"
#define SYS_BLD_CLI_GROUP_MGR_THREAD_NAME           "CLI_GROUP"
#define SYS_BLD_TELNET_GROUP_MGR_THREAD_NAME        "TELNET_GROUP"
#define SYS_BLD_SSH_GROUP_MGR_THREAD_NAME           "SSH_GROUP"
/* shumin.wang added for ui_mgr 2008-01-09 */
#define SYS_BLD_UI_MGR_GROUP_MGR_THREAD				"UI_GROUP"

#define SYS_BLD_TELNET_SHELL_THREAD_NAME            "TELNET_SHELL_TD"
#define SYS_BLD_TELNET_DAEMON_THREAD_NAME           "TELNET_DAEMON_TD"
/* Add telnet client, Jinnee.Jin, 2008, 3, 5 */
#define SYS_BLD_TELNET_CLIENT_THREAD_NAME           "TELNET_CLIENT_TD"
#define SYS_BLD_SSHD_CSC_THREAD_NAME                "SSH_TD"

#define SYS_BLD_SSHD_CSC_ASYNC_GEN_HOST_KEY_THREAD_NAME     "SSH_GEN_TD"    /* SSHD async gen-host-key thread */
#define SYS_BLD_SSHD_CSC_ASYNC_DEL_USER_KEY_THREAD_NAME     "SSH_DEL_TD"    /* SSHD async del-user-key thread */
#define SYS_BLD_SSHD_CSC_ASYNC_WRITE_HOST_KEY_THREAD_NAME   "SSH_WRITE_TD"  /* SSHD async write-host-key thread */

#define SYS_BLD_KEYGEN_CSC_THREAD_NAME              "KEYGEN_TD"

#define SYS_BLD_DRIVER_PROCESS_NAME                 "DRIVER_PROC"
#define SYS_BLD_DRIVER_GROUP_MGR_THREAD_NAME        "DRIVER_GROUP"

#define SYS_BLD_STKTPLG_PROCESS_NAME                "STKTPLG_PROC"
#define SYS_BLD_STKTPLG_GROUP_MGR_THREAD_NAME       "STKTPLG_GROUP"

#define SYS_BLD_STKCTRL_PROCESS_NAME                "STKCTRL_PROC"
#define SYS_BLD_STKCTRL_GROUP_MGR_THREAD_NAME       "STKCTRL_GROUP"

#define SYS_BLD_AUTH_PROTOCOL_PROCESS_NAME          "AUTH_PROC"
#define SYS_BLD_AUTH_PROTOCOL_GROUP_MGR_THREAD_NAME "AUTH_GROUP"

#define SYS_BLD_SNMP_PROCESS_NAME                   "SNMP_PROC"
#define SYS_BLD_SNMP_CSC_THREAD_NAME                "SNMP_TD"

#define SYS_BLD_IP_SERVICE_PROCESS_NAME             "IP_SERVICE_PROCESS"
#define SYS_BLD_IP_SERVICE_GROUP_MGR_THREAD_NAME    "IP_SERVICE_GROUP"

#define SYS_BLD_L2_L4_PROCESS_NAME                  "L2_L4_PROCESS"
#define SYS_BLD_GVRP_GROUP_MGR_THREAD_NAME          "GVRP_GROUP"
#define SYS_BLD_LACP_GROUP_MGR_THREAD_NAME          "LACP_GROUP"
#define SYS_BLD_STA_GROUP_MGR_THREAD_NAME           "STA_GROUP"
#define SYS_BLD_L2MCAST_GROUP_MGR_THREAD_NAME       "L2MCAST_GROUP"
#define SYS_BLD_SWCTRL_GROUP_MGR_THREAD_NAME        "SWCTRL_GROUP"
#define SYS_BLD_SWCTRL_CSC_THREAD_NAME              "SWCTRL_TD"
#define SYS_BLD_L2MUX_GROUP_MGR_THREAD_NAME         "L2MUX_GROUP"
#define SYS_BLD_IML_RX_THREAD_NAME                  "IML_RX"
#define SYS_BLD_IML_TX_THREAD_NAME                  "IML_TX"
#define SYS_BLD_L4_GROUP_MGR_THREAD_NAME            "L4_GROUP"
#define SYS_BLD_IGMP_SNOOPING_THREAD_NAME           "IGMPSNP"
#define SYS_BLD_AMTRL3_GROUP_MGR_THREAD_NAME        "AMTRL3_GROUP"
#define SYS_BLD_MSL_CSC_THREAD_NAME "MSL_TD"
#define SYS_BLD_CFM_GROUP_MGR_THREAD_NAME           "CFM_GROUP"
#define SYS_BLD_PPPOE_IA_GROUP_MGR_THREAD_NAME      "PPPOE_IA_GROUP"
#define SYS_BLD_ERPS_GROUP_MGR_THREAD_NAME          "ERPS_GROUP"
#define SYS_BLD_UDPHELPER_GROUP_MGR_THREAD_NAME     "UDPHELPER_GROUP"
#define SYS_BLD_DAI_GROUP_MGR_THREAD_NAME           "DAI_GROUP"
#define SYS_BLD_NETACCESS_GROUP_MGR_THREAD_NAME     "NETACCESS_GROUP "
/* add by charles */
#define SYS_BLD_NETACCESS_NMTR_THREAD_NAME			"NETACCESS_NMTR"
/*add By Tony.lei*/
#define SYS_BLD_NETACCESS_GROUP_MGR_THREAD_NAME1    "NETACCESS_NA"
#define SYS_BLD_NETACCESS_GROUP_MGR_THREAD_HASH2HSIAM     "NETACCESS_HASH2HISAM"
/*end by Tony.Lei*/
#define SYS_BLD_PORT_SECURITY_THREAD_NAME           "PORT_SEC"
#define SYS_BLD_RADIUS_THREAD_NAME                  "RADIUS"
#define SYS_BLD_NETCFG_PROCESS_NAME                 "NETCFG_PROC"
#define SYS_BLD_NETCFG_GROUP_MGR_THREAD_NAME        "NETCFG_GROUP"

#define SYS_BLD_NSM_PROCESS_NAME                    "NSM_PROC"

/*Lin.Li, for RIP porting, modify start*/
#define SYS_BLD_RIP_PROCESS_NAME                    "RIP_PROC"
#define SYS_BLD_RIP_GROUP_MGR_THREAD_NAME           "RIP_GROUP"
#define SYS_BLD_RIP_CSC_THREAD_NAME                 "RIP_TD"
/*Lin.Li, for RIP porting, modify end*/

/*Lin.Li, for OSPF porting, modify start*/
#define SYS_BLD_OSPF_PROCESS_NAME                    "OSPF_PROC"
#define SYS_BLD_OSPF_GROUP_MGR_THREAD_NAME           "OSPF_GROUP"
#define SYS_BLD_OSPF_CSC_THREAD_NAME                 "OSPF_TD"
/*Lin.Li, for OSPF porting, modify end*/

/* added by steven.gao, for OSPF6 porting */
#define SYS_BLD_OSPF6_PROCESS_NAME                    "OSPF6_PROC"
#define SYS_BLD_OSPF6_GROUP_MGR_THREAD_NAME           "OSPF6_GROUP"
#define SYS_BLD_OSPF6_CSC_THREAD_NAME                 "OSPF6_TD"
/* end of added by steven.gao, for OSPF6 porting */


/*Donny.Li, for VRRP porting, modify start*/
#define SYS_BLD_VRRP_PROCESS_NAME                    "VRRP_PROC"
#define SYS_BLD_VRRP_GROUP_MGR_THREAD_NAME           "VRRP_GROUP"
#define SYS_BLD_VRRP_CSC_THREAD_NAME                 "VRRP_TD"
/*Donny.Li, for VRRP porting, modify end*/

#define SYS_BLD_WEB_PROCESS_NAME                    "WEB_PROC"
#define SYS_BLD_WEB_GROUP_MGR_THREAD_NAME           "WEB_GROUP"
#define SYS_BLD_HTTP_CSC_THREAD_NAME                "HTTP_TD"

#define SYS_BLD_SYS_MGMT_PROCESS_NAME               "SYS_MGMT_PROC"
#define SYS_BLD_SYS_MGMT_THREAD_NAME                "SYS_MGMT"

#define SYS_BLD_CORE_UTIL_PROCESS_NAME              "CORE_UTIL_PROC"
#define SYS_BLD_UTILITY_GROUP_MGR_THREAD_NAME       "UTILITY_GROUP"
#define SYS_BLD_SYSLOG_CSC_THREAD_NAME              "SYSLOG_TD"
#define SYS_BLD_CFGDB_GROUP_MGR_THREAD_NAME	        "CFGDB_TD"

#define SYS_BLD_XFER_PROCESS_NAME                   "XFER_PROC"
#define SYS_BLD_XFER_GROUP_MGR_THREAD_NAME          "XFER_GROUP"
#define SYS_BLD_DBSYNC_TXT_CSC_THREAD_NAME          "DBSYNC_TXT_TD"

#define SYS_BLD_SYSMGMT_GROUP_MGR_THREAD_NAME       "SYSMGMT_GROUP"

#define SYS_BLD_DOT1X_CSC_THREAD_NAME               "DOT1X_TD"

#define SYS_BLD_CLUSTER_CSC_THREAD_NAME             "CLUSTER_TD"
#define SYS_BLD_CLUSTER_GROUP_MGR_THREAD_NAME       "CLUSTER_GROUP"

#define SYS_BLD_TACACS_CSC_THREAD_NAME              "TACACS_TD"
#define SYS_BLD_TACACS_CSC_CHILD_THREAD_NAME        "TACACS_CHILD_TD"

#define SYS_BLD_NMTRDRV_THREAD                      "NMTRDRV"

#define SYS_BLD_CLI_THREAD_NAME                     "CLI_TD"

#define SYS_BLD_WEBGROUP_MGR_THREAD_NAME            "WEB_TD"

#define SYS_BLD_SYSDRV_THREAD_NAME                  "SYSDRV"

#define SYS_BLD_XFER_CSC_THREAD_NAME                "XFER_TD"

#define SYS_BLD_TFTP_DOWNLOAD_THREAD_NAME           "TFTP_DOWNLOAD_THREAD"
#define SYS_BLD_SMTP_CSC_THREAD_NAME                "SMTP_TD"
#define SYS_BLD_SNTP_CSC_THREAD_NAME                "SNTP_TD"

#define SYS_BLD_DNS_RESOLVER_CSC_THREAD_NAME        "DNS_RESOL_TD"
#define SYS_BLD_DNS_PROXY_CSC_THREAD_NAME           "DNS_PROXY_TD"

#define SYS_BLD_APP_PROTOCOL_PROCESS_NAME           "APP_PROTOCOL_PROC"
#define SYS_BLD_APP_PROTOCOL_GROUP_MGR_THREAD_NAME  "APP_PROTOCOL_GROUP"

#define SYS_BLD_OAM_GROUP_MGR_THREAD_NAME           "OAM_GROUP"
#define SYS_BLD_OAM_XMITLBK_THREAD_NAME             "OAM_TXLBK_TD"

#define SYS_BLD_NETACCESS_CSC_THREAD_NAME           "NETACCESS_TD"

#define SYS_BLD_DHCPSNP_GROUP_MGR_THREAD_NAME       "DHCPSNP_GROUP"
#define SYS_BLD_DHCPSNP_CSC_THREAD_NAME             "DHCPSNP_TD"
#define SYS_BLD_DHCPV6SNP_CSC_THREAD_NAME           "DHCPV6SNP_TD"

#define SYS_BLD_PIM_PROCESS_NAME                    "PIM_PROC"
#define SYS_BLD_PIM_GROUP_MGR_THREAD_NAME           "PIM_GROUP"
#define SYS_BLD_PIM_SM_CSC_THREAD_NAME              "PIM_SM_TD"
#define SYS_BLD_PIM_DM_CSC_THREAD_NAME              "PIM_DM_TD"
#define SYS_BLD_PIM6_SM_CSC_THREAD_NAME             "PIM6_SM_TD"
#define SYS_BLD_LED_CSC_THREAD_NAME                 "LED_TD"
#define SYS_BLD_WEBAUTH_CSC_THREAD_NAME             "WEBAUTH_TD"

/* Nelson Dai add for sFlow */
#define SYS_BLD_SFLOW_PROCESS_NAME                  "SFLOW_PROC"
#define SYS_BLD_SFLOW_GROUP_MGR_THREAD_NAME         "SFLOW_GROUP"
#define SYS_BLD_SFLOW_CSC_THREAD_NAME               "SFLOW_TD"

#define SYS_BLD_DOT1X_SUP_PROCESS_NAME             "DOT1X_SUP_PROC"
#define SYS_BLD_DOT1X_SUP_GROUP_MGR_THREAD_NAME    "DOT1X_SUP_GROUP"
#define SYS_BLD_DOT1X_SUP_CSC_THREAD_NAME          "DOT1X_SUP_TD"

#define SYS_BLD_DYING_GASP_CSC_THREAD_NAME         "DYING_GASP"
#define SYS_BLD_CRAFT_PORT_TX_THREAD_NAME          "CRAFT_PORT_TX"

#define SYS_BLD_CMGR_GROUP_MGR_THREAD_NAME         "CMGR_GROUP"

#define SYS_BLD_WTDOG_PROCESS_NAME                 "WTDOG_PROC"

/*********************************************
 **  system-wised semaphore key definition  **
 *********************************************
 */
/* SYS_BLD_SYS_SEMAPHORE_KEY_OM will be used to protect shared memory and all OMs
 */
#define SYS_BLD_SYS_SEMAPHORE_KEY_OM                1
#define SYS_BLD_SYS_SEMAPHORE_KEY_AMTR              2
#define SYS_BLD_SYS_SEMAPHORE_KEY_AMTRDRV_OM        3 /*Tony.Lei*/
#define SYS_BLD_SYS_SEMAPHORE_KEY_AMTR_OM           4 /*Tony.Lei*/
#define SYS_BLD_SYS_SEMAPHORE_STKTPLG_SHOM          5
#define SYS_BLD_SYS_SEMAPHORE_STKTPLG_OM            6
#define SYS_BLD_SYS_SEMAPHORE_VLAN_OM               7
/*new*/
#define SYS_BLD_SYS_SEMAPHORE_KEY_SWDRV_OM          8
#define SYS_BLD_SYS_SEMAPHORE_SWDRVL3_OM            9
#define SYS_BLD_SYS_SEMAPHORE_BUFFERMGR             10
#define SYS_BLD_SYS_SEMAPHORE_ISC_OM                13
#define SYS_BLD_SYS_SEMAPHORE_CFGDB_OM              14
#define SYS_BLD_SYS_SEMAPHORE_CLI_OM                15
#define SYS_BLD_SYS_SEMAPHORE_DEV_NICDRV_OM         17
#define SYS_BLD_SYS_SEMAPHORE_LAN_OM                18
#define SYS_BLD_SYS_SEMAPHORE_KEY_FS_OM             19
#define SYS_BLD_SYS_SEMAPHORE_KEY_NMTRDRV_OM        20
#define SYS_BLD_SYS_SEMAPHORE_CLUSTER_OM            22
#define SYS_BLD_SYS_SEMAPHORE_KEY_COS_OM            23
#define SYS_BLD_SYS_SEMAPHORE_KEY_I2CDRV            24
#define SYS_BLD_SYS_SEMAPHORE_IGMPSNOOP             25
#define SYS_BLD_SYS_SEMAPHORE_KEY_LED_OM            26
#define SYS_BLD_SYS_SEMAPHORE_MLDSNP                27
#define SYS_BLD_SYS_SEMAPHORE_KEY_SWDRVL4           28
#define SYS_BLD_SYS_SEMAPHORE_KEY_RULE_OM           29
#define SYS_BLD_SYS_SEMAPHORE_KEY_RULE_CTRL         30
#define SYS_BLD_SYS_SEMAPHORE_KEY_NETCFG_OM         31
#define SYS_BLD_SYS_SEMAPHORE_KEY_BUFFER_MGR        32
#define SYS_BLD_SYS_SEMAPHORE_KEY_SYSCTRL_XOR_MGR   33
#define SYS_BLD_SYS_SEMAPHORE_DHCPSNP_OM            34
#define SYS_BLD_SYS_SEMAPHORE_HTTP_OM               35
#define SYS_BLD_SYS_SEMAPHORE_UC_MGR                36
#define SYS_BLD_SYS_SEMAPHORE_NETACCESS_OM          37
#define SYS_BLD_SYS_SEMAPHORE_KEY_SYS_CALLBACK_OM   38
#define SYS_BLD_SYS_SEMAPHORE_KEY_CLI_OM            39
#define SYS_BLD_SYS_SEMAPHORE_KEY_DNS_CACHE         40
#define SYS_BLD_SYS_SEMAPHORE_KEY_DNS_OM            41
#define SYS_BLD_SYS_SEMAPHORE_KEY_DNS_RESOLVER      42
#define SYS_BLD_SYS_SEMAPHORE_KEY_KEYGEN_OM         43
#define SYS_BLD_SYS_SEMAPHORE_KEY_IPFILTER          44
#define SYS_BLD_SYS_SEMAPHORE_KEY_MIB2_OM           45
#define SYS_BLD_SYS_SEMAPHORE_KEY_PING_OM           46
#define SYS_BLD_SYS_SEMAPHORE_KEY_SMTP_OM           47
#define SYS_BLD_SYS_SEMAPHORE_KEY_SNTP_OM           48
#define SYS_BLD_SYS_SEMAPHORE_KEY_SSHD_OM           49
#define SYS_BLD_SYS_SEMAPHORE_KEY_SYSLOG_OM         50
#define SYS_BLD_SYS_SEMAPHORE_KEY_TACACS_OM         51
#define SYS_BLD_SYS_SEMAPHORE_KEY_TELNET_OM         52
#define SYS_BLD_SYS_SEMAPHORE_KEY_TRACEROUTE_OM     53
#define SYS_BLD_SYS_SEMAPHORE_KEY_XSTP_OM           54
#define SYS_BLD_SYS_SEMAPHORE_KEY_AMTR_NA_OM        55
#define SYS_BLD_SYS_SEMAPHORE_KEY_SYSTIME_OM        56
#define SYS_BLD_SYS_SEMAPHORE_KEY_NAQUEUE_OM        57
#define SYS_BLD_SYS_SEMAPHORE_KEY_SNMP_TRAP         58
#define SYS_BLD_SYS_SEMAPHORE_KEY_AAA_OM            59
#define SYS_BLD_SYS_SEMAPHORE_KEY_CMGR              60
#define SYS_BLD_SYS_SEMAPHORE_KEY_VRRP              61
#define SYS_BLD_SYS_SEMAPHORE_PRIVATE_VLAN_OM       62
#define SYS_BLD_SYS_SEMAPHORE_KEY_OSPF6_OM          63
#define SYS_BLD_SYS_SEMAPHORE_RSPAN_OM              64
#define SYS_BLD_SYS_SEMAPHORE_KEY_POEDRV_OM         65
#define SYS_BLD_SYS_SEMAPHORE_KEY_GPIO              66
#define SYS_BLD_SYS_SEMAPHORE_KEY_TWSI              67
#define SYS_BLD_SYS_SEMAPHORE_KEY_SYSDRV            68
#define SYS_BLD_SYS_SEMAPHORE_KEY_BACKDOOR          69
#define SYS_BLD_SYS_SEMAPHORE_KEY_TIME_RANGE_OM     70

/************************************
 **  shared memory key definition  **
 ************************************
 */
#define SYS_BLD_SHMEM_KEY_SYSRSC          1
#define SYS_BLD_OSPF_PROC_SHARED_MEM_KEY  2
#define SYS_BLD_OSPF_PROC_SHARED_MEM_SIZE         (1024*8)

/*
 * suggest using RTMIN Signal, here define signal offset value,
 * actual signal value=SIGRTMIN+offset
 * RTMIN Signal 0 and 1 are reserved for internal usage in pthread library,
 * so to avoid similar cases,we define the available RTMIN signal for our applications starts from 16 here.
 */
#define SYS_BLD_SIGNAL_RTMIN_BASE           16
#define SYS_BLD_SIGNAL_PERIODIC_TIMER       (SYS_BLD_SIGNAL_RTMIN_BASE + 0)
#define SYS_BLD_SIGNAL_LINK_SCAN            (SYS_BLD_SIGNAL_RTMIN_BASE + 1)
#define SYS_BLD_SIGNAL_NOTIFY_MESSAGE       (SYS_BLD_SIGNAL_RTMIN_BASE + 2)

#define SYS_BLD_OSPF_DEFAULT_REF_BANDWIDTH  100000
#endif /* End of SYS_BLD_H */

