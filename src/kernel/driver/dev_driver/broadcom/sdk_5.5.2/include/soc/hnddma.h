/*
    Copyright 2001, Broadcom Corporation
    All Rights Reserved.
    
    This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
    the contents of this file may not be disclosed to third parties, copied or
    duplicated in any form, in whole or in part, without the prior written
    permission of Broadcom Corporation.
*/
/*
 * Generic Broadcom Home Networking Division (HND) DMA engine definitions.
 * This supports the following chips: BCM42xx, 44xx, 47xx .
 *
 * $Id: hnddma.h,v 1.3 Broadcom SDK $
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

#ifndef	_hnddma_soc_h_
#define	_hnddma_soc_h_

/*
 * Each DMA processor consists of a transmit channel and a receive channel.
 */
typedef volatile struct {
	/* transmit channel */
	uint32	xmtcontrol; /* enable, et al */
	uint32	xmtaddr; /* descriptor ring base address (4K aligned) */
	uint32	xmtptr; /* last descriptor posted to chip */
	uint32	xmtstatus; /* current active descriptor, et al */

	/* receive channel */
	uint32	rcvcontrol; /* enable, et al */
	uint32	rcvaddr; /* descriptor ring base address (4K aligned) */
	uint32	rcvptr; /* last descriptor posted to chip */
	uint32	rcvstatus; /* current active descriptor, et al */
} dmaregs_soc_t;

typedef volatile struct {
	/* diag access */
	uint32	fifoaddr;			/* diag address */
	uint32	fifodatalow;			/* low 32bits of data */
	uint32	fifodatahigh;			/* high 32bits of data */
	uint32	pad;				/* reserved */
} dmafifo_soc_t;

/* transmit channel control */
#define	XC_XE		((uint32)1 << 0)	/* transmit enable */
#define	XC_SE		((uint32)1 << 1)	/* transmit suspend request */
#define	XC_LE		((uint32)1 << 2)	/* loopback enable */
#define	XC_FL		((uint32)1 << 4)	/* flush request */

/* transmit descriptor table pointer */
#define	XP_LD_MASK	0xfff			/* last valid descriptor */

/* transmit channel status */
#define	XS_CD_MASK	0x0fff			/* current descriptor pointer */
#define	XS_XS_MASK	0xf000			/* transmit state */
#define	XS_XS_SHIFT	12
#define	XS_XS_DISABLED	0x0000			/* disabled */
#define	XS_XS_ACTIVE	0x1000			/* active */
#define	XS_XS_IDLE	0x2000			/* idle wait */
#define	XS_XS_STOPPED	0x3000			/* stopped */
#define	XS_XS_SUSP	0x4000			/* suspend pending */
#define	XS_XE_MASK	0xf0000			/* transmit errors */
#define	XS_XE_SHIFT	16
#define	XS_XE_NOERR	0x00000			/* no error */
#define	XS_XE_DPE	0x10000			/* descriptor protocol error */
#define	XS_XE_DFU	0x20000			/* data fifo underrun */
#define	XS_XE_BEBR	0x30000			/* bus error on buffer read */
#define	XS_XE_BEDA	0x40000 /* bus error on descriptor access */
#define	XS_FL		((uint32)1 << 20)	/* flushed */

/* receive channel control */
#define	RC_RE		((uint32)1 << 0)	/* receive enable */
#define	RC_RO_MASK	0xfe			/* receive frame offset */
#define	RC_RO_SHIFT	1
#define	RC_FM ((uint32)1 << 8) /* direct fifo receive (pio) mode */

/* receive descriptor table pointer */
#define	RP_LD_MASK	0xfff			/* last valid descriptor */

/* receive channel status */
#define	RS_CD_MASK	0x0fff /* current descriptor pointer */
#define	RS_RS_MASK	0xf000			/* receive state */
#define	RS_RS_SHIFT	12
#define	RS_RS_DISABLED	0x0000			/* disabled */
#define	RS_RS_ACTIVE	0x1000			/* active */
#define	RS_RS_IDLE	0x2000			/* idle wait */
#define	RS_RS_STOPPED	0x3000			/* reserved */
#define	RS_RE_MASK	0xf0000			/* receive errors */
#define	RS_RE_SHIFT	16
#define	RS_RE_NOERR	0x00000			/* no error */
#define	RS_RE_DPE	0x10000			/* descriptor protocol error */
#define	RS_RE_DFO	0x20000			/* data fifo overflow */
#define	RS_RE_BEBW	0x30000			/* bus error on buffer write */
#define	RS_RE_BEDA	0x40000 /* bus error on descriptor access */

/* fifoaddr */
#define	FA_OFF_MASK	0xffff			/* offset */
#define	FA_SEL_MASK	0xf0000			/* select */
#define	FA_SEL_SHIFT	16
#define	FA_SEL_XDD	0x00000			/* transmit dma data */
#define	FA_SEL_XDP	0x10000			/* transmit dma pointers */
#define	FA_SEL_RDD	0x40000			/* receive dma data */
#define	FA_SEL_RDP	0x50000			/* receive dma pointers */
#define	FA_SEL_XFD	0x80000			/* transmit fifo data */
#define	FA_SEL_XFP	0x90000			/* transmit fifo pointers */
#define	FA_SEL_RFD	0xc0000			/* receive fifo data */
#define	FA_SEL_RFP	0xd0000			/* receive fifo pointers */

/*
 * DMA Descriptor
 * Descriptors are only read by the hardware, never written back.
 */
typedef volatile struct {
	uint32	ctrl;		/* misc control bits & bufcount */
	uint32	addr;		/* data buffer address */
} dmadd_soc_t;

/*
 * Each descriptor ring must be 4096byte aligned
 * and fit within a single 4096byte page.
 */
#define	DMAMAXRINGSZ	4096
#define	DMARINGALIGN	4096

/* control flags */
#define	CTRL_BC_MASK	0x1fff			/* buffer byte count */
#define	CTRL_EOT	((uint32)1 << 28)	/* end of descriptor table */
#define	CTRL_IOC	((uint32)1 << 29)	/* interrupt on completion */
#define	CTRL_EOF	((uint32)1 << 30)	/* end of frame */
#define	CTRL_SOF	((uint32)1 << 31)	/* start of frame */

/* control flags in the range [27:20] are core-specific and not defined here */
#define	CTRL_CORE_MASK	0x0ff00000

/* export structure */
typedef volatile struct {
	/* rx error counters */
	uint		rxgiants;	/* rx giant frames */
	uint		rxnobuf;	/* rx out of dma descriptors */
	/* tx error counters */
	uint		txnobuf;	/* tx out of dma descriptors */
} hnddma_soc_t;

#ifndef di_t
#define	di_t	void
#endif

/* externs */
extern void *dma_soc_attach(void *drv, void *dev, char *name, dmaregs_soc_t *dmaregs,
	uint ntxd, uint nrxd, uint rxbufsize, uint nrxpost, uint rxoffset,
	uint ddoffset, uint dataoffset, uint *msg_level);
extern void dma_soc_detach(void *di);
extern void dma_soc_txreset(void *di);
extern void dma_soc_rxreset(void *di);
extern void dma_soc_txinit(void *di);
extern bool dma_soc_txenabled(void *di);
extern void dma_soc_rxinit(void *di);
extern void dma_soc_rxenable(void *di);
extern bool dma_soc_rxenabled(void *di);
extern void dma_soc_txsuspend(void *di);
extern void dma_soc_txresume(void *di);
extern bool dma_soc_txsuspended(void *di);
extern bool dma_soc_txstopped(void *di);
extern bool dma_soc_rxstopped(void *di);
extern int dma_soc_txfast(void *di, void *p, uint32 coreflags);
extern int dma_soc_tx(void *di, void *p, uint32 coreflags);
extern void dma_soc_fifoloopbackenable(void *di);
extern void *dma_soc_rx(void *di);
extern void dma_soc_rxfill(void *di);
extern void dma_soc_rxrecycle(void *di);
extern void dma_soc_txreclaim(void *di, bool forceall);
extern void dma_soc_rxreclaim(void *di);
extern char *dma_soc_dump(void *di, char *buf);
extern uint dma_soc_getvar(void *di, char *name);
extern void *dma_soc_getnexttxp(void *di, bool forceall);
extern void *dma_soc_getnextrxp(void *di, bool forceall);

#endif	/* _hnddma_soc_h_ */
