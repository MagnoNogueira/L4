/* MODULE NAME:  L_OpenHash.c
 * PURPOSE:
 *  This module implements the open-addressed hash table.
 *  When insert a record into open-addressed hash table,
 *  it will set the record into the hash table according to the key of the record.
 *  If the entry is already in use(collision), it will try to jump to next N
 *  entry(N must be a prime number), and see if that entry is available.
 *  The procedure will be repeated until an available entry is found.
 *  However, if the procedure has been repeated for D times and D is larger than
 *  hash depth, then the given record can't be inserted to the hash table.
 * NOTES:
 *  1.This module does not provide the API to remove a single record due to poor
 *    performance. If you do need this feature, use L_Hash.c which is implemented
 *    chained hash table.
 *
 *  2.Example to use L_OpenHash.c:
 *    Please see unit-test main() for examples to use L_OpenHash.
 *
 *  3.The maxinum number of record can be kept in the hash table is 0xFE(254)
 *    NOTE! number of record in descriptior might be adjusted because
 *    L_OPENHASH_NEXT_REC_OFFSET and number of record must be relative prime numbers.
 *
 * HISTORY
 *    2006/3/2 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2006
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>  /* for printf() */
#include <string.h> /* for memcpy() and memcmp() */
#include <stdlib.h> /* for malloc() */
#include "sys_type.h"
#include "l_openhash.h"

/* NAMING CONSTANT DECLARATIONS
 */
/* all bits set as 1
 */
#define L_OPENHASH_NULL_INDEX      (hash_table_index_T)(~0)

/* the offset to find next entry when collision
 * must be a prime number
 */
#define L_OPENHASH_NEXT_REC_OFFSET 19

/* MACRO FUNCTION DECLARATIONS
 */

/* RETURN: the the address of desc_p->workspace_p
 */
#define WK              ((L_OPENHASH_Workspace_T*)(desc_p->hash_buffer_p))

/* RETURN: the entry of the corresponding index in the hash table
 */
#define HASHTABLE(index)    (((UI8_T**)(WK+1))[index])

/* DATA TYPE DECLARATIONS
 */
typedef UI16_T hash_table_index_T;

/* the hash table is located just behind L_OPENHASH_Workspace_T
 * each entry in hash_table_p keeps the pointer to key if the entry has been set
 * the entry is NULL if it has not been set
 */
typedef struct L_OPENHASH_Workspace_S
{
    UI16_T  free_no;    /* number of free entry in hash table */
    UI8_T   padding[2];
} L_OPENHASH_Workspace_T;

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static void   L_OPENHASH_InitHashTable(L_OPENHASH_Desc_T *desc_p);
static BOOL_T L_OPENHASH_Locate(L_OPENHASH_Desc_T *desc_p, UI8_T *key_p, hash_table_index_T *hash_table_index_p);

/* hash method function
 */
static UI32_T L_OPENHASH_HashMethod_WordXor(L_OPENHASH_Desc_T  *desc_p, UI8_T *key_p);

/* STATIC VARIABLE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM BODIES
 */
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_OPENHASH_Create
 * -------------------------------------------------------------------------
 * PURPOSE:This function creates a table using Hash algorithm.
 * INPUT:  desc_p -
 *             hash_length        - max number of key can be kept in hash table
 *                                  the maximum value can be set is 65535(0xFFFF)
 *             hash_depth         - 0 means unlimited depth
 *                                  the maximum value can be set is 255(0xFF)
 *             key_length         - length of key
 *                                  the maximum value can be set is 255(0xFF)
 * OUTPUT: desc_p  - the handle of a new data storage (Database) after creation.
 * RETURN: TRUE : Sucess
 *         FALSE: Fail
 * NOTES:  When hash_length is less than 256, it's not necessary to use hash library
 * -------------------------------------------------------------------------*/
BOOL_T L_OPENHASH_Create(L_OPENHASH_Desc_T  *desc_p)
{
    L_OPENHASH_Workspace_T *wk_p;

    /* input argument validation
     */
    if(desc_p==NULL)
    {
        return FALSE;
    }

    if(desc_p->hash_depth > desc_p->hash_length)
    {
        /* critical error, use printf directly
         */
        printf("\r\n%s():improperly setting: hash_depth shouldn't be larger than hash_length", __FUNCTION__);
        return FALSE;
    }

    /* adjust num_of_rec if num_of_rec and L_OPENHASH_NEXT_REC_OFFSET are not
     * relatively prime numbers
     */
    if((desc_p->hash_length % L_OPENHASH_NEXT_REC_OFFSET)==0)
    {
        desc_p->hash_length+=1;
    }

    if(desc_p->hash_length==0)
    {
        /* critical error, use printf directly
         */
        printf("\r\n%s():illegal hash_length", __FUNCTION__);
        return FALSE;
    }

    /* allocate required buffer for wk_p
     */
    desc_p->hash_buffer_p = wk_p = (L_OPENHASH_Workspace_T*)malloc(sizeof(L_OPENHASH_Workspace_T) +
                                                                   desc_p->hash_length*sizeof(UI8_T*) /* for hash_table_p */
                                                                  );
    /* init hash table
     */
    L_OPENHASH_InitHashTable(desc_p);

    return TRUE;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_OPENHASH_Insert
 * -------------------------------------------------------------------------
 * PURPOSE:Insert a key into the hash table.
 * INPUT:  desc_p - the description of the created hash table
 *         key_p  - pointer to the key
 * OUTPUT: None
 * RETURN: L_OPENHASH_OK:        Insert sucessfully or the key already exists
 *         L_OPENHASH_TABLE_FULL:Fail to insert due to table full
 *         L_OPENHASH_DEPTH_FULL:Fail to insert due to collision and depth is full
 * NOTES:  Only pointer of the key will be kept in the hash table.
 *         The value of the key should never be changed once the
 *         key has been inserted into the hash table. Or the hash table
 *         might go wrong!
 * -------------------------------------------------------------------------*/
UI32_T L_OPENHASH_Insert(L_OPENHASH_Desc_T *desc_p, UI8_T  *key_p)
{
    hash_table_index_T hash_table_idx;

    /* input argument validation
     */
    if((desc_p==NULL) || (key_p==NULL))
    {
        return L_OPENHASH_FAIL;
    }

    /* check whether the hash table is already full
     */
    if(WK->free_no==0)
    {
        return L_OPENHASH_TABLE_FULL;
    }

    /* locate whether the record already exists
     */
    if(FALSE==L_OPENHASH_Locate(desc_p, key_p, &hash_table_idx))
    {
        /* check that whether hash table still has an entry available
         * for the input key
         */
        if(hash_table_idx==L_OPENHASH_NULL_INDEX)
        {
            return L_OPENHASH_DEPTH_FULL;
        }

        /* update hash table
         */
        HASHTABLE(hash_table_idx)=key_p;
        WK->free_no--;
    }

    return L_OPENHASH_OK;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_OPENHASH_LookUp
 * -------------------------------------------------------------------------
 * PURPOSE:Look up hash table by the given key
 * INPUT:  desc_p - the description of the created hash table
 *         key_p  - pointer to the key which is being looked up.
 * OUTPUT: None
 * RETURN: Sucess: pointer of the key found in hash table.
 *         Fail:   NULL
 * NOTES:  None
 * -------------------------------------------------------------------------*/
void* L_OPENHASH_LookUp (L_OPENHASH_Desc_T *desc_p, UI8_T *key_p)
{
    hash_table_index_T hash_tbl_idx;

    /* input argument validation
     */
    if((desc_p==NULL) || (key_p==NULL))
    {
        return NULL;
    }

    if(TRUE==L_OPENHASH_Locate(desc_p, key_p, &hash_tbl_idx))
    {
        return HASHTABLE(hash_tbl_idx);
    }
    return NULL;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_OPENHASH_DeleteAll
 * -------------------------------------------------------------------------
 * PURPOSE:Delete all records in the hash table.
 * INPUT:  desc_p - the description of the created hash table
 * OUTPUT: None
 * RETURN: None
 * NOTES:  None
 * -------------------------------------------------------------------------*/
void L_OPENHASH_DeleteAll(L_OPENHASH_Desc_T *desc_p)
{
    /* input argument validation
     */
    if(desc_p==NULL)
    {
        return;
    }

    L_OPENHASH_InitHashTable(desc_p);
    return;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_OPENHASH_Destroy
 * -------------------------------------------------------------------------
 * PURPOSE:Release all resources allocated in L_OPENHASH_Create()
 * INPUT:  desc_p - the description of the hash table which will be destroyed
 * OUTPUT: None
 * RETURN: None
 * NOTES:  None
 * -------------------------------------------------------------------------*/
void L_OPENHASH_Destroy(L_OPENHASH_Desc_T *desc_p)
{
    /* input argument validation
     */
    if(desc_p==NULL)
    {
        return;
    }

    free(desc_p->hash_buffer_p);
    desc_p->hash_buffer_p=NULL;

    return;
}

/* LOCAL SUBPROGRAM BODIES
 */
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_OPENHASH_InitHashTable
 * -------------------------------------------------------------------------
 * PURPOSE:Initialize the hash table
 * INPUT:  desc_p - the description of the hash table to be initialized
 * OUTPUT: None
 * RETURN: None
 * NOTES:  None
 * -------------------------------------------------------------------------*/
static void L_OPENHASH_InitHashTable(L_OPENHASH_Desc_T *desc_p)
{
    WK->free_no = desc_p->hash_length;
    /* init hash_table_p
     */
    memset(&(HASHTABLE(0)), 0, desc_p->hash_length*sizeof(UI8_T*));
    return;
}

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_OPENHASH_Locate
 * -------------------------------------------------------------------------
 * PURPOSE:Locate the hash table index of the given key of the record
 *         If the record is found, the corresponding index will be outputed.
 *         If the record is not found, the index that can be used for the given
 *         key will be outputed if available.
 * INPUT:  desc_p             - the description of the created hash table
 *         key_p              - the key of the record to be located
 * OUTPUT: hash_table_index_p - When return TRUE : the hash table index of the given key
 *                              When return FALSE: the hash table index that can be used
 *                                                 by the given key. The index could be
 *                                                 L_OPENHASH_NULL_INDEX if no more space
 *                                                 is available for the given key.
 *
 * RETURN: TRUE : The record has been located sucessfully.
 *         FALSE: The record can't be found.
 * NOTES:  None
 * -------------------------------------------------------------------------*/
static BOOL_T L_OPENHASH_Locate(L_OPENHASH_Desc_T *desc_p, UI8_T *key_p, hash_table_index_T *hash_table_index_p)
{
    hash_table_index_T count, depth, hash_table_idx;
    UI8_T  *hash_entry_p;

    depth = (desc_p->hash_depth==0) ? desc_p->hash_length : desc_p->hash_depth;

    hash_table_idx = L_OPENHASH_HashMethod_WordXor(desc_p, key_p);

    for (count=0; count< depth;  count++ )
    {
        if( (hash_entry_p=HASHTABLE(hash_table_idx)) != NULL)
        {
            if ( memcmp(hash_entry_p, key_p, desc_p->key_length) == 0 )
            {
                *hash_table_index_p = hash_table_idx;
                return TRUE;
            }
        }
        else
        {
            /* no more record can be looked up
             * output current hash_table_idx
             * for ease of setting new record
             */
            *hash_table_index_p = hash_table_idx;
            return FALSE;
        }

        hash_table_idx += L_OPENHASH_NEXT_REC_OFFSET;
        while(hash_table_idx >= desc_p->hash_length)
        {
            hash_table_idx -= desc_p->hash_length;
        }
    }

    /* depth for the given key is used up
     */
    *hash_table_index_p = L_OPENHASH_NULL_INDEX;
    return FALSE;
}

/* hash method function
 */
/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_OPENHASH_HashMethod_WordXor
 * -------------------------------------------------------------------------
 * PURPOSE:Hash function which implements word-sum hash method.
 * INPUT:  desc_p             - the description of the created hash table
 *         key_p              - the key of the record
 * RETURN: hash key
 * NOTES:  None
 * -------------------------------------------------------------------------*/
static UI32_T L_OPENHASH_HashMethod_WordXor(L_OPENHASH_Desc_T  *desc_p, UI8_T *key_p)
{
    UI32_T   index;
    union
    {
       UI32_T ui32;
       UI8_T  ui8[4];
    } key_value;

    key_value.ui32 = 0;

    /* key_p might not at UI32_T natural boundary, so we cannot cast it
     * into UI32_T* and de-reference as UI32_T directly
     * word-wised caculation
     */
    for(index=0; index<((desc_p->key_length)/4); index++)
    {
        key_value.ui8[0] ^= key_p[0];
        key_value.ui8[1] ^= key_p[1];
        key_value.ui8[2] ^= key_p[2];
        key_value.ui8[3] ^= key_p[3];
        /* move key_p to next 4 byte
         */
        key_p+=4;
    }

    /* process remaining part which is less than 4 bytes
     * byte-wised caculation
     */
    for(index=0; index<((desc_p->key_length)&0x3); index++)
    {
        key_value.ui8[index] ^= key_p[index];
    }

    return (key_value.ui32 % desc_p->hash_length);
}

#ifdef L_OPENHASH_UNIT_TEST

typedef struct ip_S
{
    UI8_T  ip[4];
} ip_T;

const ip_T records[] =
    {
        {{1, 1, 1, 1}},
        {{2, 2, 2, 2}},
        {{3, 3, 3, 3}},
    };
const ip_T records2[]=
    {
        {{29,0,0,0}},
        {{49,0,0,0}}
    };

static BOOL_T insert_records(L_OPENHASH_Desc_T *desc_p)
{
    UI32_T index;

    for(index=0; index<(sizeof(records)/sizeof(ip_T)); index++)
    {
        if(L_OPENHASH_OK!=L_OPENHASH_Insert(desc_p, (UI8_T*)&(records[index])))
        {
            printf("\r\ninsert_records fail");
            return FALSE;
        }
    }
    printf("\r\ninsert_records ok");
    return TRUE;
}

static BOOL_T get_one_record(L_OPENHASH_Desc_T *desc_p)
{
    ip_T record = {{2,2,2,2}};

    if(FALSE==L_OPENHASH_LookUp(desc_p, (UI8_T*)&record))
    {
        printf("\r\n%s(): L_OPENHASH_GetRecord() fail", __FUNCTION__);
        return FALSE;
    }
    printf("\r\nget_one_record ok.");
    return TRUE;
}

int main(int argc, char* argv[])
{
    L_OPENHASH_Desc_T desc;

    desc.hash_depth = 2;
    desc.key_length = sizeof(ip_T);
    desc.hash_length=20;

    /* create hash table
     */
    if(FALSE==L_OPENHASH_Create(&desc))
    {
        printf("\r\nFail to create hash table.");
        return -1;
    }

    /* insert records to hash table
     */
    if(FALSE==insert_records(&desc))
        return -1;

    /* collision test
     */
    {
        if(L_OPENHASH_OK!=L_OPENHASH_Insert(&desc, (UI8_T*)&records2[0]))
        {
            printf("\r\nL_OPENHASH_Insert() should be sucess.");
            return FALSE;
        }

        if(L_OPENHASH_DEPTH_FULL!=L_OPENHASH_Insert(&desc, (UI8_T*)&records2[1]))
        {
            printf("\r\nL_OPENHASH_Insert() should be depth full.");
            return FALSE;
        }
    }

    /* try to get one of the record from hash table
     */
    if(FALSE==get_one_record(&desc))
        return -1;

    /* delete all record in hash table
     */
    L_OPENHASH_DeleteAll(&desc);
    printf("\r\ncall L_OPENHASH_DeleteAll");

    /* insert records to hash table again
     */
    if(FALSE==insert_records(&desc))
        return -1;

    /* try to get one of the record from hash table again
     */
    if(FALSE==get_one_record(&desc))
        return -1;

    L_OPENHASH_Destroy(&desc);

    printf("\r\nUnit test pass!");
    return 0;
}
#endif

