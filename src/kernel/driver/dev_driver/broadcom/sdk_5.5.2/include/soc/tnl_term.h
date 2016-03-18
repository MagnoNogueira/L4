/*
 * $Id: tnl_term.h,v 1.2.6.1 Broadcom SDK $
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
 */

#ifndef _TUNNEL_TERMINATION_H_
#define _TUNNEL_TERMINATION_H_
#include <shared/l3.h>

#define SOC_TNL_TERM_IPV4_ENTRY_WIDTH  (1) /* 1 table entry required     */
                                           /* for IPv4 tunnel terminator.*/
#define SOC_TNL_TERM_IPV6_ENTRY_WIDTH  (4) /* 4 table entries required   */
                                           /* for IPv6 tunnel terminator.*/
#define SOC_TNL_TERM_BLOCK_SIZE   (SOC_TNL_TERM_IPV6_ENTRY_WIDTH)

#define SOC_TNL_TERM_IDX_TO_BLOCK_START(_idx_, _block_start_)    \
       (_block_start_) = (_idx_) - ((_idx_) % SOC_TNL_TERM_BLOCK_SIZE); 

/*
 * Tunnel termination entry structure. 
 * contains up to SOC_TNL_TERM_ENTRY_WIDTH_MAX L3_TUNNEL table entries
 */
typedef struct soc_tunnel_term_s {
    uint32    entry_arr[SOC_TNL_TERM_BLOCK_SIZE][SOC_MAX_MEM_FIELD_WORDS];
} soc_tunnel_term_t;


/*
 * Tunnel termination hash table. 
 */
typedef struct _soc_tnl_term_hash_s {
    int         unit;
    int         entry_count;    /* Number entries in hash table */
    int         index_count;    /* Hash index max value + 1     */
    uint16      *table;         /* Hash table with 16 bit index */
    uint16      *link_table;    /* To handle collisions         */
} _soc_tnl_term_hash_t;

/* 
 * Tunnel termination type tcam indexes. 
 */
typedef struct soc_tnl_term_state_s {
    int start;  /* start index for this entry priority. */
    int end;    /* End index for this entry priority. */
    int prev;   /* Previous (Lo to Hi) priority with non zero entry count. */
    int next;   /* Next (Hi to Lo) priority with non zero entry count. */
    int vent;   /* Number of valid entries. */
    int fent;   /* Number of free entries. */
} soc_tnl_term_state_t, *soc_tnl_term_state_p;

extern int soc_tunnel_term_init(int unit);
extern int soc_tunnel_term_deinit(int unit);
extern int soc_tunnel_term_insert(int unit, soc_tunnel_term_t *entry, uint32 *index);
extern int soc_tunnel_term_delete(int unit, soc_tunnel_term_t *key);
extern int soc_tunnel_term_delete_all(int unit);
extern int soc_tunnel_term_match(int unit, soc_tunnel_term_t *key,
                                   soc_tunnel_term_t *result);
#if defined(BCM_WARM_BOOT_SUPPORT)
extern int soc_tunnel_term_reinit(int unit);
#endif /*  BCM_WARM_BOOT_SUPPORT */

#endif	/* !_TUNNEL_TERMINATION_H_ */
