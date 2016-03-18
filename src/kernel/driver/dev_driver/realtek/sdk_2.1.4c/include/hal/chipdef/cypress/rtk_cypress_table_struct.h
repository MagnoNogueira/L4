/*
 * Include Files
 */
#include <common/rt_autoconf.h>

/* Superset Table Enum */
typedef enum rtk_cypress_table_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MCt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_SIPt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_MCt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UCt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MCt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_SIPt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_MCt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOPt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NH_LEGACYt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UCt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MC_PMSKt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_UNTAGt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLANt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVTt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVTt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASEDt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASEDt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTIt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_METERt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_LOGt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Qt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHEDt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACLt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACLt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORTt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_ROUTINGt,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MPLS_LIBt,
#endif

    CYPRESS_TABLE_LIST_END,
} rtk_cypress_table_list_t;


/* Internal Table Enum */
typedef enum rtk_int_cypress_table_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_L2_CAM_IP_MC_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_L2_CAM_IP_MC_SIP_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_L2_CAM_MC_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_L2_CAM_UC_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_L2_IP_MC_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_L2_IP_MC_SIP_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_L2_MC_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_L2_NEXT_HOP_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_L2_NH_LEGACY_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_L2_UC_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_MC_PMSK_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_UNTAG_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_VLAN_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_VLAN_EGR_CNVT_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_VLAN_IGR_CNVT_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_VLAN_IP_SUBNET_BASED_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_VLAN_MAC_BASED_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_MSTI_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_METER_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_LOG_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_OUT_Q_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_SCHED_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_EACL_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_IACL_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_SPG_PORT_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_ROUTING_RTL8390,
#endif
#if defined(CONFIG_SDK_RTL8390)
    INT_CYPRESS_MPLS_LIB_RTL8390,
#endif

    INT_CYPRESS_TABLE_LIST_END,
} rtk_int_cypress_table_list_t;


typedef enum rtk_cypress_l2_cam_ip_mcField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_ZEROtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_GIPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_MC_PMSK_IDXtf,
#endif

    CYPRESS_L2_CAM_IP_MCFIELD_LIST_END,
} rtk_cypress_l2_cam_ip_mcField_list_t;


typedef enum rtk_cypress_l2_cam_ip_mc_sipField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_SIP_SIPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_SIP_GIPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_SIP_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_SIP_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_SIP_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_IP_MC_SIP_MC_PMSK_IDXtf,
#endif

    CYPRESS_L2_CAM_IP_MC_SIPFIELD_LIST_END,
} rtk_cypress_l2_cam_ip_mc_sipField_list_t;


typedef enum rtk_cypress_l2_cam_mcField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_MC_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_MC_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_MC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_MC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_MC_MC_PMSK_IDXtf,
#endif

    CYPRESS_L2_CAM_MCFIELD_LIST_END,
} rtk_cypress_l2_cam_mcField_list_t;


typedef enum rtk_cypress_l2_cam_ucField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_SLPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_AGEtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_SA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_DA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_STATICtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_SUSPENDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_NEXT_HOPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_CAM_UC_VIDtf,
#endif

    CYPRESS_L2_CAM_UCFIELD_LIST_END,
} rtk_cypress_l2_cam_ucField_list_t;


typedef enum rtk_cypress_l2_ip_mcField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_ZEROtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_GIPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_MC_PMSK_IDXtf,
#endif

    CYPRESS_L2_IP_MCFIELD_LIST_END,
} rtk_cypress_l2_ip_mcField_list_t;


typedef enum rtk_cypress_l2_ip_mc_sipField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_SIP_SIPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_SIP_GIPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_SIP_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_SIP_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_SIP_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_IP_MC_SIP_MC_PMSK_IDXtf,
#endif

    CYPRESS_L2_IP_MC_SIPFIELD_LIST_END,
} rtk_cypress_l2_ip_mc_sipField_list_t;


typedef enum rtk_cypress_l2_mcField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_MC_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_MC_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_MC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_MC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_MC_MC_PMSK_IDXtf,
#endif

    CYPRESS_L2_MCFIELD_LIST_END,
} rtk_cypress_l2_mcField_list_t;


typedef enum rtk_cypress_l2_next_hopField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_RSVDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_SLPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_AGEtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_SA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_DA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_STATICtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_SUSPENDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_NEXT_HOPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_VLAN_TARGETtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NEXT_HOP_ROUTE_IDXtf,
#endif

    CYPRESS_L2_NEXT_HOPFIELD_LIST_END,
} rtk_cypress_l2_next_hopField_list_t;


typedef enum rtk_cypress_l2_nh_legacyField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NH_LEGACY_HOSTMAC_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NH_LEGACY_VID_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NH_LEGACY_DVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NH_LEGACY_DPAtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_NH_LEGACY_NEXT_HOPtf,
#endif

    CYPRESS_L2_NH_LEGACYFIELD_LIST_END,
} rtk_cypress_l2_nh_legacyField_list_t;


typedef enum rtk_cypress_l2_ucField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_RSVDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_SLPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_AGEtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_SA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_DA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_STATICtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_SUSPENDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_NEXT_HOPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_L2_UC_VIDtf,
#endif

    CYPRESS_L2_UCFIELD_LIST_END,
} rtk_cypress_l2_ucField_list_t;


typedef enum rtk_cypress_mc_pmskField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MC_PMSK_PMSKtf,
#endif

    CYPRESS_MC_PMSKFIELD_LIST_END,
} rtk_cypress_mc_pmskField_list_t;


typedef enum rtk_cypress_untagField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_UNTAG_PMSKtf,
#endif

    CYPRESS_UNTAGFIELD_LIST_END,
} rtk_cypress_untagField_list_t;


typedef enum rtk_cypress_vlanField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MBRtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_FID_MSTItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_L2_HASH_KEY_UCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_L2_HASH_KEY_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_VLAN_PROFILEtf,
#endif

    CYPRESS_VLANFIELD_LIST_END,
} rtk_cypress_vlanField_list_t;


typedef enum rtk_cypress_vlan_egr_cnvtField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_ORG_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_ORG_VID_RNG_CHKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_ORG_PRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_PORT_IDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_BMSK_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_BMSK_ORG_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_BMSK_ORG_VID_RNG_CHKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_BMSK_ORG_PRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_BMSK_PORT_IDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_VID_SHIFTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_NEW_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_PRI_ASSIGNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_NEW_PRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_TPID_ASSIGNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_EGR_CNVT_TPID_IDXtf,
#endif

    CYPRESS_VLAN_EGR_CNVTFIELD_LIST_END,
} rtk_cypress_vlan_egr_cnvtField_list_t;


typedef enum rtk_cypress_vlan_igr_cnvtField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_IVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_IVID_RNG_CHKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_IPRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_PORTIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_BMSK_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_BMSK_IVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_BMSK_IVID_RNG_CHKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_BMSK_IPRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_BMSK_RESERVEDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_BMSK_PORTIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_VID_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_VID_SHIFTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_NEW_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_PRI_ASSIGNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_NEW_PRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_ITAG_STStf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_OTAG_STStf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_TPID_ASSIGNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IGR_CNVT_TPID_IDXtf,
#endif

    CYPRESS_VLAN_IGR_CNVTFIELD_LIST_END,
} rtk_cypress_vlan_igr_cnvtField_list_t;


typedef enum rtk_cypress_vlan_ip_subnet_basedField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_SIPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_PORTIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_BMSK_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_BMSK_SIPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_BMSK_PORTIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_VID_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_VID_SHIFTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_NEW_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_PRI_ASSIGNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_NEW_PRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_ITAG_STStf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_OTAG_STStf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_TPID_ASSIGNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_IP_SUBNET_BASED_TPID_IDXtf,
#endif

    CYPRESS_VLAN_IP_SUBNET_BASEDFIELD_LIST_END,
} rtk_cypress_vlan_ip_subnet_basedField_list_t;


typedef enum rtk_cypress_vlan_mac_basedField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_SMACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_PORTIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_BMSK_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_BMSK_SMACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_BMSK_PORTIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_VID_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_VID_SHIFTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_NEW_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_PRI_ASSIGNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_NEW_PRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_ITAG_STStf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_OTAG_STStf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_TPID_ASSIGNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_VLAN_MAC_BASED_TPID_IDXtf,
#endif

    CYPRESS_VLAN_MAC_BASEDFIELD_LIST_END,
} rtk_cypress_vlan_mac_basedField_list_t;


typedef enum rtk_cypress_mstiField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT51tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT50tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT49tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT48tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT47tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT46tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT45tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT44tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT43tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT42tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT41tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT40tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT39tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT38tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT37tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT36tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT35tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT34tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT33tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT32tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT31tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT30tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT29tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT28tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT27tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT26tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT25tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT24tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT23tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT22tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT21tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT20tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT19tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT18tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT17tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT16tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT15tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT14tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT13tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT12tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT11tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT10tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT9tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT8tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT7tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT6tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT5tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT3tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MSTI_STATE_PORT0tf,
#endif

    CYPRESS_MSTIFIELD_LIST_END,
} rtk_cypress_mstiField_list_t;


typedef enum rtk_cypress_meterField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_METER_TYPEtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_METER_COLOR_AWAREtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_METER_LB0_RATEtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_METER_LB1_RATEtf,
#endif

    CYPRESS_METERFIELD_LIST_END,
} rtk_cypress_meterField_list_t;


typedef enum rtk_cypress_logField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_LOG_CNTRtf,
#endif

    CYPRESS_LOGFIELD_LIST_END,
} rtk_cypress_logField_list_t;


typedef enum rtk_cypress_out_qField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q0_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q0_MAX_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q1_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q1_MAX_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q2_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q2_MAX_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q3_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q3_MAX_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q4_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q4_MAX_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q5_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q5_MAX_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q6_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q6_MAX_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q7_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_Q7_MAX_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_PE_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_PE_MAX_USED_PAGE_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_P_EGR_DROP_ON_THRtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_OUT_Q_P_EGR_DROP_OFF_THRtf,
#endif

    CYPRESS_OUT_QFIELD_LIST_END,
} rtk_cypress_out_qField_list_t;


typedef enum rtk_cypress_schedField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_LB_APR_Q7tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_LB_APR_Q6tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_LB_APR_Q5tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_LB_APR_Q4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_LB_APR_Q3tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_LB_APR_Q2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_LB_APR_Q1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_LB_APR_Q0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_WEIGHT_Q7tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_WEIGHT_Q6tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_WEIGHT_Q5tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_WEIGHT_Q4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_WEIGHT_Q3tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_WEIGHT_Q2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_WEIGHT_Q1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_WEIGHT_Q0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_FIX_TKN_Q7tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_FIX_TKN_Q6tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_FIX_TKN_Q5tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_FIX_TKN_Q4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_FIX_TKN_Q3tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_FIX_TKN_Q2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_FIX_TKN_Q1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_FIX_TKN_Q0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_EGR_RATEtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SCHED_SCHED_TYPEtf,
#endif

    CYPRESS_SCHEDFIELD_LIST_END,
} rtk_cypress_schedField_list_t;


typedef enum rtk_cypress_eaclField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_11tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_10tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_9tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_8tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_7tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_6tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_5tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_3tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FIELD_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_SPMMASK_FIXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_SPNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_MGNT_VLANtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_SWITCHMACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_NOT_FIRST_FRAGtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FRAME_TYPE_L4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FRAME_TYPEtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_OTAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ITAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_OTAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ITAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FRAME_TYPE_L2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_TIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_11tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_10tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_9tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_8tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_7tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_6tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_5tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_3tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FIELD_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_SPMMASK_FIXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_SPNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_MGNT_VLANtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_SWITCHMACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_NOT_FIRST_FRAGtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FRAME_TYPE_L4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FRAME_TYPEtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_OTAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_ITAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_OTAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_ITAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_FRAME_TYPE_L2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_BMSK_TIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_NOTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_AND1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_AND2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ACT_MSK_TAG_STStf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ACT_MSK_QOS_REMARKtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ACT_MSK_PRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ACT_MSK_OVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ACT_MSK_IVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ACT_MSK_METERtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ACT_MSK_MIRRORtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ACT_MSK_LOGtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_ACT_MSK_FWDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FWD_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_FWD_PORT_INFOtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_LOG_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_LOG_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_MIR_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_MIR_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_METER_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_IVLAN_TPID_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_IVLAN_VID_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_IVLAN_TPID_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_IVLAN_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_OVLAN_TPID_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_OVLAN_VID_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_OVLAN_TPID_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_OVLAN_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_PRI_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_INT_PRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_RMK_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_RMK_VALtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_TAG_STS_OUTERtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_EACL_TAG_STS_INNERtf,
#endif

    CYPRESS_EACLFIELD_LIST_END,
} rtk_cypress_eaclField_list_t;


typedef enum rtk_cypress_iaclField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_11tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_10tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_9tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_8tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_7tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_6tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_5tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_3tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FIELD_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_SPMMASK_FIXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_SPNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_MGNT_VLANtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_SWITCHMACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_NOT_FIRST_FRAGtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FRAME_TYPE_L4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FRAME_TYPEtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_OTAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ITAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_OTAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ITAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FRAME_TYPE_L2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_TIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_11tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_10tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_9tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_8tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_7tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_6tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_5tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_3tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FIELD_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_SPMMASK_FIXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_SPNtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_MGNT_VLANtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_SWITCHMACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_NOT_FIRST_FRAGtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FRAME_TYPE_L4tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FRAME_TYPEtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_OTAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_ITAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_OTAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_ITAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_FRAME_TYPE_L2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BMSK_TIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_NOTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_AND1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_AND2tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ACT_MSK_BYPASStf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ACT_MSK_MPLStf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ACT_MSK_PRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ACT_MSK_OVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ACT_MSK_IVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ACT_MSK_METERtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ACT_MSK_MIRRORtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ACT_MSK_LOGtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_ACT_MSK_FWDtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FWD_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_FWD_PORT_INFOtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_LOG_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_LOG_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_MIR_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_METER_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_IVLAN_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_IVLAN_DATAtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_OVLAN_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_OVLAN_DATAtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_INT_PRItf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_MPLS_ACTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_MPLS_LIB_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BYPASS_ALLtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BYPASS_IGR_STPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_IACL_BYPASS_IBC_SCtf,
#endif

    CYPRESS_IACLFIELD_LIST_END,
} rtk_cypress_iaclField_list_t;


typedef enum rtk_cypress_spg_portField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_PKT_CNTtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_BAD_CRC_EN_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_LEN_TYPE_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_RNDM_OFFSET_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_RNDM_EN_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_SA_INC_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_DA_INC_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_PKT_CNT_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_LEN_RNG_START_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_LEN_RNG_END_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_DA_REPEAT_CNT_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_SA_REPEAT_CNT_0tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_BAD_CRC_EN_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_LEN_TYPE_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_RNDM_OFFSET_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_RNDM_EN_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_SA_INC_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_DA_INC_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_PKT_CNT_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_LEN_RNG_START_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_LEN_RNG_END_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_DA_REPEAT_CNT_1tf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_SPG_PORT_STREAM_SA_REPEAT_CNT_1tf,
#endif

    CYPRESS_SPG_PORTFIELD_LIST_END,
} rtk_cypress_spg_portField_list_t;


typedef enum rtk_cypress_routingField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_ROUTING_GATEWAY_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_ROUTING_SW_MAC_IDXtf,
#endif

    CYPRESS_ROUTINGFIELD_LIST_END,
} rtk_cypress_routingField_list_t;


typedef enum rtk_cypress_mpls_libField_list_e
{
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MPLS_LIB_I_LABELtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MPLS_LIB_I_EXPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MPLS_LIB_I_TTLtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MPLS_LIB_O_LABELtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MPLS_LIB_O_EXPtf,
#endif
#if defined(CONFIG_SDK_RTL8390)
    CYPRESS_MPLS_LIB_O_TTLtf,
#endif

    CYPRESS_MPLS_LIBFIELD_LIST_END,
} rtk_cypress_mpls_libField_list_t;

extern rtk_table_t rtk_cypress_table_list[];
#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
extern rtk_tableName_t rtk_cypress_tableName_list[];
#endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */
