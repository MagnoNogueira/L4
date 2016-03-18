/* =====================================================================================
 * MODULE NAME: L_HASH.c
 *
 * PURPOSE: This package provides function to utilize HASH method for data storage.
 *          An efficient Hash algorithm can effectively reduce memory required for
 *          database management and modification.
 * NOTES:
 *    1. The Caller must take care the mutual exculusion situation
 *    2. HASH is a pure code.
 *    3. Format of Hash Workspace
 *
 *      |--------------------------------------|
 *      |       Job Queue Descriptor           |
 *      |--------------------------------------|
 *      |       Job Queue Link Buffer Pointer  |  pointer points to Job Queue Link
 *      |--------------------------------------|
 *      |       Hash Header Pointer            |  pointer points to hash header Bucket
 *      |--------------------------------------|
 *      |       Data Record Pointer            |  pointer points to data record
 *      |--------------------------------------|
 *      |       Query Group Pointer            |  Q1 points to Query Group 1
 *      |  Q1 | Q2 | Q3 | Q4 | Q5 | Q6 | Q7    |
 *      |--------------------------------------|
 *      |       Job Queue Link [rec_no]        |
 *      |--------------------------------------|
 *      |       Hash Header Bucket             |
 *      |--------------------------------------|
 *      |                                      |
 *      |       Data Record                    |
 *      |                                      |
 *      |--------------------------------------|
 *      |       Query Grop 1                   |
 *      |--------------------------------------|
 *      |       Query Gruop 1  clone 1 dblist  |
 *      |--------------------------------------|
 *      |       Query Gruop 1  clone 2 dblist  |
 *      |--------------------------------------|
 *      |       Query Gruop 1  clone n dblist  |
 *      | -------------------------------------|
 *      |       Query Grop 2                   |
 *      |--------------------------------------|
 *      |       Query Gruop 2  clone n dblist  |
 *      | -------------------------------------|
 *      |       Query Group i                  |
 *      |--------------------------------------|
 *      |       Query Gruop i   clone n dblist |
 *      | -------------------------------------|
 *      |       Query Grop 7                   |
 *      |--------------------------------------|
 *      |       Query Gruop 7  clone n dblist  |
 *      |--------------------------------------|
 *
 *
 *  History :
 *      Date            Modifier    Reason
 *  -----------------------------------------------------------------------
 *      04-30-2004        amytu     First Create
 * -------------------------------------------------------------------------------------
 * Copyright(C)        Accton Techonology Corporation, 2004
 * =====================================================================================*/
/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys_type.h"
#include "sys_cpnt.h"
#include "l_cvrt.h"
#include "l_dlist.h"
#include "l_pt.h"
#include "l_hash.h"

/* #define L_HASH_BACKDOOR */
#ifdef L_HASH_BACKDOOR
#include "backdoor_mgr.h"
#endif

/* LOCAL DATATYPE DECLARATION
 */
#define DEBUG_LEVEL_TRACE     0x01
#define DEBUG_LEVEL_FSM       0x02
#define DEBUG_LEVEL_ERROR     0x04
#define DEBUG_LEVEL_DEQUEUE   0x08


#define NULL_INDEX            ((L_HASH_Index_T)0xFFFF)
#define KEY_SEGMENT_BYTES     2
#define MAX_KEY_LENGTH        32
#define HASH_WK               ((L_HASH_WorkSpace_T *)(((L_HASH_Desc_T*)desc)->buffer))
#define HASH_RECORD(ridx)     ((UI32_T)(HASH_WK->data_rec_ptr) + (HASH_WK->hash_rec_len * (ridx)))

/* DATA TYPE DECLARATIONS
 */
typedef struct L_HASH_DataRecord_S
{
    L_HASH_Index_T previous_index;
    L_HASH_Index_T next_index;
    UI8_T          state;                /* NULL_ST,DIRTY_ST,PROCESS_ST,READY_ST,DEL_WAIT_SYNC_ST */
    UI8_T          action;               /* ADD or DELETE */
    UI8_T          wait_for_sync;        /* if value =0 => don't need to wait for sync ; if value =1 => wait for sync to hisam ; if value =2 => waif to sync to callback ; if value =3 => wait for sync to callback & hisam */
    UI8_T          reserved[1];           /* data alignment */
    UI8_T          data_rec[0];          /* USER DATA RECORD */
} L_HASH_DataRecord_T;

typedef struct
{
    /* the meaning of index in first_link_idx and last_link_idx
     * implies the input argument of the macro HASH_RECORD()
     */
    L_HASH_Index_T             first_link_idx; /* index of the first record in the linked list */
    L_HASH_Index_T             last_link_idx;  /* index of the last record in the linked list */
} L_HASH_Hash_Header_T;

typedef struct L_HASH_WorkSpace_S
{
    L_DLST_Indexed_Dblist_T     job_queue_desc;
    L_PT_Descriptor_T           data_record_desc;
    L_HASH_Hash_Header_T        *hash_header_ptr;
    UI8_T                       *data_rec_ptr;
    L_DLST_Indexed_Dblist_T     *query_group_ptr[L_HASH_MAX_QUERY_GROUP];
    UI16_T                      hash_rec_len;     /* user data_rec_size + sizeof(L_HASH_DataRecord_T)  */
    UI16_T                      total_key_len;
} L_HASH_WorkSpace_T;

enum
{
    L_HASH_NULL_ST = 0,
    L_HASH_DIRTY_ST,
    L_HASH_PROCESS_ST,
    L_HASH_READY_ST,
    L_HASH_DEL_WAIT_SYNC_ST,
    /* Internal State or Transition State
     */
    L_HASH_DIRTY_TST,
    L_HASH_DEL_WAIT_SYNC_TST,

};

typedef struct
{
    UI32_T  lport;
    UI32_T  vid_ifindex;
    UI32_T  key_index;
} L_HASH_TestData_T;


/* Hash State Machine event type.
 * Event type must start from L_HASH_LAST_EV as dfined in
 * L_HASH.h because some event type are exported
 */
enum
{
    L_HASH_SET_EV = L_HASH_LAST_EV,
    L_HASH_DEL_EV,
    L_HASH_DEQ_EV
};


/*---------------------------------
 * Types for shared memory version
 *---------------------------------
 */
typedef BOOL_T (*L_HASH_AcquireFunc_T)
            (UI32_T group, void *record, UI32_T *element);

typedef struct
{
    L_DLST_ShMem_Indexed_Dblist_T     job_queue_desc;
    L_PT_ShMem_Descriptor_T           data_record_desc;
    I32_T                       hash_header_offset;
    I32_T                       data_rec_offset;
    I32_T                       query_group_offset[L_HASH_MAX_QUERY_GROUP];
    UI16_T                      hash_rec_len;     /* user data_rec_size + sizeof(L_HASH_DataRecord_T)  */
    UI16_T                      total_key_len;
/* 
 *  L_HASH_Hash_Header_T        header[desc->nbr_of_hash_bucket];
 *  UI8_T                       data_rec_buffer[hash_rec_len * desc->nbr_of_rec];
 *  L_DLST_Indexed_Dblist_T     query_group[L_HASH_MAX_QUERY_GROUP];
 *  UI8_T                       job_queue_working_buffer[];
 *  UI8_T                       query_group_working_buffer[];
 */
} L_HASH_ShMem_WorkSpace_T;




/* MACRO FUNCTION DECLARATIONS
 */
/*---------------------------------
 * Macro for shared memory version
 *---------------------------------
 */
#define SHMEM_HASH_WK            ((L_HASH_ShMem_WorkSpace_T *)L_CVRT_GET_PTR((L_HASH_ShMem_Desc_T *)desc, ((L_HASH_ShMem_Desc_T *)desc)->buffer_offset))
#define SHMEM_HASH_HEADER_PTR    ((L_HASH_Hash_Header_T *)L_CVRT_GET_PTR(wk, wk->hash_header_offset))
#define SHMEM_DATA_RECORD_PTR    ((UI8_T *)L_CVRT_GET_PTR(wk, wk->data_rec_offset))
#define SHMEM_QUERY_GROUP_PTR(i) ((L_DLST_ShMem_Indexed_Dblist_T *)L_CVRT_GET_PTR(wk, wk->query_group_offset[(i)]))
#define SHMEM_HASH_RECORD(ridx)  ((L_HASH_DataRecord_T *)(SHMEM_DATA_RECORD_PTR + wk->hash_rec_len * (ridx)))


/* LOCAL SUBPROGRAM DECLARATIONS
 */
static UI32_T L_HASH_DeleteQueryGroup_Callback(L_HASH_Index_T index,UI32_T cookie);
static BOOL_T L_HASH_AllocRecord (L_HASH_Desc_T *desc, L_HASH_Index_T *idx);
static BOOL_T L_HASH_InsertHash(L_HASH_Desc_T *desc,  L_HASH_Index_T hash_bucket_idx, L_HASH_Index_T rec_idx);
static BOOL_T L_HASH_LocateRecord(L_HASH_Desc_T  *desc, UI8_T *key, L_HASH_Index_T *hash_index, UI8_T **hash_data, L_HASH_Index_T *table_idx);
static BOOL_T L_HASH_LocateAndRemoveRecord(L_HASH_Desc_T  *desc, UI8_T *key, L_HASH_Index_T *table_idx);
static BOOL_T L_HASH_InsertQueryGroup(L_HASH_Desc_T *desc, UI8_T *rec, L_HASH_Index_T idx);
static BOOL_T L_HASH_DeriveHashIndex(L_HASH_Desc_T *desc, UI8_T *key, L_HASH_Index_T *hash_index);
static BOOL_T L_HASH_FSM(L_HASH_Desc_T *desc, UI32_T  event_type, UI8_T *hash_data, UI32_T idx);
static BOOL_T L_HASH_SetHashRecord(L_HASH_Desc_T *desc, UI8_T *user_rec, UI8_T *key, UI32_T idx);
static BOOL_T L_HASH_DeleteRecordAction(L_HASH_Desc_T *desc, UI8_T *user_rec);
static BOOL_T L_HASH_NewRecord(L_HASH_Desc_T *desc, UI8_T *user_rec, UI8_T *key, L_HASH_Index_T hash_index, L_HASH_Index_T *idx);
static BOOL_T L_HASH_ReplaceRecord(L_HASH_Desc_T *desc, UI8_T *user_rec, UI8_T *key, UI32_T idx);
static BOOL_T L_HASH_DeriveRecordKey(L_HASH_Desc_T *desc, UI8_T *user_rec, UI8_T *key_buffer);
static BOOL_T L_HASH_UpdateHashRecordState(L_HASH_Desc_T *desc,UI32_T idx,UI8_T user_state);
static void   L_HASH_PrintRecord(L_HASH_DataRecord_T *hash_rec);

/*---------------------------------
 * Local functions for shared memory version
 *---------------------------------
 */
static UI32_T L_HASH_ShMem_DeleteQueryGroup_Callback(L_HASH_Index_T index,UI32_T cookie);
static BOOL_T L_HASH_ShMem_AllocRecord (L_HASH_ShMem_Desc_T *desc, L_HASH_Index_T *idx);
static BOOL_T L_HASH_ShMem_InsertHash(L_HASH_ShMem_Desc_T *desc,  L_HASH_Index_T hash_bucket_idx, L_HASH_Index_T rec_idx);
static BOOL_T L_HASH_ShMem_LocateRecord(L_HASH_ShMem_Desc_T  *desc, UI8_T *key, L_HASH_Index_T *hash_index, UI8_T **hash_data, L_HASH_Index_T *table_idx);
static BOOL_T L_HASH_ShMem_LocateAndRemoveRecord(L_HASH_ShMem_Desc_T  *desc, UI8_T *key, L_HASH_Index_T *table_idx);
static BOOL_T L_HASH_ShMem_InsertQueryGroup(L_HASH_ShMem_Desc_T *desc, UI8_T *rec, L_HASH_Index_T idx);
static BOOL_T L_HASH_ShMem_DeriveHashIndex(L_HASH_ShMem_Desc_T *desc, UI8_T *key, L_HASH_Index_T *hash_index);
static BOOL_T L_HASH_ShMem_FSM(L_HASH_ShMem_Desc_T *desc, UI32_T  event_type, UI8_T *hash_data, UI32_T idx);
static BOOL_T L_HASH_ShMem_SetHashRecord(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec, UI8_T *key, UI32_T idx);
static BOOL_T L_HASH_ShMem_DeleteRecordAction(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec);
static BOOL_T L_HASH_ShMem_NewRecord(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec, UI8_T *key, L_HASH_Index_T hash_index, L_HASH_Index_T *idx);
static BOOL_T L_HASH_ShMem_ReplaceRecord(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec, UI8_T *key, UI32_T idx);
static BOOL_T L_HASH_ShMem_DeriveRecordKey(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec, UI8_T *key_buffer);
static BOOL_T L_HASH_ShMem_UpdateHashRecordState(L_HASH_ShMem_Desc_T *desc,UI32_T idx,UI8_T user_state);

#ifdef L_HASH_BACKDOOR
static void   L_HASH_FSM_PRINT(L_HASH_DataRecord_T *hash_data, UI32_T event_type, UI8_T state);
static void   L_HASH_BackDoor_Menu(void);
static void   L_HASH_SetDebugFlag(UI32_T debug_flag);
static void   L_HASH_PrintDebugMode(void);
static BOOL_T L_HASH_BD_Create(void);
static void   L_HASH_BD_SetRecord(void);
static void   L_HASH_BD_DeleteRecord(void);
static void   L_HASH_BD_DeleteRecordAction(void);
static void   L_HASH_BD_DequeueRecord(void);
static BOOL_T L_HASH_BD_AcquireFunction1(UI32_T group, void *rec, UI32_T *element);
static void   L_HASH_BD_WorkspaceStatus(void);
static void   L_HASH_BD_SetOperationSuccess(void);
static void   L_HASH_BD_DelOperationSuccess(void);
static void   L_HASH_BD_OperationFail(void);
static void   L_HASH_BD_RequestData(void);
static void   L_HASH_BD_DeleteQueryGroup(void);
#endif

/*---------------------------------
 * External functions for shared memory version
 *---------------------------------
 */
#if (SYS_CPNT_SWDRVL3 == TRUE)
extern BOOL_T SWDRVL3_GetElementByGroupHostEntry(UI32_T group, void *record, UI32_T *element);
extern BOOL_T SWDRVL3_GetElementByGroupNetEntry(UI32_T group, void *record, UI32_T *element);
#endif

#if (SYS_CPNT_AMTRDRV == TRUE)
extern BOOL_T AMTRDRV_OM_GetElementByGroupEntry(UI32_T group, void *record, UI32_T *element);
#endif

/* STATIC VARIABLE DECLARATIONS
 */
#ifdef L_HASH_BACKDOOR
static L_HASH_Desc_T              test_desc;
static L_HASH_TestData_T          test_data;
#endif

static UI32_T                     l_hash_debug;

/*---------------------------------
 * Local variables for shared memory version
 *---------------------------------
 */
static L_HASH_AcquireFunc_T acquire_element_index_of_query_group_fun[] =
{
#if (SYS_CPNT_SWDRVL3 == TRUE)
    SWDRVL3_GetElementByGroupHostEntry, /* L_HASH_SHMEM_ACQUIRE_TYPE_SWDRVL3_HOSTROUTE */
    SWDRVL3_GetElementByGroupNetEntry,  /* L_HASH_SHMEM_ACQUIRE_TYPE_SWDRVL3_NETROUTE */
#endif
#if (SYS_CPNT_AMTRDRV == TRUE)
    AMTRDRV_OM_GetElementByGroupEntry, /* L_HASH_SHMEM_ACQUIRE_TYPE_AMTRDRV_BYGROUP */
#endif
};


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
void L_HASH_Init()
{
    return;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_Create_InterCSC_Relation
 * -------------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT  : None.
 * OUTPUT : None.
 * RETURN : None.
 * NOTES  : None.
 * -------------------------------------------------------------------------*/
void L_HASH_Create_InterCSC_Relation(void)
{
#ifdef L_HASH_BACKDOOR
    BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack("lhash",L_HASH_BackDoor_Menu);
#endif
    return;
}

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
 * OUTPUT:  desc  -- the handle of a new data storage (Database) after creation.
 * RETURN:  TRUE \ FALSE
 * NOTES:   desc->element_of_query_group[index_of_query_group] has to be 0 for the un-existed Query Groups.
 *          CSC can use memset(desc,0, sizeof(L_HASH_Desc_T)) first to set them to 0, then initialize fields of desc to its specific requirement.
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_Create (L_HASH_Desc_T  *desc)
{
    /* Local Variable Declaration
     */
    L_HASH_WorkSpace_T       *wk;
    L_DLST_Indexed_Dblist_T  *dblist,*dblist_buf;      /* dirty list */
    L_HASH_Hash_Header_T     *hp_p;                      /* hash header */
    UI32_T                   query_index;
    UI32_T                   wk_len, header_len, query_group_len,rec_table_len, total_len, hash_rec_len;
    UI32_T                   total_keylen=0;
    UI16_T                   hash_index, dblist_idx;

    /* BODY
     */
    /* Preliminary checks
     */
    if ((desc == NULL) || (desc->nbr_of_hash_bucket == 0) ||
        (desc->nbr_of_rec == 0) || (desc->record_size == 0) ||
        (desc->hash_method == 0) || (desc->key_offset == 0) ||
        (desc->key_size == 0) || (desc->element_of_query_group == 0))
    {
        return FALSE;
    }

    {
        UI32_T key_index;

        /* validate the total length of the keys
         */

        for(key_index=0; key_index<L_HASH_MAX_FIELD_NUMBER_OF_A_KEY; key_index++)
            total_keylen += desc->key_size[key_index];

        if(total_keylen>MAX_KEY_LENGTH)
            return FALSE;
    }

    l_hash_debug = 0;
    /* calculate allocating memory
     */
    wk_len =header_len = query_group_len=rec_table_len= total_len = 0;
    wk_len = L_ALIGN(sizeof(L_HASH_WorkSpace_T), 4);
    header_len = L_ALIGN(sizeof(L_HASH_Hash_Header_T) * desc->nbr_of_hash_bucket, 4);
    hash_rec_len = L_ALIGN( sizeof(L_HASH_DataRecord_T) + desc->record_size, 4 );
    rec_table_len = hash_rec_len * desc->nbr_of_rec;

    for (query_index = 0;
         (query_index < L_HASH_MAX_QUERY_GROUP) && (desc->element_of_query_group[query_index] != 0);
         query_index++)
    {
        query_group_len += sizeof(L_DLST_Indexed_Dblist_T) * desc->element_of_query_group[query_index];
    }

    total_len = wk_len + header_len + rec_table_len + query_group_len;

    /* Allocate and initialize working area */
    if ((wk = (L_HASH_WorkSpace_T *)malloc(total_len)) == 0 )
    {
        return FALSE;
    }

    memset(wk, 0, total_len);

    wk->hash_rec_len = (UI16_T)hash_rec_len;
    wk->total_key_len = total_keylen;

    /* Create JobLList Queue
     */
    if (L_DLST_Indexed_Dblist_Create(&wk->job_queue_desc, desc->nbr_of_rec) == FALSE)
    {
        printf("Create JobQueue Error\n");
        free ( (void*) wk);
        return FALSE;
    }

    /* Link Hash Header and init header ptr.
     */
    wk->hash_header_ptr = (L_HASH_Hash_Header_T *)((UI32_T)wk + wk_len);
    for (hp_p= (L_HASH_Hash_Header_T *)wk->hash_header_ptr, hash_index =0; hash_index < desc->nbr_of_hash_bucket; hash_index++, hp_p++)
    {
        hp_p->first_link_idx = NULL_INDEX;
        hp_p->last_link_idx = NULL_INDEX;
    } /* end of for */

    /* Create Data record Table
     */
    wk->data_record_desc.buffer =(char *)( (UI32_T)wk + wk_len + header_len);
    wk->data_record_desc.buffer_len = rec_table_len;
    wk->data_record_desc.partition_len = hash_rec_len;

    if (L_PT_Create(&wk->data_record_desc) == FALSE)
    {
        L_DLST_Indexed_Dblist_Destroy (&wk->job_queue_desc);
        free ( (void*) wk);
        return FALSE;
    }

    wk->data_rec_ptr = (UI8_T *) ((UI32_T)wk + wk_len + header_len);

    /* Init Query Group
     */
    {
        int i;
        dblist_buf = (L_DLST_Indexed_Dblist_T *) ((UI32_T)wk + wk_len + header_len + rec_table_len);

        for (dblist_idx=0, query_index = 0;
             (query_index < L_HASH_MAX_QUERY_GROUP) && (desc->element_of_query_group[query_index] != 0);
             query_index++)
        {
            dblist = &dblist_buf[dblist_idx];

            wk->query_group_ptr[query_index] = dblist;

            if (L_DLST_Indexed_Dblist_Create( dblist, desc->nbr_of_rec) == FALSE)
            {
                goto QUERY_GROUP_BYE;
            }
            /* clone dblist base on the number indicated in query group index */
            for ( i=1; i<desc->element_of_query_group[query_index]; i++)
            {
                if (L_DLST_Indexed_Dblist_Clone(dblist, dblist+i) == FALSE)
                {
                    goto QUERY_GROUP_BYE;
                }
            } /* end of for */
            dblist_idx += desc->element_of_query_group[query_index];
        } /* end of for */
    }
    desc->buffer = wk;
    return TRUE;

QUERY_GROUP_BYE:

    for (dblist_idx=0, query_index = 0;
         (query_index < L_HASH_MAX_QUERY_GROUP) && (desc->element_of_query_group[query_index] != 0);
         query_index++)
    {
        L_DLST_Indexed_Dblist_Destroy (&dblist_buf[dblist_idx]);
        dblist_idx += desc->element_of_query_group[query_index];
    }

    L_DLST_Indexed_Dblist_Destroy (&wk->job_queue_desc);
    free ( (void*) wk);
    return FALSE;
} /* end of L_HASH_Create() */

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
BOOL_T L_HASH_SetRecord (L_HASH_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index)
{
    /* Local Variable Declaration
     */
    L_HASH_DataRecord_T         *hash_data;
    L_HASH_Index_T              hash_index;
    BOOL_T                      ret;
    UI8_T                       key_buffer[MAX_KEY_LENGTH];


    /* BODY */
    if ((desc == NULL) || (user_rec == NULL))
    {
       return FALSE;
    }
    hash_index = *index = 0;

    /* 1.Search for Replacement
     * 2.If record does not exist, add a new one.
     * 3.Callback function to acquirement a specific element index to
     *   insert record to query group.
     * 4.Derive state and action base by the state machine
     * 5.Insert record to job queue
     */
    /* Locate Record to check for duplicated record information.
     * An existing record shall return hash_index, hash_data and idx
     * to indicate its position in the descriptor.
     */

    if (!L_HASH_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_DeriveRecordKey in set record fails\n");
        }
        return FALSE;
    }

    if (L_HASH_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, index))
    {
        ret = L_HASH_ReplaceRecord(desc, user_rec, key_buffer, *index);
    }
    else
    {
        ret = L_HASH_NewRecord(desc, user_rec, key_buffer, hash_index, index);
    }

    if (!ret)
    {
        return FALSE;
    }

    ret = L_HASH_FSM(desc, L_HASH_SET_EV, user_rec,*index);

    return ret;
} /* end of L_HASH_SetRecord () */


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
BOOL_T L_HASH_SetRecordToHashOnly(L_HASH_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index)
{
    L_HASH_DataRecord_T         *hash_data;
    L_HASH_Index_T              hash_index;
    UI8_T                       key_buffer[MAX_KEY_LENGTH];
    BOOL_T                      retval;
    hash_index  = 0;

    if(!L_HASH_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_DeriveRecordKey in set record fails\n");
        }
        return FALSE;
    }
    /* Check the record is exist or not ? If yes then update the record else add a new record */
    if(L_HASH_LocateRecord(desc,key_buffer, &hash_index, (UI8_T **)(void *)&hash_data,index))
    {
        retval = L_HASH_ReplaceRecord(desc, user_rec,key_buffer,*index);
    }
    else
    {
        retval = L_HASH_NewRecord(desc, user_rec, key_buffer, hash_index,index);
    }

    /* update record state to ready */
    if (retval)
    {
    	L_HASH_UpdateHashRecordState(desc,*index,L_HASH_READY_ST);
    }

    return retval;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_DeleteRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Delete a record from HASH database.
 * INPUT  : desc - A specific desciptor to process.
 *          rec  - A specific record to be removed.
 *          index - Deleted record is with this index
 * OUTPUT : index - hash record index.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_DeleteRecord (L_HASH_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index)
{
    L_DLST_Indexed_Dblist_T       *dblist;
    L_HASH_DataRecord_T           *hash_data;
    L_HASH_Index_T                hash_index;
    UI32_T                        i,element_index;
    UI8_T                         key_buffer[MAX_KEY_LENGTH];


    /* BODY
     */
    if ((desc == NULL) || (user_rec ==  NULL))
    {
        return FALSE;
    }

    hash_index = *index = 0;

    if (!L_HASH_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_DeriveRecordKey in delete record fails\n");
        }
        return FALSE;
    }

    if(L_HASH_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, index) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_LocateRecord in delete record fails\n");
        }
        return FALSE;
    }

    if (hash_data->action != L_HASH_ACTION_DEL)
    {
        /* modify record action to "Del" */
        hash_data->action = L_HASH_ACTION_DEL;

        /* Delete record from all Query Groups */
        for (i = 0; (desc->element_of_query_group[i]> 0) && (i<L_HASH_MAX_QUERY_GROUP); i++)
        {
            if (desc->acquire_element_index_of_query_group_fun == 0)
            {
                break;
            }
            desc->acquire_element_index_of_query_group_fun(i, user_rec, &element_index);
            dblist = (L_DLST_Indexed_Dblist_T *)((UI32_T )(HASH_WK->query_group_ptr[i])+(element_index*sizeof(L_DLST_Indexed_Dblist_T)));
            L_DLST_Indexed_Dblist_DeleteEntry(dblist,*index);

        }
    }

    L_HASH_FSM(desc, L_HASH_DEL_EV, user_rec, *index);

    return TRUE;
} /* end of L_HASH_DeleteRecord() */

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
BOOL_T L_HASH_DeleteRecordFromHashOnly (L_HASH_Desc_T *desc, UI8_T  *rec, L_HASH_Index_T *index)
{
    L_DLST_Indexed_Dblist_T     *dblist;
    UI32_T                      i,element_index;
    UI8_T                       key_buffer[MAX_KEY_LENGTH];


    /* BODY */
    if ((desc == NULL) || (rec == NULL))
    {
        return FALSE;
    }
    element_index = 0;

    if (!L_HASH_DeriveRecordKey(desc, rec, key_buffer))
    {
        return FALSE;
    }

    /* Remove record from Hash Link */
    if (L_HASH_LocateAndRemoveRecord(desc, key_buffer, index) == FALSE)
    {
        return FALSE;
    }

    /* Remove record from JobQueue if exist*/
    L_DLST_Indexed_Dblist_DeleteEntry(&HASH_WK->job_queue_desc, *index);

    /* Delete record from all Query Groups */
    for (i = 0; (desc->element_of_query_group[i]> 0) && (i<L_HASH_MAX_QUERY_GROUP); i++)
    {
        if (desc->acquire_element_index_of_query_group_fun == 0)
        {
            break;
        }
        desc->acquire_element_index_of_query_group_fun(i, rec, &element_index);
        dblist = (L_DLST_Indexed_Dblist_T *)((UI32_T )(HASH_WK->query_group_ptr[i])+(element_index*sizeof(L_DLST_Indexed_Dblist_T)));
        L_DLST_Indexed_Dblist_DeleteEntry(dblist, *index);

    }

    /* Reset record from record table. */
    L_PT_Free ( &HASH_WK->data_record_desc, (void *)HASH_RECORD(*index) );

    return TRUE;

}

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
BOOL_T L_HASH_DeleteRecordNWaitSync (L_HASH_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index)
{
    L_DLST_Indexed_Dblist_T       *dblist;
    L_HASH_DataRecord_T           *hash_data;
    L_HASH_Index_T                hash_index;
    UI32_T                        i,element_index;
    UI8_T                         key_buffer[MAX_KEY_LENGTH];

    /* BODY */
    if ((desc == NULL) || (user_rec ==  NULL))
    {
        return FALSE;
    }

    hash_index = *index = 0;

    if (!L_HASH_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_DeriveRecordKey in delete record fails\n");
        }
        return FALSE;
    }

    if(L_HASH_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, index) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_LocateRecord in delete record fails\n");
        }
        return FALSE;
    }

    /* Remove record from JobQueue if exist*/
    L_DLST_Indexed_Dblist_DeleteEntry(&HASH_WK->job_queue_desc, *index);

    if (hash_data->action != L_HASH_ACTION_DEL)
    {
        /* modify record action to "Del" */
        hash_data->action = L_HASH_ACTION_DEL;

        /* Delete record from all Query Groups */
        for (i = 0; (desc->element_of_query_group[i]> 0) && (i<L_HASH_MAX_QUERY_GROUP); i++)
        {
            if (desc->acquire_element_index_of_query_group_fun == 0)
            {
                break;
            }
            desc->acquire_element_index_of_query_group_fun(i, user_rec, &element_index);
            dblist = (L_DLST_Indexed_Dblist_T *)((UI32_T )(HASH_WK->query_group_ptr[i])+(element_index*sizeof(L_DLST_Indexed_Dblist_T)));
            L_DLST_Indexed_Dblist_DeleteEntry(dblist,*index);

        }

        /* update record state to ready */
        L_HASH_UpdateHashRecordState(desc,*index,L_HASH_DEL_WAIT_SYNC_ST);
    }

    return TRUE;
} /* end of L_HASH_DeleteRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_DeleteAll
 * -------------------------------------------------------------------------
 * PURPOSE: Delete all records from HASH database.
 * INPUT  : desc - A specific desciptor to be removed.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_DeleteAll (L_HASH_Desc_T  *desc)
{
    L_DLST_Indexed_Dblist_T        *dblist;
    L_HASH_WorkSpace_T             *wk;
    L_HASH_Hash_Header_T           *hh_p;
    UI32_T                         query_index = 0;
    UI32_T                         hash_bucket;

    /* BODY
     */
    if (desc == NULL)
        return FALSE;
    wk = desc->buffer;
    /* delete nodes from job queue */
    if (L_DLST_Indexed_Dblist_DeleteAll_ByListArray(&wk->job_queue_desc) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_DLST_DeleteAll fails for delete job queue \n");
        }
        return FALSE;
    }
    /* delete nodes from query group
     */
    for (query_index = 0;
         (query_index < L_HASH_MAX_QUERY_GROUP) && (desc->element_of_query_group[query_index] != 0);
         query_index++)
    {

        dblist = (L_DLST_Indexed_Dblist_T *)wk->query_group_ptr[query_index];

        if(L_DLST_Indexed_Dblist_DeleteAll_ByListArray(dblist) == FALSE)
        {
            return FALSE;
        }
    } /* end of for */

    /* delete records from hash table*/
    for (hash_bucket = 0; hash_bucket < desc->nbr_of_hash_bucket; hash_bucket++)
    {
        L_HASH_Index_T current_rec_idx, next_rec_idx;

        hh_p = wk->hash_header_ptr+hash_bucket;
        for ( current_rec_idx=hh_p->first_link_idx;
              current_rec_idx!=NULL_INDEX;
              current_rec_idx=next_rec_idx)
        {
            next_rec_idx = ((L_HASH_DataRecord_T*)HASH_RECORD(current_rec_idx))->next_index;
            L_PT_Free(&wk->data_record_desc ,(void *)HASH_RECORD(current_rec_idx));
        }

        hh_p->first_link_idx = NULL_INDEX;
        hh_p->last_link_idx = NULL_INDEX;
    }
    return TRUE;
} /* end of L_HASH_DeleteAll() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_Destroy
 * -------------------------------------------------------------------------
 * PURPOSE: Destroy the existing Hash database.
 * INPUT  : desc - A specific desciptor to process.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_Destroy (L_HASH_Desc_T  *desc)
{
    if(desc == NULL)
    {
        return FALSE;
    }
    free(desc->buffer);

    return TRUE;
} /* end of L_HASH_Destroy() */

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
 *         You can add a call_Back function to do action for some specified records.
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_SearchByQueryGroup(L_HASH_Desc_T  *desc, UI32_T query_group,
                                 UI32_T query_element,UI32_T cookie,
                                 UI32_T (*call_back) (UI16_T index, UI32_T cookie))
{
    L_DLST_Indexed_Dblist_T    *dblist;
    UI32_T                     ret;

    if(desc == NULL)
    {
        return FALSE;
    }
    if((desc->element_of_query_group[query_group] == 0) || (query_element > desc->element_of_query_group[query_group]))
    {
        return FALSE;
    }
    dblist = (L_DLST_Indexed_Dblist_T *)((UI32_T )(HASH_WK->query_group_ptr[query_group])+(query_element*sizeof(L_DLST_Indexed_Dblist_T)));
    ret = L_DLST_Indexed_Dblist_Search(dblist, cookie,*call_back);
    if(ret == L_DLST_SEARCH_END_OF_LIST)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
} /* end of L_HASH_SearchByQueryGroup() */

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
BOOL_T L_HASH_SearchByQueryGroupAndDelete(L_HASH_Desc_T  *desc, UI32_T query_group, UI32_T query_element)
{
    UI32_T cookie;

    /* we need to pass desc to callback function so we have to set cookie as desc*/
    cookie =(UI32_T) desc;
    return L_HASH_SearchByQueryGroup(desc,query_group,query_element,(UI32_T)desc,L_HASH_DeleteQueryGroup_Callback);
}

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
                                 UI32_T query_element,L_DLST_Indexed_Dblist_T **dblist)
{
    if(desc == NULL)
    {
        return FALSE;
    }
    if((desc->element_of_query_group[query_group] == 0) || (query_element > desc->element_of_query_group[query_group]))
    {
        return FALSE;
    }

    *dblist = (L_DLST_Indexed_Dblist_T *)((UI32_T )(HASH_WK->query_group_ptr[query_group])+(query_element*sizeof(L_DLST_Indexed_Dblist_T)));

    return TRUE;
}

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
BOOL_T L_HASH_DequeueJobList(L_HASH_Desc_T  *desc,UI8_T **rec, UI8_T *action, L_HASH_Index_T *idx)
{
    L_HASH_DataRecord_T           *p;
    BOOL_T                        ret = FALSE;

    if((desc == NULL) || (rec == NULL) || (action == NULL))
    {
        return FALSE;
    }
    if(L_DLST_Indexed_Dblist_Dequeue(&HASH_WK->job_queue_desc, idx) == FALSE)
    {
        return FALSE;
    }
    p = (L_HASH_DataRecord_T *)HASH_RECORD(*idx);
    *rec = (UI8_T *)(p+1);
    *action = p->action;

    ret = L_HASH_FSM(desc, L_HASH_DEQ_EV, (UI8_T *)rec, *idx);
    if(l_hash_debug & DEBUG_LEVEL_DEQUEUE)
    {
        printf("L_HASH_DequeueJobList dequeues record \n");
        L_HASH_PrintRecord(p);
    }
    return ret;
} /* end of L_HASH_DequeueJobList() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_OperationResult
 * -------------------------------------------------------------------------
 * PURPOSE: Result of the job dequeue from JobList.
 * INPUT  : desc  - A specific desciptor to process.
 *          event - L_HASH_SUCCESS_EV \ L_HASH_FAIL_EV
                    L_SYNC_OK_EV \ L_HASH_SYNC_FAIL_EV
 *          rec   - Specific record
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_OperationResult (L_HASH_Desc_T  *desc, UI32_T event, UI8_T *user_rec)
{
    L_HASH_DataRecord_T          *hash_data;
    L_HASH_Index_T               hash_index,idx;
    UI8_T                        key_buffer[MAX_KEY_LENGTH];

    /* BODY */
    if ((desc == NULL) || (user_rec == NULL))
    {
        return FALSE;
    }
    hash_index = idx = 0;

    if((event != L_HASH_SET_SUCCESS_EV) && (event != L_HASH_DEL_SUCCESS_EV) && (event != L_HASH_SYNC_DONE_EV) && (event != L_HASH_FAIL_EV))
    {
        return FALSE;
    }

    if(!L_HASH_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        return FALSE;
    }
    if(L_HASH_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, &idx) == FALSE)
    {
        return FALSE;
    }
    L_HASH_FSM(desc, event, user_rec, idx);

    if (l_hash_debug & DEBUG_LEVEL_DEQUEUE)
    {
        printf("L_HASH_OperationResult sets record \n");
        L_HASH_PrintRecord(hash_data);
    }
    return TRUE;
} /* end of L_HASH_OperationResult() */

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
BOOL_T L_HASH_GetExactRecord(L_HASH_Desc_T *desc, UI8_T  *user_rec)
{
    L_HASH_DataRecord_T         *hash_data;
    L_HASH_Index_T              hash_index,idx;
    UI8_T                       key_buffer[MAX_KEY_LENGTH];

    if ((desc == NULL) || (user_rec == NULL))
    {
        return FALSE;
    }
    hash_index = idx = 0;

    if (!L_HASH_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if(l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("AMTRDRV_HASH_DeriveRecordKey in set record fails\n");
        }
        return FALSE;
    }
    /* If the record is found & its action == ADD we will return TRUE to caller
     * Since its action == Delete may only stay in OM for temperary
     */
    if((L_HASH_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, &idx))
        &&(hash_data->action != L_HASH_ACTION_DEL))
    {
        memcpy(user_rec,hash_data->data_rec,desc->record_size);
        return TRUE;
    }
    /* record isn't found */
    else
    {
        return FALSE;
    }

}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetRecordByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get a Record by giving index
 * INPUT  : desc        - A specific desciptor to process.
 *          index       - record index
 * OUTPUT : user_record - the address record
 *          action      - record action
 * RETURN : None
 * NOTES  : We assume the index is legally to access the exist record we don't do any
 *          error handler.
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_GetRecordByIndex(L_HASH_Desc_T  *desc, UI32_T index , UI8_T *user_rec,UI8_T *action)
{
    L_HASH_DataRecord_T     *data;

    data = (L_HASH_DataRecord_T *)HASH_RECORD(index);
    *action = data->action;
    memcpy(user_rec,data->data_rec,desc->record_size);
    return;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetNRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Display N record at a time
 * INPUT  : desc         - A specific desciptor to process.
 *          record_count - The number of record to display
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
void L_HASH_GetNRecord(L_HASH_Desc_T  *desc, UI32_T record_count)
{
    UI32_T                      i = 0;
    L_HASH_DataRecord_T         *hash_rec;
    UI32_T                      hash_rec_len;

    hash_rec_len = L_ALIGN( sizeof(L_HASH_DataRecord_T) + desc->record_size, 4 );

    for (;i<record_count; i++)
    {
        hash_rec = (L_HASH_DataRecord_T *)((UI32_T)HASH_WK->data_rec_ptr + (i*hash_rec_len));
        L_HASH_PrintRecord(hash_rec);
    }

    return;
} /* end of L_HASH_GetNRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetRecordPtrByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get the Record Pointer by index
 * INPUT  : desc      - A specific desciptor to process.
 *          index     -
 *          user_rec  - the address entry
 * OUTPUT : None
 * RETURN : None
 * NOTES  : We assume the index is legally to access the exist record we don't do any
 *          error handler.
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_GetRecordPtrByIndex(L_HASH_Desc_T  *desc, UI8_T **user_rec, UI32_T index )
{
    L_HASH_DataRecord_T    *data;

    data  = (L_HASH_DataRecord_T *)HASH_RECORD(index);
    *user_rec = data->data_rec;
    return;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetRecordIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get exact record's index in database by giving reocrd
 * INPUT  : desc         - A specific desciptor to process.
 *          user_rec     - the address entry need to be compared.
 * OUTPUT : record_index - the record's index in OM
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_GetRecordIndex(L_HASH_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *record_index)
{
    L_HASH_DataRecord_T             *hash_data;
    L_HASH_Index_T                  hash_index;
    UI8_T                           key_buffer[MAX_KEY_LENGTH];

    if((desc == NULL) || (user_rec == NULL))
    {
        return FALSE;
    }

    hash_index = *record_index = 0;

    if (!L_HASH_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_DeriveRecordKey in set record fails\n");
        }
        return FALSE;
    }

    if (L_HASH_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, record_index))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_GetRecordSyncValueByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get a Record sync value by index
 * INPUT  : desc           - A specific desciptor to process.
 *          index          -
 *          sync_value     - to determine it still wait for sync to hisam or callback
 * OUTPUT : None
 * RETURN : None
 * NOTES  : We assume the index is legally to access the exist record we don't do any
 *          error handler.
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_GetRecordSyncValueByIndex(L_HASH_Desc_T  *desc, UI8_T *sync_value, UI32_T index )
{
    L_HASH_DataRecord_T     *data;

    data = (L_HASH_DataRecord_T *)HASH_RECORD(index);
    *sync_value = data->wait_for_sync;
    return;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_SetRecordSyncValueByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Set a Record sync value by index
 * INPUT  : desc           - A specific desciptor to process.
 *          index          -
 *          sync_value     - to determine it still wait for sync to hisam or callback
 * OUTPUT : None
 * RETURN : None
 * NOTES  : We assume the index is legally to access the exist record we don't do any
 *          error handler.
 *         (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_SetRecordSyncValueByIndex(L_HASH_Desc_T  *desc, UI8_T sync_value, UI32_T index )
{
    L_HASH_DataRecord_T     *data;

    data = (L_HASH_DataRecord_T *)HASH_RECORD(index);
    data->wait_for_sync = sync_value;
    return;
}


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
UI32_T L_HASH_ShMem_GetWorkingBufferRequiredSize(L_HASH_ShMem_Desc_T  *desc)
{
    UI32_T                   query_index;
    UI32_T                   wk_len, header_len, query_group_len,rec_table_len, total_len, hash_rec_len, job_queue_working_buffer_len, query_group_working_buffer_len;

/* buffer
 *  L_HASH_ShMem_WorkSpace_T    wk;
 *  L_HASH_Hash_Header_T        hash_header_ptr[nbr_of_hash_bucket];
 *  UI8_T                       rec_table_buffer[rec_table_len];
 *  L_DLST_ShMem_Indexed_Dblist_T query_group_dblist_list[];
 *  UI8_T                       job_queue_working_buffer[job_queue_working_buffer_len];
 *  UI8_T                       query_group_working_buffer[query_group_working_buffer_len];
 */

    /* calculate allocating memory
     */
    wk_len =header_len = query_group_len = rec_table_len = job_queue_working_buffer_len = query_group_working_buffer_len = total_len = 0;
    wk_len = L_ALIGN(sizeof(L_HASH_ShMem_WorkSpace_T), 4);
    header_len = L_ALIGN(sizeof(L_HASH_Hash_Header_T) * desc->nbr_of_hash_bucket, 4);
    hash_rec_len = L_ALIGN( sizeof(L_HASH_DataRecord_T) + desc->record_size, 4 );
    rec_table_len = hash_rec_len * desc->nbr_of_rec;
    job_queue_working_buffer_len = L_DLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(desc->nbr_of_rec);


    for (query_index = 0;
         (query_index < L_HASH_MAX_QUERY_GROUP) && (desc->element_of_query_group[query_index] != 0);
         query_index++)
    {
        query_group_len += sizeof(L_DLST_ShMem_Indexed_Dblist_T) * desc->element_of_query_group[query_index];
        query_group_working_buffer_len += L_DLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(desc->nbr_of_rec);
    }

    total_len = wk_len + header_len + rec_table_len + query_group_len + job_queue_working_buffer_len + query_group_working_buffer_len;
    return total_len;
}

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
 *          buffer_offset
 * OUTPUT:  desc  -- the handle of a new data storage (Database) after creation.
 * RETURN:  TRUE \ FALSE
 * NOTES:   desc->element_of_query_group[index_of_query_group] has to be 0 for the un-existed Query Groups.
 *          CSC can use memset(desc,0, sizeof(L_HASH_ShMem_Desc_T)) first to set them to 0, then initialize fields of desc to its specific requirement.
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_Create (L_HASH_ShMem_Desc_T  *desc)
{
    /* Local Variable Declaration
     */
    L_HASH_ShMem_WorkSpace_T       *wk;
    L_DLST_ShMem_Indexed_Dblist_T  *dblist;              /* dirty list */
    L_HASH_Hash_Header_T     *hp_p;                      /* hash header */
    UI32_T                   query_index;
    UI32_T                   wk_len, header_len, query_group_len,rec_table_len, total_len, hash_rec_len, job_queue_working_buffer_len, query_group_working_buffer_len;
    UI32_T                   total_keylen=0;
    UI16_T                   hash_index;
    UI8_T                    *buffer;

    /* BODY
     */
    /* Preliminary checks
     */
    if ((desc == NULL) || (desc->nbr_of_hash_bucket == 0) ||
        (desc->nbr_of_rec == 0) || (desc->record_size == 0) ||
        (desc->hash_method == 0) || (desc->key_offset == 0) ||
        (desc->key_size == 0) || (desc->element_of_query_group == 0))
    {
        return FALSE;
    }

    {
        UI32_T key_index;

        /* validate the total length of the keys
         */

        for(key_index=0; key_index<L_HASH_MAX_FIELD_NUMBER_OF_A_KEY; key_index++)
            total_keylen += desc->key_size[key_index];

        if(total_keylen>MAX_KEY_LENGTH)
            return FALSE;
    }

    l_hash_debug = 0;

    /* calculate allocating memory
     */
    wk_len =header_len = query_group_len = rec_table_len = job_queue_working_buffer_len = query_group_working_buffer_len = total_len = 0;
    wk_len = L_ALIGN(sizeof(L_HASH_ShMem_WorkSpace_T), 4);
    header_len = L_ALIGN(sizeof(L_HASH_Hash_Header_T) * desc->nbr_of_hash_bucket, 4);
    hash_rec_len = L_ALIGN( sizeof(L_HASH_DataRecord_T) + desc->record_size, 4 );
    rec_table_len = hash_rec_len * desc->nbr_of_rec;
    job_queue_working_buffer_len = L_DLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(desc->nbr_of_rec);

    for (query_index = 0;
         (query_index < L_HASH_MAX_QUERY_GROUP) && (desc->element_of_query_group[query_index] != 0);
         query_index++)
    {
        query_group_len += sizeof(L_DLST_ShMem_Indexed_Dblist_T) * desc->element_of_query_group[query_index];
        query_group_working_buffer_len += L_DLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(desc->nbr_of_rec);
    }

    total_len = wk_len + header_len + rec_table_len + query_group_len + job_queue_working_buffer_len + query_group_working_buffer_len;

    /* Allocate and initialize working area */
    wk = SHMEM_HASH_WK;

    memset(wk, 0, total_len);

    wk->hash_rec_len = (UI16_T)hash_rec_len;
    wk->total_key_len = total_keylen;

    /* Create JobLList Queue
     */
    buffer = (UI8_T *)wk + wk_len + header_len + rec_table_len + query_group_len;
    if (L_DLST_ShMem_Indexed_Dblist_Create(&wk->job_queue_desc, buffer, desc->nbr_of_rec) == FALSE)
    {
        printf("Create JobQueue Error\n");
        return FALSE;
    }

    /* Link Hash Header and init header ptr.
     */
    wk->hash_header_offset = wk_len;
    for (hp_p= SHMEM_HASH_HEADER_PTR, hash_index =0; hash_index < desc->nbr_of_hash_bucket; hash_index++, hp_p++)
    {
        hp_p->first_link_idx = NULL_INDEX;
        hp_p->last_link_idx = NULL_INDEX;
    } /* end of for */

    /* Create Data record Table
     */
    wk->data_rec_offset = wk_len + header_len;

    wk->data_record_desc.buffer_offset = L_CVRT_GET_OFFSET(&wk->data_record_desc, L_CVRT_GET_PTR(wk, wk->data_rec_offset));
    wk->data_record_desc.buffer_len = rec_table_len;
    wk->data_record_desc.partition_len = hash_rec_len;

    if (L_PT_ShMem_Create(&wk->data_record_desc) == FALSE)
    {
        L_DLST_ShMem_Indexed_Dblist_Destroy (&wk->job_queue_desc);
        return FALSE;
    }

    /* Init Query Group
     */
    {
        int i;
        UI32_T dblist_offset = wk_len + header_len + rec_table_len;
        UI8_T  *query_group_working_buffer = buffer + job_queue_working_buffer_len;

        for (query_index = 0;
             (query_index < L_HASH_MAX_QUERY_GROUP) && (desc->element_of_query_group[query_index] != 0);
             query_index++)
        {
            wk->query_group_offset[query_index] = dblist_offset;
            dblist = SHMEM_QUERY_GROUP_PTR(query_index);
			
            if (L_DLST_ShMem_Indexed_Dblist_Create(dblist, query_group_working_buffer, desc->nbr_of_rec) == FALSE)
            {
                goto QUERY_GROUP_BYE;
            }
            /* clone dblist base on the number indicated in query group index */
            for ( i=1; i<desc->element_of_query_group[query_index]; i++)
            {
                if (L_DLST_ShMem_Indexed_Dblist_Clone(dblist, dblist+i) == FALSE)
                {
                    goto QUERY_GROUP_BYE;
                }
            } /* end of for */
            dblist_offset += sizeof(L_DLST_ShMem_Indexed_Dblist_T) * desc->element_of_query_group[query_index];
            query_group_working_buffer += L_DLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(desc->nbr_of_rec);
        } /* end of for */
    }
    return TRUE;

QUERY_GROUP_BYE:

    for (query_index = 0;
         (query_index < L_HASH_MAX_QUERY_GROUP) && (desc->element_of_query_group[query_index] != 0);
         query_index++)
    {
        L_DLST_ShMem_Indexed_Dblist_Destroy (SHMEM_QUERY_GROUP_PTR(query_index));
    }

    L_DLST_ShMem_Indexed_Dblist_Destroy (&wk->job_queue_desc);
    return FALSE;
} /* end of L_HASH_ShMem_Create() */

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
BOOL_T L_HASH_ShMem_SetRecord (L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index)
{
    /* Local Variable Declaration
     */
    L_HASH_DataRecord_T         *hash_data;
    L_HASH_Index_T              hash_index;
    BOOL_T                      ret;
    UI8_T                       key_buffer[MAX_KEY_LENGTH];


    /* BODY */
    if ((desc == NULL) || (user_rec == NULL))
    {
       return FALSE;
    }
    hash_index = *index = 0;

    /* 1.Search for Replacement
     * 2.If record does not exist, add a new one.
     * 3.Callback function to acquirement a specific element index to
     *   insert record to query group.
     * 4.Derive state and action base by the state machine
     * 5.Insert record to job queue
     */
    /* Locate Record to check for duplicated record information.
     * An existing record shall return hash_index, hash_data and idx
     * to indicate its position in the descriptor.
     */

    if (!L_HASH_ShMem_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_DeriveRecordKey in set record fails\n");
        }
        return FALSE;
    }

    if (L_HASH_ShMem_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, index))
    {
        ret = L_HASH_ShMem_ReplaceRecord(desc, user_rec, key_buffer, *index);
    }
    else
    {
        ret = L_HASH_ShMem_NewRecord(desc, user_rec, key_buffer, hash_index, index);
    }

    if (!ret)
    {
        return FALSE;
    }

    ret = L_HASH_ShMem_FSM(desc, L_HASH_SET_EV, user_rec,*index);

    return ret;
} /* end of L_HASH_ShMem_SetRecord () */


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
BOOL_T L_HASH_ShMem_SetRecordToHashOnly(L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index)
{
    L_HASH_DataRecord_T         *hash_data;
    L_HASH_Index_T              hash_index;
    UI8_T                       key_buffer[MAX_KEY_LENGTH];
    BOOL_T                      retval;
    hash_index  = 0;

    if(!L_HASH_ShMem_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_DeriveRecordKey in set record fails\n");
        }
        return FALSE;
    }
    /* Check the record is exist or not ? If yes then update the record else add a new record */
    if(L_HASH_ShMem_LocateRecord(desc,key_buffer, &hash_index, (UI8_T **)(void *)&hash_data,index))
    {
        retval = L_HASH_ShMem_ReplaceRecord(desc, user_rec,key_buffer,*index);
    }
    else
    {
        retval = L_HASH_ShMem_NewRecord(desc, user_rec, key_buffer, hash_index,index);
    }

    /* update record state to ready */
    if (retval)
    {
    	L_HASH_ShMem_UpdateHashRecordState(desc,*index,L_HASH_READY_ST);
    }

    return retval;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_DeleteRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Delete a record from HASH database.
 * INPUT  : desc - A specific desciptor to process.
 *          rec  - A specific record to be removed.
 *          index - Deleted record is with this index
 * OUTPUT : index - hash record index.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_DeleteRecord (L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index)
{
    L_HASH_ShMem_WorkSpace_T            *wk = SHMEM_HASH_WK;
    L_DLST_ShMem_Indexed_Dblist_T       *dblist;
    L_HASH_DataRecord_T           *hash_data;
    L_HASH_Index_T                hash_index;
    UI32_T                        i,element_index;
    UI8_T                         key_buffer[MAX_KEY_LENGTH];


    /* BODY
     */
    if ((desc == NULL) || (user_rec ==  NULL))
    {
        return FALSE;
    }

    hash_index = *index = 0;

    if (!L_HASH_ShMem_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_DeriveRecordKey in delete record fails\n");
        }
        return FALSE;
    }

    if(L_HASH_ShMem_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, index) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_LocateRecord in delete record fails\n");
        }
        return FALSE;
    }

    if (hash_data->action != L_HASH_ACTION_DEL)
    {
        /* modify record action to "Del" */
        hash_data->action = L_HASH_ACTION_DEL;

        /* Delete record from all Query Groups */
        for (i = 0; (desc->element_of_query_group[i]> 0) && (i<L_HASH_MAX_QUERY_GROUP); i++)
        {
            if (acquire_element_index_of_query_group_fun[desc->acquire_element_index_of_query_group_fun_id] == 0)
            {
                break;
            }
            acquire_element_index_of_query_group_fun[desc->acquire_element_index_of_query_group_fun_id](i, user_rec, &element_index);
            dblist = SHMEM_QUERY_GROUP_PTR(i) + element_index;
            L_DLST_ShMem_Indexed_Dblist_DeleteEntry(dblist,*index);

        }
    }

    L_HASH_ShMem_FSM(desc, L_HASH_DEL_EV, user_rec, *index);

    return TRUE;
} /* end of L_HASH_ShMem_DeleteRecord() */

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
BOOL_T L_HASH_ShMem_DeleteRecordFromHashOnly (L_HASH_ShMem_Desc_T *desc, UI8_T  *rec, L_HASH_Index_T *index)
{
    L_HASH_ShMem_WorkSpace_T          *wk = SHMEM_HASH_WK;
    L_DLST_ShMem_Indexed_Dblist_T     *dblist;
    UI32_T                      i,element_index;
    UI8_T                       key_buffer[MAX_KEY_LENGTH];


    /* BODY */
    if ((desc == NULL) || (rec == NULL))
    {
        return FALSE;
    }
    element_index = 0;

    if (!L_HASH_ShMem_DeriveRecordKey(desc, rec, key_buffer))
    {
        return FALSE;
    }

    /* Remove record from Hash Link */
    if (L_HASH_ShMem_LocateAndRemoveRecord(desc, key_buffer, index) == FALSE)
    {
        return FALSE;
    }

    /* Remove record from JobQueue if exist*/
    L_DLST_ShMem_Indexed_Dblist_DeleteEntry(&wk->job_queue_desc, *index);

    /* Delete record from all Query Groups */
    for (i = 0; (desc->element_of_query_group[i]> 0) && (i<L_HASH_MAX_QUERY_GROUP); i++)
    {
        if (acquire_element_index_of_query_group_fun[desc->acquire_element_index_of_query_group_fun_id] == 0)
        {
            break;
        }
        acquire_element_index_of_query_group_fun[desc->acquire_element_index_of_query_group_fun_id](i, rec, &element_index);
        dblist = SHMEM_QUERY_GROUP_PTR(i)+element_index;
        L_DLST_ShMem_Indexed_Dblist_DeleteEntry(dblist, *index);

    }

    /* Reset record from record table. */
    L_PT_ShMem_Free ( &wk->data_record_desc, SHMEM_HASH_RECORD(*index) );

    return TRUE;

}

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
BOOL_T L_HASH_ShMem_DeleteRecordNWaitSync (L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *index)
{
    L_HASH_ShMem_WorkSpace_T            *wk = SHMEM_HASH_WK;
    L_DLST_ShMem_Indexed_Dblist_T       *dblist;
    L_HASH_DataRecord_T           *hash_data;
    L_HASH_Index_T                hash_index;
    UI32_T                        i,element_index;
    UI8_T                         key_buffer[MAX_KEY_LENGTH];

    /* BODY */
    if ((desc == NULL) || (user_rec ==  NULL))
    {
        return FALSE;
    }

    hash_index = *index = 0;

    if (!L_HASH_ShMem_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_DeriveRecordKey in delete record fails\n");
        }
        return FALSE;
    }

    if(L_HASH_ShMem_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, index) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_LocateRecord in delete record fails\n");
        }
        return FALSE;
    }

    /* Remove record from JobQueue if exist*/
    L_DLST_ShMem_Indexed_Dblist_DeleteEntry(&wk->job_queue_desc, *index);

    if (hash_data->action != L_HASH_ACTION_DEL)
    {
        /* modify record action to "Del" */
        hash_data->action = L_HASH_ACTION_DEL;

        /* Delete record from all Query Groups */
        for (i = 0; (desc->element_of_query_group[i]> 0) && (i<L_HASH_MAX_QUERY_GROUP); i++)
        {
            if (acquire_element_index_of_query_group_fun[desc->acquire_element_index_of_query_group_fun_id] == 0)
            {
                break;
            }
            acquire_element_index_of_query_group_fun[desc->acquire_element_index_of_query_group_fun_id](i, user_rec, &element_index);
            dblist = SHMEM_QUERY_GROUP_PTR(i)+element_index;
            L_DLST_ShMem_Indexed_Dblist_DeleteEntry(dblist,*index);

        }

        /* update record state to ready */
        L_HASH_ShMem_UpdateHashRecordState(desc,*index,L_HASH_DEL_WAIT_SYNC_ST);
    }

    return TRUE;
} /* end of L_HASH_ShMem_DeleteRecordNWaitSync() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_DeleteAll
 * -------------------------------------------------------------------------
 * PURPOSE: Delete all records from HASH database.
 * INPUT  : desc - A specific desciptor to be removed.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_DeleteAll (L_HASH_ShMem_Desc_T  *desc)
{
    L_HASH_ShMem_WorkSpace_T            *wk = SHMEM_HASH_WK;
    L_DLST_ShMem_Indexed_Dblist_T       *dblist;
    L_HASH_Hash_Header_T           *hh_p;
    UI32_T                         query_index = 0;
    UI32_T                         hash_bucket;

    /* BODY
     */
    if (desc == NULL)
        return FALSE;

    /* delete nodes from job queue */
    if (L_DLST_ShMem_Indexed_Dblist_DeleteAll_ByListArray(&wk->job_queue_desc) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_DLST_DeleteAll fails for delete job queue \n");
        }
        return FALSE;
    }

    /* delete nodes from query group
     */
    for (query_index = 0;
         (query_index < L_HASH_MAX_QUERY_GROUP) && (desc->element_of_query_group[query_index] != 0);
         query_index++)
    {
        dblist = SHMEM_QUERY_GROUP_PTR(query_index);

        if(L_DLST_ShMem_Indexed_Dblist_DeleteAll_ByListArray(dblist) == FALSE)
        {
            return FALSE;
        }
    } /* end of for */
		 
    /* delete records from hash table*/
    for (hash_bucket = 0; hash_bucket < desc->nbr_of_hash_bucket; hash_bucket++)
    {
        L_HASH_Index_T current_rec_idx, next_rec_idx;

        hh_p = SHMEM_HASH_HEADER_PTR+hash_bucket;
        for ( current_rec_idx=hh_p->first_link_idx;
              current_rec_idx!=NULL_INDEX;
              current_rec_idx=next_rec_idx)
        {
            next_rec_idx = SHMEM_HASH_RECORD(current_rec_idx)->next_index;
            L_PT_ShMem_Free(&wk->data_record_desc ,SHMEM_HASH_RECORD(current_rec_idx));
        }

        hh_p->first_link_idx = NULL_INDEX;
        hh_p->last_link_idx = NULL_INDEX;
    }
	
    return TRUE;
} /* end of L_HASH_ShMem_DeleteAll() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_Destroy
 * -------------------------------------------------------------------------
 * PURPOSE: Destroy the existing Hash database.
 * INPUT  : desc - A specific desciptor to process.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_Destroy (L_HASH_ShMem_Desc_T  *desc)
{
    if(desc == NULL)
    {
        return FALSE;
    }

    return TRUE;
} /* end of L_HASH_ShMem_Destroy() */

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
 *         You can add a call_Back function to do action for some specified records.
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_SearchByQueryGroup(L_HASH_ShMem_Desc_T  *desc, UI32_T query_group,
                                 UI32_T query_element,UI32_T cookie,
                                 UI32_T (*call_back) (UI16_T index, UI32_T cookie))
{
    L_HASH_ShMem_WorkSpace_T         *wk = SHMEM_HASH_WK;
    L_DLST_ShMem_Indexed_Dblist_T    *dblist;
    UI32_T                     ret;

    if(desc == NULL)
    {
        return FALSE;
    }
    if((desc->element_of_query_group[query_group] == 0) || (query_element > desc->element_of_query_group[query_group]))
    {
        return FALSE;
    }
    dblist = SHMEM_QUERY_GROUP_PTR(query_group)+ query_element;
    ret = L_DLST_ShMem_Indexed_Dblist_Search(dblist, cookie,call_back);
    if(ret == L_DLST_SEARCH_END_OF_LIST)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
} /* end of L_HASH_ShMem_SearchByQueryGroup() */

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
BOOL_T L_HASH_ShMem_SearchByQueryGroupAndDelete(L_HASH_ShMem_Desc_T  *desc, UI32_T query_group, UI32_T query_element)
{
    UI32_T cookie;

    /* we need to pass desc to callback function so we have to set cookie as desc*/
    cookie =(UI32_T) desc;
    return L_HASH_ShMem_SearchByQueryGroup(desc,query_group,query_element,cookie,L_HASH_ShMem_DeleteQueryGroup_Callback);
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetQueryGroupDescriptor
 * -------------------------------------------------------------------------
 * PURPOSE: Get query group eleement's descriptor
 * INPUT  : desc          - A specific desciptor to process.
 *          query_group   - A specific query category to delete.
 *          query_element - Index of the specific category to operate.
 * OUTPUT : L_DLST_ShMem_Indexed_Dblist_T dblist -- the specific query group's descriptor
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *         (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_GetQueryGroupDescriptor(L_HASH_ShMem_Desc_T  *desc, UI32_T query_group,
                                 UI32_T query_element,L_DLST_ShMem_Indexed_Dblist_T **dblist)
{
    L_HASH_ShMem_WorkSpace_T         *wk = SHMEM_HASH_WK;

    if(desc == NULL)
    {
        return FALSE;
    }
    if((desc->element_of_query_group[query_group] == 0) || (query_element > desc->element_of_query_group[query_group]))
    {
        return FALSE;
    }

    *dblist = SHMEM_QUERY_GROUP_PTR(query_group)+query_element;

    return TRUE;
}

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
BOOL_T L_HASH_ShMem_DequeueJobList(L_HASH_ShMem_Desc_T  *desc,UI8_T **rec, UI8_T *action, L_HASH_Index_T *idx)
{
    L_HASH_ShMem_WorkSpace_T      *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T           *p;
    BOOL_T                        ret = FALSE;

    if((desc == NULL) || (rec == NULL) || (action == NULL))
    {
        return FALSE;
    }
    if(L_DLST_ShMem_Indexed_Dblist_Dequeue(&wk->job_queue_desc, idx) == FALSE)
    {
        return FALSE;
    }
    p = SHMEM_HASH_RECORD(*idx);
    *rec = (UI8_T *)(p+1);
    *action = p->action;

    ret = L_HASH_ShMem_FSM(desc, L_HASH_DEQ_EV, (UI8_T *)rec, *idx);
    if(l_hash_debug & DEBUG_LEVEL_DEQUEUE)
    {
        printf("L_HASH_ShMem_DequeueJobList dequeues record \n");
        L_HASH_PrintRecord(p);
    }
    return ret;
} /* end of L_HASH_ShMem_DequeueJobList() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_OperationResult
 * -------------------------------------------------------------------------
 * PURPOSE: Result of the job dequeue from JobList.
 * INPUT  : desc  - A specific desciptor to process.
 *          event - L_HASH_SUCCESS_EV \ L_HASH_FAIL_EV
                    L_SYNC_OK_EV \ L_HASH_SYNC_FAIL_EV
 *          rec   - Specific record
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_OperationResult (L_HASH_ShMem_Desc_T  *desc, UI32_T event, UI8_T *user_rec)
{
    L_HASH_DataRecord_T          *hash_data;
    L_HASH_Index_T               hash_index,idx;
    UI8_T                        key_buffer[MAX_KEY_LENGTH];

    /* BODY */
    if ((desc == NULL) || (user_rec == NULL))
    {
        return FALSE;
    }
    hash_index = idx = 0;

    if((event != L_HASH_SET_SUCCESS_EV) && (event != L_HASH_DEL_SUCCESS_EV) && (event != L_HASH_SYNC_DONE_EV) && (event != L_HASH_FAIL_EV))
    {
        return FALSE;
    }

    if(!L_HASH_ShMem_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        return FALSE;
    }
    if(L_HASH_ShMem_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, &idx) == FALSE)
    {
        return FALSE;
    }
    L_HASH_ShMem_FSM(desc, event, user_rec, idx);

    if (l_hash_debug & DEBUG_LEVEL_DEQUEUE)
    {
        printf("L_HASH_ShMem_OperationResult sets record \n");
        L_HASH_PrintRecord(hash_data);
    }
    return TRUE;
} /* end of L_HASH_ShMem_OperationResult() */

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
BOOL_T L_HASH_ShMem_GetExactRecord(L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec)
{
    L_HASH_DataRecord_T         *hash_data;
    L_HASH_Index_T              hash_index,idx;
    UI8_T                       key_buffer[MAX_KEY_LENGTH];

    if ((desc == NULL) || (user_rec == NULL))
    {
        return FALSE;
    }
    hash_index = idx = 0;

    if (!L_HASH_ShMem_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if(l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("AMTRDRV_HASH_DeriveRecordKey in set record fails\n");
        }
        return FALSE;
    }
    /* If the record is found & its action == ADD we will return TRUE to caller
     * Since its action == Delete may only stay in OM for temperary
     */
    if((L_HASH_ShMem_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, &idx))
        &&(hash_data->action != L_HASH_ACTION_DEL))
    {
        memcpy(user_rec,hash_data->data_rec,desc->record_size);
        return TRUE;
    }
    /* record isn't found */
    else
    {
        return FALSE;
    }

}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetRecordByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get a Record by giving index
 * INPUT  : desc        - A specific desciptor to process.
 *          index       - record index
 * OUTPUT : user_record - the address record
 *          action      - record action
 * RETURN : None
 * NOTES  : We assume the index is legally to access the exist record we don't do any
 *          error handler.
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_ShMem_GetRecordByIndex(L_HASH_ShMem_Desc_T  *desc, UI32_T index , UI8_T *user_rec,UI8_T *action)
{
    L_HASH_ShMem_WorkSpace_T  *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T       *data;

    data = SHMEM_HASH_RECORD(index);
    *action = data->action;
    memcpy(user_rec,data->data_rec,desc->record_size);
    return;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetNRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Display N record at a time
 * INPUT  : desc         - A specific desciptor to process.
 *          record_count - The number of record to display
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
void L_HASH_ShMem_GetNRecord(L_HASH_ShMem_Desc_T  *desc, UI32_T record_count)
{
    L_HASH_ShMem_WorkSpace_T    *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T         *hash_rec;
    UI32_T                      i = 0;

    for (;i<record_count; i++)
    {
        hash_rec = SHMEM_HASH_RECORD(i);
        L_HASH_PrintRecord(hash_rec);
    }

    return;
} /* end of L_HASH_ShMem_GetNRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetRecordPtrByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get the Record Pointer by index
 * INPUT  : desc      - A specific desciptor to process.
 *          index     -
 *          user_rec  - the address entry
 * OUTPUT : None
 * RETURN : None
 * NOTES  : We assume the index is legally to access the exist record we don't do any
 *          error handler.
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_ShMem_GetRecordPtrByIndex(L_HASH_ShMem_Desc_T  *desc, UI8_T **user_rec, UI32_T index )
{
    L_HASH_ShMem_WorkSpace_T    *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T    *data;

    data  = SHMEM_HASH_RECORD(index);
    *user_rec = data->data_rec;
    return;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetRecordIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get exact record's index in database by giving reocrd
 * INPUT  : desc         - A specific desciptor to process.
 *          user_rec     - the address entry need to be compared.
 * OUTPUT : record_index - the record's index in OM
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_GetRecordIndex(L_HASH_ShMem_Desc_T *desc, UI8_T  *user_rec, L_HASH_Index_T *record_index)
{
    L_HASH_DataRecord_T             *hash_data;
    L_HASH_Index_T                  hash_index;
    UI8_T                           key_buffer[MAX_KEY_LENGTH];

    if((desc == NULL) || (user_rec == NULL))
    {
        return FALSE;
    }

    hash_index = *record_index = 0;

    if (!L_HASH_ShMem_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_DeriveRecordKey in set record fails\n");
        }
        return FALSE;
    }

    if (L_HASH_ShMem_LocateRecord(desc, key_buffer, &hash_index, (UI8_T **)(void *)&hash_data, record_index))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_GetRecordSyncValueByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Get a Record sync value by index
 * INPUT  : desc           - A specific desciptor to process.
 *          index          -
 *          sync_value     - to determine it still wait for sync to hisam or callback
 * OUTPUT : None
 * RETURN : None
 * NOTES  : We assume the index is legally to access the exist record we don't do any
 *          error handler.
 *          (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_ShMem_GetRecordSyncValueByIndex(L_HASH_ShMem_Desc_T  *desc, UI8_T *sync_value, UI32_T index )
{
    L_HASH_ShMem_WorkSpace_T    *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T     *data;

    data = SHMEM_HASH_RECORD(index);
    *sync_value = data->wait_for_sync;
    return;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_SetRecordSyncValueByIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Set a Record sync value by index
 * INPUT  : desc           - A specific desciptor to process.
 *          index          -
 *          sync_value     - to determine it still wait for sync to hisam or callback
 * OUTPUT : None
 * RETURN : None
 * NOTES  : We assume the index is legally to access the exist record we don't do any
 *          error handler.
 *         (ADD by MIKE_YEH)
 * -------------------------------------------------------------------------*/
void L_HASH_ShMem_SetRecordSyncValueByIndex(L_HASH_ShMem_Desc_T  *desc, UI8_T sync_value, UI32_T index )
{
    L_HASH_ShMem_WorkSpace_T    *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T     *data;

    data = SHMEM_HASH_RECORD(index);
    data->wait_for_sync = sync_value;
    return;
}

/* LOCAL SUBPROGRAM DEFINITION
 */
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_LocateRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Locate a specific record in the hash table.
 * INPUT  : desc        - A specific desciptor to process.
 *          key         - Key derive from given user record
 * OUTPUT : hash_index  - bucket number to search the record
 *          hash_data   - Hash Data found in the hash table.
 *          table_idx   - Index of record in the record table.
 * RETURN : TRUE        - Record Found
 *          FALSE       - Record does not exist.
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_LocateRecord(L_HASH_Desc_T  *desc, UI8_T *key, L_HASH_Index_T *hash_index, UI8_T **hash_data, L_HASH_Index_T *table_idx)
{
    /* Local Variable Declaration
     */
    L_HASH_Hash_Header_T         *hh_p;
    L_HASH_DataRecord_T          *data;
    L_HASH_WorkSpace_T           *wk;
    L_HASH_Index_T               hash_bucket = 0, current_rec_idx;

    /* BODY
     */
    wk = desc->buffer;

    if(L_HASH_DeriveHashIndex(desc, key, &hash_bucket) == FALSE)
    {
        return FALSE;
    }

    hh_p = wk->hash_header_ptr+hash_bucket;
    *hash_index = hash_bucket;
    for(current_rec_idx = hh_p->first_link_idx;
        current_rec_idx!=NULL_INDEX;
        current_rec_idx = ((L_HASH_DataRecord_T*)HASH_RECORD(current_rec_idx))->next_index)
    {
        UI8_T current_rec_key[MAX_KEY_LENGTH];

        data = ((L_HASH_DataRecord_T*)HASH_RECORD(current_rec_idx));
        L_HASH_DeriveRecordKey(desc, data->data_rec, current_rec_key);
        if(!memcmp(current_rec_key, key, wk->total_key_len))
        {
            *hash_data = (UI8_T *)data;
            *table_idx = (UI16_T )(((UI32_T)data-(UI32_T)(HASH_WK->data_rec_ptr)) / wk->hash_rec_len);
            return TRUE;
        }
    }
    return FALSE;
} /* end of L_HASH_LocateRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_LocateAndRemoveRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Locate and remove a specific record in the hash table.
 * INPUT  : desc      - A specific desciptor to process.
 *          key       - Key derive from given user record
 * OUTPUT : table_idx - Index of record in the record table.
 * RETURN : TRUE      - Record Found
 *          FALSE     - Record does not exist.
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_LocateAndRemoveRecord(L_HASH_Desc_T  *desc, UI8_T *key, L_HASH_Index_T *table_idx)
{
    /* Local Variable Declaration
     */
    L_HASH_Hash_Header_T        *hh_p;
    L_HASH_DataRecord_T         *data_p;
    L_HASH_WorkSpace_T          *wk;
    L_HASH_Index_T              hash_bucket, current_rec_idx, next_rec_idx, previous_rec_idx;

    /* BODY
     */
    wk = desc->buffer;

    L_HASH_DeriveHashIndex(desc, key, &hash_bucket);
    hh_p = wk->hash_header_ptr+hash_bucket;

    for(current_rec_idx = hh_p->first_link_idx;
        current_rec_idx!=NULL_INDEX;
        current_rec_idx=data_p->next_index)
    {
        UI8_T current_rec_key[MAX_KEY_LENGTH];

        data_p = (L_HASH_DataRecord_T*)HASH_RECORD(current_rec_idx);
        L_HASH_DeriveRecordKey(desc, data_p->data_rec, current_rec_key);
        if(memcmp(current_rec_key, key, wk->total_key_len) != 0)
        {
            continue;
        }
        previous_rec_idx = data_p->previous_index;
        next_rec_idx = data_p->next_index;
        if(current_rec_idx == hh_p->first_link_idx)
        {
            if(next_rec_idx != NULL_INDEX)
            {
                hh_p->first_link_idx = next_rec_idx;
            }
            else
            {
                hh_p->first_link_idx = NULL_INDEX;
                hh_p->last_link_idx = NULL_INDEX;
            }
        }
        else if (current_rec_idx == hh_p->last_link_idx)
        {
            if(previous_rec_idx != NULL_INDEX)
            {
                hh_p->last_link_idx = previous_rec_idx;
                ((L_HASH_DataRecord_T*)HASH_RECORD(previous_rec_idx))->next_index = NULL_INDEX;
            }
            else
            {
                hh_p->last_link_idx = NULL_INDEX;
                hh_p->first_link_idx = NULL_INDEX;
            }
        }
        else
        {
            ((L_HASH_DataRecord_T*)HASH_RECORD(previous_rec_idx))->next_index = next_rec_idx;
            ((L_HASH_DataRecord_T*)HASH_RECORD(next_rec_idx))->previous_index = previous_rec_idx;
        }
        *table_idx = ((UI32_T)data_p-(UI32_T)(wk->data_rec_ptr)) / wk->hash_rec_len;
        return TRUE;
    } /* end of if */
    return FALSE;
} /* end of L_HASH_LocateAndRemoveRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_AllocRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Allocate a record buffer.
 * INPUT  : desc - Hash Database descriptor.
 * OUTPUT : idx  - Index of the data record table.
 * RETURN : if found return record index, else return NULL_INDEX
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_AllocRecord (L_HASH_Desc_T *desc, L_HASH_Index_T *idx)
{
    L_HASH_WorkSpace_T    *wk;
    char                  *p;

    wk = desc->buffer;
    if((p= L_PT_Allocate (&wk->data_record_desc)) == NULL )
    {
        return FALSE;
    }

    memset(p, 0, wk->hash_rec_len);
    *idx = ((UI32_T)p-(UI32_T)(wk->data_rec_ptr)) / wk->hash_rec_len;

    return TRUE;
} /* end of L_HASH_AllocRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_InsertHash
 * -------------------------------------------------------------------------
 * PURPOSE: Insert a specific index to the hash table.
 * INPUT  : desc       - Hash Database descriptor.
 *	    hash_index - A specific hash bucket to insert data.
 *	    p          - hash data record.
 * OUTPUT : NONE
 * RETURN : TRUE / FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_InsertHash(L_HASH_Desc_T *desc, L_HASH_Index_T hash_bucket_idx, L_HASH_Index_T rec_idx)
{
    L_HASH_Hash_Header_T      *hh_p;
    L_HASH_DataRecord_T       *data_p;

    /* BODY
     */
    hh_p = HASH_WK->hash_header_ptr+hash_bucket_idx;
    data_p = (L_HASH_DataRecord_T*)(HASH_RECORD(rec_idx));

    if (hh_p->first_link_idx == NULL_INDEX)
    {
        hh_p->first_link_idx = rec_idx;
        data_p->next_index = NULL_INDEX;
        data_p->previous_index = rec_idx;
        hh_p->last_link_idx = rec_idx;
    }
    else
    {
        data_p->next_index = hh_p->first_link_idx;
        data_p->previous_index = rec_idx;
        ((L_HASH_DataRecord_T*)HASH_RECORD(hh_p->first_link_idx))->previous_index = rec_idx;
        hh_p->first_link_idx = rec_idx;
    }

    return TRUE;
} /* end of L_HASH_InsertHash() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_InsertQueryGroup
 * -------------------------------------------------------------------------
 * PURPOSE: Insert a specific index to each query group.
 * INPUT  : desc - Hash Database descriptor.
 *          rec  - Data record to be insert to query group.
 *          idx  - index of data record in data record table.
 * OUTPUT : NONE
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_InsertQueryGroup(L_HASH_Desc_T *desc, UI8_T *user_rec, L_HASH_Index_T idx)
{
    L_DLST_Indexed_Dblist_T         *dblist;
    L_HASH_WorkSpace_T              *wk;
    UI32_T                          i, element_index;
    BOOL_T                          result = TRUE;


    /* BODY
     */
    wk = desc->buffer;
    element_index = 0;
    for (i = 0; (desc->element_of_query_group[i]> 0) && (i<L_HASH_MAX_QUERY_GROUP); i++)
    {
        if(desc->acquire_element_index_of_query_group_fun == 0)
        {
            break;
        }
        desc->acquire_element_index_of_query_group_fun(i, user_rec, &element_index);
        if (element_index >= desc->element_of_query_group[i])
        {
            result = FALSE;
            continue;
        }
        dblist = (L_DLST_Indexed_Dblist_T *)((UI32_T )(wk->query_group_ptr[i])+(element_index*sizeof(L_DLST_Indexed_Dblist_T)));
        if (L_DLST_Indexed_Dblist_Enqueue(dblist, idx) == FALSE)
        {
            result = FALSE;
            continue;
        }
    }
    return result;
} /* end of L_HASH_InsertQueryGroup() */

static UI32_T L_HASH_DeleteQueryGroup_Callback(L_HASH_Index_T index,UI32_T desc)
{
    L_HASH_Desc_T           *local_desc;
    L_HASH_DataRecord_T     *data;

    local_desc = (L_HASH_Desc_T *)desc;
    data = (L_HASH_DataRecord_T *)HASH_RECORD(index);
    L_HASH_DeleteRecord(local_desc, (UI8_T *)data->data_rec,&index);

    return L_DLST_SEARCH_CONTINUE;
}


/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_DeriveHashIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Derive Hash Index base on record key
 * INPUT  : desc - Hash Database descriptor.
 *          key  - Key array of given user record
 * OUTPUT : hash_index - hash bucket number
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T  L_HASH_DeriveHashIndex(L_HASH_Desc_T *desc, UI8_T *key, L_HASH_Index_T *hash_index)
{
    L_HASH_WorkSpace_T *wk = (L_HASH_WorkSpace_T*)desc->buffer;

    /* BODY
     */
    if((desc == NULL) || (key == NULL) || (hash_index == NULL))
    {
        return FALSE;
    }

    switch (desc->hash_method)
    {
        case L_HASH_HASH_METHOD_WORD_XOR:
        {
            UI16_T          *key_ptr;
            L_HASH_Index_T  i,hash_key;

            i = hash_key= 0;
            /* two bytes for each key segment
             */
            key_ptr = (UI16_T *)key;
            for (; i < (wk->total_key_len / 2); i++)
            {
                hash_key ^= *(key_ptr + i);
            }

            /* Need to take care of remainging one byte seperately
             */
            if(wk->total_key_len & 0x1) /* equal to if(wk->total_key_len % 2) */
            {
                *((UI8_T*)(&hash_key)) ^= *(UI8_T*)(key_ptr + i);
            }

            *hash_index = hash_key % (desc->nbr_of_hash_bucket - 1);
        }
            break;
        default:
            return FALSE;
            break;
    } /* end of switch */

    return TRUE;
} /* end of L_HASH_DeriveHashIndex() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_FSM
 * -------------------------------------------------------------------------
 * PURPOSE: Hash Table Finite State Machine.
 * INPUT  : desc        - Specific descriptor
 *	        event_type  - specific event type
 *	        rec         - user data
 *	        idx         - index of the specific record in the record table.
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_FSM(L_HASH_Desc_T *desc, UI32_T  event_type, UI8_T *user_rec, UI32_T idx)
{
    /* For FSM_table please refer to AMTRDRV_HASH_FSM.xls
     */
    const static UI8_T fsm_table[5][7] =
    {
        /* L_HASH_NULL_ST
         */
        {L_HASH_NULL_ST,
         L_HASH_NULL_ST,
         L_HASH_NULL_ST,
         L_HASH_NULL_ST,
         L_HASH_DIRTY_TST,
         L_HASH_NULL_ST,
         L_HASH_NULL_ST},

        /* L_HASH_DIRTY_ST
         */
        {L_HASH_DIRTY_ST,
         L_HASH_DIRTY_ST,
         L_HASH_DIRTY_ST,
         L_HASH_DIRTY_ST,
         L_HASH_DIRTY_ST,
         L_HASH_DIRTY_ST,
         L_HASH_PROCESS_ST},

        /* L_HASH_PROCESS_ST
         */
        {L_HASH_READY_ST,
         L_HASH_DEL_WAIT_SYNC_TST,
         L_HASH_PROCESS_ST,
         L_HASH_DIRTY_TST,
         L_HASH_DIRTY_TST,
         L_HASH_DIRTY_TST,
         L_HASH_PROCESS_ST},

        /* L_HASH_READY_ST
         */
        {L_HASH_READY_ST,
         L_HASH_READY_ST,
         L_HASH_READY_ST,
         L_HASH_READY_ST,
         L_HASH_DIRTY_TST,
         L_HASH_DIRTY_TST,
         L_HASH_READY_ST},

        /* L_HASH_DEL_WAIT_SYNC_ST
         */
        {L_HASH_DEL_WAIT_SYNC_ST,
         L_HASH_DEL_WAIT_SYNC_ST,
         L_HASH_DEL_WAIT_SYNC_TST,
         L_HASH_DEL_WAIT_SYNC_ST,
         L_HASH_DIRTY_TST,
         L_HASH_DEL_WAIT_SYNC_ST,
         L_HASH_DEL_WAIT_SYNC_ST}
    };

    L_HASH_DataRecord_T     *hash_data;

    if ((desc == NULL) || (user_rec == NULL))
    {
        return FALSE;
    }

    hash_data = (L_HASH_DataRecord_T *)HASH_RECORD(idx);
    switch (hash_data->state =fsm_table[hash_data->state][event_type])
    {
        case L_HASH_NULL_ST:
        case L_HASH_DIRTY_ST:
        case L_HASH_PROCESS_ST:
        case L_HASH_READY_ST:
        case L_HASH_DEL_WAIT_SYNC_ST:
            break;
        case L_HASH_DIRTY_TST:
            if (L_DLST_Indexed_Dblist_Enqueue(&HASH_WK->job_queue_desc, idx) == FALSE)
            {
                return FALSE;
            }
            hash_data->state = L_HASH_DIRTY_ST;
            break;
        case L_HASH_DEL_WAIT_SYNC_TST:
            if(hash_data->wait_for_sync != 0)
            {
                hash_data->state = L_HASH_DEL_WAIT_SYNC_ST;
            }
            else
            {
                hash_data->state = L_HASH_NULL_ST;
                if(L_HASH_DeleteRecordAction(desc, user_rec) == FALSE)
                {
                    return FALSE;
                }
            }
            break;
        default:
            break;
    } /* end of switch */
#if 0  /*mask for performance testing; water*/
    printf(" NEW FSM state :%d    Event_Type:%ld \r\n",hash_data->state,event_type);
#endif
    return TRUE;
} /* end of L_HASH_FSM() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_SetHashRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Set user record information to specific address in record table.
 * INPUT  : desc - Specific descriptor
 *          rec  - user data
 *          key  - Key information derive from user record
 *          idx  - specific table index in record table.
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_SetHashRecord(L_HASH_Desc_T *desc, UI8_T *user_rec, UI8_T *key, UI32_T idx)
{
    L_HASH_DataRecord_T *p;

    /* BODY
     */
    p = (L_HASH_DataRecord_T*)((UI32_T)(HASH_WK->data_rec_ptr) + (HASH_WK->hash_rec_len * idx));
    p->action = L_HASH_ACTION_ADD;
    p->wait_for_sync =0;
    p += 1;
    memmove(p, user_rec, desc->record_size);
    return TRUE;
} /* end of L_HASH_SetHashRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_UpdateHashRecordState
 * -------------------------------------------------------------------------
 * PURPOSE: update the hash record state to ready state
 * INPUT  : desc        - Specific descriptor
 *          idx         - specific table index in record table.
 *          user_state  - the state of FSM
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : ADD by MIKE_YEH
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_UpdateHashRecordState(L_HASH_Desc_T *desc,UI32_T idx,UI8_T user_state)
{
    L_HASH_DataRecord_T *p;

    /* BODY */
    p = (L_HASH_DataRecord_T*)((UI32_T)(HASH_WK->data_rec_ptr) + (HASH_WK->hash_rec_len * idx));
    p->state = user_state;
    return TRUE;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_DeleteRecordAction
 * -------------------------------------------------------------------------
 * PURPOSE: Delete a record from HASH database.
 * INPUT:   desc - A specific desciptor to process.
 *          rec  - A specific record to be removed.
 * OUTPUT:  None.
 * RETURN:  TRUE \ FALSE
 * NOTES:   None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_DeleteRecordAction(L_HASH_Desc_T *desc, UI8_T *user_rec)
{
    L_HASH_Index_T        idx;
    UI8_T                 key_buffer[MAX_KEY_LENGTH];


    /* BODY
     */
    if ((desc == NULL) || (user_rec == NULL))
    {
        return FALSE;
    }
    idx = 0;

    if (!L_HASH_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        return FALSE;
    }
    /* Remove record from Hash Link
     */
    if (L_HASH_LocateAndRemoveRecord(desc, key_buffer, &idx) == FALSE)
    {
        return FALSE;
    }
    /* Remove record from JobQueue if exist
     */
    L_DLST_Indexed_Dblist_DeleteEntry(&HASH_WK->job_queue_desc, idx);
    
    /* Reset record from record table.
     */
    L_PT_Free ( &HASH_WK->data_record_desc, (void *)HASH_RECORD(idx) );
    return TRUE;
} /* end of L_HASH_DeleteRecordAction() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_NewRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Insert a new record to HASH database and associate query group
 * INPUT  : desc - A specific desciptor to process.
 *          user_rec - A specific record to be process.
 *          key  - Key derive from given user record
 *          hash_index - bucket number of hash table to insert record.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_NewRecord(L_HASH_Desc_T *desc, UI8_T *user_rec, UI8_T *key, L_HASH_Index_T hash_index, L_HASH_Index_T *idx)
{
    /* BODY
     */
    if ((desc == NULL) || (user_rec == NULL) || (idx == NULL))
    {
        return FALSE;
    }

    if (L_HASH_AllocRecord (desc, idx) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_AllocRecord in new record fails\n");
        }
        return FALSE;
    }

    L_HASH_SetHashRecord(desc, user_rec, key, *idx);

    /* Insert record to hash table
     */
    if (L_HASH_InsertHash(desc, hash_index, *idx) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_InsertHash in new record fails\n");
        }
        L_PT_Free ( &HASH_WK->data_record_desc, (void *)HASH_RECORD(*idx) );
        return FALSE;
    }

    /* Insert Record to appropriate element index of each query group.
     */
    if (L_HASH_InsertQueryGroup(desc, user_rec, *idx) == FALSE)
    {
        L_HASH_LocateAndRemoveRecord(desc, key, idx);
        L_PT_Free ( &HASH_WK->data_record_desc, (void *)HASH_RECORD(*idx) );
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_InsertQueryGroup in new record fails\n");
        }
        return FALSE;
    }

    return TRUE;
} /* end of L_HASH_NewRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ReplaceRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Replace exist record in HASH database.  Check each query group
 *          to make sure record still associates with the correct query group
 *          element.
 * INPUT  : desc      - A specific desciptor to process.
 *          user_rec  - A specific record to be process.
 *          key       - Key derive from given user record
 *          idx       - table index of the hash data.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ReplaceRecord(L_HASH_Desc_T *desc, UI8_T *user_rec, UI8_T *key, UI32_T idx)
{
    /* BODY
     */
    if (L_HASH_UpdateQueryGroup(desc, idx, user_rec) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_UpdateQueryGroup in replace record fails\n");
        }
        return FALSE;
    }

    if (L_HASH_SetHashRecord(desc, user_rec, key, idx) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_SetHashRecord in replace record fails\n");
        }
        return FALSE;
    }
    return TRUE;
} /* end of L_HASH_ReplaceRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_UpdateQueryGroup
 * -------------------------------------------------------------------------
 * PURPOSE: Replace exist record in HASH database.  Check each query group
 *          to make sure record still associates with the correct query group
 *          element.
 * INPUT  : dblist - Address of dblist.
 * OUTPUT : query_element - index of the query group.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_UpdateQueryGroup(L_HASH_Desc_T *desc, L_HASH_Index_T idx, UI8_T *user_rec)
{
    L_DLST_Indexed_Dblist_T     *dblist, *clone_dblist;
    L_HASH_WorkSpace_T          *wk;
    UI32_T                      i, old_idx;
    UI32_T                      element_index = 0;
    BOOL_T                      result = TRUE;

    /* BODY
     */
    if (desc == NULL)
    {
        return FALSE;
    }
    if (desc->acquire_element_index_of_query_group_fun == 0)
    {
        return TRUE;
    }
    wk = desc->buffer;

    for (i = 0; (desc->element_of_query_group[i]> 0) && (i<L_HASH_MAX_QUERY_GROUP); i++)
    {

        desc->acquire_element_index_of_query_group_fun((UI16_T)i, user_rec, &element_index);

        if(element_index >= desc->element_of_query_group[i])
        {
            result = FALSE;
            continue;
        }

        dblist = (L_DLST_Indexed_Dblist_T *)wk->query_group_ptr[i];

        if (L_DLST_Indexed_Dblist_GetActualDblistDiscriptor(dblist, idx, &clone_dblist) == TRUE)
        {

            old_idx = ((UI32_T)clone_dblist - (UI32_T)dblist) / sizeof(L_DLST_Indexed_Dblist_T);

            if(old_idx == element_index)
            {
                continue;
            }
            if (L_DLST_Indexed_Dblist_DeleteEntry(clone_dblist, idx) == FALSE)
            {
                if(l_hash_debug & DEBUG_LEVEL_ERROR)
                {
                    printf("L_DLST_Indexed_Dblist_DeleteEntry in update query group fails\n");
                }
                result = FALSE;
                continue;
            }
        }
        clone_dblist = (L_DLST_Indexed_Dblist_T *)((UI32_T )(dblist)+(element_index*sizeof(L_DLST_Indexed_Dblist_T)));

        if (L_DLST_Indexed_Dblist_Enqueue(clone_dblist, idx) == FALSE)
        {
            if (l_hash_debug & DEBUG_LEVEL_ERROR)
            {
                printf("L_DLST_Indexed_Dblist_Enqueue in update query group fails\n");
                result = FALSE;
                continue;
            }
        }
    }
    return result;
} /* end of L_HASH_UpdateQueryGroup() */


/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_DeriveRecordKey
 * -------------------------------------------------------------------------
 * PURPOSE: Get Record key to seperate key buffer
 * INPUT  : desc        - HASH descriptor
 *          user_rec    - specific user record
 *          key_buffer  - a buffer to store key values
 * OUTPUT : key_buffer  - fill in key information
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_DeriveRecordKey(L_HASH_Desc_T *desc, UI8_T *user_rec, UI8_T *key_buffer)
{
    UI8_T       *key_ptr, *tmp_key_ptr;
    UI16_T      i;

    if ((desc == NULL) || (user_rec == NULL) || (key_buffer == NULL))
    {
        return FALSE;
    }

    key_ptr = key_buffer;
    for (i = 0; (desc->key_size[i]>0 && i < L_HASH_MAX_FIELD_NUMBER_OF_A_KEY); i++)
    {
        tmp_key_ptr = (UI8_T *)((UI32_T)user_rec + desc->key_offset[i]);
        memcpy(key_ptr, tmp_key_ptr, desc->key_size[i]);
        key_ptr += desc->key_size[i];
    } /* end of for */

    return TRUE;
} /* end of L_HASH_DeriveRecordKey() */


static void L_HASH_PrintRecord(L_HASH_DataRecord_T *hash_rec)
{
    UI8_T       display_str[30];

    if (hash_rec == NULL)
    {
        return;
    }
    printf("\n");

    switch (hash_rec->state)
    {
        case L_HASH_NULL_ST:
            strcpy((char *)display_str, "NULL");
            break;
        case L_HASH_DIRTY_ST:
            strcpy((char *)display_str, "L_HASH_DIRTY_ST");
            break;
        case L_HASH_PROCESS_ST:
            strcpy((char *)display_str, "L_HASH_PROCESS_ST");
            break;
        case L_HASH_READY_ST:
            strcpy((char *)display_str, "L_HASH_READY_ST");
            break;
        case L_HASH_DEL_WAIT_SYNC_ST:
            strcpy((char *)display_str, "L_HASH_DEL_WAIT_SYNC_ST");
            break;
        case L_HASH_DIRTY_TST:
            strcpy((char *)display_str, "L_HASH_DIRTY_TST");
            break;
        case L_HASH_DEL_WAIT_SYNC_TST:
            strcpy((char *)display_str, "L_HASH_DEL_WAIT_SYNC_TST");
            break;
        default:
            strcpy((char *)display_str, "NONE");
            break;
    }

    printf("state %s\n", (char *)display_str);
    printf("action %s\n", (hash_rec->action == L_HASH_ACTION_ADD) ? "ADD" : "DEL");

    return;
}

/*---------------------------------
 * Local functions for shared memory version
 *---------------------------------
 */
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_LocateRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Locate a specific record in the hash table.
 * INPUT  : desc        - A specific desciptor to process.
 *          key         - Key derive from given user record
 * OUTPUT : hash_index  - bucket number to search the record
 *          hash_data   - Hash Data found in the hash table.
 *          table_idx   - Index of record in the record table.
 * RETURN : TRUE        - Record Found
 *          FALSE       - Record does not exist.
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_LocateRecord(L_HASH_ShMem_Desc_T  *desc, UI8_T *key, L_HASH_Index_T *hash_index, UI8_T **hash_data, L_HASH_Index_T *table_idx)
{
    /* Local Variable Declaration
     */
    L_HASH_Hash_Header_T         *hh_p;
    L_HASH_DataRecord_T          *data;
    L_HASH_ShMem_WorkSpace_T     *wk;
    L_HASH_Index_T               hash_bucket = 0, current_rec_idx;

    /* BODY
     */
    wk = SHMEM_HASH_WK;

    if(L_HASH_ShMem_DeriveHashIndex(desc, key, &hash_bucket) == FALSE)
    {
        return FALSE;
    }

    hh_p = SHMEM_HASH_HEADER_PTR + hash_bucket;
    *hash_index = hash_bucket;
    for(current_rec_idx = hh_p->first_link_idx;
        current_rec_idx!=NULL_INDEX;
        current_rec_idx = (SHMEM_HASH_RECORD(current_rec_idx))->next_index)
    {
        UI8_T current_rec_key[MAX_KEY_LENGTH];

        data = SHMEM_HASH_RECORD(current_rec_idx);
        L_HASH_ShMem_DeriveRecordKey(desc, data->data_rec, current_rec_key);
        if(!memcmp(current_rec_key, key, wk->total_key_len))
        {
            *hash_data = (UI8_T *)data;
            *table_idx = (UI16_T )(((UI32_T)data-(UI32_T)SHMEM_DATA_RECORD_PTR) / wk->hash_rec_len);
            return TRUE;
        }
    }
    return FALSE;
} /* end of L_HASH_ShMem_LocateRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_LocateAndRemoveRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Locate and remove a specific record in the hash table.
 * INPUT  : desc      - A specific desciptor to process.
 *          key       - Key derive from given user record
 * OUTPUT : table_idx - Index of record in the record table.
 * RETURN : TRUE      - Record Found
 *          FALSE     - Record does not exist.
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_LocateAndRemoveRecord(L_HASH_ShMem_Desc_T  *desc, UI8_T *key, L_HASH_Index_T *table_idx)
{
    /* Local Variable Declaration
     */
    L_HASH_Hash_Header_T        *hh_p;
    L_HASH_DataRecord_T         *data_p;
    L_HASH_ShMem_WorkSpace_T          *wk;
    L_HASH_Index_T              hash_bucket, current_rec_idx, next_rec_idx, previous_rec_idx;

    /* BODY
     */
    wk = SHMEM_HASH_WK;

    L_HASH_ShMem_DeriveHashIndex(desc, key, &hash_bucket);
    hh_p = SHMEM_HASH_HEADER_PTR+hash_bucket;

    for(current_rec_idx = hh_p->first_link_idx;
        current_rec_idx!=NULL_INDEX;
        current_rec_idx=data_p->next_index)
    {
        UI8_T current_rec_key[MAX_KEY_LENGTH];

        data_p = SHMEM_HASH_RECORD(current_rec_idx);
        L_HASH_ShMem_DeriveRecordKey(desc, data_p->data_rec, current_rec_key);
        if(memcmp(current_rec_key, key, wk->total_key_len) != 0)
        {
            continue;
        }
        previous_rec_idx = data_p->previous_index;
        next_rec_idx = data_p->next_index;
        if(current_rec_idx == hh_p->first_link_idx)
        {
            if(next_rec_idx != NULL_INDEX)
            {
                hh_p->first_link_idx = next_rec_idx;
                SHMEM_HASH_RECORD(next_rec_idx)->previous_index = NULL_INDEX;
            }
            else
            {
                hh_p->first_link_idx = NULL_INDEX;
                hh_p->last_link_idx = NULL_INDEX;
            }
        }
        else if (current_rec_idx == hh_p->last_link_idx)
        {
            if(previous_rec_idx != NULL_INDEX)
            {
                hh_p->last_link_idx = previous_rec_idx;
                SHMEM_HASH_RECORD(previous_rec_idx)->next_index = NULL_INDEX;
            }
            else
            {
                hh_p->last_link_idx = NULL_INDEX;
                hh_p->first_link_idx = NULL_INDEX;
            }
        }
        else
        {
            SHMEM_HASH_RECORD(previous_rec_idx)->next_index = next_rec_idx;
            SHMEM_HASH_RECORD(next_rec_idx)->previous_index = previous_rec_idx;
        }
        *table_idx = current_rec_idx;
        return TRUE;
    } /* end of if */
    return FALSE;
} /* end of L_HASH_ShMem_LocateAndRemoveRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_AllocRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Allocate a record buffer.
 * INPUT  : desc - Hash Database descriptor.
 * OUTPUT : idx  - Index of the data record table.
 * RETURN : if found return record index, else return NULL_INDEX
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_AllocRecord (L_HASH_ShMem_Desc_T *desc, L_HASH_Index_T *idx)
{
    L_HASH_ShMem_WorkSpace_T    *wk;
    char                  *p;

    wk = SHMEM_HASH_WK;
    if((p= L_PT_ShMem_Allocate (&wk->data_record_desc)) == NULL )
    {
        return FALSE;
    }

    memset(p, 0, wk->hash_rec_len);
    *idx = ((UI32_T)p-(UI32_T)SHMEM_DATA_RECORD_PTR) / wk->hash_rec_len;

    return TRUE;
} /* end of L_HASH_ShMem_AllocRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_InsertHash
 * -------------------------------------------------------------------------
 * PURPOSE: Insert a specific index to the hash table.
 * INPUT  : desc       - Hash Database descriptor.
 *	    hash_index - A specific hash bucket to insert data.
 *	    p          - hash data record.
 * OUTPUT : NONE
 * RETURN : TRUE / FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_InsertHash(L_HASH_ShMem_Desc_T *desc, L_HASH_Index_T hash_bucket_idx, L_HASH_Index_T rec_idx)
{
    L_HASH_ShMem_WorkSpace_T  *wk;
    L_HASH_Hash_Header_T      *hh_p;
    L_HASH_DataRecord_T       *data_p;

    /* BODY
     */
    wk = SHMEM_HASH_WK;
    hh_p = SHMEM_HASH_HEADER_PTR+hash_bucket_idx;
    data_p = SHMEM_HASH_RECORD(rec_idx);

    if (hh_p->first_link_idx == NULL_INDEX)
    {
        hh_p->first_link_idx = rec_idx;
        data_p->next_index = NULL_INDEX;
        data_p->previous_index = NULL_INDEX;
        hh_p->last_link_idx = rec_idx;
    }
    else
    {
        data_p->next_index = hh_p->first_link_idx;
        data_p->previous_index = NULL_INDEX;
        SHMEM_HASH_RECORD(hh_p->first_link_idx)->previous_index = rec_idx;
        hh_p->first_link_idx = rec_idx;
    }

    return TRUE;
} /* end of L_HASH_ShMem_InsertHash() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_InsertQueryGroup
 * -------------------------------------------------------------------------
 * PURPOSE: Insert a specific index to each query group.
 * INPUT  : desc - Hash Database descriptor.
 *          rec  - Data record to be insert to query group.
 *          idx  - index of data record in data record table.
 * OUTPUT : NONE
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_InsertQueryGroup(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec, L_HASH_Index_T idx)
{
    L_DLST_ShMem_Indexed_Dblist_T         *dblist;
    L_HASH_ShMem_WorkSpace_T              *wk;
    UI32_T                          i, element_index;
    BOOL_T                          result = TRUE;


    /* BODY
     */
    wk = SHMEM_HASH_WK;
    element_index = 0;
    for (i = 0; (desc->element_of_query_group[i]> 0) && (i<L_HASH_MAX_QUERY_GROUP); i++)
    {
        if(acquire_element_index_of_query_group_fun[desc->acquire_element_index_of_query_group_fun_id] == 0)
        {
            break;
        }
        acquire_element_index_of_query_group_fun[desc->acquire_element_index_of_query_group_fun_id](i, user_rec, &element_index);
        if (element_index >= desc->element_of_query_group[i])
        {
            result = FALSE;
            continue;
        }
        dblist = SHMEM_QUERY_GROUP_PTR(i) + element_index;

        if (L_DLST_ShMem_Indexed_Dblist_Enqueue(dblist, idx) == FALSE)
        {
            result = FALSE;
            continue;
        }
    }
    return result;
} /* end of L_HASH_ShMem_InsertQueryGroup() */

static UI32_T L_HASH_ShMem_DeleteQueryGroup_Callback(L_HASH_Index_T index,UI32_T desc)
{
    L_HASH_ShMem_WorkSpace_T *wk = SHMEM_HASH_WK;
    L_HASH_ShMem_Desc_T      *local_desc;
    L_HASH_DataRecord_T      *data;

    local_desc = (L_HASH_ShMem_Desc_T *)desc;
    data = SHMEM_HASH_RECORD(index);
    L_HASH_ShMem_DeleteRecord(local_desc, (UI8_T *)data->data_rec,&index);

    return L_DLST_SEARCH_CONTINUE;
}


/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_DeriveHashIndex
 * -------------------------------------------------------------------------
 * PURPOSE: Derive Hash Index base on record key
 * INPUT  : desc - Hash Database descriptor.
 *          key  - Key array of given user record
 * OUTPUT : hash_index - hash bucket number
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T  L_HASH_ShMem_DeriveHashIndex(L_HASH_ShMem_Desc_T *desc, UI8_T *key, L_HASH_Index_T *hash_index)
{
    L_HASH_ShMem_WorkSpace_T *wk = SHMEM_HASH_WK;

    /* BODY
     */
    if((desc == NULL) || (key == NULL) || (hash_index == NULL))
    {
        return FALSE;
    }

    switch (desc->hash_method)
    {
        case L_HASH_HASH_METHOD_WORD_XOR:
        {
            UI16_T          *key_ptr;
            L_HASH_Index_T  i,hash_key;

            i = hash_key= 0;
            /* two bytes for each key segment
             */
            key_ptr = (UI16_T *)key;
            for (; i < (wk->total_key_len / 2); i++)
            {
                hash_key ^= *(key_ptr + i);
            }

            /* Need to take care of remainging one byte seperately
             */
            if(wk->total_key_len & 0x1) /* equal to if(wk->total_key_len % 2) */
            {
                *((UI8_T*)(&hash_key)) ^= *(UI8_T*)(key_ptr + i);
            }

            *hash_index = hash_key % (desc->nbr_of_hash_bucket - 1);
        }
            break;
        default:
            return FALSE;
            break;
    } /* end of switch */

    return TRUE;
} /* end of L_HASH_ShMem_DeriveHashIndex() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_FSM
 * -------------------------------------------------------------------------
 * PURPOSE: Hash Table Finite State Machine.
 * INPUT  : desc        - Specific descriptor
 *	        event_type  - specific event type
 *	        rec         - user data
 *	        idx         - index of the specific record in the record table.
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_FSM(L_HASH_ShMem_Desc_T *desc, UI32_T  event_type, UI8_T *user_rec, UI32_T idx)
{
    /* For FSM_table please refer to AMTRDRV_HASH_FSM.xls
     */
    const static UI8_T fsm_table[5][7] =
    {
        /* L_HASH_NULL_ST
         */
        {L_HASH_NULL_ST,
         L_HASH_NULL_ST,
         L_HASH_NULL_ST,
         L_HASH_NULL_ST,
         L_HASH_DIRTY_TST,
         L_HASH_NULL_ST,
         L_HASH_NULL_ST},

        /* L_HASH_DIRTY_ST
         */
        {L_HASH_DIRTY_ST,
         L_HASH_DIRTY_ST,
         L_HASH_DIRTY_ST,
         L_HASH_DIRTY_ST,
         L_HASH_DIRTY_ST,
         L_HASH_DIRTY_ST,
         L_HASH_PROCESS_ST},

        /* L_HASH_PROCESS_ST
         */
        {L_HASH_READY_ST,
         L_HASH_DEL_WAIT_SYNC_TST,
         L_HASH_PROCESS_ST,
         L_HASH_DIRTY_TST,
         L_HASH_DIRTY_TST,
         L_HASH_DIRTY_TST,
         L_HASH_PROCESS_ST},

        /* L_HASH_READY_ST
         */
        {L_HASH_READY_ST,
         L_HASH_READY_ST,
         L_HASH_READY_ST,
         L_HASH_READY_ST,
         L_HASH_DIRTY_TST,
         L_HASH_DIRTY_TST,
         L_HASH_READY_ST},

        /* L_HASH_DEL_WAIT_SYNC_ST
         */
        {L_HASH_DEL_WAIT_SYNC_ST,
         L_HASH_DEL_WAIT_SYNC_ST,
         L_HASH_DEL_WAIT_SYNC_TST,
         L_HASH_DEL_WAIT_SYNC_ST,
         L_HASH_DIRTY_TST,
         L_HASH_DEL_WAIT_SYNC_ST,
         L_HASH_DEL_WAIT_SYNC_ST}
    };

    L_HASH_ShMem_WorkSpace_T *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T     *hash_data;

    if ((desc == NULL) || (user_rec == NULL))
    {
        return FALSE;
    }

    hash_data = SHMEM_HASH_RECORD(idx);
    switch (hash_data->state =fsm_table[hash_data->state][event_type])
    {
        case L_HASH_NULL_ST:
        case L_HASH_DIRTY_ST:
        case L_HASH_PROCESS_ST:
        case L_HASH_READY_ST:
        case L_HASH_DEL_WAIT_SYNC_ST:
            break;
        case L_HASH_DIRTY_TST:
            if (L_DLST_ShMem_Indexed_Dblist_Enqueue(&wk->job_queue_desc, idx) == FALSE)
            {
                return FALSE;
            }
            hash_data->state = L_HASH_DIRTY_ST;
            break;
        case L_HASH_DEL_WAIT_SYNC_TST:
            if(hash_data->wait_for_sync != 0)
            {
                hash_data->state = L_HASH_DEL_WAIT_SYNC_ST;
            }
            else
            {
                hash_data->state = L_HASH_NULL_ST;
                if(L_HASH_ShMem_DeleteRecordAction(desc, user_rec) == FALSE)
                {
                    return FALSE;
                }
            }
            break;
        default:
            break;
    } /* end of switch */
#if 0  /*mask for performance testing; water*/
    printf(" NEW FSM state :%d    Event_Type:%ld \r\n",hash_data->state,event_type);
#endif
    return TRUE;
} /* end of L_HASH_ShMem_FSM() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_SetHashRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Set user record information to specific address in record table.
 * INPUT  : desc - Specific descriptor
 *          rec  - user data
 *          key  - Key information derive from user record
 *          idx  - specific table index in record table.
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_SetHashRecord(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec, UI8_T *key, UI32_T idx)
{
    L_HASH_ShMem_WorkSpace_T *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T *p;

    /* BODY
     */
    p = SHMEM_HASH_RECORD(idx);
    p->action = L_HASH_ACTION_ADD;
    p->wait_for_sync =0;
    p += 1;
    memmove(p, user_rec, desc->record_size);
    return TRUE;
} /* end of L_HASH_ShMem_SetHashRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_UpdateHashRecordState
 * -------------------------------------------------------------------------
 * PURPOSE: update the hash record state to ready state
 * INPUT  : desc        - Specific descriptor
 *          idx         - specific table index in record table.
 *          user_state  - the state of FSM
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : ADD by MIKE_YEH
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_UpdateHashRecordState(L_HASH_ShMem_Desc_T *desc,UI32_T idx,UI8_T user_state)
{
    L_HASH_ShMem_WorkSpace_T *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T *p;

    /* BODY */
    p = SHMEM_HASH_RECORD(idx);
    p->state = user_state;
    return TRUE;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_DeleteRecordAction
 * -------------------------------------------------------------------------
 * PURPOSE: Delete a record from HASH database.
 * INPUT:   desc - A specific desciptor to process.
 *          rec  - A specific record to be removed.
 * OUTPUT:  None.
 * RETURN:  TRUE \ FALSE
 * NOTES:   None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_DeleteRecordAction(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec)
{
    L_HASH_ShMem_WorkSpace_T *wk = SHMEM_HASH_WK;
    L_HASH_Index_T        idx;
    UI8_T                 key_buffer[MAX_KEY_LENGTH];


    /* BODY
     */
    if ((desc == NULL) || (user_rec == NULL))
    {
        return FALSE;
    }
    idx = 0;

    if (!L_HASH_ShMem_DeriveRecordKey(desc, user_rec, key_buffer))
    {
        return FALSE;
    }
    /* Remove record from Hash Link
     */
    if (L_HASH_ShMem_LocateAndRemoveRecord(desc, key_buffer, &idx) == FALSE)
    {
        return FALSE;
    }
    /* Remove record from JobQueue if exist
     */
    L_DLST_ShMem_Indexed_Dblist_DeleteEntry(&wk->job_queue_desc, idx);
    
    /* Reset record from record table.
     */
    L_PT_ShMem_Free ( &wk->data_record_desc, SHMEM_HASH_RECORD(idx) );
    return TRUE;
} /* end of L_HASH_ShMem_DeleteRecordAction() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_NewRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Insert a new record to HASH database and associate query group
 * INPUT  : desc - A specific desciptor to process.
 *          user_rec - A specific record to be process.
 *          key  - Key derive from given user record
 *          hash_index - bucket number of hash table to insert record.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_NewRecord(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec, UI8_T *key, L_HASH_Index_T hash_index, L_HASH_Index_T *idx)
{
    L_HASH_ShMem_WorkSpace_T *wk = SHMEM_HASH_WK;

    /* BODY
     */
    if ((desc == NULL) || (user_rec == NULL) || (idx == NULL))
    {
        return FALSE;
    }

    if (L_HASH_ShMem_AllocRecord (desc, idx) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_AllocRecord in new record fails\n");
        }
        return FALSE;
    }

    L_HASH_ShMem_SetHashRecord(desc, user_rec, key, *idx);

    /* Insert record to hash table
     */
    if (L_HASH_ShMem_InsertHash(desc, hash_index, *idx) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_InsertHash in new record fails\n");
        }
        L_PT_ShMem_Free ( &wk->data_record_desc, SHMEM_HASH_RECORD(*idx) );
        return FALSE;
    }

    /* Insert Record to appropriate element index of each query group.
     */
    if (L_HASH_ShMem_InsertQueryGroup(desc, user_rec, *idx) == FALSE)
    {
        L_HASH_ShMem_LocateAndRemoveRecord(desc, key, idx);
        L_PT_ShMem_Free ( &wk->data_record_desc, SHMEM_HASH_RECORD(*idx) );
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_InsertQueryGroup in new record fails\n");
        }
        return FALSE;
    }

    return TRUE;
} /* end of L_HASH_ShMem_NewRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_ReplaceRecord
 * -------------------------------------------------------------------------
 * PURPOSE: Replace exist record in HASH database.  Check each query group
 *          to make sure record still associates with the correct query group
 *          element.
 * INPUT  : desc      - A specific desciptor to process.
 *          user_rec  - A specific record to be process.
 *          key       - Key derive from given user record
 *          idx       - table index of the hash data.
 * OUTPUT : None.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_ReplaceRecord(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec, UI8_T *key, UI32_T idx)
{
    /* BODY
     */
    if (L_HASH_ShMem_UpdateQueryGroup(desc, idx, user_rec) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_UpdateQueryGroup in replace record fails\n");
        }
        return FALSE;
    }

    if (L_HASH_ShMem_SetHashRecord(desc, user_rec, key, idx) == FALSE)
    {
        if (l_hash_debug & DEBUG_LEVEL_ERROR)
        {
            printf("L_HASH_ShMem_SetHashRecord in replace record fails\n");
        }
        return FALSE;
    }
    return TRUE;
} /* end of L_HASH_ShMem_ReplaceRecord() */

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_UpdateQueryGroup
 * -------------------------------------------------------------------------
 * PURPOSE: Replace exist record in HASH database.  Check each query group
 *          to make sure record still associates with the correct query group
 *          element.
 * INPUT  : dblist - Address of dblist.
 * OUTPUT : query_element - index of the query group.
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_UpdateQueryGroup(L_HASH_ShMem_Desc_T *desc, L_HASH_Index_T idx, UI8_T *user_rec)
{
    L_DLST_ShMem_Indexed_Dblist_T     *dblist, *clone_dblist;
    L_HASH_ShMem_WorkSpace_T          *wk;
    UI32_T                      i, old_idx;
    UI32_T                      element_index = 0;
    BOOL_T                      result = TRUE;

    /* BODY
     */
    if (desc == NULL)
    {
        return FALSE;
    }
    if (acquire_element_index_of_query_group_fun[desc->acquire_element_index_of_query_group_fun_id] == 0)
    {
        return TRUE;
    }
    wk = SHMEM_HASH_WK;

    for (i = 0; (desc->element_of_query_group[i]> 0) && (i<L_HASH_MAX_QUERY_GROUP); i++)
    {

        acquire_element_index_of_query_group_fun[desc->acquire_element_index_of_query_group_fun_id]((UI16_T)i, user_rec, &element_index);

        if(element_index >= desc->element_of_query_group[i])
        {
            result = FALSE;
            continue;
        }

        dblist = SHMEM_QUERY_GROUP_PTR(i);

        if (L_DLST_ShMem_Indexed_Dblist_GetActualDblistDiscriptor(dblist, idx, &clone_dblist) == TRUE)
        {

            old_idx = ((UI32_T)clone_dblist - (UI32_T)dblist) / sizeof(L_DLST_ShMem_Indexed_Dblist_T);

            if(old_idx == element_index)
            {
                continue;
            }
            if (L_DLST_ShMem_Indexed_Dblist_DeleteEntry(clone_dblist, idx) == FALSE)
            {
                if(l_hash_debug & DEBUG_LEVEL_ERROR)
                {
                    printf("L_DLST_ShMem_Indexed_Dblist_DeleteEntry in update query group fails\n");
                }
                result = FALSE;
                continue;
            }
        }
        clone_dblist = dblist + element_index;

        if (L_DLST_ShMem_Indexed_Dblist_Enqueue(clone_dblist, idx) == FALSE)
        {
            if (l_hash_debug & DEBUG_LEVEL_ERROR)
            {
                printf("L_DLST_ShMem_Indexed_Dblist_Enqueue in update query group fails\n");
                result = FALSE;
                continue;
            }
        }
    }
    return result;
} /* end of L_HASH_ShMem_UpdateQueryGroup() */


/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_DeriveRecordKey
 * -------------------------------------------------------------------------
 * PURPOSE: Get Record key to seperate key buffer
 * INPUT  : desc        - HASH descriptor
 *          user_rec    - specific user record
 *          key_buffer  - a buffer to store key values
 * OUTPUT : key_buffer  - fill in key information
 * RETURN : TRUE \ FALSE
 * NOTES  : None
 * -------------------------------------------------------------------------*/
static BOOL_T L_HASH_ShMem_DeriveRecordKey(L_HASH_ShMem_Desc_T *desc, UI8_T *user_rec, UI8_T *key_buffer)
{
    UI8_T       *key_ptr, *tmp_key_ptr;
    UI16_T      i;

    if ((desc == NULL) || (user_rec == NULL) || (key_buffer == NULL))
    {
        return FALSE;
    }

    key_ptr = key_buffer;
    for (i = 0; (desc->key_size[i]>0 && i < L_HASH_MAX_FIELD_NUMBER_OF_A_KEY); i++)
    {
        tmp_key_ptr = (UI8_T *)((UI32_T)user_rec + desc->key_offset[i]);
        memcpy(key_ptr, tmp_key_ptr, desc->key_size[i]);
        key_ptr += desc->key_size[i];
    } /* end of for */

    return TRUE;
} /* end of L_HASH_ShMem_DeriveRecordKey() */
/*add by Tony.Lei*/
/*these functions are used for MAC Learning*/
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_UpdateReservedState
 * -------------------------------------------------------------------------
 * PURPOSE: update the hash record reserved state 
 * INPUT  : desc        - Specific descriptor
 *          idx         - specific table index in record table.
 *          user_state  - the state of FSM
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : ADD by MIKE_YEH
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_UpdateReservedState(L_HASH_ShMem_Desc_T *desc,UI32_T idx,UI8_T user_flag)
{
    L_HASH_ShMem_WorkSpace_T *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T *p;

    /* BODY */
    p = SHMEM_HASH_RECORD(idx);
    p->reserved[0] = p->reserved[0]|user_flag;
    return TRUE;
}
/*these functions are used for MAC Learning*/
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_HASH_ShMem_ClearReservedState
 * -------------------------------------------------------------------------
 * PURPOSE: Clear the hash record reserved state 
 * INPUT  : desc        - Specific descriptor
 *          idx         - specific table index in record table.
 *          user_state  - the state of FSM
 * OUTPUT : None
 * RETURN : TRUE \ FALSE
 * NOTES  : ADD by MIKE_YEH
 * -------------------------------------------------------------------------*/
BOOL_T L_HASH_ShMem_ClearReservedState(L_HASH_ShMem_Desc_T *desc,UI32_T idx,UI8_T user_flag)
{
    L_HASH_ShMem_WorkSpace_T *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T *p;

    /* BODY */
    p = SHMEM_HASH_RECORD(idx);
    p->reserved[0] = (p->reserved[0]&(~user_flag));
    return TRUE;
}


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
BOOL_T L_HASH_ShMem_GetReservedState(L_HASH_ShMem_Desc_T *desc,UI32_T idx,UI8_T *user_flag)
{
    L_HASH_ShMem_WorkSpace_T *wk = SHMEM_HASH_WK;
    L_HASH_DataRecord_T *p;

    /* BODY */
    p = SHMEM_HASH_RECORD(idx);
    *user_flag = p->reserved[0];
    return TRUE;
}

#ifdef L_HASH_BACKDOOR
static void L_HASH_FSM_PRINT(L_HASH_DataRecord_T *hash_data, UI32_T event_type, UI8_T state)
{
    UI8_T       display_str[30];

    L_HASH_PrintRecord(hash_data);

    switch (event_type)
    {
        case L_HASH_SET_EV:
            strcpy(display_str, "L_HASH_SET_EV");
            break;
        case L_HASH_DEL_EV:
            strcpy(display_str, "L_HASH_DEL_EV");
            break;
        case L_HASH_DEQ_EV:
            strcpy(display_str, "L_HASH_DEQ_EV");
            break;
        case L_HASH_SET_SUCCESS_EV:
            strcpy(display_str, "L_HASH_SET_SUCCESS_EV");
            break;
        case L_HASH_DEL_SUCCESS_EV:
            strcpy(display_str,"L_HASH_DEL_SUCCESS_EV");
            break;
        case L_HASH_SYNC_DONE_EV:
            strcpy(display_str,"L_HASH_SYNC_DONE_EV");
            break;
        case L_HASH_FAIL_EV:
            strcpy(display_str, "L_HASH_FAIL_EV");
            break;
        default:
            strcpy(display_str, "NONE");
            break;
    }
    printf("event_type %s\n", display_str);

    memset(display_str, 0, sizeof(display_str));
    switch (state)
    {
        case L_HASH_NULL_ST:
            strcpy(display_str, "L_HASH_NULL_ST");
            break;
        case L_HASH_DIRTY_ST:
            strcpy(display_str, "L_HASH_DIRTY_ST");
            break;
        case L_HASH_PROCESS_ST:
            strcpy(display_str, "L_HASH_PROCESS_ST");
            break;
        case L_HASH_READY_ST:
            strcpy(display_str, "L_HASH_READY_ST");
            break;
        case L_HASH_DEL_WAIT_SYNC_ST:
            strcpy(display_str, "L_HASH_DEL_WAIT_SYNC_ST");
            break;
        case L_HASH_DIRTY_TST:
            strcpy(display_str, "L_HASH_DIRTY_TST");
            break;
        case L_HASH_DEL_WAIT_SYNC_TST:
            strcpy(display_str, "L_HASH_DEL_WAIT_SYNC_TST");
            break;
        default:
            strcpy(display_str, "NONE");
            break;
    }

    printf("new state %s\n", display_str);
    return;
}

static void L_HASH_SetDebugFlag(UI32_T debug_flag)
{
    if (debug_flag == 0)
    {
        return;
    }
    else if ((debug_flag > 0) && (debug_flag < 5))
    {
        l_hash_debug ^= (0x01 << (debug_flag-1));
    }
    return;
}

static void L_HASH_PrintDebugMode(void)
{

    printf ("\n=======================================\n");
    printf (" 0: exit\n");
    printf (" 1: DEBUG_LEVEL_TRACE    \t%s\n", (l_hash_debug & DEBUG_LEVEL_TRACE) ? "ON":"OFF");
    printf (" 2: DEBUG_LEVEL_FSM     \t%s\n", (l_hash_debug & DEBUG_LEVEL_FSM) ? "ON":"OFF");
    printf (" 3: DEBUG_LEVEL_ERROR    \t%s\n", (l_hash_debug & DEBUG_LEVEL_ERROR ) ? "ON":"OFF");
    printf (" 4: DEBUG_LEVEL_DEQUEUE   \t%s\n", (l_hash_debug & DEBUG_LEVEL_DEQUEUE) ? "ON":"OFF");
    printf ("\n");
    printf ("\n=======================================\n");
    return;
}

static void L_HASH_BackDoor_Menu(void)
{
    int     ch;
    BOOL_T  eof = FALSE;
    char    buf[18];
    char    *terminal;

    /* BODY
     */
    while(!eof)
    {
        printf("\n");
        printf("0. Exit\n");
        printf("1. Set debug flag\n");
        printf("Select: ");

        BACKDOOR_MGR_RequestKeyIn(buf, 15);
        ch = (int)strtoul(buf, &terminal, 10);
        switch(ch)
        {
            case 0:
                eof = TRUE;
                break;
            case 1:
                ch = 0;
                L_HASH_PrintDebugMode();
                while (TRUE)
                {
                    BACKDOOR_MGR_RequestKeyIn(buf, 15);
                    ch = (int)strtoul(buf, &terminal, 10);
                    if (ch == 0)
                    {
                        break;
                    }
                    L_HASH_SetDebugFlag(ch);
                    L_HASH_PrintDebugMode();
                }
                break;
            default:
                break;
        }
    }
}

void hash_main(void)
{
    /* Local Variable Declaration
     */
    BOOL_T      eof = FALSE;
    UI32_T      count;
    char        str[3];

    /* BODY
     */
    while (!eof)
    {
        printf("\n==============================\n");
        printf("L_HASH Testing Function \n");
        printf(" 0: exist\n");
        printf(" 1: Create.\n");
        printf(" 2: Set Record.\n");
        printf(" 3. Delete Record.\n");
        printf(" 4. Delete Record Action.\n");
        printf(" 5. Destroy All.\n");
        printf(" 6. Dequeue Job List.\n");
        printf(" 7. Destroy.\n");
        printf(" 8. Operation Set Result Success\n");
        printf(" 9. Operation Result Fail\n");
        printf("10. Delete By Query Group\n");
        printf("11. Print Workspace status \n");
        printf("12. Get N Record Count \n");
        printf("13. Enable Debug backdoor\n");
        printf("14. Operation Del Result Sucess\n");
        printf("Select Input: ");

        gets(str);
        switch (atoi(str))
        {
            case 0:
                eof = TRUE;
                break;
            case 1:
                L_HASH_BD_Create() ;
                break;
            case 2:
                L_HASH_BD_SetRecord();
                break;
            case 3:
                L_HASH_BD_DeleteRecord();
                break;
            case 4:
                L_HASH_BD_DeleteRecordAction();
                break;
            case 5:
                L_HASH_DeleteAll(&test_desc);
                break;
            case 6:
                L_HASH_BD_DequeueRecord();
                break;
            case 7:
                L_HASH_Destroy(&test_desc);
                break;
            case 8:
                L_HASH_BD_SetOperationSuccess();
                break;
            case 9:
                L_HASH_BD_OperationFail();
                break;
            case 10:
                L_HASH_BD_DeleteQueryGroup();
                break;
            case 11:
                L_HASH_BD_WorkspaceStatus();                                                                                        				
                break;
            case 12:
                printf("Input Record Count: ");
                gets(str);
                count = atoi(str);
                printf("\n");
                L_HASH_GetNRecord(&test_desc, count);
                break;
            case 13:
                L_HASH_BackDoor_Menu();
                break;
            case 14:
                L_HASH_BD_DelOperationSuccess();
                break;
            default:
                break;
        } /* end of switch */
    }
    return;
} /* end of main() */

static BOOL_T L_HASH_BD_Create(void)
{

    memset(&test_desc, 0, sizeof(L_HASH_Desc_T));
    test_desc.nbr_of_hash_bucket= 10;
    test_desc.nbr_of_rec  = 20;
    test_desc.key_offset[0] = 0;
    test_desc.key_offset[1] = 4;
    test_desc.key_size[0] = 4;
    test_desc.key_size[1] = 4;
    test_desc.element_of_query_group[0] = 24;
    test_desc.acquire_element_index_of_query_group_fun = L_HASH_BD_AcquireFunction1;
    test_desc.hash_method  = L_HASH_HASH_METHOD_WORD_XOR;
    test_desc.record_size = sizeof(L_HASH_TestData_T);

    if(L_HASH_Create(&test_desc))
    {
        printf("Successfully Create Hash Database!\n");
    }
    else
    {
        printf("Fail to create Hash Database\n");
    }
    return TRUE;
}
static void L_HASH_BD_SetRecord(void)
{
    L_HASH_Index_T     index;
    L_HASH_BD_RequestData();
    if (L_HASH_SetRecord(&test_desc, (UI8_T *)&test_data,&index))
    {
        printf("L_HASH_SetRecord Success\n");
    }
    else
    {
        printf("L_HASH_SetRecord Fail\n");
    }
    return;
}

static BOOL_T L_HASH_BD_AcquireFunction1(UI32_T group, void *rec, UI32_T *element)
{
    L_HASH_TestData_T    *local_data;
    if (group == 0)
    {
        local_data = (L_HASH_TestData_T *)rec;
        *element = local_data->lport - 1;
    }
    else if (group == 1)
    {
        local_data = (L_HASH_TestData_T *)rec;
        *element = local_data->vid_ifindex - 1;
    }
    return TRUE;
}

static void L_HASH_BD_DeleteRecord(void)
{
    L_HASH_Index_T   index;
    L_HASH_BD_RequestData();
    if (L_HASH_DeleteRecord(&test_desc, (UI8_T *)&test_data,&index))
    {
        printf("L_HASH_DeleteRecord Success\n");
    }
    else
    {
        printf("L_HASH_DeleteRecord Fail\n");
    }
    return;
}

static void L_HASH_BD_DeleteRecordAction(void)
{
    L_HASH_BD_RequestData();
    if (L_HASH_DeleteRecordAction(&test_desc, (UI8_T *)&test_data))
    {
        printf("L_HASH_DeleteRecordAction Success\n");
    }
    else
    {
        printf("L_HASH_DeleteRecordAction Fail\n");
    }
    return;
}

static void L_HASH_BD_DequeueRecord(void)
{
    L_HASH_TestData_T               *dequeue_data;
    UI8_T                           get_action;
    L_HASH_Index_T                  get_index;

    if (L_HASH_DequeueJobList (&test_desc, (UI8_T**)&dequeue_data, &get_action, &get_index))
    {
        printf("L_HASH_DequeueJobList success.\n");
        printf("dequeue_data.lport %d\n", (int)dequeue_data->lport);
        printf("dequeue_data.vid_ifindex %d\n", (int)dequeue_data->vid_ifindex);
        printf("dequeue_data.key_index %d\n", (int)dequeue_data->key_index);
    }
    else
    {
        printf("L_HASH_DequeueJobList Fails \n");
    }
    return;
}

/* end of backdoor function */
static void L_HASH_BD_WorkspaceStatus(void)
{
    L_HASH_Desc_T                       *desc=&test_desc;
    L_HASH_WorkSpace_T                  *wk;
    L_HASH_Hash_Header_T                *hh;
    L_HASH_DataRecord_T                 *data;
    L_HASH_TestData_T                   *test_data;
    UI16_T                              table_idx;
    int                                 i;

    wk = test_desc.buffer;

    /* hash Bucket
     */
    printf("Bucket\tIDX\tlport\tvid\tkey\tState\t\n");
    for (i = 0; i < test_desc.nbr_of_hash_bucket; i++)
    {
        L_HASH_Index_T current_rec_idx;

        hh = wk->hash_header_ptr+i;
        for(current_rec_idx = hh->first_link_idx;
            current_rec_idx!=NULL_INDEX;
            current_rec_idx = ((L_HASH_DataRecord_T*)HASH_RECORD(current_rec_idx))->next_index)
        {
            data = (L_HASH_DataRecord_T*)HASH_RECORD(current_rec_idx);
            test_data = (L_HASH_TestData_T *)(data->data_rec);
            table_idx = (UI16_T )(((UI32_T)data-(UI32_T)(wk->data_rec_ptr)) / wk->hash_rec_len);
            printf("%d\t%d\t%d\t%d\t%d\t", (int)i,(int)table_idx, (int)test_data->lport, (int)test_data->vid_ifindex, (int)test_data->key_index);
            switch (data->state)
            {
                case L_HASH_NULL_ST:
                    printf("L_HASH_NULL_ST\n");
                    break;
                case L_HASH_DIRTY_ST:
                    printf("L_HASH_DIRTY_ST\n");
                    break;
                case L_HASH_PROCESS_ST:
                    printf("L_HASH_PROCESS_ST\n");
                    break;
                case L_HASH_READY_ST:
                    printf("L_HASH_READY_ST\n");
                    break;
                case L_HASH_DEL_WAIT_SYNC_ST:
                    printf("L_HASH_DEL_WAIT_SYNC_ST\n");
                    break;
                default:
                    printf("None\n");
                    break;
            }
        } /* end of if */
    }


    return;
}

static void L_HASH_BD_SetOperationSuccess(void)
{
    L_HASH_BD_RequestData();
    L_HASH_OperationResult (&test_desc, L_HASH_SET_SUCCESS_EV, (UI8_T *)&test_data);
    return;
}

static void L_HASH_BD_DelOperationSuccess(void)
{
    L_HASH_BD_RequestData();
    L_HASH_OperationResult (&test_desc, L_HASH_DEL_SUCCESS_EV, (UI8_T *)&test_data);
    return;
}

static void L_HASH_BD_OperationFail(void)
{
    L_HASH_BD_RequestData();
    L_HASH_OperationResult (&test_desc, L_HASH_FAIL_EV, (UI8_T *)&test_data);
    return;

}
static void L_HASH_BD_RequestData(void)
{
    char input[3];

    memset(&test_data, 0, sizeof(L_HASH_TestData_T));

    printf("\nEnter Port Number: ");
    gets(input);
    test_data.lport = atol(input);

    printf("\nEnter vlan Number: ");
    gets(input);
    test_data.vid_ifindex = atol(input);

    printf("\nEnter key Number: ");
    gets(input);
    test_data.key_index =  atol(input);

    return;
}

static void L_HASH_BD_DeleteQueryGroup(void)
{
    char        input[3];
    UI32_T      group, element;

    group = element = 0;

    printf("\nEnter Query Group: ");
    gets(input);
    group = atol(input);

    printf("\nEnter Group Element: ");
    gets(input);
    element = atol(input);
    if (L_HASH_SearchByQueryGroupAndDelete (&test_desc, group, element))
    {
        printf("Delete success\n");
    }
    else
    {
        printf("Delete Fail\n");
    }
    return;
}


#endif

