/*
 * $Id: rx.h,v 1.13.6.3 Broadcom SDK $
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
 * File:        rx.h
 * Purpose:     
 */

#ifndef   _BCM_INT_ROBO_RX_H_
#define   _BCM_INT_ROBO_RX_H_

#include <sal/appl/sal.h>
#include <soc/dma.h>
#include <soc/et_soc.h>
#include <bcm/error.h>
#include <bcm/rx.h>
#include <bcm_int/common/rx.h>

extern int bcm_rx_debug(int unit);

#define BCM_ROBO_RX_CHANNELS        4   /* Max number of rx channels */

/* Some default values */
#define RX_PKT_SIZE_DFLT       (2 * 1024)    /* 2 K packets */
#define RX_PPC_DFLT            1             /* 1 pkts/chain */
#define RX_CHAINS_DFLT         (NRXD-1)      /* # of chains */
#define RX_THREAD_PRI_DFLT     200

/* Could probably be pretty large now */
#define RX_PPC_MAX             SOC_DV_PKTS_MAX

/* Default unit to use for alloc/free */
#define BCM_RX_DFLT_UNIT 0


/* Queue type for packets, one per channel; possibly per COS in the future */
typedef struct rx_queue_s {
    bcm_pkt_t   *head;
    bcm_pkt_t   *tail;
    int          pps;          /* Packets/sec for queue; 0 -> disabled */
    int          burst;        /* Burst for queue */
    int          tokens;       /* Token bucket */
    sal_usecs_t  last_fill;    /* Last time tokens added  */
    int          count;        /* Number of pkts in queue */
    int          max_len;      /* Max number of pkts permitted in queue.
                                  Disabled if <= 0. */
#define RX_Q_MAX_DFLT 1000
    int          tot_pkts;     /* Total pkts through the queue (debug) */
    int          rate_disc;    /* Pkts discarded due to rate limitting */
    int          lock;         /* For locking the queue */
} rx_queue_t;

/* rx_queue_t *q below */
#define _Q_LOCK(q)         (q)->lock = sal_splhi()
#define _Q_UNLOCK(q)       sal_spl((q)->lock)

/* Steal the packets from a queue as a list.  Locking. */
#define _Q_STEAL(q, first_pkt, max)                     \
    do {                                                \
        int _i = 0;                                     \
        bcm_pkt_t *last_pkt;                            \
        bcm_pkt_t *prev_pkt = NULL;                     \
        _Q_LOCK(q);                                     \
        last_pkt = (q)->head;                           \
        (first_pkt) = last_pkt;                         \
        while (_i < (max) && last_pkt != NULL) {        \
            _i++;                                       \
            prev_pkt = last_pkt;                        \
            last_pkt = last_pkt->_next;                 \
        }                                               \
        if (prev_pkt != NULL) { /* Just to be safe */   \
             prev_pkt->_next = NULL; /* Mark list end */\
        }                                               \
        (q)->head = last_pkt;                           \
        if (last_pkt == NULL) { /* No more pkts in q */ \
            (q)->tail = NULL;                           \
        }                                               \
        (q)->count -= _i; /* took _i pkts off q */      \
        _Q_UNLOCK(q);                                   \
    } while (0)
    
#define _Q_STEAL_ALL(q, pkt_list)       \
    do {                            \
        _Q_LOCK(q);                 \
        (pkt_list) = (q)->head;     \
        (q)->head = NULL;           \
        (q)->tail = NULL;           \
/*DBG*/ (q)->count = 0;             \
        _Q_UNLOCK(q);               \
    } while (0)


/* Test if a queue is empty.  Always use count */
#define _Q_EMPTY(q) ((q)->tail == NULL)

/* Enqueue at tail; no locking here (should be in intr context) */
#define _Q_ENQUEUE(q, pkt)                            \
    do {                                              \
        (pkt)->_next = NULL;                           \
        if (_Q_EMPTY(q)) (q)->head = (q)->tail = (pkt); \
        else {                                        \
            (q)->tail->_next = (pkt);                  \
            (q)->tail = (pkt);                        \
        }                                             \
/*DBG*/ (q)->count++;                                 \
        (q)->tot_pkts++;                              \
    } while (0)

#define RX_CHAINS_MAX (NRXD-1)

/*
 * Typedef:
 *      rx_chan_ctl_t
 * Purpose:
 *      Control structure per channel
 * Notes:
 *      Tokens must be signed
 */

typedef struct rx_chan_ctl_s {
    robo_dv_t            *dv[RX_CHAINS_MAX];
    int              dcb_per_pkt;     /* Number DCB/pkt */

    /* Some stats counters */
    int              rpkt;    /* Received packets */
    int              rbyte;   /* Received bytes */
    int              dpkt;    /* Discard packets */
    int              dbyte;   /* Discard bytes */
    int              mem_fail;  /* How many packet allocation errors */
    bcm_pkt_t       *all_pkts;
} rx_chan_ctl_t;

/*
 * Typedef:
 *      rx_ctl_t
 * Purpose:
 *      Control structure per unit for RX
 * Notes:
 *      Tokens must be signed
 */

typedef struct rx_ctl_s {
    int               rc_lock;            /* For SPL locking */

    bcm_rx_cfg_t      user_cfg;           /* User configuration */

    volatile rx_callout_t  *rc_callout;         /* Callout linked list */
    volatile int      hndlr_intr_cnt;     /* Number of interrupt callouts */
    volatile int      hndlr_cnt;          /* Number of non-intr callouts
                                           * Does not include discard.
                                           */
    volatile uint32   chan_running;       /* Bitmap of channels enabled */
    int               cur_chan;           /* For fairness in refill/start */
    rx_chan_ctl_t     chan_ctl[BCM_ROBO_RX_CHANNELS]; /* internal chan ctl */
    rx_queue_t        pkt_queue[BCM_RX_COS];  /* Packets to process/cos */

    /* For global rate control */
    volatile int      tokens;             /* Rate control tokens. */
    sal_usecs_t       last_fill;          /* Last time tokens added  */

    /* Thread control */    
    char	      rx_tname[16];    /* Thread name */
    sal_sem_t         pkt_notify;      /* Semaphor for pkt thrd */
    sal_thread_t      rx_tid;          /* packet thread id */
    int               sleep_cur;       /* Current sleep time */
#define MIN_SLEEP_US 5000              /* Always sleep at least 5 ms */
#define MAX_SLEEP_US 100000            /* For non-rate limitting, .1 sec */
    volatile int      thread_running;  /* Input signal to thread */
    volatile int      thread_exit_complete; /* Output signal from thread */

    sal_mutex_t       rx_mutex;        /* Sync for handler list */

    /* Counters */
    int               restart_errs;
    int               tot_pkts;
    int               pkts_since_start; /* Since last start, how many pkts */
    int               bad_hndlr_rv;
    int               no_hndlr;
    int               not_running;
    int               thrd_not_running;
    int               dcb_errs;
    int               pkts_owned;
    int               next_unit;       /* Next unit with RX started. */
    bcm_cos_queue_t   queue_max;       /* Maximum number of cpu queues. */

    volatile void    *free_list;       /* Deferred free list */
} rx_ctl_t;

/* DV related defines and DV state transition diagram
 *
 * State desriptions:
 *    DV_S_NEEDS_FILL     The DV needs to have its packets checked
 *    DV_S_FILLED         The DV has packets and is ready to be sent
 *                        to the SOC layer.
 *    DV_S_SCHEDULED      The DV could not be started due to rate
 *                        limiting, so it has been scheduled for a DPC
 *    DV_S_ACTIVE         The DV has been handed to the SOC layer and
 *                        is receiving packets.
 *    DV_S_CHN_DONE       A chain done interrupt has occurred for this
 *                        DV.  The SOC layer is done with the DV and it
 *                        now contains packets to process.
 *    DV_S_ERROR          An error has occurred.  The DV will not be
 *                        scheduled until RX is restarted.
 *
 * DVs start in the state NEEDS_FILL.
 *
 *     NEEDS_FILL  -->  FILLED          rx_dv_fill
 *     FILLED      -->  ACTIVE          rx_chain_start
 *     FILLED      -->  SCHEDULED       rx_chain_start_or_schedule
 *     SCHEDULED   -->  ACTIVE          rx_chain_start
 *     ACTIVE      -->  CHN_DONE        rx_done_chain
 *     ACTIVE      -->  NEEDS_FILL      all pkts processed, see below
 *     CHN_DONE    -->  NEEDS_FILL      see below
 *
 * A DV transitions from CHN_DONE or ACITVE to NEEDS_FILL when all
 * its packets have been processed.  This is detected by the
 * pkt_done_cnt of DV_INFO reaching the pkts/chain level.  Note that
 * if all pkts are being handled in the interrupt handler, this may
 * occur before the DV has transitioned to CHN_DONE which occurs in
 * the chain done interrupt handler.
 */

typedef enum dv_states_e {
    DV_S_NEEDS_FILL,          /* Packets are processed, needs refill */
    DV_S_FILLED,              /* DV filled, ready to be started */
    DV_S_SCHEDULED,           /* DV has been scheduled via a DPC */
    DV_S_ACTIVE,              /* DV has been started (given to soc layer) */
    DV_S_CHN_DONE,          /* DV is done, needs service (chain done) */
    DV_S_ERROR                /* DV has seen error */
} dv_states_t;

#define DV_STATE_STRINGS { \
    "Needs Fill",          \
    "Filled",              \
    "Scheduled",           \
    "Active",              \
    "Chain Done",          \
    "Error"}

/* This is all the extra information associated with a DV for RX */
typedef struct rx_dv_info_s {
    volatile dv_states_t state;
    volatile sal_usecs_t sched_time;   /* When was DV scheduled */
    volatile int time_diff;            /* Delta time until started */
    uint8 idx;
    uint8 chan;
    volatile uint8 pkt_done_cnt;
} rx_dv_info_t;

/*
 * DV and DCB related macros
 */
#define _DV_INFO dv_public1.ptr
#define DV_INFO(dv)          (((rx_dv_info_t *)(((robo_dv_t *)dv)->_DV_INFO)))

/*
 * Record both the time that the DV is scheduled and how many
 * usecs in the future to schedule.
 */
#define DV_TIME_DIFF(dv)     (DV_INFO(dv)->time_diff)
#define DV_SCHED_TIME(dv)    (DV_INFO(dv)->sched_time)

/*
 * How much futher into the future to schedule.  If this is <= 0,
 * it indicates the DV is ready.  
 */
#define DV_FUTURE_US(dv, cur_us)                                  \
    (DV_TIME_DIFF(dv) - (int)((cur_us >= DV_SCHED_TIME(dv)) ?     \
     (SAL_USECS_SUB(cur_us, DV_SCHED_TIME(dv))) :                 \
     (SAL_USECS_ADD(cur_us, SAL_USECS_SUB(0xffffffff, DV_SCHED_TIME(dv))))))

#define DV_INDEX(dv)                 (DV_INFO(dv)->idx)
#define DV_STATE(dv)                 (DV_INFO(dv)->state)
#define DV_CHANNEL(dv)               (DV_INFO(dv)->chan)

#define DV_PKT(dv, i) \
    (RX_PKT(dv->dv_unit, DV_CHANNEL(dv), DV_INFO(dv)->idx, i))
#define DV_PKTS_PROCESSED(dv)        (DV_INFO(dv)->pkt_done_cnt)

#define DV_RX_IDX(dv)                ((dv)->dv_public2.u32)
#define DV_RX_IDX_SET(dv, val)       (dv)->dv_public2.u32 = (val)


/****************************************************************
 *
 * The following defines can only be used in bcm/rx.c as they
 * refer to the static variable rx_ctl
 *
 ****************************************************************/

/*
 * RX_LOCK/UNLOCK should only be used to synchronize with
 * interrupt handlers.  Code protected by these calls should
 * only involve memory references.
 */

#define RX_LOCK(unit)        \
    sal_mutex_take(rx_ctl[unit]->rx_mutex, sal_mutex_FOREVER)
#define RX_UNLOCK(unit)      \
    sal_mutex_give(rx_ctl[unit]->rx_mutex)

/****************************************************************
 * Access macros for rx_ctl:
 *     RX_INIT_DONE(unit)            Has init been called?
 *     RX_INIT_CHECK(unit)           Return error if init not done
 *     RX_CHAN_USED(unit, chan)      Has the channel been setup?
 *     RX_CHAN_RUNNING(unit, chan)   Is channel currently running (active)?
 *
 *     RX_PPC(unit)                  Packets/chain
 *     RX_PPS(unit)                  Global pkts/second rate limit
 *     RX_BURST(unit)                Global burst rate (full bucket setting)
 *
 *     RX_CHAN_COS(unit, chan)       Channel's COS bitmap
 *     RX_CHAN_BURST(unit)           Channel's burst rate (full bucket)
 *
 *     RX_DV(unit, chan, i)          Get pointer to i-th dv of channel
 *     RX_DV_PKT(unit, chan, dv_idx, pkt_idx)
 *                                   Pointer to packet for given dv
 *
 ****************************************************************/

/* Access macros for rx_ctl: */
#define RX_INIT_DONE(unit)          (rx_ctl[unit] != NULL)
#define RX_INIT_CHECK(unit)         if (!RX_INIT_DONE(unit)) \
                                       BCM_IF_ERROR_RETURN(bcm_rx_init(unit))
#define RX_CHAN_USED(unit, chan)    (RX_CHAN_CFG(unit, chan).chains != 0)
#define RX_CHAN_RUNNING(unit, chan) (rx_ctl[unit]->chan_running & (1 << chan))

/* Global configuration */
#define RX_PPC(unit)                (rx_ctl[unit]->user_cfg.pkts_per_chain)
#define RX_PPS(unit)                (rx_ctl[unit]->user_cfg.global_pps)
#define RX_BURST(unit)              (rx_ctl[unit]->user_cfg.max_burst)
#define RX_THREAD_PRI(unit)         (rx_ctl[unit]->user_cfg.thread_pri)
#define RX_TOKENS(unit)             (rx_ctl[unit]->tokens)
#define RX_QUEUE_MAX(unit)          (rx_ctl[unit]->queue_max)

/* Channel configuration */
#define RX_CHAN_CFG(unit, chan)     (rx_ctl[unit]->user_cfg.chan_cfg[chan])
#define RX_CHAN_FLAGS(unit, chan)   (RX_CHAN_CFG(unit, chan).flags)
#define RX_CHAINS(unit, chan)       (RX_CHAN_CFG(unit, chan).chains)
#define RX_CRC_STRIP(unit, chan) \
    (RX_CHAN_FLAGS(unit, chan) & BCM_RX_F_CRC_STRIP)
#define RX_VTAG_STRIP(unit, chan) \
    (RX_CHAN_FLAGS(unit, chan) & BCM_RX_F_VTAG_STRIP)
#define RX_PKT_SIZE(unit)     (rx_ctl[unit]->user_cfg.pkt_size)
#define RX_USER_FLAGS(unit)   (rx_ctl[unit]->user_cfg.flags)
#define RX_IGNORE_HG(unit)    (RX_USER_FLAGS(unit) & BCM_RX_F_IGNORE_HGHDR)
#define RX_IGNORE_SL(unit)    (RX_USER_FLAGS(unit) & BCM_RX_F_IGNORE_SLTAG)

/* Per COS rate control */ 
#define RX_QUEUE(unit, cos)         (&(rx_ctl[unit]->pkt_queue[cos]))
#define RX_COS_PPS(unit, cos)       (RX_QUEUE(unit, cos)->pps)
#define RX_COS_BURST(unit, cos)     (RX_QUEUE(unit, cos)->burst)
#define RX_COS_MAX_LEN(unit, cos)   (RX_QUEUE(unit, cos)->max_len)
#define RX_COS_TOKENS(unit, cos)    (RX_QUEUE(unit, cos)->tokens)

#define RX_CHAN_COS(unit, chan)     (RX_CHAN_CFG(unit, chan).cos_bmp)
#define RX_CHAN_BURST(unit, chan)   (RX_CHAINS(unit, chan) * RX_PPC(unit))

/* Internal configuration/control */
#define RX_CHAN_CTL(unit, chan)     (rx_ctl[unit]->chan_ctl[chan])
#define RX_CHAN_PKTS(unit, chan)    (rx_ctl[unit]->chan_ctl[chan].all_pkts)

#define RX_PKT(unit, chan, dv_idx, idx) \
    (&(RX_CHAN_CTL(unit, chan).all_pkts[dv_idx * RX_PPC(unit) + idx]))

/* DV and DCB related macros */
#define RX_DCB_PER_PKT(unit, chan)  (RX_CHAN_CTL(unit, chan).dcb_per_pkt)
#define RX_DCB_PER_DV(unit, chan) \
    (RX_DCB_PER_PKT(unit, chan) * RX_PPC(unit))

#define RX_DV(unit, chan, i) (RX_CHAN_CTL(unit, chan).dv[i])

/* For each channel that is currently running */
#define FOREACH_RUNNING_CHANNEL(unit, chan) \
    for (chan = 0; chan < BCM_ROBO_RX_CHANNELS; chan++) \
        if (RX_CHAN_RUNNING(unit, chan))

/* For each channel that is setup (could be or is active) */
#define FOREACH_SETUP_CHANNEL(unit, chan) \
    for (chan = 0; chan < BCM_ROBO_RX_CHANNELS; chan++) \
        if (RX_CHAN_USED(unit, chan))

#endif /* _BCM_INT_RX_H_ */
