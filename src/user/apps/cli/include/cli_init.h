/* Project Name: Mercury 
 * Module Name : CLI_INIT.C
 * Abstract    : to be included in root.c and tn_main.c to access cli
 * Purpose     : CLI initiation and CLI task creation
 *
 * History :                                                               
 *          Date        Modifier        Reason
 *          2001/6/1    Chin-Ju Chu     Create this file
 *
 * Copyright(C)      Accton Corporation, 1999, 2000 
 *
 * Note    : Inherit from Foxfire Switch product familiy designed by Orlando
 */

#ifndef _CLI_INIT_H_
#define _CLI_INIT_H_

BOOL_T CLI_INIT_InitiateProcessResource(void);


/*--------------------------------------------------------------------------
 * ROUTINE NAME - CLI_INIT_InitiateProcessResources
 *---------------------------------------------------------------------------
 * PURPOSE: This function initaites the system resources, such as queue, semaphore, 
 *          and events used by this subsystem. All the call back functions shall be
 *          registered during subsystem initiation.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:  This function must be invoked before any services in this subsystem can be executed.
 *---------------------------------------------------------------------------
 */
BOOL_T CLI_INIT_InitiateProcessResources(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - CLI_INIT_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/ 
void CLI_INIT_Create_InterCSC_Relation(void);

/*--------------------------------------------------------------------------
 * ROUTINE NAME - CLI_INIT_Create_Tasks()
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
void CLI_INIT_Create_Tasks(void);



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
void CLI_INIT_EnterMasterMode(void);



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
void CLI_INIT_EnterSlaveMode(void);



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
void CLI_INIT_EnterTransitionMode(void);


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
void CLI_INIT_SetTransitionMode(void);




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
 
void CLI_INIT_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);
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
void CLI_INIT_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port);


#endif /* end _CLI_INIT_H_ */
