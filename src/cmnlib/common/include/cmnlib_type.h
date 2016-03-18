/* MODULE NAME:  Cmnlib_type.h
 * PURPOSE:
 *  For common definition for cmnlib.
 * NOTES:
 *
 * HISTORY
 *    2/17/2006 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2006
 */
#ifndef CMNLIB_TYPE_H
#define CMNLIB_TYPE_H

/* INCLUDE FILE DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
/* for trace_id of user_id when allocate buffer with l_mm
 */
enum
{
    CMNLIB_TYPE_TRACE_ID_L_MM_MREF_CONSTRUCT=0,
    CMNLIB_TYPE_TRACE_ID_L_BASE64,
    CMNLIB_TYPE_TRACE_ID_L_LINK_LST,
    CMNLIB_TYPE_TRACE_ID_SYSFUN_BACKDOOR_SHOWCPUUSAGE
};

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

# endif    /* End of CMNLIB_TYPE_H */
