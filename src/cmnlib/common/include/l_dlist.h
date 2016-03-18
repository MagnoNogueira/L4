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
 *
 * History:
 *       Date       -- Modifier,  Reason
 *  0.1 2003.12.18  --  William, Created
 *
 * Copyright(C)      Accton Corporation, 2003
 */

#ifndef _L_DLST_H
#define _L_DLST_H


/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */
enum
{
   L_DLST_SEARCH_COMPLETED=0,
   L_DLST_SEARCH_END_OF_LIST,
   L_DLST_SEARCH_BREAK,
   L_DLST_SEARCH_CONTINUE,
   L_DLST_SEARCH_INVALID_KIDX,
   L_DLST_SEARCH_INVALID_INPUT
};


/* DATA TYPE DECLARATIONS
 */
typedef struct L_DLST_Indexed_Dblist_S
{
    UI16_T    head_index;                   /* first index of this dblist        */
    UI16_T    tail_index;                   /* last  index of this dblist        */
    void      *dblist;                      /* dblist buffer        */
}   L_DLST_Indexed_Dblist_T;

/*---------------------------------
 * Types for shared memory version
 *---------------------------------
 */
typedef struct L_DLST_ShMem_Dblist_Element_S
{
    UI16_T next_index;
    UI16_T previous_index;
    I32_T  dblist_id;  /* it's like a flag to show whether this element is used, it also saves the offset of the address of its descriptor L_DLST_ShMem_Indexed_Dblist_T. */
}L_DLST_ShMem_Dblist_Element_T;

typedef struct L_DLST_ShMem_Dblist_Buffer_S
{
    UI32_T max_number_of_items_count;    /* max number of data items this dblist can handle */
    UI32_T clone_count;
    L_DLST_ShMem_Dblist_Element_T element[1];
}L_DLST_ShMem_Dblist_Buffer_T;

typedef struct L_DLST_ShMem_Indexed_Dblist_S
{
    UI16_T    head_index;                   /* first index of this dblist        */
    UI16_T    tail_index;                   /* last  index of this dblist        */
    I32_T     dblist_offset;                /* the relative address of the working buffer with desc_p as base addr */

}   L_DLST_ShMem_Indexed_Dblist_T;


/* MACRO FUNCTION DECLARATIONS
 */
/*---------------------------------
 * Macro for shared memory version
 *---------------------------------
 */
/* MACRO NAME : L_DLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ
 * PURPOSE:
 *      Calculate the required size of the buffer for a double linked list
 *
 * INPUT:
 *      max_number_of_items: max number of items can be handled by this list.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      The required size.
 *
 * NOTES:
 *      None.
 */
#define L_DLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(max_number_of_items) \
    ((UI32_T)(((L_DLST_ShMem_Dblist_Buffer_T *)0)->element) + \
        sizeof(L_DLST_ShMem_Dblist_Element_T) * (max_number_of_items))



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
BOOL_T L_DLST_Indexed_Dblist_Create( L_DLST_Indexed_Dblist_T  *indexed_dblist, UI32_T max_number_of_items);


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
void   L_DLST_Indexed_Dblist_Destroy (L_DLST_Indexed_Dblist_T *indexed_dblist);


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
BOOL_T L_DLST_Indexed_Dblist_Enqueue2Head (L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T index);


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
BOOL_T L_DLST_Indexed_Dblist_Enqueue (L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T index);

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
BOOL_T L_DLST_Indexed_Dblist_GetFirstEntry(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T *index);

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
BOOL_T L_DLST_Indexed_Dblist_GetNextEntry(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T *index);

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
BOOL_T L_DLST_Indexed_Dblist_Dequeue (L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T *index);

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
BOOL_T L_DLST_Indexed_Dblist_DeleteAll_ByListPointerArray(L_DLST_Indexed_Dblist_T  *group_dblist[]);

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
BOOL_T   L_DLST_Indexed_Dblist_DeleteAll_ByListArray (L_DLST_Indexed_Dblist_T  group_dblist[]);

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
 */

BOOL_T L_DLST_Indexed_Dblist_SetEntryID(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T index, UI32_T dlist_id);

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
BOOL_T L_DLST_Indexed_Dblist_GetEntryID(L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T index, UI32_T *dlist_id);

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
BOOL_T L_DLST_Indexed_Dblist_DeleteEntry (L_DLST_Indexed_Dblist_T *indexed_dblist, UI16_T index);

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
BOOL_T L_DLST_Indexed_Dblist_Clone(L_DLST_Indexed_Dblist_T *clone_from, L_DLST_Indexed_Dblist_T *clone_to);

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
                                    UI32_T (*call_back) (UI16_T index, UI32_T cookie));
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
BOOL_T L_DLST_Indexed_Dblist_GetActualDblistDiscriptor(L_DLST_Indexed_Dblist_T *indexed_dblist, UI32_T index, L_DLST_Indexed_Dblist_T **actual_indexed_dblist);



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
 *      3. the descriptor and working buffer must be in shared memory
 */
BOOL_T L_DLST_ShMem_Indexed_Dblist_Create(L_DLST_ShMem_Indexed_Dblist_T  *indexed_dblist, void *working_buffer, UI32_T max_number_of_items);


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
void   L_DLST_ShMem_Indexed_Dblist_Destroy (L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist);


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
BOOL_T L_DLST_ShMem_Indexed_Dblist_Enqueue2Head (L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T index);


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
BOOL_T L_DLST_ShMem_Indexed_Dblist_Enqueue (L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T index);

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
BOOL_T L_DLST_ShMem_Indexed_Dblist_GetFirstEntry(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T *index);

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
BOOL_T L_DLST_ShMem_Indexed_Dblist_GetNextEntry(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T *index);

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
BOOL_T L_DLST_ShMem_Indexed_Dblist_Dequeue (L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T *index);

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
BOOL_T L_DLST_ShMem_Indexed_Dblist_DeleteAll_ByListPointerArray(L_DLST_ShMem_Indexed_Dblist_T  *group_dblist[]);

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
BOOL_T   L_DLST_ShMem_Indexed_Dblist_DeleteAll_ByListArray (L_DLST_ShMem_Indexed_Dblist_T  group_dblist[]);

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
 */

BOOL_T L_DLST_ShMem_Indexed_Dblist_SetEntryID(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T index, UI32_T dlist_id);

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
BOOL_T L_DLST_ShMem_Indexed_Dblist_GetEntryID(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T index, UI32_T *dlist_id);

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
BOOL_T L_DLST_ShMem_Indexed_Dblist_DeleteEntry (L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI16_T index);

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
BOOL_T L_DLST_ShMem_Indexed_Dblist_Clone(L_DLST_ShMem_Indexed_Dblist_T *clone_from, L_DLST_ShMem_Indexed_Dblist_T *clone_to);

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
                                    UI32_T (*call_back) (UI16_T index, UI32_T cookie));

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
BOOL_T L_DLST_ShMem_Indexed_Dblist_GetActualDblistDiscriptor(L_DLST_ShMem_Indexed_Dblist_T *indexed_dblist, UI32_T index, L_DLST_ShMem_Indexed_Dblist_T **actual_indexed_dblist);

#endif  /*  end of _L_DLST_H   */
