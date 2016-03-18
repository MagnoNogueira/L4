/* FUNCTION NAME: DEV_RM.H
* PURPOSE:
*         Only Suport BCM XGSIII 5630X/5650X/5660X chip
* NOTES:
*
* REASON:
*    DESCRIPTION:
*    CREATOR:
*    Date           04/19/2005
*
* Copyright(C)      Accton Corporation, 2005
*/

#ifndef _DEV_RM_HELIX_H
#define _DEV_RM_HELIX_H

/* INCLUDE FILE DECLARATIONS
 */
#include <bcm/field.h>
#include "sys_type.h"
#include "sys_adpt.h"
#include "sysfun.h"

/* NAMING CONSTANT DECLARATIONS
 */


#define DEVRM_STAGE_VFP 0
#define DEVRM_STAGE_IFP 1
#define DEVRM_STAGE_EFP 2

/* for the priority of selector
 *   56514 have 16 IFP selectors, 4 VFP selectors and 4 EFP selectors
 *   need to update this design when using different kind of chip.
 */

enum DEVRM_VFP_SELECTOR_S
{
    DEVRM_VFP_SELECTOR_0 = 0x1 , /*VFP slice 0*/
    DEVRM_VFP_SELECTOR_1 = 0x2 , /*VFP slice 1*/
    DEVRM_VFP_SELECTOR_2 = 0x4 , /*VFP slice 2*/
    DEVRM_VFP_SELECTOR_3 = 0x8   /*VFP slice 3*/
};

enum DEVRM_EFP_SELECTOR_S
{
  DEVRM_EFP_SELECTOR_0 = 0x1 , /*EFP slice 0*/
  DEVRM_EFP_SELECTOR_1 = 0x2 , /*EFP slice 1*/
  DEVRM_EFP_SELECTOR_2 = 0x4 , /*EFP slice 2*/
  DEVRM_EFP_SELECTOR_3 = 0x8   /*EFP slice 3*/
};

enum DEVRM_IFP_SELECTOR_S
{
  DEVRM_IFP_SELECTOR_0 = 0x1 ,
  DEVRM_IFP_SELECTOR_1 = 0x2 ,
  DEVRM_IFP_SELECTOR_2 = 0x4 ,
  DEVRM_IFP_SELECTOR_3 = 0x8 ,
  DEVRM_IFP_SELECTOR_4 = 0x10,
  DEVRM_IFP_SELECTOR_5 = 0x20,
  DEVRM_IFP_SELECTOR_6 = 0x40,
  DEVRM_IFP_SELECTOR_7 = 0x80,
  DEVRM_IFP_SELECTOR_8 = 0x100,
  DEVRM_IFP_SELECTOR_9 = 0x200,
  DEVRM_IFP_SELECTOR_10 = 0x400,
  DEVRM_IFP_SELECTOR_11 = 0x800,
  DEVRM_IFP_SELECTOR_12 = 0x1000,
  DEVRM_IFP_SELECTOR_13 = 0x2000,
  DEVRM_IFP_SELECTOR_14 = 0x4000,
  DEVRM_IFP_SELECTOR_15 = 0x8000
};

typedef enum
{
    DEVRM_GROUP_MODE_SINGLE=0,            /* Single-wide group (default)      */
    DEVRM_GROUP_MODE_DOUBLE,            /* Double-wide group                */
    DEVRM_GROUP_MODE_TRIPLE,            /* Triple-wide group                */
    DEVRM_GROUP_MODE_AUTO,              /* Auto-expand group based on Qset  */
    DEVRM_GROUP_MODE_INVALID              /* Always last: not usable mode     */
} DEVRM_GROUP_MODE_T;

 /*define rule priority in group*/

#define  DEVRM_RULE_PRIO_HIGH       (-1)  /* Highest in group    */
#define  DEVRM_RULE_PRIO_DEFAULT	   (-2)  /* Middle priority   */
#define  DEVRM_RULE_PRIO_LOW        (-3)  /* Lowest in group     */


/* NOTE for XGSIII:
 *    In one selector, rule with smaller index has higher priority.
 *    In multiple selectors, selector with larger index has higher priority.
 *    Rules of rate-limit should have higher priority than the user
 *    configured ones so rules of rate-limit are reserved from the rear
 *    in one selector only.
 *
 *    Rate-limit will not take effect if user configs rules of IPv6 EXT QoS.
 *
 */


/* MACRO FUNCTION DECLARATIONS
 */
/* DATA TYPE DECLARATIONS
 */

typedef enum  {
    DEVRM_FIELD_QUALIFY_SrcIp6          = bcmFieldQualifySrcIp6,            /* Source IPv6 Address. */
    DEVRM_FIELD_QUALIFY_DstIp6          = bcmFieldQualifyDstIp6,            /* Destination IPv6 Address. */
    DEVRM_FIELD_QUALIFY_SrcIp6High      = bcmFieldQualifySrcIp6High,        /* Src. IPv6 Address (High/Upper
                                                                                64 bits). */
    DEVRM_FIELD_QUALIFY_DstIp6High= bcmFieldQualifyDstIp6High,              /* Dest IPv6 Address (High/Upper
                                                                                64 bits). */
    DEVRM_FIELD_QUALIFY_SrcMac          = bcmFieldQualifySrcMac,            /* Source L2 MAC Address. */
    DEVRM_FIELD_QUALIFY_DstMac          = bcmFieldQualifyDstMac,            /* Destination L2 MAC Address. */
    DEVRM_FIELD_QUALIFY_Snap            = bcmFieldQualifySnap,              /* 40: SNAP Header. */
    DEVRM_FIELD_QUALIFY_SrcIp           = bcmFieldQualifySrcIp,             /* Source IPv4 Address. */
    DEVRM_FIELD_QUALIFY_DstIp           = bcmFieldQualifyDstIp,             /* Destination IPv4 Address. */
    DEVRM_FIELD_QUALIFY_Llc             = bcmFieldQualifyLlc,               /* 24: LLC Header. */
    DEVRM_FIELD_QUALIFY_InPort          = bcmFieldQualifyInPort,            /* Single Input Port. */
    DEVRM_FIELD_QUALIFY_InPorts         = bcmFieldQualifyInPorts,           /* Input Port Bitmap. */
    DEVRM_FIELD_QUALIFY_OutPorts        = bcmFieldQualifyOutPorts,          /* Output Port Bitmap. */
    DEVRM_FIELD_QUALIFY_Drop            = bcmFieldQualifyDrop,              /* Packet is flagged to be
                                                                               dropped . */
    DEVRM_FIELD_QUALIFY_Ip6FlowLabel    = bcmFieldQualifyIp6FlowLabel,      /* IPv6 Flow Label. */
    DEVRM_FIELD_QUALIFY_OuterVlan       = bcmFieldQualifyOutVlan,           /* Outer VLAN. */
    DEVRM_FIELD_QUALIFY_InnerVlan       = bcmFieldQualifyInnerVlan,         /* Inner VLAN (double-tagged pkts
                                                                                only). */
#ifdef BCM_SDK_5_5_2
    DEVRM_FIELD_QUALIFY_OuterVlanPri    = bcmFieldQualifyCount,             /*  Outer VLAN priority.
                                                                             *  Not support in SDK 5.5.2.
                                                                             */
#else
    DEVRM_FIELD_QUALIFY_OuterVlanPri    = bcmFieldQualifyOuterVlanPri,      /*  Outer VLAN priority. */
#endif
    DEVRM_FIELD_QUALIFY_LookupStatus    = bcmFieldQualifyLookupStatus,      /* DEPRECATED Lookup status
                                                                                (BCM_FIELD_LOOKUP_xxx). */
    DEVRM_FIELD_QUALIFY_RangeCheck      = bcmFieldQualifyRangeCheck,        /* TCP/UDP Port Range Check
                                                                                Results. */
    DEVRM_FIELD_QUALIFY_L4SrcPort       = bcmFieldQualifyL4SrcPort,         /* TCP/UDP Source Port. */
    DEVRM_FIELD_QUALIFY_L4DstPort       = bcmFieldQualifyL4DstPort,         /* TCP/UDP Destination Port. */
    DEVRM_FIELD_QUALIFY_EtherType       = bcmFieldQualifyEtherType,         /* Ethernet Type. */
    DEVRM_FIELD_QUALIFY_IpProtocol      = bcmFieldQualifyIpProtocol,        /* IP Protocol Field. */
    DEVRM_FIELD_QUALIFY_DSCP            = bcmFieldQualifyDSCP,              /* Diffserv Code Point. */
    DEVRM_FIELD_QUALIFY_Tos             = bcmFieldQualifyTos,               /* Tos - same as DSCP. */
    DEVRM_FIELD_QUALIFY_Ttl             = bcmFieldQualifyTtl,               /* Time To Live/Hop Limit. */
    DEVRM_FIELD_QUALIFY_Ip6NextHeader   = bcmFieldQualifyIp6NextHeader,     /* IPv6 Next Header/IP Protocol. */
    DEVRM_FIELD_QUALIFY_Ip6TrafficClass = bcmFieldQualifyIp6TrafficClass,   /* IPv6 Traffic Class. */
    DEVRM_FIELD_QUALIFY_Ip6HopLimit     = bcmFieldQualifyIp6HopLimit,       /* IPv6 Hop Limit. */
    DEVRM_FIELD_QUALIFY_SrcModid        = bcmFieldQualifySrcModid,          /* Source Module ID (deprecated). */
    DEVRM_FIELD_QUALIFY_SrcPortTgid     = bcmFieldQualifySrcPortTgid,       /* Source Port/Trunk Group ID
                                                                                (deprecated). */
    DEVRM_FIELD_QUALIFY_SrcPort         = bcmFieldQualifySrcPort,           /* Source Module ID and Port . */
    DEVRM_FIELD_QUALIFY_SrcTrunk        = bcmFieldQualifySrcTrunk,          /* Source Trunk Group ID . */
    DEVRM_FIELD_QUALIFY_DstModid        = bcmFieldQualifyDstModid,          /* Destination Module ID
                                                                               (deprecated). */
    DEVRM_FIELD_QUALIFY_DstPortTgid     = bcmFieldQualifyDstPortTgid,       /* Dest Port/Trunk Group ID
                                                                               (deprecated. */
    DEVRM_FIELD_QUALIFY_DstPort         = bcmFieldQualifyDstPort,           /* Destination Module ID and Port
                                           . */
    DEVRM_FIELD_QUALIFY_DstTrunk        = bcmFieldQualifyDstTrunk,          /* Destination Trunk Group ID . */
    DEVRM_FIELD_QUALIFY_TcpControl      = bcmFieldQualifyTcpControl,        /* TCP Control Flags. */
    DEVRM_FIELD_QUALIFY_PacketRes       = bcmFieldQualifyPacketRes,         /* Pkt resolution
                                                                               (BCM_FIELD_PKT_RES_xxx). */
    DEVRM_FIELD_QUALIFY_PacketFormat    = bcmFieldQualifyPacketFormat,      /* DEPRECATED Packet Format
                                                                               (BCM_FIELD_PKT_FMT_xxx). */
    DEVRM_FIELD_QUALIFY_SrcClassL2      = bcmFieldQualifySrcClassL2,        /* Source Class based on L2
                                                                                lookup result. */
    DEVRM_FIELD_QUALIFY_SrcClassL3      = bcmFieldQualifySrcClassL3,        /* Source Class based on L3
                                                                               lookup result. */
    DEVRM_FIELD_QUALIFY_SrcClassField   = bcmFieldQualifySrcClassField,     /* Source Class based on
                                                                                bcmFieldStageLookup result. */
    DEVRM_FIELD_QUALIFY_DstClassL2      = bcmFieldQualifyDstClassL2,        /* Destination Class based on L2
                                                                               lookup result. */
    DEVRM_FIELD_QUALIFY_DstClassL3      = bcmFieldQualifyDstClassL3,        /* Destination Class based on L3
                                                                               lookup result. */
    DEVRM_FIELD_QUALIFY_DstClassField   = bcmFieldQualifyDstClassField,     /* Destination Class based on
                                                                               bcmFieldStageLookup result. */
    DEVRM_FIELD_QUALIFY_IpInfo          = bcmFieldQualifyIpInfo,            /* IP Information
                                                                               (BCM_FIELD_IP_xxx). */
    DEVRM_FIELD_QUALIFY_IpProtocolCommon= bcmFieldQualifyIpProtocolCommon,  /* 3: Common IP protocols. */
    DEVRM_FIELD_QUALIFY_MHOpcode        = bcmFieldQualifyMHOpcode,          /*  Module Header Opcode. */
    DEVRM_FIELD_QUALIFY_IpFlags         = bcmFieldQualifyIpFlags,           /* IPv4 Flags. */
    DEVRM_FIELD_QUALIFY_InnerTpid       = bcmFieldQualifyInnerTpid,         /* 2: Inner VLAN TPID. */
    DEVRM_FIELD_QUALIFY_OuterTpid       = bcmFieldQualifyOuterTpid,         /* 2: Outer VLAN TPID. */
    DEVRM_FIELD_QUALIFY_IpType          = bcmFieldQualifyIpType,            /* IP Type (bcmFieldIpTypeXXX). */
    //DEVRM_FIELD_QUALIFY_Chain           = bcmFieldQualifyChain,           /* Internal use only. */
    DEVRM_FIELD_QUALIFY_Decap           = bcmFieldQualifyDecap,             /* Decapsulation occurred. */
    DEVRM_FIELD_QUALIFY_HiGig           = bcmFieldQualifyHiGig,             /* HiGig packet. */
    DEVRM_FIELD_QUALIFY_HiGigProxy      = bcmFieldQualifyHiGigProxy,        /* HiGig proxy processing is
                                                                               enabled. */
    DEVRM_FIELD_QUALIFY_Stage           = bcmFieldQualifyStage,             /* Field Processor pipeline
                                                                               stage. */
    DEVRM_FIELD_QUALIFY_StageIngress    = bcmFieldQualifyStageIngress,      /* Field Processor pipeline
                                                                               ingress stage. */
    DEVRM_FIELD_QUALIFY_StageIngressSecurity    = bcmFieldQualifyStageIngressSecurity, /* Field Processor ingress
                                                                                          security stage. */
    DEVRM_FIELD_QUALIFY_StageIngressQoS = bcmFieldQualifyStageIngressQoS,   /* Field Processor ingress QOS
                                                                               stage. */
    DEVRM_FIELD_QUALIFY_StageLookup     = bcmFieldQualifyStageLookup,       /* Field Processor pipeline
                                                                               lookup stage. */
    DEVRM_FIELD_QUALIFY_StageEgress     = bcmFieldQualifyStageEgress,       /* Field Processor pipeline
                                                                               egress stage. */
    DEVRM_FIELD_QUALIFY_StageEgressSecurity = bcmFieldQualifyStageEgressSecurity, /* Field Processor egress
                                                                                     security stage. */
    DEVRM_FIELD_QUALIFY_StageExternal   = bcmFieldQualifyStageExternal,     /* Field Processor Rules in
                                                                               External slice. */
    DEVRM_FIELD_QUALIFY_SrcIpEqualDstIp = bcmFieldQualifySrcIpEqualDstIp,   /* Src IP == Dst IP. */
    DEVRM_FIELD_QUALIFY_EqualL4Port     = bcmFieldQualifyEqualL4Port,       /* L4 Src Port == Dst Port. */
    DEVRM_FIELD_QUALIFY_TcpSequenceZero = bcmFieldQualifyTcpSequenceZero,   /* Sequence number = 0. */
    DEVRM_FIELD_QUALIFY_TcpHeaderSize   = bcmFieldQualifyTcpHeaderSize,     /* TCP Header Size. */
    DEVRM_FIELD_QUALIFY_InterfaceClassL2= bcmFieldQualifyInterfaceClassL2,  /* Vlan Class Id. */
    DEVRM_FIELD_QUALIFY_InterfaceClassL3= bcmFieldQualifyInterfaceClassL3,  /* L3 Interface Class Id. */
    DEVRM_FIELD_QUALIFY_InterfaceClassPort  = bcmFieldQualifyInterfaceClassPort,  /* Port Class Id. */
    DEVRM_FIELD_QUALIFY_L3Routable      = bcmFieldQualifyL3Routable,        /* L3 routable bit for FB2. */
    DEVRM_FIELD_QUALIFY_IpFrag          = bcmFieldQualifyIpFrag,            /* IP Fragment info. */
    DEVRM_FIELD_QUALIFY_Vrf             = bcmFieldQualifyVrf,               /* VRF Id for FB 2. */
    DEVRM_FIELD_QUALIFY_OutPort         = bcmFieldQualifyOutPort,           /* Single Output Port. */
    DEVRM_FIELD_QUALIFY_Ip4             = bcmFieldQualifyIp4,               /* Qualify IpType == IPv4Any. */
    DEVRM_FIELD_QUALIFY_Ip6             = bcmFieldQualifyIp6,               /* Qualify IpType == Ipv6Any. */
    DEVRM_FIELD_QUALIFY_L2Format        = bcmFieldQualifyL2Format,          /* Qualify on L2 header format. */
    DEVRM_FIELD_QUALIFY_VlanFormat      = bcmFieldQualifyVlanFormat,        /* Qualify on vlan tag format. */
    DEVRM_FIELD_QUALIFY_ExtensionHeaderType = bcmFieldQualifyExtensionHeaderType, /* Qualify on  Next Header Field
                                                                                     in First Extension Header. */
    DEVRM_FIELD_QUALIFY_ExtensionHeaderSubCode  = bcmFieldQualifyExtensionHeaderSubCode, /* Qualify on First byte after
                                                                                            extentsion header length field. */
    DEVRM_FIELD_QUALIFY_L4Ports         = bcmFieldQualifyL4Ports,           /* Four bytes after L3 header are
                                                                               available. */
    DEVRM_FIELD_QUALIFY_MirrorCopy      = bcmFieldQualifyMirrorCopy,        /* Qualify mirrored traffic. */
    DEVRM_FIELD_QUALIFY_TunnelTerminated=bcmFieldQualifyTunnelTerminated,   /* Qualify tunnel terminated
                                                                               traffic. */
    DEVRM_FIELD_QUALIFY_MplsTerminated  = bcmFieldQualifyMplsTerminated,    /* Qualify mpls terminated
                                                                               traffic. */
    DEVRM_FIELD_QUALIFY_InnerSrcIp      = bcmFieldQualifyInnerSrcIp,        /* Inner ip header source IPv6
                                                                               Address. */
    DEVRM_FIELD_QUALIFY_InnerDstIp      = bcmFieldQualifyInnerDstIp,        /* Inner ip header destination
                                                                               IPv6 Address. */
    DEVRM_FIELD_QUALIFY_InnerSrcIp6     = bcmFieldQualifyInnerSrcIp6,       /* Inner ip header source IPv6
                                                                               Address. */
    DEVRM_FIELD_QUALIFY_InnerDstIp6     = bcmFieldQualifyInnerDstIp6,       /* Inner ip header destination
                                                                               IPv6 Address. */
    DEVRM_FIELD_QUALIFY_InnerSrcIp6High = bcmFieldQualifyInnerSrcIp6High,   /* Inner ip header Src. IPv6
                                                                               Address (High/Upper 64 bits). */
    DEVRM_FIELD_QUALIFY_InnerDstIp6High = bcmFieldQualifyInnerDstIp6High,   /* Inner ip header Dest IPv6
                                                                               Address (High/Upper 64 bits). */
    DEVRM_FIELD_QUALIFY_InnerTtl        = bcmFieldQualifyInnerTtl,          /* Inner ip header time To
                                                                               Live/Hop Limit. */
    DEVRM_FIELD_QUALIFY_InnerDSCP       = bcmFieldQualifyInnerDSCP,         /* Inner ip header diffserv Code
                                                                               Point. */
    DEVRM_FIELD_QUALIFY_InnerIpProtocol = bcmFieldQualifyInnerIpProtocol,   /* Inner ip header IP Protocol
                                                                               Field. */
    DEVRM_FIELD_QUALIFY_InnerIpFrag     = bcmFieldQualifyInnerIpFrag,       /* Inner ip header IP Fragment
                                                                               info. */
    DEVRM_FIELD_QUALIFY_VlanTranslationHit  = bcmFieldQualifyVlanTranslationHit,  /* Vlan Translation table hit. */
    DEVRM_FIELD_QUALIFY_ForwardingVlanValid = bcmFieldQualifyForwardingVlanValid, /* Forwarding vlan is valid. */
    DEVRM_FIELD_QUALIFY_IngressStpState     = bcmFieldQualifyIngressStpState,     /* Ingress Vlan STG STP state. */
    DEVRM_FIELD_QUALIFY_L2SrcHit        = bcmFieldQualifyL2SrcHit,          /* L2 Source lookup success. */
    DEVRM_FIELD_QUALIFY_L2SrcStatic     = bcmFieldQualifyL2SrcStatic,       /* L2 Source is static entry. */
    DEVRM_FIELD_QUALIFY_L2DestHit       = bcmFieldQualifyL2DestHit,         /* L2 Destination lookup success. */
    DEVRM_FIELD_QUALIFY_L2StationMove   = bcmFieldQualifyL2StationMove,     /* L2 Source port mismatch. */
    DEVRM_FIELD_QUALIFY_L2CacheHit      = bcmFieldQualifyL2CacheHit,        /* L2 Destination cache lookup
                                                                               sucess. */
    DEVRM_FIELD_QUALIFY_L3SrcHostHit    = bcmFieldQualifyL3SrcHostHit,      /* L3 Source lookup host table
                                                                               hit. */
    DEVRM_FIELD_QUALIFY_L3DestHostHit   = bcmFieldQualifyL3DestHostHit,     /* L3 Destination lookup host
                                                                               table hit. */
    DEVRM_FIELD_QUALIFY_L3DestRouteHit  = bcmFieldQualifyL3DestRouteHit,    /* L3 Destination lookup route
                                                                               table hit. */
    DEVRM_FIELD_QUALIFY_IpmcStarGroupHit= bcmFieldQualifyIpmcStarGroupHit,  /* L3 Multicast lookup
                                                                               (Star,Group) hit. */
    DEVRM_FIELD_QUALIFY_DosAttack       = bcmFieldQualifyDosAttack,         /* Denial of Service attack
                                                                               frame. */
    DEVRM_FIELD_QUALIFY_IpAuth          = bcmFieldQualifyIpAuth,            /* IP Authenticated. */
    DEVRM_FIELD_QUALIFY_ClassId         = bcmFieldQualifyClassId,           /* internal use only. */
    DEVRM_FIELD_QUALIFY_BigIcmpCheck    = bcmFieldQualifyBigIcmpCheck,      /* internal use only. */
    DEVRM_FIELD_QUALIFY_IcmpTypeCode    = bcmFieldQualifyIcmpTypeCode,      /* internal use only. */
    DEVRM_FIELD_QUALIFY_IgmpTypeMaxRespTime = bcmFieldQualifyIgmpTypeMaxRespTime, /* internal use only. */
    DEVRM_FIELD_QUALIFY_Count           = bcmFieldQualifyCount              /* Always Last. Not a usable value. */
} DEVRM_FIELD_QUALIFY_T;

typedef enum  {
    DEVRM_FIELD_USER_QUALIFY_Udf1,                /* [0x00] User Defined 1 */
    DEVRM_FIELD_USER_QUALIFY_Udf2,                /* [0x01] User Defined 2 */

    DEVRM_FIELD_USER_QUALIFY_Count                /* Always Last. Not a usable value. */
} DEVRM_FIELD_USER_QUALIFY_T;

typedef struct
{
    UI32_T  udf_id;
    UI32_T  flags;
    UI32_T  chunk;      /* offset */
    BOOL_T  enabled;
} DEVRM_UDF_T;

typedef struct
{
    UI32_T  udf_id;
    UI8_T   udf_data[BCM_FIELD_USER_FIELD_SIZE];
    UI8_T   udf_mask[BCM_FIELD_USER_FIELD_SIZE];
} DEVRM_FieldUDF_T;

#define DEVRM_SHR_BITWIDE		32

/* (internal) Generic operation macro on bit array _a, with bit _b */
#define	DEVRM_SHR_BITOP(_a, _b, _op)	\
	(_a[(_b) / DEVRM_SHR_BITWIDE] _op (1 << ((_b) % DEVRM_SHR_BITWIDE)))

/* (internal) Number of SHR_BITDCLs needed to contain _max bits */
#define	DEVRM_SHR_BITDCLSIZE(_max)	(((_max) + DEVRM_SHR_BITWIDE - 1) / DEVRM_SHR_BITWIDE)

/* Specific operations */
#define	DEVRM_SHR_BITGET(_a, _b)	DEVRM_SHR_BITOP(_a, _b, &)
#define	DEVRM_SHR_BITSET(_a, _b)	DEVRM_SHR_BITOP(_a, _b, |=)
#define	DEVRM_SHR_BITCLR(_a, _b)	DEVRM_SHR_BITOP(_a, _b, &= ~)

#define DEVRM_PBMP_PORT_ADD(pbmp, port)       BCM_PBMP_PORT_ADD(pbmp, port)
#define DEVRM_PBMP_PORT_REMOVE(pbmp, port)    BCM_PBMP_PORT_REMOVE(pbmp, port)
#define DEVRM_PBMP_MEMBER(pbmp, port)         BCM_PBMP_MEMBER(pbmp, port)
#define DEVRM_PBMP_ASSIGN(dst_pbmp, src_pbmp) BCM_PBMP_ASSIGN(dst_pbmp, src_pbmp)
#define DEVRM_PBMP_OR(dst_pbmp, src_pbmp)     BCM_PBMP_OR(dst_pbmp, src_pbmp)
#define DEVRM_PBMP_EQ(dst_pbmp, src_pbmp)     BCM_PBMP_EQ(dst_pbmp, src_pbmp)
#define DEVRM_PBMP_CLEAR(pbmp)                BCM_PBMP_CLEAR(pbmp)
#define DEVRM_PBMP_NOT_NULL(pbmp)             BCM_PBMP_NOT_NULL(pbmp)

#define DEVRM_FIELD_QSET_ADD_INTERNAL(fset, q)      DEVRM_SHR_BITSET(((fset).w), (q))
#define DEVRM_FIELD_ACTION_ADD_INTERNAL(aset, q)    DEVRM_SHR_BITSET(((aset).w), (q))
#define DEVRM_FIELD_QSET_GET(fset,q)                DEVRM_SHR_BITGET(((fset).w),(q))

#define DEVRM_FIELD_MIRROR_FUN_TYPE_CHANG_ADD(bitmap,q)     DEVRM_SHR_BITSET(bitmap,q)
#define DEVRM_FIELD_MIRROR_FUN_TYPE_CHANG_CLEAR(bitmap,q)   DEVRM_SHR_BITCLR(bitmap,q)
#define DEVRM_FIELD_MIRROR_FUN_TYPE_CHANG_GET(bitmap,q)     DEVRM_SHR_BITGET(bitmap,q)

typedef bcm_pbmp_t DEVRM_PBMP_T;

typedef struct
{
    bcm_pbmp_t all_ports;   /* PORT ports + CPU port */
    bcm_pbmp_t cpu_port;
} DEVRM_PortBitMap_T;

typedef struct
{
    UI32_T                 num_of_chips;
    DEVRM_PortBitMap_T     port_bit_map;
}DEVRM_DevInfo_T;

typedef struct
{
                        //UI64_T          entry_qualify;
    UI32_T w[DEVRM_SHR_BITDCLSIZE(DEVRM_FIELD_QUALIFY_Count)];
    UI32_T udf_map[DEVRM_SHR_BITDCLSIZE(DEVRM_FIELD_USER_QUALIFY_Count)];

#if 1
    /*bcm_pbmp_t*/      bcm_pbmp_t      inports_data;
    /*bcm_pbmp_t*/      bcm_pbmp_t      inports_mask;
    /*bcm_port_t*/      int             inport_data;
    /*bcm_port_t*/      int             inport_mask;
#else
    /*bcm_pbmp_t*/      UI32_T          inports_data;
    /*bcm_pbmp_t*/      UI32_T          inports_mask;
#endif
    /*2008-06-27, Jinfeng Chen: Add this for egress support */
                        bcm_pbmp_t      outports_data;
                        bcm_pbmp_t      outports_mask;
                        int             outport_data;
                        int             outport_mask;

    /*bcm_module_t*/    int             srcmodid_data;
    /*bcm_module_t*/    int             srcmodid_mask;

    /*bcm_port_t*/      int             srcporttgid_data;
    /*bcm_port_t*/      int             srcporttgid_mask;

    /*bcm_module_t*/    int             dstmodid_data;
    /*bcm_module_t*/    int             dstmodid_mask;

    /*bcm_port_t*/      int             dstporttgid_data;
    /*bcm_port_t*/      int             dstporttgid_mask;

    /*bcm_l4_port_t*/   int             l4_srcport_data;
    /*bcm_l4_port_t*/   int             l4_srcport_mask;

    /*bcm_l4_port_t*/   int             l4_dstport_data;
    /*bcm_l4_port_t*/   int             l4_dstport_mask;

    /*bcm_vlan_t*/      UI16_T          outvlan_data;
    /*bcm_vlan_t*/      UI16_T          outvlan_mask;

    /*bcm_vlan_t*/      UI16_T          invlan_data;
    /*bcm_vlan_t*/      UI16_T          invlan_mask;

                        UI8_T           outvlanpri_data;
                        UI8_T           outvlanpri_mask;

    /*uint16*/          UI16_T          ethertype_data;
    /*uint16*/          UI16_T          ethertype_mask;

    /*uint8*/           UI8_T           ipprotocol_data;
    /*uint8*/           UI8_T           ipprotocol_mask;

    /*uint8*/           UI8_T           higpacket_data;
    /*uint8*/           UI8_T           higpacket_mask;

    /*uint32*/          UI32_T          lookupstatus_data;
    /*uint32*/          UI32_T          lookupstatus_mask;

    /*uint32*/          UI32_T          ipinfo_data;
    /*uint32*/          UI32_T          ipinfo_mask;

    /*uint32*/          UI32_T          packetres_data;
    /*uint32*/          UI32_T          packetres_mask;

    /*bcm_ip_t*/        UI32_T          srcip_data;
    /*bcm_ip_t*/        UI32_T          srcip_mask;

    /*bcm_ip_t*/        UI32_T          dstip_data;
    /*bcm_ip_t*/        UI32_T          dstip_mask;

    /*uint8*/           UI8_T           dscp_data;
    /*uint8*/           UI8_T           dscp_mask;

    /*uint8*/           UI8_T           ipflags_data;
    /*uint8*/           UI8_T           ipflags_mask;

    /*uint8*/           UI8_T           tcpcontrol_data;
    /*uint8*/           UI8_T           tcpcontrol_mask;

    /*uint8*/           UI8_T           ttl_data;
    /*uint8*/           UI8_T           ttl_mask;

    /*bcm_field_range_t*/ UI32_T        range;
                        int             invert;

    /*bcm_ip6_t*/       UI8_T           srcip6_data[SYS_ADPT_IPV6_ADDR_LEN];
    /*bcm_ip6_t*/       UI8_T           srcip6_mask[SYS_ADPT_IPV6_ADDR_LEN];

    /*bcm_ip6_t*/       UI8_T           dstip6_data[SYS_ADPT_IPV6_ADDR_LEN];
    /*bcm_ip6_t*/       UI8_T           dstip6_mask[SYS_ADPT_IPV6_ADDR_LEN];

    /*bcm_ip6_t*/       UI8_T           dstip6high_data[SYS_ADPT_IPV6_ADDR_LEN];
    /*bcm_ip6_t*/       UI8_T           dstip6high_mask[SYS_ADPT_IPV6_ADDR_LEN];

    /*uint8*/           UI8_T           ip6nextheader_data;
    /*uint8*/           UI8_T           ip6nextheader_mask;

    /*uint8*/           UI8_T           ip6trafficclass_data;
    /*uint8*/           UI8_T           ip6trafficclass_mask;

    /*uint32*/          UI32_T          ip6flowlabel_data;
    /*uint32*/          UI32_T          ip6flowlabel_mask;

    /*uint8*/           UI8_T           ip6hoplimit_data;
    /*uint8*/           UI8_T           ip6hoplimit_mask;

    /*bcm_mac_t*/       UI8_T           srcmac_data[SYS_ADPT_MAC_ADDR_LEN];
    /*bcm_mac_t*/       UI8_T           srcmac_mask[SYS_ADPT_MAC_ADDR_LEN];

    /*bcm_mac_t*/       UI8_T           dstmac_data[SYS_ADPT_MAC_ADDR_LEN];
    /*bcm_mac_t*/       UI8_T           dstmac_mask[SYS_ADPT_MAC_ADDR_LEN];

#if 0 /* obsolete by BCM SDK 6, use IpType, L2Format, VlanFormat instead. */
    /*uint32*/          UI32_T          packetformat_data;
    /*uint32*/          UI32_T          packetformat_mask;
#endif

    /*uint32*/          UI32_T          iptype;

    /*uint32*/          UI32_T          l2format;

    /*uint8*/           UI8_T           vlanformat_data;
    /*uint8*/           UI8_T           vlanformat_mask;

    /*uint8*/           UI8_T           mhopcode_data;
    /*uint8*/           UI8_T           mhopcode_mask;

    /*uint8*/           UI8_T           stpstate;

    /*uint8*/           UI8_T           l2srchit_data;
    /*uint8*/           UI8_T           l2srchit_mask;

    /*uint8*/           UI8_T           l2dsthit_data;
    /*uint8*/           UI8_T           l2dsthit_mask;

    /*uint8*/           UI8_T           l3routable_data;
    /*uint8*/           UI8_T           l3routable_mask;

    /*uint8*/           UI8_T           l3dsthosthit_data;
    /*uint8*/           UI8_T           l3dsthosthit_mask;

                        DEVRM_FieldUDF_T    udf[DEVRM_FIELD_USER_QUALIFY_Count];
}DEVRM_AceEntry_T;

typedef struct DEVRM_ActionEntry_S
{
    UI32_T                      action;    /* Action to perform (bcmFieldActionXXX)*/
    UI32_T                      param0;    /*Action parameter (use 0 if not required)*/
    UI32_T                      param1;    /*Action parameter (use 0 if not required)*/
    bcm_pbmp_t                  pbmp;    /*Action parameter (use 0 if not required)*/
    struct DEVRM_ActionEntry_S  *next_action;
} DEVRM_ActionEntry_T;

#define DEVRM_POLICER_METER_TYPE_TABLE  \
    METER_TYPE(BYTE)                    \
    METER_TYPE(PACKET)

typedef enum
{
#define METER_TYPE(type) DEVRM_POLICER_METER_TYPE_##type,
    DEVRM_POLICER_METER_TYPE_TABLE
#undef METER_TYPE
} DEVRM_POLICER_METER_TYPE_T;

typedef struct
{
    int      meter_num;     /*Entry in meter group (BCM_FIELD_METER_xxx)*/
                            /*BCM_FIELD_METER_PEAK*/
                            /*BCM_FIELD_METER_COMMITTED*/
                            /*BCM_FIELD_METER_BOTH*/
    UI32_T   kbits_sec;     /*Data rate to associate with commit meter (kbits/sec)*/
    UI32_T   kbytes_burst;  /*Max burst to associate with commit meter (in kilobytes)*/
    UI32_T   peak_kbits_sec;     /*Data rate to associate with commit meter (kbits/sec)*/
    UI32_T   peak_kbytes_burst;  /*Max burst to associate with commit meter (in kilobytes)*/

    UI8_T    meter_mode;    /* BCM_FIELD_METER_MODE_DEFAULT
                             * BCM_FIELD_METER_MODE_FLOW
                             * BCM_FIELD_METER_MODE_trTCM_COLOR_BLIND
                             * BCM_FIELD_METER_MODE_trTCM_COLOR_AWARE
                             * BCM_FIELD_METER_MODE_srTCM_COLOR_BLIND
                             * BCM_FIELD_METER_MODE_srTCM_COLOR_AWARE
                             */
    DEVRM_POLICER_METER_TYPE_T meter_type;
} DEVRM_MeterEntry_T;

#pragma mark -
#pragma mark packet command

#define DEVRM_PACKET_COMMAND_TABLE    \
    PKT_CMD(UNSPECIFIC)               \
    PKT_CMD(FORWARD)                  \
    PKT_CMD(REDIRECT_TO_CPU)          \
    PKT_CMD(COPY_TO_CPU)              \
    PKT_CMD(HARD_DROP)                \
    PKT_CMD(SOFT_DROP)                \
    PKT_CMD(MIRROR_TO_ANALYZER_PORT)

typedef enum
{
#define PKT_CMD(pktCmd) DEVRM_PKT_CMD_##pktCmd,

    DEVRM_PACKET_COMMAND_TABLE

#undef PKT_CMD
} DEVRM_PACKET_COMMAND_T;

#pragma mark -
#pragma mark vlan command

#define DEVRM_MODIFY_VLAN_COMMAND_TABLE \
    MODIFY_VLAN_CMD(DISABLE)            \
    MODIFY_VLAN_CMD(FOR_ALL)            \
    MODIFY_VLAN_CMD(FOR_UNTAGGED_ONLY)  \
    MODIFY_VLAN_CMD(FOR_TAGGED_ONLY)

typedef enum
{
#define MODIFY_VLAN_CMD(vlanCmd) DEVRM_MODIFY_VLAN_CMD_##vlanCmd,
    
    DEVRM_MODIFY_VLAN_COMMAND_TABLE
    
#undef MODIFY_VLAN_CMD
} DEVRM_MODIFY_VLAN_COMMAND_T;

typedef struct
{
    DEVRM_MODIFY_VLAN_COMMAND_T modify_vlan;
    UI16_T                      vid;
    BOOL_T                      add_outer_tag;
} DEVRM_ACTION_VLAN_T;

#pragma mark -
#pragma mark qos command

#define DEVRM_MODIFY_COMMAND_TABLE      \
    MODIFY_CMD(DISABLE)                 \
    MODIFY_CMD(ENABLE)

typedef enum
{
#define MODIFY_CMD(cmd) DEVRM_MODIFY_CMD_##cmd,

    DEVRM_MODIFY_COMMAND_TABLE

#undef MODIFY_CMD
} DEVRM_MODIFY_COMMAND_T;

typedef struct
{
    DEVRM_MODIFY_COMMAND_T      modify_inner_priority;
    DEVRM_MODIFY_COMMAND_T      modify_priority;        /* 1p */
    DEVRM_MODIFY_COMMAND_T      modify_queue;
    DEVRM_MODIFY_COMMAND_T      modify_dscp;
    DEVRM_MODIFY_COMMAND_T      modify_tos;
    DEVRM_MODIFY_COMMAND_T      drop_precedence;

    UI8_T                       inner_priority;
    UI8_T                       priority;               /* 1p */
    UI8_T                       queue;
    UI8_T                       dscp;
    UI8_T                       tos;
} DEVRM_ACTION_QOS_T;

#pragma mark -
#pragma mark policer

#define DEVRM_POLICER_MODE_TABLE        \
    POLICER_MODE(DISABLE)               \
    POLICER_MODE(METER_ONLY)            \
    POLICER_MODE(COUNTER_ONLY)          \
    POLICER_MODE(METER_AND_COUNTER)

typedef enum
{
#define POLICER_MODE(policerMode) DEVRM_POLICER_MODE_##policerMode,
    
    DEVRM_POLICER_MODE_TABLE

#undef POLICER_MODE

} DEVRM_POLICER_MODE_T;

#define DEVRM_POLICER_METER_MODE_TABLE  \
    METER_MODE(FLOW)                    \
    METER_MODE(TRTCM_COLOR_BLIND)       \
    METER_MODE(TRTCM_COLOR_AWARE)       \
    METER_MODE(SRTCM_COLOR_BLIND)       \
    METER_MODE(SRTCM_COLOR_AWARE)


typedef enum
{
#define METER_MODE(meterMode) DEVRM_POLICER_METER_MODE_##meterMode,

    DEVRM_POLICER_METER_MODE_TABLE

#undef METER_MODE

} DEVRM_POLICER_METER_MODE_T;

#define DEVRM_POLICER_METER_COLOR_MODE_TABLE    \
    COLOR_MODE(COLOR_BLIND)                     \
    COLOR_MODE(COLOR_AWARE)

typedef enum
{
#define COLOR_MODE(colorMode) DEVRM_POLICER_METER_##colorMode,

    DEVRM_POLICER_METER_COLOR_MODE_TABLE

#undef COLOR_MODE

} DEVRM_POLICER_METER_COLOR_MODE_T;

typedef struct
{
    DEVRM_POLICER_METER_MODE_T  mode;

    UI32_T                      kbits_sec;         /*Data rate to associate with meter (1000/sec)*/
    UI32_T                      kbytes_burst_size; //kbits_burst;       /*Max burst to associate with meter (1000)*/
    UI32_T                      peak_kbits_sec;
    UI32_T                      peak_kbytes_burst_size;
} DEVRM_POLICER_METER_T;

/*
 * typedef: enum DEVRM_POLICER_COUNTER_MODE_T
 *
 * Description: Counters Mode. This enumeration controls the
 *              counters resolution.
 *
 * Enumerations:
 *  DEVRM_POLICER_CNTR_BYTE                     - 1 Byte resolution.
 *
 *  DEVRM_POLICER_CNTR_PACKET                   - packet resolution.
 */
typedef enum
{
    DEVRM_POLICER_COUNTER_MODE_BYTE,
    DEVRM_POLICER_COUNTER_MODE_PACKET
} DEVRM_POLICER_COUNTER_MODE_T;

typedef struct
{
    DEVRM_POLICER_MODE_T            mode;

    DEVRM_POLICER_METER_T           meter;
    DEVRM_POLICER_COUNTER_MODE_T    counter_mode;

    DEVRM_PACKET_COMMAND_T          yellow_pkt_cmd;
    DEVRM_ACTION_QOS_T              yellow_pkt_qos;

    DEVRM_PACKET_COMMAND_T          red_pkt_cmd;
    DEVRM_ACTION_QOS_T              red_pkt_qos;
} DEVRM_ACTION_POLICER_T;

#pragma mark -
#pragma mark cpu

typedef struct
{
    DEVRM_MODIFY_COMMAND_T          modify_cpu_code;
    UI32_T                          cpu_code;
} DEVRM_ACTION_CPU_T;

#pragma mark -
#pragma mark action

typedef struct
{
    DEVRM_PACKET_COMMAND_T          pkt_cmd;
    DEVRM_ACTION_VLAN_T             vlan;
    DEVRM_ACTION_QOS_T              qos;
    DEVRM_ACTION_POLICER_T          policer;
    DEVRM_ACTION_CPU_T              cpu;
} DEVRM_ACTION_T;

#pragma mark -

typedef struct
{
    UI32_T group_id;
    UI32_T group_mode;
    UI32_T selector_bitmap;
    UI32_T w[DEVRM_SHR_BITDCLSIZE(DEVRM_FIELD_QUALIFY_Count)];
    DEVRM_UDF_T udf[DEVRM_FIELD_USER_QUALIFY_Count];
}DEVRM_GroupEntry_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_Initial
 *------------------------------------------------------------------------------
 * PURPOSE:  init/clear chip setting
 * INPUT:    device_id -- chip no
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_Initial();

/* selector */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a group entry.
 * INPUT:    device_id, group_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_CreateGroup(UI32_T device_id, DEVRM_GroupEntry_T *group_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a selector entry.
 * INPUT:    device_id, group_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyGroup(UI32_T device_id,UI32_T group_id) ;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create virtual group.
 * INPUT:    group_id       - group ID
 *           vir_group_id   - virtual group ID (Unique per system).
 *           priority       - virtual group priority.
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_CreateVirtualGroup(
    UI32_T group_id,
    UI32_T vir_group_id,
    int priority);

/* rule */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateRule
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id     - device ID
 *            group_id      - group ID
 *            vir_group_id  - virtual group ID
 *            rule_pri      - rule priority (no used)
 * OUTPUT   : dev_rule_id (per chip unique),
 *
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_CreateGroup() first
 *            2, MUST allocate rule resource before call DEVRM_SetRule()
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_AllocateRule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T vir_group_id,
    int rule_pri,
    UI32_T *dev_rule_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    device_id, group_index, dev_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_SetRule(
    UI32_T device_id,
    UI32_T group_index,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_MeterEntry_T *meter_entry,
    DEVRM_ActionEntry_T *action_entry,
    UI32_T action_entry_num,
    BOOL_T counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_UpdateRule
 *------------------------------------------------------------------------------
 * PURPOSE:  update a rule entry.
 * INPUT:    device_id, dev_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_UpdateRule(UI32_T device_id, UI32_T dev_rule_id, DEVRM_AceEntry_T *ace_entry, DEVRM_MeterEntry_T *meter_entry, DEVRM_ActionEntry_T *action_entry, UI32_T action_entry_num);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a rule entry.
 * INPUT:    device_id      - device ID
 *           vir_group_id   - virtual group ID
 *           dev_rule_id    - rule ID
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.If meter is shared, when delete it should check ..
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyRule(
    UI32_T device_id,
    UI32_T vir_group_id,
    UI32_T dev_rule_id);

/* meter */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  create a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry,
 *           need_reinstalled (TRUE - rule will be reinstalled)
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_CreateMeter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry, BOOL_T need_reinstalled);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_UpdateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  update a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_UpdateMeter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ShareMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  share a meter from src_rule_id to dst_rule_id
 * INPUT:    device_id, src_rule_id, dst_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_ShareMeter(UI32_T device_id, UI32_T src_rule_id, UI32_T dst_rule_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a meter entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyMeter(UI32_T device_id, UI32_T dev_rule_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  create a counter entry.
 * INPUT:    device_id, dev_rule_id, counter_mode,
 *           need_reinstalled (TRUE - rule will be reinstalled)
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_CreateCounter(UI32_T device_id,UI32_T dev_rule_id, UI32_T counter_mode, BOOL_T need_reinstalled);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a counter entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyCounter(UI32_T device_id, UI32_T dev_rule_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  get counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   green_octets_p, yellow_octets_p, red_octets_p
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_GetCounter(
    UI32_T device_id,
    UI32_T dev_rule_id,
    UI32_T *counter_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ClearCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  clear counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_ClearCounter(
    UI32_T device_id,
    UI32_T dev_rule_id
);

/* action */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a action entry.
 * INPUT:    device_id, dev_rule_id, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_SetAction(UI32_T device_id, UI32_T dev_rule_id, DEVRM_ActionEntry_T *action_entry, UI32_T action_entry_num);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a action entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyAction(UI32_T device_id, UI32_T dev_rule_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : DEVRM_HandleIPCReqMsg
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Handle the ipc request message for dev_rm.
 * INPUT:
 *    ipcmsg_p  --  input request ipc message buffer
 *
 * OUTPUT:
 *    ipcmsg_p  --  output response ipc message buffer
 *
 * RETURN:
 *    TRUE  - There is a response need to send.
 *    FALSE - There is no response to send.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p) ;

#endif   /* End of _DEV_RM_H */
