/* FUNCTION NAME: L4_MGR.H
* PURPOSE:
* NOTES:
*
*REASON:
*    DESCRIPTION:
*    CREATOR:
*    Date       04/07/2005
*
* Copyright(C)      Accton Corporation, 2005
*/

#ifndef _L4_MGR_H
#define _L4_MGR_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_cpnt.h"
#include "sysfun.h"
#include "rule_type.h"
#include "l4_backdoor.h"
#if (SYS_CPNT_COS == TRUE)
#include "cos_vm.h"
#endif
#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
#include "marker_vm.h"
#endif
#if (SYS_CPNT_DAI == TRUE)
#include "dai_type.h"
#endif

/* NAMING CONSTANT DECLARATIONS
 */
#if ((SYS_CPNT_DHCPSNP == TRUE) || (SYS_CPNT_WEBAUTH == TRUE) || (SYS_CPNT_DHCP_RELAY_OPTION82 == TRUE))

// FIXME: Remove this enum
/* define following constants for IP source guard APIs
 */
enum
{
    L4_MGR_DO_ACTION_DELETE_RULE        = FALSE,
    L4_MGR_DO_ACTION_PERMIT             = TRUE,
    L4_MGR_DO_ACTION_DROP               = TRUE,
};

#endif /* #if ((SYS_CPNT_DHCPSNP == TRUE) || (SYS_CPNT_WEBAUTH == TRUE)) */

#define MAX_PRE_VAL_DSCP    8
#define MAX_PHB_VAL_QUEUE   8

/* definitions of command which will be used in ipc message
 */
enum
{
    L4_MGR_IPCCMD_QOS_BINDPORT2POLICYMAP,
    L4_MGR_IPCCMD_QOS_UNBINDPORTFROMPOLICYMAP,
    L4_MGR_IPCCMD_QOS_BINDPORT2_DYN_POLICYMAP,
    L4_MGR_IPCCMD_QOS_UNBINDPORTFROM_DYN_POLICYMAP,
    L4_MGR_IPCCMD_QOS_GETPOLICYMAPNAMEBYPORT,
    L4_MGR_IPCCMD_QOS_GETRUNNINGPOLICYMAPNAMEBYPORT,
    L4_MGR_IPCCMD_QOS_GETPOLICYMAPIDBYNAME,
    L4_MGR_IPCCMD_QOS_CREATEPOLICYMAPBYNAME,
    L4_MGR_IPCCMD_QOS_RENAMEPOLICYMAP,
    L4_MGR_IPCCMD_QOS_SETPOLICYMAPDESCBYNAME,
    L4_MGR_IPCCMD_QOS_DELPOLICYMAPBYNAME,
    L4_MGR_IPCCMD_QOS_GETNEXTPOLICYMAP,
    L4_MGR_IPCCMD_QOS_GETNEXTRUNNINGPOLICYMAP,
    L4_MGR_IPCCMD_QOS_ADDELEMENT2POLICYMAP,
    L4_MGR_IPCCMD_QOS_DELELEMENTMETERFROMPOLICYMAP,
    L4_MGR_IPCCMD_QOS_DELELEMENTACTIONFROMPOLICYMAP,
    L4_MGR_IPCCMD_QOS_DELELEMENTFROMPOLICYMAP,
    L4_MGR_IPCCMD_QOS_GETNEXTELEMENTFROMPOLICYMAP,
    L4_MGR_IPCCMD_QOS_GETNEXTRUNNINGELEMENTFROMPOLICYMAP,
    L4_MGR_IPCCMD_QOS_GETCLASSMAPIDBYNAME,
    L4_MGR_IPCCMD_QOS_CREATECLASSMAPBYNAME,
    L4_MGR_IPCCMD_QOS_RENAMECLASSMAP,
    L4_MGR_IPCCMD_QOS_SETCLASSMAP,
    L4_MGR_IPCCMD_QOS_DELCLASSMAP,
    L4_MGR_IPCCMD_QOS_GETCLASSMAPBYNAME,
    L4_MGR_IPCCMD_QOS_GETNEXTCLASSMAP,
    L4_MGR_IPCCMD_QOS_GETNEXTRUNNINGCLASSMAP,
    L4_MGR_IPCCMD_QOS_ADDELEMENT2CLASSMAP,
    L4_MGR_IPCCMD_QOS_DELELEMENTFROMCLASSMAP,
    L4_MGR_IPCCMD_QOS_GETNEXTELEMENTFROMCLASSMAP,
    L4_MGR_IPCCMD_QOS_GETNEXTRUNNINGELEMENTFROMCLASSMAP,
    L4_MGR_IPCCMD_ACL_BINDPORT2ACL,
    L4_MGR_IPCCMD_ACL_UNBINDPORTFROMACL,
    L4_MGR_IPCCMD_ACL_BIND_PORT_2_DYN_ACL,
    L4_MGR_IPCCMD_ACL_UNBIND_PORT_FROM_DYN_ACL,
    L4_MGR_IPCCMD_ACL_GETNEXTACLBYPORT,
    L4_MGR_IPCCMD_ACL_GETNEXTUIACLBYPORTANDBINDINGTYPE,
    L4_MGR_IPCCMD_ACL_GETNEXTRUNNINGACLBYPORT,
    L4_MGR_IPCCMD_ACL_GETACLIDBYNAME,
    L4_MGR_IPCCMD_ACL_GETACLIDBYNAMEANDTYPE,
    L4_MGR_IPCCMD_ACL_CREATEACL,
    L4_MGR_IPCCMD_ACL_DELACLBYNAMEANDACLTYPE,
    L4_MGR_IPCCMD_ACL_GETNEXTACL,
    L4_MGR_IPCCMD_ACL_GETNEXTRUNNINGACL,
    L4_MGR_IPCCMD_ACL_INITACE,
    L4_MGR_IPCCMD_ACL_INIT_UI_ACE,
    L4_MGR_IPCCMD_ACL_SET_ACE_PATTERN_TO_DEFAULT_BY_TYPE,
    L4_MGR_IPCCMD_ACL_SET_UI_ACE_2_ACL,
    L4_MGR_IPCCMD_ACL_DEL_UI_ACE_FROM_ACL,
    L4_MGR_IPCCMD_ACL_GET_NEXT_UI_ACE_BY_ACL,
    L4_MGR_IPCCMD_ACL_GET_NEXT_HARDWARD_UI_ACE_BY_ACL,
    L4_MGR_IPCCMD_ACL_GET_NEXT_RUNNING_UI_ACE_BY_ACL,
    L4_MGR_IPCCMD_ACL_SETACE2ACL   ,
    L4_MGR_IPCCMD_ACL_DELACEFROMACL,
    L4_MGR_IPCCMD_ACL_GETNEXTACEBYACL,
    L4_MGR_IPCCMD_ACL_GET_NEXT_HARDWARE_ACE_BY_ACL,
    L4_MGR_IPCCMD_ACL_GET_NEXT_MIB_ACE_BY_PORT,
    L4_MGR_IPCCMD_ACL_CLEAR_ACL_COUNTER,
    L4_MGR_IPCCMD_ACL_ADD_ACL_MIRROR_ENTRY,
    L4_MGR_IPCCMD_ACL_REMOVE_ACL_MIRROR_ENTRY,
    L4_MGR_IPCCMD_ACL_GET_NEXT_ACL_MIRROR_ENTRY,
    L4_MGR_IPCCMD_ACL_GET_NEXT_RUNNING_ACL_MIRROR_ENTRY,
    L4_MGR_IPCCMD_ACL_GETTCAMUTILIZATION,
    L4_MGR_IPCCMD_ACL_GET_TCAM_ENTRY,
    L4_MGR_IPCCMD_ACL_GET_NEXT_TCAM_ENTRY,
    L4_MGR_IPCCMD_ACL_GET_SUPPORTED_TCAM_CAPABILITY_BITMAP,

    L4_MGR_IPCCMD_COS_SETIPPRECEDENCEDSCPMAPPINGSTATUS,
    L4_MGR_IPCCMD_COS_GETIPPRECEDENCEDSCPMAPPINGSTATUS,
    L4_MGR_IPCCMD_COS_GETRUNNINGIPPRECEDENCEDSCPMAPPINGSTATUS,
    L4_MGR_IPCCMD_COS_SETTCPPORTMAPPINGSTATUS,
    L4_MGR_IPCCMD_COS_GETTCPPORTMAPPINGSTATUS,
    L4_MGR_IPCCMD_COS_GETRUNNINGTCPPORTMAPPINGSTATUS,
    L4_MGR_IPCCMD_COS_SETIPPRECEDENCETOCOS,
    L4_MGR_IPCCMD_COS_GETIPPRECEDENCETOCOS,
    L4_MGR_IPCCMD_COS_DELIPPRECEDENCETOCOS,
    L4_MGR_IPCCMD_COS_DELIPPRECEDENCELPORTALLENTRY,
    L4_MGR_IPCCMD_COS_SETDSCPTOCOS,
    L4_MGR_IPCCMD_COS_SETDSCPTOCOSSTATUS,
    L4_MGR_IPCCMD_COS_GETDSCPTOCOSSTATUS,
    L4_MGR_IPCCMD_COS_GETRUNNINGDSCPTOCOSSTATUS,
    L4_MGR_IPCCMD_COS_GETDSCPTOCOS,
    L4_MGR_IPCCMD_COS_DELDSCPTOCOS,
    L4_MGR_IPCCMD_COS_DELDSCPLPORTALLENTRY,
    L4_MGR_IPCCMD_COS_SETPORTTOCOS,
    L4_MGR_IPCCMD_COS_GETPORTTOCOS,
    L4_MGR_IPCCMD_COS_DELPORTTOCOS,
    L4_MGR_IPCCMD_COS_GETNEXTIPPRECEDENCETOCOS,
    L4_MGR_IPCCMD_COS_GETNEXTDSCPTOCOS,
    L4_MGR_IPCCMD_COS_GETNEXTPORTTOCOS,
    L4_MGR_IPCCMD_COS_GETRUNNINGIPPRECEDENCETOCOS,
    L4_MGR_IPCCMD_COS_GETRUNNINGDSCPTOCOS,
    L4_MGR_IPCCMD_COS_GETRUNNINGPORTTOCOS,
    L4_MGR_IPCCMD_COS_GETNEXTRUNNINGIPPRECEDENCETOCOS,
    L4_MGR_IPCCMD_COS_GETNEXTRUNNINGDSCPTOCOS,
    L4_MGR_IPCCMD_COS_GETNEXTRUNNINGPORTTOCOS,
    L4_MGR_IPCCMD_COS_PORTCOPY,
    L4_MGR_IPCCMD_COS_ADDCOSENTRY,
    L4_MGR_IPCCMD_COS_DELCOSENTRY,
    L4_MGR_IPCCMD_COS_SETCOSENTRYROWSTATUS,
    L4_MGR_IPCCMD_COS_SETCOSENTRY,
    L4_MGR_IPCCMD_COS_GETCOSENTRY,
    L4_MGR_IPCCMD_COS_GETNEXTCOSENTRY,
    L4_MGR_IPCCMD_COS_SNMPGETNEXTCOSENTRY,
    L4_MGR_IPCCMD_COS_GETNEXTRUNNINGCOSENTRY,
    L4_MGR_IPCCMD_MARKER_SETMARKERENTRY_DSCP,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVPORTENTRY    ,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVPORTENTRY,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPORTPOLICYMAPINDEX,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPORTINGRESSIPACLINDEX,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPORTINGRESSIPV6ACLINDEX,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPORTINGRESSMACACLINDEX,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVPOLICYMAPENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVPOLICYMAPENTRY,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPNAME,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPDESCRIPTION,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPSTATUS,
    L4_MGR_IPCCMD_QOS_SETPOLICYDIFFSERVPOLICYMAPATTACHCTLACTION,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVPOLICYMAPELEMENTENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVPOLICYMAPELEMENTENTRY,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPELEMENTCLASSMAPINDEX,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPELEMENTMETERINDEX,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPELEMENTACTIONINDEX,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVPOLICYMAPELEMENTSTATUS,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVCLASSMAPENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVCLASSMAPENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVCLASSMAPINDEXOFPOLICYMAP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVCLASSMAPNAME,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVCLASSMAPDESCRIPTION,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVCLASSMAPMATCHTYPE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVCLASSMAPSTATUS,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVCLASSMAPATTACHCTLACTION,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVACLENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVACLENTRY,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACLTYPE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACLNAME,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACLSTATUS,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACLATTACHCTLACTION,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVIPACEENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVIPACEENTRY,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACETYPE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEACCESS,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACESOURCEIPADDR,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACESOURCEIPADDRBITMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEDESTIPADDR,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEDESTIPADDRBITMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEPROTOCOL,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEPREC,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACETOS,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEDSCP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACESOURCEPORTOP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEMINSOURCEPORT,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEMAXSOURCEPORT,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACESOURCEPORTBITMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEDESTPORTOP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEMINDESTPORT,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEMAXDESTPORT,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEDESTPORTBITMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACECONTROLCODE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACECONTROLCODEBITMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACEICMPTYPE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPACESTATUS,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IP_ACE_TIME_RANGE,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVMACACEENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVMACACEENTRY,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEACCESS,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEPKTFORMAT,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACESOURCEMACADDR,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACESOURCEMACADDRBITMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEDESTMACADDR,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEDESTMACADDRBITMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEVIDOP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMINVID,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEVIDBITMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMAXVID,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEETHERTYPEOP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEETHERTYPEBITMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMINETHERTYPE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMAXETHERTYPE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACESTATUS,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_TIME_RANGE,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IP_SOURCE_ADDR,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IP_SOURCE_MASK,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IP_DEST_ADDR,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IP_DEST_MASK,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IPV6_SOURCE_ADDR,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IPV6_SOURCE_PREFIX_LEN,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IPV6_DEST_ADDR,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IPV6_DEST_PREFIX_LEN,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_IP_PROTOCOL,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_SOURCE_PORT_OP,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_SOURCE_PORT,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_SOURCE_PORT_MASK,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_DEST_PORT_OP,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_DEST_PORT,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_MAC_ACE_L4_DEST_PORT_MASK,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVIPV6ACEENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVIPV6ACEENTRY,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACETYPE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACEACCESS,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACESOURCEIPADDR,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACESOURCEIPADDRPREFIX,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACEDESTIPADDR,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACEDESTIPADDRPREFIX,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACENEXTHEADER,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACEDSCP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACEFLOWLABEL,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVIPV6ACESTATUS,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_TIME_RANGE,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_SOURCE_PORT_OP,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_SOURCE_PORT,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_SOURCE_PORT_BITMASK,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_DEST_PORT_OP,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_DEST_PORT,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_IPV6_ACE_DEST_PORT_BITMASK,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVACTIONENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVACTIONENTRY,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONBITMAP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONPKTNEWPRI,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONPKTNEWPHB,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONPKTNEWIPPREC,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONPKTNEWDSCP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONREDPKTNEWDSCP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONYELLOWPKTNEWDSCP,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_ACTION_GREEN_PKT_NEW_DSCP,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_ACTION_GREEN_TRANSMIT,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONYELLOWDROP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONREDDROP,
    L4_MGR_IPCCMD_QOS_SET_DIFFSERV_ACTION_PKT_PRI,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVACTIONSTATUS,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVMETERENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVMETERENTRY,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERMODEL,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERRATE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERPEAKRATE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERBURSTSIZE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERPEAKBURSTSIZE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERINTERVAL,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMETERSTATUS,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVPOLICYMAPHWCOUNTER,
    L4_MGR_IPCCMD_QOS_CLEARDIFFSERVPOLICYMAPHWCOUNTER,
    L4_MGR_IPCCMD_QOS_GET_DIFFSERV_ACL_HW_COUNTER_ENTTRY,
    L4_MGR_IPCCMD_QOS_GET_NEXT_DIFFSERV_ACL_HW_COUNTER_ENTRY,
    L4_MGR_IPCCMD_QOS_SET_POLICYMAPPORT_POLICYMAPINDEX,
    L4_MGR_IPCCMD_QOS_SET_POLICYMAPPORT_ROWSTATUS,
    L4_MGR_IPCCMD_QOS_GET_POLICYMAPPORT_ENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXT_POLICYMAPPORT_ENTRY,
    L4_MGR_IPCCMD_QOS_SET_ACCESSGROUP_ACLINDEX,
    L4_MGR_IPCCMD_QOS_SET_ACCESSGROUP_TIMERANGENAME,
    L4_MGR_IPCCMD_QOS_SET_ACCESSGROUP_COUNTERSTATUS,
    L4_MGR_IPCCMD_QOS_SET_ACCESSGROUP_ROWSTATUS,
    L4_MGR_IPCCMD_QOS_GET_ACCESSGROUP_ENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXT_ACCESSGROUP_ENTRY,
    L4_MGR_IPCCMD_SETSRCIPFILTERBYLPORT,
    L4_MGR_IPCCMD_SETSRCMACSRCIPFILTERBYLPORT1,
    L4_MGR_IPCCMD_SETIPPKTACTIONBYLPORT,
    L4_MGR_IPCCMD_SETREDIRECTHTTPCLIENTPACKETS,
    L4_MGR_IPCCMD_SETPERMITSRCIPPACKETSBYLPORT,
    L4_MGR_IPCCMD_SETDENYIPPACKETSBYLPORT,

    L4_MGR_IPCCMD_QOS_SETINGRESSDSCP2COLOR,
    L4_MGR_IPCCMD_QOS_GETINGRESSDSCP2COLOR,
    L4_MGR_IPCCMD_QOS_SETINGRESSDSCP2COS,
    L4_MGR_IPCCMD_QOS_GETINGRESSDSCP2COS,
    L4_MGR_IPCCMD_QOS_GETNEXTINGRESSDSCP2COS,
    L4_MGR_IPCCMD_QOS_GETRUNNINGINGRESSDSCP2COLOR,
    L4_MGR_IPCCMD_QOS_GETRUNNINGINGRESSDSCP2COS,
    L4_MGR_IPCCMD_QOS_SETTCPUDP2DSCP,
    L4_MGR_IPCCMD_QOS_GETTCPUDP2DSCP,
    L4_MGR_IPCCMD_QOS_GETNEXTTCPUDP2DSCP,
    L4_MGR_IPCCMD_QOS_GETRUNNINGTCPUDP2DSCP,
    L4_MGR_IPCCMD_TRAPPACKET2CPU,
    L4_MGR_IPCCMD_QOS_GETDIFFSERVARPACEENTRY,
    L4_MGR_IPCCMD_QOS_GETNEXTDIFFSERVARPACEENTRY,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEACTION,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEPKTTYPE,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACESRCIPADDR,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACESRCIPADDRMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEDSTIPADDR,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEDSTIPADDRMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACESRCMACADDR,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACESRCMACADDRMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEDSTMACADDR,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACEDSTMACADDRMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACESTATUS,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVARPACELOGSTATUS,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACECOSOP,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACECOSBITMASK,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMINCOS,
    L4_MGR_IPCCMD_QOS_SETDIFFSERVMACACEMAXCOS,
    L4_MGR_IPCCMD_TRAPLINKLOCALTOCPU,
    L4_MGR_IPCCMD_QOS_ENABLEPORTPRIORITYMODE,
    L4_MGR_IPCCMD_QOS_DISABLEPORTPRIORITYMODE,

    L4_MGR_IPCCMD_QOS_SETTRUSTMODE,
    L4_MGR_IPCCMD_QOS_GETTRUSTMODE,
    L4_MGR_IPCCMD_QOS_SETPORTPRIORITYTRUSTMODE,
    L4_MGR_IPCCMD_QOS_GETPORTPRIORITYTRUSTMODE,

    L4_MGR_IPCCMD_QOS_SETINGRESSCOS2DSCP,
    L4_MGR_IPCCMD_QOS_GETINGRESSCOS2DSCP,
    L4_MGR_IPCCMD_QOS_GETNEXTINGRESSCOS2DSCP,
    L4_MGR_IPCCMD_QOS_SET_COS_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_COS_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_NEXT_COS_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_ACTIVE_COS_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_COS_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_SET_PORT_COS_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_PORT_COS_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_COS_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_COS_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_PORT_COS_2_INTERNAL_DSCP,

    L4_MGR_IPCCMD_QOS_SETINGRESSPRE2DSCP,
    L4_MGR_IPCCMD_QOS_RESETINGRESSPRE2DSCP,
    L4_MGR_IPCCMD_QOS_GETINGRESSPRE2DSCP,
    L4_MGR_IPCCMD_QOS_SET_PORT_PRE_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_PORT_PRE_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_PRE_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_PRE_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_RUNNING_PORT_PRE_2_INTERNAL_DSCP,

    L4_MGR_IPCCMD_QOS_SETINGRESSDSCP2DSCP,
    L4_MGR_IPCCMD_QOS_GETINGRESSDSCP2DSCP,
    L4_MGR_IPCCMD_QOS_SET_DSCP_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_DSCP_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_NEXT_DSCP_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_ACTIVE_DSCP_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_DSCP_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_SET_PORT_DSCP_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_PORT_DSCP_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_DSCP_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_DSCP_2_INTERNAL_DSCP,
    L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_PORT_DSCP_2_INTERNAL_DSCP,

    L4_MGR_IPCCMD_QOS_SETINGRESSDSCP2QUEUE,
    L4_MGR_IPCCMD_QOS_SETPORTINGRESSDSCP2QUEUE,
    L4_MGR_IPCCMD_QOS_GETINGRESSDSCP2QUEUE,
    L4_MGR_IPCCMD_QOS_GETPORTINGRESSDSCP2QUEUE,
    L4_MGR_IPCCMD_QOS_GETPRIORITYINGRESSDSCP2QUEUE,  // remove
    L4_MGR_IPCCMD_QOS_GETPORTPRIORITYINGRESSDSCP2QUEUE, // remove
    L4_MGR_IPCCMD_QOS_SET_INGRESS_PHB_2_QUEUE,
    L4_MGR_IPCCMD_QOS_GET_INGRESS_PHB_2_QUEUE,
    L4_MGR_IPCCMD_QOS_GET_ACTIVE_INGRESS_PHB_2_QUEUE,
    L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_INGRESS_PHB_2_QUEUE,
    L4_MGR_IPCCMD_QOS_SET_PORT_INGRESS_PHB_2_QUEUE,
    L4_MGR_IPCCMD_QOS_GET_PORT_INGRESS_PHB_2_QUEUE,
    L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_INGRESS_PHB_2_QUEUE,
    L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_INGRESS_PHB_2_QUEUE,
    L4_MGR_IPCCMD_QOS_GET_NEXT_RUNNING_PORT_INGRESS_PHB_2_QUEUE,

    L4_MGR_IPCCMD_QOS_SETPORTEGRESSQUEUEMODE,
    L4_MGR_IPCCMD_QOS_GETPORTEGRESSQUEUEMODE,
    L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_EGRESS_QUEUE_MODE,
    L4_MGR_IPCCMD_QOS_SET_PRIORITY_EGRESSQUEUEMODE,
    L4_MGR_IPCCMD_QOS_GET_PRIORITY_EGRESS_QUEUE_MODE,
    L4_MGR_IPCCMD_QOS_GET_ACTIVE_EGRESS_QUEUE_MODE,
    L4_MGR_IPCCMD_QOS_SETPORTPRIORITYEGRESSQUEUEMODE,
    L4_MGR_IPCCMD_QOS_GET_PORT_PRIORITY_EGRESS_QUEUE_MODE,
    L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_EGRESS_QUEUE_MODE,
    L4_MGR_IPCCMD_QOS_RESETPORTPRIORITYEGRESSQUEUEMODE,

    L4_MGR_IPCCMD_QOS_SETPORTEGRESSWRRQUEUEWEIGHT,
    L4_MGR_IPCCMD_QOS_SET_EGRESS_WRRQUEUEWEIGHT,
    L4_MGR_IPCCMD_QOS_GETPORTEGRESSWRRQUEUEWEIGHT,
    L4_MGR_IPCCMD_QOS_GET_NEXT_EGRESS_QUEUE_WEIGHT,
    L4_MGR_IPCCMD_QOS_GET_NEXT_PORT_EGRESS_QUEUE_WEIGHT,
    L4_MGR_IPCCMD_QOS_SET_PRIORITY_EGRESS_WRR_QUEUE_WEIGHT,
    L4_MGR_IPCCMD_QOS_GET_PRIORITY_EGRESS_WRR_QUEUE_WEIGHT,
    L4_MGR_IPCCMD_QOS_GET_ACTIVE_EGRESS_QUEUE_WEIGHT,
    L4_MGR_IPCCMD_QOS_GET_RUNNING_EGRESS_QUEUE_WEIGHT,
    L4_MGR_IPCCMD_QOS_SET_PORT_PRIORITY_EGRESS_WRR_QUEUE_WEIGHT,
    L4_MGR_IPCCMD_QOS_GET_PORT_PRIORITY_EGRESS_WRR_QUEUE_WEIGHT,
    L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_EGRESS_QUEUE_WEIGHT,
    L4_MGR_IPCCMD_QOS_RESETPORTPRIORITYEGRESSWRRQUEUEWEIGHT,
    L4_MGR_IPCCMD_QOS_GET_RUNNING_PORT_EGRESS_QUEUE_WEIGHT,
    L4_MGR_IPCCMD_QOS_SET_PRIORITY_EGRESS_QUEUE_STRICT_STATUS,
    L4_MGR_IPCCMD_QOS_GET_PRIORITY_EGRESS_QUEUE_STRICT_STATUS,
    L4_MGR_IPCCMD_QOS_GET_ACTIVE_EGRESS_QUEUE_STRICT_STATUS,
    L4_MGR_IPCCMD_QOS_SET_PORT_PRIORITY_EGRESS_QUEUE_STRICT_STATUS,
    L4_MGR_IPCCMD_QOS_GET_PORT_PRIORITY_EGRESS_QUEUE_STRICT_STATUS,
    L4_MGR_IPCCMD_QOS_GET_PORT_ACTIVE_EGRESS_QUEUE_STRICT_STATUS,
#if (SYS_CPNT_CLUSTER == TRUE)
    L4_MGR_IPCCMD_SETCLUSTERPKTACTION,
#endif
    L4_MGR_IPCCMD_QOS_SET_BUNDLE_GROUP,
    L4_MGR_IPCCMD_QOS_DEL_BUNDLE_GROUP,
    L4_MGR_IPCCMD_QOS_GET_BUNDLE_GROUP,
    L4_MGR_IPCCMD_QOS_GET_NEXT_BUNDLE_GROUP,

    L4_MGR_IPCCMD_PBR_BIND_ACL,
    L4_MGR_IPCCMD_PBR_UNBIND_ACL,

    /* ==================================================
     *  Aasynchronism IPC start
     * ==================================================*/
    L4_MGR_IPCCMD_FOLLOWISASYNCHRONISMIPC,

};

/*use to the definition of IPC message buffer*/
typedef struct
{
    union
    {
        UI32_T cmd;          /*cmd fnction id*/
        BOOL_T result_bool;  /*respond bool return*/
        UI32_T result_ui32;  /*respond ui32 return*/
        UI32_T result_i32;  /*respond i32 return*/
    }type;

    union
    {
        UI32_T  ui32_v;
        I32_T   i32_v;
        UI16_T  ui16_v;
        I16_T   i16_v;
        UI8_T   ip4_v[4];
        UI8_T   ui8_v;
        I8_T    i8_v;
        BOOL_T  bool_v;

        struct
        {
            UI32_T  uport_ifindex;
            UI32_T  inout_dir;
            char    pmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        }pmapuport_inout_name;

        struct
        {
            UI32_T  uport_ifindex;
            UI32_T  inout_dir;
            char    pmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        }pmapuport_name;

        struct
        {
            UI32_T  pmap_id;
            char    pmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        }pmapid_name;

        struct
        {
            char  old_pmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
            char  new_pmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        }pmapname_rename;

        struct
        {
            char  pmap_name  [SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
            char  description[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];
        }pmapname_desc;

        struct
        {
            UI32_T                      pmap_id;
            RULE_TYPE_UI_PolicyMap_T    pmap_ui;
        }pmapid_ui;

        struct
        {
            char  pmap_name [SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
            char  cmap_name [SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
            RULE_TYPE_TBParamEntry_T    meter_entry;
            RULE_TYPE_UI_Action_T       action_entry;
        }pmap_element;

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
        struct
        {
            char pmap_name [SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
            UI32_T index;
            RULE_TYPE_UI_BundleGroup_T bundle_group;
        }pmap_bundle_group;
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

        struct
        {
            char  pmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
            char  cmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        }policy_class;

        struct
        {
            UI32_T                          pmap_id;
            RULE_TYPE_UI_PolicyElement_T    pele_ui;
        }pmapid_uiele;

        struct
        {
            UI32_T  cmap_id;
            char    cmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        }cmapid_name;

        struct
        {
            char  old_cmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
            char  new_cmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        }cmapname_rename;

        struct
        {
            UI32_T                  cmap_id;
            RULE_TYPE_UI_ClassMap_T cmap_ui;
        }cmapid_ui;

        struct
        {
            char                            cmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
            RULE_TYPE_UI_ClassMapElement_T  cele_ui;
        }cmapname_uiele;

        struct
        {
            UI32_T                          cmap_id;
            UI32_T                          eleid;
            RULE_TYPE_UI_ClassMapElement_T  cele_ui;
        }cmapid_eleid_uiele;

        struct
        {
            UI32_T                          uport_ifindex;
            UI32_T                          acl_type;
            RULE_TYPE_COUNTER_ENABLE_T      counter_enable;
            char                            acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1];
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            UI32_T                          time_range_index;
            UI8_T                            time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];
#endif
            BOOL_T                          ingress_flag;
        }acluport_type_name_flag;

        struct
        {
            UI32_T                      uport_ifindex;
            UI32_T                      inout_dir;
            UI32_T                      acl_id;
            RULE_TYPE_UI_AclEntry_T     acl_ui;

            /* 2008-07-01, Jinfeng Chen:
                Because of adding egress support, we can't get
                precedence by invoking
                RULE_OM_LocalGetAclPrecedenceOnPort,
                in fact, the fuction above is not valid when system supports
                both ingress and egress binding a same ACL.
                So add this member.
             */
            UI32_T                      precedence;
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            UI8_T                      time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];
#endif
            RULE_TYPE_COUNTER_ENABLE_T  counter_enable;
        }acluport_id_ui;

        struct
        {
            UI32_T  acl_id;
            UI32_T  acl_type;
            char    acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1];
        }aclid_type_name;

        struct
        {
            UI32_T                  acl_id;
            RULE_TYPE_UI_AclEntry_T acl_ui;
        }aclid_ui;

        RULE_TYPE_Ace_Entry_T ace_entry;
        RULE_TYPE_UI_Ace_Entry_T ui_ace_entry;

        struct
        {
            RULE_TYPE_Ace_Entry_T  ace_entry;
            RULE_TYPE_AclType_T    ace_type;
        }ace_entry_type;

        struct
        {
            UI32_T                   acl_type;
            char                     acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1];
            RULE_TYPE_Ace_Entry_T    ace_entry;
        }acltype_name_ace;

        struct
        {
            UI32_T                   acl_type;
            char                     acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1];
            RULE_TYPE_UI_Ace_Entry_T ace;
        }acltype_name_uiace;

        struct
        {
            UI32_T                  acl_id;
            UI32_T                  ace_id;
            RULE_TYPE_Ace_Entry_T   ace_entry;
        }aclid_aceid_ace;

        struct
        {
            UI32_T                    acl_id;
            UI32_T                    ace_id;
            RULE_TYPE_UI_Ace_Entry_T  ace;
        }aclid_aceid_uiace;

        struct
        {
            UI32_T                      uport_ifindex;
            RULE_TYPE_InOutDirection_T  inout_profile;
            UI32_T                      acl_id;
            UI32_T                      ace_id;
            RULE_TYPE_Ace_Entry_T       ace_entry;
        }port_dir_aclid_aceid_ace;

        struct
        {
            UI32_T                     uport_ifindex;
            RULE_TYPE_InOutDirection_T inout_profile;
            UI32_T                     acl_id;
            UI32_T                     ace_id;
            RULE_TYPE_UI_Ace_Entry_T   ace;
        }port_dir_aclid_aceid_uiace;

        struct
        {
            UI32_T                     uport_ifindex;
            RULE_TYPE_InOutDirection_T inout_profile;
            UI32_T                     acl_id;
            UI32_T                     ace_id;
            RULE_TYPE_Ace_Entry_T      ace_entry;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable;
        }port_dir_aclid_aceid_ace_counterenable;

        struct
        {
            UI32_T                  uport_ifindex;
            char                    acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1];
        }port_aclname;

        struct
        {
            UI32_T                  uport_ifindex;
            UI32_T                  acl_index;
            RULE_TYPE_UI_AclEntry_T acl_entry;
        }port_aclid_aclentry;

        struct
        {
            UI32_T                      ace_id;
            RULE_TYPE_INTERFACE_INFO_T  interface;
            char                        acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1];
        }clear_acl_counter;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        struct
        {
            UI32_T                  ace_id;
            UI8_T                   time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];
        } ace_timerange;
#endif

        struct
        {
            UI32_T l_port;
            UI8_T tos;
            UI32_T cos;
        }port_tos_cos;

        struct
        {
            UI32_T l_port;
            UI8_T tos;
        }port_tos;

        struct
        {
            UI32_T l_port;
            UI8_T dscp;
            UI32_T cos;
        }port_dscp_cos;

        struct
        {
            UI32_T l_port;
            UI8_T dscp;
            BOOL_T status;
        }port_dscp_status;

        struct
        {
            UI32_T l_port;
            UI8_T dscp;
        }port_dscp;

        struct
        {
            UI32_T l_port;
            I32_T tcp_port;
            UI32_T cos;
        }port_tcp_cos;

        struct
        {
            UI32_T l_port;
            UI16_T tcp_port;
        }port_tcp;

        struct
        {
            UI32_T map_table_type;
            UI32_T src_port;
            UI8_T  dst_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
        }maptype_port_portlist;

#if (SYS_CPNT_COS == TRUE)
        struct
        {
            UI32_T ifindex;
            COS_TYPE_AclCosEntry_T cos_entry;
        }ifindex_cosentry;

        struct
        {
            UI32_T l_port;
            UI16_T priority;
        }port_priority;

        struct
        {
            UI32_T l_port;
            UI32_T priority;
            UI32_T cos;
            UI32_T cfi;
            UI32_T phb;
            UI32_T color;
        }port_cos_to_dscp;

        struct
        {
            UI32_T l_port;
            UI32_T priority;
            UI32_T pre;
            UI32_T phb;
            UI32_T color;
        } port_pre_to_dscp;

        struct
        {
            UI32_T l_port;
            UI32_T phb_ar[MAX_PRE_VAL_DSCP];
            UI32_T color_ar[MAX_PRE_VAL_DSCP];
        } port_running_pre_to_dscp;

        struct
        {
            UI32_T l_port;
            UI32_T priority;
            UI32_T dscp;
            UI32_T phb;
            UI32_T color;
        } port_dscp_to_dscp;

        struct
        {
            UI32_T l_port;
            UI32_T priority;
            UI32_T mode;
        }port_trust_mode;

        struct
        {
            UI32_T l_port;
            UI32_T priority;
            UI32_T phb;
            UI32_T queue;
        } port_phb_to_queue;

        struct
        {
            UI32_T l_port;
            UI32_T priority;
            UI32_T mode;
        }port_queue_mode;

        struct
        {
            UI32_T l_port;
            UI32_T priority;
            UI32_T queue_id;
            UI32_T weight;
            COS_VM_QUEUE_STRICT_STATUS_T strict_status;
        }port_wrr_queue_weight;
#endif /* #if (SYS_CPNT_COS == TRUE) */

        struct
        {
            UI32_T ifindex;
            char  acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1];
            UI32_T value;
        }index_aclname_value; /* for marker, not really works */

        struct
        {
            UI32_T                      ifindex;
            RULE_TYPE_InOutDirection_T  inout_dir;
            RULE_TYPE_UI_PortEntry_T    port_ui;
        }ifindex_portui;

        struct
        {
            UI32_T ui32_a1;
            UI32_T ui32_a2;
        }ui32a1_ui32a2;

        struct
        {
            UI32_T                      pmap_id;
            RULE_TYPE_MIB_PolicyMap_T   pmap_mib;
        }pmapid_mib;

        struct
        {
            UI32_T  pmap_id;
            char    pmap_desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];
        }pmapid_desc;

        struct
        {
            UI32_T ui32_a1;
            UI32_T ui32_a2;
            UI32_T ui32_a3;
        }ui32a1_ui32a2_ui32a3;

        struct
        {
            UI32_T                      pele_id;
            RULE_TYPE_PolicyElement_T   pele;
        }pmapeleid_ele;

        struct
        {
            UI32_T                      cmap_id;
            RULE_TYPE_MIB_ClassMap_T    cmap_mib;
        }cmapid_mib;

        struct
        {
            UI32_T  cmap_id;
            char    cmap_desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];
        }cmapid_desc;

        struct
        {
            UI32_T u32_a1;
            UI32_T u32_a2;
            UI32_T u32_a3;
            UI32_T u32_a4;
        }u32a1_u32a2_u32a3_u32a4;

        struct
        {
            UI32_T u32_a1;
            UI32_T u32_a2;
            UI32_T u32_a3;
            UI32_T u32_a4;
            UI32_T u32_a5;
        }u32a1_u32a2_u32a3_u32a4_u32a5;

        struct
        {
            UI32_T u32_a1;
            UI32_T u32_a2;
            UI32_T u32_a3;
            UI32_T u32_a4;
            UI32_T u32_a5;
            UI32_T u32_a6;
        }u32a1_u32a2_u32a3_u32a4_u32a5_u32a6;

        struct
        {
            UI32_T              acl_id;
            RULE_TYPE_MIB_Acl_T acl_mib;
        }aclid_mib;

        struct
        {
            UI32_T                    ace_id;
            RULE_TYPE_UI_Ace_Entry_T  ace;
        } aceid_uiace;

        struct
        {
            UI32_T  ace_id;
            UI8_T   mac[SYS_ADPT_MAC_ADDR_LEN];
        }aceid_mac;

        struct
        {
            UI32_T ipv6_ace_index;
            UI8_T ip[SYS_ADPT_IPV6_ADDR_LEN];
        }ipv6aceindex_ip;

        struct
        {
            UI32_T                  act_id;
            RULE_TYPE_MIB_Action_T  act_mib;
        }actid_mib;

        struct
        {
            UI32_T  act_id;
            UI8_T   act_bmp[SIZE_diffServActionList];
        }actid_bmp;

        struct
        {
            UI32_T                      meter_id;
            RULE_TYPE_TBParamEntry_T    meter_param;
        }meterid_param;

        struct
        {
            UI32_T lport;
            UI8_T src_mac[SYS_ADPT_MAC_ADDR_LEN];
            UI32_T sip;
            UI32_T vid;
            UI32_T action;
        }port_mac_ip_vid_action;

        struct
        {
            UI32_T lport;
            UI8_T phb[MAX_PRE_VAL_DSCP];
            UI8_T color[MAX_PRE_VAL_DSCP];
        }pre_dscp;

        struct
        {
            UI32_T lport;
            UI16_T priority;
            UI32_T phb;
            UI8_T queue[MAX_PHB_VAL_QUEUE];
        }dscp_queue;

        struct
        {
            UI32_T lport;
            UI32_T phb;
            UI8_T color[MAX_PHB_VAL_QUEUE];
        }dscp_color;

        struct
        {
            UI32_T lport;
            UI8_T phb;
            UI8_T color;
            UI8_T l_cos;
            UI8_T l_cfi;
            UI8_T dscp_cos[8];
            UI8_T dscp_cfi[8];
        }cos_dscp;

        struct
        {
            UI32_T lport;
            UI8_T phb;
            UI8_T color;
            UI8_T l_dscp;
            UI8_T dscp_dscp[8];
        }dscp_dscp;

        struct
        {
            UI32_T lport;
            UI8_T cos;
            UI8_T cfi;
            UI8_T l_phb;
            UI8_T l_color;
            UI8_T cos_phb[8];
            UI8_T cos_color[8];
        }dscp_cos;

        struct
        {
            UI32_T lport;
            UI32_T sip;
            BOOL_T is_enable;
        }port_ip_status;

        struct
        {
            UI32_T lport;
            BOOL_T is_enable;
        }port_status;

        struct
        {
            BOOL_T                  enable_flag;
            RULE_TYPE_PacketType_T  packet_type;
        }trap2cpu;

#if (SYS_CPNT_IPV6 == TRUE)
    struct
        {
            BOOL_T                  enable_flag;
            UI8_T                   link_local_addr[SYS_ADPT_IPV6_ADDR_LEN];
            UI32_T                  vlan_id;
        }traplinklocal;

#endif

#if (SYS_CPNT_PBR == TRUE)
        struct
        {
            UI32_T vlan;
            UI32_T seq_num;
            UI32_T acl_index;
            RULE_TYPE_PBR_ACTION_T pbr_action;
        } pbr_acl;
#endif /* SYS_CPNT_PBR */

        struct
        {
            UI32_T                     ifindex;
            RULE_TYPE_InOutDirection_T direction;
            UI32_T                     acl_index;
            UI32_T                     ace_index;
            RULE_TYPE_Ace_Entry_T      ace_entry;
        } acl_hw_counter;

        struct
        {
            UI32_T                     ifindex;
            RULE_TYPE_InOutDirection_T direction;
            UI32_T                     policy_map_index;
            UI32_T                     class_map_index;
            RULE_TYPE_CounterRec_T     counter;
        } qos_hw_counter;

        RULE_TYPE_TCAMUtilization_T tcam_entry_old; /* remove tcam_entry_old */
        RULE_TYPE_TCAM_ENTRY_T      tcam_entry;

        RULE_TYPE_MIB_AccessGroupEntry_T access_group_entry;
        RULE_TYPE_MIB_PolicyMapPortEntry_T policy_map_port_entry;

    } data; /* contains the supplemntal data for the corresponding cmd */
} L4_MGR_IPCMsg_T;

#define L4_MGR_MSGBUF_TYPE_SIZE    sizeof(((L4_MGR_IPCMsg_T *)0)->type)

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

 /* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * Routine Name : L4_MGR_InitiateSystemResources
 *------------------------------------------------------------------------------
 * Function : Initialize L4's MGR .
 * Input    : None
 * Output   : None
 * Return   : TRUE/FALSE
 * Note     : None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_InitiateSystemResources(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4 enter the master mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_MGR_EnterMasterMode();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_EnterSlaveMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4 enter the Slave mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_MGR_EnterSlaveMode();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_SetTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  Set transition mode
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_MGR_SetTransitionMode(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_EnterTransitionMode Mode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4 enter the transition mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_MGR_EnterTransitionMode();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_ProcessTimerEvent
 *------------------------------------------------------------------------------
 * PURPOSE: Process timer alarm event from timer
 * INPUT:   None.
 * OUTPUT:  None.
 * RETURN:  None.
 * NOTES:   called by L4_GROUP_Mgr_Thread_Function_Entry
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_ACL_ProcessTimerEvent();

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_HandleHotInsertion
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is inserted at a time.
 *------------------------------------------------------------------------------
 */
void L4_MGR_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is removed at a time.
 *------------------------------------------------------------------------------
 */
void L4_MGR_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port);

/* DiffServ port table */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_BindPort2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to policy map.
 * INPUT:    uport_ifindex, policy_map_name, inout_profile
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     One port only can bind one policy map.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_BindPort2PolicyMap(UI32_T uport_ifindex, const char *policy_map_name, RULE_TYPE_InOutDirection_T inout_profile);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_UnBindPortFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from policy map.
 * INPUT:    uport_ifindex, policy_map_name, inout_profile
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_UnBindPortFromPolicyMap(UI32_T uport_ifindex, const char *policy_map_name, RULE_TYPE_InOutDirection_T inout_profile);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetPolicyMapNameByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get binding policy map's name of port.
 * INPUT:    uport_ifinde
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetPolicyMapNameByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetRunningPolicyMapNameByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running binding policy map's name of port.
 * INPUT:    uport_ifindex
 * OUTPUT:   uport_ifindex, policy_map_name.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_QoS_GetRunningPolicyMapNameByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name
);

/* DiffServ policy-map*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetPolicyMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of policy map by it's index.
 * INPUT:    policy_map_id
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetPolicyMapNameById(UI32_T policy_map_id, char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetPolicyMapIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of policy map by it's name.
 * INPUT:    policy_map_name
 * OUTPUT:   policy_map_id.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetPolicyMapIdByName(const char *policy_map_name, UI32_T *policy_map_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_CreatePolicyMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a policy map entry by it's name
 * INPUT:    policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_CreatePolicyMapByName(const char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_RenamePolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Rename a policy map
 * INPUT:    old_policy_map_name, new_policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_RenamePolicyMap(const char *old_policy_map_name, const char *new_policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetPolicyMapDescByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Set description of a policy map by it's name
 * INPUT:    policy_map
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     Key: policy_map_name.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_SetPolicyMapDescByName(const char *policy_map_name, const char *description);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelPolicyMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a policy map entry by it's name
 * INPUT:    policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelPolicyMapByName(const char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's name
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map_index, policy_map.
 * RETURN:   Error code.
 * NOTE:     Key: policy_map_index.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetNextPolicyMap(UI32_T *policy_map_index, RULE_TYPE_UI_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextRunningPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running policy map entry.
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map_index, policy_map.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_QoS_GetNextRunningPolicyMap(UI32_T *policy_map_index, RULE_TYPE_UI_PolicyMap_T *policy_map);

/* DiffServ policy-map element*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_AddElement2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add a element to policy map
 * INPUT:    policy_map_name, class_map_name, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   RULE_TYPE_OK/RULE_TYPE_FAIL/
 *           RULE_TYPE_METER_FAIL/RULE_TYPE_ACTION_FAIL
 * NOTE:     if meter_entry != NULL, set meter_entry->row_status = L4_MGR_ROWSTATUS_CREATEANDGO
 *           if action_entry != NULL, set action_entry->row_status = L4_MGR_ROWSTATUS_CREATEANDGO
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_AddElement2PolicyMap(const char *policy_map_name, const char *class_map_name, RULE_TYPE_TBParamEntry_T *meter_entry, RULE_TYPE_UI_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelElementMeterFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete meter of element from policy map
 * INPUT:    policy_map_name, class_map_name, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     set meter_entry->row_status = L4_MGR_ROWSTATUS_DESTROY
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelElementMeterFromPolicyMap(const char *policy_map_name, const char *class_map_name, RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelElementActionFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete action of element from policy map
 * INPUT:    policy_map_name, class_map_name, action_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     set action_entry->row_status = L4_MGR_ROWSTATUS_DESTROY
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelElementActionFromPolicyMap(const char *policy_map_name, const char *class_map_name, RULE_TYPE_UI_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a element from policy map
 * INPUT:    policy_map_name, class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelElementFromPolicyMap(const char *policy_map_name, const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next element of policy map
 * INPUT:    policy_map_index, element_entry
 * OUTPUT:   element_entry.
 * RETURN:   Error code.
 * NOTE:     Key: policy_map_index, element_entry->precedent
 *           element_entry->precedent = 0 to get first element
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetNextElementFromPolicyMap(UI32_T policy_map_index, RULE_TYPE_UI_PolicyElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextRunningElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete next running element of policy map
 * INPUT:    policy_map_index, element_entry
 * OUTPUT:   element_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     Key: policy_map_index, element_entry->precedent
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_QoS_GetNextRunningElementFromPolicyMap(UI32_T policy_map_index, RULE_TYPE_UI_PolicyElement_T *element_entry);

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create/update/delete a bundle group
 * INPUT:    policy_map_name    - policy-map name
 *           index              - bundle group index
 *           bundle_group_p     - bundle group
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     Create/update a bundle by
 *             valid_status = TRUE and index is which entry want to create/update.
 *           Delete a bundle by
 *             valid_status = TRUE and index is which entry want to delete.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QoS_SetBundleGroup(
    const char *policy_map_name,
    RULE_TYPE_UI_BundleGroup_T *bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a bundle group
 * INPUT:    policy_map_index         - policy-map name
 *           ui_bundle_group_p->index - bundle index
 *
 * OUTPUT:   ui_bundle_group_p  - bundle group object
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QoS_GetBundleGroup(
    UI32_T policy_map_index,
    RULE_TYPE_UI_BundleGroup_T *bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next bundle group
 * INPUT:    policy_map_index         - policy-map name
 *           ui_bundle_group_p->index - bundle index
 *
 * OUTPUT:   policy_map_index   - policy-map name
 *           ui_bundle_group_p  - bundle group object
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QoS_GetNextBundleGroup(
    UI32_T *policy_map_index_p,
    RULE_TYPE_UI_BundleGroup_T *bundle_group_p
);
#endif /* #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) */

/* DiffServ class-map */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetClassMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of class map by it's index.
 * INPUT:    class_map_index
 * OUTPUT:   class_map_name.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetClassMapNameById(UI32_T class_map_index, char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetClassMapIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of class map by it's name.
 * INPUT:    class_map_index
 * OUTPUT:   class_map_name.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetClassMapIdByName(const char *class_map_name, UI32_T *class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_CreateClassMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a class map
 * INPUT:    class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_CreateClassMapByName(const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_RenameClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Rename a class map
 * INPUT:    old_class_map_name, new_class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_RenameClassMap(const char *old_class_map_name, const char *new_class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a class map
 * INPUT:    class_map
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     Caller need to set:
 *               --- class_map_name
 *               --- desc
 *               --- class_map_match_type
 *               --- row_status             = L4_MGR_ROWSTATUS_CREATEANDGO
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_SetClassMap(RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a class map
 * INPUT:    class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelClassMap(const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetClassMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a class map
 * INPUT:    class_map
 * OUTPUT:   class_map.
 * RETURN:   Error code.
 * NOTE:     Key: class_map->class_map_name
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetClassMapByName(RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code.
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetNextClassMap(UI32_T *class_map_index, RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextRunningClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_QoS_GetNextRunningClassMap(UI32_T *class_map_index, RULE_TYPE_UI_ClassMap_T *class_map);

/* DiffServ class-map element*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_AddElement2ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add a element to class map
 * INPUT:    class_map_name, element_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL / NOT_ALLOW_MIX_SELECTOR)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_AddElement2ClassMap(const char *class_map_name, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_DelElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a element from class map
 * INPUT:    class_map_name, element_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_DelElementFromClassMap(const char *class_map_name, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next element from class map
 * INPUT:    class_map_index, element_index
 * OUTPUT:   element_index, element_entry.
 * RETURN:   Error code.
 * NOTE:     Key:class_map_index, element_index
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetNextElementFromClassMap(UI32_T class_map_index, UI32_T *element_index, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextRunningElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running element from class map
 * INPUT:    class_map_index, element_index
 * OUTPUT:   element_index, element_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     Key:class_map_index, element_index
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_QoS_GetNextRunningElementFromClassMap(UI32_T class_map_index, UI32_T *element_index, RULE_TYPE_UI_ClassMapElement_T *element_entry);

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServPolicyMapHwCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Get diffserv counter
 * INPUT:    ifindex - port index
 *           direction - ingress or egress
 *           policy_id - policy-map id
 *           class_id - class-map id
 * OUTPUT:   counter
 * RETURN:   Error code
 * NOTE: None
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_GetDiffServPolicyMapHwCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id,
    RULE_TYPE_CounterRec_T *counter
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_ClearDiffServPolicyMapHwCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Clear diffserv counter
 * INPUT:    ifindex - port index
 *           direction - ingress or egress
 *           policy_id - policy-map id
 *           class_id - class-map id
 * OUTPUT:   None
 * RETURN:   Error code
 * NOTE: None
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_QoS_ClearDiffServPolicyMapHwCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id
);
#endif

/* ====================================== ACL port table ========================================*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_BindPort2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     ingress_flag = FALSE while egress
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_BindPort2Acl(
    UI32_T uport_ifindex,
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag,
    const UI8_T * time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_UnBindPortFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_UnBindPortFromAcl(UI32_T uport_ifindex, const char *acl_name, RULE_TYPE_AclType_T acl_type, BOOL_T ingress_flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_BindPort2DynamicAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to dynamic ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     1. One port only can bind one dynamic acl.
 *           2. The dynamic acl is got from authentication server.
 *           3. When apply dynamic acl, the user configured aclon this port
 *              should be disabled.
 *           4. If the port already bind a dynamic acl, then the bind
 *              or unbind action by user shall be effected after the dynamic
 *              acl be unbinded.
 *           5. The user acl should be restored to enable when dynamic acl be
 *              unbinded.
 *           6. The changes by this function should not be saved to configuration
 *              file.
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_BindPort2DynamicAcl(UI32_T uport_ifindex, const char *acl_name, RULE_TYPE_AclType_T acl_type, BOOL_T ingress_flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_UnBindPortFromDynamicAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from dynamic ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     1. One port only can bind one dynamic acl.
 *           2. The dynamic acl is got from authentication server.
 *           3. When apply dynamic acl, the user configured aclon this port
 *              should be disabled.
 *           4. If the port already bind a dynamic acl, then the bind
 *              or unbind action by user shall be effected after the dynamic
 *              acl be unbinded.
 *           5. The user acl should be restored to enable when dynamic acl be
 *              unbinded.
 *           6. The changes by this function should not be saved to configuration
 *              file.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_UnBindPortFromDynamicAcl(
    UI32_T uport_ifindex,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetAclByTypeAndDirection
 *------------------------------------------------------------------------------
 * PURPOSE:  get binding ACL by type and direction
 * INPUT:    uport_ifindex, acl_type, ingress_flag
 * OUTPUT:   acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetAclByTypeAndDirection(UI32_T uport_ifindex, RULE_TYPE_UI_AclEntry_T *acl_entry, BOOL_T ingress_flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next binding ACL by port
 * INPUT:    uport_ifindex, acl_index, precedence
 * OUTPUT:   acl_index, acl_entry, precedence.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_GetNextAclByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextUIAclByPortAndBindingType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next same binding type ACL by binding type and port
 * INPUT:    precedence, uport_ifindex
 * OUTPUT:   uport_ifindex, acl_entry, p_time_range_index.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetNextUIAclByPortAndBindingType(UI32_T precedence, UI32_T *uport_ifindex, RULE_TYPE_UI_AclEntry_T *acl_entry, UI8_T *p_time_range_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextRunningAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running binding ACL by port
 * INPUT:    uport_ifindex, acl_entry, precedence
 * OUTPUT:   uport_ifindex, acl_entry, precedence, p_time_range_index.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_ACL_GetNextRunningAclByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable
);

/* ACL table*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetAclNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of ACL by it's index.
 * INPUT:    acl_index
 * OUTPUT:   acl_name.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetAclNameById(UI32_T acl_index, char *acl_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetAclIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name.
 * INPUT:    acl_name
 * OUTPUT:   acl_index.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetAclIdByName(const char *acl_name, UI32_T *acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetAclIdByNameAndType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name and type.
 * INPUT:    acl_name, acl_type
 * OUTPUT:   acl_index.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetAclIdByNameAndType(const char *acl_name, RULE_TYPE_AclType_T acl_type, UI32_T *acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_CreateAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Create an ACL.
 * INPUT:    acl_name, acl_type
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_CreateAcl(const char *acl_name, RULE_TYPE_AclType_T acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_DelAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACL.
 * INPUT:    acl_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_DelAcl(const char *acl_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_DelAclByNameAndAclType
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACL by name and acl_type
 * INPUT:    acl_name, acl_type
 * OUTPUT:   NONE
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_DelAclByNameAndAclType(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL.
 * INPUT:    acl_entry
 * OUTPUT:   acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetNextAcl(UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextRunningAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running ACL.
 * INPUT:    acl_entry
 * OUTPUT:   acl_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_ACL_GetNextRunningAcl(UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry);

/* ACL ACE*/

/*------------------------------------------------------------------------------
 * FUNCTION NAME -L4_MGR_ACL_InitAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for SNMP and WWW and CLI to make a default ACE
 * INPUT    : ace_entry->ace_type
 * OUTPUT   : ace_entry
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it, need to set ace_entry->ace_type
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_InitAce(RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_ACL_InitUIAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for UI to make a default UI ACE entry
 * INPUT    : ace_entry_p->ace_type
 * OUTPUT   : ace_entry_p
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it
 *------------------------------------------------------------------------------*/
RULE_TYPE_RETURN_TYPE_T
L4_MGR_ACL_InitUIAce(
    RULE_TYPE_UI_Ace_Entry_T *ace_entry_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_ACL_SetAcePatternToDefaultByType
 *------------------------------------------------------------------------------
 * PURPOSE  : For UI to initial ace.u.{mac|ip|ipv6|arp} to default
 * INPUT    : ace_type
 * OUTPUT   : ace_entry_p
 * RETURN   : Error code
 * NOTES    : Initial the ace pattern by ace_type
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_ACL_SetAcePatternToDefaultByType(
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_AclType_T ace_type
);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_SetUIAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a UI ACE to ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_SetUIAce2Acl(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_DelUIAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a UI ACE from ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_DelUIAceFromAcl(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next UI ACE from ACL.
 * INPUT:    acl_index, ace_index_p,
 * OUTPUT:   ace_index_p, ace_p
 * RETURN:   Error code.
 * NOTE:     Key: acl_index, ace_index
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_GetNextUIAceByAcl(
    UI32_T acl_index,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextHardwareUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next hardware UI ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_index
 * OUTPUT:   ace_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_GetNextHardwareUIAceByAcl(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T acl_index,
    UI32_T *ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextRunningUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running UI ACE from ACL.
 * INPUT:    acl_index, ace_index_p, ace_entry_p
 * OUTPUT:   ace_entry_p
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_ACL_GetNextRunningUIAceByAcl(
    UI32_T acl_index,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_SetAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a ACE to ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_SetAce2Acl(const char *acl_name, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_DelAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a ACE from ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_DelAceFromAcl(const char *acl_name, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE from ACL.
 * INPUT:    acl_index, ace_index, ace_entry
 * OUTPUT:   ace_entry.
 * RETURN:   Error code.
 * NOTE:     Key: acl_index, ace_index
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_ACL_GetNextAceByAcl(UI32_T acl_index, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextHardwareAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE from ACL.
 * INPUT:    acl_index, ace_index, ace_entry
 * OUTPUT:   ace_entry.
 * RETURN:   Error code.
 * NOTE:     Key: acl_index, ace_index
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_GetNextHardwareAceByAcl(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T acl_index,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextMibAceByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE by port
 * INPUT:    ifindex       - interface index
 *           inout_profile - direction
 *           acl_index     - user config ACL index
 *           ace_index     - hardware ACE index
 * OUTPUT:   ace_entry     - ACE entry
 * RETURN:   Error code.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_GetNextMibAceByPort(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index_p,
    UI32_T *ace_index_p,
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable_p
);

#if (SYS_CPNT_ACL_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_ClearAclCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Clear ACL counter
 * INPUT:    acl_name   - ACL name
 *                        If this parameter is zero string, clear counter for all
 *                        ACLs.
 *                        If this parameter is zero string, ignores ace_index.
 *           ace_index  - ACE index.
 *                        If this parameter is 0xffffffff, clear counter for all
 *                        ACEs of the specified ACL.
 *           interface  - The interface.
 *                        This parameter can be one of the following values,
 *                        RULE_TYPE_INTERFACE_LPORT     - One port
 *                        RULE_TYPE_INTERFACE_UPORT     - One port
 *                        RULE_TYPE_INTERFACE_ALL_UNIT  - All port
 * OUTPUT:   none
 * RETURN:   Error code.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_ClearAclCounter(
    const char *acl_name,
    UI32_T ace_index,
    RULE_TYPE_INTERFACE_INFO_T *interface
);

#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextRunningAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running ACE from ACL.
 * INPUT:    acl_index, ace_index, ace_entry
 * OUTPUT:   ace_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_ACL_GetNextRunningAceByAcl(UI32_T acl_index, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_AddAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Add ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port
 *           acl_name       - Source ACL name
 * OUTPUT:   NONE
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_AddAclMirrorEntry(
    UI32_T ifindex_dest,
    const char *acl_name
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_RemoveAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Remove ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port
 *           acl_name       - Source ACL name
 * OUTPUT:   NONE
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_RemoveAclMirrorEntry(
    UI32_T ifindex_dest,
    const char *acl_name
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  GetNext ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port. Pass 0 to start.
 *           acl_index      - ACL index
 * OUTPUT:   acl_entry      - ACL entry
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_ACL_GetNextAclMirrorEntry(
    UI32_T *ifindex_dest,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ACL_GetNextRunningAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  GetNext ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port. Pass 0 to start.
 *           acl_index      - ACL index
 * OUTPUT:   acl_entry      - ACL entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_ACL_GetNextRunningAclMirrorEntry(
    UI32_T *ifindex_dest,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_GetTCAMUtilization
 *------------------------------------------------------------------------------
 * PURPOSE :  get total pce, free_pce counts, and utilization
 * INPUT   :  none
 * OUTPUT  :  tcam_util_p
 * RETURN  :  True/False
 * NOTE    :
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_GetTCAMUtilization(RULE_TYPE_TCAMUtilization_T *tcam_util_p);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_GetTcamEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get tcam entry
 * INPUT    : tcam_entry_p->unit
 *            tcam_entry_p->device_id
 *            tcam_entry_p->pool_id
 * OUTPUT   : tcam_util_p
 * RETURN   : TRUE/FALSE.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_GetTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_GetNextTcamEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get next tcam entry
 * INPUT    : tcam_entry_p->unit
 *            tcam_entry_p->device_id
 *            tcam_entry_p->pool_id
 * OUTPUT   : tcam_util_p
 * RETURN   : TRUE/FALSE.
 * NOTE     : For get first entry,
 *            use index with tcam_entry_p->unit = 0
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_GetNextTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_GetSupportedTcamCapabilityBitMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets TCAM all supporting capability
 * INPUT    :
 * OUTPUT   : bit_map_p
 * RETURN   : Error code.
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_GetSupportedTcamCapabilityBitMap(
    RULE_TYPE_TCAM_CAP_BITMAP_T *bit_map_p
);

/* ========================== For CoS ===============================================*/
#if (SYS_CPNT_COS == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_AddFirstTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_AddFirstTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - LL4_MGR_COS_AddTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_AddTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelLastTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelLastTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_CosLportConfigAsic
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the cos function that will be registed to priority manager
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_CosLportConfigAsic(UI32_T l_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TOS/DSCP/COS mapping of system
 * INPUT:    flag -- COS_VM_ipprecedence_MAPPING_ENABLED/COS_VM_DSCP_MAPPING_ENABLED
 *                   /COS_VM_DISABLE_BOTH
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:  TOS/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetIpPrecedenceDscpMappingStatus (COS_VM_MAPPING_FLAG_T flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get TOS/DSCP/COS mapping status of system
 * INPUT:    none
 * OUTPUT:   flag -- COS_MGR_ipprecedence_MAPPING_ENABLED/COS_MGR_DSCP_MAPPING_ENABLED
 *                   /COS_MGR_DISABLE_BOTH
 * RETURN:   TRUE/FALSE
 * NOTE:  tos/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetIpPrecedenceDscpMappingStatus (COS_VM_MAPPING_FLAG_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetRunningIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of tos/DSCP/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: default value is COS_MGR_MAPPING_DISABLED
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningIpPrecedenceDscpMappingStatus(COS_VM_MAPPING_FLAG_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TcpPort/COS mapping of system
 * INPUT:    flag -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetTcpPortMappingStatus (UI32_T flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetTcpPortMappingStatus(UI32_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetRunningTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: default value is COS_MGR_MAPPING_DISABLED
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningTcpPortMappingStatus(COS_VM_TCPPORT_MAPPING_FLAG_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelIpPrecedenceToCos(UI32_T l_port, UI8_T tos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelIpPrecedenceLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelIpPrecedenceLportAllEntry(UI32_T l_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_SetDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: this function will set a dscp entry's status
 * INPUT:   l_port
 *          dscp, cos status
 * OUTPUT:
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function will get a dscp entry's status
 * INPUT:   l_port
 *          dscp
 * OUTPUT:  cos status
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T *status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetRunningDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function will get a running dscp entry's status
 * INPUT:    l_port, dscp
 * OUTPUT:   status -- COS_MGR_MAPPING_ENABLED/COS_MGR_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T *status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelDSCPToCos(UI32_T l_port, UI8_T dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelDscpLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelDscpLportAllEntry(UI32_T l_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a TCP Port/CoS map in tcp_port_map_table
 *           of specific logical port
 * INPUT:    l_port, tcp port (1..65535), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. For GetNext functions, the entries in table need to be sorted
 *          according to tcp_port number
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tcp_port/CoS map in tcp_port_map_table of specific
 *           logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port of specific logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelPortToCos(UI32_T l_port, UI16_T tcp_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelPortLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelPortLportAllEntry(UI32_T l_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetNextIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in tos_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           tos -- tos index
 *           cos -- cos value
 * OUTPUT:   l_port, tos -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetNextIpPrecedenceToCos(UI32_T *l_port, UI8_T *tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetNextDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in dscp_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           dscp -- dscp index
 *           cos -- cos value
 * OUTPUT:   l_port, dscp -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetNextDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetNextPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in tcp_port_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           tcp_port -- dscp index
 *           cos -- cos value
 * OUTPUT:   l_port, tcp_port -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetNextPortToCos(UI32_T *l_port, I32_T *tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetRunningIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default tos/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tos
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetRunningDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default DSCP/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tos
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetRunningPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default TCPPort/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tos
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetNextRunningIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          tos
 * OUTPUT:  l_port
 *          tos
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetNextRunningIpPrecedenceToCos(UI32_T *l_port , UI8_T *tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetNextRunningDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          dscp
 * OUTPUT:  l_port
 *          dscp
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetNextRunningDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_COS_GetNextRunningPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          tcp_port
 * OUTPUT:  l_port
 *          tcp_port
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetNextRunningPortToCos(UI32_T *l_port, I32_T *tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_PortCopy
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will copy the source port set to destport
 * INPUT:    map_table_type, src_port, dst_port_list
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE: Since it have to support multi-thread, so it is not possible to separate
 *       parameters configuration from copy operation
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_PortCopy(UI32_T map_table_type, UI32_T src_port, UI8_T *dst_port_list);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_AddCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is cos_entry.acl_name
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_AddCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_DelCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is cos_entry.acl_name
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_DelCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetCosEntryRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, cos_entry
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is (ifindex, cos_entry.acl_name)
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetCosEntryRowStatus(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, cos_entry
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is (ifindex, cos_entry.acl_name)
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL to cos mapping
 * INPUT:    ifindex,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is (ifindex, acl_mapping->acl_name)
 *           supposely only the SNMP will call this fuction
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for CLI and WWW to get the next ACL to cos
 * INPUT:    ifindex,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use the *ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (*ifindex, acl_mapping->acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetNextCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SNMPGetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to get the next acl map to cos
 * INPUT:    ifindex
 * OUTPUT:   ifindex, acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use the *ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (*ifindex, acl_mapping->acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SNMPGetNextCosEntry(UI32_T *ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  -L4_MGR_COS_GetNextRunningCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for CLI to get the port acl binding
 * INPUT  :  if_index, entry
 * OUTPUT :  entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES  :  use the ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (ifindex, acl_mapping->acl_name)
 *
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetNextRunningCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry );

/*
 obsolete part
*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetIpAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. if strlen(acl_name) == 0 , means remove ACL binding, don't care cos parameter.
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetIpAclToCos(UI32_T l_port, UI8_T *acl_name, UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_SetMacAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. if strlen(acl_name) == 0 , means remove ACL binding, don't care cos parameter.
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_SetMacAclToCos(UI32_T l_port, UI8_T *acl_name, UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetNextAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a ACL for COS mapping
 *           of specific logical port
 * INPUT:
 * OUTPUT:   l_port, acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use *lport= 0 to get the first entry
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetNextAclToCos(UI32_T *l_port, COS_AclCosMapping_T *acl_mapping);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_COS_GetAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_COS_GetAclToCos(UI32_T l_port, COS_AclCosMapping_T *acl_mapping);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  L4_MGR_COS_GetRunningIntfAclCosMapping
 *------------------------------------------------------------------------------
 * PURPOSE:  for CLI to get the port acl binding
 * INPUT  :  if_index, entry
 * OUTPUT :  entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES  :  if entry->in_ip_acl ='\0' means disable, same as the other variable
 *
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_COS_GetRunningIntfAclCosMapping(UI32_T if_index, COS_AclCosMapping_T *acl_mapping );

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_COS_HandleHotInsertion
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is inserted at a time.
 *------------------------------------------------------------------------------
 */
void L4_MGR_COS_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_COS_HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is removed at a time.
 *------------------------------------------------------------------------------
 */
void L4_MGR_COS_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_EnablePortPriorityMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function uses to enable priority's config of COS
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   NONE
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_EnablePortPriorityMode(
    UI32_T l_port,
    UI16_T priority
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_DisablePortPriorityMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function uses to disable priority's config of COS
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   NONE
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_DisablePortPriorityMode(
    UI32_T l_port,
    UI16_T priority
);

BOOL_T L4_MGR_QOS_SetTrustMode(UI32_T lport, UI32_T mode);
BOOL_T L4_MGR_QOS_GetTrustMode(UI32_T lport, UI32_T *mode);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortPriorityTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the COS trust mode
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           mode     - trust mode, qos_if_trust_mode_t
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortPriorityTrustMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPriorityTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the COS trust mode
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: SYS_ADPT_COS_PRIORITY_XX
 * OUTPUT:   mode     - trust mode, qos_if_trust_mode_t
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortPriorityTrustMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T *mode
);

//:COS_TO_DSCP

BOOL_T L4_MGR_QOS_SetIngressCos2Dscp(UI32_T lport,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color);
BOOL_T L4_MGR_QOS_GetIngressCos2Dscp(UI32_T lport,UI32_T cos,UI32_T cfi,UI32_T *phb,UI32_T *color);
BOOL_T L4_MGR_QOS_GetNextIngressCos2Dscp(UI32_T *lport,UI32_T *cos,UI32_T *cfi,UI32_T *phb,UI32_T *color);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the global mapping of ingress cos to phb and
 *            color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetCos2InternalDscp(
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the global mapping of ingress cos to phb and
 *            color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : phb_p    - phb value
 *            color_p  - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetCos2InternalDscp(
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next the global mapping of ingress cos to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue.
 *            cfi      - cfi value.
 * OUTPUT   : cos      -
 *            cfi      -
 *            phb      - phb value.
 *            color    - color vlaue.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using cos = COS_TYPE_UNKNOWN_COS, cfi = COS_TYPE_UNKNOWN_CFI
 *            to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextCos2InternalDscp(
    UI32_T priority,
    UI32_T *cos_p,
    UI32_T *cfi_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the active mapping of ingress cos to phb and
 *            color.
 * INPUT    : cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveCos2InternalDscp(
    UI32_T cos,
    UI32_T cfi,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of cos and cfi which is
 *            mapping to the input phb and color.
 * INPUT    : phb   - phb vlaue.
 *            color - color value.
 * OUTPUT   : cos_p - cos value mapping to the input (phb, color).
 *            cfi_p - cfi vlaue mapping to the input (phb, color).
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using cos = COS_TYPE_UNKNOWN_COS, cfi = COS_TYPE_UNKNOWN_CFI
 *            to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningCos2InternalDscp(
    UI32_T phb,
    UI32_T color,
    UI32_T *cos_p,
    UI32_T *cfi_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the per-port mapping of ingress cos to phb and
 *            color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the per-port mapping of ingress cos to phb and
 *            color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next the per-port mapping of ingress cos to
 *            phb and color.
 * INPUT    : l_port_p - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos_p    - cos vlaue
 *            cfi_p    - cfi value
 * OUTPUT   : l_port_p -
 *            cos_p    -
 *            cfi_p    -
 *            phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using l_port = 0, cos = COS_TYPE_UNKNOWN_COS,
 *            cfi = COS_TYPE_UNKNOWN_CFI to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextPortCos2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *cos_p,
    UI32_T *cfi_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the per-port actived mapping of ingress cos to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveCos2InternalDscp(
    UI32_T l_port,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of ingress cos to
 *            internal dscp
 * INPUT    : l_port   - Logical port id.
 *            phb   - phb vlaue.
 *            color - color value.
 *            cos_p - The start cos to get next.
 *            cfi_p - The start cfi to get next.
 * OUTPUT   : cos_p - cos value mapping to the input (phb, color).
 *            cfi_p - cfi vlaue mapping to the input (phb, color).
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using cos = COS_TYPE_UNKNOWN_COS, cfi = COS_TYPE_UNKNOWN_CFI
 *            to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T phb,
    UI32_T color,
    UI32_T *cos_p,
    UI32_T *cfi_p
);



//:PRE_2_DSCP
BOOL_T L4_MGR_QOS_SetIngressPre2Dscp(UI32_T lport,UI32_T pre,UI32_T phb,UI32_T color);
BOOL_T L4_MGR_QOS_ResetIngressPre2Dscp(UI32_T lport);
BOOL_T L4_MGR_QOS_GetIngressPre2Dscp(UI32_T lport,UI8_T *phb,UI8_T *color);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            pre      - IP precednece vlaue
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   :
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T pre,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            pre      - IP precednece vlaue
 * OUTPUT   : phb   - phb value
 *            color - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T pre,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next the mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port_p - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            pre      - IP precednece vlaue
 *            phb_p    - phb value
 *            color_p  - color vlaue
 * OUTPUT   : l_port_p -
 *            phb_p    -
 *            color_p  -
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using l_port = 0, pre = COS_TYPE_UNKNOWN_PRE to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextPortPre2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *pre_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActivePre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the actived mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            pre      - IP precednece vlaue
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb_p      - phb value
 *            color_p    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActivePre2InternalDscp(
    UI32_T l_port,
    UI32_T pre,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetRunningPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets running config of IP precedence to internal DSCP
 *          mapping table.
 * INPUT:   l_port - Logical port id.
 * OUTPUT:  phb_ar[MAX_PRE_VAL+1]   - Array for the mapping of pre to phb.
 *          color_ar[MAX_PRE_VAL+1] - Array for the mapping of pre to color.
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetRunningPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T *phb_ar,
    UI32_T *color_ar
);


//:DSCP_2_DSCP

BOOL_T L4_MGR_QOS_SetIngressDscp2Dscp(UI32_T lport,UI32_T o_dscp,UI32_T phb,UI32_T color);
BOOL_T L4_MGR_QOS_GetIngressDscp2Dscp(UI32_T lport,UI32_T dscp,UI32_T *phb,UI32_T *color);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetDscp2InternalDscp(
    UI32_T priority,
    UI32_T dscp,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 * OUTPUT   : phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetDscp2InternalDscp(
    UI32_T priority,
    UI32_T dscp,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp_p   - dscp value
 * OUTPUT   : dscp_p   -
 *            phb_p    - phb value
 *            color_p  - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using dscp_p = COS_TYPE_UNKNOWN_DSCP to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextDscp2InternalDscp(
    UI32_T priority,
    UI32_T *dscp_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the actived global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : dscp       - dscp value
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb_p      - phb value
 *            color_p    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveDscp2InternalDscp(
    UI32_T dscp,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of ingress dscp which is
 *            mapping to input phb and color.
 * INPUT    : phb    - phb vlaue.
 *            color  - color value.
 *            dscp_p - dscp value to get next mapping.
 * OUTPUT   : dscp_p - dscp value mapping to the input phb and color.
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using dscp = COS_TYPE_UNKNOWN_DSCP to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningDscp2InternalDscp(
    UI32_T phb,
    UI32_T color,
    UI32_T *dscp_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetPortDscp2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T dscp,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 * OUTPUT   : phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortDscp2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T dscp,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp_p   - dscp value
 * OUTPUT   : l_port  -
 *            dscp_p  -
 *            phb_p   - phb value
 *            color_p - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using l_port = 0, dscp_p = COS_TYPE_UNKNOWN_DSCP to get first
 *            entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextPortDscp2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *dscp_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets actived per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            dscp     - dscp value
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_X, the actived priority.
 *            phb_p      - phb value
 *            color_p    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveDscp2InternalDscp(
    UI32_T l_port,
    UI32_T dscp,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of ingress dscp to
 *            internal dscp
 * INPUT    : l_port - Logical port id.
 *            phb    - phb vlaue.
 *            color  - color value.
 *            dscp_p - The dscp value using for get next.
 * OUTPUT   : dscp_p - dscp value mapping to the input phb and color.
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using l_port = 0, dscp = COS_TYPE_UNKNOWN_DSCP to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningPortDscp2InternalDscp(
    UI32_T l_port,
    UI32_T phb,
    UI32_T color,
    UI32_T *dscp_p
);


//:PHB_TO_QUEUE

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the global mapping of ingress DSCP to Queue
 * INPUT    : phb
 *            queue - if queue == 8, set the phb mapping to default queue.
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : This API only for user config
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetIngressDscp2Queue(
    UI32_T phb,
    UI32_T queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the mapping of ingress DSCP to Queue by port
 * INPUT    : l_port
 *            phb
 *            queue - if queue == 8, set the phb mapping to default queue.
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : This API only for user config
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortIngressDscp2Queue(
    UI32_T lport,
    UI32_T phb,
    UI32_T queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Get the global mapping of ingress DSCP to Queue
 * INPUT    : None
 * OUTPUT   : queue[MAX_PHB_VAL + 1]
 * RETURN   : TRUE/FALSE
 * NOTE     : This API gets the active config (which is running on switch)
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetIngressDscp2Queue(
    UI8_T *queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Get the mapping of ingress DSCP to Queue by port
 * INPUT    : l_port
 * OUTPUT   : queue[MAX_PHB_VAL + 1]
 * RETURN   : TRUE/FALSE
 * NOTE     : This API gets the active config (which is running on switch)
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortIngressDscp2Queue(
    UI32_T l_port,
    UI8_T *queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortRunningIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Get the user config of ingress DSCP to Queue for running-config
 *            by port
 * INPUT    : l_port - logic port
 * OUTPUT   : queue[MAX_PHB_VAL + 1]
 * RETURN   : TRUE/FALSE
 * NOTE     : This API gets the user config
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortRunningIngressDscp2Queue(
    UI32_T lport,
    UI8_T *queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPriorityIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue for all Dscp
 * INPUT:   priority - the priority of COS config, ex: SYS_ADPT_COS_PRIORITY_XX
 * OUTPUT:  queue    - mapping table of dscp to queue , queue[MAX_PHB_VAL+1]
 * RETURN:  TRUE / FALSE
 * NOTE:    Return config value by input priority
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPriorityIngressDscp2Queue(
    UI16_T priority,
    UI8_T *queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPriorityIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue for all Dscp by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: SYS_ADPT_COS_PRIORITY_XX
 * OUTPUT:  queue    - mapping table of dscp to queue , queue[MAX_PHB_VAL+1]
 * RETURN:  TRUE / FALSE
 * NOTE:    Return the config of the input priority.
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortPriorityIngressDscp2Queue(
    UI32_T l_port,
    UI16_T priority,
    UI8_T *queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the global mapping of PHB to queue
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 *            queue    - Queue id.
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetIngressPhb2Queue(
    UI32_T priority,
    UI32_T phb,
    UI32_T queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets the global mapping of PHB to queue
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 * OUTPUT   : queue_p  - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetIngressPhb2Queue(
    UI32_T priority,
    UI32_T phb,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets actived global mapping of PHB to queue
 * INPUT    : phb        - PHB value.
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            queue_p    - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveIngressPhb2Queue(
    UI32_T phb,
    UI32_T *priority_p,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of PHB to queue.
 * INPUT    : queue_p - Queue id.
 *            phb_p   - PHB value.
 * OUTPUT   : phb_p -
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using phb = COS_TYPE_UNKNOWN_PHB to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningIngressPhb2Queue(
    UI32_T queue,
    UI32_T *phb_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the per-port mapping of PHB to queue.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 *            queue    - Queue id.
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_SetPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T priority,
    UI32_T phb,
    UI32_T queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets the per-port mapping of PHB to queue
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 * OUTPUT   : queue_p  - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T priority,
    UI32_T phb,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets the next per-port mapping of PHB to queue
 * INPUT    : l_port_p - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb_p    - PHB value.
 * OUTPUT   : queue_p  - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using ifindex = 0, phb = COS_TYPE_UNKNOWN_PHB to get first entry
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetNextPortIngressPhb2Queue(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *phb_p,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets actived global mapping of PHB to queue
 * INPUT    : l_port     - Logical port id.
 *            phb        - PHB value.
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            queue_p    - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveIngressPhb2Queue(
    UI32_T l_port,
    UI32_T phb,
    UI32_T *priority_p,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextRunningPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of PHB to queue.
 * INPUT    : l_port     - Logical port id.
 *            queue_p - Queue id.
 *            phb_p   - PHB value.
 * OUTPUT   : phb_p -
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using phb = COS_TYPE_UNKNOWN_PHB to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetNextRunningPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *phb_p
);

//:DSCP_TO_COLOR
BOOL_T L4_MGR_QOS_SetIngressDscp2Color(UI32_T lport,UI32_T phb,UI32_T color);
BOOL_T L4_MGR_QOS_GetIngressDscp2Color(UI32_T lport,UI8_T *color);
BOOL_T L4_MGR_QOS_GetRunningIngressDscp2Color(UI32_T lport,UI8_T *color);

//:DSCP_TO_COS
BOOL_T L4_MGR_QOS_SetIngressDscp2Cos(UI32_T lport,UI32_T phb,UI32_T color,UI32_T cos,UI32_T cfi);
BOOL_T L4_MGR_QOS_GetIngressDscp2Cos(UI32_T lport,UI32_T phb,UI32_T color,UI32_T *cos,UI32_T *cfi);
BOOL_T L4_MGR_QOS_GetNextIngressDscp2Cos(UI32_T *lport,UI32_T *phb,UI32_T *color,UI32_T *cos,UI32_T *cfi);
BOOL_T L4_MGR_QOS_GetRunningIngressDscp2Cos(UI32_T lport,UI8_T cos,UI8_T cfi,UI8_T *l_phb,UI8_T *l_color,UI8_T *cos_phb,UI8_T *cos_color);

//:IP_PORT_DSCP
BOOL_T L4_MGR_QOS_SetTCPUDP2Dscp(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T phb,UI32_T color,UI32_T add);
BOOL_T L4_MGR_QOS_GetTCPUDP2Dscp(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T *phb,UI32_T *color);
BOOL_T L4_MGR_QOS_GetNextTCPUDP2Dscp(UI32_T *ifindex,UI32_T *protocol,UI32_T *dport,UI32_T *phb,UI32_T *color);
BOOL_T L4_MGR_QOS_GetRunningTCPUDP2Dscp(UI32_T ifindex,UI32_T *protocol,UI32_T *dport,UI32_T *phb,UI32_T *color);

//:QUEUE_MODE
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the queue mode
 * INPUT:    l_port   - logic port
 *           mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     This function will set the config on user priority
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortEgressQueueMode(
    UI32_T l_port,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the queue mode
 * INPUT:    l_port   - logic port
 * OUTPUT:   mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortEgressQueueMode(
    UI32_T l_port,
    UI32_T *mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the next queue mode by port
 * INPUT:    ifindex_p   -
 *
 * OUTPUT:   ifindex_p -
 *           mode      - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetNextPortEgressQueueMode(
    UI32_T *ifindex_p,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the queue mode
 * INPUT:    priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPriorityEgressQueueMode(
    UI16_T priority,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the queue mode
 * INPUT:    priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPriorityEgressQueueMode(
    UI16_T priority,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets actived queue mode
 * INPUT    :
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            mode     - COS_VM_SCHEDULING_METHOD_E, scheduling mode,
 * RETURN   : COS_TYPE_ERROR_CODE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveEgressQueueMode(
    UI32_T *priority_p,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the queue mode
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortPriorityEgressQueueMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the queue mode
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortPriorityEgressQueueMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T *mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets per-port actived queue mode
 * INPUT    : l_port   - Logical port id.
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            mode     - COS_VM_SCHEDULING_METHOD_E, scheduling mode,
 * RETURN   : COS_TYPE_ERROR_CODE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveEgressQueueMode(
    UI32_T l_port,
    UI32_T *priority_p,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_ResetPortPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function resets the queue mode to default value
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_ResetPortPriorityEgressQueueMode(
    UI32_T l_port,
    UI16_T priority
);

//:QUEUE_WEIGHT
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the wrr queue weight
 * INPUT:    l_port   - logic port
 *           q_id     - queue id
 *           weight   - weight
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortEgressWrrQueueWeight(
    UI32_T l_port,
    UI32_T q_id,
    UI32_T weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the wrr queue weight
 * INPUT:    l_port   - logic port
 *           q_id     - queue id
 * OUTPUT:   weight   - weight
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortEgressWrrQueueWeight(
    UI32_T l_port,
    UI32_T q_id,
    UI32_T *weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight
 * INPUT:   queue_p     -
 *
 * OUTPUT:  queue_p     -
 *          weight_p    - queue weight
 * RETURN:  TRUE / FALSE
 * NOTE:    Input queue (COS_TYPE_UNKNOWN_QUEUE) to get first queue weight,
 *
 *          In hybrid mode, this function still gets the configured wrr weight
 *          for strict-queue
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetNextEgressQueueWeight(
    UI32_T *queue_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetNextPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight and queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex_p   -
 *          queue_p     -
 *
 * OUTPUT:  ifindex_p   -
 *          queue_p     -
 *          weight      - queue weight
 * RETURN:  TRUE / FALSE
 * NOTE:    Input ifindex, queue (0, COS_TYPE_UNKNOWN_QUEUE)
 *          to get first queue weight.
 *
 *          In hybrid mode, this function still gets the configured wrr weight
 *          for strict-queue
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetNextPortEgressQueueWeight(
    UI32_T *ifindex_p,
    UI32_T *queue_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the wrr queue weight
 * INPUT:    priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           q_id     - queue id
 *           weight   - weight
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPriorityEgressWrrQueueWeight(
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the wrr queue weight
 * INPUT:    priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           q_id     - queue id
 * OUTPUT:   weight   - weighte
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPriorityEgressWrrQueueWeight(
    UI16_T priority,
    UI32_T q_id,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived queue weight.
 * INPUT:   queue      - queue
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX, the actived priority of config.
 *          weight_p   - weight.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveQueueWeight(
    UI32_T queue,
    UI32_T *priority_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetRunningEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the config of wrr queue weight
 * INPUT:   queue     - queue id
 * OUTPUT:  weight_p    - queue weight
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetRunningEgressQueueWeight(
    UI32_T queue,
    UI32_T *weight_p
);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the wrr queue weight
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           q_id     - queue id
 *           weight   - weight
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortPriorityEgressWrrQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the wrr queue weight
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           q_id     - queue id
 * OUTPUT:   weight   - weighte
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortPriorityEgressWrrQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id,
    UI32_T *weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the wrr queue weight
 * INPUT    : l_port     - logic port
 *            queue      - queue id
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            weight     - weight
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveQueueWeight(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *priority_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_ResetPortPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE:  This function resets the wrr queue weight to default value
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           q_id     - queue id
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_ResetPortPriorityEgressWrrQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetRunningPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the config of wrr queue weight by port
 * INPUT:   ifindex   - logic port
 *          queue     - queue id
 * OUTPUT:  weight_p    - queue weight
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_MGR_QOS_GetRunningPortEgressQueueWeight(
    UI32_T ifindex,
    UI32_T queue,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function only used for hybrid queue mode (ex: strict+wrr)
 *          to set the queue to be strict queue or wrr queue
 * INPUT:   priority - The priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue    - Queue id
 *          status -
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPriorityEgressQueueStrictStatus(
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_SetPortPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function only used for hybrid queue mode (ex: strict+wrr)
 *          to set the queue to be strict queue or wrr queue
 * INPUT:   ifindex  -
 *          priority - The priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue    - Queue id
 *          status   -
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_SetPortPriorityEgressQueueStrictStatus(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue status(strict/wrr)
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue
 * OUTPUT:  status_p -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPriorityEgressQueueStrictStatus(
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetActiveQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived queue status(strict/wrr)
 * INPUT:   queue      - queue
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *          status_p   - COS_VM_QUEUE_STRICT_STATUS_T.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetActiveQueueStrictStatus(
    UI32_T queue,
    UI32_T *priority_p,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex   - logic port
 *          priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue id
 * OUTPUT:  status_p -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QOS_GetPortPriorityEgressQueueStrictStatus(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QOS_GetPortActiveQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived queue status(strict/wrr) by port
 * INPUT:   l_port     - Logic port id.
 *          queue      - Queue id.
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *          status_p   - COS_VM_QUEUE_STRICT_STATUS_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QOS_GetPortActiveQueueStrictStatus(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *priority_p,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

#endif /*#if (SYS_CPNT_COS == TRUE)*/

/* For Marker */
#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_AddMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to bind a ACL to the interface that perform remark fuction
 * INPUT:    ifindex      --
 *           marker_entry --
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     Note MARK_TYPE_REMARK_DSCP and MARK_TYPE_REMARK_TOS_PRECEDENCE  can't coexist
 *           key is (ifindex, marker_entry.acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_AddMarkerEntry(UI32_T ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_DelMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  bind the ACL to interface that performed remark function
 * INPUT:    intf     --
 *           acl_name
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     search key is (ifindex , acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_DelMarkerEntry(UI32_T ifindex, UI8_T acl_name[], UI32_T acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_GetMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to get the marker entry
 * INPUT:    ifindex     --
 *           marker_entry.acl_name
 * OUTPUT:   marker_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     this API suppose only used by SNMP,
 *           L4_MGR_MARKER_GetNextMarkerEntry is enough for CLI and WWW
 *           the search key is (intf, marker_entry->acl_name)
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_GetMarkerEntry(UI32_T ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_GetNextMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL binding to the inteface performaned remark function
 * INPUT:    ifindex     -- logical port
 *           marker_entry.acl_name
 * OUTPUT:   ifindex
 *           marker_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (*ifindex, mark_entry->acl_name)
 *           use the *ifindex=0, marker_entry->acl_name='\0' to get the frist entry
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_GetNextMarkerEntry(UI32_T ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SNMPGetNextMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL binding to the inteface performaned remark function
 * INPUT:    ifindex     -- logical port
 *           marker_entry.acl_name
 * OUTPUT:   ifindex
 *           marker_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (*ifindex, mark_entry->acl_name)
 *           use the *ifindex=0, marker_entry->acl_name='\0' to get the frist entry
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SNMPGetNextMarkerEntry(UI32_T *ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_GetNextRunningMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get marker entry
 * INPUT:    ifindex     --
 *           marker_entry.acl_name
 * OUTPUT:   marker_entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_MGR_MARKER_GetNextRunningMarkerEntry(UI32_T ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SetMarkEntryRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    (ifindex, acl_name) is the key
 *           row_status
 * OUTPUT :  None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SetMarkerEntryRowStatus(UI32_T ifindex, MARKER_TYPE_MarkerEntry_T *marker_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_GetMarkerEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to get the marker entry
 * INPUT:
 * OUTPUT:   marker_entry
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_GetDefaultMarkerEntry(MARKER_TYPE_MarkerEntry_T *marker_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SetMarkerEntry_Action
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           marker_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *           SNMP do not need to care of marker_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SetMarkerEntry_Action(UI32_T ifindex, UI8_T acl_name[], UI32_T action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SetMarkerEntry_Dscp
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           marker_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *           SNMP do not need to care of marker_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SetMarkerEntry_Dscp(UI32_T ifindex, UI8_T acl_name[], UI32_T value);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SetMarkerEntry_Precedence
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           marker_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *           SNMP do not need to care of marker_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SetMarkerEntry_Precedence(UI32_T ifindex, UI8_T acl_name[], UI32_T value);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MARKER_SetMarkerEntry_Priority
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           marker_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *           SNMP do not need to care of marker_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_MARKER_SetMarkerEntry_Priority(UI32_T ifindex, UI8_T acl_name[], UI32_T value);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_MARKER_InsertModulePort
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is inserted at a time.
 *------------------------------------------------------------------------------
 */
void L4_MGR_MARKER_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_MARKER_RemoveModulePort
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is removed at a time.
 *------------------------------------------------------------------------------
 */
void L4_MGR_MARKER_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port);
#endif /*#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)*/

/* For CPU interface */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_TrapPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  trap packet to CPU or not
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_TrapPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_MirrorPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  copy packet to CPU or not
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_MirrorPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info);

/* For MIB */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get port entry by ifindex
 * INPUT    : ifindex
 *            inout_profile - direction
 * OUTPUT   : port_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServPortEntry(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_UI_PortEntry_T *port_entry
    );

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next port entry by ifindex
 * INPUT    : ifindex
 * OUTPUT   : ifindex, inout_profile, port_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServPortEntry(
    UI32_T *ifindex_p,
    RULE_TYPE_InOutDirection_T *inout_profile_p,
    RULE_TYPE_UI_PortEntry_T *port_entry_p
    );

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPortPolicyMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to policy-map(policy_map_index)
 * INPUT    : ifindex, inout_profile, policy_map_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPortPolicyMapIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index
    );

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPortIngressIpAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to ingress IP ACL(acl_index)
 * INPUT    : ifindex, acl_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPortIngressIpAclIndex(UI32_T ifindex, UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPortIngressIpv6AclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to ingress IPv6 ACL(acl_index)
 * INPUT    : ifindex, acl_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPortIngressIpv6AclIndex(UI32_T ifindex, UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPortIngressMacAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to ingress MAC ACL(acl_index)
 * INPUT    : ifindex, acl_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPortIngressMacAclIndex(UI32_T ifindex, UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServPolicyMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map by policy_map_index
 * INPUT    : policy_map_index
 * OUTPUT   : policy_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServPolicyMapEntry(UI32_T policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServPolicyMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next policy-map by policy_map_index
 * INPUT    : policy_map_index
 * OUTPUT   : policy_map_index, policy_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServPolicyMapEntry(UI32_T *policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapName
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map name by policy_map_index
 * INPUT    : policy_map_index, policy_map_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapName(UI32_T policy_map_index, const char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapDescription
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map description by policy_map_index
 * INPUT    : policy_map_index, policy_map_desc
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapDescription(UI32_T policy_map_index, const char *policy_map_desc);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map row status by policy_map_index
 * INPUT    : policy_map_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapStatus(UI32_T policy_map_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetPolicydiffServPolicyMapAttachCtlAction
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate policy-map and policy-map element
 * INPUT    : policy_map_index, policy_element_index, attach_action
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetPolicydiffServPolicyMapAttachCtlAction(UI32_T policy_map_index, UI32_T policy_element_index, UI32_T attach_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServPolicyMapElementEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map element by policy_element_index
 * INPUT    : policy_element_index
 * OUTPUT   : policy_element
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServPolicyMapElementEntry(UI32_T policy_element_index, RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServPolicyMapElementEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next policy-map element by policy_element_index
 * INPUT    : policy_element_index
 * OUTPUT   : policy_element_index, policy_element
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServPolicyMapElementEntry(UI32_T *policy_element_index, RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapElementClassMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map index by policy_element_index
 * INPUT    : policy_element_index, class_map_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapElementClassMapIndex(UI32_T policy_element_index, UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter index by policy_element_index
 * INPUT    : policy_element_index, meter_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapElementMeterIndex(UI32_T policy_element_index, UI32_T meter_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapElementActionIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : change action index by policy_element_index
 * INPUT    : policy_element_index, action_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapElementActionIndex(UI32_T policy_element_index, UI32_T action_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapElementStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map element row status by policy_element_index
 * INPUT    : policy_element_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServPolicyMapElementStatus(UI32_T policy_element_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServClassMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get class-map by class_map_index
 * INPUT    : class_map_index
 * OUTPUT   : class_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServClassMapEntry(UI32_T class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServClassMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next class-map by class_map_index
 * INPUT    : class_map_index
 * OUTPUT   : class_map_index, class_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServClassMapEntry(UI32_T *class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_Qos_GetNextDiffServClassMapIndexOfPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : get next class_map_index of the policy-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : class_map_index
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_Qos_GetNextDiffServClassMapIndexOfPolicyMap(UI32_T policy_map_index, UI32_T *class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServClassMapName
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map name by class_map_index
 * INPUT    : class_map_index, class_map_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServClassMapName(UI32_T class_map_index, const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServClassMapDescription
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map description by class_map_index
 * INPUT    : class_map_index, class_map_desc
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServClassMapDescription(UI32_T class_map_index, const char *class_map_desc);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServClassMapMatchType
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map matching type by class_map_index
 * INPUT    : class_map_index, match_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServClassMapMatchType(UI32_T class_map_index, UI32_T match_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServPolicyMapStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map row status by class_map_index
 * INPUT    : class_map_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServClassMapStatus(UI32_T class_map_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServClassMapAttachCtlAction
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate class-map and class-map element
 * INPUT    : class_map_index, index_type, class_element_index, attach_action
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : index_type(??)
 *            attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServClassMapAttachCtlAction(UI32_T class_map_index, UI32_T index_type, UI32_T class_element_index, UI32_T attach_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServAclEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get acl by acl_index
 * INPUT    : acl_index
 * OUTPUT   : acl_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServAclEntry(UI32_T acl_index, RULE_TYPE_MIB_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServAclEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next acl by acl_index
 * INPUT    : acl_index
 * OUTPUT   : acl_index, acl_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServAclEntry(UI32_T *acl_index, RULE_TYPE_MIB_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServAclType
 *------------------------------------------------------------------------------
 * PURPOSE  : change ACL type by acl_index
 * INPUT    : acl_index, acl_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServAclType(UI32_T acl_index, UI32_T acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServAclName
 *------------------------------------------------------------------------------
 * PURPOSE  : change ACL name by acl_index
 * INPUT    : acl_index, acl_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServAclName(UI32_T acl_index, const char *acl_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServAclStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change acl row status by acl_index
 * INPUT    : acl_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServAclStatus(UI32_T acl_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServAclAttachCtlAction
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate class-map and class-map element
 * INPUT    : acl_index, ace_type, ace_index, attach_action
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServAclAttachCtlAction(UI32_T acl_index, UI32_T ace_type, UI32_T ace_index, UI32_T attach_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServIpAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ace by ip_ace_index
 * INPUT    : ip_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetDiffServIpAceEntry(
    UI32_T ip_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServIpAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next ace by ip_ace_index
 * INPUT    : ip_ace_index
 * OUTPUT   : ip_ace_index, ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetNextDiffServIpAceEntry(
    UI32_T *ip_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceType
 *------------------------------------------------------------------------------
 * PURPOSE  : Set ace type by ip_ace_index
 * INPUT    : ip_ace_index, mib_ace_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceType(UI32_T ip_ace_index, UI32_T mib_ace_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceAccess
 *------------------------------------------------------------------------------
 * PURPOSE  : change access type by ip_ace_index
 * INPUT    : ip_ace_index, access_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceAccess(UI32_T ip_ace_index, UI32_T access_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceSourceIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by ip_ace_index
 * INPUT    : ip_ace_index, src_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceSourceIpAddr(UI32_T ip_ace_index, UI32_T src_ip);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceSourceIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by ip_ace_index
 * INPUT    : ip_ace_index, src_ip_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceSourceIpAddrBitmask(UI32_T ip_ace_index, UI32_T src_ip_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceDestIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by ip_ace_index
 * INPUT    : ip_ace_index, dst_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceDestIpAddr(UI32_T ip_ace_index, UI32_T dst_ip);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceDestIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip mask by ip_ace_index
 * INPUT    : ip_ace_index, dst_ip_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceDestIpAddrBitmask(UI32_T ip_ace_index, UI32_T dst_ip_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceProtocol
 *------------------------------------------------------------------------------
 * PURPOSE  : change protocol_type by ip_ace_index
 * INPUT    : ip_ace_index, protocol_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceProtocol(UI32_T ip_ace_index, UI32_T protocol_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAcePrec
 *------------------------------------------------------------------------------
 * PURPOSE  : change ip precedence by ip_ace_index
 * INPUT    : ip_ace_index, ip_precedence
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAcePrec(UI32_T ip_ace_index, UI32_T ip_precedence);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceTos
 *------------------------------------------------------------------------------
 * PURPOSE  : change tos by ip_ace_index
 * INPUT    : ip_ace_index, tos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceTos(UI32_T ip_ace_index, UI32_T tos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dscp by ip_ace_index
 * INPUT    : ip_ace_index, dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceDscp(UI32_T ip_ace_index, UI32_T dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceSourcePortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change source port operator by ip_ace_index
 * INPUT    : ip_ace_index, src_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceSourcePortOp(UI32_T ip_ace_index, UI32_T src_port_operator);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceMinSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change min src port by ip_ace_index
 * INPUT    : ip_ace_index, min_src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceMinSourcePort(UI32_T ip_ace_index, UI32_T min_src_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceMaxSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change max src port by ip_ace_index
 * INPUT    : ip_ace_index, max_src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceMaxSourcePort(UI32_T ip_ace_index, UI32_T max_src_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceSourcePortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port mask by ip_ace_index
 * INPUT    : ip_ace_index, src_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceSourcePortBitmask(UI32_T ip_ace_index, UI32_T src_port_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceDestPortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port operator by ip_ace_index
 * INPUT    : ip_ace_index, dst_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceDestPortOp(UI32_T ip_ace_index, UI32_T dst_port_operator);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceMinDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change min dst port by ip_ace_index
 * INPUT    : ip_ace_index, min_dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceMinDestPort(UI32_T ip_ace_index, UI32_T min_dst_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceMaxDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change max dst port by ip_ace_index
 * INPUT    : ip_ace_index, max_dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceMaxDestPort(UI32_T ip_ace_index, UI32_T max_dst_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceDestPortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port mask by ip_ace_index
 * INPUT    : ip_ace_index, dst_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceDestPortBitmask(UI32_T ip_ace_index, UI32_T dst_port_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceControlCode
 *------------------------------------------------------------------------------
 * PURPOSE  : change control_code by ip_ace_index
 * INPUT    : ip_ace_index, control_code
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceControlCode(UI32_T ip_ace_index, UI32_T control_code);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceControlCodeBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change control_code mask by ip_ace_index
 * INPUT    : ip_ace_index, control_code_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceControlCodeBitmask(UI32_T ip_ace_index, UI32_T control_code_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change ip ace row_status by ip_ace_index
 * INPUT    : ip_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpAceStatus(UI32_T ip_ace_index, UI32_T row_status);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE  : change ip ace time_range by ip_ace_index
 * INPUT    : ip_ace_index, time_range_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpAceTimeRange(
    UI32_T ip_ace_index,
    UI8_T *time_range_name
);
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServMacAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ace by mac_ace_index
 * INPUT    : mac_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QoS_GetDiffServMacAceEntry(
    UI32_T mac_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServMacAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next mac ace by mac_ace_index
 * INPUT    : mac_ace_index_p
 * OUTPUT   : mac_ace_index_p, ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_QoS_GetNextDiffServMacAceEntry(
    UI32_T *mac_ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceAccess
 *------------------------------------------------------------------------------
 * PURPOSE  : change access type by mac_ace_index
 * INPUT    : mac_ace_index, access_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceAccess(UI32_T mac_ace_index, UI32_T access_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAcePktformat
 *------------------------------------------------------------------------------
 * PURPOSE  : change packet format by mac_ace_index
 * INPUT    : mac_ace_index, pkt_format
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAcePktformat(UI32_T mac_ace_index, UI32_T pkt_format);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourceMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac by mac_ace_index
 * INPUT    : mac_ace_index, src_mac
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceSourceMacAddr(UI32_T mac_ace_index, const UI8_T *src_mac);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourceMacAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac mask by mac_ace_index
 * INPUT    : mac_ace_index, src_mac_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceSourceMacAddrBitmask(UI32_T mac_ace_index, const UI8_T *src_mac_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac by mac_ace_index
 * INPUT    : mac_ace_index, dst_mac
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceDestMacAddr(UI32_T mac_ace_index, const UI8_T *dst_mac);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestMacAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac mask by mac_ace_index
 * INPUT    : mac_ace_index, dst_mac_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceDestMacAddrBitmask(UI32_T mac_ace_index, const UI8_T *dst_mac_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceVidOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change vid operator by mac_ace_index
 * INPUT    : mac_ace_index, vid_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceVidOp(UI32_T mac_ace_index, UI32_T vid_operator);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMinVid
 *------------------------------------------------------------------------------
 * PURPOSE  : change min vid by mac_ace_index
 * INPUT    : mac_ace_index, min_vid
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMinVid(UI32_T mac_ace_index, UI32_T min_vid);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceVidBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change vid mask by mac_ace_index
 * INPUT    : mac_ace_index, vid_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceVidBitmask(UI32_T mac_ace_index, UI32_T vid_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMaxVid
 *------------------------------------------------------------------------------
 * PURPOSE  : change max vid by mac_ace_index
 * INPUT    : mac_ace_index, max_vid
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMaxVid(UI32_T mac_ace_index, UI32_T max_vid);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceEtherTypeOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change ether-type operator by mac_ace_index
 * INPUT    : mac_ace_index, ether_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceEtherTypeOp(UI32_T mac_ace_index, UI32_T ether_operator);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceEtherTypeBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change ether-type mask by mac_ace_index
 * INPUT    : mac_ace_index, ether_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceEtherTypeBitmask(UI32_T mac_ace_index, UI32_T ether_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMinEtherType
 *------------------------------------------------------------------------------
 * PURPOSE  : change min ether-type by mac_ace_index
 * INPUT    : mac_ace_index, min_ether_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMinEtherType(UI32_T mac_ace_index, UI32_T min_ether_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMaxEtherType
 *------------------------------------------------------------------------------
 * PURPOSE  : change max ether-type by mac_ace_index
 * INPUT    : mac_ace_index, max_ether_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMaxEtherType(UI32_T mac_ace_index, UI32_T max_ether_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change row status by mac_ace_index
 * INPUT    : mac_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceStatus(UI32_T mac_ace_index, UI32_T row_status);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE  : change ace time_range by mac_ace_index
 * INPUT    : mac_ace_index, time_range_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceTimeRange(
    UI32_T mac_ace_index,
    UI8_T *time_range_name
);
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourceIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by mac_ace_index
 * INPUT    : mac_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourceIpAddr(
    UI32_T mac_ace_index,
    UI32_T addr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourceIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by mac_ace_index
 * INPUT    : ip_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourceIpAddrBitmask(
    UI32_T mac_ace_index,
    UI32_T addr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dest ip by mac_ace_index
 * INPUT    : mac_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestIpAddr(
    UI32_T mac_ace_index,
    UI32_T addr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dest ip mask by mac_ace_index
 * INPUT    : ip_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestIpAddrBitmask(
    UI32_T mac_ace_index,
    UI32_T addr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMaceAceSourceIpv6Addr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by ipv6_ace_index
 * INPUT    : mac_ace_index, src_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMaceAceSourceIpv6Addr(
    UI32_T mac_ace_index,
    const UI8_T *src_ip
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourceIpv6AddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip prefix length by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourceIpv6AddrPrefix(
    UI32_T mac_ace_index,
    UI32_T src_ip_prefix
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestIpv6Addr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by mac_ace_index
 * INPUT    : ipv6_ace_index, dst_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestIpv6Addr(
    UI32_T mac_ace_index,
    const UI8_T *dst_ip
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestIpv6AddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip prefix length by mac_ace_index
 * INPUT    : ipv6_ace_index, dst_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestIpv6AddrPrefix(
    UI32_T mac_ace_index,
    UI32_T dst_ip_prefix
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceProtocol
 *------------------------------------------------------------------------------
 * PURPOSE  : change protocol_type by mac_ace_index
 * INPUT    : mac_ace_index, protocol_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceProtocol(
    UI32_T mac_ace_index,
    UI32_T protocol_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourcePortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change source port operator by mac_ace_index
 * INPUT    : mac_ace_index, src_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourcePortOp(
    UI32_T mac_ace_index,
    UI32_T src_port_operator
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port by mac_ace_index
 * INPUT    : ip_ace_index, src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourcePort(
    UI32_T mac_ace_index,
    UI32_T src_port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceSourcePortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port mask by mac_ace_index
 * INPUT    : ip_ace_index, src_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceSourcePortBitmask(
    UI32_T mac_ace_index,
    UI32_T src_port_mask
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestPortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port operator by mac_ace_index
 * INPUT    : ip_ace_index, dst_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestPortOp(
    UI32_T mac_ace_index,
    UI32_T dst_port_operator
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port by mac_ace_index
 * INPUT    : ip_ace_index, dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestPort(
    UI32_T mac_ace_index,
    UI32_T dst_port
);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceDestPortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port mask by mac_ace_index
 * INPUT    : ip_ace_index, dst_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServMacAceDestPortBitmask(
    UI32_T mac_ace_index,
    UI32_T dst_port_mask
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServIpv6AceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ui ace by ipv6_ace_index
 * INPUT    : ipv6_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetDiffServIpv6AceEntry(
    UI32_T ipv6_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServIpv6AceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next ui ace by ipv6_ace_index
 * INPUT    : ipv6_ace_index
 * OUTPUT   : ipv6_ace_index, ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetNextDiffServIpv6AceEntry(
    UI32_T *ipv6_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceType
 *------------------------------------------------------------------------------
 * PURPOSE  : set ace type by ipv6_ace_index
 * INPUT    : ip_ace_index, mib_ace_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceType(UI32_T ipv6_ace_index, UI32_T mib_ace_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceAccess
 *------------------------------------------------------------------------------
 * PURPOSE  : change access type by ipv6_ace_index
 * INPUT    : ipv6_ace_index, access_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceAccess(UI32_T ipv6_ace_index, UI32_T access_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceSourceIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceSourceIpAddr(UI32_T ipv6_ace_index, const UI8_T *src_ip);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceSourceIpAddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip prefix length by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceSourceIpAddrPrefix(UI32_T ipv6_ace_index, UI32_T src_ip_prefix);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDestIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceDestIpAddr(UI32_T ipv6_ace_index, const UI8_T *dst_ip);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDestIpAddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip prefix length by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceDestIpAddrPrefix(UI32_T ipv6_ace_index, UI32_T dst_ip_prefix);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceNextHeader
 *------------------------------------------------------------------------------
 * PURPOSE  : change next header by ipv6_ace_index
 * INPUT    : ipv6_ace_index, next_header
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceNextHeader(UI32_T ipv6_ace_index, UI32_T next_header);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dscp by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceDscp(UI32_T ipv6_ace_index, UI32_T dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceFlowLabel
 *------------------------------------------------------------------------------
 * PURPOSE  : change flow label by ipv6_ace_index
 * INPUT    : ipv6_ace_index, flow_label
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceFlowLabel(UI32_T ipv6_ace_index, UI32_T flow_label);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change row status by ipv6_ace_index
 * INPUT    : ipv6_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServIpv6AceStatus(UI32_T ipv6_ace_index, UI32_T row_status);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE  : change ace time_range by mac_ace_index
 * INPUT    : mac_ace_index, time_range_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceTimeRange(
    UI32_T ipv6_ace_index,
    UI8_T *time_range_name
);
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpAceSourcePortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change source port operator by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceSourcePortOp(
    UI32_T ipv6_ace_index,
    UI32_T src_port_operator
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceSourcePort(
    UI32_T ipv6_ace_index,
    UI32_T src_port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceSourcePortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port mask by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceSourcePortBitmask(
    UI32_T ipv6_ace_index,
    UI32_T src_port_mask
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDestPortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port operator by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceDestPortOp(
    UI32_T ipv6_ace_index,
    UI32_T dst_port_operator
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceDestPort(
    UI32_T ipv6_ace_index,
    UI32_T dst_port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServIpv6AceDestPortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port mask by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_MGR_QoS_SetDiffServIpv6AceDestPortBitmask(
    UI32_T ipv6_ace_index,
    UI32_T dst_port_mask
);
#endif /* SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServActionEntry(UI32_T action_index, RULE_TYPE_MIB_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_index, action_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServActionEntry(UI32_T *action_index, RULE_TYPE_MIB_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionBitmap
 *------------------------------------------------------------------------------
 * PURPOSE  : change action bitmap by action_index
 * INPUT    : action_index, action_bitmap
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionBitmap(UI32_T action_index, const UI8_T *action_bitmap);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionBitmap
 *------------------------------------------------------------------------------
 * PURPOSE  : change action bitmap by action_index
 * INPUT    : action_index, action_bitmap
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionBitmap(UI32_T action_index, const UI8_T *action_bitmap);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionPktNewPri
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new priority by action_index
 * INPUT    : action_index, pkt_new_pri
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionPktNewPri(UI32_T action_index, UI32_T pkt_new_pri);
BOOL_T L4_MGR_QoS_SetDiffServActionPktNewPhb(UI32_T action_index, UI32_T pkt_new_phb);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionPktNewIpPrec
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new ip precedence by action_index
 * INPUT    : action_index, pkt_new_ip_prec
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionPktNewIpPrec(UI32_T action_index, UI32_T pkt_new_ip_prec);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionGreenPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionRedPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (red) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionRedPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionYellowPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (yellow) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionYellowPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionYellowTransmit
 *------------------------------------------------------------------------------
 * PURPOSE  : change the action of (green) packet by action_index
 * INPUT    : action_index, transmit
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionGreenTransmit(UI32_T action_index, UI32_T transmit);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionYellowDrop
 *------------------------------------------------------------------------------
 * PURPOSE  : change the action of (yellow) packet by action_index
 * INPUT    : action_index, yellow_drop
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionYellowDrop(UI32_T action_index, UI32_T yellow_drop);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionRedDrop
 *------------------------------------------------------------------------------
 * PURPOSE  : change the action of (red) packet by action_index
 * INPUT    : action_index, red_drop
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionRedDrop(UI32_T action_index, UI32_T red_drop);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionPktPri
 *------------------------------------------------------------------------------
 * PURPOSE  : Change the packet priority for bundle group
 * INPUT    : action_index, priority
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionPktPri(UI32_T action_index, UI32_T priority);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionDropPrecedence
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) drop precedence by action_index
 * INPUT    : action_index, drop_precedence
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionDropPrecedence(UI32_T action_index, UI32_T drop_precedence);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionRedDropPrecedence
 *------------------------------------------------------------------------------
 * PURPOSE  : change (red) drop precedence by action_index
 * INPUT    : action_index, drop_precedence
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionRedDropPrecedence(UI32_T action_index, UI32_T drop_precedence);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionYellowDropPrecedence
 *------------------------------------------------------------------------------
 * PURPOSE  : change (yellow) drop precedence by action_index
 * INPUT    : action_index, drop_precedence
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionYellowDropPrecedence(UI32_T action_index, UI32_T drop_precedence);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServActionStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change action row_status by action_index
 * INPUT    : action_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServActionStatus(UI32_T action_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get meter entry by meter_index
 * INPUT    : meter_index
 * OUTPUT   : meter_param
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetDiffServMeterEntry(UI32_T meter_index, RULE_TYPE_TBParamEntry_T *meter_param);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next meter entry by meter_index
 * INPUT    : meter_index
 * OUTPUT   : meter_index, meter_param
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_GetNextDiffServMeterEntry(UI32_T *meter_index,RULE_TYPE_TBParamEntry_T *meter_param);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterModel
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter model by meter_index
 * INPUT    : meter_index, meter_model
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterModel(UI32_T meter_index, UI32_T meter_model);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterRate
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter rate by meter_index
 * INPUT    : meter_index, meter_rate
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterRate(UI32_T meter_index, UI32_T meter_rate);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterBurstSize
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter burst size by meter_index
 * INPUT    : meter_index, burst_size
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterBurstSize(UI32_T meter_index, UI32_T burst_size);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterPeakRate
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter rate by meter_index
 * INPUT    : meter_index, meter_rate
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterPeakRate(UI32_T meter_index, UI32_T meter_rate);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterPeakBurstSize
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter burst size by meter_index
 * INPUT    : meter_index, burst_size
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterPeakBurstSize(UI32_T meter_index, UI32_T burst_size);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterInterval
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter update interval by meter_index
 * INPUT    : meter_index, meter_interval
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterInterval(UI32_T meter_index, UI32_T meter_interval);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMeterStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter row_status by meter_index
 * INPUT    : meter_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMeterStatus(UI32_T meter_index, UI32_T row_status);

#if ((SYS_CPNT_DHCPSNP == TRUE) || (SYS_CPNT_DHCP_RELAY_OPTION82 == TRUE))
/*fuzhimin,20090505*/
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetDhcpSnoopingPktAction
 *------------------------------------------------------------------------------
 * PURPOSE  : for DHCP snooping
 *            config chip to control DHCP  packets
 * INPUT    : action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 *                          2   L4_MGR_DO_ACTION_REDIRECT_TO_CPU
 *                          3   L4_MGR_DO_ACTION_COPY_TO_CPU
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : if chip is already configured, update/delete config from chip
 *            if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetDhcpSnoopingPktAction(UI32_T action);

/*fuzhimin,20090505,end*/
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetDhcpClientPktAction
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to control DHCP client packets
 * INPUT    : action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 *                          2   L4_MGR_DO_ACTION_REDIRECT_TO_CPU
 *                          3   L4_MGR_DO_ACTION_COPY_TO_CPU
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : if chip is already configured, update/delete config from chip
 *            if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetDhcpClientPktAction(UI32_T action);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetDhcpClientPktActionByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to control DHCP client packets
 * INPUT    : lport
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 *                          2   L4_MGR_DO_ACTION_REDIRECT_TO_CPU
 *                          3   L4_MGR_DO_ACTION_COPY_TO_CPU
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetDhcpClientPktActionByLport(UI32_T lport, UI32_T action);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetDhcpServerPktAction
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to control DHCP server packets
 * INPUT    : action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 *                          2   L4_MGR_DO_ACTION_REDIRECT_TO_CPU
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : if chip is already configured, update/delete config from chip
 *            if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetDhcpServerPktAction(UI32_T action);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetDhcpServerPktActionByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to control DHCP server packets
 * INPUT    : lport
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 *                          2   L4_MGR_DO_ACTION_REDIRECT_TO_CPU
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetDhcpServerPktActionByLport(UI32_T lport, UI32_T action);

#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetSrcIpFilterByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source IP
 * INPUT    : lport, sip, vid
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FREE_SELECTOR_UNAVAILABLE/
 *            RULE_TYPE_OK/
 *            RULE_TYPE_FAIL
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_SetSrcIpFilterByLport(UI32_T lport, UI32_T sip, UI32_T vid, UI32_T action);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetSrcMacSrcIpFilterByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source IP and Source MAC
 * INPUT    : lport, src_mac, sip, vid
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FREE_SELECTOR_UNAVAILABLE/
 *            RULE_TYPE_OK/
 *            RULE_TYPE_FAIL
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_SetSrcMacSrcIpFilterByLport(UI32_T lport, const UI8_T *src_mac, UI32_T sip, UI32_T vid, UI32_T action);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetIpPktActionByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to drop IP packets
 * INPUT    : lport, src_mac, sip
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          4   L4_MGR_DO_ACTION_DROP
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FREE_SELECTOR_UNAVAILABLE/
 *            RULE_TYPE_OK/
 *            RULE_TYPE_FAIL
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, drop action in this API has lowest priority. That is,
 *               L4_MGR_SetDhcpClientPktActionByLport(...),
 *               L4_MGR_SetDhcpServerPktActionByLport(...),
 *               L4_MGR_SetSrcIpFilterByLport(...),
 *               L4_MGR_SetSrcMacSrcIpFilterByLport(...)
 *               dominate some IP packets do not be drop
 *            3, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_SetIpPktActionByLport(UI32_T lport, UI32_T action);
#endif /* #if (SYS_CPNT_IP_SOURCE_GUARD == TRUE) */
#endif /* SYS_CPNT_DHCPSNP */

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L4_MGR_HandleIPCReqMsg
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Handle the ipc request message for csca mgr.
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
BOOL_T L4_MGR_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_MGR_Create_InterCSC_Relation
 *------------------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT :  None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES :  None
 *------------------------------------------------------------------------------
 */
void L4_MGR_Create_InterCSC_Relation();


#if (SYS_CPNT_DAI == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_SetArpPktActionByDai
 *------------------------------------------------------------------------------
 * PURPOSE:  for ARP INSPETCION (DAI)
 *            config chip to trap ARP(BC+REPLY) packets to CPU
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetArpPktActionByDai();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_RecoverArpPktActionByDai
 *------------------------------------------------------------------------------
 * PURPOSE:  for ARP INSPETCION (DAI)
 *            config chip to recover the behavior of ARP(BC+REPLY) packets to CPU
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_RecoverArpPktActionByDai();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_ArpACLCheckForDai
 *------------------------------------------------------------------------------
 * PURPOSE:  for ARP INSPETCION (DAI)
 *            to check if match the existed arp rules with arp packets
 * INPUT:    arp_acl_name - check which arp acl
 *           arp_pkt_p    - compared arp packet
 * OUTPUT:   b_acl_drop   - is drop
 * RETURN:   TRUE/FALSE
 * NOTE:     TRUE means match rule; FALSE means no match rule
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_ArpACLCheckForDai(const char *arp_acl_name, DAI_TYPE_ArpPktFormat_T *arp_pkt_p, BOOL_T *b_acl_drop);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetDiffServARPAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ui ace by arp_ace_index
 * INPUT    : arp_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : for SNMP - diffServArpAceTable
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetDiffServARPAceEntry(
    UI32_T arp_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_GetNextDiffServArpAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next ui ace by arp_ace_index
 * INPUT    : arp_ace_index
 * OUTPUT   : arp_ace_index, ace_p
 * RETURN   : Error code
 * NOTE     : for SNMP - diffServArpAceTable
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_MGR_QoS_GetNextDiffServArpAceEntry(
    UI32_T *arp_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceAction
 *------------------------------------------------------------------------------
 * PURPOSE  : change action by arp_ace_index
 * INPUT    : arp_ace_index, action_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceAction
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceAction(UI32_T arp_ace_index, UI32_T action_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAcePktType
 *------------------------------------------------------------------------------
 * PURPOSE  : change pkt type by arp_ace_index
 * INPUT    : arp_ace_index, pkt_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAcePktType
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAcePktType(UI32_T arp_ace_index, UI32_T pkt_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceSrcIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by arp_ace_index
 * INPUT    : arp_ace_index, src_ip_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceIpAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceSrcIpAddr(UI32_T arp_ace_index, UI32_T src_ip_addr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceSrcIpAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by arp_ace_index
 * INPUT    : arp_ace_index, src_ip_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceIpAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceSrcIpAddrMask(UI32_T arp_ace_index, UI32_T src_ip_addr_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceDstIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by arp_ace_index
 * INPUT    : arp_ace_index, dst_ip_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestIpAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceDstIpAddr(UI32_T arp_ace_index, UI32_T dst_ip_addr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceSrcIpAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by arp_ace_index
 * INPUT    : arp_ace_index, src_ip_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestIpAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceDstIpAddrMask(UI32_T arp_ace_index, UI32_T dst_ip_addr_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceSrcMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac address by arp_ace_index
 * INPUT    : arp_ace_index, dst_mac_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceMacAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceSrcMacAddr(UI32_T arp_ace_index, UI8_T *src_mac_addr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceSrcMacAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac address mask by arp_ace_index
 * INPUT    : arp_ace_index, src_mac_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceMacAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceSrcMacAddrMask(UI32_T arp_ace_index, UI8_T *src_mac_addr_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceDstMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac address by arp_ace_index
 * INPUT    : arp_ace_index, dst_mac_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestMacAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceDstMacAddr(UI32_T arp_ace_index, UI8_T *dst_mac_addr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceDstMacAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac address mask by arp_ace_index
 * INPUT    : arp_ace_index, dst_mac_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestMacAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceDstMacAddrMask(UI32_T arp_ace_index, UI8_T *dst_mac_addr_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change row status by arp_ace_index
 * INPUT    : arp_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceStatus
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceStatus(UI32_T arp_ace_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServArpAceLogStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change log status by arp_ace_index
 * INPUT    : arp_ace_index, log_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp -diffServArpAceLogStatus
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServArpAceLogStatus(UI32_T arp_ace_index, BOOL_T log_status);

#if (SYS_CPNT_DAI_RATE_LIMIT_BY_RULE == TRUE)
/*------------------------------------------------------------------------------
* ROUTINE NAME - L4_MGR_DAI_SetArpPktActionAndRate
*------------------------------------------------------------------------------
* PURPOSE:  For ARP INSPETCION (DAI)
*            config chip to trap ARP packets to CPU and set rate limit
* INPUT:    enable    - TRUE: Add rule
*                       FALE: Delete rule
*           interface - interface information
*           rate      - packet per second
* OUTPUT:   None
* RETURN:   RULE_TYPE_OK / RULE_TYPE_FAIL
* NOTE:     None
*------------------------------------------------------------------------------
*/
RULE_TYPE_RETURN_TYPE_T
L4_MGR_DAI_SetArpPktActionAndRate(
    BOOL_T enable,
    RULE_TYPE_INTERFACE_INFO_T interface,
    UI32_T rate
);

/*------------------------------------------------------------------------------
* ROUTINE NAME - L4_MGR_DAI_GetArpPktCounter
*------------------------------------------------------------------------------
* PURPOSE:  For ARP INSPETCION (DAI)
*            Get the ARP packet counter by port.
* INPUT:    interface_p    - Interface information
*           trunk_member_p - Trunk member list,
*                            input "NULL" if the interface is not trunk.
*           reset_counter  - TRUE: clear counter, FALSE: do nothing.
* OUTPUT:   total_cnt_p    - ARP packet counter.
* RETURN:   RULE_TYPE_OK / RULE_TYPE_FAIL
* NOTE:     None
*------------------------------------------------------------------------------
*/
RULE_TYPE_RETURN_TYPE_T
L4_MGR_DAI_GetArpPktCounter(
    RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_TYPE_TRUNK_MEMBER_T *trunk_member_p,
    BOOL_T reset_counter,
    UI32_T *total_cnt_p
);
#endif /* SYS_CPNT_DAI_RATE_LIMIT_BY_RULE */
#endif /* SYS_CPNT_DAI */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceCosOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change cos operator by mac_ace_index
 * INPUT    : mac_ace_index, cos_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceCosOp(UI32_T mac_ace_index, UI32_T cos_operator);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceCosBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change cos mask by mac_ace_index
 * INPUT    : mac_ace_index, cos_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceCosBitmask(UI32_T mac_ace_index, UI32_T cos_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMinCos
 *------------------------------------------------------------------------------
 * PURPOSE  : change min cos by mac_ace_index
 * INPUT    : mac_ace_index, min_cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMinCos(UI32_T mac_ace_index, UI32_T min_cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_QoS_SetDiffServMacAceMaxCos
 *------------------------------------------------------------------------------
 * PURPOSE  : change max cos by mac_ace_index
 * INPUT    : mac_ace_index, max_cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_QoS_SetDiffServMacAceMaxCos(UI32_T mac_ace_index, UI32_T max_cos);

#if (SYS_CPNT_CLUSTER == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetClusterPktAction
 *------------------------------------------------------------------------------
 * PURPOSE  : for Cluster
 *            config chip to control cluster  packets
 * INPUT    : action        -- could be:
 *                          TRUE    copy cluster packet to cpu
 *                          FALSE   not copy cluster packet to cpu
 *
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : if chip is already configured, update/delete config from chip
 *            if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_MGR_SetClusterPktAction(BOOL_T action);
#endif   /* End of SYS_CPNT_CLUSTER */

#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetSrcIpv6FilterByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IPv6 Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source IPv6
 * INPUT    : lport, sip6, vid
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          1   L4_MGR_DO_ACTION_PERMIT
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FREE_SELECTOR_UNAVAILABLE/
 *            RULE_TYPE_OK/
 *            RULE_TYPE_FAIL
 * NOTE     : 1, this API is common interface to IPv6 Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_SetSrcIpv6FilterByLport(
    UI32_T  lport,
    UI8_T   sip6[SYS_TYPE_IPV6_ADDR_LEN],
    UI32_T  vid,
    UI32_T  action);


/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetIpv6PktActionByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IPv6 Source Guard
 *            config chip to drop IP packets
 * INPUT    : lport
 *            action        -- could be:
 *                          0   L4_MGR_DO_ACTION_DELETE_RULE
 *                          4   L4_MGR_DO_ACTION_DROP
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_FREE_SELECTOR_UNAVAILABLE/
 *            RULE_TYPE_OK/
 *            RULE_TYPE_FAIL
 * NOTE     : 1, this API is common interface to IPv6 Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, drop action in this API has lowest priority. That is,
 *               L4_MGR_SetDhcpClientPktActionByLport(...),
 *               L4_MGR_SetDhcpServerPktActionByLport(...),
 *               L4_MGR_SetSrcIpFilterByLport(...),
 *               L4_MGR_SetSrcMacSrcIpFilterByLport(...)
 *               dominate some IP packets do not be drop
 *            3, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
UI32_T L4_MGR_SetIpv6PktActionByLport(UI32_T lport, UI32_T action);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_MGR_SetPermitIpv6LinkLocal
 *------------------------------------------------------------------------------
 * PURPOSE  : Permit all IPv6 link local packet
 * INPUT    : is_enable - TRUE / FALSE
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK / RULE_TYPE_FAIL
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_SetPermitIpv6LinkLocal(
    BOOL_T  is_enable
);

#endif   /* End of SYS_CPNT_IPV6_SOURCE_GUARD */

#if (SYS_CPNT_PBR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_PBR_BindPbrAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Bind ACL to vlan for PBR
 * INPUT    : vlan      - (Key) Vlan ID.
 *            seq_num   - (Key) Route map sequence number.
 *            acl_index - (Key) ACL index.
 *            action_p  - Policy based routing action.
 * OUTPUT   : none
 * RETURN   : RULE_TYPE_OK / error_code(RULE_TYPE_RETURN_TYPE_T)
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_PBR_BindAcl(
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_index,
    RULE_TYPE_PBR_ACTION_T *action_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_MGR_PBR_UnbindAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Unind ACL to vlan for PBR
 * INPUT    : vlan      - (Key) vlan ID
 *            seq_num   - (Key) Route map sequence number
 *            acl_index - (Key) ACL index.
 * OUTPUT   : none
 * RETURN   : RULE_TYPE_OK / error_code(RULE_TYPE_RETURN_TYPE_T)
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_MGR_PBR_UnbindAcl(
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_index
);
#endif /* SYS_CPNT_PBR */

#endif   /* End of _L4_MGR_H */

