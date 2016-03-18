/* FUNCTION NAME: rule_lib_array.h
 * PURPOSE:
 *   An implement for fixed size array
 *
 * NOTES:
 *
 * REASON:
 *    DESCRIPTION:
 *    CREATOR:     junying
 *    Date:        2012/9/25
 *
 * Copyright(C) Edgecore Networks Corporation, 2012
 */

#ifndef _RULE_LIB_ARRAY_H_
#define _RULE_LIB_ARRAY_H_

#include "sys_type.h"

typedef void (*RULE_LIB_ARRAY_PUSH_BACK_ASSIGN_FUN_T) (void*, const void *);
typedef BOOL_T (*RULE_LIB_ARRAY_FIND_IF_COMPARE_FUN_T) (const void*, const void *);

typedef struct RULE_LIB_ARRAY_ITERATOR_T
{
    void *array;
    UI32_T i;

    void*
    (*ptr)(struct RULE_LIB_ARRAY_ITERATOR_T *this);

    void*
    (*next)(struct RULE_LIB_ARRAY_ITERATOR_T *this);

    void*
    (*prev)(struct RULE_LIB_ARRAY_ITERATOR_T *this);

    BOOL_T
    (*eof)(struct RULE_LIB_ARRAY_ITERATOR_T *this);

    BOOL_T
    (*equal)(struct RULE_LIB_ARRAY_ITERATOR_T *this, struct RULE_LIB_ARRAY_ITERATOR_T *rhs);

} RULE_LIB_ARRAY_ITERATOR_T;

typedef struct RULE_LIB_ARRAY_METADATA_T
{
    UI32_T  count_element;

    UI32_T  cb_element;

    UI32_T  offset;

    UI32_T  size;
} RULE_LIB_ARRAY_METADATA_T, * RULE_LIB_ARRAY_METADATA_PTR_T;

typedef struct RULE_LIB_ARRAY_T
{
    void    *data;

    RULE_LIB_ARRAY_METADATA_T   *metadata;

    /** Iterators
     */
    RULE_LIB_ARRAY_ITERATOR_T
    (*begin)(struct RULE_LIB_ARRAY_T *this);

    RULE_LIB_ARRAY_ITERATOR_T
    (*end)(struct RULE_LIB_ARRAY_T *this);

    /** Capacity
     */
    void
    (*resize)(struct RULE_LIB_ARRAY_T *this);

    BOOL_T
    (*empty)(struct RULE_LIB_ARRAY_T *this);

    /** Element access
     */
    void*
    (*at)(struct RULE_LIB_ARRAY_T *this, UI32_T index);

    void*
    (*front)(struct RULE_LIB_ARRAY_T *this);

    void*
    (*back)(struct RULE_LIB_ARRAY_T *this);

    /** Modifiers
     */
    BOOL_T
    (*push_back)(struct RULE_LIB_ARRAY_T *this, void *entry);

    BOOL_T
    (*push_back_assign)(struct RULE_LIB_ARRAY_T *this, void *entry, RULE_LIB_ARRAY_PUSH_BACK_ASSIGN_FUN_T assign);

    void
    (*pop_back)(struct RULE_LIB_ARRAY_T *this);

    void
    (*erase)(struct RULE_LIB_ARRAY_T *this, UI32_T position);

    void
    (*clear)(struct RULE_LIB_ARRAY_T *this);

} RULE_LIB_ARRAY_T, *RULE_LIB_ARRAY_PTR_T;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_LIB_ARRAY_ITERATOR_ConstructObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct an array iterator
 * INPUT    : array - array
 * OUTPUT   : iter  - output
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
void
RULE_LIB_ARRAY_ITERATOR_ConstructObject(
    struct RULE_LIB_ARRAY_T *array,
    struct RULE_LIB_ARRAY_ITERATOR_T *iter
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_LIB_ARRAY_ConstructObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct an array
 * INPUT    : ptr               - pointer of an array
 *            counter_element   - max count of element
 *            cb_element        - size of element in bytes
 * OUTPUT   : array_p           - output
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
void
RULE_LIB_ARRAY_ConstructObject(
    void *ptr,
    RULE_LIB_ARRAY_METADATA_T *metadata,
    RULE_LIB_ARRAY_T *array
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_LIB_ARRAY_FindIf
 *------------------------------------------------------------------------------
 * PURPOSE  : Find element in array
 * INPUT    : first             - first iterator
 *            last              - last iterator
 *            entry             - pointer of entry
 *            cmp               - compare function
 * OUTPUT   : None
 * RETURN   : Returns an iterator to the first element in the range [first,last)
 *            that compares equal to value, or last if not found.
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
RULE_LIB_ARRAY_ITERATOR_T
RULE_LIB_ARRAY_FindIf(
    RULE_LIB_ARRAY_ITERATOR_T first,
    RULE_LIB_ARRAY_ITERATOR_T last,
    const void *entry,
    RULE_LIB_ARRAY_FIND_IF_COMPARE_FUN_T cmp
);


#endif /* _RULE_LIB_ARRAY_H_ */
