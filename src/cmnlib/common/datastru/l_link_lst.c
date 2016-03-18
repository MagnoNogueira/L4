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

#ifdef LLINK_LIST_DEBUG
#include <conio.h>
#endif
#include <stdio.h>
#include "sys_type.h"
#include "sys_module.h"
#include "l_mm.h"
#include "cmnlib_type.h"
#include <stdlib.h>
#include <string.h>
#include "l_link_lst.h"
#include "l_pt.h"




/* INCLUDE FILE DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */
#define L_LINK_LST_NONPREALLOC_Malloc(size) L_MM_Malloc(size, L_MM_USER_ID2(SYS_MODULE_CMNLIB, CMNLIB_TYPE_TRACE_ID_L_LINK_LST))
#define L_LINK_LST_NONPREALLOC_Free(buf_p) L_MM_Free(buf_p)

/* DATA TYPE DECLARACTIONS
 */
/* SLIST mean a sorted list  */
typedef struct List_Tag
{
    struct List_Tag   *next;
    void               *element;
} List_T;





/* LOCAL FUNCTIONS DECLARACTIONS
 */
static List_T* L_LINK_LST_Find (L_LINK_LST_List_T *list,
                                void *in_element,
                                BOOL_T get_next,
                                List_T **pre_node);




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
BOOL_T L_LINK_LST_Create (L_LINK_LST_List_T *list,
                          UI32_T max_element_count,
                          UI32_T size_of_element,
                          BOOL_T  (*compare)(void*inlist_element, void *input_element),
                          BOOL_T is_mem_prealloc)
{
    UI32_T partition_len;
    UI32_T buffer_len;


   /*the partition length of l_pt need mutliple of 4
    */
    partition_len=(size_of_element + sizeof(List_T) + 3)/4*4;
    buffer_len=partition_len*max_element_count;
    list->compare = compare;
    list->head = 0 ;
    list->nbr_of_element = 0;
    list->max_element_count = max_element_count;
    list->size_of_element = size_of_element;

   /* VividLiou added
    */
    list->is_mem_prealloc=is_mem_prealloc;
    if(is_mem_prealloc == FALSE)
    {
        return TRUE;
    }
    else
    {
        list->desc.buffer=malloc(buffer_len);
        if(list->desc.buffer == NULL)
        {
            return FALSE;
        }


        list->desc.buffer_len=buffer_len;
        list->desc.partition_len=partition_len;
        L_PT_Create (&(list->desc));
        return TRUE;

    }


} /* L_LINK_LST_Create */




/* PURPOSE: delete a element from list
 * INPUT:   list     -- list to operation
 *          element  -- element to be deleted
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Delete (L_LINK_LST_List_T *list, void *element)
{
    List_T  *pre_node;
    List_T  *this_node;

   /* BODY
    */
    this_node = L_LINK_LST_Find (list,  element, FALSE, &pre_node);

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

    if(list->is_mem_prealloc == FALSE)
    {
        L_LINK_LST_NONPREALLOC_Free (this_node->element);
        L_LINK_LST_NONPREALLOC_Free (this_node);
    }
    else
    {
       /* VividLiou rewrite
        */
        L_PT_Free(&(list->desc) , this_node);
    }

    return TRUE;

} /* L_LINK_LST_Delete */


/* PURPOSE: delete all element from list
 * INPUT:   list     -- list to operation
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Delete_All (L_LINK_LST_List_T *list)
{
    List_T  *node, *next_node;

   /* BODY
    */

    if(list->is_mem_prealloc == FALSE)
    {
        for( node = (List_T*) list->head; node; node = next_node )
        {
            L_LINK_LST_NONPREALLOC_Free (node->element);
            next_node = node->next;
            L_LINK_LST_NONPREALLOC_Free (node);
        }
    }
    else
    {
       /* VividLiou rewrite
        */
        for ( node = (List_T*) list->head; node; node = next_node )
        {
            next_node = node->next;
            L_PT_Free(&(list->desc) , node);
        }
    }

    list->nbr_of_element = 0;
    list->head = (List_T*)0;
    return TRUE;
} /* L_LINK_LST_Delete_All */



/* PURPOSE: Get 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to get element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Get_1st (L_LINK_LST_List_T *list, void *element)
{
   /* BODY
    */
    if ( list->head )
    {
        memcpy (element, ((List_T*)list->head)->element, list->size_of_element);
        return TRUE;
    }
    return FALSE;
} /* L_LINK_LST_Get_1st */

/* PURPOSE: Remove 1st element in list
 * INPUT:   list     -- list to operation
 * OUTPUT:  element  -- buffer to store the removed element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Remove_1st (L_LINK_LST_List_T *list, void *element)
{
    List_T  *node;

    node = list->head;
    if ( node )
    {
        memcpy (element, node ->element, list->size_of_element);
        list->head = node->next;
        if(list->is_mem_prealloc == FALSE)
        {
            L_LINK_LST_NONPREALLOC_Free (node->element);
            L_LINK_LST_NONPREALLOC_Free (node);
        }
        else
        {
          /* VividLiou Rewrite
           */
            L_PT_Free(&(list->desc) , node);
        }


        list->nbr_of_element--;
        return TRUE;
    }
    return FALSE;
} /* L_LINK_LST_Remove_1st */

/* PURPOSE: Get element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Get (L_LINK_LST_List_T *list, void *element)
{
    List_T  *pre_node;
    List_T  *this_node;

   /* BODY
    */
    this_node = L_LINK_LST_Find (list,  element, FALSE, &pre_node);

    if ( this_node == 0 )
      return FALSE;

    memcpy (element, this_node->element, list->size_of_element);

    return TRUE;

} /* L_LINK_LST_Get */




/* PURPOSE: Get next element in list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to get the next element.
 * OUTPUT:  element
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Get_Next (L_LINK_LST_List_T *list, void *element)
{
    List_T  *pre_node;
    List_T  *this_node;

   /* BODY
    */
    this_node = L_LINK_LST_Find (list,  element, TRUE, &pre_node);

    if ( this_node == 0 )
      return FALSE;

    memcpy (element, this_node->element, list->size_of_element);

    return TRUE;

} /* L_LINK_LST_Get_Next */



/* PURPOSE: Set a element in list, if that element not in list, insert it.
 * INPUT:   list     -- list to operation
 *          element  -- element to be set
 *          position  -- L_LINK_LST_APPEND,
 *                    -- L_LINK_LST_INSERT_FROM_HEAD,
 *                    --
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Set ( L_LINK_LST_List_T *list, void *element, UI32_T postion)
{
   List_T  *pre_node;
   List_T  *this_node;

   /* BODY
    */

    switch(postion)
    {
        case L_LINK_LST_APPEND:
        case L_LINK_LST_INSERT_FROM_HEAD:
            break;
        default :
            return FALSE;
    }

    this_node = L_LINK_LST_Find (list,  element, FALSE, &pre_node);

    if ( this_node == 0 )
    {  /* insert */
        void * alloc_element;

        if ( list->nbr_of_element >= list->max_element_count )
            return FALSE;

        if(list->is_mem_prealloc == FALSE)
        {

            if ( (this_node  = L_LINK_LST_NONPREALLOC_Malloc ( sizeof(List_T) ) ) == 0  )
                return FALSE;

            if ( (alloc_element = L_LINK_LST_NONPREALLOC_Malloc (list->size_of_element) ) == 0 )
            {
                L_LINK_LST_NONPREALLOC_Free ( this_node );
                return FALSE;
            }
        }
        else
        {

          /* VividLiou rewrite :
             the partition of L_Pt is used as Slit_T + data area to store the elements
           */
            this_node  = (List_T*)L_PT_Allocate (&(list->desc)) ;
		    if(this_node == NULL)
		    {
                return FALSE;
            }


          /*alloc_element = (void*)this_node + sizeof(List_T);*/
		    alloc_element = (void*)&this_node[1];
        }

        memset (this_node, 0, sizeof(List_T) );
        list->nbr_of_element++;
        this_node->element  = alloc_element;
        switch(postion)
        {
            case  L_LINK_LST_INSERT_FROM_HEAD:
                if(list->head != 0)
                {
                    this_node->next=list->head;
                }

                list->head=this_node;
                break;

            case L_LINK_LST_APPEND:
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

    }

    memcpy (this_node->element, element, list->size_of_element);
    return TRUE;

} /* L_LINK_LST_Set */




/* PURPOSE: Search next element from list
 * INPUT:   list     -- list to operation
 *          element  -- according the "key" that in element, to search the next element.
 *                      if element is NULL(0) then search from 1st element.
 *          count    -- limited element count to be searched
 *          fun      -- The callback function to 'view' the element.
 *                      The return value of fun:
 *                         L_LINK_LST_SEARCH_BREAK: tell this function to break searching
 *                         L_LINK_LST_SEARCH_CONTINUE: tell this function continue searching
 * OUTPUT:  none
 * RETURN:  L_LINK_LST_SEARCH_BREAK: Searching is broken by (*fun)
 *          L_LINK_LST_SEARCH_END_OF_LIST: stop search because end of list
 *          L_LINK_LST_SEARCH_COMPLETED: stop search because reach the limited count
 * NOTE:    Caller must take care the mutual exculusion situation.
 *
 */
UI32_T L_LINK_LST_Search (L_LINK_LST_List_T *list,
                          void *element,
                          UI32_T (*fun)(const void *element, UI32_T cookie),
                          UI32_T count,
                          UI32_T cookie)
{
    List_T  *pre_node;
    List_T  *node;

   /* BODY
    */
    node = (element == 0)? (List_T*) list->head:
    L_LINK_LST_Find (list,  element, TRUE, &pre_node);

   /* BODY
    */
    for ( ; node && count ; node = node->next, count-- )
    {
        if ( (*fun) (node->element, cookie) == L_LINK_LST_SEARCH_BREAK )
            return L_LINK_LST_SEARCH_BREAK;
    }

    return (node==0)? L_LINK_LST_SEARCH_END_OF_LIST:L_LINK_LST_SEARCH_COMPLETED;
} /* L_LINK_LST_Search */




/* PURPOSE: destory a link list and free memory
 * INPUT:   list     -- list to operation
 *          element  -- element to be set
 *          position  -- L_LINK_LST_APPEND,
 *                    -- L_LINK_LST_INSERT_FROM_HEAD
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_LINK_LST_Destroy ( L_LINK_LST_List_T *list)
{

    List_T  *node, *next_node;

   /* BODY
    */

    if(list->is_mem_prealloc == FALSE)
    {
        for ( node = (List_T*) list->head; node; node = next_node )
        {
            L_LINK_LST_NONPREALLOC_Free (node->element);
            next_node = node->next;
            L_LINK_LST_NONPREALLOC_Free (node);
        }
    }
    else
    {
       /* VividLiou rewrite
        */
        for ( node = (List_T*) list->head; node; node = next_node )
        {
            next_node = node->next;
            L_PT_Free(&(list->desc) , node);
        }
    }

    list->nbr_of_element = 0;
    list->head = (List_T*)0;


    if(list->is_mem_prealloc == TRUE)
    {
        free(list->desc.buffer);
    }

    list->is_mem_prealloc=FALSE;

    return TRUE;
}



/* LOCAL SUBPROGRAM BODIES
 */
static List_T* L_LINK_LST_Find (L_LINK_LST_List_T   *list,
                                void                *in_element,
                                BOOL_T              get_next,
                                List_T              **pre_node)
{
   /* LOCAL VARIABLES
   */
    List_T *this_node;
    BOOL_T  ret;


   /* BODY
    */
    *pre_node = 0;
    if ( (this_node=list->head) == 0 )
         return 0;     /* No any element */

    for ( ; this_node; *pre_node=this_node, this_node=this_node->next)
    {
        ret = (*list->compare) (this_node->element, in_element);


        switch ( get_next*2 + ret )
        {
        case 0:  /* get, element_value != input_value */
            continue;
        case 3:  /* get next, element_value == input_value */
            *pre_node = this_node;
            return this_node->next;

        case 1 : /* get , element_value = input_value */

            return this_node;

        case 2:  /* get next, element_value != input_value */
            continue;

        } /* End of switch */

    } /* End of for */

    return 0;
} /* L_LINK_LST_Find */




#ifdef LLINK_LIST_DEBUG


typedef struct
{
   int key;
   int num;
} Elem_T;


BOOL_T compare(Elem_T *e1, Elem_T *e2)
{
   if(e1->key != e2->key)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }

}


void L_LINK_LSTmain()

{
   BOOL_T   ret;
   Elem_T   elem;
   Elem_T   elem1;
   char buf[80];

   L_LINK_LST_List_T list;
   L_LINK_LST_Create ( &list, 4,  sizeof(Elem_T), compare, TRUE );

   ungetch( 'm');

   while ( 1 )
   {
      switch ( getch() )
      {
      case 'M':
      case 'm':
         puts("\n\nSet, Del, delAll 1st, Get, getNext, List, eXit, deTroy");
         continue;

      case 'A':
      case 'a':
         ret = L_LINK_LST_Delete_All (&list);
         /*
         ungetch( 'l');
         */
         continue;

      case 'S':
      case 's':
         printf("\nkey = ");
         elem.key = atoi (cgets(buf) );
         printf("num = ");
         elem.num = atoi (cgets(buf) );
         ret = L_LINK_LST_Set ( &list, &elem, L_LINK_LST_INSERT_FROM_HEAD);
         ungetch( 'l');
         continue;

      case 'L':
      case 'l':
         memset(&elem1, 0, sizeof(elem1) );
         for(;;)
         {
            if ( !L_LINK_LST_Get_Next (&list, &elem1) )
               break;
             printf ("element: (%d, %d)\n", elem1.key, elem1.num);
         }
         continue;

      case 'D':
      case 'd':
         printf("\nkey = ");
         elem.key = atoi (cgets(buf) );
         ret = L_LINK_LST_Delete ( &list, &elem);
         ungetch( 'l');
         continue;

      case '1':
         ret = L_LINK_LST_Get_1st (&list, &elem);
         printf ("element: (%d, %d)\n", elem.key, elem.num);
         continue;

      case 'g':
      case 'G':
         printf("\nkey = ");
         elem.key = atoi (cgets(buf) );
         ret = L_LINK_LST_Get (&list, &elem);
		 if(ret == TRUE)
		 {
			printf ("element: (%d, %d)\n", elem.key, elem.num);
         }

         continue;

      case 'n':
      case 'N':
         ret = L_LINK_LST_Get_Next (&list, &elem);
         if(ret == TRUE)
         {
            printf ("element: (%d, %d)\n", elem.key, elem.num);
         }
         break;

      case 't':
      case 'T':
         ret = L_LINK_LST_Destroy (&list);

         break;
      case 'x':
      case 'X':
         exit(1);
      }
   }
}

#endif


