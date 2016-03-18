// #include <bspfuncs.h>

#include  "sys_type.h"
#include  "l_cvrt.h"
#include  "l_pt.h"


/* NAMING CONSTANT DEFINITION
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* EXPORTED FUNCTIONS BODY */
BOOL_T L_PT_Create (L_PT_Descriptor_T *desc_p)
{
	int     i, buf_nbr;
    UI32_T  *buf_p;
    UI32_T  next_position;

    /* BODY */
    desc_p->free = 0;

    /* adjust partition length, must be a multiple of 4 */
    desc_p->partition_len = (desc_p->partition_len+3)/4 * 4;

    buf_nbr = desc_p->buffer_len / desc_p->partition_len;

    desc_p->free = buf_p = (UI32_T *)desc_p->buffer;

    for (i = 0; i < (buf_nbr-1); i++)
	{
        next_position = ((UI32_T)buf_p ) + desc_p->partition_len;
        *buf_p = next_position;
        buf_p = (UI32_T *) next_position;
	}
    *buf_p = 0;

    desc_p->free_no = buf_nbr;

    return TRUE;
}/* STA_IniL_PT_Create */

void  *L_PT_Allocate (L_PT_Descriptor_T *desc_p)
{
    UI32_T   *dp;

	if ((dp = desc_p->free) != 0)
    {
        desc_p->free = (void *) *dp;
        desc_p->free_no--;
    }

	return dp;
} /* L_PT_Allocate */




BOOL_T  L_PT_Free (L_PT_Descriptor_T *desc_p, void *ptr)
{
    UI32_T *dp = ptr;

    if ( ptr == 0 )
        return FALSE;

    /* check range */

    if ( ((UI32_T)ptr < (UI32_T) desc_p->buffer) ||
        ((UI32_T)ptr >= (UI32_T) (desc_p->buffer+desc_p->buffer_len) ) )
        return FALSE;

    if ( ((UI32_T)ptr - (UI32_T)desc_p->buffer) % desc_p->partition_len )
        return FALSE;

    *dp = (UI32_T)desc_p->free;
    desc_p->free = dp;
    desc_p->free_no++;

    return TRUE;

}/* End of L_PT_Free() */





BOOL_T L_PT_ExtCreate (L_PT_ExtDescriptor_T *desc_p)
{
	int     i, buf_nbr;
    UI32_T  *buf_p;
    UI32_T  next_position;

    /* BODY */
    desc_p->free = 0;

    /* adjust partition length, must be a multiple of 4 */
    desc_p->partition_len = (desc_p->partition_len+3)/4 * 4;

    buf_nbr = desc_p->buffer_len / desc_p->partition_len;

    desc_p->free = buf_p = (UI32_T *)desc_p->buffer;

    for (i = 0; i < (buf_nbr-1); i++)
	{
        next_position = ((UI32_T)buf_p ) + desc_p->partition_len;
        *buf_p = next_position;
        buf_p = (UI32_T *) next_position;
	}
    desc_p->tail = buf_p;

    *buf_p = 0;

    desc_p->free_no = buf_nbr;

    return TRUE;
}/* STA_IniL_PT_ExtCreate */

void  *L_PT_ExtAllocate (L_PT_ExtDescriptor_T *desc_p)
{
    UI32_T   *dp;

	if ((dp = desc_p->free) != 0)
    {
        if ((desc_p->free = (void *) *dp) == 0 )
            desc_p->tail = 0;
        desc_p->free_no--;
    }

	return dp;
} /* L_PT_ExtAllocate */




BOOL_T  L_PT_ExtFree (L_PT_ExtDescriptor_T *desc_p, void *ptr, BOOL_T is_free_to_head)
{
    UI32_T *dp = ptr;

    if ( ptr == 0 )
        return FALSE;

    /* check range */

    if ( ((UI32_T)ptr < (UI32_T) desc_p->buffer) ||
        ((UI32_T)ptr >= (UI32_T) (desc_p->buffer+desc_p->buffer_len) ) )
        return FALSE;

    if ( ((UI32_T)ptr - (UI32_T)desc_p->buffer) % desc_p->partition_len )
        return FALSE;


    if (is_free_to_head)
    {
        *dp = (UI32_T)desc_p->free;
        desc_p->free = dp;
        if ( desc_p->tail == 0 )
            desc_p->tail = dp;
    }
    else
    {
        if ( desc_p->tail == 0 )
        {
            desc_p->tail = desc_p->free = dp;
            *dp=0;
        }
        else
        {
            *(UI32_T*)desc_p->tail = (UI32_T)dp;
            desc_p->tail = dp;
            *dp = 0;
        }
    }
    desc_p->free_no++;

    return TRUE;

}/* End of L_PT_ExtFree() */


/*-------------------------
 * L_PT for shared memory
 *-------------------------
 */

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_PT_ShMem_Create
 *------------------------------------------------------------------------
 * FUNCTION: Create a partition
 * INPUT:    descriptor->buffer_offset -- the offset of the buffer
 *           descriptor->buffer_len (must > partition_len)
 *           descriptor->partition_len (must a multiple of 4)
 *
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 *------------------------------------------------------------------------*/
BOOL_T L_PT_ShMem_Create (L_PT_ShMem_Descriptor_T *desc_p)
{
	int     i, buf_nbr;
    UI32_T  *buf_p;
    UI32_T  next_position;

    /* BODY */
    if (desc_p->buffer_offset == 0)
        return FALSE;

    /* adjust partition length, must be a multiple of 4 */
    desc_p->partition_len = (desc_p->partition_len+3)/4 * 4;

    buf_nbr = desc_p->buffer_len / desc_p->partition_len;

    desc_p->free_offset = desc_p->buffer_offset;
    buf_p = L_CVRT_GET_PTR(desc_p, desc_p->free_offset);

    for (i = 0; i < (buf_nbr-1); i++)
	{
        next_position = ((UI32_T)buf_p ) + desc_p->partition_len;
        *buf_p = L_CVRT_GET_OFFSET(desc_p, next_position);
        buf_p = (UI32_T *) next_position;
	}
    *buf_p = 0;

    desc_p->free_no = buf_nbr;

    return TRUE;
}/* L_PT_ShMem_Create */

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_PT_ShMem_Allocate
 *------------------------------------------------------------------------
 * FUNCTION: Allocate a buffer
 * INPUT:    desc_p    --  descriptor for L_PT on a shared memory
 * OUTPUT  : None
 * RETURN  : Non-null -- allocated buffer on a shared memory
 *           NULL     -- fail
 *------------------------------------------------------------------------*/

void  *L_PT_ShMem_Allocate (L_PT_ShMem_Descriptor_T *desc_p)
{
    UI32_T   *dp = NULL;

	if (desc_p->free_offset != 0)
    {
        dp = L_CVRT_GET_PTR(desc_p, desc_p->free_offset);
        desc_p->free_offset = *dp;
        desc_p->free_no--;
    }

	return dp;
} /* L_PT_ShMem_Allocate */

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_PT_ShMem_Free
 *------------------------------------------------------------------------
 * FUNCTION: Free an allocated partition.
 * INPUT:    desc_p  --  descriptor for L_PT on a shared memory
 *           ptr     --  allocated partition
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 *------------------------------------------------------------------------*/
BOOL_T L_PT_ShMem_Free(L_PT_ShMem_Descriptor_T *desc_p, void *ptr)
{
    void *buffer = L_CVRT_GET_PTR(desc_p, desc_p->buffer_offset);
    UI32_T *dp = ptr;

    if ( ptr == 0 )
        return FALSE;

    /* check range */

    if ( ((UI32_T)ptr < (UI32_T)buffer ) ||
        ((UI32_T)ptr >= (UI32_T)buffer + desc_p->buffer_len) )
        return FALSE;

    if ( ((UI32_T)ptr - (UI32_T)buffer) % desc_p->partition_len )
        return FALSE;

    *dp = desc_p->free_offset;
    desc_p->free_offset = L_CVRT_GET_OFFSET(desc_p, dp);
    desc_p->free_no++;

    return TRUE;
}


#ifdef _DEBUG
#include  <stdio.h>
//#include <conio.h>
//#include <ncurses.h>
static char buffer[1024];

void L_PT_main()
{
    int     cc;
    L_PT_Descriptor_T    pt_desc;

    char    *ptr=0;
    BOOL_T  ret;
    pt_desc.buffer = buffer;
    pt_desc.buffer_len = 1024;
    pt_desc.partition_len = 32;
    L_PT_Create (&pt_desc);

    while (1)
    {
        cc = getchar();

        if ( cc == 27 ) return;
        if ( cc =='1')
            ptr = L_PT_Allocate ( &pt_desc );
        else if ( cc == '2')
        {
            ret = L_PT_Free ( &pt_desc, ptr);
        }
    }

}





void L_PT_Extmain()
{
    int     cc;
    L_PT_ExtDescriptor_T    pt_desc;

    char    *ptr=0;
    BOOL_T  ret;
    pt_desc.buffer = buffer;
    pt_desc.buffer_len = 1024;
    pt_desc.partition_len = 32;
    L_PT_ExtCreate (&pt_desc);

    while (1)
    {
        cc = getchar();

        if ( cc == 27 ) return;
        if ( cc =='1')
        {
            ptr = L_PT_ExtAllocate ( &pt_desc );
            printf ("%X\r\n", ptr);
        }
        else if ( cc == '2')
        {
            ret = L_PT_ExtFree ( &pt_desc, ptr, FALSE);
        }
    }

}

void L_PT_ShMem_main()
{
    int     cc;
    L_PT_ShMem_Descriptor_T    pt_desc;

    char    *ptr=0;
    BOOL_T  ret;
    pt_desc.buffer_offset = L_CVRT_GET_OFFSET(&pt_desc, buffer);
    pt_desc.buffer_len = 1024;
    pt_desc.partition_len = 32;
    L_PT_ShMem_Create (&pt_desc);

    while (1)
    {
        cc = getchar();

        if ( cc == 27 ) return;
        if ( cc =='1')
            ptr = L_PT_ShMem_Allocate ( &pt_desc );
        else if ( cc == '2')
        {
            ret = L_PT_ShMem_Free ( &pt_desc, ptr);
        }
        DBG_DumpHex("pt_desc", sizeof(pt_desc), &pt_desc);
    }

}

#endif
