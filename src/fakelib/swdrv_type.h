#ifndef SWDRV_TYPE_H
#define SWDRV_TYPE_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_adpt.h"
#include "sys_hwcfg.h"
#include "sys_cpnt.h"
#include "dev_swdrv.h"
#include "sysfun.h"

/* NAMING CONSTANT DECLARATIONS
 */
/* debug flag
 */
#define SWDRV_TYPE_DEBUG_FLAG_GBIC_INFO          BIT_0
#define SWDRV_TYPE_DEBUG_FLAG_GBIC_SHOW_PRESENT  BIT_1
#define SWDRV_TYPE_DEBUG_FLAG_GBIC_SHOW_DDM_TEMP BIT_2
#define SWDRV_TYPE_DEBUG_FLAG_GBIC_STOP_MONITOR  BIT_3
#define SWDRV_TYPE_DEBUG_FLAG_PD                 BIT_4

#define SWDRV_TOTAL_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST ((SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT+7)/8)

/* indicates that no priority is specified in SWDRV_TYPE_L2_Address_Info_T.priority
 */
#define SWDRV_TYPE_L2_ADDRESS_WITHOUT_PRIORITY      0xff


/* SWDRV_TYPE_SFP_PRESENT_DELAY_TICKS is the delay between the SFP
 * present is detected and EEPROM of the SFP is accessed through I2C bus.
 *
 * It takes time for a inserted SFP to be powered up and become ready
 * for access. It is found that some transceivers might cause I2C bus dead
 * if CPU try to do I2C operation against it before it is ready. Thus it
 * is better to adopt this delay to ensure the SFP transceiver is accessed
 * through I2C when it is in ready state.
 *
 * The startup time is varied among different SFP transceiver.
 * This delay should be large enough for all kinds of SFP transceiver
 * to be powered up and become ready. If the value is too large, the latency
 * between the time of transceiver inserted and CLI shows the transceiver
 * info would be large. The value might also affected by what kind of the
 * transceivers are used by the customer.
 * If SYS_ADPT_SFP_PRESENT_DELAY_TICKS is not defined, 300ms will be used.
 */
#ifndef SYS_ADPT_SFP_PRESENT_DELAY_TICKS
#define SWDRV_TYPE_SFP_PRESENT_DELAY_TICKS 30
#else
#define SWDRV_TYPE_SFP_PRESENT_DELAY_TICKS SYS_ADPT_SFP_PRESENT_DELAY_TICKS
#endif

#define SWDRV_TYPE_DDM_DIAG_OFFSET 96
#define SWDRV_TYPE_DDM_DIAG_OFFSET_QSFP 22
/* SWDRV_TYPE_DDM_DIAG_SIZE:
 * for sfp, read from 96~105
 * for xfp, read from 96~109 (size:14)
 * for qsfp, read from 22~49 (size:28)
 */
#define SWDRV_TYPE_DDM_DIAG_SIZE 14
#define SWDRV_TYPE_DDM_DIAG_SIZE_QSFP 28

/* DOS protection
 *
 * need sync from SWCTRL_DosProtectionFilter_T and SWCTRL_DosProtectionRateLimit_T
 */
enum
{
    SWDRV_DOS_FILTER_LAND,
    SWDRV_DOS_FILTER_SMURF,
    SWDRV_DOS_FILTER_TCP_NULL_SCAN,
    SWDRV_DOS_FILTER_TCP_SCAN,
    SWDRV_DOS_FILTER_TCP_SYN_FIN_SCAN,
    SWDRV_DOS_FILTER_TCP_UDP_PORT_ZERO,
    SWDRV_DOS_FILTER_TCP_XMAS_SCAN,
};

enum
{
    SWDRV_DOS_RATELIMIT_ECHO_CHARGEN,
    SWDRV_DOS_RATELIMIT_TCP_FLOODING,
    SWDRV_DOS_RATELIMIT_UDP_FLOODING,
    SWDRV_DOS_RATELIMIT_WIN_NUKE,
};

/* need sync with SWCTRL_PktType_T
 */
enum
{
    SWDRV_PKTTYPE_IGMP,
    SWDRV_PKTTYPE_RESERVED_UPD,
    SWDRV_PKTTYPE_MLD,
    SWDRV_PKTTYPE_IPMC,
    SWDRV_PKTTYPE_IPMC6,
    SWDRV_PKTTYPE_DHCP_SERVER,
    SWDRV_PKTTYPE_DHCP_CLIENT,
    SWDRV_PKTTYPE_DHCP6_SERVER,
    SWDRV_PKTTYPE_DHCP6_CLIENT,
    SWDRV_PKTTYPE_ORG_SPECIFIC,
    SWDRV_PKTTYPE_ORG_SPECIFIC1,
    SWDRV_PKTTYPE_ORG_SPECIFIC2,
    SWDRV_PKTTYPE_ORG_SPECIFIC3,
    SWDRV_PKTTYPE_CDP,
    SWDRV_PKTTYPE_PVST,
    SWDRV_PKTTYPE_ND,
    SWDRV_PKTTYPE_INTRUDER,
    SWDRV_PKTTYPE_PIM,
    SWDRV_PKTTYPE_PIM6,
    SWDRV_PKTTYPE_IPV6_ALL_ROUTERS,
    SWDRV_PKTTYPE_IPV6_ALL_NODES,
    SWDRV_PKTTYPE_MAX,
};

/* TYPE DECLARATIONS
 */
typedef struct SWDRV_PoweredDeviceInfo_S
{
    UI8_T status;
    UI8_T mode;
}SWDRV_PoweredDeviceInfo_T;
#if (SYS_CPNT_SWCTRL_CABLE_DIAG == TRUE)
#if (SYS_CPNT_SWCTRL_CABLE_DIAG_CHIP == SYS_CPNT_SWCTRL_CABLE_DIAG_BROADCOM)
typedef struct SWDRV_CableDiagInfo_S
{
    UI32_T state;/* state of all pairs */
    UI32_T npairs;/* pair_* elements valid */
    UI32_T pair_state[4];  /* pair state */
    UI32_T pair_len[4];    /* pair length in metres */
    UI32_T fuzz_len;/* len values +/- this */
} SWDRV_CableDiagInfo_T;

#elif (SYS_CPNT_SWCTRL_CABLE_DIAG_CHIP == SYS_CPNT_SWCTRL_CABLE_DIAG_MARVELL) ||\
      (SYS_CPNT_SWCTRL_CABLE_DIAG_CHIP == SYS_CPNT_SWCTRL_CABLE_DIAG_RTK)
typedef struct SWDRV_CableDiagInfo_S
{
    UI32_T    last_test_time;
    UI32_T    ori_link_status; /* the link status before cable test */
    UI16_T    pair1Status;
    UI16_T    pair2Status;
    UI16_T    pair3Status;
    UI16_T    pair4Status;
    UI8_T     pair1FaultLen;
    UI8_T     pair2FaultLen;
    UI8_T     pair3FaultLen;
    UI8_T     pair4FaultLen;
    BOOL_T    under_testing;
    BOOL_T    is_dsp_mode;
}SWDRV_CableDiagInfo_T; /* need sync with SWCTRL_Cable_Info_T and DEV_SWDRV_CableDiagResultGeneral_T */
#else
#error "Chip not defined!"
#endif
#endif

#define SWDRV_TYPE_GBIC_ID_SFP                      3
#define SWDRV_TYPE_GBIC_ID_XFP                      6
#define SWDRV_TYPE_GBIC_ID_QSFP                     0xC
#define SWDRV_TYPE_GBIC_ID_QSFP_PLUS                0xD

/* GBIC/mini-gbic field length definition
 */
#define SWDRV_TYPE_GBIC_VENDOR_OUI_FIELD_LENGTH          3
#define SWDRV_TYPE_GBIC_VENDOR_NAME_FIELD_LENGTH         16
#define SWDRV_TYPE_GBIC_VENDOR_PN_FIELD_LENGTH           16
#define SWDRV_TYPE_GBIC_VENDOR_REV_FIELD_LENGTH          4
#define SWDRV_TYPE_GBIC_VENDOR_SN_FIELD_LENGTH           16
#define SWDRV_TYPE_GBIC_DATE_CODE_FIELD_LENGTH           8

/* "Fibre Channel Style 1 copper connector": 38 */
#define SWDRV_TYPE_GBIC_SNMP_CONNECTOR_FIELD_LENGTH      38
#define SWDRV_TYPE_GBIC_SNMP_FIBER_TYPE_FIELD_LENGTH     128
#define SWDRV_TYPE_GBIC_SNMP_ETH_COMPLIANCE_FIELD_LENGTH 128
/* "xxxxx MBd": 9 bytes */
#define SWDRV_TYPE_GBIC_SNMP_BAUD_RATE_FIELD_LENGTH      9
#define SWDRV_TYPE_GBIC_SNMP_VENDOR_OUI_FIELD_LENGTH     8
#define SWDRV_TYPE_GBIC_SNMP_VENDOR_NAME_FIELD_LENGTH    SWDRV_TYPE_GBIC_VENDOR_NAME_FIELD_LENGTH
#define SWDRV_TYPE_GBIC_SNMP_VENDOR_PN_FIELD_LENGTH      SWDRV_TYPE_GBIC_VENDOR_PN_FIELD_LENGTH
#define SWDRV_TYPE_GBIC_SNMP_VENDOR_REV_FIELD_LENGTH     SWDRV_TYPE_GBIC_VENDOR_REV_FIELD_LENGTH
#define SWDRV_TYPE_GBIC_SNMP_VENDOR_SN_FIELD_LENGTH      SWDRV_TYPE_GBIC_VENDOR_SN_FIELD_LENGTH
#define SWDRV_TYPE_GBIC_SNMP_DATE_CODE_FIELD_LENGTH      SWDRV_TYPE_GBIC_DATE_CODE_FIELD_LENGTH

/* "-127.99 degree C": 16 bytes */
#define SWDRV_TYPE_GBIC_SNMP_TEMP_FIELD_LENGTH (16+SWDRV_TYPE_GBIC_SNMP_ALARM_WARNING_FIELD_LENGTH)
/* "3.33 V": 6 bytes */
#define SWDRV_TYPE_GBIC_SNMP_VCC_FIELD_LENGTH (6+SWDRV_TYPE_GBIC_SNMP_ALARM_WARNING_FIELD_LENGTH)
/* "130.99 mA": 9 bytes */
#define SWDRV_TYPE_GBIC_SNMP_CURRENT_FIELD_LENGTH (9+SWDRV_TYPE_GBIC_SNMP_ALARM_WARNING_FIELD_LENGTH)
/* "-39.02 dBm": 9 bytes */
#define SWDRV_TYPE_GBIC_SNMP_POWER_FIELD_LENGTH (9+SWDRV_TYPE_GBIC_SNMP_ALARM_WARNING_FIELD_LENGTH)
/* for ddm measured info, we will append following string
 * at end of the value.
 * " high warning": 13 bytes
 */
#define SWDRV_TYPE_GBIC_SNMP_ALARM_WARNING_FIELD_LENGTH  13
#define SWDRV_TYPE_GBIC_EEPROM_MAX_LENGTH                256
#define SWDRV_TYPE_GBIC_TRANSCEIVER_FIELD_LENGTH         8

/* GBIC connector type
 */
#define SWDRV_TYPE_GBIC_CONNECTOR_UNKNOWN                0
#define SWDRV_TYPE_GBIC_CONNECTOR_SC                     1
#define SWDRV_TYPE_GBIC_CONNECTOR_FIBRE_CHANNEL_STYLE_1  2
#define SWDRV_TYPE_GBIC_CONNECTOR_FIBRE_CHANNEL_STYLE_2  3
#define SWDRV_TYPE_GBIC_CONNECTOR_BNC_TNC                4
#define SWDRV_TYPE_GBIC_CONNECTOR_FIBRE_CHANNEL_COAXIAL  5
#define SWDRV_TYPE_GBIC_CONNECTOR_FIBRE_JACK             6
#define SWDRV_TYPE_GBIC_CONNECTOR_LC                     7
#define SWDRV_TYPE_GBIC_CONNECTOR_MT_RJ                  8
#define SWDRV_TYPE_GBIC_CONNECTOR_MU                     9
#define SWDRV_TYPE_GBIC_CONNECTOR_SG                     0xA
#define SWDRV_TYPE_GBIC_CONNECTOR_OPTICAL_PIGTAIL        0xB
#define SWDRV_TYPE_GBIC_CONNECTOR_MPO_PARALLEL_OPTIC     0xC
#define SWDRV_TYPE_GBIC_CONNECTOR_HSSDC_II               0x20
#define SWDRV_TYPE_GBIC_CONNECTOR_COPPER_PIGTAIL         0x21
#define SWDRV_TYPE_GBIC_CONNECTOR_RJ45                   0x22
#define SWDRV_TYPE_GBIC_CONNECTOR_NO_SEPARABLE_CONNECTOR 0x23

/* GBIC fiber type: Fibre Channel Transmission media
 */
#define SWDRV_TYPE_GBIC_FIBER_TYPE_TWIN_AXIAL_PAIR       0x80
#define SWDRV_TYPE_GBIC_FIBER_TYPE_TWISTED_PAIR          0x40
#define SWDRV_TYPE_GBIC_FIBER_TYPE_MINIATURE_COAX        0x20
#define SWDRV_TYPE_GBIC_FIBER_TYPE_VIDEO_COAX            0x10
#define SWDRV_TYPE_GBIC_FIBER_TYPE_MULTIMODE_6           0x8
#define SWDRV_TYPE_GBIC_FIBER_TYPE_MULTIMODE_5           0x4
#define SWDRV_TYPE_GBIC_FIBER_TYPE_SINGLEMODE            0x1

/* QSFP fiber type: Fibre Channel Transmission media
 */
#define SWDRV_TYPE_QSFP_FIBER_TYPE_TWIN_AXIAL_PAIR       0x80
#define SWDRV_TYPE_QSFP_FIBER_TYPE_TWISTED_PAIR          0x40
#define SWDRV_TYPE_QSFP_FIBER_TYPE_MINIATURE_COAX        0x20
#define SWDRV_TYPE_QSFP_FIBER_TYPE_VIDEO_COAX            0x10
#define SWDRV_TYPE_QSFP_FIBER_TYPE_MULTIMODE_6           0x8
#define SWDRV_TYPE_QSFP_FIBER_TYPE_MULTIMODE_5           0x4
#define SWDRV_TYPE_QSFP_FIBER_TYPE_MULTIMODE_3           0x2
#define SWDRV_TYPE_QSFP_FIBER_TYPE_SINGLEMODE            0x1

/* QSFP 10/40g ethernet compliance codes
 */
#define SWDRV_TYPE_QSFP_40G_ETH_COMP_CODES_10GBASE_LRM          0x40
#define SWDRV_TYPE_QSFP_40G_ETH_COMP_CODES_10GBASE_LR           0x20
#define SWDRV_TYPE_QSFP_40G_ETH_COMP_CODES_10GBASE_SR           0x10
#define SWDRV_TYPE_QSFP_40G_ETH_COMP_CODES_40GBASE_CR4          0x8
#define SWDRV_TYPE_QSFP_40G_ETH_COMP_CODES_40GBASE_SR4          0x4
#define SWDRV_TYPE_QSFP_40G_ETH_COMP_CODES_40GBASE_LR4          0x2
#define SWDRV_TYPE_QSFP_40G_ETH_COMP_CODES_40G_ACTIVE_CABLE     0x1

/* XFP 10g ethernet compliance codes
 */
#define SWDRV_TYPE_XFP_10G_ETH_COMP_CODES_10GBASE_SR     0x80
#define SWDRV_TYPE_XFP_10G_ETH_COMP_CODES_10GBASE_LR     0x40
#define SWDRV_TYPE_XFP_10G_ETH_COMP_CODES_10GBASE_ER     0x20
#define SWDRV_TYPE_XFP_10G_ETH_COMP_CODES_10GBASE_LRM    0x10
#define SWDRV_TYPE_XFP_10G_ETH_COMP_CODES_10GBASE_SW     0x8
#define SWDRV_TYPE_XFP_10G_ETH_COMP_CODES_10GBASE_LW     0x4
#define SWDRV_TYPE_XFP_10G_ETH_COMP_CODES_10GBASE_EW     0x2

/* GBIC 10g ethernet compliance codes
 */
#define SWDRV_TYPE_GBIC_10G_ETH_COMP_CODES_10GBASE_ER    0x80
#define SWDRV_TYPE_GBIC_10G_ETH_COMP_CODES_10GBASE_LRM   0x40
#define SWDRV_TYPE_GBIC_10G_ETH_COMP_CODES_10GBASE_LR    0x20
#define SWDRV_TYPE_GBIC_10G_ETH_COMP_CODES_10GBASE_SR    0x10

/* GBIC ethernet compliance codes
 */
#define SWDRV_TYPE_GBIC_ETH_COMP_CODES_BASE_PX           0x80
#define SWDRV_TYPE_GBIC_ETH_COMP_CODES_BASE_BX10         0x40
#define SWDRV_TYPE_GBIC_ETH_COMP_CODES_100BASE_FX        0x20
#define SWDRV_TYPE_GBIC_ETH_COMP_CODES_100BASE_LX_LX10   0x10
#define SWDRV_TYPE_GBIC_ETH_COMP_CODES_1000BASE_T        0x8
#define SWDRV_TYPE_GBIC_ETH_COMP_CODES_1000BASE_CX       0x4
#define SWDRV_TYPE_GBIC_ETH_COMP_CODES_1000BASE_LX       0x2
#define SWDRV_TYPE_GBIC_ETH_COMP_CODES_1000BASE_SX       0x1

/* GBIC Support DDM Bit (Bit 6 of Byte 92)
 * and internal calibration (Bit 5 of Byte 92)
 * and external calibration (Bit 4 of Byte 92)
 */
#define SWDRV_TYPE_GBIC_DIAG_MONITOR_TYPE_DDM_IMPLEMENTED             0x40
#define SWDRV_TYPE_GBIC_DIAG_MONITOR_TYPE_INTERNELLY_CALIBRATE        0x20
#define SWDRV_TYPE_GBIC_DIAG_MONITOR_TYPE_EXTERNELLY_CALIBRATE        0x10

/* TYPE DECLARATIONS
 */
typedef BOOL_T (*SWDRV_TYPE_GetSfpDdmInfoFunc_T)(UI32_T dev_index, UI16_T offset, UI8_T size, UI8_T *info_p);

/* for SNMP */
 #pragma pack(1)
typedef struct SWDRV_TYPE_SfpEntry_S
{
    UI32_T  ifindex;
    UI8_T   connector_type[SWDRV_TYPE_GBIC_SNMP_CONNECTOR_FIELD_LENGTH + 2];
    UI8_T   fiber_type[SWDRV_TYPE_GBIC_SNMP_FIBER_TYPE_FIELD_LENGTH + 1];
    UI8_T   eth_comp_code[SWDRV_TYPE_GBIC_SNMP_ETH_COMPLIANCE_FIELD_LENGTH + 1];
    UI8_T   baud_rate[SWDRV_TYPE_GBIC_SNMP_BAUD_RATE_FIELD_LENGTH + 1];
    UI8_T   vendor_oui[SWDRV_TYPE_GBIC_SNMP_VENDOR_OUI_FIELD_LENGTH + 1];
    UI8_T   vendor_name[SWDRV_TYPE_GBIC_SNMP_VENDOR_NAME_FIELD_LENGTH + 1];
    UI8_T   vendor_pn[SWDRV_TYPE_GBIC_SNMP_VENDOR_PN_FIELD_LENGTH + 1];
    UI8_T   vendor_rev[SWDRV_TYPE_GBIC_SNMP_VENDOR_REV_FIELD_LENGTH + 1];
    UI8_T   vendor_sn[SWDRV_TYPE_GBIC_SNMP_VENDOR_SN_FIELD_LENGTH + 1];
    UI8_T   date_code[SWDRV_TYPE_GBIC_SNMP_DATE_CODE_FIELD_LENGTH + 1];
} SWDRV_TYPE_SfpEntry_T;
 #pragma pack()

 #pragma pack(1)
typedef struct SWDRV_TYPE_SfpDdmEntry_S
{
    UI32_T  ifindex;
    UI8_T   temperature[SWDRV_TYPE_GBIC_SNMP_TEMP_FIELD_LENGTH + 1];     /* support_ddm == TRUE */
    UI8_T   voltage[SWDRV_TYPE_GBIC_SNMP_VCC_FIELD_LENGTH + 1];          /* support_ddm == TRUE */
    UI8_T   bias_current[SWDRV_TYPE_GBIC_SNMP_CURRENT_FIELD_LENGTH + 1]; /* support_ddm == TRUE */
    UI8_T   tx_power[SWDRV_TYPE_GBIC_SNMP_POWER_FIELD_LENGTH + 1];       /* support_ddm == TRUE */
    UI8_T   rx_power[SWDRV_TYPE_GBIC_SNMP_POWER_FIELD_LENGTH + 1];       /* support_ddm == TRUE */
} SWDRV_TYPE_SfpDdmEntry_T;
 #pragma pack()

typedef enum
{
    SWDRV_TYPE_AUX_INPUT_TYPE_NOT_IMPLEMENT,
    SWDRV_TYPE_AUX_INPUT_TYPE_APD_BIAS_VOLTAGE,
    SWDRV_TYPE_AUX_INPUT_TYPE_RESERVED2,
    SWDRV_TYPE_AUX_INPUT_TYPE_TEC_CURRENT,
    SWDRV_TYPE_AUX_INPUT_TYPE_LASER_TEMPERATURE,
    SWDRV_TYPE_AUX_INPUT_TYPE_LASER_WAVELENGTH,
    SWDRV_TYPE_AUX_INPUT_TYPE_SUPPLY_VOLTAGE_0, /* +5V Supply Voltage */
    SWDRV_TYPE_AUX_INPUT_TYPE_SUPPLY_VOLTAGE_1, /* +3.3V Supply Voltage */
    SWDRV_TYPE_AUX_INPUT_TYPE_SUPPLY_VOLTAGE_2, /* +1.8V Supply Voltage */
    SWDRV_TYPE_AUX_INPUT_TYPE_SUPPLY_VOLTAGE_3, /* -5.2V Supply Voltage */
    SWDRV_TYPE_AUX_INPUT_TYPE_SUPPLY_CURRENT_0, /* +5V Supply Current */
    SWDRV_TYPE_AUX_INPUT_TYPE_RESERVED11,
    SWDRV_TYPE_AUX_INPUT_TYPE_RESERVED12,
    SWDRV_TYPE_AUX_INPUT_TYPE_SUPPLY_CURRENT_1, /* +3.3V Supply Current */
    SWDRV_TYPE_AUX_INPUT_TYPE_SUPPLY_CURRENT_2, /* +1.8V Supply Current */
    SWDRV_TYPE_AUX_INPUT_TYPE_SUPPLY_CURRENT_3, /* -5.2V Supply Current */
} SWDRV_TYPE_AuxInputType_T;

#pragma pack(1)
typedef struct SWDRV_TYPE_SfpInfoRaw_S
{
    UI8_T  identifier;                                                 /* byte(0)      */
    UI8_T  ext_identifier;                                             /* byte(1)      */
    UI8_T  connector;                                                  /* byte(2)      */
    UI8_T  transceiver[SWDRV_TYPE_GBIC_TRANSCEIVER_FIELD_LENGTH];      /* byte(3-10)   */
    UI8_T  encoding;                                                   /* byte(11)     */
    UI8_T  bitrate;                                                    /* byte(12), identify mini-gbic type*/
    UI8_T  rate_identifier;                                            /* byte(13)     */
    UI8_T  link_length_support_km;                                     /* byte(14)     */
    UI8_T  link_length_support_100m;                                   /* byte(15)     */
    UI8_T  length_50um;                                                /* byte(16)     */
    UI8_T  length_62dot5um;                                            /* byte(17)     */
    UI8_T  length_cable;                                               /* byte(18)     */
    UI8_T  length_om3;                                                 /* byte(19)     */
    UI8_T  vendor_name[SWDRV_TYPE_GBIC_VENDOR_NAME_FIELD_LENGTH];      /* byte(20-35)  */
    UI8_T  transceiver_1;                                              /* byte(36)     */
    UI8_T  vendor_oui[SWDRV_TYPE_GBIC_VENDOR_OUI_FIELD_LENGTH];        /* byte(37-39)  */
    UI8_T  vendor_pn[SWDRV_TYPE_GBIC_VENDOR_PN_FIELD_LENGTH];          /* byte(40-55)  */
    UI8_T  vendor_rev[SWDRV_TYPE_GBIC_VENDOR_REV_FIELD_LENGTH];        /* byte(56-59)  */
    UI8_T  wavelength[2];                                              /* byte(60-61)  */
    UI8_T  unallocated;                                                /* byte(62)     */
    UI8_T  cc_base;                                                    /* byte(63)     */
    UI8_T  options[2];                                                 /* byte(64-65)  */
    UI8_T  br_max;                                                     /* byte(66)     */
    UI8_T  br_min;                                                     /* byte(67)     */
    UI8_T  vendor_sn[SWDRV_TYPE_GBIC_VENDOR_SN_FIELD_LENGTH];          /* byte(68-83)  */
    UI8_T  date_code[SWDRV_TYPE_GBIC_DATE_CODE_FIELD_LENGTH];          /* byte(84-91)  */
    UI8_T  diag_monitor_type;                                          /* byte(92)     */
    UI8_T  enhanced_options;                                           /* byte(93)     */
    UI8_T  sff_8472_compliance;                                        /* byte(94)     */
    UI8_T  cc_ext;                                                     /* byte(95)     */
    UI8_T  vendor_specific[32];                                        /* byte(96-127) */
    UI8_T  reserved[128];                                              /* byte(128-255)*/
} SWDRV_TYPE_SfpInfoRaw_T;
#pragma pack()

/*  for CLI use
 */
#pragma pack(1)
typedef struct SWDRV_TYPE_SfpInfo_S
{
    BOOL_T is_valid;                                                   /* data valid or not */

    UI8_T  identifier;                                                 /* byte(0)      */
    UI8_T  connector;                                                  /* byte(2)      */
    UI8_T  transceiver[SWDRV_TYPE_GBIC_TRANSCEIVER_FIELD_LENGTH];      /* byte(3-10)   */
    UI8_T  bitrate;                                                    /* byte(12), identify mini-gbic type*/
    UI8_T  link_length_support_km;                                     /* byte(14)     */
    UI8_T  link_length_support_100m;                                   /* byte(15)     */
    UI8_T  vendor_name[SWDRV_TYPE_GBIC_VENDOR_NAME_FIELD_LENGTH + 1];  /* byte(20-35)  */
    UI8_T  vendor_oui[SWDRV_TYPE_GBIC_VENDOR_OUI_FIELD_LENGTH];        /* byte(37-39)  */
    UI8_T  vendor_pn[SWDRV_TYPE_GBIC_VENDOR_PN_FIELD_LENGTH + 1];      /* byte(40-55)  */
    UI8_T  vendor_rev[SWDRV_TYPE_GBIC_VENDOR_REV_FIELD_LENGTH + 1];    /* byte(56-59)  */
    UI16_T wavelength;                                                 /* byte(60-61)  */
    UI8_T  vendor_sn[SWDRV_TYPE_GBIC_VENDOR_SN_FIELD_LENGTH + 1];      /* byte(68-83)  */
    UI8_T  date_code[SWDRV_TYPE_GBIC_DATE_CODE_FIELD_LENGTH];          /* byte(84-91)  */
#if 0
    UI8_T  vendor_specific_ID[5];                                      /* byte(96-99) currently used by 32 only */
    UI8_T  vendor_specific_PN[13];                                     /* byte(100-106) currently used by 32 only */
#endif
    BOOL_T ext_calibrate_needed:1;  /* bit 4 of byte(92), TRUE if need external calibrate */
    BOOL_T support_ddm:1;           /* bit 6 of byte(92), TRUE if ddm is implemented */
#if (SYS_HWCFG_GBIC_HAS_RX_LOS==TRUE)
    BOOL_T is_rx_los_implemented:1; /* bit1 of byte(65) */
    BOOL_T is_rx_los_inverted:1;    /* bit2 of byte(65) */
#endif
    UI8_T  aux_monitoring;                                             /* byte(222)    */
} SWDRV_TYPE_SfpInfo_T;
#pragma pack()

/* need transform if CPU is little-endian */
/* SFF-8472 Specification for
 * Diagnostic Monitoring Interface for Optical Transceivers
 */
#pragma pack(1)
typedef struct SWDRV_TYPE_SfpDdmInfoRaw_S
{
    UI8_T  temp_high_alarm[2];/*0-1*/
    UI8_T  temp_low_alarm[2];/*2-3*/
    UI8_T  temp_high_warning[2];/*4-5*/
    UI8_T  temp_low_warning[2];/*6-7*/
    UI8_T  voltage_high_alarm[2];/*8-9*/
    UI8_T  voltage_low_alarm[2];/*10-11*/
    UI8_T  voltage_high_warning[2];/*12-13*/
    UI8_T  voltage_low_warning[2];/*14-15*/
    UI8_T  bias_high_alarm[2];/*16-17*/
    UI8_T  bias_low_alarm[2];/*18-19*/
    UI8_T  bias_high_warning[2];/*20-21*/
    UI8_T  bias_low_warning[2];/*22-23*/
    UI8_T  tx_power_high_alarm[2];/*24-25*/
    UI8_T  tx_power_low_alarm[2];/*26-27*/
    UI8_T  tx_power_high_warning[2];/*28-29*/
    UI8_T  tx_power_low_warning[2];/*30-31*/
    UI8_T  rx_power_high_alarm[2];/*32-33*/
    UI8_T  rx_power_low_alarm[2];/*34-35*/
    UI8_T  rx_power_high_warning[2];/*36-37*/
    UI8_T  rx_power_low_warning[2];/*38-39*/
    UI8_T  unallocated1[16];/*40-55*/
    UI8_T  rx_pwr_4[4];/*56-59*/
    UI8_T  rx_pwr_3[4];/*60-63*/
    UI8_T  rx_pwr_2[4];/*64-67*/
    UI8_T  rx_pwr_1[4];/*68-71*/
    UI8_T  rx_pwr_0[4];/*72-75*/
    UI8_T  tx_i_slope[2];/*76-77*/
    UI8_T  tx_i_offset[2];/*78-79*/
    UI8_T  tx_pwr_slope[2];/*80-81*/
    UI8_T  tx_pwr_offset[2];/*82-83*/
    UI8_T  t_slope[2];/*84-85*/
    UI8_T  t_offset[2];/*86-87*/
    UI8_T  v_slope[2];/*88-89*/
    UI8_T  v_offset[2];/*90-91*/
    UI8_T  unallocated2[3];/*92-94*/
    UI8_T  checksum;/*95: sum of bytes 0 - 94 */
    UI8_T  temperature[2];/*96-97*/
    UI8_T  voltage[2];/*98-99*/
    UI8_T  tx_bias_current[2];/*100-101*/
    UI8_T  tx_power[2];/*102-103*/
    UI8_T  rx_power[2];/*104-105*/
    UI8_T  unallocated3[4];/*106-109*/
    UI8_T  status_control;/*110*/
    UI8_T  reserved;/*111*/
    UI8_T  alarm_flags[2];/*112-113*/
    UI8_T  unallocated4[2];/*114-115*/
    UI8_T  warning_flags[2];/*116-117*/
    UI8_T  ext_status_control[2];/*118-119*/
    UI8_T  vendor_specific[8];/*120-127*/
    UI8_T  user_eeprom[120];/*128-247*/
    UI8_T  vendor_specific_ctrl_fun[8];/*248-255*/
} SWDRV_TYPE_SfpDdmInfoRaw_T;
#pragma pack()

/*
 * SFF-8436 Specification for
 * QSFP+ 10 Gbs 4X PLUGGABLE TRANSCEIVER
 */
#pragma pack(1)
typedef struct SWDRV_TYPE_QsfpDdmInfoRaw_S
{
    UI8_T  identifier[1];              /*0*/
    UI8_T  status[2];                  /*1-2*/
    UI8_T  interrupt_flags[19];        /*3-21*/
    UI8_T  temperature[2];             /*22-23*/
    UI8_T  reserved_0[2];              /*24-25*/
    UI8_T  voltage[2];                 /*26-27*/
    UI8_T  reserved_1[6];              /*28-33*/
    UI8_T  rx_power_1[2];              /*34-35*/
    UI8_T  rx_power_2[2];              /*36-37*/
    UI8_T  rx_power_3[2];              /*38-39*/
    UI8_T  rx_power_4[2];              /*40-41*/
    UI8_T  tx_bias_current_1[2];       /*42-43*/
    UI8_T  tx_bias_current_2[2];       /*44-45*/
    UI8_T  tx_bias_current_3[2];       /*46-47*/
    UI8_T  tx_bias_current_4[2];       /*48-49*/
    UI8_T  channel_monitor_set3[8];    /*50-57*/
    UI8_T  channel_monitor_set4[8];    /*58-65*/
    UI8_T  channel_monitor_set5[8];    /*66-73*/
    UI8_T  channel_monitor_set6[8];    /*74-81*/
    UI8_T  reserved_2[4];              /*82-85*/
    UI8_T  control[12];                /*86-97*/
    UI8_T  reserved_3[2];              /*98-99*/
    UI8_T  module_and_channel_masks[7];/*100-106*/
    UI8_T  reserved_4[12];             /*107-118*/
    UI8_T  pw_chg_entry_area[4];       /*119-122*/
    UI8_T  pw_entry_area[4];           /*123-126*/
    UI8_T  page_select_byte;           /*127*/
    UI8_T  temp_high_alarm[2];         /*128-129*/
    UI8_T  temp_low_alarm[2];          /*130-131*/
    UI8_T  temp_high_warning[2];       /*132-133*/
    UI8_T  temp_low_warning[2];        /*134-135*/
    UI8_T  reserved_5[8];              /*136-143*/
    UI8_T  voltage_high_alarm[2];      /*144-145*/
    UI8_T  voltage_low_alarm[2];       /*146-147*/
    UI8_T  voltage_high_warning[2];    /*148-149*/
    UI8_T  voltage_low_warning[2];     /*150-151*/
    UI8_T  reserved_6[8];              /*152-159*/
    UI8_T  vendor_specific[16];        /*160-175*/
    UI8_T  rx_power_high_alarm[2];     /*176-177*/
    UI8_T  rx_power_low_alarm[2];      /*178-179*/
    UI8_T  rx_power_high_warning[2];   /*180-181*/
    UI8_T  rx_power_low_warning[2];    /*182-183*/
    UI8_T  bias_high_alarm[2];         /*184-185*/
    UI8_T  bias_low_alarm[2];          /*186-187*/
    UI8_T  bias_high_warning[2];       /*188-189*/
    UI8_T  bias_low_warning[2];        /*190-191*/
    UI8_T  reserved_7[64];             /*192-255*/

} SWDRV_TYPE_QsfpDdmInfoRaw_T;
#pragma pack()

typedef struct SWDRV_TYPE_SfpDdmThreshold_S
{
    float temp_high_alarm;
    float temp_low_alarm;
    float temp_high_warning;
    float temp_low_warning;
    float voltage_high_alarm;
    float voltage_low_alarm;
    float voltage_high_warning;
    float voltage_low_warning;
    float bias_high_alarm;
    float bias_low_alarm;
    float bias_high_warning;
    float bias_low_warning;
    float tx_power_high_alarm;
    float tx_power_low_alarm;
    float tx_power_high_warning;
    float tx_power_low_warning;
    float rx_power_high_alarm;
    float rx_power_low_alarm;
    float rx_power_high_warning;
    float rx_power_low_warning;

} SWDRV_TYPE_SfpDdmThreshold_T;

typedef struct SWDRV_TYPE_SfpDdmThresholdInteger_S
{
    I16_T temp_high_alarm;
    I16_T temp_low_alarm;
    I16_T temp_high_warning;
    I16_T temp_low_warning;
    UI16_T voltage_high_alarm;
    UI16_T voltage_low_alarm;
    UI16_T voltage_high_warning;
    UI16_T voltage_low_warning;
    UI16_T bias_high_alarm;
    UI16_T bias_low_alarm;
    UI16_T bias_high_warning;
    UI16_T bias_low_warning;
    I16_T tx_power_high_alarm;
    I16_T tx_power_low_alarm;
    I16_T tx_power_high_warning;
    I16_T tx_power_low_warning;
    I16_T rx_power_high_alarm;
    I16_T rx_power_low_alarm;
    I16_T rx_power_high_warning;
    I16_T rx_power_low_warning;

} SWDRV_TYPE_SfpDdmThresholdInteger_T;

typedef struct SWDRV_TYPE_SfpDdmThresholdEntry_S
{
    SWDRV_TYPE_SfpDdmThreshold_T threshold;
    SWDRV_TYPE_SfpDdmThresholdInteger_T threshold_int;
    UI32_T ifindex;
    BOOL_T trap_enable;
    BOOL_T auto_mode;

} SWDRV_TYPE_SfpDdmThresholdEntry_T;

typedef struct SWDRV_TYPE_SfpDdmThresholdStatus_S
{
    UI8_T temp_high_alarm:1;
    UI8_T temp_low_alarm:1;
    UI8_T temp_high_warning:1;
    UI8_T temp_low_warning:1;
    UI8_T voltage_high_alarm:1;
    UI8_T voltage_low_alarm:1;
    UI8_T voltage_high_warning:1;
    UI8_T voltage_low_warning:1;
    UI8_T bias_high_alarm:1;
    UI8_T bias_low_alarm:1;
    UI8_T bias_high_warning:1;
    UI8_T bias_low_warning:1;
    UI8_T tx_power_high_alarm:1;
    UI8_T tx_power_low_alarm:1;
    UI8_T tx_power_high_warning:1;
    UI8_T tx_power_low_warning:1;
    UI8_T rx_power_high_alarm:1;
    UI8_T rx_power_low_alarm:1;
    UI8_T rx_power_high_warning:1;
    UI8_T rx_power_low_warning:1;
} SWDRV_TYPE_SfpDdmThresholdStatus_T;

typedef struct SWDRV_TYPE_SfpDdmInfoMeasured_S
{
    /* real time informations */
    float temperature;
    float voltage;
    float tx_bias_current;
    float tx_bias_current_2;
    float tx_bias_current_3;
    float tx_bias_current_4;
    float tx_power;
    float rx_power;
    float rx_power_2;
    float rx_power_3;
    float rx_power_4;
    BOOL_T rx_los_asserted;
} SWDRV_TYPE_SfpDdmInfoMeasured_T;

/* convert to double for application layer, log() & power() functions need to
 * use double type */
typedef struct SWDRV_TYPE_SfpDdmInfo_S
{
    SWDRV_TYPE_SfpDdmThreshold_T    threshold;
    SWDRV_TYPE_SfpDdmInfoMeasured_T measured;
} SWDRV_TYPE_SfpDdmInfo_T;

typedef struct SWDRV_IfTableStats_S
{
   UI64_T ifInOctets;
   UI64_T ifInUcastPkts;
   UI64_T ifInNUcastPkts;
   UI64_T ifInDiscards;
   UI64_T ifInErrors;
   UI64_T ifInUnknownProtos;
   UI64_T ifOutOctets;
   UI64_T ifOutUcastPkts;
   UI64_T ifOutNUcastPkts;
   UI64_T ifOutDiscards;
   UI64_T ifOutErrors;
   UI64_T ifOutQLen;
} SWDRV_IfTableStats_T;


typedef struct SWDRV_IfXTableStats_S
{
    UI64_T ifInMulticastPkts;
    UI64_T ifInBroadcastPkts;
    UI64_T ifOutMulticastPkts;
    UI64_T ifOutBroadcastPkts;
    UI64_T ifHCInOctets;
    UI64_T ifHCInUcastPkts;
    UI64_T ifHCInMulticastPkts;
    UI64_T ifHCInBroadcastPkts;
    UI64_T ifHCOutOctets;
    UI64_T ifHCOutUcastPkts;
    UI64_T ifHCOutMulticastPkts;
    UI64_T ifHCOutBroadcastPkts;
    UI32_T ifCounterDiscontinuityTime;
} SWDRV_IfXTableStats_T;



typedef struct SWDRV_RmonStats_S
{
   UI64_T etherStatsDropEvents;
   UI64_T etherStatsOctets;
   UI64_T etherStatsPkts;
   UI64_T etherStatsBroadcastPkts;
   UI64_T etherStatsMulticastPkts;
   UI64_T etherStatsCRCAlignErrors;
   UI64_T etherStatsUndersizePkts;
   UI64_T etherStatsOversizePkts;
   UI64_T etherStatsFragments;
   UI64_T etherStatsJabbers;
   UI64_T etherStatsCollisions;
   UI64_T etherStatsPkts64Octets;
   UI64_T etherStatsPkts65to127Octets;
   UI64_T etherStatsPkts128to255Octets;
   UI64_T etherStatsPkts256to511Octets;
   UI64_T etherStatsPkts512to1023Octets;
   UI64_T etherStatsPkts1024to1518Octets;
} SWDRV_RmonStats_T;

/* Added new element for RFC 3635
 * tc_wang 050819
 */
typedef struct SWDRV_EtherlikeStats_S
{
   UI64_T dot3StatsAlignmentErrors;
   UI64_T dot3StatsFCSErrors;
   UI64_T dot3StatsSingleCollisionFrames;
   UI64_T dot3StatsMultipleCollisionFrames;
   UI64_T dot3StatsSQETestErrors;
   UI64_T dot3StatsDeferredTransmissions;
   UI64_T dot3StatsLateCollisions;
   UI64_T dot3StatsExcessiveCollisions;
   UI64_T dot3StatsInternalMacTransmitErrors;
   UI64_T dot3StatsCarrierSenseErrors;
   UI64_T dot3StatsFrameTooLongs;
   UI64_T dot3StatsInternalMacReceiveErrors;
   UI64_T dot3StatsSymbolErrors;
   UI64_T dot3StatsDuplexStatus;
   UI32_T dot3StatsRateControlAbility;
   UI32_T dot3StatsRateControlStatus;
} SWDRV_EtherlikeStats_T;


typedef struct SWDRV_EtherlikePause_S
{
   UI32_T dot3PauseAdminMode;
   UI32_T dot3PauseOperMode;
   UI64_T dot3InPauseFrames;
   UI64_T dot3OutPauseFrames;
} SWDRV_EtherlikePause_T;


#if (SYS_CPNT_NMTR_PERQ_COUNTER == TRUE)
typedef struct
{
    struct
    {
        UI64_T ifOutOctets;
        UI64_T ifOutPkts;
        UI64_T ifOutDiscardOctets;
        UI64_T ifOutDiscardPkts;
    } cosq[SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE];
} SWDRV_IfPerQStats_T;
#endif

#if (SYS_CPNT_PFC == TRUE)
typedef struct
{
    UI64_T ieee8021PfcRequests;
    UI64_T ieee8021PfcIndications;
    struct
    {
        UI64_T ieee8021PfcRequests;
        UI64_T ieee8021PfcIndications;
    } pri[8];
} SWDRV_PfcStats_T;
#endif

#if (SYS_CPNT_CN == TRUE)
typedef struct
{
    struct
    {
       UI64_T qcnStatsOutCnms;
    } cpq[SYS_ADPT_CN_MAX_NBR_OF_CP_PER_PORT];
} SWDRV_QcnStats_T;
#endif

typedef struct
{
    struct {
        BOOL_T ifInOctets:1;
        BOOL_T ifInUcastPkts:1;
        BOOL_T ifInNUcastPkts:1;
        BOOL_T ifInDiscards:1;
        BOOL_T ifInErrors:1;
        BOOL_T ifInUnknownProtos:1;
        BOOL_T ifOutOctets:1;
        BOOL_T ifOutUcastPkts:1;
        BOOL_T ifOutNUcastPkts:1;
        BOOL_T ifOutDiscards:1;
        BOOL_T ifOutErrors:1;
        BOOL_T ifOutQLen:1;
    } if_table;
    struct {
        BOOL_T ifInMulticastPkts:1;
        BOOL_T ifInBroadcastPkts:1;
        BOOL_T ifOutMulticastPkts:1;
        BOOL_T ifOutBroadcastPkts:1;
        BOOL_T ifHCInOctets:1;
        BOOL_T ifHCInUcastPkts:1;
        BOOL_T ifHCInMulticastPkts:1;
        BOOL_T ifHCInBroadcastPkts:1;
        BOOL_T ifHCOutOctets:1;
        BOOL_T ifHCOutUcastPkts:1;
        BOOL_T ifHCOutMulticastPkts:1;
        BOOL_T ifHCOutBroadcastPkts:1;
    } ifx_table;
    struct {
       BOOL_T etherStatsDropEvents:1;
       BOOL_T etherStatsOctets:1;
       BOOL_T etherStatsPkts:1;
       BOOL_T etherStatsBroadcastPkts:1;
       BOOL_T etherStatsMulticastPkts:1;
       BOOL_T etherStatsCRCAlignErrors:1;
       BOOL_T etherStatsUndersizePkts:1;
       BOOL_T etherStatsOversizePkts:1;
       BOOL_T etherStatsFragments:1;
       BOOL_T etherStatsJabbers:1;
       BOOL_T etherStatsCollisions:1;
       BOOL_T etherStatsPkts64Octets:1;
       BOOL_T etherStatsPkts65to127Octets:1;
       BOOL_T etherStatsPkts128to255Octets:1;
       BOOL_T etherStatsPkts256to511Octets:1;
       BOOL_T etherStatsPkts512to1023Octets:1;
       BOOL_T etherStatsPkts1024to1518Octets:1;
    } rmon;
    struct {
        BOOL_T dot3StatsAlignmentErrors:1;
        BOOL_T dot3StatsFCSErrors:1;
        BOOL_T dot3StatsSingleCollisionFrames:1;
        BOOL_T dot3StatsMultipleCollisionFrames:1;
        BOOL_T dot3StatsSQETestErrors:1;
        BOOL_T dot3StatsDeferredTransmissions:1;
        BOOL_T dot3StatsLateCollisions:1;
        BOOL_T dot3StatsExcessiveCollisions:1;
        BOOL_T dot3StatsInternalMacTransmitErrors:1;
        BOOL_T dot3StatsCarrierSenseErrors:1;
        BOOL_T dot3StatsFrameTooLongs:1;
        BOOL_T dot3StatsInternalMacReceiveErrors:1;
        BOOL_T dot3StatsSymbolErrors:1;
    } ether_like;
    struct {
       BOOL_T dot3InPauseFrames:1;
       BOOL_T dot3OutPauseFrames:1;
    } ether_like_pause;
} SWDRV_StatsAbility_T;


/*  AMTRDRV_Address_Table_Entry_T
 */
typedef struct
{
#if (SYS_CPNT_MAC_IN_MAC == TRUE)
    UI32_T vid;             /* primary key. also used for hw_idx of VFI */
    UI32_T port;            /* if port is hw_idx of VP, unit/trunk_id/is_trunk is irreveant */
    UI8_T  mac[6];          /* 2nd key     */
    UI8_T  unit;
#else
    UI16_T vid;             /* primary key. also used for hw_idx of VFI */
    UI8_T  mac[6];          /* 2nd key     */
    UI8_T  unit;
    UI8_T  port;            /* if port is hw_idx of VP, unit/trunk_id/is_trunk is irreveant */
#endif
    UI8_T  trunk_id;
    UI8_T  priority;
    BOOL_T is_trunk;
    BOOL_T is_static;
    BOOL_T discard_by_sa_match;
    BOOL_T discard_by_da_match;
} SWDRV_TYPE_L2_Address_Info_T;

typedef struct
{
    UI8_T   stack_id;               /* the id in a stacking */
   /* UI8_T   operation_mode;         which mode operating now : Jeff MASK */

    UI16_T  base_port_id;
    UI16_T  port_number;            /* total number of ports */
    UI32_T  num_of_units;
#if (SYS_CPNT_STACKING == TRUE)
    UI32_T  stack_unit_tbl[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];
    UI32_T  stacking_port;
#endif /*SYS_CPNT_STACKING*/
} SWDRV_Switch_Info_T;

typedef struct
{
    UI32_T  port_type;              /* the type of this port */
    BOOL_T  is_autoneg;             /* for combo port work around, copper port cannot link up
                                     * while fiber port swtich to copper port */
    UI8_T   link_status;            /* link UP/DOWN status */
    UI8_T   speed_duplex_oper;      /* operation status of speed and duplex */
    UI8_T   flow_control_oper;      /* last flow control status */
    BOOL_T  existing;               /* this port is existing or not */

#if (SYS_CPNT_COMBO_PORT_FORCE_MODE == TRUE)
    UI32_T forcedmode;
#if (SYS_CPNT_COMBO_PORT_FORCED_MODE_SFP_SPEED == TRUE)
    UI32_T fiber_speed;
#endif
#endif

#if (SYS_CPNT_SUPPORT_FORCED_1000BASE_T_MODE == TRUE)
    UI8_T   speed_duplex_cfg;       /* keep value for 1000BASE-T work around to re-store */
    UI8_T   forced_1000t_mode;      /* keep value for 1000BASE-T work around to re-store */
#endif

#if (SYS_CPNT_SWCTRL_MDIX_CONFIG == TRUE)
    BOOL_T  port_admin;
    UI8_T   mdix_status; /* correct mdix status */
#endif

    UI8_T   module_id;
} SWDRV_Port_Info_T;

typedef struct
{
    UI8_T   link_status;            /* link UP/DOWN status */
} SWDRV_CraftPort_Info_T;

typedef struct
{
    UI8_T previous_link_st_bitmap[SWDRV_TOTAL_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];    /* bitmap of announced link status. */
    UI8_T link_st_bitmap[SWDRV_TOTAL_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];             /* bitmap of link status updated from chip. */
}SWDRV_LinkStatus_T;

typedef struct
{
    UI8_T previous_sfp_present_st_bitmap[SWDRV_TOTAL_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];    /* bitmap of announced sfp present status. */
    UI8_T sfp_present_st_bitmap[SWDRV_TOTAL_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];             /* bitmap of sfp present status updated from chip. */
}SWDRV_TYPE_SfpPresentStatus_T;

#if (SYS_CPNT_MAC_IN_MAC == TRUE)
/* for MAC-in-MAC
 *
 * NOTE: should sync to dev_swdrv.h
 */
typedef struct
{
    UI32_T isid;

   /* output param for creation; input param for deletion
    *
    * for delection,
    * 1) fill all field except hw_idx or
    * 2) specify hw_idx and zero other field
    */
    UI32_T hw_idx;
} SWDRV_MimServiceInfo_T;

typedef enum
{
    SWDRV_MIM_PORT_TYPE_HWIDX,
    SWDRV_MIM_PORT_TYPE_ACCESS,
    SWDRV_MIM_PORT_TYPE_BACKBONE,
} SWDRV_MimPortType_T;

typedef struct
{
    UI32_T isid;
    UI32_T unit;
    UI32_T port;
    SWDRV_MimPortType_T port_type;
    union {
        struct {
            UI32_T svid;
            UI32_T cvid;
        } access;
        struct {
            UI32_T egr_vid;
            UI8_T egr_src_addr[SYS_ADPT_MAC_ADDR_LEN];
            UI8_T egr_dst_addr[SYS_ADPT_MAC_ADDR_LEN];
        } backbone;
    } ext;

   /* output param for creation; input param for deletion
    *
    * to delete port by hw_idx,
    * specify port_type = HWIDX and hw_idx of port to delete.
    */
    UI32_T hw_idx;
} SWDRV_MimPortInfo_T;
#endif /* (SYS_CPNT_MAC_IN_MAC == TRUE) */

typedef struct {
    int medium_copper:1;
    int medium_fiber_100m:1;
    int medium_fiber_1000m:1;
    int medium_fiber_10g:1;
    int medium_fiber_40g:1;

    int spd_10m_hd:1;
    int spd_10m_fd:1;
    int spd_100m_hd:1;
    int spd_100m_fd:1;
    int spd_1000m_hd:1;
    int spd_1000m_fd:1;
    int spd_10g_hd:1;
    int spd_10g_fd:1;
    int spd_40g_hd:1;
    int spd_40g_fd:1;

    int pause:1;
    int pause_asymm:1;

    int autoneg:1;
} SWDRV_PortAbility_T;

/* struct definiation
 */
typedef struct
{
    UI32_T   ServiceID;      /* Service ID  */
    UI32_T   SubServiceID;      /* Service ID  */
    UI32_T   unit;           /* stack id (unit number) */
    UI32_T   port;           /* port number */
    UI32_T   option_module;  /* option module */
    UI32_T   option_num_port;
#if (SYS_CPNT_SWCTRL_MDIX_CONFIG == TRUE)
    UI32_T   MDIXMode;  /* MDIX mode */
#endif
    UI32_T   mtu;
    UI8_T    portlist[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
    union
    {
        UI32_T               return_value;
        UI32_T               general_32_bits; /*For general 32-bits parameter*/

#if (SYS_CPNT_3COM_LOOPBACK_TEST == TRUE)
        UI8_T                loopback_test_result[SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST*SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];
#endif

        struct
        {
        UI32_T               comboforcedmode;
    #if (SYS_CPNT_COMBO_PORT_FORCED_MODE_SFP_SPEED == TRUE)
            UI32_T               combo_port_forced_mode_speed;
    #endif
        }  __attribute__((packed, aligned(1)))combo_config;

        UI8_T                port_list[SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];
        UI8_T                cpu_mac_addr[6];

        struct
        {   /* Ingress rate limit or Egress rate limit */
            UI32_T           rate;      /* Ingress rate limit or Egress rate limit */
            UI32_T           trunk_id;  /* Ingress rate limit trunk id */
        } __attribute__((packed, aligned(1)))rate_limit;


        struct
        {   /* bstorm Configuration */
            UI32_T               storm_control_threshold; /* strom control threshold */
            UI32_T               storm_control_mode;      /* broadcast storm mode */
        } __attribute__((packed, aligned(1)))bstorm_config;

        struct
        {   /* Port Configuration */
            UI32_T speed_duplex;          /* port speed duplex  */
            UI32_T force_1000t_mode;      /* 100baseT forced mode */
            UI32_T capability;            /* autoneg capability */
            UI32_T vid;                   /* vlan ID            */
            UI32_T port_type;             /* port type          */
            UI32_T state;                 /* STA state          */
        /* #if (SYS_CPNT_STP == SYS_CPNT_STP_TYPE_MSTP) */
            UI32_T mstid;
            UI32_T vlan_count;
        /*changed by fen.wang*/
        #if 0
            UI16_T vlan_list[SYS_ADPT_MAX_NBR_OF_VLAN];
        #else
            UI8_T vlan_list[(SYS_ADPT_MAX_NBR_OF_VLAN/8)+1];
        #endif
            UI32_T unit_id;
      /*  #endif    */
       /*  #if (SYS_CPNT_SUPPORT_FORCED_1000BASE_T_MODE == TRUE)    */
            UI32_T copper_energy_detect;
       /*  #endif    */
        }__attribute__((packed, aligned(1))) port_config;

        struct
        {   /* VLAN */
            UI32_T pvid;                  /* permanent Vlan ID */
            UI32_T vid;                   /* vlan ID */
        } __attribute__((packed, aligned(1)))vlan;

#if (SYS_CPNT_QINQ == TRUE)
        struct
        {   /* Dot1q-tunnel (QinQ) */
            UI8_T  mode;
            UI32_T tpid;
            UI32_T status;
        } dot1q_tunnel;
#endif

        struct
        {   /* Port Mirroring */
            SYS_TYPE_Uport_T from_port;   /* source port to mirror */
            SYS_TYPE_Uport_T rx_to_port;  /* port to mirror the received packet */
            SYS_TYPE_Uport_T tx_to_port;  /* port to mirror the transmitted packet */
        } __attribute__((packed, aligned(1)))port_mirror;

#if (SYS_CPNT_VLAN_MIRROR == TRUE)
        struct
        {   /* VLAN Mirror */
            UI32_T unit;  /* unit_id*/
            UI32_T port;  /* destination port */
            UI32_T vid;   /* vlan id */
        } __attribute__((packed, aligned(1)))vlan_mirror;
#endif /* End of #if (SYS_CPNT_VLAN_MIRROR == TRUE) */

#if (SYS_CPNT_MAC_BASED_MIRROR == TRUE)
        struct /* Mac-based Mirror */
        {
            UI8_T mac[SYS_ADPT_MAC_ADDR_LEN];
        } __attribute__((packed, aligned(1)))mac_mirror;
#endif /* end of #if (SYS_CPNT_MAC_BASED_MIRROR == TRUE) */

        struct /* Mac-based Mirror */
        {
            UI32_T unit;
            UI32_T port;
            BOOL_T mode;
        } __attribute__((packed, aligned(1)))set_mirror_dest_port;

        struct
        {   /* Trunking */
            UI32_T trunk_id;              /* trunk ID */
            UI32_T port_count;            /* number of trunk members */
            UI32_T trunk_mode;            /* trunk mode */
            UI8_T  mac[6];                /* MAC address */
            UI32_T vid;                   /* vlan ID */
            SYS_TYPE_Uport_T trunk_member;/* trunk member list */
            SYS_TYPE_Uport_T port_member[SYS_ADPT_MAX_NBR_OF_PORT_PER_TRUNK]; /* trunk member list */
        }__attribute__((packed, aligned(1))) trunking;

        struct
        {   /* IGMP */
            UI32_T vid;                   /* Vlan ID */
            UI32_T trunk_id;              /* trunk ID */
        } __attribute__((packed, aligned(1)))igmp;

        struct
        {   /* QoS */
            UI32_T priority;              /* user default priority */
            UI8_T  mapping[8];            /* priority mapping */
        }__attribute__((packed, aligned(1))) qos;

        struct
        {   /* Traffic Segematation */
            SYS_TYPE_Uport_T    uplink_uport_list[SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT];      /* uplink port list for Traffic Segematation   */
            UI32_T              uplink_uport_count;
            SYS_TYPE_Uport_T    downlink_uport_list[SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT];    /* downlink port list for Traffic Segematation */
            UI32_T              downlink_uport_count;
        }__attribute__((packed, aligned(1))) traffic_segematation;

        struct
        {   /* Traffic Segematation By Port list */
            UI8_T  uplink_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST]; /* uplink port list for Traffic Segematation   */
            UI8_T  downlink_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST]; /* downlink port list for Traffic Segematation */
        } __attribute__((packed, aligned(1)))traffic_segematation_by_port_list;

#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE)
#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION)
        struct
        {   /* Private vlan port list by session group */
            UI32_T session_id;
            UI8_T  uplink_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
            UI8_T  downlink_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
        } private_vlan_port_list_by_session_group;
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION)*/
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE) */

        struct
        {   /* WRR Queue */
            UI32_T method;                /* egress scheduling method */
            UI32_T queue_id;              /* WRR queue ID */
            UI32_T queue_weight;          /* WRR queue weight */
        } __attribute__((packed, aligned(1)))wrr_queue;

        struct
        {   /* Event Notification Information */
            UI32_T port_type;             /* slave change port type */
            UI32_T speed_duplex;          /* slave change port speed duplex */
            UI32_T flow_control;          /* slave change port flow control */
            UI32_T module_id;             /* slave change module id */
        } __attribute__((packed, aligned(1)))notify;

        struct
        {   /* Protocol VLAN */
            UI32_T group_index;           /* protocol vlan group index */
            UI32_T vlan_index;            /* vlan id */
            UI32_T nbr_of_type_protocol;  /* prorocol type number */
            UI8_T  frame_type[SYS_ADPT_1V_MAX_NBR_OF_PROTOCOL_GROUP];  /* frame type */
            UI8_T  protocol_value[SYS_ADPT_1V_MAX_NBR_OF_PROTOCOL_GROUP][DEV_SWDRV_MAX_1V_PROTOCOL_VALUE_LENGTH];
            UI8_T  priority;
        } __attribute__((packed, aligned(1)))protocol_vlan;

        struct
        {   /* My MAC IP To CPU */
            UI32_T my_ip;                           /* my ip address  */
            UI8_T  my_mac[SYS_ADPT_MAC_ADDR_LEN];   /* my mac address */
        } __attribute__((packed, aligned(1)))my_mac_ip;

    #if (SYS_CPNT_DOT1X == TRUE)
        struct
        {
            UI32_T trap_mode;    /* DEV_SWDRV_DOT1X_PACKET_DISCARD = 0,
                               DEV_SWDRV_DOT1X_PACKET_FORWARD,
                               DEV_SWDRV_DOT1X_PACKET_TRAPTOCPU */
            UI32_T control_mode; /*add for set dot1x control mode -- 20090112*/
            BOOL_T eapol_frame_pass_through;
        }__attribute__((packed, aligned(1))) dot1x_auth;
    #endif /* SYS_CPNT_DOT1X == TRUE */

        /* port link status;water_huang;95_9_13
         */
        UI8_T new_link_st_bitmap[SWDRV_TOTAL_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];
        UI8_T new_sfp_present_st_bitmap[SWDRV_TOTAL_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];

        struct
        {
            UI32_T vid;
            UI8_T  port_list[SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];
        }__attribute__((packed, aligned(1)))vlan_set;

#if (SYS_CPNT_SWCTRL_CABLE_DIAG == TRUE)
        SWDRV_CableDiagInfo_T cable_diag_info;
#endif

#if (SYS_CPNT_RATE_BASED_STORM_CONTROL == TRUE)
        struct
        {
            UI32_T rate;
            UI32_T mode;
        } __attribute__((packed, aligned(1)))rate_based_storm ;
#endif

    #if (SYS_CPNT_EFM_OAM == TRUE)
        struct
        {
            UI32_T  type;
            BOOL_T  is_enable;
        }__attribute__((packed, aligned(1))) efm_oam;
    #endif /* #if (SYS_CPNT_EFM_OAM == TRUE) */

#if (SYS_CPNT_INTERNAL_LOOPBACK_TEST == TRUE)
        struct
        {
            BOOL_T  is_enable;
        }__attribute__((packed, aligned(1))) internal_loopback_status;
#endif


#if (SYS_CPNT_MAC_VLAN == TRUE)
        struct
        {
            UI8_T mac_addr[SYS_ADPT_MAC_ADDR_LEN];
            UI8_T mask[SYS_ADPT_MAC_ADDR_LEN];
            UI16_T vid;
            UI8_T priority;
        }__attribute__((packed, aligned(1))) mac_vlan_entry;
#endif


#if (SYS_CPNT_IP_SUBNET_VLAN == TRUE)
        struct
        {
            UI32_T ip_addr;
            UI32_T mask;
            UI16_T vid;
            UI8_T priority;
        }__attribute__((packed, aligned(1))) ip_subnet_vlan_entry;

#endif

#if(SYS_CPNT_VLAN_TRUNKING == TRUE)
        struct
        {
            UI32_T port;
            UI32_T vid;
            BOOL_T enable;
        }__attribute__((packed, aligned(1))) port_vlan_truking;

        struct
        {
            UI32_T vid_list[(SYS_ADPT_MAX_VLAN_ID + 31) / 32];
            UI8_T port_list[SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST * SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];
            BOOL_T enable;
        }__attribute__((packed, aligned(1))) port_vlan_truking_list;

        struct
        {
            UI32_T vid;
            UI8_T portlist[SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST * SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];
        }__attribute__((packed, aligned(1))) create_vlan_truking;
#endif
#if (SYS_CPNT_QINQ_PRIORITY_MAP == TRUE)
        UI32_T copy_inner_pri_to_outer_status;
#endif
#if (SYS_CPNT_POWER_SAVE == TRUE) || (SYS_CPNT_EEE == TRUE)
        struct
        {
            UI32_T unit;
            UI32_T port;
            BOOL_T status;
            BOOL_T link_status;
        }__attribute__((packed, aligned(1))) port_power_saving;
#endif

#if (SYS_CPNT_SFLOW == TRUE)
        struct
        {
            UI32_T rate;         /*the rate of sflow*/
            BOOL_T enable;
        }__attribute__((packed, aligned(1))) sflow_config;
#endif

        SWDRV_TYPE_SfpInfo_T            sfp_info;
        SWDRV_TYPE_SfpDdmInfo_T         sfp_ddm_info;

#if (SYS_CPNT_ERPS == TRUE)
        UI32_T  erps_port_state;
#endif

        struct
        {
            UI32_T pkt_type;
            BOOL_T to_cpu;
            BOOL_T flood;
        }__attribute__((packed, aligned(1))) packet_trap_info;

        struct
        {
            UI32_T pkt_type;
            BOOL_T to_cpu;
            BOOL_T flood;
            UI32_T vid;
        }__attribute__((packed, aligned(1))) packet_trap_byvlan_info;

#if (SYS_CPNT_SWCTRL_QINQ_SERVICE == TRUE)
#if (SYS_CPNT_SWCTRL_QINQ_SERVICE_VLAN_XLATE_BY_RULE != TRUE)
        struct
        {
            UI32_T old_vid;
            UI32_T new_vid;
            UI32_T priority;
            BOOL_T add_tag;
            BOOL_T enable;
        }__attribute__((packed, aligned(1))) vlan_translation;
#endif
#endif

        struct
        {
            UI32_T u32_a1;
            UI32_T u32_a2;
        }u32_u32;

        struct
        {
            UI32_T u32_a1;
            UI32_T u32_a2;
            UI32_T u32_a3;
        }u32_u32_u32;

        struct
        {
            UI32_T u32_a1;
            UI32_T u32_a2;
            BOOL_T bool_a1;
        }u32_u32_b;

        struct
        {
            UI32_T u32_a1;
            BOOL_T bool_a1;
        }u32_b;

#if (SYS_CPNT_IPV6_RA_GUARD == TRUE)
        struct
        {
            BOOL_T  is_enabled;
        }__attribute__((packed, aligned(1))) set_ra_and_rr_trap;
#endif /* #if (SYS_CPNT_IPV6_RA_GUARD == TRUE) */

#if (SYS_CPNT_ETS == TRUE)
        struct
        {
            UI32_T cosq2group[SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE];
            BOOL_T cosq2group_is_valid;
        }cos_group_mapping;

        struct
        {
            UI32_T method;
            UI32_T weights[SYS_ADPT_ETS_MAX_NBR_OF_TRAFFIC_CLASS];
            BOOL_T weights_is_valid;
        }cos_group_scheduling;
#endif

#if (SYS_CPNT_MAC_IN_MAC == TRUE)
        struct
        {
            SWDRV_MimServiceInfo_T mim;
            BOOL_T is_valid;
        }mim_service;

        struct
        {
            SWDRV_MimPortInfo_T mim_port;
            BOOL_T is_valid;
        }mim_service_port;
#endif
    }__attribute__((packed, aligned(1))) info;

}__attribute__((packed, aligned(1))) SWDRV_Rx_IscBuf_T;

typedef struct
{
    union
    {
        UI32_T u32;
        BOOL_T bool;
    } return_value;

    union
    {
        UI8_T  u8;
        UI32_T u32;
        struct
        {
            UI32_T u32_1;
            UI32_T u32_2;
        }u32x2;
#if (SYS_CPNT_SWCTRL_CABLE_DIAG == TRUE)
        SWDRV_CableDiagInfo_T cable_diag;
#endif
#if (SYS_CPNT_CN == TRUE)
        UI8_T port_qcn_cpid[8];
#endif
        SWDRV_PoweredDeviceInfo_T pd_info;
        SWDRV_PortAbility_T   port_ability;
    } __attribute__((packed, aligned(1))) data;
}__attribute__((packed, aligned(1))) SWDRV_IscReplyBuf_T;

typedef struct
{
    UI32_T              bcmdrv_trunk_id;  /* trunk id got from bcmdrv */
    UI8_T               used;             /* this trunk is assigned or not */
    UI32_T              member_number;    /* number of trunk member */
    SYS_TYPE_Uport_T    member_list[SYS_ADPT_MAX_NBR_OF_PORT_PER_TRUNK]; /* trunk member list */
} SWDRV_Trunk_Info_T;

typedef struct
{
    SYSFUN_DECLARE_CSC_ON_SHMEM

    UI32_T debug_flag; /* SWDRV_TYPE_DEBUG_FLAG_ */

    BOOL_T               swdrv_chip_task_is_created;

    SWDRV_Switch_Info_T  swdrv_system_info;
    SWDRV_Port_Info_T    swdrv_port_info[SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT + 1];
    SWDRV_CraftPort_Info_T    swdrv_craft_port_info;
    SWDRV_Trunk_Info_T   swdrv_trunk_info[SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM + 1];

    UI32_T swdrv_trunk_mode;

    BOOL_T swdrv_provision_complete;

    /* Each Slave(unit2~8) only uses the specific position of this array. ex. Unit2 only uses swdrv_port_link_status[1].
     * Master(unit1) will record local port link status in swdrv_port_link_status[0].
     * When receiving the port link status from Slave, Master will copy these information to swdrv_port_link_status[2~8].
     * water_huang;95_9_13
     */
    SWDRV_LinkStatus_T swdrv_port_link_status[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];

    UI32_T swdrv_sfp_present_delay_ticks;

}SWDRV_TYPE_ShmemData_T;

/* MACRO FUNCTION DECLARATIONS
 */
/* Protocol vlan */
#define protocl_vlan_group_index            info.protocol_vlan.group_index
#define protocl_vlan_vlan_index             info.protocol_vlan.vlan_index
#define protocl_vlan_nbr_of_type_protocol   info.protocol_vlan.group_index
#define protocl_vlan_frame_type             info.protocol_vlan.frame_type
#define protocl_vlan_protocol_value         info.protocol_vlan.protocol_value
#define protocl_vlan_priority               info.protocol_vlan.priority

/* For general 32-bits parameter */
/* For general purpose.
 * Any API that use 32-bit parameter could use this field to do remote call.
 */
#define general_32_bits_parameter    info.general_32_bits


#if (SYS_CPNT_3COM_LOOPBACK_TEST == TRUE)
#define lbk_test_result             info.loopback_test_result
#endif

#define ratelimit                    info.rate_limit.rate
#define ratelimit_trunkid            info.rate_limit.trunk_id

/* Storm Control */
#define storm_control_threshold      info.bstorm_config.storm_control_threshold
#define storm_control_mode           info.bstorm_config.storm_control_mode

/* Force mode */
#define combo_forcedmode             info.combo_config.comboforcedmode

#if (SYS_CPNT_COMBO_PORT_FORCED_MODE_SFP_SPEED == TRUE)
#define combo_forced_mode_speed             info.combo_config.combo_port_forced_mode_speed
#endif

#define cpu_mac_addr                 info.cpu_mac_addr

/* unknown IP multicast forwarding port list */
#define unknown_mcast_fwd_port_list  info.port_list

/* Port configuration */
#define port_cfg_speed_duplex        info.port_config.speed_duplex
#define port_cfg_force_1000t_mode    info.port_config.force_1000t_mode
#define port_cfg_capability          info.port_config.capability
#define port_cfg_vid                 info.port_config.vid
#define port_cfg_type                info.port_config.port_type
#define port_cfg_state               info.port_config.state
/*#if (SYS_CPNT_STP == SYS_CPNT_STP_TYPE_MSTP)*/
#define port_cfg_mstid               info.port_config.mstid
#define port_cfg_vlan_count          info.port_config.vlan_count
#define port_cfg_vlan_list           info.port_config.vlan_list
#define port_cfg_unit_id             info.port_config.unit_id
/*#endif */
#define port_cfg_copper_energy_detect info.port_config.copper_energy_detect
/* VLAN */
#define vlan_pvid                    info.vlan.pvid
#define vlan_vid                     info.vlan.vid

#if (SYS_CPNT_SFLOW == TRUE)
#define sflow_cfg_enable           info.sflow_config.enable
#define sflow_cfg_rate           info.sflow_config.rate
#endif

#if (SYS_CPNT_QINQ == TRUE)
/* Dot1q-tunnel (QinQ) */
#define dot1qtunnel_status           info.dot1q_tunnel.status
#define dot1qtunnel_mode             info.dot1q_tunnel.mode
#define dot1qtunnel_tpid             info.dot1q_tunnel.tpid
#endif

/* Port Mirroring */
#define port_mirror_src              info.port_mirror.from_port
#define port_mirror_rx               info.port_mirror.rx_to_port
#define port_mirror_tx               info.port_mirror.tx_to_port

#if (SYS_CPNT_VLAN_MIRROR == TRUE)
/* vlan Mirror */
#define vlan_mirror_unit             info.vlan_mirror.unit
#define vlan_mirror_port             info.vlan_mirror.port
#define vlan_mirror_vid              info.vlan_mirror.vid
#endif /* End of #if (SYS_CPNT_VLAN_MIRROR == TRUE) */

#if (SYS_CPNT_MAC_BASED_MIRROR == TRUE)
#define mac_mirror_mac               info.mac_mirror.mac
#endif

#define set_mirror_dest_port_unit    info.set_mirror_dest_port.unit
#define set_mirror_dest_port_port    info.set_mirror_dest_port.port
#define set_mirror_dest_port_mode    info.set_mirror_dest_port.mode

/* Trunking */
#define trunking_id                  info.trunking.trunk_id
#define trunking_port_count          info.trunking.port_count
#define trunking_mode                info.trunking.trunk_mode
#define trunking_mac                 info.trunking.mac
#define trunking_vid                 info.trunking.vid
#define trunking_member              info.trunking.trunk_member
#define trunking_port_member         info.trunking.port_member

/* IGMP */
#define igmp_igmp_mode               info.igmp.igmp_mode
#define igmp_trunk_id                info.igmp.trunk_id

/* Quality of Service */
#define qos_priority                 info.qos.priority
#define qos_mapping                  info.qos.mapping

/* Traffic Segematation */
#define traffic_segematation_uplink_uport_list       info.traffic_segematation.uplink_uport_list
#define traffic_segematation_downlink_uport_list     info.traffic_segematation.downlink_uport_list
#define traffic_segematation_uplink_uport_count      info.traffic_segematation.uplink_uport_count
#define traffic_segematation_downlink_uport_count    info.traffic_segematation.downlink_uport_count
#define traffic_segematation_by_port_list_uplink_port_list       info.traffic_segematation_by_port_list.uplink_port_list
#define traffic_segematation_by_port_list_downlink_port_list     info.traffic_segematation_by_port_list.downlink_port_list

/* Private vlan port list by session group */
#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE)
#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION)
#define private_vlan_port_list_by_session_group_session_id             info.private_vlan_port_list_by_session_group.session_id
#define private_vlan_port_list_by_session_group_uplink_port_list       info.private_vlan_port_list_by_session_group.uplink_port_list
#define private_vlan_port_list_by_session_group_downlink_port_list     info.private_vlan_port_list_by_session_group.downlink_port_list
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION)*/
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE) */

/* WRR Queue */
#define egress_method                info.wrr_queue.method
#define wrr_queue_id                 info.wrr_queue.queue_id
#define wrr_queue_weight             info.wrr_queue.queue_weight

/* Event Notification */
#define event_port_type              info.notify.port_type
#define event_speed_duplex           info.notify.speed_duplex
#define event_flow_control           info.notify.flow_control
#define event_module_id              info.notify.module_id

#if (SYS_CPNT_DOT1X == TRUE)
#define dot1x_auth_trap_mode         info.dot1x_auth.trap_mode
#define dot1x_auth_control_mode      info.dot1x_auth.control_mode
#endif /* SYS_CPNT_DOT1X == TRUE */

#if (SYS_CPNT_RATE_BASED_STORM_CONTROL == TRUE)
#define storm_rate        info.rate_based_storm.rate
#define storm_mode        info.rate_based_storm.mode
#endif

#define SWDRV_LPORT_INDEX(lport)            (((lport)-1)/8)
#define SWDRV_LPORT_BIT_IN_UI8_T(lport)     (1 << ( 7 - (((lport)-1)%8)))
#define SWDRV_UNIT_TO_UNITBMP(unit_id)      ((0x01) << (unit_id-1))

#endif

