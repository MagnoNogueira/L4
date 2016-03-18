#ifndef L_ISAM_H
#define L_ISAM_H
#include "sys_type.h"


/* NAMING CONSTANT
 */
#define NULL_INDEX            0xFFFF


/* TERM */
/*
   nidx:    index number of NodeTable
   iidx:    index number of IndexTable
*/


/* DATA TYPES
 */
typedef struct
{
   UI16_T   nidx_1st;         /* 1st nidx of node-list */
   UI16_T   nidx_last;        /* last nidx of node-list */
   UI16_T   nbr;
} Index_T;



typedef struct
{
   UI16_T   total_index_nbr;                 /* total element number of "index-part" */
   UI16_T   curr_index_nbr;                  /* used by isam.c */
   UI16_T   total_hash_nbr;                  /* total element number of "hash-part"  */
   UI16_T   hash_deepth;                     /* */
   /* Note: For general design, the "key" is extract from record, and save to separated
    *       area. But here, the memory and simple issue, I view the key is a subset of
    *       record, and the key is in the front part of the record.
    */
   UI16_T   key_len;
   Index_T  *index;        /* point to index table */
   /* Note: A node is a record, some application can create an node-extention-part
    *       for runtime used.
    */
   UI16_T   *hash;         /* point to hash table */
   UI16_T   *node_link;    /* point to node_link table, extension of node, used for link_list */
   UI16_T   N1;            /* Abs (list nbr - average_nbr) < N1 => need not adjust */
   UI16_T   N2;            /* each list > N2 => split */
} L_ISAM_Desc_T;




/* EXPORTED FUNCTIONs
 */

/* PURPOSE: Initiate Index-part
 * INPUT:   desc     -- descriptor of ISAM object
 */
void     L_ISAM_Init(L_ISAM_Desc_T *desc);

/* PURPOSE: Insert a node to ISAM
 * INPUT:   desc        -- descriptor of ISAM object
 *          key
 *          nidx
 *  OUTPUT: ret_nidx    --- if return false then output the nidx of conflict node
 *  RETURN: TRUE        --- Insert ok
 *          FALSE       --- do not insert node[nidx] because key confict.
 */
BOOL_T L_ISAM_InsertNode_1 (L_ISAM_Desc_T *desc, UI8_T *key, UI32_T nidx, UI32_T *ret_nidx);


/* PURPOSE: Delete a node from ISAM
 * INPUT:   desc     -- descriptor of ISAM object
 *          key      -- key to delete
 * OUTPUT:  none
 * RETURN:  deleted nidx
 * SIDE_EFFECT: ISAM structure
 */
UI32_T L_ISAM_DeleteNode (L_ISAM_Desc_T *desc, UI8_T *key);



UI32_T L_ISAM_GetNextNode (L_ISAM_Desc_T *desc, UI8_T *key);

/* PURPOSE: Locate the position of given key
 * INPUT:   desc     -- descriptor of ISAM object
 *          key      -- key to locate
 * OUTPUT:  iidx
 *          nidx     -- if located ok, store the node-index
 *          pre_nidx -- store the previous node-index
 * RETURN:  TRUE if locate ok, else return FALSE
 */
BOOL_T   L_ISAM_Locate (L_ISAM_Desc_T *desc,
                      UI8_T *key,
                      UI32_T *iidx,
                      UI32_T *nidx,
                      UI32_T *pre_nidx);

int L_ISAM_memcmp( const UI8_T *buf1, const UI8_T *buf2, UI32_T count );

#endif

