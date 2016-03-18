/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtEnvDep.h
*
* DESCRIPTION:    Hardware environment depended types definition
*
* NOTE :
*       -- next comments relevant for the system that manipulate the CPSS via
*          the PSS
*       1. This file name remain gtEnvDep.h (like on PSS)
*       2. This file remain with __gtEnvDeph deliberately in order to be
*          duplicate with the PSS --> allowing to call the CPSS from the PSS
*          without any compiler warnings
*       3. This file need to be synchronized with the one in the PSS in order
*          to get correct behavior (not depended on the order of
*          #include <...H file> from CPSS and PSS)
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/

#ifndef __gtEnvDeph
#define __gtEnvDeph


#ifdef _VISUALC

/* Disable warning messages for VC W4 warnings level */

/* warning of casting of void * to function's pointer :
warning C4055: 'type cast' :
from data pointer 'void *' to function pointer
         'unsigned int (__cdecl *)(int ,unsigned long ,unsigned long *)'
*/
#pragma warning( disable : 4055 )


/* warning for code like " while(1) {....}":
  warning C4127: conditional expression is constant*/
#pragma warning( disable : 4127 )

#endif /* _VISUALC */

/* Add indication for the simulation to know that GT_CHAR is implimented,
   so the simulation will not define it. */
#ifndef IMPL_GT_CHAR
#define IMPL_GT_CHAR
#endif

typedef void  GT_VOID, *GT_VOID_PTR;
typedef char  GT_CHAR, *GT_CHAR_PTR;

#ifndef UNIX
typedef char  GT_8,   *GT_8_PTR;
typedef unsigned char  GT_U8,   *GT_U8_PTR;
typedef short GT_16,  *GT_16_PTR;
typedef unsigned short GT_U16,  *GT_U16_PTR;

#define VAR_32BIT   long

#if defined(CPU) && defined(SIMSPARCSOLARIS)
    #if (CPU==SIMSPARCSOLARIS)
        #undef VAR_32BIT
        #define VAR_32BIT   int
    #endif
#endif

typedef VAR_32BIT   GT_32,  *GT_32_PTR;
typedef unsigned VAR_32BIT   GT_U32,  *GT_U32_PTR;

typedef VAR_32BIT           GT_INTPTR;
typedef unsigned VAR_32BIT  GT_UINTPTR;
typedef VAR_32BIT           GT_PTRDIFF;
typedef unsigned VAR_32BIT  GT_SIZE_T;

#else /* UNIX */
/* use system defined types */
#include <inttypes.h>
#include <stddef.h>
typedef int8_t      GT_8,   *GT_8_PTR;
typedef uint8_t     GT_U8,  *GT_U8_PTR;
typedef int16_t     GT_16,  *GT_16_PTR;
typedef uint16_t    GT_U16, *GT_U16_PTR;
typedef int32_t     GT_32,  *GT_32_PTR;
typedef uint32_t    GT_U32,  *GT_U32_PTR;

typedef intptr_t    GT_INTPTR;
typedef uintptr_t   GT_UINTPTR;
typedef ptrdiff_t   GT_PTRDIFF;
typedef size_t      GT_SIZE_T;
#endif

typedef void   *GT_PTR;

typedef union
{
    GT_U8   c[8];
    GT_U16  s[4];
    GT_U32  l[2];
}GT_U64;

/*
 * typedef: enum GT_BOOL
 *
 * Description: Enumeration of boolean.
 *
 * Enumerations:
 *    GT_FALSE - false.
 *    GT_TRUE  - true.
 */
typedef enum
{
    GT_FALSE = 0,
    GT_TRUE  = 1
} GT_BOOL;


/* added macro for memory barrier synchronize   */

#ifdef PPC_CPU
#ifdef __GNUC__
#ifdef _FreeBSD
#define GT_SYNC __asm __volatile (" eieio")
#else
#define GT_SYNC __asm__("   eieio")
#endif
#else
#ifdef _DIAB_TOOL
#define GT_SYNC asm(" eieio ")
#else
#error
#endif
#endif
#endif

#ifdef MIPS_CPU
#define GT_SYNC __asm__ __volatile__ ("sync")
#endif

#ifdef INTEL_CPU
#define GT_SYNC
#endif

#ifdef CPU_ARM
#define GT_SYNC
#endif


#endif   /* __gtEnvDeph */


