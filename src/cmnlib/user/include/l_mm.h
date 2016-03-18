/* Module Name: L_MM.H (User space)
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
 *   the allocated buffer by L_MM_Malloc(), L_MM_PtAlloc() is consisted of :
 *      buffer :=  L_MM_Monitor_T + Application's Data Block + trail Magic bytes(8 bytes)
 *   L_MM depends on L_IPCMEM on linux, so it is necessary to call L_IPCMEM_Init() for
 *   L_MM to work properly.
 *
 * History:
 *    2005/6       -- Ryan, Create
 *    2006/1/17    -- Charlie, add a API L_MM_AllocateTxBuffer() for buffer
 *                             allocation of packet transmission
 *    2006/10/17   -- Charlie, port to linux
 *    2008/7/3     -- Tiger Liu, remove unused function, move common definition from l_mm.h to this l_mm_type.h
 * Copyright(C)      Accton Corporation, 2004-2006
 */

#ifndef L_MM_H
#define L_MM_H
/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "l_mm_type.h"

#define L_MM_DEBUG_SHOW_ERROR_MSG TRUE

/* NAME CONSTANT DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
typedef void (*L_MM_Mref_FreeFun_T) (void *buf, void *cookie);

typedef struct L_MM_Monitor_S L_MM_Monitor_T;
typedef struct L_MM_Mref_S L_MM_Mref_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_Init
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
void L_MM_Init (void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - L_MM_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/
void L_MM_Create_InterCSC_Relation(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_Malloc
 *------------------------------------------------------------------------------
 * PURPOSE:
 *   Allocate a buffer making use of malloc with L_MM_Monitor_T field prefixed
 *   to Application's Data Block internally
 *
 * INPUT:
 *  size    -- the size of the buffer to allocate for Application's Data Block
 *             (not including L_MM_Monitor_T)
 *  user_id -- used to monitor allocated buffer which is stored in L_MM_Monitor_T field
 *             user_id = trace_id(6) + pool_id(2) + module_id(8)
 *             pool_id is at user's own usage/knowledge.
 *             It can be used to stand for buffer being created by L_MM_PtCreate
 *             or L_MM_Malloc or 1st PtCreate and 2nd PtCreate...
 *             or it can be combined with trace_id to form a different trace information
 *             for user's own trace purpose.
 *             user_id is got from macro L_MM_USER_ID (modul_id, pool_id, trace_id)
 *             or L_MM_USER_ID2(modul_id,ext_trace_id)
 *
 * OUTPUT:
 *  None.
 *
 * RETURN:
 *  address of the allocated data block for Application (pointer to Application's Data Block).
 *
 * NOTES:
 *   1. This function will use malloc to allocate N bytes where
 *      N = size + sizeof(L_MM_Monitor_T) + L_MM_TRAIL_MAGIC_BYTE_LEN
 *   2. Compared to L_MM_PtAlloc which is for fixed-sized buffer allocation from
 *      predefined partition, L_MM_Malloc is a varied-sized buffer allocation
 *   3. SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 *------------------------------------------------------------------------------
 */
void *L_MM_Malloc (UI32_T size, UI16_T user_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_Free
 *------------------------------------------------------------------------------
 * PURPOSE:
 *   Free the buffer which is allocated by L_MM_Malloc
 *
 * INPUT:
 *  buffer  -- pointer to Application's Data Block
 *
 * OUTPUT:
 *  None.
 *
 * RETURN:
 *  None.
 *
 * NOTES:
 *   1. This function will free the buffer starts at the input buffer address - sizeof(L_MM_Monitor_T)
 *   2. SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 *------------------------------------------------------------------------------
 */
void L_MM_Free (void *buffer);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_AllocateTxBuffer
 *------------------------------------------------------------------------------
 * PURPOSE: This function is devised for transmission exclusively.
 *          In this function, the buffer will be allocated and a mref handle
 *          will be constructed. The pdu offset will be set as
 *          SYS_ADPT_TX_BUFFER_MAX_RESERVED_HEADER_LEN to guarantee that lower
 *          level module can use this buffer to transmit directly.
 *          Callers should write data to buffer from pdu pointer which could be
 *          retrieved via L_MM_Mref_GetPdu().
 *
 * INPUT:
 *        tx_pdu_size           - The size of the buffer for pdu
 *        user_id               - used to monitor allocated buffer which is stored in L_MM_Monitor_T field
 *                                user_id = trace_id(6) + pool_id(2) + module_id(8)
 *                                pool_id is at user's own usage/knowledge.
 *                                It can be used to stand for buffer being created by L_MM_AllocateTxBuffer
 *                                or 1st L_MM_AllocateTxBuffer, 2nd L_MM_AllocateTxBuffer, etc.
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
 *------------------------------------------------------------------------------
 */
L_MM_Mref_Handle_T* _L_MM_AllocateTxBuffer(UI32_T tx_pdu_size, UI16_T user_id);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_AllocateTxBuffer(tx_pdu_size, user_id)                                                         \
        ({                                                                                                      \
            L_MM_Mref_Handle_T *_rc;                                                                             \
            _rc =_L_MM_AllocateTxBuffer(tx_pdu_size, user_id);                                                  \
            if (_rc == NULL)                                                                                    \
                printf("\r\n%s() line %d: L_MM_AllocateTxBuffer() return NULL.", __FUNCTION__, __LINE__);       \
            _rc;                                                                                                \
        })
#else
    #define L_MM_AllocateTxBuffer _L_MM_AllocateTxBuffer
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_AllocateTxBufferForPktForwarding
 *------------------------------------------------------------------------------
 * PURPOSE: This function is to allocate Tx Buffer for forwarding a received packet.
 *
 * INPUT:
 *        rcv_mref_handle_p     - L_MM_Mref of received packet
 *        tx_pdu_size           - The size of the buffer for pdu
 *        user_id               - used to monitor allocated buffer which is stored in L_MM_Monitor_T field
 *
 * OUTPUT:
 *          None
 *
 * RETURN:  The MREF handle for packet transmission
 *          NULL if fail.
 *
 * NOTES:
 *          see L_MM_AllocateTxBuffer for detail
 *------------------------------------------------------------------------------
 */
L_MM_Mref_Handle_T *_L_MM_AllocateTxBufferForPktForwarding(L_MM_Mref_Handle_T *rcv_mref_handle_p, UI32_T tx_pdu_size, UI16_T user_id);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_AllocateTxBufferForPktForwarding(rcv_mref_handle_p, tx_pdu_size, user_id)                      \
        ({                                                                                                      \
            L_MM_Mref_Handle_T *_rc;                                                                            \
            _rc =_L_MM_AllocateTxBufferForPktForwarding(rcv_mref_handle_p, tx_pdu_size, user_id);               \
            if (_rc == NULL)                                                                                    \
                printf("\r\n%s() line %d: L_MM_AllocateTxBufferForPktForwarding() return NULL.", __FUNCTION__, __LINE__);   \
            _rc;                                                                                                \
        })
#else
    #define L_MM_AllocateTxBufferForPktForwarding _L_MM_AllocateTxBufferForPktForwarding
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_AllocateTxBufferFromDedicatedBufPool
 *------------------------------------------------------------------------------
 * PURPOSE: Allocate buffer for transimition from the specified buffer pool.
 *
 * INPUT:
 *          buffer_pool_id      - buffer pool id where the buffer will be allocated
 *          user_id             - used to monitor allocated buffer which is stored in L_MM_Monitor_T field
 *                                user_id = trace_id(6) + pool_id(2) + module_id(8)
 *                                pool_id is at user's own usage/knowledge.
 *                                It can be used to stand for buffer being created by L_MM_AllocateTxBuffer
 *                                or 1st L_MM_AllocateTxBuffer, 2nd L_MM_AllocateTxBuffer, etc.
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
 *------------------------------------------------------------------------------
 */
L_MM_Mref_Handle_T *_L_MM_AllocateTxBufferFromDedicatedBufPool(UI32_T buffer_pool_id, UI32_T user_id);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_AllocateTxBufferFromDedicatedBufPool(buffer_pool_id, user_id)                                                      \
        ({                                                                                                                          \
            L_MM_Mref_Handle_T *_rc;                                                                                                 \
            _rc = _L_MM_AllocateTxBufferFromDedicatedBufPool(buffer_pool_id, user_id);                                              \
            if (_rc == NULL)                                                                                                        \
                printf("\r\n%s() line %d: L_MM_AllocateTxBufferFromDedicatedBufPool() return NULL.", __FUNCTION__, __LINE__);       \
            _rc;                                                                                                                    \
        })
#else
    #define L_MM_AllocateTxBufferFromDedicatedBufPool _L_MM_AllocateTxBufferFromDedicatedBufPool
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_Mref_Construct
 *------------------------------------------------------------------------------
 * PURPOSE: Initiate the mref information in raw-buffer
 *
 * INPUT:
 *        buf            - the address of data block, received packet block.
 *        buf_len        - the size of data block, not including reserved length(L_MM_MREF_PREFIX_RESERVED_SIZE),
 *                         this is used for range checking.
 *        pdu_offset     - the position of pdu, the offset(byte count) from the
 *                         caller usable buf
 *        pdu_len        - pdu length
 *        type           - the type of free function. the free function will be called
 *                         when ref_count reaches 0
 *                         L_MM_MREF_FREE_FUN_TX_BUFFER : free function for tx buffer.
 *                         L_MM_MREF_FREE_FUN_RX_BUFFER : free function for rx buffer.
 *                         L_MM_MREF_FREE_FUN_CUSTOM    : user-specified free function. user must specify free function in 'cookie'.
 *        cookie         - cookie of free function (0 means no cookie)
 *                         if type is L_MM_MREF_FREE_FUN_CUSTOM, it indicates the free function.
 *
 * OUTPUT:
 *        None
 *
 * RETURN:  pointer to handle of L_MM_Mref if success
 *          or return NULL if fail
 *
 * NOTES:
 *      1. ref_count will be set to 1 in L_MM_Mref_Construct
 *      2. the buffer must be TX/RX buffer.
 *      3. When type is L_MM_MREF_FREE_FUN_CUSTOM, the given buf 
 *         must be able to referenced by the free function pointer.
 *         (The given buf and free function pointer must be valid in
 *          the context of the caller of L_MM_Mref_Release())
 *------------------------------------------------------------------------------
 */
L_MM_Mref_Handle_T*  _L_MM_Mref_Construct (void*               buf,
                                          UI16_T              buf_len,
                                          UI16_T              pdu_offset,
                                          UI16_T              pdu_len,
                                          UI16_T              type,
                                          void                *cookie,
                                          void                *cookie_params);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_Mref_Construct(buf, buf_len, pdu_offset, pdu_len, type, cookie, cookie_params)                                \
        ({                                                                                                      \
            L_MM_Mref_Handle_T *_rc;                                                                             \
            _rc = _L_MM_Mref_Construct(buf, buf_len, pdu_offset, pdu_len, type, cookie, cookie_params);                        \
            if (_rc == NULL)                                                                                    \
                printf("\r\n%s() line %d: L_MM_Mref_Construct() return NULL.", __FUNCTION__, __LINE__);         \
            _rc;                                                                                                \
        })
#else
    #define L_MM_Mref_Construct _L_MM_Mref_Construct
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_Mref_AddRefCount
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
BOOL_T _L_MM_Mref_AddRefCount(L_MM_Mref_Handle_T* mref_handle_p, UI32_T inc_count);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_Mref_AddRefCount(mref_handle_p, inc_count)                                                 \
        ({                                                                                                  \
            BOOL_T  _rc;                                                                                    \
            _rc = _L_MM_Mref_AddRefCount(mref_handle_p, inc_count);                                         \
            if (!_rc)                                                                                       \
                printf("\r\n%s() line %d: _L_MM_Mref_AddRefCount() return fail.", __FUNCTION__, __LINE__);  \
            _rc;                                                                                            \
        })
#else
    #define L_MM_Mref_AddRefCount _L_MM_Mref_AddRefCount
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_Mref_SetFreeFuncTypeAndCookie
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
 *                     L_MM_MREF_FREE_FUN_CUSTOM    : user-specified free function.
 *  cookie          -- cookie of free function (0 means no cookie)
 *                     if type is L_MM_MREF_FREE_FUN_CUSTOM, it indicates the free function.
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
BOOL_T _L_MM_Mref_SetFreeFuncTypeAndCookie(L_MM_Mref_Handle_T* mref_handle_p, UI32_T free_fun_type, void *cookie);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_Mref_SetFreeFuncTypeAndCookie(mref_handle_p, free_fun_type, cookie)                                    \
        ({                                                                                                              \
            BOOL_T  _rc;                                                                                                \
            _rc = _L_MM_Mref_SetFreeFuncTypeAndCookie(mref_handle_p, free_fun_type, cookie);                            \
            if (!_rc)                                                                                                   \
                printf("\r\n%s() line %d: L_MM_Mref_SetFreeFuncTypeAndCookie() return fail.", __FUNCTION__, __LINE__);  \
            _rc;                                                                                                        \
        })
#else
    #define L_MM_Mref_SetFreeFuncTypeAndCookie _L_MM_Mref_SetFreeFuncTypeAndCookie
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_Mref_MovePdu
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
void* _L_MM_Mref_MovePdu(L_MM_Mref_Handle_T* mref_handle_p, I32_T offset, UI32_T *pdu_len_p);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_Mref_MovePdu(mref_handle_p, offset, pdu_len_p)                                         \
        ({                                                                                              \
            void*  _rc;                                                                                 \
            _rc = _L_MM_Mref_MovePdu(mref_handle_p, offset, pdu_len_p);                                 \
            if (_rc == NULL)                                                                            \
                printf("\r\n%s() line %d: L_MM_Mref_MovePdu() return NULL.", __FUNCTION__, __LINE__);   \
            _rc;                                                                                        \
        })
#else
    #define L_MM_Mref_MovePdu _L_MM_Mref_MovePdu
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L_MM_Mref_ShiftPdu
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
BOOL_T _L_MM_Mref_ShiftPdu(L_MM_Mref_Handle_T* mref_handle_p, UI32_T shift_offset);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_Mref_ShiftPdu(mref_handle_p, shift_offset)                                             \
        ({                                                                                              \
            BOOL_T  _rc;                                                                                \
            _rc = _L_MM_Mref_ShiftPdu(mref_handle_p, shift_offset);                                     \
            if (!_rc)                                                                                   \
                printf("\r\n%s() line %d: L_MM_Mref_ShiftPdu() return FALSE.", __FUNCTION__, __LINE__); \
            _rc;                                                                                        \
        })
#else
    #define L_MM_Mref_ShiftPdu _L_MM_Mref_ShiftPdu
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L_MM_Mref_SetPduLen
 *------------------------------------------------------------------------------
 * PURPOSE:  Set the length of pdu.
 *
 * INPUT:    mref_handle_p  -  pointer to handle of L_MM_Mref
 *           pdu_len        -  length of pdu
 *
 * OUTPUT:   None.
 *
 * RETURN:   TRUE  -- Sucess
 *           FALSE -- Fail
 *
 * NOTE:
 *    For IUC client STKTPLG, it will use IUC to send packet with size 18,
 *    plus the two header in front of it as follows:
 *               XGS ether header, length = 18
 *               IUC header, length       = 8
 *    (the payload size 18 will store in IUC header)
 *
 *    The total size will be 18+18+8 = 44, but the minimal packet size is 64,
 *    so padding of size 20 will be add before send by chip driver.
 *    At the remote unit, this packet will be receive with size 64.
 *
 *    In remote IUC, its pdu will move 18+8=26 bytes and dispatch to STKTPLG,
 *    but this will cause problem because the payload become 64-26 = 38 bytes
 *    instead of 18 which it originally be. Thus the pdu length should be set
 *    as 18 before passing mref to STKTPLG.
 *------------------------------------------------------------------------------
 */
BOOL_T _L_MM_Mref_SetPduLen(L_MM_Mref_Handle_T* mref_handle_p, UI32_T pdu_len);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_Mref_SetPduLen(mref_handle_p, pdu_len)                                                     \
        ({                                                                                                  \
            BOOL_T  _rc;                                                                                    \
            _rc = _L_MM_Mref_SetPduLen(mref_handle_p, pdu_len);                                             \
            if (!_rc)                                                                                       \
                printf("\r\n%s() line %d: L_MM_Mref_SetPduLen() return FALSE.", __FUNCTION__, __LINE__);    \
            _rc;                                                                                            \
        })
#else
    #define L_MM_Mref_SetPduLen _L_MM_Mref_SetPduLen
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_Mref_GetPdu
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
void  *_L_MM_Mref_GetPdu (L_MM_Mref_Handle_T* mref_handle_p, UI32_T *pdu_len);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_Mref_GetPdu(mref_handle_p, pdu_len)                                                    \
        ({                                                                                              \
            void*  _rc;                                                                                 \
            _rc = _L_MM_Mref_GetPdu(mref_handle_p, pdu_len);                                            \
            if (_rc == NULL)                                                                            \
                printf("\r\n%s() line %d: L_MM_Mref_GetPdu() return NULL.", __FUNCTION__, __LINE__);    \
            _rc;                                                                                        \
        })
#else
    #define L_MM_Mref_GetPdu _L_MM_Mref_GetPdu
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L_MM_Mref_GetAvailSzBeforePdu
 *------------------------------------------------------------------------------
 * PURPOSE:  Get the available buffer size before current pdu
 * INPUT:    mref_handle_p  -  pointer to handle of L_MM_Mref
 * OUTPUT:   None.
 * RETURN:   Available size(byte count) before pdu
 *           Or return L_MM_ERROR_RETURN_VAL if fail
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
UI32_T _L_MM_Mref_GetAvailSzBeforePdu(L_MM_Mref_Handle_T* mref_handle_p);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_Mref_GetAvailSzBeforePdu(mref_handle_p)                                                            \
        ({                                                                                                          \
            UI32_T _rc;                                                                                             \
            _rc = _L_MM_Mref_GetAvailSzBeforePdu(mref_handle_p);                                                    \
            if (_rc == L_MM_ERROR_RETURN_VAL)                                                                       \
                printf("\r\n%s() line %d: L_MM_Mref_GetAvailSzBeforePdu() return fail.", __FUNCTION__, __LINE__);   \
            _rc;                                                                                                    \
        })
#else
    #define L_MM_Mref_GetAvailSzBeforePdu _L_MM_Mref_GetAvailSzBeforePdu
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_Mref_Release
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
UI32_T  _L_MM_Mref_Release(L_MM_Mref_Handle_T** mref_handle_pp);

#if (L_MM_DEBUG_SHOW_ERROR_MSG == TRUE)
    #define L_MM_Mref_Release(mref_handle_pp)                                                           \
        ({                                                                                              \
            UI32_T _rc;                                                                                 \
            _rc = _L_MM_Mref_Release(mref_handle_pp);                                                   \
            if (_rc == L_MM_ERROR_RETURN_VAL)                                                           \
                printf("\r\n%s() line %d: L_MM_Mref_Release() return fail.", __FUNCTION__, __LINE__);   \
            _rc;                                                                                        \
        })
#else
    #define L_MM_Mref_Release _L_MM_Mref_Release
#endif

#endif /* #ifndef L_MM_H */

