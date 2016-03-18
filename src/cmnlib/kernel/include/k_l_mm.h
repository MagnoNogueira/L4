/* Module Name: L_MM.H(Kernel Space)
 * Purpose:
 *  MM (Memory Management/Memory Monitor) provides memory allocation/free functions
 *    with Monitor Information included at the head of each allocated buffer.
 *    Therefore the usage of the allocated buffer can be easily traced.
 *  MM also provides MREF (Memory Reference/Multiple Reference) for a single data block to be
 *    multiplely accessed by tasks without replicating or reproducing the data block.
 *    Therefore the overhead of memory allocation/free/copy can be reduced to enhance the whole
 *    system's performance.
 *
 * Notes:
 *   The allocated buffer must have the following format with the Mornitor Information at the head of
 *   the allocated buffer by L_MM_Malloc(), K_L_MM_PtAlloc() is consisted of :
 *      buffer :=  K_L_MM_Monitor_T + Application's Data Block + trail Magic bytes(8 bytes)
 *   L_MM depends on L_IPCMEM on linux, so it is necessary to call L_IPCMEM_Init() for
 *   L_MM to work properly.
 *
 * History:
 *    2005/6       -- Ryan, Create
 *    2006/1/17    -- Charlie, add a API K_L_MM_AllocateTxBuffer() for buffer
 *                             allocation of packet transmission
 *    2006/10/17   -- Charlie, port to linux
 * Copyright(C)      Accton Corporation, 2004-2006
 */

#ifndef L_MM_H
#define L_MM_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "l_mm_type.h"

/* NAME CONSTANT DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
typedef struct L_MM_Monitor_S   K_L_MM_Monitor_T;
typedef struct L_MM_Mref_S      K_L_MM_Mref_T;

/* L_MM_PtDesc_T: Descriptor of memory partition
 * The buffer will be divided into partitions,
 * Note: partition_len <= buffer_len
 * Note: real partition_len = sizeof(K_L_MM_Monitor_T) + application record length + L_MM_TRAIL_MAGIC_BYTE_LEN
 */
typedef struct
{
    UI8_T   *buffer;
    UI32_T  buffer_len;             /* buffer length, the length must > 12 */
    UI32_T  partition_len;          /* application record length */
    UI32_T  nbr_of_free_partition;  /* unused partition number, read only */
    void    *free_partition_list;   /* The free list pointer, internal used field */
    UI8_T   buffer_type;            /* used in the parameter of K_L_MM_PtCreate()
                                     * for storing buffer_type in Monitor_T
                                     * Note that the 2 MSB of buffer_type will be
                                     * used by l_mm internally. Users should not
                                     * change this value directly.
                                     * This value will be set when K_L_MM_PtCreate()
                                     * is called.
                                     */
} L_MM_PtDesc_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_Init
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Initialize L_MM.
 * INPUT:
 *    None.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    None.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void K_L_MM_Init (void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - K_L_MM_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/
void K_L_MM_Create_InterCSC_Relation(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_GetBufferType
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Get the user buffer type of the given buffer.
 * INPUT:
 *    buf_p  --  pointer to a buffer allocated by K_L_MM_PtAlloc())
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Return User buffer type if sucess.
 *    Return L_MM_ERROR_RETURN_VAL if fail.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
UI32_T K_L_MM_GetBufferType(void* buf_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - K_L_MM_PtGetTotalRequiredSize
 *------------------------------------------------------------------------------
 * PURPOSE:  Get the total required size to create a partition according to
 *           the given one-partition size and the number of partition.
 * INPUT:    partition_size  -  size of a partition
 *           partition_num   -  number of partition
 * OUTPUT:   None.
 * RETURN:   required buffer size to create partitions
 *           return 0 if error
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
UI32_T K_L_MM_PtGetTotalRequiredSize(UI32_T partition_size, UI32_T partition_num);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_PtCreate
 *------------------------------------------------------------------------------
 * PURPOSE:
 *   Create a memory partition
 *
 * INPUT:
 *  desc -
 *     buffer        -- pointer to the memory area
 *     buffer_len    -- size of the memory area (in bytes, must be got from
 *                                               K_L_MM_PtGetTotalRequiredSize)
 *     partition_len -- size of each partition
 *     buffer_type   -- type of buffer (to indicate which kinds of buffers are
 *                                      used, valid value is from 0 to 63)
 *
 * OUTPUT:
 *  None.
 *
 * RETURN:
 *  FALSE -- can't create the partition successfully.
 *  TRUE --  the partition is created successfully
 *
 * NOTES:
 *  1. The buffer will be divided into partitions,
 *      partition_len <= buffer_len
 *  2. partition_len means the length of partition which is transparent to user
 *     Real occupied partition size can be got from L_MM_GET_PT_REAL_PT_SZ.
 *  3. buffer_type will be stored in K_L_MM_Monitor_T field
 *  4. each partition is consisted of:
 *     [K_L_MM_Monitor_T] + [user_partition_buff] + [L_MM_TRAIL_MAGIC_BYTES]
 *     only user_partition_buff is transparent to user
 *  4. leading and trailing magic bytes are initialized for each partition
 *     when this api is called.
 *  5. The two MSB bits of buffer_type is reserved for internal use.
 *     Application can only use the other 6 bits.
 *  6. K_L_MM_Monitor_T.buffer_addr will only be initialized when the buffer is
 *     allocated.
 *  7. Partition is not allwed to be destroyed once it is created.
 *     In other words, the argument desc.buffer should never be freed.
 *------------------------------------------------------------------------------
 */
BOOL_T K_L_MM_PtCreate (L_MM_PtDesc_T *desc);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_PtAlloc
 *------------------------------------------------------------------------------
 * PURPOSE:
 *   Allocate a buffer from the specified partition pool with K_L_MM_Monitor_T
 *   field prefixed to Application's Data Block
 *
 * INPUT:
 *  desc    -- partition descriptor
 *  user_id -- assigned to monitor structure which is prefixed to Application's
 *             Data Block
 *             user_id = trace_id(6) + pool_id(2) + module_id(8)
 *             pool_id is at user's own usage/knowledge.
 *             It can be used to stand for buffer being created by K_L_MM_PtCreate
 *             or L_MM_Malloc or 1st PtCreate and 2nd PtCreate...
 *             or it can be combined with trace_id to form a different trace
 *             information for user's own trace purpose.
 *             user_id is got from macro L_MM_USER_ID (modul_id, pool_id, trace_id)
 *             or L_MM_USER_ID2(modul_id,ext_trace_id)
 *
 * OUTPUT:
 *  None.
 *
 * RETURN:
 *  address of the allocated data block for Application (pointer to Application's Data Block).
 *  Return null if no more data block is available.
 *
 * NOTES:
 *   1. The allocated buffer has K_L_MM_Monitor_T field at its head but it's NOT
 *      the starting address of K_L_MM_Monitor_T field is returned.
 *   2. Compared to L_MM_Malloc which is for varied-sized buffer allocation,
 *       K_L_MM_PtAlloc is a fixed-sized buffer allocation from a pre-defined partition
 *   3. SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 *------------------------------------------------------------------------------
 */
void *K_L_MM_PtAlloc(L_MM_PtDesc_T *desc, UI16_T user_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_PtFree
 *------------------------------------------------------------------------------
 * PURPOSE:
 *   Free the buffer which is allocated by K_L_MM_PtAlloc
 *
 * INPUT:
 *  desc    -- partition descriptor
 *  buffer  -- pointer to Application's Data Block
 *
 * OUTPUT:
 *  None.
 *
 * RETURN:
 *  TRUE  - Free sucessfully
 *  FALSE - Fail to free
 *
 * NOTES:
 *   1. This function will free the buffer starts at the input buffer address - sizeof(K_L_MM_Monitor_T)
 *   2. Buffer address and leading and trail magic bytes will be verified.
 *   3. SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 *------------------------------------------------------------------------------
 */
BOOL_T K_L_MM_PtFree (L_MM_PtDesc_T *desc, void *buffer);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_AllocateTxBuffer
 *------------------------------------------------------------------------------
 * PURPOSE: This function is devised for transmission exclusively.
 *          In this function, the buffer will be allocated and a mref handle
 *          will be constructed. The pdu offset will be set as
 *          SYS_ADPT_TX_BUFFER_MAX_RESERVED_HEADER_LEN to guarantee that lower
 *          level module can use this buffer to transmit directly.
 *          Callers should write data to buffer from pdu pointer which could be
 *          retrieved via K_L_MM_Mref_GetPdu().
 *
 * INPUT:
 *        tx_pdu_size           - The size of the buffer for pdu
 *        user_id               - used to monitor allocated buffer which is stored in K_L_MM_Monitor_T field
 *                                user_id = trace_id(6) + pool_id(2) + module_id(8)
 *                                pool_id is at user's own usage/knowledge.
 *                                It can be used to stand for buffer being created by K_L_MM_AllocateTxBuffer
 *                                or 1st L_MM_AllocateTxBuffer, 2nd K_L_MM_AllocateTxBuffer, etc.
 *                                Or it can be combined with trace_id to form a different trace information
 *                                for user's own trace purpose.
 *                                user_id is got from macro L_MM_USER_ID (modul_id, pool_id, trace_id)
 *                                or or L_MM_USER_ID2(modul_id,ext_trace_id)
 *
 * OUTPUT:
 *          None
 *
 * RETURN:  The MREF handle for packet transmission
 *          NULL if fail.
 *
 * NOTES:
 *      1. Cares should be taken for SYS_ADPT_TX_BUFFER_MAX_RESERVED_HEADER_LEN
 *         to ensure that the value of the constant always equal to the maximum
 *         reserved header length while transmitting packet in the system.
 *      2. user_id will be used once L_IPCMEM_Allocate() be able to accept user_id.
 *      3. ATTENTION! K_L_MM_FreeTxBuffer() MUST be called if user_defined_free_fun
 *         is provided!
 *------------------------------------------------------------------------------
 */
L_MM_Mref_Handle_T* K_L_MM_AllocateTxBuffer(UI32_T              tx_pdu_size,
                                            UI16_T              user_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - K_L_MM_FreeTxBuffer
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the free function for MREF handle created through
 *           K_L_MM_AllocateTxBuffer.
 * INPUT:
 *           buf    - buffer to be freed
 *           cookie - The passed argument when K_L_MM_AllocateTxBuffer() is called
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:
 *     1. This function must be called in the user-defined free function which
 *        is passed to K_L_MM_AllocateTxBuffer().
 *------------------------------------------------------------------------------
 */
void K_L_MM_FreeTxBuffer(void* buf, void* cookie);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - K_L_MM_Mref_GetMrefPoolRequiredSize
 *------------------------------------------------------------------------------
 * PURPOSE:  Get the total required size for mref pool.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   required buffer size to create partitions
 *           return 0 if error
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
UI32_T K_L_MM_Mref_GetMrefPoolRequiredSize(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_Mref_AddRefCount
 *------------------------------------------------------------------------------
 * PURPOSE:
 *   Add ref_count by 1
 *
 * INPUT:
 *  mref_handle_p  -- pointer to handle of L_MM_Mref
 *  inc_count      -- number of reference count to be increased
 *
 * OUTPUT:
 *  None.
 *
 * RETURN:
 *  TRUE  -- done
 *  FALSE -- invalid mref
 *
 * NOTES:
 *   SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 *------------------------------------------------------------------------------
 */
BOOL_T K_L_MM_Mref_AddRefCount (L_MM_Mref_Handle_T* mref_handle_p, UI32_T inc_count);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_Mref_SetFreeFuncTypeAndCookie
 *------------------------------------------------------------------------------
 * PURPOSE:
 *   Set the free function type and cookie for the given mref.
 *
 * INPUT:
 *  mref_handle_p   -- pointer to handle of L_MM_Mref
 *  free_fun_type   -- the type of free function. the free function will be called
 *                     when ref_count reaches 0
 *                     L_MM_MREF_FREE_FUN_TX_BUFFER : free function for tx buffer.
 *                     L_MM_MREF_FREE_FUN_RX_BUFFER : free function for rx buffer.
 *  cookie          -- this cookie will pass to free_func
 *
 * OUTPUT:
 *  None.
 *
 * RETURN:
 *  TRUE  -- done
 *  FALSE -- can't set the free function and its cookie,
 *           possible reason: 1. invalid mref
 *                            2. cookie has already set.
 *                            3. ref_count is larger than 1
 *
 * NOTES:
 *  1. Free function and its cookie should only be allowed to be changed
 *     in the same CSC(Note that the same CSC doesn't need to be the same C file)
 *  2. For avoid misuse, free function and its cookie can only be changed when
 *     a) cookie is NULL
 *     b) reference count is not greater than 1
 *  3. SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 *------------------------------------------------------------------------------
 */
BOOL_T K_L_MM_Mref_SetFreeFuncTypeAndCookie( L_MM_Mref_Handle_T* mref_handle_p, UI32_T free_fun_type, void *cookie);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_Mref_MovePdu
 *------------------------------------------------------------------------------
 * PURPOSE:
 *  Move pdu pointer forward or backword and automatically adjust the pdu length
 *
 * INPUT:
 *  mref_handle_p -- pointer to handle of L_MM_Mref
 *  offset        -- the postive value means moving pointer toward the PDU's rear;
 *                   the negative value means moving pointer toward the PDU's
 *                   front.
 *
 * OUTPUT:
 *  pdu_len_p     -- The length of pdu after this function is returned will be
 *                   kept in this variable.
 *
 * RETURN:
 *  Sucess  -- Return pointer to the pdu when pdu is moved successfully.
 *  Fail    -- Return NULL when failure
 *
 * NOTES:
 *   SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 *------------------------------------------------------------------------------
 */
void* K_L_MM_Mref_MovePdu(L_MM_Mref_Handle_T* mref_handle_p, I32_T offset, UI32_T *pdu_len_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - K_L_MM_Mref_ShiftPdu
 *------------------------------------------------------------------------------
 * PURPOSE:  Shift the pdu block toward the end of the buffer.
 *
 * INPUT:    mref_handle_p  -  pointer to handle of L_MM_Mref
 *           shift_offset   -  the byte count to shift from original place
 *
 * OUTPUT:   None.
 *
 * RETURN:   TRUE  -- Sucess
 *           FALSE -- Fail
 *
 * NOTE:     The shift operation will only sucess when the left buffer size
 *           behind current pdu block is large enough
 *------------------------------------------------------------------------------
 */
BOOL_T K_L_MM_Mref_ShiftPdu(L_MM_Mref_Handle_T* mref_handle_p, UI32_T shift_offset);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_Mref_GetPdu
 *------------------------------------------------------------------------------
 * PURPOSE: get PDU address and PDU length
 *
 * INPUT:
 *  mref_handle_p  - pointer to handle of L_MM_Mref
 *
 * OUTPUT:
 *  pdu_len - length of pdu
 *
 * RETURN: pdu address or NULL if fail
 *
 * NOTES:
 *   SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 *------------------------------------------------------------------------------
 */
void  *K_L_MM_Mref_GetPdu (L_MM_Mref_Handle_T* mref_handle_p, UI32_T *pdu_len);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_Mref_Release
 *------------------------------------------------------------------------------
 * PURPOSE:
 *  Free the mref, if reference count is 0 then free the associated buffer.
 * INPUT:
 *  mref_handle_pp  -- pointer to pointer of handle of L_MM_Mref
 *
 * OUTPUT:
 *  None.
 *
 * RETURN:
 *   Sucess - reference count left, if return 0 mean the buffer is freed.
 *   Fail   - Return L_MM_ERROR_RETURN_VAL if fail
 *
 * NOTE:
 *   1.SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 *   2.mref_handle_pp will be set as NULL when reference count equals zero
 *     after this function is returned.
 *------------------------------------------------------------------------------
 */
UI32_T  K_L_MM_Mref_Release (L_MM_Mref_Handle_T** mref_handle_pp);

#endif /* #ifndef L_MM_H */

