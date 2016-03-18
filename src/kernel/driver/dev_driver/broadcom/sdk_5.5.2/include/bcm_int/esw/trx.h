/*
 * $Id: trx.h,v 1.16.6.2.2.1 Broadcom SDK $
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
 * File:        trx.h
 * Purpose:     Function declarations for Triumph Micro Architecture 
 *              based devices.
 */

#ifndef _BCM_INT_TRX_H_
#define _BCM_INT_TRX_H_

#if defined(BCM_TRX_SUPPORT)
#if defined(INCLUDE_L3)

#define BCM_TRX_DEFIP128_HASH_SZ            (0x6)
#define BCM_TRX_DEFIP128(_unit_)             l3_trx_defip_128[(_unit_)]
#define BCM_TRX_DEFIP128_ARR(_unit_)         BCM_TRX_DEFIP128((_unit_))->entry_array
#define BCM_TRX_DEFIP128_IDX_MAX(_unit_)     BCM_TRX_DEFIP128((_unit_))->idx_max
#define BCM_TRX_DEFIP128_USED_COUNT(_unit_)  BCM_TRX_DEFIP128((_unit_))->used_count
#define BCM_TRX_DEFIP128_TOTAL(_unit_)       BCM_TRX_DEFIP128((_unit_))->total_count
#define BCM_TRX_DEFIP128_URPF_OFFSET(_unit_) BCM_TRX_DEFIP128((_unit_))->urpf_offset
#define BCM_TRX_DEFIP128_ENTRY_SET(_unit_, _idx_, _plen_, _hash_)          \
            BCM_TRX_DEFIP128_ARR((_unit_))[(_idx_)].prefix_len = (_plen_); \
            BCM_TRX_DEFIP128_ARR((_unit_))[(_idx_)].entry_hash = (_hash_)


typedef struct _bcm_l3_defip_128_entry_s {
    uint8  prefix_len; /* Route entry  prefix length.*/
    uint8  entry_hash; /* Route entry key hash.      */
} _bcm_l3_defip_128_entry_t;

typedef struct _bcm_l3_defip_128_table_s {
    _bcm_l3_defip_128_entry_t *entry_array; /* Cam entries array.          */
    uint16 idx_max;                   /* Last cam entry index.             */
    uint16 used_count;                /* Used cam entry count.             */
    uint16 total_count;               /* Total number of available entries.*/
    uint16 urpf_offset;               /* Src lookup start offset.          */
} _bcm_l3_defip_128_table_t;

extern int _bcm_trx_defip_128_init(int unit);
extern int _bcm_trx_defip_128_deinit(int unit);
extern int _bcm_trx_defip_128_get(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                  int *nh_ecmp_idx); 
extern int _bcm_trx_defip_128_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                  int nh_ecmp_idx);
extern int _bcm_trx_defip_128_delete(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_trx_defip_128_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
extern int _bcm_trx_l3_defip_mem_get(int unit, uint32 flags, 
                                     int plen, soc_mem_t *mem);
extern int _bcm_trx_tnl_type_to_hw_code(int unit, bcm_tunnel_type_t tnl_type, 
                                        int *hw_code, int *entry_type);
extern int _bcm_trx_tnl_hw_code_to_type(int unit, int hw_tnl_type, 
                                         int entry_type, bcm_tunnel_type_t
                                         *tunnel_type);
extern int _bcm_trx_tunnel_dscp_map_port_set(int unit, bcm_port_t port,
                                             bcm_tunnel_dscp_map_t *dscp_map);
extern int _bcm_trx_tunnel_dscp_map_port_get(int unit, bcm_port_t port,
                                             bcm_tunnel_dscp_map_t *dscp_map);
extern int _bcm_trx_multicast_ipmc_read(int unit, int ipmc_id, 
                                        bcm_pbmp_t *l2_pbmp,
                                        bcm_pbmp_t *l3_pbmp);
#endif /* INCLUDE_L3 */

#if defined(BCM_FIELD_SUPPORT)

extern int _bcm_field_trx_tcp_ttl_tos_init(int unit);
extern int _bcm_field_trx_write_slice_map_vfp(int unit, _field_stage_t *stage_fc, 
                                        _field_virtual_map_t new_fp_virtual_map[]);
extern int _bcm_field_trx_write_slice_map_egress(int unit, _field_stage_t *stage_fc, 
                                        _field_virtual_map_t new_fp_virtual_map[]);
extern int _bcm_field_trx_mirror_ingress_add(int unit, soc_mem_t mem, 
                                             _field_entry_t *f_ent,
                                             _field_action_t *fa, uint32 *buf);
extern int _bcm_field_trx_mirror_egress_add(int unit, soc_mem_t mem, 
                                            _field_entry_t *f_ent,
                                            _field_action_t *fa, uint32 *buf);
extern int _bcm_field_trx_mirror_ingress_delete(int unit, soc_mem_t mem, 
                                                _field_entry_t *f_ent, 
                                                bcm_gport_t *mirror_dest, 
                                                uint32 *buf);
extern int _bcm_field_trx_mirror_egress_delete(int unit, soc_mem_t mem, 
                                               _field_entry_t *f_ent, 
                                               bcm_gport_t *mirror_dest, 
                                               uint32 *buf);
extern int _bcm_field_trx_mtp_release(int unit, _field_entry_t *f_ent,
                                      int tcam_idx);
extern int _bcm_field_trx_action_support_check(int unit, _field_entry_t *f_ent,
                                                bcm_field_action_t action, 
                                                int *result);
extern int _bcm_field_trx_qualify_ip_type(int unit, _field_entry_t *f_ent, 
                                          bcm_field_IpType_t type);
extern int _bcm_field_trx_egress_key_match_type_set(int unit, 
                                                    _field_entry_t *f_ent);
extern int _bcm_field_trx_action_delete(int unit, _field_entry_t *f_ent,
                                        _field_action_t *fa, int tcam_idx, 
                                        uint32 param0, uint32 param1);
extern int _bcm_field_trx_tcam_get(int unit, soc_mem_t mem, 
                                   _field_entry_t *f_ent, uint32 *buf);
extern int _bcm_field_trx_action_counter_update(int unit, soc_mem_t mem, 
                                                _field_entry_t *f_ent, 
                                                _field_action_t *fa,
                                                uint32 *buf);
extern int _bcm_field_trx_action_get(int unit, soc_mem_t mem,
                                     _field_entry_t *f_ent, 
                                      int tcam_idx, _field_action_t *fa, 
                                      uint32 *buf);
extern int _bcm_field_trx_range_check_set(int unit, int range, uint32 flags, 
                                          int enable, bcm_l4_port_t min, 
                                          bcm_l4_port_t max);
extern int _bcm_field_trx_udf_spec_set(int unit, bcm_field_udf_spec_t *udf_spec, 
                                       uint32 flags, uint32 offset);
extern int _bcm_field_trx_udf_spec_get(int unit, bcm_field_udf_spec_t *udf_spec, 
                                       uint32 *flags, uint32 *offset);
extern int _bcm_field_trx_udf_write(int unit, bcm_field_udf_spec_t *udf_spec, 
                                    uint32 udf_num, uint32 user_num);
extern int _bcm_field_trx_udf_read(int unit, bcm_field_udf_spec_t *udf_spec, 
                                   uint32 udf_num, uint32 user_num);
extern int _bcm_field_trx_mtp_release(int unit, _field_entry_t *f_ent, int
                                      tcam_idx);
extern int _bcm_field_trx_redirect_profile_delete(int unit, int index);
extern int _bcm_field_trx_redirect_profile_alloc(int unit, _field_entry_t *f_ent, 
                                          _field_action_t *fa);
extern int _bcm_field_trx_redirect_profile_ref_count_get(int unit, int index,
                                                         int *ref_count);

#endif /* BCM_FIELD_SUPPORT */

#endif /* BCM_TRX_SUPPORT */
#endif  /* !_BCM_INT_TRX_H_ */
