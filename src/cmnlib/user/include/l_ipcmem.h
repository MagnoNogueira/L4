/* MODULE NAME:  L_IPCMEM.h
 * PURPOSE:
 * This module is for allocating and freeing buffer for lan packet transmission.
 *
 * NOTES:
 * This module need to cooperate with mem device.
 * Before using this library, ensure that "/dev/mem" must have been created.
 * Note that the behavior of /dev/mem of linux 2.4 and linux 2.6 is slightly
 * different. The current implementation is for linux 2.6.
 *
 * HISTORY
 *    2006.9.21 - Charlie Chen, Created
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

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/*
 * EXPORTED FUNCTIONS DECLARATION
 */

/*  FUNCTION NAME : L_IPCMEM_Preinitialize_SetVirtualBaseAddress
 *  PURPOSE:
 *      Set the start address of memory that IPCMEM will use.
 *
 *  INPUT:
 *      virtual_base_address    -- the start address
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      None.
 *
 *  NOTES:
 *      This function should only be called in driver process
 *      and before calling L_IPCMEM_Initialize().
 */
void L_IPCMEM_Preinitialize_SetVirtualBaseAddress(UI32_T virtual_base_address);

// temporarily
void L_IPCMEM_Preinitialize_InitMemoryPool(void *(*alloc_func)(unsigned int size));

/*  FUNCTION NAME : L_IPCMEM_Initialize
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
 *      TRUE  --  Sucess
 *      FALSE --  Fail
 *  NOTES:
 *      None.
 */
BOOL_T L_IPCMEM_Initialize(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - L_IPCMEM_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/
void L_IPCMEM_Create_InterCSC_Relation(void);

/*  FUNCTION NAME : L_IPCMEM_GetPtr
 *  PURPOSE:
 *      This function will convert the offset to the logical address.
 *  INPUT:
 *      offset  --  The offset to the beginning of the ipcmem.
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      The pointer to logical address of the specified offset.
 *      Return NULL if offset is invalid.
 *  NOTES:
 *      None.
 */
void* L_IPCMEM_GetPtr(UI32_T offset);

/*  FUNCTION NAME : L_IPCMEM_GetOffset
 *  PURPOSE:
 *      Get the offset to the beginning of L_IPCMEM memory address.
 *
 *  INPUT:
 *      buf_p -- the buffer allocated on the region of IPCMEM.
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      Sucess -- the offset to the beginning of IPCMEM memory address of the given buffer
 *      Fail   -- a negative value will be returned if buf_p is invalid.
 *
 *  NOTES:
 *      None.
 */
I32_T L_IPCMEM_GetOffset(void* buf_p);

/*  FUNCTION NAME : L_IPCMEM_GetPhysicalBaseAddress
 *  PURPOSE:
 *      Get the physical address of the beginning of IPCMEM memory address.
 *
 *  INPUT:
 *      None.
 *
 *  OUTPUT:
 *      physical_base_address - Physical address of the beginning of IPCMEM.
 *
 *  RETURN:
 *      Sucess -- return TRUE
 *      Fail   -- return FALSE
 *
 *  NOTES:
 *      TBD.
 */
BOOL_T L_IPCMEM_GetPhysicalBaseAddress(UI32_T *physical_base_address);

/*  FUNCTION NAME : L_IPCMEM_GetTxBufStartAddress
 *  PURPOSE:
 *      Get the starting address(logical address) of the Tx buffer.
 *
 *  OUTPUT:
 *      tx_buf_start_address -- the starting address of the Tx buffer
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      Sucess -- return TRUE
 *      Fail   -- return FALSE
 *
 *  NOTES:
 *      None.
 */
BOOL_T L_IPCMEM_GetTxBufStartAddress(UI32_T *tx_buf_start_address);

/*  FUNCTION NAME : L_IPCMEM_GetRxBufStartAddress
 *  PURPOSE:
 *      Get the starting address(logical address) of the Rx buffer.
 *
 *  OUTPUT:
 *      rx_buf_start_address -- the starting address of the Rx buffer
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      Sucess -- return TRUE
 *      Fail   -- return FALSE
 *
 *  NOTES:
 *      1.This API is for NIC to prepare its Rx buffer. Rx buffer
 *        is also located at the place just behind the buffer used by L_IPCMEM.
 *      2.On Linux, IPCMEM and Rx buffer for NIC are all located in the region
 *        of high memory.
 */
BOOL_T L_IPCMEM_GetRxBufStartAddress(UI32_T *rx_buf_start_address);

/*  FUNCTION NAME : L_IPCMEM_GetUCBufStartAddress
 *  PURPOSE:
 *      Get the starting address(logical address) of the UC buffer.
 *
 *  OUTPUT:
 *      uc_buf_start_address -- the starting address of the UC buffer
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      Sucess -- return TRUE
 *      Fail   -- return FALSE
 *
 *  NOTES:
 *      1.This API is for UC MGR to prepare its buffer.
 *      2.On Linux, UC buffer is located in the region of high memory.
 */
BOOL_T L_IPCMEM_GetUCBufStartAddress(UI32_T *uc_buf_start_address);

/*  FUNCTION NAME : L_IPCMEM_AttachUCBuffer
 *  PURPOSE:
 *      Attach the UC buffer.
 *
 *  OUTPUT:
 *      uc_physical_start_addr -- the physical starting address of the UC buffer
 *      uc_buf_start_address   -- the virtual starting address of the UC buffer
 *
 *  OUTPUT:
 *      None.
 *
 *  RETURN:
 *      Sucess -- return TRUE
 *      Fail   -- return FALSE
 *
 *  NOTES:
 *      1.This API is for UC MGR to prepare its buffer.
 *      2.On Linux, UC buffer is located in the region of high memory.
 */
BOOL_T L_IPCMEM_AttachUCBuffer(UI32_T uc_physical_start_addr, UI32_T *uc_buf_start_address);

#endif
