/* MODULE NAME:  cli_mgr.c
 * PURPOSE:
 *     This is MGR functions for CLI
 *
 * NOTES:
 *
 * REASON:
 * Description:
 * HISTORY
 *    5/20/2007 - Rich Lee, Created for Linux platform
 *
 * Copyright(C)      Accton Corporation, 2006
 */
/* INCLUDE FILE DECLARATIONS
 */
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "sysfun.h"
#include "cli_api.h"
#include "cli_mgr.h"
#include "cli_lib.h"
#include "cli_runcfg.h"
#include "cli_om.h"

#include "leaf_1213.h"
#include "swctrl.h"
#include "fs.h"
#include "stdlib.h"
#include "sys_cpnt.h"
#include "sys_module.h"
#include "l_mm.h"

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
#include "l_link_lst.h"
#endif

#include "cli_type.h"
#include "sys_callback_mgr.h"

#if 0 /* rich */
#include "xfer_mgr.h"
#include "xfer_buf_mgr.h"
#include "cli_backdoor.h"
#include "sys_time.h"
#include "stkctrl_task.h"
#include "stkctrl_pom.h"

#include "cli_proc_comm.h"
#include "l_ipcmsgq.h"
#include "stktplg_mgr.h"
#endif /* rich */


#include "fs_type.h"
#include "cli_task.h"
#include "sys_mgr.h"
#include "cli_auth.h"
#include "sysfun.h"

#include "l_ipcio.h"

#include "l_threadgrp.h"
#include "buffer_mgr.h"

#if 0 /* rich */
#if (SYS_CPNT_3COM_CLI_PROVISION == TRUE)
#include "cli_runcfg_3com.h"
#endif

#if (SYS_CPNT_3COM_CLI == TRUE)
#include "cfgdb_mgr.h"
#endif
#endif /* rich */

#if (SYS_CPNT_MOTD == TRUE)
#include "SYS_BNR_PMGR.h"
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
#include "stktplg_pom.h"
#endif

#if (SYS_CPNT_CLI_BANNER == TRUE)
#include "cli_banner.h"
#endif


/* NAMING CONSTANT DECLARATIONS
 */

/*------------------------------------------------------------------------
 * GLOBAL VARIABLES
 *-----------------------------------------------------------------------*/
#if 0  /* rich linux */
#define CLI_USE_CSC_CHECK_OPER_MODE(RET_VAL, function_no)                             \
  if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_SLAVE_MODE)  {   \
     EH_PMGR_Handle_Exception(SYS_MODULE_CLI, function_no, EH_TYPE_MSG_NOT_IN_MASTER_MODE, SYSLOG_LEVEL_INFO);\
     return (RET_VAL);                                                      \
  }

#define CLI_USE_CSC_CHECK_OPER_MODE_WITHOUT_RETURN_VALUE(function_no)               \
  if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_SLAVE_MODE)  {   \
    EH_PMGR_Handle_Exception(SYS_MODULE_CLI, function_no, EH_TYPE_MSG_NOT_IN_MASTER_MODE, SYSLOG_LEVEL_INFO);\
    return;                                                                 \
  }

#else

#define CLI_USE_CSC_CHECK_OPER_MODE(RET_VAL, function_no)                             \
  if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_SLAVE_MODE)  {   \
     return (RET_VAL);                                                      \
  }

#define CLI_USE_CSC_CHECK_OPER_MODE_WITHOUT_RETURN_VALUE(function_no)               \
  if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_SLAVE_MODE)  {   \
    return;                                                                 \
  }
#endif /* rich */

#define CLI_RETURN_AND_RELEASE_CSC(RET_VAL)  {               \
    return (RET_VAL);                                           \
  }

#define CLI_RETURN_AND_RELEASE_CSC_WITHOUT_RETUEN_VALUE()  { \
    return;                                                     \
  }

#define CLI_MGR_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id)    SYSFUN_OM_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id)
#define CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority)    SYSFUN_OM_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority)

/* MACRO FUNCTION DECLARATIONS
 */
#ifndef _countof
#define _countof(ary)   (sizeof(ary)/sizeof(*ary))
#endif

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static UI32_T   cli_mgr_semaphore_id;
static UI32_T CLI_MGR_current_session_count;
static BOOL_T CLI_MGR_Is_Now_Provision;
static BOOL_T CLI_MGR_All_Cscs_Know_Provision_Complete;
static BOOL_T CLI_MGR_Go_To_Provision;
static UI8_T *CLI_MGR_Provision_Pointer;
static BOOL_T CLI_MGR_Realize_EnterTransitionMode;
static BOOL_T CLI_MGR_Is_XferInProgress;
static BOOL_T CLI_MGR_XferProcessResult;
static UI32_T provision_error_command_count;
#if (SYS_CPNT_3COM_CLI == TRUE)
static BOOL_T CLI_MGR_Is_PrintComplete;
#endif

#if (SYS_CPNT_3COM_PASSWORD_RECOVERY == TRUE)
static BOOL_T  clear_password_recovery = FALSE;
#endif

#if (SYS_CPNT_DBSYNC_TXT ==TRUE)
static UI32_T time_ticks_b;
static UI32_T time_ticks_a;
#endif

#if (SYS_CPNT_AUTOBAUDRATE == TRUE)
static  BOOL_T (*autobaud_register)(void) ;
static  BOOL_T (*autobaud_unregister)(void) ;
#endif

/* STATIC VARIABLE DECLARATIONS
 */
/* ipc key for doing ipcio
 * this is used by backdoor
 */
/* buffer for doing ipcio
 */

 static BOOL_T CLI_MGR_Will_PrintProvisionMsg;

static SYS_TYPE_CallBack_T *provision_complete_callback;
static SYS_TYPE_CallBack_T *enter_transition_callback;
static SYS_TYPE_CallBack_T *module_provision_complete_callback;
/*atic void CLI_MGR_WebSaveRunningCfg_Callback(UI32_T cookie, UI32_T status);*/

#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
static void CLI_MGR_ReloadNotify_Callback(UI32_T remaining);
#endif  /* SYS_CPNT_SYSMGMT_DEFERRED_RELOAD */

static BOOL_T is_config_file_ok;
/*to fix telnet cli session do not show error message*/
static UI32_T CLI_MGR_xfer_callback_status;
static BOOL_T CLI_MGR_is_read_xfer_callback_status;
static UI8_T *CLI_MGR_WebAllocXFerPtr;

/* support CLI to display progress of saving config in the telnet session
 * when system reboot/renumber/initial.
 */
static BOOL_T   CLI_MGR_stkctrl_saving_config_callback_status;

//static UI8_T tftp_error_msg[MAXSIZE_fileCopyTftpErrMsg+1] = {0};
#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
static char    device_cfg[ CLI_MGR_NUMBER_OF_TOTAL_DEVICE ][ CLI_MGR_RUNTIME_PROVISION_UC_MEM_SIZE ];
#else
static char  module_cfg[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][CLI_MGR_RUNTIME_PROVISION_UC_MEM_SIZE];
#endif
/*atic BOOL_T CLI_MGR_CheckStkVersionValidate(void);*/
//static BOOL_T CLI_MGR_is_reproduce_done; rich move to OM
static BOOL_T   cli_buf_allocated = 0;
static char     cli_buf [CLI_DEF_MAX_CONFIG_SIZE];
static char     stacking_buf [CLI_DEF_MAX_HEADER*CLI_DEF_DISP_WIDTH];
static UI32_T    cli_provision_dirtybit[ SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ];

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)

static BOOL_T
CLI_MGR_LocalPrivLvlCmdEntryCompare(
    void *inlist,
    void *input
);

static L_LINK_LST_List_T*
CLI_MGR_LocalGetPrivLvlCmdList(
    UI8_T cmd_mode
);

static CLI_PrivLvlCmdEntry_T *
CLI_MGR_LocalFindPrivLvlCmdEntry(
    UI8_T cmd_mode,
    BOOL_T is_all,
    const char *cmd_str
);

static void 
CLI_MGR_LocalFreePrivLvlCmdEntryList();

static BOOL_T
CLI_MGR_LocalDelPrivLvlCmdEntry(
    UI8_T cmd_mode,
    BOOL_T is_all,
    const char *cmd_str
);

static BOOL_T
CLI_MGR_LocalAddPrivLvlCmdEntry(
    UI8_T cmd_mode,
    BOOL_T is_all,
    UI8_T pri_lvl,
    const char *cmd_str
);

static BOOL_T
CLI_MGR_LocalGetNextPrivLvlCmdEntry(
    UI32_T *idx,
    CLI_PrivLvlCmdEntry_T *plce_p
);

static BOOL_T 
CLI_MGR_LocalUpdatePrivilege(
    UI8_T cmd_mode
);

/* NOTE: CLI_ACCESS_MODE_MAX_NUM is starting from 0
 */
static L_LINK_LST_List_T    privilege_list[ CLI_ACCESS_MODE_MAX_NUM ];

/* squid */
#endif    /* #if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE) */

/*--------------------- extern ---------------------*/
/* cli_main.c */
#if (SYS_CPNT_MOTD == TRUE)
extern BOOL_T operate_sys_banner_msg_status;
extern UI8_T  operate_sys_banner_msg_by_session_id;
extern UI8_T  operate_sys_banner_msg_delimiting_character;
extern UI8_T  cli_sys_bnr_msg[SYS_ADPT_MAX_MOTD_LENGTH + 1];
extern UI32_T cli_sys_bnr_msg_position;
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/


static BOOL_T    cli_mgr_enter_runtime_provision_mode = FALSE;
static I32_T     cli_mgr_buffermgmt_offset =0;
SYSFUN_DECLARE_CSC;

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
static BOOL_T  cli_mgr_dhcp_rx_option_config_status;
static BOOL_T  cli_mgr_is_startup_factory_default;

static char  cli_mgr_dhcp_rx_option_config_address[SYS_ADPT_DHCP_MAX_DOMAIN_NAME_LEN]={0};
static char  cli_mgr_dhcp_rx_option_config_filename_ar[MAXSIZE_tftpSrcFile+1]={0};
#endif  /* #if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE) */



/* EXPORTED SUBPROGRAM BODIES
 */

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Init
 *-----------------------------------------------------------
 * FUNCTION: To initialize the MGR module of CLI
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Init()
{
#if CLI_BACKDOOR
    CLI_BACKDOOR_Init();
#endif
/*   if(SYSFUN_CreateSem (1, SYSFUN_SEM_FIFO, &cli_mgr_semaphore_id) != SYSFUN_OK) */
   if(SYSFUN_CreateSem(SYSFUN_SEMKEY_PRIVATE, 1, SYSFUN_SEM_FIFO, &cli_mgr_semaphore_id) !=SYSFUN_OK)
    {
        SYSFUN_Debug_Printf ("\r\n CLI: Failed to create semaphore. LOCK");
        while (1);
    }
    CLI_MGR_current_session_count = 0;
   provision_complete_callback = NULL;
   enter_transition_callback   = NULL;
   CLI_MGR_Is_Now_Provision = FALSE;
    CLI_MGR_All_Cscs_Know_Provision_Complete = FALSE;
   CLI_MGR_Go_To_Provision  = TRUE;
   CLI_MGR_Provision_Pointer = NULL;
   CLI_MGR_Realize_EnterTransitionMode = TRUE;
   CLI_MGR_Is_XferInProgress = FALSE;
   CLI_MGR_XferProcessResult = FALSE;
   CLI_MGR_Will_PrintProvisionMsg = TRUE;
   CLI_MGR_Will_PrintProvisionMsg = TRUE;
   provision_error_command_count = 0;
#if (SYS_CPNT_3COM_CLI == TRUE)
   CLI_MGR_Is_PrintComplete = FALSE;
#endif
#if (SYS_CPNT_AUTOBAUDRATE == TRUE)
   autobaud_register = NULL;
   autobaud_unregister = NULL;
#endif
   CLI_MGR_WebAllocXFerPtr = 0;
   is_config_file_ok = TRUE;
   CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_COMPLETED;
   CLI_MGR_is_read_xfer_callback_status = FALSE;
   CLI_MGR_stkctrl_saving_config_callback_status = FALSE;

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
    cli_mgr_dhcp_rx_option_config_status = FALSE;
    cli_mgr_is_startup_factory_default = FALSE;
#endif  /* #if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE) */


#if 0 /* rich */
   STKCTRL_TASK_Register_SavingConfig_CallBack(&CLI_MGR_Set_StkctrlSavingConfigStatus);
#endif
#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
    memset( device_cfg, 0, sizeof( device_cfg ) );
#else
    memset(module_cfg, 0, sizeof(module_cfg));
#endif
    module_provision_complete_callback = NULL;

    CLI_LIB_Init();
    CLI_OM_InitOmSem();
    
#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    {
        UI32_T i;
        
        for (i=0; i < _countof(privilege_list); ++i)
        {
            L_LINK_LST_Create(&privilege_list[i],
                              CLI_DEF_MAX_CMDS_NUM,
                              sizeof(CLI_PrivLvlCmdEntry_T),
                              CLI_MGR_LocalPrivLvlCmdEntryCompare,
                              FALSE);
        }
    }
#endif
}

/*--------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/
void CLI_MGR_Create_InterCSC_Relation(void)
{
#if CLI_BACKDOOR
    CLI_BACKDOOR_Init();
#endif
#if 0
    STKCTRL_TASK_Register_SavingConfig_CallBack(&CLI_MGR_Set_StkctrlSavingConfigStatus);
#endif
} /* end of CLI_MGR_Create_InterCSC_Relation */

void CLI_MGR_Set_Realize_EnterTransitionMode(BOOL_T is_Realized)
{
   CLI_MGR_Realize_EnterTransitionMode = is_Realized;
}


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
void CLI_MGR_Register_ProvisionComplete_CallBack (void (*fun)(void))
{
    SYS_TYPE_REGISTER_CALLBACKFUN(provision_complete_callback);
}

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
void CLI_MGR_AutoBaud_Register (BOOL_T (*fun)(void))
{
    autobaud_register = fun;
}

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
void CLI_MGR_AutoBaud_UnRegister (BOOL_T(*fun)(void))
{
    autobaud_register = NULL;
}


/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_CheckAutobaud
 *-----------------------------------------------------------
 * FUNCTION: Call the function in the the callback list that registered
 *           by call CLI_MGR_AutoBaud_Register();
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : Notenable
 * NOTE    : None.
 *----------------------------------------------------------*/
UI32_T CLI_MGR_CheckAutobaud(void)
{
    if(SYS_PMGR_Get_Autobaudrate_Switch()== FALSE)
        return CLI_MGR_AUTOBAUDRATE_NOT_ENABLE;
    else
        return CLI_MGR_AUTOBAUDRATE_DETECT_ENTERKEY;
}
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
BOOL_T CLI_MGR_InitAndKeepIp()
{
#if 0 /* rich */
   UI8_T  src_filename[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1]={0};
   UI8_T  *xfer_buf;
   UI32_T xbuf_length;

   if( (xfer_buf=(UI8_T *)XFER_BUF_PMGR_Allocate()) == NULL)
   {

      return FALSE;
   }

   if (XFER_PMGR_LocalToStream (SYS_DFLT_restartConfigFile,
                               xfer_buf,
                               &xbuf_length,
                               SYS_ADPT_MAX_FILE_SIZE)!=FS_RETURN_OK )
   {

      XFER_BUF_PMGR_Free(xfer_buf);
      return FALSE;
   }

#if(SYS_CPNT_3COM_CLI_PROVISION == FALSE)
   {
      //char *first,*second,*third;
      int i;
      //UI16_T num;

      for(i=xbuf_length;i>=xbuf_length-20;i--)
      {
         if(strstr(&xfer_buf[i],"end")!=NULL)
         {
            //memcpy(&xfer_buf[i],0,sizeof(xfer_buf[i]));
            //strstr(&xfer_buf[i],"end") = 0;
            memset(&xfer_buf[i],0,sizeof(xfer_buf[i]));
            break;
         }
      }
   }

   if(CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY == CLI_RUNCFG_AppendCfg(xfer_buf, SYS_ADPT_MAX_FILE_SIZE))
   {
      //CLI_LIB_PrintStr("\r\nError! No enough space to collect all information.\r\n");
      XFER_BUF_PMGR_Free(xfer_buf);
      return FALSE;
   }
#endif /*#if(SYS_CPNT_3COM_CLI_PROVISION == FALSE)*/

    /*modify 2004/4/20 bryant*/
#if(SYS_CPNT_CFGDB == TRUE)
   if (FALSE == CFGDB_MGR_ResetConfiguration(CFGDB_MGR_SECTION_TYPE_TO_BMP(CFGDB_MGR_SECTION_TYPE_EXTERNAL_AND_LOCAL), TRUE))
   {
       CLI_LIB_PrintStr("Error! Failed to copy delete cfgdb\r\n");
   }
#else
   if (FS_DeleteFile(DUMMY_DRIVE ,CFGDB_MGR_BINARY_CFG_FILE_NAME) != FS_RETURN_OK)
   {
       CLI_LIB_PrintStr("Error! Failed to copy delete cfgdb\r\n");
   }
#endif

   if(FS_GetStartupFilename(DUMMY_DRIVE,FS_FILE_TYPE_CONFIG, src_filename)!=FS_RETURN_OK)
   {
      return FALSE;
   }

   if(FS_WriteFile(1,src_filename, "Xfer", FS_FILE_TYPE_CONFIG, xfer_buf, strlen(xfer_buf),0)!=FS_RETURN_OK)
   {
    //CLI_LIB_PrintStr("Error! Failed to copy stream to Local\r\n");
       XFER_BUF_PMGR_Free(xfer_buf);
       return FALSE;
   }
   else
   {
      XFER_BUF_PMGR_Free(xfer_buf);
   }

    STKCTRL_TASK_ReloadSystem();
#endif /* rich */
   return TRUE;
}

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Get_Is_PrintComplete
 *-----------------------------------------------------------
 * FUNCTION: Get if print complete-string.
 * INPUT   : None.
 * OUTPUT  : TRUE/FALSE
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Get_Is_PrintComplete(BOOL_T *status)
{
#if (SYS_CPNT_3COM_CLI == TRUE)
    *status = CLI_MGR_Is_PrintComplete;
#endif
}

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Set_Is_PrintComplete
 *-----------------------------------------------------------
 * FUNCTION: Set if print complete-string.
 * INPUT   : TRUE/FALSE
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Set_Is_PrintComplete(BOOL_T status)
{
    CLI_MGR_Is_PrintComplete = status;
}
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
UI32_T CLI_MGR_SetPasswordRecovery(BOOL_T status)
{
#if 0 /* rich linux */
    UI8_T  src_filename[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1]={0};
    UI8_T  *xfer_buf;
    UI32_T xbuf_length;
    int i;

    if(status==TRUE)
    {
       clear_password_recovery =FALSE;
    }

    if(clear_password_recovery == FALSE)
    {

       if(status==FALSE)
       {
          clear_password_recovery =TRUE;
       }

       if(FS_GetStartupFilename(DUMMY_DRIVE,FS_FILE_TYPE_CONFIG, src_filename)!=FS_RETURN_OK)
       {
          return FALSE;
       }

       if((xfer_buf=(UI8_T *)XFER_BUF_MGR_Allocate()) == NULL)
       {
          return FALSE;
       }

       if (XFER_PMGR_LocalToStream (src_filename,
                                  xfer_buf,
                                  &xbuf_length,
                                  SYS_ADPT_MAX_FILE_SIZE)!=FS_RETURN_OK )
       {

         XFER_BUF_PMGR_Free(xfer_buf);
         return FALSE;
       }


       if(status==FALSE)
       {
          for(i=xbuf_length;i>=xbuf_length-30;i--)
          {
             if(strstr(&xfer_buf[i],"password-recovery")!=NULL)
             {
                memset(&xfer_buf[i],0,sizeof(xfer_buf[i]));
                break;
             }
             if(strstr(&xfer_buf[i],"end")!=NULL)
             {
                memset(&xfer_buf[i],0,sizeof(xfer_buf[i]));
             }
          }

          strcat(xfer_buf, "\nend\n");
       }
       else
       {
          for(i=xbuf_length;i>=xbuf_length-20;i--)
          {
             if(strstr(&xfer_buf[i],"end")!=NULL)
             {
                memset(&xfer_buf[i],0,sizeof(xfer_buf[i]));
                break;
             }
          }
          strcat(xfer_buf, "\npassword-recovery\nend\n");
       }

       if(FS_WriteFile(1,src_filename, "Xfer", FS_FILE_TYPE_CONFIG, xfer_buf, strlen(xfer_buf),0)!=FS_RETURN_OK)
       {
           XFER_BUF_PMGR_Free(xfer_buf);
           return FALSE;
       }
       else
       {
          XFER_BUF_PMGR_Free(xfer_buf);
       }
    }
#endif
    return TRUE;
}

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_IsDoingPasswordRecovery
 *-----------------------------------------------------------
 * FUNCTION: If is doing or not doing password recovery within 30 sec.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE
 * NOTE    : None.
 *----------------------------------------------------------*/
BOOL_T CLI_MGR_IsDoingPasswordRecovery()
{
   if(CLI_AUTH_isDoingPasswordRecovery()==TRUE)
   {
      return TRUE;
   }
   return FALSE;
}

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
void CLI_MGR_Notify_ProvisionComplete(void)
{
/* call SYS_MGR for process callback handler
 */
    SYS_CALLBACK_MGR_ProvisionCompleteCallback(SYS_MODULE_CLI);


}

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
void CLI_MGR_Register_EnterTransitionMode_CallBack (void (*fun)(void))
{
    SYS_TYPE_REGISTER_CALLBACKFUN(enter_transition_callback);
}

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
void CLI_MGR_Notify_EnterTransitionMode(void)
{
    char   *provision_p = NULL;

    provision_p = (char *) BUFFER_MGR_GetPtr(CLI_MGR_Get_BuffermgmtOffset());

    if (provision_p != NULL)
    {
        /* check is config file
        */
        if (memcmp(provision_p, CLI_DEF_RUNTIME_PROVISION_MAGIC_WORD, sizeof(CLI_DEF_RUNTIME_PROVISION_MAGIC_WORD)-1) == 0)
        {
            if ((provision_p+sizeof(CLI_DEF_RUNTIME_PROVISION_MAGIC_WORD)-1)[0] == '\0')
            {
                CLI_LIB_PrintStr("\r\nInvalid configuration file; cannot perform runtime provision\r\n");
                CLI_MGR_Set_BuffermgmtOffset(0);
                BUFFER_MGR_Free(provision_p);
                provision_p = NULL;
                return;
            }
        }
    }

    /* call SYS_MGR for process callback handler
     */
    SYS_CALLBACK_MGR_EnterTransitionModeCallback(SYS_MODULE_CLI);
    return;
}

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
void CLI_MGR_Opmode_Enter_Master_Mode()
{
   SYSFUN_ENTER_MASTER_MODE();
}

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
void CLI_MGR_SetTransitionMode(void)
{
    SYSFUN_SET_TRANSITION_MODE();
    /*
    must set provision flag that provision can do
    */
    CLI_MGR_Is_Now_Provision = FALSE;
    CLI_MGR_All_Cscs_Know_Provision_Complete = FALSE;
    CLI_MGR_Go_To_Provision  = TRUE;
    CLI_MGR_Provision_Pointer = NULL;
    CLI_MGR_Realize_EnterTransitionMode = TRUE;
    provision_error_command_count = 0;

#if ( SYS_CPNT_UNIT_HOT_SWAP == FALSE )
    /* ES4649-38-00001 */
    memset(module_cfg, 0, sizeof(module_cfg));
#endif

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    CLI_MGR_LocalFreePrivLvlCmdEntryList();
    CLI_LIB_Init();
#endif
}
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
void CLI_MGR_Opmode_Enter_Slave_Mode()
{
   SYSFUN_ENTER_SLAVE_MODE();
}

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
void CLI_MGR_Opmode_Enter_Transition_Mode()
{
   SYSFUN_ENTER_TRANSITION_MODE();
   while(!CLI_MGR_Realize_EnterTransitionMode)
   {
      CLI_LIB_PrintStr("still don't know in Transition mode, wait...\n");
      SYSFUN_Sleep(10);
   }
}

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
UI32_T CLI_MGR_Opmode_Get_Operation_Mode()
{
   return SYSFUN_GET_CSC_OPERATING_MODE();
}

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
void CLI_MGR_Set_Provision_Flag(BOOL_T status)
{
    CLI_MGR_Is_Now_Provision = status;
}

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Get_Provision_Flag
 *-----------------------------------------------------------
 * FUNCTION: Get if provision is in progrerss.
 * INPUT   : None.
 * OUTPUT  : TRUE/FALSE
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Get_Provision_Flag(BOOL_T *status)
{
    *status = CLI_MGR_Is_Now_Provision;
}

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Wait_Provision_Finish
 *-----------------------------------------------------------
 * FUNCTION: A function that wait until provision has been completed
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : Whether provision succeeded and still in master mode.
 * NOTE    : None.
 *----------------------------------------------------------*/
BOOL_T CLI_MGR_Wait_Provision_Finish()
{
    BOOL_T status;

    CLI_MGR_Get_Provision_Flag(&status);

    /* prevent from race condition, actually it's unnecessary
     */
    if(!status)
        CLI_MGR_Set_Provision_Flag(TRUE);

    while (CLI_MGR_Is_Now_Provision)
    {

        /* If opmode is not master mode any more,
         * we should not continue master mode action.
         */
        if (CLI_MGR_OPMODE_MASTER_MODE != CLI_MGR_Opmode_Get_Operation_Mode())
        {
            /* If this function returns FALSE, it will cause
             * the caller in "cli_task.c" to break out of:
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
            return FALSE;
        }
        SYSFUN_Sleep(10);
    }
    return TRUE;
}


/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_WaitAllCscKnowProvisionComplete
 *-----------------------------------------------------------
 * FUNCTION: A function that wait until all CSCs know provision complete.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_WaitAllCscsKnowProvisionComplete()
{
    while (CLI_MGR_All_Cscs_Know_Provision_Complete==FALSE)
    {

        CLI_MAIN_HandleMGR_Thread(SYSFUN_TIMEOUT_NOWAIT, 0, NULL);

        if(CLI_MGR_Opmode_Get_Operation_Mode()!=CLI_MGR_OPMODE_MASTER_MODE)
        {
            break;
        }

        SYSFUN_Sleep(10);
    }
}


/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_AllCscsKnowProvisionComplete
 *-----------------------------------------------------------
 * FUNCTION: A function to make CLI know al CSCs already know provision complete.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : Call by STKCTRL.
 *----------------------------------------------------------*/
void CLI_MGR_AllCscsKnowProvisionComplete()
{
    CLI_MGR_All_Cscs_Know_Provision_Complete = TRUE;
}

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
void CLI_MGR_Set_Is_Go_To_Provision(BOOL_T is_Go_To_Provision)
{
   CLI_MGR_Go_To_Provision = is_Go_To_Provision;
}

/*-----------------------------------------------------------
 * ROUTINE NAME -  CLI_MGR_Get_Is_Go_To_Provision
 *-----------------------------------------------------------
 * FUNCTION: Get the flag that judge if CLI can perfoem provision or not
 * INPUT   : None.
 * OUTPUT  : TRUE/FALSE
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Get_Is_Go_To_Provision(BOOL_T *is_Go_To_Provision)
{
   *is_Go_To_Provision = CLI_MGR_Go_To_Provision;
}

void CLI_MGR_Set_Provision_Pointer(UI8_T *Provision_Ptr)
{
   CLI_MGR_Provision_Pointer = Provision_Ptr;
}

void CLI_MGR_Get_Provision_Pointer(UI8_T **Provision_Ptr)
{
   *Provision_Ptr = CLI_MGR_Provision_Pointer;
}

void CLI_MGR_Set_PrintProvisionMsg(BOOL_T to_print)
{
   CLI_MGR_Will_PrintProvisionMsg = to_print;
}

void CLI_MGR_Get_PrintProvisionMsg(BOOL_T *to_print)
{
   *to_print = CLI_MGR_Will_PrintProvisionMsg;
}




void CLI_MGR_Set_XferProgressStatus(BOOL_T Is_Xfer_In_Progress)
{
   CLI_MGR_Is_XferInProgress = Is_Xfer_In_Progress;
}

void CLI_MGR_Get_XferProgressStatus(BOOL_T *result)
{
   *result = CLI_MGR_Is_XferInProgress;
}




void CLI_MGR_Set_XferProgressResult(BOOL_T result)
{
   CLI_MGR_XferProcessResult = result;
}

void CLI_MGR_Get_XferProgressResult(BOOL_T *result)
{
   *result = CLI_MGR_XferProcessResult;
}

void CLI_MGR_Get_XferCallBackStatus(UI32_T *callback_status)
{
   *callback_status = CLI_MGR_xfer_callback_status;
   CLI_MGR_is_read_xfer_callback_status = TRUE;
}


#if (SYS_CPNT_3COM_CLI == TRUE)
void CLI_MGR_XferCopy_Callback(UI32_T cookie, UI32_T status)
{
//    XFER_MGR_FileCopyMgt_T  file_copy_mgt_info;
//    UI8_T       message_prefix[40] = {0};

    CLI_MGR_is_read_xfer_callback_status = FALSE;
    while(!CLI_MGR_is_read_xfer_callback_status)/*this while loop is because callback function very fast,*/
       SYSFUN_Sleep(1);                         /*cli can not read the correct value of xfer status,*/
                                                /*so when callback function call once, must wait cli read*/
                                                /*this value then set another status*/
    /* Initialize
     */
   switch(status)
   {
   case XFER_MGR_FILE_COPY_START_REBOOTING:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_START_REBOOTING;
        break;
   case XFER_MGR_FILE_COPY_TFTP_UNDEF_ERROR:
        //if (FALSE == XFER_MGR_GetTftpErrorMsg(tftp_error_msg))
        //{
        //    strcpy(tftp_error_msg, "TFTP status undefined.\r\n");
        //}
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_UNDEF_ERROR;
        break;

   case XFER_MGR_FILE_COPY_TFTP_FILE_NOT_FOUND:
        //strcpy(tftp_error_msg, "TFTP File not found");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_FILE_NOT_FOUND;
        break;

   case XFER_MGR_FILE_COPY_TFTP_ACCESS_VIOLATION:
        //strcpy(tftp_error_msg, "TFTP access violation");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_ACCESS_VIOLATION;
        break;

   case XFER_MGR_FILE_COPY_TFTP_DISK_FULL:
        //strcpy(tftp_error_msg, "TFTP Disk full");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_DISK_FULL;
        break;

   case XFER_MGR_FILE_COPY_TFTP_ILLEGAL_OPERATION:
        //strcpy(tftp_error_msg, "TFTP Illegal operation");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_ILLEGAL_OPERATION;
        break;

   case XFER_MGR_FILE_COPY_TFTP_UNKNOWN_TRANSFER_ID:
        //strcpy(tftp_error_msg, "TFTP Unknown trasfer id");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_UNKNOWN_TRANSFER_ID;
        break;

   case XFER_MGR_FILE_COPY_TFTP_FILE_EXISTED:
        //strcpy(tftp_error_msg, "TFTP File exists");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_FILE_EXISTED;
        break;

   case XFER_MGR_FILE_COPY_TFTP_NO_SUCH_USER:
        //strcpy(tftp_error_msg, "TFTP No such user");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_NO_SUCH_USER;
        break;

   case XFER_MGR_FILE_COPY_TFTP_TIMEOUT:
        //strcpy(tftp_error_msg, "TFTP Timeout");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_TIMEOUT;
        break;

   case XFER_MGR_FILE_COPY_TFTP_SEND_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Send error.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_SEND_ERROR;
        break;

   case XFER_MGR_FILE_COPY_TFTP_RECEIVE_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Receive error.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_RECEIVE_ERROR;
        break;

   case XFER_MGR_FILE_COPY_TFTP_SOCKET_OPEN_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Socket open error.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_SOCKET_OPEN_ERROR;
        break;

   case XFER_MGR_FILE_COPY_TFTP_SOCKET_BIND_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Socket bind error.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_SOCKET_BIND_ERROR;
        break;

   case XFER_MGR_FILE_COPY_TFTP_USER_CANCELED:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "User canceled operation.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_USER_CANCELED;
        break;

   case XFER_MGR_FILE_COPY_COMPLETED: /*The final message*/
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Upgrade process successfully completed.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_COMPLETED;
        CLI_MGR_Set_XferProgressStatus(FALSE);
        CLI_MGR_Is_PrintComplete = TRUE;
        break;

   case XFER_MGR_FILE_COPY_SET_STARTUP_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Set startup file error.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_SET_STARTUP_ERROR;
        break;

   case XFER_MGR_FILE_COPY_FILE_SIZE_EXCEED:
        //strcpy(tftp_error_msg, "File too large");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_FILE_SIZE_EXCEED;
        break;

   case XFER_MGR_FILE_COPY_MAGIC_WORD_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Failed to set to running-config word.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_MAGIC_WORD_ERROR;
        break;

   case XFER_MGR_FILE_COPY_SUCCESS:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, message_prefix);
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "successful.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_SUCCESS;
        CLI_MGR_Set_XferProgressResult(TRUE);
        CLI_MGR_Is_PrintComplete = TRUE;
        break;

   case XFER_MGR_FILE_COPY_ERROR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_ERROR;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, message_prefix);
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "failed because ");
        //if(strlen(tftp_error_msg) == 0)
        //{
        //    CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Incompatible image");
        //}
        //else
        //{
        //    CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, tftp_error_msg);
        //    memset(tftp_error_msg, 0, sizeof(tftp_error_msg));
        //}
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "\r\n");
        CLI_MGR_Set_XferProgressResult(FALSE);
        CLI_MGR_Is_PrintComplete = TRUE;
        break;

   case XFER_MGR_FILE_COPY_HEADER_CHECKSUM_ERROR:
        //strcpy(tftp_error_msg, "Header checksum error");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_HEADER_CHECKSUM_ERROR;
        break;

   case XFER_MGR_FILE_COPY_IMAGE_CHECKSUM_ERROR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_IMAGE_CHECKSUM_ERROR;
        //strcpy(tftp_error_msg, "Image checksum error");
      break;

   case XFER_MGR_FILE_COPY_IMAGE_TYPE_ERROR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_IMAGE_TYPE_ERROR;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Image type error.\r\n");
        break;

   case XFER_MGR_FILE_COPY_BUSY:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_BUSY;
        //strcpy(tftp_error_msg, "TFTP in progress");
        break;

   case XFER_MGR_FILE_COPY_PARA_ERROR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_PARA_ERROR;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Parameter error.\r\n");
        break;

   case XFER_MGR_FILE_COPY_TFTP_COMPLETED:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_COMPLETED;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "TFTP completed.\r\n");
        break;

   case XFER_MGR_FILE_COPY_WRITE_FLASH_FINISH:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_WRITE_FLASH_FINISH;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Write to FLASH finish.\r\n");
        break;

   case XFER_MGR_FILE_COPY_WRITE_FLASH_ERR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_WRITE_FLASH_ERR;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Write to FLASH error.\r\n");
        break;

   case XFER_MGR_FILE_COPY_WRITE_FLASH_PROGRAMMING:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_WRITE_FLASH_PROGRAMMING;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Write to FLASH Programming.\r\n");
        break;

   case XFER_MGR_FILE_COPY_READ_FILE_ERROR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_READ_FILE_ERROR;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Read file error.\r\n");
        break;

   case XFER_MGR_FILE_COPY_UNKNOWN:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_UNKNOWN;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Unknown error.\r\n");
        break;

   default:
        CLI_MGR_xfer_callback_status = 0xff;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Unknown error.\r\n");
        break;
   }
}

void CLI_MGR_XferCopy_Callback2(UI32_T cookie, UI32_T status)
{
   switch(status)
   {
   case XFER_MGR_FILE_COPY_TFTP_UNDEF_ERROR:
        //if (FALSE == XFER_MGR_GetTftpErrorMsg(tftp_error_msg))
        //{
        //    strcpy(tftp_error_msg, "TFTP status undefined.\r\n");
        //}
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_UNDEF_ERROR;
        break;

   case XFER_MGR_FILE_COPY_TFTP_FILE_NOT_FOUND:
        //strcpy(tftp_error_msg, "TFTP File not found");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_FILE_NOT_FOUND;
        break;

   case XFER_MGR_FILE_COPY_TFTP_ACCESS_VIOLATION:
        //strcpy(tftp_error_msg, "TFTP access violation");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_ACCESS_VIOLATION;
        break;

   case XFER_MGR_FILE_COPY_TFTP_DISK_FULL:
        //strcpy(tftp_error_msg, "TFTP Disk full");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_DISK_FULL;
        break;

   case XFER_MGR_FILE_COPY_TFTP_ILLEGAL_OPERATION:
        //strcpy(tftp_error_msg, "TFTP Illegal operation");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_ILLEGAL_OPERATION;
        break;

   case XFER_MGR_FILE_COPY_TFTP_UNKNOWN_TRANSFER_ID:
        //strcpy(tftp_error_msg, "TFTP Unknown trasfer id");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_UNKNOWN_TRANSFER_ID;
        break;

   case XFER_MGR_FILE_COPY_TFTP_FILE_EXISTED:
        //strcpy(tftp_error_msg, "TFTP File exists");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_FILE_EXISTED;
        break;

   case XFER_MGR_FILE_COPY_TFTP_NO_SUCH_USER:
        //strcpy(tftp_error_msg, "TFTP No such user");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_NO_SUCH_USER;
        break;

   case XFER_MGR_FILE_COPY_TFTP_TIMEOUT:
        //strcpy(tftp_error_msg, "TFTP Timeout");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_TIMEOUT;
        break;

   case XFER_MGR_FILE_COPY_TFTP_SEND_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Send error.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_SEND_ERROR;
        break;

   case XFER_MGR_FILE_COPY_TFTP_RECEIVE_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Receive error.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_RECEIVE_ERROR;
        break;

   case XFER_MGR_FILE_COPY_TFTP_SOCKET_OPEN_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Socket open error.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_SOCKET_OPEN_ERROR;
        break;

   case XFER_MGR_FILE_COPY_TFTP_SOCKET_BIND_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Socket bind error.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_SOCKET_BIND_ERROR;
        break;

   case XFER_MGR_FILE_COPY_TFTP_USER_CANCELED:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "User canceled operation.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_USER_CANCELED;
        break;

   case XFER_MGR_FILE_COPY_COMPLETED: /*The final message*/
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Upgrade process successfully completed.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_COMPLETED;
        CLI_MGR_Set_XferProgressStatus(FALSE);
        CLI_MGR_Is_PrintComplete = TRUE;
        break;

   case XFER_MGR_FILE_COPY_SET_STARTUP_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Set startup file error.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_SET_STARTUP_ERROR;
        break;

   case XFER_MGR_FILE_COPY_FILE_SIZE_EXCEED:
        //strcpy(tftp_error_msg, "File too large");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_FILE_SIZE_EXCEED;
        break;

   case XFER_MGR_FILE_COPY_MAGIC_WORD_ERROR:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Failed to set to running-config word.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_MAGIC_WORD_ERROR;
        break;

   case XFER_MGR_FILE_COPY_SUCCESS:
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, message_prefix);
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "successful.\r\n");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_SUCCESS;
        CLI_MGR_Set_XferProgressResult(TRUE);
        CLI_MGR_Is_PrintComplete = TRUE;
        break;

   case XFER_MGR_FILE_COPY_ERROR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_ERROR;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, message_prefix);
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "failed because ");
        //if(strlen(tftp_error_msg) == 0)
        //{
        //    CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Incompatible image");
        //}
        //else
        //{
        //    CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, tftp_error_msg);
        //    memset(tftp_error_msg, 0, sizeof(tftp_error_msg));
        //}
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "\r\n");
        CLI_MGR_Set_XferProgressResult(FALSE);
        CLI_MGR_Is_PrintComplete = TRUE;
        break;

   case XFER_MGR_FILE_COPY_HEADER_CHECKSUM_ERROR:
        //strcpy(tftp_error_msg, "Header checksum error");
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_HEADER_CHECKSUM_ERROR;
        break;

   case XFER_MGR_FILE_COPY_IMAGE_CHECKSUM_ERROR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_IMAGE_CHECKSUM_ERROR;
        //strcpy(tftp_error_msg, "Image checksum error");
      break;

   case XFER_MGR_FILE_COPY_IMAGE_TYPE_ERROR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_IMAGE_TYPE_ERROR;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Image type error.\r\n");
        break;

   case XFER_MGR_FILE_COPY_BUSY:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_BUSY;
        //strcpy(tftp_error_msg, "TFTP in progress");
        break;

   case XFER_MGR_FILE_COPY_PARA_ERROR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_PARA_ERROR;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Parameter error.\r\n");
        break;

   case XFER_MGR_FILE_COPY_TFTP_COMPLETED:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_TFTP_COMPLETED;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "TFTP completed.\r\n");
        break;

   case XFER_MGR_FILE_COPY_WRITE_FLASH_FINISH:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_WRITE_FLASH_FINISH;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Write to FLASH finish.\r\n");
        break;

   case XFER_MGR_FILE_COPY_WRITE_FLASH_ERR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_WRITE_FLASH_ERR;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Write to FLASH error.\r\n");
        break;

   case XFER_MGR_FILE_COPY_WRITE_FLASH_PROGRAMMING:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_WRITE_FLASH_PROGRAMMING;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Write to FLASH Programming.\r\n");
        break;

   case XFER_MGR_FILE_COPY_READ_FILE_ERROR:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_READ_FILE_ERROR;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Read file error.\r\n");
        break;

   case XFER_MGR_FILE_COPY_UNKNOWN:
        CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_UNKNOWN;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Unknown error.\r\n");
        break;

   default:
        CLI_MGR_xfer_callback_status = 0xff;
        //CLI_LIB_PrintStrForCallback((CLI_TASK_WorkingArea_T *)cookie, "Unknown error.\r\n");
        break;
   }
}

#else /* else of #if (SYS_CPNT_3COM_CLI == TRUE) */

void CLI_MGR_XferCopy_Callback(UI32_T cookie, UI32_T status)
{
    CLI_MGR_is_read_xfer_callback_status = FALSE;
    while(!CLI_MGR_is_read_xfer_callback_status)/*this while loop is because callback function very fast,*/
       SYSFUN_Sleep(1);                         /*cli can not read the correct value of xfer status,*/
                                                /*so when callback function call once, must wait cli read*/
                                                /*this value then set another status*/

    switch(status)
    {
        case XFER_MGR_FILE_COPY_COMPLETED: /*The final message*/
            CLI_MGR_Set_XferProgressStatus(FALSE);
            CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_COMPLETED;
            break;

        case XFER_MGR_FILE_COPY_SUCCESS:
            CLI_MGR_Set_XferProgressResult(TRUE);
            CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_SUCCESS;
            break;

        case XFER_MGR_FILE_COPY_ERROR:
            CLI_MGR_Set_XferProgressResult(FALSE);
            CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_ERROR;
            break;

        case XFER_MGR_FILE_COPY_WRITE_LOADER_TO_FLASH:
            CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_WRITE_LOADER_TO_FLASH;
            break;

        case XFER_MGR_FILE_COPY_FILE_NOT_FOUND:
        case XFER_MGR_FILE_COPY_SERVER_PERMISSION_DENIED:
        case XFER_MGR_FILE_COPY_STORAGE_FULL:
        case XFER_MGR_FILE_COPY_TFTP_ILLEGAL_OPERATION:
        case XFER_MGR_FILE_COPY_TFTP_UNKNOWN_TRANSFER_ID:
        case XFER_MGR_FILE_COPY_TFTP_FILE_EXISTED:
        case XFER_MGR_FILE_COPY_TFTP_NO_SUCH_USER:
        case XFER_MGR_FILE_COPY_TIMEOUT:
        case XFER_MGR_FILE_COPY_TFTP_SEND_ERROR:
        case XFER_MGR_FILE_COPY_TFTP_RECEIVE_ERROR:
        case XFER_MGR_FILE_COPY_TFTP_SOCKET_OPEN_ERROR:
        case XFER_MGR_FILE_COPY_TFTP_SOCKET_BIND_ERROR:
        case XFER_MGR_FILE_COPY_TFTP_USER_CANCELED:
        case XFER_MGR_FILE_COPY_SET_STARTUP_ERROR:
        case XFER_MGR_FILE_COPY_FILE_SIZE_EXCEED:
        case XFER_MGR_FILE_COPY_MAGIC_WORD_ERROR:
        case XFER_MGR_FILE_COPY_HEADER_CHECKSUM_ERROR:
        case XFER_MGR_FILE_COPY_IMAGE_CHECKSUM_ERROR:
        case XFER_MGR_FILE_COPY_IMAGE_TYPE_ERROR:
        case XFER_MGR_FILE_COPY_BUSY:
        case XFER_MGR_FILE_COPY_PARA_ERROR:
        case XFER_MGR_FILE_COPY_WRITE_FLASH_FINISH:
        case XFER_MGR_FILE_COPY_WRITE_FLASH_ERR:
        case XFER_MGR_FILE_COPY_WRITE_FLASH_PROGRAMMING:
        case XFER_MGR_FILE_COPY_READ_FILE_ERROR:
        case XFER_MGR_FILE_COPY_UNKNOWN:
        case XFER_MGR_FILE_COPY_CONNECT_ERROR:
        case XFER_MGR_FILE_COPY_SERVER_NOT_ACCEPT_PROVIDED_CIPHERS:
        case XFER_MGR_FILE_COPY_SERVER_NOT_SUPPORT_FTPS:
        case XFER_MGR_FILE_COPY_DATA_CONNECTION_OPEN_ERROR:
        case XFER_MGR_FILE_COPY_SERVER_NOT_IN_SERVICE:
        case XFER_MGR_FILE_COPY_LOG_IN_ERROR:
        case XFER_MGR_FILE_COPY_FILE_UNAVAILABLE:
        case XFER_MGR_FILE_COPY_INVALID_FILE_NAME:
        case XFER_MGR_FILE_COPY_UNCLASSIFIED_ERROR:
        case XFER_MGR_FILE_COPY_PROJECT_ID_ERROR:
        case XFER_MGR_FILE_COPY_PROGRESS_UNIT1:
        case XFER_MGR_FILE_COPY_PROGRESS_UNIT2:
        case XFER_MGR_FILE_COPY_PROGRESS_UNIT3:
        case XFER_MGR_FILE_COPY_PROGRESS_UNIT4:
        case XFER_MGR_FILE_COPY_PROGRESS_UNIT5:
        case XFER_MGR_FILE_COPY_PROGRESS_UNIT6:
        case XFER_MGR_FILE_COPY_PROGRESS_UNIT7:
        case XFER_MGR_FILE_COPY_PROGRESS_UNIT8:
        case XFER_MGR_FILE_COPY_FILE_NUM_EXCEED:
        case XFER_MGR_FILE_COPY_SAME_VERSION:
            CLI_MGR_xfer_callback_status = status;
            break;

        default:
            CLI_MGR_xfer_callback_status = XFER_MGR_FILE_COPY_UNCLASSIFIED_ERROR;
            break;
    }
}

#endif /* end of else #if (SYS_CPNT_3COM_CLI == TRUE) */

BOOL_T CLI_MGR_SaveRunningCfg(char *filename)
{
   UI32_T function_no;
   function_no = CLI_MGR_SAVERUNNINGCFG;
   CLI_USE_CSC_CHECK_OPER_MODE(FALSE, function_no);
   if(strlen(filename) == 0 || strlen(filename)>SYS_ADPT_FILE_SYSTEM_NAME_LEN)
   {
#if (SYS_CPNT_EH == TRUE)
      EH_PMGR_Handle_Exception1(SYS_MODULE_CLI, function_no, EH_TYPE_MSG_INVALID_PARAMETER, SYSLOG_LEVEL_INFO, "file name");
#endif
      CLI_RETURN_AND_RELEASE_CSC(FALSE);
   }

   /*action*/
#if (SYS_CPNT_DBSYNC_TXT ==TRUE)
   time_ticks_b = SYS_TIME_GetSystemTicksBy10ms();
#endif

   if(!XFER_PMGR_StreamToLocal((UI8_T *)filename,
                              FS_FILE_TYPE_CONFIG,
                              0,
                              SYS_BLD_CLI_GROUP_IPCMSGQ_KEY,
                              0))
   {
      CLI_RETURN_AND_RELEASE_CSC(FALSE);
   }

   /* XFER_BUF_MGR_Free(CLI_MGR_WebAllocXFerPtr); */
      CLI_RETURN_AND_RELEASE_CSC(TRUE);

}
#if 0 /* rich */
static void CLI_MGR_WebSaveRunningCfg_Callback(UI32_T cookie, UI32_T status)
{
#if (SYS_CPNT_DBSYNC_TXT ==TRUE)
   if(status == XFER_MGR_FILE_COPY_COMPLETED)
   {
      DBSYNC_TXT_PMGR_Set_IsDoingAutosave(FALSE);
      time_ticks_a = SYS_TIME_GetSystemTicksBy10ms();
      //printf("\r\nautosave time: %lu(secs),%lu(ticks).\r\n",(time_ticks_a-time_ticks_b)/100,time_ticks_a-time_ticks_b);
   }

   if(!(status ==XFER_MGR_FILE_COPY_TFTP_COMPLETED||
      status ==XFER_MGR_FILE_COPY_WRITE_FLASH_FINISH||
      status ==XFER_MGR_FILE_COPY_WRITE_FLASH_PROGRAMMING||
      status ==XFER_MGR_FILE_COPY_SUCCESS||
      status ==XFER_MGR_FILE_COPY_COMPLETED))
   {
      DBSYNC_TXT_PMGR_SetDirty(TRUE);
   }

#endif

   if(cookie == 0 && status == XFER_MGR_COMPLETED)
   {
      XFER_BUF_PMGR_Free(CLI_MGR_WebAllocXFerPtr);
   }
}
#endif /* rich */
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
UI32_T CLI_MGR_Get_RunningCfg(void *buffer, UI32_T buffer_size)
{

    UI32_T  unit = 0;
   UI32_T function_no;
   function_no = CLI_MGR_GET_RUNNINGCFG;
   CLI_USE_CSC_CHECK_OPER_MODE(FALSE, function_no);

#if (SYS_CPNT_3COM_CLI_PROVISION == TRUE)
   if (CLI_RUNCFG_3COM_Get_RunningCfg(buffer, buffer_size) == CLI_MGR_RUNCFG_RETURN_NO_ENOUGH_MEMORY)
#else
   if (CLI_RUNCFG_Get_RunningCfg(buffer, buffer_size) == CLI_MGR_RUNCFG_RETURN_NO_ENOUGH_MEMORY)
#endif
   {
#if (SYS_CPNT_EH == TRUE)
      EH_PMGR_Handle_Exception(SYS_MODULE_CLI, function_no, EH_TYPE_MSG_MEM_ALLOCATE, SYSLOG_LEVEL_INFO);
#endif
      CLI_RETURN_AND_RELEASE_CSC(CLI_MGR_RUNCFG_RETURN_NO_ENOUGH_MEMORY);
   }

#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
    for ( unit = 0; unit < CLI_MGR_NUMBER_OF_TOTAL_DEVICE; unit ++ )
    {
        if ( strlen( device_cfg[ unit ] ) > 0 )
        {
            strcat( (char *)buffer, device_cfg[ unit ] );
        }
    }
#else
    for (unit = 0; unit < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; unit++)
    {
        if (strlen(module_cfg[unit]) > 0)
        {
            strcat((char *)buffer, module_cfg[unit]);
        }
    }
#endif

   CLI_RETURN_AND_RELEASE_CSC(CLI_MGR_RUNCFG_RETURN_OK);

}


BOOL_T CLI_MGR_SetKillWorkingSpaceFlag(UI32_T sessid)
{
   UI32_T function_no;
   function_no = CLI_MGR_SETKILLWORKINGSPACEFLAG;
   CLI_USE_CSC_CHECK_OPER_MODE(FALSE, function_no);

   if (CLI_TASK_SetKillWorkingSpaceFlag(sessid) == FALSE)
   {
      CLI_RETURN_AND_RELEASE_CSC(FALSE);
   }

   CLI_RETURN_AND_RELEASE_CSC(TRUE);
}


/*

*/

/*-----------------------------------------------------------
 * ROUTINE NAME -  CLI_MGR_Set_Config_File_Flag
 *-----------------------------------------------------------
 * FUNCTION: Set the flag, if CLI's config is valid or not
 * INPUT   : is_ok. TRUE/FALSE
 * OUTPUT  : none
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Set_Config_File_Flag(BOOL_T is_ok)
{
   is_config_file_ok = is_ok;
}

/*-----------------------------------------------------------
 * ROUTINE NAME -  CLI_MGR_Get_Config_File_Flag
 *-----------------------------------------------------------
 * FUNCTION: Get the flag that judge if CLI's config is valid or not
 * INPUT   : None.
 * OUTPUT  : TRUE/FALSE
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------*/
void CLI_MGR_Get_Config_File_Flag(BOOL_T *is_ok)
{
   *is_ok = is_config_file_ok;
}

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
BOOL_T CLI_MGR_Add_ProvisionCMDCounter()
{
    provision_error_command_count++;
    return TRUE;
}
/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Clear_ProvisionCMDCounter
 *-----------------------------------------------------------
 * FUNCTION: Copy factory default config file to startup config file
 *           but keep ip address setting.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE.
 * NOTE    : None.
 *----------------------------------------------------------*/
BOOL_T CLI_MGR_Clear_ProvisionCMDCounter()
{
    provision_error_command_count = 0;
    return TRUE;
}
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
BOOL_T CLI_MGR_Get_ProvisionCMDCounter(UI32_T *error_command_count)
{
    *error_command_count = provision_error_command_count;
    return TRUE;
}

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
void CLI_MGR_Register_ModuleProvisionComplete_CallBack (void (*fun)(void))
{
    SYS_TYPE_REGISTER_CALLBACKFUN(module_provision_complete_callback);
}

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
void CLI_MGR_Notify_ModuleProvisionComplete(UI32_T unit)
{
#if 0
    SYS_TYPE_CallBack_T  *fun_list;

    for(fun_list=module_provision_complete_callback; fun_list; fun_list=fun_list->next)
        fun_list->func();
#else
/* call SYS_MGR for process callback handler
 */
 #if 1 /* rich */ /*add by fen.wang*/
    SYS_CALLBACK_MGR_ModuleProvisionCompleteCallback(SYS_MODULE_CLI,unit);
    #endif /* rich */
#endif

}

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
void CLI_MGR_HandleHotInsertion( UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default )
{
#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
    UI32_T  unit = 0;
    UI32_T  port = 0;
    UI32_T  master_id = 0;

    STKTPLG_POM_GetMasterUnitId( (UI8_T *)&master_id );

    CLI_MGR_CheckModuleVersion( );

    unit = CLI_MGR_IFINDEX_TO_UNIT( starting_port_ifindex );
    port = CLI_MGR_IFINDEX_TO_PORT( starting_port_ifindex );

    if ( FALSE == use_default )
    {
        if ( STKTPLG_POM_IsModulePort( unit, port ) == TRUE )
        {
            CLI_MGR_Set_Provision_Flag( TRUE );
            CLI_TASK_CreateTask(    CLI_TYPE_MODULE_PROVISION,
                                    0,
                                    unit,
                                    device_cfg[ unit - 1 + SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ],
                                    strlen( device_cfg[ unit - 1 + SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ] ) );
        }
        else if ( unit != master_id )
        {
            CLI_MGR_Set_Provision_Flag( TRUE );
            CLI_TASK_CreateTask(    CLI_TYPE_MODULE_PROVISION,
                                    0,
                                    unit,
                                    device_cfg[ unit - 1 ],
                                    strlen( device_cfg[ unit - 1 ] ) );
        }
        else
        {
            ;
            /* do nothing */
        }
    }
    else
    {
        if ( STKTPLG_POM_IsModulePort( unit, port ) == TRUE )
        {
            memset( device_cfg[ unit - 1 + SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ], 0, sizeof( device_cfg[ unit - 1 + SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ] ) );
/*add by fen.wang,sometimes stktplg cannot assign right unit id,because the unit id from cli is not correct for when do  hot add ,cli will not save the topo
when hotswap add ,save the topo info in flash*/
#if 0
            CLI_MGR_Notify_ModuleProvisionComplete( );
#else
            {
                        BOOL_T is_provision;
                        BOOL_T is_config_file_ok;
                        char   file_name[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1];

                        CLI_MGR_Get_Provision_Flag(&is_provision);
                        CLI_MGR_Get_Config_File_Flag(&is_config_file_ok);

                        if(is_provision != TRUE)
                        {
                            //module provision complete.
                            CLI_MGR_Notify_ModuleProvisionComplete(unit);


                            if (is_config_file_ok == TRUE)
                            {
                                SYSFUN_Debug_Printf("\r\nUnit %ld CLI provision has been completed\r\n",unit);
                            }
                            /*save the stacking config*/

                           if(CLI_TASK_Reproduce_Config(file_name))
                           {
#if (TRUE == SYS_CPNT_3COM_CLI)
                               CLI_MGR_SetIsReproduceDone(TRUE);
#endif

                           }

                        }
                    }
#endif
        }
        else if ( unit != master_id )
        {
            memset(device_cfg[unit - 1], 0, sizeof(device_cfg[unit - 1]));
            /*add by fen.wang,sometimes stktplg cannot assign right unit id,because the unit id from cli is not correct for when do  hot add ,cli will not save the topo
            when hotswap add ,save the topo info in flash*/
#if 0
            CLI_MGR_Notify_ModuleProvisionComplete( );
#else
            {
                        BOOL_T is_provision;
                        BOOL_T is_config_file_ok;
                        char   file_name[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1];

                        CLI_MGR_Get_Provision_Flag(&is_provision);
                        CLI_MGR_Get_Config_File_Flag(&is_config_file_ok);

                        if(is_provision != TRUE)
                        {
                            //module provision complete.
                             CLI_MGR_Notify_ModuleProvisionComplete(unit);


                            if (is_config_file_ok == TRUE)
                            {
                                SYSFUN_Debug_Printf("\r\nUnit %ld CLI provision has been completed\r\n",unit);
                            }
                            /*save the stacking config*/

                           if(CLI_TASK_Reproduce_Config(file_name))
                           {
#if (TRUE == SYS_CPNT_3COM_CLI)
                               CLI_MGR_SetIsReproduceDone(TRUE);
#endif

                           }

                        }
                    }
#endif
        }
        else
        {
            ;
            /* do nothing */
        }
    }
    return;
#else
    UI32_T  unit = 0;

    CLI_MGR_CheckModuleVersion();

    unit = (starting_port_ifindex - 1)/SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT + 1;

    if (FALSE == use_default)
    {
        CLI_MGR_Set_Provision_Flag(TRUE);
        CLI_TASK_CreateTask( CLI_TYPE_MODULE_PROVISION,
                             NULL,
                             unit,
                             module_cfg[unit - 1],
                             strlen(module_cfg[unit - 1]));
    }
    else
    {
        memset(module_cfg[unit - 1], 0, sizeof(module_cfg[unit - 1]));
        CLI_MGR_Notify_ModuleProvisionComplete(unit);
    }
    return;
#endif
}

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
void CLI_MGR_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
#if 1 /* rich */

#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
    UI32_T  unit = 0;
    UI32_T  port = 0;
    UI32_T  master_id = 0;

    printf( "CLI hot remove started!\r\n" );
    STKTPLG_POM_GetMasterUnitId( (UI8_T *)&master_id );
    unit = CLI_MGR_IFINDEX_TO_UNIT( starting_port_ifindex );
    port = CLI_MGR_IFINDEX_TO_PORT( starting_port_ifindex );

    if ( STKTPLG_POM_IsModulePort( unit, port ) == TRUE )
    {
        memset( device_cfg[unit - 1 + SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK], 0, CLI_MGR_RUNTIME_PROVISION_UC_MEM_SIZE );
        CLI_RUNCFG_Get_DeviceRunningCfg( starting_port_ifindex, number_of_port, device_cfg[unit - 1 + SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK], CLI_MGR_RUNTIME_PROVISION_UC_MEM_SIZE );
    }
    else if ( master_id != unit )
    {
        memset( device_cfg[unit - 1], 0, CLI_MGR_RUNTIME_PROVISION_UC_MEM_SIZE);
        CLI_RUNCFG_Get_DeviceRunningCfg( starting_port_ifindex, number_of_port, device_cfg[unit - 1], CLI_MGR_RUNTIME_PROVISION_UC_MEM_SIZE );
    }
    else
    {
        ;
        /* do nothing */
    }

#if 0
    for ( unit_axis = 0; unit_axis < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK * 2; unit_axis ++ )
    {
        printf( "Unit %ld:\r\n", unit_axis + 1 );
        printf( "%s\r\n", device_cfg[ unit_axis ] );
    }
#endif
    return;
#else
    UI32_T  unit = 0;

    unit = (starting_port_ifindex - 1)/SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT + 1;

    memset(module_cfg[unit - 1], 0, CLI_MGR_RUNTIME_PROVISION_UC_MEM_SIZE);

    CLI_RUNCFG_Get_ModuleRunningCfg(starting_port_ifindex, number_of_port, module_cfg[unit - 1], CLI_MGR_RUNTIME_PROVISION_UC_MEM_SIZE);

    return;
#endif
#endif /* rich */
}
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
void CLI_MGR_AppendModuleCfg(UI32_T cmd_index, CLI_STRLB_CMD_T *cmd, UI8_T *pre_attr, UI8_T *post_attr)
{

    char    cmd_string[100] = {0};
    UI32_T  unit = 0;

    for (unit = 0; unit < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; unit++)
    {
        if (strlen(cmd[unit].unit_port_string) > 0)
        {
            memset(cmd_string, 0, sizeof(cmd_string));
#if 0
            CLI_RUNCFG_ConstructCmd(cmd_index, cmd[unit].unit_port_string, pre_attr, post_attr, cmd_string);
#endif
            strcat(module_cfg[unit], cmd_string);
        }
    }
}
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
BOOL_T CLI_MGR_IsModulePort(UI32_T ifindex)
{
    UI32_T              unit = 0;
    UI32_T              port = 0;
    UI32_T              start_port_id = 0;
    UI32_T              nbr_of_ports = 0;

    unit = ( ((UI32_T)((ifindex-1)/SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT))+1 );
    port = ( ifindex - (unit-1)*SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT );

    if(!STKTPLG_POM_GetUnitMainboardPorts(unit, &start_port_id, &nbr_of_ports))
    {
        return FALSE;
    }

    if((port>=start_port_id)&&(port<=(start_port_id+nbr_of_ports-1)))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

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
BOOL_T CLI_MGR_IncreaseRemoteSession(void)
{
    UI32_T function_no;
    UI32_T orig_priority;

    function_no = CLI_MGR_INCREASEREMOTESESSION;
    CLI_USE_CSC_CHECK_OPER_MODE(FALSE, function_no);
    CLI_MGR_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id);
    if(CLI_MGR_current_session_count>=SYS_ADPT_MAX_TELNET_NUM)
    {
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        CLI_RETURN_AND_RELEASE_CSC(FALSE);
    }
    else
    {
        CLI_MGR_current_session_count++;
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        CLI_RETURN_AND_RELEASE_CSC(TRUE);
    }

}

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
BOOL_T CLI_MGR_DecreaseRemoteSession(void)
{
    UI32_T function_no;
    UI32_T orig_priority;

    function_no = CLI_MGR_DECREASEREMOTESESSION;
    CLI_USE_CSC_CHECK_OPER_MODE(FALSE, function_no);
    CLI_MGR_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id);

    if(CLI_MGR_current_session_count>SYS_ADPT_MAX_TELNET_NUM||CLI_MGR_current_session_count==0)
    {
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        CLI_RETURN_AND_RELEASE_CSC(FALSE);
    }
    else
    {
        CLI_MGR_current_session_count--;
        if(CLI_MGR_current_session_count<0)
        {
            CLI_MGR_current_session_count = 0;
        }
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        CLI_RETURN_AND_RELEASE_CSC(TRUE);
    }
}

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
BOOL_T CLI_MGR_CheckModuleVersionSession(void)
{
#if 0 /* rich linux */
//    BOOL_T is_stack_status_normal;
    STKTPLG_MGR_Switch_Info_T   switch_info;
    STKTPLG_MGR_switchModuleInfoEntry_T module_info;
    UI32_T unit_id = 0;

#if (SYS_CPNT_3COM_CLI == TRUE)
    UI32_T version1=0,version2=0,version3=0,version4=0;
#endif

    UI8_T  main_board_version[50]={0};
    UI8_T  module_version[50]={0};
    UI8_T  expected_module_version[50]={0};
    UI8_T  master_unit_id=0;
    UI32_T ext_unit_id = 0;

    memset(&switch_info,0,sizeof(STKTPLG_MGR_Switch_Info_T));
    memset(&module_info,0,sizeof(STKTPLG_MGR_switchModuleInfoEntry_T));

    if(!STKTPLG_PMGR_GetMasterUnitId(&master_unit_id))
    {
        CLI_LIB_PrintStr("Failed to get master unit number.");
        return FALSE;
    }

    switch_info.sw_unit_index = master_unit_id;
    if (STKTPLG_PMGR_GetSwitchInfo(&switch_info) != TRUE)
    {
        CLI_LIB_PrintStr_1("Failed to get unit %d switch info.",master_unit_id);
        return FALSE;
    }

#if (SYS_CPNT_3COM_CLI == TRUE)
    memset(&version1,0,sizeof(UI32_T));
    memset(&version2,0,sizeof(UI32_T));
    memset(&version3,0,sizeof(UI32_T));
    memset(&version4,0,sizeof(UI32_T));
    SYSFUN_Sscanf(switch_info.sw_opcode_ver,"%lu.%lu.%lu.%lu",&version1,&version2,&version3,&version4);
    SYSFUN_Sprintf(main_board_version,"%lu.%02lu.%02lu.%02lu",version1,version2,version3,version4);
#else
    strcpy(main_board_version,switch_info.sw_opcode_ver);
#endif

#if (SYS_CPNT_3COM_CLI == TRUE)
    memset(&version1,0,sizeof(UI32_T));
    memset(&version2,0,sizeof(UI32_T));
    memset(&version3,0,sizeof(UI32_T));
    memset(&version4,0,sizeof(UI32_T));
    SYSFUN_Sscanf(switch_info.sw_module_expected_opcode_ver,"%lu.%lu.%lu.%lu",&version1,&version2,&version3,&version4);
    SYSFUN_Sprintf(expected_module_version,"%lu.%02lu.%02lu.%02lu",version1,version2,version3,version4);
#else
    strcpy(expected_module_version,switch_info.sw_module_expected_opcode_ver);
#endif

    //if(STKTPLG_PMGR_GetStackStatus(&is_stack_status_normal))
    //{
        //if(!is_stack_status_normal)
        if(!CLI_MGR_CheckStkVersionValidate())
        {
            CLI_LIB_PrintStr("\r\n");
            CLI_LIB_PrintStr_1("Current firmware version of main board: %s\r\n",main_board_version);
            CLI_LIB_PrintStr_1("Expected firmware version of expansion module: %s\r\n",expected_module_version);
            CLI_LIB_PrintStr("\r\n");
            CLI_LIB_PrintStr("The following unit(s) and expansion module(s) are found\r\n");
            CLI_LIB_PrintStr("to have a version of firmware which is different from that of the master\r\n");
            CLI_LIB_PrintStr("unit. Please upgrade the firmware using TFTP.\r\n");
            CLI_LIB_PrintStr("\r\n");

            module_info.unit_index = master_unit_id;
            module_info.module_index = 1;
            STKTPLG_PMGR_GetSwitchModuleInfoEntry(&module_info);

#if (SYS_CPNT_3COM_CLI == TRUE)
            memset(&version1,0,sizeof(UI32_T));
            memset(&version2,0,sizeof(UI32_T));
            memset(&version3,0,sizeof(UI32_T));
            memset(&version4,0,sizeof(UI32_T));
            SYSFUN_Sscanf(module_info.op_code_ver,"%lu.%lu.%lu.%lu",&version1,&version2,&version3,&version4);
            SYSFUN_Sprintf(module_version,"%lu.%02lu.%02lu.%02lu",version1,version2,version3,version4);
#else
            strcpy(module_version,module_info.op_code_ver);
#endif

            if( STKTPLG_PMGR_OptionModuleIsExist(master_unit_id,&ext_unit_id) && module_info.module_validation == STKTPLG_MGR_MODULE_INVALID )
            {
                CLI_LIB_PrintStr_2("Unit %d, expansion module 1, version %s\r\n",master_unit_id,module_version);
            }

            while(STKTPLG_POM_GetNextUnit(&unit_id))
            {
                if(unit_id == master_unit_id)
                    continue;
                switch_info.sw_unit_index = unit_id;
                if (STKTPLG_PMGR_GetSwitchInfo(&switch_info) != TRUE)
                {
                    CLI_LIB_PrintStr_1("Failed to get unit %lu switch info.",unit_id);
                    return FALSE;
                }

#if (SYS_CPNT_3COM_CLI == TRUE)
                memset(&version1,0,sizeof(UI32_T));
                memset(&version2,0,sizeof(UI32_T));
                memset(&version3,0,sizeof(UI32_T));
                memset(&version4,0,sizeof(UI32_T));
                SYSFUN_Sscanf(switch_info.sw_opcode_ver,"%lu.%lu.%lu.%lu",&version1,&version2,&version3,&version4);
                SYSFUN_Sprintf(main_board_version,"%lu.%02lu.%02lu.%02lu",version1,version2,version3,version4);
#else
                strcpy(main_board_version,switch_info.sw_opcode_ver);
#endif

                module_info.unit_index = unit_id;
                module_info.module_index = 1;
                STKTPLG_PMGR_GetSwitchModuleInfoEntry(&module_info);

#if (SYS_CPNT_3COM_CLI == TRUE)
                memset(&version1,0,sizeof(UI32_T));
                memset(&version2,0,sizeof(UI32_T));
                memset(&version3,0,sizeof(UI32_T));
                memset(&version4,0,sizeof(UI32_T));
                SYSFUN_Sscanf(module_info.op_code_ver,"%lu.%lu.%lu.%lu",&version1,&version2,&version3,&version4);
                SYSFUN_Sprintf(module_version,"%lu.%02lu.%02lu.%02lu",version1,version2,version3,version4);
#else
                strcpy(module_version,module_info.op_code_ver);
#endif

                if(switch_info.sw_validation == STKTPLG_MGR_SWITCH_INVALID)
                {
                    CLI_LIB_PrintStr_2("Unit %lu, main board, version %s\r\n",unit_id,main_board_version);
                }

                if( STKTPLG_PMGR_OptionModuleIsExist(unit_id,&ext_unit_id) && module_info.module_validation == STKTPLG_MGR_MODULE_INVALID )
                {
                    CLI_LIB_PrintStr_2("Unit %lu, expansion module 1, version %s\r\n",unit_id,module_version);
                }
            }
            CLI_LIB_PrintStr("\r\n");
        }
    //}
#endif
    return TRUE;
}

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
BOOL_T CLI_MGR_CheckModuleVersion(void)
{
#if 0 /* rich linux */
//    BOOL_T is_stack_status_normal;
    STKTPLG_MGR_Switch_Info_T   switch_info;
    STKTPLG_MGR_switchModuleInfoEntry_T module_info;
    UI32_T unit_id = 0;

#if (SYS_CPNT_3COM_CLI == TRUE)
    UI32_T version1=0,version2=0,version3=0,version4=0;
#endif

    UI8_T  main_board_version[50]={0};
    UI8_T  module_version[50]={0};
    UI8_T  expected_module_version[50]={0};
    UI8_T  master_unit_id=0;
    UI32_T ext_unit_id = 0;

    memset(&switch_info,0,sizeof(STKTPLG_MGR_Switch_Info_T));
    memset(&module_info,0,sizeof(STKTPLG_MGR_switchModuleInfoEntry_T));

    if(!STKTPLG_PMGR_GetMasterUnitId(&master_unit_id))
    {
        SYSFUN_Debug_Printf("Failed to get master unit number.");
        return FALSE;
    }

    switch_info.sw_unit_index = master_unit_id;
    if (STKTPLG_PMGR_GetSwitchInfo(&switch_info) != TRUE)
    {
        SYSFUN_Debug_Printf("Failed to get unit %d switch info.",master_unit_id);
        return FALSE;
    }

#if (SYS_CPNT_3COM_CLI == TRUE)
    memset(&version1,0,sizeof(UI32_T));
    memset(&version2,0,sizeof(UI32_T));
    memset(&version3,0,sizeof(UI32_T));
    memset(&version4,0,sizeof(UI32_T));
    SYSFUN_Sscanf(switch_info.sw_opcode_ver,"%lu.%lu.%lu.%lu",&version1,&version2,&version3,&version4);
    SYSFUN_Sprintf(main_board_version,"%lu.%02lu.%02lu.%02lu",version1,version2,version3,version4);
#else
    strcpy(main_board_version,switch_info.sw_opcode_ver);
#endif

#if (SYS_CPNT_3COM_CLI == TRUE)
    memset(&version1,0,sizeof(UI32_T));
    memset(&version2,0,sizeof(UI32_T));
    memset(&version3,0,sizeof(UI32_T));
    memset(&version4,0,sizeof(UI32_T));
    SYSFUN_Sscanf(switch_info.sw_module_expected_opcode_ver,"%lu.%lu.%lu.%lu",&version1,&version2,&version3,&version4);
    SYSFUN_Sprintf(expected_module_version,"%lu.%02lu.%02lu.%02lu",version1,version2,version3,version4);
#else
    strcpy(expected_module_version,switch_info.sw_module_expected_opcode_ver);
#endif

    //if(STKTPLG_PMGR_GetStackStatus(&is_stack_status_normal))
    //{
        //if(!is_stack_status_normal)
        if(!CLI_MGR_CheckStkVersionValidate())
        {
            SYSFUN_Debug_Printf("\r\n");
            SYSFUN_Debug_Printf("Current firmware version of main board: %s\r\n",main_board_version);
            SYSFUN_Debug_Printf("Expected firmware version of expansion module: %s\r\n",expected_module_version);
            SYSFUN_Debug_Printf("\r\n");
            SYSFUN_Debug_Printf("The following unit(s) and expansion module(s) are found\r\n");
            SYSFUN_Debug_Printf("to have a version of firmware which is different from that of the master\r\n");
            SYSFUN_Debug_Printf("unit. Please upgrade the firmware using TFTP.\r\n");
            SYSFUN_Debug_Printf("\r\n");

            module_info.unit_index = master_unit_id;
            module_info.module_index = 1;
            STKTPLG_PMGR_GetSwitchModuleInfoEntry(&module_info);

#if (SYS_CPNT_3COM_CLI == TRUE)
            memset(&version1,0,sizeof(UI32_T));
            memset(&version2,0,sizeof(UI32_T));
            memset(&version3,0,sizeof(UI32_T));
            memset(&version4,0,sizeof(UI32_T));
            SYSFUN_Sscanf(module_info.op_code_ver,"%lu.%lu.%lu.%lu",&version1,&version2,&version3,&version4);
            SYSFUN_Sprintf(module_version,"%lu.%02lu.%02lu.%02lu",version1,version2,version3,version4);
#else
            strcpy(module_version,module_info.op_code_ver);
#endif

            if( STKTPLG_PMGR_OptionModuleIsExist(master_unit_id,&ext_unit_id) && module_info.module_validation == STKTPLG_MGR_MODULE_INVALID )
            {
                if (module_info.module_validation == STKTPLG_MGR_MODULE_INVALID)
                {
                    SYSFUN_Debug_Printf("Unit %d, expansion module 1, version %s\r\n",master_unit_id,module_version);
                    #if 0 /* rich */
                    LED_PMGR_SetModuleLED(master_unit_id, LEDDRV_COLOR_AMBERFLASH);
                    #endif
                }
                else
                {
                    #if 0 /* rich  */
                    LED_PMGR_SetModuleLED(master_unit_id, LEDDRV_COLOR_GREEN);
                    #endif
                }
            }

            while(STKTPLG_POM_GetNextUnit(&unit_id))
            {
                if(unit_id == master_unit_id)
                    continue;
                switch_info.sw_unit_index = unit_id;
                if (STKTPLG_PMGR_GetSwitchInfo(&switch_info) != TRUE)
                {
                    SYSFUN_Debug_Printf("Failed to get unit %lu switch info.",unit_id);
                    return FALSE;
                }

#if (SYS_CPNT_3COM_CLI == TRUE)
                memset(&version1,0,sizeof(UI32_T));
                memset(&version2,0,sizeof(UI32_T));
                memset(&version3,0,sizeof(UI32_T));
                memset(&version4,0,sizeof(UI32_T));
                SYSFUN_Sscanf(switch_info.sw_opcode_ver,"%lu.%lu.%lu.%lu",&version1,&version2,&version3,&version4);
                SYSFUN_Sprintf(main_board_version,"%lu.%02lu.%02lu.%02lu",version1,version2,version3,version4);
#else
                strcpy(main_board_version,switch_info.sw_opcode_ver);
#endif

                module_info.unit_index = unit_id;
                module_info.module_index = 1;
                STKTPLG_PMGR_GetSwitchModuleInfoEntry(&module_info);

#if (SYS_CPNT_3COM_CLI == TRUE)
                memset(&version1,0,sizeof(UI32_T));
                memset(&version2,0,sizeof(UI32_T));
                memset(&version3,0,sizeof(UI32_T));
                memset(&version4,0,sizeof(UI32_T));
                SYSFUN_Sscanf(module_info.op_code_ver,"%lu.%lu.%lu.%lu",&version1,&version2,&version3,&version4);
                SYSFUN_Sprintf(module_version,"%lu.%02lu.%02lu.%02lu",version1,version2,version3,version4);
#else
                strcpy(module_version,module_info.op_code_ver);
#endif
                if(switch_info.sw_validation == STKTPLG_MGR_SWITCH_INVALID)
                {
                    SYSFUN_Debug_Printf("Unit %lu, main board, version %s\r\n",unit_id,main_board_version);
                }

                if( STKTPLG_PMGR_OptionModuleIsExist(unit_id,&ext_unit_id) && module_info.module_validation == STKTPLG_MGR_MODULE_INVALID )
                {
                    if (module_info.module_validation == STKTPLG_MGR_MODULE_INVALID)
                    {
                        SYSFUN_Debug_Printf("Unit %lu, expansion module 1, version %s\r\n",unit_id,module_version);
                        #if 0 /* rich linux */
                        LED_PMGR_SetModuleLED(unit_id, LEDDRV_COLOR_AMBERFLASH);
                        #endif
                    }
                    else
                    {
                        #if 0 /* rich linux */
                        LED_PMGR_SetModuleLED(unit_id, LEDDRV_COLOR_GREEN);
                        #endif
                    }
                }
            }
            SYSFUN_Debug_Printf("\r\n");
        }
    //}
#endif
    return TRUE;
}

/* FUNCTION NAME: CLI_MGR_GetStackingDB
 * PURPOSE: This API is used to get the mac and unit id and device type
 * in CLI configuration file header
 * INPUT:   None.
 * OUTPUT:  stacking_db : array of structure.
 * RETUEN:  0  -- failure
 *          otherwise -- success. the returned value is the number of entries
 * NOTES:   None.
 */

UI8_T CLI_MGR_GetStackingDB(STKTPLG_OM_StackingDB_T stacking_db[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK])
{

    char  type_str[30] = {0};
    char  content_str[30] = {0};
    UI32_T len = 0;
    UI32_T i = 0, j = 0, k = 0;
    char     *tmp_xbuf;
    char     *tmp_xbuf_origin;
    char     *cmd_buf;
    UI32_T  ret_val = 0;
    UI32_T  pars_count = 0;
    UI32_T  read_count = 0;
    UI32_T  unit_id = 0;
    char   mac_addr_str[20];
    UI32_T  device_type = 0;
    char   mac_addr[SYS_ADPT_MAC_ADDR_LEN] = {0};
    char   startupfile[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1] = {0};
    FS_File_Attr_T file_attr;
    UI32_T xfer_buf_length=0;
    char  temp_num[4] = {0};
    UI32_T fs_ret  = 0;
    UI32_T orig_priority;

    /*2004/5/11
     *byrant
     *use static buffer instead of cli_mgr_allocate buffer
     */
    CLI_MGR_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id);
    memset(stacking_buf,0,sizeof(stacking_buf));
    tmp_xbuf_origin = stacking_buf;

    if((cmd_buf = L_MM_Malloc(CLI_DEF_MAX_BUFSIZE, L_MM_USER_ID2(SYS_MODULE_CLI, CLI_TYPE_TRACE_ID_CLI_MGR_GETSTACKINGDB))) == NULL)
    {
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        return 0;
    }
    memset(cmd_buf,0,sizeof(cmd_buf));
    tmp_xbuf = tmp_xbuf_origin;
    if(FS_RETURN_OK!=FS_GetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, (UI8_T *)startupfile))
    {
        CLI_LIB_PrintStr("Failed to get start file name\r\n");
        L_MM_Free(cmd_buf);
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        return 0;
    }
    file_attr.file_type_mask = FS_FILE_TYPE_MASK(FS_FILE_TYPE_CONFIG);
    strcpy((char *)file_attr.file_name, (char *)startupfile);
    if(FS_GetFileInfo(DUMMY_DRIVE, &file_attr) != FS_RETURN_OK)
    {
        CLI_LIB_PrintStr("Failed to get start file info\r\n");
        L_MM_Free(cmd_buf);
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        return 0;
    }
    fs_ret = FS_ReadFile( DUMMY_DRIVE, file_attr.file_name ,  (UI8_T *)tmp_xbuf, CLI_DEF_MAX_HEADER*CLI_DEF_DISP_WIDTH, &xfer_buf_length);
    if ( fs_ret != FS_RETURN_OK && fs_ret != FS_RETURN_FILE_TRUNCATED)
    {
        CLI_LIB_PrintStr("Failed to read file info\r\n");
        L_MM_Free(cmd_buf);
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        return 0;
    }
    while ( read_count < CLI_DEF_MAX_HEADER )
    {
        if (CLI_MGR_ReadLine_From_XferBuf(cmd_buf, &tmp_xbuf))
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
                pars_count ++;


                if(content_str[0] != '0'||content_str[1] != '0')
                {
                    unit_id = 0;
                    device_type = 0;
                    memset(mac_addr,0,SYS_ADPT_MAC_ADDR_LEN);
                    memset(mac_addr_str,0,sizeof(mac_addr_str));

                    memcpy(temp_num,content_str,2);
                    temp_num[2] = '\0';
                    unit_id = atoi((char *)temp_num);

                    memcpy(temp_num,content_str+21,2);
                    temp_num[2] = '\0';
                    device_type = atoi((char *)temp_num);

                    memcpy(mac_addr_str,content_str+3,17);
                    mac_addr_str[17] = '\0';
                    CLI_LIB_AtoEa(mac_addr_str, mac_addr);

                    stacking_db[pars_count - 1].unit_id = unit_id;
                    memcpy(stacking_db[pars_count - 1].mac_addr,mac_addr,SYS_ADPT_MAC_ADDR_LEN);
                    stacking_db[pars_count -1 ].device_type = device_type;
                    ret_val ++;
                }

                if(pars_count == SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK)
                {
                    SYSFUN_Debug_Printf("\r\n[get stackingMac done ret_val] = %lu\r\n",ret_val);
                    L_MM_Free(cmd_buf);
                    CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
                    return  ret_val;
                }
            }
        }
        else
        {
            L_MM_Free(cmd_buf);
            CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
            return ret_val;
        }
    }

    L_MM_Free(cmd_buf);
    CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
    return ret_val;
}
#if 0 /* rich */
/*If any switch/module version is invalidate return FALSE*/
static BOOL_T CLI_MGR_CheckStkVersionValidate(void)
{
    UI32_T  unit_id;
    STKTPLG_MGR_Switch_Info_T switch_info;
    STKTPLG_MGR_switchModuleInfoEntry_T module_info;

    unit_id = 0;
    while(STKTPLG_POM_GetNextUnit(&unit_id))
    {
        memset(&switch_info, 0, sizeof(STKTPLG_MGR_Switch_Info_T));
        memset(&module_info, 0, sizeof(STKTPLG_MGR_switchModuleInfoEntry_T));
        switch_info.sw_unit_index = unit_id;
        module_info.unit_index = unit_id;
        module_info.module_index = 1;

        if(!STKTPLG_PMGR_GetSwitchInfo(&switch_info))
        {
            return FALSE;
        }

        if(STKTPLG_PMGR_GetSwitchModuleInfoEntry(&module_info))
        {
           if(switch_info.sw_validation == STKTPLG_MGR_SWITCH_INVALID || module_info.module_validation ==STKTPLG_MGR_MODULE_INVALID)
            {
                return FALSE;
            }
        }
        else
        {
            if(switch_info.sw_validation == STKTPLG_MGR_SWITCH_INVALID)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}
#endif /* rich */

BOOL_T CLI_MGR_ReadLine_From_XferBuf(char *cmd_buf, char **xbuf_p)
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

/* FUNCTION NAME: CLI_MGR_GetIsReproduceDone
 * PURPOSE: This API is used to get the cli reproduce config file done
 * INPUT:   None.
 * OUTPUT:  None
 * RETUEN:  TRUE  -- success
 *          FALSE -- failure
 * NOTES:   None.
 */
BOOL_T CLI_MGR_GetIsReproduceDone(void)
{
    return CLI_OM_GetIsReproduceDone();
}

/* FUNCTION NAME: CLI_MGR_SetIsReproduceDone
 * PURPOSE: This API is used to get the cli reproduce config file done
 * INPUT:   None.
 * OUTPUT:  None
 * RETUEN:  TRUE  -- success
 *          FALSE -- failure
 * NOTES:   None.
 */
BOOL_T CLI_MGR_SetIsReproduceDone(BOOL_T state)
{
    return CLI_OM_SetIsReproduceDone(state);
}


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
void *CLI_MGR_AllocateBuffer(void)
{


    if (cli_buf_allocated)
    {

        return NULL;
    } /* End of if */

    cli_buf_allocated= TRUE;
    memset(cli_buf, 0, sizeof(cli_buf));

    return cli_buf;

 }/* End of CLI_MGR_AllocateBuffer() */


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
BOOL_T  CLI_MGR_FreeBuffer(void *buf_P)
{
    if( ! cli_buf_allocated )
    {
        return FALSE;
    }
    else
    {
        cli_buf_allocated = FALSE;
        memset(cli_buf, 0, sizeof(cli_buf));
    }/* end if */

    return TRUE;
}/* End of CLI_MGR_FreeBuffer() */

#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MGR_AddDeviceCfg
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will append accton cli device config to buffer
 *
 * INPUT    : unit, cmd_buffer
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     :
 * ------------------------------------------------------------------------
 */
void CLI_MGR_AddDeviceCfg( UI32_T device_id, UI8_T * cmd_buffer )
{
    if ( device_id <= 0 && device_id > CLI_MGR_NUMBER_OF_TOTAL_DEVICE )
    {
        return;
    }
    else
    {
        strcat( device_cfg[ device_id - 1 ], (char *)cmd_buffer );
        strcat( device_cfg[ device_id - 1 ], "\n" );
    }
    return;
}
#else
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
void CLI_MGR_AddModuleCfg(UI32_T unit, UI8_T *cmd_buffer)
{
    if (unit<=0 && unit>SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK)
    {
        return;
    }
    else
    {
        strcat(module_cfg[unit-1], cmd_buffer);
        strcat(module_cfg[unit-1], "\n");
    }
    return;
}
#endif

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
BOOL_T CLI_MGR_GetConfiguartionModuleTypes(UI32_T  module_types[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK])
{
#if 0 /* rich */
    UI8_T  type_str[30] = {0};
    UI8_T  content_str[30] = {0};
    UI32_T len = 0;
    UI32_T i = 0, j = 0, k = 0;
    UI8_T     *tmp_xbuf;
    UI8_T     *tmp_xbuf_origin;
    UI8_T     *cmd_buf;
    UI32_T  read_count = 0;
    UI8_T   startupfile[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1] = {0};
    FS_File_Attr_T file_attr;
    UI32_T xfer_buf_length=0;

    /*init module_types*/

    memset(module_types, 0, sizeof(UI32_T)*SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK);

    if((tmp_xbuf_origin = (UI8_T *)CLI_MGR_AllocateBuffer()) == NULL)
    {
        return FALSE;
    }

    if((cmd_buf = (UI8_T *)L_MM_Malloc(CLI_DEF_MAX_BUFSIZE, L_MM_USER_ID2(SYS_MODULE_CLI, CLI_TYPE_TRACE_ID_CLI_MGR_GETCONFIGUARTIONMODULETYPES))) == NULL)
    {
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        return FALSE;
    }
    memset(cmd_buf,0,sizeof(cmd_buf));
    tmp_xbuf = tmp_xbuf_origin;

    if(FS_RETURN_OK!=FS_GetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, startupfile))
    {
        CLI_LIB_PrintStr("Failed to get start file name\r\n");
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        L_MM_Free(cmd_buf);
        return FALSE;
    }

    file_attr.file_type_mask = FS_FILE_TYPE_MASK(FS_FILE_TYPE_CONFIG);
    strcpy(file_attr.file_name, startupfile);
    if(FS_GetFileInfo(DUMMY_DRIVE, &file_attr) != FS_RETURN_OK)
    {
        CLI_LIB_PrintStr("Failed to get start file info\r\n");
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        L_MM_Free(cmd_buf);
        return FALSE;
    }

    if (FS_ReadFile( DUMMY_DRIVE, file_attr.file_name ,  tmp_xbuf, CLI_DEF_MAX_CONFIG_SIZE, &xfer_buf_length) != FS_RETURN_OK)
    {
        CLI_LIB_PrintStr("Failed to read file info\r\n");
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        L_MM_Free(cmd_buf);
        return FALSE;
    }

    while ( read_count < CLI_DEF_MAX_HEADER )
    {
        if (CLI_LIB_ReadLine_From_XferBuf(cmd_buf, &tmp_xbuf))
        {
            read_count ++;
            len = strlen(cmd_buf);
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

            if (strcmp(type_str, "stackingDB") == 0)
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
            }

            if (strcmp(type_str, "stackingDB") == 0)
            {
                UI32_T  unit_id = 0;
                UI8_T   module_types_str[4] = {0};

                for(unit_id = 0 ;unit_id <SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ; unit_id++)
                {
                    memcpy(module_types_str,content_str + (unit_id*2 ),2);
                    module_types[unit_id] = CLI_LIB_AtoUl(module_types_str ,10 );
                    SYSFUN_Debug_Printf("stacking DB: unit = %lu, module type = %lu\n", unit_id, module_types[unit_id]);
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

    CLI_MGR_FreeBuffer(tmp_xbuf_origin);
    L_MM_Free(cmd_buf);
#endif /* rich */
    return TRUE;

}

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
UI32_T CLI_MGR_GetUintNeedProvision(void)
{
    UI32_T i;
    UI32_T orig_priority;

    CLI_MGR_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id);

    for(i=0;i<SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;i++)
    {
        if( cli_provision_dirtybit[i] == 1 )
        {
            return i+1;
        }
    }
    CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);

    return 0;
}

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
void CLI_MGR_SetUintIsNeedProvision(UI32_T unit, BOOL_T is_need)
{
    UI32_T orig_priority;

    if(unit > SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK || unit < 1)
        return;
    CLI_MGR_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id);
    if(is_need)
    {
        cli_provision_dirtybit[unit -1] = 1 ;
    }
    else
    {
        cli_provision_dirtybit[unit -1] = 0 ;
    }
    CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
}

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
BOOL_T CLI_MGR_GetModuleCfg(UI32_T unit, UI8_T *module_buffer, UI32_T *module_buffer_len)
{
    if(unit > SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK || unit < 1)
        return FALSE;
    module_buffer = module_cfg[unit - 1];
    *module_buffer_len = strlen(module_cfg[unit - 1]);
    return TRUE;
}
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
BOOL_T CLI_MGR_SetSysBannerMsgStatus(char delimiter)
{
#if (SYS_CPNT_MOTD == TRUE)

    UI32_T function_no;
    UI32_T orig_priority;

    function_no = CLI_MGR_SETSYSBANNERMSGSTATUS;
    CLI_USE_CSC_CHECK_OPER_MODE(FALSE, function_no);
    CLI_MGR_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id);


    if(operate_sys_banner_msg_status == FALSE)
    {
        operate_sys_banner_msg_status = TRUE;
        memset(cli_sys_bnr_msg, 0, SYS_ADPT_MAX_MOTD_LENGTH + 1);
        cli_sys_bnr_msg_position = 0;
        operate_sys_banner_msg_by_session_id = CLI_TASK_GetMySessId();
        operate_sys_banner_msg_delimiting_character = delimiter;
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        CLI_RETURN_AND_RELEASE_CSC(TRUE);
    }
    else
    {
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        CLI_RETURN_AND_RELEASE_CSC(FALSE);
    }
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/
    return TRUE;
}


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
BOOL_T CLI_MGR_DisableSysBannerMsg()
{
#if (SYS_CPNT_MOTD == TRUE)

    UI32_T function_no;
    UI32_T orig_priority;

    function_no = CLI_MGR_DISABLESYSBANNERMSGSTATUS;
    CLI_USE_CSC_CHECK_OPER_MODE(FALSE, function_no);
    CLI_MGR_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id);

    if(operate_sys_banner_msg_status == FALSE)
    {
        if(SYS_BNR_PMGR_SetSysBnrMsg(SYS_BNR_PMGR_MOTD_TYPE, NULL) == TRUE)
        {
            CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
            CLI_RETURN_AND_RELEASE_CSC(TRUE);

        }
        else
        {
            CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
            CLI_RETURN_AND_RELEASE_CSC(FALSE);
        }
    }
    else
    {
        CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
        CLI_RETURN_AND_RELEASE_CSC(FALSE);
    }
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/
    return TRUE;
}

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
void CLI_MGR_Set_StkctrlSavingConfigStatus(BOOL_T status)
{
   CLI_MGR_stkctrl_saving_config_callback_status = status;
}

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
void CLI_MGR_Get_StkctrlSavingConfigStatus(BOOL_T *status)
{
   *status = CLI_MGR_stkctrl_saving_config_callback_status;
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Get_RuntimeProvisionFlag
 *------------------------------------------------------------------------
 * FUNCTION: Get cli enter runtime provision flag
 * INPUT   : none
 * OUTPUT   : none
 * RETURN  : TRUE --  cli will enter runtime provision now
 *                 FALSe -- cli use normal provision
 * NOTE    :
 *-----------------------------------------------------------------------*/
BOOL_T CLI_MGR_Get_RuntimeProvisionFlag()
{
    return cli_mgr_enter_runtime_provision_mode;
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Set_RuntimeProvisionFlag
 *------------------------------------------------------------------------
 * FUNCTION: Set cli enter runtime provision flag
 * INPUT   : BOOL_T val
 *                 TRUE --  cli will enter runtime provision now
 *                 FALSE -- cli use normal provision
 *
 * OUTPUT   : none
 * RETURN  : none
 * NOTE    :
 *-----------------------------------------------------------------------*/
void CLI_MGR_Set_RuntimeProvisionFlag(BOOL_T val)
{
    cli_mgr_enter_runtime_provision_mode = val;
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Get_BuffermgmtOffset
 *------------------------------------------------------------------------
 * FUNCTION: Get buffermgmt offset in order for runtime provision can use
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : I32_T offset
 * NOTE    :
 *-----------------------------------------------------------------------*/
I32_T CLI_MGR_Get_BuffermgmtOffset()
{
    return cli_mgr_buffermgmt_offset;
}

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_Set_BuffermgmtOffset
 *------------------------------------------------------------------------
 * FUNCTION: Set buffermgmt offset in order for runtime provision can use
 * INPUT   : I32_T offset
 * OUTPUT  : none
 * RETURN  : none
 * NOTE    :
 *-----------------------------------------------------------------------*/
void CLI_MGR_Set_BuffermgmtOffset(I32_T offset)
{
    cli_mgr_buffermgmt_offset = offset;
}


void CLI_MGR_HandleSysCallbackIPCMsg(SYSFUN_Msg_T* msgbuf_p)
{
    SYS_CALLBACK_MGR_Msg_T *sys_cbmsg_p = (SYS_CALLBACK_MGR_Msg_T*)msgbuf_p->msg_buf;

    switch(sys_cbmsg_p->callback_event_id)
    {
        case SYS_CALLBACK_MGR_CALLBACKEVENTID_ANNOUNCE_XFER_RESULT:
        case SYS_CALLBACK_MGR_CALLBACKEVENTID_ANNOUNCE_CLI_XFER_RESULT:
        {
            SYS_CALLBACK_MGR_AnnounceXferResult_CBData_T  *cbdata_msg_p = (SYS_CALLBACK_MGR_AnnounceXferResult_CBData_T *)sys_cbmsg_p->callback_data;
            CLI_MGR_XferCopy_Callback(cbdata_msg_p->cookie, cbdata_msg_p->status);
        }
		break;

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
		case SYS_CALLBACK_MGR_CALLBACKEVENTID_CLI_DYNAMIC_PROVISION_VIA_DHCP:
		    SYS_CALLBACK_MGR_HandleIPCMsgAndCallback(msgbuf_p,
                CLI_MGR_DhcpRxOptionConfig_Callback);
        break;
#endif

#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
        case SYS_CALLBACK_MGR_CALLBACKEVENTID_ANNOUNCE_RELOAD_REMAINING_TIME:
		    SYS_CALLBACK_MGR_HandleIPCMsgAndCallback(
                msgbuf_p,
                CLI_MGR_ReloadNotify_Callback
                );
            break;
#endif
        default:
            SYSFUN_Debug_Printf("\r\n%s: received callback_event that is not handled(%d)",
                __FUNCTION__, sys_cbmsg_p->callback_event_id);
    }
}

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
BOOL_T CLI_MGR_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p)
{
    CLI_MGR_IPCMsg_T *mgr_msg_p;

    if(ipcmsg_p==NULL)
        return FALSE;

    mgr_msg_p = (CLI_MGR_IPCMsg_T*)ipcmsg_p->msg_buf;

    /* Every ipc request will fail when operating mode is transition mode
     */
    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_TRANSITION_MODE)
    {
        mgr_msg_p->type.result = FALSE;
        ipcmsg_p->msg_size = CLI_MGR_MSGBUF_TYPE_SIZE;
        return TRUE;
    }

    switch(mgr_msg_p->type.cmd)
    {
    case CLI_MGR_IPC_ALLCSCSKNOWPROVISIONCOMPLETE:
        CLI_MGR_AllCscsKnowProvisionComplete();
        ipcmsg_p->msg_size=CLI_MGR_MSGBUF_TYPE_SIZE;
        break;

    case CLI_MGR_IPC_SAVING_CONFIG_STATUS:
        CLI_MGR_Set_StkctrlSavingConfigStatus(mgr_msg_p->data.config_status_data.status);
        ipcmsg_p->msg_size=CLI_MGR_MSGBUF_TYPE_SIZE;
        break;

    case CLI_MGR_IPC_ENTER_TRANSITION_MODE:
        CLI_MGR_Set_BuffermgmtOffset(mgr_msg_p->data.running_config_data.buffer);
        CLI_MGR_Set_RuntimeProvisionFlag(TRUE);
        CLI_MGR_Notify_EnterTransitionMode();
        ipcmsg_p->msg_size=CLI_MGR_MSGBUF_TYPE_SIZE;
        break;

    case CLI_MGR_IPC_INC_REMOTE_SESSION:
        ipcmsg_p->msg_size=CLI_MGR_MSGBUF_TYPE_SIZE;
        mgr_msg_p->type.result = CLI_MGR_IncreaseRemoteSession();
        break;

    case CLI_MGR_IPC_DEC_REMOTE_SESSION:
        ipcmsg_p->msg_size=CLI_MGR_MSGBUF_TYPE_SIZE;
        mgr_msg_p->type.result = CLI_MGR_DecreaseRemoteSession();
        break;

    case CLI_MGR_IPC_SET_KILL_WORKING_SPACE_FLAG:
        ipcmsg_p->msg_size=CLI_MGR_MSGBUF_TYPE_SIZE;
        mgr_msg_p->type.result = CLI_MGR_SetKillWorkingSpaceFlag(mgr_msg_p->data.session_data.session_id);
        break;

    case CLI_MGR_IPC_GET_RUNNING_CONFIG:
        ipcmsg_p->msg_size=CLI_MGR_MSGBUF_TYPE_SIZE;
        mgr_msg_p->type.result = CLI_MGR_Get_RunningCfg(BUFFER_MGR_GetPtr(mgr_msg_p->data.running_config_data.buffer),
                                                        mgr_msg_p->data.running_config_data.buffer_size);
        break;

    case CLI_MGR_IPC_CLOSE_ALL_TELNET_SESSIONS:
        ipcmsg_p->msg_size = CLI_MGR_MSGBUF_TYPE_SIZE;
        mgr_msg_p->type.result = CLI_MGR_CloseAllTelnetSessions();
        break;

    case CLI_MGR_IPC_HANDLE_CHANGED_IP_MGMT_FILTER:
        ipcmsg_p->msg_size = CLI_MGR_MSGBUF_TYPE_SIZE;
        mgr_msg_p->type.result = CLI_MGR_HandleChangedIpMgmtFilter();
        break;

    default:
        SYSFUN_Debug_Printf("%s(): Invalid cmd.\n", __FUNCTION__);
        mgr_msg_p->type.result = FALSE;
        ipcmsg_p->msg_size = CLI_MGR_MSGBUF_TYPE_SIZE;
        break;
    }

    return TRUE;
}

#if(SYS_CPNT_CLUSTER==TRUE)
void CLI_MGR_SetTelnetRelayingFlag(UI32_T tnpdTaskId, BOOL_T isRelaying)
{
    CLI_TASK_SetTelnetRelayingFlag(tnpdTaskId, isRelaying);
}
#endif


#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_LocalPrivLvlCmdEntryCompare
 *---------------------------------------------------------------------------
 * PURPOSE  : Compare function for link list of privilege level command entry
 * INPUT    : inlist    - node in list
 *            input     - new node
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
static BOOL_T
CLI_MGR_LocalPrivLvlCmdEntryCompare(
    void *inlist,
    void *input)
{
    CLI_PrivLvlCmdEntry_T *e1 = (CLI_PrivLvlCmdEntry_T *) inlist;
    CLI_PrivLvlCmdEntry_T *e2 = (CLI_PrivLvlCmdEntry_T *) input;
    
    if (e1->cmd_mode != e2->cmd_mode)
    {
        assert(0);
        return FALSE;
    }
    
    if (e1->is_all != e2->is_all)
    {
        return FALSE;
    }
    
    if (strcmp(e1->cmd_str, e2->cmd_str) != 0)
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_LocalGetPrivLvlCmdList
 *---------------------------------------------------------------------------
 * PURPOSE  : Get privilege level command list
 * INPUT    : cmd_mode    - command mode
 * OUTPUT   : None
 * RETURN   : privilege level command list
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
static L_LINK_LST_List_T*
CLI_MGR_LocalGetPrivLvlCmdList(
    UI8_T cmd_mode)
{
    if (_countof(privilege_list) <= cmd_mode)
    {
        return NULL;
    }
    
    return &privilege_list[cmd_mode];
}

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_LocalFindPrivLvlCmdEntry
 *---------------------------------------------------------------------------
 * PURPOSE  : find an entry with cmd_mode & cmd_str
 * INPUT    : cmd_mode, is_all, cmd_str_len, cmd_str
 * OUTPUT   : None
 * RETURN   : pointer to an entry (NULL -> Failed)
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
static CLI_PrivLvlCmdEntry_T *
CLI_MGR_LocalFindPrivLvlCmdEntry(
    UI8_T cmd_mode,
    BOOL_T is_all,
    const char *cmd_str)
{   
    L_LINK_LST_List_T       *list_p = CLI_MGR_LocalGetPrivLvlCmdList(cmd_mode);
    CLI_PrivLvlCmdEntry_T   key;

    if (NULL == list_p)
    {
        return NULL;
    }
    
    memset(&key, 0, sizeof(key));
    
    key.cmd_mode = cmd_mode;
    key.is_all   = is_all;
    
    strncpy(key.cmd_str, cmd_str, sizeof(key.cmd_str));
    if (key.cmd_str[ sizeof(key.cmd_str)-1 ] != '\0')
    {
        return NULL;
    }
    
    return L_LINK_LST_GetPtr(list_p, &key);
}

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_LocalFreePrivLvlCmdEntryList
 *---------------------------------------------------------------------------
 * PURPOSE  : free all entries in one list.
 * INPUT    : plce_list_head
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
static void 
CLI_MGR_LocalFreePrivLvlCmdEntryList()
{
    UI32_T i;
    
    for (i=0; i<_countof(privilege_list); ++i)
    {
        L_LINK_LST_List_T *list_p = CLI_MGR_LocalGetPrivLvlCmdList(i);
        
        L_LINK_LST_Delete_All(list_p);
    }
}

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
static BOOL_T
CLI_MGR_LocalDelPrivLvlCmdEntry(
    UI8_T cmd_mode,
    BOOL_T is_all,
    const char *cmd_str)
{
    L_LINK_LST_List_T       *list_p = CLI_MGR_LocalGetPrivLvlCmdList(cmd_mode);
    CLI_PrivLvlCmdEntry_T   entry;
    
    if (NULL == list_p)
    {
        return FALSE;
    }
    
    memset(&entry, 0, sizeof(entry));
    entry.cmd_mode  = cmd_mode;
    entry.is_all    = is_all;
    
    strncpy(entry.cmd_str, cmd_str, sizeof(entry.cmd_str));
    if (entry.cmd_str[sizeof(entry.cmd_str)-1] != '\0')
    {
        return FALSE;
    }
    
    return L_LINK_LST_Delete(list_p, &entry);
}

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_LocalAddPrivLvlCmdEntry
 *---------------------------------------------------------------------------
 * PURPOSE  : add a plce to the list
 * INPUT    : cmd_mode, pri_lvl, cmd_str_len, cmd_str
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : always add a new one to the tail of the list
 *---------------------------------------------------------------------------
 */
static BOOL_T
CLI_MGR_LocalAddPrivLvlCmdEntry(
    UI8_T cmd_mode,
    BOOL_T is_all,
    UI8_T pri_level,
    const char *cmd_str)
{
    L_LINK_LST_List_T       *list_p = CLI_MGR_LocalGetPrivLvlCmdList(cmd_mode);
    CLI_PrivLvlCmdEntry_T   entry;
    
    if (NULL == list_p)
    {
        return FALSE;
    }
    
    memset(&entry, 0, sizeof(entry));
    entry.cmd_mode  = cmd_mode;
    entry.is_all    = is_all;
    entry.pri_level = pri_level;
    
    strncpy(entry.cmd_str, cmd_str, sizeof(entry.cmd_str));
    if (entry.cmd_str[sizeof(entry.cmd_str)-1] != '\0')
    {
        return FALSE;
    }
    
    return L_LINK_LST_Set(list_p, &entry, L_LINK_LST_APPEND);   
}

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_GetNextPrivLvlCmdEntry
 *---------------------------------------------------------------------------
 * PURPOSE  : get next plce
 * INPUT    : idx
 * OUTPUT   : idx, cmd_mode, pri_lvl, cmd_str
 * RETURN   : TRUE/FALSE
 * NOTE     : *idx = 0 implies get first entry
 *---------------------------------------------------------------------------
 */
static BOOL_T
CLI_MGR_LocalGetNextPrivLvlCmdEntry(
    UI32_T *idx,
    CLI_PrivLvlCmdEntry_T *plce_p)
{
    L_LINK_LST_List_T       *list_p;
    UI32_T                  i;
    
    if (0 == *idx)
    {
        for (i=0; i < _countof(privilege_list); ++i)
        {
            list_p = CLI_MGR_LocalGetPrivLvlCmdList(i);
            
            if (TRUE  == L_LINK_LST_Get_1st(list_p, plce_p))
            {
                *idx = 1;
                return TRUE;
            }
        }
    }
    else
    {
        list_p = CLI_MGR_LocalGetPrivLvlCmdList(plce_p->cmd_mode);
        
        if (TRUE == L_LINK_LST_Get_Next(list_p, plce_p))
        {
            return TRUE;
        }
        
        for (i=plce_p->cmd_mode+1; i < _countof(privilege_list); ++i)
        {
            list_p = CLI_MGR_LocalGetPrivLvlCmdList(i);
            
            if (TRUE == L_LINK_LST_Get_1st(list_p, plce_p))
            {
                return TRUE;
            }
        }
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_LocalUpdatePrivilege
 *---------------------------------------------------------------------------
 * PURPOSE  : Update privilege on the specified command mode
 * INPUT    : cmd_mode  - command mode
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
static BOOL_T
CLI_MGR_LocalUpdatePrivilege(
    UI8_T cmd_mode)
{
    UI32_T idx;
    CLI_PrivLvlCmdEntry_T entry;
    UI32_T len = 0;
    char *mode_str;
    
    CLI_LIB_GetCommandModeString(cmd_mode, NULL, &len);
    
    mode_str = calloc(len, 1);
    if (!mode_str)
    {
        return FALSE;
    }
    
    /* Reassign privilege for all command on the specified mode
     */
    CLI_LIB_GetCommandModeString(cmd_mode, mode_str, &len);
    CLI_LIB_SetCommandPrivilege(mode_str, CLI_DEF_RETURN_DEFAULT_PRIVILEGE_LEVEL, "", FALSE);
    
    idx = 0;
    while (TRUE == CLI_MGR_GetNextPrivilegeCmd(&idx, &entry))
    {
        if (entry.cmd_mode != cmd_mode)
        {
            continue;
        }
        
        CLI_LIB_SetCommandPrivilege(mode_str, entry.pri_level, entry.cmd_str, entry.is_all);
    }
    
    free(mode_str);
    return TRUE;    
}

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_LocalNormalizeCommandString
 *---------------------------------------------------------------------------
 * PURPOSE  : Normalize user input command string
 * INPUT    : cmd_str   - command string
 *            buf_sz    - buffer size of result parameter
 * OUTPUT   : result    - normalized command string
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
static BOOL_T
CLI_MGR_LocalNormalizeCommandString(
    const char * cmd_str,
    char * result,
    UI32_T buf_sz)
{
    char * cmd_str_dup = strdup(cmd_str);
    char * argv[CLI_DEF_MAX_PRIVILEGE_CMD_STRING/2] = {0};
    UI32_T argc = sizeof(argv)/sizeof(*argv);
    BOOL_T ret;
        
    if (!cmd_str_dup)
    {
        return FALSE;
    }
    
    CLI_LIB_SplitString(cmd_str_dup, argv, &argc);
    ret = CLI_LIB_JoinStringArray(argv, argc, " ", result, &buf_sz);

    if (ret)
    {
        L_STDLIB_StringN_To_Lower((UI8_T *)result, buf_sz);
    }

    free(cmd_str_dup);
    return ret;
}

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
    const char *_cmd_str)
{
    UI32_T                  orig_priority;
    enum CLI_ACCESS_MODE_E  cmd_mode;
    char                    *norm_cmd_str = NULL;
    BOOL_T                  ret = FALSE;
    
    if (NULL == _cmd_str)
    {
        return FALSE;
    }
    
    CLI_MGR_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id);
    
    if (FALSE == CLI_LIB_GetCommandMode(mode_str, &cmd_mode))
    {
        /* Invalid command mode string
         */
        goto leave_and_free_resource;
    }
    
    norm_cmd_str = strdup(_cmd_str);
    if (!norm_cmd_str)
    {
        goto leave_and_free_resource;
    }
    
    if (FALSE == CLI_MGR_LocalNormalizeCommandString(_cmd_str, norm_cmd_str, strlen(norm_cmd_str)+1))
    {
        goto leave_and_free_resource;   
    }   
    
    ret = CLI_MGR_LocalDelPrivLvlCmdEntry(cmd_mode, is_all, norm_cmd_str);
    
    if (FALSE == ret)
    {
        goto leave_and_free_resource;       
    }
    
    CLI_MGR_LocalUpdatePrivilege(cmd_mode);

    CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);

    return TRUE;
    
leave_and_free_resource:
    
    free(norm_cmd_str);
    
    CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
    return FALSE;   
}

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
    const char *_cmd_str)
{
    UI32_T                  orig_priority;
    CLI_PrivLvlCmdEntry_T   *plce_p;
    char                    *norm_cmd_str = NULL;
    enum CLI_ACCESS_MODE_E  cmd_mode;
    BOOL_T                  ret = TRUE;
    
    if (NULL == _cmd_str)
    {
        return FALSE;
    }

    CLI_MGR_ENTER_CRITICAL_SECTION(cli_mgr_semaphore_id);
    
    if (FALSE == CLI_LIB_GetCommandMode(mode_str, &cmd_mode))
    {
        /* Invalid command mode string
         */
        goto leave_and_free_resource;
    }
    
    norm_cmd_str = strdup(_cmd_str);
    if (!norm_cmd_str)
    {
        goto leave_and_free_resource;
    }
    
    if (FALSE == CLI_MGR_LocalNormalizeCommandString(_cmd_str, norm_cmd_str, strlen(norm_cmd_str)+1))
    {
        goto leave_and_free_resource;   
    }

    plce_p = CLI_MGR_LocalFindPrivLvlCmdEntry (cmd_mode, is_all, norm_cmd_str);
    if (NULL == plce_p)
    {
        ret = CLI_MGR_LocalAddPrivLvlCmdEntry (cmd_mode, is_all, pri_lvl, norm_cmd_str);
        
        if (FALSE == ret)
        {
            goto leave_and_free_resource;       
        }
        
        ret = CLI_LIB_SetCommandPrivilege(mode_str, pri_lvl, norm_cmd_str, is_all);
        
        if (FALSE == ret)
        {
            CLI_MGR_LocalDelPrivLvlCmdEntry(cmd_mode, is_all, norm_cmd_str);
            CLI_MGR_LocalUpdatePrivilege(cmd_mode);
            
            goto leave_and_free_resource;       
        }
    }
    else
    {
        UI16_T old_pri_lvl = plce_p->pri_level;
        plce_p->pri_level = pri_lvl;

        ret = CLI_MGR_LocalUpdatePrivilege(cmd_mode);
        
        if (FALSE == ret)
        {
            plce_p->pri_level = old_pri_lvl;
            
            CLI_MGR_LocalUpdatePrivilege(cmd_mode);
            
            goto leave_and_free_resource;
        }
    }
    
    CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);

    return TRUE;
    
leave_and_free_resource:
    
    free(norm_cmd_str);
    
    CLI_MGR_LEAVE_CRITICAL_SECTION(cli_mgr_semaphore_id, orig_priority);
    return FALSE;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_MGR_GetNextRunningPrivilegeCmd(
    UI32_T *idx, CLI_PrivLvlCmdEntry_T *plce_p)
{
    BOOL_T  ret;

    CLI_USE_CSC_CHECK_OPER_MODE (SYS_TYPE_GET_RUNNING_CFG_FAIL, CLI_MGR_GETNEXTRUNNINGPRIVILEGECMD);

    ret = CLI_MGR_LocalGetNextPrivLvlCmdEntry (idx, plce_p);

    if (TRUE == ret)
    {
        CLI_RETURN_AND_RELEASE_CSC( SYS_TYPE_GET_RUNNING_CFG_SUCCESS );
    }
    else
    {
        CLI_RETURN_AND_RELEASE_CSC( SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE );
    }
}

/* squid */

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
BOOL_T CLI_MGR_GetNextPrivilegeCmd(
    UI32_T *idx, CLI_PrivLvlCmdEntry_T *plce_p)
{
    BOOL_T  ret;

    CLI_USE_CSC_CHECK_OPER_MODE (FALSE, CLI_MGR_GETNEXTPRIVILEGECMD);

    ret = CLI_MGR_LocalGetNextPrivLvlCmdEntry (idx, plce_p);

    CLI_RETURN_AND_RELEASE_CSC (ret);
}

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
void CLI_MGR_ParseCfmVlanList(UI8_T *input_p, UI32_T *primary_vid, UI32_T *vid_num, UI8_T *vid_list)
{
    UI32_T  err_idx=0;
    UI32_T  lower_val = 0, upper_val = 0;
    UI32_T  vlan=0;
    UI8_T   Token[CLI_DEF_MAX_BUFSIZE] = {0};
    UI8_T   delemiters[2] = {0};
    BOOL_T  first_vid=TRUE;

    delemiters[0] = ',';

    do
    {
       memset(Token, 0, CLI_DEF_MAX_BUFSIZE);

       input_p = (UI8_T *)CLI_LIB_Get_Token((char*)input_p, (char*)Token, (char*)delemiters);

       if(!CLI_LIB_Get_Lower_Upper_Value((char*)Token, &lower_val, &upper_val, &err_idx))
           break;

       for(vlan=lower_val; vlan<=upper_val; vlan++)
       {
           if(TRUE == first_vid)
           {
               *primary_vid=vlan;
               first_vid=FALSE;
           }

           vid_list[(UI32_T)((vlan-1)/8)] |= (1 << (7 - ((vlan-1)%8)));

           (*vid_num)++;
       }

    }while((input_p != 0) && !isspace(*input_p));

}
#endif  /* #if (SYS_CPNT_CFM == TRUE) */

#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_MGR_ReloadNotify_Callback
 * -------------------------------------------------------------------------
 * FUNCTION: callback for notifying remaining time to reload
 * INPUT   : UI32_T remaining - Remaining time
 * RETURN  :
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
static void CLI_MGR_ReloadNotify_Callback(UI32_T remaining)
{
    CLI_TASK_WorkingArea_T    *ctrl_P;
    CLI_TASK_OnlineUserInfo_T user_info;
    UI32_T  index;

    for(index = CLI_TASK_MIN_SESSION_ID; index<= CLI_TASK_MAX_SESSION_ID-1; index++)
    {
        /* check whether the line is active ?
         */
        if(CLI_TASK_GetOnLineUserInfoByIndex(index, &user_info)==TRUE)
        {
            ctrl_P = CLI_TASK_GetWorkingAreaByIndex(index);

            if (ctrl_P != NULL)
            {
                ctrl_P->reload_countdown = remaining;
            }
        }
    }
}
#endif  /* #if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE) */


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
    (UI32_T option66_length, UI8_T *option66_data_p, UI32_T option67_length, char *option67_data_p)
{
    /* if option66_length > 0 then option66 data is tftp server
     * if option67_length > 0 then option67 data is bootfile name
     */
    if( (option66_length == 0) || (option67_length == 0) )
    {
        cli_mgr_dhcp_rx_option_config_status = FALSE;
    }
    else
    {
        if( strlen(option67_data_p) > MAXSIZE_tftpSrcFile)
        {
            CLI_LIB_PrintStr("Invalid file name.\r\n");
        }
        else if( strlen((char *)option66_data_p) > SYS_ADPT_DHCP_MAX_DOMAIN_NAME_LEN )
        {
            CLI_LIB_PrintStr("Invalid TFTP server IP address.\r\n");
        }
        else
        {
            cli_mgr_dhcp_rx_option_config_status = TRUE;
            memcpy(cli_mgr_dhcp_rx_option_config_address, option66_data_p, option66_length);
            cli_mgr_dhcp_rx_option_config_address[option66_length]='\0';
            memcpy(cli_mgr_dhcp_rx_option_config_filename_ar, option67_data_p, option67_length);
            cli_mgr_dhcp_rx_option_config_filename_ar[option67_length]='\0';
        }
    }
}

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
        (BOOL_T *dhcp_status_p, char *tftp_address_p, char *file_name_p)
{
    *dhcp_status_p = cli_mgr_dhcp_rx_option_config_status;

    if(cli_mgr_dhcp_rx_option_config_status==TRUE)
    {
        strcpy(tftp_address_p, cli_mgr_dhcp_rx_option_config_address);
        strcpy(file_name_p, cli_mgr_dhcp_rx_option_config_filename_ar);
    }
}

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
void CLI_MGR_Set_Startup_FactoryDefault_Flag(BOOL_T is_default)
{
    cli_mgr_is_startup_factory_default = is_default;
}

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
void CLI_MGR_Get_Startup_FactoryDefault_Flag(BOOL_T *is_default)
{
    *is_default = cli_mgr_is_startup_factory_default;
}
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
void CLI_MGR_DisplayBanner()
{
    char  string_ar[BANNER_MAX_NOTE_DATA_LENGTH+1] = {0};

    CLI_LIB_PrintStr("\r\n***************************************************************\r\n");

    if(FALSE == CLI_BANNER_GetCompany(string_ar))
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr("Failed to get Company\r\n");
#endif
    }
    CLI_LIB_PrintStr(string_ar);

    CLI_LIB_PrintStr("\r\nWARNING - MONITORED ACTIONS AND ACCESSES\r\n");

    if(FALSE == CLI_BANNER_GetDepartment(string_ar))
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr("Failed to get Department\r\n");
#endif
    }
    CLI_LIB_PrintStr(string_ar);
    CLI_LIB_PrintStr("\r\n\r\n");

    {
        int i=0;
        BannerManagerInfo_T manager_info_p;

        memset(&manager_info_p, 0, sizeof(BannerManagerInfo_T));

        while(i<3)
        {
            i++;

            if(!CLI_BANNER_GetManagerInfo(i, &manager_info_p))
            {
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr("Failed to get Manager_Info\r\n");
#endif
            }

            if(strlen(manager_info_p.name_ar)==0) continue;

            CLI_LIB_PrintStr(manager_info_p.name_ar);
            CLI_LIB_PrintStr(" - ");
            CLI_LIB_PrintStr(manager_info_p.phone_number_ar);
            CLI_LIB_PrintStr("\r\n");
        }
    }

    CLI_LIB_PrintStr("\r\nStation's information:\r\n");

    if(FALSE == CLI_BANNER_GetEquipmentLocation(string_ar))
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr("Failed to get Equipment Location\r\n");
#endif
    }
    CLI_LIB_PrintStr(string_ar);
    CLI_LIB_PrintStr("\r\n\r\n");

    {
        BannerEquipmentInfo_T equipment_info_p;

        memset(&equipment_info_p, 0, sizeof(BannerEquipmentInfo_T));

        if(FALSE == CLI_BANNER_GetEquipmentInfo(&equipment_info_p))
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr("Failed to get Equipment Info\r\n");
#endif
    }

        if(strlen(equipment_info_p.manufacturer_ar)>0)
        {
            CLI_LIB_PrintStr(equipment_info_p.manufacturer_ar);
        }

        if(strlen(equipment_info_p.manufacturer_id_ar)>0)
        {
            CLI_LIB_PrintStr(" - ");
            CLI_LIB_PrintStr(equipment_info_p.manufacturer_id_ar);
            CLI_LIB_PrintStr("\r\n");
        }
        CLI_LIB_PrintStr("Floor / Row / Rack / Sub-Rack\r\n");
        CLI_LIB_PrintStr(equipment_info_p.floor_ar);
        CLI_LIB_PrintStr(" / ");
        CLI_LIB_PrintStr(equipment_info_p.row_ar);
        CLI_LIB_PrintStr(" / ");
        CLI_LIB_PrintStr(equipment_info_p.rack_ar);
        CLI_LIB_PrintStr(" / ");
        CLI_LIB_PrintStr(equipment_info_p.self_rack_ar);
        CLI_LIB_PrintStr("\r\n");
    }

    {
        BannerDCPowerInfo_T dc_power_info_p;
        memset(&dc_power_info_p, 0, sizeof(BannerDCPowerInfo_T));

        if(FALSE == CLI_BANNER_GetDCPowerInfo(&dc_power_info_p))
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr("Failed to get EquipmentInfo\r\n");
#endif
        }
        CLI_LIB_PrintStr("DC power supply:\r\n");
        CLI_LIB_PrintStr("Power Source A: Floor / Row / Rack / Electrical circuit\r\n");
        CLI_LIB_PrintStr(dc_power_info_p.floor_ar);
        CLI_LIB_PrintStr(" / ");
        CLI_LIB_PrintStr(dc_power_info_p.row_ar);
        CLI_LIB_PrintStr(" / ");
        CLI_LIB_PrintStr(dc_power_info_p.rack_ar);
        CLI_LIB_PrintStr(" / ");
        CLI_LIB_PrintStr(dc_power_info_p.electrical_circuit_ar);
        CLI_LIB_PrintStr("\r\n");
    }

    CLI_LIB_PrintStr("\r\nNumber of LP: ");

    if(FALSE == CLI_BANNER_GetLPNumber(string_ar))
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr("Failed to get LPNumber\r\n");
#endif
    }
    CLI_LIB_PrintStr(string_ar);

    CLI_LIB_PrintStr("\r\nPosition MUX: ");

    if(FALSE == CLI_BANNER_GetMUX(string_ar))
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr("Failed to get MUX\r\n");
#endif
    }
    CLI_LIB_PrintStr(string_ar);

    CLI_LIB_PrintStr("\r\nIP LAN: ");

    if(FALSE == CLI_BANNER_GetIpLan(string_ar))
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr("Failed to get Ip Lan\r\n");
#endif
    }
    CLI_LIB_PrintStr(string_ar);

    CLI_LIB_PrintStr("\r\nNote: ");

    if(FALSE == CLI_BANNER_GetNote(string_ar))
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr("Failed to get Note\r\n");
#endif
    }
    CLI_LIB_PrintStr(string_ar);
    CLI_LIB_PrintStr("\r\n");

    CLI_LIB_PrintStr("***************************************************************\r\n");
}
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
BOOL_T CLI_MGR_CloseAllTelnetSessions()
{
    CLI_TASK_CloseTelnetSessions();
    return TRUE;
}
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
BOOL_T CLI_MGR_HandleChangedIpMgmtFilter()
{
    CLI_TASK_TelnetIPFilter();
    return TRUE;
}
#endif /* #if (SYS_CPNT_MGMT_IP_FLT == TRUE) */

