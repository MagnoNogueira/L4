/* -------------------------------------------------------------------------------------
 * FILE NAME : L_CIRLST.c (CIRCULAR LIST)
 * -------------------------------------------------------------------------------------
 * PURPOSE   : Circular List Implementation
 *             Deq output order is first in, first out.(FIFO)
 * NOTE      : none
 * MODIFICATION HISTORY :
 * Modifier     Date        Version     Description
 * -------------------------------------------------------------------------------------
 * ryan         3-09-2005   V1.0        First Created
 * peter_yu     3-30-2005   V1.0        Add File Header
 * -------------------------------------------------------------------------------------
 * Copyright(C)                         Accton Technology Corp. 2005
 * -------------------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <string.h>
#include "sys_type.h"
#include "l_cvrt.h"
#include "l_cirlst.h"
#include "sysfun.h"

/* INCLUDE FILE DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */

/* DATA TYPE DECLARACTIONS
 */

/* Pointer structure for the operations of linked list  */

typedef struct
{
    UI8_T           *head;
    UI8_T           *tail;
    UI16_T          nbr_in_list;
    UI16_T          record_len;
    UI32_T          nbr_of_record;
    UI8_T           *last_buffer_address;
} List_T;


/* MACRO FUNCTION DECLARACTIONS
 */
#define LIST_DATA(list)  ((UI8_T*)(list+1))
#define ADVANCE_POINTER(list, p) {  p+=list->record_len; if ( p >= list->last_buffer_address) p=LIST_DATA(list);}

/*---------------------------------
 * Macro for shared memory version
 *---------------------------------
 */
#define LIST_DATA_OFFSET(list) ((list)->buffer_offset)
#define ADVANCE_OFFSET(list, o) { (o)+=(list)->record_len; if ( (o) >= (list)->last_buffer_offset) (o)=LIST_DATA_OFFSET(list);}


/* EXPORTED FUNCTION DECLARACTIONS
 */
/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_Create
 * ---------------------------------------------------------------------
 * PURPOSE  : Create a list
 * INPUT    : list          -- list
 *            nbr_of_record -- max number of record
 *            record_len    -- length for each record
 * OUTPUT   : none
 * RETURN   : descriptor or NULL if fail
 * NOTE     : fail on record_len=0.
 */
L_CIRLST_List_T L_CIRLST_Create ( UI32_T nbr_of_record , UI16_T record_len )
{
    List_T *list;

    if ( record_len <= 0 )
    {
        return (L_CIRLST_List_T) NULL;
    }
    if ( (list = malloc (record_len * nbr_of_record + sizeof(List_T)) ) == NULL )
    {
        return (L_CIRLST_List_T) NULL;
    }

    memset ( list, 0, sizeof(List_T));
    list->head = list->tail = LIST_DATA(list);

    list->nbr_of_record = (UI16_T)nbr_of_record;
    list->record_len = record_len;
    list->last_buffer_address = LIST_DATA(list) + record_len * nbr_of_record ;

    return (L_CIRLST_List_T)list;
} /* L_CIRLST_Create */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_Deq
 * ---------------------------------------------------------------------
 * PURPOSE  : DEQ from list
 *            Deq output order is first in, first out.(FIFO)
 * INPUT    : clist          -- list to operate
 * OUTPUT   : record_p       -- a pointer to store the record address
 * RETURN   : TRUE/FALSE
 * NOTE     : return TRUE when succeed,
 *            FALSE when fail on clist=NULL, record_p=NULL, no record in list.
 */
BOOL_T L_CIRLST_Deq (L_CIRLST_List_T clist, void **element_p)
{
    List_T  *list = (List_T *)clist;

    /* BODY
    */
    if( (list == NULL) || (element_p == NULL) )
    {
        return FALSE;
    }
    if ( list->nbr_in_list == 0 )
    {
        return FALSE;
    }
    *element_p = list->head;
    ADVANCE_POINTER (list, list->head);
    list->nbr_in_list--;

    return TRUE;

} /* L_CIRLST_Deq */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_Enq
 * ---------------------------------------------------------------------
 * PURPOSE  : Append an record to the tail of this list.
 * INPUT    : clist         -- list to operate
 *            record        -- record to be inserted
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : 1. record belong the caller
 *            2. return TRUE when succeed,
 *               FALSE when fail on clist=NULL, record=NULL, record in list is full.
 */
BOOL_T L_CIRLST_Enq ( L_CIRLST_List_T clist , void *element )
{
    List_T  *list = (List_T*)clist;

    /* BODY
    */
    if( (list == NULL) || (element == NULL) )
    {
        return FALSE;
    }
    if ( list->nbr_in_list == list->nbr_of_record )
    {
        return FALSE;
    }
    memcpy (list->tail, element, list->record_len);
    ADVANCE_POINTER (list, list->tail);
    list->nbr_in_list++;

    return TRUE;
} /* L_CIRLST_Enq */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_Clean
 * ---------------------------------------------------------------------
 * PURPOSE  : delete all record in this list.
 * INPUT    : list          -- list to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_CIRLST_Clean ( L_CIRLST_List_T clist )
{
    List_T  *list = (List_T*)clist;

    if( list == NULL )
    {
        return;
    }

    list ->head = list->tail = LIST_DATA(list);
    list->nbr_in_list = 0;
} /* L_CIRLST_Clean */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_Destroy
 * ---------------------------------------------------------------------
 * PURPOSE  : destroy the list.
 * INPUT    : list          -- list to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_CIRLST_Destroy ( L_CIRLST_List_T clist )
{
    free ( (void*) clist );
} /* L_CIRLST_Destroy */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_GetTailPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the list tail pointer.
 * INPUT    : list       -- list to operate
 * OUTPUT   : none
 * RETURN   : pointer of the tail
 * NOTE     : none
 */
UI8_T *L_CIRLST_GetTailPointer (L_CIRLST_List_T clist)
{
    List_T *list = (List_T*)clist;

    return list->tail;
}

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_GetHeadPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the list head pointer.
 * INPUT    : clist       -- list to operate
 * OUTPUT   : none
 * RETURN   : pointer of the head
 * NOTE     : none
 */
UI8_T *L_CIRLST_GetHeadPointer (L_CIRLST_List_T clist)
{
    List_T *list = (List_T*)clist;

    return list->head;
}

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_DoOperationAndMoveHead
 * ---------------------------------------------------------------------
 * PURPOSE  : do specified operation (by pass queue element into function) from head
 *            of the list until some specified location(pointer), move the head of
 *            the list to that location too.
 * INPUT    : clist       -- list to operate
 *            end_p       -- end position (pointer) (not include this position)
 *            func        -- pass list msg into this function
 *            cookie      -- cookie for func
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : 1. the number of func parameters is according to record_len of List_T.
 *               In this function, it only pass in one UI32_T which is assume to
 *               be smaller than record_len (so record_len must larger then 4 bytes).
 */
UI32_T L_CIRLST_DoOperationAndMoveHead (L_CIRLST_List_T clist, UI8_T *end_p, void *func(UI32_T *msg_p, UI32_T cookie), UI32_T cookie)
{
    List_T  *list = (List_T*)clist;
    UI32_T  *ptr;
	UI32_T   ret  = 0;

    if ( NULL == list )
    {
        return ret;
    }

	/* check the validity of the parameter end_p
	 */
	if (end_p >= list->last_buffer_address || end_p < LIST_DATA(list))
	{
	    SYSFUN_Debug_Printf("L_CIRLST: parameter [end_p] out of range\n");
		return ret;
    }
	if (0 != ((end_p - LIST_DATA(list)) % (list->record_len)))
	{
	    SYSFUN_Debug_Printf("L_CIRLST: parameter [end_p] is invalid\n");
	    return ret;
	}

    while (list->head != end_p && list->nbr_in_list > 0)
    {
        ptr = (UI32_T*)list->head;
        func(ptr, cookie);
        ADVANCE_POINTER(list, list->head);
		list->nbr_in_list--;
		ret++;
    }

	return ret;
}



/*---------------------------------
 * Exported functions for shared memory version
 *---------------------------------
 */
/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_Create
 * ---------------------------------------------------------------------
 * PURPOSE  : Create a list
 * INPUT    : working_buffer -- the working buffer for the list in shared memory.
 *            nbr_of_record  -- max number of record
 *            record_len     -- length for each record
 * OUTPUT   : list           -- the descriptor
 * RETURN   : TRUE/FALSE
 * NOTE     : fail on record_len=0.
 */
BOOL_T L_CIRLST_ShMem_Create (L_CIRLST_ShMem_List_T *list,
                              void *working_buffer,
                              UI32_T nbr_of_record,
                              UI16_T record_len)
{
    if ( list == NULL || working_buffer == NULL || record_len <= 0 )
    {
        return FALSE;
    }

    memset ( list, 0, sizeof(L_CIRLST_ShMem_List_T));

    list->buffer_offset = L_CVRT_GET_OFFSET(list, working_buffer);

    list->head_offset = list->tail_offset = LIST_DATA_OFFSET(list);

    list->nbr_of_record = (UI16_T)nbr_of_record;
    list->record_len = record_len;
    list->last_buffer_offset = LIST_DATA_OFFSET(list) + L_CIRLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(nbr_of_record, record_len);

    return TRUE;
} /* L_CIRLST_ShMem_Create */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_Deq
 * ---------------------------------------------------------------------
 * PURPOSE  : DEQ from list
 *            Deq output order is first in, first out.(FIFO)
 * INPUT    : list           -- the descriptor
 * OUTPUT   : record_p       -- a pointer to store the record address
 * RETURN   : TRUE/FALSE
 * NOTE     : return TRUE when succeed,
 *            FALSE when fail on list=NULL, element_p=NULL, no record in list.
 */
BOOL_T L_CIRLST_ShMem_Deq (L_CIRLST_ShMem_List_T *list, void **element_p)
{
    /* BODY
    */
    if( (list == NULL) || (element_p == NULL) )
    {
        return FALSE;
    }
    if ( list->nbr_in_list == 0 )
    {
        return FALSE;
    }
    *element_p = L_CVRT_GET_PTR(list, list->head_offset);
    ADVANCE_OFFSET (list, list->head_offset);
    list->nbr_in_list--;

    return TRUE;

} /* L_CIRLST_ShMem_Deq */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_Enq
 * ---------------------------------------------------------------------
 * PURPOSE  : Append an record to the tail of this list.
 * INPUT    : list           -- the descriptor
 *            record         -- record to be inserted
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : 1. record belong the caller
 *            2. return TRUE when succeed,
 *               FALSE when fail on list=NULL, element=NULL, record in list is full.
 */
BOOL_T L_CIRLST_ShMem_Enq ( L_CIRLST_ShMem_List_T *list , void *element )
{
    /* BODY
    */
    if( (list == NULL) || (element == NULL) )
    {
        return FALSE;
    }
    if ( list->nbr_in_list == list->nbr_of_record )
    {
        return FALSE;
    }
    memcpy (L_CVRT_GET_PTR(list, list->tail_offset), element, list->record_len);
    ADVANCE_OFFSET (list, list->tail_offset);
    list->nbr_in_list++;

    return TRUE;
} /* L_CIRLST_ShMem_Enq */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_Clean
 * ---------------------------------------------------------------------
 * PURPOSE  : delete all record in this list.
 * INPUT    : list           -- the descriptor
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_CIRLST_ShMem_Clean ( L_CIRLST_ShMem_List_T *list )
{
    if( list == NULL )
    {
        return;
    }

    list ->head_offset = list->tail_offset = LIST_DATA_OFFSET(list);
    list->nbr_in_list = 0;
} /* L_CIRLST_ShMem_Clean */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_Destroy
 * ---------------------------------------------------------------------
 * PURPOSE  : destroy the list.
 * INPUT    : list           -- the descriptor
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_CIRLST_ShMem_Destroy ( L_CIRLST_ShMem_List_T *list )
{
    /* nothing to do
     */
} /* L_CIRLST_ShMem_Destroy */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_GetTailPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the list tail pointer.
 * INPUT    : list           -- the descriptor
 * OUTPUT   : none
 * RETURN   : pointer of the tail
 * NOTE     : none
 */
UI8_T *L_CIRLST_ShMem_GetTailPointer (L_CIRLST_ShMem_List_T *list)
{
    return L_CVRT_GET_PTR(list, list->tail_offset);
}

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_GetHeadPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the list head pointer.
 * INPUT    : list           -- the descriptor
 * OUTPUT   : none
 * RETURN   : pointer of the head
 * NOTE     : none
 */
UI8_T *L_CIRLST_ShMem_GetHeadPointer (L_CIRLST_ShMem_List_T *list)
{
    return L_CVRT_GET_PTR(list, list->head_offset);
}

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_DoOperationAndMoveHead
 * ---------------------------------------------------------------------
 * PURPOSE  : do specified operation (by pass queue element into function) from head
 *            of the list until some specified location(pointer), move the head of
 *            the list to that location too.
 * INPUT    : list           -- the descriptor
 *            end_p       -- end position (pointer) (not include this position)
 *            func        -- pass list msg into this function
 *            cookie      -- cookie for func
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : 1. the number of func parameters is according to record_len of L_CIRLST_ShMem_List_T.
 *               In this function, it only pass in one UI32_T which is assume to
 *               be smaller than record_len (so record_len must larger then 4 bytes).
 */
UI32_T L_CIRLST_ShMem_DoOperationAndMoveHead (L_CIRLST_ShMem_List_T *list, UI8_T *end_p, void *func(UI32_T *msg_p, UI32_T cookie), UI32_T cookie)
{
    UI32_T  end;
    UI32_T  *ptr;
	UI32_T  ret  = 0;

    if ( NULL == list )
    {
        return ret;
    }

	/* check the validity of the parameter end_p
	 */
	end = L_CVRT_GET_OFFSET(list, end_p);
	if (end >= list->last_buffer_offset || end < LIST_DATA_OFFSET(list))
	{
	    SYSFUN_Debug_Printf("L_CIRLST: parameter [end_p] out of range\n");
		return ret;
    }
	if (0 != ((end - LIST_DATA_OFFSET(list)) % (list->record_len)))
	{
	    SYSFUN_Debug_Printf("L_CIRLST: parameter [end_p] is invalid\n");
	    return ret;
	}

    while (list->head_offset != end && list->nbr_in_list > 0)
    {
        ptr = L_CVRT_GET_PTR(list, list->head_offset);
        func(ptr, cookie);
        ADVANCE_OFFSET(list, list->head_offset);
		list->nbr_in_list--;
		ret++;
    }

	return ret;
}

#ifdef L_CIRLST_UNIT_TEST
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "l_cirlst.h"
#include "sysfun.h"

#define L_CIRLST_UNIT_TEST_SHMEM_KEY      10
#define L_CIRLST_UNIT_TEST_BUFFER_SIZE    1024
#define L_CIRLST_UNIT_TEST_MAGIC_FLAG     0x13452898

#define L_CIRLST_UNIT_TEST_NBR_OF_RECORD  100
#define L_CIRLST_UNIT_TEST_RECORD_LEN     4

int L_CIRLST_ShMem_main()
{
    UI32_T shm_id;
    UI32_T *shm_p;
    L_CIRLST_ShMem_List_T *desc_p;
    int i;

    srand(time(NULL));

    assert(SYSFUN_CreateShMem(L_CIRLST_UNIT_TEST_SHMEM_KEY, L_CIRLST_UNIT_TEST_BUFFER_SIZE, &shm_id) == SYSFUN_OK);

    assert(shm_p = SYSFUN_AttachShMem(shm_id));
    desc_p = (L_CIRLST_ShMem_List_T *)(shm_p + 1);

    if (*shm_p != L_CIRLST_UNIT_TEST_MAGIC_FLAG)
    {
        assert(sizeof(UI32_T) + sizeof(L_CIRLST_ShMem_List_T) + L_CIRLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(L_CIRLST_UNIT_TEST_NBR_OF_RECORD, L_CIRLST_UNIT_TEST_RECORD_LEN) <= L_CIRLST_UNIT_TEST_BUFFER_SIZE);
        assert(L_CIRLST_ShMem_Create(desc_p, (char *)desc_p + sizeof(*desc_p), L_CIRLST_UNIT_TEST_NBR_OF_RECORD, L_CIRLST_UNIT_TEST_RECORD_LEN));
        *shm_p = L_CIRLST_UNIT_TEST_MAGIC_FLAG;
    }

    printf("head %x\t", (unsigned int)L_CIRLST_ShMem_GetHeadPointer(desc_p));
    printf("tail %x\t", (unsigned int)L_CIRLST_ShMem_GetTailPointer(desc_p));

    for (i = 0; i < 100; i++)
    {
        UI32_T element, *element_p;

        switch (rand() % 10)
        {
            case 0:
                L_CIRLST_ShMem_Clean(desc_p);
                printf("clean\t");
                break;

            case 1:
            case 2:
            case 3:
                if (L_CIRLST_ShMem_Deq(desc_p, (void **)&element_p))
                    printf("pop %lu\t", *element_p);
                else
                    printf("empty\t");
                break;

            default:
                element = rand() % 10;
                if (L_CIRLST_ShMem_Enq(desc_p, &element))
                    printf("push %lu\t", element);
                else
                    printf("full\t");
        }
    }

    printf("\n");

    SYSFUN_DetachShMem(desc_p);

    return 0;
}
#endif /* L_CIRLST_UNIT_TEST */

