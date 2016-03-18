/* Module Name: L_MM.C(User space)
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
 *   The allocated buffer by L_MM_Malloc(), L_MM_PtAlloc() is consisted of :
 *      buffer :=  L_MM_Monitor_T + Application's Data Block + trail Magic bytes(L_MM_TRAIL_MAGIC_BYTE_LEN bytes)
 *   The allocated buffer for L_MM_Mref_Construct() is consisted of:
 *      buffer :=  L_MM_Monitor_T + L_MM_Mref_T + Application's Data Block + trail Magic bytes(L_MM_TRAIL_MAGIC_BYTE_LEN bytes)
 *   L_MM depends on L_IPCMEM on linux, so it is necessary to call L_IPCMEM_Init() for
 *   L_MM to work properly.
 * History:
 *    2005/6       -- Ryan, Create
 *    2006/1/17    -- Charlie, add a API L_MM_AllocateTxBuffer() for buffer
 *                             allocation of packet transmission
 *    2006/10/13   -- Charlie, port to linux
 *    2008/7/3     -- Tiger Liu, remove unused function, rename functions L_MM_XXX to K_L_MM_XXX.
 * Copyright(C)      Accton Corporation, 2004, 2005, 2006
 */

/* INCLUDE FILE DECLARATIONS
 */
/* standard C include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Accton include files */

#include "sys_type.h"
#include "sysfun.h"
#include "l_mm.h"
#include "l_mm_type.h"
#include "l_ipcmem.h"

/* NAMING CONSTANT DECLARACTION
 */

#define L_MM_TX_BUF_MAX  5

L_MM_Mref_Handle_T temp_tx_buf[L_MM_TX_BUF_MAX];


#define L_MM_LEADING_MAGIC_BYTE_LEN    1
#define L_MM_TRAIL_MAGIC_BYTE_LEN      4 /* should be multiple of 4 */

#define L_MM_MAGIC_BYTE_VALUE          0xAA

#define L_MM_MAX_USER_BUFFER_TYPE      0x3F

/* MACRO FUNCTION DECLARACTION
 */
#define L_MM_ENTER_CRITICAL_SECTION() SYSFUN_ENTER_CRITICAL_SECTION(l_mm_sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER)
#define L_MM_LEAVE_CRITICAL_SECTION() SYSFUN_LEAVE_CRITICAL_SECTION(l_mm_sem_id)

#define L_MM_DEBUG_MSG(msg) \
    if(l_mm_backdoor_dbg_msg_flag==TRUE)\
    {\
        printf("\r\n%s():"msg, __FUNCTION__);\
    }

/* L_MM_GET_ONE_PT_REAL_SIZE
 * PURPOSE:
 *    While using L_MM_PT, users need to reserve extra space in addition to its own application.
 *    This macro is for ease of calculating REAL required size of each partition.
 *    This macro can help calculating the REAL size required by L_MM_Pt
 *    by giving each desired partition size.
 * INPUT:
 *    usr_partition_size -- The size of partition specified by user
 * RETURN: The real ocupied size for each partition of L_MM_Pt.
 *
 * NOTE:
 *    adjust partition length, must be a multiple of 4
 */
#define L_MM_GET_ONE_PT_REAL_SIZE(usr_partition_size) ((((usr_partition_size) + sizeof(L_MM_Monitor_T) + L_MM_TRAIL_MAGIC_BYTE_LEN + 3) / 4 ) * 4 )

#define L_MM_GET_PT_TOTAL_REQUIRED_SZ(usr_partition_size, partition_num) (L_MM_GET_ONE_PT_REAL_SIZE(usr_partition_size) * partition_num)

#if (BACKDOOR_OPEN==TRUE)
static BOOL_T L_MM_CheckFreedBuf(L_MM_Monitor_T* mm_p);
#define L_MM_DEBUG_DO_CHECK_FREED_BUFFER() \
    if(l_mm_backdoor_validate_free_flag==TRUE) \
    { \
        if(L_MM_CheckFreedBuf(mm_p)==FALSE) \
        { \
            return; \
        } \
    }
#else
#define L_MM_DEBUG_DO_CHECK_FREED_BUFFER()
#endif

#if (L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER==TRUE)
    /* For debug purpose: to measure that is the buffer allocated by L_MM_Malloc()
     * has been used over a specified period of time(the coding rule is 1 sec now)
     */
    #define L_MM_DEBUG_MAX_USE_TIME_OF_MALLOC_BUFFER    100

    /* for checking elapsed time
     */
    #define L_MM_DEBUG_DO_CHECK_ELLAPSED_TIME_OF_MALLOC_BUFFER() \
        if(l_mm_backdoor_check_elapsed_time_flag==TRUE) \
        { \
          \
            if(TRUE==L_MATH_TimeOut32(SYSFUN_GetSysTick(), mm_p->allocation_time+L_MM_DEBUG_MAX_USE_TIME_OF_MALLOC_BUFFER)) \
            { \
                printf("\r\nL_MM_Free():Elapsed time of the buffer is over 1 sec. Tid=%lu.module_id=%hu.ext_trace_id=%hu.size=%hu.elapsed_time=%luticks.", \
                    SYSFUN_TaskIdSelf(), L_MM_GET_MODULEID_FROM_USERID(mm_p->uid), L_MM_GET_EXT_TRACEID_FROM_USERID(mm_p->uid), mm_p->buf_len, curr_tick-mm_p->allocation_time, NULL); \
            } \
        }
    /* for showing backdoor menu item
     */
    #define L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER_SHOW_BACKDOOR_MENU_ITEM() \
        printf("\r\n 8. Toggle malloc buffer elapsed time check flag");

    /* for toggling the flag for checking elapsed time of malloc buffer
     */
    #define L_MM_DEBUG_TOGGLE_FLAG_OF_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER() \
        l_mm_backdoor_check_elapsed_time_flag=!l_mm_backdoor_check_elapsed_time_flag; \
        printf("\r\nFlag %s", l_mm_backdoor_check_elapsed_time_flag ? "On" : "Off");

    #define L_MM_DEBUG_GET_CURRENT_TICK() \
        UI32_T curr_tick = SYSFUN_GetSysTick();
#else
    #define L_MM_DEBUG_DO_CHECK_ELLAPSED_TIME_OF_MALLOC_BUFFER()
    #define L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER_SHOW_BACKDOOR_MENU_ITEM()
    #define L_MM_DEBUG_TOGGLE_FLAG_OF_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER()
    #define L_MM_DEBUG_GET_CURRENT_TICK()
#endif

#define L_MM_GET_MONITOR_PART_FROM_MREF(mref_p) ((L_MM_Monitor_T*)mref_p-1)

/* for define RX free function
 */
BOOL_T DEV_NICDRV_MREF_MemFree(void *buf, UI32_T cookie, void *cookie_params)
{
    return TRUE;
}

#define L_MM_FreeRxBuffer   DEV_NICDRV_MREF_MemFree

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static void   L_MM_InsertTo_MonitorList(L_MM_Monitor_T *mm_p);
static void   L_MM_RemoveFrom_MonitorList(L_MM_Monitor_T *mm_p);
static BOOL_T L_MM_Valid_Buf(void *buff_p);

/* STATIC VARIABLE DECLARATIONS
 */
static UI32_T l_mm_sem_id;
static L_MM_Monitor_T* l_mm_monitor_lst_head =  NULL;  /* for monitor list */

/* static variables for debug/backdoor
 */
static UI32_T l_mm_backdoor_buff_nbr = 0;                /* record total number of the allocated buffers */
static BOOL_T l_mm_backdoor_dbg_msg_flag = TRUE;        /* l_mm debug message flag */
#if (BACKDOOR_OPEN==TRUE)
static BOOL_T l_mm_backdoor_validate_free_flag = FALSE;  /* This flag can be changed by backdoor.
                                                          * If this flag is on, when L_MM_Free() is called,
                                                          * it will check that whether is any buffer in moniter
                                                          * list falls in the range of the buffer to be freed.
                                                          * An error message will be shown if this kind of buffer
                                                          * do exists. This error will occur if user of L_MM_Pt
                                                          * free the buffer block which is used to partition.
                                                          * In order to protect l_mm_monitor_lst_head from corruption,
                                                          * we will not free the specified buffer.
                                                          */
#endif
#if (L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER==TRUE)
static BOOL_T l_mm_backdoor_check_elapsed_time_flag = FALSE;
#endif

/* EXPORTED FUNCTION DECLARACTION
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
void L_MM_Init (void)
{
//    if (SYSFUN_CreateSem(SYSFUN_SEMKEY_PRIVATE, 1, SYSFUN_SEM_FIFO, &l_mm_sem_id) != SYSFUN_OK)
//    {
//        SYSFUN_LogUrgentMsg("L_MM_Init: Failed to init semaphore");
//    }

    memset(temp_tx_buf, 0, sizeof(temp_tx_buf));

} /* end of void L_MM_Init (void) */

/*--------------------------------------------------------------------------
 * FUNCTION NAME - L_MM_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/
void L_MM_Create_InterCSC_Relation(void)
{
    return;
}

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
void *L_MM_Malloc (UI32_T size, UI16_T user_id)
{
    L_MM_Monitor_T *mm_p;
    UI32_T real_alloc_sz = size + sizeof(L_MM_Monitor_T) + L_MM_TRAIL_MAGIC_BYTE_LEN;

    if ((mm_p =  (L_MM_Monitor_T*)malloc(real_alloc_sz)) == NULL)
    {
        L_MM_DEBUG_MSG("fails to allocate buffer");
        return NULL;
    }

    memset(mm_p, 0, sizeof(L_MM_Monitor_T));

    mm_p->buffer_type = L_MM_CONVERT_TO_INTERNAL_BUFTYPE(L_MM_DEFAULT_BUFFER_TYPE); /* default buffer_type */
    mm_p->buffer_addr = mm_p;
    mm_p->task_id = SYSFUN_TaskIdSelf();
    mm_p->allocation_time = SYSFUN_GetSysTick();
    mm_p->uid = user_id;
    mm_p->buf_len = real_alloc_sz;

    /* fill leading magic byte */
    L_MM_FILL_LEADING_MAGIC_BYTE(mm_p);
    /* fill trail magic byte */
    L_MM_FILL_TRAIL_MAGIC_BYTE(mm_p);
    /* insert the monitor information to the monitor link list */
    L_MM_InsertTo_MonitorList(mm_p);

    return (void*)((UI32_T)mm_p + sizeof(L_MM_Monitor_T));
}

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
void L_MM_Free (void *buffer)
{
    L_MM_Monitor_T *mm_p;
    L_MM_DEBUG_GET_CURRENT_TICK();

    if ( buffer == NULL )
    {
       /* This is considered as critical issue.
        * Show error message directly here
        */
        printf("\r\n%s():buffer is NULL", __FUNCTION__);
        return;
    }

    mm_p = (L_MM_Monitor_T*)((UI32_T)buffer - sizeof(L_MM_Monitor_T));

    /* validate buffer */
    if(L_MM_Valid_Buf(buffer) == FALSE)
    {
       /* This is considered as critical issue.
        * Show error message directly here
        */
        printf("\r\n%s():invalid buffer", __FUNCTION__);
        return;
    }

    L_MM_DEBUG_DO_CHECK_FREED_BUFFER();

    L_MM_DEBUG_DO_CHECK_ELLAPSED_TIME_OF_MALLOC_BUFFER();

    /* remove from the monitor link list */
    L_MM_RemoveFrom_MonitorList(mm_p);

    free((void*)mm_p);
    return;
}

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
L_MM_Mref_Handle_T* _L_MM_AllocateTxBuffer(UI32_T tx_pdu_size, UI16_T user_id)
{
    int i;

    for (i = 0; i < L_MM_TX_BUF_MAX; ++i)
    {
        if (temp_tx_buf[i].current_usr_id == 0 || temp_tx_buf[i].current_usr_id == user_id)
        {
            temp_tx_buf[i].current_usr_id = user_id;

//            memset(temp_tx_buf[i].parameter_block, 0, sizeof(temp_tx_buf[i].parameter_block));

            return &temp_tx_buf[i];
        }
    }

    return NULL;
}

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
void  *_L_MM_Mref_GetPdu (L_MM_Mref_Handle_T* mref_handle_p, UI32_T *pdu_len)
{
    *pdu_len = sizeof(mref_handle_p->parameter_block);
    return mref_handle_p->parameter_block;
//    return NULL;
}

/*===========================
 * LOCAL SUBPROGRAM BODIES
 *===========================
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L_MM_InsertTo_MonitorList
 *------------------------------------------------------------------------------
 * PURPOSE:  Insert a monitor node into monitor list
 * INPUT:    mm_p  -  pointer to monitor node to be inserted
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     1. Caller of this function must guarntee that the input argument is
 *              valid.
 *------------------------------------------------------------------------------
 */
static void L_MM_InsertTo_MonitorList(L_MM_Monitor_T *mm_p)
{
    L_MM_ENTER_CRITICAL_SECTION();

    mm_p->next_p = l_mm_monitor_lst_head;
    mm_p->prev_p = NULL;

    if  (l_mm_monitor_lst_head != NULL)
    {
        l_mm_monitor_lst_head->prev_p = mm_p;
    }
    l_mm_monitor_lst_head = mm_p;
    l_mm_backdoor_buff_nbr++;

    L_MM_LEAVE_CRITICAL_SECTION();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L_MM_RemoveFrom_MonitorList
 *------------------------------------------------------------------------------
 * PURPOSE:  Remove a monitor node into monitor list
 * INPUT:    mm_p  -  pointer to monitor node to be removed
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     1. Caller of this function must guarntee that the input argument is
 *              valid.
 *------------------------------------------------------------------------------
 */
static void L_MM_RemoveFrom_MonitorList(L_MM_Monitor_T *mm_p)
{
    L_MM_ENTER_CRITICAL_SECTION();

    mm_p->buffer_addr = NULL; /* invalidate buffer_addr to avoid free the same mm_p twice */

    if (mm_p == l_mm_monitor_lst_head)
    {
        l_mm_monitor_lst_head = mm_p->next_p;
        if(l_mm_monitor_lst_head!=NULL)
            l_mm_monitor_lst_head->prev_p = NULL;
    }
    else
    {
        mm_p->prev_p->next_p = mm_p->next_p;
        if(mm_p->next_p!=NULL)
            mm_p->next_p->prev_p = mm_p->prev_p;
    }
    l_mm_backdoor_buff_nbr--;

    L_MM_LEAVE_CRITICAL_SECTION();
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L_MM_Valid_Buf
 *------------------------------------------------------------------------------
 * PURPOSE:  For validating buffer prefixed with L_MM_Monitor_T and suffixed with
 *           trail magic bytes.
 * INPUT:    buff_p  -  pointer to buffer being validated
 * OUTPUT:   None.
 * RETURN:   TRUE  -- Valid buffer
 *           FALSE -- Invalid buffer
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static BOOL_T L_MM_Valid_Buf(void *buff_p)
{
    L_MM_Monitor_T *mm_p;
    BOOL_T is_valid_magic_bytes;

    mm_p = (L_MM_Monitor_T*)((UI32_T)buff_p - sizeof(L_MM_Monitor_T));

    /* check buffer address
     * buffer_addr with buffer type equal to L_MM_MREF_BUFFER_TYPE
     * contains kernel space addr, there is no way to check this addr in user
     * space
     */
    if (L_MM_GET_INTERNAL_BUFTYPE(mm_p->buffer_type) != L_MM_MREF_BUFFER_TYPE &&
        mm_p->buffer_addr != mm_p)
    {
         L_MM_DEBUG_MSG("buffer validation fails through buffer_addr check");
         return FALSE;
    }

    /* check leading magic byte */
    L_MM_IS_VALID_LEADING_MAIC_BYTES(&is_valid_magic_bytes, mm_p);
    if(is_valid_magic_bytes==FALSE)
    {
        L_MM_DEBUG_MSG("buffer validation fails through leading magic byte check");
        printf("%s: TaskId: %d   buff_p: %08x\r\n", __FUNCTION__, (int)SYSFUN_TaskIdSelf(), (unsigned int)buff_p);
        DBG_DumpHex("", sizeof(*mm_p), (char *)mm_p);
        return FALSE;
    }

    /* check trail magic byte */
    L_MM_IS_VALID_TRAIL_MAGIC_BYTES(&is_valid_magic_bytes, mm_p);
    if (is_valid_magic_bytes==FALSE)
    {
        L_MM_DEBUG_MSG("buffer validation fails through trail magic byte check");
        return FALSE;
    }

    return TRUE;
}

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
UI32_T  _L_MM_Mref_Release(L_MM_Mref_Handle_T** mref_handle_pp)
{
    (*mref_handle_pp)->current_usr_id = 0;

    //memset((*mref_handle_pp)->parameter_block, 0, sizeof((*mref_handle_pp)->parameter_block));

    return 0;
}

#if (BACKDOOR_OPEN==TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_CheckFreedBuf
 *------------------------------------------------------------------------------
 * PURPOSE  : This function is used to check whether the given buffer is allowed
 *            to be freed. If one of the address of the buffer in monitor list
 *            falls in the given buffer region, the given buffer can't be freed
 *            in order to prevent monitor list from corruption.
 *
 * INPUT    : mm_p  -  The monitor header of the buffer which has internal buffer
 *                     type L_MM_PT_BUFFER_TYPE
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     :
 *            1.This function will only be called by L_MM_Free when
 *              l_mm_backdoor_validate_free_flag is TRUE.
 *            2.l_mm_backdoor_validate_free_flag can only be set through BACKDOOR.
 *            3.This function is primary for debug.
 *            4.Caller of this function should guarantee the input argument is
 *              valid.
 *-----------------------------------------------------------------------------*/
static BOOL_T L_MM_CheckFreedBuf(L_MM_Monitor_T* mm_p)
{
    UI32_T          part_front_p, part_rear_p;
    L_MM_Monitor_T *monitor_node;

    part_front_p = (UI32_T)mm_p + sizeof(L_MM_Monitor_T);
    part_rear_p  = (UI32_T)mm_p + (UI32_T)mm_p->buf_len - L_MM_TRAIL_MAGIC_BYTE_LEN;

    /* check that is there any address of node in monitor list falls in
     * this buffer range
     */
    monitor_node = l_mm_monitor_lst_head;
    do
    {
        if(((UI32_T)monitor_node>=part_front_p) &&
           ((UI32_T)monitor_node< part_rear_p))
        {
            printk("\r\n<3>Error: An allocated buffer fall in the region of the buffer to be freed");
            L_MM_DEBUG_DUMP_MONITOR_INFO(monitor_node);
            return FALSE;
        }
        else
        {
            monitor_node = monitor_node->next_p;
        }
    }while(monitor_node!=NULL);

    return TRUE;
} /* end of L_MM_CheckFreedBuf */

#endif

