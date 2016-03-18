

/* Package Name: HISAM.C
 * Purpose: This package provides the functions to use ISAM structure.
 *          HISAM provides record-based data management capability,
 *          a sorted index table plus hashing function, makes searching
 *          more effiently.
 * Notes:
 *    1. The Caller must take care the mutual exculusion situation
 *    2. HISAM is a pure code.
 *    3. HISAM is an algorithm, original ISAM algorithm, plus storage management,
 *       to supprot record-based database system.
 *    4. HISAM suitable for any size record table (or database), but suggests in range
 *       500 to 100,000 records, when entry-number is less than 1000, Sort_List is better
 *       choice. More than 100,000 records, HISAM sould be adjusted for better performance.
 * History:
 *    5/7/99 -- Ryan
 *       isam.c create
 *    6/8/01 -- ryan
 *       re-design, and rename to hisam.c
 * Copyright(C)      Accton Corporation, 2001
 */



/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys_type.h"
#include "l_hisam.h"
#include "l_pt.h"
#include "l_cvrt.h"
#include "sysfun.h"   /* for using SYSFUN_Debug_Printf() */

/* NAMING CONSTANT DECLARATIONS
 */
#define NULL_INDEX            0xFFFF

/* HASHIDX_NET_SPAN: the span used to locate the next hash index when
 *                   hash key collision occurs
 */
#define HASHIDX_NEXT_SPAN        17

/* MACRO FUNCTION DECLARATIONS
 */

#ifdef HISAM_DEBUG
   static int mcount;
   static void* MyMalloc(UI32_T size);
   static void  MyFree (void *p);
#else
   #define MyMalloc  malloc
   #define MyFree    free
#endif /* HISAM_DEBUG */

/* #define PHY_REC_LEN     (desc->record_length+1) */
#define PHY_REC_LEN     L_ALIGN(desc->record_length+1,4)

/* RETURN: the the address of desc->wk */
#define WK              ((Work_T*)(desc->wk))

/* RETURN: the the address of record_table[ridx] */
#define RECORD(ridx)    (WK->record_table+PHY_REC_LEN * (ridx)+1)


/* RETURN: the the address of ipart[kidx] */
#define IPART(kidx)     &(WK->ipart[kidx])


/* PURPOSE: Check the record (static or dynamic)
 * INPUT:   ridx  - index of record_table
 * OUTPUT:  none
 * RETUEN:  if the record is static record then return TRUE, else return FALSE
 * NOTES:
 */
#define IS_STATIC_RECORD(ridx)   *(BOOL_T *)(RECORD(ridx)-1)


/* PURPOSE: Copy RECORD(record_index) to rec
 * INPUT:   ridx  - index of record_table
 * OUTPUT:  rec   - buffer pointer to store a record
 * RETUEN:  rec
 * NOTES:
 */
#define GET_RECORD(rec,ridx)     memmove(rec,RECORD(ridx),desc->record_length)


/* PURPOSE: Copy rec to RECORD(record_index)
 * INPUT:   rec   - buffer pointer to store a record
 *          ridx  - index of record_table
 * OUTPUT:  none
 * RETUEN:  none
 * NOTES:
 */
#define SET_RECORD(rec,ridx,is_static)       \
{  char *p = (char *)RECORD(ridx);                   \
   memmove(p,rec,desc->record_length);       \
   *--p = is_static;                         \
}


#define Abs(a,b)  (((a)>(b))? (a)-(b): (b)-(a))


/*---------------------------------
 * Macro for shared memory version
 *---------------------------------
 */

/* calculate size
 */
#define L_HISAM_SHMEM_GET_RECORD_TABLE_SIZE(desc) (desc->total_record_nbr * PHY_REC_LEN)
#define L_HISAM_SHMEM_GET_HASH_TABLE_SIZE(desc)   (desc->total_hash_nbr  * sizeof(UI16_T))
#define L_HISAM_SHMEM_GET_INDEX_TABLE_SIZE(desc)  (desc->total_index_nbr * sizeof(Index_T))
#define L_HISAM_SHMEM_GET_RLINK_TABLE_SIZE(desc)  (desc->total_record_nbr* sizeof(UI16_T))

/* calculate offset
 */
#define L_HISAM_SHMEM_GET_RECORD_BUFFER_OFFSET_TO_WK()  sizeof(ShMem_Work_T)
#define L_HISAM_SHMEM_GET_HASH_TABLE_BUFFER_OFFSET_TO_WK(desc)  (L_HISAM_SHMEM_GET_RECORD_BUFFER_OFFSET_TO_WK() + L_HISAM_SHMEM_GET_RECORD_TABLE_SIZE(desc))
#define L_HISAM_SHMEM_GET_INDEX_TABLE_BUFFER_OFFSET_TO_WK(desc, number_of_key)  (L_HISAM_SHMEM_GET_HASH_TABLE_BUFFER_OFFSET_TO_WK(desc) + L_HISAM_SHMEM_GET_HASH_TABLE_SIZE(desc) * (number_of_key))
#define L_HISAM_SHMEM_GET_RLINK_TABLE_BUFFER_OFFSET_TO_WK(desc, number_of_key)  (L_HISAM_SHMEM_GET_INDEX_TABLE_BUFFER_OFFSET_TO_WK(desc, number_of_key) + L_HISAM_SHMEM_GET_INDEX_TABLE_SIZE(desc) * (number_of_key))

/* addressing function
 */
#define L_HISAM_SHMEM_WK(desc) ((ShMem_Work_T *)L_CVRT_GET_PTR((desc), (desc)->wk_offset))
#define L_HISAM_SHMEM_RECORD_TABLE(wk)    ((UI8_T *)L_CVRT_GET_PTR((wk), (wk)->record_table_offset))
#define L_HISAM_SHMEM_RECORD(ridx)    (record_table + PHY_REC_LEN * (ridx) + 1)
#define L_HISAM_SHMEM_INDEX_TABLE(ipart)  ((Index_T *)L_CVRT_GET_PTR((ipart), (ipart)->index_table_offset))
#define L_HISAM_SHMEM_HASH_TABLE(ipart)   ((UI16_T *)L_CVRT_GET_PTR((ipart), (ipart)->hash_table_offset))
#define L_HISAM_SHMEM_RLINK_TABLE(ipart)  ((UI16_T *)L_CVRT_GET_PTR((ipart), (ipart)->rlink_table_offset))

#define L_HISAM_SHMEM_IS_STATIC_RECORD(ridx)   *(BOOL_T *)(L_HISAM_SHMEM_RECORD(ridx)-1)

/* PURPOSE: Copy RECORD(record_index) to rec
 * INPUT:   ridx  - index of record_table
 * OUTPUT:  rec   - buffer pointer to store a record
 * RETUEN:  rec
 * NOTES:
 */
#define L_HISAM_SHMEM_GET_RECORD(rec,ridx)     memmove(rec,L_HISAM_SHMEM_RECORD(ridx),desc->record_length)

/* PURPOSE: Copy rec to RECORD(record_index)
 * INPUT:   rec   - buffer pointer to store a record
 *          ridx  - index of record_table
 * OUTPUT:  none
 * RETUEN:  none
 * NOTES:
 */
#define L_HISAM_SHMEM_SET_RECORD(rec,ridx,is_static)       \
{  char *p = (char *)L_HISAM_SHMEM_RECORD(ridx);                   \
   memmove(p,rec,desc->record_length);                     \
   *--p = is_static;                                       \
}



/* DATA TYPE DECLARATIONS
 */

typedef struct
{
   UI16_T   ridx_1st;         /* 1st ridx of record-list */
   UI16_T   ridx_last;        /* last ridx of record-list */
   UI16_T   nbr;
} Index_T;



/* working area, for internal used */

typedef struct
{
   L_HISAM_KeyDef_T        key_def;
   L_HISAM_Integer_Type_T  integer_type[L_HISAM_MAX_FIELD_NUMBER_OF_A_KEY];
   Index_T                 *index_table;           /* point to index table       */
   UI16_T                  *hash_table;            /* point to hash table        */
   UI16_T                  *rlink_table;           /* point to Record Link table */
   UI16_T                  key_length;
   UI16_T                  curr_index_nbr;
   UI8_T                   in_key[L_HISAM_MAX_KEY_LEN];
   UI8_T                   work_key[L_HISAM_MAX_KEY_LEN];
} Ipart_T;

typedef struct
{
   UI32_T   key_number;             /* the number of key instance, internal used */
   UI8_T    *record_table;          /* point to a record table      */
   Ipart_T  ipart [L_HISAM_MAX_KEY_NUMBER];
   /* partition descriptor for record table, reference L_PT.H
    * This descriptor is used for record_table's allocation & MyFree
    */
   L_PT_Descriptor_T  record_part_desc;
} Work_T;


/*---------------------------------
 * Types for shared memory version
 *---------------------------------
 */
typedef struct
{
   L_HISAM_KeyDef_T        key_def;
   L_HISAM_Integer_Type_T  integer_type[L_HISAM_MAX_FIELD_NUMBER_OF_A_KEY];
   I32_T                   index_table_offset;           /* point to index table       */
   I32_T                   hash_table_offset;            /* point to hash table        */
   I32_T                   rlink_table_offset;           /* point to Record Link table */
   UI16_T                  key_length;
   UI16_T                  curr_index_nbr;
   UI8_T                   in_key[L_HISAM_MAX_KEY_LEN];
   UI8_T                   work_key[L_HISAM_MAX_KEY_LEN];
} ShMem_Ipart_T;

typedef struct
{
   UI32_T   key_number;             /* the number of key instance, internal used */
   I32_T   record_table_offset ;    /* point to a record table      */
   ShMem_Ipart_T  ipart [L_HISAM_MAX_KEY_NUMBER];
   /* partition descriptor for record table, reference L_PT.H
    * This descriptor is used for record_table's allocation & MyFree
    */
   L_PT_ShMem_Descriptor_T  record_part_desc;
} ShMem_Work_T;



/* LOCAL SUBPROGRAM DECLARATIONS
 */
static void    L_HISAM_Adjust (L_HISAM_Desc_T *desc, Ipart_T *ipart);
static UI32_T  L_HISAM_AllocRecord (L_HISAM_Desc_T *desc);

static void    L_HISAM_DeleteAllIndex (L_HISAM_Desc_T *desc, UI8_T *rec, UI32_T ridx);
static void    L_HISAM_DeleteAllSecondaryIndex (L_HISAM_Desc_T *desc, UI8_T *rec, UI32_T ridx);
static void    L_HISAM_DeleteIndex (L_HISAM_Desc_T *desc, Ipart_T *ipart, UI8_T *rec,
                                  UI32_T record_index);

static void    L_HISAM_FreeRecord (L_HISAM_Desc_T *desc, UI32_T record_index);

static UI32_T  L_HISAM_HashIndex   (L_HISAM_Desc_T *desc, UI32_T  key_len, UI8_T *key);
static BOOL_T  L_HISAM_Hash_Locate (L_HISAM_Desc_T *desc, Ipart_T *ipart,  UI32_T *ret_hidx);
static void    L_HISAM_Hash_Delete (L_HISAM_Desc_T *desc, Ipart_T *ipart);
static void    L_HISAM_Hash_Insert (L_HISAM_Desc_T *desc, Ipart_T *ipart,  UI32_T insert_ridx);

static void    L_HISAM_InitTables (L_HISAM_Desc_T *desc);
static void    L_HISAM_InsertAllIndex (L_HISAM_Desc_T *desc, UI8_T *rec, UI32_T ridx);
static void    L_HISAM_InsertAllSecondaryIndex (L_HISAM_Desc_T *desc, UI8_T *rec, UI32_T ridx);
static void    L_HISAM_InsertIndex (L_HISAM_Desc_T *desc, Ipart_T *ipart, UI8_T *rec,
                                  UI32_T record_index);

static UI32_T  L_HISAM_KeyLength (L_HISAM_KeyDef_T  *key_def);
static void    L_HISAM_KeyOfRecord (L_HISAM_KeyDef_T *key_def, UI8_T *rec, UI8_T *key);

static void    L_HISAM_Lshift_Index_Entry (Ipart_T *ipart, UI32_T iidx);
static BOOL_T  L_HISAM_Locate (L_HISAM_Desc_T *desc, Ipart_T *ipart, UI32_T *iidx,
                             UI32_T *record_index, UI32_T *pre_record_index);

static BOOL_T  L_HISAM_QuickLocate (L_HISAM_Desc_T *desc, Ipart_T *ipart, UI32_T *iidx,
                             UI32_T *record_index, UI32_T *pre_record_index);

static int KeyCompare(Ipart_T *ipart);

#ifdef HISAM_DEBUG
static void    L_HISAM_Dump (L_HISAM_Desc_T *desc, UI32_T kidx);
static void    L_HISAM_Dump1 (L_HISAM_Desc_T *desc, UI32_T kidx);
#endif


/*---------------------------------
 * Local functions for shared memory version
 *---------------------------------
 */
static void    L_HISAM_ShMem_Adjust (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart);
static UI32_T  L_HISAM_ShMem_AllocRecord (L_HISAM_ShMem_Desc_T *desc);

static void    L_HISAM_ShMem_DeleteAllIndex (L_HISAM_ShMem_Desc_T *desc, UI8_T *rec, UI32_T ridx);
static void    L_HISAM_ShMem_DeleteAllSecondaryIndex (L_HISAM_ShMem_Desc_T *desc, UI8_T *rec, UI32_T ridx);
static void    L_HISAM_ShMem_DeleteIndex (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart, UI8_T *rec,
                                  UI32_T record_index);

static void    L_HISAM_ShMem_FreeRecord (L_HISAM_ShMem_Desc_T *desc, UI32_T record_index);

static BOOL_T  L_HISAM_ShMem_Hash_Locate (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart,  UI32_T *ret_hidx);
static void    L_HISAM_ShMem_Hash_Delete (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart);
static void    L_HISAM_ShMem_Hash_Insert (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart,  UI32_T insert_ridx);

static void    L_HISAM_ShMem_InitTables (L_HISAM_ShMem_Desc_T *desc);
static void    L_HISAM_ShMem_InsertAllIndex (L_HISAM_ShMem_Desc_T *desc, UI8_T *rec, UI32_T ridx);
static void    L_HISAM_ShMem_InsertAllSecondaryIndex (L_HISAM_ShMem_Desc_T *desc, UI8_T *rec, UI32_T ridx);
static void    L_HISAM_ShMem_InsertIndex (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart, UI8_T *rec,
                                  UI32_T record_index);

static void    L_HISAM_ShMem_Lshift_Index_Entry (ShMem_Ipart_T *ipart, UI32_T iidx);
static BOOL_T  L_HISAM_ShMem_Locate (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart, UI32_T *iidx,
                             UI32_T *record_index, UI32_T *pre_record_index);

static BOOL_T  L_HISAM_ShMem_QuickLocate (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart, UI32_T *iidx,
                             UI32_T *record_index, UI32_T *pre_record_index);


/* STATIC VARIABLE DECLARATIONS
 */
const static Index_T  empty_index_element = { NULL_INDEX, NULL_INDEX, 0};


/* EXPORTED SUBPROGRAM BODIES
 */

/* PURPOSE: This function create a HISAM structure.
 * INPUT:   desc  -- Description of data storage (Database) capacity,
 *                   include hash-table size (total_hash_nbr),
 *                   index-table size (total_index_nbr),
 *                   record-table size (total_record_nbr),
 *                   record-size (record_length),
 *                   key-size (key_len).
 *          number_of_key      -- number of key definition. That is, number of
 *                                element in key_def_table
 *          key_def_table      -- array of key definition
 *          integer_type_table -- 2-dimension array for definition of integer type
 *                                of each field in a key. The first index of the
 *                                array represents the index to a key.
 *                                The second index represents the index to a field
 *                                within a key. For example:
 *                                integer_type_table[0][0] defines the integer
 *                                type of the first field in the first key
 *                                (namely the key defined in key_def_table[0])
 *
 * OUTPUT:  desc  -- the handle of a new data storage (Database) after creation.
 * RETUEN:  TRUE/FALSE
 * NOTES:   1. Before use the HISAM structure, MUST call this function first. and
 *             then use the desc to call other relate functions.
 *          2. Before creation, caller MUST fill data storage capability,
 *             L_HISAM_Create() uses this to allocate storage space, and the desciptor (desc)
 *             becomes a handle of new object (HISAM database).
 *          3. The 1st key( define in key_def_table[0]) is the primary key which must identify
 *             each record uniquely.
 *          4. Suggestion: We suggest to avoid to use non-unique-key, it may make the
 *             application more compicate.
 *          5. Caller must specify correct integer type in integer_type_table
 *             or the data might be sorted incorrectly
 *
 */
BOOL_T L_HISAM_CreateV2(L_HISAM_Desc_T *desc, UI32_T number_of_key,
                        const L_HISAM_KeyDef_T *key_def_table,
                        const L_HISAM_Integer_Type_T integer_type_table[][L_HISAM_MAX_FIELD_NUMBER_OF_A_KEY])

{
   Work_T   *wk;
   Ipart_T  *ipart;
   UI32_T   i, j;

   /* desc->total_hash_nbr shall not equal to HASHIDX_NEXT_SPAN.
    * Adjust to HASHIDX_NEXT_SPAN+1 if it is set as HASHIDX_NEXT_SPAN.
    */
   if(desc->total_hash_nbr == HASHIDX_NEXT_SPAN)
      desc->total_hash_nbr++;

   /* Allocate working area */
   if ( (wk = desc->wk = MyMalloc(sizeof(Work_T) )) == 0 )
      return FALSE;
   WK->key_number = number_of_key;

   /* Allocate record_table */
   if ( (wk->record_table  = MyMalloc( desc->total_record_nbr*PHY_REC_LEN ))==0)
   {
      MyFree (desc->wk);
      return FALSE;
   }

   /* Allocate index_table, ipart->hash_table, ecordLinkTable */
   for (i=0, ipart=wk->ipart; i< WK->key_number; i++, ipart++)
   {
      ipart->hash_table  = MyMalloc( desc->total_hash_nbr  * sizeof(UI16_T)  );
      ipart->index_table = MyMalloc( desc->total_index_nbr * sizeof(Index_T) );
      ipart->rlink_table = MyMalloc( desc->total_record_nbr* sizeof(UI16_T)  );

      /* if allocation fail then return false */
      if (  ipart->hash_table == 0 || ipart->index_table == 0 || ipart->rlink_table == 0 )
         goto Allocate_Fail;
      ipart->key_def = key_def_table[i];
      if ( integer_type_table != 0 )
          memcpy (ipart->integer_type, integer_type_table[i], sizeof(ipart->integer_type) );
      else
          memset (ipart->integer_type, L_HISAM_NOT_INTEGER, sizeof(ipart->integer_type));
      ipart->key_length = (UI16_T) L_HISAM_KeyLength (&ipart->key_def);
   } /* End of for */


   /* Set the initial value */
   L_HISAM_InitTables (desc);
   return TRUE;

Allocate_Fail:
   MyFree (desc->wk);
   MyFree (wk->record_table);
   for (ipart=wk->ipart, j=0; j<=i; j++, ipart++)
   {
      MyFree (ipart->hash_table);
      MyFree (ipart->index_table);
      MyFree (ipart->rlink_table);
   }
   return FALSE;

} /* L_HISAM_CreateV2 */



/* PURPOSE: Destroy the exited HISAM structure, including the storage.
 * INPUT:   desc  -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETUEN:  TRUE  -- Successfully release resource of HISAM.
 *          FALSE -- desc not a handle of HISAM database.
 * NOTES:
 */
BOOL_T L_HISAM_Destroy (L_HISAM_Desc_T *desc)
{
   /* LOCAL VARIABLES */
   Work_T   *wk = desc->wk;
   Ipart_T  *ipart;
   UI32_T   i;

   /* BODY */
   if ( desc == NULL )
      return FALSE;

   MyFree (wk->record_table);

   for (ipart=wk->ipart, i=0; i< WK->key_number; i++, ipart++)
   {
      MyFree (ipart->hash_table);
      MyFree (ipart->index_table);
      MyFree (ipart->rlink_table);
   }

   MyFree (desc->wk);
   return TRUE;
} /* L_HISAM_Destroy */


/* PURPOSE: Set a record, added or replaced, into HISAM database.
 * INPUT:   desc        -- the handle of a HISAM database.
 *          rec         -- record to be saved.
 *          is_static   -- TRUE, The setting record is a static record.
 *                         FALSE, do not replace if record already existed.
 * OUTPUT:  desc->last_reference_record_index
 * RETUEN:  L_HISAM_INSERT      -- succeed to insert a new record.
 *          L_HISAM_REPLACE     -- succeed to replace a existed record.
 *          L_HISAM_CONFLICT    -- fail to set because record alrady existed.
 *          L_HISAM_NO_BUFFER   -- fail to set because no enough buffer
 * NOTES:   1. The HISAM algorithm use 1st key(primary key) to determine whether the
 *             input record is a new record or not.
 *          2. The static record will replace the existed record.
 *          3. The dynamic record will replace exited dynamic record but can't replace
 *             the existed static record.
 */
UI32_T L_HISAM_SetRecord (L_HISAM_Desc_T *desc, UI8_T *rec, BOOL_T is_static)
{
   /* LOCAL VARIABLES
    */
   Ipart_T  *ipart;
   UI32_T iidx, record_index, pre_record_index;

   /* BODY
    */

   /* Use the primary key to determine wether is a new record */
   ipart = &WK->ipart[0];     /* ipart of primary key */

   L_HISAM_KeyOfRecord (&ipart->key_def, rec, ipart->in_key);


   if ( L_HISAM_QuickLocate ( desc,  ipart, &iidx, &record_index, &pre_record_index) )
   {
      /* if found && is_static ) replace it */
      if ( is_static || !IS_STATIC_RECORD(record_index) )
      {
         L_HISAM_DeleteAllSecondaryIndex (desc, RECORD(record_index), record_index); /* Lewis: keep the primary key for the case of port move or is_static changed */
         L_HISAM_InsertAllSecondaryIndex (desc, rec, record_index);
         SET_RECORD(rec, record_index, is_static);
         desc->last_reference_record_index = record_index;
         return L_HISAM_REPLACE;
      }
      else  /* dynamic record can't replace static record */
         return L_HISAM_CONFLICT;
   } /* End of if L_HISAM_QuickLocate() */

   /* record not found, Insert it */

   /* allocate a buffer */
   if ( (record_index = L_HISAM_AllocRecord (desc)) == NULL_INDEX )
      return L_HISAM_NO_BUFFER;

   /* copy rec to allocated buffer */
   SET_RECORD(rec, record_index, is_static);

   L_HISAM_InsertAllIndex (desc, rec, record_index);
   desc->last_reference_record_index=record_index;
   return L_HISAM_INSERT;
} /* L_HISAM_SetRecord */





/* PURPOSE: Delete all records of HISAM database.
 * INPUT:   desc    -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETUEN:  none
 */
void L_HISAM_DeleteAllRecord (L_HISAM_Desc_T *desc)
{
   L_HISAM_InitTables (desc);
} /* L_HISAM_DeleteAllRecord */


/* PURPOSE: Delete a specified record
 * INPUT:   desc    -- the handle of a HISAM database.
 *          key     -- key of primary key
 * OUTPUT:  none
 * RETUEN:  none
 */
BOOL_T L_HISAM_DeleteRecord (L_HISAM_Desc_T *desc, UI8_T *key)
{
   UI32_T iidx, record_index, pre_record_index;
   Ipart_T  *ipart = IPART(0);      /* primary key */

   /* Use primary key to locate the specified record */

   memcpy (ipart->in_key, key, ipart->key_length);

   if ( !L_HISAM_QuickLocate ( desc, ipart, &iidx, &record_index, &pre_record_index) )
      return FALSE;

   /* Record is found */
   L_HISAM_DeleteAllIndex (desc, RECORD(record_index), record_index);

   L_HISAM_FreeRecord(desc, record_index);

   desc->last_reference_record_index = record_index;

   return TRUE;
} /* L_HISAM_DeleteRecord */

/* PURPOSE: Get record address by record index
 * INPUT:   ridx:   record index
 * OUTPUT:  none
 * RETUEN:  record address or null if ridx out of range
 */
void *L_HISAM_GetRecordAddressByIndex(L_HISAM_Desc_T *desc, UI32_T ridx)
{
    if ( ridx >= desc->total_record_nbr )
        return 0;
    return RECORD(ridx);
} /* L_HISAM_GetNextRecord */



/* PURPOSE: Get next record of given key
 * INPUT:   desc    -- the handle of a HISAM database.
 *          kidx    -- key index
 *          key     -- the record-key to be searched.
 * OUTPUT:  rec     -- the buffer to save retrieved record.
 *          desc->last_reference_record_index
 * RETUEN:  none
 */
BOOL_T L_HISAM_GetNextRecord (L_HISAM_Desc_T *desc, UI32_T kidx, UI8_T *key, UI8_T *rec)
{
   UI32_T   iidx, pre_record_index;
   UI32_T   record_index;
   Ipart_T  *ipart = IPART(kidx);


   /* BODY */
   if ( kidx >= WK->key_number)
      return FALSE;

   /* locate position of current key */
   memcpy (ipart->in_key, key, ipart->key_length);
   if ( L_HISAM_QuickLocate( desc, ipart, &iidx, &record_index, &pre_record_index) )
   {
      record_index = ipart->rlink_table [record_index];
   }
   else
   {
      if ( pre_record_index == NULL_INDEX  )
         record_index = ipart->index_table[iidx].ridx_1st;
      else
         record_index = ipart->rlink_table [pre_record_index];
   }

   if ( record_index != NULL_INDEX )
   {
      GET_RECORD(rec, record_index);
      desc->last_reference_record_index = record_index;
      return TRUE;
   }
   return FALSE;
} /* L_HISAM_GetNextRecord */


/* PURPOSE: Get a record of given key
 * INPUT:   desc    -- the handle of a HISAM database.
 *          kidx    -- key index
 *          key     -- the searched record-key.
 * OUTPUT:  rec     -- the buffer to save retrieved record.
 *          desc->last_reference_record_index
 * RETUEN:  TRUE/FALSE
 */
BOOL_T L_HISAM_GetRecord (L_HISAM_Desc_T *desc, UI32_T kidx, UI8_T *key,  UI8_T *rec)
{
   UI32_T iidx, pre_record_index;
   UI32_T record_index;
   Ipart_T *ipart = IPART(kidx);

   if ( kidx >= WK->key_number)
      return FALSE;

   memcpy (ipart->in_key, key, ipart->key_length);
   /* locate position of current key */
   if ( L_HISAM_QuickLocate( desc, ipart, &iidx, &record_index, &pre_record_index) )
   {
      GET_RECORD(rec, record_index);
      desc->last_reference_record_index = record_index;
      return TRUE;
   }
   return FALSE;
} /* L_HISAM_GetNextRecord */



UI32_T L_HISAM_GetFreeBufNo (L_HISAM_Desc_T *desc)
{
    return WK->record_part_desc.free_no;
}



/* PURPOSE: Search next element(record) from HISAM-structure
 * INPUT:   desc     -- HISAM descriptor to operation
 *          kidx     -- key index
 *          key      -- according the "key" to search the next element.
 *                      if element is NULL(0) then search from 1st element.
 *          call_back-- The callback function to 'view' the element.
 *                      The return value of call_back:
 *                         L_HISAM_SEARCH_BREAK: tell this function to break searching
 *                         L_HISAM_SEARCH_CONTINUE: tell this function continue searching
 *                         L_HISAM_SEARCH_SKIP: tell this function to skip counting
 *          count    -- limited element count to be searched
 * OUTPUT:  key -- return the key back so that the user can do furthur search from the next record
 * RETURN:  L_HISAM_SEARCH_BREAK: Searching is broken by (*fun)
 *          L_HISAM_SEARCH_END_OF_LIST: stop search because end of list
 *          L_HISAM_SEARCH_COMPLETED: stop search because reach the limited count
 *          L_HISAM_SEARCH_INVALID_KIDX: stop search because invalid key index
 * NOTE:    1. Caller must take care the mutual exculusion situation.
 *          2. Each time call back to call_back will modify desc->last_reference_record_index
 */
UI32_T L_HISAM_Search (L_HISAM_Desc_T *desc, UI32_T kidx, UI8_T *key,
                     UI32_T (*call_back) (char*rec), UI32_T count)
{
   return  __L_HISAM_Search_WithCookie (desc, kidx, key, (UI32_T (*)(char*, UI32_T)) call_back, count, 0, FALSE);
} /* L_HISAM_Search */



UI32_T __L_HISAM_Search_WithCookie (
   L_HISAM_Desc_T *desc, UI32_T kidx, UI8_T *key,
   UI32_T (*call_back) (char*rec, UI32_T cookie),UI32_T count, UI32_T cookie, BOOL_T get_next)
{
   Ipart_T  *ipart = IPART(kidx);
   UI32_T iidx, pre_record_index;
   UI32_T record_index;
   UI32_T i=0;
   UI32_T callback_result;

   if ( kidx >= WK->key_number)
      return L_HISAM_SEARCH_INVALID_KIDX;

   /* locate position of current key */
   memcpy (ipart->in_key, key, ipart->key_length);
   if ( !L_HISAM_QuickLocate( desc, ipart, &iidx, &record_index, &pre_record_index) )
   {
      /* Get the next record index */
      if ( pre_record_index == NULL_INDEX  )
         record_index = ipart->index_table[iidx].ridx_1st;
      else
         record_index = ipart->rlink_table [pre_record_index];
   }
   else if (get_next == TRUE)
   {
        record_index = ipart->rlink_table [record_index];
   }

   while (TRUE)
   {
      if ( record_index == NULL_INDEX )
         return L_HISAM_SEARCH_END_OF_LIST;
      /* End of if */

      desc->last_reference_record_index = record_index;
      callback_result = call_back ( (char *)RECORD(record_index), cookie );

      if (callback_result == L_HISAM_SEARCH_BREAK )
         return L_HISAM_SEARCH_BREAK;

      if(callback_result != L_HISAM_SEARCH_SKIP) /* whether to skip counting of this record */
      {
          if ( ++i == count )
          {
	      record_index = ipart->rlink_table[record_index];
	      if ( record_index == NULL_INDEX )
                 return L_HISAM_SEARCH_END_OF_LIST;
	      else
	      {
	         L_HISAM_KeyOfRecord(&ipart->key_def,RECORD(record_index), key); /* return the key back so that the user can do furthur search from the next record */
                 return L_HISAM_SEARCH_COMPLETED;
	      }
	  }
      }

      record_index = ipart->rlink_table[record_index];
   } /* End of while */

} /* L_HISAM_Search_WithCookie */

/*---------------------------------
 * Exported functions for shared memory version
 *---------------------------------
 */
/* PURPOSE: Calculate the required size of the working buffer for a HISAM structure
 * INPUT:   desc  -- Description of data storage (Database) capacity,
 *                   include hash-table size (total_hash_nbr),
 *                   index-table size (total_index_nbr),
 *                   record-table size (total_record_nbr),
 *                   record-size (record_length),
 *          number_of_key      -- number of key definition. That is, number of
 *                                element in key_def_table
 * OUTPUT:  none
 * RETUEN:  The required size
 */
UI32_T L_HISAM_ShMem_GetWorkingBufferRequiredSize(L_HISAM_ShMem_Desc_T *desc, UI32_T number_of_key)
{
    return (sizeof(ShMem_Work_T) +
            L_HISAM_SHMEM_GET_RECORD_TABLE_SIZE(desc) +
            (number_of_key) * (
                L_HISAM_SHMEM_GET_HASH_TABLE_SIZE(desc) +
                L_HISAM_SHMEM_GET_INDEX_TABLE_SIZE(desc) +
                L_HISAM_SHMEM_GET_RLINK_TABLE_SIZE(desc)));
}

/* PURPOSE: This function create a HISAM structure.
 * INPUT:   desc  -- Description of data storage (Database) capacity,
 *                   include hash-table size (total_hash_nbr),
 *                   index-table size (total_index_nbr),
 *                   record-table size (total_record_nbr),
 *                   record-size (record_length),
 *                   key-size (key_len).
 *          number_of_key      -- number of key definition. That is, number of
 *                                element in key_def_table
 *          key_def_table      -- array of key definition
 *          integer_type_table -- 2-dimension array for definition of integer type
 *                                of each field in a key. The first index of the
 *                                array represents the index to a key.
 *                                The second index represents the index to a field
 *                                within a key. For example:
 *                                integer_type_table[0][0] defines the integer
 *                                type of the first field in the first key
 *                                (namely the key defined in key_def_table[0])
 *
 * OUTPUT:  desc  -- the handle of a new data storage (Database) after creation.
 * RETUEN:  TRUE/FALSE
 * NOTES:   1. Before use the HISAM structure, MUST call this function first. and
 *             then use the desc to call other relate functions.
 *          2. Before creation, caller MUST fill data storage capability,
 *             L_HISAM_ShMem_Create() uses this to allocate storage space, and the desciptor (desc)
 *             becomes a handle of new object (HISAM database).
 *          3. The 1st key( define in key_def_table[0]) is the primary key which must identify
 *             each record uniquely.
 *          4. Suggestion: We suggest to avoid to use non-unique-key, it may make the
 *             application more compicate.
 *          5. Caller must specify correct integer type in integer_type_table
 *             or the data might be sorted incorrectly
 *
 */
BOOL_T L_HISAM_ShMem_CreateV2(L_HISAM_ShMem_Desc_T *desc,
                        UI32_T number_of_key,
                        const L_HISAM_KeyDef_T *key_def_table,
                        const L_HISAM_Integer_Type_T integer_type_table[][L_HISAM_MAX_FIELD_NUMBER_OF_A_KEY])

{
   ShMem_Work_T   *wk;
   ShMem_Ipart_T  *ipart;
   UI32_T   i;
   UI8_T    *hash_table_buffer, *index_table_buffer, *rlink_table_buffer;

   /* Allocate working area */
   if ( desc->wk_offset == 0 )
      return FALSE;
   wk = L_HISAM_SHMEM_WK(desc);
   wk->key_number = number_of_key;

   /* Allocate record_table */
   wk->record_table_offset = L_HISAM_SHMEM_GET_RECORD_BUFFER_OFFSET_TO_WK();

   /* Allocate index_table, ipart->hash_table, ecordLinkTable */

   hash_table_buffer = L_CVRT_GET_PTR(wk, L_HISAM_SHMEM_GET_HASH_TABLE_BUFFER_OFFSET_TO_WK(desc));
   index_table_buffer = L_CVRT_GET_PTR(wk, L_HISAM_SHMEM_GET_INDEX_TABLE_BUFFER_OFFSET_TO_WK(desc, number_of_key));
   rlink_table_buffer = L_CVRT_GET_PTR(wk, L_HISAM_SHMEM_GET_RLINK_TABLE_BUFFER_OFFSET_TO_WK(desc, number_of_key));

   for (i=0, ipart=wk->ipart; i< wk->key_number; i++, ipart++)
   {
      ipart->hash_table_offset = L_CVRT_GET_OFFSET(ipart, hash_table_buffer);
      ipart->index_table_offset = L_CVRT_GET_OFFSET(ipart, index_table_buffer);
      ipart->rlink_table_offset = L_CVRT_GET_OFFSET(ipart, rlink_table_buffer);

      hash_table_buffer += L_HISAM_SHMEM_GET_HASH_TABLE_SIZE(desc);
      index_table_buffer += L_HISAM_SHMEM_GET_INDEX_TABLE_SIZE(desc);
      rlink_table_buffer += L_HISAM_SHMEM_GET_RLINK_TABLE_SIZE(desc);

      ipart->key_def = key_def_table[i];
      if ( integer_type_table != 0 )
          memcpy (ipart->integer_type, integer_type_table[i], sizeof(ipart->integer_type) );
      else
          memset (ipart->integer_type, L_HISAM_NOT_INTEGER, sizeof(ipart->integer_type));
      ipart->key_length = (UI16_T) L_HISAM_KeyLength (&ipart->key_def);
   } /* End of for */


   /* Set the initial value */
   L_HISAM_ShMem_InitTables (desc);
   return TRUE;

} /* L_HISAM_ShMem_CreateV2 */



/* PURPOSE: Destroy the exited HISAM structure, including the storage.
 * INPUT:   desc  -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETUEN:  TRUE  -- Successfully release resource of HISAM.
 *          FALSE -- desc not a handle of HISAM database.
 * NOTES:
 */
BOOL_T L_HISAM_ShMem_Destroy (L_HISAM_ShMem_Desc_T *desc)
{
   return TRUE;
} /* L_HISAM_ShMem_Destroy */


/* PURPOSE: Set a record, added or replaced, into HISAM database.
 * INPUT:   desc        -- the handle of a HISAM database.
 *          rec         -- record to be saved.
 *          is_static   -- TRUE, The setting record is a static record.
 *                         FALSE, do not replace if record already existed.
 * OUTPUT:  desc->last_reference_record_index
 * RETUEN:  L_HISAM_INSERT      -- succeed to insert a new record.
 *          L_HISAM_REPLACE     -- succeed to replace a existed record.
 *          L_HISAM_CONFLICT    -- fail to set because record alrady existed.
 *          L_HISAM_NO_BUFFER   -- fail to set because no enough buffer
 * NOTES:   1. The HISAM algorithm use 1st key(primary key) to determine whether the
 *             input record is a new record or not.
 *          2. The static record will replace the existed record.
 *          3. The dynamic record will replace exited dynamic record but can't replace
 *             the existed static record.
 */
UI32_T L_HISAM_ShMem_SetRecord (L_HISAM_ShMem_Desc_T *desc, UI8_T *rec, BOOL_T is_static)
{
   /* LOCAL VARIABLES
    */
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   ShMem_Ipart_T  *ipart;
   UI8_T  *record_table = L_HISAM_SHMEM_RECORD_TABLE(wk);
   UI32_T iidx, record_index, pre_record_index;

   /* BODY
    */

   /* Use the primary key to determine wether is a new record */
   ipart = wk->ipart;     /* ipart of primary key */

   L_HISAM_KeyOfRecord (&ipart->key_def, rec, ipart->in_key);


   if ( L_HISAM_ShMem_QuickLocate ( desc,  ipart, &iidx, &record_index, &pre_record_index) )
   {
      /* if found && is_static ) replace it */
      if ( is_static || !L_HISAM_SHMEM_IS_STATIC_RECORD(record_index) )
      {
         L_HISAM_ShMem_DeleteAllSecondaryIndex (desc, L_HISAM_SHMEM_RECORD(record_index), record_index); /* Lewis: keep the primary key for the case of port move or is_static changed */
         L_HISAM_ShMem_InsertAllSecondaryIndex (desc, rec, record_index);
         L_HISAM_SHMEM_SET_RECORD(rec, record_index, is_static);
         desc->last_reference_record_index = record_index;
         return L_HISAM_REPLACE;
      }
      else  /* dynamic record can't replace static record */
         return L_HISAM_CONFLICT;
   } /* End of if L_HISAM_ShMem_QuickLocate() */

   /* record not found, Insert it */

   /* allocate a buffer */
   if ( (record_index = L_HISAM_ShMem_AllocRecord (desc)) == NULL_INDEX )
      return L_HISAM_NO_BUFFER;

   /* copy rec to allocated buffer */
   L_HISAM_SHMEM_SET_RECORD(rec, record_index, is_static);

   L_HISAM_ShMem_InsertAllIndex (desc, rec, record_index);
   desc->last_reference_record_index=record_index;
   return L_HISAM_INSERT;
} /* L_HISAM_ShMem_SetRecord */





/* PURPOSE: Delete all records of HISAM database.
 * INPUT:   desc    -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETUEN:  none
 */
void L_HISAM_ShMem_DeleteAllRecord (L_HISAM_ShMem_Desc_T *desc)
{
   L_HISAM_ShMem_InitTables (desc);
} /* L_HISAM_ShMem_DeleteAllRecord */


/* PURPOSE: Delete a specified record
 * INPUT:   desc    -- the handle of a HISAM database.
 *          key     -- key of primary key
 * OUTPUT:  none
 * RETUEN:  none
 */
BOOL_T L_HISAM_ShMem_DeleteRecord (L_HISAM_ShMem_Desc_T *desc, UI8_T *key)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   UI8_T  *record_table = L_HISAM_SHMEM_RECORD_TABLE(wk);
   UI32_T iidx, record_index, pre_record_index;
   ShMem_Ipart_T  *ipart = wk->ipart;      /* primary key */

   /* Use primary key to locate the specified record */

   memcpy (ipart->in_key, key, ipart->key_length);

   if ( !L_HISAM_ShMem_QuickLocate ( desc, ipart, &iidx, &record_index, &pre_record_index) )
      return FALSE;

   /* Record is found */
   L_HISAM_ShMem_DeleteAllIndex (desc, L_HISAM_SHMEM_RECORD(record_index), record_index);

   L_HISAM_ShMem_FreeRecord(desc, record_index);

   desc->last_reference_record_index = record_index;

   return TRUE;
} /* L_HISAM_ShMem_DeleteRecord */

/* PURPOSE: Get record address by record index
 * INPUT:   ridx:   record index
 * OUTPUT:  none
 * RETUEN:  record address or null if ridx out of range
 */
void *L_HISAM_ShMem_GetRecordAddressByIndex(L_HISAM_ShMem_Desc_T *desc, UI32_T ridx)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   UI8_T  *record_table = L_HISAM_SHMEM_RECORD_TABLE(wk);

   if ( ridx >= desc->total_record_nbr )
      return 0;
   return L_HISAM_SHMEM_RECORD(ridx);
} /* L_HISAM_ShMem_GetRecordAddressByIndex */



/* PURPOSE: Get next record of given key
 * INPUT:   desc    -- the handle of a HISAM database.
 *          kidx    -- key index
 *          key     -- the record-key to be searched.
 * OUTPUT:  rec     -- the buffer to save retrieved record.
 *          desc->last_reference_record_index
 * RETUEN:  none
 */
BOOL_T L_HISAM_ShMem_GetNextRecord (L_HISAM_ShMem_Desc_T *desc, UI32_T kidx, UI8_T *key, UI8_T *rec)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   UI8_T  *record_table = L_HISAM_SHMEM_RECORD_TABLE(wk);
   UI32_T   iidx, pre_record_index;
   UI32_T   record_index;
   ShMem_Ipart_T  *ipart = &(wk->ipart[kidx]);


   /* BODY */
   if ( kidx >= wk->key_number)
      return FALSE;

   /* locate position of current key */
   memcpy (ipart->in_key, key, ipart->key_length);
   if ( L_HISAM_ShMem_QuickLocate( desc, ipart, &iidx, &record_index, &pre_record_index) )
   {
      record_index = L_HISAM_SHMEM_RLINK_TABLE(ipart)[record_index];
   }
   else
   {
      if ( pre_record_index == NULL_INDEX  )
         record_index = L_HISAM_SHMEM_INDEX_TABLE(ipart)[iidx].ridx_1st;
      else
         record_index = L_HISAM_SHMEM_RLINK_TABLE(ipart)[pre_record_index];
   }

   if ( record_index != NULL_INDEX )
   {
      L_HISAM_SHMEM_GET_RECORD(rec, record_index);
      desc->last_reference_record_index = record_index;
      return TRUE;
   }
   return FALSE;
} /* L_HISAM_ShMem_GetNextRecord */


/* PURPOSE: Get a record of given key
 * INPUT:   desc    -- the handle of a HISAM database.
 *          kidx    -- key index
 *          key     -- the searched record-key.
 * OUTPUT:  rec     -- the buffer to save retrieved record.
 *          desc->last_reference_record_index
 * RETUEN:  TRUE/FALSE
 */
BOOL_T L_HISAM_ShMem_GetRecord (L_HISAM_ShMem_Desc_T *desc, UI32_T kidx, UI8_T *key,  UI8_T *rec)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   UI8_T  *record_table = L_HISAM_SHMEM_RECORD_TABLE(wk);
   UI32_T iidx, pre_record_index;
   UI32_T record_index;
   ShMem_Ipart_T *ipart = &wk->ipart[kidx];

   if ( kidx >= wk->key_number)
      return FALSE;

   memcpy (ipart->in_key, key, ipart->key_length);
   /* locate position of current key */
   if ( L_HISAM_ShMem_QuickLocate( desc, ipart, &iidx, &record_index, &pre_record_index) )
   {
      L_HISAM_SHMEM_GET_RECORD(rec, record_index);
      desc->last_reference_record_index = record_index;
      return TRUE;
   }
   return FALSE;
} /* L_HISAM_ShMem_GetRecord */



UI32_T L_HISAM_ShMem_GetFreeBufNo (L_HISAM_ShMem_Desc_T *desc)
{
    return L_HISAM_SHMEM_WK(desc)->record_part_desc.free_no;
}



/* PURPOSE: Search next element(record) from HISAM-structure
 * INPUT:   desc     -- HISAM descriptor to operation
 *          kidx     -- key index
 *          key      -- according the "key" to search the next element.
 *                      if element is NULL(0) then search from 1st element.
 *          call_back-- The callback function to 'view' the element.
 *                      The return value of call_back:
 *                         L_HISAM_SEARCH_BREAK: tell this function to break searching
 *                         L_HISAM_SEARCH_CONTINUE: tell this function continue searching
 *                         L_HISAM_SEARCH_SKIP: tell this function to skip counting
 *          count    -- limited element count to be searched
 * OUTPUT:  key -- return the key back so that the user can do furthur search from the next record
 * RETURN:  L_HISAM_SEARCH_BREAK: Searching is broken by (*fun)
 *          L_HISAM_SEARCH_END_OF_LIST: stop search because end of list
 *          L_HISAM_SEARCH_COMPLETED: stop search because reach the limited count
 *          L_HISAM_SEARCH_INVALID_KIDX: stop search because invalid key index
 * NOTE:    1. Caller must take care the mutual exculusion situation.
 *          2. Each time call back to call_back will modify desc->last_reference_record_index
 */
UI32_T L_HISAM_ShMem_Search (L_HISAM_ShMem_Desc_T *desc, UI32_T kidx, UI8_T *key,
                     UI32_T (*call_back) (char*rec), UI32_T count)
{
   return  L_HISAM_ShMem_Search_WithCookie (desc, kidx, key, (UI32_T (*)(char*, UI32_T)) call_back, count, 0);
} /* L_HISAM_ShMem_Search */



UI32_T __L_HISAM_ShMem_Search_WithCookie (
   L_HISAM_ShMem_Desc_T *desc, UI32_T kidx, UI8_T *key,
   UI32_T (*call_back) (char*rec, UI32_T cookie),UI32_T count, UI32_T cookie, BOOL_T get_next)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   UI8_T  *record_table = L_HISAM_SHMEM_RECORD_TABLE(wk);
   ShMem_Ipart_T  *ipart = &wk->ipart[kidx];
   UI32_T iidx, pre_record_index;
   UI32_T record_index;
   UI32_T i=0;
   UI32_T callback_result;

   if ( kidx >= wk->key_number)
      return L_HISAM_SEARCH_INVALID_KIDX;

   /* locate position of current key */
   memcpy (ipart->in_key, key, ipart->key_length);
   if ( !L_HISAM_ShMem_QuickLocate( desc, ipart, &iidx, &record_index, &pre_record_index) )
   {
      /* Get the next record index */
      if ( pre_record_index == NULL_INDEX  )
         record_index = L_HISAM_SHMEM_INDEX_TABLE(ipart)[iidx].ridx_1st;
      else
         record_index = L_HISAM_SHMEM_RLINK_TABLE(ipart)[pre_record_index];
   }
   else if (get_next == TRUE)
   {
      record_index = L_HISAM_SHMEM_RLINK_TABLE(ipart)[record_index];
   }

   while (TRUE)
   {
      if ( record_index == NULL_INDEX )
         return L_HISAM_SEARCH_END_OF_LIST;
      /* End of if */

      desc->last_reference_record_index = record_index;
      callback_result = call_back ( (char *)L_HISAM_SHMEM_RECORD(record_index), cookie );

      if (callback_result == L_HISAM_SEARCH_BREAK )
         return L_HISAM_SEARCH_BREAK;

      if(callback_result != L_HISAM_SEARCH_SKIP) /* whether to skip counting of this record */
      {
          if ( ++i == count )
          {
	      record_index = L_HISAM_SHMEM_RLINK_TABLE(ipart)[record_index];
	      if ( record_index == NULL_INDEX )
                 return L_HISAM_SEARCH_END_OF_LIST;
	      else
	      {
	         L_HISAM_KeyOfRecord(&ipart->key_def,L_HISAM_SHMEM_RECORD(record_index), key); /* return the key back so that the user can do furthur search from the next record */
                 return L_HISAM_SEARCH_COMPLETED;
	      }
	  }
      }

      record_index = L_HISAM_SHMEM_RLINK_TABLE(ipart)[record_index];
   } /* End of while */

} /* L_HISAM_ShMem_Search_WithCookie */




/* LOCAL FUNCTIONS BODY
 */

/* PURPOSE: Adjust the HISAM structure to form a balance tree.
 * INPUT:   desc    -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETUEN:  none
 * NOTE:    none
 */

static void L_HISAM_Adjust (L_HISAM_Desc_T *desc, Ipart_T *ipart)
{
   /* LOCAL VARIABLES */
   Index_T  *ip;
   UI32_T i;
   UI32_T iidx, sum;
   UI32_T nn;              /* average record number for a list */

   /* BODY */

   for (ip = ipart->index_table, sum=0, i=0; i<ipart->curr_index_nbr; i++, ip++)
      sum += ip->nbr;

   nn = (sum+desc->total_index_nbr-1) /desc->total_index_nbr;
   for ( ip = ipart->index_table, iidx=0;  iidx <= (UI32_T)(desc->total_index_nbr-2); iidx++, ip++)
   {
      if ( ip->nbr == 0  && ip[1].nbr==0 )
         break;

      while ( Abs(ip->nbr,nn) > desc->N1 )
      {
         if ( ip->nbr < nn )
         {
            if ( ip[1].nbr == 0 )
               return;
            if ( (UI16_T)(ip->nbr+ip[1].nbr) <= nn )
            {  /* join next list */
               ip->nbr += ip[1].nbr;

               ip->ridx_last = ip[1].ridx_last;
               L_HISAM_Lshift_Index_Entry(ipart, (UI32_T)(iidx+1));
               continue;
            }
            else
            {  /* move some record from next list */
               UI32_T j;
               UI32_T move_no, idx;

               move_no = nn - ip->nbr;
               idx = ip[1].ridx_1st;
               for ( j=0; j<(move_no-1); j++)
                  idx = ipart->rlink_table[idx];
               ip->ridx_last = (UI16_T)idx;
               ip[1].ridx_1st = ipart->rlink_table[idx];
               ip->nbr = (UI16_T)nn;
               ip[1].nbr -= (UI16_T)move_no;
               break;
            }
         } /* END if ( ip->nbr < nn )  */
         else  /* ( ip->nbr > nn ) */
         {  /* split list */
            UI32_T j;
            UI32_T idx;

            idx = ip->ridx_1st;
            for ( j=0; j<(nn-1); j++)
               idx = ipart->rlink_table[idx];

            ip[1].ridx_1st = ipart->rlink_table[idx];
            if ( ip[1].ridx_last == NULL_INDEX )
            {
               ip[1].ridx_last = ip->ridx_last;
               ipart->curr_index_nbr = (UI16_T)(iidx +2);
            }

            ip[1].nbr += (ip->nbr-(UI16_T)nn);
            ip->nbr = (UI16_T)nn;
            ip->ridx_last = (UI16_T)idx;
            break;
         } /* END (ip->nbr>nn ) */
      } /* END while */
   } /* END for */
} /* L_HISAM_Adjust */




/* PURPOSE: Allocate a record buffer.
 * INPUT:   desc    -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETURN:  if found return record index, else return NULL_INDEX
 * NOTE:
 */

static UI32_T L_HISAM_AllocRecord (L_HISAM_Desc_T *desc)
{
   char *p;

   if ( (p=L_PT_Allocate (&WK->record_part_desc)) == NULL )
      return NULL_INDEX;

   return (p-(char*)(WK->record_table)) / PHY_REC_LEN;
}



static void  L_HISAM_DeleteAllIndex (L_HISAM_Desc_T *desc, UI8_T *rec, UI32_T ridx)
{
   UI32_T i;

   for (i=0; i<WK->key_number; i++)
      L_HISAM_DeleteIndex (desc, IPART(i), rec, ridx);
}



static void    L_HISAM_DeleteAllSecondaryIndex (L_HISAM_Desc_T *desc, UI8_T *rec, UI32_T ridx)
{
   UI32_T i;

   for (i=1; i<WK->key_number; i++)
      L_HISAM_DeleteIndex (desc, IPART(i), rec, ridx);
}

static void L_HISAM_DeleteIndex (L_HISAM_Desc_T *desc, Ipart_T *ipart, UI8_T *rec, UI32_T record_index)
{
   UI32_T ridx, iidx, pre_record_index;
   Index_T  *ip;


   /* generate key */
   L_HISAM_KeyOfRecord (&ipart->key_def, rec, ipart->in_key);

   L_HISAM_Hash_Delete (desc, ipart);

   /* find record */
   if ( !L_HISAM_Locate ( desc, ipart, &iidx, &ridx, &pre_record_index) )
      return;

   ip = &ipart->index_table[iidx];
   ip->nbr--;

   if ( pre_record_index != NULL_INDEX)
      ipart->rlink_table [pre_record_index] = ipart->rlink_table [record_index];

   if ( record_index == ip->ridx_1st )
   {  /* del 1st */
      ip->ridx_1st = ipart->rlink_table [record_index];
      if ( ip->nbr == 0 )
         L_HISAM_Lshift_Index_Entry (ipart, iidx);
   }
   else if ( record_index ==  ip->ridx_last )
      ip->ridx_last = (UI16_T) pre_record_index;
      /* END if */

} /* L_HISAM_DeleteIndex */




/* PURPOSE: Free a record buffer.
 * INPUT:   desc           -- the handle of a HISAM database.
 *          record_index   -- index of a record
 * OUTPUT:  none
 * RETURN:  none
 * NOTE:
 */
static void L_HISAM_FreeRecord (L_HISAM_Desc_T *desc, UI32_T record_index)
{
   L_PT_Free ( &WK->record_part_desc, RECORD(record_index)-1 );
}




/* PURPOSE: Caculate the hash index based on given key.
 * INPUT:   desc    -- the handle of a HISAM database.
 *          key     -- record-key to be hashed.
 * OUTPUT:  None
 * RETURN:  hash index
 * NOTE:    none
 */
static UI32_T L_HISAM_HashIndex(L_HISAM_Desc_T *desc, UI32_T key_len, UI8_T *key)
{
   /* LOCAL VARIABLES
    */
   UI32_T   len;
   union
   {
      UI32_T ui32;
      UI8_T  ui8[4];
   } key_value;


   /* BODY
    */
   key_value.ui32 = 0;

   for ( len =0; len< key_len; len++ )
   {
      key_value.ui8[ len & 3 ] += key[len];
   }

   return (UI32_T) (key_value.ui32 % desc->total_hash_nbr);

} /* L_HISAM_HashIndex */





/* PURPOSE: Search the ipart->hash_table to find the entry of given key.
 * INPUT:   desc     -- the handle of a HISAM database.
 *          key      -- searching key
 * OUTPUT:  ret_hidx -- the matching hash entry's index
 * RETURN:  return TRUE if found else return FALSE
 * NOTE:    none
 */
static BOOL_T L_HISAM_Hash_Locate(L_HISAM_Desc_T *desc, Ipart_T  *ipart, UI32_T *ret_hidx)
{

   UI32_T   i, hidx, ridx;

   hidx = L_HISAM_HashIndex( desc, ipart->key_length, ipart->in_key);

   for ( i=0; i< desc->hash_depth;  i++ )
   {
      if ( (ridx=ipart->hash_table[hidx]) != NULL_INDEX )
      {
         L_HISAM_KeyOfRecord (&ipart->key_def,  RECORD(ridx), ipart->work_key);

         if (KeyCompare(ipart) == 0 )
         {
            *ret_hidx = hidx;
            return TRUE;
         }
      } /* END if */
      hidx += HASHIDX_NEXT_SPAN; /* hash next function */
      while (hidx >= desc->total_hash_nbr) /* wrap hidx */
         hidx -= desc->total_hash_nbr;
   }
   return FALSE;

} /* L_HISAM_Hash_Locate */





/* PURPOSE: Delete a record index (insert_ridx) from ipart->hash_table.
 * INPUT:   desc    -- the handle of a HISAM database.
 *          key     -- searching key
 * OUTPUT:  none
 * RETURN:  none
 * NOTE:
 */
static void L_HISAM_Hash_Delete (L_HISAM_Desc_T *desc, Ipart_T *ipart)
{
   UI32_T   hidx;

   if ( L_HISAM_Hash_Locate (desc, ipart, &hidx) )
         ipart->hash_table[hidx] = NULL_INDEX ;  /* delete it */

} /* L_HISAM_Hash_Delete */



/* PURPOSE: Insert a record index (insert_ridx) into ipart->hash_table.
 * INPUT:   desc        -- the handle of a HISAM database.
 *          key         -- searching key
 *          insert_ridx -- the record inddex to be insert.
 * OUTPUT:  none
 * RETURN:  none
 * NOTE:    If the same "hash_key" has no room to insert, current implemetation
 *          is just ignore this insertion.
 */
static void    L_HISAM_Hash_Insert (L_HISAM_Desc_T *desc, Ipart_T *ipart, UI32_T insert_ridx)
{
   UI16_T   i;
   UI32_T   hidx;
   UI32_T   empty_hidx = NULL_INDEX;
   UI32_T   record_index;

   hidx = L_HISAM_HashIndex( desc, ipart->key_length, ipart->in_key);

   for ( i=0; i< desc->hash_depth;  i++ )
   {
      if ( (record_index=ipart->hash_table[hidx]) == insert_ridx)
         return;

      if ( record_index == NULL_INDEX  &&  empty_hidx == NULL_INDEX)
         empty_hidx = hidx;

      hidx += HASHIDX_NEXT_SPAN; /* hash next function */
      while (hidx >= desc->total_hash_nbr) /* wrap hidx */
         hidx -= desc->total_hash_nbr;
   }
   if ( empty_hidx != NULL_INDEX )
      ipart->hash_table[empty_hidx] = (UI16_T)insert_ridx;
} /* L_HISAM_Hash_Insert */



/* FUNCTION NAME : L_HISAM_InitTables
 * PURPOSE: Initialize the indexing and hashing structure of HISAM database.
 * INPUT:   desc    -- the handle of a HISAM database.
 * OUTPUT:  none.
 * RETUEN:  None.
 * NOTE:
 */
static void L_HISAM_InitTables(L_HISAM_Desc_T *desc)
{
   Work_T   *wk = desc->wk;
   Ipart_T  *ipart;
   UI32_T   kidx;

   /* Set the initial value */
   for (kidx=0, ipart=IPART(0); kidx< WK->key_number; kidx++, ipart++)
   {
      UI32_T   i;
      Index_T  *ip;
      UI16_T   *hp;
      for (ip=ipart->index_table,i =0; i < desc->total_index_nbr; i++, ip++)
         *ip = empty_index_element;

      for (hp=ipart->hash_table, i =0; i < desc->total_hash_nbr; i++, hp++)
         *hp = NULL_INDEX;
      ipart->curr_index_nbr = 0;
   }

   /* use L-Pt to manage record table */
   wk->record_part_desc.buffer = (char*)wk->record_table;
   wk->record_part_desc.buffer_len = PHY_REC_LEN * desc->total_record_nbr ;
   wk->record_part_desc.partition_len = PHY_REC_LEN;
   L_PT_Create ( &wk->record_part_desc);

} /* L_HISAM_InitTables */



static void    L_HISAM_InsertAllIndex (L_HISAM_Desc_T *desc, UI8_T *rec, UI32_T ridx)
{
   UI32_T i;

   for (i=0; i<WK->key_number; i++)
      L_HISAM_InsertIndex (desc, IPART(i), rec, ridx);
}



static void    L_HISAM_InsertAllSecondaryIndex (L_HISAM_Desc_T *desc, UI8_T *rec, UI32_T ridx)
{
   UI32_T i;

   for (i=1; i<WK->key_number; i++)
      L_HISAM_InsertIndex (desc, IPART(i), rec, ridx);
}

static void L_HISAM_InsertIndex (L_HISAM_Desc_T *desc, Ipart_T *ipart, UI8_T *rec, UI32_T record_index)
{
   UI32_T   iidx, ridx, pre_record_index;
   Index_T  *ip;

   /* BODY
    */

   /* generate key */
   L_HISAM_KeyOfRecord (&ipart->key_def, rec, ipart->in_key);

   /* find record */
   if ( L_HISAM_QuickLocate ( desc, ipart, &iidx, &ridx, &pre_record_index) ) /* Lewis: would never be TRUE in current HISAM code */
   {
      SYSFUN_Debug_Printf("\r\nL_HISAM_InsertIndex: shall not insert a record which has the same key with an existed record in database.");
      return;
   }

   /* record not found */
   L_HISAM_Hash_Insert(desc, ipart, record_index);

   if ( ipart->curr_index_nbr == 0 )
   {
      ipart->rlink_table[record_index] = NULL_INDEX;
      ipart->curr_index_nbr = 1;
   }

   ip = &(ipart->index_table[iidx]);
   if ( pre_record_index == NULL_INDEX)
   {  /* insert at 1st */
      ipart->rlink_table[record_index] = ip->ridx_1st;

      ip->ridx_1st = (UI16_T)record_index;
      if ( ip->nbr == 0 )
         ip->ridx_last = (UI16_T)record_index;
   } /* END if */
   else
   {
      if ( (ipart->rlink_table[record_index] = ipart->rlink_table[pre_record_index])
            == NULL_INDEX)  /* Lewis: check whether to insert after the last valid node in index table */
      {
         if ( (ip->nbr >= desc->N2) &&  (iidx < (desc->total_index_nbr-1)) )
         {
            ip++; ipart->curr_index_nbr++;
            ip->ridx_last = ip->ridx_1st = (UI16_T)record_index;
         }
         else
         {
            ip->ridx_last = (UI16_T)record_index;
          }
      }

      if ( ipart->rlink_table[record_index] == ip->ridx_1st )
         ip->ridx_1st = (UI16_T)record_index;

      ipart->rlink_table [pre_record_index]= (UI16_T)record_index;
   }
   if ( ++ip->nbr > desc->N2 )
   {
      L_HISAM_Adjust(desc, ipart);
   }

} /* L_HISAM_InsertIndex */






static UI32_T  L_HISAM_KeyLength (L_HISAM_KeyDef_T  *key_def)
{
   UI32_T i, len;

   for ( len=0, i=0; i< key_def->field_number; i++)
      len += key_def->length[i];

   return len;
}







static void L_HISAM_KeyOfRecord (L_HISAM_KeyDef_T *key_def, UI8_T *rec, UI8_T *key)
{
   UI32_T i;

   for ( i=0; i< key_def->field_number; i++)
   {
      memcpy (key, rec+key_def->offset[i], key_def->length[i]);
      key += key_def->length[i];
   }
} /* L_HISAM_KeyOfRecord */




/* PURPOSE: Shift index_table's entries that after iidx (not include iidx entry)
 *          to left.
 * INPUT:   desc    -- the handle of a HISAM database.
 *          iidx    -- iidx+1 is the starting entry to be shift
 * OUTPUT:  none
 * RETUEN:  none
 * NOTE:    none
 */
static void L_HISAM_Lshift_Index_Entry (Ipart_T  *ipart, UI32_T iidx)
{
   /* LOCAL VARIABLES */
   Index_T  *ip;
   int      copy_len;

   /* BODY */
   copy_len = (ipart->curr_index_nbr - iidx -1 ) * sizeof(Index_T);
   ip = ipart->index_table + iidx;
   if ( copy_len > 0 )
      memmove ( ip,  ip+1, copy_len);

   ipart->index_table[ipart->curr_index_nbr-1] = empty_index_element;
   ipart->curr_index_nbr --;
} /* L_HISAM_Lshift_Index_Entry */



static BOOL_T L_HISAM_Locate (L_HISAM_Desc_T *desc, Ipart_T *ipart, UI32_T *iidx,
                    UI32_T *record_index, UI32_T *pre_record_index)
{
   /* LOCAL VARIABLES */
   int r,m, l;
   UI32_T   i;
   Index_T  *ip;
   int      ret;

   /* BODY
    */
   ip = ipart->index_table;
   m=l=0, r=ipart->curr_index_nbr-1;


   /* compare 1st inode */
   if ( r == -1 )
   {
      *iidx = 0;
      *pre_record_index = NULL_INDEX;
      return FALSE;
   }


   L_HISAM_KeyOfRecord (&ipart->key_def,  RECORD(ip->ridx_last), ipart->work_key);

   if ((ret = KeyCompare(ipart)) <=0 )
      goto Search_RecordList;

   /* compare last */
   L_HISAM_KeyOfRecord (&ipart->key_def,  RECORD(ip[r].ridx_last), ipart->work_key);

   if (  (ret = KeyCompare(ipart)) ==0 )
   {
      m=r;  goto Search_RecordList;
   }

   if ( ret > 0 )
   {
      *iidx = r;
      *pre_record_index = ip[r].ridx_last;
      return FALSE;
   }

   /* binary search */
   while (1)
   {
      m = (l+r) >> 1;
      if ( l==m )
      {
         m=r;  goto Search_RecordList;
      }


      L_HISAM_KeyOfRecord (&ipart->key_def,  RECORD(ip[m].ridx_last), ipart->work_key);

      ret = KeyCompare(ipart) ;

      if ( ret == 0 )
         goto Search_RecordList;

      if ( ret > 0 )
         l=m;
      else
         r=m;
   } /* while */


Search_RecordList:
   ip+=m;
   *iidx = m;
   if ( m == 0 )
      *pre_record_index= NULL_INDEX;
   else
      *pre_record_index= ip[-1].ridx_last;

   for ( *record_index=ip->ridx_1st, i=0; i< ip->nbr; i++)
   {
      L_HISAM_KeyOfRecord (&ipart->key_def,  RECORD(*record_index), ipart->work_key);
      ret = KeyCompare(ipart);

      if ( ret == 0 )
         return TRUE;
      if ( ret < 0 )
         return FALSE;
      *pre_record_index = *record_index;
      *record_index = ipart->rlink_table[*record_index];
   }
   /* should not goto here */
   return FALSE;
} /* L_HISAM_Locate */


/* PURPOSE: Locate a record of a given key
 * INPUT:   desc    -- the handle of a HISAM database.
 *          key     -- record-key be searched.
 * OUTPUT:  iidx,
 *          record_index,
 *          pre_record_index
 * RETUEN:  If record is found then return TRUE, else return FALSE
 * NOTE:    When return TRUE, iidx and pre_record_index is not defined.
 */
static BOOL_T L_HISAM_QuickLocate (L_HISAM_Desc_T *desc, Ipart_T *ipart, UI32_T *iidx,
                    UI32_T *record_index, UI32_T *pre_record_index)
{
   UI32_T hidx;

   /* Serch the Hash-Table first */
   if ( L_HISAM_Hash_Locate (desc, ipart, &hidx) )
   {
      *record_index = ipart->hash_table[hidx] ;
      return  TRUE;
   }

   return L_HISAM_Locate (desc, ipart, iidx, record_index, pre_record_index);
} /* L_HISAM_QuickLocate */


static int KeyCompare(Ipart_T *ipart)
{
   char *p1, *p2;
   int  key_len, i, ret;
   union
   {
      UI16_T   s;
      UI32_T   l;
      UI8_T    b[4];
   }v1, v2;


   p1= (char *)ipart->in_key;
   p2= (char *)ipart->work_key;

   for ( i=0; i< ipart->key_def.field_number; i++ )
   {
      key_len = ipart->key_def.length[i];
      switch ( ipart->integer_type[i] )
      {
            case L_HISAM_2BYTE_INTEGER:
         v1.b[0] = p1[0];
         v1.b[1] = p1[1];
         v2.b[0] = p2[0];
         v2.b[1] = p2[1];
         ret = v1.s - v2.s;
         break;

            case L_HISAM_4BYTE_INTEGER:
         v1.b[0] = p1[0];
         v1.b[1] = p1[1];
         v1.b[2] = p1[2];
         v1.b[3] = p1[3];

         v2.b[0] = p2[0];
         v2.b[1] = p2[1];
         v2.b[2] = p2[2];
         v2.b[3] = p2[3];
         ret = v1.l - v2.l;
         break;

      default:
         ret = memcmp (p1, p2, key_len);
         break;

      } /* end of switch */
      if ( ret < 0 )
            return -1;

        if ( ret > 0 )
            return 1;

      p1+=key_len, p2+=key_len;
      continue;
   } /* end of for */
   return 0;
} /* end of KeyCompare */



/* PURPOSE: Adjust the HISAM structure to form a balance tree.
 * INPUT:   desc    -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETUEN:  none
 * NOTE:    none
 */

static void L_HISAM_ShMem_Adjust (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart)
{
   /* LOCAL VARIABLES */
   Index_T  *ip;
   UI32_T i;
   UI32_T iidx, sum;
   UI32_T nn;              /* average record number for a list */

   /* BODY */

   for (ip = L_HISAM_SHMEM_INDEX_TABLE(ipart), sum=0, i=0; i<ipart->curr_index_nbr; i++, ip++)
      sum += ip->nbr;

   nn = (sum+desc->total_index_nbr-1) /desc->total_index_nbr;
   for ( ip = L_HISAM_SHMEM_INDEX_TABLE(ipart), iidx=0;  iidx <= (UI32_T)(desc->total_index_nbr-2); iidx++, ip++)
   {
      if ( ip->nbr == 0  && ip[1].nbr==0 )
         break;

      while ( Abs(ip->nbr,nn) > desc->N1 )
      {
         if ( ip->nbr < nn )
         {
            if ( ip[1].nbr == 0 )
               return;
            if ( (UI16_T)(ip->nbr+ip[1].nbr) <= nn )
            {  /* join next list */
               ip->nbr += ip[1].nbr;

               ip->ridx_last = ip[1].ridx_last;
               L_HISAM_ShMem_Lshift_Index_Entry(ipart, (UI32_T)(iidx+1));
               continue;
            }
            else
            {  /* move some record from next list */
               UI32_T j;
               UI32_T move_no, idx;

               move_no = nn - ip->nbr;
               idx = ip[1].ridx_1st;
               for ( j=0; j<(move_no-1); j++)
                  idx = L_HISAM_SHMEM_RLINK_TABLE(ipart)[idx];
               ip->ridx_last = (UI16_T)idx;
               ip[1].ridx_1st = L_HISAM_SHMEM_RLINK_TABLE(ipart)[idx];
               ip->nbr = (UI16_T)nn;
               ip[1].nbr -= (UI16_T)move_no;
               break;
            }
         } /* END if ( ip->nbr < nn )  */
         else  /* ( ip->nbr > nn ) */
         {  /* split list */
            UI32_T j;
            UI32_T idx;

            idx = ip->ridx_1st;
            for ( j=0; j<(nn-1); j++)
               idx = L_HISAM_SHMEM_RLINK_TABLE(ipart)[idx];

            ip[1].ridx_1st = L_HISAM_SHMEM_RLINK_TABLE(ipart)[idx];
            if ( ip[1].ridx_last == NULL_INDEX )
            {
               ip[1].ridx_last = ip->ridx_last;
               ipart->curr_index_nbr = (UI16_T)(iidx +2);
            }

            ip[1].nbr += (ip->nbr-(UI16_T)nn);
            ip->nbr = (UI16_T)nn;
            ip->ridx_last = (UI16_T)idx;
            break;
         } /* END (ip->nbr>nn ) */
      } /* END while */
   } /* END for */
} /* L_HISAM_ShMem_Adjust */




/* PURPOSE: Allocate a record buffer.
 * INPUT:   desc    -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETURN:  if found return record index, else return NULL_INDEX
 * NOTE:
 */

static UI32_T L_HISAM_ShMem_AllocRecord (L_HISAM_ShMem_Desc_T *desc)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   char *p;

   if ( (p=L_PT_ShMem_Allocate (&wk->record_part_desc)) == NULL )
      return NULL_INDEX;

   return (L_CVRT_GET_OFFSET(L_HISAM_SHMEM_RECORD_TABLE(wk), p)) / PHY_REC_LEN;
}



static void  L_HISAM_ShMem_DeleteAllIndex (L_HISAM_ShMem_Desc_T *desc, UI8_T *rec, UI32_T ridx)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   UI32_T i;

   for (i=0; i<wk->key_number; i++)
      L_HISAM_ShMem_DeleteIndex (desc, &(wk->ipart[i]), rec, ridx);
}



static void    L_HISAM_ShMem_DeleteAllSecondaryIndex (L_HISAM_ShMem_Desc_T *desc, UI8_T *rec, UI32_T ridx)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   UI32_T i;

   for (i=1; i<wk->key_number; i++)
      L_HISAM_ShMem_DeleteIndex (desc, &(wk->ipart[i]), rec, ridx);
}

static void L_HISAM_ShMem_DeleteIndex (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart, UI8_T *rec, UI32_T record_index)
{
   UI32_T ridx, iidx, pre_record_index;
   Index_T  *ip;


   /* generate key */
   L_HISAM_KeyOfRecord (&ipart->key_def, rec, ipart->in_key);

   L_HISAM_ShMem_Hash_Delete (desc, ipart);

   /* find record */
   if ( !L_HISAM_ShMem_Locate ( desc, ipart, &iidx, &ridx, &pre_record_index) )
      return;

   ip = L_HISAM_SHMEM_INDEX_TABLE(ipart) + iidx;
   ip->nbr--;

   if ( pre_record_index != NULL_INDEX)
      L_HISAM_SHMEM_RLINK_TABLE(ipart)[pre_record_index] = L_HISAM_SHMEM_RLINK_TABLE(ipart)[record_index];

   if ( record_index == ip->ridx_1st )
   {  /* del 1st */
      ip->ridx_1st = L_HISAM_SHMEM_RLINK_TABLE(ipart)[record_index];
      if ( ip->nbr == 0 )
         L_HISAM_ShMem_Lshift_Index_Entry (ipart, iidx);
   }
   else if ( record_index ==  ip->ridx_last )
      ip->ridx_last = (UI16_T) pre_record_index;
      /* END if */

} /* L_HISAM_ShMem_DeleteIndex */




/* PURPOSE: Free a record buffer.
 * INPUT:   desc           -- the handle of a HISAM database.
 *          record_index   -- index of a record
 * OUTPUT:  none
 * RETURN:  none
 * NOTE:
 */
static void L_HISAM_ShMem_FreeRecord (L_HISAM_ShMem_Desc_T *desc, UI32_T record_index)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   UI8_T *record_table = L_HISAM_SHMEM_RECORD_TABLE(wk);
   L_PT_ShMem_Free ( &wk->record_part_desc, L_HISAM_SHMEM_RECORD(record_index)-1 );
}


/* PURPOSE: Search the ipart->hash_table to find the entry of given key.
 * INPUT:   desc     -- the handle of a HISAM database.
 *          key      -- searching key
 * OUTPUT:  ret_hidx -- the matching hash entry's index
 * RETURN:  return TRUE if found else return FALSE
 * NOTE:    none
 */
static BOOL_T L_HISAM_ShMem_Hash_Locate(L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T  *ipart, UI32_T *ret_hidx)
{
   ShMem_Work_T *wk = L_HISAM_SHMEM_WK(desc);
   UI8_T    *record_table = L_HISAM_SHMEM_RECORD_TABLE(wk);
   UI32_T   i, hidx, ridx;

   hidx = L_HISAM_HashIndex((L_HISAM_Desc_T *)desc, ipart->key_length, ipart->in_key);

   for ( i=0; i< desc->hash_depth;  i++ )
   {
      if ( (ridx=L_HISAM_SHMEM_HASH_TABLE(ipart)[hidx]) != NULL_INDEX )
      {
         L_HISAM_KeyOfRecord (&ipart->key_def, L_HISAM_SHMEM_RECORD(ridx), ipart->work_key);

         if (KeyCompare((Ipart_T *)ipart) == 0 )
         {
            *ret_hidx = hidx;
            return TRUE;
         }
      } /* END if */
      hidx += HASHIDX_NEXT_SPAN; /* hash next function */
      while (hidx >= desc->total_hash_nbr) /* wrap hidx */
         hidx -= desc->total_hash_nbr;
   }
   return FALSE;

} /* L_HISAM_ShMem_Hash_Locate */





/* PURPOSE: Delete a record index (insert_ridx) from ipart->hash_table.
 * INPUT:   desc    -- the handle of a HISAM database.
 *          key     -- searching key
 * OUTPUT:  none
 * RETURN:  none
 * NOTE:
 */
static void L_HISAM_ShMem_Hash_Delete (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart)
{
   UI32_T   hidx;

   if ( L_HISAM_ShMem_Hash_Locate (desc, ipart, &hidx) )
         L_HISAM_SHMEM_HASH_TABLE(ipart)[hidx] = NULL_INDEX ;  /* delete it */

} /* L_HISAM_ShMem_Hash_Delete */



/* PURPOSE: Insert a record index (insert_ridx) into ipart->hash_table.
 * INPUT:   desc        -- the handle of a HISAM database.
 *          key         -- searching key
 *          insert_ridx -- the record inddex to be insert.
 * OUTPUT:  none
 * RETURN:  none
 * NOTE:    If the same "hash_key" has no room to insert, current implemetation
 *          is just ignore this insertion.
 */
static void    L_HISAM_ShMem_Hash_Insert (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart, UI32_T insert_ridx)
{
   UI16_T   i;
   UI32_T   hidx;
   UI32_T   empty_hidx = NULL_INDEX;
   UI32_T   record_index;

   hidx = L_HISAM_HashIndex((L_HISAM_Desc_T *)desc, ipart->key_length, ipart->in_key);

   for ( i=0; i< desc->hash_depth;  i++ )
   {
      if ( (record_index=L_HISAM_SHMEM_HASH_TABLE(ipart)[hidx]) == insert_ridx)
         return;

      if ( record_index == NULL_INDEX  &&  empty_hidx == NULL_INDEX)
         empty_hidx = hidx;

      hidx += HASHIDX_NEXT_SPAN; /* hash next function */
      while (hidx >= desc->total_hash_nbr) /* wrap hidx */
         hidx -= desc->total_hash_nbr;
   }
   if ( empty_hidx != NULL_INDEX )
      L_HISAM_SHMEM_HASH_TABLE(ipart)[empty_hidx] = (UI16_T)insert_ridx;
} /* L_HISAM_ShMem_Hash_Insert */



/* FUNCTION NAME : L_HISAM_ShMem_InitTables
 * PURPOSE: Initialize the indexing and hashing structure of HISAM database.
 * INPUT:   desc    -- the handle of a HISAM database.
 * OUTPUT:  none.
 * RETUEN:  None.
 * NOTE:
 */
static void L_HISAM_ShMem_InitTables(L_HISAM_ShMem_Desc_T *desc)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   ShMem_Ipart_T  *ipart;
   UI32_T   kidx;

   /* Set the initial value */
   for (kidx=0, ipart=wk->ipart; kidx< wk->key_number; kidx++, ipart++)
   {
      UI32_T   i;
      Index_T  *ip;
      UI16_T   *hp;
      for (ip=L_HISAM_SHMEM_INDEX_TABLE(ipart),i =0; i < desc->total_index_nbr; i++, ip++)
         *ip = empty_index_element;

      for (hp=L_HISAM_SHMEM_HASH_TABLE(ipart), i =0; i < desc->total_hash_nbr; i++, hp++)
         *hp = NULL_INDEX;
      ipart->curr_index_nbr = 0;
   }

   /* use L-Pt to manage record table */
   wk->record_part_desc.buffer_offset = L_CVRT_GET_OFFSET(&wk->record_part_desc, L_HISAM_SHMEM_RECORD_TABLE(wk));
   wk->record_part_desc.buffer_len = L_HISAM_SHMEM_GET_RECORD_TABLE_SIZE(desc);
   wk->record_part_desc.partition_len = PHY_REC_LEN;
   L_PT_ShMem_Create(&wk->record_part_desc);

} /* L_HISAM_ShMem_InitTables */



static void    L_HISAM_ShMem_InsertAllIndex (L_HISAM_ShMem_Desc_T *desc, UI8_T *rec, UI32_T ridx)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   UI32_T i;

   for (i=0; i<wk->key_number; i++)
      L_HISAM_ShMem_InsertIndex (desc, &(wk->ipart[i]), rec, ridx);
}



static void    L_HISAM_ShMem_InsertAllSecondaryIndex (L_HISAM_ShMem_Desc_T *desc, UI8_T *rec, UI32_T ridx)
{
   ShMem_Work_T   *wk = L_HISAM_SHMEM_WK(desc);
   UI32_T i;

   for (i=1; i<wk->key_number; i++)
      L_HISAM_ShMem_InsertIndex (desc, &(wk->ipart[i]), rec, ridx);
}

static void L_HISAM_ShMem_InsertIndex (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart, UI8_T *rec, UI32_T record_index)
{
   UI32_T   iidx, ridx, pre_record_index;
   Index_T  *ip;

   /* BODY
    */

   /* generate key */
   L_HISAM_KeyOfRecord (&ipart->key_def, rec, ipart->in_key);

   /* find record */
   if ( L_HISAM_ShMem_QuickLocate ( desc, ipart, &iidx, &ridx, &pre_record_index) ) /* Lewis: would never be TRUE in current HISAM code */
   {
      SYSFUN_Debug_Printf("\r\nL_HISAM_InsertIndex: shall not insert a record which has the same key with an existed record in database.");
      return;
   }

   /* record not found */
   L_HISAM_ShMem_Hash_Insert(desc, ipart, record_index);

   if ( ipart->curr_index_nbr == 0 )
   {
      L_HISAM_SHMEM_RLINK_TABLE(ipart)[record_index] = NULL_INDEX;
      ipart->curr_index_nbr = 1;
   }

   ip = &(L_HISAM_SHMEM_INDEX_TABLE(ipart)[iidx]);
   if ( pre_record_index == NULL_INDEX)
   {  /* insert at 1st */
      L_HISAM_SHMEM_RLINK_TABLE(ipart)[record_index] = ip->ridx_1st;

      ip->ridx_1st = (UI16_T)record_index;
      if ( ip->nbr == 0 )
         ip->ridx_last = (UI16_T)record_index;
   } /* END if */
   else
   {
      if ( (L_HISAM_SHMEM_RLINK_TABLE(ipart)[record_index] = L_HISAM_SHMEM_RLINK_TABLE(ipart)[pre_record_index])
            == NULL_INDEX)  /* Lewis: check whether to insert after the last valid node in index table */
      {
         if ( (ip->nbr >= desc->N2) &&  (iidx < (desc->total_index_nbr-1)) )
         {
            ip++; ipart->curr_index_nbr++;
            ip->ridx_last = ip->ridx_1st = (UI16_T)record_index;
         }
         else
         {
            ip->ridx_last = (UI16_T)record_index;
          }
      }

      if ( L_HISAM_SHMEM_RLINK_TABLE(ipart)[record_index] == ip->ridx_1st )
         ip->ridx_1st = (UI16_T)record_index;

      L_HISAM_SHMEM_RLINK_TABLE(ipart)[pre_record_index]= (UI16_T)record_index;
   }
   if ( ++ip->nbr > desc->N2 )
   {
      L_HISAM_ShMem_Adjust(desc, ipart);
   }

} /* L_HISAM_ShMem_InsertIndex */


/* PURPOSE: Shift index_table's entries that after iidx (not include iidx entry)
 *          to left.
 * INPUT:   desc    -- the handle of a HISAM database.
 *          iidx    -- iidx+1 is the starting entry to be shift
 * OUTPUT:  none
 * RETUEN:  none
 * NOTE:    none
 */
static void L_HISAM_ShMem_Lshift_Index_Entry (ShMem_Ipart_T  *ipart, UI32_T iidx)
{
   /* LOCAL VARIABLES */
   Index_T  *ip;
   int      copy_len;

   /* BODY */
   copy_len = (ipart->curr_index_nbr - iidx -1 ) * sizeof(Index_T);
   ip = L_HISAM_SHMEM_INDEX_TABLE(ipart) + iidx;
   if ( copy_len > 0 )
      memmove ( ip,  ip+1, copy_len);

   L_HISAM_SHMEM_INDEX_TABLE(ipart)[ipart->curr_index_nbr-1] = empty_index_element;
   ipart->curr_index_nbr --;
} /* L_HISAM_ShMem_Lshift_Index_Entry */



static BOOL_T L_HISAM_ShMem_Locate (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart, UI32_T *iidx,
                    UI32_T *record_index, UI32_T *pre_record_index)
{
   /* LOCAL VARIABLES */
   ShMem_Work_T *wk = L_HISAM_SHMEM_WK(desc);
   UI8_T    *record_table = L_HISAM_SHMEM_RECORD_TABLE(wk);
   int      r,m, l;
   UI32_T   i;
   Index_T  *ip;
   int      ret;

   /* BODY
    */
   ip = L_HISAM_SHMEM_INDEX_TABLE(ipart);
   m=l=0, r=ipart->curr_index_nbr-1;


   /* compare 1st inode */
   if ( r == -1 )
   {
      *iidx = 0;
      *pre_record_index = NULL_INDEX;
      return FALSE;
   }


   L_HISAM_KeyOfRecord (&ipart->key_def,  L_HISAM_SHMEM_RECORD(ip->ridx_last), ipart->work_key);

   if ((ret = KeyCompare((Ipart_T *)ipart)) <=0 )
      goto Search_RecordList;

   /* compare last */
   L_HISAM_KeyOfRecord (&ipart->key_def,  L_HISAM_SHMEM_RECORD(ip[r].ridx_last), ipart->work_key);

   if (  (ret = KeyCompare((Ipart_T *)ipart)) ==0 )
   {
      m=r;  goto Search_RecordList;
   }

   if ( ret > 0 )
   {
      *iidx = r;
      *pre_record_index = ip[r].ridx_last;
      return FALSE;
   }

   /* binary search */
   while (1)
   {
      m = (l+r) >> 1;
      if ( l==m )
      {
         m=r;  goto Search_RecordList;
      }


      L_HISAM_KeyOfRecord (&ipart->key_def,  L_HISAM_SHMEM_RECORD(ip[m].ridx_last), ipart->work_key);

      ret = KeyCompare((Ipart_T *)ipart) ;

      if ( ret == 0 )
         goto Search_RecordList;

      if ( ret > 0 )
         l=m;
      else
         r=m;
   } /* while */


Search_RecordList:
   ip+=m;
   *iidx = m;
   if ( m == 0 )
      *pre_record_index= NULL_INDEX;
   else
      *pre_record_index= ip[-1].ridx_last;

   for ( *record_index=ip->ridx_1st, i=0; i< ip->nbr; i++)
   {
      L_HISAM_KeyOfRecord (&ipart->key_def,  L_HISAM_SHMEM_RECORD(*record_index), ipart->work_key);
      ret = KeyCompare((Ipart_T *)ipart);

      if ( ret == 0 )
         return TRUE;
      if ( ret < 0 )
         return FALSE;
      *pre_record_index = *record_index;
      *record_index = L_HISAM_SHMEM_RLINK_TABLE(ipart)[*record_index];
   }
   /* should not goto here */
   return FALSE;
} /* L_HISAM_ShMem_Locate */


/* PURPOSE: Locate a record of a given key
 * INPUT:   desc    -- the handle of a HISAM database.
 *          key     -- record-key be searched.
 * OUTPUT:  iidx,
 *          record_index,
 *          pre_record_index
 * RETUEN:  If record is found then return TRUE, else return FALSE
 * NOTE:    When return TRUE, iidx and pre_record_index is not defined.
 */
static BOOL_T L_HISAM_ShMem_QuickLocate (L_HISAM_ShMem_Desc_T *desc, ShMem_Ipart_T *ipart, UI32_T *iidx,
                    UI32_T *record_index, UI32_T *pre_record_index)
{
   UI32_T hidx;

   /* Serch the Hash-Table first */
   if ( L_HISAM_ShMem_Hash_Locate (desc, ipart, &hidx) )
   {
      *record_index = L_HISAM_SHMEM_HASH_TABLE(ipart)[hidx] ;
      return  TRUE;
   }

   return L_HISAM_ShMem_Locate (desc, ipart, iidx, record_index, pre_record_index);
} /* L_HISAM_ShMem_QuickLocate */


/* TEST PROGRAM BODY
 */

/* Test Program */
#ifdef HISAM_DEBUG

/*#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
*/



static void* MyMalloc(UI32_T size)
{
   mcount++;
   return malloc(size);
}
static void MyFree (void *p)
{
   mcount--;
   free (p);
}

/*****************************
 *****************************/
void L_HISAM_Dump (L_HISAM_Desc_T *desc, UI32_T kidx)
{
   Ipart_T  *ipart = IPART(kidx);
   UI32_T   iidx;
   UI32_T   record_index;
   UI32_T   i, j=0;
   Index_T  *ip;

   if ( kidx >= WK->key_number) return;

   ip  = ipart->index_table;
   puts("\n");

   /* initate index */
   for ( iidx =0; iidx < ipart->curr_index_nbr; iidx++, ip++)
   {
      printf("\n\niidx=%d, nbr=%d\n", iidx, ip->nbr);

      for ( record_index=ip->ridx_1st, i=0;
            i< ip->nbr;
            i++, record_index = ipart->rlink_table[record_index] )
      {
         printf("(ridx=%d,%s)  ", record_index, RECORD(record_index) );
         if ( (i & 3) == 3 ) puts("");
         j++;
      }

   }
   printf("\r\ntotal %d records",j);
}



static dump1_idx;
static UI32_T Dump1_call_back (char *rec)
{
   printf("%s  ", rec);
   if ( (++dump1_idx & 7 ) == 0 )
      puts("");
   return L_HISAM_SEARCH_CONTINUE;
}


static void L_HISAM_Dump1 (L_HISAM_Desc_T *desc, UI32_T kidx)
{
   UI8_T   key[8];

   if ( kidx >= WK->key_number) return;
   memset (key, 0, sizeof(key) );
   puts("\n\nL_HISAM_Search()");
   dump1_idx = 0;
   L_HISAM_Search (desc, kidx, key, Dump1_call_back, 0);
   printf("\r\ntotal %d records",dump1_idx);
}

BOOL_T L_HISAM_GetRecordandNextKey (L_HISAM_Desc_T *desc, UI32_T kidx, UI8_T *key,  UI8_T *rec)
{
   UI32_T iidx, pre_record_index;
   UI32_T record_index;
   Ipart_T *ipart = IPART(kidx);

   if ( kidx >= WK->key_number)
      return FALSE;

   memcpy (ipart->in_key, key, ipart->key_length);
   /* locate position of current key */
   if ( L_HISAM_QuickLocate( desc, ipart, &iidx, &record_index, &pre_record_index) )
   {
      GET_RECORD(rec, record_index);

	      record_index = ipart->rlink_table[record_index];
	      if ( record_index != NULL_INDEX )
	         L_HISAM_KeyOfRecord(&ipart->key_def,RECORD(record_index), key); /* return the key back so that the user can do furthur search from the next record */
   	
      return TRUE;
   }
   else
    {
          /* Get the next record index */
      if ( pre_record_index == NULL_INDEX  )
         record_index = ipart->index_table[iidx].ridx_1st;
      else
         record_index = ipart->rlink_table [pre_record_index];
      if ( record_index == NULL_INDEX )
        return FALSE;
      else
        {
              GET_RECORD(rec, record_index);

	      record_index = ipart->rlink_table[record_index];
	      if ( record_index != NULL_INDEX )
	         L_HISAM_KeyOfRecord(&ipart->key_def,RECORD(record_index), key); /* return the key back so that the user can do furthur search from the next record */
            return TRUE;
        }

   }
   return FALSE;
} /* L_HISAM_GetRecordandNextKey */




void HisamMain()
{

#define NO_REC      (2 * 1024)
int no_rec = NO_REC;


    const static L_HISAM_KeyDef_T key_def_table[] =	{
        /* Key = ipCidrRouteDestip + ipCidrRouteMask + ipCidrRouteTos + ipCidrRouteNextHop
         */
        {   4,			                /* field number */
            {0, 4, 8, 12, 0, 0, 0, 0},	/* offset */
            {4, 4, 4, 4, 0, 0, 0, 0}	/* len */
        },

        #if 0
       {   1,			                /* field number */
            {0, 0, 0, 0, 0, 0, 0, 0},	/* offset */
            {4, 0, 0, 0, 0, 0, 0, 0}	/* len */
        },
        #endif
        /* Key = ipCidrRouteNextHop + ipCidrRouteDestip + ipCidrRouteMask + ipCidrRouteTos
         */
        {   4,                          /* field number */
            {12, 0, 4, 8, 0, 0, 0, 0},  /* offset */
            {4, 4, 4, 4, 0, 0, 0, 0}    /* len */
        },

#if 0
        {   2,                          /* field number */
            {12, 0, 4, 8, 0, 0, 0, 0},  /* offset */
            {4, 4, 4, 4, 0, 0, 0, 0}    /* len */
        },
#endif

        /* Key = dip + reverse netmask
         */
        {   2,                          /* field number */
            {0, 4, 0, 0, 0, 0, 0, 0},  /* offset */
            {4, 4, 0, 0, 0, 0, 0, 0}    /* len */
        },						
    };
								
   time_t ltime1, ltime2;

   L_HISAM_Desc_T desc;
   static UI32_T kidx;
   static UI8_T  key[20], old_key[20], rec[33];
   static UI8_T  rec_tab [NO_REC] [32], (*rec_p)[32];
   UI32_T i, j, cc, k, kk=1, key_s=0, key_e=0;


   /* BODY */
   desc.hash_depth = 4;
   desc.N1 = 2;
   desc.N2 = 30;
   desc.record_length = 32;
   desc.total_hash_nbr = no_rec;
   desc.total_index_nbr = no_rec/desc.N2+1;
   desc.total_record_nbr = no_rec;

   if (L_HISAM_Create(&desc, 1, key_def_table)==FALSE)
    {
        printf("\r\nL_HISAM_Create() Fail!");
        return;
    }


    for ( i=0; i<no_rec; i++)
    {
        sprintf ( rec_tab[i], "%04X%04X%04X%04X%04X%04X%04X%04X", i, i, i, i, i, i, i, i);
    }


   while (1)
   {
      printf ("\r\nS,s:set G:Get  N:next D:del, A:del-All U:dump k:kidx C:destroy R: delete Odd key M: delete Even key B: Add Odd Key E: Add Even Key");
      printf ("\r\nH: set key range (start) J:set key range (end)");
      printf ("\r\n--> ");

      cc = getchar();getchar();
      printf("%c",cc);
      switch (cc)
      {
        case 'H':
        case 'h':
             printf("\r\nset key range (start)");
            key_s = 0;
            while((cc=getchar()) != '\n')
            { getchar();
                if(cc<'0' || cc > '9')
                {
                    printf("Erro in Input\n");
                    break;
                }
                    cc -= '0';
                key_s = key_s*10+cc;
            }
            printf("\r\nkey_s=%d",key_s);
            break;
        case 'J':
        case 'j':
             printf("\r\nset key range (end)");
            key_e = 0;
            while((cc=getchar()) != '\n')
            { getchar();
                if(cc<'0' || cc > '9')
                {
                    printf("Erro in Input\n");
                    break;
                }
                    cc -= '0';
                key_e = key_e*10+cc;
            }
            printf("\r\nkey_e=%d",key_e);
            break;
            break;
      case 'C': case 'c':
         L_HISAM_Destroy (&desc);
         L_HISAM_Create(&desc, 1, key_def_table);
         break;

      case 27:
         exit(0);

      case 'S':   /* Static Set */
      case 's':   /* Dynamic Set */


         time(&ltime1);

         for ( k=0; k<kk; k++) {
             for ( rec_p=rec_tab[key_s], i=key_s; i<=key_e; i++, rec_p++)
             {  UI32_T   ret;

               if ( i== (no_rec-1))
               {
                  ret=ret;
               }

                ret = L_HISAM_SetRecord (&desc, rec_p[0], (BOOL_T)(cc == 'S') ) ;

                if ( ret == L_HISAM_CONFLICT || ret == L_HISAM_NO_BUFFER )
                {
                   puts("set fail");
                   break;
                }
             } /* End of for */
           /* L_HISAM_DeleteAllRecord (&desc);*/
         }
         time(&ltime2);
         printf("setting time = %d", (int)(ltime2-ltime1));
       /*  getchar();*/

         goto DUMP;
         break;
      case 'R':
      case 'r': /* delete odd */

         for ( k=0; k<kk; k++) {
             for ( rec_p=rec_tab[key_s], i=key_s; i<=key_e; i++, rec_p++)
             {  UI32_T   ret;
               if (i%2==0)
                continue;
               memcpy(key, rec_tab[i], 16);
               L_HISAM_DeleteRecord (&desc, key);


             } /* End of for */
           /* L_HISAM_DeleteAllRecord (&desc);*/
         }
         break;
      case 'M':
      case 'm': /* delete even */

         for ( k=0; k<kk; k++) {
             for ( rec_p=rec_tab[key_s], i=key_s; i<=key_e; i++, rec_p++)
             {  UI32_T   ret;
               if (i%2!=0)
                continue;
               memcpy(key, rec_tab[i], 16);
               L_HISAM_DeleteRecord (&desc, key);


             } /* End of for */
           /* L_HISAM_DeleteAllRecord (&desc);*/
         }
         break;
      case 'B':
      case 'b': /* add odd */

         for ( k=0; k<kk; k++) {
             for ( rec_p=rec_tab[key_s], i=key_s; i<=key_e; i++, rec_p++)
             {  UI32_T   ret;
               if (i%2==0)
                continue;
                     ret = L_HISAM_SetRecord (&desc, rec_p[0], FALSE ) ;
                if ( ret == L_HISAM_CONFLICT || ret == L_HISAM_NO_BUFFER )
                {
                   puts("set fail");
                   break;
                }
             } /* End of for */
           /* L_HISAM_DeleteAllRecord (&desc);*/
         }
         break;
      case 'E':
      case 'e': /* add even */

         for ( k=0; k<kk; k++) {
             for ( rec_p=rec_tab[key_s], i=key_s; i<=key_e; i++, rec_p++)
             {  UI32_T   ret;
               if (i%2!=0)
                continue;
                     ret = L_HISAM_SetRecord (&desc, rec_p[0], FALSE ) ;
                if ( ret == L_HISAM_CONFLICT || ret == L_HISAM_NO_BUFFER )
                {
                   puts("set fail");
                   break;
                }
             } /* End of for */
           /* L_HISAM_DeleteAllRecord (&desc);*/
         }
         break;
      case 'A': case 'a': /* Delete All */
         L_HISAM_DeleteAllRecord (&desc);
         break;

      case 'D': case 'd': /* Delete */
         printf("input the primary key: ");
         gets (key);
         L_HISAM_DeleteRecord (&desc, key);
         break;

      case 'U': case 'u':
DUMP:
         L_HISAM_Dump (&desc, kidx);
         L_HISAM_Dump1 (&desc, kidx);
         break;

      case 'K': case 'k':
         printf("input kidx: "); gets(key);
         kidx = atoi(key);
         break;

      case 'G': case 'g':
         /* Test Get */
         while(1)
         {
            printf("\ninput key: "); gets(key);
            if ( key[0] == 27 ) break;

            if ( L_HISAM_GetRecord ( &desc, kidx, key, rec) )
            {
               printf ( "Get Ok, kidx=%d, key=%s, rec=%s\n", kidx, key, rec);
            }
            else
               printf ( "Get Fail, key=%s\n", key);

         }
         break;

      case 'N': case'n':
         /* test L_HISAM_GetNextRecord */
         memset(key, 0, sizeof(key) );
         memcpy(old_key,key,20);
         j=0;
         while ( L_HISAM_GetRecordandNextKey ( &desc, kidx, key, rec) )
         {  rec[32]='\0';
            printf ( "\r\nL_HISAM_GetRecordandNextKey() Ok, record key=%s, rec=%s", old_key, rec);
            j++;
            if (memcmp(old_key, key, 20)==0)
                break;
            memcpy(old_key,key,20);
         }
         printf("\r\ntotal %d records",j);

      } /* End of switch */
   } /* End of while */
}

#endif /* HISAM_DEBUG */

