/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChErrataMng.h
*
* DESCRIPTION:
*       DxCh manager for the Errata issues.
*
*
* FILE REVISION NUMBER:
*       $Revision: 35 $
*
*******************************************************************************/

#ifndef __prvCpssDxChErrataMngh
#define __prvCpssDxChErrataMngh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/* macro for references purpose only -- must be 0 --> never 1 */
#define PRV_CPSS_DXCH_ERRATA_REFERENCE_CNS  0

/* number GT_U32 bmps needed to hold the known FEr errata bmp */
#define PRV_CPSS_DXCH_ERRATA_BMP_MAX_NUM_CNS \
                (1 + (((PRV_CPSS_DXCH_ERRATA_MAX_NUM_E)+1) >> 5))

/* get if the erratum need to be WA by CPSS
    devNum - the device id of the DxCh device
    FErNum -- id of the erratum
*/
#define PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,FErNum) \
  (((PRV_CPSS_DXCH_PP_MAC(devNum))->errata.FErArray[(FErNum)>>5] &(1<<((FErNum)&0x1f)))? \
      GT_TRUE : GT_FALSE)

/* set that the erratum need to be WA by CPSS
    devNum - the device id of the DxCh device
    FErNum -- 1 based number
*/
#define PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,FErNum) \
    (PRV_CPSS_DXCH_PP_MAC(devNum))->errata.FErArray[(FErNum)>>5] |= (1<<((FErNum)&0x1f))

/* set that the erratum NOT need to be WA by CPSS
    devNum - the device id of the DxCh device
    FErNum -- 1 based number
*/
#define PRV_CPSS_DXCH_ERRATA_CLEAR_MAC(devNum,FErNum) \
    (PRV_CPSS_DXCH_PP_MAC(devNum))->errata.FErArray[(FErNum)>>5] &= ~(1<<((FErNum)&0x1f))

/**
 * Typedef: enum PRV_CPSS_DXCH_ERRATA_ENT
 *
 * Description: enumerator for the numbers of Erratum that handles in the CPSS
 *              code
 *
 * Fields:
 *  PRV_CPSS_DXCH_FDB_AGING_WITH_REMOVAL_MODE_WA_E - In FDB Aging with removal
 *              mode, AA messages are sent for entries removed due to an
 *              invalid device.
 *              The CPU should ignore those messages, after ensuring that the
 *              DeviceId is invalid
 *              (FEr#22)
 *  PRV_CPSS_DXCH_TRUNK_CONFIG_FOR_TRUNK_NEXT_HOP_WA_E - When the Next Hop is a
 *              trunk, the router sets the TrunkHash in the descriptor to
 *              PktHash[2:0]. This means that the value of the TrunkHash may be
 *              set to any value: 0 through 7.
 *              If the Next Hop is a trunk with less than 8 members,
 *              the device may fetch an invalid trunk member.
 *              (FEr#137)
 *  PRV_CPSS_DXCH_INCORRECT_SRCDEV_FIELD_IN_MAILBOX_TO_CPU_DSA_TAG_WA_E -
 *              by Copy The mailbox mechanism enables implementation of a
 *              topology discovery algorithm in a stacking system.
 *              A FROM_CPU DSA-tagged packet with a mailbox indication is
 *              reformatted as a TO_CPU DSA-tagged packet and is forwarded
 *              to the host CPU.
 *              Unlike other TO_CPU DSA-tagged packets, in which the SrcPort and
 *              SrcDev fields preserve their value as it was originally set in
 *              the DSA tag, in a mailbox TO_CPU DSA tag, the SrcPort is set to
 *              the local device Source port number (at which the FROM_CPU DSA-
 *              tagged packet with a mailbox indication was received) and the
 *              SrcDev field is set to the local device number. However, due to
 *              this erratum, the SrcDev field is not set to the local device
 *              number.
 *              (FEr#6)
 *  PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E  -
 *              Disabling the Egress Rate Shaper under traffic may hang its
 *              relevant transmit queue.
 *              To disable the Egress Rate Shaper follow the following procedure:
 *              1. Set MaxBucketSize to 0xFFF, Tokens to 0xFFF, SlowRateEn to 0.
 *              2. Set TokenBucketEn to 0.
 *              (FEr#47)
 *  PRV_CPSS_DXCH_JUMBO_FRAMES_MODE_SDWRR_WEIGHT_LIMITATION_WA_E  -
 *              The transmit queue Shaped Deficit Weighted Round Robin (SDWRR)
 *              weight may be configured from 0 through 255,
 *              corresponding to a weight of 1 through 256 respectively.
 *              However, due to this erratum, in Jumbo frames mode the maximal
 *              weight that may be configured is 254.
 *              (FEr#29)
 *  PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E -
 *              All packets forwarded to the CPU are forwarded with a
 *              TO_CPU DSA tag. One of the fields in this tag is <PktOrigBC>.
 *              However, when a packet forwarded to the CPU is from another
 *              device, it gets the packet's byte count as it enters the second
 *              device and not the original byte count of the packet that was
 *              received on the first device. The <PktOrigBC> will be either
 *              four or eight bytes larger than the received packet's byte count,
 *              according to the DSA tag that was added.
 *              (FEr#89)
 *  PRV_CPSS_DXCH_RATE_LIMIT_GIG_COUNTER_BITS_NUM_WA_E -
 *              When the value of the 1000MWindow field, in the Ingress Rate
 *              limit Configuration Register0 is higher than 31, the rate limit
 *              does not work properly.
 *              The byte/packets counter of the rate limit (in ports that were
 *              rate limit enabled) never return to zero, causing the bridge to
 *              drop all traffic from these ports.
 *              (FEr#140)
 *  PRV_CPSS_DXCH_XG_MIB_READ_OVERSIZE_AND_JABBER_COUNTERS_WA_E -
 *              Counters 24 (oversize) and 25 (jabber) in XG MIBs, which are
 *              placed in one entry, are treated as one long 64-bit counter.
 *              Therefore, when any one of them is read, counter 24 will always
 *              be accessed. Any additional read, even for another counter,
 *              will then access counter 25.
 *              (FEr#85)
 *  PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E -
*              in some cases when performing scanning operations and the AUQ is
*              full the learning operation is also stalled.
*              The reason is:
*              the learning and scanning machines share an access arbiter.
*              In order to insure data coherency in the FDB only a one of
*              the clients s allowed to access the FDB at any given time.
*              An access being with a read and ends with a write and only
*              then is the arbitration changed.
*              In the scenario you saw, since the scanning machine is
*              waiting for the AUQ to become available, it does not release
*              the access arbiter.
*              The learning machine in this case is stalled as well and is
*              waiting for the scanning machine to release the access arbiter.
 *  PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E -
 *              To assign non-DSA tagged packets with a source-ID according to
 *              the port default configuration (ignoring the source FDB entry
 *              Source-ID assignment), set the corresponding port bit in the
 *              field in the Source-ID Assignment Mode Configuration register.
 *              However, due to this erratum, when reading from this register,
 *              wrong data is returned.
 *              (FEr#1084)
 *   PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E  -
 *              Bridge Access Matrix Line <7> register is write only.
 *              Shadow of the register should be used by SW.
 *              (FEr#1006)
 *   PRV_CPSS_DXCH2_POLICY_ACTION_DIRECT_BURST_ONLY_READ_WA_E -
 *              Direct access and indirect access to tables is supported
 *              by the device. However, due to this erratum, direct access
 *              to the Policy Action entry table,
 *              (address space: 0x0B8C0000 - 0x0B8C3FFC) is not supported
 *              for single access. (burst only). (FEr#1090)
 *
 *   PRV_CPSS_DXCH2_DIRECT_ACCESS_TO_POLICER_ADDRESS_SPACE_WA_E -
 *              Direct access to policer engine address space is not functional
 *              WA : The policer engine registers and tables, address space,
 *              0x0C000000 - 0x0C800000, must be accessed by using the indirect
 *              access mechanism, as specified in the device's Functional
 *              Specifications Doc. No. MV-S103086-00.
 *              (FEr#1001)
 *   PRV_CPSS_DXCH2_ROUTER_ARP_AND_TS_TABLE_NOT_SUPPORT_BURST_ACCESS_VIA_PCI_WA_E -
 *              Router ARP and Tunnel Start table do not support burst access via PCIõ
 *              The device supports burst reads from its internal memory address
 *              space. However, due to this erratum, the Router Address
 *              Resolution Protocol (ARP) and Tunnel Start (TS) table,
 *              0x07E80000 - 0x07E87FF4, cannot be accessed by using the burst
 *              access feature.
 *              WA : Access to the Router ARP and TS table must done by single
 *              32-bit read/write access.
 *              (FEr#1003)
 *   PRV_CPSS_DXCH2_RDWRTRIG_BIT_IN_VLT_TABLES_ACCESS_CONTROL_REGISTER_IGNORED_E -
 *              RdWrTrig, bit[15] in the VLT Tables Access Control register is ignored.
 *              When the RdWrTrig field, bit[15] in the VLT Tables Access
 *              Control register (offset: 0x0A000010) is set to 1, a read or
 *              write command is performed. After the action is completed, this
 *              bit is cleared. However, due to this erratum, a read or write
 *              command is performed for every write access to the register,
 *              regardless of the value of the RdWrTrig field.
 *              WA : No dummy write commands should be done to the VLT Tables
 *              Access Control register (offset: 0x0A000010).
 *              (FEr#1007)
 *   PRV_CPSS_DXCH2_INDIRECT_ACCESS_ROUTER_AND_TT_TCAM_NOT_FUNCTION_CORRECTLY_WA_E -
 *              Indirect access for Router and Tunnel Termination (TT) TCAM does
 *              not function correctly.
 *              The device supports indirect access to the Router and Tunnel
 *              Termination (TT) table. Valid bits should be written to the
 *              arrays according to their location: Valid[0] to Array0, Valid[1]
 *              to Array1, etc. However, due to this erratum, this does not
 *              function correctly. So when writing to one array, set all bits
 *              to 1 to validate the entry, or all bits to 0 to invalidate the
 *              entry. Do the same for writing to all arrays in parallel.
 *              WA: When using the indirect access mechanism to access the Router
 *              and Tunnel termination (TT) table bits[4:0], the Valid field,
 *              bits[4:0] in the Router and TT TCAM Access Data Control register
 *              (offset: 0x02800418), must be set to the same value, either 0x0
 *              to invalidate or 0x1F to validate.
 *              An additional option is to use the direct access mechanism to
 *              access the Router and Tunnel termination (TT) table.
 *              (FEr#1062)
 *   PRV_CPSS_DXCH_RM_XG_PHY_PATH_FAILURE_WA_E -
 *              XG PHY path failure.Wrong value of PHY register 0xFF34.
 *              WA: The following register write sequence should be performed
 *              after power-up, reset or after XG PHY power-up: FF00, FF00, and
 *              E000 to the PHY register 0xFF34.
 *              (RM#83)
 *   PRV_CPSS_DXCH2_RM_RESERVED_REGISTERS_0x0B820108_WA_E -
 *              Reserved registers 0x0B820108.Wrong value of reserved register
 *              0x0B820108.
 *              WA: Set the value 0xB80 to register 0x0B820108.
 *              (RM#1081)
 *  PRV_CPSS_DXCH_RM_FDB_TABLE_ENTRIES_WA_E -
 *              Some FDB table entries must be invalidated (RM#28)
 *              The FDB table has 16K entries. But half of FDB need
 *              to be invalidated by an application.
 *  PRV_CPSS_DXCH2_PCL_EGRESS_ACTION_CNTR_VLAN_WRONG_BITS_POS_WA_E -
 *              The position of Vlan modification and of Match Counter
 *              fields is different from described (and from Egress PCL action)
 *              (FEr#1087)
 *  PRV_CPSS_DXCH3_VLT_INDIRECT_ACCESS_WA_E -
 *              VLT Tables Indirect Access is not functional.
 *              (FEr#2006)
 *  PRV_CPSS_DXCH3_SAMPLE_AT_RESET_NET_REF_CLOCK_SELECT_WA_E -
 *              Sampled at Reset Network reference clock select is not functional
 *              (FEr#2014)
 *  PRV_CPSS_DXCH3_RM_BM_GE_XG_PORT_GROUP_LIMIT_CFG_REG_WA_E -
 *              Buffer Management GigE Ports Group Limits Configuration Register
 *              (Offset: 0x03000004) and
 *              Buffer Management HyperG.Stack Ports Group Limits Configuration
 *              Register (Offset: 0x03000008)
 *              default values are incorrect
 *              (RM#2003)
 *  PRV_CPSS_DXCH3_RM_GE_PORTS_MODE1_REG_WA_E -
 *              LMS GigE Ports Mode Register1 (Offset: 0x05804144)
 *              default values are incorrect.
 *              (RM#2007)
 *  PRV_CPSS_DXCH3_RM_XG_PORTS_MODE_REG_WA_E -
 *              LMS HyperG.Stack Ports Mode Register (Offset: 0x84804144)
 *              default value is incorrect.
 *              (RM#2008)
 *  PRV_CPSS_DXCH3_RM_TCAM_PLL_REG_WA_E -
 *              The TCAM PLL Register (Offset: 0x000000BC) default values
 *              are incorrect.
 *              (RM#2011)
 *               FEr#2012: TCAM/CORE/MPPM clock frequencies above 250 MHz
 *               are not operational
 *  PRV_CPSS_DXCH3_RM_LMS0_1_GROUP1_LED_INTERFACE_WA_E -
 *              LMS0/1 Group1LED Interface0 Control Register 1
 *              and Class6 Manipulation Register default values are incorrect.
 *              (RM#2024)
 *  PRV_CPSS_DXCH3_RM_MAC2ME_LOW_REG_WA_E -
 *              MAC2ME Registers Low default values are incorrect.
 *              (RM#2026)
 *  PRV_CPSS_DXCH3_RM_AN_CNF_REG_WA_E -
 *              PHY Auto-Negotiation Configuration Register0
 *              and PHY Auto-Negotiation Configuration Register2
 *              default values are incorrect.
 *              (RM#2061)
 *  PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E -
 *              The direct access to Pre-Egress Engine (EQ) tables for both
 *              reads and writes, under traffic is not functional. Pre-Egress
 *              Engine (EQ) tables include; CPU Code Table, Ingress STC Table,
 *              QoSProfile to QoS Table, Statistical Rate Limits Table, TO CPU
 *              Packet Rate Limiters, and Trunks Members Table.
 *              WA: The Pre-Egress Engine (EQ) tables address space must be
 *              accessed for read or for write by using indirect access only.
 *              (FEr#2028)
 *  PRV_CPSS_DXCH3_TXQ_PARITY_CALCULATION_E -
 *              TxQ Parity calculation is wrong for Descriptors & FLL memories.
 *              The TxQ calculates parity on the FLL RAM & Desciptor RAM,
 *              but calculates it on a different number of bits used when
 *              writing the data to the RAMs.
 *              This causes parity error on almost every packet reaching the TxQ.
 *              WA: interrupts ParityErrorIn TxqPtrMem FllRamParity and
 *              ParityErrorIn TxqDescMem in the Transmit Queue General Interrupt
 *              Cause register must be masked
 *              (FEr#204)
 *  PRV_CPSS_DXCH3_SDMA_WA_E -
 *              SDMA resource errors may cause PCI Express packets reception
 *              malfunction.
 *              When using the PCI Express host management interface
 *              for forwarding packets to/from the host CPU, the
 *              CPU receives packets from the device directly to
 *              a pre-allocated space in the host memory. To
 *              receive packets for a given CPU traffic class
 *              queue, the CPU prepares a linked list of
 *              descriptors located in the host memory. A
 *              resource error event occurs when it reaches the
 *              end of available descriptors in the linked list.
 *              This resource error occurs if either: The
 *              current descriptor for the Rx SDMA has a NULL
 *              next-descriptor pointer. OR The SDMA has reached
 *              a descriptor that is owned by the CPU (i.e., a
 *              cyclic descriptor list is implemented). This may
 *              happen at the start of a packet or in the middle
 *              of writing a packet to host memory (if it
 *              occupies more than a single descriptor buffer).
 *              This may also happen due to a speculative
 *              pre-fetch of a descriptor (i.e, without any
 *              current packet destined to the given queue). The
 *              device terminates packet writing to the host
 *              memory if it reaches the end of the linked list
 *              before completion of packet reception.Functional
 *              Impact SDMA resource errors may cause a PCI
 *              Express packets reception malfunction. WA:
 *              Configure the SDMA to drop packets upon resource
 *              errors, by setting to 1 the Retry AbortTC<
 *              traffic class queue> fields in the SDMA
 *              Configuration Register
 *   PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E -
 *              When the CPU reads the POLICY TCAM or ROUTER TCAM
 *              via direct address space the ECC error exception signaled.
 *              WA is to mask the interrupt for the CPU read time and to restore
 *              (unmask) it for all other time.
 *              (FEr#2033)
 *
 *              ECC check fails on reading part of PCL/Router TCAM row
 *              The PCL/Router TCAM is used by the PCL/Router engine for IPv4/6
 *              Unicast/Multicast lookups, and is also used by the tunnel
 *              termination/interface lookup.
 *              The PCL/Router TCAM is organized as rows, where each row contains
 *              four arrays of data; Array0, Array1, Array2, and Array3.
 *              PCL/Router TCAM on-chip table memory has built-in data integrity
 *              by Error Correction Code (ECC).
 *              However, due to this erratum, data returns on indirect read to
 *              part of a row (not all 4 arrays) or direct read containing
 *              incorrect ECC cause the ECC check to fail.
 *              WA : To indirectly read PCL/Router TCAM data part of a row
 *              (not all 4 arrays), or to read directly, perform the following:
 *              1. Mask the ECC interrupt.
 *              2. Read PCL/Router TCAM data.
 *              3. Read the Cause register and clear the interrupt.
 *              4. Unmask the ECC interrupt.
 *              (FEr#2050)
 *
 *           -- NOTE: must fill info in
 *              PRV_CPSS_DXCH_ERRATA_STC::info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E
 *              regarding the interrupt register and index , that a device
 *              family that needs this WA should set according to it's specific
 *              values.
 *
 *  PRV_CPSS_DXCH3_OUT_OF_PROFILE_QOS_ATTR_E -
 *              If the QoS Remarking Control En Modify DSCP or the En Modify UP
 *              are set in the Metering table entry for a Yellow or
 *              Red non-conforming packet, these QoS attributes will be changed
 *              regardless of the non-conforming Yellow or Red packet command.
 *              WA: Enable modification of UP/DSCP for non-conforming packets
 *              when <Policer Cmd> = 2 (QoS Profile Mark by table)
 *              (FEr#2046)
 * PRV_CPSS_DXCH3_TXQ_FULL_INTERRUPT_NOT_FUNCTION_WA_E -
 *              An Interrupt Cause register and an Interrupt Mask register are
 *              defined for each functional block. However, due to this erratum,
 *              - A write to the Transmit Queue Flush Interrupt Mask register
 *                (offset: 0x01800114), also causes a write to the Transmit
 *                Queue Full Interrupt Mask register (offset: 0x018001EC).
 *              - A write to the Transmit Queue Full Interrupt Mask register
 *                (offset: 0x018001EC) does not function.
 *              WA: disable get of TXQ Full interrupts in the Transmit Queue
 *              Interrupt Summary Mask register - 0x01800104
 *              (FEr#1098)
 * PRV_CPSS_DXCH3_RM_GE_SERDES_MISC_CONF_REG_WA_E -
 *              Network Port SERDES Misc Conf Register default values are incorrect.
 *              WA: Use the EEPROM or CPU to change the default value of Network
 *              Port SERDES Misc Conf Register: 0x0000000D at addresses
 *              0x09800014 + n*0x400: where n (0-23) represents Port.
 *              (RM#2062)
 * PRV_CPSS_DXCH3_RM_BM_TX_FIFO_THRESHOLD_CONF_REG_WA_E -
 *              Buffer Memory Tx Fifo Threshold and MPPM Access Arbiter
 *              Configuration Register default value of GigTxFifoThreshold is incorrect.
 *              WA: Use the EEPROM or CPU to change the default value of
 *              Buffer Memory Tx Fifo Threshold and MPPM Access Arbiter
 *              Configuration Register:0x444e610c at address 0x0780006C.
 *              (RM#2063)
 *              used only on ch3 XG devices
 * PRV_CPSS_DXCH3_TCAM_EFUSE_NOT_TRIGGERED_AUTO_WA_E -
 *              The device incorporates Policy TCAM and Router TCAM on-chip memories that
 *              contain an eFUSE mechanism. When the device is reset, the eFUSE mechanism
 *              replacement operation is not automatically activated.
 *              (FEr#2191)
 * PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E -
 *              Policy and Router TCAM Tables indirect read is not functional.
 *              Direct access and indirect access to tables are supported by the device.
 *              However, due to this erratum, the indirect read from the
 *              Policy and Router TCAM tables address space is not supported.
 *              Functional Impact :
 *              The indirect read from the Policy and Router TCAM tables address
 *              space is not functional.
 *              WA : The Policy and Router TCAM tables address space must be
 *              accessed for read by using the direct access only.
 *              (FEr#2004)
 *  PRV_CPSS_DXCH_RM_FDB_GLOBAL_CONF_REG_WA_E -
 *               Wrong default in FDB Global Configuration register
 *
 * PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E -
 *              for CH3 :
 *              The device supports IP Longest Prefix Match based on the Policy TCAM
 *              and policy-based routing based on the Router TCAM.
 *              However, due to this erratum, the ingress policy action can assign a
 *              router Lookup Translation Table (LTT) pointer only in the range of first column.
 *              TCAM based routing is operational at full range.
 *              Functional Impact :
 *              The ingress policy action can assign a  router Lookup Translation Table (LTT)
 *              pointer only in the range of first column.
 *              WA : None.
 *              (FEr#2018)
 *              for XCAT/LION :
 *              Policy base routing LTT index can be written only to array0 of the LTT line.
 *              The Lookup Translation Table (LTT) is build from 4 entries in a line.
 *              When using the LTT for policy based routing, the index that recieved from the
 *              PCL/TTI used by the LTT as a line number, when the array number if always 0.
 *              Functional Impact :
 *              Thus, this feature uses only 25% of the capability of the LTT table.
 *              WA : When the application enter LTT index X in the PCL/TTI action, the actual
 *              value that will be written is divided by 4.
 *             (BTS#525)
 *  PRV_CPSS_DXCH_XCAT_TABLE_SIZE_FDB_16K_INSTEAD_8K_WA_E -
 *              The device has 16K FDB entries instead of 8K entries
  *  PRV_CPSS_DXCH_XCAT_TABLE_SIZE_NH_4K_INSTEAD_8K_WA_E
 *              The device has 4K NH (next hops) entries instead of 8K entries
 *  PRV_CPSS_DXCH_XCAT_TABLE_SIZE_TS_1K_INSTEAD_2K_WA_E
 *              The device has 1K TS (tunnel start) entries instead of 2K entries
 *  PRV_CPSS_DXCH_XCAT_TABLE_CNC_NUM_BLOCKS_1_INSTEAD_2_WA_E
 *              The device has single (1) CNC block instead of 2 blocks
 *  PRV_CPSS_DXCH_XCAT_TABLE_POLICERS_NUM_1K_INSTEAD_2K_WA_E -
 *              The device has 1K policer meter entries (and another 1K for counting)
 *              instead of 2K entries flex for metering/counting
 *  PRV_CPSS_DXCH_XCAT_FDB_AGING_TIMEOUT_WA_E -
 *              The device support aging time with granularity of 20 seconds
 *              instead of 10 seconds.
 *  PRV_CPSS_DXCH_XCAT_TR101_WA_E -
 *              Bobcat device revision A0 does not support TR101 feature.
 * PRV_CPSS_DXCH_XCAT_IPLR_SECOND_STAGE_SUPPORT_WA_E
 *              Bobcat device revision A0 does not support
 *              Policer Ingress second stage support flag feature.
 * PRV_CPSS_DXCH_XCAT_SGMII_MODE_ON_STACKING_PORTS_WA_E -
 *              The SGMII mode on stacking ports is working intermittently
 * PRV_CPSS_DXCH_XCAT_RM_TOTAL_BUFFER_LIMIT_CONF_IN_TXQ_E -
 *              Wrong default value in Total Buffer Limit Configuration in TxQ.
 * PRV_CPSS_DXCH_XCAT_RM_CNC_ENABLE_COUNTING_E -
 *              Wrong default values in Counting Mode Configuration of IPCL and
 *              in TTI Unit Global Configuration of TTI.
 *              The "Enable Counting" should be set by default.
 * PRV_CPSS_DXCH_XCAT_RM_TXQ_EXT_CNTR_REG_WA_E -
 *              Wrong default value of bit#28 of Transmit Queue Extended Control
 *              register. The bit should be set by default.
 *
 * PRV_CPSS_DXCH_XCAT_RM_MIRROR_INTERFACE_PARAMETER_REGISTER_I_WA_E -
 *              Wrong default values in Mirror Interface Parameter Register<%i> -
 *              bit #20 should be with value 1
 *
 * PRV_CPSS_DXCH_XCAT_RM_POLICY_ENGINE_CONFIGURATION_REGISTER_WA_E -
 *              Wrong default values in Policy Engine Configuration -
 *              bit #12 should be with value 1
 *
 * PRV_CPSS_DXCH_XCAT_RM_TXQ_DEQUEUE_SWRR_WEIGHT_REGISTER_3_WA_E -
 *              Wrong default values in Dequeue SWRR Weight Register 3 -
 *              bit #0..8 : set to value 0x4b ,
 *                  because fields of XG and Gig1 weights are swapped
 *
 * PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL0_WA_E -
 *              Wrong default values in IPLR0,1 --> Policer Control0 -
 *              bit #21 : set to 1
 *
 * PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL1_WA_E -
 *              Wrong default values in IPLR0,1 --> Policer Control1 -
 *              bit #0 : set to 1
 *
 * PRV_CPSS_DXCH_XCAT_RM_POLICER_CONTROL2_WA_E -
 *              Wrong default values in IPLR0,1,EPLR --> Policer Control2 -
 *              bit #0 : set to 1
 *
 *  PRV_CPSS_DXCH_XCAT_RM_EGRESS_POLICER_GLOBAL_CONFIGURATION_WA_E
 *              Wrong default values in Egress Policer Global Configuration -
 *              bit #0 : set to 1
 *
 *  PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_WA_E -
 *              Wrong default values in BCN control -
 *              bit #1 : set to 0
 *              bit #3 : set to 1
 *
 *  PRV_CPSS_DXCH_XCAT_RM_PORTS_BCN_AWARENESS_TABLE_WA_E -
 *              Wrong default values in Ports BCN Awareness Table -
 *              bits #0..27 : set to 0x0FFFFFFF
 *
 * PRV_CPSS_DXCH_XCAT_RM_TTI_UNIT_GLOBAL_CONFIGURATION_WA_E -
 *              Wrong default values in TTI Unit Global Configuration -
 *              bit #3 : set to 1
 *              bit #19 : set to 0
 *
 *  PRV_CPSS_DXCH_XCAT_RM_EXTENDED_DSA_BYPASS_BRIDGE_WA_E -
 *              bit #8 in the 'per port' 'PORT VLAN QOS configuration'
 *                      ('Extended DSA Bypass Bridg') : set to 1
 *
 *  PRV_CPSS_DXCH_XCAT_RM_PORT_SERIAL_PARAMETERS_CONFIGURATION_WA_E -
 *              Wrong default values in Port Serial Parameters Configuration
 *              bit #1 : set to 0
 *
 *  PRV_CPSS_DXCH_XCAT_RM_LMS0_LMS1_MISC_CONFIGURATIONS_WA_E -
 *              Wrong default values in LMS0 Misc Configurations , LMS1 Misc
 *              Configurations.
 *              bit #15..16 : set to 2
 *              bit #20 : set to 0
 *
 *  PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E -
 *              Traffic from the CPU via SDMA mechanism can corrupt the switch
 *              internal buffer memory.
 *              Solution: Traffic sent from the CPU is padded with 8 bytes of
 *              zero’s for byte counts of <byteblock>*n+k (1<=k<=8) (including
 *              DSA tag), where n and k are integers and <byteblock> is 256 for
 *              DxChXcat and 512 for DxChLion,
 *
 * PRV_CPSS_DXCH_XCAT_TABLE_EPLR_NOT_SUPPORT_WA_E -
 *              EPLR (egress policer) not supported.
 *
 * PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E -
 *              when PP sending a packet to CPU from port that it is a member of
 *              trunk, the <source port> field should hold the source port (not
 *              the trunkId) , in DSA Marvell TAG header.
 *              but due to the erratum this field includes the <trunk ID> / 2.
 *
 * PRV_CPSS_DXCH_XCAT_IEEE_RESERVED_MC_CONFG_REG_READ_WA_E -
 *              Reading of IEEE Reserved Multicast Configuration registers
 *              should be peformed from:
 *              0x200X820 instead of 0x200X810 (table 2)
 *              0x200X828 instead of 0x200X818 (table 3)
 *
 * PRV_CPSS_DXCH_XCAT_RM_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E -
 *              Enable CPU lacks source port information on traffic
 *              received from trunk workaround:
 *              Set bit #17 in register 0x0C0002B0
 *              (RM#3017)
 *
 * PRV_CPSS_DXCH_XCAT_GIGA_PORT_PRBS_PARAM_INIT_WA_E -
 *              FEr#3054: Gig port PRBS is not functional.
 *              Initialization PRBS parameters of Giga MAC for ports 24-27.
 *              Bits 15:14 of regisres
 *              0x0A806014, 0x0A806414, 0x0A806814, 0x0A806C14
 *              set to 0x03
 *
 * PRV_CPSS_DXCH_LION_RM_BUFFER_MANAGEMENT_INIT_VALUES_WA_E -
 *              Device buffer limits are incorrect.
 *              Buffers might be allocated to more than one flow causing
 *              transmission of corrupted packets.
 *              WA: Per management control interface, write to the following
 *              registers:
 *              Change bit0 of 0x03000060 to 0x1 to enable the next write.
 *              Change offset 0x03000200 to 0x7FF
 *              Change bit 0 of 0x03000060 to 0x0
 *              Change bits[30:22] of 0x03000000 to 0x3F
 *              Change bits[21:11] of 0x03000000 to 0x7FF
 *              (RM#2700)
 *
 * PRV_CPSS_DXCH_LION_RM_SDMA_ACTIVATION_WA_E -
 *              The default value of the <SelPortSDMA> field in the Global
 *              Control register (offset 0x58 in each pipe) is 0x0.
 *              This value selects the CPU port rather than the SDMA as the
 *              interface for packet to/from CPU. To activate the SDMA as the
 *              interface to/from CPU, the first required modification prior to
 *              any access to SDMA registers is to set this bit to 0x1.
 *              If an access to any of the SDMA registers is performed prior to
 *              setting the <SelPortSDMA> field, then the PCI Express interface
 *              will hang.
 *              WA : Per management control interface, write to the following
 *              registers: Change bit 20 of 0x58 to 0x1
 *              (RM#2701)
 *
 *  PRV_CPSS_DXCH_LION_TABLE_SIZE_ROUTER_TCAM_13K_INSTEAD_16K_WA_E -
 *              Router TCAM size is limited to 13K entries
 *              (FEr#2743)
 *
 *  PRV_CPSS_DXCH_LION_INDIRECT_ACCESS_TO_TCAM_IS_NOT_SUPPORTED_WA_E -
 *              Indirect access to TCAM (Router and PCL) is not supported
 *              (FEr#2744 --> include FEr#2004 as part of it)
 *
 *  PRV_CPSS_DXCH_LION_TCAM_SYNCHRONIZATION_RESTRICTION_WA_E
 *              The device TCAM can operate at a frequency similar to or higher
 *              than the core frequency.
 *              In addition, the TCAM holds a FIFO of pending requests for
 *              synchronizing the core domain with the TCAM domain.
 *              However, due to this erratum, the TCAM clock frequency must be
 *              higher than the core clock frequency.
 *              Functional Impact : TCAM search results will be erroneous.
 *              WA : With the 25 MHz reference clock, the following sequence
 *              needs to be implemented per control pipe:
 *              1. Power down PLL: change address 0x0000036C bits[3:0] to 0xD.
 *              2. Wait for at least 20 uSec.
 *              3. Change PLL frequency to 364 MHz: change address 0x00000368 to 0x30AE013.
 *              4. Power up PLL, but block the clock: change address 0x0000036C bits[3:0] to 0xC.
 *              5. Wait at least 50 uSec for clock stabilization.
 *              6. Enable clock to the TCAM: change address 0x0000036C bits[3:0] to 0xE.
 *              (FEr#2742)
 *
 */
typedef enum{

    PRV_CPSS_DXCH_FDB_AGING_WITH_REMOVAL_MODE_WA_E,
    PRV_CPSS_DXCH_TRUNK_CONFIG_FOR_TRUNK_NEXT_HOP_WA_E,
    PRV_CPSS_DXCH_INCORRECT_SRCDEV_FIELD_IN_MAILBOX_TO_CPU_DSA_TAG_WA_E,
    PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E,
    PRV_CPSS_DXCH_JUMBO_FRAMES_MODE_SDWRR_WEIGHT_LIMITATION_WA_E,
    PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E,
    PRV_CPSS_DXCH_RATE_LIMIT_GIG_COUNTER_BITS_NUM_WA_E,
    PRV_CPSS_DXCH_XG_MIB_READ_OVERSIZE_AND_JABBER_COUNTERS_WA_E,
    PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E,
    PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E,
    PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E,
    PRV_CPSS_DXCH2_POLICY_ACTION_DIRECT_BURST_ONLY_READ_WA_E,
    PRV_CPSS_DXCH2_DIRECT_ACCESS_TO_POLICER_ADDRESS_SPACE_WA_E,
    PRV_CPSS_DXCH2_ROUTER_ARP_AND_TS_TABLE_NOT_SUPPORT_BURST_ACCESS_VIA_PCI_WA_E,
    PRV_CPSS_DXCH2_RDWRTRIG_BIT_IN_VLT_TABLES_ACCESS_CONTROL_REGISTER_IGNORED_E,
    PRV_CPSS_DXCH2_INDIRECT_ACCESS_ROUTER_AND_TT_TCAM_NOT_FUNCTION_CORRECTLY_WA_E,
    PRV_CPSS_DXCH_RM_XG_PHY_PATH_FAILURE_WA_E,
    PRV_CPSS_DXCH2_RM_RESERVED_REGISTERS_0x0B820108_WA_E,
    PRV_CPSS_DXCH_RM_FDB_TABLE_ENTRIES_WA_E,
    PRV_CPSS_DXCH2_PCL_EGRESS_ACTION_CNTR_VLAN_WRONG_BITS_POS_WA_E,
    PRV_CPSS_DXCH3_VLT_INDIRECT_ACCESS_WA_E,
    PRV_CPSS_DXCH3_SAMPLE_AT_RESET_NET_REF_CLOCK_SELECT_WA_E,
    PRV_CPSS_DXCH3_RM_BM_GE_XG_PORT_GROUP_LIMIT_CFG_REG_WA_E,
    PRV_CPSS_DXCH3_RM_GE_PORTS_MODE1_REG_WA_E,
    PRV_CPSS_DXCH3_RM_XG_PORTS_MODE_REG_WA_E,
    PRV_CPSS_DXCH3_RM_TCAM_PLL_REG_WA_E,
    PRV_CPSS_DXCH3_RM_LMS0_1_GROUP1_LED_INTERFACE_WA_E,
    PRV_CPSS_DXCH3_RM_MAC2ME_LOW_REG_WA_E,
    PRV_CPSS_DXCH3_RM_AN_CNF_REG_WA_E,
    PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E,
    PRV_CPSS_DXCH3_TXQ_PARITY_CALCULATION_E,
    PRV_CPSS_DXCH3_SDMA_WA_E,
    PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E,
    PRV_CPSS_DXCH3_OUT_OF_PROFILE_QOS_ATTR_E,
    PRV_CPSS_DXCH3_TXQ_FULL_INTERRUPT_NOT_FUNCTION_WA_E,
    PRV_CPSS_DXCH3_RM_GE_SERDES_MISC_CONF_REG_WA_E,
    PRV_CPSS_DXCH3_RM_BM_TX_FIFO_THRESHOLD_CONF_REG_WA_E,
    PRV_CPSS_DXCH3_TCAM_EFUSE_NOT_TRIGGERED_AUTO_WA_E,
    PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E,
    PRV_CPSS_DXCH_RM_FDB_GLOBAL_CONF_REG_WA_E,
    PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E,

    PRV_CPSS_DXCH_XCAT_TABLE_SIZE_FDB_16K_INSTEAD_8K_WA_E,
    PRV_CPSS_DXCH_XCAT_TABLE_SIZE_NH_4K_INSTEAD_8K_WA_E,
    PRV_CPSS_DXCH_XCAT_TABLE_SIZE_TS_1K_INSTEAD_2K_WA_E,
    PRV_CPSS_DXCH_XCAT_TABLE_CNC_NUM_BLOCKS_1_INSTEAD_2_WA_E,
    PRV_CPSS_DXCH_XCAT_TABLE_POLICERS_NUM_1K_INSTEAD_2K_WA_E,
    PRV_CPSS_DXCH_XCAT_FDB_AGING_TIMEOUT_WA_E,
    PRV_CPSS_DXCH_XCAT_TR101_WA_E,
    PRV_CPSS_DXCH_XCAT_IPLR_SECOND_STAGE_SUPPORT_WA_E,
    PRV_CPSS_DXCH_XCAT_SGMII_MODE_ON_STACKING_PORTS_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_TOTAL_BUFFER_LIMIT_CONF_IN_TXQ_E,
    PRV_CPSS_DXCH_XCAT_RM_CNC_ENABLE_COUNTING_E,
    PRV_CPSS_DXCH_XCAT_RM_TXQ_EXT_CNTR_REG_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_MIRROR_INTERFACE_PARAMETER_REGISTER_I_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_POLICY_ENGINE_CONFIGURATION_REGISTER_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_TXQ_DEQUEUE_SWRR_WEIGHT_REGISTER_3_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL0_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL1_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_POLICER_CONTROL2_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_EGRESS_POLICER_GLOBAL_CONFIGURATION_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_PORTS_BCN_AWARENESS_TABLE_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_TTI_UNIT_GLOBAL_CONFIGURATION_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_EXTENDED_DSA_BYPASS_BRIDGE_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_PORT_SERIAL_PARAMETERS_CONFIGURATION_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_LMS0_LMS1_MISC_CONFIGURATIONS_WA_E,
    PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E,
    PRV_CPSS_DXCH_XCAT_TABLE_EPLR_NOT_SUPPORT_WA_E,
    PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E,
    PRV_CPSS_DXCH_XCAT_IEEE_RESERVED_MC_CONFG_REG_READ_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E,
    PRV_CPSS_DXCH_XCAT_GIGA_PORT_PRBS_PARAM_INIT_WA_E,

    PRV_CPSS_DXCH_LION_RM_BUFFER_MANAGEMENT_INIT_VALUES_WA_E,
    PRV_CPSS_DXCH_LION_RM_SDMA_ACTIVATION_WA_E,
    PRV_CPSS_DXCH_LION_TABLE_SIZE_ROUTER_TCAM_13K_INSTEAD_16K_WA_E,
    PRV_CPSS_DXCH_LION_INDIRECT_ACCESS_TO_TCAM_IS_NOT_SUPPORTED_WA_E,
    PRV_CPSS_DXCH_LION_TCAM_SYNCHRONIZATION_RESTRICTION_WA_E,

    /* the last one */
    PRV_CPSS_DXCH_ERRATA_MAX_NUM_E
}PRV_CPSS_DXCH_ERRATA_ENT;


/*
 * Typedef: struct PRV_CPSS_DXCH_ERRATA_STC
 *
 * Description: A structure to hold all Errata info that CPSS need to fix
 *              (workaround) for the PP .
 *
 * Fields:
 *      FErArray - array of bmp of FEr errata
 *                 every erratum is represented by 1 bit (32 errata in a bmp)
 *
 *      srcIdAssignedModeConfRegData -
 *                                data of srcIdAssignedModeConfReg (0x02040070).
 *      bridgeAccessMatrixLine7RegData -
 *                  data of Bridge Access Matrix Line <7> register (0x0200061C).
 *
 *      info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E :
 *              info relevant to PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E
 *              pclInterruptCauseRegAddr - interrupt register for the ECC
 *                  interrupt to the PCL tcam
 *              pclInterruptIndex - the index of the interrupt in the enum of
 *                  the interrupts
 *              routerInterruptCauseRegAddr - interrupt register for the ECC
 *                  interrupt to the Router tcam
 *              routerInterruptIndex - the index of the interrupt in the enum of
 *                  the interrupts
 *      info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E:
 *              info relevant to PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E
 *              enabled - was it enabled by the application --> see function
 *                  cpssDxChTrunkTrafficToCpuWaInit(...)
 *              extraTrunksInfoPtr - (pointer to) extra trunks info needed for the
 *                  WA. the pointer is allocated dynamically during
 *                  cpssDxChTrunkInit(...) only when required(WA needed for the
 *                  PP and application enabled the WA)
 *                  additionalInfoBmp - bitmap for additional WA info
 *      info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E:
 *              enabled - was it enabled by the application --> see function
 *                  cpssDxChHwPpImplementWaInit(...)
 *              devNum - device number of PP that connected to CPU with MII port
 *      info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E:
 *              enabled - was it enabled by the application --> see function
 *                  cpssDxChHwPpImplementWaInit(...)
 *
 */
typedef struct{
    GT_U32          FErArray[PRV_CPSS_DXCH_ERRATA_BMP_MAX_NUM_CNS];
    GT_U32          srcIdAssignedModeConfRegData;
    GT_U32          bridgeAccessMatrixLine7RegData;

    struct{
        GT_U32      pclInterruptCauseRegAddr;
        GT_U32      pclInterruptIndex;

        GT_U32      routerInterruptCauseRegAddr;
        GT_U32      routerInterruptIndex;
    }info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E;

    struct{
        GT_BOOL     enabled;
        void*       extraTrunksInfoPtr;
        GT_U32      additionalInfoBmp;
    }info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E;

    struct{
        GT_BOOL     enabled;
        GT_U8       devNum;
    }info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E;

    struct{
        GT_BOOL     enabled;
    }info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E;

}PRV_CPSS_DXCH_ERRATA_STC;

/* first trunk ID needed for the WA for "wrong trunk id source port information of packet to CPU" */
/*
    (28*2)=56 is first trunk , because 0..27 are valid ports
    but we need to align the trunk-Id to 5 bits:
    bits
trunk bits 6 5 4 3 2 1 0
           ---------------
value      |1|a|b|c|d|e|0|
           ---------------

           the abcde are the 5 bits for trunkId values from the application.

           NOTE: value of abcde == 0,31 are not allowed -->
            so possible values are: 1..30 --> 30 values -->

    so the 7 bit values are :
        66,68,70..122,124 (even values only)

    devices with 127 trunks supports also cascade trunks 126,127 (no traffic to CPU)
*/
#define XCAT_FIRST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU \
    56

/* last trunk ID needed for the WA for "wrong trunk id source port information of packet to CPU" */
/*
    (62*2) is the last trunk ,because 63 which is CPU port
*/
#define XCAT_LAST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU \
    (62*2)

/* macro to check that the WA for trunks requested by the application
   (even if PP not require), for "wrong trunk id source port information of
   packet to CPU" */
#define PRV_CPSS_DXCH_CHECK_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_MAC(_devNum)  \
    ((PRV_CPSS_DXCH_PP_MAC(_devNum)->errata.                                \
        info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.        \
            enabled == GT_TRUE) ? 1 : 0)

/* macro to check that the WA for "packets sent from cpu by sdma corrupted"
   requested by the application (even if PP not require)
*/
#define PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(_devNum)  \
    ((PRV_CPSS_DXCH_PP_MAC(_devNum)->errata.                                \
        info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E.        \
            enabled == GT_TRUE) ? 1 : 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChErrataMngh */


