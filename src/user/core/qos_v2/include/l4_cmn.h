/*------------------------------------------------------------------------------
 * File_Name : L4_CMN.h
 *
 * Purpose   : Provide semaphore protect for L4 component
 *
 * 2003/3/31    : Jeff Kao Create
 *
 * Copyright(C)      Accton Corporation, 2002, 2003
 *
 * Note    :
 *------------------------------------------------------------------------------
 */
#ifndef _L4_CMN_H
#define _L4_CMN_H
#include "sys_type.h"


/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_CMN_InitiateSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Create L4 componet semaphore
 * INPUT : None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES : None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_CMN_Initiate_System_Resources(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_CMN_Create_InterCSC_Relation
 *------------------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT : None
 * OUTPUT: None
 * RETURN: None
 * NOTES : None
 *------------------------------------------------------------------------------
 */
void L4_CMN_Create_InterCSC_Relation(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_CMN_EnterCriticalSection
 *------------------------------------------------------------------------------
 * PURPOSE: Enter L4 componet semaphore
 * INPUT : None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES : None
 *------------------------------------------------------------------------------
 */
UI32_T L4_CMN_EnterCriticalSection();

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_CMN_LeaveCriticalSection
 *------------------------------------------------------------------------------
 * PURPOSE: Leave L4 componet semaphore
 * INPUT : None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES : None
 *------------------------------------------------------------------------------
 */
UI32_T L4_CMN_LeaveCriticalSection();

#endif