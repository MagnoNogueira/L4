/*
 * $Id: portmode.h,v 1.26 Broadcom SDK $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * 
 */

#ifndef _PORT_IOCTL_H
#define _PORT_IOCTL_H

typedef struct phy_ioctl_req{
	union {
		int arg_int;
	}arg;
}phy_ioctl_req_t;

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


#endif/*_PORT_IOCTL_H */

