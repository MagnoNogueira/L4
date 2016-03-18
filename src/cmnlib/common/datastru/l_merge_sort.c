/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys_type.h"
#include "l_merge_sort.h"
#include "l_sort_lst.h"

/* NAMING CONSTANT DECLARATIONS
 */
/*-------------------------------------------------------------------------
 * Macro NAME - L_MERGE_SORT_INSERT_SORT_LIST
 *-------------------------------------------------------------------------
 * PURPOSE: Insert a sort element to the sort list for the reason that
 *          we can know the input bufer index when we get the smallest
 *          element from sort list.
 * INPUT  : inbuf_index	--	the index of input buffer
 *			recordbuf_p	--	the pointer of record buffer
 *			compfun_p	--	the real compare callback function
 * OUTPUT : sort_list_p	--	the sort list
 * RETURN : None
 * NOTE	  : None
 *-------------------------------------------------------------------------*/
#define L_MERGE_SORT_INSERT_SORT_LIST(index,record_p,compfun_p,sort_list_p) \
{ \
	L_MERGE_SORT_Sort_Element_T element; \
	element.inbuf_index = index; \
	element.recordbuf_p = record_p; \
	element.input_compfun_p = compfun_p; \
	L_SORT_LST_Set ( sort_list_p, &element); \
}

/* DATA TYPE DECLARACTIONS
 */
typedef struct
{
	UI32_T	sorted_count;
	UI8_T   *recordbuf_p;
} L_MERGE_SORT_Workingbuf_T;

typedef struct
{
	UI32_T	inbuf_index;
	void	*recordbuf_p;
	int		(*input_compfun_p)(void *element1, void *element2);
} L_MERGE_SORT_Sort_Element_T;

/* LOCAL FUNCTIONS DECLARACTIONS
 */
static int L_MERGE_SORT_Compare (void *element1, void *element2);

/* EXPORTED FUNCTIONS BODIES
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
							BOOL_T (*supplementfun_p)(UI32_T inbuf_index, L_MERGE_SORT_Inbuf_Desc_T *inbuf_p))
{
	UI32_T empty_inbuf_count;
	UI32_T index;
	UI8_T  *outbuf_pos_p;
	UI32_T sorted_count;
	L_SORT_LST_List_T sort_list;
	L_MERGE_SORT_Workingbuf_T *working_buf_p;


	/* Init
	 */
	empty_inbuf_count = 0;
	sorted_count = 0;
	outbuf_pos_p = outbuf;
	working_buf_p = (L_MERGE_SORT_Workingbuf_T *)malloc(inbuf_no*sizeof(L_MERGE_SORT_Workingbuf_T));
	if(working_buf_p==NULL)
	{
		printf("\nmalloc working_buf_p failed\n");
		return 0;
	}
	memset(working_buf_p,0,inbuf_no*sizeof(L_MERGE_SORT_Workingbuf_T));

	/* Create a sort list
	 */
	L_SORT_LST_Create ( &sort_list, inbuf_no, sizeof(L_MERGE_SORT_Sort_Element_T),
		L_MERGE_SORT_Compare );

	/* Supplement all the input buffers and insert the first record buffer to the sort list
	 */
	for(index = 0; index < inbuf_no; index++)
	{
		if(inbuf_desc_p[index].record_count == 0)
		{
			if(!(*supplementfun_p) ( index, &inbuf_desc_p[index] ))
				empty_inbuf_count++;
		}

		if(inbuf_desc_p[index].record_count != 0)
		{
			working_buf_p[index].recordbuf_p = inbuf_desc_p[index].recordbuf_p;
			L_MERGE_SORT_INSERT_SORT_LIST (index,
				working_buf_p[index].recordbuf_p,compfun_p,&sort_list);
		}
	}

	/* Until all the input buffers are empty or the sorted count meets request sort count,
	 * get the smallest element then put into outbuf and remove the smallest element.
     */
	while( (sorted_count < request_sort_count) && (empty_inbuf_count < inbuf_no) )
	{
		L_MERGE_SORT_Sort_Element_T sort_element;

		if(!L_SORT_LST_Remove_1st ( &sort_list, &sort_element))
			break;	

		sorted_count++;
		memcpy(outbuf_pos_p,sort_element.recordbuf_p,record_size);
		outbuf_pos_p += record_size;

        /* 1. Increse sorted_count of selected inbuf & adjust pointer to next record
         * 2. is inbuf empty?
         * 3. if the input buffer has no more records, resupplement the input buffer.
         * 4. if the specified inbuf no more records to suplement then ignore this inbuf
         *    and continue do the merge-sort for the rest inbufs.
         */
		working_buf_p[sort_element.inbuf_index].sorted_count++;

		if(working_buf_p[sort_element.inbuf_index].sorted_count < inbuf_desc_p[sort_element.inbuf_index].record_count)
		{
			working_buf_p[sort_element.inbuf_index].recordbuf_p += record_size;		
		}
		else
		{
			if(!(*supplementfun_p) ( sort_element.inbuf_index, &inbuf_desc_p[sort_element.inbuf_index] ))
            {
				empty_inbuf_count++;
                continue;
            }
			working_buf_p[sort_element.inbuf_index].sorted_count = 0;
			working_buf_p[sort_element.inbuf_index].recordbuf_p = inbuf_desc_p[sort_element.inbuf_index].recordbuf_p;
		}
		
		L_MERGE_SORT_INSERT_SORT_LIST (sort_element.inbuf_index,
			working_buf_p[sort_element.inbuf_index].recordbuf_p,compfun_p,&sort_list);

	}

	L_SORT_LST_Delete_All(&sort_list);
	free(working_buf_p);

	return sorted_count;
} /* L_MERGE_SORT_Sort */

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_MERGE_SORT_Compare
 *-------------------------------------------------------------------------
 * PURPOSE: The function call will call the real compare function
 *			which is a parameter inputed in L_MERGE_SORT_Sort().
 * INPUT  : element1	--	the first compareson element
 *			element2	--	the second compareson element
 * OUTPUT : None
 * RETURN :
 * NOTE   : The function will be called by L_SORT_LST
 *-------------------------------------------------------------------------*/
static int L_MERGE_SORT_Compare (void *element1, void *element2)
{
    L_MERGE_SORT_Sort_Element_T *elem1, *elem2;

    elem1 = (L_MERGE_SORT_Sort_Element_T *)element1;
    elem2 = (L_MERGE_SORT_Sort_Element_T *)element2;

	return elem1 -> input_compfun_p(elem1 -> recordbuf_p, elem2 -> recordbuf_p);
} /* L_MERGE_SORT_Compare */

