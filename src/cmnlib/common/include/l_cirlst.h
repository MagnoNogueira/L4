/* -------------------------------------------------------------------------------------
 * FILE NAME : L_CIRLST.h (CIRCULAR LIST)
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

#ifndef L_CIRLST_H
#define L_CIRLST_H

#include "sys_type.h"


/* NAMING CONSTANT DECLARACTIONS
 */

/* DATA TYPE DECLARACTIONS
 */
typedef UI32_T  L_CIRLST_List_T;


/*---------------------------------
 * Types for shared memory version
 *---------------------------------
 */
typedef struct
{
    I32_T           head_offset; /* the relative address of the first record with desc_p as base addr */
    I32_T           tail_offset; /* the relative address of the last record with desc_p as base addr */
    UI16_T          nbr_in_list;
    UI16_T          record_len;
    UI32_T          nbr_of_record;
    I32_T           last_buffer_offset;  /* the relative address of the tail of the working buffer with desc_p as base addr */
    I32_T           buffer_offset; /* the relative address of the working buffer with desc_p as base addr */
} L_CIRLST_ShMem_List_T;


/* MACRO FUNCTION DECLARACTIONS
 */
/*---------------------------------
 * Macro for shared memory version
 *---------------------------------
 */
/* ---------------------------------------------------------------------
 * MACRO NAME - L_CIRLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ
 * ---------------------------------------------------------------------
 * PURPOSE  : Calculate the required size of the buffer for a list
 * INPUT    : nbr_of_record -- max number of record
 *            record_len    -- length for each record
 * OUTPUT   : none
 * RETURN   : The required size
 * NOTE     : none
 */
#define L_CIRLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(nbr_of_record, record_len)    ((record_len) * (nbr_of_record))


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
L_CIRLST_List_T L_CIRLST_Create ( UI32_T nbr_of_record, UI16_T record_len);



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
BOOL_T L_CIRLST_Deq (L_CIRLST_List_T clist, void **record_p);


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
BOOL_T L_CIRLST_Enq (L_CIRLST_List_T clist, void *record);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_Clean
 * ---------------------------------------------------------------------
 * PURPOSE  : delete all record in this list.
 * INPUT    : clist          -- list to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_CIRLST_Clean (L_CIRLST_List_T clist);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_Destroy
 * ---------------------------------------------------------------------
 * PURPOSE  : destroy the list.
 * INPUT    : clist          -- list to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_CIRLST_Destroy (L_CIRLST_List_T clist);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_GetTailPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the list tail pointer.
 * INPUT    : clist       -- list to operate
 * OUTPUT   : none
 * RETURN   : pointer of the tail
 * NOTE     : none
 */
UI8_T *L_CIRLST_GetTailPointer (L_CIRLST_List_T clist);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_GetHeadPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the list head pointer.
 * INPUT    : clist       -- list to operate
 * OUTPUT   : none
 * RETURN   : pointer of the head
 * NOTE     : none
 */
UI8_T *L_CIRLST_GetHeadPointer (L_CIRLST_List_T clist);

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
 * RETURN   : affect(decease) how many element in queue
 * NOTE     : 1. the number of func parameters is according to record_len of List_T.
 *               In this function, it only pass in one UI32_T which is assume to
 *               be smaller than record_len (so record_len must larger then 4 bytes).
 */
UI32_T L_CIRLST_DoOperationAndMoveHead (L_CIRLST_List_T clist, UI8_T *endp, void *func(UI32_T *msg_p, UI32_T cookie), UI32_T cookie);



/*---------------------------------
 * Exported functions for shared memory version
 *---------------------------------
 */
/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_Create
 * ---------------------------------------------------------------------
 * PURPOSE  : Create a list
 * INPUT    : working_buffer -- the working buffer for the list in shared memory.
 *                              the size must be L_CIRLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ()
 *            nbr_of_record  -- max number of record
 *            record_len     -- length for each record
 * OUTPUT   : list           -- the descriptor
 * RETURN   : TRUE/FALSE
 * NOTE     : 1. fail on record_len=0.
 *            2. the descriptor and working buffer must be in shared memory
 */
BOOL_T L_CIRLST_ShMem_Create (L_CIRLST_ShMem_List_T *list,
                              void *working_buffer,
                              UI32_T nbr_of_record,
                              UI16_T record_len);

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
BOOL_T L_CIRLST_ShMem_Deq (L_CIRLST_ShMem_List_T *list, void **element_p);

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
BOOL_T L_CIRLST_ShMem_Enq ( L_CIRLST_ShMem_List_T *list , void *element );

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_Clean
 * ---------------------------------------------------------------------
 * PURPOSE  : delete all record in this list.
 * INPUT    : list           -- the descriptor
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_CIRLST_ShMem_Clean ( L_CIRLST_ShMem_List_T *list );

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_Destroy
 * ---------------------------------------------------------------------
 * PURPOSE  : destroy the list.
 * INPUT    : list           -- the descriptor
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_CIRLST_ShMem_Destroy ( L_CIRLST_ShMem_List_T *list );

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_GetTailPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the list tail pointer.
 * INPUT    : list           -- the descriptor
 * OUTPUT   : none
 * RETURN   : pointer of the tail
 * NOTE     : none
 */
UI8_T *L_CIRLST_ShMem_GetTailPointer (L_CIRLST_ShMem_List_T *list);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_CIRLST_ShMem_GetHeadPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the list head pointer.
 * INPUT    : list           -- the descriptor
 * OUTPUT   : none
 * RETURN   : pointer of the head
 * NOTE     : none
 */
UI8_T *L_CIRLST_ShMem_GetHeadPointer (L_CIRLST_ShMem_List_T *list);

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
UI32_T L_CIRLST_ShMem_DoOperationAndMoveHead (L_CIRLST_ShMem_List_T *list, UI8_T *end_p, void *func(UI32_T *msg_p, UI32_T cookie), UI32_T cookie);

#endif
