/* MODULE NAME:  L_IPCMEM_PRIVATE.h
 * PURPOSE:
 *  Private definitions for L_Mem.
 *
 * NOTES:
 *
 * HISTORY
 *    2006/11/1 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2006
 */
#ifndef L_IPCMEM_PRIVATE_H
#define L_IPCMEM_PRIVATE_H

/* INCLUDE FILE DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */
#define L_IPCMEM_NUMBER_OF_BUF_TYPE         5

/* get himem from broadcom?
 */
#ifndef MARVELL_CPSS
#define L_IPCMEM_ALLOC_MEM_FROM_BCM         TRUE
#else
#define L_IPCMEM_ALLOC_MEM_FROM_BCM         FALSE
#endif
#if (L_IPCMEM_ALLOC_MEM_FROM_BCM == TRUE)
#define L_IPCMEM_ALLOC_MEM_FROM_BCM_KERNEL  FALSE
#endif

/* L_IPCMEM system call command definition
 */
enum
{
    L_IPCMEM_CMD_BACKDOOR = 0,
    L_IPCMEM_CMD_GET_PHYSICAL_BASE_ADDRESS,
    L_IPCMEM_CMD_GET_IPCMEM_INFO,
    L_IPCMEM_CMD_INITIALIZE_BUF_POOL,
};

/* L_IPCMEM system call backdoor command definition
 */
enum
{
    L_IPCMEM_BACKDOOR_CMD_GETBACKDOORINFO =0,
    L_IPCMEM_BACKDOOR_CMD_GETTMZINFO
};

#define MEM_TMZ_SIZE        1024
#define MEM_TMZ_PATTERN     0x32


/* The ID for each type of memory
 * ------------------------------
 * high memory initial procedure
 *  1. run acctonlkm first.
 *  2. driver call L_IPCMEM_Preinitialize to set base address and
 *     initialize memory pool in kernel.
 *  3. processes call L_IPCMEM_Initialize to get their local base
 *     address.
 * uc buffer
 *  1. uc base address/size is set after calling L_IPCMEM_AttachUCBuffer.
 *     it means that uc base address/size isn't available until ucmgmt initial.
 */
enum {
    L_IPCMEM_MEM_ID_TX,
    L_IPCMEM_MEM_ID_RX,
    L_IPCMEM_MEM_ID_MREF,
//    L_IPCMEM_MEM_ID_UC,
    L_IPCMEM_MEM_ID_MAX
};


/* MACRO FUNCTION DECLARATIONS
 */
#define L_IPCMEM_DEBUG    0

#if L_IPCMEM_DEBUG
    #define L_IPCMEM_Debug_Print(...) printf(__VA_ARGS__)
    #define K_L_IPCMEM_DEBUG_PRINT(...)  printk(__VA_ARGS__)
#else
    #define L_IPCMEM_Debug_Print(...)
    #define K_L_IPCMEM_DEBUG_PRINT(...)  
#endif


/* DATA TYPE DECLARATIONS
 */

/* structure for retrieving L_IPCMEM backdoor information
 */
typedef struct L_Mem_BackdoorInfoEntry_S
{
    UI32_T alloc_counter;
    UI32_T free_counter;
    UI32_T used_count;
    UI32_T peak_count;
    UI32_T fail_count;
    UI16_T partition_len;
    UI16_T num_of_block;
} L_Mem_BackdoorInfoEntry_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

#endif    /* End of L_IPCMEM_PRIVATE_H */

