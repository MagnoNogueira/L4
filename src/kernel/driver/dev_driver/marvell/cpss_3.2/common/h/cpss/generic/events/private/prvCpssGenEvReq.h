/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenEvReq.h
*
* DESCRIPTION:
*       Includes unified CPSS private event routine.
*       this file is used to define bind between the cpssDriver of PP to
*       unify events , and for FA and for XBAR
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/
#ifndef __prvCpssGenEvReqh
#define __prvCpssGenEvReqh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>
#include <cpss/generic/events/cpssGenEventCtrl.h>

/*
 * typedef: enum PRV_CPSS_DEVICE_TYPE_ENT
 *
 * Description: Enumeration of types of devices PP/FA/XBAR/DRAGONITE
 *
 *        PRV_CPSS_DEVICE_TYPE_PP_E - PP device
 *        PRV_CPSS_DEVICE_TYPE_FA_E - FA device
 *        PRV_CPSS_DEVICE_TYPE_XBAR_E - XBAR device
 *        PRV_CPSS_DEVICE_TYPE_DRAGONITE_E - DRAGONITE device
 *
 */
typedef enum{
    PRV_CPSS_DEVICE_TYPE_PP_E,
    PRV_CPSS_DEVICE_TYPE_FA_E,
    PRV_CPSS_DEVICE_TYPE_XBAR_E,
    PRV_CPSS_DEVICE_TYPE_DRAGONITE_E,

    PRV_CPSS_DEVICE_TYPE_LAST_E
}PRV_CPSS_DEVICE_TYPE_ENT;

#define STR(strname)    \
    #strname

#define UNI_EV_NAME                                                      \
/* PCI */                                                                \
STR(CPSS_PP_SLV_WRITE_ERR_E                                           ), \
STR(CPSS_PP_MAS_WRITE_ERR_E                                           ), \
STR(CPSS_PP_ADDR_ERR_E                                                ), \
STR(CPSS_PP_MAS_ABORT_E                                               ), \
STR(CPSS_PP_TARGET_ABORT_E                                            ), \
STR(CPSS_PP_SLV_READ_ERR_E                                            ), \
STR(CPSS_PP_MAS_READ_ERR_E                                            ), \
STR(CPSS_PP_RETRY_CNTR_E                                              ), \
                                                                         \
/* Misc */                                                               \
STR(CPSS_PP_MISC_TWSI_TIME_OUT_E                                      ), \
STR(CPSS_PP_MISC_TWSI_STATUS_E                                        ), \
STR(CPSS_PP_MISC_ILLEGAL_ADDR_E                                       ), \
STR(CPSS_PP_MISC_CPU_PORT_RX_OVERRUN_E                                ), \
STR(CPSS_PP_MISC_CPU_PORT_TX_OVERRUN_E                                ), \
STR(CPSS_PP_MISC_TX_CRC_PORT_E                                        ), \
STR(CPSS_PP_MISC_C2C_W_FAR_END_UP_E                                   ), \
STR(CPSS_PP_MISC_C2C_N_FAR_END_UP_E                                   ), \
STR(CPSS_PP_MISC_C2C_DATA_ERR_E                                       ), \
STR(CPSS_PP_MISC_MSG_TIME_OUT_E                                       ), \
STR(CPSS_PP_MISC_UPDATED_STAT_E                                       ), \
STR(CPSS_PP_GPP_E                                                     ), \
STR(CPSS_PP_MISC_UPLINK_W_ECC_ON_DATA_E                               ), \
STR(CPSS_PP_MISC_UPLINK_W_ECC_ON_HEADER_E                             ), \
STR(CPSS_PP_MISC_UPLINK_N_ECC_ON_DATA_E                               ), \
STR(CPSS_PP_MISC_UPLINK_N_ECC_ON_HEADER_E                             ), \
STR(CPSS_PP_MISC_PEX_ADDR_UNMAPPED_E                                  ), \
STR(CPSS_PP_MISC_GENXS_READ_DMA_DONE_E                                ), \
                                                                         \
/* Ingress Buffer Manager */                                             \
STR(CPSS_PP_BM_EMPTY_CLEAR_E                                          ), \
STR(CPSS_PP_BM_EMPTY_INC_E                                            ), \
STR(CPSS_PP_BM_AGED_PACKET_E                                          ), \
STR(CPSS_PP_BM_MAX_BUFF_REACHED_E                                     ), \
STR(CPSS_PP_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E                         ), \
STR(CPSS_PP_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E                          ), \
STR(CPSS_PP_BM_INVALID_ADDRESS_E                                      ), \
STR(CPSS_PP_BM_WRONG_SRC_PORT_E                                       ), \
STR(CPSS_PP_BM_MC_INC_OVERFLOW_E                                      ), \
STR(CPSS_PP_BM_MC_INC_UNDERRUN_E                                      ), \
STR(CPSS_PP_BM_TQ_PARITY_ERROR_PORT_E                                 ), \
STR(CPSS_PP_BM_RX_MEM_READ_ECC_ERROR_E                                ), \
STR(CPSS_PP_BM_VLT_ECC_ERR_E                                          ), \
STR(CPSS_PP_BM_MISC_E                                                 ), \
STR(CPSS_PP_BM_TRIGGER_AGING_DONE_E                                   ), \
STR(CPSS_PP_BM_PORT_RX_FULL_E                                         ), \
STR(CPSS_PP_BM_ALL_GIGA_PORTS_BUFF_LIMIT_REACHED_E                    ), \
STR(CPSS_PP_BM_ALL_HG_STACK_PORTS_BUFF_LIMIT_REACHED_E                ), \
                                                                         \
/* Port */                                                               \
STR(CPSS_PP_PORT_LINK_STATUS_CHANGED_E                                ), \
STR(CPSS_PP_PORT_AN_COMPLETED_E                                       ), \
STR(CPSS_PP_PORT_RX_FIFO_OVERRUN_E                                    ), \
STR(CPSS_PP_PORT_TX_FIFO_UNDERRUN_E                                   ), \
STR(CPSS_PP_PORT_TX_FIFO_OVERRUN_E                                    ), \
STR(CPSS_PP_PORT_TX_UNDERRUN_E                                        ), \
STR(CPSS_PP_PORT_ADDRESS_OUT_OF_RANGE_E                               ), \
STR(CPSS_PP_PORT_PRBS_ERROR_E                                         ), \
STR(CPSS_PP_PORT_SYNC_STATUS_CHANGED_E                                ), \
STR(CPSS_PP_PORT_TX_CRC_ERROR_E                                       ), \
STR(CPSS_PP_PORT_ILLEGAL_SEQUENCE_E                                   ), \
STR(CPSS_PP_PORT_IPG_TOO_SMALL_E                                      ), \
STR(CPSS_PP_PORT_FAULT_TYPE_CHANGE_E                                  ), \
STR(CPSS_PP_PORT_FC_STATUS_CHANGED_E                                  ), \
STR(CPSS_PP_PORT_CONSECUTIVE_TERM_CODE_E                              ), \
STR(CPSS_PP_INTERNAL_PHY_E                                            ), \
STR(CPSS_PP_PORT_NO_BUFF_PACKET_DROP_E                                ), \
STR(CPSS_PP_PORT_XAUI_PHY_E                                           ), \
STR(CPSS_PP_PORT_COUNT_COPY_DONE_E                                    ), \
STR(CPSS_PP_PORT_COUNT_EXPIRED_E                                      ), \
/* Group Of Ports */                                                     \
STR(CPSS_PP_GOP_ADDRESS_OUT_OF_RANGE_E                                ), \
STR(CPSS_PP_GOP_COUNT_EXPIRED_E                                       ), \
STR(CPSS_PP_GOP_COUNT_COPY_DONE_E                                     ), \
STR(CPSS_PP_GOP_GIG_BAD_FC_PACKET_GOOD_CRC_E                          ), \
STR(CPSS_PP_GOP_XG_BAD_FC_PACKET_GOOD_CRC_E                           ), \
STR(CPSS_PP_XSMI_WRITE_DONE_E                                         ), \
                                                                         \
/* Tx Queue */                                                           \
STR(CPSS_PP_TQ_WATCHDOG_EX_PORT_E                                     ), \
STR(CPSS_PP_TQ_TXQ2_FLUSH_PORT_E                                      ), \
STR(CPSS_PP_TQ_TXQ2_MG_FLUSH_E                                        ), \
STR(CPSS_PP_TQ_ONE_ECC_ERROR_E                                        ), \
STR(CPSS_PP_TQ_TWO_ECC_ERROR_E                                        ), \
STR(CPSS_PP_TQ_MG_READ_ERR_E                                          ), \
STR(CPSS_PP_TQ_HOL_REACHED_PORT_E                                     ), \
STR(CPSS_PP_TQ_RED_REACHED_PORT_E                                     ), \
STR(CPSS_PP_TQ_TOTAL_DESC_UNDERFLOW_E                                 ), \
STR(CPSS_PP_TQ_TOTAL_DESC_OVERFLOW_E                                  ), \
STR(CPSS_PP_TQ_TOTAL_BUFF_UNDERFLOW_E                                 ), \
STR(CPSS_PP_TQ_SNIFF_DESC_DROP_E                                      ), \
STR(CPSS_PP_TQ_MLL_PARITY_ERR_E                                       ), \
STR(CPSS_PP_TQ_MC_FIFO_OVERRUN_E                                      ), \
STR(CPSS_PP_TQ_MISC_E                                                 ), \
STR(CPSS_PP_TQ_PORT_DESC_FULL_E                                       ), \
STR(CPSS_PP_TQ_MC_DESC_FULL_E                                         ), \
STR(CPSS_PP_TQ_GIGA_FIFO_FULL_E                                       ), \
STR(CPSS_PP_TQ_XG_MC_FIFO_FULL_E                                      ), \
STR(CPSS_PP_TQ_PORT_FULL_XG_E                                         ), \
                                                                         \
/* Ethernet Bridge */                                                    \
STR(CPSS_PP_EB_AUQ_FULL_E                                             ), \
STR(CPSS_PP_EB_AUQ_PENDING_E                                          ), \
STR(CPSS_PP_EB_AUQ_OVER_E                                             ), \
STR(CPSS_PP_EB_AUQ_ALMOST_FULL_E                                      ), \
STR(CPSS_PP_EB_FUQ_FULL_E                                             ), \
STR(CPSS_PP_EB_FUQ_PENDING_E                                          ), \
STR(CPSS_PP_EB_NA_FIFO_FULL_E                                         ), \
STR(CPSS_PP_EB_MG_ADDR_OUT_OF_RANGE_E                                 ), \
STR(CPSS_PP_EB_VLAN_TBL_OP_DONE_E                                     ), \
STR(CPSS_PP_EB_SECURITY_BREACH_UPDATE_E                               ), \
STR(CPSS_PP_EB_VLAN_SECURITY_BREACH_E                                 ), \
STR(CPSS_PP_EB_NA_NOT_LEARNED_SECURITY_BREACH_E                       ), \
STR(CPSS_PP_EB_SA_MSG_DISCARDED_E                                     ), \
STR(CPSS_PP_EB_QA_MSG_DISCARDED_E                                     ), \
STR(CPSS_PP_EB_SA_DROP_SECURITY_BREACH_E                              ), \
STR(CPSS_PP_EB_DA_DROP_SECURITY_BREACH_E                              ), \
STR(CPSS_PP_EB_DA_SA_DROP_SECURITY_BREACH_E                           ), \
STR(CPSS_PP_EB_NA_ON_LOCKED_DROP_SECURITY_BREACH_E                    ), \
STR(CPSS_PP_EB_MAC_RANGE_DROP_SECURITY_BREACH_E                       ), \
STR(CPSS_PP_EB_INVALID_SA_DROP_SECURITY_BREACH_E                      ), \
STR(CPSS_PP_EB_VLAN_NOT_VALID_DROP_SECURITY_BREACH_E                  ), \
STR(CPSS_PP_EB_VLAN_NOT_MEMBER_DROP_DROP_SECURITY_BREACH_E            ), \
STR(CPSS_PP_EB_VLAN_RANGE_DROP_SECURITY_BREACH_E                      ), \
STR(CPSS_PP_EB_INGRESS_FILTER_PCKT_E                                  ), \
STR(CPSS_PP_MAC_TBL_READ_ECC_ERR_E                                    ), \
STR(CPSS_PP_MAC_SFLOW_E                                               ), \
STR(CPSS_PP_MAC_NUM_OF_HOP_EXP_E                                      ), \
STR(CPSS_PP_MAC_NA_LEARNED_E                                          ), \
STR(CPSS_PP_MAC_NA_NOT_LEARNED_E                                      ), \
STR(CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E                               ), \
STR(CPSS_PP_MAC_MG_ADDR_OUT_OF_RANGE_E                                ), \
STR(CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E                                ), \
STR(CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E                                ), \
STR(CPSS_PP_MAC_NA_SELF_LEARNED_E                                     ), \
STR(CPSS_PP_MAC_NA_FROM_CPU_LEARNED_E                                 ), \
STR(CPSS_PP_MAC_NA_FROM_CPU_DROPPED_E                                 ), \
STR(CPSS_PP_MAC_AGED_OUT_E                                            ), \
STR(CPSS_PP_MAC_FIFO_2_CPU_EXCEEDED_E                                 ), \
STR(CPSS_PP_MAC_1ECC_ERRORS_E                                         ), \
STR(CPSS_PP_MAC_2ECC_ERRORS_E                                         ), \
STR(CPSS_PP_MAC_TBL_OP_DONE_E                                         ), \
STR(CPSS_PP_MAC_PACKET_RSSI_LESS_THAN_THRESH_E                        ), \
STR(CPSS_PP_MAC_UPD_AVG_RSSI_LESS_THAN_THRESH_E                       ), \
STR(CPSS_PP_MAC_STG_TBL_DATA_ERROR_E                                  ), \
STR(CPSS_PP_MAC_VIDX_TBL_DATA_ERROR_E                                 ), \
STR(CPSS_PP_LX_LB_ERR_E                                               ), \
STR(CPSS_PP_LX_CTRL_MEM_2_RF_ERR_E                                    ), \
STR(CPSS_PP_LX_TCB_CNTR_E                                             ), \
STR(CPSS_PP_LX_IPV4_MC_ERR_E                                          ), \
STR(CPSS_PP_LX_IPV4_LPM_ERR_E                                         ), \
STR(CPSS_PP_LX_IPV4_ROUTE_ERR_E                                       ), \
STR(CPSS_PP_LX_IPV4_CNTR_E                                            ), \
STR(CPSS_PP_LX_L3_L7_ERR_ADDR_E                                       ), \
STR(CPSS_PP_LX_TRUNK_ADDR_OUT_OF_RANGE_E                              ), \
STR(CPSS_PP_LX_IPV4_REFRESH_AGE_OVERRUN_E                             ), \
STR(CPSS_PP_LX_PCE_PAR_ERR_E                                          ), \
STR(CPSS_PP_LX_TC_2_RF_CNTR_ALRM_E                                    ), \
STR(CPSS_PP_LX_TC_2_RF_PLC_ALRM_E                                     ), \
STR(CPSS_PP_LX_TC_2_RF_TBL_ERR_E                                      ), \
STR(CPSS_PP_LX_CLASSIFIER_HASH_PAR_ERR_E                              ), \
STR(CPSS_PP_LX_FLOW_LKUP_PAR_ERR_E                                    ), \
STR(CPSS_PP_LX_FLOW_KEY_TBL_PAR_ERR_E                                 ), \
STR(CPSS_PP_LX_MPLS_ILM_TBL_PAR_ERR_E                                 ), \
STR(CPSS_PP_LX_MPLS_CNTR_E                                            ), \
STR(CPSS_PP_EB_TCC_E                                                  ), \
/* Network Interface */                                                  \
STR(CPSS_PP_RX_BUFFER_QUEUE0_E                                        ), \
STR(CPSS_PP_RX_BUFFER_QUEUE1_E                                        ), \
STR(CPSS_PP_RX_BUFFER_QUEUE2_E                                        ), \
STR(CPSS_PP_RX_BUFFER_QUEUE3_E                                        ), \
STR(CPSS_PP_RX_BUFFER_QUEUE4_E                                        ), \
STR(CPSS_PP_RX_BUFFER_QUEUE5_E                                        ), \
STR(CPSS_PP_RX_BUFFER_QUEUE6_E                                        ), \
STR(CPSS_PP_RX_BUFFER_QUEUE7_E                                        ), \
STR(CPSS_PP_RX_ERR_QUEUE0_E                                           ), \
STR(CPSS_PP_RX_ERR_QUEUE1_E                                           ), \
STR(CPSS_PP_RX_ERR_QUEUE2_E                                           ), \
STR(CPSS_PP_RX_ERR_QUEUE3_E                                           ), \
STR(CPSS_PP_RX_ERR_QUEUE4_E                                           ), \
STR(CPSS_PP_RX_ERR_QUEUE5_E                                           ), \
STR(CPSS_PP_RX_ERR_QUEUE6_E                                           ), \
STR(CPSS_PP_RX_ERR_QUEUE7_E                                           ), \
STR(CPSS_PP_RX_CNTR_OVERFLOW_E                                        ), \
STR(CPSS_PP_TX_BUFFER_QUEUE_E                                         ), \
STR(CPSS_PP_TX_ERR_QUEUE_E                                            ), \
STR(CPSS_PP_TX_END_E                                                  ), \
                                                                         \
/* Per Port Per lane events */                                           \
STR(CPSS_PP_PORT_LANE_PRBS_ERROR_E                                    ), \
STR(CPSS_PP_PORT_LANE_DISPARITY_ERROR_E                               ), \
STR(CPSS_PP_PORT_LANE_SYMBOL_ERROR_E                                  ), \
STR(CPSS_PP_PORT_LANE_CJR_PAT_ERROR_E                                 ), \
STR(CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E                         ), \
STR(CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E                           ), \
STR(CPSS_PP_PORT_LANE_DETECTED_IIAII_E                                ), \
                                                                         \
/* Per Port PCS events */                                                \
STR(CPSS_PP_PORT_PCS_LINK_STATUS_CHANGED_E                            ), \
STR(CPSS_PP_PORT_PCS_DESKEW_TIMEOUT_E                                 ), \
STR(CPSS_PP_PORT_PCS_DETECTED_COLUMN_IIAII_E                          ), \
STR(CPSS_PP_PORT_PCS_DESKEW_ERROR_E                                   ), \
STR(CPSS_PP_PORT_PCS_PPM_FIFO_UNDERRUN_E                              ), \
STR(CPSS_PP_PORT_PCS_PPM_FIFO_OVERRUN_E                               ), \
                                                                         \
/* Egress Buffer Manager */                                              \
STR(CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E                                   ), \
STR(CPSS_PP_BM_EGRESS_EMPTY_INC_E                                     ), \
STR(CPSS_PP_BM_EGRESS_AGED_PACKET_E                                   ), \
STR(CPSS_PP_BM_EGRESS_MAX_BUFF_REACHED_E                              ), \
STR(CPSS_PP_BM_EGRESS_PORT_RX_BUFFERS_CNT_UNDERRUN_E                  ), \
STR(CPSS_PP_BM_EGRESS_PORT_RX_BUFFERS_CNT_OVERRUN_E                   ), \
STR(CPSS_PP_BM_EGRESS_INVALID_ADDRESS_E                               ), \
STR(CPSS_PP_BM_EGRESS_WRONG_SRC_PORT_E                                ), \
STR(CPSS_PP_BM_EGRESS_MC_INC_OVERFLOW_E                               ), \
STR(CPSS_PP_BM_EGRESS_MC_INC_UNDERRUN_E                               ), \
STR(CPSS_PP_BM_EGRESS_TQ_PARITY_ERROR_PORT_E                          ), \
STR(CPSS_PP_BM_EGRESS_RX_MEM_READ_ECC_ERROR_E                         ), \
STR(CPSS_PP_BM_EGRESS_VLT_ECC_ERR_E                                   ), \
STR(CPSS_PP_BM_EGRESS_MISC_E                                          ), \
                                                                         \
/* PEX events */                                                         \
STR(CPSS_PP_PEX_DL_DOWN_TX_ACC_ERR_E                                  ), \
STR(CPSS_PP_PEX_MASTER_DISABLED_E                                     ), \
STR(CPSS_PP_PEX_ERROR_WR_TO_REG_E                                     ), \
STR(CPSS_PP_PEX_HIT_DEFAULT_WIN_ERR_E                                 ), \
STR(CPSS_PP_PEX_COR_ERROR_DET_E                                       ), \
STR(CPSS_PP_PEX_NON_FATAL_ERROR_DET_E                                 ), \
STR(CPSS_PP_PEX_FATAL_ERROR_DET_E                                     ), \
STR(CPSS_PP_PEX_DSTATE_CHANGED_E                                      ), \
STR(CPSS_PP_PEX_BIST_E                                                ), \
STR(CPSS_PP_PEX_RCV_ERROR_FATAL_E                                     ), \
STR(CPSS_PP_PEX_RCV_ERROR_NON_FATAL_E                                 ), \
STR(CPSS_PP_PEX_RCV_ERROR_COR_E                                       ), \
STR(CPSS_PP_PEX_RCV_CRS_E                                             ), \
STR(CPSS_PP_PEX_PEX_SLAVE_HOT_RESET_E                                 ), \
STR(CPSS_PP_PEX_PEX_SLAVE_DISABLE_LINK_E                              ), \
STR(CPSS_PP_PEX_PEX_SLAVE_LOOPBACK_E                                  ), \
STR(CPSS_PP_PEX_PEX_LINK_FAIL_E                                       ), \
STR(CPSS_PP_PEX_RCV_A_E                                               ), \
STR(CPSS_PP_PEX_RCV_B_E                                               ), \
STR(CPSS_PP_PEX_RCV_C_E                                               ), \
STR(CPSS_PP_PEX_RCV_D_E                                               ), \
                                                                         \
/* Buffer Memory events */                                               \
STR(CPSS_PP_BUF_MEM_MAC_ERROR_E                                       ), \
STR(CPSS_PP_BUF_MEM_ONE_ECC_ERROR_E                                   ), \
STR(CPSS_PP_BUF_MEM_TWO_OR_MORE_ECC_ERRORS_E                          ), \
                                                                         \
/* Policer events */                                                     \
STR(CPSS_PP_POLICER_ADDR_OUT_OF_MEMORY_E                              ), \
STR(CPSS_PP_POLICER_DATA_ERR_E                                        ), \
STR(CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E                               ), \
STR(CPSS_PP_POLICER_IPFIX_ALARM_E                                     ), \
STR(CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E          ), \
                                                                         \
/* Policy Engine events */                                               \
STR(CPSS_PP_PCL_TCC_ECC_ERR_E                                         ), \
STR(CPSS_PP_PCL_MG_ADDR_OUT_OF_RANGE_E                                ), \
STR(CPSS_PP_PCL_LOOKUP_DATA_ERROR_E                                   ), \
STR(CPSS_PP_PCL_ACTION_ERROR_DETECTED_E                               ), \
STR(CPSS_PP_PCL_MG_LOOKUP_RESULTS_READY_E                             ), \
STR(CPSS_PP_PCL_INLIF_TABLE_DATA_ERROR_E                              ), \
STR(CPSS_PP_PCL_CONFIG_TABLE_DATA_ERROR_E                             ), \
STR(CPSS_PP_PCL_LOOKUP_FIFO_FULL_E                                    ), \
                                                                         \
/* Centralized Counters (CNC) events */                                  \
STR(CPSS_PP_CNC_WRAPAROUND_BLOCK_E                                    ), \
STR(CPSS_PP_CNC_DUMP_FINISHED_E                                       ), \
                                                                         \
/* SCT */                                                                \
STR(CPSS_PP_SCT_RATE_LIMITER_E                                        ), \
                                                                         \
/* Egress and Ingress SFLOW (STC) */                                     \
STR(CPSS_PP_EGRESS_SFLOW_E                                            ), \
STR(CPSS_PP_INGRESS_SFLOW_SAMPLED_CNTR_E                              ), \
STR(CPSS_PP_EGRESS_SFLOW_SAMPLED_CNTR_E                               ), \
                                                                         \
/* TTI events */                                                         \
STR(CPSS_PP_TTI_CPU_ADDRESS_OUT_OF_RANGE_E                            ), \
STR(CPSS_PP_TTI_ACCESS_DATA_ERROR_E                                   ), \
                                                                         \
/* DIT events */                                                         \
STR(CPSS_PP_DIT_MC_DIT_TTL_TRHOLD_EXCEED_E                            ), \
STR(CPSS_PP_DIT_UC_DIT_TTL_TRHOLD_EXCEED_E                            ), \
STR(CPSS_PP_DIT_DATA_ERROR_E                                          ), \
STR(CPSS_PP_DIT_MC0_PARITY_ERROR_E                                    ), \
STR(CPSS_PP_DIT_INT_TABLE_ECC_ERROR_CNTR_E                            ), \
STR(CPSS_PP_DIT_MC0_IP_TV_PARITY_ERROR_CNTR_E                         ), \
                                                                         \
/* External TCAM events */                                               \
STR(CPSS_PP_EXTERNAL_TCAM_INT_SUM_E                                   ), \
STR(CPSS_PP_EXTERNAL_TCAM_IO_ERR_E                                    ), \
STR(CPSS_PP_EXTERNAL_TCAM_RX_PARITY_ERR_E                             ), \
STR(CPSS_PP_EXTERNAL_TCAM_MG_CMD_EXE_DONE_E                           ), \
STR(CPSS_PP_EXTERNAL_TCAM_NEW_DATA_IN_MAILBOX_E                       ), \
STR(CPSS_PP_EXTERNAL_TCAM_MG_WRONG_EXP_RES_LEN_E                      ), \
STR(CPSS_PP_EXTERNAL_TCAM_RSP_IDQ_EMPTY_E                             ), \
STR(CPSS_PP_EXTERNAL_TCAM_PHASE_ERROR_E                               ), \
STR(CPSS_PP_EXTERNAL_TCAM_TRAINING_SEQUENCE_DONE_E                    ), \
                                                                         \
/* External Memory events */                                             \
STR(CPSS_PP_EXTERNAL_MEMORY_PARITY_ERROR_E                            ), \
STR(CPSS_PP_EXTERNAL_MEMORY_ECC_ERROR_E                               ), \
STR(CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_PARITY_ERR_LOW_E              ), \
STR(CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_PARITY_ERR_HIGH_E             ), \
STR(CPSS_PP_EXTERNAL_MEMORY_RLDRAM_UNIT_MG_CMD_DONE_E                 ), \
                                                                         \
/* PP VOQ events */                                                      \
STR(CPSS_PP_VOQ_DROP_DEV_EN_E                                         ), \
STR(CPSS_PP_VOQ_GLOBAL_DESC_FULL_E                                    ), \
STR(CPSS_PP_VOQ_ADDR_OUT_OF_RANGE_E                                   ), \
STR(CPSS_PP_VOQ_ECC_ERR_E                                             ), \
STR(CPSS_PP_VOQ_GLOBAL_BUF_FULL_E                                     ), \
                                                                         \
/* PP XBAR events */                                                     \
STR(CPSS_PP_XBAR_INVALID_TRG_DROP_CNTR_RL_E                           ), \
STR(CPSS_PP_XBAR_SRC_FILTER_DROP_CNTR_RL_E                            ), \
STR(CPSS_PP_XBAR_RX_CELL_CNTR_RL_E                                    ), \
STR(CPSS_PP_XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL_E                       ), \
STR(CPSS_PP_XBAR_SHP_TX_DROP_CNTR_RL_E                                ), \
STR(CPSS_PP_XBAR_RX_FIFO_OVERRUN_CNTR_RL_E                            ), \
STR(CPSS_PP_XBAR_RX_FIFO_OVERRUN_CNTR_INC_E                           ), \
STR(CPSS_PP_XBAR_INVALID_TRG_DROP_CNTR_INC_E                          ), \
STR(CPSS_PP_XBAR_SRC_FILTER_DROP_CNTR_INC_E                           ), \
STR(CPSS_PP_XBAR_RX_CELL_CNTR_INC_E                                   ), \
STR(CPSS_PP_XBAR_PRIORITY_0_3_TX_DROP_CNTR_INC_E                      ), \
STR(CPSS_PP_XBAR_SHP_TX_DROP_CNTR_INC_E                               ), \
STR(CPSS_PP_XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF_E                 ), \
STR(CPSS_PP_XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON_E                  ), \
STR(CPSS_PP_XBAR_INVALID_ADDR_E                                       ), \
STR(CPSS_PP_XBAR_TX_CELL_CNTR_RL_E                                    ), \
STR(CPSS_PP_XBAR_TX_CELL_CNTR_INC_E                                   ), \
                                                                         \
/* PP Fabric Reassembly engine events */                                 \
STR(CPSS_PP_CRX_MAIL_ARRIVED_E                                        ), \
STR(CPSS_PP_CRX_READ_EMPTY_CPU_E                                      ), \
STR(CPSS_PP_CRX_DATA_DROP_E                                           ), \
STR(CPSS_PP_CRX_CPU_DROP_E                                            ), \
STR(CPSS_PP_CRX_AGED_OUT_CONTEXT_E                                    ), \
STR(CPSS_PP_CRX_CRC_DROP_E                                            ), \
STR(CPSS_PP_CRX_PACKET_LENGTH_DROP_E                                  ), \
STR(CPSS_PP_CRX_MAX_BUFFERS_DROP_E                                    ), \
STR(CPSS_PP_CRX_BAD_CONTEXT_DROP_E                                    ), \
STR(CPSS_PP_CRX_NO_BUFFERS_DROP_E                                     ), \
STR(CPSS_PP_CRX_MULTICAST_CONGESTION_DROP_E                           ), \
STR(CPSS_PP_CRX_DATA_FIFO_UNDERRUN_E                                  ), \
STR(CPSS_PP_CRX_DATA_FIFO_OVERRUN_E                                   ), \
STR(CPSS_PP_CRX_CPU_UNDERRUN_E                                        ), \
STR(CPSS_PP_CRX_CPU_OVERRUN_E                                         ), \
STR(CPSS_PP_CRX_E2E_FIFO_UNDERRUN_E                                   ), \
STR(CPSS_PP_CRX_E2E_FIFO_OVERRUN_E                                    ), \
STR(CPSS_PP_CRX_UNKNOWN_CELL_TYPE_E                                   ), \
STR(CPSS_PP_CRX_COUNTER_BAD_ADDR_E                                    ), \
STR(CPSS_PP_CRX_PSM_BAD_ADDR_E                                        ), \
                                                                         \
/* PP Fabric Segmentation engine events */                               \
STR(CPSS_PP_TXD_SEG_FIFO_PARITY_ERR_E                                 ), \
STR(CPSS_PP_TXD_CPU_MAIL_SENT_E                                       ), \
STR(CPSS_PP_TXD_SEG_FIFO_OVERRUN_E                                    ), \
STR(CPSS_PP_TXD_WRONG_CH_TO_FABRIC_DIST_E                             ), \
                                                                         \
/* IPVX events */                                                        \
STR(CPSS_PP_IPVX_DATA_ERROR_E                                         ), \
STR(CPSS_PP_IPVX_ADDRESS_OUT_OF_MEMORY_E                              ), \
STR(CPSS_PP_IPVX_LPM_DATA_ERROR_E                                     ), \
                                                                         \
/* Statistic Interface */                                                \
STR(CPSS_PP_STAT_INF_TX_SYNC_FIFO_FULL_E                              ), \
STR(CPSS_PP_STAT_INF_TX_SYNC_FIFO_OVERRUN_E                           ), \
STR(CPSS_PP_STAT_INF_TX_SYNC_FIFO_UNDERRUN_E                          ), \
STR(CPSS_PP_STAT_INF_RX_SYNC_FIFO_FULL_E                              ), \
STR(CPSS_PP_STAT_INF_RX_SYNC_FIFO_OVERRUN_E                           ), \
STR(CPSS_PP_STAT_INF_RX_SYNC_FIFO_UNDERRUN_E                          ), \
                                                                         \
/* PP HyperG Link ports events */                                        \
STR(CPSS_PP_HGLINK_PING_RECEIVED_E                                    ), \
STR(CPSS_PP_HGLINK_PING_SENT_E                                        ), \
STR(CPSS_PP_HGLINK_MAC_TX_OVERRUN_E                                   ), \
STR(CPSS_PP_HGLINK_MAC_TX_UNDERRUN_E                                  ), \
STR(CPSS_PP_HGLINK_RX_ERR_CNTR_RL_E                                   ), \
STR(CPSS_PP_HGLINK_BAD_PCS_TO_MAC_REFORMAT_RX_E                       ), \
STR(CPSS_PP_HGLINK_BAD_HEAD_CELL_RX_E                                 ), \
STR(CPSS_PP_HGLINK_BAD_LENGTH_CELL_RX_E                               ), \
STR(CPSS_PP_HGLINK_FC_CELL_RX_INT_E                                   ), \
STR(CPSS_PP_HGLINK_FC_CELL_TX_INT_E                                   ), \
STR(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC0_E                            ), \
STR(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC1_E                            ), \
STR(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC2_E                            ), \
STR(CPSS_PP_HGLINK_FC_XOFF_DEAD_LOCK_TC3_E                            ), \
STR(CPSS_PP_HGLINK_ADDR_OUT_OF_RANGE_E                                ), \
                                                                         \
/* TCC-TCAM */                                                           \
STR(CPSS_PP_TCC_TCAM_ERROR_DETECTED_E                                 ), \
STR(CPSS_PP_TCC_TCAM_BIST_FAILED_E                                    ), \
                                                                         \
/* BCN */                                                                \
STR(CPSS_PP_BCN_COUNTER_WRAP_AROUND_ERR_E                             ), \
                                                                         \
/* Xbar events */                                                        \
STR(CPSS_XBAR_HG_LANE_SYNCH_CHANGE_E                                  ), \
STR(CPSS_XBAR_HG_LANE_COMMA_ALIGN_CHANGE_E                            ), \
STR(CPSS_XBAR_GPORT_INVALID_TRG_DROP_CNT_INC_E                        ), \
STR(CPSS_XBAR_GPORT_DIAG_CELL_RX_CNTR_INC_E                           ), \
STR(CPSS_XBAR_MISC_E                                                  ), \
STR(CPSS_XBAR_HG_PING_RECEIVED_E                                      ), \
STR(CPSS_XBAR_HOST_RX_FIFO_PEND_E                                     ), \
STR(CPSS_XBAR_HOST_SEND_CELL_E                                        ), \
STR(CPSS_XBAR_HG_ALIGNMENT_LOCK_CHANGE_E                              ), \
STR(CPSS_XBAR_HG_PING_E                                               ), \
STR(CPSS_XBAR_SD_INV_ADDR_E                                           ), \
STR(CPSS_XBAR_SXBAR_INV_ADDR_E                                        ), \
STR(CPSS_XBAR_S_INV_ADDR_E                                            ), \
STR(CPSS_XBAR_G_INV_ADDR_E                                            ), \
STR(CPSS_XBAR_MC_INV_ADDR_E                                           ), \
                                                                         \
/* Fa events */                                                          \
STR(CPSS_FA_UC_PRIO_BUF_FULL_E                                        ), \
STR(CPSS_FA_MC_PRIO_BUF_FULL_E                                        ), \
STR(CPSS_FA_UC_RX_BUF_FULL_E                                          ), \
STR(CPSS_FA_MC_RX_BUF_FULL_E                                          ), \
STR(CPSS_FA_WD_FPORT_GRP_E                                            ), \
STR(CPSS_FA_MC_WD_EXP_E                                               ), \
STR(CPSS_FA_GLBL_DESC_FULL_E                                          ), \
STR(CPSS_FA_MAC_UPD_THRS_E                                            ), \
STR(CPSS_FA_DROP_DEV_EN_E                                             ), \
STR(CPSS_FA_MISC_I2C_TIMEOUT_E                                        ), \
STR(CPSS_FA_MISC_STATUS_E                                             ), \
                                                                         \
/* DRAGONITE events */                                                         \
STR(CPSS_DRAGONITE_PORT_ON_E                                                ), \
STR(CPSS_DRAGONITE_PORT_OFF_E                                               ), \
STR(CPSS_DRAGONITE_DETECT_FAIL_E                                            ), \
STR(CPSS_DRAGONITE_PORT_FAULT_E                                             ), \
STR(CPSS_DRAGONITE_PORT_UDL_E                                               ), \
STR(CPSS_DRAGONITE_PORT_FAULT_ON_STARTUP_E                                  ), \
STR(CPSS_DRAGONITE_PORT_PM_E                                                ), \
STR(CPSS_DRAGONITE_POWER_DENIED_E                                           ), \
STR(CPSS_DRAGONITE_OVER_TEMP_E                                              ), \
STR(CPSS_DRAGONITE_TEMP_ALARM_E                                             ), \
STR(CPSS_DRAGONITE_DEVICE_FAULT_E                                           ), \
STR(CPSS_DRAGONITE_OVER_CONSUM_E                                            ), \
STR(CPSS_DRAGONITE_VMAIN_LOW_AF_E                                           ), \
STR(CPSS_DRAGONITE_VMAIN_LOW_AT_E                                           ), \
STR(CPSS_DRAGONITE_VMAIN_HIGH_E                                             ), \
STR(CPSS_DRAGONITE_READ_EVENT_E                                             ), \
STR(CPSS_DRAGONITE_WRITE_EVENT_E                                            ), \
STR(CPSS_DRAGONITE_ERROR_E                                                  )

/*******************************************************************************
* PRV_CPSS_EVENT_MASK_DEVICE_SET_FUNC
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                     queue number / other ) the event was received upon.
*                   may use value PRV_CPSS_DRV_EV_REQ_UNI_EV_EXTRA_DATA_ANY_CNS
*                   to indicate 'ALL interrupts' that relate to this unified
*                   event
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event or the evExtData within the unified
*                      event are not found in this device interrupts
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       This function called when OS interrupts and the ExtDrv interrupts are
*       locked !
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_EVENT_MASK_DEVICE_SET_FUNC)
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData,
    IN CPSS_EVENT_MASK_SET_ENT  operation
);


/*******************************************************************************
* prvCpssGenEventMaskDeviceBind
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       deviceType - device Type - PP/FA/Xbar device
*       funcPtr    - pointer to function that will be called when the
*                    Application wants to mask/unmask events relate to the
*                    deviceType
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad deviceType value or bad device number
*       GT_BAD_PTR  - funcPtr is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenEventMaskDeviceBind
(
    IN PRV_CPSS_DEVICE_TYPE_ENT    deviceType,
    IN PRV_CPSS_EVENT_MASK_DEVICE_SET_FUNC funcPtr
);

/*******************************************************************************
* PRV_CPSS_EVENT_GPP_ISR_CONNECT_FUNC
*
* DESCRIPTION:
*       This function connects an Isr for a given Gpp interrupt.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR) with GPP
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is connected.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     reception.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       To disconnect a Gpp Isr, call this function with a NULL parameter in
*       the isrFuncPtr param.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_EVENT_GPP_ISR_CONNECT_FUNC)
(
    IN  GT_U8                   devNum,
    IN  CPSS_EVENT_GPP_ID_ENT   gppId,
    IN  CPSS_EVENT_ISR_FUNC     isrFuncPtr,
    IN  void                    *cookie
);

/*******************************************************************************
* prvCpssGenEventGppIsrConnectBind
*
* DESCRIPTION:
*       This function binds function that connects an Isr for a given Gpp
*       interrupt.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR) with GPP
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is connected.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     reception.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PTR  - funcPtr is NULL pointer
*
* COMMENTS:
*       To disconnect a Gpp Isr, call this function with a NULL parameter in
*       the isrFuncPtr param.
*
*******************************************************************************/
GT_STATUS prvCpssGenEventGppIsrConnectBind
(
    IN PRV_CPSS_EVENT_GPP_ISR_CONNECT_FUNC funcPtr
);

/*******************************************************************************
* prvCpssGenEventRequestsDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of the event related.
*       NOTE: function not 'free' the allocated memory. only detach from it ,
*             and restore DB to 'pre-init' state
*
*             The assumption is that the 'cpssOsMalloc' allocations will be not
*             valid any more by the application , so no need to 'free' each and
*             every allocation !
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenEventRequestsDbRelease
(
    void
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenEvReqh */

