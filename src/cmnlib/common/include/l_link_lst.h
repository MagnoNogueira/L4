/*---------------------------------------------------------------------
 * File_Name : L_LINK_list.h
 *
 * Purpose   : Basically Link List operation
 *             1. link list can insert from the head or append to the last
 *             2. Caller must take care the mutual exculusion situation.
 *
 *
 *
 *
 *
 * Copyright(C)      Accton Corporation, 2002, 2003
 *
 * Note    : Designed for Mercury
 *---------------------------------------------------------------------
 */

#ifndef L_LINK_LST_H
#define L_LINK_LST_H

#include "sys_type.h"
#include "l_pt.h"


/* NAMING CONSTANT DECLARACTIONS
 */
enum
{
   L_LINK_LST_SEARCH_COMPLETED,
   L_LINK_LST_SEARCH_END_OF_LIST,
   L_LINK_LST_SEARCH_BREAK,
   L_LINK_LST_SEARCH_CONTINUE
};


enum
{

    L_LINK_LST_APPEND,
    L_LINK_LST_INSERT_FROM_HEAD
};


/* DATA TYPE DECLARACTIONS
 */
typedef struct
{
   UI32_T   max_element_count;
   UI32_T   nbr_of_element;
   UI32_T   size_of_element;
   void     *head;
   BOOL_T   (*compare)(void*inlist_element, void *input_element);
   BOOL_T   is_mem_prealloc;

   /* insternal use , not export to the user when call L_LINK_LST_Create
    */
   L_PT_Descriptor_T  desc;


} L_LINK_LST_List_T;


/* PURPOSE: Create a list
 * INPUT:   list     -- list
 *          max_element_count
 *          size_of_element
 *          compare
 *          is_mem_prealloc
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Create (L_LINK_LST_List_T *list,
                          UI32_T max_element_count,
                          UI32_T size_of_element,
                          BOOL_T  (*compare)(void*inlist_element, void *input_element),
                          BOOL_T is_mem_prealloc);


/* PURPOSE: delete a element from list
 * INPUT:   list     -- list to operation
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Delete (L_LINK_LST_List_T *list, void *element);


/* PURPOSE: delete all elements from list
 * INPUT:   list     -- list to operation
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Delete_All (L_LINK_LST_List_T *list);



/* PURPOSE: Get 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to get element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Get_1st (L_LINK_LST_List_T *list, void *element);


/* PURPOSE: Remove 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to store the removed element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Remove_1st (L_LINK_LST_List_T *list, void *element);



/* PURPOSE: Get element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Get (L_LINK_LST_List_T *list, void *element);






/* PURPOSE: Get next element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the next element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Get_Next(L_LINK_LST_List_T *list, void *element);




/* PURPOSE: Set a element in list, if that element not in list, insert it.
 * INPUT:   list     -- list to operation
 *          element  -- element to be set
 *          position  -- L_LINK_LST_APPEND,
 *                    -- L_LINK_LST_INSERT_FROM_HEAD
 *                    --
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 * NOTES :  if posiot == L_LINK_LST_AFTER, then put after perv
 */
BOOL_T L_LINK_LST_Set ( L_LINK_LST_List_T *list, void *element, UI32_T postion);



/* PURPOSE: Search next element from list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to search the next element.
 *                      if element is NULL(0) then search from 1st element.
 *          count    -- limited element count to be searched
 *          fun      -- The callback function to 'view' the element.
 *                      The return value of fun:
 *                         L_LINK_LST_SEARCH_BREAK: tell this function to break searching
 *                         L_LINK_LST_SEARCH_CONTINUE: tell this function continue searching
 *          cookie  --
 * OUTPUT:  none
 * RETURN:  L_LINK_LST_SEARCH_BREAK: Searching is broken by (*fun)
 *          L_LINK_LST_SEARCH_END_OF_LIST: stop search because end of list
 *          L_LINK_LST_SEARCH_COMPLETED: stop search because reach the limited count
 * NOTE:    Caller must take care the mutual exculusion situation.
 */
UI32_T L_LINK_LST_Search (L_LINK_LST_List_T *list,
                          void *element,
                          UI32_T (*fun)(const void *element, UI32_T cookie),
                          UI32_T count,
                          UI32_T cookie);





/* PURPOSE: Set a element in list, if that element not in list, insert it.
 * INPUT:   list     -- list to operation
 *          element  -- element to be set
 *          position  -- L_LINK_LST_APPEND,
 *                    -- L_LINK_LST_INSERT_FROM_HEAD
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Destroy ( L_LINK_LST_List_T *list);


#endif /* L_LINK_LST_H */

