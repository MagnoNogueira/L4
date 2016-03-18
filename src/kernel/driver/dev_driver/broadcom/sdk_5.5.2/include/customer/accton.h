/*
 * $Id: accton.h,v 1.1.2.2 2005/03/28 11:02:21 nmou Exp $
 * $Copyright: (c) 2005 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:	accton.h
 * Purpose:     Accton-specific APIs 
 *
 */

#ifndef _ACCTON_H_
#define _ACCTON_H_

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

#endif /* _ACCTON_H_ */
