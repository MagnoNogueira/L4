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
#include <stdlib.h>
#include <memory.h>
#include "rule_lib_array.h"

#ifndef _countof
#define _countof(_Array) (sizeof(_Array)/sizeof(_Array[0]))
#endif

#ifndef ASSERT
#define ASSERT(eq)
#endif

void*
RULE_LIB_ARRAY_ITERATOR_Ptr(
    struct RULE_LIB_ARRAY_ITERATOR_T *this)
{
    return ((RULE_LIB_ARRAY_T*)this->array)->at(this->array, this->i);
}

void*
RULE_LIB_ARRAY_ITERATOR_Next(
    struct RULE_LIB_ARRAY_ITERATOR_T *this)
{
    return ((RULE_LIB_ARRAY_T*)this->array)->at(this->array, this->i++);
}

BOOL_T
RULE_LIB_ARRAY_ITERATOR_Eof(
    struct RULE_LIB_ARRAY_ITERATOR_T *this)
{
    return (((RULE_LIB_ARRAY_T*)this->array)->metadata->size <= this->i) ? TRUE : FALSE;
}

BOOL_T
RULE_LIB_ARRAY_ITERATOR_Equal(
    struct RULE_LIB_ARRAY_ITERATOR_T *this,
    struct RULE_LIB_ARRAY_ITERATOR_T *rhs)
{
    return (this->array == rhs->array && this->i == rhs->i) ? TRUE : FALSE;
}

RULE_LIB_ARRAY_ITERATOR_T
RULE_LIB_ARRAY_Begin(
    struct RULE_LIB_ARRAY_T *this)
{
    RULE_LIB_ARRAY_ITERATOR_T iter;

    RULE_LIB_ARRAY_ITERATOR_ConstructObject(this, &iter);

    iter.i = 0;

    return iter;
}

RULE_LIB_ARRAY_ITERATOR_T
RULE_LIB_ARRAY_End(
    struct RULE_LIB_ARRAY_T *this)
{
    RULE_LIB_ARRAY_ITERATOR_T iter;

    RULE_LIB_ARRAY_ITERATOR_ConstructObject(this, &iter);

    iter.i = this->metadata->size;

    return iter;
}

BOOL_T
RULE_LIB_ARRAY_Empty(
    struct RULE_LIB_ARRAY_T *this)
{
    ASSERT(NULL != this);

    return (0 == this->metadata->size) ? TRUE : FALSE;
}

void*
RULE_LIB_ARRAY_At(
    struct RULE_LIB_ARRAY_T *this,
    UI32_T index)
{
    ASSERT(NULL != this);
    ASSERT(index <= this->count_element);

    return &((UI8_T*)this->data)[index * this->metadata->cb_element];
}

void*
RULE_LIB_ARRAY_Front(
    struct RULE_LIB_ARRAY_T *this)
{
    ASSERT(NULL != this);
    ASSERT(TRUE != this->empty(this));

    if (this->empty(this))
    {
        return NULL;
    }

    return &((UI8_T*)this->data)[0];
}

void*
RULE_LIB_ARRAY_Back(
    struct RULE_LIB_ARRAY_T *this)
{
    ASSERT(NULL != this);
    ASSERT(TRUE != this->empty(this));

    if (this->empty(this))
    {
        return NULL;
    }

    return &((UI8_T*)this->data)[(this->metadata->size-1) * this->metadata->cb_element];
}

BOOL_T
RULE_LIB_ARRAY_PushBack(
    struct RULE_LIB_ARRAY_T *this,
    void *entry)
{
    UI8_T *ent_p;

    ASSERT(NULL != this);

    if (this->metadata->size == this->metadata->count_element)
    {
        return FALSE;
    }

    ent_p = &((UI8_T*)this->data)[this->metadata->size * this->metadata->cb_element];

    memcpy(ent_p, entry, this->metadata->cb_element);

    this->metadata->size++;
    return TRUE;
}

BOOL_T
RULE_LIB_ARRAY_PushBackAassign(
    struct RULE_LIB_ARRAY_T *this,
    void *entry,
    RULE_LIB_ARRAY_PUSH_BACK_ASSIGN_FUN_T assign)
{
    UI8_T *ent_p;

    ASSERT(NULL != this);

    if (this->metadata->size == this->metadata->count_element)
    {
        return FALSE;
    }

    ent_p = &((UI8_T*)this->data)[this->metadata->size * this->metadata->cb_element];

    assign(ent_p, entry);

    this->metadata->size++;
    return TRUE;
}

void
RULE_LIB_ARRAY_PopBack(
    struct RULE_LIB_ARRAY_T *this)
{
    ASSERT(this);
    ASSERT(TRUE != this->empty(this));

    if (this->empty(this))
    {
        return;
    }

    this->metadata->size -= 1;
}

void
RULE_LIB_ARRAY_Swap(
    struct RULE_LIB_ARRAY_T *this,
    UI32_T index1,
    UI32_T index2)
{
    unsigned char *temp;
    void *p1;
    void *p2;

    p1 = RULE_LIB_ARRAY_At(this, index1);
    p2 = RULE_LIB_ARRAY_At(this, index2);

    ASSERT(NULL != p1);
    ASSERT(NULL != p2);

    if (NULL == p1 || NULL == p2)
    {
        return;
    }

    if (p1 == p2)
    {
        return;
    }

    temp = malloc(this->metadata->cb_element);

    ASSERT(NULL != temp);

    if (NULL == temp)
    {
        return;
    }

    memcpy(temp, p2, this->metadata->cb_element);
    memcpy(p2, p1, this->metadata->cb_element);
    memcpy(p1, temp, this->metadata->cb_element);

    free(temp);
}

void
RULE_LIB_ARRAY_Erase(
    struct RULE_LIB_ARRAY_T *this,
    UI32_T position)
{
    unsigned char *temp;

    ASSERT(this);
    ASSERT(TRUE != this->empty(this));
    ASSERT(position < this->size);

    if (this->empty(this))
    {
        return;
    }

    if (this->metadata->size <= position)
    {
        return;
    }

//    if (position < this->size)
//    {
//        memmove(&((UI8_T*)this->data)[position * this->cb_element],
//                &((UI8_T*)this->data)[(position + 1) * this->cb_element],
//                (this->size - (position + 1)) * this->cb_element);
//    }

    temp = malloc(this->metadata->cb_element);

    ASSERT(NULL != temp);

    if (NULL == temp)
    {
        return;
    }

    /** Move position to back and resize
     * before:
     * [x], a, b, c
     * after:
     * a, b, c, [x]
     */
    memcpy(temp, RULE_LIB_ARRAY_At(this, position), this->metadata->cb_element);

    memmove(&((UI8_T*)this->data)[position * this->metadata->cb_element],
            &((UI8_T*)this->data)[(position + 1) * this->metadata->cb_element],
            (this->metadata->size - (position + 1)) * this->metadata->cb_element);

    memcpy(RULE_LIB_ARRAY_At(this, this->metadata->size-1), temp, this->metadata->cb_element);

    free(temp);

    this->metadata->size -= 1;
}

void
RULE_LIB_ARRAY_Clear(
    struct RULE_LIB_ARRAY_T *this)
{
    ASSERT(this);

    this->metadata->size = 0;
}

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
    struct RULE_LIB_ARRAY_ITERATOR_T *iter)
{
    ASSERT(NULL != array);
    ASSERT(NULL != iter);

    memset(iter, 0, sizeof(*iter));

    iter->array = array;

    iter->ptr   = RULE_LIB_ARRAY_ITERATOR_Ptr;
    iter->next  = RULE_LIB_ARRAY_ITERATOR_Next;
    iter->eof   = RULE_LIB_ARRAY_ITERATOR_Eof;

    iter->equal = RULE_LIB_ARRAY_ITERATOR_Equal;
}

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
    RULE_LIB_ARRAY_T *array)
{
    ASSERT(NULL != ptr);
    ASSERT(NULL != metadata);

    ASSERT(NULL != array);

    array->data             = ptr;
    array->metadata         = metadata;

    array->begin            = RULE_LIB_ARRAY_Begin;
    array->end              = RULE_LIB_ARRAY_End;

    array->empty            = RULE_LIB_ARRAY_Empty;

    array->at               = RULE_LIB_ARRAY_At;
    array->front            = RULE_LIB_ARRAY_Front;
    array->back             = RULE_LIB_ARRAY_Back;

    array->push_back        = RULE_LIB_ARRAY_PushBack;
    array->push_back_assign = RULE_LIB_ARRAY_PushBackAassign;
    array->pop_back         = RULE_LIB_ARRAY_PopBack;
    array->erase            = RULE_LIB_ARRAY_Erase;
    array->clear            = RULE_LIB_ARRAY_Clear;
}

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
    RULE_LIB_ARRAY_FIND_IF_COMPARE_FUN_T cmp)
{
    for (; TRUE != first.equal(&first, &last); first.next(&first))
    {
        if (TRUE == cmp(first.ptr(&first), entry))
        {
            break;
        }
    }

    return first;
}
