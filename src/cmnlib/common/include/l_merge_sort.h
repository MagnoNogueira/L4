#ifndef L__MERGE_SORT_H
#define L__MERGE_SORT_H

#include "sys_type.h"


/* NAMING CONSTANT DECLARACTIONS
 */



/* DATA TYPE DECLARACTIONS
 */
typedef struct
{
   UI32_T   record_count;
   void     *recordbuf_p;
} L_MERGE_SORT_Inbuf_Desc_T;



/* EXPORTED FUNCTION DECLARACTIONS
 */
/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_MERGE_SORT_Sort
 *-------------------------------------------------------------------------
 * PURPOSE  : merge sort
 * INPUT    : inbuf_no              -- the total number of input description buffer
 *            inbuf_desc_p			-- the pointer of input description buffer
 *                                     note that:
 *                                     the record_count of inbuf should be initialized as 0 and
 *                                     the recordbuf_p of inbuf should be initialized as the
 *                                     caller's key
 *            request_sort_count	-- the request sort count
 *            record_size			-- the size of record buffer in input description buffer
 *            compfun_p				-- the compare callback function
 *            supplementfun_p		-- the supplement callback function
 * OUTPUT   : outbuf				-- the sorted array
 * RETURN   : the sorted count
 * NOTE		: 1. Create a sort list.
 *			  2. Supplement all the input buffers.
 *			  3. Insert the first record buffer in each input buffer to the sort list.
 *			  4. Get the smallest element in the sort list then put into outbuf.
 *			  5. Remove the smallest element in the sort list then insert the next
 *				 record buffer in the input buffer to the sort list.
 *			  6. Check if the input buffer is empty, if it is then supplement the input buffer.
 *			  7. Repeate step 4, 5 and 6 until all the input buffers can't supplement
 *				 any element or the sorted count meets request sort count.
 *-------------------------------------------------------------------------*/
UI32_T L_MERGE_SORT_Sort(	UI32_T inbuf_no,
							L_MERGE_SORT_Inbuf_Desc_T *inbuf_desc_p,
							void *outbuf,
							UI32_T request_sort_count,
							UI32_T record_size,
							int (*compfun_p)(void *record1, void *record2),
							BOOL_T (*supplementfun_p)(UI32_T inbuf_index, L_MERGE_SORT_Inbuf_Desc_T *inbuf_p));




#endif /* L__MERGE_SORT_H */

