/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtOsMem.h
*
* DESCRIPTION:
*       Operating System wrapper. Memory manipulation facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 19 $
*******************************************************************************/

#ifndef __gtOsMemh
#define __gtOsMemh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

/************* Defines ********************************************************/

#define OSMEM_DEFAULT_BLOCK_STEP_SIZE 4
#define OSMEM_DEFAULT_STEPS_LIMIT     128
#define OSMEM_DEFAULT_POOLS_LIMIT     65536

/* Obsolete will be removed in the next version */
/* #define OSMEM_DEFAULT_MEM_INIT_SIZE   2048*1024 */

#define OSMEM_MAX_MEMORY_SIZE_USE 64*1024*1024 /* 64MB */

/* don't disable debug if UTs included in the build! */
#define OSMEMPOOLS_DEBUG

/*
 * Typedef: struct OSMEM_POOL_FREE_ELEM
 *
 * Description:
 *      This structure represent a node in the pool's free list.
 *
 * Fields:
 *      next_elem     - next node in the memory pool's list.
 *      size          - the size of the memory block (used only when the various
 *                      sizes pool).
 *
 */

typedef struct os_mem_pool_free_elem{
    struct os_mem_pool_free_elem    *next_elem;
    GT_U32                          size;
}OSMEM_POOL_FREE_ELEM,*OSMEM_POOL_FREE_ELEM_PTR;


/*
 * Typedef: struct OSMEM_POOL_ELEM
 *
 * Description:
 *      This structure represent a node followed by a memory block in the memory
 *      linked list managed by the memory pools.
 *
 * Fields:
 *        poolIndex -     real required for dynamic allocation size
 *                for example if the required size is 150 bytes but the allocated pool 
 *                length is 256 bytes this  field will be set to 150
 * 
 */

typedef struct os_mem_pool_elem{
    GT_U32                      poolIndex;
}OSMEM_POOL_ELEM,*OSMEM_POOL_ELEM_PTR;

/*
 * Typedef: struct OSMEM_VAR_POOL_ELEM
 *
 * Description:
 *      This structure represent a node followed by a memory block in the 
 *      various sizes memory linked list managed by the memory pools.
 *
 * Fields:
 *      poolIndex     - the index of the pool which this element belongs to.
 *      size          - the size of the element.
 *
 */

typedef struct os_mem_var_pool_elem{
    GT_U32                      size;
    GT_U32                      poolIndex;
}OSMEM_VAR_POOL_ELEM,*OSMEM_VAR_POOL_ELEM_PTR;
/*
 * Typedef: struct OSMEM_POOL_HDR
 *
 * Description:
 *      This structure holds the header of a list of memory managed pool for
 *      the osMalloc mechanism.
 *
 * Fields:
 *      poolIndex          - the index in the pool array of this pool.
 *      elem_size          - the size of elements this pool manage (if 0 see
 *                           osMemPoolsInit).
 *      free_size          - the size of free space in this pool
 *      size               - the size of the memory pool (allocated + free).
 *                           was allocated.
 *      alloc_times        - the amount of times alloc was called to allocated
 *                           memory from this pool.
 *      free_times         - the amount of times free was called to free memory
 *                           from this pool.
 *      malloc_used        - the amount of times this pool was needed to use
 *                           malloc.
 *      chunkIndex         - the index of the chunk from which the next element
 *                           should be allocated from.
 *      first_free_PTR     - pointer to the first free element.
 *
 */

typedef struct {
    GT_U32                      poolIndex;
    GT_U32                      elem_size;
    GT_U32                      free_size;
    GT_U32                      overAlloc;
    GT_U32                      alloc_times;
    GT_U32                      free_times;
    GT_U32                      malloc_used;
    GT_U32                      size;

    GT_U32                      chunkIndex;

    struct os_mem_pool_free_elem    *first_free_PTR;
      
}OSMEM_POOL_HDR,*OSMEM_POOL_HDR_PTR;


/*
 * Typedef: struct OSMEM_CHUNK_POINTER
 *
 * Description:
 *      This structure represent a chunk of memory allocated for the memory
 *      manager.
 *
 * Fields:
 *      startAddr   - a pointer to the beginning of the chunk.
 *      dynamicAddr - a pointer to the end of the dynamic memory allocated from
 *                    this chunk.
 *      staticAddr  - a pointer to the beginning of the static memory allocated
 *                    from this chunk.
 *      freeSize    - the current free size in the chunk.
 *
 */

typedef struct os_mem_chunk_pointer{
    GT_CHAR*                    startAddr;
    GT_CHAR*                    dynamicAddr;
    GT_CHAR*                    staticAddr;
    GT_U32                      freeSize;

}OSMEM_CHUNK_POINTER,*OSMEM_CHUNK_POINTER_PTR;



/*******************************************************************************
* GT_MEMORY_ALLOC_FUNC
*
* DESCRIPTION:
*       Allocates memory
*
* INPUTS:
*       size - size in bytes
*
* OUTPUTS:
*       None
*
* RETURNS:
*       A pointer to the allocated memory or NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_VOID* (*GT_MEMORY_ALLOC_FUNC)
(
    unsigned int size
);

/*******************************************************************************
* GT_MEMORY_FREE_FUNC
*
* DESCRIPTION:
*       Free allocated memory
*
* INPUTS:
*       ptr - A pointer to the allocated memory
*
* OUTPUTS:
*       None
*
* RETURNS:
*       A pointer to the allocated memory or NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_VOID (*GT_MEMORY_FREE_FUNC)
(
    GT_VOID *ptr
);


/*
 * typedef: enum GT_PSS_VERSION_ENT
 *
 * Description: This enumeration defines the memory allocation method
 *
 * Fields:
 *      GT_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E - in the case of memory 
 *                          chunk depletion an out of memory will be reported
 *      GT_MEMORY_MANAGE_METHOD_REALLOC_ON_DEMAND_E - in the case of memory 
 *                          chunk depletion a new chunk will be allocated
 */
typedef enum
{
    GT_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E,
    GT_MEMORY_MANAGE_METHOD_REALLOC_ON_DEMAND_E

}GT_MEMORY_MANAGE_METHOD_ENT;

/*
 * Typedef: struct OSMEM_CHUNK_ARRAY
 *
 * Description:
 *      This structure represent the array of chunks for the memory mangaer use
 *
 * Fields:
 *      chunkArray     - the array holding the pointer to the chunks.
 *      chunkSize      - the size of a chunk (one size for all).
 *      lastChunkIndex - a pointer to the beginning of the last allocated chunk.
 *      staticIndex    - the index from which to allocated the static memory.
 *      chunkArraySize - the size of the chunkArray
 *      memAllocFuncPtr - the function to be used in memory allocation.
 *      memAllocMethod - determines the method of the allocation 
 *
 */

typedef struct os_mem_chunk_array{
    OSMEM_CHUNK_POINTER_PTR         chunkArray;
    GT_U32                          chunkSize;
    GT_U32                          lastChunkIndex;
    GT_U32                          staticIndex;
    GT_U32                          chunkArraySize;
    GT_MEMORY_ALLOC_FUNC            memAllocFuncPtr;
    GT_MEMORY_FREE_FUNC             memFreeFuncPtr;
    GT_MEMORY_MANAGE_METHOD_ENT     memAllocMethod;

}OSMEM_CHUNK_ARRAY,*OSMEM_CHUNK_ARRAY_PTR;





/************* Functions ******************************************************/

/*******************************************************************************
* osBzero
*
* DESCRIPTION:
*       Fills the first nbytes characters of the specified buffer with 0
*
* INPUTS:
*       start  - start address of memory block to be zeroed
*       nbytes - size of block to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID osBzero
(
    IN GT_CHAR *start,
    IN GT_U32  nbytes
);

/*******************************************************************************
* osMemSet
*
* DESCRIPTION:
*       Stores 'simbol' converted to an unsigned char in each of the elements
*       of the array of unsigned char beginning at 'start', with size 'size'.
*
* INPUTS:
*       start  - start address of memory block for setting
*       simbol - character to store, converted to an unsigned char
*       size   - size of block to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to set memory block
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID * osMemSet
(
    IN GT_VOID * start,
    IN int       symbol,
    IN GT_U32    size
);

/*******************************************************************************
* osMemCpy
*
* DESCRIPTION:
*       Copies 'size' characters from the object pointed to by 'source' into
*       the object pointed to by 'destination'. If copying takes place between
*       objects that overlap, the behavior is undefined.
*
* INPUTS:
*       destination - destination of copy
*       source      - source of copy
*       size        - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to destination
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID * osMemCpy
(
    IN GT_VOID *       destination,
    IN const GT_VOID * source,
    IN GT_U32          size
);


/*******************************************************************************
* osMemCmp
*
* DESCRIPTION:
*       Compare 'size' characters from the object pointed to by 'str1' to
*       the object pointed to by 'str2'.
*
* INPUTS:
*       str1 - first memory area
*       str2 - second memory area
*       size - size of memory to compare
*
* OUTPUTS:
*       None
*
* RETURNS:
*       > 0  - if str1 is alfabetic bigger than str2
*       == 0 - if str1 is equal to str2
*       < 0  - if str1 is alfabetic smaller than str2
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_32 osMemCmp
(
    IN const GT_VOID  *str1,
    IN const GT_VOID  *str2,
    IN GT_U32          size
);


/*******************************************************************************
* osMemInit
*
* DESCRIPTION:
*       Initialize the memory management.
*
* INPUTS:
*       size            - memory size to manage.
*       reallocOnDemand - defines whether in the case of memory chunk depleation
*                         a new chunk will be allocated or an out of memory will
*                         be reported.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_NO_RESOURCE - not enough memory in the system.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMemInit
(
    IN GT_U32 size,
    IN GT_BOOL reallocOnDemand
);


/*******************************************************************************
* osMemLibInit
*
* DESCRIPTION:
*       Initialize the memory management with a user memory function. 
*       Can be used to allocate from a dedicated memory pool that remains till 
*       HW reset.
*
* INPUTS:
*       memAllocFuncPtr - the function to be used in memory allocation.
*       memFreeFuncPtr - the function to be used in memory allocation.
*       size - The memory size to manage
*       memAllocMethod - determines the method of the allocation 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_NO_RESOURCE - not enough memory in the system.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMemLibInit
(    
    IN GT_MEMORY_ALLOC_FUNC memAllocFuncPtr,
    IN GT_MEMORY_FREE_FUNC memFreeFuncPtr,
    IN GT_U32 size,
    IN GT_MEMORY_MANAGE_METHOD_ENT memAllocMethod
);


/*******************************************************************************
* osStaticAlloc
*
* DESCRIPTION:
*       Allocates memory block of specified size, this memory will not be free.
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       Usage of this function is only on FIRST initialization.
*
*******************************************************************************/
GT_VOID *osStaticMalloc
(
    IN GT_U32 size
);

/*******************************************************************************
* osMalloc
*
* DESCRIPTION:
*       Allocates memory block of specified size.
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
GT_VOID *osMalloc
(
    IN GT_U32 size
);

/*******************************************************************************
* osRealloc
*
* DESCRIPTION:
*       Reallocate memory block of specified size.
*
* INPUTS:
*       ptr  - pointer to previously allocated buffer
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*       Recommended to avoid usage of this function.
*
*******************************************************************************/
GT_VOID *osRealloc
(
    IN GT_VOID * ptr ,
    IN GT_U32    size
);

/*******************************************************************************
* osFree
*
* DESCRIPTION:
*       Deallocates or frees a specified memory block.
*
* INPUTS:
*       memblock - previously allocated memory block to be freed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
GT_VOID osFree
(
    IN GT_VOID* const memblock
);

/*******************************************************************************
* osCacheDmaMalloc
*
* DESCRIPTION:
*       Allocate a cache-safe buffer  of specified size for DMA devices
*       and drivers
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       A pointer to the cache-safe buffer, or NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID *osCacheDmaMalloc
(
    IN GT_U32 size
);

/*******************************************************************************
* osCacheDmaFree
*
* DESCRIPTION:
*       Free the buffer acquired with osCacheDmaMalloc()
*
* INPUTS:
*       ptr - pointer to malloc/free buffer
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osCacheDmaFree
(
    IN GT_VOID *ptr
);

/*******************************************************************************
* osPhy2Virt
*
* DESCRIPTION:
*       Converts physical address to virtual.
*
* INPUTS:
*       phyAddr  - physical address
*
* OUTPUTS:
*       virtAddr - virtual address
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osPhy2Virt
(
    IN  GT_UINTPTR phyAddr,
    OUT GT_UINTPTR *virtAddr
);

/*******************************************************************************
* osVirt2Phy
*
* DESCRIPTION:
*       Converts virtual address to physical.
*
* INPUTS:
*       virtAddr - virtual address
*
* OUTPUTS:
*       phyAddr  - physical address
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osVirt2Phy
(
    IN  GT_UINTPTR virtAddr,
    OUT GT_UINTPTR *phyAddr
);

/*******************************************************************************
* osMemPoolsClear
*
* DESCRIPTION:
*      returns all the allocated memory from all the pools to free.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if success.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMemPoolsClear
(
    GT_VOID
);

/*******************************************************************************
* osMemGetHeapBytesAllocated
*
* DESCRIPTION:
*       returns the number of bytes allocated in heap  
*
* INPUTS:
*        
*                             
* OUTPUTS:
*       None.
*
* RETURNS:
*
*       
* COMMENTS:
*
*******************************************************************************/
GT_U32 osMemGetHeapBytesAllocated(GT_VOID);

/*******************************************************************************
* osMemStartHeapAllocationCounter
*
* DESCRIPTION:
*        set the current value of heap allocated bytes to the allocation counter  
*
* INPUTS:
*        
*                     
* OUTPUTS:
*       None.
*
* RETURNS:
*   
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID osMemStartHeapAllocationCounter(GT_VOID);

/*******************************************************************************
* osMemGetHeapAllocationCounter
*
* DESCRIPTION:
*        returns the delta of current allocated bytes number and the value of allocation counter set by 
*        preveous startHeapAllocationCounter() function
*
* INPUTS:
*        
*                             
* OUTPUTS:
*       None.
*
* RETURNS:
*       
* COMMENTS:
*
*******************************************************************************/
GT_U32 osMemGetHeapAllocationCounter(GT_VOID);

#ifdef __cplusplus
}
#endif

#endif  /* __gtOsMemh */
/* Do Not Add Anything Below This Line */

