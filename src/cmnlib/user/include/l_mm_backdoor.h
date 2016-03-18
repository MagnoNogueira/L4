/* MODULE NAME:  l_mm_backdoor.h
 * PURPOSE:
 *     User space l_mm backdoor
 *
 * NOTES:
 *
 * HISTORY
 *    2007/10/9 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2007
 */
#ifndef L_MM_BACKDOOR_H
#define L_MM_BACKDOOR_H

/* INCLUDE FILE DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_BACKDOOR_BackDoorMenu
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    L_MM kernel space backdoor menu entry funtion.
 *
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
 *    This function should be called directly by backdoor_mgr which handles backdoor
 *    transactions.
 *------------------------------------------------------------------------------
 */

void L_MM_BACKDOOR_BackDoorMenu(void);

#endif    /* End of L_MM_BACKDOOR_H */

