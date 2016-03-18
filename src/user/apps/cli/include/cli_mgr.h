/* MODULE NAME:  cli_mgr.h
 * PURPOSE:
 *     functions declarations and related definations
 *
 * NOTES:
 *
 * REASON:
 * Description:
 * HISTORY
 *    06/02/2007 - rich Lee, Created for Linux platform
 *
 * Copyright(C)      Accton Corporation, 2007
 */
#ifndef CLI_MGR_H
#define CLI_MGR_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_cpnt.h"
#include "sysfun.h"
#include "cli_def.h"


//#if 0 /* rich linux */
//#include "stktplg_mgr.h"
//#else
//#include "stktplg_om.h"
//#endif

/* NAMING CONSTANT DECLARATIONS
 */
#define CLI_MGR_RUNTIME_PROVISION_MAGIC_WORD        "runtime_provision"
#define CLI_MGR_RUNTIME_PROVISION_UC_MEM_SIZE       1024*100    /*100k*/
#define CLI_MGR_RUNTIME_PROVISION_UC_MEM_BOUNDRY    4
#define CLI_MGR_OEM_LOGO_UC_MEM_SIZE                8
#define CLI_MGR_NUMBER_OF_DEVICE_PER_UNIT           2
#define CLI_MGR_NUMBER_OF_TOTAL_DEVICE              CLI_MGR_NUMBER_OF_DEVICE_PER_UNIT * SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK
#define CLI_MGR_UPORT_TO_IFINDEX(unit,port)         ((unit-1)*SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT+port)
#define CLI_MGR_IFINDEX_TO_UNIT(ifindex)            ( ((UI32_T)((ifindex-1)/SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT))+1 )
#define CLI_MGR_IFINDEX_TO_PORT(ifindex)            ( ifindex - (CLI_MGR_IFINDEX_TO_UNIT(ifindex)-1)*SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT )



/* definitions of command in CLI which will be used in ipc message
 */
/* MACRO FUNCTION DECLARATIONS
 */
/* Macro function for calculation of ipc msg_buf size based on structure name
 * used in CSCA_OM_IPCMsg_T.data
 */



#define CLI_MGR_GET_MSGBUFSIZE(field_name)                       \
            (CLI_MGR_MSGBUF_TYPE_SIZE +                        \
            sizeof(((CLI_MGR_IPCMsg_T*)0)->data.field_name))


#define CLI_MGR_MSGBUF_TYPE_SIZE sizeof(union CLI_MGR_IPCMsg_Type_U)


/* DATA TYPE DECLARATIONS
 */
/* structure for the request/response ipc message in cli group pmgr and mgr
 */
typedef struct
{
    union CLI_MGR_IPCMsg_Type_U
    {
        UI32_T cmd;    /* for sending IPC request. */
        UI32_T result; /* for response */
    } type;

    union
    {
        struct CLI_MGR_IPCMsg_Reproduce_Data_S
        {
            UI32_T is_reproduce;
        } Reproduce_data;

        struct CLI_MGR_IPCMSG_STKCTRL_SAVING_CONFIG_STATUS_S
        {
            BOOL_T status;
        } config_status_data;

        struct CLI_MGR_IPCMsg_Session_Data_S
        {
            UI32_T session_id;
        } session_data;

        struct CLI_MGR_IPCMsg_Running_config_S
        {
            I32_T   buffer;
            UI32_T  buffer_size;
        } running_config_data;
    } data; /* contains the supplemntal data for the corresponding cmd */
} CLI_MGR_IPCMsg_T;

typedef struct
{
    char   unit_port_string[50];
}CLI_STRLB_CMD_T;

enum CLI_MGR_OPMODE_E
{
   CLI_MGR_OPMODE_MASTER_MODE     = SYS_TYPE_STACKING_MASTER_MODE,
   CLI_MGR_OPMODE_TRANSITION_MODE = SYS_TYPE_STACKING_TRANSITION_MODE,
   CLI_MGR_OPMODE_SLAVE_MODE      = SYS_TYPE_STACKING_SLAVE_MODE
};

enum CLI_MGR_RUNCFG_RETURN_TYPE_E
{
   CLI_MGR_RUNCFG_RETURN_OK = 0,
   CLI_MGR_RUNCFG_RETURN_NO_ENOUGH_MEMORY = 0xffffffff
};

enum CLI_MGR_AUTOBAUDRATE_RETURN_E
{
    CLI_MGR_AUTOBAUDRATE_NOT_DETECT_ENTERKEY = FALSE,
    CLI_MGR_AUTOBAUDRATE_DETECT_ENTERKEY = TRUE,
    CLI_MGR_AUTOBAUDRATE_NOT_ENABLE = 2
};

/* definitions of command which will be used in ipc message
 */
enum
{
    CLI_MGR_IPC_ALLCSCSKNOWPROVISIONCOMPLETE,
    CLI_MGR_IPC_GET_IS_REPRODUCE_DONE,
    CLI_MGR_IPC_SAVING_CONFIG_STATUS,
    CLI_MGR_IPC_ENTER_TRANSITION_MODE,
    CLI_MGR_IPC_INC_REMOTE_SESSION,
    CLI_MGR_IPC_DEC_REMOTE_SESSION,
    CLI_MGR_IPC_SET_KILL_WORKING_SPACE_FLAG,
    CLI_MGR_IPC_GET_RUNNING_CONFIG,
    CLI_MGR_IPC_CLOSE_ALL_TELNET_SESSIONS,
    CLI_MGR_IPC_HANDLE_CHANGED_IP_MGMT_FILTER,
};

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
void CLI_MGR_HandleSysCallbackIPCMsg(SYSFUN_Msg_T* msgbuf_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_MGR_HandleIPCReqMsg
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Handle the ipc request message for cli mgr.
 * INPUT:
 *    ipcmsg_p  --  input request ipc message buffer
 *
 * OUTPUT:
 *    ipcmsg_p  --  output response ipc message buffer
 *
 * RETURN:
 *    TRUE  - There is a response need to send.
 *    FALSE - There is no response to send.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p);

/* Function - CLI_MGR_GetVersionNumberDisplayString
 * Purpose  - Get the display string form UI32_T to a standard version display format.
 * Input    - UI32_T version -> version number
 *            UI8_T * str -> string buffer at max 11 length
 * Output   - str -> the output string
 * Note     - Constants from sys_imghdr.h
 *  #define IMGHDR_IMAGE_LOADERTYPE                 0
 *  #define IMGHDR_IMAGE_RUNTIMETYPE                1
 *  #define IMGHDR_IMAGE_DIAGNOSTICTYPE             2
 * History  - Reginald 2004.12.22 Create
 */
BOOL_T CLI_MGR_GetVersionNumberDisplayString( UI32_T version, UI32_T image_type, UI8_T * str );

/* Routine: CLI_MGR_GetRovingAnalysisModulePortUnit
 * Purpose: Get unit if add a module port as mirror destination
 * Input:   unit id
 * Output:  None.
 * Return:  TRUE: Success. FALSE: failed.
 * Note:    None.
 * History: Create  Reginald Ian    2004.11.10
 */
BOOL_T CLI_MGR_GetRovingAnalysisModulePortUnit( UI32_T * unit_id );

/* Routine: CLI_MGR_SetRovingAnalysisModulePortUnit
 * Purpose: Set unit if add a module port as mirror destination
 * Input:   unit id
 * Output:  None.
 * Return:  TRUE: Success. FALSE: failed.
 * Note:    None.
 * History: Create  Reginald Ian    2004.11.10
 */
BOOL_T CLI_MGR_SetRovingAnalysisModulePortUnit( UI32_T unit_id );

/* Routine: CLI_MGR_GetRovingAnalysisModulePortAdded
 * Purpose: Set flag on if add a module port as mirror destination
 * Input:   None.
 * Output:  None.
 * Return:  TRUE: Added. FALSE: Had not Added.
 * Note:    None.
 * History: Create  Reginald Ian    2004.11.10
 */
BOOL_T CLI_MGR_GetRovingAnalysisModulePortAdded( void );

/* Routine: CLI_MGR_SetRovingAnalysisModulePortAdded
 * Purpose: Set flag on if add a module port as mirror destination
 * Input:   truefalse: true or false value.
 * Output:  None.
 * Return:  TRUE: Success. FALSE: failed.
 * Note:    None.
 * History: Create  Reginald Ian
 */
BOOL_T CLI_MGR_SetRovingAnalysisModulePortAdded( BOOL_T truefalse );

/* Routine: CLI_MGR_ShowModuleConfigBuffer
 * Purpose: show the content of module buffer
 * Input:   None.
 * Output:  None.
 * Return:  TRUE: Success. FALSE: failed.
 * Note:    None.
 * History: Create  Reginald Ian    Create for print out debug message.
 */
BOOL_T CLI_MGR_ShowModuleConfigBuffer( void );

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Init
 *-----------------------------------------------------------
 * FUNCTION: To initialize the MGR module of CLI
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Init();

void CLI_MGR_Set_Realize_EnterTransitionMode(BOOL_T is_Realized);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Register_ProvisionComplete_CallBack
 *-----------------------------------------------------------
 * FUNCTION: To register which function to callback when CLI
 *           provision has been completed.
 * INPUT   : The pointer to the function that wish to callback
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Register_ProvisionComplete_CallBack (void (*fun)(void));

#if (SYS_CPNT_AUTOBAUDRATE == TRUE)
/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_AutoBaud_Register
 *-----------------------------------------------------------
 * FUNCTION: This function registers autobaud.
 *
 * INPUT   : The pointer to the function that wish to callback
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_AutoBaud_Register (BOOL_T (*fun)(void));

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_AutoBaud_UnRegister
 *-----------------------------------------------------------
 * FUNCTION: This function unregisters autobaud.
 *
 * INPUT   : The pointer to the function that wish to callback
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_AutoBaud_UnRegister (BOOL_T (*fun)(void));


/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_CheckAutobaud
 *-----------------------------------------------------------
 * FUNCTION: Call the function in the the callback list that registered
 *           by call CLI_MGR_AutoBaud_Register();
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
UI32_T CLI_MGR_CheckAutobaud(void);
#endif

#if (SYS_CPNT_3COM_CLI == TRUE)
/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_InitAndKeepIp
 *-----------------------------------------------------------
 * FUNCTION: Copy factory default config file to startup config file
 *           but keep ip address setting.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE.
 * NOTE    : None.
 *----------------------------------------------------------*/
BOOL_T CLI_MGR_InitAndKeepIp();


/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Get_Is_PrintComplete
 *-----------------------------------------------------------
 * FUNCTION: Get if print complete string.
 * INPUT   : None.
 * OUTPUT  : TRUE/FALSE
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Get_Is_PrintComplete(BOOL_T *status);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Set_Is_PrintComplete
 *-----------------------------------------------------------
 * FUNCTION: Set if print complete-string.
 * INPUT   : TRUE/FALSE
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Set_Is_PrintComplete(BOOL_T status);

#endif

#if (SYS_CPNT_3COM_PASSWORD_RECOVERY == TRUE)

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_SetPasswordRecovery
 *-----------------------------------------------------------
 * FUNCTION: Set or clear password recovery flag.
 * INPUT   : TRUE/FALSE
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE
 * NOTE    : None.
 *----------------------------------------------------------*/
UI32_T CLI_MGR_SetPasswordRecovery(BOOL_T status);


/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_IsDoingPasswordRecovery
 *-----------------------------------------------------------
 * FUNCTION: If is doing or not doing password recovery within 30 sec.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE
 * NOTE    : None.
 *----------------------------------------------------------*/
BOOL_T CLI_MGR_IsDoingPasswordRecovery();

#endif
/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Notify_ProvisionComplete
 *-----------------------------------------------------------
 * FUNCTION: Call the function in the the callback list that registered
 *           by call CLI_MGR_Register_ProvisionComplete_CallBack();
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Notify_ProvisionComplete(void);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Notify_ModuleProvisionComplete
 *-----------------------------------------------------------
 * FUNCTION: Call the function in the the callback list that registered
 *           by call CLI_MGR_Register_ModuleProvisionComplete_CallBack();
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Notify_ModuleProvisionComplete(UI32_T unit);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Get_ProvisionCMDCounter
 *-----------------------------------------------------------
 * FUNCTION: Copy factory default config file to startup config file
 *           but keep ip address setting.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE.
 * NOTE    : None.
 *----------------------------------------------------------*/
BOOL_T CLI_MGR_Get_ProvisionCMDCounter(UI32_T *error_command_count);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Add_ProvisionCMDCounter
 *-----------------------------------------------------------
 * FUNCTION: Copy factory default config file to startup config file
 *           but keep ip address setting.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE.
 * NOTE    : None.
 *----------------------------------------------------------*/
BOOL_T CLI_MGR_Add_ProvisionCMDCounter();

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Register_EnterTransitionMode_CallBack
 *-----------------------------------------------------------
 * FUNCTION: To register which function to callback when CLI
 *           want system to enter transition mode.
 * INPUT   : The pointer to the function that wish to callback.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Register_EnterTransitionMode_CallBack (void (*fun)(void));

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Notify_EnterTransitionMode
 *-----------------------------------------------------------
 * FUNCTION: Call the function in the the callback list that registered
 *           by call CLI_MGR_Register_EnterTransitionMode_CallBack();
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Notify_EnterTransitionMode(void);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_SetTransitionMode
 *-----------------------------------------------------------
 * FUNCTION: To force the operation mode of the module of CLI
 *           to enter master mode.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_SetTransitionMode(void);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Opmode_Enter_Master_Mode
 *-----------------------------------------------------------
 * FUNCTION: To force the operation mode of the module of CLI
 *           to enter master mode.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void   CLI_MGR_Opmode_Enter_Master_Mode();

/*-----------------------------------------------------------
 * ROUTINE NAME -  CLI_MGR_Opmode_Enter_Slave_Mode
 *-----------------------------------------------------------
 * FUNCTION: To force the operation mode of the module of CLI
 *           to enter slave mode.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void   CLI_MGR_Opmode_Enter_Slave_Mode();

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Opmode_Enter_Transition_Mode
 *-----------------------------------------------------------
 * FUNCTION: To force the operation mode of the module of CLI
 *           to enter transition mode.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void   CLI_MGR_Opmode_Enter_Transition_Mode();

/*-----------------------------------------------------------
 * ROUTINE NAME -  CLI_MGR_Opmode_Get_Operation_Mode
 *-----------------------------------------------------------
 * FUNCTION: Get to operation mode of CLI module.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : CLI_MGR_OPMODE_MASTER_MODE/CLI_MGR_OPMODE_TRANSITION_MODE/
 *           CLI_MGR_OPMODE_SLAVE_MODE
 * NOTE    : None.
 *----------------------------------------------------------*/
UI32_T CLI_MGR_Opmode_Get_Operation_Mode();

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Set_Provision_Flag
 *-----------------------------------------------------------
 * FUNCTION: Tell CLI_MGR if provision is in progress or not.
 * INPUT   : TRUE:  if provision is triggered.
             FALSE: if provision has been completed.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : Default when initialize is FALSE.
 *----------------------------------------------------------*/
void CLI_MGR_Set_Provision_Flag(BOOL_T status);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Get_Provision_Flag
 *-----------------------------------------------------------
 * FUNCTION: Get if provision is in progrerss.
 * INPUT   : None.
 * OUTPUT  : TRUE/FALSE
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Get_Provision_Flag(BOOL_T *status);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Wait_Provision_Finish
 *-----------------------------------------------------------
 * FUNCTION: A function that wait until provision has been completed
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : Whether provision succeeded and still in master mode.
 * NOTE    : None.
 *----------------------------------------------------------*/
BOOL_T CLI_MGR_Wait_Provision_Finish();



/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_WaitAllCscsKnowProvisionComplete
 *-----------------------------------------------------------
 * FUNCTION: A function that wait until all CSCs know provision complete.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_WaitAllCscsKnowProvisionComplete();


/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_AllCscsKnowProvisionComplete
 *-----------------------------------------------------------
 * FUNCTION: A function to make CLI know al CSCs already know provision complete.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : Call by STKCTRL.
 *----------------------------------------------------------*/
void CLI_MGR_AllCscsKnowProvisionComplete();


/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Set_Is_Go_To_Provision
 *-----------------------------------------------------------
 * FUNCTION: Set the flag that judge if CLI can perfoem provision or not
 * INPUT   : TRUE:  Let CLI can perform provision
 *           FALSE: Let CLI cannot perform provision
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void  CLI_MGR_Set_Is_Go_To_Provision(BOOL_T is_Go_To_Provision);

/*-----------------------------------------------------------
 * ROUTINE NAME -  CLI_MGR_Get_Is_Go_To_Provision
 *-----------------------------------------------------------
 * FUNCTION: Get the flag that judge if CLI can perfoem provision or not
 * INPUT   : None.
 * OUTPUT  : TRUE/FALSE
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void  CLI_MGR_Get_Is_Go_To_Provision(BOOL_T *is_Go_To_Provision);

void CLI_MGR_Set_PrintProvisionMsg(BOOL_T to_print);
void CLI_MGR_Get_PrintProvisionMsg(BOOL_T *to_print);

void CLI_MGR_Set_Provision_Pointer(UI8_T *Provision_Ptr);
void CLI_MGR_Get_Provision_Pointer(UI8_T **Provision_Ptr);


void CLI_MGR_Set_XferProgressStatus(BOOL_T Is_Xfer_In_Progress);
void CLI_MGR_Get_XferProgressStatus(BOOL_T *Is_Xfer_In_Progress);

void CLI_MGR_Get_XferCallBackStatus(UI32_T *callback_status);

void CLI_MGR_Set_XferProgressResult(BOOL_T result);
void CLI_MGR_Get_XferProgressResult(BOOL_T *result);

/*-----------------------------------------------------------
 * ROUTINE NAME -  CLI_MGR_Set_Config_File_Flag
 *-----------------------------------------------------------
 * FUNCTION: Set the flag, if CLI's config is valid or not
 * INPUT   : is_ok. TRUE/FALSE
 * OUTPUT  : none
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Set_Config_File_Flag(BOOL_T is_ok);
/*-----------------------------------------------------------
 * ROUTINE NAME -  CLI_MGR_Get_Config_File_Flag
 *-----------------------------------------------------------
 * FUNCTION: Get the flag that judge if CLI's config is valid or not
 * INPUT   : None.
 * OUTPUT  : TRUE/FALSE
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Get_Config_File_Flag(BOOL_T *is_ok);

void CLI_MGR_XferCopy_Callback(UI32_T cookie, UI32_T status);

/*-----------------------------------------------------------
 * ROUTINE NAME -  CLI_MGR_SaveRunningCfg
 *-----------------------------------------------------------
 * FUNCTION: This function should be called by WEB and provide
 *           the functionality of saving current system configuration
 *           to file system.
 * INPUT   : filename - The destination file name.
 *                      The filename length should >= 1, and
 *                      the filename length should <= SYS_ADPT_FILE_SYSTEM_NAME_LEN
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE
 * NOTE    : It only means    1) Allocating buffer from XFER successfully.
 *                            2) Buffer size is enough to collect all configuration.
 *           and doesn't mean 1) Saving configuration to FS successfully,
 *           if return value is TRUE.
 *           In the way, user in the WEB should check file system
 *           to realize if saving is successful or not.
 *----------------------------------------------------------*/
BOOL_T CLI_MGR_SaveRunningCfg(char *filename);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Get_RunningCfg
 *-----------------------------------------------------------
 * FUNCTION: This is a running config generator. Use the function
 *           call to collect the configuration of the system, and
 *           store in the style of CLI command.
 * INPUT   : buffer      - The pointer to the starting address to
 *                         store the configuration.
 *         : buffer_size - The size of the buffer to store the
 *                         configuration.
 * OUTPUT  : None.
 * RETURN  : CLI_MGR_RUNCFG_RETURN_OK - Collect configuration successfully.
 *           CLI_MGR_RUNCFG_RETURN_NO_ENOUGH_MEMORY - Buffer size is too
 *                                  to collect all configuration.
 * NOTE    : None.
 *----------------------------------------------------------*/
UI32_T CLI_MGR_Get_RunningCfg(void *buffer, UI32_T buffer_size);

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_SetKillWorkingSpaceFlag
 *------------------------------------------------------------------------
 * FUNCTION: Set kill_itself flag of working space according session id.
 * INPUT   : session_id
 * OUTPUT  : none
 * RETURN  : BOOL_T : TRUE  -- set ok.
 *                    FLASE -- failure.
 * NOTE    :
 *-----------------------------------------------------------------------*/
BOOL_T CLI_MGR_SetKillWorkingSpaceFlag(UI32_T sessid);
#if (SYS_CPNT_3COM_CLI == TRUE)
void CLI_MGR_XferCopy_Callback2(UI32_T cookie, UI32_T status);

#endif

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Register_ModuleProvisionComplete_CallBack
 *-----------------------------------------------------------
 * FUNCTION: To register which function to callback when CLI
 *           module provision has been completed.
 * INPUT   : The pointer to the function that wish to callback
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Register_ModuleProvisionComplete_CallBack (void (*fun)(void));

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_HandleHotInsertion
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
void CLI_MGR_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_HandleHotRemoval
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
void CLI_MGR_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port);

#if ( SYS_CPNT_UNIT_HOT_SWAP != TRUE )
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_AppendModuleCfg
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will append module config to DRAM buffer.
 * INPUT    : cmd_index     -- the command index defined in CLI_RUNCFG_MODULE_PORT_CMD_T
 *            cmd           -- the unit port string of each module
 *            pre_attr      -- the pre-attribute of command
 *            post_attr     -- the post-attribute of command
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_MGR_AppendModuleCfg(UI32_T cmd_index, CLI_STRLB_CMD_T *cmd, UI8_T *pre_attr, UI8_T *post_attr);
#endif

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_IsModulePort
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will check ifindex is a module port
 * INPUT    : ifindex     -- ifindex
 * OUTPUT   : None
 * RETURN  : BOOL_T : TRUE  -- module port. (unit must exist, too)
 *                    FLASE -- not module port.
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_IsModulePort(UI32_T ifindex);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_IncreaseRemoteSession
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will increase counter of remote seesion
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE  -- success.
 *                     FLASE -- fail.
 * NOTE     : Maximux of total session is 4 include telnet and ssh
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_IncreaseRemoteSession(void);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_IncreaseRemoteSession
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will decrease counter of remote seesion
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE  -- success.
 *                     FLASE -- fail.
 * NOTE     : Maximux of total session is 4 include telnet and ssh
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_DecreaseRemoteSession(void);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_CheckModuleVersion
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will check main board and module version
 *            if not syc display message
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE  -- success.
 *                     FLASE -- fail.
 * NOTE     :
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_CheckModuleVersion(void);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_CheckModuleVersionSession
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will check main board and module version
 *            if not syc display message
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE  -- success.
 *                     FLASE -- fail.
 * NOTE     :
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_CheckModuleVersionSession(void);

/* FUNCTION NAME: CLI_MGR_GetStackingDB
 * PURPOSE: This API is used to get the mac and unit id and device type
 * in CLI configuration file header
 * INPUT:   None.
 * OUTPUT:  stacking_db : array of structure.
 * RETUEN:  0  -- failure
 *          otherwise -- success. the returned value is the number of entries
 * NOTES:   None.
 */

//UI8_T CLI_MGR_GetStackingDB(STKTPLG_OM_StackingDB_T stacking_db[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK]);

/* FUNCTION NAME: CLI_MGR_GetIsReproduceDone
 * PURPOSE: This API is used to get the cli reproduce config file done
 * INPUT:   None.
 * OUTPUT:  None
 * RETUEN:  TRUE  -- success
 *          FALSE -- failure
 * NOTES:   None.
 */
BOOL_T CLI_MGR_GetIsReproduceDone(void);

/* FUNCTION NAME: CLI_MGR_SetIsReproduceDone
 * PURPOSE: This API is used to get the cli reproduce config file done
 * INPUT:   None.
 * OUTPUT:  None
 * RETUEN:  TRUE  -- success
 *          FALSE -- failure
 * NOTES:   None.
 */
BOOL_T CLI_MGR_SetIsReproduceDone(BOOL_T state);


/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_AllocateBuffer
 *------------------------------------------------------------------------
 * FUNCTION: This function will malloc cli_buf
 * INPUT   : None
 * OUTPUT  : cli_buf
 * RETURN  : TRUE(Success; return cli_buf); FALSE
 * NOTE    :
 *------------------------------------------------------------------------
 */
void *CLI_MGR_AllocateBuffer(void);



/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_FreeBuffer
 *------------------------------------------------------------------------
 * FUNCTION: This function will free bufffer
 * INPUT   : void *buf_P
 * OUTPUT  : None
 * RETURN  : TRUE(Success); FALSE
 * NOTE    :
 *------------------------------------------------------------------------
 */
BOOL_T  CLI_MGR_FreeBuffer(void *buf_P);


#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_AddDeviceCfg
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will append accton cli device config to buffer
 *
 * INPUT    : device_id, cmd_buffer
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     :
 * ------------------------------------------------------------------------
 */
void CLI_MGR_AddDeviceCfg(UI32_T device_id, UI8_T *cmd_buffer);
#endif
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_AddModuleCfg
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will append accton cli module config to buffer
 *
 * INPUT    : unit, cmd_buffer
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     :
 * ------------------------------------------------------------------------
 */
void CLI_MGR_AddModuleCfg(UI32_T unit, UI8_T *cmd_buffer);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_GetConfiguartionModuleTypes
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get module type in config file
 *
 * INPUT    : module_types
 * OUTPUT   : module_types
 * RETURN   : TRUE/FALSE
 * NOTE     :
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_GetConfiguartionModuleTypes(UI32_T  module_types[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK]);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_GetUintNeedProvision
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get module type in config file
 *
 * INPUT    : void
 * OUTPUT   : None
 * RETURN   : UI32_T the minmux unit is queue
 * NOTE     :
 * ------------------------------------------------------------------------
 */
UI32_T CLI_MGR_GetUintNeedProvision(void);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_SetUintIsNeedProvision
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get module type in config file
 *
 * INPUT    : 1)unit : the unit you want change in queue
 *          : 2)is_need : the status you want change in queue
 * OUTPUT   : void
 * RETURN   : void
 * NOTE     :
 * ------------------------------------------------------------------------
 */
void CLI_MGR_SetUintIsNeedProvision(UI32_T unit, BOOL_T is_need);

#if ( SYS_CPNT_UNIT_HOT_SWAP != TRUE )
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_GetModuleCfg
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get module type in config file
 *
 * INPUT    : unit
 * OUTPUT   : 1)module_buffer : pointer of unit's provision buffer
 *            2)module_buffer_len : lengh of unit's provision buffer
 * RETURN   : TRUE/FALSE
 * NOTE     : module_buffer is just a pointer assign not memcpy
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_GetModuleCfg(UI32_T unit, UI8_T *module_buffer, UI32_T *module_buffer_len);
#endif

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_SetSysBannerMsgStatus
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will set System Banner Message Status
 * INPUT    : delimiter
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE  -- success.
 *                     FLASE -- fail.
 * NOTE     :
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_SetSysBannerMsgStatus(char delimiter);


 /* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_DisableSysBannerMsg
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will remove System Banner Message
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE  -- success.
 *                     FLASE -- fail.
 * NOTE     :
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_DisableSysBannerMsg();


/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_Set_StkctrlSavingConfigStatus
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will be registered to STKCTRL task, STKCTRL
 *            will callback the status of saving config by this function.
 * INPUT    : status - the status of saving config
 *            TRUE: complete, FALSE: not complete
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_MGR_Set_StkctrlSavingConfigStatus(BOOL_T status);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_Get_StkctrlSavingConfigStatus
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get the status of saving config from
 *            CLI's OM.
 * INPUT    : None
 * OUTPUT   : status - the status of saving config
 *            TRUE: complete, FALSE: not complete
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_MGR_Get_StkctrlSavingConfigStatus(BOOL_T *status);

#if(SYS_CPNT_CLUSTER==TRUE)
void CLI_MGR_SetTelnetRelayingFlag(UI32_T tnpdTaskId, BOOL_T isRelaying);
#endif

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_DelPrivLvlCmdEntry
 *---------------------------------------------------------------------------
 * PURPOSE  : del a plce in the list
 * INPUT    : cmd_mode, cmd_str_len, cmd_str
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
BOOL_T
CLI_MGR_DelPrivLvlCmdEntry(
    const char *mode_str,
    BOOL_T is_all,
    const char *cmd_str
);

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_AddPrivLvlCmdEntry
 *---------------------------------------------------------------------------
 * PURPOSE  : add a plce to the list
 * INPUT    : cmd_mode, pri_lvl, cmd_str_len, cmd_str
 * OUTPUT   : is_replaced
 * RETURN   : TRUE/FALSE
 * NOTE     : replace the old one if the entry exists already
 *            if API return FALSE, the value of is_replaced = false
 *---------------------------------------------------------------------------
 */
BOOL_T
CLI_MGR_AddPrivLvlCmdEntry(
    const char * mode_str,
    BOOL_T is_all,
    UI8_T pri_lvl,
    const char *cmd_str
);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_GetNextRunningPrivilegeCmd
 * -------------------------------------------------------------------------
 * FUNCTION: get privilege cmd of running config
 * INPUT   : idx
 * OUTPUT  : idx, plce_p
 * RETURN  : SYS_TYPE_Get_Running_Cfg_T
 * NOTE    : *idx = 0 implies get first entry
 * -------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
CLI_MGR_GetNextRunningPrivilegeCmd(
    UI32_T *idx,
    CLI_PrivLvlCmdEntry_T *plce_p
);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_GetNextPrivilegeCmd
 * -------------------------------------------------------------------------
 * FUNCTION: get next privilege cmd
 * INPUT   : idx
 * OUTPUT  : idx, plce_p
 * RETURN  : TRUE/FALSE
 * NOTE    : *idx = 0 implies get first entry
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_MGR_GetNextPrivilegeCmd(
    UI32_T *idx,
    CLI_PrivLvlCmdEntry_T *plce_p
);

#endif    /* #if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE) */

#if (SYS_CPNT_CFM == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_Get_StkctrlSavingConfigStatus
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get the status of saving config from
 *            CLI's OM.
 * INPUT    : UI8_T *input_p - input VLAN list
 * OUTPUT   : primary_vid - the first VLAN index in input list
 *            vid_num - VLAN number
 *            vid_list - the array store the vids
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_MGR_ParseCfmVlanList(UI8_T *input_p, UI32_T *primary_vid, UI32_T *vid_num, UI8_T *vid_list);
#endif

BOOL_T  CLI_MGR_ReadLine_From_XferBuf(char *cmd_buf, char **tmp_xbuf);

BOOL_T CLI_MGR_Get_RuntimeProvisionFlag(void);
I32_T CLI_MGR_Get_BuffermgmtOffset(void);
void CLI_MGR_Set_RuntimeProvisionFlag(BOOL_T val);
void CLI_MGR_Set_BuffermgmtOffset(I32_T offset);

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_DhcpRxOptionConfig_Callback
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will be registered to DHCP mgr, DHCP will
 *            callback the information of getting remote config file by this function.
 * INPUT    : *option66_length_p - tftp server length
              *option66_data_p - tftp server
              *option67_length_p - bootfile name length
              *option67_data_p - bootfile name
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_MGR_DhcpRxOptionConfig_Callback
    (UI32_T option66_length_p, UI8_T *option66_data_p, UI32_T option67_length_p, char *option67_data_p);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_Get_DhcpRxOptionConfig
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get the information of getting remote
              config file.
 * INPUT    : None
 * OUTPUT   : dhcp_status_p - the status of remote config
 *            tftp_address_p - tftp server address
              file_name_p - config file name
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_MGR_Get_DhcpRxOptionConfig
        (BOOL_T *dhcp_status_p, char *tftp_address_p, char *file_name_p);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_Set_Startup_FactoryDefault_Flag
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to set startup factory default.
 * INPUT    : is_complete
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_MGR_Set_Startup_FactoryDefault_Flag(BOOL_T is_default);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_Get_Startup_FactoryDefault_Flag
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to set startup factory default.
 * INPUT    : is_complete
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_MGR_Get_Startup_FactoryDefault_Flag(BOOL_T *is_default);
#endif  /* #if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE) */

#if (SYS_CPNT_CLI_BANNER == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_DisplayBanner
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to display banner information before user login
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : called by cli_main.c
 * ------------------------------------------------------------------------
 */
void CLI_MGR_DisplayBanner();
#endif  /*#if (SYS_CPNT_CLI_BANNER == TRUE)*/

#if (SYS_CPNT_TELNET == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_CloseAllTelnetSessions
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will close all telnet sessions .
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_CloseAllTelnetSessions();
#endif /* #if (SYS_CPNT_TELNET == TRUE) */

#if (SYS_CPNT_MGMT_IP_FLT == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_HandleChangedIpMgmtFilter
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will handle changed IP mgmt filter
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_MGR_HandleChangedIpMgmtFilter();
#endif /* #if (SYS_CPNT_MGMT_IP_FLT == TRUE) */

#endif /* #ifndef CLI_MGR_H */

