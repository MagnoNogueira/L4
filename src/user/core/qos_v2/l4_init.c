
#include "sys_type.h"
#include "sys_cpnt.h"
#include "l4_init.h"
#include "sysfun.h"
#include "l4_mgr.h"
#include "l4_backdoor.h"

#if (L4_SUPPORT_ACCTON_BACKDOOR == TRUE)
#include "backdoor_mgr.h"
#endif /* L4_SUPPORT_ACCTON_BACKDOOR == TRUE */

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
BOOL_T L4_INIT_Initiate_System_Resources() 
{
    BOOL_T ret=TRUE;
    
    L4_MGR_InitiateSystemResources();
    
#if(SYS_CPNT_COS == TRUE)
#if (SYS_CPNT_QOS_V2 == FALSE)
    ret &= L4_COS_MGR_Initiate_System_Resources();
#endif
#endif
        
    return ret;    
}    

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
void L4_INIT_Create_InterCSC_Relation()
{
    L4_MGR_Create_InterCSC_Relation();
#if(SYS_CPNT_COS == TRUE)
#if (SYS_CPNT_QOS_V2 == FALSE)
    L4_COS_MGR_Create_InterCSC_Relation();
#endif
#endif
   
}


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
void L4_INIT_SetTransitionMode(void)
{
    L4_MGR_SetTransitionMode();
}    



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
void L4_INIT_EnterMasterMode()
{
    L4_MGR_EnterMasterMode();
}    


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
void L4_INIT_EnterSlaveMode()
{
    L4_MGR_EnterSlaveMode();
}    



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
void L4_INIT_EnterTransitionMode()
{
    L4_MGR_EnterTransitionMode();
}    

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
void L4_INIT_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default)
{
    L4_MGR_HandleHotInsertion(starting_port_ifindex, number_of_port, use_default);

    return;
}

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
void L4_INIT_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    L4_MGR_HandleHotRemoval(starting_port_ifindex, number_of_port);

    return;
}

