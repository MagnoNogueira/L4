/*
 * $Id: memtune.h,v 1.15.2.10 Broadcom SDK $
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
 * File:        memtune.h
 * Purpose:     External memory interface tuning routines.
 */

#ifndef   _SOC_MEMTUNE_H_
#define   _SOC_MEMTUNE_H_

#include <soc/register.h>

/* Top-level routine */
typedef enum soc_mem_interfaces_e {
    SOC_MEM_INTERFACE_RLDRAM_CH0,
    SOC_MEM_INTERFACE_RLDRAM_CH1,
    SOC_MEM_INTERFACE_SRAM,
    SOC_MEM_INTERFACE_QDR,
    SOC_MEM_INTERFACE_TCAM,
    SOC_MEM_INTERFACE_COUNT
} soc_mem_interfaces;

typedef struct soc_memtune_data_s {
    struct soc_memtune_ctrl_s *mt_ctrl;
    int                    interface;
    int                    sub_interface;
    int                    rseed;
    int                    config;
    int                    verbose;
    int                    suppress_fail;
    int                    summary;
    int                    summary_header;
    int                    test_num;
    int                    full_test;
    uint32                 slice_mask;
    int                    freq;
    int                    phase_sel_ovrd;
    int                    manual_settings; /* =1 if manual settings */
    int                    man_phase_sel;   /* user specified value */
    int                    man_em_latency;  /* user specified value */
    int                    man_ddr_latency; /* user specified value */
    int                    man_tx_offset;   /* user specified value */
    int                    man_rx_offset;   /* user specified value */
} soc_memtune_data_t;

/* Saved results for a specific overridden phase_sel value */
typedef struct soc_memtune_phase_sel_s {
    int          area;
    int          width;
    int          height;
    int          shape;
    int          em_latency;
    int          ddr_latency;
    int          tx_offset;
    int          rx_offset;
    int          fail_count;
} soc_memtune_phase_sel_t;

#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_TEST_NAME      0x0001
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY        0x0002
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY_HEADER 0x0004
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX    0x0008
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS       0x0010
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_SELECTION      0x0020
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL           0x0040
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS           0x0080
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_NOPS           0x0100

typedef struct soc_memtune_ctrl_s {
    void                    *data;
    int                     unit;
    char                    *intf_name;
    uint32                  flags;
    int                     (*prog_hw1_fn)(soc_memtune_data_t *);
    int                     (*prog_hw2_fn)(soc_memtune_data_t *);
    int                     (*test_fn)(soc_memtune_data_t *);

    /* for tx / rx phase offset tuning */
    int                     phase_sel_cur;
    int                     phase_sel_min;
    int                     phase_sel_max;
    int                     em_latency_cur;
    int                     em_latency_min;
    int                     em_latency_max;
    int                     ddr_latency_cur;
    int                     ddr_latency_min;
    int                     ddr_latency_max;
    int                     tx_offset_cur;
    int                     tx_offset_min;
    int                     tx_offset_max;
    int                     rx_offset_cur;
    int                     rx_offset_min;
    int                     rx_offset_max;
    int                     w2r_nops_cur;
    int                     w2r_nops_min;
    int                     w2r_nops_max;
    int                     r2w_nops_cur;
    int                     r2w_nops_min;
    int                     r2w_nops_max;

    /* for TCAM DBUS_out (DPEO) tuning */
    int                     dpeo_sel_cur;
    int                     dpeo_sync_dly_cur;
    int                     dpeo_sync_dly_min;
    int                     dpeo_sync_dly_max;
    int                     fcd_dpeo_cur;
    int                     fcd_dpeo_min;
    int                     fcd_dpeo_max;

    /* for TCAM RBUS tuning */
    int                     rbus_sync_dly_cur;
    int                     rbus_sync_dly_min;
    int                     rbus_sync_dly_max;
    int                     fcd_rbus_cur;
    int                     fcd_rbus_min;
    int                     fcd_rbus_max;

    int                     bist_poll_count;
    int                     cur_fail_count;
    int                     *fail_array;
    soc_memtune_phase_sel_t ps_data[4];
} soc_memtune_ctrl_t;

extern int soc_mem_interface_tune(int unit, soc_memtune_data_t *mt_data);

/* Specific implementations */

typedef struct soc_er_memtune_data_s {
    soc_reg_t    ddr_reg1;
    soc_reg_t    ddr_reg2;
    soc_reg_t    ddr_reg3;
    soc_reg_t    timing_reg;
    soc_reg_t    req_cmd;
    soc_reg_t    bist_ctrl;
    soc_mem_t    write_mem;
    int          channel;
} soc_er_memtune_data_t;

typedef struct soc_tr_memtune_data_s {
    soc_reg_t    ddr_reg1;
    soc_reg_t    ddr_reg2;
    soc_reg_t    ddr_reg3;
    soc_reg_t    tmode0;
    soc_reg_t    etc_ctl;
    soc_reg_t    inst_status;
} soc_tr_memtune_data_t;

#define SOC_ER_PHASE_SEL_MIN            0
#define SOC_ER_PHASE_SEL_MAX            3
#define SOC_ER_EM_LATENCY_MIN           7
#define SOC_ER_EM_LATENCY_MAX           8
#define SOC_ER_SEER_DDR_LATENCY_MIN     0
#define SOC_ER_RLDRAM_DDR_LATENCY_MIN   1
#define SOC_ER_DDR_LATENCY_MAX          2
#define SOC_ER_RLDRAM_TX_OFFSET_MIN     0
#define SOC_ER_RLDRAM_TX_OFFSET_MAX     7
#define SOC_ER_RLDRAM_RX_OFFSET_MIN     0
#define SOC_ER_RLDRAM_RX_OFFSET_MAX     7
#define SOC_ER_DDR_BIST_COUNT           10
#define SOC_ER_RLDRAM_BIST_POLL_COUNT   1000
#define SOC_ER_RLDRAM_BIST_POLL_INTERVAL 100
#define SOC_ER_RLDRAM_SLICE_MASK        0xf
#define SOC_ER_SRAM_SLICE_MASK          0xf
#define SOC_ER_QDR_SLICE_MASK           0x3

/* Triumph: SRAM */
#define SOC_TR_PHASE_SEL_MIN            0
#define SOC_TR_PHASE_SEL_MAX            3
#define SOC_TR_EM_LATENCY_MIN           0
#define SOC_TR_EM_LATENCY_MAX           7
#define SOC_TR_DDR_LATENCY_MIN          1
#define SOC_TR_DDR_LATENCY_MAX          1
#define SOC_TR_SRAM_TX_OFFSET_MIN       0
#define SOC_TR_SRAM_TX_OFFSET_MAX       24
#define SOC_TR_SRAM_RX_OFFSET_MIN       0
#define SOC_TR_SRAM_RX_OFFSET_MAX       24
#define SOC_TR_SRAM_W2R_NOPS_MIN        0
#define SOC_TR_SRAM_W2R_NOPS_MAX        3
#define SOC_TR_SRAM_R2W_NOPS_MIN        0
#define SOC_TR_SRAM_R2W_NOPS_MAX        3
#define SOC_TR_SRAM_SLICE_MASK          0x3

/* Triumph: TCAM */
#define SOC_TR_TCAM_TX_OFFSET_MIN       0
#define SOC_TR_TCAM_TX_OFFSET_MAX       26
#define SOC_TR_TCAM_RX_OFFSET_MIN       0
#define SOC_TR_TCAM_RX_OFFSET_MAX       26

/* Triumph: TCAM DBUS_out (DPEO) */
#define SOC_TR_DPEO_SYNC_DLY_MIN        0
#define SOC_TR_DPEO_SYNC_DLY_MAX        31
#define SOC_TR_FCD_DPEO_MIN             0
#define SOC_TR_FCD_DPEO_MAX             1

/* Triumph: TCAM RBUS */
#define SOC_TR_RBUS_SYNC_DLY_MIN        1  /* 1 means 32 */
#define SOC_TR_RBUS_SYNC_DLY_MAX        31
#define SOC_TR_FCD_RBUS_MIN             0
#define SOC_TR_FCD_RBUS_MAX             1  /* only use 0 and 1 */


/* Triumph: config variable encoding for both SRAM and TCAM tuning result */
#define SOC_TR_MEMTUNE_CONFIG_VALID_MASK      0x1

/* Triumph: config variable encoding for SRAM tuning result */
#define SOC_TR_MEMTUNE_OVRD_SM_MASK           0x1
#define SOC_TR_MEMTUNE_OVRD_SM_SHIFT          1
#define SOC_TR_MEMTUNE_PHASE_SEL_MASK         0x3
#define SOC_TR_MEMTUNE_PHASE_SEL_SHIFT        2
#define SOC_TR_MEMTUNE_EM_LATENCY_MASK        0x7
#define SOC_TR_MEMTUNE_EM_LATENCY_SHIFT       4
#define SOC_TR_MEMTUNE_DDR_LATENCY_MASK       0x3
#define SOC_TR_MEMTUNE_DDR_LATENCY_SHIFT      7
#define SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK     0x1f
#define SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT    9
#define SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK     0x1f
#define SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT    14
#define SOC_TR_MEMTUNE_DDR_W2R_NOPS_MASK      0x3
#define SOC_TR_MEMTUNE_DDR_W2R_NOPS_SHIFT     19
#define SOC_TR_MEMTUNE_DDR_R2W_NOPS_MASK      0x3
#define SOC_TR_MEMTUNE_DDR_R2W_NOPS_SHIFT     21
#define SOC_TR_MEMTUNE_DDR_FREQ_MASK          0x1ff
#define SOC_TR_MEMTUNE_DDR_FREQ_SHIFT         23

/* Triumph: config variable encoding for TCAM tuning result */
#define SOC_TR_MEMTUNE_RBUS_SYNC_DLY_MASK     0x1f
#define SOC_TR_MEMTUNE_RBUS_SYNC_DLY_SHIFT    8
#define SOC_TR_MEMTUNE_DPEO_SYNC_DLY_MASK     0x1f
#define SOC_TR_MEMTUNE_DPEO_SYNC_DLY_SHIFT    13
#define SOC_TR_MEMTUNE_FCD_DPEO_MASK          0x1
#define SOC_TR_MEMTUNE_FCD_DPEO_SHIFT         18
#define SOC_TR_MEMTUNE_FCD_RBUS_MASK          0x3
#define SOC_TR_MEMTUNE_FCD_RBUS_SHIFT         22
#define SOC_TR_MEMTUNE_TCAM_TX_OFFSET_MASK    0x3f
#define SOC_TR_MEMTUNE_TCAM_TX_OFFSET_SHIFT   1
#define SOC_TR_MEMTUNE_TCAM_RX_OFFSET_MASK    0x3f
#define SOC_TR_MEMTUNE_TCAM_RX_OFFSET_SHIFT   7
#define SOC_TR_MEMTUNE_TCAM_FREQ_MASK         0x3ff
#define SOC_TR_MEMTUNE_TCAM_FREQ_SHIFT        13

#endif /* _SOC_MEMTUNE_H_ */
