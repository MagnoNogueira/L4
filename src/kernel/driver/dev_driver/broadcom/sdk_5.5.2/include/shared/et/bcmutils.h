/*
    Copyright 2001, Broadcom Corporation
    All Rights Reserved.

    This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
    the contents of this file may not be disclosed to third parties, copied or
    duplicated in any form, in whole or in part, without the prior written
    permission of Broadcom Corporation.
*/
/*
 * Misc useful os-independent macros and functions.
 *
 * Copyright(c) 2001 Broadcom Corporation
 * $Id: bcmutils.h,v 1.1 Broadcom SDK $
 */

#ifndef	_bcmutils_h_
#define	_bcmutils_h_

#ifndef MIN
#define	MIN(a, b)		(((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define	MAX(a, b)		(((a)>(b))?(a):(b))
#endif

#define CEIL(x, y)		(((x) + ((y)-1)) / (y))
#define	ROUNDUP(x, y)		((((ulong)(x)+((y)-1))/(y))*(y))
#define	ISALIGNED(a, x)		(((uint)(a) & ((x)-1)) == 0)
#define	ISPOWEROF2(x)		((((x)-1)&(x))==0)
#define	OFFSETOF(type, member)	((uint) &((type *)0)->member)
#define ARRAYSIZE(a)		(sizeof(a)/sizeof(a[0]))

/* bit map related macros */
#ifndef setbit
#define	NBBY	8	/* 8 bits per byte */
#define	setbit(a,i)	((a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define	clrbit(a,i)	((a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define	isset(a,i)	((a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define	isclr(a,i)	(((a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)
#endif

#define	NBITS(type)	(sizeof (type) * 8)

/*
 * Spin at most 'us' microseconds while 'exp' is true.
 * Caller should explicitly test 'exp' when this completes
 * and take appropriate error action if 'exp' is still true.
 */
#define SPINWAIT(exp, us) { \
	uint countdown = (us) + 9; \
	while ((exp) && (countdown >= 10)) {\
		OSL_DELAY(10); \
		countdown -= 10; \
	} \
}

/* generic osl packet queue */
struct pktq {
	void *head;
	void *tail;
	uint  len;
	uint  maxlen; 
};
#define DEFAULT_QLEN	128

#define	pktq_len(q)		((q)->len)
#define	pktq_avail(q)	((q)->maxlen - (q)->len)
#define	pktq_head(q)	((q)->head)
#define	pktq_full(q)	((q)->len >= (q)->maxlen)

/* crc defines */
#define CRC8_INIT_VALUE  0xff		/* Initial CRC8 checksum value */
#define CRC8_GOOD_VALUE  0x9f		/* Good final CRC8 checksum value */
#define CRC16_INIT_VALUE 0xffff		/* Initial CRC16 checksum value */
#define CRC16_GOOD_VALUE 0xf0b8		/* Good final CRC16 checksum value */
#define CRC32_INIT_VALUE 0xffffffff /* Initial CRC32 checksum value */
#define CRC32_GOOD_VALUE 0xdebb20e3 /* Good final CRC32 checksum value */

/* externs */
extern uint bcm_atoi(char *s);
extern ulong bcm_strtoul(char *cp, char **endp, uint base);
extern void deadbeef(char *p, uint len);
extern void prhex(char *msg, uchar *buf, uint len);
extern void prpkt(char *msg, void *drv, void *p0);
extern uchar *bcm_ether_ntoa(char *ea, char *buf);
extern int bcm_ether_atoe(char *p, char *ea);
extern int bcm_isprint(uchar c);
extern uint8 crc8(uint8 *p, uint nbytes, uint8 crc);
extern uint16 crc16(uint8 *p, uint nbytes, uint16 crc);
extern uint32 crc32(uint8 *p, uint nbytes, uint32 crc);
#ifdef BCMDBG
extern void testcrc32(void);
#endif
extern uint8 *bcm_parse_tlvs(uint8 *buf, int buflen, uint key);
extern void pktqinit(struct pktq *q, int maxlen);
extern void pktenq(struct pktq *q, void *p, bool lifo);
extern void *pktdeq(struct pktq *q);

#ifdef BCMINTERNAL
extern void bcmlog(char *fmt, uint a1, uint a2);
extern void bcmdumplog(uchar *buf, int size);
#else
#define	bcmlog(fmt, a1, a2)
#define	bcmdumplog(buf, size)	*buf = '\0'
#endif
extern uint16 bcmswap16(uint16 val);

#endif	/* _bcmutils_h_ */
