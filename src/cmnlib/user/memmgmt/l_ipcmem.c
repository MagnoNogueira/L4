/* MODULE NAME:  L_IPCMEM.c
 * PURPOSE:
 * This module is for allocating and freeing buffer for lan packet transmission.
 *
 * NOTES:
 * This module need to cooperate with lanbuf device.
 * Before using this library, ensure that lanbuf module have been loaded into
 * kernel and "/dev/lanbuf" must have been created.
 *
 * HISTORY
 *    2006.9.21 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2006
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <unistd.h>    /* for getpagesize() */
#include <sys/mman.h>  /* for mmap() */
#include <sys/types.h> /* for open() */
#include <sys/stat.h>  /* for open() */
#include <fcntl.h>     /* for open() */
#include <errno.h>

#include "sys_type.h"
#include "sys_adpt.h"
#include "sys_hwcfg.h"
#include "sysfun.h"
#include "l_cvrt.h"
#include "l_ipcmem.h"
#include "l_ipcmem_type.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static BOOL_T L_IPCMEM_GetIPCMemInfo(UI32_T *physical_base_addr_p,
                                     UI32_T *physical_mem_size_p,
                                     UI32_T ipcmem_size[L_IPCMEM_MEM_ID_MAX],
                                     UI32_T ipcmem_offset[L_IPCMEM_MEM_ID_MAX]);

/* STATIC VARIABLE DECLARATIONS
 */
static void   *mem_buffer_base_addr = NULL;
static void   *mvpp_buffer_base_addr = NULL;
static int    fd = -1;
static int    mvppfd = -1;

static UI32_T l_ipcmem_mem_size_ar[L_IPCMEM_MEM_ID_MAX];
static UI32_T l_ipcmem_mem_offset_ar[L_IPCMEM_MEM_ID_MAX];

static void   *uc_mem_buffer_base_addr = NULL;

static BOOL_T l_ipcmem_has_initialize = FALSE;

/* EXPORTED SUBPROGRAM BODIES
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
void L_IPCMEM_Preinitialize_SetVirtualBaseAddress(UI32_T virtual_base_address)
{
#if (L_IPCMEM_ALLOC_MEM_FROM_BCM == TRUE)
    mem_buffer_base_addr = (void *)virtual_base_address;
#else
    /* do nothing */
#endif
}

void L_IPCMEM_Preinitialize_InitMemoryPool(void *(*alloc_func)(unsigned int size))
{
#if (L_IPCMEM_ALLOC_MEM_FROM_BCM == TRUE && L_IPCMEM_ALLOC_MEM_FROM_BCM_KERNEL == FALSE)
    void *mem_buffer_real_start_addr = NULL;
#endif
    UI32_T mem_buffer_real_start_offset;

#if (L_IPCMEM_ALLOC_MEM_FROM_BCM == TRUE && L_IPCMEM_ALLOC_MEM_FROM_BCM_KERNEL == FALSE)
    if (mem_buffer_base_addr == NULL || alloc_func == NULL)
    {
        printf("\r\n%s: alloc_func is invalid.\r\n", __FUNCTION__);
        return;
    }

    mem_buffer_real_start_addr = alloc_func(8 * SYS_TYPE_1M_BYTES);
    mem_buffer_real_start_offset = (UI32_T)mem_buffer_real_start_addr - (UI32_T)mem_buffer_base_addr;
#else
    /* -1 means to initialize memory pool without specified memory buffer.
     */
    mem_buffer_real_start_offset = (UI32_T)-1;
#endif

    SYSFUN_Syscall(SYSFUN_SYSCALL_L_IPCMEM, L_IPCMEM_CMD_INITIALIZE_BUF_POOL, mem_buffer_real_start_offset, 0, 0, 0);
}

/*  FUNCTION NAME : L_IPCMEM_Initialize
 *  PURPOSE:
 *      Initializes L_IPCMEM.
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
 *
 *  NOTES:
 *      None.
 */
BOOL_T L_IPCMEM_Initialize(void)
{
    UI32_T physical_base_addr, physical_mem_size;
    UI32_T total_size_of_buf_pool = 0;
    UI32_T i;

    if (l_ipcmem_has_initialize)
    {
        return TRUE;
    }

    if (fd == -1)
        fd = open("/dev/mem", O_RDWR);

    if (!L_IPCMEM_GetIPCMemInfo(&physical_base_addr, &physical_mem_size, l_ipcmem_mem_size_ar, l_ipcmem_mem_offset_ar))
    {
        printf("\r\n%s:L_IPCMEM_GetIPCMemInfo fail.", __FUNCTION__);
        return FALSE;
    }

    L_IPCMEM_Debug_Print("\r\n%s:physical_base_addr = %08x physical_mem_size = %08x",
            __FUNCTION__, (unsigned int)physical_base_addr, (unsigned int)physical_mem_size);

    for (i = 0; i < L_IPCMEM_MEM_ID_MAX; i++)
    {
        total_size_of_buf_pool += l_ipcmem_mem_size_ar[i];
        L_IPCMEM_Debug_Print("\r\n%s:mem%d size:%08x offset:%08x", __FUNCTION__, (int)i, (unsigned int)l_ipcmem_mem_size_ar[i], (unsigned int)l_ipcmem_mem_offset_ar[i]);
    }

    L_IPCMEM_Debug_Print("\r\n%s:fd is %d", __FUNCTION__, fd);

#ifndef MARVELL_CPSS
    if (mem_buffer_base_addr == NULL)
    {
        mem_buffer_base_addr = mmap(0, (size_t)physical_mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, physical_base_addr);
        if(mem_buffer_base_addr == MAP_FAILED)
        {
            printf("\r\n%s:mmap fail.", __FUNCTION__);
            return FALSE;
        }
    }

    L_IPCMEM_Debug_Print("\r\n%s:mem_buffer_base_addr = %08x\r\n", __FUNCTION__, (int)mem_buffer_base_addr);
#else
    if (mem_buffer_base_addr == NULL)/* map tx and mref. */
    {
        mem_buffer_base_addr = mmap(SYS_ADPT_DMA_VIRTUAL_ADDR-2*SYS_TYPE_1M_BYTES, (size_t)physical_mem_size-2*SYS_TYPE_1M_BYTES, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_SHARED, fd, physical_base_addr);
        if(mem_buffer_base_addr == MAP_FAILED)
        {
            printf("\r\n%s:mmap fail.", __FUNCTION__);
            return FALSE;
        }
    }

    L_IPCMEM_Debug_Print("\r\n%s:mem_buffer_base_addr = %08x\r\n", __FUNCTION__, (int)mem_buffer_base_addr);

    if (mvppfd == -1)
        mvppfd = open("/dev/mvPP", O_RDWR);
    
    L_IPCMEM_Debug_Print("\r\n%s:fd is %d", __FUNCTION__, mvppfd);
    if (mvpp_buffer_base_addr == NULL)/* map dma buffer. */
    {
        mvpp_buffer_base_addr = mmap(SYS_ADPT_DMA_VIRTUAL_ADDR, 2*SYS_TYPE_1M_BYTES, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_SHARED, mvppfd, physical_base_addr+2*SYS_TYPE_1M_BYTES);
        if(mvpp_buffer_base_addr == MAP_FAILED)
        {
            printf("\r\n%s:mmap fail.", __FUNCTION__);
            return FALSE;
        }
    }
    L_IPCMEM_Debug_Print("\r\n%s:mvpp_buffer_base_addr = %08x\r\n", __FUNCTION__, (int)mvpp_buffer_base_addr);
#endif


#if 0
    {
        void *mref_buf_p = L_CVRT_GET_PTR(mem_buffer_base_addr, l_ipcmem_mem_offset_ar[L_IPCMEM_MEM_ID_MREF]);
        printf("Dump: %08x", (unsigned int)mref_buf_p);
        DBG_DumpHex("", 128, mref_buf_p);
    }
#endif

    l_ipcmem_has_initialize = TRUE;
    return TRUE;
}

/*--------------------------------------------------------------------------
 * FUNCTION NAME - L_IPCMEM_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/
void L_IPCMEM_Create_InterCSC_Relation(void)
{
    return;
}

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
void* L_IPCMEM_GetPtr(UI32_T offset)
{
    return L_CVRT_GET_PTR(mem_buffer_base_addr, offset);
}

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
I32_T L_IPCMEM_GetOffset(void* buf_p)
{
    I32_T offset = L_CVRT_GET_OFFSET(mem_buffer_base_addr, buf_p);

    if(buf_p==NULL)
        return -1;

    if(mem_buffer_base_addr == NULL)
    {
        SYSFUN_Debug_Printf("\r\n%s():L_IPCMEM has not been initialized", __FUNCTION__);
    }

    if(offset < 0)
    {
        SYSFUN_Debug_Printf("\r\n%s():Invalid buf_p", __FUNCTION__);
    }
    return offset;
}

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
BOOL_T L_IPCMEM_GetPhysicalBaseAddress(UI32_T *physical_base_address)
{
    if (physical_base_address==NULL)
        return FALSE;

    return SYSFUN_Syscall(SYSFUN_SYSCALL_L_IPCMEM, L_IPCMEM_CMD_GET_PHYSICAL_BASE_ADDRESS, (UI32_T)physical_base_address, 0, 0, 0);
}

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
BOOL_T L_IPCMEM_GetTxBufStartAddress(UI32_T *tx_buf_start_address)
{
    if ((!l_ipcmem_has_initialize) && (!L_IPCMEM_Initialize()))
        return FALSE;

    *tx_buf_start_address = (UI32_T)L_CVRT_GET_PTR(mem_buffer_base_addr, l_ipcmem_mem_offset_ar[L_IPCMEM_MEM_ID_TX]);
    return TRUE;
}

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
BOOL_T L_IPCMEM_GetRxBufStartAddress(UI32_T *rx_buf_start_address)
{
    if ((!l_ipcmem_has_initialize) && (!L_IPCMEM_Initialize()))
        return FALSE;

    *rx_buf_start_address = (UI32_T)L_CVRT_GET_PTR(mem_buffer_base_addr, l_ipcmem_mem_offset_ar[L_IPCMEM_MEM_ID_RX]);
    return TRUE;
}

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
BOOL_T L_IPCMEM_GetUCBufStartAddress(UI32_T *uc_buf_start_address)
{
/* This function is obsolete now,
 * ucmgmt should use L_IPCMEM_AttachUCBuffer instead.
 */
    if (uc_mem_buffer_base_addr == NULL)
        return FALSE;

    *uc_buf_start_address = (UI32_T)uc_mem_buffer_base_addr;
    return TRUE;
}

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
BOOL_T L_IPCMEM_AttachUCBuffer(UI32_T uc_physical_start_addr, UI32_T *uc_buf_start_address)
{
    UI32_T uc_real_physical_addr;
    void *uc_read_virtual_addr;

    if (uc_mem_buffer_base_addr == NULL)
    {
        int pagesize = getpagesize();

        uc_real_physical_addr = L_ALIGN(uc_physical_start_addr + 1 - pagesize, pagesize);
        uc_read_virtual_addr = mmap(0, L_ALIGN(SYS_HWCFG_UC_MEM_SIZE, pagesize), PROT_READ|PROT_WRITE, MAP_SHARED, fd, uc_real_physical_addr);
        if (uc_read_virtual_addr == MAP_FAILED)
        {
            printf("\r\n%s:mmap fail: %d\r\n", __FUNCTION__, errno);
            return FALSE;
        }
        uc_mem_buffer_base_addr = (void *)((UI32_T)uc_read_virtual_addr + uc_physical_start_addr - uc_real_physical_addr);
    }
    *uc_buf_start_address = (UI32_T)uc_mem_buffer_base_addr;
    return TRUE;
}

/* LOCAL SUBPROGRAM BODIES
 */
static BOOL_T L_IPCMEM_GetIPCMemInfo(UI32_T *physical_base_addr_p,
                                     UI32_T *physical_mem_size_p,
                                     UI32_T ipcmem_size[L_IPCMEM_MEM_ID_MAX],
                                     UI32_T ipcmem_offset[L_IPCMEM_MEM_ID_MAX])
{
    return SYSFUN_Syscall(SYSFUN_SYSCALL_L_IPCMEM,
                          L_IPCMEM_CMD_GET_IPCMEM_INFO,
                          (UI32_T)physical_base_addr_p,
                          (UI32_T)physical_mem_size_p,
                          (UI32_T)ipcmem_size,
                          (UI32_T)ipcmem_offset);
}
