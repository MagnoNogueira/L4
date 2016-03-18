/*
 * $Id: accton_phy.h,v 1.1.2.2 2005/03/28 11:02:21 nmou Exp $
 * $Copyright: (c) 2005 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:	accton_phy.h
 * Purpose:     Accton-specific APIs 
 *
 */

#ifndef _ACCTON_PHY_H_
#define _ACCTON_PHY_H_

#include <shared/portmode.h>
#include <shared/port.h>
#include <shared/phyconfig.h>

#include <bcm/types.h>

#include <bcm/port.h>

typedef enum bcm_phy_mau_type_e {
	MauAutoNegRemoteFaultAdvertised_noError,
	MauAutoNegRemoteFaultAdvertised_offline,
	MauAutoNegRemoteFaultAdvertised_linkFailure,
	MauAutoNegRemoteFaultAdvertised_autoNegError,
	MauAutoNegRemoteSignaling_detected,
	MauAutoNegRemoteSignaling_notdetected,
	MauAutoNegConfig_other,
 	MauAutoNegConfig_configuring,
 	MauAutoNegConfig_complete,
 	MauAutoNegConfig_disabled,
 	MauAutoNegConfig_parallelDetectFail,
 	MauAutoNegCapReceivedBits_bOther,
 	MauAutoNegCapReceivedBits_b10baseT,
 	MauAutoNegCapReceivedBits_b10baseTFD,
 	MauAutoNegCapReceivedBits_b100beT4as,
 	MauAutoNegCapReceivedBits_b100baseTX,
 	MauAutoNegCapReceivedBits_b100baseTXFD,
 	MauAutoNegCapReceivedBits_b100baseT2,
 	MauAutoNegCapReceivedBits_b100baseT2FD,
 	MauAutoNegCapReceivedBits_bFdxPause,
 	MauAutoNegCapReceivedBits_bFdxAPause,
 	MauAutoNegCapReceivedBits_bFdxSPause,
 	MauAutoNegCapReceivedBits_bFdxBPause,
 	MauAutoNegCapReceivedBits_b1000baseX,
 	MauAutoNegCapReceivedBits_b1000baseXFD,
 	MauAutoNegCapReceivedBits_b1000baseT,
 	MauAutoNegCapReceivedBits_b1000baseTFD,
 	MauAutoNegRemoteFaultReceived_noError,
 	MauAutoNegRemoteFaultReceived_offline,
 	MauAutoNegRemoteFaultReceived_linkFailure,
 	MauAutoNegRemoteFaultReceived_autoNegError,
 	MauJabberState_other,
 	MauJabberState_unknown,
 	MauJabberState_noJabber,
 	MauJabberState_jabbering
} bcm_phy_mau_type_t;

typedef struct phy_ioctl_req {
	union {
		int arg_int;
	} arg;
} phy_ioctl_req_t;

#define	PHY_IOC_GET_AN_STATE			0
#define	PHY_IOC_GET_LINKPA_AN_SIG		1
#define	PHY_IOC_GET_LINKPA_AN_RFAULT	2
#define	PHY_IOC_GET_AN_RFAULT			3
#define	PHY_IOC_GET_JABBER_STATE		4

#define PHY_IOC_ifMauJabberState_other	1L
#define PHY_IOC_ifMauJabberState_unknown	2L
#define PHY_IOC_ifMauJabberState_noJabber	3L
#define PHY_IOC_ifMauJabberState_jabbering	4L

#define PHY_IOC_ifMauAutoNegRemoteSignaling_detected	1L
#define PHY_IOC_ifMauAutoNegRemoteSignaling_notdetected	2L

#define PHY_IOC_ifMauAutoNegConfig_other	1L
#define PHY_IOC_ifMauAutoNegConfig_configuring	2L
#define PHY_IOC_ifMauAutoNegConfig_complete	3L
#define PHY_IOC_ifMauAutoNegConfig_disabled	4L
#define PHY_IOC_ifMauAutoNegConfig_parallelDetectFail	5L

#define PHY_IOC_ifMauAutoNegRemoteFaultAdvertised_noError	1L
#define PHY_IOC_ifMauAutoNegRemoteFaultAdvertised_offline	2L
#define PHY_IOC_ifMauAutoNegRemoteFaultAdvertised_linkFailure	3L
#define PHY_IOC_ifMauAutoNegRemoteFaultAdvertised_autoNegError	4L

#define PHY_IOC_ifMauAutoNegRemoteFaultReceived_noError	1L
#define PHY_IOC_ifMauAutoNegRemoteFaultReceived_offline	2L
#define PHY_IOC_ifMauAutoNegRemoteFaultReceived_linkFailure	3L
#define PHY_IOC_ifMauAutoNegRemoteFaultReceived_autoNegError	4L

/* added by charles, accton expantion for phy control call*/
extern int accton_bcm_port_phy_ioctl(int unit,int port,unsigned int cmd,void * io_req);
#define bcm_port_phy_ioctl accton_bcm_port_phy_ioctl

#endif /* _ACCTON_PHY_H_ */

