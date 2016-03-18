/*
 * $Id: port.h,v 1.18 Broadcom SDK $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * This file defines common network port parameters.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define port parameters.
 */

#ifndef _SHR_PORT_H
#define _SHR_PORT_H

/*
 * Typedef:
 *    _shr_port_t
 * Purpose:
 *    Port number type for shared definitions
 * Note:
 *    Currently it is NOT used to define bcm_port_t and soc_port_t, but thus
 *    should be changed
 */
typedef int _shr_port_t;

/*
 * Defines:
 *	_SHR_PORT_DUPLEX_*
 * Purpose:
 *	Defines duplexity of a port
 */

typedef enum _shr_port_duplex_e {
    _SHR_PORT_DUPLEX_HALF,
    _SHR_PORT_DUPLEX_FULL,
    _SHR_PORT_DUPLEX_COUNT	/* last, please */
} _shr_port_duplex_t;

/*
 * Defines:
 *	_SHR_PORT_IF_*
 * Purpose:
 *	Defines interface type between MAC and PHY.
 */

typedef enum _shr_port_if_e {
    _SHR_PORT_IF_NOCXN,		/* No physical connection */
    _SHR_PORT_IF_NULL,		/* Pass-through connection without PHY */
    _SHR_PORT_IF_MII,
    _SHR_PORT_IF_GMII,
    _SHR_PORT_IF_SGMII,
    _SHR_PORT_IF_TBI,
    _SHR_PORT_IF_XGMII,
    _SHR_PORT_IF_RGMII,
    _SHR_PORT_IF_RvMII,
    _SHR_PORT_IF_COUNT		/* last, please */
} _shr_port_if_t;

/*
 * Defines:
 *	_SHR_PORT_MS_*
 * Purpose:
 *	Defines master/slave mode of a port (PHY).
 */

typedef enum _shr_port_ms_e {
    _SHR_PORT_MS_SLAVE,
    _SHR_PORT_MS_MASTER,
    _SHR_PORT_MS_AUTO,
    _SHR_PORT_MS_NONE,
    _SHR_PORT_MS_COUNT		/* last, please */
} _shr_port_ms_t;

/*
 * Defines:
 *	_SHR_PORT_ENCAP_*
 * Purpose:
 *	Defines various header encapsulation modes.
 * Notes:
 *	WARNING: values must match MAC_TXCTRL register HDRMODE field.
 */

typedef enum _shr_port_encap_e {
    _SHR_PORT_ENCAP_IEEE = 0,	/* IEEE 802.3 Ethernet-II */
    _SHR_PORT_ENCAP_HIGIG = 1,	/* HIGIG Header mode       */
    _SHR_PORT_ENCAP_B5632 = 2,	/* BCM5632 Header mode     */
    _SHR_PORT_ENCAP_HIGIG2 = 3,	/* HIGIG2 Header mode      */
    _SHR_PORT_ENCAP_COUNT	/* last, please */
} _shr_port_encap_t;

#define	_SHR_PORT_ENCAP_NAMES_INITIALIZER { \
	"IEEE", \
	"HiGig", \
	"B5632", \
	"HiGig2", \
	}

/*
 * Defines:
 *	_SHR_PORT_STP_*
 * Purpose:
 *	Defines the spanning tree states of a port.
 */

typedef enum _shr_port_stp_e {
    _SHR_PORT_STP_DISABLE,
    _SHR_PORT_STP_BLOCK,
    _SHR_PORT_STP_LISTEN,
    _SHR_PORT_STP_LEARN,
    _SHR_PORT_STP_FORWARD,
    _SHR_PORT_STP_COUNT    /* last, please */
} _shr_port_stp_t;

/*
 * Defines:
 *      _SHR_PORT_MDIX_*
 * Purpose:
 *      Defines the MDI crossover (MDIX) modes for the port
 */
typedef enum _shr_port_mdix_e {
    _SHR_PORT_MDIX_AUTO,
    _SHR_PORT_MDIX_FORCE_AUTO,
    _SHR_PORT_MDIX_NORMAL,
    _SHR_PORT_MDIX_XOVER,
    _SHR_PORT_MDIX_COUNT    /* last, please */
} _shr_port_mdix_t;

/*
 * Defines:
 *      _SHR_PORT_MDIX_STATUS_*
 * Purpose:
 *      Defines the MDI crossover state
 */
typedef enum _shr_port_mdix_status_e {
    _SHR_PORT_MDIX_STATUS_NORMAL,
    _SHR_PORT_MDIX_STATUS_XOVER,
    _SHR_PORT_MDIX_STATUS_COUNT       /* last, please */
} _shr_port_mdix_status_t;

/*
 * Defines:
 *      _SHR_PORT_MEDIUM_*
 * Purpose:
 *      Supported physical mediums
 */
typedef enum _shr_port_medium_e {
    _SHR_PORT_MEDIUM_NONE              = 0,
    _SHR_PORT_MEDIUM_COPPER            = 1,
    _SHR_PORT_MEDIUM_FIBER             = 2,
    _SHR_PORT_MEDIUM_COUNT             /* last, please */
} _shr_port_medium_t;

/*
 * Defines:
 *      _SHR_PORT_MCAST_FLOOD_*
 * Purpose:
 *      Multicast packet flooding mode
 */
typedef enum _shr_port_mcast_flood_e {
    _SHR_PORT_MCAST_FLOOD_ALL     = 0,
    _SHR_PORT_MCAST_FLOOD_UNKNOWN = 1,
    _SHR_PORT_MCAST_FLOOD_NONE    = 2,
    _SHR_PORT_MCAST_FLOOD_COUNT         /* last, please */
} _shr_port_mcast_flood_t;

/*
 * Defines:
 *	_SHR_PORT_CABLE_STATE_*
 * Purpose:
 *	Cable diag state (per pair and overall)
 */
typedef enum {
    _SHR_PORT_CABLE_STATE_OK,
    _SHR_PORT_CABLE_STATE_OPEN,
    _SHR_PORT_CABLE_STATE_SHORT,
    _SHR_PORT_CABLE_STATE_OPENSHORT,
    _SHR_PORT_CABLE_STATE_CROSSTALK,
    _SHR_PORT_CABLE_STATE_UNKNOWN,
    _SHR_PORT_CABLE_STATE_COUNT		/* last, as always */
} _shr_port_cable_state_t;

#define	_SHR_PORT_CABLE_STATE_NAMES_INITIALIZER { \
	"Ok", \
	"Open", \
	"Short", \
	"Open/Short", \
	"Crosstalk", \
	"Unknown", \
	}

/*
 * Defines:
 *	_shr_port_cable_diag_t
 * Purpose:
 *	Shared definition of port (phy) cable diag status structure.
 *	Used both in soc/phy layer and in bcm layer.
 */
typedef struct _shr_port_cable_diag_s {
    _shr_port_cable_state_t	state;		/* state of all pairs */
    int				npairs;		/* pair_* elements valid */
    _shr_port_cable_state_t	pair_state[4];	/* pair state */
    int				pair_len[4];	/* pair length in metres */
    int				fuzz_len;	/* len values +/- this */
} _shr_port_cable_diag_t;

/*
 * Defines:
 *     _SHR_PORT_PHY_CONTROL_*
 * Purpose:
 *     PHY specific control settings
 */
typedef enum _shr_port_phy_control_e {
    _SHR_PORT_PHY_CONTROL_WAN,
    _SHR_PORT_PHY_CONTROL_PREEMPHASIS,
    _SHR_PORT_PHY_CONTROL_DRIVER_CURRENT,
    _SHR_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT
} _shr_port_phy_control_t;

/*
 * Defines:
 *     _SHR_PORT_PRBS_POLYNOMIAL_*
 * Purpose:
 *     PRBS polynomial type
 */
typedef enum _shr_port_prbs_polynomial_e {
    _SHR_PORT_PRBS_POLYNOMIAL_X7_X6_1,
    _SHR_PORT_PRBS_POLYNOMIAL_X15_X14_1
} _shr_port_prbs_polynomial_t;

#endif	/* !_SHR_PORT_H */
