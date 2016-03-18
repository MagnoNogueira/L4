/* MODULE NAME:  l_ipcmem_backdoor.h
 * PURPOSE:
 *     User space l_ipcmem backdoor
 *
 * NOTES:
 *
 * HISTORY
 *    2009/9/28 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2009
 */
#ifndef L_IPCMEM_BACKDOOR_H
#define L_IPCMEM_BACKDOOR_H

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
 * ROUTINE NAME : L_IPCMEM_BACKDOOR_BackDoorMenu
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    L_IPCMEM kernel space backdoor menu entry funtion.
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

void L_IPCMEM_BACKDOOR_BackDoorMenu(void);

#endif    /* End of L_IPCMEM_BACKDOOR_H */


