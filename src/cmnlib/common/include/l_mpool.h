/* Module Name: L_MPOOL.H
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

#ifndef 	L_MPOOL_H
#define 	L_MPOOL_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"


/* NAME CONSTANT DECLARATIONS
 */


/* MACRO FUNCTION DECLARATIONS
 */


/* DATA TYPE DECLARATIONS
 */
typedef	UI32_T		L_MPOOL_HANDLE;


/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */




/*-------------------------------
 *  Memory Partition Management
 *-------------------------------
 *  All functions is based on L_MEM, L_PT.
 *  Memory allocated from L_MEM.
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
 */
L_MPOOL_HANDLE  L_MPOOL_CreateBlockPool (UI32_T block_size, UI32_T block_no);


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
 *      FALSE   -- Invalid descriptor; no space is allocated or
 *                 invalid descriptor.
 *
 * NOTES:
 *      None.
 */
BOOL_T  L_MPOOL_DestroyBlockPool (L_MPOOL_HANDLE desc);


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
BOOL_T  L_MPOOL_RecreateBlockPool (L_MPOOL_HANDLE desc);



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
BOOL_T  L_MPOOL_AllocateBlock (L_MPOOL_HANDLE desc, void **pptr);


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
BOOL_T  L_MPOOL_FreeBlock (L_MPOOL_HANDLE desc, void *ptr);



#endif   /* L_MPOOL_H */
