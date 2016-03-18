#ifndef L_LST_H
#define L_LST_H

#include "sys_type.h"
#include "l_pt.h"

/* NAMING CONSTANT DECLARACTIONS
 */
enum
{
    L_LST_SEARCH_COMPLETED,
    L_LST_SEARCH_END_OF_QUEUE,
    L_LST_SEARCH_BREAK,
    L_LST_SEARCH_CONTINUE
};

/* DATA TYPE DECLARACTIONS
 */
typedef struct
{
    UI32_T   nbr_of_element;
    UI32_T   size_of_element;
    void     *head;
    void     *tail;
    int      (*compare)(void *inlist_element, void *input_element);

   /* insternal use
    */
   L_PT_Descriptor_T  ptdesc;
} L_LST_List_T;



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
BOOL_T L_LST_Create (L_LST_List_T  *list,
                     UI32_T        max_element_count,
                     UI32_T        size_of_element,
                     int (*compare)(void*inlist_element, void *input_element)
                    );


/* PURPOSE: delete an element from list
 * INPUT:   list     -- list to operate
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LST_Delete (L_LST_List_T *list, void *element);


/* PURPOSE: delete all elements from list
 * INPUT:   list     -- list to operate
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LST_DeleteAll (L_LST_List_T *list);


/* PURPOSE: Get 1st element in list
 * INPUT:   list     -- list to operate
 * OUTPUT:  element  -- buffer to get element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LST_GetFirst (L_LST_List_T *list, void *element);


/* PURPOSE: Remove 1st element in list
 * INPUT:   list     -- list to operate
 * OUTPUT:  element  -- buffer to store the removed element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LST_RemoveFirst (L_LST_List_T *list, void *element);


/* PURPOSE: Get element in list
 * INPUT:   list     -- list to operate
 *          element  -- according the "key" in element to get other fields of element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 * NOTE:    "key" is specified and used in compare function
 */
BOOL_T L_LST_Get (L_LST_List_T *list, void *element);



/* PURPOSE: Get next element in list
 * INPUT:   list    -- list to operate
 *          element  -- according the "key" in element to get the next element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 * NOTE:    "key" is specified and used in compare function
 */
BOOL_T L_LST_GetNext (L_LST_List_T *list, void *element);


/* PURPOSE: Set an element in list, if that element not in list, append it.
 * INPUT:   list     -- list to operate
 *          element  -- element to be set
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LST_Set (L_LST_List_T *list, void *element);


#endif /* L_LST_H */

