#include <stdio.h>
#include "sys_type.h"
#include <stdlib.h>
#include <string.h>
#include "l_lst.h"


/* INCLUDE FILE DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */
#define L_LST_Malloc() L_PT_Allocate(&(list->ptdesc))
#define L_LST_Free(buf_p) L_PT_Free(&(list->ptdesc), buf_p)

/* DATA TYPE DECLARACTIONS
 */

/* Pointer structure for the operations of linked list  */

typedef struct List_S
{
    struct List_S   *next;
    void            *element;
} List_T;


/* LOCAL FUNCTIONS DECLARACTIONS
 */
static List_T *L_LST_Find (L_LST_List_T    *list,
                           void            *in_element,
                           BOOL_T          get_next,
                           List_T          **pre_node);


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
                     int (*compare)(void*inlist_element, void *input_element))
{
    if( (list==NULL) ||
        (compare==NULL))
    {
        /* critical error, use printf to show error message
         */
        printf("\r\n%s(): invalid arguments.", __FUNCTION__);
        return FALSE;
    }

    list->compare = compare;
    list->head = 0 ;
    list->tail = 0 ;
    list->nbr_of_element = 0;
    list->size_of_element = size_of_element;

    list->ptdesc.partition_len = sizeof(List_T);
    list->ptdesc.buffer_len = max_element_count*sizeof(List_T);
    list->ptdesc.buffer = malloc(list->ptdesc.buffer_len);

    if((list->ptdesc.buffer==NULL) ||
       (L_PT_Create(&(list->ptdesc))==FALSE))
    {
        /* critical error, use printf to show error message
         */
        if(list->ptdesc.buffer)
        {
            free(list->ptdesc.buffer);
        }
        printf("\r\n%s(): create fail.", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
} /* L_LST_Create */


/* PURPOSE: delete an element from list
 * INPUT:   list     -- list to operate
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LST_Delete (L_LST_List_T *list, void *element)
{
   List_T  *pre_node;
   List_T  *this_node;

   /* BODY
    */
   this_node = L_LST_Find (list,  element, FALSE, &pre_node);

   if ( this_node == 0 )
      return FALSE;

   if ( pre_node == 0 )
   {  /* remove 1st element */
      list->head = ((List_T*)list->head) ->next;
   }
   else
   {
      pre_node->next = pre_node->next->next;
   }

   list->nbr_of_element--;
   L_LST_Free (this_node);

   return TRUE;

} /* L_LST_Delete */


/* PURPOSE: delete all elements from list
 * INPUT:   list     -- list to operate
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LST_DeleteAll (L_LST_List_T *list)
{
   List_T  *node, *next_node;

   /* BODY
    */
   for ( node = (List_T*) list->head; node; node = next_node )
   {
      next_node = node->next;
      L_LST_Free (node);
   }
   list->nbr_of_element = 0;
   list->head = (List_T*)0;
   list->tail = (List_T*)0;
   return TRUE;
} /* L_LST_DeleteAll */



/* PURPOSE: Get 1st element in list
 * INPUT:   list     -- list to operate
 * OUTPUT:  element  -- buffer to get element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LST_GetFirst (L_LST_List_T *list, void *element)
{
   /* BODY
    */
   if ( list->head )
   {
      memcpy (element, ((List_T*)list->head) ->element, list->size_of_element);
      return TRUE;
   }
   return FALSE;
} /* L_LST_GetFirst */



/* PURPOSE: Remove 1st element in list
 * INPUT:   list     -- list to operate
 * OUTPUT:  element  -- buffer to store the removed element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LST_RemoveFirst (L_LST_List_T *list, void *element)
{
   List_T  *node;

   node = list->head;
   if ( node )
   {
      memcpy (element, node ->element, list->size_of_element);
      list->head = node->next;
      L_LST_Free (node);
      list->nbr_of_element--;
      return TRUE;
   }
   return FALSE;
} /* L_LST_RemoveFirst */



/* PURPOSE: Get element in list
 * INPUT:   list     -- list to operate
 *          element  -- according the "key" in element to get other fields of element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 * NOTE:    "key" is specified and used in compare function
 */
BOOL_T L_LST_Get (L_LST_List_T *list, void *element)
{
   List_T  *pre_node;
   List_T  *this_node;

   /* BODY
    */
   this_node = L_LST_Find (list,  element, FALSE, &pre_node);

   if ( this_node == 0 )
      return FALSE;

   memcpy (element, this_node->element, list->size_of_element);

   return TRUE;

} /* L_LST_Get */


/* PURPOSE: Get next element in list
 * INPUT:   list    -- list to operate
 *          element  -- according the "key" in element to get the next element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 * NOTE:    "key" is specified and used in compare function
 */
BOOL_T L_LST_GetNext (L_LST_List_T *list, void *element)
{
   List_T  *pre_node;
   List_T  *this_node;

   /* BODY
    */
   this_node = L_LST_Find (list,  element, FALSE, &pre_node);

   if (this_node == 0 || this_node->next == 0)
      return FALSE;

   memcpy (element, this_node->next->element, list->size_of_element);

   return TRUE;

} /* L_LST_GetNext */



/* PURPOSE: Append an element to the tail of this list.
 * INPUT:   list     -- list to operate
 *          element  -- element to be set
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LST_Set (L_LST_List_T *list, void *element)
{
   List_T  *pre_node;
   List_T  *this_node;

   /* BODY
    */
   this_node = L_LST_Find (list,  element, FALSE, &pre_node);

   if ( this_node != NULL )
      return FALSE;
   else
   {  /* append */

      if ( (this_node  = L_LST_Malloc()) == NULL )
         return FALSE;

      memset (this_node, 0, sizeof(List_T) );

      list->nbr_of_element++;
      this_node->element  = element;

      if ( pre_node == NULL )
      {  /* insert at head */
         this_node->next = list->head;
         list->head = this_node;
      }
      else
      {
         this_node->next = pre_node->next;
         pre_node->next = this_node;
      }
      list->tail = this_node;

   }
   return TRUE;

} /* L_LST_Set */



/* LOCAL SUBPROGRAM BODIES
 */
static List_T *L_LST_Find (L_LST_List_T  *list,
                            void          *in_element,
                            BOOL_T        get_next,
                            List_T       **pre_node)
{
   /* LOCAL VARIABLES
   */
   List_T   *this_node;
   int              ret;


   /* BODY
    */
   *pre_node = 0;
   if ( (this_node=list->head) == 0 )
         return 0;     /* this list is empty */

   for ( ; this_node; *pre_node=this_node, this_node=this_node->next)
   {
      ret = (*list->compare) (this_node->element, in_element);

      if ( ret != 0 )
         continue;

      /* element_value == input_value */

      switch ( get_next*2 + (this_node->next!=0) )
      {
      case 3:  /* get next, and next element exist */
         *pre_node = this_node;
         return this_node->next;
      case 2:  /* get next, next element not exist */
         return 0;
      case 1 : /* get, next element exist */
      case 0 : /* get, next element not exist */
         return this_node;
      } /* End of switch */
   } /* End of for */

   return 0;
} /* L_LST_Find */

#ifdef L_LST_DEBUG

#define INVALID_INDEX -1

typedef struct
{
    int   key;
    int   entry_idx;
}Elem_T;

int compare(Elem_T *e1, Elem_T *e2)
{
    return (e1->key - e2->key);
}

int l_lst_main(void)
{
    Elem_T            elem;
    Elem_T            elem1;
    Elem_T            elem_pool[4];
    L_LST_List_T      list;
    L_PT_Descriptor_T ptdesc;
    BOOL_T            is_cont=TRUE;
    BOOL_T            ret;

    ptdesc.partition_len=sizeof(Elem_T);
    ptdesc.buffer_len=sizeof(elem_pool);
    ptdesc.buffer = (char*)&elem_pool;

    if(FALSE==L_PT_Create(&ptdesc))
    {
        printf("\r\nL_PT_Create fail.");
        return -1;
    }

    if(FALSE==L_LST_Create ( &list, 4,  sizeof(Elem_T), (int (*)(void*, void*))compare ))
    {
        printf("\r\nL_LST_Create fail.");
        return -1;
    }

   while ( is_cont )
   {
      switch ( getchar() )
      {
      case 'M':
      case 'm':
         puts("\r\n(S)et, (D)el, del(A)ll, (1)st, (G)et, get(N)ext, (L)ist, e(X)it, (R)emove1st");
         break;

      case 'S':
      case 's':
         {
            Elem_T *new_elm_p = (Elem_T*)L_PT_Allocate(&ptdesc);

            if(new_elm_p==NULL)
            {
                printf("\r\nBuffer pool has been used up.");
                break;
            }

            new_elm_p->entry_idx = (new_elm_p-elem_pool);
            printf("\r\nkey = ");
            scanf("%d", &(new_elm_p->key));
            ret = L_LST_Set ( &list, new_elm_p);
            printf("\r\nL_LST_Set return %d", ret);
            if(ret==FALSE)
            {
               L_PT_Free(&ptdesc, new_elm_p);
            }
         }
         break;

      case 'D':
      case 'd':
         {
            Elem_T removed_entry;

            printf("\r\nkey = ");
            scanf("%d", &(removed_entry.key));
            if(FALSE==L_LST_Get(&list, &removed_entry))
            {
               removed_entry.entry_idx=INVALID_INDEX;
            }

            ret = L_LST_Delete ( &list, &removed_entry);
            printf("\r\nL_LST_Delete return %d", ret);
            if((ret==TRUE) && (removed_entry.entry_idx!=INVALID_INDEX))
            {
               L_PT_Free(&ptdesc, (Elem_T*)&elem_pool + removed_entry.entry_idx);
            }
         }
         break;

      case 'A':
      case 'a':
         ret = L_LST_DeleteAll (&list);
         printf("\r\nL_LST_Delete_All return %d", ret);
         if(FALSE==L_PT_Create(&ptdesc))
         {
            printf("\r\nReset buffer pool fail");
         }
         break;

      case '1':
         ret = L_LST_GetFirst (&list, &elem);
         printf ("\r\nelement: (%d, %d) ret=%d", elem.key, elem.entry_idx, ret);
         break;

      case 'g':
      case 'G':
         printf("\r\nkey = ");
         scanf("%d", &(elem.key));
         ret = L_LST_Get (&list, &elem);
         printf ("\r\nelement: (%d, %d) ret=%d", elem.key, elem.entry_idx, ret);
         break;

      case 'n':
      case 'N':
         ret = L_LST_GetNext (&list, &elem);
         printf ("\r\nelement: (%d, %d) ret=%d", elem.key, elem.entry_idx, ret);
         break;

      case 'L':
      case 'l':
         /* get first
          */
         if(FALSE==L_LST_GetFirst (&list, &elem1))
         {
             printf("\r\n list is empty");
             break;
         }
         printf ("\r\nelement: (%d, %d)", elem1.key, elem1.entry_idx);
         for(;;)
         {
            if ( !L_LST_GetNext (&list, &elem1) )
            {
               break;
            }
            printf ("\r\nelement: (%d, %d)", elem1.key, elem1.entry_idx);
         }
         break;

      case 'x':
      case 'X':
         is_cont=FALSE;
         break;

      case 'R':
      case 'r':
         ret = L_LST_RemoveFirst(&list, &elem);
         printf("\r\nL_SORT_LST_Remove_1st return %d", ret);
         if(ret==TRUE)
         {
            printf("\r\nRemoved entry={%d,%d}", elem.key, elem.entry_idx);
            L_PT_Free(&ptdesc, (Elem_T*)&elem_pool + elem.entry_idx);
         }
         break;
      }
   }
   return 0;

}
#endif

