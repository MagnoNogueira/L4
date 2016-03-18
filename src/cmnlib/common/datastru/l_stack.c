/*-----------------------------------------------------------------------------
 * Module   : l_stack.c
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

/* INCLUDE FILE DECLARATIONS
 */
#include <stdlib.h>
#include "l_cvrt.h"
#include "l_stack.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define L_STACK_MAGIC_WORD              9527
#define L_STACK_MAX_ELEMENT_NUMBER      2048

/* DATA TYPE DECLARATIONS
 */
typedef struct
{
    /* The number which is referred to the type of the L_STACK */
    UI32_T  magic_word;

    /* the value of the max number of elements which the stack can supply */
    UI32_T  max_number_of_element;

    /* the value of the current number of elements in the stack */
    UI32_T  number_of_element;

    /* the ptr which indicates the stack */
    UI32_T  *stack_buf;

} L_STACK_Desc_T;

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
#define L_STACK_SHMEM_DATA(stack_p) ((UI32_T *)L_CVRT_GET_PTR((stack_p), (stack_p)->stack_buf_offset))

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
UI32_T L_STACK_Create(UI32_T size)
{
    L_STACK_Desc_T *stack_desc_p;

    if(size <= 0 || size > L_STACK_MAX_ELEMENT_NUMBER)
    {
        return NULL;
    }

    if((stack_desc_p = (L_STACK_Desc_T *)malloc(sizeof(L_STACK_Desc_T))) == NULL)
    {
        return NULL;
    }

    stack_desc_p->magic_word = L_STACK_MAGIC_WORD;
    stack_desc_p->max_number_of_element = size;
    stack_desc_p->number_of_element = 0;
    stack_desc_p->stack_buf = (UI32_T *)malloc(size * sizeof(UI32_T));

    if(stack_desc_p->stack_buf == NULL)
    {
        stack_desc_p->magic_word = 0;
        free(stack_desc_p);
        return NULL;
    }

    return (UI32_T)stack_desc_p;

} /* End of L_STACK_Create() */

/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_Destroy
 *----------------------------------------------------------------------------------
 * PURPOSE : Destroy the stack
 * INPUT   : handler - the handler of the destroyed stack
 * OUTPUT  : None
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_Destroy(UI32_T handler)
{
    L_STACK_Desc_T *stack_p;

    stack_p = ( L_STACK_Desc_T *)handler;

    if(stack_p->magic_word != L_STACK_MAGIC_WORD)
    {
        return FALSE;
    }

    free(stack_p->stack_buf);
    stack_p->magic_word = 0;
    free(stack_p);

    return TRUE;

} /* End of L_STACK_Destroy() */

/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_Pop
 *----------------------------------------------------------------------------------
 * PURPOSE : Pop the top element out of the stack
 * INPUT   : handler - the handler of the stack
 * OUTPUT  : *value  - the value of the popped element.
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_Pop(UI32_T handler, UI32_T *value)
{
    L_STACK_Desc_T *stack_p;

    stack_p = (L_STACK_Desc_T *)handler;

    if(stack_p->magic_word != L_STACK_MAGIC_WORD)
    {
        return FALSE;
    }

    if(stack_p->number_of_element == 0)
    {
        return FALSE;
    }

    *value = stack_p->stack_buf[--stack_p->number_of_element];
    return TRUE;

} /* End of L_STACK_Pop() */

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
BOOL_T L_STACK_Push(UI32_T handler, UI32_T value)
{
    L_STACK_Desc_T *stack_p;

    stack_p = ( L_STACK_Desc_T *)handler;

    if(stack_p->magic_word != L_STACK_MAGIC_WORD)
    {
        return FALSE;
    }

    if(stack_p->number_of_element == stack_p->max_number_of_element)
    {
        return FALSE;
    }

    stack_p->stack_buf[stack_p->number_of_element++] = value;

    return TRUE;

} /* End of L_STACK_Push() */


/*---------------------------------
 * Exported functions for shared memory version
 *---------------------------------
 */
/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_ShMem_Create
 *----------------------------------------------------------------------------------
 * PURPOSE : Create a stack
 * INPUT   : buffer  - the working buffer for this stack in shared memory.
 *           size    - the max number of elements in the stack.
 * OUTPUT  : stack_p - the descriptor
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : 1. the max value of size is defined by L_STACK_MAX_ELEMENT_NUMBER
 *           2. if create fail, return NULL
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_ShMem_Create(L_STACK_ShMem_Desc_T *stack_p, void *buffer, UI32_T size)
{
    if(size <= 0 || size > L_STACK_MAX_ELEMENT_NUMBER)
    {
        return FALSE;
    }

    if(buffer == NULL)
    {
        return FALSE;
    }

    stack_p->magic_word = L_STACK_MAGIC_WORD;
    stack_p->max_number_of_element = size;
    stack_p->number_of_element = 0;
    stack_p->stack_buf_offset = L_CVRT_GET_OFFSET(stack_p, buffer);

    return TRUE;

} /* End of L_STACK_ShMem_Create() */

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
BOOL_T L_STACK_ShMem_Destroy(L_STACK_ShMem_Desc_T *stack_p)
{
    /* nothing to do.
     */
    return TRUE;

} /* End of L_STACK_ShMem_Destroy() */

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
BOOL_T L_STACK_ShMem_Pop(L_STACK_ShMem_Desc_T *stack_p, UI32_T *value)
{
    if(stack_p->magic_word != L_STACK_MAGIC_WORD)
    {
        return FALSE;
    }

    if(stack_p->number_of_element == 0)
    {
        return FALSE;
    }

    *value = L_STACK_SHMEM_DATA(stack_p)[--stack_p->number_of_element];
    return TRUE;

} /* End of L_STACK_ShMem_Pop() */

/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_ShMem_GetNumberOfElement
 *----------------------------------------------------------------------------------
 * PURPOSE : Get the element counters of  the stack
 * INPUT   : stack_p - the descriptor
 * OUTPUT  : *counters  -  element counters.
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */
BOOL_T L_STACK_ShMem_GetNumberOfElement(L_STACK_ShMem_Desc_T *stack_p, UI16_T *counters)
{
    if(stack_p->magic_word != L_STACK_MAGIC_WORD)
    {
        return FALSE;
    }
    *counters = stack_p->number_of_element;
    return TRUE;

} /* End of L_STACK_ShMem_Pop() */
/*----------------------------------------------------------------------------------
 * FUNCTION NAME : L_STACK_ShMem_GetElementValueByindex
 *----------------------------------------------------------------------------------
 * PURPOSE : Get the value by the element index
 * INPUT   : stack_p - the descriptor
                     index
 * OUTPUT  : *value  - the value of the element.
 * RETURN  : TRUE  - success
 *           FALSE - failure
 * NOTE    : None
 *---------------------------------------------------------------------------------
 */

BOOL_T L_STACK_ShMem_GetElementValueByindex(L_STACK_ShMem_Desc_T *stack_p, UI16_T index,UI32_T *value)
{
    if(stack_p->magic_word != L_STACK_MAGIC_WORD)
    {
        return FALSE;
    }

    *value = L_STACK_SHMEM_DATA(stack_p)[index];
    return TRUE;

} /* End of L_STACK_ShMem_Pop() */

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
BOOL_T L_STACK_ShMem_Push(L_STACK_ShMem_Desc_T *stack_p, UI32_T value)
{
    if(stack_p->magic_word != L_STACK_MAGIC_WORD)
    {
        return FALSE;
    }

    if(stack_p->number_of_element == stack_p->max_number_of_element)
    {
        return FALSE;
    }

    L_STACK_SHMEM_DATA(stack_p)[stack_p->number_of_element++] = value;

    return TRUE;

} /* End of L_STACK_ShMem_Push() */


#ifdef L_STACK_UNIT_TEST
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "l_stack.h"
#include "sysfun.h"

#define L_STACK_UNIT_TEST_SHMEM_KEY    10
#define L_STACK_UNIT_TEST_BUFFER_SIZE  1024
#define L_STACK_UNIT_TEST_MAGIC_FLAG   0x12345678

#define L_STACK_UNIT_TEST_STACK_SIZE   100

int L_STACK_ShMem_main()
{
    UI32_T shm_id;
    UI32_T *shm_p;
    L_STACK_ShMem_Desc_T *desc_p;
    int i;

    srand(time(NULL));

    assert(SYSFUN_CreateShMem(L_STACK_UNIT_TEST_SHMEM_KEY, L_STACK_UNIT_TEST_BUFFER_SIZE, &shm_id) == SYSFUN_OK);

    assert(shm_p = SYSFUN_AttachShMem(shm_id));
    desc_p = shm_p + 1;

    if (*shm_p != L_STACK_UNIT_TEST_MAGIC_FLAG)
    {
        assert(sizeof(UI32_T) + sizeof(L_STACK_ShMem_Desc_T) + L_STACK_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(L_STACK_UNIT_TEST_STACK_SIZE) <= L_STACK_UNIT_TEST_BUFFER_SIZE);
        assert(L_STACK_ShMem_Create(desc_p, (char *)desc_p + sizeof(*desc_p), L_STACK_UNIT_TEST_STACK_SIZE));
        *shm_p = L_STACK_UNIT_TEST_MAGIC_FLAG;
    }

    for (i = 0; i < 100; i++)
    {
        UI32_T value;

        switch (rand() % 2)
        {
            case 0:
                if (L_STACK_ShMem_Pop(desc_p, &value))
                    printf("pop %u\t", value);
                else
                    printf("empty\t");
                break;

            default:
                value = rand() % 10;
                if (L_STACK_ShMem_Push(desc_p, value))
                    printf("push %u\t", value);
                else
                    printf("full\t");
        }
    }

    printf("\n");

    SYSFUN_DetachShMem(desc_p);

    return 0;
}
#endif /* L_STACK_UNIT_TEST */

/* End of l_stack.c */
