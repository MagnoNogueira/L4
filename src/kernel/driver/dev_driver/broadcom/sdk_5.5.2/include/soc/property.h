/*
 * $Id: property.h,v 1.128.2.31.2.3 Broadcom SDK $
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
 * SOC Property Names
 *
 * NOTE: Remember to make a $SDK/rc/config.bcm entry for each entry here.
 */

#ifndef _SOC_PROPERTY_H
#define _SOC_PROPERTY_H

#define spn_STATION_MAC_ADDRESS         "station_mac_address"
#define spn_STATION_IP_ADDRESS          "station_ip_address"
#define spn_STATION_IP_NETMASK          "station_ip_netmask"
#define spn_STATION_HOSTNAME            "station_hostname"
#define spn_OS                          "os"
#define spn_DEBUG_FLAGS                 "debug_flags"
#define spn_POLLED_IRQ_MODE             "polled_irq_mode"
#define spn_POLLED_IRQ_DELAY            "polled_irq_delay"
#define spn_POLLED_IRQ_PRIORITY         "polled_irq_priority"
#define spn_FILTER_ENABLE               "filter_enable"
#define spn_BCM_NUM_COS                 "bcm_num_cos"
#define spn_BCM_FILTER_CLEAR_FE         "bcm_filter_clear_fe"
#define spn_BCM_FILTER_CLEAR_GE         "bcm_filter_clear_ge"
#define spn_BCM_FILTER_CLEAR_XE         "bcm_filter_clear_xe"
#define spn_BCM_LINKSCAN_PBMP           "bcm_linkscan_pbmp"
#define spn_BCM_LINKSCAN_INTERVAL       "bcm_linkscan_interval"
#define spn_BCM_LINKSCAN_MAXERR         "bcm_linkscan_maxerr"
#define spn_BCM_LINKSCAN_ERRDELAY       "bcm_linkscan_errdelay"
#define spn_BCM_STAT_PBMP               "bcm_stat_pbmp"
#define spn_BCM_STAT_INTERVAL           "bcm_stat_interval"
#define spn_BCM_STAT_SYNC_TIMEOUT       "bcm_stat_sync_timeout"
#define spn_BCM_STAT_FLAGS              "bcm_stat_flags"
#define spn_BCM_STAT_JUMBO              "bcm_stat_jumbo"
#define spn_BCM_TX_THREAD_PRI           "bcm_tx_thread_pri"
#define spn_BCM_RX_THREAD_PRI           "bcm_rx_thread_pri"
#define spn_LINKSCAN_THREAD_PRI         "linkscan_thread_pri"
#define spn_ARL_LOOKUP_TRIES            "arl_lookup_tries"
#define spn_ARL_STATIC_FF               "arl_static_ff"
#define spn_ARL_LOOKUP_TRIES            "arl_lookup_tries"
#define spn_PDMA_TIMEOUT_USEC           "pdma_timeout_usec"
#define spn_CDMA_TIMEOUT_USEC           "cdma_timeout_usec"
#define spn_CDMA_PIO_HOLD_ENABLE        "cdma_pio_hold_enable"
#define spn_TDMA_TIMEOUT_USEC           "tdma_timeout_usec"
#define spn_TDMA_INTR_ENABLE            "tdma_intr_enable"
#define spn_TSLAM_TIMEOUT_USEC          "tslam_timeout_usec"
#define spn_TSLAM_INTR_ENABLE           "tslam_intr_enable"
#define spn_SOC_CTR_MAXERR              "soc_ctr_maxerr"
#define spn_SOC_SKIP_RESET              "soc_skip_reset"
#define spn_COUNTER_THREAD_PRI          "counter_thread_pri"
#define spn_LCCDRAIN_TIMEOUT_USEC       "lccdrain_timeout_usec"
#define spn_SOC_DMA_ABORT_TIMEOUT       "soc_dma_abort_timeout"
#define spn_SOC_SCOREBOARD_ENABLE       "soc_scoreboard_enable"
#define spn_SOC_SCOREBOARD_INTERVAL     "soc_scoreboard_interval"
#define spn_L3_ENABLE                   "l3_enable"
#define spn_IPMC_ENABLE                 "ipmc_enable"
#define spn_IPMC_DO_VLAN                "ipmc_do_vlan"
#define spn_TRUNK_EXTEND                "trunk_extend"
#define spn_MMU_HBM_GE                  "mmu_hbm_ge"
#define spn_MMU_HBM_CPU                 "mmu_hbm_cpu"
#define spn_MMU_IBP_GE                  "mmu_ibp_ge"
#define spn_MMU_IBP_CPU                 "mmu_ibp_cpu"
#define spn_MMU_HBM_FE                  "mmu_hbm_fe"
#define spn_MMU_IBP_FE                  "mmu_ibp_fe"
#define spn_GBPFULL_WARN_ENABLE         "gbpfull_warn_enable"
#define spn_GBPFULL_WARN_TIME           "gbpfull_warn_time"
#define spn_ARLDMA_SIZE                 "arldma_size"
#define spn_ARL_RESYNC_DELAY            "arl_resync_delay"
#define spn_ARLMSG_THREAD_PRI           "arlmsg_thread_pri"
#define spn_L2XMSG_AVL                  "l2xmsg_avl"
#define spn_L2XMSG_MODE                 "l2xmsg_mode"
#define spn_L2XMSG_THREAD_PRI           "l2xmsg_thread_pri"
#define spn_L2XMSG_THREAD_USEC          "l2xmsg_thread_usec"
#define spn_L2XMSG_SHADOW_HIT_BITS      "l2xmsg_shadow_hit_bits"
#define spn_L2XMSG_MAXERR               "l2xmsg_maxerr"
#define spn_L2XMSG_CHUNKS               "l2xmsg_chunks"
#define spn_L2XMSG_HOSTBUF_SIZE         "l2xmsg_hostbuf_size"
#define spn_ARL_CLEAN_TIMEOUT_USEC      "arl_clean_timeout_usec"
#define spn_MEM_SCAN_THREAD_PRI         "mem_scan_thread_pri"
#define spn_MEM_SCAN_MAXERR             "mem_scan_maxerr"
#define spn_MEM_SCAN_CHUNK_SIZE         "mem_scan_chunk_size"
#define spn_SCHAN_TIMEOUT_USEC          "schan_timeout_usec"
#define spn_MIIM_TIMEOUT_USEC           "miim_timeout_usec"
#define spn_BIST_TIMEOUT_MSEC           "bist_timeout_msec"
#define spn_SCHAN_INTR_ENABLE           "schan_intr_enable"
#define spn_SCHAN_ERROR_BLOCK_USEC      "schan_error_block_usec"
#define spn_MIIM_INTR_ENABLE            "miim_intr_enable"
#define spn_ARL_RATE_LIMIT              "arl_rate_limit"
#define spn_MMU_SDRAM_ENABLE            "mmu_sdram_enable"
#define spn_MMU_SDRAM_DLL0              "mmu_sdram_dll0"
#define spn_MMU_SDRAM_DLL1              "mmu_sdram_dll1"
#define spn_MMU_SDRAM_DLL2              "mmu_sdram_dll2"
#define spn_MMU_SDRAM_CLOCK_EXTERNAL    "mmu_sdram_clock_external"
#define spn_MMU_SDRAM_WIDTH             "mmu_sdram_width"
#define spn_DIAG_LB_PACKET_TIMEOUT      "diag_lb_packet_timeout"
#define spn_DIAG_LB_LINK_TIMEOUT        "diag_lb_link_timeout"
#define spn_DIAG_LB_FILL_RX             "diag_lb_fill_rx"
#define spn_DIAG_TRAFFIC_LINK_TIMEOUT   "diag_traffic_link_timeout"
#define spn_DIAG_PW_THREAD_PRI          "diag_pw_thread_pri"
#define spn_DIAG_PW_BUFFER_SIZE         "diag_pw_buffer_size"
#define spn_CFAP_TESTS                  "cfap_tests"
#define spn_PHY_ENABLE                  "phy_enable"
#define spn_PHY_NULL                    "phy_null"
#define spn_PHY_SIMUL                   "phy_simul"
#define spn_PHY_AUTOMEDIUM              "phy_automedium"
#define spn_PHY_FIBER_PREF              "phy_fiber_pref"
#define spn_PHY_FIBER_DEGLITCH_USECS    "phy_fiber_deglitch_usecs"
#define spn_PHY_AUTONEG_TIMEOUT         "phy_autoneg_timeout"
#define spn_PHY_SERDES                  "phy_serdes"
#define spn_PHY_SERDES_AUTOS            "phy_serdes_autos"
#define spn_PHY_5464S                   "phy_5464S"
#define spn_PHY_5673                    "phy_5673"
#define spn_PHY_5690                    "phy_5690"
#define spn_PHY_8706                    "phy_8706"
#define spn_PHY_8072			"phy_8072"
#define spn_PHY_AN_C73                  "phy_an_c73"
#define spn_PHY_EXT_ROM_BOOT		"phy_ext_rom_boot"
#define spn_PHY_HALF_PWR_MODE           "phy_half_power"
#define spn_PHY_LOW_POWER_MODE          "phy_low_power_mode"
#define spn_PHY_OCTAL_PORT_FIRST        "phy_octal_port_first"
#define spn_PHY_HL65_1LANE_MODE         "phy_hl65_1lane_mode"
#define spn_PHY_53115_AN100TX_WAR       "phy_53115_an100tx_war"
#define spn_PHY_53115_B0_IOP_WAR        "phy_53115_b0_iop_war"
#define spn_TURBO                       "turbo"
#define spn_TURBO_DRV                   "turbo_drv"
#define spn_TURBO_PHASE                 "turbo_phase"
#define spn_TURBO_IOV                   "turbo_iov"
#define spn_GIG_IOV                     "gig_iov"
#define spn_IF_TBI                      "if_tbi"
#define spn_XGS_BCM5632_HDR_MODE        "bcm5632_mode"
#define spn_BCM_METER_CLEAR_FE          "bcm_meter_clear_fe"
#define spn_BCM_METER_CLEAR_GE          "bcm_meter_clear_ge"
#define spn_BCM_METER_CLEAR_XE          "bcm_meter_clear_xe"
#define spn_XGXS_PREEMPHASIS            "xgxs_preemphasis"
#define spn_XGXS_DRIVER_CURRENT         "xgxs_driver_current"
#define spn_XGXS_PRE_DRIVER_CURRENT     "xgxs_pre_driver_current"
#define spn_XGXS_PLLLOCK                "xgxs_plllock"
#define spn_XGXS_EQUALIZER              "xgxs_equalizer"
#define spn_XGXS_OFFSET                 "xgxs_offset"
#define spn_XGXS_LCPLL_XTAL_REFCLK      "xgxs_lcpll_xtal_refclk"
#define spn_XGXS_LCPLL                  "xgxs_lcpll"
#define spn_XGXS_LCPLL_12GBPS           "xgxs_lcpll_12gbps"
#define spn_XGXS_LCPLL_13GBPS           "xgxs_lcpll_13gbps"
#define spn_XGXS_PDETECT_10G            "xgxs_pdetect_10g"
#define spn_XGXS_TX_LANE_MAP            "xgxs_tx_lane_map"
#define spn_XGXS_RX_LANE_MAP            "xgxs_rx_lane_map"
#define spn_SERDES_LCPLL                "serdes_lcpll"
#define spn_SERDES_PREEMPHASIS          "serdes_preemphasis"
#define spn_SERDES_PRE_DRIVER_CURRENT   "serdes_pre_driver_current"
#define spn_SERDES_DRIVER_CURRENT       "serdes_driver_current"
#define spn_SERDES_AUTOMEDIUM           "serdes_automedium"
#define spn_SERDES_FIBER_PREF           "serdes_fiber_pref"
#define spn_PHY_PREEMPHASIS             "phy_preemphasis"
#define spn_PHY_DRIVER_CURRENT          "phy_driver_current"
#define spn_PHY_PRE_DRIVER_CURRENT      "phy_pre_driver_current"
#define spn_PBMP_VALID                  "pbmp_valid"
#define spn_LLA_TESTS                   "lla_tests"
#define spn_STACK_MAC_ADDRESS           "stacking_mac_address"
#define spn_STACK_OUI                   "stacking_oui"
#define spn_STACK_SYSTEM_BOARD          "stack_system_board"
#define spn_STACK_ENABLE                "stack_enable"
#define spn_STACK_MODE                  "stack_mode"
#define spn_STACK_SIMPLEX               "stack_simplex"
#define spn_STACK_CPU_PRIORITY          "stack_cpu_priority"
#define spn_STACK_UNIT                  "stack_unit"
#define spn_STACK_PORT_SIMPLEX          "stack_port_simplex"
#define spn_STACK_PORT_DUPLEX1          "stack_port_duplex1"
#define spn_STACK_PORT_DUPLEX2          "stack_port_duplex2"
#define spn_LOSSLESS_MODE               "lossless_mode"
#define spn_MIRROR_5670                 "mirror_5670_mode"
#define spn_CORE_CLOCK_12G              "core_clock_12G"
#define spn_MMU_HOL_JITTER              "mmu_hol_jitter"
#define spn_MMU_HOL_HYSTERESIS          "mmu_hol_hysteresis"
#define spn_PHY_RESET_TIMEOUT           "phy_reset_timeout"
#define spn_I2C_NVRAM_SKIP              "i2c_nvram_skip"
#define spn_I2C_HCLK_SKIP               "i2c_hclk_skip"
#define spn_I2C_POE_POWER               "i2c_poe_power"
#define spn_PHY_XAUI_TX_LANE_SWAP       "phy_xaui_tx_lane_swap"
#define spn_PHY_XAUI_RX_LANE_SWAP       "phy_xaui_rx_lane_swap"
#define spn_PHY_XAUI_TX_POLARITY_FLIP   "phy_xaui_tx_polarity_flip"
#define spn_PHY_XAUI_RX_POLARITY_FLIP   "phy_xaui_rx_polarity_flip"
#define spn_PHY_PCS_TX_POLARITY_FLIP    "phy_pcs_tx_polarity_flip"
#define spn_PHY_PCS_RX_POLARITY_FLIP    "phy_pcs_rx_polarity_flip"
#define spn_CX4_TO_HG                   "cx4_to_higig"
#define spn_10G_IS_CX4                  "10g_is_cx4"
#define spn_FORCE_OPTRXLOSLVL           "force_optrxloslvl"
#define spn_FORCE_OPTTXENBLVL           "force_opttxenblvl"
#define spn_FORCE_OPTTXRSTLVL           "force_opttxrstlvl"
#define spn_FORCE_OPTBIASFLTLVL         "force_optbiasfltlvl"
#define spn_FORCE_OPTTEMPFLTLVL         "force_opttempfltlvl"
#define spn_FORCE_OPTPRFLTLVL           "force_optprfltlvl"
#define spn_FORCE_OPTTXFLTLVL           "force_opttxfllvl"
#define spn_FORCE_OPTRXLOSLVL           "force_optrxloslvl"
#define spn_FORCE_OPTRXFLTLVL           "force_optrxfltlvl"
#define spn_FORCE_OPTTXONLVL            "force_opttxonlvl"
#define spn_BYPASS_MCU                  "bypass_mcu"
#define spn_MDIO_EXTERNAL_MASTER        "mdio_external_master"
#define spn_PHY_LED1_MODE               "phy_led1_mode"
#define spn_PHY_LED2_MODE               "phy_led2_mode"
#define spn_PHY_LED3_MODE               "phy_led3_mode"
#define spn_PHY_LED4_MODE               "phy_led4_mode"
#define spn_PHY_LED_CTRL                "phy_led_ctrl"
#define spn_PHY_LED_SELECT              "phy_led_select"
#define spn_PHY_FIBER_DETECT            "phy_fiber_detect"
#define spn_MCU_DRV_STR0                "mcu_drv_str0"
#define spn_MCU_DRV_STR1                "mcu_drv_str1"
#define spn_MCU_PAD_DATA_CLASS2         "mcu_pad_data_class2"
#define spn_MCU_PAD_DATA_DRIVE          "mcu_pad_data_drive"
#define spn_MCU_PAD_DATA_SLEW           "mcu_pad_data_slew"
#define spn_MCU_PAD_ADDR_CLASS2         "mcu_pad_addr_class2"
#define spn_MCU_PAD_ADDR_DRIVE          "mcu_pad_addr_drive"
#define spn_MCU_PAD_ADDR_SLEW           "mcu_pad_addr_slew"
#define spn_MCU_DELAY_DQI_ADJ_DIR       "mcu_delay_dqi_adj_dir"
#define spn_MCU_DELAY_DQI_ADJ_VAL       "mcu_delay_dqi_adj_val"
#define spn_MCU_DELAY_ADDR_ADJ_DIR      "mcu_delay_addr_adj_dir"
#define spn_MCU_DELAY_ADDR_ADJ_VAL      "mcu_delay_addr_adj_val"
#define spn_MCU_16BIT_DDR               "mcu_16bit_ddr"
#define spn_PORT_INIT_SPEED             "port_init_speed"
#define spn_PORT_INIT_DUPLEX            "port_init_duplex"
#define spn_PORT_INIT_ADV               "port_init_adv"
#define spn_PORT_INIT_AUTONEG           "port_init_autoneg"
#define spn_PORT_PHY_ADDR               "port_phy_addr"
#define spn_PORT_PHY_ID0                "port_phy_id0"
#define spn_PORT_PHY_ID1                "port_phy_id1"
#define spn_PORT_PHY_ADDR1              "port_phy_addr1"
#define spn_L2DELETE_CHUNKS             "l2delete_chunks"
#define spn_VLANDELETE_CHUNKS           "vlandelete_chunks"
#define spn_FILTER_RESIZE               "filter_resize"
#define spn_MMU_XQ_WEIGHT               "mmu_xq_weight"
#define spn_MMU_XQ_AGING                "mmu_xq_aging"
#define spn_EMULATION_REGS              "emulation_regs"
#define spn_PBMP_XPORT_XE               "pbmp_xport_xe"
#define spn_PBMP_XPORT_GE               "pbmp_xport_ge"
#define spn_PBMP_GPORT_STACK            "pbmp_gport_stack"
#define spn_MEMCMD_TIMEOUT_USEC         "memcmd_timeout_usec"
#define spn_MEMCMD_INTR_ENABLE          "memcmd_intr_enable"
#define spn_IPFIX_INTR_ENABLE           "ipfix_intr_enable"
#define spn_L2MOD_DMA_INTR_ENABLE       "l2mod_dma_intr_enable"
#define spn_SEER_EXT_TABLE_CFG          "seer_ext_table_cfg"
#define spn_SEER_EXT_TCAM_SELECT        "seer_ext_tcam_select"
#define spn_SEER_HOST_HASH_TABLE_CFG    "seer_host_hash_table_cfg"
#define spn_SEER_MVL_HASH_TABLE_CFG     "seer_mvl_hash_table_cfg"
#define spn_SEER_HSE_EM_LATENCY7        "seer_hse_em_latency7"
#define spn_SEER_CSE_EM_LATENCY7        "seer_cse_em_latency7"
#define spn_MCU_HG_FORCE                "mcu_hg_force"
#define spn_PHY_XFP_CLOCK               "phy_xfp_clock"
#define spn_PHY_56XXX                   "phy_56xxx"
#define spn_BSAFE_TIMEOUT_USEC          "bsafe_timeout_usec"
#define spn_TCAM_RESET_USEC             "tcam_reset_usec"
#define spn_E2E_64_MODULES              "e2e_64_modules"
#define spn_L3_DEFIP_MAP                "l3_defip_map"
#define spn_SEER_INIT_TIMEOUT_USEC      "seer_init_timeout_usec"
#define spn_PARITY_ENABLE               "parity_enable"
#define spn_PLL600_SLOWCLK              "pll600_slowclk"
#define spn_MCU_CHANNEL_BITMAP          "mcu_channel_bitmap"
#define spn_MCU_TCRD                    "mcu_tcrd"
#define spn_MCU_TCWD                    "mcu_tcwd"
#define spn_MCU_TWL                     "mcu_twl"
#define spn_MCU_DLL90_OFFSET_TX         "mcu_dll90_offset_tx"
#define spn_MCU_DLL90_OFFSET3           "mcu_dll90_offset3"
#define spn_MCU_DLL90_OFFSET2           "mcu_dll90_offset2"
#define spn_MCU_DLL90_OFFSET1           "mcu_dll90_offset1"
#define spn_MCU_DLL90_OFFSET0_QK        "mcu_dll90_offset0_qk"
#define spn_MCU_DLL90_OFFSET_QKB        "mcu_dll90_offset_qkb"
#define spn_MCU_OVRD_SM_EN              "mcu_ovrd_sm_en"
#define spn_MCU_PHASE_SEL               "mcu_phase_sel"
#define spn_MCU_SEL_EARLY2_3            "mcu_sel_early2_3"
#define spn_MCU_SEL_EARLY1_3            "mcu_sel_early1_3"
#define spn_MCU_SEL_EARLY2_2            "mcu_sel_early2_2"
#define spn_MCU_SEL_EARLY1_2            "mcu_sel_early1_2"
#define spn_MCU_SEL_EARLY2_1            "mcu_sel_early2_1"
#define spn_MCU_SEL_EARLY1_1            "mcu_sel_early1_1"
#define spn_MCU_SEL_EARLY2_0            "mcu_sel_early2_0"
#define spn_MCU_SEL_EARLY1_0            "mcu_sel_early1_0"
#define spn_SEER_LPM_TRAVERSE_ENTRIES   "seer_lpm_traverse_entries"
#define spn_MMU_RESET_TRIES             "mmu_reset_tries"
#define spn_MMU_PLL_LOCK_TESTS          "mmu_pll_lock_tests"
#define spn_MCU_ODT_IMP_ENABLE          "mcu_odt_imp_enable"
#define spn_DDR72_DLL90_OFFSET_TX       "ddr72_dll90_offset_tx"
#define spn_DDR72_DLL90_OFFSET3         "ddr72_dll90_offset3"
#define spn_DDR72_DLL90_OFFSET2         "ddr72_dll90_offset2"
#define spn_DDR72_DLL90_OFFSET1         "ddr72_dll90_offset1"
#define spn_DDR72_DLL90_OFFSET0_QK      "ddr72_dll90_offset0_qk"
#define spn_DDR72_DLL90_OFFSET_QKB      "ddr72_dll90_offset_qkb"
#define spn_DDR72_OVRD_SM_EN            "ddr72_ovrd_sm_en"
#define spn_DDR72_PHASE_SEL             "ddr72_phase_sel"
#define spn_DDR72_SEL_EARLY2_3          "ddr72_sel_early2_3"
#define spn_DDR72_SEL_EARLY1_3          "ddr72_sel_early1_3"
#define spn_DDR72_SEL_EARLY2_2          "ddr72_sel_early2_2"
#define spn_DDR72_SEL_EARLY1_2          "ddr72_sel_early1_2"
#define spn_DDR72_SEL_EARLY2_1          "ddr72_sel_early2_1"
#define spn_DDR72_SEL_EARLY1_1          "ddr72_sel_early1_1"
#define spn_DDR72_SEL_EARLY2_0          "ddr72_sel_early2_0"
#define spn_DDR72_SEL_EARLY1_0          "ddr72_sel_early1_0"
#define spn_QDR36_DLL90_OFFSET_TX       "qdr36_dll90_offset_tx"
#define spn_QDR36_DLL90_OFFSET_QK       "qdr36_dll90_offset_qk"
#define spn_QDR36_DLL90_OFFSET_QKB      "qdr36_dll90_offset_qkb"
#define spn_QDR36_OVRD_SM_EN            "qdr36_ovrd_sm_en"
#define spn_QDR36_PHASE_SEL             "qdr36_phase_sel"
#define spn_QDR36_SEL_EARLY2_1          "qdr36_sel_early2_1"
#define spn_QDR36_SEL_EARLY1_1          "qdr36_sel_early1_1"
#define spn_QDR36_SEL_EARLY2_0          "qdr36_sel_early2_0"
#define spn_QDR36_SEL_EARLY1_0          "qdr36_sel_early1_0"
#define spn_SEER_TUNNEL_SAM             "seer_tunnel_sam"
#define spn_EXT_TCAM_SHARING_MASTER     "ext_tcam_sharing_master"
#define spn_EXT_TCAM_SHARING_SLAVE      "ext_tcam_sharing_slave"
#define spn_MEM_CLEAR_CHUNK_SIZE        "mem_clear_chunk_size"
#define spn_MEM_CLEAR_HW_ACCELERATION   "mem_clear_hw_acceleration"
#define spn_L2MC_IN_L2ENTRY             "l2mc_in_l2entry"
#define spn_PHY_WAN_MODE                "phy_wan_mode"
#define spn_PHY_TX_INVERT               "phy_tx_invert"
#define spn_PHY_RX_INVERT               "phy_rx_invert"
#define spn_L2_TABLE_SIZE               "l2_table_size"
#define spn_L3_TABLE_SIZE               "l3_table_size"
#define spn_STG_TABLE_SIZE              "stg_table_size"
#define spn_REG_WRITE_LOG               "reg_write_log"
#define spn_BCM_FIELD_ENTRY_SZ          "bcm_field_entry_sz"
#define spn_BCM_FIELD_KEY_PATTERN       "bcm_field_key_pattern"
#define spn_RCPU_VLAN_ID                "rcpu_vlan"
#define spn_RCPU_USE_OOB                "rcpu_use_oob"
#define spn_RCPU_OOB_CHANNEL            "rcpu_oob_channel"
#define spn_RCPU_RX_PBMP                "rcpu_rx_pbmp"
#define spn_RCPU_PORT                   "rcpu_port"
#define spn_RCPU_MASTER_UNIT            "rcpu_master_unit"
#define spn_RCPU_SLAVE_MODID            "rcpu_slave_modid"
#define spn_RCPU_MASTER_MODID           "rcpu_master_modid"
#define spn_RCPU_HIGIG_PORT             "rcpu_higig_port"
#define spn_PCI2EB_OVERRIDE             "pci2eb_override"
#define spn_DUAL_HASH_RECURSE_DEPTH     "dual_hash_recurse_depth"
#define spn_MMU_FLOW_PERCENT            "mmu_flow_percent"
#define spn_MMU_FLOW_FANIN              "mmu_flow_fanin"
#define spn_MMU_RED_DROP_PERCENT        "mmu_red_drop_percent"
#define spn_MMU_YELLOW_DROP_PERCENT     "mmu_yellow_drop_percent"
#define spn_MMU_STATIC_BYTES            "mmu_static_bytes"
#define spn_MMU_STATIC_PERCENT          "mmu_static_percent"
#define spn_MMU_RESET_BYTES             "mmu_reset_bytes"
#define spn_MMU_OVERCOMMIT              "mmu_overcommit"
#define spn_MMU_OVERCOMMIT_STACK        "mmu_overcommit_stack"
#define spn_L3_IPMC_VALID_AS_HIT        "l3_ipmc_valid_as_hit"
#define spn_EXT_SRAM_TUNING             "ext_sram_tuning"
#define spn_EXT_TCAM_TUNING             "ext_tcam_tuning"
#define spn_EXT_TCAM_CASCADE_COUNT      "ext_tcam_cascade_count"
#define spn_EXT_L2_FWD_TABLE_SIZE       "ext_l2_fwd_table_size"
#define spn_EXT_IP4_FWD_TABLE_SIZE      "ext_ip4_fwd_table_size"
#define spn_EXT_IP6U_FWD_TABLE_SIZE     "ext_ip6u_fwd_table_size"
#define spn_EXT_IP6_FWD_TABLE_SIZE      "ext_ip6_fwd_table_size"
#define spn_EXT_L2_ACL_TABLE_SIZE       "ext_l2_acl_table_size"
#define spn_EXT_IP4_ACL_TABLE_SIZE      "ext_ip4_acl_table_size"
#define spn_EXT_IP6S_ACL_TABLE_SIZE     "ext_ip6s_acl_table_size"
#define spn_EXT_IP6F_ACL_TABLE_SIZE     "ext_ip6f_acl_table_size"
#define spn_EXT_L2C_ACL_TABLE_SIZE      "ext_l2c_acl_table_size"
#define spn_EXT_IP4C_ACL_TABLE_SIZE     "ext_ip4c_acl_table_size"
#define spn_EXT_IP6C_ACL_TABLE_SIZE     "ext_ip6c_acl_table_size"
#define spn_EXT_L2IP4_ACL_TABLE_SIZE    "ext_l2ip4_acl_table_size"
#define spn_EXT_L2IP6_ACL_TABLE_SIZE    "ext_l2ip6_acl_table_size"
#define spn_EXT_TCAM_MODE               "ext_tcam_mode"
#define spn_EXT_TCAM_DEV_TYPE           "ext_tcam_dev_type"
#define spn_EXT_SRAM_MODE               "ext_sram_mode"
#define spn_EXT_SRAM_SPEED              "ext_sram_speed"
#define spn_EXT_SRAM0_PRESENT           "ext_sram0_present"
#define spn_EXT_SRAM1_PRESENT           "ext_sram1_present"
#define spn_EXT_AD_MODE                 "ext_ad_mode"
#define spn_EXT_IP6_FWD_KEY             "ext_ip6_fwd_key"
#define spn_EXT_L2_ACL_KEY              "ext_l2_acl_key"
#define spn_EXT_IP4_ACL_KEY             "ext_ip4_acl_key"
#define spn_EXT_IP6_ACL_KEY             "ext_ip6_acl_key"
#define spn_EXT_LOOKUP_ON_XPORT         "ext_lookup_on_xport"
#define spn_PHY_SGMII_AUTONEG           "phy_sgmii_autoneg"
#define spn_DOT1PRI_COS0                "rcpu_dot1pri_cos0"
#define spn_DOT1PRI_COS1                "rcpu_dot1pri_cos1"
#define spn_DOT1PRI_COS2                "rcpu_dot1pri_cos2"
#define spn_DOT1PRI_COS3                "rcpu_dot1pri_cos3"
#define spn_DOT1PRI_COS4                "rcpu_dot1pri_cos4"
#define spn_DOT1PRI_COS5                "rcpu_dot1pri_cos5"
#define spn_DOT1PRI_COS6                "rcpu_dot1pri_cos6"
#define spn_DOT1PRI_COS7                "rcpu_dot1pri_cos7"
#define spn_MH_TC_COS0                  "rcpu_mh_tc_cos0"
#define spn_MH_TC_COS1                  "rcpu_mh_tc_cos1"
#define spn_MH_TC_COS2                  "rcpu_mh_tc_cos2"
#define spn_MH_TC_COS3                  "rcpu_mh_tc_cos3"
#define spn_MH_TC_COS4                  "rcpu_mh_tc_cos4"
#define spn_MH_TC_COS5                  "rcpu_mh_tc_cos5"
#define spn_MH_TC_COS6                  "rcpu_mh_tc_cos6"
#define spn_MH_TC_COS7                  "rcpu_mh_tc_cos7"
#define spn_CPU_TC_COS0                 "rcpu_cpu_tc_cos0"
#define spn_CPU_TC_COS1                 "rcpu_cpu_tc_cos1"
#define spn_CPU_TC_COS2                 "rcpu_cpu_tc_cos2"
#define spn_CPU_TC_COS3                 "rcpu_cpu_tc_cos3"
#define spn_CPU_TC_COS4                 "rcpu_cpu_tc_cos4"
#define spn_CPU_TC_COS5                 "rcpu_cpu_tc_cos5"
#define spn_CPU_TC_COS6                 "rcpu_cpu_tc_cos6"
#define spn_CPU_TC_COS7                 "rcpu_cpu_tc_cos7"
#define spn_CPU_QUEUE                   "rcpu_cpu_queue"
#define spn_MH_SRC_PID_ENABLE           "rcpu_mh_src_pid_enable"
#define spn_CPU_TC_ENABLE               "rcpu_mh_cpu_cos_enable"
#define spn_MH_TC_MAP_ENABLE            "rcpu_mh_tc_map_enable"
#define spn_DOT1PRI_MAP_ENABLE          "rcpu_dot1pri_map_enable"
#define spn_LMD_ENABLE_PBMP             "lmd_enable_pbmp"
#define spn_MULTI_SBUS_CMDS_SPACING     "multi_sbus_cmds_spacing"
#define spn_IPFIX_HOSTBUF_SIZE          "ipfix_hostbuf_size"
#define spn_IPFIX_THREAD_PRI            "ipfix_thread_pri"
#define spn_GPORT_RSV_MASK              "gport_rsv_mask"
#define spn_POST_ENABLE                 "post_init_enable"
#define spn_MODULE_64PORTS              "module_64ports"
#define spn_GPORT                       "bcm_use_gport"
#define spn_HIGIG2_MULTICAST_VLAN_RANGE "higig2_multicast_vlan_range"
#define spn_HIGIG2_MULTICAST_L2_RANGE   "higig2_multicast_l2_range"
#define spn_HIGIG2_MULTICAST_L3_RANGE   "higig2_multicast_l3_range"
#define spn_MULTICAST_L2_RANGE          "multicast_l2_range"
#define spn_MULTICAST_L3_RANGE          "multicast_l3_range"
#define spn_TSLAM_DMA_ENABLE            "tslam_dma_enable"
#define spn_TABLE_DMA_ENABLE            "table_dma_enable"
#define spn_RATE_I2C_DIVISOR            "rate_i2c_divisor"
#define spn_RATE_I2C_DIVIDEND           "rate_i2c_dividend"
#define spn_RATE_STDMA_DIVISOR          "rate_stdma_divisor"
#define spn_RATE_STDMA_DIVIDEND         "rate_stdma_dividend"
#define spn_RATE_EXT_MDIO_DIVISOR       "rate_ext_mdio_divisor"
#define spn_RATE_EXT_MDIO_DIVIDEND      "rate_ext_mdio_dividend"
#define spn_RATE_INT_MDIO_DIVISOR       "rate_int_mdio_divisor"
#define spn_RATE_INT_MDIO_DIVIDEND      "rate_int_mdio_dividend"
#define spn_BCM_OAM_THREAD_PRI          "bcm_oam_thread_pri"
#define spn_SYSSNAKE_IGNORE_PBMP        "ss_ignore_pbmp"
#define spn_SWITCH_BYPASS_MODE          "switch_bypass_mode"
#define spn_DIAG_SHELL_USE_SLAM         "diag_shell_use_slam"
#define spn_RLINK_L2_REMOTE_MAX         "rlink_l2_remote_max"
#define spn_RLINK_L2_LOCAL_MAX          "rlink_l2_local_max"
#define spn_RLINK_LINK_REMOTE_MAX       "rlink_link_remote_max"
#define spn_RLINK_LINK_LOCAL_MAX        "rlink_link_local_max"
#define spn_RLINK_AUTH_REMOTE_MAX       "rlink_auth_remote_max"
#define spn_RLINK_AUTH_LOCAL_MAX        "rlink_auth_local_max"
#define spn_RLINK_RX0_REMOTE_MAX        "rlink_rx0_remote_max"
#define spn_RLINK_RX1_REMOTE_MAX        "rlink_rx1_remote_max"
#define spn_RLINK_RX2_REMOTE_MAX        "rlink_rx2_remote_max"
#define spn_RLINK_RX3_REMOTE_MAX        "rlink_rx3_remote_max"
#define spn_RLINK_RX4_REMOTE_MAX        "rlink_rx4_remote_max"
#define spn_RLINK_RX5_REMOTE_MAX        "rlink_rx5_remote_max"
#define spn_RLINK_RX6_REMOTE_MAX        "rlink_rx6_remote_max"
#define spn_RLINK_RX7_REMOTE_MAX        "rlink_rx7_remote_max"
#define spn_CONQUEROR_16X16             "conqueror_16x16"
#define spn_SKIP_L2_USER_ENTRY          "skip_L2_USER_ENTRY"

/* VENDOR_BROADCOM [ */
#define spn_LMD_ENABLE_OVERRIDE         "lmd_enable_override"
/* VENDOR_BROADCOM ] */

#define spn_BCM5321         "bcm5321"
#define spn_BCM5320         "bcm5320"

/* SBX properties */
/*
 * UCODE Port Properties
 *
 * The following properties are for FE to convey mapping
 * from microcode port to physical port.
 *
 *   ucode_num_ports - defines the maximum number of microcode visible
 *                     ports on any module.
 *
 *   ucode_port_<number> = <b-type><b-num>[.<b-port>:<s-type><s-num>.<s-port>]
 *
 * where,
 *   <b-type> is the front-panel block type: ge, xe, spi, pci
 *   <b-num>  is the front-panel block number (0-1)
 *   <b-port> is the front-panel port number (within the block)
 *   <s-type> is the system-side block type: spi
 *   <s-num>  is the system-side block number (0-1)
 *   <s-port> is the system-side port number (within the block)
 *
 * Notes:
 * Mapping for a SPI bus port or PCI port should only contain the
 * fields <b-type> and <b-num>.
 *
 * Examples:
 *   ucode_num_ports.1    = 32
 *   ucode_port.port1.1   = ge0.0:spi1.0    <-- Front panel GE port
 *   ucode_port.port27.1  = spi0.0:spi1.1   <-- Front panel SPI subport
 *   ucode_port.port31.1  = spi0            <-- SPI port (for SPI0)
 *   ucode_port.port32.1  = pci0            <-- CPU port
 */
#define spn_UCODE_NUM_PORTS             "ucode_num_ports"
#define spn_UCODE_PORT                  "ucode_port"

/* Block types for ucode port property <b-type> <s-type> */
#define spn_UCODE_PORT_TYPE_GE          "ge"
#define spn_UCODE_PORT_TYPE_XE          "xe"
#define spn_UCODE_PORT_TYPE_SPI         "spi"
#define spn_UCODE_PORT_TYPE_PCI         "pci"
#define spn_UCODE_PORT_TYPE_HG          "hg"

/*
 * Following properties are vaild on FE to indicate
 * if the Microcode support the given feature
 */
#define spn_FE_UCODE_FEATURE_HG         "ucode_feature_hg"

/* bcm_rx parses out the sbx Egress Route header when set */
#define spn_RX_PARSE_ERH                "rx_parse_erh"
/* bcm_rx parses out two headers (ERG and HG) when set */
#define spn_RX_PARSE_ERH_HG             "rx_parse_erh_hg"

/*
 * Following properties are global
 *   spn_FABRIC_CONFIGURATION
 *      - fabric mode
 *        0: DMode (qe2000 + bm3200 system)
 *        1:
 *        2:
 *   spn_MTU_SIZE
 *      - MTU size in bytes used for wred calculation
 *   spn_BIST_ENABLE
 *      - 0: not run self test, 1: run self test
 *   spn_HALF_BUS_MODE
 *      - 0: full-bus mode system, 1: half-bus mode system
 *   spn_ACTIVE_SWITCH_CONTROLLER_ID
 *      - 0: Switch controller 0 is active, 1: Switch controller 1 is active
 *   spn_REDUNDANCY_MODE
 *      - 0:     manual mode
 *        1:     1+1 control and data plane
 *        2:     1+1 control and Load sharing data plane
 *        3:     1+1 control and Enhanced Load sharing data plane
 *        4:     Load sharing data plane
 *        5:     Enhanced load sharing data plane
 *   spn_MAX_FAILED_LINKS
 *      - max number of failed links before QE is mapped out, used for
 *        load sharing data plane redundancy mode
 *   spn_HYBRID_MODE
 *      - System configuration is Hybrid mode. 1 indicates hybrid mode
 *   spn_NUM_MAX_FABRIC_PORT_ON_MODULE
 *      - maximim fabric ports on any module. Required for Hybrid mode.
 *   spn_QE_TME_MODE
 *      - specifies if the QE is running in TME mode.
 *        1 means TME
 *        2 means Hybrid
 *
 * Following properties are global but required for sirius and polaris only
 *   spn_40G_PIPE_MODE
 *   spn_BACKPLANE_SERDES_SPEED
 *   spn_BACKPLANE_SERDES_ENCODING
 *   spn_QE2K_LINKS
 *   spn_QE4K_LINKS
 */
#define spn_FABRIC_CONFIGURATION        "fabric_configuration"
#define spn_DISCARD_MTU_SIZE            "discard_mtu_size"
#define spn_DISCARD_QUEUE_SIZE          "discard_queue_size"
#define spn_BIST_ENABLE                 "bist_enable"
#define spn_HALF_BUS_MODE               "half_bus_mode"

#define spn_40G_PIPE_MODE               "40g_pipe_mode"
#define spn_BACKPLANE_SERDES_SPEED      "backplane_serdes_speed"
#define spn_BACKPLANE_SERDES_ENCODING   "backplane_serdes_encoding"
#define spn_QE2K_LINKS                  "qe2k_links"
#define spn_QE4K_LINKS                  "qe4k_links"

#define spn_ACTIVE_SWITCH_CONTROLLER_ID "active_switch_controller_id"
#define spn_REDUNDANCY_MODE             "redundancy_mode"
#define spn_MAX_FAILED_LINKS            "max_failed_links"
#define spn_BCM_COSQ_INIT               "bcm_cosq_init"
#define spn_QE_TME_MODE                 "qe_tme_mode"
#define spn_HOLD_PRI_NUM_TIMESLOTS       "hold_pri_num_timeslots"
#define spn_HYBRID_MODE                  "hybrid_mode"
#define spn_NUM_MAX_FABRIC_PORTS_ON_MODULE  "num_max_fabric_ports_on_module"

/*
 * L2 Aging Cycles
 *
 * Indicates the number of cycles in an L2 aging interval.  This value
 * affects the number of L2 entries to be processed by the aging engine
 * during a run.
 * A value of 1 results in processing the entire L2 table during
 * an age run cycle.
 */
#define spn_L2_AGE_CYCLES                "l2_age_cycles"


/*
 * On SBX QE devices Local McGroup resources start from the following index.
 *    - VLANS McGroup  => 0-4095
 *    - Global McGroup => 4096 - (value indicated by (spn_MC_GROUP_LOCAL_START_INDEX - 1))
 *    - Local McGroup  => (value indicated by spn_MC_GROUP_LOCAL_START_INDEX) - (max device limit)
 */
#define spn_MC_GROUP_LOCAL_START_INDEX  "mcast_group_local_start_index"


/*
 * Following properties are valid on all devices
 */
#define spn_PORT_TYPE                    "port_type"
#define spn_PORT_IS_SCI                  "port_is_sci"
#define spn_PORT_IS_SFI                  "port_is_sfi"
#define spn_LINK_THRESHOLD               "link_threshold"
#define spn_LINK_DRIVER_STRENGTH         "link_driver_strength"
#define spn_LINK_DRIVER_EQUALIZATION     "link_driver_equalization"

/*
 * Following properties are vaild on FE to indicate which
 * SPI is connected into the fabric/QE
 */
#define spn_FE_SPI0_FABRIC              "fe_spi0_fabric"
#define spn_FE_SPI1_FABRIC              "fe_spi1_fabric"

/*
 * Following properties are valid on QE only
 *   spn_NUM_MODULES
 *      - specifies the number of QEs in the system
 *   spn_NUM_MODULES_00_31_MASK
 *      - mask of queues with module/node 00 - 31
 *   spn_NUM_MODULES_32_63_MASK
 *      - mask of queues with module/node 32 - 63
 *   spn_NUM_MODULES_64_95_MASK
 *      - mask of queues with module/node 64 - 95
 *   spn_SPI_0_REF_CLOCK_SPEED
 *      - Reference Clock speed in KHz for SPI port 0.
 *   spn_SPI_0_CLOCK_SPEED
 *      - Clock speed specified in KHz for SPI port 0.
 *   spn_SPI_1_REF_CLOCK_SPEED
 *      - Reference Clock speed in KHz for SPI port 1. 
 *   spn_SPI_1_CLOCK_SPEED
 *      - Clock speed specified in KHz for SPI port 1.
 *   spn_QE_SPI_0_SUBPORTS
 *      - specifies the number of sub-ports on SPI port 0
 *   spn_QE_SPI_1_SUBPORTS
 *      - specifies the number of sub-ports on SPI port 1
 *   spn_SPI_0_NUM_TX_SUBPORTS
 *      - specifies the number of Tx sub-ports on SPI port 0
 *   spn_SPI_0_NUM_RX_SUBPORTS
 *      - specifies the number of Rx sub-ports on SPI port 0
 *   spn_SPI_1_NUM_TX_SUBPORTS
 *      - specifies the number of Tx sub-ports on SPI port 1
 *   spn_SPI_1_NUM_RX_SUBPORTS
 *      - specifies the number of Rx sub-ports on SPI port 1
 *   spn_SPI_0_TX_CAL_STATUS_REP_CNT
 *      - specifies Tx cal_m for SPI Port 0
 *   spn_SPI_0_RX_CAL_STATUS_REP_CNT
 *      - specifies Rx cal_m for SPI Port 0
 *   spn_SPI_1_TX_CAL_STATUS_REP_CNT
 *      - specifies Tx cal_m for SPI Port 1
 *   spn_SPI_1_RX_CAL_STATUS_REP_CNT
 *      - specifies Rx cal_m for SPI Port 0
 *   spn_QE_SPI_0_SUBPORT_SPEED
 *      - specifies the SPI 0 subport speed in Mbps
 *        if not specified, default to 1Gbps if spn_QE_SPI_0_SUBPORTS
 *        has more than 1 subport, otherwise default to 10 Gbps
 *   spn_QE_SPI_1_SUBPORT_SPEED
 *      - specifies the SPI 1 subport speed in Mbps
 *        if not specified, default to 1Gbps if spn_QE_SPI_1_SUBPORTS
 *        has more than 1 subport, otherwise default to 10 Gbps
 *   spn_QE_SPI_0_SUBPORT_IS_REQUEUE
 *      - specifies the SPI 0 subport is in requeue mode (for hybrid mode)
 *   spn_QE_SPI_1_SUBPORT_IS_REQUEUE
 *      - specifies the SPI 1 subport is in requeue mode (for hybrid mode)
 *   spn_QE_MEMORY_PART
 *      - specifies the memory part
 *        0:256mb DDR2 memory   , 2: 512mb DDR2 memory
 *   spn_QE_MAX_ARRIVAL_RATE
 *      - max arrival rate in Mbps
 *   spn_QE_CLOCK_SPEED
 *      - qe device clock speed in Mhz
 *   spn_QE_LONG_DDR_MEMTEST
 *      -
 *   spn_QE_2_5GBPS_LINKS
 *      -
 *   spn_QE_EG_MVT_SIZE
 *      - Number of MVT entries. Valid range (0,1,2)
 *        0: 12k entries.  1: 24k entries.  2: 48k entries
 *   spn_QE_EG_MC_DROP_ON_FULL
 *      -
 *   spn_QE_EI_PORT_TIMEOUT
 *      -
 *   spn_QE_GLOBAL_SHAPING_ADJUST
 *      -
 *   spn_QE_MIX_HIGH_LOW_RATE_FLOWS
 *      -
 *   spn_QE_SPI_0_FULL_PACKET_MODE
 *      - SPI 0 interface full packet mode
 *        0: Burst Interleaved
 *        1: Packet interleaved, full packet mode
 *        2: Packet Interleaved, N:1 channel sharing mode
 *   spn_QE_SPI_1_FULL_PACKET_MODE
 *      - SPI 1 interface full packet mode
 *        0: Burst Interleaved
 *        1: Packet interleaved, full packet mode
 *        2: Packet Interleaved, N:1 channel sharing mode
 *   spn_QE_QUEUES_PER_INGRESS_SHAPER
 *      - Number of queues for each ingress shaper. Valid Range(1,4,8,16)
 *   spn_QE_SC_TXDMA_SOT_DELAY_CLOCKS
 *      -
 *   spn_QE_SFI_TIMESLOT_OFFSET_CLOCKS
 *      -
 *   spn_QE_EP_DISABLE                 "qe_ep_disable"
 *      - Set to 1 to disable EP
 *   spn_QE_MVT_FORMAT                 "qe_mvt_format"
 *      - Indicates EP format (MVTDA, MVTDB), Reference "encap_id" parameter
 *        of multicast Group API.
 *        0 -> Format required by gu2k, 1 -> 0-13 bits are mvtda, and 14-17 bits are mvtdb
 *   spn_QE_GRANT_OFFSET               "qe_grant_offset"
 *      - Number of clock cycles to wait after Start of Timeslot signal.
 *        Valid range (0-31), Default to 0xE
 *   spn_EGRESS_MC_EF_TIMEOUT
 *      - Timeout specified in micro-seconds
 *        A value of 0 indicates no timeout
 *   spn_EGRESS_MC_NEF_TIMEOUT
 *      - Timeout specified in micro-seconds
 *        A value of 0 indicates no timeout
 *   spn_EGRESS_FIFO_INDEPENDENT_FC
 *      - Egress FIFO independent flow control, 1 -> single bit full status mode,
 *                                              0 -> double bit full status mode
 *
 */
#define spn_NUM_MODULES                   "num_modules"
#define spn_NUM_MODULES_00_31_MASK        "num_modules_00_31_mask"
#define spn_NUM_MODULES_32_63_MASK        "num_modules_32_63_mask"
#define spn_NUM_MODULES_64_95_MASK        "num_modules_64_95_mask"
#define spn_SPI_0_REF_CLOCK_SPEED         "spi_0_ref_clock_speed"
#define spn_SPI_0_CLOCK_SPEED             "spi_0_clock_speed"
#define spn_SPI_1_REF_CLOCK_SPEED         "spi_1_ref_clock_speed"
#define spn_SPI_1_CLOCK_SPEED             "spi_1_clock_speed"
#define spn_QE_SPI_0_SUBPORTS             "qe_spi_0"
#define spn_QE_SPI_1_SUBPORTS             "qe_spi_1"
#define spn_SPI_0_NUM_TX_SUBPORTS         "spi_0_num_tx_subports"
#define spn_SPI_0_NUM_RX_SUBPORTS         "spi_0_num_rx_subports"
#define spn_SPI_1_NUM_TX_SUBPORTS         "spi_1_num_tx_subports"
#define spn_SPI_1_NUM_RX_SUBPORTS         "spi_1_num_rx_subports"
#define spn_SPI_0_TX_CAL_STATUS_REP_CNT   "spi_0_tx_cal_status_rep_cnt"
#define spn_SPI_0_RX_CAL_STATUS_REP_CNT   "spi_0_rx_cal_status_rep_cnt"
#define spn_SPI_1_TX_CAL_STATUS_REP_CNT   "spi_1_rx_cal_status_rep_cnt"
#define spn_SPI_1_RX_CAL_STATUS_REP_CNT   "spi_1_tx_cal_status_rep_cnt"
#define spn_QE_SPI_0_SUBPORT_SPEED        "qe_spi_0_subport_speed"
#define spn_QE_SPI_1_SUBPORT_SPEED        "qe_spi_1_subport_speed"
#define spn_QE_SPI_0_SUBPORT_IS_REQUEUE   "qe_spi_0_subport_is_requeue"
#define spn_QE_SPI_1_SUBPORT_IS_REQUEUE   "qe_spi_1_subport_is_requeue"
#define spn_QE_HALF_BUS_MODE              "qe_half_bus_mode"
#define spn_QE_MEMORY_PART                "qe_memory_part"
#define spn_QE_MAX_ARRIVAL_RATE           "qe_max_arrival_rate"
#define spn_QE_CLOCK_SPEED                "qe_clock_speed"
#define spn_QE_LONG_DDR_MEMTEST           "qe_long_ddr_memtest"
#define spn_QE_2_5GBPS_LINKS              "qe_2_5gbps_links"
#define spn_QE_EG_MVT_SIZE                "qe_eg_mvt_size"
#define spn_QE_EG_MC_DROP_ON_FULL         "qe_eg_mc_drop_on_full"
#define spn_QE_EI_PORT_TIMEOUT            "qe_ei_port_timeout"
#define spn_QE_GLOBAL_SHAPING_ADJUST      "qe_global_shaping_adjust"
#define spn_QE_MIX_HIGH_LOW_RATE_FLOWS    "qe_mix_high_low_rate_flows"
#define spn_QE_SPI_0_FULL_PACKET_MODE     "qe_spi_0_full_packet_mode"
#define spn_QE_SPI_1_FULL_PACKET_MODE     "qe_spi_1_full_packet_mode"
#define spn_QE_QUEUES_PER_INGRESS_SHAPER  "qe_queues_per_ingress_shaper"
#define spn_QE_SC_TXDMA_SOT_DELAY_CLOCKS  "qe_sc_txdma_sot_delay_clocks"
#define spn_QE_SFI_TIMESLOT_OFFSET_CLOCKS "qe_sfi_timeslot_offset_clocks"
#define spn_QE_EP_DISABLE                 "qe_ep_disable"
#define spn_QE_MVT_FORMAT                 "qe_mvt_format"
#define spn_QE_GRANT_OFFSET               "qe_grant_offset"
#define spn_EGRESS_MC_EF_TIMEOUT          "egress_mc_ef_timeout"
#define spn_EGRESS_MC_NEF_TIMEOUT         "egress_mc_nef_timeout"
#define spn_EGRESS_FIFO_INDEPENDENT_FC    "egress_fifo_independent_fc"


#define spn_QE_MVT_OLD_CONFIGURATION      "qe_mvt_old_configuration"


/*
 * Following properties are valid on BME or SE or LCM or BME+SE device
 *    spn_BM_DEVICE_MODE
 *       - specifies mode the BM device is running with
 *         0: BME only
 *         1: SE only
 *         2: BME+SE
 *         3: LCM only
 */
#define spn_BM_DEVICE_MODE              "bm_device_mode"

/*
 * Following properties are vaild on BME
 *    spn_BME_SWITCH_CONTROLLER_ID
 *       - switch controller id for the BME
 *    spn_BME_NUM_ESETS
 *       - Number of ESETS. Relevant for Polaris/QE2000 configuration.
 *         This configuration allows 1024 ESETS. ESETS above 128 require
 *         special VOQ allocation algorithm.
 */
#define spn_BME_SWITCH_CONTROLLER_ID        "bme_switch_controller_id"
#define spn_BME_NUM_ESETS                   "bme_num_esets"
#define spn_ENABLE_ALL_MODULE_ARBITRATION   "enable_all_module_arbitration"

/* number of QEs in the system */
#define spn_NUM_MODULES                 "num_modules"

/*
 * Following properties are vaild on SE
 */

/*
 * Following properties are vaild on LCM
 *    spn_LCM_PASSTHROUGH_MODE
 *       - 1 for passthrough mode, 0 for 1+1 mode
 *    spn_LCM_ACTIVE_PLANE_ID
 *       - 0/1 specify active data plane id
 *    spn_LCM_DATAPLANE_0_MAP
 *       - per port(link) property, specify the destination port(link) for the port on
 *         dataplane 0
 *    spn_LCM_DATAPLANE_1_MAP
 *       - per port(link) property, specify the destination port(link) for the port on
 *         dataplane 1
 *    spn_LCM_XCFG_AB_INPUT_POLARITY_REVERSED
 *       -
 */
#define spn_LCM_PASSTHROUGH_MODE            "lcm_passthrough_mode"
#define spn_LCM_ACTIVE_PLANE_ID             "lcm_active_plane_id"
#define spn_LCM_DATAPLANE_0_MAP             "lcm_dataplane_0_map"
#define spn_LCM_DATAPLANE_1_MAP             "lcm_dataplane_1_map"
#define spn_LCM_XCFG_AB_INPUT_POLARITY_REVERSED "lcm_xcfg_ab_input_polarity_reversed"

/*
 * Following properties are used by the diag shell only
 *    spn_DIAG_CHASSIS
 *       - defines the chassis type, 0 standalone
 *                                   1 fabric card + line cards
 *    spn_DIAG_COSQ_INIT
 *       - diag shell performs gport adds. Must be used with bcm_cosq_init=0
 */
#define spn_DIAG_CHASSIS                     "diag_chassis"
#define spn_DIAG_SERDES_MASK                 "diag_serdes_mask"
#define spn_DIAG_NODES_MASK                  "diag_nodes_mask"
#define spn_DIAG_SLAVE_FC                    "diag_slave_fc"
#define spn_DIAG_SLOT                        "diag_slot"
#define spn_DIAG_COSQ_INIT                   "diag_cosq_init"
#define spn_DIAG_EASY_RELOAD                 "diag_easy_reload"
#define spn_DIAG_DISABLE_INTERRUPTS          "diag_disable_interrupts"
#endif  /* !_SOC_PROPERTY_H */


