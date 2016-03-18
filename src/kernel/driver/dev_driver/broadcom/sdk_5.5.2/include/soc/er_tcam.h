/*
 * $Id: er_tcam.h,v 1.32.2.4 Broadcom SDK $
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
 * File:      er_tcam_h
 */

#ifndef _SOC_ER_TCAM_H
#define _SOC_ER_TCAM_H

#define TCAM_SIZE_18MB          0x00
#define TCAM_REQ_RSP_LATENCY    0x00
#define TCAM_BLOCK_ENTRIES  0x800

typedef enum soc_tcam_partition_type_e {
    TCAM_PARTITION_RAW,
    TCAM_PARTITION_ACL,          /* Easyrider */
    TCAM_PARTITION_LPM,          /* Easyrider */
    TCAM_PARTITION_FWD_L2,       /* Triumph */
    TCAM_PARTITION_FWD_IP4,      /* Triumph */
    TCAM_PARTITION_FWD_IP6U,     /* Triumph */
    TCAM_PARTITION_FWD_IP6,      /* Triumph */
    TCAM_PARTITION_ACL_L2,       /* Triumph */
    TCAM_PARTITION_ACL_IP4,      /* Triumph */
    TCAM_PARTITION_ACL_IP6S,     /* Triumph */
    TCAM_PARTITION_ACL_IP6F,     /* Triumph */
    TCAM_PARTITION_ACL_L2C,      /* Triumph */
    TCAM_PARTITION_ACL_IP4C,     /* Triumph */
    TCAM_PARTITION_ACL_IP6C,     /* Triumph */
    TCAM_PARTITION_ACL_L2IP4,    /* Triumph */
    TCAM_PARTITION_ACL_L2IP6,    /* Triumph */
    TCAM_PARTITION_COUNT
} soc_tcam_partition_type_t;

#define TCAM_PARTITION_FLAGS_TYPE_L2          0x01
#define TCAM_PARTITION_FLAGS_TYPE_L3          0x02
#define TCAM_PARTITION_FLAGS_TYPE_ACL         0x03
#define TCAM_PARTITION_FLAGS_TYPE_MASK        0x03
#define TCAM_PARTITION_FLAGS_AD_IN_SRAM0      0x08 /* forward or policy tbl */
#define TCAM_PARTITION_FLAGS_AD_IN_SRAM1      0x10 /* forward or policy tbl */
#define TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM0 0x20
#define TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM1 0x40

typedef struct soc_tcam_partition_s {
    int num_entries;
    int num_entries_include_pad;
    int tcam_width_shift;
    int tcam_base;
    int sram_width_shift;
    int sram_base;
    int counter_base;
    int hbit_base;
    uint32 flags;
} soc_tcam_partition_t;

typedef struct soc_tcam_info_s {
    int subtype;
    int num_tcams;
    int mode;
    int tcam_freq;
    int sram_freq;
    soc_tcam_partition_t partitions[TCAM_PARTITION_COUNT];
} soc_tcam_info_t;

typedef struct er_tcam_config_s {
    int tcams;
    int acl_entries;
    int lpm_entries;
} er_tcam_config_t;

extern soc_tcam_info_t soc_tcam_info;

#ifdef BCM_EASYRIDER_SUPPORT

extern er_tcam_config_t configs[];

extern int er_tcam_init_type1(int u, int ext_table_cfg);
extern int er_tcam_init_type2(int u, int ext_table_cfg);
extern int soc_er_tcam_type1_read_entry(int u, int cfg, int part, int index,
        uint32 * mask, uint32 * data, int * valid);
extern int soc_er_tcam_type2_read_entry(int u, int cfg, int part, int index,
        uint32 * mask, uint32 * data, int * valid);
extern int soc_er_tcam_type1_write_entry(int u, int cfg, int part, int index,
        uint32 * mask, uint32 * data);
extern int soc_er_tcam_type2_write_entry(int u, int cfg, int part, int index,
        uint32 * mask, uint32 * data);
extern int soc_er_tcam_type1_set_valid(int u, int cfg, int part, int index,
        int valid);
extern int soc_er_tcam_type2_set_valid(int u, int cfg, int part, int index,
        int valid);
extern int soc_er_tcam_type1_search_entry(int u, int cfg, int part, int upper,
                                          uint32 *data, int *index);
extern int soc_er_tcam_type1_write_ib(int unit, uint32 addr, uint32 data);
extern int soc_er_tcam_type1_write_ima(int unit, uint32 addr,
                                      uint32 d0_msb, uint32 d1_lsb);
extern int soc_er_tcam_type1_read_ima(int unit, uint32 addr,
                                      uint32 *d0_msb, uint32 *d1_lsb);
extern int soc_er_tcam_type1_write_dbreg(int unit, uint32 addr,
                                         uint32 d0_msb, uint32 d1_lsb);
extern int soc_er_tcam_type1_read_dbreg(int unit, uint32 addr,
                                        uint32 *d0_msb, uint32 *d1_lsb);
extern int soc_er_tcam_type2_parity_diagnose(int unit);
extern int soc_er_tcam_type1_memtest(int unit, uint32 pattern1,
                                     uint32 pattern2);

#endif

#ifdef BCM_TRIUMPH_SUPPORT

#define TCAM_TR_WORDS_PER_ENTRY     3

enum {
  TCAM_TR_OP_WRITE = 0,
  TCAM_TR_OP_SINGLE_SEARCH1 = 1,
  TCAM_TR_OP_SINGLE_SEARCH0 = 2,
  TCAM_TR_OP_PARALLEL_SEARCH = 3,
  TCAM_TR_OP_READ = 4,
  TCAM_TR_OP_READ_MASK = 5
};

extern int tr_tcam_init_type1(int unit);
extern int soc_tr_tcam_type1_write_entry(int unit, int part, int index,
                                         uint32 *mask, uint32 *data);
extern int soc_tr_tcam_type1_read_entry(int unit, int part, int index,
                                        uint32 *mask, uint32 *data,
                                        int *valid);
extern int soc_tr_tcam_type1_delete_entry(int unit, int part, int index);
extern int soc_tr_tcam_type1_search_entry(int unit, int part1, int part2,
                                          uint32 *data, int *index1,
                                          int *index2);
extern int soc_tr_tcam_type1_write_reg(int unit, uint32 addr, uint32 d0_msb,
                                       uint32 d1, uint32 d2_lsb);
extern int soc_tr_tcam_type1_read_reg(int unit, uint32 addr, uint32 *d0_msb,
                                      uint32 *d1, uint32 *d2_lsb);
extern int soc_tr_tcam_type1_memtest_dpeo(int unit);


#endif /* BCM_TRIUMPH_SUPPORT */

extern int soc_tcam_init(int unit);
extern int soc_tcam_get_info(int unit, int *type, int *subtype, int *dc_val,
                             soc_tcam_info_t **tcam_info);
extern int soc_tcam_get_part_size(int unit, soc_tcam_partition_type_t part,
                                  int *num_entries,
                                  int *num_entries_include_pad);
extern int soc_tcam_part_index_to_mem_index(int unit,
                                            soc_tcam_partition_type_t part,
                                            int part_index,
                                            soc_mem_t mem, int *mem_index);
extern int soc_tcam_mem_index_to_raw_index(int unit, soc_mem_t mem,
                                           int mem_index, soc_mem_t *real_mem,
                                           int *raw_index);
extern int soc_tcam_read_entry(int unit, int part, int index, uint32 *mask,
                               uint32 *data, int *valid);
extern int soc_tcam_write_entry(int unit, int part, int index, uint32 *mask,
                                uint32 *data);
extern int soc_tcam_set_valid(int unit, int part, int index, int valid);
extern int soc_tcam_search_entry(int unit, int part1, int part2, uint32 *data,
                                 int *index1, int *index2);
extern int soc_tcam_write_ib(int unit, uint32 addr, uint32 data);
extern int soc_tcam_write_ima(int unit, uint32 addr, uint32 d0_msb,
                              uint32 d1_lsb);
extern int soc_tcam_read_ima(int unit, uint32 addr, uint32 *d0_msb,
                             uint32 *d1_lsb);
extern int soc_tcam_write_dbreg(int unit, uint32 addr, uint32 d0_msb,
                                uint32 d1, uint32 d2_lsb);
extern int soc_tcam_read_dbreg(int unit, uint32 addr, uint32 *d0_msb,
                               uint32 *d1, uint32 *d2_lsb);
extern int soc_sram_init(int unit);

#endif /* !_SOC_ER_TCAM_H */
