

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


#ifndef _L_HISAM_H
#define _L_HISAM_H


#include "sys_type.h"


/* NAMING CONSTANT DECLARACTIONS
 */
#define L_HISAM_MAX_FIELD_NUMBER_OF_A_KEY      8
#define L_HISAM_MAX_KEY_NUMBER                 4
#define L_HISAM_MAX_KEY_LEN                    128

enum
{
   L_HISAM_INSERT = 0,
   L_HISAM_REPLACE,
   L_HISAM_CONFLICT,
   L_HISAM_NO_BUFFER
};

enum
{
   L_HISAM_SEARCH_COMPLETED=0,
   L_HISAM_SEARCH_END_OF_LIST,
   L_HISAM_SEARCH_BREAK,
   L_HISAM_SEARCH_CONTINUE,
   L_HISAM_SEARCH_SKIP,  /* Skip counting of a record */
   L_HISAM_SEARCH_INVALID_KIDX
};


/* DATA TYPE DECLARATIONS
 */
typedef enum
{
   L_HISAM_NOT_INTEGER=0,
   L_HISAM_2BYTE_INTEGER,
   L_HISAM_4BYTE_INTEGER,
} L_HISAM_Integer_Type_T;

/* NOTE: Use this descriptor to Describe the capacity of HISAM structure.
 *       Before use the HISAM structure, MUST call L_HISAM_Create() first to
 *       to set the capacity of HISAM. And then use the descriptor as a handle
 *       to call other relate functions.
 */
typedef struct
{

    UI32_T   field_number;          /* field number that form a key */
    UI32_T   offset[ L_HISAM_MAX_FIELD_NUMBER_OF_A_KEY];  /* field offset from a record */
    UI32_T   length[ L_HISAM_MAX_FIELD_NUMBER_OF_A_KEY];  /* filed length */
} L_HISAM_KeyDef_T;


typedef struct
{
   UI32_T   total_record_nbr; /* total element number of record table */
   UI32_T   total_index_nbr;  /* total element number of "index-part", each element
                               * associate to a record_list
                               */
   UI32_T   total_hash_nbr;   /* total element number of "hash-part"  */
   UI32_T   record_length;    /* size of a record */
   UI32_T   hash_depth;       /* */
   UI32_T   N1;               /* Abs (list_nbr - average_nbr) < N1 => need not adjust */
   UI32_T   N2;               /* each list > N2 => split */
   void     *wk;              /* working area, for HISAM internal used */
   /* store last get or set record index
    * Note: Must avoid race condition, can only reference in OM
    */
   UI32_T   last_reference_record_index;
} L_HISAM_Desc_T;


/*---------------------------------
 * Types for shared memory version
 *---------------------------------
 */
typedef struct
{
   UI32_T   total_record_nbr; /* total element number of record table */
   UI32_T   total_index_nbr;  /* total element number of "index-part", each element
                               * associate to a record_list
                               */
   UI32_T   total_hash_nbr;   /* total element number of "hash-part"  */
   UI32_T   record_length;    /* size of a record */
   UI32_T   hash_depth;       /* */
   UI32_T   N1;               /* Abs (list_nbr - average_nbr) < N1 => need not adjust */
   UI32_T   N2;               /* each list > N2 => split */
   I32_T   wk_offset;         /* the relative address of the working buffer with desc_p as base addr. */
   /* store last get or set record index
    * Note: Must avoid race condition, can only reference in OM
    */
   UI32_T   last_reference_record_index;
} L_HISAM_ShMem_Desc_T;


/* EXPORTED SUBPROGRAM DECLARATIONS
 */
/* PURPOSE: This function create a HISAM structure.
 * INPUT:   desc  -- Description of data storage (Database) capacity,
 *                   include hash-table size (total_hash_nbr),
 *                   index-table size (total_index_nbr),
 *                   record-table size (total_record_nbr),
 *                   record-size (record_length),
 *                   key-size (key_len).
 *          number_of_key
 *          key_def_table
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
 *
 */
#define L_HISAM_Create(desc, number_of_key, key_def_table)          \
        L_HISAM_CreateV2(desc, number_of_key, key_def_table, 0)

/* PURPOSE: This function create a HISAM structure.
 * INPUT:   desc               -- Description of data storage (Database) capacity,
 *                                include hash-table size (total_hash_nbr),
 *                                index-table size (total_index_nbr),
 *                                record-table size (total_record_nbr),
 *                                record-size (record_length),
 *                                key-size (key_len).
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
                    const L_HISAM_Integer_Type_T  integer_type_table[][L_HISAM_MAX_FIELD_NUMBER_OF_A_KEY]);

/* PURPOSE: Destroy the exited HISAM structure, including the storage.
 * INPUT:   desc  -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETUEN:  TRUE  -- Successfully release resource of HISAM.
 *          FALSE -- desc not a handle of HISAM database.
 * NOTES:
 */
BOOL_T L_HISAM_Destroy (L_HISAM_Desc_T *desc);


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
UI32_T L_HISAM_SetRecord (L_HISAM_Desc_T *desc, UI8_T *rec, BOOL_T is_static);


/* PURPOSE: Delete all records of HISAM database.
 * INPUT:   desc    -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETUEN:  none
 */
void L_HISAM_DeleteAllRecord (L_HISAM_Desc_T *desc);



/* PURPOSE: Delete a specified record
 * INPUT:   desc    -- the handle of a HISAM database.
 *          key     -- key of primary key
 * OUTPUT:  desc->last_reference_record_index
 * RETUEN:  none
 */
BOOL_T L_HISAM_DeleteRecord (L_HISAM_Desc_T *desc, UI8_T *key);



/* PURPOSE: Get next record of given key
 * INPUT:   desc    -- the handle of a HISAM database.
 *          kidx    -- key index
 *          key     -- the record-key to be searched.
 * OUTPUT:  rec     -- the buffer to save retrieved record.
 *          desc->last_reference_record_index
 * RETUEN:  none
 */
BOOL_T L_HISAM_GetNextRecord (L_HISAM_Desc_T *desc, UI32_T kidx, UI8_T *key, UI8_T *rec);



/* PURPOSE: Get a record of given key
 * INPUT:   desc    -- the handle of a HISAM database.
 *          kidx    -- key index
 *          key     -- the searched record-key.
 * OUTPUT:  rec     -- the buffer to save retrieved record.
 *          desc->last_reference_record_index
 * RETUEN:  none
 */
BOOL_T L_HISAM_GetRecord (L_HISAM_Desc_T *desc, UI32_T kidx, UI8_T *key, UI8_T *rec);



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
UI32_T L_HISAM_Search (L_HISAM_Desc_T *desc, UI32_T kidx, UI8_T *key, UI32_T (*call_back) (char*rec), UI32_T count);



UI32_T __L_HISAM_Search_WithCookie(
   L_HISAM_Desc_T *desc, UI32_T kidx, UI8_T *key,
   UI32_T (*call_back) (char*rec, UI32_T cookie),UI32_T count, UI32_T cookie, BOOL_T get_next);

#define L_HISAM_Search_WithCookie(desc, kidx, key, call_back, count, cookie) \
    __L_HISAM_Search_WithCookie(desc, kidx, key, call_back, count, cookie, FALSE)

#define L_HISAM_SearchNext_WithCookie(desc, kidx, key, call_back, count, cookie) \
    __L_HISAM_Search_WithCookie(desc, kidx, key, call_back, count, cookie, TRUE)


/* PURPOSE: Get the number of free buffer.
 * INPUT:   desc     -- HISAM descriptor to operation
 * OUTPUT:  none
 * RETURN:  number of free buffer
 */
UI32_T L_HISAM_GetFreeBufNo (L_HISAM_Desc_T *desc);

/* PURPOSE: Get record address by record index
 * INPUT:   ridx:   record index
 * OUTPUT:  none
 * RETUEN:  record address or null if ridx out of range
 */
void *L_HISAM_GetRecordAddressByIndex(L_HISAM_Desc_T *desc, UI32_T ridx);


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
UI32_T L_HISAM_ShMem_GetWorkingBufferRequiredSize(L_HISAM_ShMem_Desc_T *desc, UI32_T number_of_key);

/* PURPOSE: This function create a HISAM structure.
 * INPUT:   desc  -- Description of data storage (Database) capacity,
 *                   include hash-table size (total_hash_nbr),
 *                   index-table size (total_index_nbr),
 *                   record-table size (total_record_nbr),
 *                   record-size (record_length),
 *                   offset-of-working-buffer (wk_offset),
 *          number_of_key
 *          key_def_table
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
 *          5. The size of working buffer must be L_HISAM_ShMem_GetWorkingBufferRequiredSize()
 *          6. the descriptor and working buffer must be in shared memory
 */
#define L_HISAM_ShMem_Create(desc, number_of_key, key_def_table)          \
        L_HISAM_ShMem_CreateV2(desc, number_of_key, key_def_table, 0)

/* PURPOSE: This function create a HISAM structure.
 * INPUT:   desc               -- Description of data storage (Database) capacity,
 *                                include hash-table size (total_hash_nbr),
 *                                index-table size (total_index_nbr),
 *                                record-table size (total_record_nbr),
 *                                record-size (record_length),
 *                                key-size (key_len).
 *                                wk_offset
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
BOOL_T L_HISAM_ShMem_CreateV2(L_HISAM_ShMem_Desc_T *desc, UI32_T number_of_key,
                    const L_HISAM_KeyDef_T *key_def_table,
                    const L_HISAM_Integer_Type_T  integer_type_table[][L_HISAM_MAX_FIELD_NUMBER_OF_A_KEY]);

/* PURPOSE: Destroy the exited HISAM structure, including the storage.
 * INPUT:   desc  -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETUEN:  TRUE  -- Successfully release resource of HISAM.
 *          FALSE -- desc not a handle of HISAM database.
 * NOTES:
 */
BOOL_T L_HISAM_ShMem_Destroy (L_HISAM_ShMem_Desc_T *desc);


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
UI32_T L_HISAM_ShMem_SetRecord (L_HISAM_ShMem_Desc_T *desc, UI8_T *rec, BOOL_T is_static);


/* PURPOSE: Delete all records of HISAM database.
 * INPUT:   desc    -- the handle of a HISAM database.
 * OUTPUT:  none
 * RETUEN:  none
 */
void L_HISAM_ShMem_DeleteAllRecord (L_HISAM_ShMem_Desc_T *desc);



/* PURPOSE: Delete a specified record
 * INPUT:   desc    -- the handle of a HISAM database.
 *          key     -- key of primary key
 * OUTPUT:  desc->last_reference_record_index
 * RETUEN:  none
 */
BOOL_T L_HISAM_ShMem_DeleteRecord (L_HISAM_ShMem_Desc_T *desc, UI8_T *key);



/* PURPOSE: Get next record of given key
 * INPUT:   desc    -- the handle of a HISAM database.
 *          kidx    -- key index
 *          key     -- the record-key to be searched.
 * OUTPUT:  rec     -- the buffer to save retrieved record.
 *          desc->last_reference_record_index
 * RETUEN:  none
 */
BOOL_T L_HISAM_ShMem_GetNextRecord (L_HISAM_ShMem_Desc_T *desc, UI32_T kidx, UI8_T *key, UI8_T *rec);



/* PURPOSE: Get a record of given key
 * INPUT:   desc    -- the handle of a HISAM database.
 *          kidx    -- key index
 *          key     -- the searched record-key.
 * OUTPUT:  rec     -- the buffer to save retrieved record.
 *          desc->last_reference_record_index
 * RETUEN:  none
 */
BOOL_T L_HISAM_ShMem_GetRecord (L_HISAM_ShMem_Desc_T *desc, UI32_T kidx, UI8_T *key, UI8_T *rec);



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
UI32_T L_HISAM_ShMem_Search (L_HISAM_ShMem_Desc_T *desc, UI32_T kidx, UI8_T *key, UI32_T (*call_back) (char*rec), UI32_T count);



UI32_T __L_HISAM_ShMem_Search_WithCookie(
   L_HISAM_ShMem_Desc_T *desc, UI32_T kidx, UI8_T *key,
   UI32_T (*call_back) (char*rec, UI32_T cookie),UI32_T count, UI32_T cookie, BOOL_T get_next);

#define L_HISAM_ShMem_Search_WithCookie(desc, kidx, key, call_back, count, cookie) \
    __L_HISAM_ShMem_Search_WithCookie(desc, kidx, key, call_back, count, cookie, FALSE)

#define L_HISAM_ShMem_SearchNext_WithCookie(desc, kidx, key, call_back, count, cookie) \
    __L_HISAM_ShMem_Search_WithCookie(desc, kidx, key, call_back, count, cookie, TRUE)

/* PURPOSE: Get the number of free buffer.
 * INPUT:   desc     -- HISAM descriptor to operation
 * OUTPUT:  none
 * RETURN:  number of free buffer
 */
UI32_T L_HISAM_ShMem_GetFreeBufNo (L_HISAM_ShMem_Desc_T *desc);

/* PURPOSE: Get record address by record index
 * INPUT:   ridx:   record index
 * OUTPUT:  none
 * RETUEN:  record address or null if ridx out of range
 */
void *L_HISAM_ShMem_GetRecordAddressByIndex(L_HISAM_ShMem_Desc_T *desc, UI32_T ridx);



#endif /* _L_HISAM_H */

