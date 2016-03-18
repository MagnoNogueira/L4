/* MODULE NAME: l4_backdoor.h
 * PURPOSE:
 *  Implement l4 backdoor
 *
 * NOTES:
 *
 * History:
 *    2005/07/22 : mfhorng      Create this file
 *
 * Copyright(C)      Accton Corporation, 2005
 */
#ifndef L4_BACKDOOR_H
#define L4_BACKDOOR_H

/* INCLUDE FILE DECLARATIONS
 */


/* NAMING CONSTANT DECLARATIONS
 */
#define L4_SUPPORT_ACCTON_BACKDOOR        TRUE


/* MACRO FUNCTION DECLARATIONS
 */


/* DATA TYPE DECLARATIONS
 */


/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
#if (L4_SUPPORT_ACCTON_BACKDOOR == TRUE)

/*------------------------------------------------------------------------------
 * FUNCTION NAME:  L4_Backdoor_CallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : l4 backdoor callback function
 * INPUT    : none
 * OUTPUT   : none.
 * RETURN   : none
 * NOTES    : none.
 *------------------------------------------------------------------------------
 */
void L4_Backdoor_CallBack();

#endif /* L4_SUPPORT_ACCTON_BACKDOOR == TRUE */


#endif /* End of L4_BACKDOOR_H */
