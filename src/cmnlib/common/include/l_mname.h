/* Module Name: L_MNAME.H
 * Purpose:
 *      Supports API to get module name for all components.
 *
 * Notes:
 *      None.
 *
 * History:
 *      Date        --  Modifier,       Reason
 *      2003.02.07  --  Erica Li,       Create
 *
 * Copyright(C)      Accton Corporation, 1999 - 2003
 */

#ifndef 	_L_MNAME_H
#define 	_L_MNAME_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_module.h"

/* NAME CONSTANT DECLARATIONS
 */
/* The maximal string length will be restricted at 16.
 */
#define L_MNAME_MAX_NAME_LEN     16

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/* FUNCTION NAME : L_MNAME_GetModuleName
 * PURPOSE:
 *      Get component module name by module id.
 *
 * INPUT:
 *      module_id   - defined in sys_module.h
 *
 * OUTPUT:
 *      module_name - a string which length will be less than L_MNAME_MAX_NAME_LEN.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. Caller must prepare enough string buffer for copy string.
 */
void L_MNAME_GetModuleName(SYS_MODULE_ID_T module_id, UI8_T *module_name);



#endif	/*	_SYS_MODULE	*/

