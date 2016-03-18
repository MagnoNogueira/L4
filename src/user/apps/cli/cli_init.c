/* Project Name: Mercury 
 * Module Name : CLI_INIT.C
 * Abstract    : to be included in root.c to access cli
 * Purpose     : CLI initiation and CLI task creation
 *
 * History :                                                               
 *          Date        Modifier        Reason
 *          2001/6/1    Chin-Ju Chu     Create this file
 *
 * Copyright(C)      Accton Corporation, 2001
 *
 */

 
/*------------------------------------------------------------------------
 * INCLUDE STRUCTURES                             
 *------------------------------------------------------------------------*/
#include "sys_type.h"
#include "cli_init.h"
#include "cli_type.h"
#include "cli_task.h"
#include "cli_mgr.h"
#include "cli_main.h"



#if defined(JBOS) 
#define CLI_BACKDOOR 1
#endif


#if CLI_BACKDOOR
#include "cli_backdoor.h"
#endif

/* EXPORTED SUBPROGRAM BODIES
 */
extern void CLI_MGR_Create_InterCSC_Relation(void);


BOOL_T CLI_INIT_InitiateProcessResource(void)
{
    return TRUE;
}
/*--------------------------------------------------------------------------
 * ROUTINE NAME - CLI_INIT_InitiateProcessResources
 *---------------------------------------------------------------------------
 * PURPOSE: This function initaites the system resources, such as queue, semaphore, 
 *          and events used by this subsystem. All the call back functions shall be
 *          registered during subsystem initiation.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE: 1. This function must be invoked before any tasks in this subsystem can be created.
 *       2. This function must be invoked before any services in this subsystem can be executed.
 *---------------------------------------------------------------------------
 */
BOOL_T CLI_INIT_InitiateProcessResources(void)
{
    CLI_MGR_Init();
    CLI_TASK_Init();
    return TRUE;
} /* end of CLI_INIT_Init() */

/*--------------------------------------------------------------------------
 * FUNCTION NAME - CLI_INIT_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/ 
void CLI_INIT_Create_InterCSC_Relation(void)
{
    CLI_MGR_Create_InterCSC_Relation();
} /* end of CLI_INIT_Create_InterCSC_Relation */


/*--------------------------------------------------------------------------
 * ROUTINE NAME - CLI_INIT_Create_Task()
 *---------------------------------------------------------------------------
 * PURPOSE: This function creates all the task of this subsystem.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE: 1. This function shall not be invoked before CLI_INIT_Init() is performed.
 *       2. All the telnet session task will be dynamically created by the incoming
 *          telent request.
 *---------------------------------------------------------------------------
 */
void CLI_INIT_Create_Tasks(void)
{

    /* 1.Create console task since this fuction can only be called by Root
     * 2.Specify Only session Type, need not to care socket, *buffer, and buffer_size
     */
    /*add unit id 0 means all unit*/
    (void)CLI_TASK_CreateTask (CLI_TYPE_UART, 0, 0,NULL, 0);

} /* End of CLI_INIT_Create_Task() */



/*-------------------------------------------------------------------------
 * ROUTINE NAME - CLI_INIT_EnterMasterMode                         
 *-------------------------------------------------------------------------
 * PURPOSE: This function initiates all the system database, and also configures
 *          the switch to the initiation state based on the specified "System Boot
 *          Configruation File". After that, the CLI subsystem will enter the
 *          Master Operation mode.                                                            
 * INPUT   : None														   
 * OUTPUT  : None														   
 * RETURN  : None														   
 * NOTE: 1. All the other subsystems must enter the master mode before this function
 *          can be invoked. 
 *       2. If "System Boot Configruation File" does not exist, the system database and
 *          switch will be initiated to the factory default value.
 *       3. CLI/Telnet sessions will handle all Cisco Commands only when this subsystem
 *          is in the Master Operation mode 
 *-------------------------------------------------------------------------
 */
void CLI_INIT_EnterMasterMode(void)
{

	CLI_MGR_Opmode_Enter_Master_Mode();
	/* CLI_SESS_Enter_Master_Mode(); */
} /* End of CLI_INIT_DB_Init_And_Enter_Master_Mode() */



/*-------------------------------------------------------------------------
 * ROUTINE NAME - CLI_INIT_EnterSlaveMode									
 *-------------------------------------------------------------------------
 * Purpose: This function forces this subsystem enter the Slave Operation mode.										
 * INPUT   : None															
 * OUTPUT  : None															
 * RETURN  : None														
 * NOTE: In Slave Operation mode, any Cisco Commands issued from local console or 
 *       Telnet will be ignored.															
 *-------------------------------------------------------------------------
 */
 
void CLI_INIT_EnterSlaveMode(void)
{
	CLI_MGR_Opmode_Enter_Slave_Mode();
	/* CLI_SESS_Enter_Slave_Mode(); */
	
	/* release resources
	 */
} /* end CLI_INIT_Enter_Slave_Mode() */


/*-------------------------------------------------------------------------
 * ROUTINE NAME - CLI_INIT_EnterTransitionMode									
 *-------------------------------------------------------------------------
 * Purpose: This function forces this subsystem enter the Transition Operation mode.										
 * INPUT   : None															
 * OUTPUT  : None															
 * RETURN  : None														
 * NOTE: In Transition Operation mode, any Cisco Commands issued from local console or 
 *       Telnet will be ignored.													
 *-------------------------------------------------------------------------
 */
void CLI_INIT_EnterTransitionMode(void)
{
    CLI_MGR_Opmode_Enter_Transition_Mode();
}

/*-------------------------------------------------------------------------
 * ROUTINE NAME - CLI_INIT_SetTransitionMode									
 *-------------------------------------------------------------------------
 * Purpose: This function set this subsystem to Transition Operation mode.										
 * INPUT   : None															
 * OUTPUT  : None															
 * RETURN  : None														
 * NOTE: In Transition Operation mode, any Cisco Commands issued from local console or 
 *       Telnet will be ignored.													
 *-------------------------------------------------------------------------
 */
void CLI_INIT_SetTransitionMode(void)
{
	CLI_MGR_SetTransitionMode();
	CLI_TASK_SetTransitionMode();
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_INIT_HandleHotInsertion
 * ------------------------------------------------------------------------
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
 * ------------------------------------------------------------------------
 */
void CLI_INIT_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default)
{
    CLI_MGR_HandleHotInsertion(starting_port_ifindex, number_of_port, use_default);
    return;  
}

 

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_INIT_HandleHotRemoval
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is removed at a time.
 * ------------------------------------------------------------------------
 */
void CLI_INIT_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port)
{    
    CLI_MGR_HandleHotRemoval(starting_port_ifindex, number_of_port);
    return;   
}
/* End of CLI_INIT.C */


