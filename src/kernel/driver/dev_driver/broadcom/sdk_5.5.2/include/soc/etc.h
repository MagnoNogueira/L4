/*
    Copyright 2001, Broadcom Corporation
    All Rights Reserved.
    
    This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
    the contents of this file may not be disclosed to third parties, copied or
    duplicated in any form, in whole or in part, without the prior written
    permission of Broadcom Corporation.
*/
/*
 * Common [OS-independent] header file for
 * Broadcom BCM44XX 10/100Mbps Ethernet Device Driver
 *
 * $Id: etc.h,v 1.3.68.1 Broadcom SDK $
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
 */

#ifndef _etc_soc_h_
#define _etc_soc_h_

#define	MAXMULTILIST	32
#define ET_SOC_WATCHDOG_ENABLE 0

#ifndef ch_t
#define	ch_t void
#endif

struct etc_soc_info;	/* forward declaration */

/* each chip type (4413, 47xx) supports a set of chip-type-specific ops */
struct chops {
    bool (*id)(uint vendor, uint device);		/* return true if match */
    void *(*attach)(struct etc_soc_info *etc, void *dev, void *regs);
    void (*detach)(ch_t *ch);			/* free chip private state */
    void (*reset)(ch_t *ch);			/* chip reset */
    void (*init)(ch_t *ch, bool full);	/* chip init */
    void (*tx)(ch_t *ch, void *p);			/* transmit frame */
    void *(*rx)(ch_t *ch);				/* receive frame */
    void (*rxfill)(ch_t *ch);			/* post dma rx buffers */
    int (*getintrevents)(ch_t *ch);			/* return intr events */
    bool (*errors)(ch_t *ch);			/* handle chip errors */
    void (*intrson)(ch_t *ch);			/* enable chip interrupts */
    void (*intrsoff)(ch_t *ch);			/* disable chip interrupts */
    void (*txreclaim)(ch_t *ch, bool all); /* reclaim transmit resources */
    void (*rxreclaim)(ch_t *ch); /* reclaim receive resources */
    void (*statsupd)(ch_t *ch);			/* update sw stat counters */
    void (*enablepme)(ch_t *ch);			/* enable PME */
    void (*disablepme)(ch_t *ch);			/* disable PME */
    void (*phyreset)(ch_t *ch, uint phyaddr);	/* reset phy */
    uint16 (*phyrd)(ch_t *ch, uint phyaddr, uint reg);	/* read phy register */
    void (*phywr)(ch_t *ch, uint phyaddr, uint reg, uint16 val);	
    /* write phy register */
    void (*dump)(ch_t *ch, char *buf);		/* debugging output */
    void (*longname)(ch_t *ch, char *buf, uint bufsize);	
    /* return descriptive name */
    void (*duplexupd)(ch_t *ch); /* keep mac duplex consistent */
    void (*rxreset)(ch_t *ch);
    void (*rxinit)(ch_t *ch);
    bool (*recover)(ch_t *ch);
};

/*
 * "Common" os-independent software state structure.
 */
typedef struct etc_soc_info {
	void		*et;		/* pointer to os-specific private state */
	uint		unit;		/* device instance number */
	bool		up;		/* interface up and running */
	bool		promisc;	/* promiscuous destination address */
	bool		loopbk;		/* loopback override mode */

	int		forcespeed;	
	/* disable autonegotiation and force speed/duplex */
	uint		advertise; 
	/* control speed/duplex advertised capability bits */
	bool		needautoneg;	/* request restart autonegotiation */
	int		speed;		/* current speed: 10, 100 */
	int		duplex;		/* current duplex: 0=half, 1=full */

	bool		piomode;	/* enable programmed io (!dma) */
	void		*pioactive;	
	/* points to pio packet being transmitted */
	volatile uint	*txavail;	/* dma: # tx descriptors available */

	uint16		vendorid;	/* pci function vendor id */
	uint16		deviceid;	/* pci function device id */
	uint		chip;		/* chip number */
	uint		chiprev;	/* chip revision */

	bool		nicmode;	/* is this core using its own pci i/f */

	struct chops	*chops;		/* pointer to chip-specific opsvec */
	void		*ch;		/* pointer to chip-specific state */

	uint		coreunit;	/* sb chips: chip enet instance # */
	uint		phyaddr;	/* sb chips: mdio 5-bit phy address */
	uint		mdcport;	
	/* sb chips: which mii to use (enet core #) to access our phy */

	struct ether_addr cur_etheraddr; /* our local ethernet address */
	struct ether_addr perm_etheraddr; 
	/* original sprom local ethernet address */

	struct ether_addr multicast[MAXMULTILIST];
	uint		nmulticast;
	bool		allmulti;	/* enable all multicasts */

	bool		linkstate;	/* link integrity state */
	bool		pm_modechange;	/* true if mode change is to due pm */

	uint32		now;		/* elapsed seconds */

	/* chip-maintained plus a few sw-maintained stat counters */
	bcmenetmib_t	mib;		/* mib statistic counters */
	uint32		txframe;	/* transmitted frames */
	uint32		txbyte;		/* transmitted bytes */
	uint32		rxframe;	/* received frames */
	uint32		rxbyte;		/* received bytes */
	uint32		txerror;	/* total tx errors */
	uint32		txnobuf;	/* tx out-of-buffer errors */
	uint32		rxerror;	/* total rx errors */
	uint32		rxnobuf;	/* rx out-of-buffer errors */
	uint32		reset;		/* reset count */
	uint32		dmade;		/* pci descriptor errors */
	uint32		dmada;		/* pci data errors */
	uint32		dmape;		/* descriptor protocol error */
	uint32		rxdmauflo;	/* receive descriptor underflow */
	uint32		rxoflo;		/* receive fifo overflow */
	uint32		txuflo;		/* transmit fifo underflow */
	uint32		rxbadlen;	/* 802.3 len field != read length */
} etc_soc_info_t;

typedef struct et_soc_info {
    etc_soc_info_t	*etc;	/* pointer to common os-independent data */
    int         dev;		/* backpoint to device */
    sal_mutex_t soc_robo_dma_lock;	/* per-device perimeter lock */
#define ET_SOC_DMA_LOCK(s) sal_mutex_take(s->soc_robo_dma_lock, sal_mutex_FOREVER)
#define ET_SOC_DMA_UNLOCK(s)	sal_mutex_give(s->soc_robo_dma_lock)
    sal_mutex_t tx_dma_lock;	/* lock for tx chain */
#define ET_TX_DMA_LOCK(s) sal_mutex_take(s->tx_dma_lock, sal_mutex_FOREVER)
#define ET_TX_DMA_UNLOCK(s)	sal_mutex_give(s->tx_dma_lock)
    sal_mutex_t rx_dma_lock;	/* lock for rx chain */
#define ET_RX_DMA_LOCK(s) sal_mutex_take(s->rx_dma_lock, sal_mutex_FOREVER)
#define ET_RX_DMA_UNLOCK(s)	sal_mutex_give(s->rx_dma_lock)
    robo_dv_t	*txq_head;	/* head of send queue */
    robo_dv_t	*txq_tail;	/* tail of send queue */
    int		txq_cnt;	/* number of pending txq */
    robo_dv_t	*rxq_head;	/* head of receive queue */
    robo_dv_t	*rxq_tail;	/* tail of receive queue */
    int		rxq_cnt;	/* number of pending rxq */
#if ET_SOC_WATCHDOG_ENABLE
    sal_usecs_t	timer;		/* one second watchdog timer */
#define SOC_WD_TIMER		10000000
#endif
    bool        et_soc_init;
    bool        et_soc_intr_pend;
    bool        et_soc_rxmon;
} et_soc_info_t;

/* interrupt event bitvec */
#define	INTR_TX		0x1
#define	INTR_RX		0x2
#define	INTR_ERROR	0x4
#define	INTR_TO		0x8
#define	INTR_NEW	0x10

/* special phyaddr values */
#define	PHY_NOMDC	30		/* phy is really another mac w/no mdc */
#define	PHY_NONE	31		/* no phy present */

/* forcespeed values */
#define	ET_AUTO		-1
#define	ET_10HALF	0
#define	ET_10FULL	1
#define	ET_100HALF	2
#define	ET_100FULL	3

/* a few misc mii/mdio definitions */
#define	CTL_RESET	(1 << 15)	/* reset */
#define	CTL_LOOP	(1 << 14)	/* loopback */
#define	CTL_SPEED	(1 << 13)	/* speed selection 0=10, 1=100 */
#define	CTL_ANENAB	(1 << 12)	/* autonegotiation enable */
#define	CTL_RESTART	(1 << 9)	/* restart autonegotiation */
#define	CTL_DUPLEX	(1 << 8)	/* duplex mode 0=half, 1=full */

#define	ADV_10FULL (1 << 6) /* autonegotiate advertise 10full capability */
#define	ADV_10HALF (1 << 5) /* autonegotiate advertise 10half capability */
#define	ADV_100FULL (1 << 8) /* autonegotiate advertise 100full capability */
#define	ADV_100HALF (1 << 7) /* autonegotiate advertise 100half capability */

/* Link partner ability register. */
#define LPA_SLCT                0x001f  /* Same as advertise selector  */
#define LPA_10HALF              0x0020  /* Can do 10mbps half-duplex   */
#define LPA_10FULL              0x0040  /* Can do 10mbps full-duplex   */
#define LPA_100HALF             0x0080  /* Can do 100mbps half-duplex  */
#define LPA_100FULL             0x0100  /* Can do 100mbps full-duplex  */
#define LPA_100BASE4            0x0200  /* Can do 100mbps 4k packets   */
#define LPA_RESV                0x1c00  /* Unused...                   */
#define LPA_RFAULT              0x2000  /* Link partner faulted        */
#define LPA_LPACK               0x4000  /* Link partner acked us       */
#define LPA_NPAGE               0x8000  /* Next page bit               */

#define LPA_DUPLEX		(LPA_10FULL | LPA_100FULL)
#define LPA_100			(LPA_100FULL | LPA_100HALF | LPA_100BASE4)


#define	STAT_REMFAULT	(1 << 4)	/* remote fault */
#define	STAT_LINK	(1 << 2)	/* link status */
#define	STAT_JAB	(1 << 1)	/* jabber detected */
#define	AUX_FORCED	(1 << 2)	/* forced 10/100 */
#define	AUX_SPEED	(1 << 1)	/* speed 0=10mbps 1=100mbps */
#define	AUX_DUPLEX	(1 << 0)	/* duplex 0=half 1=full */

/* common ioctl definitions */
#define	ETCUP		0
#define	ETCDOWN		1
#define ETCLOOP		2
#define ETCDUMP		3
#define ETCSETMSGLEVEL	4
#define	ETCPROMISC	5
#define	ETCRESV1	6
#define	ETCSPEED	7
#define ETCPHYRD	9
#define ETCPHYWR	10

/* exported prototypes */
extern struct chops *etc_soc_chipmatch(uint vendor, uint device);
extern void *etc_soc_attach(void *et, uint vendor, uint device, uint unit, 
    void *dev, void *regsva);
extern void etc_soc_detach(etc_soc_info_t *etc);
extern void etc_soc_reset(etc_soc_info_t *etc);
extern void etc_soc_debug(etc_soc_info_t *etc);
extern void etc_soc_init(etc_soc_info_t *etc, bool full);
extern void etc_soc_up(etc_soc_info_t *etc);
extern uint etc_soc_down(etc_soc_info_t *etc, int reset);
extern int etc_soc_ioctl(etc_soc_info_t *etc, int cmd, void *arg);
extern void etc_soc_promisc(etc_soc_info_t *etc, uint on);
extern void etc_soc_dump(etc_soc_info_t *etc, uchar *buf, int size);
extern void etc_soc_watchdog(etc_soc_info_t *etc);
extern uint etc_soc_totlen(etc_soc_info_t *etc, void *p);

#endif	/* _etc_soc_h_ */
