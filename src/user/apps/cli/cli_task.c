/* Project Name: Mercury
 * Module Name : CLI_TASK.C
 * Abstract    : to be included in root.c and tn_main.c to access cli
 * Purpose     : CLI initiation and CLI task creation
 *
 * History :
 *          Date        Modifier        Reason
 *          2001/11/6   Charles Cheng   Modify the mechanism to init the session
 *          2007/06/01  Rich Lee        porting to Linux Platform
 * Copyright(C)      Accton Corporation, 2001
 */


/*------------------------------------------------------------------------
 * INCLUDE STRUCTURES
 *------------------------------------------------------------------------
 */
#include <assert.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "sys_type.h"
#include "sys_cpnt.h"
#include "sysfun.h"
#include "sys_dflt.h"
#include "sysfun.h"
#include "sys_module.h"
#include "sys_bld.h"
#include "l_mm.h"
#include "sys_callback_mgr.h"
#include "sys_dfltcfg.h"
#include "telnetd.h"
#include "sshd_mgr.h"
#include "fs_type.h"
#include "stktplg_pom.h"
#include "fs.h"
#include "uc_mgr.h"
#include "buffer_mgr.h"

#include "cli_api.h"
#include "cli_def.h"
#include "cli_task.h"
#include "cli_main.h"
#include "cli_mgr.h"
#include "cli_lib.h"
#include "cli_io.h"
#include "temp.h"
#include "mgmt_ip_flt.h"

#if (SYS_CPNT_CLI_BANNER == TRUE)
#include "cli_banner.h"
#endif

#if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE)
#include "sw_watchdog_mgr.h"
#endif

static UI32_T    cli_ipc_msgq_handle;

/*------------------------------------------------------------------------
 *                      LOCAL VARIABLES
 *-----------------------------------------------------------------------*/

/* Session Control Data used for sessions (Declare working buffer for each thread)
 */
/*
05/05/2004 11:25上午 modify pttch add for hot-swap module provision porting from hagrid
                    CLI_TASK_MAX_SESSION_NUM+1 for accton backdoor cli in 3com use
                    CLI_TASK_MAX_SESSION_NUM+2 for module provision use
*/
// static CLI_TASK_WorkingArea_T cli_working_buffer[CLI_TASK_MAX_SESSION_NUM + 2];
static CLI_TASK_WorkingArea_T cli_working_buffer[CLI_TASK_MAX_SESSION_NUM];

//static UI32_T                 cli_task_smid;       /* semaphore id         */
static BOOL_T is_transition_done;
/*------------------------------------------------------------------------
 * LOCAL FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
static BOOL_T CLI_TASK_Check_Factorydefault_The_Same(char *xfer_buf ,char *sys_dfltcfg);
static void   CLI_TASK_Main(CLI_TASK_WorkingArea_T *working_area);
static void  *CLI_TASK_AllocatWorkingSpace(UI32_T sess_type, UI32_T *sess_id);
static void   CLI_TASK_FreeWorkingSpace(CLI_TASK_WorkingArea_T *WorkingSpace);

extern char *sys_dfltcfg;
static BOOL_T CLI_TASK_SetStackingDB(void);
static BOOL_T CLI_TASK_ReadLine_From_XferBuf(char *cmd_buf, char **tmp_xbuf);
static UI32_T cli_task_semid;

UI32_T  CLI_MGR_ReadSystemConfig(char *xbuffer, UI32_T *xbuf_length)
{

    char  bootcfg_filename[SYS_ADPT_FILE_SYSTEM_NAME_LEN + 1];
    UI32_T retval;
    UI32_T unit_id;


    STKTPLG_POM_GetMyUnitID(&unit_id);
    SYSFUN_Debug_Printf("\n unit id is %lu\n",unit_id);

    retval = FS_GetStartupFilename(unit_id, FS_FILE_TYPE_CONFIG, (UI8_T *)bootcfg_filename);

    if( retval != FS_RETURN_OK )
    {
        SYSFUN_Debug_Printf("%s %d\n", __FUNCTION__,__LINE__);
        strcpy(xbuffer, (char *)sys_dfltcfg);
        *xbuf_length = strlen(xbuffer);

        SYSFUN_Debug_Printf("xbuf_length is %lu\n", *xbuf_length);
        return retval;
    }

    SYSFUN_Debug_Printf("%s %d\n", __FUNCTION__,__LINE__);
    return FS_ReadFile( unit_id, (UI8_T *)bootcfg_filename,  (UI8_T *)xbuffer, SYS_ADPT_MAX_FILE_SIZE, xbuf_length);



}/* End of XFER_MGR_ReadSystemConfig() */

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_Del
 *-----------------------------------------------------------
 * FUNCTION:Delete task
 * INPUT   :CLI_TASK_WorkingArea_T *WorkingSpace
 * OUTPUT  :None.
 * RETURN  :None.
 * NOTE    :None.
 *----------------------------------------------------------*/
void CLI_TASK_Del(CLI_TASK_WorkingArea_T *working_area)
{
    I32_T   socket;
    UI8_T   sess_type;

    assert(working_area != NULL);
    socket = working_area->socket;
    sess_type = working_area->sess_type;

    working_area->is_empty = TRUE;
    SYSFUN_DelSelfThread();
}


BOOL_T CLI_TASK_GET_IPC_Handle( UI32_T * handler_p)
{

    *handler_p = cli_ipc_msgq_handle;
//    printf("\n task handler is %lu\n", cli_ipc_msgq_handle);
//    printf("\n *handler_p is %lu\n", *handler_p);
    return TRUE;
}
/* EXPORTED SUBPROGRAM BODIES
 */
/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_Init
 *-----------------------------------------------------------
 * FUNCTION:Initiate CLI sessions data
 * INPUT   :None.
 * OUTPUT  :None.
 * RETURN  :None.
 * NOTE    :Must be called before any cli session task
 *----------------------------------------------------------*/
void CLI_TASK_Init(void)
{
	UI8_T i;

	memset(cli_working_buffer, 0, sizeof(cli_working_buffer));

/*
05/05/2004 11:25上午 modify pttch add for hot-swap module provision porting from hagrid
*/
    // for (i = 0; i <= CLI_TASK_MAX_SESSION_NUM+1; i++)
    for (i = 0; i < CLI_TASK_MAX_SESSION_NUM; i++)
    {
        cli_working_buffer[i].is_empty = TRUE;
        cli_working_buffer[i].print_interactive = TRUE;

#if (SYS_CPNT_CLI_TERMINAL == TRUE) || (SYS_CPNT_CLI_FILTERING == TRUE)
        CLI_LIB_SetFilteringTerminalDefault(&cli_working_buffer[i]);
#endif  /* #if (SYS_CPNT_CLI_TERMINAL == TRUE) || (SYS_CPNT_CLI_FILTERING == TRUE) */

    }
    //if (SYSFUN_CreateSem (1, SYSFUN_SEM_FIFO, &cli_task_smid) != SYSFUN_OK)
    //{
    //    perror("\r\n CLI_TASK: Failed to create semaphore. \r\n Lock the Thread.\r\n");
    //    while(1);
    //}

#if (SYS_CPNT_CLI_BANNER == TRUE)
    CLI_BANNER_Initialize();
#endif

    CLI_MGR_Set_Is_Go_To_Provision(TRUE);
    is_transition_done = FALSE;
    if(SYSFUN_CreateSem(SYSFUN_SEMKEY_PRIVATE, 1, SYSFUN_SEM_FIFO, &cli_task_semid) !=SYSFUN_OK)
    {
        SYSFUN_Debug_Printf("%s:get cli task sem id fail.\n", __FUNCTION__);
    }

}

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_SetTransitionMode();
 *-----------------------------------------------------------
 * FUNCTION:Set task to transition mode
 * INPUT   :None.
 * OUTPUT  :None.
 * RETURN  :None.
 * NOTE    :Must be called before any cli session task
 *----------------------------------------------------------*/
void CLI_TASK_SetTransitionMode()
{
    is_transition_done = FALSE;
}

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_EnterTransitionMode();
 *-----------------------------------------------------------
 * FUNCTION:Force task to transition mode
 * INPUT   :None.
 * OUTPUT  :None.
 * RETURN  :None.
 * NOTE    :Must be called before any cli session task
 *----------------------------------------------------------*/
void CLI_TASK_EnterTransitionMode()
{
 	SYSFUN_TASK_ENTER_TRANSITION_MODE(is_transition_done);
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_AllocatTelnetWorkingSpace
 *------------------------------------------------------------------------
 * PURPOSE :
 * INPUT   :
 * OUTPUT  :
 * RETURN  :
 * NOTE    :
 *------------------------------------------------------------------------*/
void *CLI_TASK_AllocatTelnetWorkingSpace(void)
{
	UI32_T sess_id;
	return CLI_TASK_AllocatWorkingSpace(CLI_TYPE_TELNET, &sess_id);
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_FreeTelnetWorkingSpace
 *------------------------------------------------------------------------
 * PURPOSE :
 * INPUT   :
 * OUTPUT  :
 * RETURN  :
 * NOTE    :
 *------------------------------------------------------------------------*/
void CLI_TASK_FreeTelnetWorkingSpace(void *WorkingSpace)
{
	CLI_TASK_FreeWorkingSpace(WorkingSpace);
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_SetTelnetSessionContext
 *------------------------------------------------------------------------
 * PURPOSE :
 * INPUT   :
 * OUTPUT  :
 * RETURN  :
 * NOTE    :
 *------------------------------------------------------------------------*/
BOOL_T  CLI_TASK_SetTelnetSessionContext(void *WorkingSpace, UI32_T tid, I32_T socket_id)
{
	return CLI_TASK_SetSessionContext((CLI_TASK_WorkingArea_T *)WorkingSpace, tid, socket_id, CLI_TYPE_TELNET, 0,0, 0);
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_SessionMain
 *------------------------------------------------------------------------
 * PURPOSE :
 * INPUT   :
 * OUTPUT  :
 * RETURN  :
 * NOTE    :
 *------------------------------------------------------------------------*/
void CLI_TASK_TelnetSessionMain(void *ctrl_P)
{
    CLI_MAIN_Enter_Main_Routine((CLI_TASK_WorkingArea_T *)ctrl_P);
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_CreateTask
 *------------------------------------------------------------------------
 * PURPOSE: This function creates the CLI session task.
 * INPUT   : 1. session_type
 *           2. socket_id
 *           3. unit  0 means all unit
                 1-8 means unit-id
 *           4. *buffer
 *           5. buffer_size
 * OUTPUT  : None
 * RETURN  : BOOL_T
 * NOTE    : 1.This function shall not be invoked before CLI_INIT_Init() is executed.
 *           2.socket_id is valid only if CLI_TYPE_SessionType_T != CLI_TYPE_UART
 *           3.*buffer and buffer_size is used for provision cli task only
 *
 *------------------------------------------------------------------------*/
BOOL_T CLI_TASK_CreateTask(UI32_T cli_session_type, I32_T  socket_id, UI32_T unit,char *prov_buffer, UI32_T prov_buffer_len)
{
    CLI_TASK_WorkingArea_T *WorkingSpace;
    char    taskname[10];
    //UI8_T   buff[50] = {0};
    UI32_T  tid = 0, sess_id = 0;
    UI32_T orig_priority;


    /* determine session id
     */
    if((WorkingSpace = CLI_TASK_AllocatWorkingSpace(cli_session_type, &sess_id)) == NULL)
    {
    	if(cli_session_type == CLI_TYPE_TELNET)
    	{
    	   SYSFUN_Debug_Printf("no more telnet cli task could be created\r\n");
    	}
    	return FALSE;
    }

    /* Specify task name
     */
    sprintf(taskname, "CLITASK%lu", sess_id);
    //sprintf(buff,"CLI_TASK: taskname: %s\r\n", taskname);
    //CLI_LIB_PrintStr(buff);

    /* lock task to prevent context switching
     */
    orig_priority=CLI_TASK_EnterCriticalSection(cli_task_semid);

    /* suger, 08-11-2004, ES4549-08-00017
     * enlarge the stack size from SYS_BLD_TASK_LARGE_STACK_SIZE + SYS_BLD_TASK_COMMON_STACK_SIZE
     * to SYS_BLD_TASK_LARGE_STACK_SIZE + SYS_BLD_TASK_LARGE_STACK_SIZE;
     * renamed to SYS_BLD_CLI_TASK_STACK_SIZE, Zhong Qiyao, 2004.12.09
     */

    CLI_TASK_SetSessionContext(WorkingSpace, tid, socket_id, cli_session_type, unit,prov_buffer, prov_buffer_len);

    /* Spawn a CLI Session Task
     */
    if(SYSFUN_SpawnThread(SYS_BLD_CLI_THREAD_PRIORITY,
                          SYS_BLD_CLI_THREAD_SCHED_POLICY,
                          /*(char *)SYS_BLD_CLI_THREAD_NAME,*/
                            taskname,
#ifdef ES3526MA_POE_7LF_LN /* Yongxin.Zhao added, 2009-06-19, 17:15:00 */
                          (SYS_BLD_CLI_TASK_STACK_SIZE * 2),
#else
                          SYS_BLD_CLI_TASK_STACK_SIZE,
#endif /* ES3526MA_POE_7LF_LN */
                          SYSFUN_TASK_NO_FP,
                          CLI_TASK_Main,
                          WorkingSpace,
                          &tid)!=SYSFUN_OK)
    {
        CLI_TASK_FreeWorkingSpace(WorkingSpace);
        SYSFUN_Debug_Printf("%s:SYSFUN_SpawnThread fail.\n", __FUNCTION__);
        CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
        return FALSE;
    }

#if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE)
    /* only monitor cli main thread CLITASK0, ignore provision's cli thread*/
    if (sess_id == 0)
    {
        SW_WATCHDOG_MGR_RegisterMonitorThread(SW_WATCHDOG_CLI, tid,SYS_ADPT_CLI_SW_WATCHDOG_TIMER);
    }
#endif

    /* unlock task to continue context switching
     */
    CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);

    SYSFUN_Debug_Printf("\r\n");
    return TRUE;
}

void CLI_TASK_DumpCliWorkingBuffer(void)
{
    int    i;
    UI32_T orig_priority;

    orig_priority = CLI_TASK_EnterCriticalSection(cli_task_semid);

    for (i = 1; i <= 4; i++)
    {
        SYSFUN_Debug_Printf("\r\n[%2d] is_empty = %s, cli_tid = %ld, socket = %ld",
               i,
               cli_working_buffer[i].is_empty?"true ":"false",
               cli_working_buffer[i].cli_tid,
               cli_working_buffer[i].socket);
    }
    SYSFUN_Debug_Printf("\r\n");
    CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);

    return;
}

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetMyWorkingArea
 *-----------------------------------------------------------
 * FUNCTION: Get working buffer
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : addr - Currnet address
 *----------------------------------------------------------*/
CLI_TASK_WorkingArea_T *CLI_TASK_GetMyWorkingArea(void)
{
    UI32_T tid = SYSFUN_TaskIdSelf();
    int    i;
    UI32_T orig_priority;

    orig_priority = CLI_TASK_EnterCriticalSection(cli_task_semid);

    for (i = 0; i < CLI_TASK_MAX_SESSION_NUM; i++)
    {
        if (cli_working_buffer[i].cli_tid == tid)
        {
            assert(!(cli_working_buffer[i].is_empty));
            CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
            return &(cli_working_buffer[i]);
        }
    }
    CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);

    SYSFUN_Debug_Printf("\r\n%s(): cannot find CLI working space, current task id = %ld", __FUNCTION__, tid);
    CLI_TASK_DumpCliWorkingBuffer();
    return NULL;
}

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetProvisionArea
 *-----------------------------------------------------------
 * FUNCTION: Get working buffer
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 *----------------------------------------------------------*/
CLI_TASK_WorkingArea_T *CLI_TASK_GetProvisionArea(void)
{
    return (&cli_working_buffer[CLI_TASK_PROVISION_SESSION_ID]);
}

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_TnSessExit
 *-----------------------------------------------------------
 * FUNCTION: clear resource used by current telnet session
 *           then kill backdoor and self task, this would
 *           cause a non-returning system call
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 *----------------------------------------------------------*/
void CLI_TASK_TnSessExit(CLI_TASK_WorkingArea_T *ctrl_P)
{
#if 0 /* rich linux */
    SYSFUN_Sleep(200);

    /* clear full-duplex connection
     */
    shutdown(ctrl_P->socket, SHUT_RDWR);
    s_close(ctrl_P->socket);

    SYSFUN_DelSelfThread();
#endif

#if 0
	/* Delete tn_backdooryyyy task
	 */
	{
	  char taskname[20];

      sprintf(taskname, "tn_backdoor%d", taskIdSelf());
      taskDelete(taskNameToId((char *)taskname));
	}
#endif

} /* end CLI_TASK_TnSessExit() */


/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetMySessId
 *------------------------------------------------------------------------
 * FUNCTION: Get session id of calling cli session
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : cli seesion serial number (session id)
 * NOTE    : assume existence of cli_working_buffer[] structure
 *-----------------------------------------------------------------------*/
UI8_T CLI_TASK_GetMySessId(void)
{
	UI32_T id;
	UI8_T i;

    id = SYSFUN_TaskIdSelf();

/*
05/05/2004 11:25 modify pttch add for hot-swap module provision porting from hagrid
                 +2 is one for accton cli backdoor, one for module provision
*/
    // for (i = 0; i < CLI_TASK_MAX_SESSION_NUM+2; i++)
    for (i = 0; i < CLI_TASK_MAX_SESSION_NUM; i++)
    {
        if (cli_working_buffer[i].is_empty == FALSE)
        {
            if (cli_working_buffer[i].cli_tid == id)
                return i;
            }
        }
    return CLI_TASK_NULL_SESS_ID;
}



/* LOCAL SUBPROGRAM BODIES
 */
/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_Main
 *------------------------------------------------------------------------
 * FUNCTION: cli starting routine (for UART session)
 * INPUT   : cli_session_type
 *           socket_number
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : 1.called by CLI_TASK_CreateTask()
 *
 *           3.argument task id, sess_type, and socket are needed to
 *             by backdoor task
 *------------------------------------------------------------------------
 */
static void CLI_TASK_Main(CLI_TASK_WorkingArea_T *working_area)
{

   /* 1. Each CLI UART/Telnet session will need a child task (Backdoor Debugging Task)
    *    to support backdoor debugging function.
    * 2. The CLI session task will suspend itself after his child task is created.
    * 3. The Backdoor Debugging Task will resume his parent task
    *    (the assocaited CLI session Task) after task initiation is complete.
    *    Then, the Backdoor Debugging Task will suspend itself.
    */

   /* Each CLI UART/Telnet session will need a child task (Backdoor Debugging Task)
	 * to support backdoor debugging function.
	 */

    //CLI_MGR_Opmode_Enter_Master_Mode();


    /* Note: Each time when CLI subsystem enters the Master Operation mode,
    *       The CLI UART session will perform system provision based on the specified
    *       "System Boot Config File" to initiate the system database and switch.
    */
    assert(working_area != NULL);
    working_area->cli_tid = SYSFUN_TaskIdSelf();

    /* wakka, move to cli_group.c
     */
#if 0
    /* create mgr ipc message queue handle
     */
    if(working_area->sess_type == CLI_TYPE_UART)
    {    if(SYSFUN_CreateMsgQ(SYS_BLD_CLI_GROUP_IPCMSGQ_KEY, SYSFUN_MSGQ_BIDIRECTIONAL, &cli_ipc_msgq_handle)!=SYSFUN_OK)
        {
            SYSFUN_Debug_Printf("%s: SYSFUN_CreateMsgQ fail.\n", __FUNCTION__);
            return;
        }

        //printf("\n %s %d,ipc_msgq_handle is %lu\n" , __FUNCTION__ , __LINE__, cli_ipc_msgq_handle);
    }
#endif
    while (1)
    {
        CLI_MAIN_HandleMGR_Thread(SYSFUN_TIMEOUT_NOWAIT, 0 , NULL);

        switch ( CLI_MGR_Opmode_Get_Operation_Mode() )
        {
            //printf("start provisiopn\n");
            case CLI_MGR_OPMODE_MASTER_MODE:
                switch(working_area->sess_type)
                {
                    case CLI_TYPE_UART:
                    {
                        char   *provision_p = NULL;
                        UI32_T xfer_buf_length=0;
                        BOOL_T is_Go_To_Provision;
                        BOOL_T is_config_file_ok;
                        BOOL_T is_need_save = FALSE;
                        BOOL_T is_startup   = FALSE;
                        BOOL_T is_factory_default_exist = FALSE;
                        BOOL_T is_other_startup = FALSE;
                        BOOL_T is_master_mode;


                        CLI_MGR_Set_Realize_EnterTransitionMode(FALSE);

                        CLI_MGR_Get_Is_Go_To_Provision(&is_Go_To_Provision);

                        while(is_Go_To_Provision == TRUE)
                        {
                            CLI_MGR_Set_Is_Go_To_Provision(FALSE);

                            /* this is normal provision mode -- means when cold/warm start
                             */
                            if(CLI_MGR_Get_RuntimeProvisionFlag() != TRUE)
                            {
                                if((provision_p = (char *)BUFFER_MGR_Allocate()) == NULL)
                                {
                                    CLI_LIB_PrintStr("\r\nCannot allocate memory from XFER; cannot perform startup provision\r\n");
                                }
                                else
                                {
                                    FS_File_Attr_T file_attr;
                                    UI32_T my_unit = 0;
#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
									char  src_filename_ar[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1]={0};
#endif

                                    memset(&file_attr, 0, sizeof(FS_File_Attr_T));

                                    STKTPLG_POM_GetMyUnitID(&my_unit);

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
									if (FS_GetStartupFilename( my_unit, FS_FILE_TYPE_CONFIG, (UI8_T *)src_filename_ar)!=FS_RETURN_OK)
									{
										CLI_LIB_PrintStr("Cannot get startup configuration file\r\n");
									}
									if (strcmp( src_filename_ar, SYS_DFLT_restartConfigFile)==0)
									{
										/* provision by factory default config file */
										CLI_MGR_Set_Startup_FactoryDefault_Flag(TRUE);
									}
#endif

                                    file_attr.file_type_mask = FS_FILE_TYPE_MASK(FS_FILE_TYPE_CONFIG);
                                    strcpy((char *)file_attr.file_name, SYS_DFLT_restartConfigFile);

                                    if (FS_GetFileInfo(my_unit, &file_attr) == FS_RETURN_OK)
                                    {

                                        is_factory_default_exist = TRUE;
                                        FS_ReadFile( my_unit, file_attr.file_name ,  (UI8_T *)provision_p, SYS_ADPT_MAX_FILE_SIZE, &xfer_buf_length);
                                        /*check factory config file context if unit not the same, regenerate the file*/

                                        if (CLI_TASK_Check_Factorydefault_The_Same(provision_p ,(char *)sys_dfltcfg) == FALSE)
                                        {

                                            if (file_attr.startup_file == TRUE)
                                            {

                                                FS_ResetStartupFilename(my_unit, FS_FILE_TYPE_CONFIG);
                                                is_startup = TRUE;
                                            }
                                            else
                                                is_other_startup = TRUE;

                                            FS_DeleteProtectedFile(my_unit, (UI8_T *)SYS_DFLT_restartConfigFile);
                                            is_need_save = TRUE;
                                        }/*end of CLI_TASK_Check_Factorydefault_changed*/

                                    }
                                    else
                                    {
                                        /* if not get Factory***, will not return ok */
                                        if (XFER_PMGR_ReadSystemConfig((UI8_T *)provision_p, &xfer_buf_length) == FS_RETURN_OK)
                                        {
                                            /*need check if other config is startup file or not*/
                                            is_other_startup = TRUE;
                                        }
                                    }

                                    if (is_need_save == TRUE || is_factory_default_exist == FALSE)
                                    {
                                        /*
                                        92.03.05 pttch add, because xfer spec change so need free and allocate buffer again
                                        */
                                        BUFFER_MGR_Free(provision_p);
                                        provision_p = NULL;
                                        /*save to the FLASH*/

                                        CLI_MGR_Set_XferProgressStatus(TRUE);


                                        if(!XFER_PMGR_StreamToLocal( (UI8_T *)SYS_DFLT_restartConfigFile, FS_FILE_TYPE_CONFIG, (UI32_T)working_area,SYS_BLD_CLI_GROUP_IPCMSGQ_KEY,CLI_MGR_XferCopy_Callback))
                                        {
                                           CLI_LIB_PrintStr("Cannot save Factory Default Configuration to FLASH\r\n");
                                        }
                                        else
                                        {
                                            while(1)
                                            {
                                                BOOL_T Is_Xfer_In_Progress;
                                                UI32_T  callback_status = 0;
                                                CLI_LIB_PrintStr(".");
                                                /* to polling xfer status */
                                                CLI_MAIN_HandleMGR_Thread(SYSFUN_TIMEOUT_NOWAIT, 0 , NULL);
                                                CLI_MGR_Get_XferCallBackStatus(&callback_status);
                                                CLI_MGR_Get_XferProgressStatus(&Is_Xfer_In_Progress);
                                                if(!Is_Xfer_In_Progress)
                                                {
                                                    CLI_LIB_PrintStr("\r\n");
                                                    break;
                                                }
                                                SYSFUN_Sleep(10);

                                            }

                                            {
                                                UI32_T my_unit_id;


                                                STKTPLG_POM_GetMyUnitID(&my_unit_id);
                                                //set the file as startup-config
                                                if (is_startup == TRUE || is_other_startup == FALSE)
                                                {
                                                    if(FS_SetStartupFilename(my_unit_id,FS_FILE_TYPE_CONFIG, (UI8_T *)SYS_DFLT_restartConfigFile) == FS_RETURN_OK)
                                                    {
                                                        CLI_LIB_PrintStr("Set Factory Default Configuration as startup-config\r\n");
                                                    }
                                                    else
                                                    {
                                                        CLI_LIB_PrintStr("Cannot set Factory Default Configuration as startup-config\r\n");

                                                        if( FS_DeleteFile(my_unit_id, (UI8_T *)SYS_DFLT_restartConfigFile) != FS_RETURN_OK)
                                                            CLI_LIB_PrintStr("Cannot delete Factory Default Configuration as startup-config\r\n");
                                                        else
                                                            CLI_LIB_PrintStr("Delete Factory Default Configuration\r\n");
                                                    }
                                                }
                                            }
                                        }/*stream to local else*/

                                    }
                                    /*
                                     *   91.12.31 pttch add, because xfer spec change so need free and allocate buffer again
                                     */
                                    if (provision_p == NULL)
                                    {

                                        if((provision_p = (char *)BUFFER_MGR_Allocate()) == NULL)
                                        {

                                            CLI_LIB_PrintStr("\r\nCannot allocate memory from XFER; cannot perform startup provision\r\n");
                                        }
                                    }


                                    /* here it reset provision_p, for it complete check Factory_Default */
                                    if(provision_p != NULL)
                                        memset(provision_p, 0, sizeof(SYS_ADPT_MAX_FILE_SIZE));


                                    if (XFER_PMGR_ReadSystemConfig((UI8_T *)provision_p, &xfer_buf_length) != FS_RETURN_OK)
                                    {

                                        CLI_LIB_PrintStr("\r\nCannot get startup configuration file; get Factory Default Configuration and save as startup-config\r\n");
                                        BUFFER_MGR_Free(provision_p);
                                        provision_p = NULL;
                                        CLI_MGR_Set_XferProgressStatus(TRUE);
                                        if(!XFER_PMGR_StreamToLocal( (UI8_T *)SYS_DFLT_restartConfigFile, FS_FILE_TYPE_CONFIG, (UI32_T)working_area, SYS_BLD_CLI_GROUP_IPCMSGQ_KEY,CLI_MGR_XferCopy_Callback))
                                        {

                                            CLI_LIB_PrintStr("Cannot save Factory Default Configuration to FLASH\r\n");
                                        }
                                        else
                                        {
                                            while(1)
                                            {
                                                BOOL_T Is_Xfer_In_Progress;
                                                UI32_T  callback_status = 0;
                                                CLI_LIB_PrintStr(".");
                                                CLI_MAIN_HandleMGR_Thread(SYSFUN_TIMEOUT_NOWAIT, 0 , NULL);
                                                CLI_MGR_Get_XferCallBackStatus(&callback_status);
                                                CLI_MGR_Get_XferProgressStatus(&Is_Xfer_In_Progress);
                                                if(!Is_Xfer_In_Progress)
                                                {
                                                    CLI_LIB_PrintStr("\r\n");
                                                    break;
                                                }
                                                SYSFUN_Sleep(10);

                                            }

                                            {
                                                UI32_T my_unit_id;

                                                STKTPLG_POM_GetMyUnitID(&my_unit_id);
                                                /*set the file as startup-config*/
                                                if(FS_SetStartupFilename(my_unit_id,FS_FILE_TYPE_CONFIG, (UI8_T *)SYS_DFLT_restartConfigFile) == FS_RETURN_OK)
                                                {
                                                    CLI_LIB_PrintStr("Set Factory Default Configuration as startup-config\r\n");
                                                }
                                                else
                                                {
                                                    CLI_LIB_PrintStr("Cannot set Factory Default Configuration as startup-config\r\n");

                                                    if( FS_DeleteFile(my_unit_id, (UI8_T *)SYS_DFLT_restartConfigFile) != FS_RETURN_OK)
                                                        CLI_LIB_PrintStr("Cannot delete Factory Default Configuration as startup-config\r\n");
                                                    else
                                                        CLI_LIB_PrintStr("Delete Factory Default Configuration\r\n");
                                                }
                                            }
                                        }
                                        if((provision_p = (char *)BUFFER_MGR_Allocate()) == NULL)
                                        {

                                            CLI_LIB_PrintStr("\r\nCannot allocate memory from XFER; cannot perform startup provision\r\n");
                                        }
                                        strcpy(provision_p, (char *)sys_dfltcfg);
                                        xfer_buf_length = strlen(provision_p);
                                    } /* end of if (XFER_PMGR_ReadSystemConfig(xfer_buf, &xfer_buf_length) == FS_RETURN_OK) */

                                } /* end of if((xfer_buf = (UI8_T *)XFER_BUF_PMGR_Allocate()) != NULL) */
                                CLI_TASK_CreateTask(CLI_TYPE_PROVISION, 0, 0,provision_p, xfer_buf_length);

                                /* wait for provision to finish
                                 * if not in master mode, then quit
                                 */

                                is_master_mode = CLI_MGR_Wait_Provision_Finish();


                                BUFFER_MGR_Free(provision_p);
                                provision_p = NULL;

                                /* If opmode is not master mode any more,
                                 * we should not continue master mode action.
                                 */
                                if ( is_master_mode != TRUE )
                                {
                                    /* Break out of:
                                     * while(is_Go_To_Provision == TRUE || is_config_file_ok == FALSE)
                                     *
                                     * After breaking, this will also break out of:
                                     * switch(working_area->sess_type)
                                     * case CLI_TYPE_UART:
                                     *
                                     * and then break out of:
                                     * switch ( CLI_MGR_Opmode_Get_Operation_Mode() )
                                     * case CLI_MGR_OPMODE_MASTER_MODE:
                                     */

                                    break;
                                }
                            } /* end of if(CLI_MGR_Get_RuntimeProvisionFlag() != TRUE) */
                            /* following is for runtime provision */
                            else
                            {

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
						        CLI_MGR_Set_Startup_FactoryDefault_Flag(FALSE);
#endif
                                provision_p =(char *) BUFFER_MGR_GetPtr(CLI_MGR_Get_BuffermgmtOffset());

                                /* Runtime provision */
                                if((provision_p != NULL) && ((provision_p+strlen(CLI_DEF_RUNTIME_PROVISION_MAGIC_WORD))[0] != 0))
                                {

                                    CLI_TASK_CreateTask( CLI_TYPE_PROVISION,
                                                          0,
                                                          0,
                                                          provision_p+strlen(CLI_DEF_RUNTIME_PROVISION_MAGIC_WORD),
                                                          strlen((provision_p+strlen(CLI_DEF_RUNTIME_PROVISION_MAGIC_WORD))) );

                                    CLI_MGR_Wait_Provision_Finish();

                                }
                                else
                                {
                                    CLI_LIB_PrintStr("\r\nInvalid configuration file; cannot perform runtime provision\r\n");
                                }
                                CLI_MGR_Set_RuntimeProvisionFlag(FALSE);
                                CLI_MGR_Set_BuffermgmtOffset(0);
                                BUFFER_MGR_Free(provision_p);
                                provision_p = NULL;

                            }

                            /* suger, 2006-01-17
                             * If the UART task quit CLI_MGR_Wait_Provision_Finish() because of TCN,
                             * should break righ away to avoid processing error data such as boot up
                             * with Factory_Default_Config instead of original config file.
                             */
                            if(CLI_MGR_Opmode_Get_Operation_Mode() != CLI_MGR_OPMODE_MASTER_MODE)
                            {

                                break;
                            }
                            CLI_MGR_Get_Is_Go_To_Provision(&is_Go_To_Provision);
                            CLI_MGR_Get_Config_File_Flag(&is_config_file_ok);
                            if (is_config_file_ok == FALSE)
                            {
                                UI32_T now_unit = 0;

                                CLI_LIB_PrintStr("Start-up config file contains error.\r\n");

                                /* for STKPLG is the upper layer, to use OM instead of PMGR
                                 */

                                STKTPLG_POM_GetMyUnitID(&now_unit);
                                if(FS_SetStartupFilename(now_unit,FS_FILE_TYPE_CONFIG, (UI8_T *)SYS_DFLT_restartConfigFile) == FS_RETURN_OK)
                                {
                                    CLI_LIB_PrintStr("Set Factory Default Configuration as startup-config\r\n");
                                }
                                else
                                {
                                    CLI_LIB_PrintStr("Cannot set Factory Default Configuration as startup-config\r\n");
                                }
                                SYSFUN_Sleep(10);
                            }
                            else
                            {

                                CLI_LIB_PrintStr("\r\nNotifying provision complete\r\n");
                                CLI_MGR_Notify_ProvisionComplete();
                                CLI_MGR_WaitAllCscsKnowProvisionComplete();

                                {
                                    char  file_name[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1];

                                    if(CLI_TASK_Reproduce_Config(file_name))
                                    {
#if (TRUE == SYS_CPNT_3COM_CLI)
                                        CLI_MGR_SetIsReproduceDone(TRUE);
#endif

                                        #if 0//rich
                                        if (FALSE == XFER_PMGR_AutoConfigToUnit(file_name,(UI32_T)working_area,SYS_BLD_CLI_GROUP_IPCMSGQ_KEY, NULL))
                                        {
                                            CLI_LIB_PrintStr("Failed to do auto sync\r\n");
                                        }
                                        #endif
                                    }
                                }
                            }
                            SYSFUN_Sleep(100);
                        } /* while(is_Go_To_Provision == TRUE) */

                    }
                    break;

                    case CLI_TYPE_PROVISION:
                    {
                        BOOL_T is_provision;
                        BOOL_T is_config_file_ok;

                        if(!CLI_TASK_SetStackingDB())
                        {

                            SYSFUN_Debug_Printf("\r\nFailed for CLI to set stackingDB\r\n");
                        }

                        CLI_MGR_Get_Provision_Flag(&is_provision);
                        CLI_MGR_Get_Config_File_Flag(&is_config_file_ok);

                        if(is_provision != TRUE)
                        {
                            /*free resource*/
                            if (is_config_file_ok == TRUE)
                            {
                                CLI_LIB_PrintStr("\r\nCLI provision has been completed\r\n");
                            }
                            /*delete task*/
                            CLI_TASK_Del(working_area);

                        }
#if 0
						else
						{
#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
                    		//DHCP_PMGR_RxOptionConfigStatus(TRUE);
#endif
						}
#endif
                    }
                    break;

/*
  05/05/2004 11:25上午 modify pttch add for hot-swap module provision porting from hagrid
                      Add module provision code
*/
                    case CLI_TYPE_MODULE_PROVISION:
                    {
                        BOOL_T is_provision;
                        BOOL_T is_config_file_ok;
                        char   file_name[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1];

                        CLI_MGR_Get_Provision_Flag(&is_provision);
                        CLI_MGR_Get_Config_File_Flag(&is_config_file_ok);

                        if(is_provision != TRUE)
                        {
                            //module provision complete.
                            SYSFUN_Debug_Printf("Module provision complete\r\n");
                            CLI_MGR_Notify_ModuleProvisionComplete(working_area->unit);
                            /*free resource*/
                            memset(working_area->prov_buffer, 0, working_area->prov_length);

                            if (is_config_file_ok == TRUE)
                            {
                                CLI_LIB_PrintStr("\r\nCLI provision has been completed\r\n");
                            }
                            /*save the stacking config*/
/*add by fen.wang,sometimes stktplg cannot assign right unit id,because the unit id from cli is not correct for when do  hot add ,cli will not save the topo
  when hotswap add ,save the topo info in flash*/

                           if(CLI_TASK_Reproduce_Config(file_name))
                           {
#if (TRUE == SYS_CPNT_3COM_CLI)
                               CLI_MGR_SetIsReproduceDone(TRUE);
#endif

                           }
                            /*delete task*/
                            CLI_TASK_Del(working_area);
                        }
                    }
                    break;


                    case CLI_TYPE_TELNET:
                    {
                        if(working_area->is_empty != TRUE)
                        {
                            /*free resource of working area*/
                            working_area->is_empty = TRUE;
                            /*delete task*/
                            CLI_TASK_TnSessExit(working_area);
                        }
                    }
                    break;
                } /* end of switch(working_area->sess_type) */

         /* Enter session service rutine
          */
                if (working_area->is_empty == FALSE)
                    CLI_MAIN_Enter_Main_Routine(working_area);

                break;

            case CLI_MGR_OPMODE_TRANSITION_MODE:
             /*
               91.12.30 pttch add for when plug in/out stack line will let CLI provision task
               can not delete. Add this check for delete provision task.
             */
                if (working_area->sess_type == CLI_TYPE_PROVISION)
                {
                    BOOL_T is_provision;

                    CLI_MGR_Get_Provision_Flag(&is_provision);

                    if(is_provision != TRUE)
                    {
                    /*free resource*/
                        CLI_LIB_PrintStr("\r\nCLI provision has been completed\r\n");


                    /*delete task*/
                        CLI_TASK_Del(working_area);
                    }
                }
                is_transition_done = TRUE;
                SYSFUN_Sleep(10);
                //CLI_MGR_Set_Realize_EnterTransitionMode(FALSE);
                is_transition_done = TRUE;
                SYSFUN_Sleep(10);
                //CLI_MGR_Set_Realize_EnterTransitionMode(FALSE);
                break;
            case CLI_MGR_OPMODE_SLAVE_MODE:
#if (SYS_CPNT_STACKING == TRUE)
              /* suger, 2006-01-17
              * PROVISION task should kill itself even in slave mode, otherwise it will
              * continuously process with wrong data in the while loop below.
              */
            if (working_area->sess_type == CLI_TYPE_PROVISION)
            {
                BOOL_T is_provision;

                CLI_MGR_Get_Provision_Flag(&is_provision);

                if(is_provision != TRUE)
                {
                   CLI_LIB_PrintStr("\r\nCLI provision has been completed\r\n");

                   /*delete task*/
                   CLI_TASK_Del(working_area);
                }
             }


            CLI_IO_OpenUart();
            while(1)
            {
                char cmd_buf[CLI_DEF_MAX_BUFSIZE+1];
                UI16_T    ret_type;

                /*jingyan zheng add to fix defect ES4827G-FLF-ZZ-00231*/
                cmd_buf[0] = 0;
                ret_type = CLI_PARS_ReadLine(cmd_buf, CLI_DEF_MAX_BUFSIZE, FALSE, FALSE);

                /*jingyan zheng mark this line to fix defect ES4827G-FLF-ZZ-00231*/
//                CLI_LIB_PrintStr("\r\n\r\n");

                if( CLI_MGR_Opmode_Get_Operation_Mode()!= CLI_MGR_OPMODE_SLAVE_MODE)
                    break;
                SYSFUN_Sleep(10);
            }
#else /* #if (SYS_CPNT_STACKING == TRUE) */
                SYSFUN_Sleep(10);
#endif
                break;

            default:
                SYSFUN_Sleep(10);
                break;
        } /*  switch ( CLI_MGR_Opmode_Get_Operation_Mode() ) */
    } /* while(1) */
}





/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_AllocatWorkingSpace
 *------------------------------------------------------------------------
 * PURPOSE : Allocat the resource of the working space, that is used for handle
 *           the CLI session
 * INPUT   : sess_type: 1. CLI_TYPE_UART
 *                      2. CLI_TASK_TELNET
 *                      3. CLI_TASK_PROVISION
 * OUTPUT  : NONE
 * RETURN  : The pointer of the allocated working space if it's possible, and NULL
 *           if resource is not enough, i.e. session number is full
 * NOTE    : The return pointer type is (void*), this is because this function
 *           should be exported to TELNET and it's not necessary for TELNET to
 *           to know the knowledge of the working space.
 *------------------------------------------------------------------------*/
#if 0
static void *CLI_TASK_AllocatWorkingSpace(UI32_T sess_type, UI32_T *sess_id)
{
    UI32_T  orig_priority;

    orig_priority = CLI_TASK_EnterCriticalSection(cli_task_semid);

    switch (sess_type) {
    case CLI_TYPE_UART:
        if (cli_working_buffer[CLI_TASK_UART_SESSION_ID].is_empty == TRUE)
        {
            cli_working_buffer[CLI_TASK_UART_SESSION_ID].is_empty = FALSE;
            *sess_id = CLI_TASK_UART_SESSION_ID;
            CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
            return &cli_working_buffer[CLI_TASK_UART_SESSION_ID];
        }
        else
        {
            CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
            return NULL;
        }
   case CLI_TASK_TELNET:
      {
         UI32_T i;


         for(i=CLI_TASK_MIN_TELNET_SESSION_ID; i<=CLI_TASK_MAX_TELNET_SESSION_ID; i++)
         {
         	if(cli_working_buffer[i].is_empty == TRUE)
         	{
         	   cli_working_buffer[i].is_empty = FALSE;
         	   *sess_id = i;

         	   CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
         	   return &cli_working_buffer[i];
         	}
         }
         CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
         return NULL;
      }
      break;

   case CLI_TASK_PROVISION:
      if(cli_working_buffer[CLI_TASK_PROVISION_SESSION_ID].is_empty == TRUE)
      {
      	 cli_working_buffer[CLI_TASK_PROVISION_SESSION_ID].is_empty = FALSE;
      	 *sess_id = CLI_TASK_PROVISION_SESSION_ID;
      	 CLI_MGR_Set_Provision_Flag(TRUE);

      	  CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
          return &cli_working_buffer[CLI_TASK_PROVISION_SESSION_ID];
      }
      else
      {
         CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
         return NULL;
      }
      break;

/*
  05/05/2004 11:25上午 modify pttch add for hot-swap module provision porting from hagrid
                      Add module provision code
*/
   case CLI_TYPE_MODULE_PROVISION:
      if(cli_working_buffer[CLI_TASK_MODULE_PROVISION_SESSION_ID].is_empty == TRUE)
      {
      	 cli_working_buffer[CLI_TASK_MODULE_PROVISION_SESSION_ID].is_empty = FALSE;
      	 *sess_id = CLI_TASK_MODULE_PROVISION_SESSION_ID;
      	 //CLI_MGR_Set_Provision_Flag(TRUE);

      	  CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
          return &cli_working_buffer[CLI_TASK_MODULE_PROVISION_SESSION_ID];
      }
      else
      {
         CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
         return NULL;
      }
      break;

   default:
      CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
      return NULL;
   }
    CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
}

#else

static void *CLI_TASK_AllocatWorkingSpace(UI32_T sess_type, UI32_T *sess_id)
{
    UI32_T  orig_priority = CLI_TASK_EnterCriticalSection(cli_task_semid);
    int     index;

    *sess_id = CLI_TASK_NULL_SESS_ID;

    switch (sess_type) {
    case CLI_TYPE_UART:
        if (cli_working_buffer[CLI_TASK_UART_SESSION_ID].is_empty) {
            *sess_id = CLI_TASK_UART_SESSION_ID;
#if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE)
            cli_working_buffer[*sess_id].monitor_id = SW_WATCHDOG_CLI;
#endif
        }
        break;
    case CLI_TASK_TELNET:
        for (index = CLI_TASK_MIN_TELNET_SESSION_ID;
             index <= CLI_TASK_MAX_TELNET_SESSION_ID;
             index++) {
            if (cli_working_buffer[index].is_empty) {
                *sess_id = index;
#if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE)
                cli_working_buffer[*sess_id].monitor_id =
                    SW_WATCHDOG_MAX_MONITOR_ID + SYS_ADPT_MAX_TELNET_NUM + index - 1;
#endif
                break;
            }
        }
        break;
    case CLI_TASK_PROVISION:
        if (cli_working_buffer[CLI_TASK_PROVISION_SESSION_ID].is_empty) {
            *sess_id = CLI_TASK_PROVISION_SESSION_ID;
            CLI_MGR_Set_Provision_Flag(TRUE);
        }
        break;
    case CLI_TYPE_MODULE_PROVISION:
        for (index = CLI_TASK_MIN_MODULE_PROVISION_SESSION_ID;
             index <= CLI_TASK_MAX_MODULE_PROVISION_SESSION_ID;
             index++) {
            if (cli_working_buffer[index].is_empty) {
                *sess_id = index;
                break;
            }
        }
        break;
    default:
        break;
    }

    if (*sess_id != CLI_TASK_NULL_SESS_ID) {
        /* must set before leave critical section */
        cli_working_buffer[*sess_id].is_empty = FALSE;
    }

    CLI_TASK_LeaveCriticalSection(cli_task_semid, orig_priority);
    return (*sess_id != CLI_TASK_NULL_SESS_ID) ? &cli_working_buffer[*sess_id]
                                               : NULL;
}

#endif

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_FreeWorkingSpace
 *------------------------------------------------------------------------
 * PURPOSE : Release the resource of the working space, that is used for handle
 *           the CLI session
 * INPUT   : WorkingSpace:
 * OUTPUT  : NONE
 * RETURN  : NONE
 * NOTE    : The input pointer type is (void*), this is because this function
 *           should be exported to TELNET and it's not necessary for TELNET to
 *           to know the knowledge of the working space.
 *           Of course, I know that I can just "(CLI_TASK_WorkingArea_T*)WorkingSpace->is_empty"
 *           to implement this, but I want to forbidden the accident and debug
 *------------------------------------------------------------------------*/
static void CLI_TASK_FreeWorkingSpace(CLI_TASK_WorkingArea_T *WorkingSpace)
{
    UI32_T orig_priority;

    assert(WorkingSpace != NULL);

    orig_priority=CLI_TASK_EnterCriticalSection(cli_task_semid);

    WorkingSpace->is_empty = TRUE;
    WorkingSpace->cli_tid  = 0;

#if (SYS_CPNT_CLI_TERMINAL == TRUE) || (SYS_CPNT_CLI_FILTERING == TRUE)
        CLI_LIB_SetFilteringTerminalDefault(WorkingSpace);
#endif  /* #if (SYS_CPNT_CLI_TERMINAL == TRUE)|| (SYS_CPNT_CLI_FILTERING == TRUE) */

    /*for sshv2, when free workspace need reset flag*/
    if(WorkingSpace->kill_itself == TRUE )
    {
        WorkingSpace->kill_itself = FALSE;
    }

    CLI_TASK_LeaveCriticalSection(cli_task_semid,orig_priority);
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_SetSessionContext
 *------------------------------------------------------------------------
 * PURPOSE :
 * INPUT   :
 * OUTPUT  :
 * RETURN  :
 * NOTE    :
 *------------------------------------------------------------------------*/
BOOL_T  CLI_TASK_SetSessionContext(CLI_TASK_WorkingArea_T *WorkingSpace,
                                   UI32_T tid,
                                   I32_T  socket_id,
                                   UI32_T sess_type,
                                   UI32_T unit,
                                   char  *prov_buffer,
                                   UI32_T prov_buffer_len)
{
    assert(WorkingSpace != NULL);
    assert(!(WorkingSpace->is_empty));

    WorkingSpace->cli_tid     = tid;
    WorkingSpace->socket      = socket_id;

    if(sess_type == CLI_TYPE_TELNET)
    {
        struct sockaddr_storage nam;
        size_t    nlen = sizeof(nam);

        UI32_T r_port;
        UI32_T l_port;

        UI32_T tnpd_tid;


        if (getpeername(socket_id, (struct sockaddr *) &nam, &nlen) == 0)
        {
            L_INET_AddrIp_T in;

            if (nam.ss_family == AF_INET)
            {
                struct sockaddr_in  *p_name = (struct sockaddr_in *) &nam;

                r_port = L_STDLIB_Ntoh16(p_name->sin_port);
            }
            else
            {
                struct sockaddr_in6 *p_name = (struct sockaddr_in6 *) &nam;

                r_port = L_STDLIB_Ntoh16(p_name->sin6_port);
            }

            TNPD_GetSessionPair(r_port, &in, &l_port,&tnpd_tid);

#if(SYS_CPNT_CLUSTER==TRUE)
            WorkingSpace->CMenu.TelnetTnpdTaskId = tnpd_tid;
#endif

            /*92.03.27 pttch add for disconnect session*/
            WorkingSpace->CMenu.RemoteSessionType = CLI_TYPE_TELNET;

#if ( SYS_CPNT_SSHD == TRUE || SYS_CPNT_SSH2 == TRUE)/*isiah*/
            if (tnpd_tid == 0)
            {
                /*92.03.27 pttch add for disconnect session*/
                WorkingSpace->CMenu.RemoteSessionType = CLI_TYPE_SSH;
                SSHD_MGR_GetSessionPair(r_port, &in, &l_port);
            }
#endif

            memcpy(&WorkingSpace->CMenu.TelnetIPAddress, &in, sizeof(WorkingSpace->CMenu.TelnetIPAddress));
        }
        else
        {
            CLI_LIB_PrintStr("cannot get peername\r\n");
            return FALSE;
        }
    }
    else
    {
        memset(&WorkingSpace->CMenu.TelnetIPAddress, 0, sizeof(WorkingSpace->CMenu.TelnetIPAddress));
        WorkingSpace->CMenu.RemoteSessionType = CLI_TYPE_UART;
    }

    WorkingSpace->sess_type   = sess_type;
    WorkingSpace->prov_buffer = prov_buffer;
    WorkingSpace->prov_length = prov_buffer_len;
    WorkingSpace->unit = unit;
    WorkingSpace->enter_debug_mode = FALSE;
    return TRUE;
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetOnLineUserInfoByIndex
 *------------------------------------------------------------------------
 * FUNCTION: Get next information of online user
 * INPUT   : index - session ID. For Uart or Telnet session only
 * OUTPUT  : user_infor - user information of the session
 * RETURN  : TRUE - successed; FALSE - failed
 * NOTE    :
 *-----------------------------------------------------------------------*/
BOOL_T CLI_TASK_GetOnLineUserInfoByIndex(UI32_T index, CLI_TASK_OnlineUserInfo_T *user_info)
{
   if (index > CLI_TASK_MAX_USER_SESSION_ID)
      return FALSE;

   if(cli_working_buffer[index].is_empty  ||
      cli_working_buffer[index].CMenu.AuthenState != CLI_DEF_AUTHEN_STATE_PASS)
   {
      return FALSE;
   }

    switch (cli_working_buffer[index].sess_type)
    {
        case CLI_TYPE_UART:
        strncpy(user_info->line, CLI_TASK_CONSOLE_STR, sizeof(user_info->line)-1);
        user_info->line[sizeof(user_info->line) -1] = '\0';
        user_info->location[0] = '\0';
        break;

        case CLI_TYPE_TELNET:
        if (cli_working_buffer[index].CMenu.RemoteSessionType == CLI_TYPE_TELNET)
        {
            sprintf(user_info->line, CLI_TASK_TELNET_FORMAT_STR, index-1);
        }
        else
        {
            sprintf(user_info->line, CLI_TASK_SSH_FORMAT_STR, index-1);
        }

        L_INET_InaddrToString((L_INET_Addr_T *)&cli_working_buffer[index].CMenu.TelnetIPAddress,
            sizeof(user_info->location)-1, (UI8_T *)user_info->location);

        break;

        default:
        return FALSE;
    }

    memset(user_info->username, 0, SYS_ADPT_MAX_USER_NAME_LEN+1);
    strcpy(user_info->username, cli_working_buffer[index].CMenu.UserName);

    user_info->idel_time = SYSFUN_GetSysTick()- cli_working_buffer[index].CMenu.IdleStartTick;

    return TRUE;
}

/*isiah*/
/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_SetKillWorkingSpaceFlag
 *------------------------------------------------------------------------
 * FUNCTION: Set kill_itself flag of working space according session id.
 * INPUT   : session_id
 * OUTPUT  : none
 * RETURN  : BOOL_T : TRUE  -- set ok.
 *                    FLASE -- failure.
 * NOTE    :
 *-----------------------------------------------------------------------*/
BOOL_T CLI_TASK_SetKillWorkingSpaceFlag(UI32_T sessid)
{
    #if 0 /* rich linux */
    if(sessid > (CLI_TASK_MAX_SESSION_ID - 1))
    {
        EH_PMGR_Handle_Exception1(SYS_MODULE_CLI, CLI_TASK_SETKILLWORKINGSPACEFLAG, EH_TYPE_MSG_INVALID_PARAMETER, SYSLOG_LEVEL_INFO, "session number");
        return FALSE;
    }

    if(cli_working_buffer[sessid].is_empty )
    {
        EH_PMGR_Handle_Exception1(SYS_MODULE_CLI, CLI_TASK_SETKILLWORKINGSPACEFLAG, EH_TYPE_MSG_INVALID_PARAMETER, SYSLOG_LEVEL_INFO, "session number");
        return FALSE;
    }
    #endif

    cli_working_buffer[sessid].kill_itself = TRUE;
    return TRUE;
}


/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetKillWorkingSpaceFlag
 *------------------------------------------------------------------------
 * FUNCTION: Get kill_itself flag of working space
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : BOOL_T : value of kill_itself
 * NOTE    :
 *-----------------------------------------------------------------------*/
BOOL_T CLI_TASK_GetKillWorkingSpaceFlag()
{
    UI8_T my_sess_id = CLI_TASK_GetMySessId();

    return cli_working_buffer[my_sess_id].kill_itself;
}


/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_Check_Factorydefault_The_Same
 *------------------------------------------------------------------------
 * FUNCTION: Check Factory default file is the same or not
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : TRUE(The same)/FALSE(Not the same)
 * NOTE    :
 *-----------------------------------------------------------------------*/
static BOOL_T CLI_TASK_Check_Factorydefault_The_Same(char *xfer_buf ,char *sys_dfltcfg)
{
   UI32_T i;
   char  *file_ptr;
   char  *def_ptr;

   i = 0;
   file_ptr = xfer_buf;
   def_ptr = sys_dfltcfg;
   while(*(sys_dfltcfg+i) != 0)
   {
      if (*(xfer_buf+i) != *(sys_dfltcfg+i))
      {SYSFUN_Debug_Printf("\n xfer : %c    sys_dflt : %c \n",*(xfer_buf+i),*(sys_dfltcfg+i));
      i++;
      SYSFUN_Debug_Printf("\n xfer : %c    sys_dflt : %c \n",*(xfer_buf+i),*(sys_dfltcfg+i));
      i++;
      SYSFUN_Debug_Printf("\n xfer : %c    sys_dflt : %c \n",*(xfer_buf+i),*(sys_dfltcfg+i));
      i++;
      SYSFUN_Debug_Printf("\n xfer : %c    sys_dflt : %c \n",*(xfer_buf+i),*(sys_dfltcfg+i));
      i++;
      SYSFUN_Debug_Printf("\n xfer : %c    sys_dflt : %c \n",*(xfer_buf+i),*(sys_dfltcfg+i));
      i++;
      SYSFUN_Debug_Printf("\n xfer : %c    sys_dflt : %c \n",*(xfer_buf+i),*(sys_dfltcfg+i));

           return FALSE;
           }
      i++;
   }

   return TRUE;
}

/* when only has Factory default , this will produce startip1.cfg for config file */
/*Reproduce config file for stackingDB may change*/
 BOOL_T CLI_TASK_Reproduce_Config(char *file_name)
{
    char   startup_filename[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1] = {0};
    char   *buf;
    char   *buf_temp;
    char   sub_str[60] = {0};
    STKTPLG_OM_StackingDB_T stacking_db_old[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];
    STKTPLG_OM_StackingDB_T stacking_db_new[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];
    UI8_T ret_db_count = 0;
    UI8_T rew_count = 0;
    UI8_T unmatch_count = 0;
    UI32_T use_unit = 0,i,j,k,m;
    UI32_T current_setp = 0;
    char   mac_addr[SYS_ADPT_MAC_ADDR_LEN] = {0};
    char  old_str_buffer[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][30];
    char  final_str_buffer[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][30];
	UI32_T buf_count;

    memset(stacking_db_old,0,sizeof(stacking_db_old));
    memset(stacking_db_new,0,sizeof(stacking_db_new));

	if(FS_RETURN_OK !=FS_GetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, (UI8_T *)startup_filename))
	{
        return FALSE;
	}

	ret_db_count = CLI_MGR_GetStackingDB(stacking_db_old);

	if((buf = (char *)CLI_MGR_AllocateBuffer()) == NULL)
	{
		SYSFUN_Debug_Printf("\r\n[CLI_TASK_Reproduce_Config not enough memery]\r\n");
        return FALSE;
	}


	if(strcmp(SYS_DFLT_restartConfigFile,startup_filename) == 0)
	{
		sprintf(startup_filename, SYS_DFLT_CLI_AUTO_GENERATE_START_UP_FILENAME);

		if(CLI_MGR_RUNCFG_RETURN_NO_ENOUGH_MEMORY == CLI_MGR_Get_RunningCfg(buf, CLI_DEF_MAX_CONFIG_SIZE) )
        {

            SYSFUN_Debug_Printf("\r\n[Failed for CLI to collect running]\r\n");
            CLI_MGR_FreeBuffer(buf);
            return FALSE;
        }
	}
	else
	{
		if(FS_ReadFile(DUMMY_DRIVE,(UI8_T *)startup_filename, (UI8_T *)buf, CLI_DEF_MAX_CONFIG_SIZE, &buf_count) != FS_RETURN_OK)
		{
			CLI_MGR_FreeBuffer(buf);
			return FALSE;
		}
	}


	STKTPLG_POM_GetCurrentStackingDB(stacking_db_new);

	if(ret_db_count == 0)
	{
	    if(FS_WriteFile(DUMMY_DRIVE,(UI8_T *)startup_filename, (UI8_T *)"CLI", FS_FILE_TYPE_CONFIG, (UI8_T *)buf, strlen(buf),0)!=FS_RETURN_OK)
        {

            SYSFUN_Debug_Printf("\r\n\r\n[Failed FS_WriteFile]\r\n\r\n\r\n");
            CLI_MGR_FreeBuffer(buf);
            return FALSE;
        }
	    if(FS_SetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, (UI8_T *)startup_filename)!=FS_RETURN_OK)
	    {

	        SYSFUN_Debug_Printf("\r\n\r\n[Failed FS_SetStartupFilename]\r\n\r\n\r\n");
	        CLI_MGR_FreeBuffer(buf);
	        return FALSE;
	    }
	}
	else
    {
                    /*put old stackingDB in a array*/

		for(i=0;i<SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;i++)
		{
		    char        mac_str[20] = {0};
		    UI32_T      device_type = 0;
		    UI32_T      unit_id = 0;
		    char        sub_str[40] = {0};
		    CLI_LIB_EAtoStr((char *)stacking_db_old[i].mac_addr, mac_str);
		    device_type = stacking_db_old[i].device_type;
		    unit_id = stacking_db_old[i].unit_id;
		    SYSFUN_Sprintf((char *)sub_str,"%02lu_%s_%02lu",unit_id,mac_str,device_type);
		    strcpy((char *)old_str_buffer[i], (char *)sub_str);
		}


        /*compare old and new*/
        for(i=0;i<SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;i++)
		{
		    char        mac_str_new[20] = {0};
		    UI32_T      device_type_new = 0;
		    UI32_T      unit_id_new = 0;
		    BOOL_T      is_find = FALSE;
		    BOOL_T      is_unit_use = FALSE;

		    CLI_LIB_EAtoStr((char *)stacking_db_new[i].mac_addr, mac_str_new);
		    device_type_new = stacking_db_new[i].device_type;
		    unit_id_new = stacking_db_new[i].unit_id;
		    if( unit_id_new == 0)
		    {
		        continue;
		    }

		    for(j=0;j<SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;j++)
		    {
		        char        mac_str_old[20] = {0};
		        UI32_T      device_type_old = 0;
		        UI32_T      unit_id_old = 0;

		        CLI_LIB_EAtoStr((char *)stacking_db_old[j].mac_addr, mac_str_old);
		        device_type_old = stacking_db_old[j].device_type;
		        unit_id_old = stacking_db_old[j].unit_id;
		        if( unit_id_old == 0)
		        {
		            continue;
		        }
		        if(strcmp(mac_str_new,mac_str_old) == 0)
		        {
		            /*same mac but unit change -> use new to replace old*/
		            if(unit_id_new != unit_id_old)
		            {
		                SYSFUN_Sprintf(sub_str,"%02lu_%s_%02lu",unit_id_new,mac_str_new,device_type_new);
		                strcpy(old_str_buffer[j], sub_str);
		                stacking_db_old[j].unit_id = unit_id_new;
		                CLI_LIB_AtoEa(mac_str_new, mac_addr);
		                memcpy(stacking_db_old[j].mac_addr,mac_addr,SYS_ADPT_MAC_ADDR_LEN);
		                stacking_db_old[j].device_type = device_type_new;
		                /*serch if new unit id is use and delete it */
		                for(k = 0; k <SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ; k++)
		                {
		                    if(stacking_db_old[k].unit_id == unit_id_new)
		                    {
		                        if(k == j)
		                            continue;
		                        strcpy(old_str_buffer[k],"00_00-00-00-00-00-00_00");
		                        stacking_db_old[k].unit_id = 0;
		                        memset(stacking_db_old[k].mac_addr,0,SYS_ADPT_MAC_ADDR_LEN);
		                        stacking_db_old[k].device_type = 0;
		                    }
		                }
		                unmatch_count++;
		            }
		            is_find = TRUE;
		        }

		        /*check if new unit id use in old header*/
		        if( unit_id_new == unit_id_old )
		        {
		            is_unit_use =TRUE;
		            use_unit = j;
		        }
		    }

		    if(is_find == FALSE )
		    {
		        rew_count++;
		        if( is_unit_use == FALSE)
		        {
		            /*serch empty block */
		            for(k = 0; k <SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ; k++)
		            {
		                if(stacking_db_old[k].unit_id == 0)
		                {
		                    SYSFUN_Sprintf(sub_str,"%02lu_%s_%02lu",unit_id_new,mac_str_new,device_type_new);
		                    strcpy(old_str_buffer[k], sub_str);
		                    stacking_db_old[k].unit_id = unit_id_new;
		                    CLI_LIB_AtoEa(mac_str_new, mac_addr);
		                    memcpy(stacking_db_old[k].mac_addr,mac_addr,SYS_ADPT_MAC_ADDR_LEN);
		                    stacking_db_old[k].device_type = device_type_new;
		                    /*serch if new unit id is use and delete it */
		                    for(m = 0; m <SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ; m++)
		                    {
		                        if(m == k)
		                            continue;
		                        if(stacking_db_old[m].unit_id == unit_id_new)
		                        {
		                            strcpy(old_str_buffer[m],"00_00-00-00-00-00-00_00");
		                            stacking_db_old[m].unit_id = 0;
		                            memset(stacking_db_old[m].mac_addr,0,SYS_ADPT_MAC_ADDR_LEN);
		                            stacking_db_old[m].device_type = 0;
		                        }
		                    }
		                    break;
		                }
		            }
		            //append
		        }
		        else
		        {
		            SYSFUN_Sprintf(sub_str,"%02lu_%s_%02lu",unit_id_new,mac_str_new,device_type_new);
		            strcpy(old_str_buffer[use_unit],sub_str);
		            stacking_db_old[use_unit].unit_id = unit_id_new;
		            CLI_LIB_AtoEa(mac_str_new, mac_addr);
		            memcpy(stacking_db_old[use_unit].mac_addr,mac_addr,SYS_ADPT_MAC_ADDR_LEN);
		            stacking_db_old[use_unit].device_type = device_type_new;
		            //override
		        }
		    }

		}

	    if(rew_count>0 || unmatch_count>0)
	    {
	        /* replace to old config buffer*/
	        char     *cmd_buf;
	        UI32_T len = 0;
	        char  type_str[30] = {0};
	        UI32_T start_pont = 0;
	        char  temp_str[60] = {0};
	        UI32_T read_count = 0;

	        /*sort*/
	        for(i = 0;i < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; i++)
	        {
	            strcpy(final_str_buffer[i],"00_00-00-00-00-00-00_00");
	        }
	        for(i = 1;i <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; i++)
	        {
	            for(j = 0;j < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; j++)
	            {
	                if(stacking_db_old[j].unit_id == i)
	                {
	                    strcpy(final_str_buffer[current_setp],old_str_buffer[j]);
	                    current_setp++;
	                }
	            }
	        }
	        for(i = 0;i<strlen(buf);i++)
	        {
	            if(memcmp(buf+i,"!<stackingMac>",14) == 0)
	            {
	                start_pont = i;
	                break;
	            }
	        }
	        if((cmd_buf = (char *)L_MM_Malloc(CLI_DEF_MAX_BUFSIZE, L_MM_USER_ID2(SYS_MODULE_CLI, CLI_TYPE_TRACE_ID_CLI_TASK_REPRODUCE_CONFIG))) ==NULL)
	        {
	            CLI_MGR_FreeBuffer(buf);
	            return FALSE;
	        }
	        memset(cmd_buf,0,sizeof(cmd_buf));
	        buf_temp = buf;
	        k = 0;
	        while ( read_count < CLI_DEF_MAX_HEADER )
	        {
	            if (CLI_TASK_ReadLine_From_XferBuf(cmd_buf, &buf_temp))
	            {
	                read_count ++;
	                len = strlen(cmd_buf);
	                i = 0;
	                j = 0;

	                for (i = 1; i < len; i++ )
	            	{
	            	    if (cmd_buf[i] == '>')
	            	    {
	            	        break;
	            	    }
	            	    if (cmd_buf[i] == '<')
	            	    {
	            	        continue;
	            	    }
	            	    else
	            	    {
	            	        /*prevent buffer excreed max*/
	            	        if (j>=30)
	            	        {
	            	            break;
	            	        }
	            	        type_str[j] = cmd_buf[i];
	            	        j++;
	            	    }
	            	}

	            	if (strcmp(type_str, "stackingMac") == 0)
	            	{
	                	sprintf(temp_str,"!<stackingMac>%s</stackingMac>\n",final_str_buffer[k]);
	        		    memcpy(buf + (start_pont+strlen(temp_str)*k),temp_str,strlen(temp_str));
	        		    k ++;
	        		}
	                /*
	                06/08/2004 11:00 pttch type_str need init.
	                */
	        		memset(type_str, 0, sizeof(type_str));
	            }
	            else
	            {
	                L_MM_Free(cmd_buf);
	                CLI_MGR_FreeBuffer(buf);
	                return FALSE;
	            }
	        }
	        if(FS_WriteFile(DUMMY_DRIVE,(UI8_T *)startup_filename, (UI8_T *)"CLI", FS_FILE_TYPE_CONFIG, (UI8_T *)buf, strlen((char *)buf),0)!=FS_RETURN_OK)
	        {
	            SYSFUN_Debug_Printf("\r\n\r\n[Failed FS_WriteFile]\r\n\r\n\r\n");
	            L_MM_Free(cmd_buf);
	           CLI_MGR_FreeBuffer(buf);
	            return FALSE;
	        }

	        if(FS_SetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, (UI8_T *)startup_filename)!=FS_RETURN_OK)
	        {
	            SYSFUN_Debug_Printf("\r\n\r\n[Failed FS_SetStartupFilename]\r\n\r\n\r\n");
	            L_MM_Free(cmd_buf);
	            CLI_MGR_FreeBuffer(buf);
	            return FALSE;
	        }
	        L_MM_Free(cmd_buf);
	    }

	}

	CLI_MGR_FreeBuffer(buf);
	strcpy(file_name , startup_filename);
	return TRUE;
#if 0
    if( FS_RETURN_OK ==FS_GetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, (UI8_T *)startup_filename))
    {

        if(strcmp(SYS_DFLT_restartConfigFile,startup_filename) == 0)
        {

#if (TRUE == SYS_CPNT_3COM_CLI)
            sprintf(startup_filename, SYS_DFLT_DBSYNC_TXT_DUAL_FILENAME, 1);
#else
            sprintf(startup_filename, SYS_DFLT_CLI_AUTO_GENERATE_START_UP_FILENAME);
#endif
        }

        ret_db_count = CLI_MGR_GetStackingDB(stacking_db_old);

        /*collect running-config*/
        {

            if((buf = (char *)CLI_MGR_AllocateBuffer()) != NULL)
            {

                if(CLI_MGR_RUNCFG_RETURN_NO_ENOUGH_MEMORY == CLI_MGR_Get_RunningCfg(buf, CLI_DEF_MAX_CONFIG_SIZE) )
                {

                    SYSFUN_Debug_Printf("\r\n[Failed for CLI to collect running]\r\n");
                    CLI_MGR_FreeBuffer(buf);
                    return FALSE;
                }

                /*compare old buffer and new buffer*/
                /*if new mac not in old buffer then it's new device*/
                /*if new device's unit id is in old buffer then replace old one*/
                /*if new device's unit id is not in old buffer then append*/
                /*if new count > 0 then write file*/

                STKTPLG_POM_GetCurrentStackingDB(stacking_db_new);

                if(ret_db_count == 0)
                {

                    if(FS_WriteFile(DUMMY_DRIVE,(UI8_T *)startup_filename, (UI8_T *)"CLI", FS_FILE_TYPE_CONFIG, (UI8_T *)buf, strlen(buf),0)!=FS_RETURN_OK)
                    {

                        SYSFUN_Debug_Printf("\r\n\r\n[Failed FS_WriteFile]\r\n\r\n\r\n");
                        CLI_MGR_FreeBuffer(buf);
                        return FALSE;
                    }

                    if(FS_SetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, (UI8_T *)startup_filename)!=FS_RETURN_OK)
                    {

                        SYSFUN_Debug_Printf("\r\n\r\n[Failed FS_SetStartupFilename]\r\n\r\n\r\n");
                        CLI_MGR_FreeBuffer(buf);
                        return FALSE;
                    }
                }
                else
                {
                    /*put old stackingDB in a array*/
                    {
                        for(i=0;i<SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;i++)
                        {
                            char        mac_str[20] = {0};
                            UI32_T      device_type = 0;
                            UI32_T      unit_id = 0;
                            char        sub_str[40] = {0};
                            CLI_LIB_EAtoStr((char *)stacking_db_old[i].mac_addr, mac_str);
                            device_type = stacking_db_old[i].device_type;
                            unit_id = stacking_db_old[i].unit_id;
                            SYSFUN_Sprintf((char *)sub_str,"%02lu_%s_%02lu",unit_id,mac_str,device_type);
                            strcpy((char *)old_str_buffer[i], (char *)sub_str);
//                            printf("\n %s %d \n ", __FUNCTION__,__LINE__);
                        }
                    }

                    /*compare old and new*/
                    for(i=0;i<SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;i++)
                    {
                        char        mac_str_new[20] = {0};
                        UI32_T      device_type_new = 0;
                        UI32_T      unit_id_new = 0;
                        BOOL_T      is_find = FALSE;
                        BOOL_T      is_unit_use = FALSE;
//printf("\n %s %d \n ", __FUNCTION__,__LINE__);
                        CLI_LIB_EAtoStr((char *)stacking_db_new[i].mac_addr, mac_str_new);
                        device_type_new = stacking_db_new[i].device_type;
                        unit_id_new = stacking_db_new[i].unit_id;
                        if( unit_id_new == 0)
                        {
                            continue;
                        }

                        for(j=0;j<SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;j++)
                        {
                            char        mac_str_old[20] = {0};
                            UI32_T      device_type_old = 0;
                            UI32_T      unit_id_old = 0;

                            CLI_LIB_EAtoStr((char *)stacking_db_old[j].mac_addr, mac_str_old);
                            device_type_old = stacking_db_old[j].device_type;
                            unit_id_old = stacking_db_old[j].unit_id;
                            if( unit_id_old == 0)
                            {
                                continue;
                            }
                            if(strcmp(mac_str_new,mac_str_old) == 0)
                            {
                                /*same mac but unit change -> use new to replace old*/
                                if(unit_id_new != unit_id_old)
                                {
                                    SYSFUN_Sprintf(sub_str,"%02lu_%s_%02lu",unit_id_new,mac_str_new,device_type_new);
                                    strcpy(old_str_buffer[j], sub_str);
                                    stacking_db_old[j].unit_id = unit_id_new;
                                    CLI_LIB_AtoEa(mac_str_new, mac_addr);
                                    memcpy(stacking_db_old[j].mac_addr,mac_addr,SYS_ADPT_MAC_ADDR_LEN);
                                    stacking_db_old[j].device_type = device_type_new;
                                    /*serch if new unit id is use and delete it */
                                    for(k = 0; k <SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ; k++)
                                    {
                                        if(stacking_db_old[k].unit_id == unit_id_new)
                                        {
                                            if(k == j)
                                                continue;
                                            strcpy(old_str_buffer[k],"00_00-00-00-00-00-00_00");
                                            stacking_db_old[k].unit_id = 0;
                                            memset(stacking_db_old[k].mac_addr,0,SYS_ADPT_MAC_ADDR_LEN);
                                            stacking_db_old[k].device_type = 0;
                                        }
                                    }
                                    unmatch_count++;
                                }
                                is_find = TRUE;
                            }

                            /*check if new unit id use in old header*/
                            if( unit_id_new == unit_id_old )
                            {
                                is_unit_use =TRUE;
                                use_unit = j;
                            }
                        }

                        if(is_find == FALSE )
                        {
                            rew_count++;
                            if( is_unit_use == FALSE)
                            {
                                /*serch empty block */
                                for(k = 0; k <SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ; k++)
                                {
                                    if(stacking_db_old[k].unit_id == 0)
                                    {
                                        SYSFUN_Sprintf(sub_str,"%02lu_%s_%02lu",unit_id_new,mac_str_new,device_type_new);
                                        strcpy(old_str_buffer[k], sub_str);
                                        stacking_db_old[k].unit_id = unit_id_new;
                                        CLI_LIB_AtoEa(mac_str_new, mac_addr);
                                        memcpy(stacking_db_old[k].mac_addr,mac_addr,SYS_ADPT_MAC_ADDR_LEN);
                                        stacking_db_old[k].device_type = device_type_new;
                                        /*serch if new unit id is use and delete it */
                                        for(m = 0; m <SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ; m++)
                                        {
                                            if(m == k)
                                                continue;
                                            if(stacking_db_old[m].unit_id == unit_id_new)
                                            {
                                                strcpy(old_str_buffer[m],"00_00-00-00-00-00-00_00");
                                                stacking_db_old[m].unit_id = 0;
                                                memset(stacking_db_old[m].mac_addr,0,SYS_ADPT_MAC_ADDR_LEN);
                                                stacking_db_old[m].device_type = 0;
                                            }
                                        }
                                        break;
                                    }
                                }
                                //append
                            }
                            else
                            {
                                SYSFUN_Sprintf(sub_str,"%02lu_%s_%02lu",unit_id_new,mac_str_new,device_type_new);
                                strcpy(old_str_buffer[use_unit],sub_str);
                                stacking_db_old[use_unit].unit_id = unit_id_new;
                                CLI_LIB_AtoEa(mac_str_new, mac_addr);
                                memcpy(stacking_db_old[use_unit].mac_addr,mac_addr,SYS_ADPT_MAC_ADDR_LEN);
                                stacking_db_old[use_unit].device_type = device_type_new;
                                //override
                            }
                        }

                }

                    if(rew_count>0 || unmatch_count>0)
                    {
                        /* replace to old config buffer*/
                        char     *cmd_buf;
                        UI32_T len = 0;
                        char  type_str[30] = {0};
	                    UI32_T start_pont = 0;
                        char  temp_str[60] = {0};
                        UI32_T read_count = 0;

                        /*sort*/
                        for(i = 0;i < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; i++)
                        {
                            strcpy(final_str_buffer[i],"00_00-00-00-00-00-00_00");
                        }
                        for(i = 1;i <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; i++)
                        {
                            for(j = 0;j < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; j++)
                            {
                                if(stacking_db_old[j].unit_id == i)
                                {
                                    strcpy(final_str_buffer[current_setp],old_str_buffer[j]);
                                    current_setp++;
                                }
                            }
                        }
                        for(i = 0;i<strlen(buf);i++)
                        {
                            if(memcmp(buf+i,"!<stackingMac>",14) == 0)
                            {
                                start_pont = i;
                                break;
                            }
                        }
                        if((cmd_buf = (char *)L_MM_Malloc(CLI_DEF_MAX_BUFSIZE, L_MM_USER_ID2(SYS_MODULE_CLI, CLI_TYPE_TRACE_ID_CLI_TASK_REPRODUCE_CONFIG))) ==NULL)
                        {
                            CLI_MGR_FreeBuffer(buf);
                            return FALSE;
                        }
                        memset(cmd_buf,0,sizeof(cmd_buf));
                        buf_temp = buf;
                        k = 0;
                        while ( read_count < CLI_DEF_MAX_HEADER )
                        {
                            if (CLI_TASK_ReadLine_From_XferBuf(cmd_buf, &buf_temp))
                            {
                                read_count ++;
                                len = strlen(cmd_buf);
                                i = 0;
                                j = 0;

                                for (i = 1; i < len; i++ )
                            	{
                            	    if (cmd_buf[i] == '>')
                            	    {
                            	        break;
                            	    }
                            	    if (cmd_buf[i] == '<')
                            	    {
                            	        continue;
                            	    }
                            	    else
                            	    {
                            	        /*prevent buffer excreed max*/
                            	        if (j>=30)
                            	        {
                            	            break;
                            	        }
                            	        type_str[j] = cmd_buf[i];
                            	        j++;
                            	    }
                            	}

                            	if (strcmp(type_str, "stackingMac") == 0)
                            	{
                                	sprintf(temp_str,"!<stackingMac>%s</stackingMac>\n",final_str_buffer[k]);
                        		    memcpy(buf + (start_pont+strlen(temp_str)*k),temp_str,strlen(temp_str));
                        		    k ++;
                        		}
                                /*
                                06/08/2004 11:00 pttch type_str need init.
                                */
                        		memset(type_str, 0, sizeof(type_str));
                            }
                            else
                            {
                                L_MM_Free(cmd_buf);
                                CLI_MGR_FreeBuffer(buf);
                                return FALSE;
                            }
                        }

                        if(FS_WriteFile(DUMMY_DRIVE,(UI8_T *)startup_filename, (UI8_T *)"CLI", FS_FILE_TYPE_CONFIG, (UI8_T *)buf, strlen((char *)buf),0)!=FS_RETURN_OK)
                        {
                            SYSFUN_Debug_Printf("\r\n\r\n[Failed FS_WriteFile]\r\n\r\n\r\n");
                            L_MM_Free(cmd_buf);
                           CLI_MGR_FreeBuffer(buf);
                            return FALSE;
                        }

                        if(FS_SetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, (UI8_T *)startup_filename)!=FS_RETURN_OK)
                        {
                            SYSFUN_Debug_Printf("\r\n\r\n[Failed FS_SetStartupFilename]\r\n\r\n\r\n");
                            L_MM_Free(cmd_buf);
                            CLI_MGR_FreeBuffer(buf);
                            return FALSE;
                        }
                        L_MM_Free(cmd_buf);
                    }
                }

                CLI_MGR_FreeBuffer(buf);
            }
            else
            {

                SYSFUN_Debug_Printf("\r\n[CLI_TASK_Reproduce_Config not enough memery]\r\n");
                return FALSE;
            }
        }
    }

    strcpy(file_name , startup_filename);
    return TRUE;
#endif
}

/*Set StackingDB to swctrl for renumber unit ID*/
static BOOL_T CLI_TASK_SetStackingDB(void)
{

    char   type_str[30] = {0};
	char   content_str[30] = {0};
	UI32_T len = 0;
	UI32_T i = 0, j = 0, k = 0;
    char     *tmp_xbuf;
    char     *tmp_xbuf_origin;
    char     *cmd_buf;
    UI32_T  read_count = 0;
    UI32_T  unit_id = 0;
    char    mac_addr_str[20];
	char    mac_addr[SYS_ADPT_MAC_ADDR_LEN] = {0};
    char    startupfile[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1] = {0};
    FS_File_Attr_T file_attr;
    UI32_T xfer_buf_length=0;
    char   temp_num[4] = {0};
    UI8_T  mac_addr_table[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][6];
    UI32_T sucess_count  = 0;
    UI32_T device_type_arr[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];
    UI32_T device_type = 0;


    memset(mac_addr_table,0,sizeof(mac_addr_table));

    for(i=0;i<SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;i++)
    {
      device_type_arr[i] = 0xffffffff;
    }

    if((tmp_xbuf_origin = (char *)CLI_MGR_AllocateBuffer()) == NULL)
    {
        return FALSE;
    }

    if((cmd_buf = (char *)L_MM_Malloc(CLI_DEF_MAX_BUFSIZE, L_MM_USER_ID2(SYS_MODULE_CLI, CLI_TYPE_TRACE_ID_CLI_TASK_SETSTACKINGDB))) == NULL)
    {
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        return FALSE;
    }
    memset(cmd_buf,0,sizeof(cmd_buf));
    tmp_xbuf = tmp_xbuf_origin;

    if(FS_RETURN_OK!=FS_GetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, (UI8_T *)startupfile))
    {
        CLI_LIB_PrintStr("Failed to get start file name\r\n");
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        L_MM_Free(cmd_buf);
        return FALSE;
    }

    file_attr.file_type_mask = FS_FILE_TYPE_MASK(FS_FILE_TYPE_CONFIG);
    strcpy((char *)file_attr.file_name, (char *)startupfile);
    if(FS_GetFileInfo(DUMMY_DRIVE, &file_attr) != FS_RETURN_OK)
    {
        CLI_LIB_PrintStr("Failed to get start file info\r\n");
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        L_MM_Free(cmd_buf);
        return FALSE;
    }

    if (FS_ReadFile( DUMMY_DRIVE, file_attr.file_name ,  (UI8_T *)tmp_xbuf, CLI_DEF_MAX_CONFIG_SIZE, &xfer_buf_length) != FS_RETURN_OK)
    {
        CLI_LIB_PrintStr("Failed to read file info\r\n");
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        L_MM_Free(cmd_buf);
        return FALSE;
    }

    while ( read_count < CLI_DEF_MAX_HEADER )
    {
        if (CLI_TASK_ReadLine_From_XferBuf(cmd_buf, &tmp_xbuf))
        {
            read_count ++;
            len = strlen((char *)cmd_buf);
            i = 0;
            j = 0;
            k = 0;

            for (i = 1; i < len; i++ )
        	{
        	    if (cmd_buf[i] == '>')
        	    {
        	        break;
        	    }
        	    if (cmd_buf[i] == '<')
        	    {
        	        continue;
        	    }
        	    else
        	    {
        	        /*prevent buffer excreed max*/
        	        if (j>=30)
        	        {
        	            break;
        	        }
        	        type_str[j] = cmd_buf[i];
        	        j++;
        	    }
        	}

        	k = 0;
            j = 0;
        	if (strcmp((char *)type_str, "stackingMac") == 0)
        	{
        	    for (k = i+1; k < len; k++ )
        	    {
        	        if (cmd_buf[k] == '<')
        	        {
        	            break;
        	        }
        	        else
        	        {
                        /*prevent buffer excreed max*/
        	            if (j>=30)
        	            {
        	                break;
        	            }
        	            content_str[j] = cmd_buf[k];
        	            j++;
        	        }
        	    }

                if(content_str[0] != '0'||content_str[1] != '0')
            	{
            	    unit_id = 0;
                    memset(mac_addr,0,SYS_ADPT_MAC_ADDR_LEN);
            	    memset(mac_addr_str,0,sizeof(mac_addr_str));

    		        memcpy(temp_num,content_str,2);
    		        temp_num[2] = '\0';
    		        unit_id = atoi((char *)temp_num);

    		        memcpy(mac_addr_str,content_str+3,17);
    		        mac_addr_str[17] = '\0';
    		        CLI_LIB_AtoEa(mac_addr_str, mac_addr);

    		        memcpy(temp_num,content_str+21,2);
    		        temp_num[2] = '\0';
    		        device_type = atoi((char *)temp_num);

    		        device_type_arr[unit_id - 1] = device_type;
    		        memcpy((char *)mac_addr_table[unit_id - 1],mac_addr,SYS_ADPT_MAC_ADDR_LEN);
    		        sucess_count++;
    		    }
            }
        }
        else
        {
            CLI_MGR_FreeBuffer(tmp_xbuf_origin);
            L_MM_Free(cmd_buf);
            return FALSE;
        }
    }

    if(sucess_count > 0)
    {
        if( SWCTRL_PMGR_SetUnitsBaseMacAddrTable(mac_addr_table))
        {
            SWCTRL_PMGR_SetUnitsDeviceTypeTable(device_type_arr);
            CLI_MGR_FreeBuffer(tmp_xbuf_origin);
            L_MM_Free(cmd_buf);
            return TRUE;
        }
        else
        {
            SWCTRL_PMGR_SetUnitsDeviceTypeTable(device_type_arr);
            CLI_MGR_FreeBuffer(tmp_xbuf_origin);
            L_MM_Free(cmd_buf);
            return FALSE;
        }
    }
    else
    {
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        L_MM_Free(cmd_buf);
        return TRUE;
    }

}


static BOOL_T CLI_TASK_ReadLine_From_XferBuf(char *cmd_buf, char **xbuf_p)
{
    UI32_T i;

    memset(cmd_buf, 0, CLI_DEF_MAX_BUFSIZE);

    for ( i=0; i<CLI_DEF_MAX_BUFSIZE && ( *(*xbuf_p+i) != '\n' && *(*xbuf_p+i)!='\0'); i++ )
    {
/*
   pttch 92.04.24 add for check config file is valid or not
*/
       if (*(*xbuf_p+i) > 127)
       {
          return FALSE;
       }
       *(cmd_buf+i) = *(*xbuf_p+i);
    }

    *(cmd_buf+i) = 0;
    *xbuf_p = *xbuf_p + (i+1);

    return TRUE;
}

#if (SYS_CPNT_CLUSTER == TRUE)
/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_SetTelnetRelayingFlag
 *------------------------------------------------------------------------
 * FUNCTION: Set responding telnet relaying status
 * INPUT   : tnpdTaskId, isRelaying
 * OUTPUT  : none
 * RETURN  : none
 * NOTE    :
 *-----------------------------------------------------------------------*/
void CLI_TASK_SetTelnetRelayingFlag(UI32_T tnpdTaskId, BOOL_T isRelaying)
{
    UI8_T i;

    // for (i = 0; i < CLI_TASK_MAX_SESSION_NUM+2; i++)
    for (i = CLI_TASK_MIN_TELNET_SESSION_ID; i <= CLI_TASK_MAX_TELNET_SESSION_ID; i++)
    {
        if (cli_working_buffer[i].CMenu.TelnetTnpdTaskId == tnpdTaskId)
        {
            cli_working_buffer[i].CMenu.IsTelnetRelaying = isRelaying;
            return;
        }
    }
}
#endif

/*jingyan zheng add for defect ES3628BT-FLF-ZZ-00275*/
void CLI_TASK_CloseTelnetSessions(void )
{
    int i;

    for (i = CLI_TASK_MIN_TELNET_SESSION_ID; i <= CLI_TASK_MAX_TELNET_SESSION_ID; i++)
    {
        if (cli_working_buffer[i].is_empty == FALSE )
        {
            if(cli_working_buffer[i].sess_type == CLI_TYPE_TELNET)
            {
                CLI_TASK_SetKillWorkingSpaceFlag(i);
            }
        }
    }
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_TelnetIPFilter
 *------------------------------------------------------------------------
 * FUNCTION: This function will close telnet sessions if the ip address
                    of the telnet session is not in the management ip filter range
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : none
 * NOTE    :
 *-----------------------------------------------------------------------*/
void CLI_TASK_TelnetIPFilter()
{
    int i;

    for (i = CLI_TASK_MIN_TELNET_SESSION_ID; i <= CLI_TASK_MAX_TELNET_SESSION_ID; i++)
    {
        if (cli_working_buffer[i].is_empty == FALSE && cli_working_buffer[i].sess_type == CLI_TYPE_TELNET)
        {
            if (!MGMT_IP_FLT_IsValidIpFilterAddress(MGMT_IP_FLT_TELNET, &(cli_working_buffer[i].CMenu.TelnetIPAddress)))
            {
                CLI_TASK_SetKillWorkingSpaceFlag(i);
            }
        }
    }
}

#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetWorkingAreaByIndex
 *-----------------------------------------------------------
 * FUNCTION: Get working buffer by index
 * INPUT   : UI32_T index
 * OUTPUT  : None
 * RETURN  :
 *----------------------------------------------------------*/
CLI_TASK_WorkingArea_T *CLI_TASK_GetWorkingAreaByIndex( UI32_T index)
{
    if( (index==CLI_TASK_NULL_SESS_ID) || (index >= CLI_TASK_MAX_SESSION_ID) )
    {
        return NULL;
    }
    return (&cli_working_buffer[index]);
}
#endif /* SYS_CPNT_SYSMGMT_DEFERRED_RELOAD */
