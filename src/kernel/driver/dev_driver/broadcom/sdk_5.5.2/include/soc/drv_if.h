/*
 * $Id: drv_if.h,v 1.36.2.12 Broadcom SDK $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * this header file defines driver services and is included in drv.h only
 */

#ifndef _DRV_IF_H
#define _DRV_IF_H

#include <soc/portmode.h>
#include <soc/types.h>
#include <soc/cfp.h>

/* mem type */
typedef enum drv_mem_e {
    DRV_MEM_ARL = 0x10000,
    DRV_MEM_VLAN,
    DRV_MEM_MSTP,
    DRV_MEM_MCAST,
    DRV_MEM_SECMAC,
    DRV_MEM_GEN,
    DRV_MEM_ARL_HW,
    DRV_MEM_MARL,
    DRV_MEM_VLANVLAN, /* VLAN Translation table */
    DRV_MEM_MACVLAN, /* Mac based VLAN table */
    DRV_MEM_PROTOCOLVLAN, /* Protocol VLAN table */
    DRV_MEM_FLOWVLAN, /* Flow based VLAN table */
    DRV_MEM_EGRVID_REMARK, /* Egress VID Remark table */
    DRV_MEM_TCAM_DATA,    
    DRV_MEM_TCAM_MASK,    
    DRV_MEM_CFP_ACT,    
    DRV_MEM_CFP_METER,    
    DRV_MEM_CFP_STAT_IB,    
    DRV_MEM_CFP_STAT_OB
} drv_mem_t;

/* mem field type */
typedef enum drv_mem_field_e {
    /* ARL/MCAST table */
    DRV_MEM_FIELD_MAC = 0x10000, /* ARL/MCAST/SECMAC */
    DRV_MEM_FIELD_VLANID,
    DRV_MEM_FIELD_SRC_PORT,
    DRV_MEM_FIELD_DEST_BITMAP, /* MCAST */
    DRV_MEM_FIELD_DEST_BITMAP1,
    DRV_MEM_FIELD_CHIP_ID,
    DRV_MEM_FIELD_PRIORITY,
    DRV_MEM_FIELD_VALID, /* ARL/MCAST/VLAN/SECMAC */
    DRV_MEM_FIELD_VALID1, /* MCAST */
    DRV_MEM_FIELD_STATIC,
    DRV_MEM_FIELD_AGE, /* ARL/MSTP */
    DRV_MEM_FIELD_ARL_CONTROL,
    /* VLAN table */
    DRV_MEM_FIELD_SPT_GROUP_ID,
    DRV_MEM_FIELD_PORT_BITMAP,
    DRV_MEM_FIELD_OUTPUT_UNTAG,
    DRV_MEM_FIELD_FWD_MODE,

    /* MSTP table */
    DRV_MEM_FIELD_MSTP_PORTST,

    /* SECMAC (Security MAC) table */

    /* VLAN Translation table */
    DRV_MEM_FIELD_TRANSPARENT_MODE,
    DRV_MEM_FIELD_MAPPING_MODE,
    DRV_MEM_FIELD_NEW_VLANID,

    /* Egress VLAN Translation table */
    DRV_MEM_FIELD_OUTER_OP,
    DRV_MEM_FIELD_OUTER_VID,
    DRV_MEM_FIELD_INNER_OP,
    DRV_MEM_FIELD_INNER_VID,

    /* Protocol VLAN table */
    DRV_MEM_FIELD_ETHER_TYPE

} drv_mem_field_t;

/* mem search/insert/delete flags */
typedef enum drv_mem_op_flag_e {
    DRV_MEM_OP_BY_INDEX           = 0x1,
    DRV_MEM_OP_BY_HASH_BY_MAC     = 0x2,
    DRV_MEM_OP_BY_HASH_BY_VLANID  = 0x4,
    DRV_MEM_OP_DELETE_BY_PORT = 0x8,
    DRV_MEM_OP_DELETE_BY_VLANID = 0x10,
    DRV_MEM_OP_DELETE_BY_SPT =0x20,
    DRV_MEM_OP_DELETE_BY_STATIC =0x40,
    DRV_MEM_OP_DELETE_ALL_ARL =0x80,
    DRV_MEM_OP_SEARCH_VALID_START = 0x100,
    DRV_MEM_OP_SEARCH_VALID_GET = 0x200,
    DRV_MEM_OP_SEARCH_PORT = 0x400,
    DRV_MEM_OP_SEARCH_DONE = 0x800,
    DRV_MEM_OP_REPLACE = 0x1000
} drv_mem_op_flag_t;

/* trunk hash field type */
typedef enum drv_trunk_hash_field_e {
    DRV_TRUNK_HASH_FIELD_MACDA = 0x1,
    DRV_TRUNK_HASH_FIELD_MACSA = 0x2,
    DRV_TRUNK_HASH_FIELD_ETHERTYPE = 0x4,
    DRV_TRUNK_HASH_FIELD_VLANID = 0x8,
    DRV_TRUNK_HASH_FIELD_IP_MACDA = 0x10,
    DRV_TRUNK_HASH_FIELD_IP_MACSA = 0x20,
    DRV_TRUNK_HASH_FIELD_IP_ETHERTYPE = 0x40,
    DRV_TRUNK_HASH_FIELD_IP_VLANID = 0x80,
    DRV_TRUNK_HASH_FIELD_L3 = 0x100
} drv_trunk_hash_field_t;

/* trunk flag type */
typedef enum drv_trunk_flag_e {
    DRV_TRUNK_FLAG_ENABLE = 0x1,
    DRV_TRUNK_FLAG_DISABLE = 0x2,
    DRV_TRUNK_FLAG_BITMAP = 0x4,
    DRV_TRUNK_FLAG_HASH_CURRENT = 0x8,
    DRV_TRUNK_FLAG_HASH_DEFAULT = 0x10
} drv_trunk_flag_t;

/* trunk timers */
typedef enum drv_trunk_timer_e {
    DRV_TRUNK_TIMER_FAILOVER = 1,
    DRV_TRUNK_TIMER_FAILBACK
} drv_trunk_timer_t;

/* System configuration property type */
typedef enum drv_dev_prop_e {
    /* used by (dev_prop_get/set) */
    DRV_DEV_PROP_MCAST_NUM = 1,
    DRV_DEV_PROP_AGE_TIMER_MAX_S,
    DRV_DEV_PROP_TRUNK_NUM,
    DRV_DEV_PROP_TRUNK_MAX_PORT_NUM,
    DRV_DEV_PROP_COSQ_NUM,
    DRV_DEV_PROP_MSTP_NUM,
    DRV_DEV_PROP_SEC_MAC_NUM_PER_PORT,
    DRV_DEV_PROP_COSQ_MAX_WEIGHT_VALUE,
    DRV_DEV_PROP_AUTH_PBMP,
    DRV_DEV_PROP_RATE_CONTROL_PBMP,
    DRV_DEV_PROP_VLAN_ENTRY_NUM,
    DRV_DEV_PROP_BPDU_NUM,
    DRV_DEV_PROP_INTERNAL_MII_PBMP,
    DRV_DEV_PROP_EXTERNAL_MII_PBMP,
    DRV_DEV_PROP_CFP_TCAM_SIZE,
    DRV_DEV_PROP_CFP_UDFS_NUM,
    DRV_DEV_PROP_CFP_RNG_NUM,
    DRV_DEV_PROP_CFP_VID_RNG_NUM,
    DRV_DEV_PROP_SA_STATION_MOVE_DROP
} drv_dev_prop_t;

/* tag or port-base vlan */
typedef enum drv_vlan_mode_e {
    DRV_VLAN_MODE_TAG = 1,
    DRV_VLAN_MODE_PORT_BASE,
    DRV_VLAN_MODE_MISS_DROP_EN,
    DRV_VLAN_MODE_MISS_DROP_DIS
} drv_vlan_mode_t;

/* vlan property */
typedef enum drv_vlan_prop_type_e {
    DRV_VLAN_PROP_DROP_VTABLE_MISS,
    DRV_VLAN_PROP_COUNT,
    DRV_VLAN_PROP_VLAN_LEARNING_MODE,
    DRV_VLAN_PROP_SP_TAG_TPID,      /* service provider tag TPID */
    DRV_VLAN_PROP_DOUBLE_TAG_MODE,
    DRV_VLAN_PROP_IDT_MODE_ENABLE,  /* iDT_Mode(intelligent DT_Mode) */
    DRV_VLAN_PROP_EVR_INIT,     /* bcm53115 new table */
    DRV_VLAN_PROP_V2V,     
    DRV_VLAN_PROP_ISP_PORT,
    DRV_VLAN_PROP_V2V_PORT,
    DRV_VLAN_PROP_MAC2V_PORT,
    DRV_VLAN_PROP_PROTOCOL2V_PORT,
    DRV_VLAN_PROP_TRUST_VLAN_PORT,
    DRV_VLAN_PROP_INNER_TAG_PORT
}drv_vlan_prop_type_t;

/* vlan xlate property */
typedef enum drv_vt_table_type_e{
    DRV_VLAN_XLAT_INGRESS,
    DRV_VLAN_XLAT_EGRESS,
    
    DRV_VLAN_XLAT_EVR  /* target at Egress VLAN Remark table */
}drv_vt_tbl_type_t;

/* vlan xlate property */
typedef enum drv_vt_prop_type_e{
    DRV_VLAN_PROP_VT_MODE,      /* VT mode : trasparent / mapping */
    DRV_VLAN_PROP_ING_VT_SPVID,     /* ingress SP VID */
    DRV_VLAN_PROP_EGR_VT_SPVID,     /* egress SP VID */
    DRV_VLAN_PROP_ING_VT_PRI,
    DRV_VLAN_PROP_EGR_VT_PRI,
    DRV_VLAN_PROP_ING_VT_SPTPID,    /* ingress SP TPID */
    DRV_VLAN_PROP_EGR_VT_SPTPID,     /* egress SP TPID */

    /* for CFP based VLAN translation */
    DRV_VLAN_PROP_EVR_UNI_UNTAG_ACTION_ID,  /* indicating to a EVR index */
    DRV_VLAN_PROP_EVR_VT_NEW_ENTRY_ID,  /* indicating to a EVR index */
    DRV_VLAN_PROP_EVR_VT_ISP_CHANGE  /* indicating to a EVR index */
    
}drv_vt_prop_type_t;

/* queue operation mode */
typedef enum drv_q_mode_e {
    DRV_QUEUE_MODE_STRICT = 1,
    DRV_QUEUE_MODE_WRR,
    DRV_QUEUE_MODE_HYBRID,
    DRV_QUEUE_MODE_1STRICT_3WRR,
    DRV_QUEUE_MODE_2STRICT_2WRR
} drv_q_mode_t;

typedef enum drv_q_map_e {
    DRV_QUEUE_MAP_NONE = 0,
    DRV_QUEUE_MAP_PRIO,
    DRV_QUEUE_MAP_TOS,
    DRV_QUEUE_MAP_DFSV,
    DRV_QUEUE_MAP_PORT, /* Port based priority */
    DRV_QUEUE_MAP_MAC, /* Mac based priority */
    DRV_QUEUE_MAP_HYBRID /* hybrid priority from port-based, Diffserv, 802.1p, ... */
} drv_q_map_t;

typedef enum drv_rx_reason_e {
    DRV_RX_REASON_MIRRORING,
    DRV_RX_REASON_SA_LEARNING,
    DRV_RX_REASON_SWITCHING,
    DRV_RX_REASON_PROTO_TERM,
    DRV_RX_REASON_PROTO_SNOOP,
    DRV_RX_REASON_EXCEPTION,
    DRV_RX_REASON_COUNT
}drv_rx_reason_t;

/* port type */
typedef enum drv_port_type_e {
    DRV_PORT_TYPE_10_100 = 1,
    DRV_PORT_TYPE_G,
    DRV_PORT_TYPE_XG,
    DRV_PORT_TYPE_CPU, /* MII port */
    DRV_PORT_TYPE_MGNT, /* SPI */
    DRV_PORT_TYPE_ALL
} drv_port_type_t;

/* port state */
typedef enum drv_portst_e {
    DRV_PORTST_DISABLE = 1,
    DRV_PORTST_BLOCK,
    DRV_PORTST_LISTEN,
    DRV_PORTST_LEARN,
    DRV_PORTST_FORWARD
} drv_portst_t;

/* station mac property */
typedef enum drv_mac_type_e {
    DRV_MAC_CUSTOM_BPDU = 1,
    DRV_MAC_MULTIPORT_0,
    DRV_MAC_MULTIPORT_1,
    DRV_MAC_MULTIPORT_2,
    DRV_MAC_MULTIPORT_3,
    DRV_MAC_MULTIPORT_4,
    DRV_MAC_MULTIPORT_5,
    DRV_MAC_CUSTOM_EAP,
    DRV_MAC_MIRROR_IN,
    DRV_MAC_MIRROR_OUT,
    DRV_MAC_IGMP_REPLACE,
    DRV_MAC_SECURITY_ADD,
    DRV_MAC_SECURITY_REMOVE,
    DRV_MAC_SECURITY_CLEAR
} drv_mac_type_t;

/* port/mac advertisable abilities */
/* adopt abilities defined in port.h */
typedef enum drv_abil_type_e {
    DRV_PORT_ABIL_10MB_HD = SOC_PM_10MB_HD,
    DRV_PORT_ABIL_10MB_FD = SOC_PM_10MB_FD,
    DRV_PORT_ABIL_100MB_HD = SOC_PM_100MB_HD,
    DRV_PORT_ABIL_100MB_FD = SOC_PM_100MB_FD,
    DRV_PORT_ABIL_1000MB_HD = SOC_PM_1000MB_HD,
    DRV_PORT_ABIL_1000MB_FD = SOC_PM_1000MB_FD,
    DRV_PORT_ABIL_10GB_HD = SOC_PM_10GB_HD,
    DRV_PORT_ABIL_10GB_FD = SOC_PM_10GB_FD,
    DRV_PORT_ABIL_TBI = SOC_PM_TBI,
    DRV_PORT_ABIL_MII = SOC_PM_MII,
    DRV_PORT_ABIL_GMII = SOC_PM_GMII,
    DRV_PORT_ABIL_SGMII = SOC_PM_SGMII,
    DRV_PORT_ABIL_XGMII = SOC_PM_XGMII,
    DRV_PORT_ABIL_LB_MAC = SOC_PM_LB_MAC,
    DRV_PORT_ABIL_LB_PHY = SOC_PM_LB_PHY,
    DRV_PORT_ABIL_PAUSE_TX = SOC_PM_PAUSE_TX,
    DRV_PORT_ABIL_PAUSE_RX = SOC_PM_PAUSE_RX,
    DRV_PORT_ABIL_PAUSE = SOC_PM_PAUSE,
    DRV_PORT_ABIL_PAUSE_ASYMM = SOC_PM_PAUSE_ASYMM
} drv_abil_type_t;

/* port operation property */
/* constants for port speed/duplex/an/pause -- bitmap */
typedef enum drv_port_prop_e {
    DRV_PORT_PROP_SPEED= 1,
    DRV_PORT_PROP_DUPLEX,
    DRV_PORT_PROP_AUTONEG,
    DRV_PORT_PROP_TX_PAUSE,
    DRV_PORT_PROP_RX_PAUSE,
    DRV_PORT_PROP_LOCAL_ADVERTISE,
    DRV_PORT_PROP_REMOTE_ADVERTISE,
    DRV_PORT_PROP_PORT_ABILITY,
    DRV_PORT_PROP_MAC_ABILITY,
    DRV_PORT_PROP_PHY_ABILITY,
    DRV_PORT_PROP_INTERFACE,
    DRV_PORT_PROP_MAC_ENABLE,
    DRV_PORT_PROP_ENABLE,
    DRV_PORT_PROP_ENABLE_DROP_NON1Q,
    DRV_PORT_PROP_ENABLE_DROP_1Q,
    DRV_PORT_PROP_ENABLE_RX,
    DRV_PORT_PROP_ENABLE_TX,
    DRV_PORT_PROP_ENABLE_TXRX,
    DRV_PORT_PROP_IPG_FE,
    DRV_PORT_PROP_IPG_GE,
    DRV_PORT_PROP_JAM,
    DRV_PORT_PROP_BPDU_RX,
    DRV_PORT_PROP_RESTART_AUTONEG,
    DRV_PORT_PROP_MAC_LOOPBACK,
    DRV_PORT_PROP_PHY_LOOPBACK,
    DRV_PORT_PROP_PHY_MEDIUM,
    DRV_PORT_PROP_PHY_MEDIUM_CONFIG_COPPER,
    DRV_PORT_PROP_PHY_MEDIUM_CONFIG_FIBER,
    DRV_PORT_PROP_PHY_MDIX,
    DRV_PORT_PROP_PHY_MDIX_STATUS,
    DRV_PORT_PROP_MS,
    DRV_PORT_PROP_SEC_MAC_MODE_NONE,
    DRV_PORT_PROP_SEC_MAC_MODE_STATIC_ACCEPT,
    DRV_PORT_PROP_SEC_MAC_MODE_STATIC_REJECT,
    DRV_PORT_PROP_SEC_MAC_MODE_DYNAMIC_SA_NUM,
    DRV_PORT_PROP_SEC_MAC_MODE_DYNAMIC_SA_MATCH,
    DRV_PORT_PROP_SEC_MAC_MODE_EXTEND,
    DRV_PORT_PROP_SEC_MAC_MODE_SIMPLIFY,
    DRV_PORT_PROP_PHY_LINKUP_EVT,
    DRV_PORT_PROP_PHY_LINKDN_EVT,
    DRV_PORT_PROP_PHY_RESET,
    DRV_PORT_PROP_PHY_CABLE_DIAG,
    DRV_PORT_PROP_PHY_LINK_CHANGE,
    DRV_PORT_PROP_PAUSE_FRAME_BYPASS_RX,
    DRV_PORT_PROP_PAUSE_FRAME_BYPASS_TX,
    DRV_PORT_PROP_DTAG_MODE_INTELLIGENT,
    DRV_PORT_PROP_DTAG_MODE,
    DRV_PORT_PROP_DTAG_ISP_PORT,
    DRV_PORT_PROP_DTAG_TPID,
    DRV_PORT_PROP_802_1X_MODE,
    DRV_PORT_PROP_802_1X_BLK_RX,
    DRV_PORT_PROP_MAC_BASE_VLAN,
    DRV_PORT_PROP_MAX_FRAME_SZ,
    DRV_PORT_PROP_INGRESS_VLAN_CHK
} drv_port_prop_t;

typedef enum drv_port_rate_limit_action_e {
    DRV_PORT_PROP_INGRESS_RATE_LIMIT_ACTION_DROP = 1,
    DRV_PORT_PROP_INGRESS_RATE_LIMIT_ACTION_FLOW_CONTROL
} drv_port_rate_limit_action_t;


/* additional constants for port status */
typedef enum drv_port_status_e {
    DRV_PORT_STATUS_LINK_UP = 1,
    DRV_PORT_STATUS_LINK_SPEED,
    DRV_PORT_STATUS_LINK_DUPLEX,
    DRV_PORT_STATUS_PROBE,
    DRV_PORT_STATUS_AUTONEG_ENABLE,
    DRV_PORT_STATUS_AUTONEG_DISABLED,
    DRV_PORT_STATUS_AUTONEG_COMPLETED,
    DRV_PORT_STATUS_AUTONEG_FAILED,
    DRV_PORT_STATUS_AUTONEG_PAGE_RECEIVED,
    DRV_PORT_STATUS_SRC_VLAN_VIOLATION,
    DRV_PORT_STATUS_DST_VLAN_VIOLATION,
    DRV_PORT_STATUS_ALL_QUEUE_DROP,
    DRV_PORT_STATUS_IN_BUF_FULL,
    DRV_PORT_STATUS_L2_TBL_UNAVAIL,
    DRV_PORT_STATUS_SECURITY_VIOLATION_DISABLED,
    DRV_PORT_STATUS_TX_PAUSE_DISABLED,
    DRV_PORT_STATUS_RX_PAUSE_DISABLED,
    DRV_PORT_STATUS_INIT,
    DRV_PORT_STATUS_PHY_DRV_NAME
} drv_port_status_t;

typedef enum drv_port_status_duplex_e {
    DRV_PORT_STATUS_DUPLEX_HALF = 1,
    DRV_PORT_STATUS_DUPLEX_FULL
} drv_port_status_duplex_t;

typedef enum drv_port_status_speed_e {
    DRV_PORT_STATUS_SPEED_10M = 1,
    DRV_PORT_STATUS_SPEED_100M,
    DRV_PORT_STATUS_SPEED_1G,
    DRV_PORT_STATUS_SPEED_10G
} drv_port_status_speed_t;

/* port operation mode */
typedef enum drv_port_oper_mode_e {
    DRV_PORT_MODE_MAC_MASTER = 1,
    DRV_PORT_MODE_PHY_MASTER_RO,
    DRV_PORT_MODE_PHY_MASTER_RW
} drv_port_oper_mode_t;

/* port-base, 802.1x security */
typedef enum drv_security_state_e {
    DRV_SECURITY_PORT_UNCONTROLLED = 1,
    DRV_SECURITY_PORT_UNAUTHENTICATE,
    DRV_SECURITY_PORT_AUTHENTICATED
} drv_security_state_t;

typedef enum drv_security_flag_e {
    DRV_SECURITY_VIOLATION_NONE = 0x1,
    DRV_SECURITY_VIOLATION_STATIC_ACCEPT =0x2,
    DRV_SECURITY_VIOLATION_STATIC_REJECT = 0x4,
    DRV_SECURITY_VIOLATION_SA_NUM = 0x8,
    DRV_SECURITY_VIOLATION_SA_MATCH = 0x10,
    DRV_SECURITY_BLOCK_IN = 0x20,
    DRV_SECURITY_BLOCK_INOUT = 0x40,
    DRV_SECURITY_LEARN = 0x80,
    DRV_SECURITY_IGNORE_LINK= 0x100,
    DRV_SECURITY_IGNORE_VIOLATION = 0x200,
    DRV_SECURITY_EAP_MODE_BASIC = 0x400,
    DRV_SECURITY_EAP_MODE_EXTEND = 0x800,
    DRV_SECURITY_EAP_MODE_SIMPLIFIED = 0x1000
} drv_security_flag_t;

/* switch to CPU type */
typedef enum drv_switch_type_e {
    /* Trap */
    DRV_SWITCH_TRAP_IPMC = 0x1,
    DRV_SWITCH_TRAP_IGMP = 0x2,
    DRV_SWITCH_TRAP_GARP = 0x4,
    DRV_SWITCH_TRAP_ARP = 0x8,
    DRV_SWITCH_TRAP_8023AD = 0x10,
    DRV_SWITCH_TRAP_ICMP = 0x20,
    DRV_SWITCH_TRAP_BPDU1 = 0x40,
    DRV_SWITCH_TRAP_BPDU2 = 0x80,
    DRV_SWITCH_TRAP_RARP = 0x100,
    DRV_SWITCH_TRAP_8023AD_DIS = 0x200,
    DRV_SWITCH_TRAP_BGMP = 0x400,
    DRV_SWITCH_TRAP_8021X = 0x800,
    DRV_SWITCH_TRAP_LLDP = 0x1000,
    DRV_SWITCH_TRAP_BCST = 0x2000,
    DRV_SWITCH_TRAP_ICMPV6 = 0x4000,
    DRV_SWITCH_TRAP_MLD = 0x8000,
    DRV_SWITCH_TRAP_IGMP_UNKNOW = 0x10000,
    DRV_SWITCH_TRAP_IGMP_QUERY = 0x20000,
    DRV_SWITCH_TRAP_IGMP_REPLEV = 0x40000,
    DRV_SWITCH_TRAP_MLD_QUERY = 0x80000,
    DRV_SWITCH_TRAP_MLD_REPDONE = 0x100000,
    
    /* disable IGMP pkt(all type) to CPU. */
    DRV_SWITCH_TRAP_IGMP_DISABLE = 0x200000, 
    
    /* disable MLD pkt(all type) to CPU */
    DRV_SWITCH_TRAP_MLD_DISABLE = 0x400000,
    
    /* disable ICMPV6 trap feature */
    DRV_SWITCH_TRAP_ICMPV6_DISABLE = 0x800000
} drv_switch_type_t;

typedef enum drv_snoop_type_e {
    /* Snoop */
    DRV_SNOOP_IGMP = 0x1,
    DRV_SNOOP_ARP = 0x2,
    DRV_SNOOP_RARP = 0x4,
    DRV_SNOOP_ICMP = 0x8,
    DRV_SNOOP_ICMPV6 = 0x10,
    DRV_SNOOP_DHCP = 0x20,
    DRV_SNOOP_MLD = 0x40,
    DRV_SNOOP_IGMP_UNKNOW = 0x80,
    DRV_SNOOP_IGMP_QUERY = 0x100,
    DRV_SNOOP_IGMP_REPLEV = 0x200,
    DRV_SNOOP_MLD_QUERY = 0x400,
    DRV_SNOOP_MLD_REPDONE = 0x800,
    DRV_SNOOP_IGMP_DISABLE = 0x1000,  /* disable IGMP pkt(all type) to CPU */
    DRV_SNOOP_IGMP_UNKNOW_DISABLE = 0x2000, 
    DRV_SNOOP_IGMP_QUERY_DISABLE = 0x4000,
    DRV_SNOOP_IGMP_REPLEV_DISABLE = 0x8000,
    DRV_SNOOP_MLD_DISABLE = 0x10000, /* disable MLD pkt(all type) to CPU */
    DRV_SNOOP_MLD_QUERY_DISABLE = 0x20000,
    DRV_SNOOP_MLD_REPDONE_DISABLE = 0x40000,
    DRV_SNOOP_ICMPV6_DISABLE = 0x80000    /* disable ICMPv6 snooping */
} drv_snoop_type_t;
  
/* Storm Control type */
typedef enum drv_storm_control_type_e {
    DRV_STORM_CONTROL_BCAST = 0x1,
    DRV_STORM_CONTROL_MCAST = 0x2,
    DRV_STORM_CONTROL_DLF = 0x4,
    DRV_STORM_CONTROL_SALF = 0x8,
    DRV_STORM_CONTROL_RSV_MCAST = 0x10
} drv_storm_control_type_t;

/* Storm Control rate definition */
typedef enum drv_storm_control_rate_e {
    DRV_STORM_RATE_ENABLE,    
    DRV_STORM_RATE_3POINT3_PERCENT,      
    DRV_STORM_RATE_5_PERCENT,      
    DRV_STORM_RATE_10_PERCENT,      
    DRV_STORM_RATE_20_PERCENT   
} drv_storm_control_rate_t;

/* Port Rate Control:Rate defintion */
typedef enum drv_rate_control_rate_e {
    DRV_RATE_CONTROL_INGRESS_EN,
    DRV_RATE_CONTROL_EGRESS_EN,
    DRV_RATE_CONTROL_FIXED,
    DRV_RATE_CONTROL_RATIO,
    DRV_RATE_CONTROL_INGRESS_RATE_LIMIT,
    DRV_RATE_CONTROL_INGRESS_RATE_LIMIT_ACTION
} drv_rate_control_rate_t;

/* Port Rate Control: action defintion */
typedef enum drv_rate_control_act_e {
    DRV_RATE_ACT_DROP,
    DRV_RATE_ACT_FC       /* FC is Flow Control */  
} drv_rate_control_act_t;

/* Port Rate Control: config defintion */
typedef enum drv_rate_control_config_e {
    DRV_RATE_CONFIG_RATE_TYPE,
    DRV_RATE_CONFIG_DROP_ENABLE,
    DRV_RATE_CONFIG_PKT_MASK
} drv_rate_control_config_t;

typedef enum drv_rate_control_type_e {
    DRV_RATE_CONFIG_RATE_TYPE_FIXED,
    DRV_RATE_CONFIG_RATE_TYPE_RATIO
} drv_rate_control_config_type_t;


/* Port Rate Control: config defintion */
typedef enum drv_rate_control_direction_e {
    DRV_RATE_CONTROL_DIRECTION_INGRESSS = 1,
    DRV_RATE_CONTROL_DIRECTION_EGRESSS
} drv_rate_control_direction_t;

/* mirror enable flags */
typedef enum drv_port_mrror_enable_e {
    DRV_PORT_MIRROR_INGRESS_ENABLE = 1,
    DRV_PORT_MIRROR_EGRESS_ENABLE
} drv_port_mirror_enable_t;

/* SNMP counters */
typedef enum drv_counter_thread_e {
    DRV_COUNTER_THREAD_START,
    DRV_COUNTER_THREAD_STOP,
    DRV_COUNTER_THREAD_SYNC
} drv_counter_thread_t;

typedef enum drv_snmp_counter_e {
    /* RFC 1213 */
    DRV_SNMP_IF_IN_OCTETS,
    DRV_SNMP_IF_IN_UNICAST_PKTS,
    DRV_SNMP_IF_IN_NON_UNICAST_PKTS,
    DRV_SNMP_IF_IN_DISCARDS,
    DRV_SNMP_IF_IN_ERRORS,
    DRV_SNMP_IF_IN_UNKNOWN_PROTOS,
    DRV_SNMP_IF_OUT_OCTETS,
    DRV_SNMP_IF_OUT_UNICAST_PKTS,
    DRV_SNMP_IF_OUT_NON_UNICAST_PKTS,
    DRV_SNMP_IF_OUT_DISCARDS,
    DRV_SNMP_IF_OUT_ERRORS,
    DRV_SNMP_IF_OUT_QLEN,
    DRV_SNMP_IP_IN_RECEIVES,
    DRV_SNMP_IP_IN_HDR_ERRORS,
    DRV_SNMP_IP_FORWARD_DATAGRAMS,
    DRV_SNMP_IP_IN_DISCARDS,

        /* RFC 1493 */
    DRV_SNMP_DOT1D_BASEPORT_DELAY_EXCEEDED_DISCARDS,
    DRV_SNMP_DOT1D_BASEPORT_MTU_EXCEEDED_DISCARDS,
    DRV_SNMP_DOT1D_TPPORT_IN_FRAMES,
    DRV_SNMP_DOT1D_TPPORT_OUT_FRAMES,
    DRV_SNMP_DOT1D_PORT_IN_DISCARDS,
    
    /* RFC 1757 */ 
    DRV_SNMP_ETHER_STATS_DROP_EVENTS,
    DRV_SNMP_ETHER_STATS_MULTICAST_PKTS,
    DRV_SNMP_ETHER_STATS_BROADCAST_PKTS,
    DRV_SNMP_ETHER_STATS_UNDERSIZE_PKTS,
    DRV_SNMP_ETHER_STATS_FRAGMENTS,
    DRV_SNMP_ETHER_STATS_PKTS_64_OCT,
    DRV_SNMP_ETHER_STATS_PKTS_65TO127_OCT,
    DRV_SNMP_ETHER_STATS_PKTS_128TO255_OCT,
    DRV_SNMP_ETHER_STATS_PKTS_256TO511_OCT,
    DRV_SNMP_ETHER_STATS_PKTS_512TO1023_OCT,
    DRV_SNMP_ETHER_STATS_PKTS_1024TO1518_OCT,
    DRV_SNMP_ETHER_STATS_OVERSIZE_PKTS,
    DRV_SNMP_ETHER_STATS_JABBERS,
    DRV_SNMP_ETHER_STATS_OCTETS,
    DRV_SNMP_ETHER_STATS_PKTS,
    DRV_SNMP_ETHER_STATS_COLLISIONS,
    DRV_SNMP_ETHER_STATS_CRC_ALIGN_ERRORS,
    DRV_SNMP_ETHER_STATS_TX_NO_ERRORS,
    DRV_SNMP_ETHER_STATS_RX_NO_ERRORS,

    /* RFC 2665 */
    DRV_SNMP_DOT3_STATS_ALIGN_ERRORS,
    DRV_SNMP_DOT3_STATS_FCS_ERRORS,
    DRV_SNMP_DOT3_STATS_SINGLE_COLLISION_FRAMES,
    DRV_SNMP_DOT3_STATS_MULTIPLE_COLLISION_FRAMES,
    DRV_SNMP_DOT3_STATS_SQETTEST_ERRORS,
    DRV_SNMP_DOT3_STATS_DEFERRED_TRANSMISSIONS,
    DRV_SNMP_DOT3_STATS_LATE_COLLISIONS,
    DRV_SNMP_DOT3_STATS_EXCESSIVE_COLLISIONS,
    DRV_SNMP_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS,
    DRV_SNMP_DOT3_STATS_CARRIER_SENSE_ERRORS,
    DRV_SNMP_DOT3_STATS_FRAME_TOO_LONGS,
    DRV_SNMP_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS,
    DRV_SNMP_DOT3_STATS_SYMBOL_ERRORS,
    DRV_SNMP_DOT3_CONTROL_IN_UNKNOWN_OPCODES,
    DRV_SNMP_DOT3_IN_PAUSE_FRAMES,
    DRV_SNMP_DOT3_OUT_PAUSE_FRAMES,

    /* RFC 2233 */
    DRV_SNMP_IFHC_IN_OCTETS,
    DRV_SNMP_IFHC_IN_UNICAST_PKTS,
    DRV_SNMP_IFHC_IN_MULTICAST_PKTS,
    DRV_SNMP_IFHC_IN_BROADCAST_PKTS,
    DRV_SNMP_IFHC_OUT_OCTETS,
    DRV_SNMP_IFHC_OUT_UNICAST_PKTS,
    DRV_SNMP_IFHC_OUT_MULTICAST_PKTS,
    DRV_SNMP_IFHC_OUT_BROADCAST_PKTS,

    /*** RFC 2465 ***/

    DRV_SNMP_IPV6_STATS_IN_RECEIVES,
    DRV_SNMP_IPV6_STATS_IN_HDR_ERRORS,
    DRV_SNMP_IPV6_STATS_IN_ADDR_ERRORS,
    DRV_SNMP_IPV6_STATS_IN_DISCARDS,
    DRV_SNMP_IPV6_STATS_OUT_FORW_DATAGRAMS,
    DRV_SNMP_IPV6_STATS_OUT_DISCARDS,
    DRV_SNMP_IPV6_STATS_IN_MCAST_PKTS,
    DRV_SNMP_IPV6_STATS_OUT_MCAST_PKTS,

    /*** Additional Broadcom stats ***/
    DRV_SNMP_BCM_IPMC_BRIDGED_PKTS,
    DRV_SNMP_BCM_IPMC_ROUTED_PKTS,
    DRV_SNMP_BCM_IPMC_IN_DROPPED_PKTS,
    DRV_SNMP_BCM_IPMC_OUT_DROPPED_PKTS,

    DRV_SNMP_BCM_ETHER_STATS_PKTS_1519TO1522_OCT,
    DRV_SNMP_BCM_ETHER_STATS_PKTS_1522TO2047_OCT,
    DRV_SNMP_BCM_ETHER_STATS_PKTS_2048TO4095_OCT,
    DRV_SNMP_BCM_ETHER_STATS_PKTS_4095TO9216_OCT,

    DRV_SNMP_BCM_CUSTOM_RCV0,
    DRV_SNMP_BCM_CUSTOM_RCV1,
    DRV_SNMP_BCM_CUSTOM_RCV2,
    DRV_SNMP_BCM_CUSTOM_RCV3,
    DRV_SNMP_BCM_CUSTOM_RCV4,
    DRV_SNMP_BCM_CUSTOM_RCV5,
    DRV_SNMP_BCM_CUSTOM_RCV6,
    DRV_SNMP_BCM_CUSTOM_RCV7,
    DRV_SNMP_BCM_CUSTOM_RCV8,
    DRV_SNMP_BCM_CUSTOM_TRANS0,
    DRV_SNMP_BCM_CUSTOM_TRANS1,
    DRV_SNMP_BCM_CUSTOM_TRANS2,
    DRV_SNMP_BCM_CUSTOM_TRANS3,
    DRV_SNMP_BCM_CUSTOM_TRANS4,
    DRV_SNMP_BCM_CUSTOM_TRANS5,
    DRV_SNMP_BCM_CUSTOM_TRANS6,
    DRV_SNMP_BCM_CUSTOM_TRANS7,
    DRV_SNMP_BCM_CUSTOM_TRANS8,
    DRV_SNMP_BCM_CUSTOM_TRANS9,
    DRV_SNMP_BCM_CUSTOM_TRANS10,
    DRV_SNMP_BCM_CUSTOM_TRANS11,
    DRV_SNMP_BCM_CUSTOM_TRANS12,
    DRV_SNMP_BCM_CUSTOM_TRANS13,
    DRV_SNMP_BCM_CUSTOM_TRANS14,
    
    DRV_SNMP_COUNTER_COUNT
}drv_snmp_counter_t;

typedef enum drv_mcast_index_e {
    DRV_MCAST_INDEX_ADD = 1,
    DRV_MCAST_INDEX_REMOVE
}drv_mcast_index_t;

/* Ethernet AV control type */
typedef enum drv_eav_control_flag_e {
    DRV_EAV_CONTROL_TIME_STAMP_TO_IMP = 1,
    DRV_EAV_CONTROL_MAX_AV_SIZE,
    DRV_EAV_CONTROL_STREAM_CLASSA_PCP,
    DRV_EAV_CONTROL_STREAM_CLASSB_PCP
} drv_eav_control_flag_t;

/* Ethernet AV Time Sync flag */
typedef enum drv_eav_time_sync_flag_e {
    DRV_EAV_TIME_SYNC_TIME_BASE = 0x1,
        /* p0 : time base value (ns) */
    DRV_EAV_TIME_SYNC_TIME_ADJUST,
        /* p0 : time increment value (ns), p1 : adjust priod (ticks) */
    DRV_EAV_TIME_SYNC_TICK_COUNTER, 
        /* p0 : tick counter in a slot time */
    DRV_EAV_TIME_SYNC_SLOT_NUMBER,
        /* p0 : slot number */
    DRV_EAV_TIME_SYNC_MACRO_SLOT_PERIOD,
        /* p0 : macro slot time (ms). */
    DRV_EAV_TIME_SYNC_SLOT_ADJUST
        /* p0 : slot adjustmenet (ticks), p1 : adjust period (slots) */
} drv_eav_time_sync_flag_t;

/* Ethernet AV queue control type */
typedef enum drv_eav_queuel_flag_e {
    DRV_EAV_QUEUE_Q4_BANDWIDTH = 0x1,
        /* parameter : bandwidth value (bytes/slot time) */
    DRV_EAV_QUEUE_Q5_BANDWIDTH,
        /* parameter : bandwidth value (bytes/slot time) */
    DRV_EAV_QUEUE_Q5_WINDOW
        /* parameter : enable/disable */
} drv_eav_queuel_flag_t;

/* Dos Attack type */
typedef enum drv_dos_type_e {
    DRV_DOS_NONE = 0x1,
    DRV_DOS_TCP_FRAG,
    DRV_DOS_SMURF,
    DRV_DOS_SYN_WITH_SP_LT1024,
    DRV_DOS_SYN_FIN_SCAN,
    DRV_DOS_XMASS_SCAN,
    DRV_DOS_XMASS_WITH_TCP_SEQ_ZERO,
    DRV_DOS_NULL_SCAN,
    DRV_DOS_NULL_WITH_TCP_SEQ_ZERO,
    DRV_DOS_BLAT,
    DRV_DOS_LAND,
    DRV_DOS_MIN_TCP_HDR_SZ,
    DRV_DOS_TCP_FRAG_OFFSET,
    DRV_DOS_TCP_BLAT,
    DRV_DOS_UDP_BLAT,
    DRV_DOS_ICMPV4_FRAGMENTS,
    DRV_DOS_ICMPV6_FRAGMENTS,
    DRV_DOS_MAX_ICMPV4_SIZE,
    DRV_DOS_MAX_ICMPV6_SIZE,
    DRV_DOS_ICMPV4_LONG_PING,
    DRV_DOS_ICMPV6_LONG_PING,
    DRV_DOS_DISABLE_LEARN
} drv_dos_type_t;

/* Port Learn Mode */
typedef enum drv_port_learn_mode_e {
    DRV_PORT_HW_LEARN = 0x1,
    DRV_PORT_DISABLE_LEARN,
    DRV_PORT_SW_LEARN
} drv_port_learn_mode_t;



typedef struct {
    /* =============================================== */
    /* register access */
    int (*reg_read)(int unit, uint32 addr, void *data, int len);
    int (*reg_write)(int unit, uint32 addr, void *data, int len);
    int (*reg_length_get)(int unit, uint32 reg); 
    uint32 (*reg_addr)(int unit, uint32 reg, int port, int index);
    int (*reg_field_set)(int unit, uint32 reg, uint32 *regbuf, 
        uint32 field, uint32 *fldbuf);
    int (*reg_field_get)(int unit, uint32 reg, uint32 *regbuf, 
        uint32 field, uint32 *fldbuf);
    

    /* =============================================== */
    /* for MDIO/PHY access */
    int (*miim_read)(int unit, uint8 phyid, uint8 phyaddr, uint16 *data);
    int (*miim_write)(int unit, uint8 phyid, uint8 phyaddr, uint16 data);

    /* init an integer map for port-to-phy_addr mapping */
    int (*phy_addr_set)(int unit, int **map);

    /* set p, a pointer to mac_driver_t, to device specific mac driver */
    int (*mac_driver_set)(int unit, uint32 **p);

    /* =============================================== */
    /* for memory/table access */

    /* get memory/table width-in-bit and length */
    int (*mem_length_get)(int unit, uint32 mem, uint32 *data);
    int (*mem_width_get)(int unit, uint32 mem, uint32 *data);

    /* read/write count entries from specified offset location */
    int (*mem_read)(int unit, uint32 mem, uint32 entry_id, uint32 count, 
        uint32 *entry);
    int (*mem_write)(int unit, uint32 mem, uint32 entry_id, uint32 count, 
        uint32 *entry);

    /* get/set field of a memory/table entry */
    int (*mem_field_get)(int unit, uint32 mem, uint32 field_index, 
        uint32 *entry, uint32 *fld_data);
    int (*mem_field_set)(int unit, uint32 mem, uint32 field_index, 
        uint32 *entry, uint32 *fld_data);

    /* clear the whole memory/table */
    int (*mem_clear)(int unit, uint32 mem);

    /* search entries for match key(es) */
    /* key is an entry with selected fields set. */
    /* flags indicated which fields should be used as searching keys. */
    int (*mem_search)(int unit, uint32 mem, uint32 *key, uint32 *entry, 
        uint32 *entry1, uint32 flags);
    int (*mem_insert)(int unit, uint32 mem, uint32 *entry, uint32 flags);
    int (*mem_delete)(int unit, uint32 mem, uint32 *entry, uint32 flags);
    int (*mem_cache_get)(int unit, uint32 mem, uint32 *enable);
    int (*mem_cache_set)(int unit, uint32 mem, uint32 enable);

    /* =============================================== */
    /* for MIB access */
    
    /* Need to be defined */
    int (*counter_thread_set)(int unit, uint32 thread_op, uint32 flags, 
        int interval, soc_pbmp_t bmp);
    int (*counter_set)(int unit, soc_pbmp_t bmp, uint32 counter_type, uint64 val);
    int (*counter_get)(int unit, uint32 port, uint32 counter_type, uint64 *val);
    int (*counter_reset)(int unit);

    /* =============================================== */
    /* tag/port vlan */
    int (*vlan_mode_set)(int unit, uint32 mode);
    int (*vlan_mode_get)(int unit, uint32 *mode);

    /* config port base vlan */
    int (*port_vlan_pvid_set)(int unit, uint32 port, uint32 vid, uint32 prio);
    int (*port_vlan_pvid_get)(int unit, uint32 port, uint32 *vid, uint32 *prio);
    int (*port_vlan_set)(int unit, uint32 port, soc_pbmp_t bmp);
    int (*port_vlan_get)(int unit, uint32 port, soc_pbmp_t *bmp);

    int (*vlan_property_set)(int unit, uint32 prop_type, uint32 prop_val);
    int (*vlan_property_get)(int unit, uint32 prop_type, uint32 *prop_val);

    /* port based enable/disable on differernt VLAN propterty */
    int (*vlan_prop_port_enable_set)(int unit, uint32 prop_type, soc_pbmp_t bmp, uint32 val);
    int (*vlan_prop_port_enable_get)(int unit, uint32 prop_type, uint32 port_n, uint32 *val);

    /* vlan translation */
    int (*vlan_vt_set)(int unit, uint32 prop_type, uint32 vid, uint32 port, uint32 val);
    int (*vlan_vt_get)(int unit, uint32 prop_type, uint32 vid, uint32 port, uint32 *val);
    int (*vlan_vt_add)(int unit, uint32 vt_type, uint32 port, uint32 cvid, uint32 sp_vid, 
                                uint32 pri, uint32 mode);
    int (*vlan_vt_delete)(int unit, uint32 vt_type, uint32 port, uint32 vid);
    int (*vlan_vt_delete_all)(int unit, uint32 vt_type);
    /* =============================================== */
    /* trunk */
    /* driver process bits for local ports */
    /* additional bits indicate remote ports if any */
    int (*trunk_set)(int unit, int tid, soc_pbmp_t bmp, uint32 flag, uint32 hash_op);
    int (*trunk_get)(int unit, int tid, soc_pbmp_t *bmp, uint32 flag, uint32 *hash_op);
    int (*trunk_hash_field_add)(int unit, uint32 field_type);
    int (*trunk_hash_field_remove)(int unit, uint32 field_type);

    /* =============================================== */
    /* output queues */
    /* set output queue operation mode - strict or WRR */
    int (*queue_mode_set)(int unit, uint32 bmp, uint32 mode);
    int (*queue_mode_get)(int unit, uint32 port, uint32 *mode);
    int (*queue_count_set)(int unit, uint32 port_type, uint8 count);
    int (*queue_count_get)(int unit, uint32 port_type, uint8 *count);

    /* set WRR weight */
    int (*queue_WRR_weight_set)(int unit, uint32 port_type, uint8 queue, 
        uint32 weight);
    int (*queue_WRR_weight_get)(int unit, uint32 port_type, uint8 queue, 
        uint32 *weight);

    /* config output queue mapping */
    int (*queue_prio_set)(int unit, uint8 prio, uint8 queue_n);
    int (*queue_prio_get)(int unit, uint8 prio, uint8 *queue_n);
    int (*queue_tos_set)(int unit, uint8 precedence, uint8 queue_n);
    int (*queue_tos_get)(int unit, uint8 precedence, uint8 *queue_n);
    int (*queue_dfsv_set)(int unit, uint8 code_point, uint8 queue_n);
    int (*queue_dfsv_get)(int unit, uint8 code_point, uint8 *queue_n);

    /* enable/disable selected mapping - prio/diffserv/tos */
    int (*queue_mapping_type_set)(int unit, soc_pbmp_t bmp, uint32 mapping_type, 
        uint8 enable);
    int (*queue_mapping_type_get)(int unit, uint32 port, uint32 mapping_type, 
        uint8 *enable);

    /* =============================================== */
    /* HW Aging */
    int (*age_timer_set)(int unit, uint32 enable, uint32 age_time);
    int (*age_timer_get)(int unit, uint32 *enable, uint32 *age_time);

    /* =============================================== */
    /* station property */
    /* mac_type - Customer BPDU, ...  */
    int (*mac_set)(int unit, soc_pbmp_t pbmp, uint32 mac_type, uint8* mac, uint32 bpdu_idx);
    int (*mac_get)(int unit, uint32 port, uint32 mac_type, soc_pbmp_t *bmp, uint8* mac);

    /* =============================================== */
    /* port mirror */
    int (*mirror_set)(int unit, uint32 enable, uint32 monitor_port, 
        soc_pbmp_t ingress_bmp, soc_pbmp_t egress_bmp);
    int (*mirror_get)(int unit, uint32 *enable, uint32 *monitor_port, 
        soc_pbmp_t *ingress_bmp, soc_pbmp_t *egress_bmp);

    /* =============================================== */
    /* port property */
    /* set/get port operation mode - polling, master-phy, master-port */
    int (*port_oper_mode_set)(int unit, uint32 bmp, uint32 oper_mode);
    int (*port_oper_mode_get)(int unit, int port_n, uint32 *oper_mode);

    /* set/get port properties - port state, speed, duplex, auto-negotiation,*/
    /*   flow control, authenticate state */
    int (*port_set)(int unit, soc_pbmp_t bmp, uint32 prop_type, uint32 prop_val);
    int (*port_get)(int unit, uint32 port_n, uint32 prop_type, 
        uint32 *prop_val);

    int (*port_advertise_set)(int unit, soc_pbmp_t bmp, uint32 prop_mask);
    int (*port_advertise_get)(int unit, int port_n, uint32 *prop_val);

    /* get port status - port state, link, speed, duplex, auto-negotiation */
    int (*port_status_get)(int unit, uint32 port, uint32 status_type, 
        uint32 *val);
        
    /* update port MAC state(speed/duplex) by SW override mode */
    int (*port_sw_mac_update)(int unit, soc_pbmp_t bmp);
    
    /* get the bitmap mask for selected port type - FE, GE, XG, CPU */
    /* for 5345M, FE=0, GE=0xFFFFFF, XG=0, CPU=0x1000000 */
    int (*port_bitmap_get)(int unit, uint32 port_type, soc_pbmp_t *bitmap);

    /* =============================================== */
    /* set/get port security */
    /* mask(bitmap) - drop violation, disable port, 802.1x_on */
    int (*security_set)(int unit, soc_pbmp_t bmp, uint32 state, uint32 mask);
    int (*security_get)(int unit, uint32 port, uint32 *state, uint32 *mask);
    int (*security_egress_set)(int unit, soc_pbmp_t bmp, int enable);
    int (*security_egress_get)(int unit, int port, int *enable);

    /* =============================================== */
    /* multiple STP */
    int (*mstp_config_set)(int unit, uint32 vlan_id, uint32 mstp_gid);
    int (*mstp_config_get)(int unit, uint32 vlan_id, uint32 *mstp_gid);
    int (*mstp_port_set)(int unit, uint32 mstp_gid, uint32 port, 
        uint32 port_state);
    int (*mstp_port_get)(int unit, uint32 mstp_gid, uint32 port, 
        uint32 *port_state);
    

    /* =============================================== */
    /* trap and snoop */
    /* trap enable/disable */
    int (*trap_set)(int unit, uint32 trap_mask);
    int (*trap_get)(int unit, uint32 *trap_mask);
    int (*snoop_set)(int unit, uint32 snoop_mask);
    int (*snoop_get)(int unit, uint32 *snoop_mask);

    /* =============================================== */
    /* rate control */
    /* limit - best match */
    /* action - drop, flow control */
    int (*rate_config_set)(int unit, soc_pbmp_t pbmp, uint32 config_type, 
        uint32 value);
    int (*rate_config_get)(int unit, uint32 port, uint32 config_type, 
        uint32 *value);
    int (*rate_set)(int unit, soc_pbmp_t pbmp, int direction, uint32 limit, 
        uint32 burst_size);
    int (*rate_get)(int unit, uint32 port, int direction, uint32 *limit, 
        uint32 *burst_size);

    /* =============================================== */
    /* storm control */
    /* storm control enable/disable */
    int (*storm_control_enable_set)(int unit, uint32 port, uint8 enable);
    int (*storm_control_enable_get)(int unit, uint32 port, uint8 *enable);
    /* type - broadcast, multicast, unknow unicast */
    /* limit - best match */
    int (*storm_control_set)(int unit, soc_pbmp_t bmp, uint32 type, 
        uint32 limit, uint32 burst_size);
    int (*storm_control_get)(int unit, uint32 port, uint32 *type, 
        uint32 *limit, uint32 *burst_size);

    /* =============================================== */
    /* device specific property access */
    int (*dev_prop_get)(int unit, uint32 prop_type, uint32 *prop_val);
    int (*dev_prop_set)(int unit, uint32 prop_type, uint32 prop_val);

    int  (*mcast_init)(int unit);
    int  (*mcast_to_marl)(int unit, uint32 *marl_entry, uint32 *mcaddr);
    int  (*mcast_from_marl)(int unit, uint32 *mcaddr, uint32 *marl_entry);
    int  (*mcast_bmp_get)(int unit, uint32 *entry, soc_pbmp_t *bmp);
    int  (*mcast_bmp_set)(int unit, uint32 *entry, soc_pbmp_t bmp, uint32 flag);

    /* =============================================== */
    /* arl service */
    int (*arl_table_process)(int unit, uint32 *key, void *hw_arl, void  *sw_arl);
    int (*arl_sync)(int unit, uint32 *key, void *old_arl, void *new_arl);
    int (*arl_learn_enable_set)(int unit, soc_pbmp_t pbmp, uint32 mode);
    int (*arl_learn_enable_get)(int unit, soc_port_t port, uint32 *mode);

    /*=============================================== */
    /* queue service for bcm5396 */
    int (*queue_port_prio_to_queue_set)(int unit, uint8 port, 
                                                uint8 prio, uint8 queue_n);
    int (*queue_port_prio_to_queue_get)(int unit, uint8 port, 
                                                uint8 prio, uint8 *queue_n);
    int (*queue_port_dfsv_set)(int unit, uint8 port, uint8 dscp, 
                                                uint8 prio, uint8 queue_n);
    int (*queue_port_dfsv_get)(int unit, uint8 port, uint8 dscp, 
                                                uint8 *prio, uint8 *queue_n);
    int (*queue_prio_remap_set)(int unit, uint8 pre_prio, uint8 prio);
    int (*queue_prio_remap_get)(int unit, uint8 pre_prio, uint8 *prio);
    int (*queue_dfsv_remap_set)(int unit, uint8 dscp, uint8 prio);
    int (*queue_dfsv_remap_get)(int unit, uint8 dscp, uint8 *prio);
    int (*queue_rx_reason_set)(int unit, uint8 reason, uint32 queue);
    int (*queue_rx_reason_get)(int unit, uint8 reason, uint32 *queue);
    /*  cfp driver service */
    int (*cfp_init)(int unit);
    int (*cfp_action_get)(int unit, uint32* action, drv_cfp_entry_t* entry, uint32* act_param);
    int (*cfp_action_set)(int unit, uint32 action, drv_cfp_entry_t* entry, uint32 act_param1, uint32 act_param2);
    int (*cfp_control_get)(int unit, uint32 control_type, uint32 param1, uint32 *param2);
    int (*cfp_control_set)(int unit, uint32 control_type, uint32 param1, uint32 param2);
    int (*cfp_entry_read)(int unit, uint32 index, uint32 ram_type, drv_cfp_entry_t *entry);
    int (*cfp_entry_search)(int unit, uint32 flags, uint32 *index, drv_cfp_entry_t *entry);
    int (*cfp_entry_write)(int unit, uint32 index, uint32 ram_type, drv_cfp_entry_t *entry);
    int (*cfp_field_get)(int unit, uint32 mem_type, uint32 field_type, drv_cfp_entry_t* entry, uint32* fld_val);
    int (*cfp_field_set)(int unit, uint32 mem_type, uint32 field_type, drv_cfp_entry_t* entry, uint32* fld_val);
    int (*cfp_meter_get)(int unit, drv_cfp_entry_t* entry, uint32 *kbits_sec, uint32 *kbits_burst)  ;
    int (*cfp_meter_set)(int unit, drv_cfp_entry_t* entry, uint32 kbits_sec, uint32 kbits_burst);
    int (*cfp_qset_get)(int unit, uint32 qual, drv_cfp_entry_t *entry, uint32 *val);
    int (*cfp_qset_set)(int unit, uint32 qual, drv_cfp_entry_t *entry, uint32 val);
    int (*cfp_slice_id_select)(int unit, drv_cfp_entry_t *entry, uint32 *slice_id, uint32 flags);
    int (*cfp_slice_to_qset)(int unit, uint32 slice_id, drv_cfp_entry_t *entry);
    int (*cfp_stat_get)(int unit, uint32 stat_type, uint32 index, uint32* counter);
    int (*cfp_stat_set)(int unit, uint32 stat_type, uint32 index, uint32 counter);
    int (*cfp_udf_get)(int unit, uint32 port, uint32 udf_index, uint32 *offset, uint32 *base);
    int (*cfp_udf_set)(int unit, uint32 port, uint32 udf_index, uint32 offset, uint32 base);
    int (*cfp_ranger)(int unit, uint32 flags, uint32 min, uint32 max);
    int (*cfp_range_set)(int unit, uint32 type, uint32 id, uint32 param1, uint32 param2);
    int (*cfp_sub_qual_by_udf)(int unit, int enable, int slice_id, uint32 sub_qual, 
        drv_cfp_qual_udf_info_t * qual_udf_info);
    /* ethernet AV */
    int (*eav_control_set)(int unit, uint32 type, uint32 param);
    int (*eav_control_get)(int unit, uint32 type, uint32 *param);
    int (*eav_enable_set)(int unit, uint32 port, uint32 enable);
    int (*eav_enable_get)(int unit, uint32 port, uint32 *enable);
    int (*eav_link_status_set)(int unit, uint32 port, uint32 link);
    int (*eav_link_status_get)(int unit, uint32 port, uint32 *link);
    int (*eav_egress_timestamp_get)(int unit, uint32 port, uint32 *param);
    int (*eav_time_sync_set)(int unit, uint32 type, uint32 p0, uint32 p1);
    int (*eav_time_sync_get)(int unit, uint32 type, uint32 *p0, uint32 *p1);
    int (*eav_queue_control_set)(int unit, uint32 port, uint32 type, uint32 param);
    int (*eav_queue_control_get)(int unit, uint32 port, uint32 type, uint32 *param);
    int (*eav_time_sync_mac_set)(int unit, uint8* mac, uint16 ethertype);
    int (*eav_time_sync_mac_get)(int unit, uint8* mac, uint16 *ethertype);
    /* DoS attack prevention */
    int (*dos_enable_set)(int unit, uint32 type, uint32 param);
    int (*dos_enable_get)(int unit, uint32 type, uint32 *param);
} drv_if_t;


extern int drv_reg_read(int unit, uint32 addr, void *data, int len);
extern int drv_reg_write(int unit, uint32 addr, void *data, int len);
extern int drv_reg_length_get(int unit, uint32 reg);
extern uint32 drv_reg_addr(int unit, uint32 reg, int port, int index);
extern int drv_reg_field_set(int unit, uint32 reg, uint32 * regbuf, 
                                                    uint32 field, uint32 *fldbuf);
extern int drv_reg_field_get(int unit, uint32 reg, uint32 * regbuf, 
                                                    uint32 field, uint32 *fldbuf);
extern int drv_miim_read(int unit, uint8 phyid, uint8 phyaddr, uint16 *data);
extern int drv_miim_write(int unit, uint8 phyid, uint8 phyaddr, uint16 data);
extern int drv_phy_addr_set(int unit, int **map);
extern int drv_mac_driver_set(int unit, uint32 **p);
extern int drv_mem_length_get(int unit, uint32 mem, uint32 *data);
extern int drv_mem_width_get(int unit, uint32 mem, uint32 *data);
extern int drv_mem_read(int unit, uint32 mem, uint32 entry_id, 
                                                    uint32 count, uint32 *entry);
extern int drv_mem_write(int unit, uint32 mem, uint32 entry_id, 
                                                    uint32 count, uint32 *entry);
extern int drv_mem_field_get(int unit, uint32 mem, uint32 field_index, 
                                                    uint32 *entry, uint32 *fld_data);
extern int drv_mem_field_set(int unit, uint32 mem, uint32 field_index, 
                                                    uint32 *entry, uint32 *fld_data);
extern int drv_mem_clear(int unit, uint32 mem);
extern int drv_mem_search(int unit, uint32 mem, uint32 *key, 
                                                    uint32 *entry, uint32 *entry1, uint32 flags);
extern int drv_mem_insert(int unit, uint32 mem, uint32 *entry, uint32 flags);
extern int drv_mem_delete(int unit, uint32 mem, uint32 *entry, uint32 flags);
extern int drv_mem_cache_get(int unit, uint32 mem, uint32 *enable);
extern int drv_mem_cache_set(int unit, uint32 mem, uint32 enable);
extern int drv_counter_thread_set(int unit, uint32 thread_op, 
                                                    uint32 flags, int interval, soc_pbmp_t bmp);
extern int drv_counter_set(int unit, soc_pbmp_t port, 
                                                    uint32 counter_type, uint64 val);
extern int drv_counter_get(int unit, uint32 port, 
                                                    uint32 counter_type, uint64 *val);
extern int drv_counter_reset(int unit);
extern int drv_vlan_mode_set(int unit, uint32 mode);
extern int drv_vlan_mode_get(int unit, uint32 *mode);
extern int drv_port_vlan_pvid_set(int unit, uint32 bmp, 
                                                    uint32 vid, uint32 prio);
extern int drv_port_vlan_pvid_get(int unit, uint32 bmp, 
                                                    uint32 *vid, uint32 *prio);
extern int drv_port_vlan_set(int unit, uint32 port, soc_pbmp_t bmp);
extern int drv_port_vlan_get(int unit, uint32 port, soc_pbmp_t *bmp);
extern int drv_vlan_prop_set(int unit, uint32 prop_type, uint32 prop_val);
extern int drv_vlan_prop_get(int unit, uint32 prop_type, uint32 *prop_val);
extern int drv_vlan_prop_port_enable_set(int unit, uint32 prop_type, 
                            soc_pbmp_t bmp, uint32 val);
extern int drv_vlan_prop_port_enable_get(int unit, uint32 prop_type, 
                            uint32 port_n, uint32 *val);
extern int drv_vlan_vt_set(int unit, uint32 prop_type, uint32 vid, uint32 port, 
                            uint32 prop_val);
extern int drv_vlan_vt_get(int unit, uint32 prop_type, uint32 vid, uint32 port, 
                            uint32 *prop_val);
extern int drv_vlan_vt_add(int unit, uint32 vt_type, uint32 port, uint32 cvid, 
                            uint32 sp_vid, uint32 pri, uint32 mode);
extern int drv_vlan_vt_delete(int unit, uint32 vt_type, uint32 port, 
                            uint32 vid);
extern int drv_vlan_vt_delete_all(int unit, uint32 vt_type);
extern int drv_trunk_set(int unit, int tid, soc_pbmp_t bmp, uint32 flag, uint32 hash_op);
extern int drv_trunk_get(int unit, int tid, soc_pbmp_t *bmp, uint32 flag, uint32 *hash_op);
extern int drv_trunk_hash_field_add(int unit, uint32 field_type);
extern int drv_trunk_hash_field_remove(int unit, uint32 field_type);
extern int drv_trunk_timer_set(int unit, uint32 timer_type, uint32 time);
extern int drv_trunk_timer_get(int unit, uint32 timer_type, uint32 *time);
extern int drv_queue_mode_set(int unit, uint32 bmp, uint32 mode);
extern int drv_queue_mode_get(int unit, uint32 port, uint32 *mode);
extern int drv_queue_count_set(int unit, uint32 port_type, uint8 count);
extern int drv_queue_count_get(int unit, uint32 port_type, uint8 *count);
extern int drv_queue_WRR_weight_set(int unit, uint32 port_type, 
                                                    uint8 queue, uint32 weight);
extern int drv_queue_WRR_weight_get(int unit, uint32 port_type, 
                                                    uint8 queue, uint32 *weight);
extern int drv_queue_prio_set(int unit, uint8 prio, uint8 queue_n);
extern int drv_queue_prio_get(int unit, uint8 prio, uint8 *queue_n);
extern int drv_queue_tos_set(int unit, uint8 precedence, uint8 queue_n);
extern int drv_queue_tos_get(int unit, uint8 precedence, uint8 *queue_n);
extern int drv_queue_dfsv_set(int unit, uint8 code_point, uint8 queue_n);
extern int drv_queue_dfsv_get(int unit, uint8 code_point, uint8 *queue_n);
extern int drv_queue_mapping_type_set(int unit, soc_pbmp_t bmp, 
                                                    uint32 mapping_type, uint8 state);
extern int drv_queue_mapping_type_get(int unit, uint32 port, 
                                                    uint32 mapping_type, uint8 *state);
extern int drv_age_timer_set(int unit, uint32 enable, uint32 age_time);
extern int drv_age_timer_get(int unit, uint32 *enable, uint32 *age_time);
extern int drv_mac_set(int unit, soc_pbmp_t pbmp, uint32 mac_type, uint8* mac, uint32 bpdu_idx);
extern int drv_mac_get(int unit, uint32 port, uint32 mac_type, soc_pbmp_t *bmp,
                                        uint8* mac);
extern int drv_mirror_set(int unit, uint32 enable, uint32 monitor_port, 
                                            soc_pbmp_t ingress_bmp, soc_pbmp_t egress_bmp);
extern int drv_mirror_get(int unit, uint32 *enable, uint32 *monitor_port, 
                                            soc_pbmp_t *ingress_bmp, soc_pbmp_t *egress_bmp);
extern int drv_port_oper_mode_set(int unit, uint32 bmp, uint32 oper_mode);
extern int drv_port_oper_mode_get(int unit, int port_n, uint32 *oper_mode);
extern int drv_port_set(int unit, soc_pbmp_t bmp, uint32 prop_type, uint32 prop_val);
extern int drv_port_get(int unit, uint32 port_n, uint32 prop_type, 
                                                                        uint32 *prop_val);
extern int drv_port_advertise_set(int unit, soc_pbmp_t bmp, uint32 prop_mask);
extern int drv_port_advertise_get(int unit, int port_n, uint32 *prop_val);
extern int drv_port_status_get(int unit, uint32 port, 
                                                    uint32 status_type, uint32 *val);
extern int drv_phy_set(int unit, uint32 phy_id, uint32 reg_id, uint32 value);
extern int drv_phy_get(int unit, uint32 phy_id, uint32 reg_id, uint32 *value);
extern int drv_port_sw_mac_update(int unit, soc_pbmp_t bmp);
extern int drv_port_bitmap_get(int unit, uint32 port_type, soc_pbmp_t *bitmap);
extern int drv_security_set(int unit, soc_pbmp_t bmp, uint32 state, uint32 mask);
extern int drv_security_get(int unit, uint32 port, uint32 *state, uint32 *mask);
extern int drv_NULL_security_set(int unit, uint32 bmp, uint32 state, uint32 mask);
extern int drv_NULL_security_get(int unit, uint32 bmp, uint32 *state, uint32 *mask);
extern int drv_security_egress_set(int unit, soc_pbmp_t bmp, int enable);
extern int drv_security_egress_get(int unit, int port, int *enable);
extern int drv_mstp_config_set(int unit, uint32 vlan_id, uint32 mstp_gid);
extern int drv_mstp_config_get(int unit, uint32 vlan_id, uint32 *mstp_gid);
extern int drv_mstp_port_set(int unit, uint32 mstp_gid, 
                                        uint32 port, uint32 port_state);
extern int drv_mstp_port_get(int unit, uint32 mstp_gid, 
                                        uint32 port, uint32 *port_state);
extern int drv_trap_set(int unit, uint32 trap_mask);
extern int drv_trap_get(int unit, uint32 *trap_mask);
extern int drv_snoop_set(int unit, uint32 snoop_mask);
extern int drv_snoop_get(int unit, uint32 *snoop_mask);
extern int drv_rate_config_set(int unit, soc_pbmp_t pbmp, 
                                        uint32 config_type, uint32 value);
extern int drv_rate_config_get(int unit, uint32 port, 
                                        uint32 config_type, uint32 *value);
extern int drv_rate_set(int unit, soc_pbmp_t bmp, int direction, 
                                        uint32 limit, uint32 burst_size);
extern int drv_rate_get(int unit, uint32 port, int direction, 
                                        uint32 *limit, uint32 *burst_size);
extern int drv_storm_control_enable_set(int unit, uint32 port, uint8 enable);
extern int drv_storm_control_enable_get(int unit, uint32 port, uint8 *enable);
extern int drv_storm_control_set(int unit, soc_pbmp_t bmp, 
                                        uint32 type, uint32 limit, uint32 burst_size);
extern int drv_storm_control_get(int unit, uint32 port, 
                                        uint32 *type, uint32 *limit, uint32 *burst_size);
extern int drv_dev_prop_get(int unit, uint32 prop_type, uint32 *prop_val);
extern int drv_dev_prop_set(int unit, uint32 prop_type, uint32 prop_val);
extern int drv_mcast_init(int unit);
extern int drv_mcast_to_marl(int unit, uint32 *marl_entry,
              uint32 *mcaddr);
extern int drv_mcast_from_marl(int unit, uint32 *mcaddr,
                uint32 *marl_entry);
extern int  drv_mcast_bmp_get(int unit, uint32 *entry, soc_pbmp_t *bmp);
extern int  drv_mcast_bmp_set(int unit, uint32 *entry, soc_pbmp_t bmp, uint32 flag);

extern int drv_arl_table_process(int unit, uint32 *key,
                                                void *hw_arl, void  *sw_arl);
extern int drv_arl_sync(int unit, uint32 *key, void *old_arl, void *new_arl);
extern int drv_arl_learn_enable_set(int unit, soc_pbmp_t pbmp, uint32 mode);
extern int drv_arl_learn_enable_get(int unit, soc_port_t port, uint32 *mode);

extern int drv_queue_port_prio_to_queue_set(int unit, uint8 port, 
                                                uint8 prio, uint8 queue_n);
extern int drv_queue_port_prio_to_queue_get(int unit, uint8 port, 
                                                uint8 prio, uint8 *queue_n);
extern int drv_queue_port_dfsv_set(int unit, uint8 port, uint8 dscp, 
                                                uint8 prio, uint8 queue_n);
extern int drv_queue_port_dfsv_get(int unit, uint8 port, uint8 dscp, 
                                                uint8 *prio, uint8 *queue_n);
extern int drv_queue_prio_remap_set(int unit, uint8 pre_prio, uint8 prio);
extern int drv_queue_prio_remap_get(int unit, uint8 pre_prio, uint8 *prio);
extern int drv_queue_dfsv_remap_set(int unit, uint8 dscp, uint8 prio);
extern int drv_queue_dfsv_remap_get(int unit, uint8 dscp, uint8 *prio);
extern int drv_queue_rx_reason_set(int unit, uint8 reason, uint32 queue);
extern int drv_queue_rx_reason_get(int unit, uint8 reason, uint32 *queue);

/* For software ARL shadow table */
extern int _drv_arl_database_delete(int unit, int index, void * entry);
extern int _drv_arl_database_insert(int unit, int index, void * entry);
extern int _drv_arl_database_delete_by_portvlan
    (int unit, uint32 src_port, uint32 vlanid, uint32 flags);

extern int drv_eav_control_set(int unit, uint32 type, uint32 param);
extern int drv_eav_control_get(int unit, uint32 type, uint32 *param);
extern int drv_eav_enable_set(int unit, uint32 port, uint32 enable);
extern int drv_eav_enable_get(int unit, uint32 port, uint32 *enable);
extern int drv_eav_link_status_set(int unit, uint32 port, uint32 link);
extern int drv_eav_link_status_get(int unit, uint32 port, uint32 *link);
extern int drv_eav_egress_timestamp_get(int unit, uint32 port, uint32 *timestamp);
extern int drv_eav_time_sync_set(int unit, uint32 type, uint32 p0, uint32 p1);
extern int drv_eav_time_sync_get(int unit, uint32 type, uint32 *p0, uint32 *p1);
extern int drv_eav_queue_control_set(int unit, uint32 port, uint32 type, uint32 param);
extern int drv_eav_queue_control_get(int unit, uint32 port, uint32 type, uint32 *param);
extern int drv_eav_time_sync_mac_set(int unit, uint8* mac, uint16 ethertype);
extern int drv_eav_time_sync_mac_get(int unit, uint8* mac, uint16 *ethertype);

extern int drv_dos_enable_set(int unit, uint32 type, uint32 param);
extern int drv_dos_enable_get(int unit, uint32 type, uint32 *param);

#endif  /* end of drv_if.h */
