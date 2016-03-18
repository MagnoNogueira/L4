#ifndef L_SORT_LST_H
#define L_SORT_LST_H

#include "sys_type.h"
#include "l_pt.h"

/* NAMING CONSTANT DECLARACTIONS
 */
enum
{
   L_SORT_LST_SEARCH_COMPLETED,
   L_SORT_LST_SEARCH_END_OF_LIST,
   L_SORT_LST_SEARCH_BREAK,
   L_SORT_LST_SEARCH_CONTINUE
};

/*---------------------------------
 * Constants for shared memory version
 *---------------------------------
 */
/* The type of compare function for the sorted list in shared memory.
 */
enum L_SORT_LST_SHMEM_COMPARE_FUNC_ID_E {
#if (SYS_CPNT_SWDRVL3_CACHE == TRUE)
    L_SORT_LST_SHMEM_COMPARE_FUNC_ID_SWDRVL3_CACHE_OM_ADD,
    L_SORT_LST_SHMEM_COMPARE_FUNC_ID_SWDRVL3_CACHE_OM_DEL,
#endif
    L_SORT_LST_SHMEM_COMPARE_FUNC_ID_NETCFG_OM_IP,
    L_SORT_LST_SHMEM_COMPARE_FUNC_ID_MAX
};



/* DATA TYPE DECLARACTIONS
 */
/* DATA TYPE DECLARACTIONS 
 */
  /*maggie liu, Merge from ECN430, ES3628BT-FLF-ZZ-00551*/
typedef struct SORT_LST
{
   UI32_T   max_element_count;
   UI32_T   nbr_of_element;
   UI32_T   size_of_element; 
   void     *head;
   int      (*compare)(void*inlist_element, void *input_element);
} L_SORT_LST_List_T;



/*---------------------------------
 * Types for shared memory version
 *---------------------------------
 */
typedef struct
{
    I32_T    next_offset; /* the offset with list_desc as base addr. */
} L_SORT_LST_ShMem_Slist_T;

typedef struct
{
    UI32_T   nbr_of_element;
    UI32_T   size_of_element;
    I32_T    head_offset; /* the offset with list_desc as base addr. */
    UI32_T   compare_func_id;
    /* internal use
     */
    L_PT_ShMem_Descriptor_T  ptdesc;
} L_SORT_LST_ShMem_List_T;



/* MACRO FUNCTION DECLARACTIONS
 */
/*---------------------------------
 * Macro for shared memory version
 *---------------------------------
 */
#define L_SORT_LST_SHMEM_ONE_PARTITION_SZ(size_of_element) \
    L_ALIGN(sizeof(L_SORT_LST_ShMem_Slist_T)+(size_of_element),4)

#define L_SORT_LST_SHMEM_PT_BUFFER_REQUIRED_SZ(max_element_count, size_of_element) \
    ((max_element_count) * L_SORT_LST_SHMEM_ONE_PARTITION_SZ(size_of_element))

/* PURPOSE: Calculate the required size of the buffer for a list
 * INPUT:   max_element_count
 *          size_of_element
 * OUTPUT:  None
 * RETURN:  he required size
 */
#define L_SORT_LST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(max_element_count, size_of_element) \
    L_SORT_LST_SHMEM_PT_BUFFER_REQUIRED_SZ(max_element_count, size_of_element)



/* EXPORTED FUNCTION DECLARACTIONS
 */
/* PURPOSE: Create a list
 * INPUT:   list     -- list
 *          max_element_count
 *          size_of_element
 *          compare
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Create (
   L_SORT_LST_List_T *list,
   UI32_T max_element_count,
   UI32_T size_of_element,
   int  (*compare)(void*inlist_element, void *input_element)
   );


/* PURPOSE: delete a element from list
 * INPUT:   list     -- list to operation
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Delete (L_SORT_LST_List_T *list, void *element);


/* PURPOSE: delete all elements from list
 * INPUT:   list     -- list to operation
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Delete_All (L_SORT_LST_List_T *list);



/* PURPOSE: Get 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to get element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Get_1st (L_SORT_LST_List_T *list, void *element);


/* PURPOSE: Remove 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to store the removed element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Remove_1st (L_SORT_LST_List_T *list, void *element);



/* PURPOSE: Get element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Get (L_SORT_LST_List_T *list, void *element);



/* PURPOSE: Get next element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the next element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Get_Next (L_SORT_LST_List_T *list, void *element);



/* PURPOSE: Get the address of the element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the address of the element.
 * OUTPUT:  none
 * RETURN:  the address of the element and NULL on failure.
 */
void *L_SORT_LST_GetPtr (L_SORT_LST_List_T *list, void *element);



/* PURPOSE: Set a element in list, if that element not in list, insert it.
 * INPUT:   list     -- list to operation
 *          element  -- element to be set
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Set ( L_SORT_LST_List_T *list, void *element);



/* PURPOSE: Search next element from list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to search the next element.
 *                      if element is NULL(0) then search from 1st element.
 *          count    -- limited element count to be searched
 *          fun      -- The callback function to 'view' the element.
 *                      The return value of fun:
 *                         L_SORT_LST_SEARCH_BREAK: tell this function to break searching
 *                         L_SORT_LST_SEARCH_CONTINUE: tell this function continue searching
 * OUTPUT:  none
 * RETURN:  L_SORT_LST_SEARCH_BREAK: Searching is broken by (*fun)
 *          L_SORT_LST_SEARCH_END_OF_LIST: stop search because end of list
 *          L_SORT_LST_SEARCH_COMPLETED: stop search because reach the limited count
 * NOTE:    Caller must take care the mutual exculusion situation.
 */
UI32_T L_SORT_LST_Search (
L_SORT_LST_List_T *list, void *element, UI32_T (*fun)(const void *element), UI32_T count);


/*---------------------------------
 * Exported functions for shared memory version
 *---------------------------------
 */
/* PURPOSE: Create a list
 * INPUT:   list           -- the buffer of list in shared memory
 *          working_buffer -- the working buffer for the list in shared memory
 *                            the size of buffer must be L_SORT_LST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ()
 *          max_element_count
 *          size_of_element
 *          compare_func_id -- the type of compare function to use
 * OUTPUT:  list           -- the list descriptor
 * RETURN:  TRUE/FALSE
 * NOTE:    the descriptor and working buffer must be in shared memory
 */
BOOL_T L_SORT_LST_ShMem_Create (
    L_SORT_LST_ShMem_List_T *list,
    void *working_buffer,
    UI32_T max_element_count,
    UI32_T size_of_element,
    UI32_T compare_func_id);


/* PURPOSE: delete a element from list
 * INPUT:   list     -- list to operation
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Delete (L_SORT_LST_ShMem_List_T *list, void *element);


/* PURPOSE: delete all elements from list
 * INPUT:   list     -- list to operation
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Delete_All (L_SORT_LST_ShMem_List_T *list);



/* PURPOSE: Get 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to get element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Get_1st (L_SORT_LST_ShMem_List_T *list, void *element);


/* PURPOSE: Remove 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to store the removed element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Remove_1st (L_SORT_LST_ShMem_List_T *list, void *element);



/* PURPOSE: Get element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Get (L_SORT_LST_ShMem_List_T *list, void *element);



/* PURPOSE: Get next element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the next element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Get_Next (L_SORT_LST_ShMem_List_T *list, void *element);



/* PURPOSE: Get the address of the element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the address of the element.
 * OUTPUT:  none
 * RETURN:  the address of the element and NULL on failure.
 */
void *L_SORT_LST_ShMem_GetPtr (L_SORT_LST_ShMem_List_T *list, void *element);



/* PURPOSE: Set a element in list, if that element not in list, insert it.
 * INPUT:   list     -- list to operation
 *          element  -- element to be set
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Set ( L_SORT_LST_ShMem_List_T *list, void *element);



/* PURPOSE: Search next element from list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to search the next element.
 *                      if element is NULL(0) then search from 1st element.
 *          count    -- limited element count to be searched
 *          fun      -- The callback function to 'view' the element.
 *                      The return value of fun:
 *                         L_SORT_LST_SEARCH_BREAK: tell this function to break searching
 *                         L_SORT_LST_SEARCH_CONTINUE: tell this function continue searching
 * OUTPUT:  none
 * RETURN:  L_SORT_LST_SEARCH_BREAK: Searching is broken by (*fun)
 *          L_SORT_LST_SEARCH_END_OF_LIST: stop search because end of list
 *          L_SORT_LST_SEARCH_COMPLETED: stop search because reach the limited count
 * NOTE:    Caller must take care the mutual exculusion situation.
 */
UI32_T L_SORT_LST_ShMem_Search (
L_SORT_LST_ShMem_List_T *list, void *element, UI32_T (*fun)(const void *element), UI32_T count);


#endif /* L_SORT_LST_H */

