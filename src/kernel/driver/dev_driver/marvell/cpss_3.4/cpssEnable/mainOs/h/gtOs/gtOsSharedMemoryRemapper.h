/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtOsSharedMemoryRemapper.h
*
* DESCRIPTION:
*       This file contains declarations for remapping routines which remaps
*       regular libcpss.so shared object (TEXT section is common) to
*       CPSS Shared Library (also BSS and DATA sections are common).
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __gtOsSharedMemoryRemapperh
#define __gtOsSharedMemoryRemapperh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

/************* Defines ********************************************************/

#define SHARED_MEMORY_DATA_CNS	"/dev/shm/CPSS_SHM_DATA"
#define SHARED_MEMORY_SEM_CNS	"/CPSS_SHM_SEM"

/* Address of shared BSS/DATA section */
#ifdef ASIC_SIMULATION
/* For simulation TEXT section can be too large (over 16Mb with Golden Module)
 * so we should have enough space to avoid section overlapping */
#   define SHARED_DATA_ADDR_CNS	0x26000000
#else
/*#   ifdef PPC_CPU*/
/* For PPC ELDK default ldscript uses address 0x10000000 as default address
 * for TEXT section. From the other size, we MUST locate ALL code into 16Mb
 * block to provide all offsets to fit into REL24 symbol. So we should use
 * 0x10?????? section here.
 * */
/*#       define SHARED_DATA_ADDR_CNS 0x10A00000
#   else*/
/* In some hardware architectures TEXT and PLT sections should have
 * addresses with difference no more than 24bit jump.
 * So we should place shared section with the same hi-word as code.
 * */
#       define SHARED_DATA_ADDR_CNS	0x20800000
/*#   endif*/
#endif


/************* Typedefs ********************************************************/
/*
 * Typedef: struct SECTIONS_STCT
 *
 * Description:
 *      This structure presents addresses begin and end of section.
 *
 * Fields:
 *      startPtr	- began an address a section.
 *      donePtr		- address of end of section.
 *
 *******************************************************************************/
typedef struct SECTION_STCT
{
    GT_VOID	*startPtr;
    GT_VOID	*donePtr;
} SECTION_STC;

/************* Functions ******************************************************/

/*******************************************************************************
* shrMemUnlinkShmObj
*
* DESCRIPTION:
*       Unlink shared files and semaphore
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*   GT_OK   - Operation succeeded
*   GT_FAIL - Operaton failed
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS shrMemUnlinkShmObj
(
    GT_VOID
);

/*******************************************************************************
* shrMemDoShmMap
*
* DESCRIPTION:
*       Do sharing data for all processes
*
* INPUTS:
*       isFirstClient - is client is the first CPSS client(Enabler) or not.
*
* OUTPUTS:
*       None
*
* RETURNS:
*   GT_OK   - Operation succeeded
*   GT_FAIL - Operaton failed
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS shrMemDoShmMap
(
    GT_BOOL isFirstClient
);

/*******************************************************************************
* shrMemPrintMapsDebugInfo
*
* DESCRIPTION:
*       Ouputs /proc/self/maps to stderr for debug purpose
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       Should be used only for debugging on SHARED_MEMORY=1
*
*******************************************************************************/
GT_VOID shrMemPrintMapsDebugInfo(GT_VOID);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __gtOsSharedMemoryRemapperh */
