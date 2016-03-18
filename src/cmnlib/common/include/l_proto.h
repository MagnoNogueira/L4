#ifndef _L_PROTO_H
#define _L_PROTO_H

#define	L_PROTO_IP		       0		/* dummy for IP */
#define	L_PROTO_HOPOPTS   0		/* IP6 hop-by-hop options */
#define	L_PROTO_ICMP		1		/* control message protocol */
#define	L_PROTO_IGMP		2		/* group mgmt protocol */
#define	L_PROTO_GGP		3		/* gateway^2 (deprecated) */
#define   L_PROTO_IPV4		4 		/* IPv4 encapsulation */
#define   L_PROTO_IPIP		4 		/* for compatibility */
#define	L_PROTO_TCP		6		/* tcp */
#define	L_PROTO_ST		7		/* Stream protocol II */
#define	L_PROTO_EGP		8		/* exterior gateway protocol */
#define	L_PROTO_PIGP		9		/* private interior gateway */
#define	L_PROTO_RCCMON	10		/* BBN RCC Monitoring */
#define	L_PROTO_NVPII		11		/* network voice protocol*/
#define	L_PROTO_PUP		12		/* pup */
#define	L_PROTO_ARGUS		13		/* Argus */
#define	L_PROTO_EMCON	14		/* EMCON */
#define	L_PROTO_XNET		15		/* Cross Net Debugger */
#define	L_PROTO_CHAOS		16		/* Chaos*/
#define	L_PROTO_UDP		17		/* user datagram protocol */
#define	L_PROTO_MUX		18		/* Multiplexing */
#define	L_PROTO_MEAS		19		/* DCN Measurement Subsystems */
#define	L_PROTO_HMP		20		/* Host Monitoring */
#define	L_PROTO_PRM		21		/* Packet Radio Measurement */
#define	L_PROTO_IDP		22		/* xns idp */
#define	L_PROTO_TRUNK1	23		/* Trunk-1 */
#define	L_PROTO_TRUNK2	24		/* Trunk-2 */
#define	L_PROTO_LEAF1		25		/* Leaf-1 */
#define	L_PROTO_LEAF2		26		/* Leaf-2 */
#define	L_PROTO_RDP		27		/* Reliable Data */
#define	L_PROTO_IRTP		28		/* Reliable Transaction */
#define	L_PROTO_TP		29 		/* tp-4 w/ class negotiation */
#define	L_PROTO_BLT		30		/* Bulk Data Transfer */
#define	L_PROTO_NSP		31		/* Network Services */
#define	L_PROTO_INP		32		/* Merit Internodal */
#define	L_PROTO_SEP		33		/* Sequential Exchange */
#define	L_PROTO_3PC		34		/* Third Party Connect */
#define	L_PROTO_IDPR		35		/* InterDomain Policy Routing */
#define	L_PROTO_XTP		36		/* XTP */
#define	L_PROTO_DDP		37		/* Datagram Delivery */
#define	L_PROTO_CMTP		38		/* Control Message Transport */
#define	L_PROTO_TPXX		39		/* TP++ Transport */
#define	L_PROTO_IL		       40		/* IL transport protocol */
#define	L_PROTO_IPV6		41		/* IP6 header */
#define	L_PROTO_SDRP		42		/* Source Demand Routing */
#define	L_PROTO_ROUTING   43		/* IP6 routing header */
#define	L_PROTO_FRAGMENT	44		/* IP6 fragmentation header */
#define	L_PROTO_IDRP		45		/* InterDomain Routing*/
#define	L_PROTO_RSVP		46 		/* resource reservation */
#define	L_PROTO_GRE		47		/* General Routing Encap. */
#define	L_PROTO_MHRP		48		/* Mobile Host Routing */
#define	L_PROTO_BHA		49		/* BHA */
#define	L_PROTO_ESP		50		/* IP6 Encap Sec. Payload */
#define	L_PROTO_AH		51		/* IP6 Auth Header */
#define	L_PROTO_INLSP		52		/* Integ. Net Layer Security */
#define	L_PROTO_SWIPE		53		/* IP with encryption */
#define	L_PROTO_NHRP		54		/* Next Hop Resolution */
/* 55-57: Unassigned */
#define	L_PROTO_ICMPV6	58		/* ICMP6 */
#define	L_PROTO_NONE		59		/* IP6 no next header */
#define	L_PROTO_DSTOPTS	60		/* IP6 destination option */
#define	L_PROTO_AHIP		61		/* any host internal protocol */
#define	L_PROTO_CFTP		62		/* CFTP */
#define	L_PROTO_HELLO		63		/* "hello" routing protocol */
#define	L_PROTO_SATEXPAK	64		/* SATNET/Backroom EXPAK */
#define	L_PROTO_KRYPTOLAN	65		/* Kryptolan */
#define	L_PROTO_RVD		66		/* Remote Virtual Disk */
#define	L_PROTO_IPPC		67		/* Pluribus Packet Core */
#define	L_PROTO_ADFS		68		/* Any distributed FS */
#define	L_PROTO_SATMON	69		/* Satnet Monitoring */
#define	L_PROTO_VISA		70		/* VISA Protocol */
#define	L_PROTO_IPCV		71		/* Packet Core Utility */
#define	L_PROTO_CPNX		72		/* Comp. Prot. Net. Executive */
#define	L_PROTO_CPHB		73		/* Comp. Prot. HeartBeat */
#define	L_PROTO_WSN		74		/* Wang Span Network */
#define	L_PROTO_PVP		75		/* Packet Video Protocol */
#define	L_PROTO_BRSATMON	76		/* BackRoom SATNET Monitoring */
#define	L_PROTO_ND		77		/* Sun net disk proto (temp.) */
#define	L_PROTO_WBMON	78		/* WIDEBAND Monitoring */
#define	L_PROTO_WBEXPAK	79		/* WIDEBAND EXPAK */
#define	L_PROTO_EON		80		/* ISO cnlp */
#define	L_PROTO_VMTP		81		/* VMTP */
#define	L_PROTO_SVMTP		82		/* Secure VMTP */
#define	L_PROTO_VINES		83		/* Banyon VINES */
#define	L_PROTO_TTP		84		/* TTP */
#define	L_PROTO_IGP		85		/* NSFNET-IGP */
#define	L_PROTO_DGP		86		/* dissimilar gateway prot. */
#define	L_PROTO_TCF		87		/* TCF */
#define	L_PROTO_IGRP		88		/* Cisco/GXS IGRP */
#define	L_PROTO_OSPFIGP	89		/* OSPFIGP */
#define	L_PROTO_SRPC		90		/* Strite RPC protocol */
#define	L_PROTO_LARP		91		/* Locus Address Resoloution */
#define	L_PROTO_MTP		92		/* Multicast Transport */
#define	L_PROTO_AX25		93		/* AX.25 Frames */
#define	L_PROTO_IPEIP		94		/* IP encapsulated in IP */
#define	L_PROTO_MICP		95		/* Mobile Int.ing control */
#define	L_PROTO_SCCSP		96		/* Semaphore Comm. security */
#define	L_PROTO_ETHERIP	97		/* Ethernet IP encapsulation */
#define	L_PROTO_ENCAP		98		/* encapsulation header */
#define	L_PROTO_APES		99		/* any private encr. scheme */
#define	L_PROTO_GMTP		100		/* GMTP*/
#define	L_PROTO_IPCOMP	108		/* payload compression (IPComp) */
#define	L_PROTO_PIM		103		/* Protocol Independent Mcast */
#define	L_PROTO_MLD		58		/* MLD */
#define	L_PROTO_PGM		113		/* PGM */
#define  L_PROTO_SCTP		132		/* SCTP (RFC2960) */
/* 134-254: Partly Unassigned */
/* 255: Reserved */
/* BSD Private, local use, namespace incursion */
#define	L_PROTO_DIVERT	254		/* divert pseudo-protocol */
#define	L_PROTO_RAW		255		/* raw IP packet */
#define	L_PROTO_MAX		256

/* last return value of *_input(), meaning "all job for this pkt is done".  */
#define	L_PROTO_DONE		257

/*each field's offset in ip header*/
#define L_PROTO_IP_HDR_OFFSET_DIP  (16)
#define L_PROTO_IP_HDR_OFFSET_SIP  (20)
#define L_PROTO_IP_HDR_OFFSET_PROTO (9)
#define L_PROTO_IPV6_HDR_OFFSET_PROTO (6)
#define L_PROTO_IPV6_HDR_OFFSET_DIP  (24)


#define L_PROTO_MULTICAST_ADDRESS        (0xE0000000UL)

#define L_PROTO_ALLHOSTS_GROUP             (0xE0000001UL)/* 224.0.0.1 */

#define L_PROTO_ALLRTRS_GROUP               (0xE0000002UL)/* 224.0.0.2 */

#define L_PROTO_PIM_ALLRTRS_GROUP       (0xE000000DUL)/* 224.0.0.13 */

#define L_PROTO_IGMPv3_RTRS_GROUP       (0xE0000016UL)/* 224.0.0.22 */

#define L_PROTO_MAX_LOCAL_GROUP          (0xE00000FFUL)/* 224.0.0.255 */

#define L_PROTO_SSM_RANGE_ADDRESS       (0xE8000000UL)/* 232.0.0.0/8 */

#endif /*_L_PROTO_H*/
