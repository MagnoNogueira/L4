/* =====================================================================================
 * MODULE NAME: L_HASH.h
 *
 * PURPOSE: This package provides function to utilize HASH method for data storage.
 *          An efficient Hash algorithm can effectively reduce memory required for
 *          database management and modification.
 * NOTES:	
 *    1. The Caller must take care the mutual exculusion situation
 *    2. HASH is a pure code.
 *
 *  History :
 *      Date            Modifier    Reason
 *  -----------------------------------------------------------------------
 *      04-30-2004        amytu     First Create
 * -------------------------------------------------------------------------------------
 * Copyright(C)        Accton Techonology Corporation, 2004
 * =====================================================================================*/

#ifndef _L_HASH_H
#define _L_HASH_H

#include "l_dlist.h"

/* NAMING CONSTANT DECLARACTIONS
 */
#define L_HASH_MAX_FIELD_NUMBER_OF_A_KEY      8
#define L_HASH_MAX_QUERY_GROUP                8

enum
{
    L_HASH_SEARCH_COMPLETED = 0,
    L_HASH_SEARCH_END_OF_LIST,
    L_HASH_SEARCH_BREAK,
    L_HASH_SEARCH_CONTINUE,
    L_HASH_SEARCH_DEL_AND_BREAK,
    L_HASH_SEARCH_DEL_AND_CONTINUE,
    L_HASH_SEARCH_INVALID_INDEX	
};

/*---------------------------------
 * Constants for shared memory version
 *---------------------------------
 */
/* The type of acquire function for the sorted list in shared memory.
 */
enum {
#if (SYS_CPNT_SWDRVL3 == TRUE)
    L_HASH_SHMEM_ACQUIRE_TYPE_SWDRVL3_HOSTROUTE,
    L_HASH_SHMEM_ACQUIRE_TYPE_SWDRVL3_NETROUTE,
#endif
#if (SYS_CPNT_AMTRDRV == TRUE)
    L_HASH_SHMEM_ACQUIRE_TYPE_AMTRDRV_BYGROUP,
#endif
    L_HASH_SHMEM_ACQUIRE_TYPE_MAX
};


/* DATA TYPE DECLARATIONS
 */
typedef UI16_T L_HASH_Index_T;

/* Represents different hashing algorithm to store record into
 * the hash tables
 */
typedef enum
{
    L_HASH_HASH_METHOD_WORD_XOR = 1
} L_HASH_Method_T;

typedef enum
{
    L_HASH_ACTION_ADD = 1,
    L_HASH_ACTION_DEL = 2
} L_HASH_Action_T;

typedef enum
{
    L_HASH_SET_SUCCESS_EV,
    L_HASH_DEL_SUCCESS_EV,
    L_HASH_SYNC_DONE_EV,
    L_HASH_FAIL_EV,
    L_HASH_LAST_EV
} L_HASH_Event_T;

/* 1. Data Record table must start from 0.
 * 2. This Hash database supports single set of Hash Key
 */
typedef struct
{
    UI16_T  nbr_of_hash_bucket;
    UI16_T  nbr_of_rec;
    UI16_T  key_offset[L_HASH_MAX_FIELD_NUMBER_OF_A_KEY];
    UI16_T  key_size[L_HASH_MAX_FIELD_NUMBER_OF_A_KEY];
    UI16_T  element_of_query_group [L_HASH_MAX_QUERY_GROUP];
    BOOL_T  (*acquire_element_index_of_query_group_fun)
            (UI32_T group, void *record, UI32_T *element);
    UI16_T  record_size;
    UI8_T   hash_method;   /* how to get the bucket number.... */
    void    *buffer;
} L_HASH_Desc_T;

/*---------------------------------
 * Types for shared memory version
 *---------------------------------
 */
typedef struct
{
    UI16_T  nbr_of_hash_bucket;
    UI16_T  nbr_of_rec;
    UI16_T  key_offset[L_HASH_MAX_FIELD_NUMBER_OF_A_KEY];
    UI16_T  key_size[L_HASH_MAX_FIELD_NUMBER_OF_A_KEY];
    UI16_T  element_of_query_group [L_HASH_MAX_QUERY_GROUP];
    UI16_T  acquire_element_index_of_query_group_fun_id;
    UI16_T  record_size;
    UI8_T   hash_method;   /* how to get the bucket number.... */
    I32_T   buffer_offset; /* relative address of the working buffer with desc_p as base addr. */
} L_HASH_ShMem_Desc_T;



/* EXPORTED FUNCTION PROTOTYPE
 */
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_Init
 * -------------------------------------------------------------------------
 * PURPOSE: None.
 * INPUT  : None.
 * OUTPUT : None.
 * RETURN : None.
 * NOTES  : None.
 * -------------------------------------------------------------------------*/
void L_HASH_Init(void);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_Create_InterCSC_Relation
 * -------------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT  : None.
 * OUTPUT : None.
 * RETURN : None.
 * NOTES  : None.
 * -------------------------------------------------------------------------*/
void L_HASH_Create_InterCSC_Relation(void);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_Create
 * -------------------------------------------------------------------------
 * PURPOSE: This function creates a table using Hash algorithm.
 * INPUT  : desc-
 *          nbr_of_hash_bucket
 *          nbr_of_rec
 *          key_offset
 *          key_size
 *          element_of_query_group
 *          *acquire_element_index_of_query_group_fun
 *          record_size
 *          hash_method
 * OUTPUT : desc  -- the handle of a new data storage (Database) after creation.
 * RETURN : TRUE \ FALSE
 * NOTES:   desc->element_of_query_group[index_of_query_group] has to be 0 for the un-existed Query Groups.
 *          CSC can use memset(desc,0, sizeof(L_HASH_Desc_T)) first to set them to 0, then initialize fields of desc to its specific requirement.
 *
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_Create(L_HASH_Desc_T  *desc);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_SetRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Set a record, added or replaced, into HASH database.
 * INPUT  : desc  - A specific desciptor to process.
 *          rec   - A specific record to be add.
 *          index - Added record is with this index
 * OUTPUT : index - hash_record_index
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_SetRecord (L_HASH_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_SetRecordToHashOnly
 * -------------------------------------------------------------------------
 * PURPOSE: Set a record, added or replaced, into HASH database.
 * INPUT  : desc - A specific desciptor to process.
 *          rec  - A specific record to be add.
 *          index - Added record is with this index
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : This API is only set record in Hash table not insert to job queue
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_SetRecordToHashOnly(L_HASH_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_DeleteRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Delete a record from HASH database.
 * INPUT  : desc  - A specific desciptor to process.
 *          rec   - A specific record to be removed.
 *          index - Deleted record is with this index
 * OUTPUT : index - hash record index.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_DeleteRecord (L_HASH_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_DeleteRecordFromHashOnly
 * -------------------------------------------------------------------------
 * PURPOSE: Delete a record from HASH database.
 * INPUT  : desc - A specific desciptor to process.
 *          rec  - A specific record to be removed.
 *          index - Deleted record is with this index
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : This API is delete Record from Hash table directly not running FSM.
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_DeleteRecordFromHashOnly (L_HASH_Desc_T *desc, UI8_T  *rec, L_HASH_Index_T *index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_DeleteRecordNWaitSync
 * -------------------------------------------------------------------------
 * PURPOSE: Delete a record from HASH database with FSM.
 * INPUT  : desc - A specific desciptor to process.
 *          rec  - A specific record to be removed.
 *          index - Deleted record is with this index
 * OUTPUT : index - hash record index.
 * RETURN : TRUE \ FALSE
 * NOTES  : In this API, record won't be put in job queue.
 *              And enter "L_HASH_DEL_WAIT_SYNC_ST" state directly.
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_DeleteRecordNWaitSync (L_HASH_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_DeleteAll
 * -------------------------------------------------------------------------
 * PURPOSE: Delete all records from HASH database.
 * INPUT  : desc - A specific desciptor to be removed.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_DeleteAll (L_HASH_Desc_T  *desc);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_Destroy
 * -------------------------------------------------------------------------
 * PURPOSE: Destroy the existing Hash database.
 * INPUT  : desc - A specific desciptor to process.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_Destroy (L_HASH_Desc_T  *desc);


/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_SearchByQueryGroup
 * -------------------------------------------------------------------------
 * PURPOSE: Search all items in that giving Query Group.
 * INPUT  : desc          - A specific desciptor to process.
 *          query_group   - A specific query category to delete.
 *          query_element - Index of the specific category to operate.
 *          cookie        - conditions checking for call back function
 *          *call_back    - a call back function to provide the specified function.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *         (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_SearchByQueryGroup(L_HASH_Desc_T  *desc, UI32_T query_group,
                                 UI32_T query_element,UI32_T cookie,
                                 UI32_T (*call_back)(UI16_T index, UI32_T cookie));

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_SearchByQueryGroupAndDelete
 * -------------------------------------------------------------------------
 * PURPOSE: Search all items from that giving Query Group and delete them.
 * INPUT  : desc          - A specific desciptor to process.
 *          query_group   - A specific query category to delete.
 *          query_element - Index of the specific category to operate.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : This is a default delete records function. This function may call
 *          L_Hash_SearchByQueryGroup with default delete callback function.
 *          Because the user doesn't need to implement another callback function
 *          to delete records. If user needs to delete some specified records not all.
 *          It needs to use L_HASH_SearchByQueryGroup and also with it's own callback
 *          function to reach the goal.
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_SearchByQueryGroupAndDelete(L_HASH_Desc_T  *desc, UI32_T query_group, UI32_T query_element);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetQueryGroupDescriptor
 * -------------------------------------------------------------------------
 * PURPOSE: Get query group eleement's descriptor
 * INPUT  : desc          - A specific desciptor to process.
 *          query_group   - A specific query category to delete.
 *          query_element - Index of the specific category to operate.
 * OUTPUT : L_DLST_Indexed_Dblist_T dblist -- the specific query group's descriptor
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *         (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_GetQueryGroupDescriptor(L_HASH_Desc_T  *desc, UI32_T query_group,
                                 UI32_T query_element, L_DLST_Indexed_Dblist_T **dblist);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_DequeueJobList
 * -------------------------------------------------------------------------
 * PURPOSE: Dequeue a record from JobList for application to process.
 * INPUT  : desc   - A specific desciptor to process.
 * OUTPUT : rec    - A specific record for application to process
 *          action - L_HASH_ACTION_ADD \ L_HASH_ACTION_DEL
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_DequeueJobList (L_HASH_Desc_T  *desc, UI8_T **rec, UI8_T *action, L_HASH_Index_T *idx);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_OperationResult
 * -------------------------------------------------------------------------
 * PURPOSE: Result of the job dequeue from JobList.
 * INPUT  : desc  - A specific desciptor to process.
 *          event - L_HASH_SUCCESS_EV \ L_HASH_FAIL_EV
 *                  L_HASH_SYNC_OK_EV \ L_HASH_SYNC_FAIL_EV
 *          rec   - Specific record
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_OperationResult (L_HASH_Desc_T  *desc, UI32_T event, UI8_T *user_rec);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetExactRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Get exact record at that time
 * INPUT  : desc     - A specific desciptor to process.
 *          user_rec - the address entry need to be compared.
 * OUTPUT : user_rec - the exact record
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_GetExactRecord(L_HASH_Desc_T  *desc, UI8_T  *user_rec);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetRecordByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get a Record by giving index
 * INPUT  : desc        - A specific desciptor to process.
 *          index       - record index
 * OUTPUT : user_record - the address record
 *          action      - record action
 * RETURN : None
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_GetRecordByIndex(L_HASH_Desc_T  *desc,UI32_T index,UI8_T *user_rec,UI8_T *action);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetRecordPtrByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get the Record Pointer by index
 * INPUT  : desc      - A specific desciptor to process.
 *          index     -
 *          user_rec  - the address entry
 * OUTPUT : None
 * RETURN : None
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_GetRecordPtrByIndex(L_HASH_Desc_T  *desc, UI8_T **user_rec,UI32_T index );

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetRecordIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get exact record's index in database by giving record
 * INPUT  : desc         - A specific desciptor to process.
 *          user_rec     - the address entry need to be compared.
 * OUTPUT : record_index - the record's index in OM
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_GetRecordIndex(L_HASH_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *record_index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetRecordSyncValueByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get a Record sync value by index
 * INPUT  : desc           - A specific desciptor to process.
 *          index          -
 *          sync_value     - to determine it still wait for sync to hisam or callback
 * OUTPUT : None
 * RETURN : None
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_GetRecordSyncValueByIndex(L_HASH_Desc_T  *desc, UI8_T *sync_value, UI32_T index );

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_SetRecordSyncValueByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Set a Record sync value by index
 * INPUT  : desc           - A specific desciptor to process.
 *          index          -
 *          sync_value     - to determine it still wait for sync to hisam or callback
 * OUTPUT : None
 * RETURN : None
 * NOTES  : None
 *         (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_SetRecordSyncValueByIndex(L_HASH_Desc_T  *desc, UI8_T sync_value, UI32_T index );

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_UpdateQueryGroup
 * -------------------------------------------------------------------------
 * PURPOSE: Replace exist record in HASH database.  Check each query group
 *          to make sure record still associates with the correct query group
 *          element.
 * INPUT  : dblist        - Address of dblist.
 * OUTPUT : query_element - index of the query group.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_UpdateQueryGroup(L_HASH_Desc_T *desc, L_HASH_Index_T idx, UI8_T *user_rec);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetNRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Display N record at a time
 * INPUT:   desc         - A specific desciptor to process.
 *          record_count - The number of record to display
 * OUTPUT:  None
 * RETURN:  TRUE \ FALSE
 * NOTES:   This function is for backdoor used only
 * -------------------------------------------------------------------------*/
void L_HASH_GetNRecord(L_HASH_Desc_T  *desc, UI32_T record_count);



/*---------------------------------
 * Exported functions for shared memory version
 *---------------------------------
 */
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetWorkingBufferRequiredSize
 * -------------------------------------------------------------------------
 * PURPOSE: Calculate the required size of the working buffer for a hash
 * INPUT  : desc-
 *          nbr_of_hash_bucket
 *          nbr_of_rec
 *          element_of_query_group
 *          record_size
 * OUTPUT:  None
 * RETURN:  The required size
 * NOTES:   None
 * -------------------------------------------------------------------------*/
UI32_T L_HASH_ShMem_GetWorkingBufferRequiredSize(L_HASH_ShMem_Desc_T  *desc);


/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_Create
 * -------------------------------------------------------------------------
 * PURPOSE: This function creates a table using Hash algorithm.
 * INPUT  : desc-
 *          nbr_of_hash_bucket
 *          nbr_of_rec
 *          key_offset
 *          key_size
 *          element_of_query_group
 *          acquire_element_index_of_query_group_fun_id
 *          record_size
 *          hash_method
 *          buffer_offset -- the relative address of the working buffer with desc_p as base addr.
 *                           the size of the buffer must be L_HASH_ShMem_GetWorkingBufferRequiredSize()
 * OUTPUT:  desc  -- the handle of a new data storage (Database) after creation.
 * RETURN:  TRUE \ FALSE
 * NOTES:   1. desc->element_of_query_group[index_of_query_group] has to be 0 for the un-existed Query Groups.
 *             CSC can use memset(desc,0, sizeof(L_HASH_ShMem_Desc_T)) first to set them to 0, then initialize fields of desc to its specific requirement.
 *          2. the descriptor and working buffer must be in shared memory
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_Create(L_HASH_ShMem_Desc_T  *desc);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_SetRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Set a record, added or replaced, into HASH database.
 * INPUT  : desc  - A specific desciptor to process.
 *          rec   - A specific record to be add.
 *          index - Added record is with this index
 * OUTPUT : index - hash_record_index
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_SetRecord (L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_SetRecordToHashOnly
 * -------------------------------------------------------------------------
 * PURPOSE: Set a record, added or replaced, into HASH database.
 * INPUT  : desc - A specific desciptor to process.
 *          rec  - A specific record to be add.
 *          index - Added record is with this index
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : This API is only set record in Hash table not insert to job queue
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_SetRecordToHashOnly(L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_DeleteRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Delete a record from HASH database.
 * INPUT  : desc  - A specific desciptor to process.
 *          rec   - A specific record to be removed.
 *          index - Deleted record is with this index
 * OUTPUT : index - hash record index.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_DeleteRecord (L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_DeleteRecordFromHashOnly
 * -------------------------------------------------------------------------
 * PURPOSE: Delete a record from HASH database.
 * INPUT  : desc - A specific desciptor to process.
 *          rec  - A specific record to be removed.
 *          index - Deleted record is with this index
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : This API is delete Record from Hash table directly not running FSM.
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_DeleteRecordFromHashOnly (L_HASH_ShMem_Desc_T *desc, UI8_T  *rec, L_HASH_Index_T *index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_DeleteRecordNWaitSync
 * -------------------------------------------------------------------------
 * PURPOSE: Delete a record from HASH database with FSM.
 * INPUT  : desc - A specific desciptor to process.
 *          rec  - A specific record to be removed.
 *          index - Deleted record is with this index
 * OUTPUT : index - hash record index.
 * RETURN : TRUE \ FALSE
 * NOTES  : In this API, record won't be put in job queue.
 *              And enter "L_HASH_DEL_WAIT_SYNC_ST" state directly.
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_DeleteRecordNWaitSync (L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_DeleteAll
 * -------------------------------------------------------------------------
 * PURPOSE: Delete all records from HASH database.
 * INPUT  : desc - A specific desciptor to be removed.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_DeleteAll (L_HASH_ShMem_Desc_T  *desc);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_Destroy
 * -------------------------------------------------------------------------
 * PURPOSE: Destroy the existing Hash database.
 * INPUT  : desc - A specific desciptor to process.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_Destroy (L_HASH_ShMem_Desc_T  *desc);


/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_SearchByQueryGroup
 * -------------------------------------------------------------------------
 * PURPOSE: Search all items in that giving Query Group.
 * INPUT  : desc          - A specific desciptor to process.
 *          query_group   - A specific query category to delete.
 *          query_element - Index of the specific category to operate.
 *          cookie        - conditions checking for call back function
 *          *call_back    - a call back function to provide the specified function.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *         (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_SearchByQueryGroup(L_HASH_ShMem_Desc_T  *desc, UI32_T query_group,
                                 UI32_T query_element,UI32_T cookie,
                                 UI32_T (*call_back)(UI16_T index, UI32_T cookie));

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_SearchByQueryGroupAndDelete
 * -------------------------------------------------------------------------
 * PURPOSE: Search all items from that giving Query Group and delete them.
 * INPUT  : desc          - A specific desciptor to process.
 *          query_group   - A specific query category to delete.
 *          query_element - Index of the specific category to operate.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : This is a default delete records function. This function may call
 *          L_Hash_SearchByQueryGroup with default delete callback function.
 *          Because the user doesn't need to implement another callback function
 *          to delete records. If user needs to delete some specified records not all.
 *          It needs to use L_HASH_ShMem_SearchByQueryGroup and also with it's own callback
 *          function to reach the goal.
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_SearchByQueryGroupAndDelete(L_HASH_ShMem_Desc_T  *desc, UI32_T query_group, UI32_T query_element);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetQueryGroupDescriptor
 * -------------------------------------------------------------------------
 * PURPOSE: Get query group eleement's descriptor
 * INPUT  : desc          - A specific desciptor to process.
 *          query_group   - A specific query category to delete.
 *          query_element - Index of the specific category to operate.
 * OUTPUT : L_DLST_Indexed_Dblist_T dblist -- the specific query group's descriptor
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *         (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_GetQueryGroupDescriptor(L_HASH_ShMem_Desc_T  *desc, UI32_T query_group,
                                 UI32_T query_element, L_DLST_ShMem_Indexed_Dblist_T **dblist);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_DequeueJobList
 * -------------------------------------------------------------------------
 * PURPOSE: Dequeue a record from JobList for application to process.
 * INPUT  : desc   - A specific desciptor to process.
 * OUTPUT : rec    - A specific record for application to process
 *          action - L_HASH_ACTION_ADD \ L_HASH_ACTION_DEL
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_DequeueJobList (L_HASH_ShMem_Desc_T  *desc, UI8_T **rec, UI8_T *action, L_HASH_Index_T *idx);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_OperationResult
 * -------------------------------------------------------------------------
 * PURPOSE: Result of the job dequeue from JobList.
 * INPUT  : desc  - A specific desciptor to process.
 *          event - L_HASH_SUCCESS_EV \ L_HASH_FAIL_EV
 *                  L_HASH_SYNC_OK_EV \ L_HASH_SYNC_FAIL_EV
 *          rec   - Specific record
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_OperationResult (L_HASH_ShMem_Desc_T  *desc, UI32_T event, UI8_T *user_rec);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetExactRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Get exact record at that time
 * INPUT  : desc     - A specific desciptor to process.
 *          user_rec - the address entry need to be compared.
 * OUTPUT : user_rec - the exact record
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_GetExactRecord(L_HASH_ShMem_Desc_T  *desc, UI8_T  *user_rec);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetRecordByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get a Record by giving index
 * INPUT  : desc        - A specific desciptor to process.
 *          index       - record index
 * OUTPUT : user_record - the address record
 *          action      - record action
 * RETURN : None
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_ShMem_GetRecordByIndex(L_HASH_ShMem_Desc_T  *desc,UI32_T index,UI8_T *user_rec,UI8_T *action);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetRecordPtrByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get the Record Pointer by index
 * INPUT  : desc      - A specific desciptor to process.
 *          index     -
 *          user_rec  - the address entry
 * OUTPUT : None
 * RETURN : None
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_ShMem_GetRecordPtrByIndex(L_HASH_ShMem_Desc_T  *desc, UI8_T **user_rec,UI32_T index );

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetRecordIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get exact record's index in database by giving record
 * INPUT  : desc         - A specific desciptor to process.
 *          user_rec     - the address entry need to be compared.
 * OUTPUT : record_index - the record's index in OM
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_GetRecordIndex(L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *record_index);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetRecordSyncValueByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get a Record sync value by index
 * INPUT  : desc           - A specific desciptor to process.
 *          index          -
 *          sync_value     - to determine it still wait for sync to hisam or callback
 * OUTPUT : None
 * RETURN : None
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_ShMem_GetRecordSyncValueByIndex(L_HASH_ShMem_Desc_T  *desc, UI8_T *sync_value, UI32_T index );

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_SetRecordSyncValueByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Set a Record sync value by index
 * INPUT  : desc           - A specific desciptor to process.
 *          index          -
 *          sync_value     - to determine it still wait for sync to hisam or callback
 * OUTPUT : None
 * RETURN : None
 * NOTES  : None
 *         (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_ShMem_SetRecordSyncValueByIndex(L_HASH_ShMem_Desc_T  *desc, UI8_T sync_value, UI32_T index );

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_UpdateQueryGroup
 * -------------------------------------------------------------------------
 * PURPOSE: Replace exist record in HASH database.  Check each query group
 *          to make sure record still associates with the correct query group
 *          element.
 * INPUT  : dblist        - Address of dblist.
 * OUTPUT : query_element - index of the query group.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_UpdateQueryGroup(L_HASH_ShMem_Desc_T *desc, L_HASH_Index_T idx, UI8_T *user_rec);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetNRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Display N record at a time
 * INPUT:   desc         - A specific desciptor to process.
 *          record_count - The number of record to display
 * OUTPUT:  None
 * RETURN:  TRUE \ FALSE
 * NOTES:   This function is for backdoor used only
 * -------------------------------------------------------------------------*/
void L_HASH_ShMem_GetNRecord(L_HASH_ShMem_Desc_T  *desc, UI32_T record_count);

/*these functions are used for MAC Learning*/
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_UpdateReservedState
 * -------------------------------------------------------------------------
 * PURPOSE: update the hash record resverved state
 * INPUT  : desc        - Specific descriptor
 *          idx         - specific table index in record table.
 *          user_state  - the state of FSM
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : ADD by MIKE_YEH
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_UpdateReservedState(L_HASH_ShMem_Desc_T *desc,UI32_T idx,UI8_T user_state);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetReservedState
 * -------------------------------------------------------------------------
 * PURPOSE: get the hash record reserved state
 * INPUT  : desc        - Specific descriptor
 *          idx         - specific table index in record table.
 *          *user_state  - the state of reserved
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : ADD by MIKE_YEH
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_GetReservedState(L_HASH_ShMem_Desc_T *desc,UI32_T idx,UI8_T *user_state);

/*these functions are used for MAC Learning*/
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_ClearReservedState
 * -------------------------------------------------------------------------
 * PURPOSE: clear the hash record resverved state
 * INPUT  : desc        - Specific descriptor
 *          idx         - specific table index in record table.
 *          user_state  - the state of FSM
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : ADD by MIKE_YEH
 * -------------------------------------------------------------------------*/

BOOL_T L_HASH_ShMem_ClearReservedState(L_HASH_ShMem_Desc_T *desc,UI32_T idx,UI8_T user_flag);

#endif
