/*
 * $Id: dma.h,v 1.28 Broadcom SDK $
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
 * File:        socdma.h
 * Purpose:     Maps out structures used for DMA operations and
 *              exports routines.
 */

#ifndef _SOC_DMA_H
#define _SOC_DMA_H

#include <sal/types.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <sal/core/spl.h>

#include <shared/types.h>

#include <soc/types.h>
#include <soc/cmic.h>
#include <soc/dcb.h>

#define SOC_DMA_ROUND_LEN(x)    (((x) + 3) & ~3)

/* Type for DMA channel number */

typedef _shr_dma_chan_t    dma_chan_t;

/*
 * Defines:
 *      SOC_DMA_DV_FREE_CNT/SOC_DMA_DV_FREE_SIZE
 * Purpose:
 *      Defines the number of free DV's that the DMA code will cache
 *      to avoid calling alloc/free routines. FREE_CNT is number of 
 *      dv_t's, and FREE_SIZE is the number of DCBs in the DV. 
 * Notes:
 *      Allocation requests for DV's with < FREE_SIZE dcbs MAY result in 
 *      an allocation of a DV with FREE_SIZE dcbs. Allocations with 
 *      > FREE_SIZE will always result in calls to memory allocation
 *      routines.
 */

#define SOC_DMA_DV_FREE_CNT     16
#define SOC_DMA_DV_FREE_SIZE    8

#define SOC_DV_PKTS_MAX         16      /* Determines alloc size of
                                           DMA'able data in DVs */

typedef int8    dvt_t;                  /* DV Type definition */

#define         DV_NONE         0       /* Disable/Invalid */
#define         DV_TX           1       /* Transmit data */
#define         DV_RX           2       /* Receive data  */

#define SOC_DMA_DV_TX_FREE_SIZE 3
#define SOC_DMA_DV_RX_FREE_SIZE 1

/*
 * Typedef:
 *      soc_tx_param_t
 * Purpose:
 *      Strata XGS hardware specific info related to packet TX;
 *      Will be used as a vehicle for passing information to
 *      the DCB-Add-TX operation.
 *      Information is per packet.  The data buffer pointer and
 *      byte count is passed separately.
 */

typedef struct soc_tx_param_s {
    uint8             cos;          /* The local COS# to use */
    uint8             prio_int;     /* Internal priority of the packet */
    uint8             src_port;     /* Header/tag ONLY.  Use rx_port below */
    uint8             src_mod;
    uint8             dest_port;    /* Header/tag ONLY.  Use tx_pbmp below */
    uint8             dest_mod;
    uint8             opcode;
    uint8             pfm;          /* see BCM_PORT_PFM_* */

    soc_pbmp_t        tx_pbmp;      /* Target ports */
    soc_pbmp_t        tx_upbmp;     /* Untagged ports */
    soc_pbmp_t        tx_l3pbmp;    /* L3 ports */

    /* Uses SOC_DMA and SOC_DMA_F flags from below */
    uint32            flags;

#if 0 /* Not yet */
    /* Representations for HiGig header data (12 bytes) */
    union {
        uint8         higig8[12];
        uint32        higig32[3];
        /* Add higig_hdr_t here */
    } higig_u;

/* Accessors for the HiGig header in the soc_tx_param_t structure */
#define SOC_TXP_HIGIG8(data)     ((data).higig_u.higig8)
#define SOC_TXP_HIGIG32(data)    ((data).higig_u.higig32)
#define SOC_TXP_HG_HDR(data)     ((data).higig_u.hg_hdr)

#endif

    /* Other new headers/tags may be added here */
} soc_tx_param_t;

/*
 * Typedef:
 *      dv_t
 * Purpose:
 *      Maps out the I/O Vec structure used to pass DMA chains to the
 *      the SOC DMA routines "DMA I/O Vector".
 * Notes:
 *      To start a DMA request, the caller must fill in:
 *              dv_op: operation to perform (DV_RX or DV_TX).
 *              dv_flags: Set DV_F_NOTIFY_DSC for descriptor done callout
 *                        Set DV_F_NOTIFY_CHN for chain done callout
 *                        Set DV_F_WAIT to suspend in dma driver
 *              dv_valid: # valid DCB entries (this field is initialized
 *                      by soc_dma_dv_alloc, and set properly if
 *                      soc_dma_add_dcb is called to build chain).
 *              dv_done_chain: NULL if no callout for chain done, or the
 *                      address of the routine to call when chain done
 *                      is seen. It is called with 2 parameters, the
 *                      unit # and a pointer to the DV chain done has been
 *                      seen on.
 *              dv_done_desc: NULL for synchronous call, or the address of
 *                      the function to call on descriptor done. The function
 *                      is with 3 parameters, the unit #, a pointer to
 *                      the DV structure, and a pointer to the DCB completed.
 *                      One call is made for EVERY DCB, and only if the
 *                      DCB is DONE.
 *              dv_done_packet: NULL if no callout for packet done, or the
 *                      address of the routine to call when packet done
 *                      is seen. It has the same prototype as dv_done_desc.
 *              dv_public1 - 4: Not used by DMA routines,
 *                      for use by caller.
 *
 *     Scatter/gather is implemented through multiple DCBs pointing to
 *     different buffers with the S/G bit set.  End of S/G chain (end of
 *     packet) is indicated by having S/G bit clear in the DCB.
 *
 *     Chains of packets can be associated with a single DV.  This
 *     keeps the HW busy DMA'ing packets even as interrupts are
 *     processed.  DVs can be chained (a software construction)
 *     which will start a new DV from this file rather than calling
 *     back.  This is not done much in our code.
 */

typedef struct dv_s {
    struct dv_s *dv_next,               /* Queue pointers if required */
                *dv_chain;              /* Pointer to next DV in chain */
    int         dv_unit;                /* Unit dv is allocated on */
    uint32      dv_magic;               /* Used to indicate valid */
    dvt_t       dv_op;                  /* Operation to be performed */
    dma_chan_t  dv_channel;             /* Channel queued on */
    int         dv_flags;               /* Flags for operation */
    /* Fix soc_dma_dv_reset if you add flags */
#   define      DV_F_NOTIFY_DSC         0x01    /* Notify on dsc done */
#   define      DV_F_NOTIFY_CHN         0x02    /* Notify on chain done */
#   define      DV_F_COMBINE_DCB        0x04    /* Combine DCB where poss. */
#   define      DV_F_NEEDS_REFILL       0x10    /* Needs to be refilled */
    int         dv_cnt;                 /* # descriptors allocated */
    int         dv_vcnt;                /* # descriptors valid */
    int         dv_dcnt;                /* # descriptors done */
    void        (*dv_done_chain)(int u, struct dv_s *dv_chain);
    void        (*dv_done_desc)(int u, struct dv_s *dv, dcb_t *dcb);
    void        (*dv_done_packet)(int u, struct dv_s *dv, dcb_t *dcb);
    any_t       dv_public1;             /* For caller */
    any_t       dv_public2;             /* For caller */
    any_t       dv_public3;             /* For caller */
    any_t       dv_public4;             /* For caller */

    /*
     * Information for SOC_TX_PKT_PROPERTIES.
     * Normally, packets are completely independent across a DMA TX chain.
     * In order to program the cmic dma tx register effeciently, the data
     * in soc_tx_param must be consistent for all packets in the chain.
     */
    soc_tx_param_t   tx_param;

    /*
     * Buffer for gather-inserted data.  Possibly includes:
     *     HiGig hdr     (12 bytes)
     *     HiGig2 hdr    (16 bytes) (HG2/SL mutually exclusive)
     *     SL tag        (4 bytes)
     *     VLAN tag      (4 bytes)
     *     Dbl VLAN tag  (4 bytes)
     */
    uint8       *dv_dmabuf;

#define SOC_DV_DMABUF_SIZE (SOC_DV_PKTS_MAX * 24)
#define SOC_DV_HG_HDR(dv, i) (&((dv)->dv_dmabuf[SOC_DV_PKTS_MAX*i+0]))
#define SOC_DV_SL_TAG(dv, i) (&((dv)->dv_dmabuf[SOC_DV_PKTS_MAX*i+12]))
#define SOC_DV_VLAN_TAG(dv, i) (&((dv)->dv_dmabuf[SOC_DV_PKTS_MAX*i+16]))
#define SOC_DV_VLAN_TAG2(dv, i) (&((dv)->dv_dmabuf[SOC_DV_PKTS_MAX*i+20]))

    dcb_t       *dv_dcb;
} dv_t;

/*
 * Typedef:
 *      sdc_t (SOC DMA Control)
 * Purpose:
 *      Each DMA channel on each SOC has one of these structures to
 *      track the currently active or queued operations.
 */

typedef struct sdc_s {
    dma_chan_t  sc_channel;             /* Channel # for reverse lookup */
    dvt_t       sc_type;                /* DV type that we accept */
    uint8       sc_flags;               /* See SDMA_CONFIG_XXX */
    dv_t        *sc_q;                  /* Request queue head */
    dv_t        *sc_q_tail;             /* Request queue tail */
    dv_t        *sc_dv_active;          /* Pointer to individual active DV */
    int         sc_q_cnt;               /* # requests queued + active */
} sdc_t;

/* Do not alter ext_dcb flag below */
#define SET_NOTIFY_CHN_ONLY(flags) do { \
    (flags) |=  DV_F_NOTIFY_CHN; \
    (flags) &= ~DV_F_NOTIFY_DSC; \
} while (0)


/* Try to avoid all other flags */
#define         SOC_DMA_F_PKT_PROP      0x10000000    /* 1 << 28 */

/*
 * Note DMA_F_INTR is NOT a normal flag.
 *    Interrupt mode is the default behavior and is ! POLLED mode.
 */
#define         SOC_DMA_F_INTR          0x00 /* Interrupt Mode */

#define         SOC_DMA_F_MBM           0x01 /* Modify bit MAP */
#define         SOC_DMA_F_POLL          0x02 /* POLL mode */
#define         SOC_DMA_F_TX_DROP       0x04 /* Drop if no ports */
#define         SOC_DMA_F_JOIN          0x08 /* Allow low level DV joins */
#define         SOC_DMA_F_DEFAULT       0x10 /* Default channel for type */
#define         SOC_DMA_F_CLR_CHN_DONE  0x20 /* Clear Chain-done on start */
#define         SOC_DMA_F_INTR_ON_DESC  0x40 /* Interrupt per descriptor */

extern int      soc_dma_init(int unit);
extern int      soc_dma_attach(int unit, int reset);
extern int      soc_dma_detach(int unit);

extern int      soc_dma_chan_config(int unit, dma_chan_t chan, dvt_t type, 
                                    uint32 flags);
extern dvt_t    soc_dma_chan_dvt_get(int unit, dma_chan_t chan);

extern dv_t     *soc_dma_dv_alloc(int unit, dvt_t, int cnt);
extern void     soc_dma_dv_free(int unit, dv_t *);
extern void     soc_dma_dv_reset(dvt_t, dv_t *);
extern int      soc_dma_dv_join(dv_t *dv_list, dv_t *dv_add);

extern int      soc_dma_desc_add(dv_t *, sal_vaddr_t, uint16, pbmp_t, 
                                 pbmp_t, pbmp_t, uint32 flags, uint32 *hgh);

#define         SOC_DMA_COS(_x)         ((_x) << 0)
#define         SOC_DMA_COS_GET(_x)     (((_x) >> 0) & 7)
#define         SOC_DMA_CRC_REGEN       (1 << 3)
#define         SOC_DMA_CRC_GET(_x)     (((_x) >> 3) & 1)
#define         SOC_DMA_RLD             (1 << 4)
#define         SOC_DMA_HG              (1 << 20)
#define         SOC_DMA_STATS           (1 << 21)
#define         SOC_DMA_PURGE           (1 << 22)

/* 
 * In current type 3 descriptor, DMOD is 5 bits; DPORT is 4 bits.  
 * TGID/PORT indicator is 1 bit, (we ignore it for now).
 *
 * For now we use 5 bits for mod, 6 bits for port (for tucana) 
 * and 1 for trunk indicator
 */

#define _SDP_MSK 0x3f           /* 10:5 */
#define _SDP_S 5                /* 10:5 */
#define _SDM_MSK 0x1f           /* 15:11 */
#define _SDM_S 11               /* 15:11 */
#define _SDT_MSK 0x1            /* 16:16 */
#define _SDT_S 16               /* 16:16 */
#define _SMHOP_MSK 0x7          /* 19:17 */
#define _SMHOP_S 17             /* 19:17 */

/* 
 * type 9 descriptor, Higig, stats, purge bits
 */

#define _SHG_MSK 0x1            /* 20:20 */
#define _SHG_S 20               /* 20:20 */
#define _SSTATS_MSK 0x1         /* 21:21 */
#define _SSTATS_S 21            /* 21:21 */
#define _SPURGE_MSK 0x1         /* 22:22 */
#define _SPURGE_S 22            /* 22:22 */

#define SOC_DMA_DPORT_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SDP_S, _SDP_MSK)
#define SOC_DMA_DPORT_SET(flags, val)  \
            SOC_SM_FLAGS_SET(flags, val, _SDP_S, _SDP_MSK)
       
#define SOC_DMA_DMOD_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SDM_S, _SDM_MSK)
#define SOC_DMA_DMOD_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SDM_S, _SDM_MSK)

#define SOC_DMA_DTGID_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SDT_S, _SDT_MSK)
#define SOC_DMA_DTGID_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SDT_S, _SDT_MSK)

#define SOC_DMA_MHOP_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SMHOP_S, _SMHOP_MSK)
#define SOC_DMA_MHOP_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SMHOP_S, _SMHOP_MSK)

#define SOC_DMA_HG_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SHG_S, _SHG_MSK)
#define SOC_DMA_HG_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SHG_S, _SHG_MSK)

#define SOC_DMA_STATS_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SSTATS_S, _SSTATS_MSK)
#define SOC_DMA_STATS_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SSTATS_S, _SSTATS_MSK)

#define SOC_DMA_PURGE_GET(flags) \
            SOC_SM_FLAGS_GET(flags, _SPURGE_S, _SPURGE_MSK)
#define SOC_DMA_PURGE_SET(flags, val) \
            SOC_SM_FLAGS_SET(flags, val, _SPURGE_S, _SPURGE_MSK)

extern int      soc_dma_rld_desc_add(dv_t *, sal_vaddr_t);
extern void     soc_dma_desc_end_packet(dv_t *);

extern int      soc_dma_start(int unit, dma_chan_t, dv_t *);
extern int      soc_dma_abort_dv(int unit, dv_t *);
extern int      soc_dma_abort(int unit);

/* Wait on synchronous send - requires a context */
extern int      soc_dma_wait(int unit, dv_t *dv_chain);
extern int      soc_dma_wait_timeout(int unit, dv_t *dv_chain, int usec);

extern void     soc_dma_higig_dump(int unit, char *pfx, uint8 *addr,
                                   int len, int pkt_len, int *ether_offset);
extern void     soc_dma_ether_dump(int unit, char *pfx, uint8 *addr,
                                   int len, int offset);

extern int      soc_dma_dv_valid(dv_t *dv);
extern void     soc_dma_dump_dv(int unit, char *pfx, dv_t *);
extern void     soc_dma_dump_pkt(int unit, char *pfx, uint8 *addr, int len,
                                 int decode);

/* Interrupt Routines */

extern void     soc_dma_done_desc(int unit, uint32 chan);
extern void     soc_dma_done_chain(int unit, uint32 chan);

#ifdef  BCM_XGS3_SWITCH_SUPPORT
int soc_dma_tx_purge(int unit, dma_chan_t c);
#endif  /* BCM_XGS3_SWITCH_SUPPORT */

/*
 * Simplified API for ROM TX/RX polled mode.
 * See $SDK/doc/dma_rom.txt for more information.
 */

extern int      soc_dma_rom_init(int unit, int max_packet_rx); 
extern int      soc_dma_rom_detach(int unit);
extern dcb_t    *soc_dma_rom_dcb_alloc(int unit, int psize); 
extern int      soc_dma_rom_dcb_free(int unit, dcb_t *dcb); 
extern int      soc_dma_rom_tx_start(int unit, dcb_t *dcb); 
extern int      soc_dma_rom_tx_poll(int unit, int *done); 
extern int      soc_dma_rom_tx_abort(int unit); 
extern int      soc_dma_rom_rx_poll(int unit, dcb_t **dcb); 

/****************************************************************
 * BROADCOM tag for ROBO CHIPS
 ****************************************************************/
typedef union {
    uint32  brcm_val;   /* Raw 32-bit value */
    struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
        _cnt:14,    /* frame octet count */
        :9,     /* reserved */
        _src_portid:6;  /* source port id */
#else /* LE_HOST */
    uint32  _src_portid:6,  /* source port id */
        :9,     /* reserved */
        _cnt:14,    /* frame octet count */
            _op:3;      /* opcode of this frame */
#endif 
    } brcm_ucast;
/* define for common usage, before knowing what real type is */
#define    brcm_op              brcm_ucast._op
#define    brcm_cnt             brcm_ucast._cnt
#define    brcm_src_portid      brcm_ucast._src_portid
    struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
            _cnt:14,    /* frame octet count */
        :10,        /* reserved */
        _src_portid:5;  /* source port id */
#else /* LE_HOST */
    uint32  _src_portid:5,  /* source port id */
        :10,        /* reserved */
        _cnt:14,    /* frame octet count */
            _op:3;      /* opcode of this frame */
#endif
    } brcm_mcast;
    struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
        :24,        /* reserved */
        _dst_portid:5;  /* destination port id */
#else /* LE_HOST */
    uint32  _dst_portid:5,  /* destination port id */
        :24,        /* reserved */
            _op:3;      /* opcode of this frame */
#endif
    } brcm_egr_dir;
#define    brcm_dst_portid      brcm_egr_dir._dst_portid
    struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
        :24,        /* reserved */
        _src_portid:5;  /* source port id */
#else /* LE_HOST */
    uint32  _src_portid:5,  /* source port id */
        :24,        /* reserved */
            _op:3;      /* opcode of this frame */
#endif
    } brcm_igr_dir;
/* bcm5348/5347 */
      struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
        :2,     /* reserved */
        _dst_pbmp:27;   /* destination pbmp */
#else /* LE_HOST */
    uint32  _dst_pbmp:27,   /* destination pbmp */
        :2,     /* reserved */
            _op:3;      /* opcode of this frame */
#endif
    } brcm_multi_port_lo;
/* bcm5348/5347 */
      struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
        _dst_pbmp:29;   /* destination pbmp */
#else /* LE_HOST */
    uint32  _dst_pbmp:29,   /* destination pbmp */
            _op:3;      /* opcode of this frame */
#endif
    } brcm_multi_port_hi;
#define    brcm_dst_pbmp      brcm_multi_port_hi._dst_pbmp
/* for bcm5395 */
    struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
           _tc:3,                   /* traffic class */
           _te:2,                   /* tag enforement */
           _ts:1,                   /* time stamp request */
        _dst_pbmp:23;   /* destination pbmp */
#else /* LE_HOST */
    uint32  _dst_pbmp:23,   /* destination pbmp */
            _ts:1,            /* time stamp request */
            _te:2,            /* tag enforement */
            _tc:3,            /* traffic class */
            _op:3;      /* opcode of this frame */
#endif
    } brcm_multi_port_tag_stamp;
#define    brcm_tc  brcm_multi_port_tag_stamp._tc
#define    brcm_te  brcm_multi_port_tag_stamp._te
#define    brcm_ts  brcm_multi_port_tag_stamp._ts

/* for bcm5395 IMP egress packet transfer*/
    struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
        _cnt:14,    /* frame octet count */
        _reason:7, /* reason code */
        _tc:3, /* traffic class */
        _src_portid:5;  /* source port id */
#else /* LE_HOST */
    uint32  _src_portid:5,  /* source port id */
        _tc:3, /* traffic class */
        _reason:7, /* reason code */
        _cnt:14,    /* frame octet count */
            _op:3;      /* opcode of this frame */
#endif 
    } brcm_5395_imp_egress_tag;
/* define for common usage, before knowing what real type is */
#define    brcm_5395_reason              brcm_5395_imp_egress_tag._reason
#define    brcm_5395_tc             brcm_5395_imp_egress_tag._tc


/* for bcm53242 */
      struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
              _tq:2,                    /* traffic queue */
              _te:2,                    /* tag enforement */
        _dst_pbmp:25;   /* destination pbmp */
#else /* LE_HOST */
    uint32  _dst_pbmp:25,   /* destination pbmp */
               _te:2,                   /* tag enforement */
               _tq:2,                   /* traffic queue */
            _op:3;      /* opcode of this frame */
#endif
    } brcm_tq_te_bmp;
#define     brcm_te_53242   brcm_tq_te_bmp._te
#define     brcm_tq_53242   brcm_tq_te_bmp._tq
/* for bcm53242 IMP egress packet transfer*/
    struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
        _cnt:12,    /* frame octet count */
        _cos:2, /* COS queue */
        :1,     /* reserved */
        _reason:6, /* reason code */
        _ermon:2,
        _src_portid:6;  /* source port id */
#else /* LE_HOST */
    uint32  _src_portid:6,  /* source port id */
        _ermon:2,
        _reason:6, /* reason code */
        :1,     /* reserved */
        _cos:2, /* COS queue */
        _cnt:12,    /* frame octet count */
            _op:3;      /* opcode of this frame */
#endif 
    } brcm_53242_imp_egress_tag;
/* define for common usage, before knowing what real type is */
#define    brcm_53242_reason              brcm_53242_imp_egress_tag._reason
#define    brcm_53242_cosq             brcm_53242_imp_egress_tag._cos

/* for bcm53115 */
    struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
           _tc:3,                   /* traffic class */
           _te:2,                   /* tag enforement */
           _ts:1,                   /* time stamp request */
           :16,                       /* reserved */
           _dst_pbmp:7;   /* destination pbmp */
#else /* LE_HOST */
    uint32  _dst_pbmp:7,   /* destination pbmp */
           :16,                       /* reserved */
           _ts:1,            /* time stamp request */
           _te:2,            /* tag enforement */
           _tc:3,            /* traffic class */
           _op:3;      /* opcode of this frame */
#endif
    } brcm_53115_multi_port_tag_stamp;
#define    brcm_tc_53115  brcm_53115_multi_port_tag_stamp._tc
#define    brcm_te_53115  brcm_53115_multi_port_tag_stamp._te
#define    brcm_ts_53115  brcm_53115_multi_port_tag_stamp._ts

/* for bcm53115 IMP egress packet transfer*/
    struct {
#ifdef  BE_HOST
    uint32  _op:3,      /* opcode of this frame */
        :5,     /* reserved */
        _classification_id:8,    /* classification id */
        _reason:8, /* reason code */
        _tc:3, /* traffic class */
        _src_portid:5;  /* source port id */
#else /* LE_HOST */
    uint32  _src_portid:5,  /* source port id */
        _tc:3, /* traffic class */
        _reason:8, /* reason code */
        _classification_id:8,    /* classification id */
        :5,     /* reserved */
            _op:3;      /* opcode of this frame */
#endif 
    } brcm_53115_imp_egress_tag;
/* define for common usage, before knowing what real type is */
#define    brcm_53115_reason              brcm_53115_imp_egress_tag._reason
#define    brcm_53115_tc             brcm_53115_imp_egress_tag._tc

} brcm_t;           /* Control bit definition */


/*
 * CPU Opcodes
 */

#define BRCM_OP_UCAST   0x00        /* Normal unicast frame */
#define BRCM_OP_MCAST   0x01        /* Normal multicast frame */
#define BRCM_OP_EGR_DIR 0x02        /* Egress directed frame */
#define BRCM_OP_IGR_IDR 0x03        /* Ingress directed frame */
#define   BRCM_OP_MULTI_PORT    0x3     /* Multicast Egress direct for bcm5324 */
#define   BRCM_OP_MULTI_PORT_24_52    0x4     /* Multicast Egress direct for bcm5348 */
#define   BRCM_OP_MULTI_PORT_LS    0x4     /* Multicast Egress direct for bcm53242 */
#define   BRCM_OP_MULTI_PORT_IMP_HS    0x5     /* Multicast Egress direct for bcm53242 */

/*
 * Typedef:
 *  robo_dv_t
 * Purpose:
 *  Maps out the I/O Vec structure used to pass DMA chains to the
 *  the SOC DMA routines "DMA I/O Vector".
 * Notes:
 *  To start a DMA request, the caller must fill in:
 *              dv_op: operation to perform (DV_RX or DV_TX).
 *              dv_done_chain: NULL if no callout for chain done, or the
 *                      address of the routine to call when chain done
 *                      is seen. It is called with 2 parameters, the
 *                      unit # and a pointer to the DV chain done has been
 *                      seen on.
 *              dv_done_desc: NULL for synchronous call, or the address of
 *                      the function to call on descriptor done. The function
 *                      is with 3 parameters, the unit #, a pointer to
 *                      the DV structure, and a pointer to the DCB completed.
 *                      One call is made for EVERY DCB, and only if the
 *                      DCB is DONE.
 *              dv_done_packet: NULL if no callout for packet done, or the
 *                      address of the routine to call when packet done
 *                      is seen. It has the same prototype as dv_done_desc.
 *              dv_public1 - 4: Not used by DMA routines,
 *                      for use by caller.
 *
 *     Scatter/gather is implemented through multiple DCBs.
 *
 *     Chains of packets can be associated with a single DV.  This
 *     keeps the HW busy DMA'ing packets even as interrupts are
 *     processed.  DVs can be chained (a software construction)
 *     which will start a new DV from this file rather than calling
 *     back.  This is not done much in our code.
 */

typedef struct robo_dv_s {
    struct robo_dv_s *dv_next;               /* Queue pointers if required */
    int         dv_unit;                /* Unit dv is allocated on */
    uint32      dv_magic;               /* Used to indicate valid */
    dvt_t       dv_op;                  /* Operation to be performed */
    int         dv_flags;               /* Flags for operation */
    int16       dv_length;              /* Total actual length of dv,
                       including brcm tag */
    int16       dv_cnt;                 /* # descriptors allocated */
    int16       dv_vcnt;                /* # descriptors valid */
    int16       dv_dcnt;                /* # descriptors done */
    brcm_t      dv_tag;                 /* DMA control word */
    void        (*dv_done_chain)(int u, struct robo_dv_s *dv_chain);
    void        (*dv_done_desc)(int u, struct robo_dv_s *dv, robo_dcb_t *dcb);
    void        (*dv_done_packet)(int u, struct robo_dv_s*dv, robo_dcb_t *dcb);
    any_t       dv_public1;             /* For caller */
    any_t       dv_public2;             /* For caller */
    any_t       dv_public3;             /* For caller */
    any_t       dv_public4;             /* For caller */
    sal_vaddr_t dv_dmabufhdr;           /* DA SA BRCM_type BRCM_tag */
#define SOC_ROBO_DV_DEST_MAC(dv)             (&(((uint8 *)(dv)->dv_dmabufhdr)[0]))
#define SOC_ROBO_DV_SRC_MAC(dv)              (&(((uint8 *)(dv)->dv_dmabufhdr)[6]))
#define SOC_ROBO_DV_BRCM_TAG(dv)             (&(((uint8 *)(dv)->dv_dmabufhdr)[12]))
#define SOC_ROBO_DV_VLAN_TAG(dv)             (&(((uint8 *)(dv)->dv_dmabufhdr)[18]))
/* 4 bytes BRCM TAG for BCM53115 */
#define SOC_ROBO_DV_VLAN_TAG_53115(dv)             (&(((uint8 *)(dv)->dv_dmabufhdr)[16]))
    sal_vaddr_t dv_dmabufcrc;           /* inner_FCS outer_FCS */
#define SOC_DMA_TX_HDR    22            /* 2*MAC_ADDR(6) + BRCM TAG(2+4) +
                     * VLAN TAG(4, if need)
                     */
#define SOC_DMA_TX_HDR_53115    20            /* 2*MAC_ADDR(6) + BRCM TAG(4) +
                     * VLAN TAG(4, if need)
                     */
#define SOC_DMA_TX_CRC    8             /* Two CRC append */
    robo_dcb_t       *dv_dcb;                /* linked list head of dcbs */
} robo_dv_t;

#define dv_brcm_tag          dv_tag.brcm_val

/* define for common usage, before knowing what real type is */
#define dv_opcode            dv_tag.brcm_op
#define dv_mir               dv_tag.brcm_mir
#define dv_mo                dv_tag.brcm_mo
#define dv_len               dv_tag.brcm_cnt
#define dv_dst_devid         dv_tag.brcm_dst_devid
#define dv_src_devid         dv_tag.brcm_src_devid
#define dv_dst_portid        dv_tag.brcm_dst_portid
#define dv_src_portid        dv_tag.brcm_src_portid
#define dv_dst_pbmp         dv_tag.brcm_dst_pbmp
#define dv_dst_pbmp_53242   dv_tag.brcm_dst_pbmp_53242 

#define dv_tc   dv_tag.brcm_tc
#define dv_tc_53242   dv_tag.brcm_tq_53242
#define dv_te   dv_tag.brcm_te
#define dv_te_53242 dv_tag.brcm_te_53242
#define dv_ts   dv_tag.brcm_ts

#define dv_reason_53242 dv_tag.brcm_53242_reason
#define dv_cosq_53242 dv_tag.brcm_53242_cosq

#define dv_reason_5395 dv_tag.brcm_5395_reason
#define dv_tc_5395 dv_tag.brcm_5395_tc

#define dv_reason_53115 dv_tag.brcm_53115_reason
#define dv_tc_53115 dv_tag.brcm_tc_53115
#define dv_te_53115   dv_tag.brcm_te_53115
#define dv_ts_53115   dv_tag.brcm_ts_53115

#define TAG_NO_ENFORCEMENT  0
#define UNTAG_ENFORCEMENT   1
#define TAG_ENFORCEMENT         2

extern int      soc_robo_dma_attach(int unit);
extern int      soc_robo_dma_detach(int unit);
extern int      soc_robo_dma_init(int unit);
extern int      soc_robo_dma_desc_add(robo_dv_t *dv, sal_vaddr_t addr, uint16 cnt); 
extern robo_dv_t     *soc_robo_dma_dv_alloc(int unit, dvt_t op, int cnt);
extern int      soc_robo_dma_dv_join(robo_dv_t *dv_list, robo_dv_t *dv_add);
extern void     soc_robo_dma_dv_reset(dvt_t, robo_dv_t *);
extern void     soc_robo_dma_dv_free(int unit, robo_dv_t *dv);
extern int      soc_robo_dma_start(int unit, robo_dv_t *dv_chain);
extern int      soc_robo_dma_rxstop(int unit);
extern int      soc_robo_dma_rxenable(int unit);


/* Wait on synchronous send - requires a context */
extern int      soc_robo_dma_wait(int unit, robo_dv_t *dv_chain);
extern int      soc_robo_dma_wait_timeout(int unit, robo_dv_t *dv_chain, int usec);

extern int      soc_robo_dma_dv_valid(robo_dv_t *dv);
extern void     soc_robo_dma_dump_dv(int unit, char *pfx, robo_dv_t *);
extern void     soc_robo_dma_dump_pkt(int unit, char *pfx, uint8 *addr, int len);
extern void     soc_robo_dma_ether_dump(int unit, char *pfx, uint8 *addr,
                                   int len, int offset);

extern int      et_soc_attach(int unit);
extern void     et_soc_debug(int unit);
extern int      et_soc_open(int unit);
extern int      et_soc_close(int unit);
extern int      et_soc_start(int unit, robo_dv_t *dv_chain);
extern int      et_soc_rx_chain(int unit, robo_dv_t *dv_chain);
extern robo_dv_t     *et_soc_rx_chain_get(int unit);
extern void     et_soc_rxmon_on(int unit);
extern void     et_soc_rxmon_off(int unit);

extern void soc_robo_dma_reinit(int unit);

#endif  /* !_SOC_DMA_H */
