/* Module Name: CMNLIB_INIT.H
 * Purpose: This file take care the initialization stuff of common lib:
 *          1. Initialize the common lib resources.
 *          2. Create the Timer tasks.
 * Notes:
 * History:
 *    10/22/2001       -- Jason Hsue, Created
 *
 * Copyright(C)      Accton Corporation, 1999 - 2001
 */

#ifndef L_CMNLIB_INIT_H
#define L_CMNLIB_INIT_H

/* FUNCTION NAME: L_CMNLIB_INIT_InitiateProcessResources
 * PURPOSE: This function is used to initialize the common lib module.
 * INPUT:   None.
 * OUTPUT:  None.
 * RETUEN:  None.
 * NOTES:
 */
void L_CMNLIB_INIT_InitiateProcessResources(void);

/* FUNCTION NAME: L_CMNLIB_INIT_Create_InterCSC_Relation
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT:   None.
 * OUTPUT:  None.
 * RETUEN:  None.
 * NOTES:
 */
void L_CMNLIB_INIT_Create_InterCSC_Relation(void);

#endif
