/*-----------------------------------------------------------------------------
 * Module   : l_stack.h
 *-----------------------------------------------------------------------------
 * PURPOSE  : This package provides functions to utilize the stack method for
              data storage.
 *-----------------------------------------------------------------------------
 * NOTES    : The Caller must take care of the mutual exculusion situation.
 *-----------------------------------------------------------------------------
 * History  :
 *      Date        --  Modifier,   Reason
 *      10-20-2005      Harold,     First Create
 *-----------------------------------------------------------------------------
 * Copyright(C)                               Accton Corporation, 2005
 *-----------------------------------------------------------------------------
 */

#ifndef L_STACK_H
#define L_STACK_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
/*---------------------------------
 * Types for shared memory version
 *---------------------------------
 */
typedef struct
{
    /* The number which is referred to the type of the L_STACK */
    UI32_T  magic_word;

    /* the value of the max number of elements which the stack can supply */
    UI32_T  max_number_of_element;

    /* the value of the current number of elements in the stack */
    UI32_T  number_of_element;

    /* the offset (base addr. is desc_p) of the buffer which indicates the stack */
    I32_T   stack_buf_offset;

} L_STACK_ShMem_Desc_T;

/* LOCAL SUBPROGRAM DECLARATIONS
 */

/* STATIC VARIABLE DECLARATIONS
 */

/* MACRO FUNCTIONS DECLARACTION
 */
/*---------------------------------
 * Macro for shared memory version
 *---------------------------------
 */
/*----------------------------------------------------------------------------------
 * MACRO NAME : L_STACK_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ
 *----------------------------------------------------------------------------------
 * PURPOSE : Calculate the required size of the working buffer for a stack
 * INPUT   : size - the max number of elements in the stack.
 * OUTPUT  : None
 * RETURN  : The required size
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */
#define L_STACK_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(size)  ((size) * sizeof(UI32_T))


/* EXPORTED SUBPROGRAM BODIES
 */
/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_Create
 *----------------------------------------------------------------------------------
 * PURPOSE : Create a stack
 * INPUT   : size - the max number of elements in the stack.
 * OUTPUT  : None
 * RETURN  : The handler of the stack
 * NOTE    : 1. the max value of size is defined by L_STACK_MAX_ELEMENT_NUMBER
 *           2. if create fail, return NULL
 *---------------------------------------------------------------------------------
 */
UI32_T L_STACK_Create(UI32_T size);

/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_Destroy
 *----------------------------------------------------------------------------------
 * PURPOSE : Destroy the stack
 * INPUT   : *handler - the handler of the destroyed stack
 * OUTPUT  : None
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_Destroy(UI32_T handler);

/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_Pop
 *----------------------------------------------------------------------------------
 * PURPOSE : Pop the top element out of the stack
 * INPUT   : *handler - the handler of the stack
 * OUTPUT  : *value   - the value of the popped element.
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_Pop(UI32_T handler, UI32_T *value);

/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_Push
 *----------------------------------------------------------------------------------
 * PURPOSE : Push the element into the stack
 * INPUT   : handler - the handler of the stack
 *           value   - the value of the push element
 * OUTPUT  : None
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_Push(UI32_T handler, UI32_T value);



/*---------------------------------
 * Exported functions for shared memory version
 *---------------------------------
 */
/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_ShMem_Create
 *----------------------------------------------------------------------------------
 * PURPOSE : Create a stack
 * INPUT   : buffer  - the working buffer for this stack in shared memory.
 *                     the size of buffer must be L_STACK_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ()
 *           size    - the max number of elements in the stack.
 * OUTPUT  : stack_p - the descriptor
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : 1. the max value of size is defined by L_STACK_MAX_ELEMENT_NUMBER
 *           2. if create fail, return NULL
 *           3. the descriptor and working buffer must be in shared memory
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_ShMem_Create(L_STACK_ShMem_Desc_T *stack_p, void *buffer, UI32_T size);

/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_ShMem_Destroy
 *----------------------------------------------------------------------------------
 * PURPOSE : Destroy the stack
 * INPUT   : stack_p - the descriptor
 * OUTPUT  : None
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_ShMem_Destroy(L_STACK_ShMem_Desc_T *stack_p);

/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_ShMem_Pop
 *----------------------------------------------------------------------------------
 * PURPOSE : Pop the top element out of the stack
 * INPUT   : stack_p - the descriptor
 * OUTPUT  : *value  - the value of the popped element.
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_ShMem_Pop(L_STACK_ShMem_Desc_T *stack_p, UI32_T *value);

/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_ShMem_Push
 *----------------------------------------------------------------------------------
 * PURPOSE : Push the element into the stack
 * INPUT   : stack_p - the descriptor
 *           value   - the value of the push element
 * OUTPUT  : None
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_ShMem_Push(L_STACK_ShMem_Desc_T *stack_p, UI32_T value);

BOOL_T L_STACK_ShMem_GetNumberOfElement(L_STACK_ShMem_Desc_T *stack_p, UI16_T *value);

BOOL_T L_STACK_ShMem_GetElementValueByindex(L_STACK_ShMem_Desc_T *stack_p, UI16_T index,UI32_T *value);

#endif /* L_STACK_H */

