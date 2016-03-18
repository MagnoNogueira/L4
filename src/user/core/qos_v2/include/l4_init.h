
#include "sys_type.h"

#ifndef _L4_INIT_H
#define _L4_INIT_H





/*------------------------------------------------------------------------------
 * ROUTINE NAME  -L4_Initiate_System_Resources
 *------------------------------------------------------------------------------
 * PURPOSE:
 * INPUT : None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES : 
 *------------------------------------------------------------------------------
 */
BOOL_T L4_INIT_Initiate_System_Resources();


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_INIT_SetTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4_INIT to enter transition mdoe
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_INIT_SetTransitionMode(void);



/*------------------------------------------------------------------------------
 * ROUTINE NAME -L4_INIT_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4_INIT enter the master mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_INIT_EnterMasterMode();


/*------------------------------------------------------------------------------
 * ROUTINE NAME -L4_INIT_EnterEnterSlaveMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4_INIT enter the slave mode
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_INIT_EnterSlaveMode();



/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_INIT_EnterTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the L4_INIT to enter transition mdoe
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void L4_INIT_EnterTransitionMode();

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_INIT_HandleHotInsertion
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is inserted at a time.
 *------------------------------------------------------------------------------
 */
void L4_INIT_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_INIT_HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is removed at a time.
 *------------------------------------------------------------------------------
 */
void L4_INIT_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  -L4_INIT_Initiate_System_Resources
 *------------------------------------------------------------------------------
 * PURPOSE:
 * INPUT : None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES : 
 *------------------------------------------------------------------------------
 */
BOOL_T L4_INIT_Initiate_System_Resources() ;

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_INIT_Create_InterCSC_Relation
 *------------------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT :  None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES :  None
 *------------------------------------------------------------------------------
 */
void L4_INIT_Create_InterCSC_Relation();

#endif
