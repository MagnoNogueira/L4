/* Module Name: L_DLST
 * Purpose:
 *      The module provides a generic Indexed-Double Linked List for any applications
 *      which requires a quick response time to insert/update/delete a item from the
 *      the linked list.
 *
 * Notes:
 *      1. The Indexed-Double Linked List is a double linked list with index which provides
 *         good performance for the data access/retriveed from the list.
 *      2. Caller needs to do protection.
 *      3. not supposed to be called by ISR
 *
 * History:
 *       Date       -- Modifier,  Reason
 *  0.1 2003.12.22  --  Penny,   Created
 *  0.2 2004.04.23  --  Penny,   adding checking for dblist cookie while call
 *                                  L_DLST_ShMem_Indexed_Dblist_Destroy()
 *  0.3 2004.05.20  --  Penny,   adding new APIs L_DLST_ShMem_Indexed_Dblist_GetActualDblistDiscriptor()
 *                                  and L_DLST_ShMem_Indexed_Dblist_Search();
 *
 * Copyright(C)      Accton Corporation, 2003
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys_type.h"
#include "l_cvrt.h"
#include "l_dlist.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define L_DLST_NULL_INDEX 0xFFFF

/* DATA TYPE DECLARACTIONS
 */
typedef struct L_DLST_Dblist_Element_S
{
    UI16_T next_index;
    UI16_T previous_index;
    UI32_T dblist_id;  /* it's like a flag to show whether this element is used, it also saves the valid address of its descriptor L_DLST_Indexed_Dblist_T. */
}L_DLST_Dblist_Element_T;

typedef struct L_DLST_Dblist_Buffer_S
{
    UI32_T max_number_of_items_count;    /* max number of data items this dblist can handle */
    UI32_T clone_count;
    L_DLST_Dblist_Element_T element[1];
}L_DLST_Dblist_Buffer_T;

/* MACRO FUNCTION DECLARATIONS
 */
/*---------------------------------
 * Macro for shared memory version
 *---------------------------------
 */
#define L_DLST_SHMEM_GET_DBLIST(indexed_dblist) \
    ((indexed_dblist)->dblist_offset == 0 ?     \
        NULL :                                  \
        (L_DLST_ShMem_Dblist_Buffer_T *)L_CVRT_GET_PTR((indexed_dblist), (indexed_dblist)->dblist_offset))

/* STATIC VARIABLE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */

/* EXPORTED SUBPROGRAM BODIES
 */
/* FUNCTION NAME : L_DLST_Indexed_Dblist_Create
 * PURPOSE:
 *      Create and initiate a indexed double linked list.
 *
 * INPUT:
 *      max_number_of_items: max number of items can be handled by this indexed double linked list.
 *
 * OUTPUT:
 *      indexed_dblist: A indexed double linked list with capacity to manage max number of items
 *              defined by max_number_of_items.
 *
 * RETURN:
 *      TRUE  -- the list successfully created.
 *      FALSE -- can't create the list.
 *
 * NOTES:
 *      1. The following actions will be performed each time when this function is invoked:
 *          - The head_index and tail_index will be reset
 *          - If there are any itemes existed in this indexed double linked list, all the items will be
 *            deleted/removed/free automatically
 *      2. This is a generic indexed double linked list. So, the application process shall own the object.
 */
BOOL_T L_DLST_Indexed_Dblist_Create(L_DLST_Indexed_Dblist_T  *indexed_dblist, UI32_T max_number_of_items)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    L_DLST_Dblist_Buffer_T *dblist;
    UI32_T i;
    /* LOCAL VARIABLES DEFINITION
     */

    /* BODY
     */
    if (indexed_dblist == NULL ||
        indexed_dblist->dblist != NULL ||
        max_number_of_items == 0 ||
        max_number_of_items > (L_DLST_NULL_INDEX-1))
        return FALSE;

    indexed_dblist->head_index   = L_DLST_NULL_INDEX;
    indexed_dblist->tail_index   = L_DLST_NULL_INDEX;
    indexed_dblist->dblist       = NULL;

    /* alloc memory for dblist */
    dblist = (L_DLST_Dblist_Buffer_T *)malloc(sizeof(UI32_T)/*for clone count */ + sizeof(UI32_T)/* for max_number_of_items */+ (sizeof(L_DLST_Dblist_Element_T)*max_number_of_items));
    if (dblist == NULL)
        return  FALSE;      /*  No more space   */

    dblist->clone_count = 1;
    dblist->max_number_of_items_count = max_number_of_items;
    /* Init each element */
    for (i=0; i<max_number_of_items; i++)
    {
        dblist->element[i].dblist_id         = 0;
        dblist->element[i].next_index        = L_DLST_NULL_INDEX;
        dblist->element[i].previous_index    = L_DLST_NULL_INDEX;
    }

    indexed_dblist->dblist = (void *)dblist;

    return TRUE;

}/* end of L_DLST_Indexed_Dblist_Create */

/* FUNCTION NAME : L_DLST_Indexed_Dblist_Destroy
 * PURPOSE:
 *      Destroy a indexed double linked list.
 *
 * INPUT:
 *      indexed_dblist: a indexed double linked list to be destroyed.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *
 * NOTES:
 *      If there are any itemes existed in this indexed double linked list, all the items will be
 *      deleted/removed/free automatically. Then the indexed double linked list will be free/deleted.
 */
void L_DLST_Indexed_Dblist_Destroy(L_DLST_Indexed_Dblist_T *indexed_dblist)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY
     */
    if (indexed_dblist == NULL)
        return;

    if (indexed_dblist->dblist)
        free(indexed_dblist->dblist);

    memset(indexed_dblist, 0, sizeof(L_DLST_Indexed_Dblist_T));
    return;

} /* end of L_DLST_Indexed_Dblist_Destroy */

/* FUNCTION NAME : L_DLST_Indexed_Dblist_Enqueue2Head
 * PURPOSE:
 *      Insert specified index to list head.
 *      The first entry will be returned based on this index in next item retrieved.
 *
 * INPUT:
 *      indexed_dblist: the list dblist which to be inserted.
 *      index     : entry to be enqueued from head.
 *
 * OUTPUT:
 *      NONE.
 *
 * RETURN:
 *      TRUE  -- queue to head successfully
 *      FALSE -- can't queue to list, maybe list not existed or
 *               the element already enqueued.
 *
 * NOTES:
 *
 */
BOOL_T L_DLST_Indexed_Dblist_Enqueue2Head(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T index)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    L_DLST_Dblist_Buffer_T *dblist;
    L_DLST_Dblist_Element_T *element;
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = (L_DLST_Dblist_Buffer_T *)indexed_dblist->dblist;
    if (dblist == NULL || index > (dblist->max_number_of_items_count-1))
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    if (element[index].dblist_id != 0)
        return FALSE;

    element[index].previous_index = L_DLST_NULL_INDEX;
    element[index].next_index     = L_DLST_NULL_INDEX;
    element[index].dblist_id = (UI32_T)indexed_dblist;

    if (indexed_dblist->head_index != L_DLST_NULL_INDEX)
    {
        element[index].next_index                           = indexed_dblist->head_index;
        element[indexed_dblist->head_index].previous_index  = index;
    }

    //indexed_dblist->dblist = (void *)element;
    indexed_dblist->head_index = index;

    if (indexed_dblist->tail_index == L_DLST_NULL_INDEX)
    {
        indexed_dblist->tail_index = index;
    }

    return TRUE;
}/* end of L_DLST_Indexed_Dblist_Enqueue2Head */

/* FUNCTION NAME : L_DLST_Indexed_Dblist_Enqueue
 * PURPOSE:
 *      Append specified index to list.
 *
 * INPUT:
 *      indexed_dblist: the list dblist which to be inserted.
 *      index     : entry to be appended.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- append to tail of list
 *      FALSE -- can't queue to list, maybe list not existed or
 *               the element already enqueued.
 *
 * NOTES:
 *
 */
BOOL_T L_DLST_Indexed_Dblist_Enqueue(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T index)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    L_DLST_Dblist_Element_T *element;
    L_DLST_Dblist_Buffer_T *dblist;
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = (L_DLST_Dblist_Buffer_T  *)indexed_dblist->dblist;

    if (dblist == NULL || index > (dblist->max_number_of_items_count-1))
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    if (element[index].dblist_id != 0)
        return FALSE;

    element[index].previous_index  = L_DLST_NULL_INDEX;
    element[index].next_index      = L_DLST_NULL_INDEX;
    element[index].dblist_id = (UI32_T)indexed_dblist ;

    if (indexed_dblist->tail_index != L_DLST_NULL_INDEX)
    {
        element[index].previous_index  = indexed_dblist->tail_index;
        element[indexed_dblist->tail_index].next_index = index;
    }

    //indexed_dblist->dblist = (void *)element;
    indexed_dblist->tail_index = index;

    if (indexed_dblist->head_index == L_DLST_NULL_INDEX)
        indexed_dblist->head_index = index;

    return TRUE;

}/* end of L_DLST_Indexed_Dblist_Enqueue */

/* FUNCTION NAME : L_DLST_Indexed_Dblist_GetFirstEntry
 * PURPOSE:
 *      Get and first entry from list.
 *
 * INPUT:
 *      indexed_dblist: the list dblist which to be retrieved.
 *
 * OUTPUT:
 *      index     : the first element in the list.
 *
 * RETURN:
 *      TRUE  -- got the first element
 *      FALSE -- the list is empty or dblist incorrect.
 *
 * NOTES:
 *      This API will not delete the first entry.
 */
BOOL_T L_DLST_Indexed_Dblist_GetFirstEntry(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T *index)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_Dblist_Element_T *element;
    L_DLST_Dblist_Buffer_T *dblist;


    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == NULL ||
        indexed_dblist->head_index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = (L_DLST_Dblist_Buffer_T *)indexed_dblist->dblist;

    if (dblist == NULL)
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    if (element[indexed_dblist->head_index].dblist_id == 0)	
	    return FALSE;
	
    *index = indexed_dblist->head_index;

    return TRUE;

}/* end of L_DLST_Indexed_Dblist_GetFirstEntry */


/* FUNCTION NAME : L_DLST_Indexed_Dblist_GetNextEntry
 * PURPOSE:
 *      Get next entry from list by giving index.
 *
 * INPUT:
 *     indexed_dblist: the list dblist which to be retrieved.
 *     index             : giving index
 * OUTPUT:
 *      index     : the next element in the list.
 *
 * RETURN:
 *      TRUE  -- got the next node
 *      FALSE -- the list is empty or dblist incorrect.
 *
 * NOTES:
 *
 */
BOOL_T L_DLST_Indexed_Dblist_GetNextEntry(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T *index)
{

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_Dblist_Buffer_T  *dblist;
    L_DLST_Dblist_Element_T *element;
    UI16_T next_record_index;
    /* BODY
     */
    if (indexed_dblist == NULL ||
        indexed_dblist->dblist == NULL)
        return FALSE;
	
	dblist = (L_DLST_Dblist_Buffer_T *)indexed_dblist->dblist;
	element = dblist->element;

    if (element == NULL)
        return FALSE;

    next_record_index = element[*index].next_index;

    if ( next_record_index == L_DLST_NULL_INDEX )
         return FALSE;

    *index = next_record_index;

    return TRUE;
}
/* FUNCTION NAME : L_DLST_Indexed_Dblist_Dequeue
 * PURPOSE:
 *      Get and remove first entry from list.
 *
 * INPUT:
 *      indexed_dblist: the list dblist which to be retrieved.
 *
 * OUTPUT:
 *      index     : the first element in the list.
 *
 * RETURN:
 *      TRUE  -- got the first element
 *      FALSE -- the list is empty or dblist incorrect.
 *
 * NOTES:
 *
 */
BOOL_T L_DLST_Indexed_Dblist_Dequeue(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T *index)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_Dblist_Element_T *element;
    L_DLST_Dblist_Buffer_T *dblist;

    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == NULL ||
        indexed_dblist->head_index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = (L_DLST_Dblist_Buffer_T *)indexed_dblist->dblist;

    if (dblist == NULL)
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    if (element[indexed_dblist->head_index].dblist_id == 0)	
	    return FALSE;
	
    *index = indexed_dblist->head_index;
    if (element[indexed_dblist->head_index].next_index != L_DLST_NULL_INDEX)
        indexed_dblist->head_index = element[indexed_dblist->head_index].next_index;
    else
        indexed_dblist->head_index = L_DLST_NULL_INDEX;

    if (indexed_dblist->head_index != L_DLST_NULL_INDEX)
		element[indexed_dblist->head_index].previous_index = L_DLST_NULL_INDEX;

    if (indexed_dblist->head_index == L_DLST_NULL_INDEX)
        indexed_dblist->tail_index = L_DLST_NULL_INDEX;


    element[*index].dblist_id = 0;
    element[*index].next_index      = L_DLST_NULL_INDEX;
    element[*index].previous_index  = L_DLST_NULL_INDEX;

    return TRUE;
}/* end of L_DLST_Indexed_Dblist_Dequeue*/

/* FUNCTION NAME : L_DLST_Indexed_Dblist_DeleteEntry
 * PURPOSE:
 *      Remove the specified entry from list.
 *
 * INPUT:
 *      indexed_dblist: the list dblist which to be inserted.
 *      index     : the special entry to be removed
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- successfully remove the entry from list
 *      FALSE -- can't find the element or not a valid list.
 *
 * NOTES:
 *
 */
BOOL_T L_DLST_Indexed_Dblist_DeleteEntry (L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T index)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_Dblist_Element_T *element;
    L_DLST_Dblist_Buffer_T *dblist;
    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

   dblist = (L_DLST_Dblist_Buffer_T *)indexed_dblist->dblist;
   if (dblist == NULL)
        return FALSE;

   element = dblist->element;

    if (element == NULL)
        return FALSE;

   if(element[index].dblist_id == 0)
        return FALSE;

	
   if (indexed_dblist->head_index == index)
   {
        if (element[index].next_index != L_DLST_NULL_INDEX)
            indexed_dblist->head_index = element[index].next_index;
        else
            indexed_dblist->head_index = L_DLST_NULL_INDEX;
   }

   if (indexed_dblist->tail_index == index)
   {
        if (element[index].previous_index != L_DLST_NULL_INDEX)
            indexed_dblist->tail_index = element[index].previous_index;
        else
            indexed_dblist->tail_index = L_DLST_NULL_INDEX;
   }

   if (element[index].previous_index != L_DLST_NULL_INDEX)
        element[element[index].previous_index].next_index = element[index].next_index;

   if (element[index].next_index != L_DLST_NULL_INDEX)
        element[element[index].next_index].previous_index = element[index].previous_index;

   element[index].dblist_id       = 0;
   element[index].previous_index  = L_DLST_NULL_INDEX;
   element[index].next_index      = L_DLST_NULL_INDEX;

   return TRUE;

}/* end of L_DLST_Indexed_Dblist_DeleteEntry */

/* FUNCTION NAME : L_DLST_Indexed_Dblist_Clone
 * PURPOSE:
 *      Clone a indexed double linked list.
 *
 * INPUT:
 *      clone_from: the original indexed_dblist to be cloned from.
 *      clone_to  : the new indexed_dblist to be cloned to.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- successfully clone the indexed list
 *      FALSE -- fail to  clone the indexed list.
 *
 * NOTES:
 *      1. The user has the responsibility to allocate the memory
 *          for both clone_from and clone_to buffers.
 *      2. head_index and tail_index will not be cloned to new indexed_dblist.
 */
BOOL_T L_DLST_Indexed_Dblist_Clone(L_DLST_Indexed_Dblist_T *clone_from, L_DLST_Indexed_Dblist_T *clone_to)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    L_DLST_Dblist_Buffer_T *dblist;
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY */
    if (clone_from == NULL ||
        clone_to == NULL)
        return FALSE;

    clone_to->head_index   = L_DLST_NULL_INDEX;
    clone_to->tail_index   = L_DLST_NULL_INDEX;
    clone_to->dblist       = clone_from->dblist;

    /* Increase clone_count */
    dblist = (L_DLST_Dblist_Buffer_T *)clone_to->dblist;

    if (dblist == NULL)
        return FALSE;

    dblist->clone_count++;
    return TRUE;

} /* end of L_DLST_Indexed_Dblist_Clone*/

/* FUNCTION NAME : L_DLST_Indexed_Dblist_Search
 * PURPOSE:
 *      Search next element(record) from indexed dblist structure
 *
 * INPUT:
 *      indexed_dblist: the indexed_dblist to be searched from.
 *      cookie        : the buffer provided by caller for passing to callback function
 *      callback      : The callback function to 'view' the element.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      L_DLST_SEARCH_BREAK: Searching is broken by (*fun)
 *      L_DLST_SEARCH_END_OF_LIST: stop search because end of list
 *      L_DLST_SEARCH_COMPLETED: stop search because reach the limited count
 *      L_DLST_SEARCH_INVALID_KIDX: stop search because invalid key index
 *      L_DLST_SEARCH_INVALID_INPUT: return error while indexed_dblist is NULL pointer
 *
 * NOTES:
 *
 */
UI32_T L_DLST_Indexed_Dblist_Search(L_DLST_Indexed_Dblist_T *indexed_dblist,
                                    UI32_T cookie,
                                    UI32_T (*call_back) (UI16_T index, UI32_T cookie))
{
    /* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_Dblist_Buffer_T  *dblist;
    L_DLST_Dblist_Element_T *element;
    UI16_T record_index, next_record_index;
    /* BODY
     */
    if (indexed_dblist == NULL ||
        indexed_dblist->dblist == NULL)
        return L_DLST_SEARCH_INVALID_INPUT;
	
	dblist = (L_DLST_Dblist_Buffer_T *)indexed_dblist->dblist;
	element = dblist->element;

    if (element == NULL)
        return L_DLST_SEARCH_INVALID_INPUT;

    /* List is empty */
    if (indexed_dblist->head_index == L_DLST_NULL_INDEX)
	    return L_DLST_SEARCH_END_OF_LIST;
	
	record_index = indexed_dblist->head_index;
	
	while (1)
	{	
	    if ( record_index == L_DLST_NULL_INDEX )
            return L_DLST_SEARCH_END_OF_LIST;
            next_record_index = element[record_index].next_index;
            if ((*call_back)(record_index, cookie) == L_DLST_SEARCH_BREAK)
                return L_DLST_SEARCH_BREAK;
            record_index = next_record_index;
        }


}/* L_DLST_Indexed_Dblist_Search */

/* FUNCTION NAME : L_DLST_Indexed_Dblist_GetActualDblistDiscriptor
 * PURPOSE:
 *      Get the actual index_dblist by input index
 *
 * INPUT:
 *      indexed_dblist: the original indexed_dblist to be cloned from.
 *      index         : the input index to search for the actual indexed_dblist
 *      actual_indexed_dblist : the address to store the indexed_dblist which
 *                                  contains the element with the input index.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- successfully
 *      FALSE -- fail
 *
 * NOTES:
 *      1. The user has the responsibility to allocate the memory
 *          for indexed_dblist.
 *
 */
BOOL_T L_DLST_Indexed_Dblist_GetActualDblistDiscriptor(L_DLST_Indexed_Dblist_T *indexed_dblist, UI32_T index, L_DLST_Indexed_Dblist_T **actual_indexed_dblist)
{
	/* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_Dblist_Element_T *element;
    L_DLST_Dblist_Buffer_T  *dblist;
    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = (L_DLST_Dblist_Buffer_T *)indexed_dblist->dblist;
    if (dblist == NULL || index > (dblist->max_number_of_items_count-1))
        return FALSE;

    element = dblist->element;
    if (element == NULL)
        return FALSE;

    if (element[index].dblist_id == 0)
        return FALSE;
  *actual_indexed_dblist = (L_DLST_Indexed_Dblist_T *)element[index].dblist_id;
    return TRUE;

}/* end of L_DLST_Indexed_Dblist_GetActualDblistDiscriptor */

/* FUNCTION NAME : L_DLST_Indexed_Dblist_DeleteAll_ByListPointerArray
 * PURPOSE:
 *      Delete all entries in the list.
 *
 * INPUT:
 *      group_dblist: pointer array of a grouped dblists which need to be cleared.
 *                          all elements in the pointed dblist are deleted & all pointed descriptors in the group (main & cloned) are initialized.
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- the list is cleared.
 *      FALSE -- this is not a list.
 *
 * NOTES:
 *      1. if the list is already empty, return TRUE.
 *
 *      2.------------------------
 *        | A | B | C | D | E | F |  .... address of descriptor
 *        --|---|---|---|---|---|-
 *          V   V   V   V   V   V
 *         [_] [_] [_] [_] [_] [_]   .... descriptor
 *          |   |   |   |   |   |
 *          V   V   V   V   V   V
 *         |-| |-| |-| |-| |-| |-|   .... dlist
 *         |-| |-| |-| |-| |-| |-|
 *         |-| |-| |-| |-| |-| |-|
 *         |_| |_| |_| |_| |_| |_|
 */
BOOL_T L_DLST_Indexed_Dblist_DeleteAll_ByListPointerArray(L_DLST_Indexed_Dblist_T  *group_dblist[])
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    L_DLST_Dblist_Buffer_T *dblist;
    UI32_T i, clone_count;
    UI32_T max_number_of_items;
    /* LOCAL VARIABLES DEFINITION
     */

    /* BODY
     */
    if (group_dblist[0] == NULL)
        return FALSE;

    /* getting clone_count */
    dblist = (L_DLST_Dblist_Buffer_T *)group_dblist[0]->dblist;
    if (dblist == NULL)
        return FALSE;
    clone_count = dblist->clone_count;
    /* getting max_number_of_items */
    max_number_of_items = dblist->max_number_of_items_count;

    /* Init each element */
    for (i=0; i<max_number_of_items; i++)
    {
        dblist->element[i].dblist_id         = 0;
        dblist->element[i].next_index        = L_DLST_NULL_INDEX;
        dblist->element[i].previous_index    = L_DLST_NULL_INDEX;
    }
    /* Init each dblist descriptor */
    for ( i=0; i< clone_count; i++)
    {
        group_dblist[i]->head_index   = L_DLST_NULL_INDEX;
        group_dblist[i]->tail_index   = L_DLST_NULL_INDEX;
    }
    return TRUE;

}

/* FUNCTION NAME : L_DLST_Indexed_Dblist_DeleteAll_ByListArray
 * PURPOSE:
 *      Delete all entries in the list.
 *
 * INPUT:
 *      group_dblist: the group dblist which is to be cleared.
 *          all elements in dblist are deleted & all descriptors in the group (main & cloned) are initialized.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- the list is cleared.
 *      FALSE -- this is not a list.
 *
 * NOTES:
 *      1. if the list is already empty, return TRUE.
 *
 *      2.------------------------
 *        | A | B | C | D | E | F |  .... descriptor
 *        --|---|---|---|---|---|-
 *          V   V   V   V   V   V
 *         |-| |-| |-| |-| |-| |-|   .... dlist
 *         |-| |-| |-| |-| |-| |-|
 *         |-| |-| |-| |-| |-| |-|
 *         |_| |_| |_| |_| |_| |_|
 *
 */
BOOL_T L_DLST_Indexed_Dblist_DeleteAll_ByListArray(L_DLST_Indexed_Dblist_T  group_dblist[])
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    L_DLST_Dblist_Buffer_T *dblist;
    UI32_T i, clone_count;
    UI32_T max_number_of_items;
    /* LOCAL VARIABLES DEFINITION
     */

    /* BODY
     */
    if (group_dblist == NULL)
        return FALSE;

    /* getting clone_count */

    dblist = (L_DLST_Dblist_Buffer_T *)group_dblist[0].dblist;
    if (dblist == NULL)
        return FALSE;
    clone_count = dblist->clone_count;
    /* getting max_number_of_items */
    max_number_of_items = dblist->max_number_of_items_count;

    /* Init each element */
    for (i=0; i<max_number_of_items; i++)
    {
        dblist->element[i].dblist_id         = 0;
        dblist->element[i].next_index        = L_DLST_NULL_INDEX;
        dblist->element[i].previous_index    = L_DLST_NULL_INDEX;
    }
    /* Init each dblist descriptor */
    for ( i=0; i< clone_count; i++)
    {
        group_dblist[i].head_index   = L_DLST_NULL_INDEX;
        group_dblist[i].tail_index   = L_DLST_NULL_INDEX;
    }
    return TRUE;

}/* end of L_DLST_Indexed_Dblist_DeleteAll */

/* FUNCTION NAME : L_DLST_Indexed_Dblist_SetEntryID
 * PURPOSE:
 *      Set entry's ID.
 *
 * INPUT:
 *              indexed_dblist: which link list
 *              index:the specific entry to set ID
 *              dlist_id: id
 * OUTPUT:
 *              none
 *
 * RETURN:
 *      TRUE  -- Set ID successfully.
 *      FALSE -- Set ID failed.
 *
 * NOTES :
 *      If you really need to change element's id value you need to take care of
 *      this element. You can't call Dblist_GetActualDblistDiscriptor since it
 *      may have unexpected exception.
 */

BOOL_T L_DLST_Indexed_Dblist_SetEntryID(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T index, UI32_T dlist_id)
{
 /* LOCAL CONSTANT DECLARATIONS
     */
    L_DLST_Dblist_Element_T *element;
    L_DLST_Dblist_Buffer_T *dblist;
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = (L_DLST_Dblist_Buffer_T  *)indexed_dblist->dblist;

    if (dblist == NULL || index > (dblist->max_number_of_items_count-1))
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    element[index].dblist_id = dlist_id ;

    return TRUE;
	
}


/* FUNCTION NAME : L_DLST_Indexed_Dblist_GetEntryID
 * PURPOSE:
 *      Get entry's ID.
 *
 * INPUT:
 *              indexed_dblist: which link list
 *              index:the specific entry to set ID
 *
 * OUTPUT:
 *              dlist_id: id
 *
 * RETURN:
 *      TRUE  -- Get ID successfully.
 *      FALSE -- Get ID failed.
 */

BOOL_T L_DLST_Indexed_Dblist_GetEntryID(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T index, UI32_T *dlist_id)
{
 /* LOCAL CONSTANT DECLARATIONS
     */
    L_DLST_Dblist_Element_T *element;
    L_DLST_Dblist_Buffer_T *dblist;
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = (L_DLST_Dblist_Buffer_T  *)indexed_dblist->dblist;

    if (dblist == NULL || index > (dblist->max_number_of_items_count-1))
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    *dlist_id = element[index].dblist_id ;

    return TRUE;
	
}

/*---------------------------------
 * Exported functions for shared memory version
 *---------------------------------
 */
/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_Create
 * PURPOSE:
 *      Create and initiate a indexed double linked list.
 *
 * INPUT:
 *      working_buffer: the buffer to store dboule linked list.
 *                      the size of the buffer can be calculated by L_DLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ()
 *      max_number_of_items: max number of items can be handled by this indexed double linked list.
 *
 * OUTPUT:
 *      indexed_dblist: A indexed double linked list with capacity to manage max number of items
 *              defined by max_number_of_items.
 *
 * RETURN:
 *      TRUE  -- the list successfully created.
 *      FALSE -- can't create the list.
 *
 * NOTES:
 *      1. The following actions will be performed each time when this function is invoked:
 *          - The head_index and tail_index will be reset
 *          - If there are any itemes existed in this indexed double linked list, all the items will be
 *            deleted/removed/free automatically
 *      2. This is a generic indexed double linked list. So, the application process shall own the object.
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_Create(L_DLST_ShMem_Indexed_Dblist_T  *indexed_dblist, void *working_buffer, UI32_T max_number_of_items)
{
    /* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Buffer_T *dblist;
    UI32_T i;

    /* BODY
     */
    if (indexed_dblist == NULL ||
        working_buffer == NULL ||
        max_number_of_items == 0 ||
        max_number_of_items > (L_DLST_NULL_INDEX-1))
        return FALSE;

    indexed_dblist->head_index    = L_DLST_NULL_INDEX;
    indexed_dblist->tail_index    = L_DLST_NULL_INDEX;
    indexed_dblist->dblist_offset = L_CVRT_GET_OFFSET(indexed_dblist, working_buffer);

    /* alloc memory for dblist */
    dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);

    dblist->clone_count = 1;
    dblist->max_number_of_items_count = max_number_of_items;
    /* Init each element */
    for (i=0; i<max_number_of_items; i++)
    {
        dblist->element[i].dblist_id         = 0;
        dblist->element[i].next_index        = L_DLST_NULL_INDEX;
        dblist->element[i].previous_index    = L_DLST_NULL_INDEX;
    }

    return TRUE;

}/* end of L_DLST_ShMem_Indexed_Dblist_Create */

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_Destroy
 * PURPOSE:
 *      Destroy a indexed double linked list.
 *
 * INPUT:
 *      indexed_dblist: a indexed double linked list to be destroyed.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *
 * NOTES:
 *      If there are any itemes existed in this indexed double linked list, all the items will be
 *      deleted/removed/free automatically. Then the indexed double linked list will be free/deleted.
 */
void L_DLST_ShMem_Indexed_Dblist_Destroy(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY
     */
    if (indexed_dblist == NULL)
        return;

    memset(indexed_dblist, 0, sizeof(L_DLST_ShMem_Indexed_Dblist_T));
    return;

} /* end of L_DLST_ShMem_Indexed_Dblist_Destroy */

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_Enqueue2Head
 * PURPOSE:
 *      Insert specified index to list head.
 *      The first entry will be returned based on this index in next item retrieved.
 *
 * INPUT:
 *      indexed_dblist: the list dblist which to be inserted.
 *      index     : entry to be enqueued from head.
 *
 * OUTPUT:
 *      NONE.
 *
 * RETURN:
 *      TRUE  -- queue to head successfully
 *      FALSE -- can't queue to list, maybe list not existed or
 *               the element already enqueued.
 *
 * NOTES:
 *
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_Enqueue2Head(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T index)
{
    /* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Buffer_T *dblist;
    L_DLST_ShMem_Dblist_Element_T *element;

    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);
    if (dblist == NULL || index > (dblist->max_number_of_items_count-1))
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    if (element[index].dblist_id != 0)
        return FALSE;

    element[index].previous_index = L_DLST_NULL_INDEX;
    element[index].next_index     = L_DLST_NULL_INDEX;
    element[index].dblist_id = L_CVRT_GET_OFFSET(dblist, indexed_dblist);

    if (indexed_dblist->head_index != L_DLST_NULL_INDEX)
    {
        element[index].next_index                           = indexed_dblist->head_index;
        element[indexed_dblist->head_index].previous_index  = index;
    }

    //indexed_dblist->dblist = (void *)element;
    indexed_dblist->head_index = index;

    if (indexed_dblist->tail_index == L_DLST_NULL_INDEX)
    {
        indexed_dblist->tail_index = index;
    }

    return TRUE;
}/* end of L_DLST_ShMem_Indexed_Dblist_Enqueue2Head */

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_Enqueue
 * PURPOSE:
 *      Append specified index to list.
 *
 * INPUT:
 *      indexed_dblist: the list dblist which to be inserted.
 *      index     : entry to be appended.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- append to tail of list
 *      FALSE -- can't queue to list, maybe list not existed or
 *               the element already enqueued.
 *
 * NOTES:
 *
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_Enqueue(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T index)
{
    /* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Element_T *element;
    L_DLST_ShMem_Dblist_Buffer_T *dblist;

    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);

    if (dblist == NULL || index > (dblist->max_number_of_items_count-1))
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    if (element[index].dblist_id != 0)
        return FALSE;

    element[index].previous_index  = L_DLST_NULL_INDEX;
    element[index].next_index      = L_DLST_NULL_INDEX;
    element[index].dblist_id = L_CVRT_GET_OFFSET(dblist, indexed_dblist);

    if (indexed_dblist->tail_index != L_DLST_NULL_INDEX)
    {
        element[index].previous_index  = indexed_dblist->tail_index;
        element[indexed_dblist->tail_index].next_index = index;
    }

    //indexed_dblist->dblist = (void *)element;
    indexed_dblist->tail_index = index;

    if (indexed_dblist->head_index == L_DLST_NULL_INDEX)
        indexed_dblist->head_index = index;

    return TRUE;

}/* end of L_DLST_ShMem_Indexed_Dblist_Enqueue */

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_GetFirstEntry
 * PURPOSE:
 *      Get and first entry from list.
 *
 * INPUT:
 *      indexed_dblist: the list dblist which to be retrieved.
 *
 * OUTPUT:
 *      index     : the first element in the list.
 *
 * RETURN:
 *      TRUE  -- got the first element
 *      FALSE -- the list is empty or dblist incorrect.
 *
 * NOTES:
 *      This API will not delete the first entry.
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_GetFirstEntry(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T *index)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Element_T *element;
    L_DLST_ShMem_Dblist_Buffer_T *dblist;


    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == NULL ||
        indexed_dblist->head_index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);

    if (dblist == NULL)
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    if (element[indexed_dblist->head_index].dblist_id == 0)	
	    return FALSE;
	
    *index = indexed_dblist->head_index;

    return TRUE;

}/* end of L_DLST_ShMem_Indexed_Dblist_GetFirstEntry */


/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_GetNextEntry
 * PURPOSE:
 *      Get next entry from list by giving index.
 *
 * INPUT:
 *     indexed_dblist: the list dblist which to be retrieved.
 *     index             : giving index
 * OUTPUT:
 *      index     : the next element in the list.
 *
 * RETURN:
 *      TRUE  -- got the next node
 *      FALSE -- the list is empty or dblist incorrect.
 *
 * NOTES:
 *
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_GetNextEntry(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T *index)
{

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Buffer_T  *dblist;
    L_DLST_ShMem_Dblist_Element_T *element;
    UI16_T next_record_index;
    /* BODY
     */
    if (indexed_dblist == NULL ||
        indexed_dblist->dblist_offset== 0)
        return FALSE;
	
	dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);
	element = dblist->element;

    if (element == NULL)
        return FALSE;

    next_record_index = element[*index].next_index;

    if ( next_record_index == L_DLST_NULL_INDEX )
         return FALSE;

    *index = next_record_index;

    return TRUE;
}
/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_Dequeue
 * PURPOSE:
 *      Get and remove first entry from list.
 *
 * INPUT:
 *      indexed_dblist: the list dblist which to be retrieved.
 *
 * OUTPUT:
 *      index     : the first element in the list.
 *
 * RETURN:
 *      TRUE  -- got the first element
 *      FALSE -- the list is empty or dblist incorrect.
 *
 * NOTES:
 *
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_Dequeue(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T *index)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Element_T *element;
    L_DLST_ShMem_Dblist_Buffer_T *dblist;

    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == NULL ||
        indexed_dblist->head_index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);

    if (dblist == NULL)
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    if (element[indexed_dblist->head_index].dblist_id == 0)	
	    return FALSE;
	
    *index = indexed_dblist->head_index;
    if (element[indexed_dblist->head_index].next_index != L_DLST_NULL_INDEX)
        indexed_dblist->head_index = element[indexed_dblist->head_index].next_index;
    else
        indexed_dblist->head_index = L_DLST_NULL_INDEX;

    if (indexed_dblist->head_index != L_DLST_NULL_INDEX)
		element[indexed_dblist->head_index].previous_index = L_DLST_NULL_INDEX;

    if (indexed_dblist->head_index == L_DLST_NULL_INDEX)
        indexed_dblist->tail_index = L_DLST_NULL_INDEX;


    element[*index].dblist_id = 0;
    element[*index].next_index      = L_DLST_NULL_INDEX;
    element[*index].previous_index  = L_DLST_NULL_INDEX;

    return TRUE;
}/* end of L_DLST_ShMem_Indexed_Dblist_Dequeue*/

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_DeleteEntry
 * PURPOSE:
 *      Remove the specified entry from list.
 *
 * INPUT:
 *      indexed_dblist: the list dblist which to be inserted.
 *      index     : the special entry to be removed
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- successfully remove the entry from list
 *      FALSE -- can't find the element or not a valid list.
 *
 * NOTES:
 *
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_DeleteEntry (L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T index)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Element_T *element;
    L_DLST_ShMem_Dblist_Buffer_T *dblist;
    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

   dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);
   if (dblist == NULL)
        return FALSE;

   element = dblist->element;

    if (element == NULL)
        return FALSE;

   if(element[index].dblist_id == 0)
        return FALSE;

	
   if (indexed_dblist->head_index == index)
   {
        if (element[index].next_index != L_DLST_NULL_INDEX)
            indexed_dblist->head_index = element[index].next_index;
        else
            indexed_dblist->head_index = L_DLST_NULL_INDEX;
   }

   if (indexed_dblist->tail_index == index)
   {
        if (element[index].previous_index != L_DLST_NULL_INDEX)
            indexed_dblist->tail_index = element[index].previous_index;
        else
            indexed_dblist->tail_index = L_DLST_NULL_INDEX;
   }

   if (element[index].previous_index != L_DLST_NULL_INDEX)
        element[element[index].previous_index].next_index = element[index].next_index;

   if (element[index].next_index != L_DLST_NULL_INDEX)
        element[element[index].next_index].previous_index = element[index].previous_index;

   element[index].dblist_id       = 0;
   element[index].previous_index  = L_DLST_NULL_INDEX;
   element[index].next_index      = L_DLST_NULL_INDEX;

   return TRUE;

}/* end of L_DLST_ShMem_Indexed_Dblist_DeleteEntry */

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_Clone
 * PURPOSE:
 *      Clone a indexed double linked list.
 *
 * INPUT:
 *      clone_from: the original indexed_dblist to be cloned from.
 *      clone_to  : the new indexed_dblist to be cloned to.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- successfully clone the indexed list
 *      FALSE -- fail to  clone the indexed list.
 *
 * NOTES:
 *      1. The user has the responsibility to allocate the memory
 *          for both clone_from and clone_to buffers.
 *      2. head_index and tail_index will not be cloned to new indexed_dblist.
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_Clone(L_DLST_ShMem_Indexed_Dblist_T *clone_from, L_DLST_ShMem_Indexed_Dblist_T *clone_to)
{
    /* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Buffer_T *dblist;

    /* BODY */
    if (clone_from == NULL ||
        clone_to == NULL)
        return FALSE;

    dblist = L_DLST_SHMEM_GET_DBLIST(clone_from);
    if (dblist == NULL)
        return FALSE;

    clone_to->head_index    = L_DLST_NULL_INDEX;
    clone_to->tail_index    = L_DLST_NULL_INDEX;
    clone_to->dblist_offset = L_CVRT_GET_OFFSET(clone_to, dblist);

    /* Increase clone_count */
    dblist->clone_count++;
    return TRUE;

} /* end of L_DLST_ShMem_Indexed_Dblist_Clone*/

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_Search
 * PURPOSE:
 *      Search next element(record) from indexed dblist structure
 *
 * INPUT:
 *      indexed_dblist: the indexed_dblist to be searched from.
 *      cookie        : the buffer provided by caller for passing to callback function
 *      callback      : The callback function to 'view' the element.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      L_DLST_SEARCH_BREAK: Searching is broken by (*fun)
 *      L_DLST_SEARCH_END_OF_LIST: stop search because end of list
 *      L_DLST_SEARCH_COMPLETED: stop search because reach the limited count
 *      L_DLST_SEARCH_INVALID_KIDX: stop search because invalid key index
 *      L_DLST_SEARCH_INVALID_INPUT: return error while indexed_dblist is NULL pointer
 *
 * NOTES:
 *
 */
UI32_T L_DLST_ShMem_Indexed_Dblist_Search(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist,
                                    UI32_T cookie,
                                    UI32_T (*call_back) (UI16_T index, UI32_T cookie))
{
    /* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Buffer_T  *dblist;
    L_DLST_ShMem_Dblist_Element_T *element;
    UI16_T record_index, next_record_index;
    /* BODY
     */
    if (indexed_dblist == NULL ||
        indexed_dblist->dblist_offset == 0)
        return L_DLST_SEARCH_INVALID_INPUT;
	
	dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);
	element = dblist->element;

    if (element == NULL)
        return L_DLST_SEARCH_INVALID_INPUT;

    /* List is empty */
    if (indexed_dblist->head_index == L_DLST_NULL_INDEX)
	    return L_DLST_SEARCH_END_OF_LIST;
	
	record_index = indexed_dblist->head_index;
	
	while (1)
	{	
	    if ( record_index == L_DLST_NULL_INDEX )
            return L_DLST_SEARCH_END_OF_LIST;
            next_record_index = element[record_index].next_index;
            if ((*call_back)(record_index, cookie) == L_DLST_SEARCH_BREAK)
                return L_DLST_SEARCH_BREAK;
            record_index = next_record_index;
        }


}/* L_DLST_ShMem_Indexed_Dblist_Search */

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_GetActualDblistDiscriptor
 * PURPOSE:
 *      Get the actual index_dblist by input index
 *
 * INPUT:
 *      indexed_dblist: the original indexed_dblist to be cloned from.
 *      index         : the input index to search for the actual indexed_dblist
 *      actual_indexed_dblist : the address to store the indexed_dblist which
 *                                  contains the element with the input index.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- successfully
 *      FALSE -- fail
 *
 * NOTES:
 *      1. The user has the responsibility to allocate the memory
 *          for indexed_dblist.
 *
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_GetActualDblistDiscriptor(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI32_T index, L_DLST_ShMem_Indexed_Dblist_T **actual_indexed_dblist)
{
	/* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Element_T *element;
    L_DLST_ShMem_Dblist_Buffer_T  *dblist;
    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);
    if (dblist == NULL || index > (dblist->max_number_of_items_count-1))
        return FALSE;

    element = dblist->element;
    if (element == NULL)
        return FALSE;

    if (element[index].dblist_id == 0)
        return FALSE;
  *actual_indexed_dblist = L_CVRT_GET_PTR(dblist, element[index].dblist_id);
    return TRUE;

}/* end of L_DLST_ShMem_Indexed_Dblist_GetActualDblistDiscriptor */

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_DeleteAll_ByListPointerArray
 * PURPOSE:
 *      Delete all entries in the list.
 *
 * INPUT:
 *      group_dblist: pointer array of a grouped dblists which need to be cleared.
 *                          all elements in the pointed dblist are deleted & all pointed descriptors in the group (main & cloned) are initialized.
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- the list is cleared.
 *      FALSE -- this is not a list.
 *
 * NOTES:
 *      1. if the list is already empty, return TRUE.
 *
 *      2.------------------------
 *        | A | B | C | D | E | F |  .... address of descriptor
 *        --|---|---|---|---|---|-
 *          V   V   V   V   V   V
 *         [_] [_] [_] [_] [_] [_]   .... descriptor
 *          |   |   |   |   |   |
 *          V   V   V   V   V   V
 *         |-| |-| |-| |-| |-| |-|   .... dlist
 *         |-| |-| |-| |-| |-| |-|
 *         |-| |-| |-| |-| |-| |-|
 *         |_| |_| |_| |_| |_| |_|
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_DeleteAll_ByListPointerArray(L_DLST_ShMem_Indexed_Dblist_T  *group_dblist[])
{
    /* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Buffer_T *dblist;
    UI32_T i, clone_count;
    UI32_T max_number_of_items;

    /* BODY
     */
    if (group_dblist[0] == NULL)
        return FALSE;

    /* getting clone_count */
    dblist = L_DLST_SHMEM_GET_DBLIST(group_dblist[0]);
    if (dblist == NULL)
        return FALSE;
    clone_count = dblist->clone_count;
    /* getting max_number_of_items */
    max_number_of_items = dblist->max_number_of_items_count;

    /* Init each element */
    for (i=0; i<max_number_of_items; i++)
    {
        dblist->element[i].dblist_id         = 0;
        dblist->element[i].next_index        = L_DLST_NULL_INDEX;
        dblist->element[i].previous_index    = L_DLST_NULL_INDEX;
    }
    /* Init each dblist descriptor */
    for ( i=0; i< clone_count; i++)
    {
        group_dblist[i]->head_index   = L_DLST_NULL_INDEX;
        group_dblist[i]->tail_index   = L_DLST_NULL_INDEX;
    }
    return TRUE;

}

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_DeleteAll_ByListArray
 * PURPOSE:
 *      Delete all entries in the list.
 *
 * INPUT:
 *      group_dblist: the group dblist which is to be cleared.
 *          all elements in dblist are deleted & all descriptors in the group (main & cloned) are initialized.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE  -- the list is cleared.
 *      FALSE -- this is not a list.
 *
 * NOTES:
 *      1. if the list is already empty, return TRUE.
 *
 *      2.------------------------
 *        | A | B | C | D | E | F |  .... descriptor
 *        --|---|---|---|---|---|-
 *          V   V   V   V   V   V
 *         |-| |-| |-| |-| |-| |-|   .... dlist
 *         |-| |-| |-| |-| |-| |-|
 *         |-| |-| |-| |-| |-| |-|
 *         |_| |_| |_| |_| |_| |_|
 *
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_DeleteAll_ByListArray(L_DLST_ShMem_Indexed_Dblist_T  group_dblist[])
{
    /* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    L_DLST_ShMem_Dblist_Buffer_T *dblist;
    UI32_T i, clone_count;
    UI32_T max_number_of_items;

    /* BODY
     */
    if (group_dblist == NULL)
        return FALSE;

    /* getting clone_count */

    dblist = L_DLST_SHMEM_GET_DBLIST(&group_dblist[0]);
    if (dblist == NULL)
        return FALSE;
    clone_count = dblist->clone_count;
    /* getting max_number_of_items */
    max_number_of_items = dblist->max_number_of_items_count;

    /* Init each element */
    for (i=0; i<max_number_of_items; i++)
    {
        dblist->element[i].dblist_id         = 0;
        dblist->element[i].next_index        = L_DLST_NULL_INDEX;
        dblist->element[i].previous_index    = L_DLST_NULL_INDEX;
    }

    /* Init each dblist descriptor */
    for ( i=0; i< clone_count; i++)
    {
        group_dblist[i].head_index   = L_DLST_NULL_INDEX;
        group_dblist[i].tail_index   = L_DLST_NULL_INDEX;
    }

    return TRUE;
}/* end of L_DLST_Indexed_Dblist_DeleteAll */

/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_SetEntryID
 * PURPOSE:
 *      Set entry's ID.
 *
 * INPUT:
 *              indexed_dblist: which link list
 *              index:the specific entry to set ID
 *              dlist_id: id
 * OUTPUT:
 *              none
 *
 * RETURN:
 *      TRUE  -- Set ID successfully.
 *      FALSE -- Set ID failed.
 *
 * NOTES :
 *      If you really need to change element's id value you need to take care of
 *      this element. You can't call Dblist_GetActualDblistDiscriptor since it
 *      may have unexpected exception.
 */

BOOL_T L_DLST_ShMem_Indexed_Dblist_SetEntryID(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T index, UI32_T dlist_id)
{
 /* LOCAL CONSTANT DECLARATIONS
     */
    L_DLST_ShMem_Dblist_Element_T *element;
    L_DLST_ShMem_Dblist_Buffer_T *dblist;
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);

    if (dblist == NULL || index > (dblist->max_number_of_items_count-1))
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    element[index].dblist_id = dlist_id ;

    return TRUE;
	
}


/* FUNCTION NAME : L_DLST_ShMem_Indexed_Dblist_GetEntryID
 * PURPOSE:
 *      Get entry's ID.
 *
 * INPUT:
 *              indexed_dblist: which link list
 *              index:the specific entry to set ID
 *
 * OUTPUT:
 *              dlist_id: id
 *
 * RETURN:
 *      TRUE  -- Get ID successfully.
 *      FALSE -- Get ID failed.
 */

BOOL_T L_DLST_ShMem_Indexed_Dblist_GetEntryID(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T index, UI32_T *dlist_id)
{
 /* LOCAL CONSTANT DECLARATIONS
     */
    L_DLST_ShMem_Dblist_Element_T *element;
    L_DLST_ShMem_Dblist_Buffer_T *dblist;
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY
     */
    if (indexed_dblist == NULL ||
        index == L_DLST_NULL_INDEX)
        return FALSE;

    dblist = L_DLST_SHMEM_GET_DBLIST(indexed_dblist);

    if (dblist == NULL || index > (dblist->max_number_of_items_count-1))
        return FALSE;

    element = dblist->element;

    if (element == NULL)
        return FALSE;

    *dlist_id = element[index].dblist_id ;

    return TRUE;
	
}


/* LOCAL SUBPROGRAM
 */


#ifdef L_DLIST_DEBUG

static  L_DLST_Indexed_Dblist_T  indexed_dblist1, indexed_dblist2;
static  L_DLST_Indexed_Dblist_T  *indexed_dblist;
static  L_DLST_Indexed_Dblist_T *actual_indexed_dblist;
static  UI32_T max_number_of_items = 5;


static UI32_T L_DLIST_DEBUG_Callback(UI32_T idx, UI32_T cookie)
{
	printf("\n Search Callback: index =%lu, cookie =%lu", idx, cookie);
    return L_DLST_SEARCH_CONTINUE;
} /* end of L_HASH_SearchQueryGroup_Callback() */

void L_DLIST_DEBUG_main()
{


   BOOL_T   ret;
   UI32_T   ret_val;
   char str[3];
   UI32_T index;
   int  i;


     puts("\n****************************");
     puts("\r*  [C]reat index dblist    *");
     puts("\r*  Set to [h]ead           *");
     puts("\r*  Set to [t]ail           *");
     puts("\r*  [G]et: dequeue          *");
     puts("\r*  [D]elete one element    *");
     puts("\r*  Delete [a]ll elements   *");
     puts("\r*  Cl[o]ne dblist.         *");
	 puts("\r*  Get [l]ink node.        *");
	 puts("\r*  [S]each.                  *");
     puts("\r*  E[x]it dblist.          *");
     puts("\r****************************");

   while (1)
   {
      switch ( getch() )
      {
      case 'M':
      case 'm':
        puts("\n[c]reat, set to [h]ead, set to [t]ail,");
        puts("\r[g]et, [d]elete one, delete [a]ll, cl[o]ne, e[x]it");

         continue;

      case 'D':
      case 'd':
delete_index_choose:
        printf("\nChoose dblist1 or dblist2 [1,2]:");
        if (gets(str) == NULL)
    	{
    		goto delete_index_choose;
    	} /* end if gets(index) */
    	else
    	{
    		if (strcmp(str,"") == 0)//press ENTER		
    		indexed_dblist = &indexed_dblist1;	
    	}
    	
    	printf ("%s\n",str);
    	i = atoi(str);
    	switch(i)
    	{
    	    case 1:
    	        indexed_dblist = &indexed_dblist1;	
    	        break;
    	
    	    case 2:
    	        indexed_dblist = &indexed_dblist2;	
    	        break;
    	
    	    default:
    	        printf("\nInvalid input!");
    	        goto delete_index_choose;
        }

delete_index:
        printf("\nInput index to delete: index=");
        if (gets(str) == NULL)
    	{
    		goto delete_index;
    	} /* end if gets(index) */
    	else
    	{
    		if (strcmp(str,"") == 0)
    			goto delete_index; //press ENTER			
    	}
    	
    	printf ("%s\n",str);

		index = atol(str);
        ret = L_DLST_Indexed_Dblist_DeleteEntry(indexed_dblist, index);

        if (!ret)
            printf("\nDelete failed!");

        ungetch( 'm');
        continue;

      case 'A':
      case 'a':
delete_all_choose:
        printf("\nChoose dblist1 or dblist2[1,2]:");
        if (gets(str) == NULL)
    	{
    		goto delete_all_choose;
    	} /* end if gets(index) */
    	else
    	{
    		if (strcmp(str,"") == 0)//press ENTER		
    		indexed_dblist = &indexed_dblist1;	
    	}
    	
    	printf ("%s\n",str);
    	i = atoi(str);
    	switch(i)
    	{
    	    case 1:
    	        indexed_dblist = &indexed_dblist1;	
    	        break;
    	
    	    case 2:
    	        indexed_dblist = &indexed_dblist2;	
    	        break;
    	
    	    default:
    	        printf("\nInvalid input!");
    	        goto delete_all_choose;
        }

        ret = L_DLST_Indexed_Dblist_DeleteAll_ByListArray(indexed_dblist);

        if (!ret)
            printf("\nDelete All failed!");

        ungetch( 'm');
        continue;

      case 'C':
      case 'c':
      indexed_dblist = &indexed_dblist1;
        ret = L_DLST_Indexed_Dblist_Create(indexed_dblist, max_number_of_items);

        if (!ret)
            printf("\nCreate failed!");
        else
        {
            printf("\nCreate successfully with max_number_of_items = %d",(int)max_number_of_items);
        }

         ungetch( 'm');
         continue;


      case 'g':
      case 'G':
get_choose:
        printf("\nChoose dblist1 or dblist2[1,2]:");
        if (gets(str) == NULL)
    	{
    		goto get_choose;
    	} /* end if gets(index) */
    	else
    	{
    		if (strcmp(str,"\r") == 0)//press ENTER		
    		indexed_dblist = &indexed_dblist1;	
    	}
    	
    	printf ("%s\n",str);
    	i = atoi(str);
    	switch(i)
    	{
    	    case 1:
    	        indexed_dblist = &indexed_dblist1;	
    	        break;
    	
    	    case 2:
    	        indexed_dblist = &indexed_dblist2;	
    	        break;
    	
    	    default:
    	        printf("\nInvalid input!");
    	        goto get_choose;
        }


         printf("\nGet index = ");

         ret = L_DLST_Indexed_Dblist_Dequeue(indexed_dblist, &index);;
         if (ret)
         {
             printf("%lu",index);
         }
         else
         {
            printf("\nNothing in Queue!");
         }

         ungetch( 'm');
         continue;

      case 'h':
      case 'H':
enQhead_choose:
        printf("\nChoose dblist1 or dblist2[1,2]:");
        if (gets(str) == NULL)
    	{
    		goto enQhead_choose;
    	} /* end if gets(index) */
    	else
    	{
    		if (strcmp(str,"") == 0)//press ENTER		
    		indexed_dblist = &indexed_dblist1;	
    	}
    	
    	printf ("%s\n",str);
    	i = atoi(str);
    	switch(i)
    	{
    	    case 1:
    	        indexed_dblist = &indexed_dblist1;	
    	        break;
    	
    	    case 2:
    	        indexed_dblist = &indexed_dblist2;	
    	        break;
    	
    	    default:
    	        printf("\nInvalid input!");
    	        goto enQhead_choose;
        }

get_msg1:
        printf("\n Key in index [0~n]:");
        if (gets(str) == NULL)
    	{
    		goto get_msg1;
    	} /* end if gets(msg_key) */
    	else
    	{
    		if (strcmp(str,"") == 0)
    			goto get_msg1; //press ENTER			
    	}
    	
    	printf ("%s\n",str);

		index = atol(str);
		if (index >= ((L_DLST_Dblist_Buffer_T *)indexed_dblist->dblist)->max_number_of_items_count)
            goto get_msg1;

         ret = L_DLST_Indexed_Dblist_Enqueue2Head(indexed_dblist, index);
         if (ret)
         {
            printf("\nSet successfully.");
         }
         else
         {
            printf("\nSet failed.");
         }
         ungetch( 'm');
         continue;

      case 't':
      case 'T':
enQtail_choose:
        printf("\nChoose dblist1 or dblist2[1,2]:");
        if (gets(str) == NULL)
    	{
    		goto enQtail_choose;
    	} /* end if gets(index) */
    	else
    	{
    		if (strcmp(str,"") == 0)//press ENTER		
    		indexed_dblist = &indexed_dblist1;	
    	}
    	
    	printf ("%s\n",str);
    	i = atoi(str);
    	switch(i)
    	{
    	    case 1:
    	        indexed_dblist = &indexed_dblist1;	
    	        break;
    	
    	    case 2:
    	        indexed_dblist = &indexed_dblist2;	
    	        break;
    	
    	    default:
    	        printf("\nInvalid input!");
    	        goto enQtail_choose;
        }

get_msg:
        printf("\n Key in index [0~n]:");
        if (gets(str) == NULL)
    	{
    		goto get_msg;
    	} /* end if gets(msg_key) */
    	else
    	{
    		if (strcmp(str,"") == 0)
    			goto get_msg; //press ENTER			
    	}
    	
    	printf ("%s\n",str);

		index = atol(str);
		if (index >= ((L_DLST_Dblist_Buffer_T *)indexed_dblist->dblist)->max_number_of_items_count)
            goto get_msg1;

         ret = L_DLST_Indexed_Dblist_Enqueue(indexed_dblist, index);
         if (ret)
         {
            printf("\nSet successfully.");
         }
         else
         {
            printf("\nSet failed.");
         }
         ungetch( 'm');
         continue;

      case 'o':
      case 'O':
        ret = L_DLST_Indexed_Dblist_Clone(&indexed_dblist1, &indexed_dblist2);
        if (ret)
         {
            printf("\nClone successfully.");
         }
         else
         {
            printf("\nClone failed.");
         }
         ungetch( 'm');
         continue;

	  case 's':
	  case 'S':
	  /* (UI32_T (*)(UI32_T, UI32_T))L_DLIST_DEBUG_Callback */
	     ret_val = L_DLST_Indexed_Dblist_Search(indexed_dblist,
                                            1,
                                            (UI32_T (*)(UI32_T, UI32_T))L_DLIST_DEBUG_Callback);

         if (ret_val == L_DLST_SEARCH_COMPLETED)
            printf("\n L_DLST_SEARCH_COMPLETED");
         else if (ret_val == L_DLST_SEARCH_END_OF_LIST)
            printf("\n L_DLST_SEARCH_END_OF_LIST");
         else
            printf("\n L_DLST_SEARCH invalid input");

        continue;

      case 'l':
      case 'L':
get_link:
        printf("\n Key in index [0~n]:");
        if (gets(str) == NULL)
    	{
    		goto get_link;
    	} /* end if gets(msg_key) */
    	else
    	{
    		if (strcmp(str,"") == 0)
    			goto get_link; //press ENTER			
    	}
    	
    	printf ("%s\n",str);

		index = atol(str);
        if (L_DLST_Indexed_Dblist_GetActualDblistDiscriptor(indexed_dblist, index, (L_DLST_Indexed_Dblist_T **)&actual_indexed_dblist)  == TRUE)
        {
            if (actual_indexed_dblist == &indexed_dblist1)
                printf("\nget link node> indexed_dblist1");
            else if (actual_indexed_dblist == &indexed_dblist2)
                printf("\nget link node> indexed_dblist2");
            else
                printf("\nget link node> error");

        }
        else
            printf("\nget link node> failed");

        continue;

      case 'x':
      case 'X':
         exit(1);

      default:
        ungetch( 'm');
        continue;
      }
   }/* end of while(1) */

}
#endif

