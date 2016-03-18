/* Module Name: CMNLIB_INIT.C
 * Purpose: This file take care the initialization stuff of common lib:
 *          1. Initialize the common lib resources.
 *          2. Create the Timer tasks.
 * Notes:
 * History:
 *    10/22/2001       -- Jason Hsue, Created
 *
 * Copyright(C)      Accton Corporation, 1999 - 2001
 */


/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "l_ipcmem.h"
#include "l_mm.h"
#include "sysfun.h"
#include "l_hash.h"
#include "syslog_pmgr.h"

/* FUNCTION NAME: L_CMNLIB_INIT_InitiateProcessResources
 * PURPOSE: This function is used to initialize the common lib module.
 * INPUT:   None.
 * OUTPUT:  None.
 * RETUEN:  None.
 * NOTES:
 *
 */
void L_CMNLIB_INIT_InitiateProcessResources(void)
{
    /* initialize memory pool, mreference and system function
     */
printf("%s:%d ipcmem disabled\r\n", __FUNCTION__, __LINE__);
    if (0)
    L_IPCMEM_Initialize();
    
    L_MM_Init();
    SYSFUN_Init();

//    SYSLOG_PMGR_InitiateProcessResource(); /* SYSFUN will ref SYSLOG_PMGR */
// TODO: change design
#if 0
#ifndef INCLUDE_DIAG
    L_HASH_Init();
#endif
#endif
} /* L_CMNLIB_INIT_InitiateProcessResources() */

/* FUNCTION NAME: L_CMNLIB_INIT_Create_InterCSC_Relation
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT:   None.
 * OUTPUT:  None.
 * RETUEN:  None.
 * NOTES:
 */
void L_CMNLIB_INIT_Create_InterCSC_Relation(void)
{
    L_IPCMEM_Create_InterCSC_Relation();
    L_MM_Create_InterCSC_Relation();

// TODO: change design
#if 0
#ifndef INCLUDE_DIAG
    L_HASH_Create_InterCSC_Relation();
#endif
#endif
}

