/*-----------------------------------------------------------------------------
 * FILE NAME: time_range_backdoor.h
 *-----------------------------------------------------------------------------
 * PURPOSE:
 *    Implementations for the Time_Range backdoor
 *
 * NOTES:
 *    None.
 *
 * HISTORY:
 *    2010/03/08 - Nelson Dai, created for Time_Range backdoor.
 *
 * Copyright(C)      Accton Corporation, 2010
 *-----------------------------------------------------------------------------
 */


#ifndef TIME_RANGE_BACKDOOR_H
#define TIME_RANGE_BACKDOOR_H

/* INCLUDE FILE DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */


/* LOCAL SUBPROGRAM DECLARATIONS
 */

/* STATIC VARIABLE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM BODIES
 */

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_BACKDOOR_Main
 *--------------------------------------------------------------------------
 * PURPOSE  : This is backdoor main function.
 * INPUT    : None
 * OUTPUT   : None
 * RETUEN   : None
 * NOTES    : Registered in TIME_RANGE_MGR_Create_InterCSC_Relation().
 *--------------------------------------------------------------------------*/
void TIME_RANGE_BACKDOOR_Main(void);

#endif /* TIME_RANGE_BACKDOOR_H */
