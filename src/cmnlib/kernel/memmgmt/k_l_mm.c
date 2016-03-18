/* Module Name: L_MM.C(Kernel space)
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
 *   The allocated buffer by K_L_MM_PtAlloc() is consisted of :
 *      buffer :=  K_L_MM_Monitor_T + Application's Data Block + trail Magic bytes(L_MM_TRAIL_MAGIC_BYTE_LEN bytes)
 *   The allocated buffer for K_L_MM_Mref_Construct() is consisted of:
 *      buffer :=  K_L_MM_Monitor_T + K_L_MM_Mref_T + Application's Data Block + trail Magic bytes(L_MM_TRAIL_MAGIC_BYTE_LEN bytes)
 *   L_MM depends on L_IPCMEM on linux, so it is necessary to call L_IPCMEM_Init() for
 *   L_MM to work properly.
 * History:
 *    2005/6       -- Ryan, Create
 *    2006/1/17    -- Charlie, add a API K_L_MM_AllocateTxBuffer() for buffer
 *                             allocation of packet transmission
 *    2006/10/13   -- Charlie, port to linux
 *    2008/7/3     -- Tiger Liu, remove unused function, rename functions L_MM_XXX to K_L_MM_XXX.
 * Copyright(C)      Accton Corporation, 2004, 2005, 2006
 */

/* INCLUDE FILE DECLARATIONS
 */
#include "config.h"

#include <memory.h>
#include <stdio.h>
#define	printk			printf
#define kmalloc			malloc
#define kfree			free
#define copy_from_user	memcpy
#define copy_to_user	memcpy


/* Accton include files */
#include "sys_type.h"
#include "sys_adpt.h"
#include "sys_module.h"
#include "cmnlib_type.h"
#include "l_math.h"
#include "k_sysfun.h"
#include "k_l_ipcmem.h"
#include "k_l_mm.h"
// #include "l_ipcmem_type.h" // temporarily

#define BACKDOOR_OPEN TRUE

/* NAMING CONSTANT DECLARACTION
 */

#define L_MM_BUFFER_TYPE_MREF_POOL     0

/* MACRO FUNCTION DECLARACTION
 */
#define L_MM_DEBUG_MSG(msg)                         \
    do {                                            \
        if(l_mm_backdoor_dbg_msg_flag==TRUE)        \
            printk("\r\n<6>%s():"msg, __FUNCTION__);\
    } while (0)

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
#define L_MM_GET_ONE_PT_REAL_SIZE(usr_partition_size) ((((usr_partition_size) + sizeof(K_L_MM_Monitor_T) + L_MM_TRAIL_MAGIC_BYTE_LEN + 3) / 4 ) * 4 )

#define L_MM_GET_PT_TOTAL_REQUIRED_SZ(usr_partition_size, partition_num) L_MM_GET_ONE_PT_REAL_SIZE(usr_partition_size) * partition_num

#define L_MM_GET_MONITOR_PART_FROM_MREF(mref_p) ((K_L_MM_Monitor_T*)mref_p-1)


/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static UI32_T K_L_MM_Operation(UI32_T cmd, UI32_T arg1, UI32_T arg2, UI32_T arg3, UI32_T arg4);
static void   K_L_MM_InsertTo_MonitorList(K_L_MM_Monitor_T *mm_p);
static void   K_L_MM_RemoveFrom_MonitorList(K_L_MM_Monitor_T *mm_p);
static BOOL_T K_L_MM_Valid_Buf(void *buff_p);
static L_MM_Mref_Handle_T*  K_L_MM_Mref_Construct (void*               buf,
                                          UI16_T              buf_len,
                                          UI16_T              pdu_offset,
                                          UI16_T              pdu_len,
                                          UI16_T              type,
                                          void                *cookie,
                                          void                *cookie_params);

#if (BACKDOOR_OPEN==TRUE)
static BOOL_T K_L_MM_GetBufferInfo(L_MM_Backdoor_BufferInfo_T *buffer_info_p);
static BOOL_T K_L_MM_GetMrefInfo(L_MM_Backdoor_MrefInfo_T *mref_info_p);
#endif

/* STATIC VARIABLE DECLARATIONS
 */
static K_L_MM_Monitor_T* l_mm_monitor_lst_head =  NULL;  /* for monitor list */

/* static variables for debug/backdoor
 */
static BOOL_T l_mm_backdoor_dbg_msg_flag = TRUE;        /* l_mm debug message flag */
static UI32_T l_mm_backdoor_buff_nbr = 0;                /* record total number of the allocated buffers */

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

static L_MM_PtDesc_T   mref_pool_desc;

/* EXPORTED FUNCTION DECLARACTION
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
void K_L_MM_Init (void)
{
#if (L_IPCMEM_ALLOC_MEM_FROM_BCM == TRUE && L_IPCMEM_ALLOC_MEM_FROM_BCM_KERNEL == FALSE)
    if (K_L_IPCMEM_GetStartOfMemBufferAddr() == NULL)
        return;
#endif

    if (!K_L_IPCMEM_GetMRefBufStartAddress((UI32_T *)&(mref_pool_desc.buffer)))
    {
        SYSFUN_LogUrgentMsg("K_L_MM_Init: Failed to init mref_pool_desc");
        return;
    }

    mref_pool_desc.buffer_len = L_MM_GET_PT_TOTAL_REQUIRED_SZ(sizeof(K_L_MM_Mref_T), SYS_ADPT_MAX_NBR_OF_MREF_DESC_BLOCK);
    mref_pool_desc.buffer_type = L_MM_BUFFER_TYPE_MREF_POOL;
    mref_pool_desc.partition_len = sizeof(K_L_MM_Mref_T);

    if(FALSE==K_L_MM_PtCreate(&mref_pool_desc))
    {
        SYSFUN_LogUrgentMsg("K_L_MM_Init: Failed to init mref_pool_desc");
    }
} /* end of void K_L_MM_Init (void) */

/*--------------------------------------------------------------------------
 * FUNCTION NAME - K_L_MM_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/
void K_L_MM_Create_InterCSC_Relation(void)
{
    SYSFUN_RegisterCallBackFunc(SYSFUN_SYSCALL_L_MM, K_L_MM_Operation);
} /* end of K_L_MM_Create_InterCSC_Relation */

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_GetBufferType
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Get the user buffer type of the given buffer.
 * INPUT:
 *    buf_p  --  pointer to a buffer allocated by l_mm(L_MM_Malloc() or K_L_MM_PtAlloc())
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
UI32_T K_L_MM_GetBufferType(void* buf_p)
{
    K_L_MM_Monitor_T *mm_p;

    if(buf_p==NULL)
    {
        L_MM_DEBUG_MSG("buf_p is NULL");
        return L_MM_ERROR_RETURN_VAL;
    }

    /* validate buffer */
    if(K_L_MM_Valid_Buf(buf_p) == FALSE)
    {
        L_MM_DEBUG_MSG("invalid buffer");
        return  L_MM_ERROR_RETURN_VAL;
    }

    mm_p = (K_L_MM_Monitor_T*)buf_p - 1;

    return L_MM_GET_APP_BUFTYPE(mm_p->buffer_type);
} /* end of K_L_MM_GetBufferType */

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
UI32_T K_L_MM_PtGetTotalRequiredSize(UI32_T partition_size, UI32_T partition_num)
{
    if((partition_size==0) || (partition_num==0))
        return 0;

    return L_MM_GET_PT_TOTAL_REQUIRED_SZ(partition_size, partition_num);
}

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
BOOL_T K_L_MM_PtCreate (L_MM_PtDesc_T *desc)
{
    UI32_T  i, buf_nbr;
    UI32_T  next_position, real_part_len;
    K_L_MM_Monitor_T *mm_p = NULL;

    /* BODY */

    /* validate buffer_type */
    if((desc==NULL) ||
       (desc->buffer_type > L_MM_MAX_USER_BUFFER_TYPE))
    {
        L_MM_DEBUG_MSG("invalid buffer type");
        return FALSE;
    }

    if(desc->partition_len > desc->buffer_len)
    {
        if(l_mm_backdoor_dbg_msg_flag==TRUE)
        {
            printk("\r\n<6>%s() fails: invalid partition_len or buffer_len setting\r\npartition_len=%lu, buffer_len=%lu",
                   __FUNCTION__,
                   desc->partition_len,
                   desc->buffer_len);
        }
        return FALSE;
    }

    real_part_len = L_MM_GET_ONE_PT_REAL_SIZE(desc->partition_len);
    desc->free_partition_list = 0;

    buf_nbr = desc->buffer_len / real_part_len;

    next_position = (UI32_T) (desc->free_partition_list = desc->buffer);

    for (i = 0; i < buf_nbr; i++)
    {
        mm_p = (K_L_MM_Monitor_T*) next_position;
        next_position += real_part_len;
        mm_p->buffer_type = L_MM_CONVERT_TO_INTERNAL_BUFTYPE(L_MM_PT_BUFFER_TYPE) | desc->buffer_type; /* store the buffer type in Monitor field */
        mm_p->buffer_addr = NULL; /* this field is only filled when the buffer is allocated */
        mm_p->next_p = (K_L_MM_Monitor_T*)next_position;
        mm_p->buf_len = real_part_len;
        L_MM_FILL_LEADING_MAGIC_BYTE(mm_p);
        L_MM_FILL_TRAIL_MAGIC_BYTE(mm_p);
    }
    mm_p->next_p = NULL;

    desc->nbr_of_free_partition = buf_nbr;

    return TRUE;
}

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
void *K_L_MM_PtAlloc(L_MM_PtDesc_T *desc, UI16_T user_id)
{
    UI32_T   oint;
    K_L_MM_Monitor_T *mm_p;

    if(desc==NULL)
    {
        L_MM_DEBUG_MSG("desc is NULL");
        return NULL;
    }

    oint = SYSFUN_InterruptLock ();

    if ((mm_p =  (K_L_MM_Monitor_T*)desc->free_partition_list) != NULL)
    {
        desc->free_partition_list = (void *) mm_p->next_p;
        desc->nbr_of_free_partition --;
    }

    SYSFUN_InterruptUnlock ( oint );    /* restore int */

    if (mm_p == NULL)
    {
        L_MM_DEBUG_MSG("no more partition can be allocated");
        return NULL;
    }

    mm_p->buffer_addr = mm_p;
    mm_p->task_id = SYSFUN_TaskIdSelf();
    mm_p->allocation_time = SYSFUN_GetSysTick();
    mm_p->uid = user_id;
    mm_p->buffer_type = mm_p->buffer_type;

    /* insert the monitor information to the monitor link list */
    K_L_MM_InsertTo_MonitorList(mm_p);

    return (void*)((UI32_T)mm_p + sizeof(K_L_MM_Monitor_T));
}

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
BOOL_T K_L_MM_PtFree(L_MM_PtDesc_T *desc, void *buffer)
{
    UI32_T    oint,real_part_len;
    K_L_MM_Monitor_T *mm_p;

    if ( (desc==NULL) || (buffer == NULL) )
    {
       /* This is considered as critical issue.
        * Show error message directly here
        */
       printk("\r\n<4>%s():desc or buffer is NULL", __FUNCTION__);
       return FALSE;
    }

    real_part_len = L_MM_GET_ONE_PT_REAL_SIZE(desc->partition_len);
    mm_p = (K_L_MM_Monitor_T*)((UI32_T)buffer - sizeof(K_L_MM_Monitor_T));

    /* check range */
    if ( ( (UI32_T)mm_p < (UI32_T) desc->buffer) || /* check boundary of front parition */
         ( ((UI32_T)mm_p + real_part_len) > (UI32_T) (desc->buffer+desc->buffer_len) ) ) /* check boundary of rear parition */
    {
       /* This is considered as critical issue.
        * Show error message directly here
        */
        printk("\r\n<4>%s():buffer to be freed doesn't belong to this desc", __FUNCTION__);
        L_MM_DEBUG_DUMP_MONITOR_INFO(mm_p);
        return FALSE;
    }

    /* alignment check */
    if ( ((UI32_T)mm_p - (UI32_T)desc->buffer) % real_part_len )
    {
       /* This is considered as critical issue.
        * Show error message directly here
        */
        printk("\r\n<4>%s():buffer to be freed has alignment error", __FUNCTION__);
        L_MM_DEBUG_DUMP_MONITOR_INFO(mm_p);
        return FALSE;
    }

    /* validate buffer */
    if(K_L_MM_Valid_Buf(buffer) == FALSE)
    {
       /* This is considered as critical issue.
        * Show error message directly here
        */
        printk("\r\n<4>%s():invalid buffer", __FUNCTION__);
        return  FALSE;
    }

    /* remove from the monitor link list */
    K_L_MM_RemoveFrom_MonitorList(mm_p);

    /* return to partition's free list */
    oint = SYSFUN_InterruptLock ();
    mm_p->next_p = (K_L_MM_Monitor_T*)desc->free_partition_list;
    desc->free_partition_list = (void*)mm_p;
    desc-> nbr_of_free_partition++;
    SYSFUN_InterruptUnlock ( oint );    /* restore int */

    return TRUE;
} /* end of K_L_MM_PtFree */

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
 *                                or 1st K_L_MM_AllocateTxBuffer, 2nd K_L_MM_AllocateTxBuffer, etc.
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
 *      2. user_id will be used once K_L_IPCMEM_Allocate() be able to accept user_id.
 *      3. ATTENTION! K_L_MM_FreeTxBuffer() MUST be called if user_defined_free_fun
 *         is provided!
 *------------------------------------------------------------------------------
 */
L_MM_Mref_Handle_T* K_L_MM_AllocateTxBuffer(UI32_T              tx_pdu_size,
                                            UI16_T              user_id)
{
    void*               buffer_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              actual_buf_size;

    actual_buf_size= tx_pdu_size + SYS_ADPT_TX_BUFFER_MAX_RESERVED_HEADER_LEN;

    buffer_p = K_L_IPCMEM_Allocate(actual_buf_size, user_id);

    if(buffer_p == NULL)
    {
        /* This is considered as critical issue.
         * Show error message directly here
         */
        printk("\r\n<4>%s():K_L_IPCMEM_Allocate fail!", __FUNCTION__);
        return NULL;
    }

    mref_handle_p = K_L_MM_Mref_Construct(buffer_p, actual_buf_size, SYS_ADPT_TX_BUFFER_MAX_RESERVED_HEADER_LEN,
                                        tx_pdu_size,
                                        L_MM_MREF_FREE_FUN_TX_BUFFER, NULL, NULL);

    return mref_handle_p;
}

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
void K_L_MM_FreeTxBuffer(void* buf, void* cookie)
{
    if(buf == NULL)
    {
        SYSFUN_Debug_Printf("\r\n%s():desc or buffer is NULL", __FUNCTION__);
        return;
    }

    K_L_IPCMEM_Free(buf);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_AllocateTxBufferFromDedicatedBufPool
 *------------------------------------------------------------------------------
 * PURPOSE: Allocate buffer for transimition from the specified buffer pool.
 *
 * INPUT:
 *          buffer_pool_id      - buffer pool id where the buffer will be allocated
 *          user_id             - used to monitor allocated buffer which is stored in K_L_MM_Monitor_T field
 *                                user_id = trace_id(6) + pool_id(2) + module_id(8)
 *                                pool_id is at user's own usage/knowledge.
 *                                It can be used to stand for buffer being created by K_L_MM_AllocateTxBuffer
 *                                or 1st K_L_MM_AllocateTxBuffer, 2nd K_L_MM_AllocateTxBuffer, etc.
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
 *          None
 *------------------------------------------------------------------------------
 */
static L_MM_Mref_Handle_T *K_L_MM_AllocateTxBufferFromDedicatedBufPool(UI32_T buffer_pool_id, UI32_T user_id)
{
    void*               buffer_p;
    L_MM_Mref_Handle_T* mref_handle_p;
    UI32_T              buf_type;
    UI32_T              tx_pdu_size, actual_buf_size;

    switch (buffer_pool_id)
    {
        case L_MM_TX_BUF_POOL_ID_STKTPLG_HBT1:
            buf_type = L_IPCMEM_DEDICATED_BUF_TYPE_STKTPLG_HBT1;
            break;
#if (SYS_CPNT_DHCP == TRUE)
        case L_MM_TX_BUF_POOL_ID_DHCP_TX:
            buf_type = L_IPCMEM_DEDICATED_BUF_TYPE_DHCP_TX;
            break;
#endif
        default:
            printk("\r\n<4>%s(): buffer_pool_id:%d", __FUNCTION__, (int)buffer_pool_id);
            return NULL;
    }

    buffer_p = K_L_IPCMEM_AllocateFromDedicatedBufPool(buf_type, user_id, &actual_buf_size);

    if(buffer_p == NULL)
    {
        /* This is considered as critical issue.
         * Show error message directly here
         */
        printk("\r\n<4>%s(): K_L_IPCMEM_AllocateFromDedicatedBufPool fail!", __FUNCTION__);
        return NULL;
    }

    tx_pdu_size = actual_buf_size - SYS_ADPT_TX_BUFFER_MAX_RESERVED_HEADER_LEN;
    mref_handle_p = K_L_MM_Mref_Construct(buffer_p, actual_buf_size, SYS_ADPT_TX_BUFFER_MAX_RESERVED_HEADER_LEN,
                                        tx_pdu_size,
                                        L_MM_MREF_FREE_FUN_TX_BUFFER, NULL, NULL);

    return mref_handle_p;
}

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
UI32_T K_L_MM_Mref_GetMrefPoolRequiredSize(void)
{
    return L_MM_GET_PT_TOTAL_REQUIRED_SZ(sizeof(K_L_MM_Mref_T), SYS_ADPT_MAX_NBR_OF_MREF_DESC_BLOCK);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_Mref_Construct
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
 *        cookie         - cookie of free function (0 means no cookie)
 *
 * OUTPUT:
 *        None
 *
 * RETURN:  pointer to handle of L_MM_Mref if success
 *          or return NULL if fail
 *
 * NOTES:
 *      1. ref_count will be set to 1 in K_L_MM_Mref_Construct
 *      2. the buffer must be TX/RX buffer.
 *------------------------------------------------------------------------------
 */
static L_MM_Mref_Handle_T*  K_L_MM_Mref_Construct (void*               buf,
                                          UI16_T              buf_len,
                                          UI16_T              pdu_offset,
                                          UI16_T              pdu_len,
                                          UI16_T              type,
                                          void                *cookie,
                                          void                *cookie_params)
{
    K_L_MM_Mref_T*    new_mref_p;
    K_L_MM_Monitor_T* monitor_p;

    new_mref_p = (K_L_MM_Mref_T*)K_L_MM_PtAlloc(&mref_pool_desc,
                                            L_MM_USER_ID2(SYS_MODULE_CMNLIB,
                                                          CMNLIB_TYPE_TRACE_ID_L_MM_MREF_CONSTRUCT));

    if(new_mref_p==NULL)
    {
        L_MM_DEBUG_MSG("failed to alloc new_mref_p");
        return NULL;
    }

    monitor_p = (K_L_MM_Monitor_T*)new_mref_p - 1;

    /* overwrite buffer type */
    monitor_p->buffer_type = L_MM_CONVERT_TO_INTERNAL_BUFTYPE(L_MM_MREF_BUFFER_TYPE);

    /* initialized new_mref_p */
    new_mref_p->ref_count = 1;
    new_mref_p->pdu_offset = pdu_offset;
    new_mref_p->pdu_len = pdu_len;
    new_mref_p->buf_len = buf_len;
    new_mref_p->free_func_type = type;
    new_mref_p->cookie = cookie;
    new_mref_p->cookie_params = cookie_params;
    new_mref_p->buffer_addr = buf;
    memset(&(new_mref_p->user_handle), 0, sizeof(L_MM_Mref_Handle_T));

    return (L_MM_Mref_Handle_T*)new_mref_p;
} /* end of K_L_MM_Mref_Construct() */

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
BOOL_T K_L_MM_Mref_AddRefCount (L_MM_Mref_Handle_T* mref_handle_p, UI32_T inc_count)
{
    UI32_T          oint;
    K_L_MM_Mref_T*    mref_p;

    if( (mref_p=(K_L_MM_Mref_T*)mref_handle_p) == NULL)
    {
        L_MM_DEBUG_MSG("mref_handle_p is NULL");
        return FALSE;
    }

    /* validate buffer */
    if(K_L_MM_Valid_Buf((void*)mref_p)==FALSE)
    {
        L_MM_DEBUG_MSG("invalid mref_handle_p");
        return  FALSE;
    }

    oint = SYSFUN_InterruptLock();
    mref_p->ref_count+=inc_count;
    SYSFUN_InterruptUnlock(oint);    /* restore int */

    return TRUE;
}

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
BOOL_T K_L_MM_Mref_SetFreeFuncTypeAndCookie( L_MM_Mref_Handle_T* mref_handle_p, UI32_T free_fun_type, void *cookie)
{
    UI32_T         oint;
    K_L_MM_Mref_T    *mref_p;

    if( (mref_p=(K_L_MM_Mref_T*)mref_handle_p) == NULL)
    {
        L_MM_DEBUG_MSG("mref_handle_p is NULL");
        return FALSE;
    }

    /* validate buffer */
    if(K_L_MM_Valid_Buf((void*)mref_p) == FALSE)
    {
        L_MM_DEBUG_MSG("invalid mref_handle_p");
        return  FALSE;
    }

    /* is free function and its cookie allowed to change?
     * only allow to change when:
     * (cookie must be NULL) AND (ref count must less or equal to 1)
     */
    if( (mref_p->cookie==NULL) && (mref_p->ref_count<=1) )
    {
        oint = SYSFUN_InterruptLock ();
        mref_p->free_func_type = free_fun_type;
        mref_p->cookie = cookie;
        SYSFUN_InterruptUnlock (oint );    /* restore int */
        return TRUE;
    }

    L_MM_DEBUG_MSG("Not allow to replace free function and cookie");
    return FALSE;

}

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
void* K_L_MM_Mref_MovePdu(L_MM_Mref_Handle_T* mref_handle_p, I32_T offset, UI32_T *pdu_len_p)
{
    UI32_T       oint;
    K_L_MM_Mref_T *mref_p;

    if( ((mref_p=(K_L_MM_Mref_T*)mref_handle_p) == NULL) ||
        (pdu_len_p==NULL))
    {
        L_MM_DEBUG_MSG("mref_handle_p or pdu_len is NULL.");
        return NULL;
    }

    /* validate buffer */
    if(K_L_MM_Valid_Buf((void*)mref_p) == FALSE)
    {
        L_MM_DEBUG_MSG("Invalid mref_handle_p");
        return  NULL;
    }

    /* boundary check */
    if ( (mref_p->pdu_offset + offset) > mref_p->buf_len)
    {
        if(l_mm_backdoor_dbg_msg_flag==TRUE)
        {
            printk("\r\n<4>%s() fails: the specified moving offset "
                   "exceeds the end of the buffer\r\nmoving offset=%ld",
                   __FUNCTION__, offset);
            L_MM_DEBUG_DUMP_MONITOR_INFO(((K_L_MM_Monitor_T*)mref_p-1));
        }
        return NULL;
    }

    /* offset can't be negative */
    if ((I32_T)mref_p->pdu_offset + offset < 0)
    {
        if(l_mm_backdoor_dbg_msg_flag==TRUE)
        {
            printk("\r\n<4>%s() fails: pdu_offset becomes negative value"
                   "mref->pdu_offset=%u,moving offset=%ld\r\n",
                   __FUNCTION__, mref_p->pdu_offset, offset);
            L_MM_DEBUG_DUMP_MONITOR_INFO(((K_L_MM_Monitor_T*)mref_p-1));
        }
        return NULL;
    }

    oint = SYSFUN_InterruptLock (); /* protect below so that GetPdu doesn't need protection */
    /* move pdu pointer & adjust pdu length */
    *pdu_len_p = mref_p->pdu_len = mref_p->pdu_len - offset;
    mref_p->pdu_offset = (UI16_T)(mref_p->pdu_offset + offset);
    SYSFUN_InterruptUnlock (oint );    /* restore int */

    return ((UI8_T*)(mref_p->buffer_addr) + mref_p->pdu_offset);
} /* end of K_L_MM_Mref_MovePdu */

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
BOOL_T K_L_MM_Mref_ShiftPdu(L_MM_Mref_Handle_T* mref_handle_p, UI32_T shift_offset)
{
    K_L_MM_Mref_T *mref_p;
    UI8_T       *pdu;

    if( (mref_p=(K_L_MM_Mref_T*)mref_handle_p) == NULL)
    {
        L_MM_DEBUG_MSG("mref_handle_p is NULL.");
        return FALSE;
    }

    /* check whether the buffer behind pdu is large enough to do pdu shifting */
    if( (mref_p->buf_len - mref_p->pdu_offset - mref_p->pdu_len) < shift_offset)
    {
        L_MM_DEBUG_MSG("The left buffer size behind pdu is not enough");
        return FALSE;
    }

    /* shift pdu */
    pdu = (UI8_T*)mref_p->buffer_addr + mref_p->pdu_offset;
    memmove(pdu + shift_offset, pdu, mref_p->pdu_len);

    /* update pdu offset */
    mref_p->pdu_offset += shift_offset;

    return TRUE;
}

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
void  *K_L_MM_Mref_GetPdu (L_MM_Mref_Handle_T* mref_handle_p, UI32_T *pdu_len)
{
    K_L_MM_Mref_T *mref_p;

    if( (mref_p=(K_L_MM_Mref_T*)mref_handle_p) == NULL)
    {
        L_MM_DEBUG_MSG("mref_handle_p is NULL.");
        return NULL;
    }

    /* validate buffer */
    if(K_L_MM_Valid_Buf((void*)mref_p)==FALSE)
    {
        L_MM_DEBUG_MSG("invalid mref_handle_p");
        return NULL;
    }

    *pdu_len= mref_p->pdu_len;
    return ((UI8_T*)(mref_p->buffer_addr) + mref_p->pdu_offset);
} /* end of K_L_MM_Mref_GetPdu */

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
UI32_T  K_L_MM_Mref_Release (L_MM_Mref_Handle_T** mref_handle_pp)
{
    UI32_T       oint,ref_count;
    K_L_MM_Mref_T *mref_p;

    if( (mref_handle_pp) == NULL)
    {
       /* This is considered as critical issue.
        * Show error message directly here
        */
        printk("\r\n<4>%s():mref_handle_p is NULL", __FUNCTION__);
        return L_MM_ERROR_RETURN_VAL;
    }

    mref_p=(K_L_MM_Mref_T*)(*mref_handle_pp);

    /* validate buffer */
    if(K_L_MM_Valid_Buf((void*)mref_p)==FALSE)
    {
       /* This is considered as critical issue.
        * Show error message directly here
        */
        printk("\r\n<4>%s():invalid mref_handle_p", __FUNCTION__);
        return L_MM_ERROR_RETURN_VAL;
    }

    oint = SYSFUN_InterruptLock ();
    ref_count = --(mref_p->ref_count);
    SYSFUN_InterruptUnlock (oint);    /* restore int */

    if (ref_count == 0)
    {
        *mref_handle_pp=NULL;

        if (mref_p->free_func_type == L_MM_MREF_FREE_FUN_TX_BUFFER)
            K_L_MM_FreeTxBuffer(mref_p->buffer_addr, mref_p->cookie);

        if(K_L_MM_PtFree(&mref_pool_desc, mref_p)==FALSE)
        {
           /* This is considered as critical issue.
            * Show error message directly here
            */
            printk("\r\n<4>%s():Fail to free mref descriptor", __FUNCTION__);
        }
    }

    return ref_count;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - K_L_MM_Operation
 *------------------------------------------------------------------------------
 * PURPOSE:
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static UI32_T K_L_MM_Operation(UI32_T cmd, UI32_T arg1, UI32_T arg2, UI32_T arg3, UI32_T arg4)
{
    UI32_T ret=L_MM_ERROR_RETURN_VAL;

    switch(cmd)
    {
        case L_MM_CMD_ALLOCATE_TX_BUFFER:
        {
            L_MM_Mref_Handle_T *mref_handle_p;
            /* arg1: tx_pdu_size
             * arg2: user_id
             * arg3: NULL
             * arg4: NULL
             * ret : offset to L_MM_Mref_Handle_T* / L_MM_ERROR_RETURN_VAL
             */
            mref_handle_p = K_L_MM_AllocateTxBuffer(arg1, arg2);
            ret = K_L_IPCMEM_GetOffset(mref_handle_p);
            break;
        }
        case L_MM_CMD_ALLOCATE_TX_BUFFER_FROM_DEDICATED_BUFFER:
        {
            L_MM_Mref_Handle_T *mref_handle_p;
            /* arg1: buf_type
             * arg2: user_id
             * arg3: NULL
             * arg4: NULL
             * ret : offset to L_MM_Mref_Handle_T*
             */
            mref_handle_p = K_L_MM_AllocateTxBufferFromDedicatedBufPool(arg1, arg2);
            ret = K_L_IPCMEM_GetOffset(mref_handle_p);
            break;
        }
        case L_MM_CMD_CONSTRUCT:
        {
            L_MM_Mref_ConstructArg_T arg;
            L_MM_Mref_Handle_T *mref_handle_p;
            /* arg1: L_MM_Mref_ConstructArg_T*
             * arg2: NULL
             * arg3: NULL
             * arg4: NULL
             * ret : L_MM_Mref_Handle_T*
             */
            SYSFUN_CopyFromUser(&arg, (void *)arg1, sizeof(arg));
            if(arg.free_func_type==L_MM_MREF_FREE_FUN_CUSTOM)
                mref_handle_p = K_L_MM_Mref_Construct((void*)(arg.buf_offset), arg.buf_len, arg.pdu_offset, arg.pdu_len, arg.free_func_type, arg.cookie, arg.cookie_params);
            else
            mref_handle_p = K_L_MM_Mref_Construct(K_L_IPCMEM_GetPtr((UI32_T)arg.buf_offset), arg.buf_len, arg.pdu_offset, arg.pdu_len, arg.free_func_type, arg.cookie, arg.cookie_params);
            ret = (UI32_T)K_L_IPCMEM_GetOffset(mref_handle_p);
            break;
        }
        case L_MM_CMD_ADD_REF_COUNT:
        {
            L_MM_Mref_Handle_T *mref_handle_p;
            /* arg1: offset to L_MM_Mref_Handle_T*
             * arg2: inc_count
             * arg3: NULL
             * arg4: NULL
             * ret : TRUE/FALSE
             */
            mref_handle_p = (L_MM_Mref_Handle_T*)K_L_IPCMEM_GetPtr(arg1);
            ret = (UI32_T)K_L_MM_Mref_AddRefCount(mref_handle_p, arg2);
            break;
        }
        case L_MM_CMD_SET_FREE_FUNC_TYPE_AND_COOKIE:
        {
            L_MM_Mref_Handle_T *mref_handle_p;
            /* arg1: offset to L_MM_Mref_Handle_T*
             * arg2: free_fun_type
             * arg3: cookie
             * arg4: NULL
             * ret : TRUE/FALSE
             */
            mref_handle_p = (L_MM_Mref_Handle_T*)K_L_IPCMEM_GetPtr(arg1);
            ret = (UI32_T)K_L_MM_Mref_SetFreeFuncTypeAndCookie(mref_handle_p, arg2, (void *)arg3);
            break;
        }
        case L_MM_CMD_MOVE_PDU:
        {
            L_MM_Mref_Handle_T *mref_handle_p;
            void*              new_pdu_p;
            UI32_T             pdu_len;
            /* arg1: offset to L_MM_Mref_Handle_T*
             * arg2: offset
             * arg3: pdu_len_p
             * arg4: NULL
             * ret : offset to new_pdu_p / L_MM_ERROR_RETURN_VAL
             */
            mref_handle_p = (L_MM_Mref_Handle_T*)K_L_IPCMEM_GetPtr(arg1);
            new_pdu_p = K_L_MM_Mref_MovePdu(mref_handle_p, (I32_T)arg2, &pdu_len);
            SYSFUN_CopyToUser((void *)arg3, &pdu_len, sizeof(pdu_len));
            if(((K_L_MM_Mref_T*)mref_handle_p)->free_func_type!=L_MM_MREF_FREE_FUN_CUSTOM)
                ret = (UI32_T)K_L_IPCMEM_GetOffset(new_pdu_p);
            else
                ret = (UI32_T)new_pdu_p;
            break;
        }
        case L_MM_CMD_SHIFT_PDU:
        {
            L_MM_Mref_Handle_T *mref_handle_p;
            /* arg1: offset to L_MM_Mref_Handle_T*
             * arg2: shift_offset
             * arg3: NULL
             * arg4: NULL
             * ret : TRUE/FALSE
             */
            mref_handle_p = (L_MM_Mref_Handle_T*)K_L_IPCMEM_GetPtr(arg1);
            ret = (UI32_T)K_L_MM_Mref_ShiftPdu(mref_handle_p, arg2);
            break;
        }
        case L_MM_CMD_RELEASE:
        {
            L_MM_Mref_Handle_T *mref_handle_p;
            /* arg1: offset to L_MM_Mref_Handle_T*
             * arg2: NULL
             * arg3: NULL
             * arg4: NULL
             * ret : reference count / L_MM_ERROR_RETURN_VAL
             */
            mref_handle_p = (L_MM_Mref_Handle_T *)K_L_IPCMEM_GetPtr(arg1);
            ret = K_L_MM_Mref_Release(&mref_handle_p);
            break;
        }
        case L_MM_CMD_GET_BUFFER_OFFSET:
        {
            K_L_MM_Mref_T *mref_p;
            /* arg1: offset to L_MM_Mref_Handle_T*
             * arg2: NULL
             * arg3: NULL
             * arg4: NULL
             * ret : the offset of buffer_addr
             */
            mref_p = (K_L_MM_Mref_T *)K_L_IPCMEM_GetPtr(arg1);
            ret = (UI32_T)K_L_IPCMEM_GetOffset(mref_p->buffer_addr);
            break;
        }
        case L_MM_CMD_BACKDOOR:
            switch(arg1)
            {
                case L_MM_BACKDOOR_CMD_GETBUFFERINFO:
                {
                    /* arg2: L_MM_Backdoor_BufferInfo_T*
                     * ret : TRUE  -- get an entry sucessfully
                     *       FALSE -- no more entry to get
                     */
                    L_MM_Backdoor_BufferInfo_T backdoor_buf_info;
                    if(0!=copy_from_user(&backdoor_buf_info, (void*)arg2, sizeof(backdoor_buf_info)))
                    {
                        printk("\r\n<4>%s():copy_from_user fail", __FUNCTION__);
                        return FALSE;
                    }
                    ret = K_L_MM_GetBufferInfo(&backdoor_buf_info);
                    if(0!=copy_to_user((void*)arg2, &backdoor_buf_info, sizeof(backdoor_buf_info)))
                    {
                        printk("\r\n<4>%s():copy_to_user fail", __FUNCTION__);
                        return FALSE;
                    }
                    break;
                }
                case L_MM_BACKDOOR_CMD_GETMREFINFO:
                {
                    /* arg2: L_MM_Backdoor_MrefInfo_T*
                     * ret : TRUE  -- get an entry sucessfully
                     *       FALSE -- no more entry to get
                     */
                    L_MM_Backdoor_MrefInfo_T backdoor_mref_info;
                    if(0!=copy_from_user(&backdoor_mref_info, (void*)arg2, sizeof(backdoor_mref_info)))
                    {
                        printk("\r\n<4>%s():copy_from_user fail", __FUNCTION__);
                        return FALSE;
                    }
                    ret = K_L_MM_GetMrefInfo(&backdoor_mref_info);
                    if(0!=copy_to_user((void*)arg2, &backdoor_mref_info, sizeof(backdoor_mref_info)))
                    {
                        printk("\r\n<4>%s():copy_to_user fail", __FUNCTION__);
                        return FALSE;
                    }
                    break;
                }
                case L_MM_BACKDOOR_CMD_GETTOTALNBROFALLOCATEDBUFFER:
                    /* ret : total number of allocated buffer
                     */
                     ret = l_mm_backdoor_buff_nbr;
                    break;
                case L_MM_BACKDOOR_CMD_TOGGLEDBGMSGFLAG:
                    /* ret : result of toggle debug message flag */
                    l_mm_backdoor_dbg_msg_flag = !l_mm_backdoor_dbg_msg_flag;
                    ret = l_mm_backdoor_dbg_msg_flag;
                    break;
                case L_MM_BACKDOOR_CMD_TOGGLEVALIDATEFREEFLAG:
                    /* ret : result of validate free flag
                     */
                    l_mm_backdoor_validate_free_flag = (l_mm_backdoor_validate_free_flag==TRUE) ? FALSE : TRUE;
                    ret = l_mm_backdoor_validate_free_flag;
                    break;
                default:
                    printk("\r\n<4>Unknown L_MM Backdoor cmd");
                    ret = FALSE;
            }
            break;
        default:
            printk("\r\n<4>%s:Unknown cmd(%d).", __FUNCTION__, (int)cmd);
    }

    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - K_L_MM_InsertTo_MonitorList
 *------------------------------------------------------------------------------
 * PURPOSE:  Insert a monitor node into monitor list
 * INPUT:    mm_p  -  pointer to monitor node to be inserted
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     1. Caller of this function must guarntee that the input argument is
 *              valid.
 *------------------------------------------------------------------------------
 */
static void K_L_MM_InsertTo_MonitorList(K_L_MM_Monitor_T *mm_p)
{
    UI32_T   oint;

    oint = SYSFUN_InterruptLock ();
    mm_p->next_p = l_mm_monitor_lst_head;
    mm_p->prev_p = NULL;

    if  (l_mm_monitor_lst_head != NULL)
    {
        l_mm_monitor_lst_head->prev_p = mm_p;
    }
    l_mm_monitor_lst_head = mm_p;
    l_mm_backdoor_buff_nbr++;

    SYSFUN_InterruptUnlock (oint );    /* restore int */
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
static void K_L_MM_RemoveFrom_MonitorList(K_L_MM_Monitor_T *mm_p)
{
    UI32_T   oint;

    oint = SYSFUN_InterruptLock ();

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

    SYSFUN_InterruptUnlock ( oint );    /* restore int */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - K_L_MM_Valid_Buf
 *------------------------------------------------------------------------------
 * PURPOSE:  For validating buffer prefixed with K_L_MM_Monitor_T and suffixed with
 *           trail magic bytes.
 * INPUT:    buff_p  -  pointer to buffer being validated
 * OUTPUT:   None.
 * RETURN:   TRUE  -- Valid buffer
 *           FALSE -- Invalid buffer
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static BOOL_T K_L_MM_Valid_Buf(void *buff_p)
{
    K_L_MM_Monitor_T *mm_p;
    BOOL_T is_valid_magic_bytes;

    mm_p = (K_L_MM_Monitor_T*)((UI32_T)buff_p - sizeof(K_L_MM_Monitor_T));

    /* check buffer address */
    if (mm_p->buffer_addr != mm_p)
    {
         L_MM_DEBUG_MSG("buffer validation fails through buffer_addr check");
         return FALSE;
    }

    /* check leading magic byte */
    L_MM_IS_VALID_LEADING_MAIC_BYTES(&is_valid_magic_bytes, mm_p);
    if(is_valid_magic_bytes==FALSE)
    {
        L_MM_DEBUG_MSG("buffer validation fails through leading magic byte check");
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

#if (BACKDOOR_OPEN==TRUE)

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_GetBufferInfo
 *------------------------------------------------------------------------------
 * PURPOSE:
 *  Get buffer info from the monitor list.
 *------------------------------------------------------------------------------
 * INPUT:
 * buffer_info_p->last_ref_monitor_p       --  Keep the pointer to last referenced
 *                                             monitor_p. Callers should set this
 *                                             field as NULL to retrieve the first
 *                                             entry.
 *
 * OUTPUT:
 * buffer_info_p->uid                      --  user id. See description of L_MM_USER_ID
 *                                             in L_Mm.h
 * buffer_info_p->buf_len                  --  the size of the buffer
 * buffer_info_p->buffer_type              --  buffer type. See description of L_MM_USER_ID
 *                                             in L_Mm.h
 * buffer_info_p->is_head_magic_bytes_ok   --  Is the head magic bytes of the buffer right?
 * buffer_info_p->is_trail_magic_bytes_ok  --  Is the tail magic bytes of the buffer right?
 * buffer_info_p->task_id                  --  the task id of the task who request this buffer
 * buffer_info_p->allocation_time          --  the time that the buffer was allocated
 * buffer_info_p->buffer_addr              --  the address of the buffer in kernel
 *
 * RETURN:
 *  TRUE  --  An entry is retrieved sucessfully.
 *  FALSE --  No more entry can be retrieved.
 *
 * NOTES:
 *   1.SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 */
static BOOL_T K_L_MM_GetBufferInfo(L_MM_Backdoor_BufferInfo_T *buffer_info_p)
{
    UI32_T oint;
    K_L_MM_Monitor_T *monitor_p;

    if(buffer_info_p->last_ref_monitor_p==NULL)
        monitor_p = l_mm_monitor_lst_head;
    else
        monitor_p = ((K_L_MM_Monitor_T*)(buffer_info_p->last_ref_monitor_p))->next_p;

    if(monitor_p==NULL)
        return FALSE;

    buffer_info_p->last_ref_monitor_p = monitor_p;

    oint = SYSFUN_InterruptLock();
    /* check whether the monitor_p is still valid(i.e.has not been freed)
     */
    if(monitor_p->buffer_addr==NULL)
    {
        SYSFUN_InterruptUnlock(oint);
        return FALSE;
    }

    /* monitor_p is valid, start filling buffer_info_p
     */
    buffer_info_p->task_id = monitor_p->task_id;
    buffer_info_p->allocation_time = monitor_p->allocation_time;
    buffer_info_p->buffer_addr = monitor_p->buffer_addr;
    buffer_info_p->last_ref_monitor_p = monitor_p;
    buffer_info_p->uid            = monitor_p->uid;
    buffer_info_p->buf_len        = monitor_p->buf_len;
    buffer_info_p->buffer_type    = monitor_p->buffer_type;

    L_MM_IS_VALID_LEADING_MAIC_BYTES(&(buffer_info_p->is_valid_leading_magic_byte), monitor_p);
    L_MM_IS_VALID_TRAIL_MAGIC_BYTES(&(buffer_info_p->is_valid_trail_magic_byte), monitor_p);
    SYSFUN_InterruptUnlock(oint);
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : K_L_MM_GetMrefInfo
 *------------------------------------------------------------------------------
 * PURPOSE:
 *  Get mref info from the monitor list.
 *------------------------------------------------------------------------------
 * INPUT:
 * mref_info_p->buffer_info.last_ref_monitor_p --  Keep the pointer to last referenced
 *                                                   monitor_p. Callers should set this
 *                                                   field as NULL to retrieve the first
 *                                                   entry.
 *
 * OUTPUT:
 * mref_info_p->buffer_info.uid                      --  user id. See description of L_MM_USER_ID
 *                                             in L_Mm.h
 * mref_info_p->buffer_info.buf_len                  --  the size of the buffer
 * mref_info_p->buffer_info.buffer_type              --  buffer type. See description of L_MM_USER_ID
 *                                             in L_Mm.h
 * mref_info_p->buffer_info.is_head_magic_bytes_ok   --  Is the head magic bytes of the buffer right?
 * mref_info_p->buffer_info.is_trail_magic_bytes_ok  --  Is the tail magic bytes of the buffer right?
 * mref_info_p->buffer_info.task_id                  --  the task id of the task who request this buffer
 * mref_info_p->buffer_info.allocation_time          --  the time that the buffer was allocated
 * mref_info_p->buffer_info.buffer_addr              --  the address of the buffer in kernel
 * mref_info_p->current_usr_id                       --  id of the current user of MREF
 * mref_info_p->next_usr_id                          --  id of the next user of MREF
 * mref_info_p->ref_count                            --  reference count
 * mref_info_p->pdu_offset                           --  pdu offset
 * mref_info_p->pdu_len                              --  pdu length
 *
 * RETURN:
 *  TRUE  --  An entry is retrieved sucessfully.
 *  FALSE --  No more entry can be retrieved.
 *
 * NOTES:
 *   1.SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 */
static BOOL_T K_L_MM_GetMrefInfo(L_MM_Backdoor_MrefInfo_T *mref_info_p)
{
    UI32_T oint;
    K_L_MM_Monitor_T *monitor_p;
    K_L_MM_Mref_T    *mref_p;

    if((mref_info_p->buffer_info).last_ref_monitor_p==NULL)
        monitor_p = l_mm_monitor_lst_head;
    else
        monitor_p = ((K_L_MM_Monitor_T*)((mref_info_p->buffer_info).last_ref_monitor_p))->next_p;

    if(monitor_p==NULL)
        return FALSE;

    /* locate mref buffer in the monitor list
     */
    oint = SYSFUN_InterruptLock();
    while(L_MM_GET_INTERNAL_BUFTYPE(monitor_p->buffer_type)!=L_MM_MREF_BUFFER_TYPE)
    {
        monitor_p = monitor_p->next_p;
        if(monitor_p==NULL)
    {
        SYSFUN_InterruptUnlock(oint);
        return FALSE;
    }
    }

    mref_p = (K_L_MM_Mref_T*)(monitor_p+1);

    /* monitor_p is valid, start filling mref_info_p->buffer_info
     */
    (mref_info_p->buffer_info).task_id = monitor_p->task_id;
    (mref_info_p->buffer_info).allocation_time = monitor_p->allocation_time;
    (mref_info_p->buffer_info).buffer_addr = monitor_p->buffer_addr;
    (mref_info_p->buffer_info).last_ref_monitor_p = monitor_p;
    (mref_info_p->buffer_info).uid            = monitor_p->uid;
    (mref_info_p->buffer_info).buf_len        = monitor_p->buf_len;
    (mref_info_p->buffer_info).buffer_type    = monitor_p->buffer_type;
    /* filling other mref_p fields
     */
    mref_info_p->current_usr_id = mref_p->user_handle.current_usr_id;
    mref_info_p->next_usr_id = mref_p->user_handle.next_usr_id;
    mref_info_p->ref_count = mref_p->ref_count;
    mref_info_p->pdu_offset = mref_p->pdu_offset;
    mref_info_p->pdu_len = mref_p->pdu_len;

    L_MM_IS_VALID_LEADING_MAIC_BYTES(&((mref_info_p->buffer_info).is_valid_leading_magic_byte), monitor_p);
    L_MM_IS_VALID_TRAIL_MAGIC_BYTES(&((mref_info_p->buffer_info).is_valid_trail_magic_byte), monitor_p);
    SYSFUN_InterruptUnlock(oint);
    (mref_info_p->buffer_info).last_ref_monitor_p = monitor_p;
    return TRUE;
}

#endif

