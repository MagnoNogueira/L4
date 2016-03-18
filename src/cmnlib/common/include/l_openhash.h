/* MODULE NAME:  L_OpenHash.h
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
#ifndef L_OPENHASH_H
#define L_OPENHASH_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

typedef struct
{
    UI16_T  hash_length;
    UI8_T   hash_depth;
    UI8_T   key_length;
    void    *hash_buffer_p; /* for hash internal use */
} L_OPENHASH_Desc_T;

typedef enum
{
    L_OPENHASH_OK = 0,
    L_OPENHASH_FAIL,
    L_OPENHASH_TABLE_FULL,
    L_OPENHASH_DEPTH_FULL
} L_OPENHASH_Ret_E;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
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
BOOL_T L_OPENHASH_Create(L_OPENHASH_Desc_T  *desc_p);

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
UI32_T L_OPENHASH_Insert(L_OPENHASH_Desc_T *desc_p, UI8_T  *key_p);

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
void* L_OPENHASH_LookUp (L_OPENHASH_Desc_T *desc_p, UI8_T *key_p);



/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_OPENHASH_DeleteAll
 * -------------------------------------------------------------------------
 * PURPOSE:Delete all records in the hash table.
 * INPUT:  desc_p - the description of the created hash table
 * OUTPUT: None
 * RETURN: None
 * NOTES:  None
 * -------------------------------------------------------------------------*/
void L_OPENHASH_DeleteAll(L_OPENHASH_Desc_T *desc_p);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: L_OPENHASH_Destroy
 * -------------------------------------------------------------------------
 * PURPOSE:Release all resources allocated in L_OPENHASH_Create()
 * INPUT:  desc_p - the description of the hash table which will be destroyed
 * OUTPUT: None
 * RETURN: None
 * NOTES:  None
 * -------------------------------------------------------------------------*/
void L_OPENHASH_Destroy(L_OPENHASH_Desc_T *desc_p);

#endif    /* End of L_OPENHASH_H */

