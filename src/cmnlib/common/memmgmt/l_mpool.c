/* Module Name: L_MPOOL.C
 * Purpose:
 *      L_MPOOL Keeps all utilities for IP_TASK, Phase2. Eg. Memory management.
 *		For caller, just create the object, specify amount of memory block and size,
 *		then use it, allocate or free. When no more use it, please destroy it.
 *
 * Notes:
 *      1. Memory pool allocated from malloc(), system function, system memory.
 *		2. Usage :
 *			L_MPOOL_HANDLE	handle;
 *			handle = L_MPOOL_CreateBlockPool(blk_size, blk_number);
 *			if (L_MPOOL_AllocateBlock(handle, &blk_pointer))
 *			{
 *				if (! L_MPOOL_FreeBlock(handle, blk_pointer)
 *					showErrorMessage();
 *			}
 *			L_MPOOL_DestroyBlockPool (handle);
 *
 * History:
 *       Date       -- Modifier,  Reason
 *  0.1 2001.08.20  --  William, Created
 *	0.2	2001.12.13	--	William, remove #include "VxWorks.h"
 *	0.3	2001.12.18	--	William, Change L_MPOOL_CreateBlockPool() spec.
 *								 Return handle not just TRUE/FALSE.
 *
 * Copyright(C)      Accton Corporation, 1999, 2000   				
 */

/* INCLUDE FILE DECLARATIONS
 */
#ifdef _DEBUG
#include <malloc.h>
#include <memory.h>
#include <string.h>
#endif

#include <string.h>
#include <stdlib.h>
#include "sys_type.h"
#include "sysfun.h"
#include "l_pt.h"
#include "l_mpool.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define	L_MPOOL_HANDLE_MARKER			0x4a4d504c


/* MACRO FUNCTION DECLARATIONS
 */
extern	void	DBG_Print (char*);


/* DATA TYPE DECLARATIONS
 */
typedef	struct L_MPOOL_OBJ_S
{
	UI32_T				mpool_tag;		/* 	should be "IMPL"	*/
	UI32_T				own_task_id;	/*	the owner task ID	*/
	L_PT_Descriptor_T	pool_desc;		/*	the real pool holder*/
}	L_MPOOL_OBJ_T;


/* LOCAL SUBPROGRAM DECLARATIONS
 */
static BOOL_T  L_MPOOL_VerifyHandle (L_MPOOL_HANDLE desc);


/* STATIC VARIABLE DECLARATIONS
 */


/* EXPORTED SUBPROGRAM BODIES
 */


/* FUNCTION NAME : L_MPOOL_CreateBlockPool
 * PURPOSE:
 *      Build a block pool specified as caller described.
 *      Allocate one large block of memory (array of block) from malloc,
 *      and initialized by L_PT.
 * INPUT:
 *      block_size  -- size of each block, the allocating unit.
 *      block_no    -- total number of block preserved.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *		NULL	--	No more space.
 *		others	--	the handler of L_MPOOL object.
 *      	    	descriptor address; descriptor keeps partition information,
 *                  used in later operations, allocate, free, destroy.
 *                  After creation, descriptor's fields will be updated to
 *                  proper value.
 *
 * NOTES :
 *      1. 'buffer' in descriptor must be NULL (0); otherwise, assumes
 *          descriptor is in using.
 *      2. The allocating size will be adjusted to proper value (alignment 4 bytes).
 *
 *		2001.12.18, Change spec. Here is the old spec.
 *			BOOL_T  L_MPOOL_CreateBlockPool (UI32_T desc, UI32_T block_size, UI32_T block_no)
 *      			TRUE    --  Partition is created.
 *      			FALSE   --  No space could be use or partition already ceated.
 */
L_MPOOL_HANDLE  L_MPOOL_CreateBlockPool (UI32_T block_size, UI32_T block_no)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_MPOOL_OBJ_T		*p_handle;
	L_PT_Descriptor_T	*p_desc;
    UI32_T  			total_pool_size;
    UI32_T  			adjusted_block_size;
    void    			*pool;
    /* BODY
     */
/*
    printf (" L_MPOOL_CreateBlockPool : Create -- (desc=%x, size=%d, no=%d, buffer=%x)\n",
                (int) desc, (int)block_size, (int)block_no, (int)desc_p->buffer);
*/
    if ((block_size==0)||(block_no==0))
    {
        DBG_PrintText (" L_MPOOL_CreateBlockPool : invalid arg.\n");
        return  0;      /*  no space pool is strange ! */
    }

	/*	allocate desciptor from L_MEM	*/
	if ((p_handle=(L_MPOOL_OBJ_T*) malloc(sizeof(L_MPOOL_OBJ_T)))==NULL)
		return	0;
	else
	{
		memset ((char*)p_handle, 0, sizeof(L_MPOOL_OBJ_T));
		p_handle->mpool_tag 	= L_MPOOL_HANDLE_MARKER;
		p_handle->own_task_id	= SYSFUN_TaskIdSelf();
	}

    /*  Alignment to 4 bytes boundary   */
    adjusted_block_size = ( (block_size+3) & 0xfffffffc);
    /*  Allocate space and create the partition */
    total_pool_size = adjusted_block_size * block_no;
/*
	printf (" L_MPOOL : allocate space size =%d\n", (int)total_pool_size);
*/
    if ((pool=(void*)malloc (total_pool_size))==NULL)
    {
        DBG_PrintText (" L_MPOOL_CreateBlockPool : Mem-Alloc failure.\n");
        free(p_handle);
        return  0;      /*  No more space   */
    }
	p_desc 				  = &p_handle->pool_desc;
    p_desc->buffer        = (char*) pool;
    p_desc->buffer_len    = total_pool_size;
    p_desc->partition_len = adjusted_block_size;
    if (!L_PT_Create(p_desc))
    {
        free (pool);
        free(p_handle);
        DBG_PrintText (" L_MPOOL_CreateBlockPool : L_PT failure.\n");
        return  0;
    }
/*
    DBG_PrintText (" L_MPOOL_CreateBlockPool : buffer created.\n");
*/
    return ((L_MPOOL_HANDLE) p_handle);
}   /*  End of L_MPOOL_CreateBlockPool  */


/* FUNCTION NAME : L_MPOOL_DestroyBlockPool
 * PURPOSE:
 *      Free all space used by this descriptor.
 * INPUT:
 *      desc    -- descriptor address; keep track of partition information.
 *
 * OUTPUT:
 *      desc    -- descriptor address; All fields of descriptor are cleared,
 *                 at least 'buffer' is set to NULL(0).
 *
 * RETURN:
 *      TRUE    -- Successfully free.
 *      FALSE   -- Can't destroy; no space is allocated or
 *                 invalid descriptor.
 *
 * NOTES:
 *      None.
 */
BOOL_T  L_MPOOL_DestroyBlockPool (L_MPOOL_HANDLE desc)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_MPOOL_OBJ_T		*p_handle;
    /* BODY
     */
    /*	Verify the desc is L_MPOOL handle or not	*/
    if (!L_MPOOL_VerifyHandle(desc))
    	return	FALSE;

	p_handle	= (L_MPOOL_OBJ_T*) desc;
	
    free (p_handle->pool_desc.buffer);
    memset ((char*)p_handle, 0, sizeof(L_MPOOL_OBJ_T));
    free (p_handle);
    return  TRUE;
}   /*  End of L_MPOOL_DestroyBlockPool  */


/* FUNCTION NAME : L_MPOOL_RecreateBlockPool
 * PURPOSE:
 *      Recreate space been allocated, no allocating/free is performed.
 * INPUT:
 *      desc    -- descriptor address; keep track of partition information.
 *
 * OUTPUT:
 *      desc    -- descriptor address; All fields of descriptor are reinitialized.
 *
 * RETURN:
 *      TRUE    -- Successfully.
 *      FALSE   -- Invalid descriptor; no space is allocated or
 *                 invalid descriptor.
 *
 * NOTES:
 *      None.
 */
BOOL_T  L_MPOOL_RecreateBlockPool (L_MPOOL_HANDLE desc)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_MPOOL_OBJ_T		*p_handle;

    /* BODY
     */
    /*	Verify the desc is L_MPOOL handle or not	*/
    if (!L_MPOOL_VerifyHandle(desc))
    	return	FALSE;

	p_handle	= (L_MPOOL_OBJ_T*) desc;

    if (!L_PT_Create(&p_handle->pool_desc))
        return  FALSE;
    return  TRUE;
}   /*  End of L_MPOOL_RecreateBlockPool    */


/* FUNCTION NAME : L_MPOOL_AllocateBlock
 * PURPOSE:
 *      Allocate one block from block-pool.
 * INPUT:
 *      desc    -- descriptor address; this descriptor keeps track pool
 *                 usage.
 *
 * OUTPUT:
 *      pptr    -- Address of a pointer; saves address of the allocated
 *                 block.
 *
 * RETURN:
 *      TRUE    -- Block is allocated.
 *      FALSE   -- No more block could be allocated or invalid descriptor.
 *
 * NOTES:
 *          (  Something must be known to use this function. )
 */
BOOL_T  L_MPOOL_AllocateBlock (L_MPOOL_HANDLE desc, void **pptr)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_MPOOL_OBJ_T		*p_handle;

    /* BODY
     */
    /*	Verify the desc is L_MPOOL handle or not	*/
    if ((pptr==0)||(!L_MPOOL_VerifyHandle(desc)))
    	return	FALSE;

	p_handle	= (L_MPOOL_OBJ_T*) desc;

	*pptr = 0;

    if ((*pptr = L_PT_Allocate (&p_handle->pool_desc))==NULL)
    {
        DBG_PrintText (" L_MPOOL_AllocateBlock : L_PT allocate fail.\n");
        return  FALSE;
    }
/*
	printf (" L_MPOOL_AllocateBlock : ptr=%08x\n", (int) *pptr);
*/
    return  TRUE;
}   /*  End of L_MPOOL_AllocateBlock  */


/* FUNCTION NAME : L_MPOOL_FreeBlock
 * PURPOSE:
 *      Free a block to block-pool.
 * INPUT:
 *      desc    -- descriptor address; this descriptor keeps track pool
 *                 usage.
 *      ptr     -- address of block to be free.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE    --  Successfully free the block.
 *      FALSE   --  Invalid descriptor or block address.
 *
 * NOTES:
 *          (  Something must be known to use this function. )
 */
BOOL_T  L_MPOOL_FreeBlock (L_MPOOL_HANDLE desc, void *ptr)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    /* LOCAL VARIABLES DEFINITION
     */
    L_MPOOL_OBJ_T		*p_handle;

    /* BODY
     */
    /*	Verify the desc is L_MPOOL handle or not	*/
    if ((ptr==0)||(!L_MPOOL_VerifyHandle(desc)))
    	return	FALSE;

	p_handle	= (L_MPOOL_OBJ_T*) desc;

    L_PT_Free (&p_handle->pool_desc, ptr);
    return  TRUE;
}   /*  End of L_MPOOL_FreeBlock  */



/*===========================
 * LOCAL SUBPROGRAM BODIES
 *===========================
 */
/* FUNCTION NAME : L_MPOOL_VerifyHandle
 * PURPOSE:
 *      Free a block to block-pool.
 * INPUT:
 *      desc    -- descriptor address; this descriptor keeps track pool
 *                 usage.
 *      ptr     -- address of block to be free.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      TRUE    --  Successfully free the block.
 *      FALSE   --  Invalid descriptor or block address.
 *
 * NOTES:
 *          (  Something must be known to use this function. )
 */
static BOOL_T  L_MPOOL_VerifyHandle (L_MPOOL_HANDLE desc)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    /* BODY
     */
    if ((0==desc)||(L_MPOOL_HANDLE_MARKER!=((L_MPOOL_OBJ_T*)desc)->mpool_tag))
    	return	FALSE;
    else
    	return	TRUE;
}	/*	end of L_MPOOL_VerifyHandle	*/


