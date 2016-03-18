/* MODULE NAME:  L_IPCMEM.h
 * PURPOSE:
 * This module is for allocating and freeing buffer for lan packet transmission.
 * NOTES:
 *
 * HISTORY
 *    2006.09.21  -- Charlie Chen, Created
 *    2008.07.03  -- Tiger Liu, remove unused function, rename functions L_IPCMEM_XXX to K_L_IPCMEM_XXX.
 *
 * Copyright(C)      Accton Corporation, 2006
 */

#ifndef _L_IPCMEM_H
#define _L_IPCMEM_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define L_IPCMEM_MIN_SIZE       64
#define L_IPCMEM_MAX_SIZE       1800 /* 2004-04-21, Penny [Mercury_V3-00727]: enlarge the max size from 1600 to 1800 */

/* The type of dedicated buffer pool.
 */
enum {
    L_IPCMEM_DEDICATED_BUF_TYPE_STKTPLG_HBT1,
#if (SYS_CPNT_DHCP == TRUE)
    L_IPCMEM_DEDICATED_BUF_TYPE_DHCP_TX,
#endif
};

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/*
 * EXPORTED FUNCTIONS DECLARATION
 */

/*  FUNCTION NAME : K_L_IPCMEM_Initialize
 *  PURPOSE:
 *      Initialize L_IPCMEM module.
 *
 *  INPUT:
 *      None.
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      None.
 *  NOTES:
 *      None.
 */
void K_L_IPCMEM_Initialize(void);

/*  FUNCTION NAME : K_L_IPCMEM_Uninitialize
 *  PURPOSE:
 *      Uninitialize L_IPCMEM module.
 *
 *  INPUT:
 *      None.
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      None.
 *  NOTES:
 *      None.
 */
void K_L_IPCMEM_Uninitialize(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - K_L_IPCMEM_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/
void K_L_IPCMEM_Create_InterCSC_Relation(void);

/* 	FUNCTION NAME : K_L_IPCMEM_Allocate
 * 	PURPOSE:
 *  	This function is primary used for allocate buffer for packet transmission.
 *
 * 	INPUT:
 *   size    -- the size of the buffer to be allocated.
 *   user_id -- this id will be passed to L_MM for the memory monitor information.
 *              user_id = trace_id(6) + pool_id(2) + module_id(8)
 *              pool_id is at user's own usage/knowledge.
 *              user_id is got from macro L_MM_USER_ID (modul_id, pool_id, trace_id)
 *              or L_MM_USER_ID2(modul_id,ext_trace_id)
 *
 * 	OUTPUT:
 *   None.
 *
 * 	RETURN:
 *   Address of the allocated buffer
 *   Return null if no more buffer is available.
 *
 * 	NOTES:
 *   The allocated buffer can be monitored through l_mm backdoor.
 */
void *K_L_IPCMEM_Allocate(UI32_T size, UI16_T user_id);

/*  FUNCTION NAME : K_L_IPCMEM_Free
 *  PURPOSE:
 *      Free memory block to memory pool.
 *  INPUT:
 *      buf_p -- the pointer points to the memory block to be freed.
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      TRUE    -- Sucess.
 *      FALSE   -- Fail.
 *
 *  NOTES:
 *      None.
 */
BOOL_T K_L_IPCMEM_Free(void *buf_p);

/* 	FUNCTION NAME : K_L_IPCMEM_AllocateFromDedicatedBufPool
 * 	PURPOSE:
 *  	This function is used for allocate dedicated buffer for packet transmission.
 *
 * 	INPUT:
 *   buf_type -- the type of the buffer to be allocated.
 *   user_id -- this id will be passed to L_MM for the memory monitor information.
 *              user_id = trace_id(6) + pool_id(2) + module_id(8)
 *              pool_id is at user's own usage/knowledge.
 *              user_id is got from macro L_MM_USER_ID (modul_id, pool_id, trace_id)
 *              or L_MM_USER_ID2(modul_id,ext_trace_id)
 *
 * 	OUTPUT:
 *   buf_size_p -- allocated size of buffer.
 *
 * 	RETURN:
 *   Address of the allocated buffer
 *   Return null if no more buffer is available.
 *
 * 	NOTES:
 *   The allocated buffer can be monitored through l_mm backdoor.
 */
void *K_L_IPCMEM_AllocateFromDedicatedBufPool(UI32_T buf_type, UI32_T user_id, UI32_T *buf_size_p);

/*  FUNCTION NAME : K_L_IPCMEM_GetStartOfMemBufferAddr
 *  PURPOSE:
 *      This function will return the start address(This address includes
 *      IPCMEM memory buffer) of the IPCMEM memory buffer.
 *  INPUT:
 *      None.
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      The buffer of the start address of the IPCMEM memory buffer.
 *  NOTES:
 *      None.
 */
void* K_L_IPCMEM_GetStartOfMemBufferAddr(void);

/*  FUNCTION NAME : K_L_IPCMEM_GetEndOfMemBufferAddr
 *  PURPOSE:
 *      This function will return the end address(This address is not included
 *      in L_IPCMEM memory buffer) of the L_IPCMEM memory buffer.
 *  INPUT:
 *      None.
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      The buffer of the end address of the L_IPCMEM memory buffer.
 *  NOTES:
 *      Behind the IPCMEM memory buffer is the buffer for NIC Rx buffer.
 */
void* K_L_IPCMEM_GetEndOfMemBufferAddr(void);

/*  FUNCTION NAME : K_L_IPCMEM_GetOffset
 *  PURPOSE:
 *      Get the offset to the beginning of IPCMEM memory address of
 *      the given buffer.
 *  INPUT:
 *      buf_p -- the buffer allocated by K_L_IPCMEM_Allocate
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      Sucess -- the offset to the beginning of IPCMEM memory address.
 *      Fail   -- a negative value will be returned if buf_p is invalid.
 *
 *  NOTES:
 *
 */
UI32_T K_L_IPCMEM_GetOffset(void* buf_p);

/*  FUNCTION NAME : K_L_IPCMEM_GetPtr
 *  PURPOSE:
 *      This function will convert the offset to the real address.
 *  INPUT:
 *      None.
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      Sucess -- The logical address of the specified offset.
 *      Fail   -- NULL
 *  NOTES:
 *      None.
 */
void* K_L_IPCMEM_GetPtr(UI32_T offset);

/*  FUNCTION NAME : K_L_IPCMEM_GetMRefBufStartAddress
 *  PURPOSE:
 *      Get the starting address(logical address) of the mref buffer.
 *
 *  OUTPUT:
 *      mref_buf_start_address -- the starting address of the mref buffer
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      Sucess -- return TRUE
 *      Fail   -- return FALSE
 *
 *  NOTES:
 *      1.This API is for L_MM to prepare mref buffer.
 *      2.On Linux, mref buffer is located in the region of high memory.
 */
BOOL_T K_L_IPCMEM_GetMRefBufStartAddress(UI32_T *mref_buf_start_address);


#endif

