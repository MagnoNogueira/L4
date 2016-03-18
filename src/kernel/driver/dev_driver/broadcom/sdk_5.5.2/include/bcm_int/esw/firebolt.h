/*
 * $Id: firebolt.h,v 1.170.6.8 Broadcom SDK $
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
 * File:        firebolt.h
 * Purpose:     Function declarations for Firebolt bcm functions
 */

#ifndef _BCM_INT_FIREBOLT_H_
#define _BCM_INT_FIREBOLT_H_

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l2.h>
#include <soc/l2u.h>

#include <bcm/field.h>
#include <bcm/tunnel.h>
#ifdef INCLUDE_L3

/* Magic numbers  definition. */
#define BCM_XGS3_L3_IPV6_PREFIX_LEN      (128)
#define BCM_XGS3_L3_IPV4_PREFIX_LEN       (32)
#define BCM_XGS3_L3_ECMP_MAX       (32) /* Maximum number of ecmp paths.    */
#define BCM_XGS3_BITS_IN_BYTE       (8) /* Number of bits in one byte.      */
#define BCM_XGS3_FULL_BYTE       (0xff) /* All bits set in a byte.          */
#define BCM_XGS3_L3_L2CPU_NH_IDX    (0) /* Reserved trap to cpu nh index.   */
#define BCM_XGS3_L3_INVALID_INDEX  (-1) /* Generic invalid table index.     */
#define BCM_XGS3_EGRESS_IDX_MIN   (0x186A0) /* Egress object start index    */
#define BCM_XGS3_MPATH_EGRESS_IDX_MIN (0x30D40) /* Multipath egress object  */
                                                /* start index.             */
#define BCM_XGS3_MPLS_IF_EGRESS_IDX_MIN (0x493E0) /* MPLS interface egress  */
                                                  /* object start index.    */

/* Software table width. */
#define _BCM_SINGLE_WIDE    (1)
#define _BCM_DOUBLE_WIDE    (2)
#define _BCM_TRIPLE_WIDE    (3)
#define _BCM_QUAD_WIDE      (4)

 
/* Masks */
#define BCM_XGS3_L3_INVALID_ENT_SZ (0xffff)/* L3 table invalid entry size.  */
#define BCM_XGS3_UINT32_FULL_MASK  (0xffffffff)/* Full uint32 mask.         */
#define BCM_XGS3_L3_IP4_FULL_MASK  BCM_XGS3_UINT32_FULL_MASK/* Full ipv4 mask.*/

/* Software Tables operations. */
#define  BCM_XGS3_L3_OP_GET                 (1 << 0) /* Table get.   */
#define  BCM_XGS3_L3_OP_ADD                 (1 << 1) /* Table set.   */
#define  BCM_XGS3_L3_OP_DEL                 (1 << 2) /* Table remove.*/

/* L3 module internal flags definition. */
#define  _BCM_L3_SHR_EGRESS_MODE            (1 << 0) /* Egress object enable. */
#define  _BCM_L3_SHR_WITH_ID                (1 << 1) /* Index provided.       */
#define  _BCM_L3_SHR_UPDATE                 (1 << 2) /* Update existing entry.*/
#define  _BCM_L3_SHR_MATCH_DISABLE          (1 << 3) /* Don't try to match an */
                                                     /* entry to existing one */ 
                                                     /* but create a new .    */ 
#define  _BCM_L3_SHR_SKIP_INDEX_ZERO        (1 << 4) /* Skip entry 0 in a     */
                                                     /* table.                */
#define  _BCM_L3_SHR_WRITE_DISABLE          (1 << 5) /* Don't write to HW     */

/* Tunnel initiator entry types. */
#define BCM_XGS3_TUNNEL_INIT_NONE     (0x0)
#define BCM_XGS3_TUNNEL_INIT_V4       (0x1)
#define BCM_XGS3_TUNNEL_INIT_V6       (0x2)
#define BCM_XGS3_TUNNEL_INIT_MPLS     (0x3)



#define BCM_XGS3_L3_EGRESS_MODE_ISSET(_u_)  \
            ((BCM_XGS3_L3_FLAGS(_u_) & _BCM_L3_SHR_EGRESS_MODE))

#define BCM_XGS3_L3_MPATH_EGRESS_UNSUPPORTED(_u_)                \
    ((!BCM_XGS3_L3_INITIALIZED(_u_)) ? (BCM_E_INIT) :            \
     (!BCM_XGS3_L3_ECMP_MAX_PATHS(_u_)) ? (BCM_E_UNAVAIL) :      \
     (!BCM_XGS3_L3_EGRESS_MODE_ISSET(_u_))? (BCM_E_DISABLED) : BCM_E_NONE)  

/*
 * Supplied as opaque data to del by interface routines
 */
typedef struct _bcm_if_del_pattern_s {
    
    bcm_if_t l3_intf;   /* Interface index.                         */
    int negate;         /* 0 means match pattern; 1 means not match */
} _bcm_if_del_pattern_t;

/*
 * Supplied as opaque data to table walk routines.
 */
typedef struct _bcm_trvs_range_s {
    int current_count; /* Current entry index. */
    int start;         /* Start index to read. */
    int end;           /* Last index to read.  */
} _bcm_trvs_range_t;

/* General comparison/action  routine. */
                                /*unit, pattern,  data1, data2  cmp_result*/
typedef int (*bcm_xgs3_ent_op_cb)(int,   void *,  void*,  void*,  int *);

/*
 * Supplied as opaque data to show/update/delete 
 * matching a certain rule routines. 
 */
typedef struct _bcm_l3_trvrs_data_s { 
     int flags;                          /* Entry flags to match.         */
     void *pattern;                      /* Test pattern to match.        */
     bcm_xgs3_ent_op_cb cmp_cb;          /* Test match routine.           */
     bcm_xgs3_ent_op_cb op_cb;           /* Delete operation routine.     */
     bcm_l3_route_traverse_cb defip_cb;  /* Caller notification callback. */
     bcm_l3_egress_traverse_cb egress_cb;/* Caller notification callback. */
     void *cookie;                       /* User cookie to pass back incb.*/
} _bcm_l3_trvrs_data_t;

/* Tunnel initiator data. */ 
typedef struct _bcm_tnl_init_data_s {
     int tnl_idx;            /* Tunnel initiator entry index in hw. */
#ifdef BCM_EASYRIDER_SUPPORT
     int tnl_ttl;                  /* Tunnel initiator L3 ttl.            */
     bcm_tunnel_type_t tnl_type;   /* Tunnel initiator type.              */
     int adj_mac_idx;              /* Adjacent mac entry index in hw.     */
     int dscp_map;                 /* DSCP map id.                        */
#endif /* BCM_EASYRIDER_SUPPORT */
}_bcm_tnl_init_data_t;

/* Tunnel terminator type encoding. */
typedef struct _bcm_tnl_term_type_s {
    int tnl_auto;                  /* Tunnel terminator type AUTO/CONFIG. */
    int tnl_sub_type;              /* Tunnel terminator sutbype.          */
    int tnl_udp;                   /* UDP tunnel flag.                    */
    int tnl_pim_sm;                /* PIM_SM tunnel flag.                 */
    int tnl_gre;                   /* GRE tunnel flag.                    */
    int tnl_gre_v4_payload;        /* GRE IPv4 payload allowed.           */
    int tnl_gre_v6_payload;        /* GRE IPv4 payload allowed.           */
    int tnl_outer_hdr_ipv6;        /* Tunnel outer header is IPv6.        */
}_bcm_tnl_term_type_t;

/* Ingress interface descriptor. */
typedef struct _bcm_l3_ingress_intf_s {
    int intf_id;                   /* Interface id.         */
    uint32 flags;                  /* Inteface flags.       */
    int vrf;                       /* Virtual router id.    */
    int if_class;                  /* Interface class id.   */
} _bcm_l3_ingress_intf_t;

/* Generic l3 table extension structure. */
typedef struct _bcm_l3_tbl_ext_s {
    uint32 ref_count;         /* Entry reference count.   */
    uint16 data_hash;         /* Table key hash.          */
} _bcm_l3_tbl_ext_t;

/* Generic l3 table structure. */
typedef struct _bcm_l3_tbl_s {
    int idx_min;                     /* First table index.     */
    int idx_max;                     /* Last table index.      */
    int idx_maxused;                 /* Maximum used index.    */ 
    _bcm_l3_tbl_ext_t *ext_arr;      /* Entries extended info. */
} _bcm_l3_tbl_t;

/* Generic l3 table insertion/delete/update structure. */
typedef struct _bcm_l3_tbl_op_s {
    _bcm_l3_tbl_t *tbl_ptr;  /* Software table pointer.                  */
    uint32 oper_flags;       /* Operation flags. (Add/Replce) etc.       */
    uint8 width;             /* Number of entries required for one record*/ 
    int  entry_index;        /* Entry table index.                       */
    void *entry_buffer;      /* Table entry data.                        */
                             /* Entry hash calculation routine.          */
    int (*hash_func) (int, void *, uint16 *); 
                             /* Entry comparison routine.                */
    int (*cmp_func) (int, void *, int, int *);
                             /* Entry addtion routine.                   */
    int (*add_func) (int, int, void *);
                             /* Entry deletion routine.                  */
    int (*delete_func) (int, int);
} _bcm_l3_tbl_op_t;


/*
 * Ecmp configuration info.
 */
typedef struct _bcm_l3_ecmp_info_s {
    int ecmp_max_paths;         /* Maximum number of ECMP paths. */
    int ecmp_inuse;             /* No changes to ecmp_max_paths. */
} _bcm_l3_ecmp_info_t;

#if defined(BCM_MIRAGE_SUPPORT)
#define BCM_MIRAGE_L3_PORT             (0x1f)
#endif /* BCM_MIRAGE_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_MIRAGE_SUPPORT)

#define BCM_XGS3_L3_RP_MAX_PREFIXES(_unit_) \
           (soc_feature(unit, soc_feature_fp_routing_mirage) ? (126) : (254))
#define BCM_XGS3_V4_IN_V6_OFFSET          (12)
#define BCM_XGS3_L3_ENT_VALID         (1 << 0)

/* 
 * Raptor host/routes entry information. 
 */
typedef struct _bcm_rp_l3_entry_s {
    bcm_field_entry_t rule_hdlr; /* Hw FP entry handler.*/
    int prefixlen;               /* Prefix length.      */
    union {
        bcm_ip_t ip4;            /* IPv6 address.       */
        bcm_ip6_t ip6;           /* IPv4 address.       */
    }addr;
    int nh_idx;                  /* Next hop index.     */
    int flags;                   /* Entry flags.        */
} _bcm_rp_l3_entry_t;

/* 
 * FP groups handlers & info.  
 */  
typedef struct _bcm_rp_l3_fp_group_s {
    bcm_field_group_t grp_hdlr;  /* Hw FP group handler.             */
    int max_prefix_len_idx;      /* Max prefix length rule index.    */
    int min_prefix_len_idx;      /* Min prefix length rule index.    */
    int rule_count;              /* Number of active rules in group. */ 
} _bcm_rp_l3_fp_group_t;

/* 
 * Raptor L3 table for hosts & routes
 */
typedef struct _bcm_rp_l3_data_s {
    _bcm_rp_l3_fp_group_t *l3_grp;   /* Hw FP group handlers.           */
    _bcm_rp_l3_entry_t *l3_arr;      /* Hosts/Routes array.             */
    int route_maxused_rule_id;       /* Maximum rule id for routes.     */
    int host_minused_rule_id;        /* Minimum rule id for hosts.      */
    int rules_per_group;             /* Number of rules per group.      */
    int l3_group_num;                /* Number of FP groups used for l3.*/
    sal_mutex_t l3_mutex;            /* Protection mutex.               */
} _bcm_rp_l3_data_t;
#endif /* BCM_RAPTOR_SUPPORT || BCM_MIRAGE_SUPPORT */

/* 
 *  Hw operation calls;
 */
typedef struct _bcm_l3_hw_calls_s {

    /*
     *  Interface calls. 
     */
    /* Flush all the tables routine. */
    int (*l3_clear_all)(int unit);
    /* Interface info get routine. */
    int (*if_get) (int unit, _bcm_l3_intf_cfg_t *intf_info);
    /* Interface add routine. */
    int (*if_add) (int unit, _bcm_l3_intf_cfg_t *intf_info);
    /* Interface delete routine. */
    int (*if_del) (int unit, int ifindex);
    /* Get vrf info for interface. */
    int (*if_vrf_get)(int unit, _bcm_l3_intf_cfg_t *intf_info);
    /* Bind interface to vrf. */
    int (*if_vrf_bind)(int unit, _bcm_l3_intf_cfg_t *intf_info);
    /* Unbind interface from vrf. */
    int (*if_vrf_unbind)(int unit, _bcm_l3_intf_cfg_t *intf_info);
    /* Set interface classification group. */
    int (*if_group_set)(int unit, _bcm_l3_intf_cfg_t *intf_info);
    /* Get interface classification group. */
    int (*if_group_get)(int unit, _bcm_l3_intf_cfg_t *intf_info);
    /* Get interface as tunnel initiator info. */
    int (*if_tnl_init_get)(int unit, int ifindex, 
                           _bcm_tnl_init_data_t *tnl_info);
    /* Set interface as tunnel initiator. */
    int (*if_tnl_init_set)(int unit, int ifindex, 
                           _bcm_tnl_init_data_t *tnl_info);
    /* Unset interface as tunnel initiator. */
    int(*if_tnl_init_reset)(int unit, int ifindex);

    /*
     *  Next hop calls. 
     */
    /* Get next hop entry. */
    int (*nh_get)(int unit, int idx,  bcm_l3_egress_t  *nh_entry);
    /* Add next hop entry. */
    int (*nh_add)(int unit, int idx, void *nh_entry);
    /* Delete next hop entry. */
    int (*nh_del)(int unit, int idx);
    /* Next hop traverse routine. */ 
    int (*nh_update_match)(int unit, _bcm_l3_trvrs_data_t *trv_data);

    /* 
     * Ecmp group handling. 
     */
    /* Get ecmp group members. */
    int (*ecmp_grp_get)(int unit, int ecmp_idx, int ecmp_count, int *nh_idx);
    /* Add ecmp group. */
    int (*ecmp_grp_add)(int unit, int ecmp_grp, void *buf);
    /* Delete ecmp group. */
    int (*ecmp_grp_del)(int unit, int ecmp_grp);
    /* Preserve maximum ecmp group size. */
    int (*max_grp_size_set)(int unit, int grp_size);

    /* 
     *  L3 entry calls. 
     */
    /* Get l3 entry. */ 
    int (*l3_get)(int unit, _bcm_l3_cfg_t *l3cfg, int *nh_idx);
    /* Add l3 entry. */ 
    int (*l3_add)(int unit, _bcm_l3_cfg_t *l3cfg, int nh_idx);
    /* Add l3 entry. */ 
    int (*l3_del)(int unit, _bcm_l3_cfg_t *l3cfg);
    /* Get l3 entry by index. */
    int (*l3_get_by_idx)(int unit,  void *dma_ptr, int idx, 
                         _bcm_l3_cfg_t *l3cfg, int *nh_idx);
    /* Get l3 entry bucket start index*/ 
    int (*l3_bucket_get)(int unit,_bcm_l3_cfg_t *l3cfg, 
                         int *idx_primary, int *idx_secondary);

    /*
     *  Multicast calls. 
     */
    /* Read mcast entry. */
    int (*ipmc_get)(int unit, _bcm_l3_cfg_t *l3cfg);
    /* Add mcast entry. */
    int (*ipmc_add)(int unit, _bcm_l3_cfg_t *l3cfg);
    /* Delete mcast entry. */
    int (*ipmc_del)(int unit, _bcm_l3_cfg_t *l3cfg);
    /* Get l3 mcast entry by index. */
    int (*ipmc_get_by_idx)(int unit, void *dma_ptr, 
                           int idx, _bcm_l3_cfg_t *l3cfg);
    /* 
     *  LPM calls. 
     */
    /*  Get route entry. */  
    int (*lpm_get)(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx);
    /*  Add route entry. */  
    int (*lpm_add)(int unit, _bcm_defip_cfg_t *lpm_cfg, int nh_ecmp_idx);
    /*  Delete route entry. */  
    int (*lpm_del)(int unit, _bcm_defip_cfg_t *lpm_cfg);
    /*  Generic travers all entries call. */
    int (*lpm_update_match)(int unit, _bcm_l3_trvrs_data_t *trv_data);

#ifdef BCM_EASYRIDER_SUPPORT
    /*  Get adjacent mac entry. */
    int (*adj_mac_get)(int unit, int idx, bcm_mac_t mac);
    /*  Add adjacent mac entry. */
    int (*adj_mac_add)(int unit, int idx, void *buf);
    /*  Delete adjacent mac entry. */
    int (*adj_mac_del)(int unit, int idx);
#endif /* BCM_EASYRIDER_SUPPORT */
    /* 
     *  Ingress interface calls.
     */
    /* Get ingress interface entry. */
    int (*ing_intf_get)(int unit, _bcm_l3_ingress_intf_t *iif);

    /* Add ingress interface entry. */
    int (*ing_intf_add)(int unit, _bcm_l3_ingress_intf_t *iif);

    /* Delete ingress interface entry. */
    int (*ing_intf_del)(int unit, int intf_id);


    /* 
     *  Tunnel terminator calls.
     */
    /* Get tunnel terminator entry. */
    int (*tnl_term_get)(int unit, bcm_tunnel_terminator_t *info);

    /* Add tunnel terminator entry. */
    int (*tnl_term_add)(int unit, bcm_tunnel_terminator_t *info);

    /* Delete tunnel terminator entry. */
    int (*tnl_term_del)(int unit, bcm_tunnel_terminator_t *info);

    /* 
     *  Tunnel initiator calls.
     */
    /* Get tunnel initiator entry. */
    int (*tnl_init_get)(int unit, int idx, bcm_tunnel_initiator_t *tnl_info);
    /* Add tunnel initiator entry. */
    int (*tnl_init_add)(int unit, int idx, void *buf);
    /* Delete tunnel initiator entry. */
    int (*tnl_init_del)(int unit, int idx);

    /* 
     *  Tunnel dscp calls.
     */
    /* Get tunnel dscp entry. */
    int (*tnl_dscp_get)(int unit, int id, bcm_tunnel_dscp_map_t *tnl_info);
    /* Add tunnel dscp entry. */
    int (*tnl_dscp_set)(int unit, int id, void *buf);
} _bcm_l3_hw_calls_t;

/* Table propertires & entry sizes.  */
typedef struct _bcm_l3_table_prop_s {
    soc_mem_t  tbl_mem;          /* Table memory location. */
    uint16     tbl_entry_sz;     /* Table entry size.      */
    void       *tbl_mem_fields;  /* Table memory fields.   */ 
} _bcm_l3_table_prop_t;

/* Intra chip common fields. */

/* L3 interface memory fields. */
typedef struct _bcm_l3_intf_fields_s {
    soc_field_t vlan_id;         /* Vlan id field.                */
    soc_field_t mac_addr;        /* Physical address field.       */ 
    soc_field_t ttl;             /* Time To Live threshold field. */
    soc_field_t tnl_id;          /* Tunnel initiator id field.    */
    soc_field_t l2_switch;       /* L2 switch interface field.    */
}_bcm_l3_intf_fields_t;

/* Next hop table fields. */
typedef struct _bcm_l3_nh_fields_s {
    soc_field_t mac_addr;        /* Physical address field.       */
    soc_field_t ifindex;         /* Interface index field.        */
    soc_field_t module;          /* Module id field.              */
    soc_field_t port_tgid;       /* Port/trung id field.          */
}_bcm_l3_nh_fields_t;

/* l3 table fields. */
typedef struct _bcm_l3_fields_s {
    soc_field_t vrf;             /* Virtual router id field.       */
    soc_field_t hit;             /* Entry used field.              */
    soc_field_t rpe;             /* Priority override field.       */
    soc_field_t dst_discard;     /* Destination discard field.     */
    soc_field_t nh_idx;          /* Next hop index field.          */
    soc_field_t priority;        /* Priority field.                */
    soc_field_t valid;           /* Valid entry field.             */
    soc_field_t l3mc_index;      /* ????  field.                   */
    soc_field_t vlan_id;         /* Vlan id field.                 */
#ifdef BCM_FIREBOLT_SUPPORT
    soc_field_t v6_entry;        /* IPv6 field.                    */
    soc_field_t ipmc_entry;      /* IPMC field.                    */
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_TRX_SUPPORT
    soc_field_t class_id;         /* Class id field.               */
#endif /* BCM_TRX_SUPPORT */
}_bcm_l3_fields_t;

/* L3 chip operation structure. */
typedef struct _bcm_l3_hw_op_table_s {
    _bcm_l3_table_prop_t l3_intf;       /* L3 Interface memory.            */
    _bcm_l3_table_prop_t l3_ing_intf;   /* L3 Ingress Interface memory.    */
    _bcm_l3_table_prop_t l3_nh;         /* L3 Next hop table memory.       */
    _bcm_l3_table_prop_t l3_v4;         /* L3 v4 table memory.             */
    _bcm_l3_table_prop_t l3_v6;         /* L3 v6 table memory.             */
    _bcm_l3_table_prop_t l3_ipmc_v4;    /* L3 v4 ipmc table memory.        */
    _bcm_l3_table_prop_t l3_ipmc_v6;    /* L3 v6 ipmc table memory.        */
    _bcm_l3_table_prop_t l3_defip;      /* L3 route table memory.          */
    _bcm_l3_table_prop_t l3_tnl_init_v4;/* Tunnel initiator v4 tbl memory. */
    _bcm_l3_table_prop_t l3_tnl_init_v6;/* Tunnel initiator v6 tbl memory. */
    _bcm_l3_table_prop_t l3_tnl_term;   /* Tunnel terminator tbl memory.   */
    _bcm_l3_table_prop_t l3_tnl_dscp;   /* Tunnel DSCP tbl memory.         */
    _bcm_l3_table_prop_t l3_ecmp;       /* ECMP table memory.              */
    _bcm_l3_table_prop_t l3_v6_prefix_map;/* IPv6 prefix map memory.       */
#ifdef BCM_EASYRIDER_SUPPORT
    _bcm_l3_table_prop_t l3_adj_mac;    /* ADJ_MAC table memory.           */
#endif /* BCM_EASYRIDER_SUPPORT */
    _bcm_l3_hw_calls_t *hw_call;        /* Chip programming routines set.  */ 
} _bcm_l3_hw_op_table_t;


/* L3 software tables. */
typedef struct _bcm_l3_module_data_s {
    uint32                   l3_op_flags;   /* Unit operation flags.     */ 
    _bcm_l3_hw_op_table_t    l3_hw_op;      /* HW operation table.       */
    _bcm_l3_ecmp_info_t      l3_ecmp_info;  /* Ecmp groups size info.    */
    _bcm_l3_tbl_t            l3_next_hop;   /* Next hop table.           */
    _bcm_l3_tbl_t            l3_ecmp_grp;   /* Ecmp groups table.        */
    _bcm_l3_tbl_t            l3_tnl_init;   /* Tunnel initiator table.   */
#ifdef BCM_EASYRIDER_SUPPORT
    _bcm_l3_tbl_t            l3_adj_mac;    /* Adjacent mac table.       */ 
#endif /* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_MIRAGE_SUPPORT)
    _bcm_rp_l3_data_t        l3_rp_prefix;   /* Raptor prefixes table.    */  
#endif /* BCM_RAPTOR_SUPPORT || BCM_MIRAGE_SUPPORT */
} _bcm_l3_module_data_t;

/* Macros. */
/* Software table access macros. */ 
#define BCM_XGS3_L3_TBL(_u_, _var_)      (l3_module_data[_u_]->l3_ ## _var_)
#define BCM_XGS3_L3_TBL_PTR(_u_, _var_)  (&(BCM_XGS3_L3_TBL(_u_, _var_)))

/* L3 module operational flags. */
#define BCM_XGS3_L3_FLAGS(_u_)           (l3_module_data[_u_]->l3_op_flags)

/* HW calls verification & execution macros. */
/* Check if hw call is set. */
#define BCM_XGS3_L3_HWCALL_CHECK(_u_, FN)              \
            (BCM_XGS3_L3_TBL(_u_, hw_op).hw_call &&    \
            (BCM_XGS3_L3_TBL(_u_, hw_op).hw_call->FN))
/* Call execution macro. */
#define BCM_XGS3_L3_HWCALL_EXEC(_u_, FN)            \
            (*(BCM_XGS3_L3_TBL(_u_, hw_op).hw_call->FN))
/* Get function pointer macro. */
#define BCM_XGS3_L3_HWCALL(_u_, FN)  \
            (BCM_XGS3_L3_TBL(_u_, hw_op).hw_call->FN)

/* Generic memory allocation routine. */
#define BCM_XGS3_L3_ALLOC(_ptr_,_size_,_descr_)                      \
            do {                                                     \
                if ((FALSE == BCM_XGS3_L3_INITIALIZED(unit)) ||      \
                    (NULL == _ptr_)) {                               \
                   _ptr_ = sal_alloc(_size_, _descr_);               \
                }                                                    \
                if((_ptr_) != NULL) {                                \
                    sal_memset((_ptr_), 0, _size_);                  \
                }                                                    \
            } while (0)

/* Calculate final comparison result based on compare & negate  */
#define BCM_XGS3_L3_IS_EQUAL(_res_, _negate_)                 \
            (((_res_ == BCM_L3_CMP_EQUAL) && (!_negate_)) ||  \
             ((_res_ != BCM_L3_CMP_EQUAL) && (_negate_)))

/* Decrement counter if positive. */
#define BCM_XGS3_L3_DEC_IF_POSITIVE(_val_) ((_val_ > 0) ?  _val_-- : _val_)

#ifdef BCM_EASYRIDER_SUPPORT
/* Invalid mpls label.*/
#define BCM_XGS3_L3_MPLS_LBL_MAX      (0xfffff) 
#define BCM_XGS3_L3_INVALID_MPLS_LABEL  (BCM_XGS3_L3_MPLS_LBL_MAX + 1)
#define BCM_XGS3_L3_MPLS_LBL_VALID(_label_)                      \
             (_label_ <= BCM_XGS3_L3_MPLS_LBL_MAX)

#define BCM_ER_VRF_INVALID_RETURN(_u_, _v6_, _vrf_)        \
    if (SOC_IS_EASYRIDER(_u_)) {                           \
        if (((_vrf_) < BCM_L3_VRF_DEFAULT) ||              \
            ((_v6_) && ((_vrf_) != BCM_L3_VRF_DEFAULT))){  \
            return (BCM_E_PARAM);                          \
        }                                                  \
    }
#else  /* BCM_EASYRIDER_SUPPORT */
#define BCM_ER_VRF_INVALID_RETURN(_u_, _v6_, _vrf_)
#endif /* BCM_EASYRIDER_SUPPORT */

/* Debug messages macros. */
#define BCM_XGS3_L3_OUT(flags, stuff)  BCM_DEBUG(((flags) | BCM_DBG_L3), stuff)
#define BCM_XGS3_L3_WARN(stuff)        BCM_XGS3_L3_OUT(BCM_DBG_WARN, stuff)
#define BCM_XGS3_L3_ERR(stuff)         BCM_XGS3_L3_OUT(BCM_DBG_ERR, stuff)
#define BCM_XGS3_L3_VERB(stuff)        BCM_XGS3_L3_OUT(BCM_DBG_VERBOSE, stuff)

/* L3 memories map & configuration structure. */
#define BCM_L3_MEM_ENT_SIZE(_u_, _mem_)         \
             WORDS2BYTES(soc_mem_entry_words((_u_), (_mem_)))

#define BCM_XGS3_L3_MEM(_u_, _var_)                          \
            ((BCM_XGS3_L3_TBL(_u_, hw_op).l3_ ## _var_).tbl_mem)

#define BCM_XGS3_L3_ENT_SZ(_u_, _var_)                       \
            ((BCM_XGS3_L3_TBL(_u_, hw_op).l3_ ## _var_).tbl_entry_sz)

#define BCM_XGS3_L3_MEM_FIELDS(_u_, _var_)                   \
            ((BCM_XGS3_L3_TBL(_u_, hw_op).l3_ ## _var_).tbl_mem_fields)

#define BCM_XGS3_L3_HW_CALL(_u_)                             \
            (BCM_XGS3_L3_TBL(_u_, hw_op).hw_call)

/* Tables protection macros. */
#define BCM_XGS3_L3_LOCK(_u_, _var_)                         \
            MEM_LOCK(_u_, BCM_XGS3_L3_MEM(_u_, _var_))

#define BCM_XGS3_L3_UNLOCK(_u_, _var_)                       \
            MEM_UNLOCK(_u_, BCM_XGS3_L3_MEM(_u_, _var_))


/* Global sdk L3 structure references. */
/* L3 initialized flag. */
#define BCM_XGS3_L3_INITIALIZED(_u_) _bcm_l3_bk_info[_u_].l3_initialized
/* L3 interface count. */
#define BCM_XGS3_L3_IF_COUNT(_u_) _bcm_l3_bk_info[_u_].l3intf_count
/* L3 interface inuse bitmap array. */
#define BCM_XGS3_L3_IF_INUSE(_u_) _bcm_l3_bk_info[_u_].l3_intf_used
/* L3 interface added to arl bitmap. */
#define BCM_XGS3_L3_IF_ADD2ARL(_u_) _bcm_l3_bk_info[_u_].add_to_arl
/* Interface table size. */
#define BCM_XGS3_L3_IF_TBL_SIZE(_u_) _bcm_l3_bk_info[_u_].l3_intf_table_size
/* L3 ingress interface count. */
#define BCM_XGS3_L3_ING_IF_COUNT(_u_) _bcm_l3_bk_info[_u_].l3iif_count
/* L3 ingress interface inuse bitmap array. */
#define BCM_XGS3_L3_ING_IF_INUSE(_u_) _bcm_l3_bk_info[_u_].l3_iif_used
/* L3 ingress interface table size. */
#define BCM_XGS3_L3_ING_IF_TBL_SIZE(_u_) _bcm_l3_bk_info[_u_].l3_iif_table_size
/* Next hop table size. */
#define BCM_XGS3_L3_NH_TBL_SIZE(_u_) _bcm_l3_bk_info[_u_].nh_table_size
/* Ecmp table size. */
#define BCM_XGS3_L3_ECMP_TBL_SIZE(_u_) _bcm_l3_bk_info[_u_].ecmp_table_size
/* Defip table size. */
#define BCM_XGS3_L3_DEFIP_TBL_SIZE(_u_) _bcm_l3_bk_info[_u_].defip_table_size
/* Tunnel table size. */
#define BCM_XGS3_L3_TUNNEL_TBL_SIZE(_u_) _bcm_l3_bk_info[_u_].tunnel_table_size
/* Tunnel DSCP map inuse bitmap array. */
#define BCM_XGS3_L3_TUNNEL_DSCP_MAP_INUSE(_u_) \
    _bcm_l3_bk_info[_u_].dscp_map_used
/* Number of Tunnel DSCP map instances. */
#define BCM_XGS3_L3_TUNNEL_DSCP_MAP_TBL_SIZE(_u_) \
    _bcm_l3_bk_info[_u_].num_dscp_maps

/* Cpu interface index. */
#define BCM_XGS3_L3_L2CPU_INTF_IDX(_u_)                     \
	        (soc_mem_index_max((_u_), BCM_XGS3_L3_MEM(unit, intf)))

/* For Easyrider, reserve 8 L3_INTF entries MPLS incoming interfaces */
#define BCM_XGS3_L3_MPLS_INTF_IDX_MIN(_u_)                  \
                (SOC_IS_EASYRIDER((_u_)) ?                  \
                 BCM_XGS3_L3_L2CPU_INTF_IDX(_u_) - 9 :      \
                 BCM_XGS3_L3_L2CPU_INTF_IDX(_u_))

#define BCM_XGS3_L3_MPLS_INTF_IDX_MAX(_u_)                  \
                (SOC_IS_EASYRIDER((_u_)) ?                  \
                 BCM_XGS3_L3_L2CPU_INTF_IDX(_u_) - 1 :      \
                 BCM_XGS3_L3_L2CPU_INTF_IDX(_u_))


/* Set interface index */
#define BCM_XGS3_L3_INTF_IDX_SET(_l3_intf_,_ifindex_)           \
            {                                                   \
                _l3_intf_.l3i_index = _ifindex_;                \
                _l3_intf_.l3i_flags |= BCM_L3_WITH_ID;          \
            }

/* Copy IPv4 address into lower 4 bytes of IPv6 address. */
#define BCM_XGS3_L3_CP_V4_ADDR_TO_V6(_v6_,_v4_)                      \
            {                                                        \
                sal_memset(_v6_, 0, 12);                             \
                sal_memcpy((uint8 *)_v6_ + BCM_XGS3_V4_IN_V6_OFFSET, \
                       &_v4_, sizeof(bcm_ip_t));                     \
            }

/* Unipath egress object index valid. */
#define BCM_XGS3_L3_EGRESS_IDX_VALID(_u_, _idx_)                     \
            ((_idx_ >= BCM_XGS3_EGRESS_IDX_MIN ) &&                  \
             (_idx_ < BCM_XGS3_EGRESS_IDX_MIN + BCM_XGS3_L3_NH_TBL_SIZE(_u_)))

/* Multipath egress object index valid. */
#define BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(_u_, _idx_)              \
       ((_idx_ >= BCM_XGS3_MPATH_EGRESS_IDX_MIN) &&                 \
        (_idx_ < (BCM_XGS3_MPATH_EGRESS_IDX_MIN +                   \
                  BCM_XGS3_L3_ECMP_TBL_SIZE(_u_))))

/* Counter update macro. */
#define BCM_XGS3_L3_GEN_CNTR_UPD(_cntr_,_op_)               \
            _cntr_ += (_op_)?(1):(-1)

/* Trunk id validation macro. */
#define BCM_XGS3_L3_IF_INVALID_TGID_RETURN(unit, _flags_,_port_tgid_)      \
            if (_flags_ & BCM_L3_TGID) {                                   \
                if (_port_tgid_ &                                          \
                    ~(BCM_TGID_PORT_TRUNK_MASK(unit) |                     \
                      BCM_TGID_PORT_TRUNK_MASK_HI(unit))){                 \
                    return (BCM_E_BADID);                                  \
                }                                                          \
            }
/* Check if l3 entry is multicast. */
#define BCM_XGS3_L3_MCAST_ENTRY(_l3cfg_)                     \
            ((((_l3cfg_)->l3c_flags & BCM_L3_IP6) &&         \
              (BCM_IP6_MULTICAST((_l3cfg_)->l3c_ip6))) ||    \
             ((!((_l3cfg_)->l3c_flags & BCM_L3_IP6)) &&      \
              (BCM_IP4_MULTICAST((_l3cfg_)->l3c_ip_addr)))) 

/* Lookup error or overrite not supported return macro. */
#define BCM_XGS3_IF_ERROR_OR_ENTRY_EXISTS_RETURN(_rv_)      \
            if (_rv_ < 0 && _rv_ != BCM_E_NOT_FOUND) {      \
                return rv;                                  \
            } else if (!_rv_) {                             \
                return BCM_E_EXISTS;                        \
            }

/* Lookup error return macro. */ 
#define BCM_XGS3_LKUP_IF_ERROR_RETURN(_rv_, _not_found_err_code_) \
            if (_rv_ == SOC_E_NOT_FOUND) {                        \
                return _not_found_err_code_;                      \
            } else if (_rv_ < 0) {                                \
                return _rv_;                                      \
            }

#define BCM_XGS3_V6_FLD32_GET_IF_FBX(_u_, _v6_, _mem_, _buf_, _fld_) \
          ((_v6_ && SOC_IS_FBX(_u_) &&                               \
             SOC_MEM_FIELD_VALID(_u_, _mem_, _fld_)) ?               \
               soc_mem_field32_get(_u_, _mem_, _buf_, _fld_) : 0)

#define BCM_XGS3_V6_FLD32_SET_IF_FBX(_u_, _v6_, _mem_, _buf_, _fld_, _val_)\
            if (_v6_ && SOC_IS_FBX(_u_) &&                                 \
                SOC_MEM_FIELD_VALID(_u_, _mem_, _fld_)) {                  \
               soc_mem_field32_set(_u_, _mem_, _buf_, _fld_, _val_);       \
            }

#ifdef BCM_FIREBOLT_SUPPORT
/* Increment counter if device is firebolt. */ 
#define BCM_XGS3_INC_IF_FIREBOLT(_u_,_idx_)               \
            _idx_ +=  (SOC_IS_FBX(_u_) && (!SOC_IS_RAPTOR(_u_))) ? 1 : 0 
 
/* Calculate ip4 based index based on ip6/ipmc entry index. */ 
#define BCM_XGS3_FB_IP4_BASED_IDX_CALC(_u_,_idx_,_flags_)  \
            if (SOC_IS_FBX(_u_) && (!SOC_IS_RAPTOR(_u_))){ \
                if (_flags_ & BCM_L3_IP6) {                \
                    _idx_ *= 2;                            \
                    if (_flags_ & BCM_L3_IPMC) {           \
                        _idx_ *= 2;                        \
                    }                                      \
                }                                          \
            }
/* Calculate ip6/ipmc index based on ip4 entry index. */ 
#define BCM_XGS3_FB_IPMC_IP6_IDX_CALC(_u_,_idx_,_result_,_flags_) \
            if(SOC_IS_FBX(_u_) && (!SOC_IS_RAPTOR(_u_))) { \
                _result_ = _idx_;                                 \
                if(_flags_ & BCM_L3_IP6) {                        \
                    _result_ >>= 1;                               \
                    _idx_+= (_flags_ & BCM_L3_IPMC)? 3 : 1;       \
                }                                                 \
            }        
#endif /* BCM_FIREBOLT_SUPPORT */
/* L3 table usage statistics.*/
/* L3 table size. */
#define BCM_XGS3_L3_TBL_SIZE(_u_)      _bcm_l3_bk_info[_u_].l3_table_size
#define BCM_XGS3_L3_IP4_CNT(_u_)       _bcm_l3_bk_info[_u_].l3_ip4_added
#define BCM_XGS3_L3_IP6_CNT(_u_)       _bcm_l3_bk_info[_u_].l3_ip6_added
#define BCM_XGS3_L3_IP4_IPMC_CNT(_u_)  _bcm_l3_bk_info[_u_].l3_ipmc4_added
#define BCM_XGS3_L3_IP6_IPMC_CNT(_u_)  _bcm_l3_bk_info[_u_].l3_ipmc6_added

 
/* Defip table usage statistics.*/
#define BCM_XGS3_L3_DEFIP_TBL_SIZE(_u_) _bcm_l3_bk_info[_u_].defip_table_size
#define BCM_XGS3_L3_DEFIP_IP4_CNT(_u_)  _bcm_l3_bk_info[_u_].defip_count
#define BCM_XGS3_L3_DEFIP_IP6_CNT(_u_)  _bcm_l3_bk_info[_u_].defip_ip6_count
#define BCM_XGS3_L3_DEFIP_CNT_UPD(_u_,_ipv6_,_o_)                             \
            if(_ipv6_) {                                                      \
               BCM_XGS3_L3_GEN_CNTR_UPD(BCM_XGS3_L3_DEFIP_IP6_CNT(_u_),_o_);  \
            }                                                                 \
            else {                                                            \
               BCM_XGS3_L3_GEN_CNTR_UPD(BCM_XGS3_L3_DEFIP_IP4_CNT(_u_),_o_);  \
            }
/* Defip table usage update macros. */ 
#define BCM_XGS3_L3_DEFIP_CNT_INC(_u_,_ipv6_)      \
            BCM_XGS3_L3_DEFIP_CNT_UPD(_u_,_ipv6_,1)
#define BCM_XGS3_L3_DEFIP_CNT_DEC(_u_,_ipv6_)      \
            BCM_XGS3_L3_DEFIP_CNT_UPD(_u_,_ipv6_,0)

/* Route operation params verification macro. */
#define BCM_XGS3_L3_DEFIP_INVALID_PARAMS_RETURN(_u_, _lpm_cfg_)              \
            if (!_lpm_cfg_) {                                                \
                return (BCM_E_PARAM);                                        \
            }                                                                \
            if ((_lpm_cfg_)->defip_flags & BCM_L3_IP6) {                     \
                if((_lpm_cfg_)->defip_sub_len > BCM_L3_IP6_MAX_NETLEN) {     \
                    return (BCM_E_PARAM);                                    \
                }                                                            \
                BCM_ER_VRF_INVALID_RETURN(_u_, TRUE, (_lpm_cfg_)->defip_vrf);\
            } else if((_lpm_cfg_)->defip_sub_len > BCM_L3_IP4_MAX_NETLEN) {  \
                  return (BCM_E_PARAM);                                      \
            }                                                                \
/* Traverse table not empty check macro. */ 
#define BCM_XGS3_L3_IF_DEFIP_CNT_ZERO_RETURN(_u_, _ipv6_)            \
            if ((_ipv6_ && !BCM_XGS3_L3_DEFIP_IP6_CNT(_u_)) ||       \
                (!_ipv6_ && !BCM_XGS3_L3_DEFIP_IP4_CNT(_u_))) {      \
                  return BCM_E_NONE;                                 \
            }

/* Generic table management macros. */
#define BCM_XGS3_L3_ENT_HASH(_tbl_ptr, _i_) ((_tbl_ptr)->ext_arr[_i_]).data_hash

/* Compare entry hash macro. */
#define BCM_XGS3_L3_ENT_HASH_CMP(_tbl_ptr, _i_, _h_)             \
            (_h_ == ((_tbl_ptr)->ext_arr[_i_]).data_hash)

/* Reset entry extended info macro. */
#define BCM_XGS3_L3_ENT_RESET(_tbl_ptr, _i_, _width_)                         \
            {                                                                 \
                int _idx_;                                                    \
                for (_idx_ = 0; _idx_ < (_width_); _idx_++) {                 \
                     sal_memset(((_tbl_ptr)->ext_arr + (_i_ + _idx_)), 0,     \
                           sizeof(_bcm_l3_tbl_ext_t));                        \
                }                                                             \
            }

/* Init entry extended info macro. */
#define BCM_XGS3_L3_ENT_INIT(_tbl_ptr, _i_, _width_, _hash_)                  \
            {                                                                 \
                int _idx_;                                                    \
                for (_idx_ = 0; _idx_ < (_width_); _idx_++) {                 \
                    ((_tbl_ptr)->ext_arr[(_i_) + _idx_]).data_hash = (_hash_);\
                    if (0 == ((_tbl_ptr)->ext_arr[(_i_) + _idx_]).ref_count) {\
                        ((_tbl_ptr)->ext_arr[(_i_)]).ref_count = 1;           \
                    }                                                         \
                }                                                             \
            }

/* Get entry reference count. */
#define BCM_XGS3_L3_ENT_REF_CNT(_tbl_ptr, _i_)                   \
            ((_tbl_ptr)->ext_arr[_i_]).ref_count

/* Increment entry reference count. */
#define BCM_XGS3_L3_ENT_REF_CNT_INC(_tbl_ptr, _i_, _width_)           \
            {                                                         \
                int _idx_ = (_width_);                                \
                for (_idx_ = 0; _idx_ < (_width_); _idx_++) {         \
                    ((_tbl_ptr)->ext_arr[(_i_) + _idx_]).ref_count++; \
                }                                                     \
            }
/* Decrement entry reference count. */
#define BCM_XGS3_L3_ENT_REF_CNT_DEC(_tbl_ptr, _i_, _width_)           \
            {                                                         \
                int _idx_ = (_width_);                                \
                for (_idx_ = 0; _idx_ < (_width_); _idx_++) {         \
                    ((_tbl_ptr)->ext_arr[(_i_) + _idx_]).ref_count--; \
                }                                                     \
            }

/* Maximum number of paths in ecmp group on device. */
#define BCM_XGS3_L3_ECMP_MAX_PATHS(_u_)                  \
            BCM_XGS3_L3_TBL(_u_, ecmp_info).ecmp_max_paths

/* Maximum ecmp group id. */ 
#define BCM_XGS3_L3_ECMP_IN_USE(_u_)                     \
            BCM_XGS3_L3_TBL(_u_, ecmp_info).ecmp_inuse

/* Reset ecmp group next hops macro. */
#define BCM_XGS3_L3_ECMP_GROUP_RESET(_p_)                \
            sal_memset(_p_,0,(BCM_XGS3_L3_ECMP_MAX * sizeof(int)))

/* Calculate number of ecmp group next hops. */
#define BCM_XGS3_L3_ECMP_GROUP_SIZE(_ecmp_count_)  (_ecmp_count_ + 1)

#define BCM_XGS3_L3_CNTRS_RESET(_u_)                           \
            do {                                               \
                _bcm_l3_bk_info[_u_].l3intf_count = 0;         \
                _bcm_l3_bk_info[_u_].defip_count= 0;           \
                _bcm_l3_bk_info[_u_].defip_ip6_count= 0;       \
                _bcm_l3_bk_info[_u_].l3_ip4_added= 0;          \
                _bcm_l3_bk_info[_u_].l3_ip6_added= 0;          \
                _bcm_l3_bk_info[_u_].l3_ipmc4_added= 0;        \
                _bcm_l3_bk_info[_u_].l3_ipmc6_added= 0;        \
            } while (0)


#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_MIRAGE_SUPPORT)
#undef BCM_XGS3_L3_FORWARD_IPV4_WITH_OPTIONS 
#define BCM_XGS3_L3_EVEN_IDX(_rule_idx_)  ((_rule_idx_ & 0x1) ? 0 : 1)
#define BCM_XGS3_L3_CNTR_MODE(_rule_idx_)                                  \
            (!BCM_XGS3_L3_EVEN_IDX(_rule_idx_) ?                           \
            BCM_FIELD_COUNTER_MODE_NO_YES : BCM_FIELD_COUNTER_MODE_YES_NO)

#define BCM_XGS3_L3_RP_HOST_ENTRY(_ent_)                               \
            (((_ent_->flags & BCM_L3_IP6) &&                           \
              (_ent_->prefixlen == BCM_XGS3_L3_IPV6_PREFIX_LEN)) ||    \
             ((!(_ent_->flags & BCM_L3_IP6)) &&                        \
              (_ent_->prefixlen == BCM_XGS3_L3_IPV4_PREFIX_LEN)))
#endif /* BCM_RAPTOR_SUPPORT || BCM_MIRAGE_SUPPORT */
#endif /*  INCLUDE_L3  */
/* Read memory macro. */
#define BCM_XGS3_MEM_READ(_u_, _m_, _idx_, _buf_)                          \
	        ((SOC_MEM_IS_VALID(_u_, _m_)) ?                                \
            soc_mem_read(_u_, _m_, MEM_BLOCK_ANY, _idx_, (void *)_buf_) :  \
            BCM_E_UNAVAIL)

/* Write memory macro. */
#define BCM_XGS3_MEM_WRITE(_u_, _m_, _idx_, _buf_)                         \
	        ((SOC_MEM_IS_VALID(_u_, _m_)) ?                                \
            soc_mem_write(_u_, _m_, MEM_BLOCK_ALL, _idx_, (void *)_buf_) : \
            BCM_E_UNAVAIL)

/* Clear memory macro. */
#define BCM_XGS3_MEM_CLEAR(_u_, _m_)                                       \
	        ((SOC_MEM_IS_VALID(_u_, _m_)) ?                                \
             soc_mem_clear(_u_, _m_, MEM_BLOCK_ALL, TRUE) : BCM_E_NONE)


/* Caluculate kbit per second rate in  64kbit quantum equivalent. */
#define BCM_XGS3_PORT_RATE_64KBIT_QUANTUM(_kbit_, _rate_)      \
            {                                                  \
               if (_kbit_ > (0xFFFFFFFF - 63)) {               \
                   _kbit_ = (0xFFFFFFFF - 63);                 \
               }                                               \
               if ((_rate_ = (_kbit_ + 63) / 64) > 0x3FFFF) {  \
                    _rate_ = 0x3FFFF;                          \
               }                                               \
            }

/* Set bitmap in memory entry. */ 
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
#define _BCM_XGS3_BITMAP_SET(_unit_, _mem_, _fld_, _buf_, _pbmp_)              \
      do {                                                                     \
          if (soc_feature((_unit_), soc_feature_table_hi)) {                   \
              soc_mem_field32_set((_unit_), (_mem_), (_buf_), (_fld_##_HIf),   \
                                  SOC_PBMP_WORD_GET((_pbmp_), 1));             \
              soc_mem_field32_set((_unit_), (_mem_), (_buf_), (_fld_##_LOf),   \
                                  SOC_PBMP_WORD_GET((_pbmp_), 0));             \
          } else {                                                             \
              soc_mem_field32_set((_unit_), (_mem_), (_buf_), (_fld_##f),      \
                                  SOC_PBMP_WORD_GET((_pbmp_), 0));             \
          }                                                                    \
      } while (0)

/* Get bitmap from memory entry. */ 
#define _BCM_XGS3_BITMAP_GET(_unit_, _mem_, _fld_, _buf_, _pbmp_)              \
      do {                                                                     \
          if (soc_feature((_unit_), soc_feature_table_hi)) {                   \
             SOC_PBMP_WORD_SET((_pbmp_), 0,                                    \
               soc_mem_field32_get((_unit_), (_mem_), (_buf_), (_fld_##_LOf)));\
             SOC_PBMP_WORD_SET((_pbmp_), 1,                                    \
               soc_mem_field32_get((_unit_), (_mem_), (_buf_), (_fld_##_HIf)));\
          } else {                                                             \
             SOC_PBMP_WORD_SET((_pbmp_), 0,                                    \
               soc_mem_field32_get((_unit_), (_mem_), (_buf_), (_fld_##f)));   \
          }                                                                    \
      } while (0)
#else  /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
#define _BCM_XGS3_BITMAP_SET(_unit_, _mem_, _fld_, _buf_, _pbmp_)              \
            soc_mem_field32_set((_unit_), (_mem_), (_buf_), (_fld_##f),        \
                                 SOC_PBMP_WORD_GET((_pbmp_), 0))               \

/* Get bitmap from memory entry. */ 
#define _BCM_XGS3_BITMAP_GET(_unit_, _mem_, _fld_, _buf_, _pbmp_)              \
            SOC_PBMP_WORD_SET((_pbmp_), 0,                                     \
                soc_mem_field32_get((_unit_), (_mem_), (_buf_), (_fld_##f)))   
#endif /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
/* 
 *  FUNCTIONS  PROTOTYPES.
 */

/* 6 bit field, msb indicates trunk */
#define BCM_TGID_TRUNK_LO_BITS(unit)       (SOC_TRUNK_BIT_POS(unit))
#define BCM_TGID_TRUNK_INDICATOR(unit)     (1 << SOC_TRUNK_BIT_POS(unit))
#define BCM_TGID_PORT_TRUNK_MASK(unit)     ((1 << SOC_TRUNK_BIT_POS(unit)) - 1)
#define BCM_TGID_PORT_TRUNK_MASK_HI(unit)  (0x3 << SOC_TRUNK_BIT_POS(unit))


/*
 * Routines for internal use
 */

extern int _bcm_fb_l2_from_l2u(int unit, 
                        bcm_l2_addr_t *l2addr, l2u_entry_t *l2u_entry);
extern int _bcm_fb_l2_to_l2u(int unit, 
                      l2u_entry_t *l2u_entry, bcm_l2_addr_t *l2addr);
extern int _bcm_fb_l2_from_l2x(int unit,
                     bcm_l2_addr_t *l2addr, l2x_entry_t *l2x_entry);
extern int _bcm_fb_l2_to_l2x(int unit,
                   l2x_entry_t *l2x_entry, bcm_l2_addr_t *l2addr);

/****************************************************************
 *
 * Firebolt functions
 *
 ****************************************************************/

extern int bcm_fb_l2_init(int unit);
extern int bcm_fb_l2_term(int unit);
extern int bcm_fb_l2_addr_get(int unit, bcm_mac_t mac_addr, bcm_vlan_t vid,
                       bcm_l2_addr_t *l2addr);
extern int bcm_fb_l2_addr_add(int unit, bcm_l2_addr_t *l2addr);
extern int bcm_fb_l2_addr_delete(int, bcm_mac_t, bcm_vlan_t);
extern int bcm_fb_l2_addr_delete_by_port(int, bcm_module_t, bcm_port_t,
                                  uint32);
extern int bcm_fb_l2_addr_delete_by_mac(int, bcm_mac_t, uint32);
extern int bcm_fb_l2_addr_delete_by_vlan(int, bcm_vlan_t, uint32);
extern int bcm_fb_l2_addr_delete_by_trunk(int, bcm_trunk_t, uint32);
extern int bcm_fb_l2_addr_delete_by_mac_port(int, bcm_mac_t,
                                      bcm_module_t, bcm_port_t,
                                      uint32);
extern int bcm_fb_l2_addr_delete_by_vlan_port(int, bcm_vlan_t,
                                       bcm_module_t, bcm_port_t,
                                       uint32);
extern int _bcm_fb_l2_addr_replace_by_mac(int unit, uint32 flags, _bcm_l2_replace_t *rep_st);
extern int _bcm_fb_l2_addr_replace_by_vlan_dest(int unit, uint32 flags, _bcm_l2_replace_t *rep_st);
extern int bcm_fb_l2_addr_delete_mcast(int unit, uint32 flags);
extern int bcm_fb_l2_addr_register(int, bcm_l2_addr_callback_t, void*);
extern int bcm_fb_l2_addr_unregister(int, bcm_l2_addr_callback_t, void*);
extern int bcm_fb_l2_conflict_get(int, bcm_l2_addr_t *, bcm_l2_addr_t *,
                           int, int *);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_fb_l2_reload_mbi(int unit);
#else
#define _bcm_fb_l2_reload_mbi(_u)    (BCM_E_UNAVAIL)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_fb_l2_sw_dump(int unit);
extern void _bcm_xgs3_mcast_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern int bcm_xgs3_port_cfg_init(int, bcm_port_t, bcm_vlan_data_t *);
extern int bcm_xgs3_port_cfg_get(int, bcm_port_t, bcm_port_cfg_t *);
extern int bcm_xgs3_port_cfg_set(int, bcm_port_t, bcm_port_cfg_t *);

extern int bcm_fb_er_vlan_init(int unit, bcm_vlan_data_t *vd);
extern int bcm_fb_er_vlan_create(int unit, bcm_vlan_t vid);
extern int bcm_fb_er_vlan_destroy(int unit, bcm_vlan_t vid);
extern int bcm_fb_er_vlan_port_add(int unit, bcm_vlan_t vid,
                            pbmp_t pbmp, pbmp_t ubmp);
extern int bcm_fb_er_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp);
extern int bcm_fb_er_vlan_port_get(int unit, bcm_vlan_t vid,
                            pbmp_t *pbmp, pbmp_t *ubmp);
extern int bcm_fb_er_vlan_stg_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg_ptr);
extern int bcm_fb_er_vlan_stg_set(int unit, bcm_vlan_t vid, bcm_stg_t stg);
extern int bcm_fb_er_vlan_reload(int unit, vlist_t **list, int *count);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_fb2_vlan_profile_reload(int unit);
extern int _bcm_fb2_vlan_profile_init(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_fb2_vlan_profile_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern int bcm_xgs3_port_rate_ingress_set(int unit, bcm_port_t port,
                                 uint32 kbits_sec, 
                                 uint32 kbits_burst);
extern int bcm_xgs3_port_rate_ingress_get(int unit, bcm_port_t port,
                                 uint32 *kbits_sec, 
                                 uint32 *kbits_burst);
extern int bcm_fb_port_rate_pause_get(int unit, bcm_port_t port,
                               uint32 *kbits_pause, 
                               uint32 *kbits_resume);

extern int bcm_xgs3_port_sample_rate_set(int unit, bcm_port_t port, 
                                int ingress_rate, int egress_rate);
extern int bcm_xgs3_port_sample_rate_get(int unit, bcm_port_t port, 
                                int *ingress_rate, int *egress_rate);

extern int bcm_xgs3_trunk_set(int, bcm_trunk_t,
                        bcm_trunk_add_info_t *, trunk_private_t *);
extern int bcm_xgs3_trunk_get(int, bcm_trunk_t,
                        bcm_trunk_add_info_t *, trunk_private_t *);
extern int bcm_xgs3_trunk_destroy(int, bcm_trunk_t, trunk_private_t *);
extern int bcm_xgs3_trunk_mcast_join(int, bcm_trunk_t, bcm_vlan_t, 
                               bcm_mac_t, trunk_private_t *);
extern int _bcm_xgs3_trunk_get_port_property(int unit, 
                                             bcm_module_t mod,
                                             bcm_port_t port, 
                                             bcm_trunk_t *tid);

extern int  _bcm_xgs3_trunk_swfailover_init(int unit);
extern void _bcm_xgs3_trunk_swfailover_detach(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _xgs3_trunk_reinit(int unit, int ngroups_fp, trunk_private_t *);
extern int _xgs3_trunk_fabric_reinit(int unit, int min_tid, int max_tid,
                                     trunk_private_t *);
#else
#define    _xgs3_trunk_reinit(unit, ng_fp, tp)            (BCM_E_UNAVAIL)
#define    _xgs3_trunk_fabric_reinit(unit, min, max, tp)  (BCM_E_UNAVAIL)
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int _bcm_xgs3_l2mc_free_index(int unit, int *l2mc_id);
extern int _bcm_xgs3_l2mc_id_alloc(int unit, int l2mc_id);
extern int _bcm_xgs3_l2mc_id_free(int unit, int l2mc_id);
extern void _bcm_xgs3_l2mc_pbmp_set(int unit, l2mc_entry_t *l2mc, bcm_pbmp_t pbmp);
extern void _bcm_xgs3_l2mc_pbmp_get(int unit, l2mc_entry_t *l2mc, bcm_pbmp_t *pbmp);
extern int bcm_xgs3_mcast_addr_add(int, bcm_mcast_addr_t *);
extern int bcm_xgs3_mcast_addr_remove(int, bcm_mac_t, bcm_vlan_t);
extern int bcm_xgs3_mcast_port_get(int, bcm_mac_t, bcm_vlan_t,
                             bcm_mcast_addr_t *);
extern int _bcm_xgs3_mcast_index_port_get(int unit, int l2mc_index, 
                               bcm_mcast_addr_t *mcaddr);
extern int bcm_xgs3_mcast_init(int);
extern int bcm_xgs3_mcast_addr_add_w_l2mcindex(int unit,
                                         bcm_mcast_addr_t *mcaddr);
extern int bcm_xgs3_mcast_addr_remove_w_l2mcindex(int unit,
                                            bcm_mcast_addr_t *mcaddr);
extern int bcm_xgs3_mcast_port_add(int unit, bcm_mcast_addr_t *mcaddr);
extern int bcm_xgs3_mcast_port_remove(int unit, bcm_mcast_addr_t *mcaddr);

extern int bcm_xgs3_stg_stp_init(int unit, bcm_stg_t stg);
extern int bcm_xgs3_stg_stp_set(int unit, bcm_stg_t stg, 
                         bcm_port_t port, int stp_state);
extern int bcm_xgs3_stg_stp_get(int unit, bcm_stg_t stg, 
                         bcm_port_t port, int *stp_state);

extern int bcm_xgs3_vlan_init(int unit, bcm_vlan_data_t *vd);
extern int bcm_xgs3_vlan_create(int unit, bcm_vlan_t vid);
extern int bcm_xgs3_vlan_destroy(int unit, bcm_vlan_t vid);
extern int bcm_xgs3_vlan_port_add(int unit, bcm_vlan_t vid, 
			    pbmp_t pbmp, pbmp_t ubmp);
extern int bcm_xgs3_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp);
extern int bcm_xgs3_vlan_port_get(int unit, bcm_vlan_t vid, 
			    pbmp_t *pbmp, pbmp_t *ubmp);
extern int bcm_xgs3_vlan_stg_get(int unit, bcm_vlan_t vid, 
			   bcm_stg_t *stg_ptr);
extern int bcm_xgs3_vlan_stg_set(int unit, bcm_vlan_t vid, bcm_stg_t stg);
extern int bcm_xgs3_vlan_reload(int unit, vlist_t **list, int *count);
extern int _bcm_xgs3_vlan_mcast_flood_set(int unit,
                            bcm_vlan_t vlan,
                            bcm_vlan_mcast_flood_t mode);
extern int _bcm_xgs3_vlan_mcast_flood_get(int unit,
                            bcm_vlan_t vlan,
                            bcm_vlan_mcast_flood_t *mode);
extern int _bcm_xgs3_vlan_control_vlan_set(int unit, bcm_vlan_t vid,
                            bcm_vlan_control_vlan_t control);
extern int _bcm_xgs3_vlan_control_vlan_get(int unit, bcm_vlan_t vid,
                            bcm_vlan_control_vlan_t *control);
extern int _bcm_fb_vlan_translate_add(int unit, int port, 
                                      bcm_vlan_t old_vid,
                                      bcm_vlan_t new_vid, 
                                      int prio, int xtable);
extern int _bcm_fb_vlan_translate_delete(int unit, int port, 
                                         bcm_vlan_t old_vid,int xtable);
extern int _bcm_fb_vlan_translate_get(int unit, int port, bcm_vlan_t old_vid,
                                      bcm_vlan_t *new_vid, int *prio, int xtable);
extern int _bcm_fb_vlan_translate_traverse_int_cb(int unit, void *trv_info);
extern int _bcm_fb_vlan_translate_parse(int unit, soc_mem_t mem, uint32* vent,
                             _bcm_vlan_translate_traverse_t *trvs_info);
extern int bcm_fb_cosq_init(int unit);
extern int bcm_fb_cosq_detach(int unit);
extern int bcm_fb_cosq_config_set(int unit, int numq);
extern int bcm_fb_cosq_config_get(int unit, int *numq);
extern int bcm_fb_er_cosq_mapping_set(int unit,
				      bcm_port_t port,
				      bcm_cos_t priority,
				      bcm_cos_queue_t cosq);
extern int bcm_fb_er_cosq_mapping_get(int unit,
				      bcm_port_t port,
				      bcm_cos_t priority,
				      bcm_cos_queue_t *cosq);
extern int bcm_fb_cosq_port_sched_set(int unit, bcm_pbmp_t, int mode,
                               const int weights[], int delay);
extern int bcm_fb_cosq_port_sched_get(int unit, bcm_pbmp_t, int *mode,
                               int weights[], int *delay);
extern int bcm_fb_cosq_sched_weight_max_get(int unit, int mode,
                                     int *weight_max);
extern int bcm_fb_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                   bcm_cos_queue_t cosq,
                                   uint32 kbits_sec_min,
                                   uint32 kbits_sec_max,
                                   uint32 flags);
extern int bcm_fb_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                   bcm_cos_queue_t cosq,
                                   uint32 *kbits_sec_min,
                                   uint32 *kbits_sec_max,
                                   uint32 *flags);
extern int bcm_fb_cosq_discard_set(int unit, uint32 flags);
extern int bcm_fb_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_fb_cosq_discard_port_set(int unit, bcm_port_t port,
                                 bcm_cos_queue_t cosq,
                                 uint32 color,
                                 int drop_start,
                                 int drop_slope,
                                 int average_time);
extern int bcm_fb_cosq_discard_port_get(int unit, bcm_port_t port,
                                 bcm_cos_queue_t cosq,
                                 uint32 color,
                                 int *drop_start,
                                 int *drop_slope,
                                 int *average_time);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_xgs3_mcast_cleanup(int unit);
#else
#define _bcm_xgs3_mcast_cleanup(_u)  (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_fb_l2_sw_dump(int unit);
extern void _bcm_xgs3_mcast_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef INCLUDE_L3
extern int bcm_xgs3_l3_fbx_defip_init(int unit);
extern int bcm_xgs3_l3_fbx_defip_deinit(int unit);
extern int bcm_xgs3_l3_egress_mode_set(int unit, int mode);
extern int bcm_xgs3_l3_egress_mode_get(int unit, int *mode);
extern int bcm_xgs3_l3_tables_init(int unit);
extern int bcm_xgs3_l3_tables_cleanup(int);
extern int bcm_xgs3_l3_enable(int, int);
extern int bcm_xgs3_l3_ip4_enable(int, int);
extern int bcm_xgs3_l3_ip6_enable(int, int);
extern int bcm_xgs3_l3_intf_get(int, _bcm_l3_intf_cfg_t *);
extern int bcm_xgs3_l3_intf_get_by_vid(int, _bcm_l3_intf_cfg_t *);
extern int bcm_xgs3_l3_intf_create(int, _bcm_l3_intf_cfg_t *);
extern int bcm_xgs3_l3_intf_id_create(int, _bcm_l3_intf_cfg_t *);
extern int bcm_xgs3_l3_intf_lookup(int, _bcm_l3_intf_cfg_t *);
extern int bcm_xgs3_l3_intf_del(int, _bcm_l3_intf_cfg_t *);
extern int bcm_xgs3_l3_intf_del_all(int);

extern int bcm_xgs3_l3_egress_create(int unit, uint32 flags, 
                                     bcm_l3_egress_t *egr, bcm_if_t *intf);
extern int bcm_xgs3_l3_egress_destroy(int unit, bcm_if_t intf);
extern int bcm_xgs3_l3_egress_get (int unit, bcm_if_t intf, 
                                   bcm_l3_egress_t *egr);
extern int bcm_xgs3_l3_egress_find(int unit, bcm_l3_egress_t *egr, 
                                    bcm_if_t *intf);
extern int bcm_xgs3_l3_egress_traverse(int unit, 
                                       bcm_l3_egress_traverse_cb trav_fn,
                                       void *user_data);
extern int bcm_xgs3_l3_egress_multipath_create(int unit, uint32 flags,
                                               int intf_count, 
                                               bcm_if_t *intf_array, 
                                               bcm_if_t *mpintf);
extern int bcm_xgs3_l3_egress_multipath_destroy(int unit, bcm_if_t mpintf);
extern int bcm_xgs3_l3_egress_multipath_get(int unit, bcm_if_t mpintf,
                                            int intf_size,
                                            bcm_if_t *intf_array, 
                                            int *intf_count);
extern int bcm_xgs3_l3_egress_multipath_add(int unit, bcm_if_t mpintf, 
                                            bcm_if_t intf);
extern int bcm_xgs3_l3_egress_multipath_delete(int unit, bcm_if_t mpintf,
                                               bcm_if_t intf);
extern int bcm_xgs3_l3_egress_multipath_find(int unit, int intf_count,
                                             bcm_if_t *intf_array, 
                                             bcm_if_t *mpintf);
extern int bcm_xgs3_l3_egress_multipath_traverse(int unit,
                             bcm_l3_egress_multipath_traverse_cb trav_fn,
                             void *user_data);
extern int bcm_xgs3_l3_get(int, _bcm_l3_cfg_t *);
extern int bcm_xgs3_l3_add(int, _bcm_l3_cfg_t *);
extern int bcm_xgs3_l3_del(int, _bcm_l3_cfg_t *);
extern int bcm_xgs3_l3_del_prefix(int, _bcm_l3_cfg_t *);
extern int bcm_xgs3_l3_del_intf(int, _bcm_l3_cfg_t *, int);
extern int bcm_xgs3_l3_del_all(int);
extern int bcm_xgs3_l3_replace(int, _bcm_l3_cfg_t *);
extern int bcm_xgs3_l3_age(int unit, uint32 flags, 
                           bcm_l3_host_traverse_cb age_out, void *user_data);
extern int bcm_xgs3_l3_ip4_traverse(int unit, uint32 start, uint32 end,
                                    bcm_l3_host_traverse_cb cb, 
                                    void *user_data);
extern int bcm_xgs3_l3_ip6_traverse(int unit, uint32 start, uint32 end,
                                     bcm_l3_host_traverse_cb cb, 
                                     void *user_data);

extern int bcm_xgs3_l3_conflict_get(int unit, bcm_l3_key_t *ipkey,
               bcm_l3_key_t *cf_array, int cf_max, int *cf_count);

extern int bcm_xgs3_defip_get(int, _bcm_defip_cfg_t *);
extern int bcm_xgs3_defip_ecmp_get_all(int, _bcm_defip_cfg_t *,
                                  bcm_l3_route_t *, int, int *);
extern int bcm_xgs3_defip_add(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_xgs3_defip_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_xgs3_defip_del_intf(int, _bcm_defip_cfg_t *, int);
extern int bcm_xgs3_defip_del_all(int);
extern int bcm_xgs3_defip_age(int unit, bcm_l3_route_traverse_cb age_out,
                              void *user_data);
extern int bcm_xgs3_defip_traverse(int unit, int flags, 
                                   uint32 start, uint32 end, 
                                   bcm_l3_route_traverse_cb trav_fn, 
                                   void *user_data);

extern int bcm_xgs3_defip_ip4_traverse(int unit, uint32 start, uint32 end, 
                                       bcm_l3_route_traverse_cb trav_fn, 
                                       void *user_data);
extern int bcm_xgs3_defip_ip6_traverse(int unit, uint32 start, uint32 end, 
                                       bcm_l3_route_traverse_cb trav_fn, 
                                       void *user_data);
extern int bcm_xgs3_l3_ip6_prefix_map_get(int unit, int map_size, 
                                          bcm_ip6_t *ip6_array, int *ip6_count);
extern int bcm_xgs3_l3_ip6_prefix_map_add(int unit, bcm_ip6_t ip6_addr);
extern int bcm_xgs3_l3_ip6_prefix_map_delete(int unit, bcm_ip6_t ip6_addr);
extern int bcm_xgs3_l3_ip6_prefix_map_delete_all(int unit);

extern int bcm_xgs3_l3_info(int unit, bcm_l3_info_t *l3_info);
extern int bcm_xgs3_l3_get_by_index(int unit, int index, _bcm_l3_cfg_t *l3cfg);
extern int bcm_xgs3_l3_del_by_index(int unit, int index, int ip6_flag);
extern int bcm_xgs3_max_ecmp_get(int unit, int *max_paths);
extern int bcm_xgs3_max_ecmp_set(int unit, int max_paths);

extern int bcm_xgs3_tunnel_terminator_add(int unit, 
                                          bcm_tunnel_terminator_t *info);
extern int bcm_xgs3_tunnel_terminator_delete(int unit, 
                                             bcm_tunnel_terminator_t *info);
extern int bcm_xgs3_tunnel_terminator_update(int unit,
                                             bcm_tunnel_terminator_t *info);
extern int bcm_xgs3_tunnel_terminator_get(int unit,
                                          bcm_tunnel_terminator_t *info);
extern int bcm_xgs3_tunnel_initiator_set(int unit, bcm_l3_intf_t *intf,
                                         bcm_tunnel_initiator_t *tunnel);
extern int bcm_xgs3_tunnel_initiator_clear(int unit, bcm_l3_intf_t *intf);
extern int bcm_xgs3_tunnel_initiator_get(int unit, bcm_l3_intf_t *intf,
                                         bcm_tunnel_initiator_t *tunnel);
extern int bcm_xgs3_tunnel_dscp_map_create(int unit, uint32 flags, int *dscp_map_id);
extern int bcm_xgs3_tunnel_dscp_map_destroy(int unit, int dscp_map_id);
extern int bcm_xgs3_tunnel_dscp_map_set(int unit, int dscp_map_id,
                                        bcm_tunnel_dscp_map_t *dscp_map);
extern int bcm_xgs3_tunnel_dscp_map_get(int unit, int dscp_map_id,
                                        bcm_tunnel_dscp_map_t *dscp_map);
extern int bcm_xgs3_tunnel_config_set(int unit, bcm_tunnel_config_t *tconfig);
extern int bcm_xgs3_tunnel_config_get(int unit, bcm_tunnel_config_t *tconfig);

extern int bcm_fb_er_ipmc_init(int unit);
extern int bcm_fb_er_ipmc_detach(int unit);
extern int bcm_fb_er_ipmc_get(int unit, int index, bcm_ipmc_addr_t *ipmc);
extern int bcm_fb_er_ipmc_lookup(int unit, int *index, bcm_ipmc_addr_t *ipmc);
extern int bcm_fb_er_ipmc_add(int unit, bcm_ipmc_addr_t *ipmc);
extern int bcm_fb_er_ipmc_put(int unit, int index, bcm_ipmc_addr_t *ipmc);
extern int bcm_fb_er_ipmc_delete(int unit, bcm_ipmc_addr_t *ipmc);
extern int bcm_fb_er_ipmc_delete_all(int unit);
extern int bcm_fb_er_ipmc_enable(int unit, int enable);
extern int bcm_fb_er_ipmc_src_port_check(int unit, int enable);
extern int bcm_fb_er_ipmc_src_ip_search(int unit, int enable);
extern int bcm_fb_er_ipmc_egress_port_set(int unit, bcm_port_t port,
                                   const bcm_mac_t mac,  int untag,
                                   bcm_vlan_t vid, int ttl_threshold);
extern int bcm_fb_er_ipmc_egress_port_get(int unit, bcm_port_t port,
                                   mac_addr_t mac, int *untag,
                                   bcm_vlan_t *vid,
                                   int *ttl_threshold);
extern int bcm_xgs3_ipmc_traverse(int unit, uint32 flags, bcm_ipmc_traverse_cb cb, 
                                  void *user_data);
extern int bcm_xgs3_ipmc_age(int unit, uint32 flags, bcm_ipmc_traverse_cb age_cb,
                             void *user_data);
extern int bcm_xgs3_ipmc_id_alloc(int unit, int ipmc_id);
extern int bcm_xgs3_ipmc_id_free(int unit, int ipmc_id);
extern int bcm_xgs3_ipmc_create(int unit, int *ipmc_id);

extern int bcm_fb_ipmc_repl_init(int unit);
extern int bcm_fb_ipmc_repl_detach(int unit);
extern int bcm_fb_ipmc_repl_get(int unit, int index, bcm_port_t port,
                         bcm_vlan_vector_t vlan_vec);
extern int bcm_fb_ipmc_repl_add(int unit, int index, bcm_port_t port,
                         bcm_vlan_t vlan);
extern int bcm_fb_ipmc_repl_delete(int unit, int index, bcm_port_t port,
                            bcm_vlan_t vlan);
#ifdef BCM_TRX_SUPPORT
extern int _bcm_tr_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port, 
                                        int nh_index, int is_l3);
extern int _bcm_tr_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                                           int if_max, int nh_index, int is_l3);
extern int _bcm_tr_l3_ingress_interface_get(int unit, 
                                            _bcm_l3_ingress_intf_t *iif);
extern int _bcm_tr_l3_ingress_interface_set(int unit, 
                                            _bcm_l3_ingress_intf_t *iif);
#endif
extern int bcm_fb_ipmc_repl_delete_all(int unit, int index,
                                bcm_port_t port);
extern int bcm_fb_ipmc_repl_set(int unit, int mc_index, bcm_port_t port,
                                bcm_vlan_vector_t vlan_vec);
extern int bcm_fb_ipmc_egress_intf_add(int unit, int index, bcm_port_t port,
                                bcm_l3_intf_t *l3_intf);
extern int bcm_fb_ipmc_egress_intf_delete(int unit, int index, bcm_port_t port,
                                   bcm_l3_intf_t *l3_intf);
extern int bcm_fb_ipmc_egress_intf_set(int unit, int mc_index,
                                       bcm_port_t port, int if_count,
                                       bcm_if_t *if_array, int is_l3);
extern int bcm_fb_ipmc_egress_intf_get(int unit, int mc_index,
                                       bcm_port_t port,
                                       int if_max, bcm_if_t *if_array,
                                       int *if_count);
extern int bcm_xgs3_nh_get(int unit, int nh_idx, bcm_l3_egress_t *nh_info);
extern int bcm_xgs3_nh_add(int unit, uint32 flags, 
                           bcm_l3_egress_t *nh_info, int *index);
extern int bcm_xgs3_nh_del(int unit, uint32 flags, int nh_index);
extern int bcm_xgs3_ecmp_group_del(int unit, int ecmp_index);
extern int bcm_xgs3_get_nh_from_egress_object(int unit, bcm_if_t intf,
                                              uint32 flags, int *index);
extern int bcm_xgs3_tnl_init_add(int unit, uint32 flags, 
                                 bcm_tunnel_initiator_t *tnl_entry, int *index);
extern int bcm_xgs3_tnl_init_del(int unit, uint32 flags, int ent_index);

/* The following are not implemented for Firebolt */
extern int bcm_xgs3_lpm_defip_sw_tbl_empty(int unit, int *count);
extern int bcm_xgs3_lpm_defip_hw_tbl_empty(int unit, int *count);
extern int bcm_xgs3_lpm_check_table(int unit);
extern int bcm_xgs3_lpm_delete(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_xgs3_lpm_insert(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_xgs3_lpm_lookup(int unit, ip_addr_t iproute,
                               _bcm_defip_cfg_t* lpm_cfg);
extern int bcm_xgs3_adj_mac_add(int unit, bcm_mac_t dmac, int *index);
extern int bcm_xgs3_adj_mac_del(int unit, int ent_index);
extern int bcm_xgs3_l3_egress_id_parse(int unit, bcm_if_t intf, 
                                       uint32 *flags, int *nh_ecmp_id);
extern int bcm_xgs3_l3_tbl_dma(int unit, soc_mem_t tbl_mem, uint16
                               tbl_entry_sz, const char *descr, char **res_ptr,
                               int *entry_count);
extern int bcm_xgs3_internal_lpm_vrf_calc(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                                           int *vrf_id, int *vrf_mask);
extern int bcm_xgs3_l3_mask6_apply(bcm_ip6_t mask, bcm_ip6_t addr);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_xgs3_ipmc_reinit(int unit);
extern int _bcm_xgs3_l3_reinit(int unit);
#else
#define _bcm_xgs3_ipmc_reinit(_u)    (BCM_E_UNAVAIL)
#define _bcm_xgs3_l3_reinit(_u)    (BCM_E_UNAVAIL)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_xgs3_ipmc_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif /* INCLUDE_L3 */

extern int bcm_fb_egress_packet_ifg_set(int unit, uint32 value);
extern int bcm_fb_port_rate_egress_set(int unit, bcm_port_t port,
                                uint32 kbits_sec, 
                                uint32 kbits_burst);
extern int bcm_fb_port_rate_egress_get(int unit, bcm_port_t port,
                                uint32 *kbits_sec, 
                                uint32 *kbits_burst);

/* Utility routines for tracking outer tpid entries in Firebolt2 */
extern int _bcm_fb2_outer_tpid_init(int unit); 
extern int _bcm_fb2_priority_map_init(int unit); 
extern void _bcm_fb2_outer_tpid_entry_delete(int unit, int index);
extern void _bcm_fb2_outer_tpid_entry_get(int unit, uint16 *tpid, int index);
extern uint16 _bcm_fb2_outer_tpid_default_get(int unit);
extern int _bcm_fb2_outer_tpid_entry_add(int unit, uint16 tpid, int *index);
extern int _bcm_fb2_outer_tpid_lkup(int unit, uint16, int *index);
extern int _bcm_fb2_outer_tpid_tab_lock(int unit);
extern int _bcm_fb2_outer_tpid_tab_unlock(int unit);

/* Utility routines for metering and egress rate control */
extern uint8 _bcm_fb_kbits_to_bucketsize(uint32 kbits_burst);
extern uint32 _bcm_fb_bucketsize_to_kbits(uint8 reg_val);

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
/* Utility routines for metering and egress rate control */
extern uint32 _bcm_fb2_kbits_to_bucket_encoding(uint32 kbits_burst,
                                                int linear_mode);
extern uint32 _bcm_fb2_bucket_encoding_to_kbits(uint32 reg_val,
                                                int linear_mode);
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRIUMPH_SUPPORT */

extern int bcm_fb_vlan_init(int unit, bcm_vlan_data_t *vd);
extern int bcm_fb_vlan_port_get(int unit, bcm_vlan_t vid,
                         pbmp_t *pbmp, pbmp_t *ubmp);
extern int bcm_fb_vlan_stg_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg_ptr);

#ifdef BCM_FIELD_SUPPORT
#include <bcm_int/esw/field.h>
extern int _bcm_field_fb_init(int unit, _field_control_t *control);

/* These functions are in src/bcm/esw/firebolt/field.c, 
 * and used by firebolt, raptor, triumph */
extern int _bcm_field_fb_table_fpf_destroy(_qual_info_t **table, int size);
extern int _field_fb_mode_set(int unit, uint8 slice_numb,
                                  _field_group_t *fg, uint8 flags);

extern int _bcm_field_fb_udf_spec_set(int unit, bcm_field_udf_spec_t *udf_spec,
                                      uint32 flags, uint32 offset);
extern int _bcm_field_fb_udf_spec_get(int unit, bcm_field_udf_spec_t *udf_spec, 
                                      uint32 *flags, uint32 *offset);

extern int _bcm_field_fb_udf_write(int unit, bcm_field_udf_spec_t *udf_spec, 
                                   uint32 udf_num, uint32 user_num);
extern int _bcm_field_fb_udf_read(int unit, bcm_field_udf_spec_t *udf_spec, 
                                  uint32 udf_num, uint32 user_num);

extern int _bcm_field_fb_selcode_get(int unit, _field_stage_t *stage_fc, 
                                     bcm_field_qset_t *qset_req,
                                     _field_group_t *fg);
extern int _bcm_field_fb_selcode_to_qset(int unit, 
                                         _field_stage_t *stage_fc, 
                                         _field_group_t *fg,
                                         int code_id,
                                         bcm_field_qset_t *qset);
extern int _bcm_field_fb_qual_lists_get(int unit, _field_stage_t *stage_fc,
                                        _field_group_t *fg);
extern int _bcm_field_fb_policer_install(int unit, _field_entry_t *f_ent, 
                                         _field_policer_t *f_pl);
extern int _bcm_field_fb_group_remove(int unit, _field_group_t *fg);
extern int _bcm_field_fb_group_install(int unit, _field_group_t *fg);
extern int _bcm_field_fb_selcodes_install(int unit, _field_group_t *fg, 
                                          uint8 slice_numb, bcm_pbmp_t pbmp,
                                          int selcode_index);
extern int _bcm_field_fb_write_slice_map(int unit, _field_stage_t *stage_fc, 
                                  _field_virtual_map_t new_fp_virtual_map[]);
extern int _bcm_field_fb_mtp_release(int unit, _field_entry_t *f_ent, 
                                     int tcam_idx);
extern int _bcm_field_fb_entry_remove(int unit, _field_entry_t *f_ent, 
                                      int tcam_idx);
extern int _bcm_field_fb_entry_move(int unit, _field_entry_t **entry_arr,
                                    int amount);
extern void _bcm_field_fb_counter_adjust_wide_mode(int unit, 
                                   soc_mem_t policy_mem,
                                   _field_entry_t *f_ent_pri, 
                                   _field_entry_t *f_ent_sec, 
                                   uint32 p_entry_pri[],
                                   uint32 p_entry_sec[]);
extern void _bcm_field_fb_meter_adjust_wide_mode(int unit, 
                                                 soc_mem_t policy_mem,
                                                 _field_policer_t *f_pl,
                                                 _field_entry_t *f_ent_pri, 
                                                 _field_entry_t *f_ent_sec, 
                                                 uint32 p_entry_pri[],
                                                 uint32 p_entry_sec[]);
extern int _field_fb_tcam_policy_mem_get(int unit, _field_stage_id_t stage_id,
                                         soc_mem_t *tcam_mem,
                                         soc_mem_t *policy_mem);
extern int _bcm_field_entry_qual_get(int unit, bcm_field_entry_t entry,
                                     bcm_field_qualify_t qual,
                                     _field_entry_t **entry_p);

extern int _bcm_field_fb_meter_idx_get(int unit, _field_entry_t *f_ent,
                                        soc_mem_t mem, uint32 *buf);
extern int _bcm_field_fb_meter_control_set(int unit, uint32 value);
#if defined(INCLUDE_L3)
int
_bcm_field_policy_set_l3_info(int unit, soc_mem_t mem, 
                             int value, uint32 *buf);
#endif /* INCLUDE_L3 */
#endif /* BCM_FIELD_SUPPORT */

#endif  /* !_BCM_INT_FIREBOLT_H_ */
