/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys_type.h"
#include "sys_cpnt.h"
#include "l_cvrt.h"
#include "l_sort_lst.h"


/* NAMING CONSTANT DECLARATIONS
 */
/*---------------------------------
 * Constants for shared memory version
 *---------------------------------
 */
#define L_SORT_LST_SHMEM_NBR_OF_COMPARE_FUNC (sizeof(shmem_compare_func) / sizeof(shmem_compare_func[0]))


/* DATA TYPE DECLARACTIONS
 */
/* SLIST mean a sorted list  */
 typedef struct Slist_Tag
{
    struct Slist_Tag   *next;
    void               *element;
} Slist_T;


/*---------------------------------
 * Types for shared memory version
 *---------------------------------
 */
typedef int (*L_SORT_LST_ShMem_CompareFunc_T)(void *inlist_element, void *input_element);



/* MACRO FUNCTION DECLARACTIONS
 */
#define L_SORT_Malloc()     L_PT_Allocate(&(list->ptdesc))
#define L_SORT_Free(buf_p)  L_PT_Free(&(list->ptdesc), buf_p)

#define L_SORT_GET_USERDATA(Slist_p) ((Slist_T*)Slist_p->element)  /*maggie liu, Merge from ECN430, ES3628BT-FLF-ZZ-00551*/

/*---------------------------------
 * Macro for shared memory version
 *---------------------------------
 */
#define L_SORT_LST_ShMem_Malloc()     L_PT_ShMem_Allocate(&list->ptdesc)
#define L_SORT_LST_ShMem_Free(buf_p)  L_PT_ShMem_Free(&list->ptdesc, (buf_p))
#define L_SORT_LST_SHMEM_GET_USERDATA(node) ((void *)((L_SORT_LST_ShMem_Slist_T*)(node)+1))
#define L_SORT_LST_SHMEM_GET_HEAD(list) ((list)->head_offset == 0 ? NULL : (L_SORT_LST_ShMem_Slist_T *)L_CVRT_GET_PTR((list), (list)->head_offset))
#define L_SORT_LST_SHMEM_GET_NEXT(list, node) ((node)->next_offset == 0 ? NULL : (L_SORT_LST_ShMem_Slist_T *)L_CVRT_GET_PTR((list), (node)->next_offset))



/* LOCAL FUNCTIONS DECLARACTIONS
 */
static Slist_T *L_SORT_LST_Find (
   L_SORT_LST_List_T *list,  void *in_element,
   BOOL_T get_next, Slist_T **pre_node);

/*---------------------------------
 * Local functions for shared memory version
 *---------------------------------
 */
static L_SORT_LST_ShMem_Slist_T *L_SORT_LST_ShMem_Find (
   L_SORT_LST_ShMem_List_T *list,  void *in_element,
   BOOL_T get_next, L_SORT_LST_ShMem_Slist_T **pre_node);

/*---------------------------------
 * External functions for shared memory version
 *---------------------------------
 */
#if (SYS_CPNT_SWDRVL3_CACHE == TRUE)
extern int SWDRVL3_CACHE_OM_AddCompare(void *elm1, void *elm2);
extern int SWDRVL3_CACHE_OM_DelCompare(void *elm1, void *elm2);
#endif
extern int NETCFG_OM_IP_CompareRif(void* rec1, void* rec2);


/* LOCAL VARIABLE DECLARATIONS
 */
/*---------------------------------
 * Local variables for shared memory version
 *---------------------------------
 */
static L_SORT_LST_ShMem_CompareFunc_T shmem_compare_func[] =
{
#if (SYS_CPNT_SWDRVL3_CACHE == TRUE)
    SWDRVL3_CACHE_OM_AddCompare, /* L_SORT_LST_SHMEM_COMPARE_FUNC_ID_SWDRVL3_CACHE_OM_ADD */
    SWDRVL3_CACHE_OM_DelCompare, /* L_SORT_LST_SHMEM_COMPARE_FUNC_ID_SWDRVL3_CACHE_OM_DEL */
#endif
    //NETCFG_OM_IP_CompareRif, /* L_SORT_LST_SHMEM_COMPARE_FUNC_ID_NETCFG_OM_IP */
};

/* EXPORTED FUNCTIONS BODIES
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
   )
{
   list->compare = compare;
   list->head = 0 ;
   list->nbr_of_element = 0;
   list->max_element_count = max_element_count;
   list->size_of_element = size_of_element;
   return TRUE;
} /* L_SORT_LST_Create */




/* PURPOSE: delete a element from list
 * INPUT:   list     -- list to operation
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Delete (L_SORT_LST_List_T *list, void *element)
{
   Slist_T  *pre_node;
   Slist_T  *this_node;

   /* BODY
    */
   this_node = L_SORT_LST_Find (list,  element, FALSE, &pre_node);

   if ( this_node == 0 )
      return FALSE;

   if ( pre_node == 0 )
   {  /* remove 1st element */
      list->head = ((Slist_T*)list->head) ->next;
   }
   else
   {
      pre_node->next = pre_node->next->next;
   }

   list->nbr_of_element--;
   free (this_node->element);
   free (this_node);

   return TRUE;

} /* L_SORT_LST_Delete */


/* PURPOSE: delete all element from list
 * INPUT:   list     -- list to operation
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Delete_All (L_SORT_LST_List_T *list)
{
   Slist_T  *node, *next_node;

   /* BODY
    */
   for ( node = (Slist_T*) list->head; node; node = next_node )
   {
      free (node->element);
      next_node = node->next;
      free (node);
   }
   list->nbr_of_element = 0;
   list->head = (Slist_T*)0;
   return TRUE;
} /* L_SORT_LST_Delete_All */



/* PURPOSE: Get 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to get element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Get_1st (L_SORT_LST_List_T *list, void *element)
{
   /* BODY
    */
   if ( list->head )
   {
      memcpy (element, ((Slist_T*)list->head) ->element, list->size_of_element);
      return TRUE;
   }
   return FALSE;
} /* L_SORT_LST_Get_1st */

/* PURPOSE: Remove 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to store the removed element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Remove_1st (L_SORT_LST_List_T *list, void *element)
{
   Slist_T  *node;

   node = list->head;
   if ( node )
   {
      memcpy (element, node ->element, list->size_of_element);
      list->head = node->next;
      free (node->element);
      free (node);
      list->nbr_of_element--;
      return TRUE;
   }
   return FALSE;
} /* L_SORT_LST_Remove_1st */

/* PURPOSE: Get element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Get (L_SORT_LST_List_T *list, void *element)
{
   Slist_T  *pre_node;
   Slist_T  *this_node;

   /* BODY
    */
   this_node = L_SORT_LST_Find (list,  element, FALSE, &pre_node);

   if ( this_node == 0 )
      return FALSE;

   memcpy (element, this_node->element, list->size_of_element);

   return TRUE;

} /* L_SORT_LST_Get */




/* PURPOSE: Get next element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the next element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Get_Next (L_SORT_LST_List_T *list, void *element)
{
   Slist_T  *pre_node;
   Slist_T  *this_node;

   /* BODY
    */
   this_node = L_SORT_LST_Find (list,  element, TRUE, &pre_node);

   if ( this_node == 0 )
      return FALSE;

   memcpy (element, this_node->element, list->size_of_element);

   return TRUE;

} /* L_SORT_LST_Get_Next */



/* PURPOSE: Set a element in list, if that element not in list, insert it.
 * INPUT:   list     -- list to operation
 *          element  -- element to be set
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_Set ( L_SORT_LST_List_T *list, void *element)
{
   Slist_T  *pre_node;
   Slist_T  *this_node;

   /* BODY
    */
   this_node = L_SORT_LST_Find (list,  element, FALSE, &pre_node);

   if ( this_node == 0 )
   {  /* insert */
      void * alloc_element;

      if ( list->nbr_of_element >= list->max_element_count )
         return FALSE;

      if ( (this_node  = malloc ( sizeof(Slist_T) ) ) == 0  )
         return FALSE;

      if ( (alloc_element = malloc (list->size_of_element) ) == 0 )
      {
         free ( this_node );
         return FALSE;
      }

      memset (this_node, 0, sizeof(Slist_T) );

      list->nbr_of_element++;
      this_node->element  = alloc_element;

      if ( pre_node == 0 )
      {  /* insert at head */
         this_node->next = list->head;
         list->head = this_node;
      }
      else
      {
         this_node->next = pre_node->next;
         pre_node->next = this_node;
      }
   }

   memcpy (this_node->element, element, list->size_of_element);
   return TRUE;

} /* L_SORT_LST_Set */




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
L_SORT_LST_List_T *list, void *element, UI32_T (*fun)(const void *element), UI32_T count)
{
   Slist_T  *pre_node;
   Slist_T  *node;

   /* BODY
    */
   node = (element == 0)? (Slist_T*) list->head:
   L_SORT_LST_Find (list,  element, TRUE, &pre_node);

   /* BODY
    */
   for ( ; node && count ; node = node->next, count-- )
   {
      if ( (*fun) (node->element) == L_SORT_LST_SEARCH_BREAK )
         return L_SORT_LST_SEARCH_BREAK;
   }
   return (node==0)? L_SORT_LST_SEARCH_END_OF_LIST:L_SORT_LST_SEARCH_COMPLETED;
} /* L_SORT_LST_Search */



/* LOCAL SUBPROGRAM BODIES
 */
static Slist_T *L_SORT_LST_Find (
   L_SORT_LST_List_T   *list,
   void              *in_element,
   BOOL_T            get_next,
   Slist_T           **pre_node)
{
   /* LOCAL VARIABLES
   */
   Slist_T *this_node;
   int    ret;


   /* BODY
    */
   *pre_node = 0;
   if ( (this_node=list->head) == 0 )
         return 0;     /* No any element */

   for ( ; this_node; *pre_node=this_node, this_node=this_node->next)
   {
      ret = (*list->compare) (this_node->element, in_element );

      if ( ret < 0 )
         continue;

      switch ( get_next*2 + (ret>0) )
      {
      case 0:  /* get, element_value == input_value */
      case 3:  /* get next, element_value > input_value */
         return this_node;

      case 1 : /* get, element_value > input_value */
         return 0;

      case 2:  /* get next, element_value == input_value */
         *pre_node = this_node;
         return this_node->next;
      } /* End of switch */
   } /* End of for */

   return 0;
} /* L_SORT_LST_Find */





#ifdef LSORT_DEBUG
static  L_SORT_LST_List_T list;

typedef struct
{
   int key;
   int num;
} Elem_T;


int compare(Elem_T *e1, Elem_T *e2)
{
   return (e1->key - e2->key);
}


void main()
{
   BOOL_T   ret;
   Elem_T   elem;
   Elem_T   elem1;
   char buf[80];

   L_SORT_LST_Create ( &list, 4,  sizeof(Elem_T), compare );

   ungetch( 'm');

   while ( 1 )
   {
      switch ( getch() )
      {
      case 'M':
      case 'm':
         puts("\n\nSet, Del, delAll 1st, Get, getNext, List, eXit");
         continue;

      case 'A':
      case 'a':
         ret = L_SORT_LST_Delete_All (&list);
         ungetch( 'l');
         continue;

      case 'S':
      case 's':
         printf("\nkey = ");
         elem.key = atoi (cgets(buf) );
         printf("num = ");
         elem.num = atoi (cgets(buf) );
         ret = L_SORT_LST_Set ( &list, &elem);
         ungetch( 'l');
         continue;

      case 'L':
      case 'l':
         memset(&elem1, 0, sizeof(elem1) );
         for(;;)
         {
            if ( !L_SORT_LST_Get_Next (&list, &elem1) )
               break;
             printf ("element: (%d, %d)\n", elem1.key, elem1.num);
         }
         continue;

      case 'D':
      case 'd':
         printf("\nkey = ");
         elem.key = atoi (cgets(buf) );
         ret = L_SORT_LST_Delete ( &list, &elem);
         ungetch( 'l');
         continue;

      case '1':
         ret = L_SORT_LST_Get_1st (&list, &elem);
         printf ("element: (%d, %d)\n", elem.key, elem.num);
         continue;

      case 'g':
      case 'G':
         printf("\nkey = ");
         elem.key = atoi (cgets(buf) );
         ret = L_SORT_LST_Get (&list, &elem);
         printf ("element: (%d, %d)\n", elem.key, elem.num);
         continue;

      case 'n':
      case 'N':
         ret = L_SORT_LST_Get_Next (&list, &elem);
         printf ("element: (%d, %d)\n", elem.key, elem.num);
         break;
      case 'x':
      case 'X':
         exit(1);
      }
   }
}

#endif

/* PURPOSE: Get the address of the element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the address of the element.
 * OUTPUT:  none
 * RETURN:  the address of the element and NULL on failure.
 */
void *L_SORT_LST_GetPtr (L_SORT_LST_List_T *list, void *element)
{
   /* 2007.06.26. wakka
    */
   Slist_T  *pre_node;
   Slist_T  *this_node;

   /* BODY
    */
   this_node = L_SORT_LST_Find (list,  element, FALSE, &pre_node);

   return (this_node == 0 ? NULL : L_SORT_GET_USERDATA(this_node));

} /* L_SORT_LST_GetPtr */

/*---------------------------------
 * Exported functions for shared memory version
 *---------------------------------
 */
/* PURPOSE: Create a list
 * INPUT:   list     -- list
 *          working_buffer -- the working buffer for the list in shared memory
 *          max_element_count
 *          size_of_element
 *          compare_func_id -- the type of compare function to use
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Create (
    L_SORT_LST_ShMem_List_T *list,
    void *working_buffer,
    UI32_T max_element_count,
    UI32_T size_of_element,
    UI32_T compare_func_id)
{
    if( (list == NULL) ||
        (working_buffer == NULL) ||
        (compare_func_id >= L_SORT_LST_SHMEM_NBR_OF_COMPARE_FUNC))
    {
        /* critical error, use printf to show error message
         */
        printf("\r\n%s(): invalid arguments.", __FUNCTION__);
        return FALSE;
    }

    list->compare_func_id = compare_func_id;
    list->head_offset = 0;
    list->nbr_of_element = 0;
    list->size_of_element = size_of_element;
    /* L_PT will round up partition size to 4-byte alignment.
     * If we do not round up partition size here, the available number of
     * partition will be less than we expect.
     */
    list->ptdesc.buffer_offset = L_CVRT_GET_OFFSET(&list->ptdesc, working_buffer);
    list->ptdesc.buffer_len = L_SORT_LST_SHMEM_PT_BUFFER_REQUIRED_SZ(max_element_count, size_of_element);
    list->ptdesc.partition_len = L_SORT_LST_SHMEM_ONE_PARTITION_SZ(size_of_element);

    if (!L_PT_ShMem_Create(&list->ptdesc))
    {
        /* critical error, use printf to show error message
         */
        printf("\r\n%s(): create fail.", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
} /* L_SORT_LST_ShMem_Create */




/* PURPOSE: delete a element from list
 * INPUT:   list     -- list to operation
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Delete (L_SORT_LST_ShMem_List_T *list, void *element)
{
   L_SORT_LST_ShMem_Slist_T  *pre_node;
   L_SORT_LST_ShMem_Slist_T  *this_node;

   /* BODY
    */
   this_node = L_SORT_LST_ShMem_Find (list,  element, FALSE, &pre_node);

   if ( this_node == 0 )
      return FALSE;

   if ( pre_node == 0 )
   {  /* remove 1st element */
      list->head_offset = this_node->next_offset;
   }
   else
   {
      pre_node->next_offset = this_node->next_offset;
   }

   list->nbr_of_element--;
   L_SORT_LST_ShMem_Free(this_node);

   return TRUE;

} /* L_SORT_LST_ShMem_Delete */


/* PURPOSE: delete all element from list
 * INPUT:   list     -- list to operation
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Delete_All (L_SORT_LST_ShMem_List_T *list)
{
   L_SORT_LST_ShMem_Slist_T  *node, *next_node;

   /* BODY
    */
   for ( node = L_SORT_LST_SHMEM_GET_HEAD(list); node; node = next_node )
   {
      next_node = L_SORT_LST_SHMEM_GET_NEXT(list, node);
      L_SORT_LST_ShMem_Free(node);
   }
   list->nbr_of_element = 0;
   list->head_offset = 0;
   return TRUE;
} /* L_SORT_LST_ShMem_Delete_All */



/* PURPOSE: Get 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to get element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Get_1st (L_SORT_LST_ShMem_List_T *list, void *element)
{
   /* BODY
    */
   if ( list->head_offset )
   {
      memcpy (element, L_SORT_LST_SHMEM_GET_USERDATA(L_SORT_LST_SHMEM_GET_HEAD(list)), list->size_of_element);
      return TRUE;
   }
   return FALSE;
} /* L_SORT_LST_ShMem_Get_1st */

/* PURPOSE: Remove 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to store the removed element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Remove_1st (L_SORT_LST_ShMem_List_T *list, void *element)
{
   L_SORT_LST_ShMem_Slist_T  *node;

   node = L_SORT_LST_SHMEM_GET_HEAD(list);
   if ( node )
   {
      memcpy (element, L_SORT_LST_SHMEM_GET_USERDATA(node), list->size_of_element);
      list->head_offset = node->next_offset;
      L_SORT_LST_ShMem_Free(node);
      list->nbr_of_element--;
      return TRUE;
   }
   return FALSE;
} /* L_SORT_LST_ShMem_Remove_1st */

/* PURPOSE: Get element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Get (L_SORT_LST_ShMem_List_T *list, void *element)
{
   L_SORT_LST_ShMem_Slist_T  *pre_node;
   L_SORT_LST_ShMem_Slist_T  *this_node;

   /* BODY
    */
   this_node = L_SORT_LST_ShMem_Find (list,  element, FALSE, &pre_node);

   if ( this_node == 0 )
      return FALSE;

   memcpy (element, L_SORT_LST_SHMEM_GET_USERDATA(this_node), list->size_of_element);

   return TRUE;

} /* L_SORT_LST_ShMem_Get */




/* PURPOSE: Get next element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the next element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Get_Next (L_SORT_LST_ShMem_List_T *list, void *element)
{
   L_SORT_LST_ShMem_Slist_T  *pre_node;
   L_SORT_LST_ShMem_Slist_T  *this_node;

   /* BODY
    */
   this_node = L_SORT_LST_ShMem_Find (list,  element, TRUE, &pre_node);

   if ( this_node == 0 )
      return FALSE;

   memcpy (element, L_SORT_LST_SHMEM_GET_USERDATA(this_node), list->size_of_element);

   return TRUE;

} /* L_SORT_LST_ShMem_Get_Next */


/* PURPOSE: Get the address of the element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the address of the element.
 * OUTPUT:  none
 * RETURN:  the address of the element and NULL on failure.
 */
void *L_SORT_LST_ShMem_GetPtr (L_SORT_LST_ShMem_List_T *list, void *element)
{
   /* 2007.06.26. wakka
    */
   L_SORT_LST_ShMem_Slist_T  *pre_node;
   L_SORT_LST_ShMem_Slist_T  *this_node;

   /* BODY
    */
   this_node = L_SORT_LST_ShMem_Find (list,  element, FALSE, &pre_node);

   return (this_node == 0 ? NULL : L_SORT_LST_SHMEM_GET_USERDATA(this_node));

} /* L_SORT_LST_ShMem_GetPtr */



/* PURPOSE: Set a element in list, if that element not in list, insert it.
 * INPUT:   list     -- list to operation
 *          element  -- element to be set
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_SORT_LST_ShMem_Set ( L_SORT_LST_ShMem_List_T *list, void *element)
{
   L_SORT_LST_ShMem_Slist_T  *pre_node;
   L_SORT_LST_ShMem_Slist_T  *this_node;

   /* BODY
    */
   this_node = L_SORT_LST_ShMem_Find (list,  element, FALSE, &pre_node);

   if ( this_node == 0 )
   {  /* insert */

      if( (this_node = L_SORT_LST_ShMem_Malloc()) == NULL)
      {
         return FALSE;
      }

      list->nbr_of_element++;

      if ( pre_node == NULL )
      {  /* insert at head */
         this_node->next_offset = list->head_offset;
         list->head_offset = L_CVRT_GET_OFFSET(list, this_node);
      }
      else
      {
         this_node->next_offset = pre_node->next_offset;
         pre_node->next_offset = L_CVRT_GET_OFFSET(list, this_node);
      }
   }

   memcpy (L_SORT_LST_SHMEM_GET_USERDATA(this_node), element, list->size_of_element);
   return TRUE;

} /* L_SORT_LST_ShMem_Set */




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
L_SORT_LST_ShMem_List_T *list, void *element, UI32_T (*fun)(const void *element), UI32_T count)
{
   L_SORT_LST_ShMem_Slist_T  *pre_node;
   L_SORT_LST_ShMem_Slist_T  *node;

   /* BODY
    */
   node = (element == 0)? L_SORT_LST_SHMEM_GET_HEAD(list):
   L_SORT_LST_ShMem_Find (list,  element, TRUE, &pre_node);

   /* BODY
    */
   for ( ; node && count ; node = L_SORT_LST_SHMEM_GET_NEXT(list, node), count-- )
   {
      if ( (*fun) (L_SORT_LST_SHMEM_GET_USERDATA(node)) == L_SORT_LST_SEARCH_BREAK )
         return L_SORT_LST_SEARCH_BREAK;
   }
   return (node==0)? L_SORT_LST_SEARCH_END_OF_LIST:L_SORT_LST_SEARCH_COMPLETED;
} /* L_SORT_LST_ShMem_Search */

/*---------------------------------
 * local functions for shared memory version
 *---------------------------------
 */
static L_SORT_LST_ShMem_Slist_T *L_SORT_LST_ShMem_Find (
   L_SORT_LST_ShMem_List_T   *list,
   void              *in_element,
   BOOL_T            get_next,
   L_SORT_LST_ShMem_Slist_T           **pre_node)
{
   /* LOCAL VARIABLES
   */
   L_SORT_LST_ShMem_Slist_T *this_node;
   int    ret;


   /* BODY
    */
   *pre_node = 0;
   if ((this_node = L_SORT_LST_SHMEM_GET_HEAD(list)) == NULL)
         return 0;     /* No any element */

   for (; this_node; *pre_node=this_node, this_node=L_SORT_LST_SHMEM_GET_NEXT(list, this_node))
   {
      ret = shmem_compare_func[list->compare_func_id](L_SORT_LST_SHMEM_GET_USERDATA(this_node), in_element);

      if ( ret < 0 )
         continue;

      switch ( get_next*2 + (ret>0) )
      {
      case 0:  /* get, element_value == input_value */
      case 3:  /* get next, element_value > input_value */
         return this_node;

      case 1 : /* get, element_value > input_value */
         return 0;

      case 2:  /* get next, element_value == input_value */
         *pre_node = this_node;
         return L_SORT_LST_SHMEM_GET_NEXT(list, this_node);
      } /* End of switch */
   } /* End of for */

   return 0;
} /* L_SORT_LST_ShMem_Find */




#ifdef L_SORT_LST_DEBUG
static  L_SORT_LST_List_T list;

typedef struct
{
   int key;
   int num;
} Elem_T;


static int compare(Elem_T *e1, Elem_T *e2)
{
   return (e1->key - e2->key);
}

static UI32_T SearchFunc1(const void *entry)
{
    Elem_T *elem_p = (Elem_T*)entry;

    if(elem_p->key>100)
    {
        printf("\r\nFound {%d,%d}", elem_p->key, elem_p->num);
    }
    return L_SORT_LST_SEARCH_CONTINUE;
}

static UI32_T SearchFunc2(const void *entry)
{
    Elem_T *elem_p = (Elem_T*)entry;

    if(elem_p->key<=100)
    {
        printf("\r\nFound {%d,%d}", elem_p->key, elem_p->num);
        return L_SORT_LST_SEARCH_CONTINUE;
    }
    return L_SORT_LST_SEARCH_BREAK;
}

int L_SORT_LST_main(void)
{
   BOOL_T   is_cont=TRUE;
   BOOL_T   ret;
   Elem_T   elem;
   Elem_T   elem1;

   L_SORT_LST_Create ( &list, 4,  sizeof(Elem_T), (int (*)(void*, void*))compare );

   while ( is_cont )
   {
      switch ( getchar() )
      {
      case 'M':
      case 'm':
         puts("\r\n(S)et, (D)el, del(A)ll, (1)st, (G)et, get(N)ext, (L)ist, e(X)it, (R)emove1st, (Y)search_test1, (Z)search_test2");
         continue;

      case 'R':
      case 'r':
         ret = L_SORT_LST_Remove_1st(&list, &elem);
         printf("\r\nL_SORT_LST_Remove_1st return %d", ret);
         if(ret==TRUE)
         {
            printf("\r\nRemoved entry={%d,%d}", elem.key, elem.num);
         }
         break;

      case 'Y':
      case 'y':
         printf("\r\nSearch list for entries with key which is larger than 100");
         L_SORT_LST_Search(&list, NULL, SearchFunc1, 4);
         break;

      case 'Z':
      case 'z':
         printf("\r\nSearch list for entries with key which is smaller or equal to 100");
         L_SORT_LST_Search(&list, NULL, SearchFunc2, 4);
         break;

      case 'A':
      case 'a':
         ret = L_SORT_LST_Delete_All (&list);
         printf("\r\nL_SORT_LST_Delete_All return %d", ret);
         continue;

      case 'S':
      case 's':
         printf("\r\nkey = ");
         scanf("%d", &(elem.key));
         printf("\r\nnum = ");
         scanf("%d", &(elem.num));
         ret = L_SORT_LST_Set ( &list, &elem);
         printf("\r\nL_SORT_LST_Set return %d", ret);
         continue;

      case 'L':
      case 'l':
         memset(&elem1, 0, sizeof(elem1) );
         for(;;)
         {
            if ( !L_SORT_LST_Get_Next (&list, &elem1) )
               break;
             printf ("\r\nelement: (%d, %d)", elem1.key, elem1.num);
         }
         continue;

      case 'D':
      case 'd':
         printf("\r\nkey = ");
         scanf("%d", &elem.key);
         ret = L_SORT_LST_Delete ( &list, &elem);
         printf("\r\nL_SORT_LST_Delete return %d", ret);
         continue;

      case '1':
         ret = L_SORT_LST_Get_1st (&list, &elem);
         printf ("\r\nelement: (%d, %d) ret=%d", elem.key, elem.num, ret);
         continue;

      case 'g':
      case 'G':
         printf("\r\nkey = ");
         scanf("%d", &(elem.key));
         ret = L_SORT_LST_Get (&list, &elem);
         printf ("\r\nelement: (%d, %d) ret=%d", elem.key, elem.num, ret);
         continue;

      case 'n':
      case 'N':
         ret = L_SORT_LST_Get_Next (&list, &elem);
         printf ("\r\nelement: (%d, %d) ret=%d", elem.key, elem.num, ret);
         break;
      case 'x':
      case 'X':
         is_cont=FALSE;
         break;
      }
   }
   return 0;
}

#endif



