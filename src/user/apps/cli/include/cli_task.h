/*
 * Project Name: Mercury
 * Module Name : CLI_TASK.H
 * Abstract    : to be included in root.c and tn_main.c to access cli
 * Purpose     : CLI initiation and CLI task creation
 *
 * History :
 *          Date        Modifier        Reason
 *          2001/6/1    Chin-Ju Chu     Create this file
 *
 * Copyright(C)      Accton Corporation, 2001
 *
 *
 */

#ifndef CLI_TASK_H
#define CLI_TASK_H

#include "sys_adpt.h"

#include "sys_type.h"
#include "../cli_def.h"
#include "sys_cpnt.h"
#include "l_inet.h" /* L_INET_MAX_IP4ADDR_STR_LEN, L_INET_MAX_IP6ADDR_STR_LEN */

/*

|-----------|-----------|------------|------------|------------|------------|
|     0     |     1     |      2     |      3     |      4     |     5      |
|-----------|-----------|------------|------------|------------|------------|
|   Uart    |    Tn1    |     Tn2    |     Tn3    |     Tn4    |  Provision |
|-----------|-----------|------------|------------|------------|------------|
               Min. Tn                                Max. Tn
*/

#define CLI_TASK_EnterCriticalSection(sem_id)    SYSFUN_OM_ENTER_CRITICAL_SECTION(sem_id)
#define CLI_TASK_LeaveCriticalSection(sem_id, orig_priority)    SYSFUN_OM_LEAVE_CRITICAL_SECTION(sem_id, orig_priority)

/* 2008-11-05, steven.jiang change number of cli sessions to support hotswap
 * arrangement of CLI working space array cli_working_buffer[CLI_TASK_MAX_SESSION_NUM + 2]
 *   example:
 *      0,      Console (UART)
 *      1 - 4,  Telnet
 *      5,      Provision (system)
 *      6,      Backdoor
 *      7 - 14, Provision (module)
 */

#define CLI_TASK_MAX_CONSOLE_SESSION_NUM            1
#define CLI_TASK_MAX_TELNET_SESSION_NUM             SYS_ADPT_MAX_TELNET_NUM
#define CLI_TASK_MAX_SSH_SESSION_NUM                0
#define CLI_TASK_MAX_PROVISION_SESSION_NUM          1
#define CLI_TASK_MAX_MODULE_PROVISION_SESSION_NUM   8
#define CLI_TASK_MAX_BACKDOOR_SESSION_NUM           1
#define CLI_TASK_MAX_SESSION_NUM                   (CLI_TASK_MAX_CONSOLE_SESSION_NUM        \
                                                    + CLI_TASK_MAX_TELNET_SESSION_NUM       \
                                                    + CLI_TASK_MAX_SSH_SESSION_NUM          \
                                                    + CLI_TASK_MAX_PROVISION_SESSION_NUM    \
                                                    + CLI_TASK_MAX_BACKDOOR_SESSION_NUM     \
                                                    + CLI_TASK_MAX_MODULE_PROVISION_SESSION_NUM)    /* 15 */
#define CLI_TASK_MIN_SESSION_ID                     0
#define CLI_TASK_MAX_SESSION_ID                     (CLI_TASK_MAX_SESSION_NUM - 1)  /* 14 */
#define CLI_TASK_UART_SESSION_ID                    CLI_TASK_MIN_SESSION_ID
#define CLI_TASK_MIN_TELNET_SESSION_ID              (CLI_TASK_UART_SESSION_ID + 1)
#define CLI_TASK_MAX_TELNET_SESSION_ID              (CLI_TASK_UART_SESSION_ID + CLI_TASK_MAX_TELNET_SESSION_NUM)
#define CLI_TASK_MIN_SSH_SESSION_ID                 -1
#define CLI_TASK_MAX_SSH_SESSION_ID                 -1
#define CLI_TASK_PROVISION_SESSION_ID               (CLI_TASK_MAX_TELNET_SESSION_ID + 1)
#define CLI_TASK_BACKDOOR_SESSION_ID                (CLI_TASK_PROVISION_SESSION_ID + 1)
#define CLI_TASK_MIN_MODULE_PROVISION_SESSION_ID    (CLI_TASK_BACKDOOR_SESSION_ID + 1)
#define CLI_TASK_MAX_MODULE_PROVISION_SESSION_ID    (CLI_TASK_BACKDOOR_SESSION_ID + CLI_TASK_MAX_MODULE_PROVISION_SESSION_NUM)
#define CLI_TASK_MAX_USER_SESSION_ID                CLI_TASK_MAX_TELNET_SESSION_ID

#define CLI_TASK_NULL_SESS_ID           255                                         /* Null cli session id */

#define CLI_TASK_INCREASE_COUNTER       0                                           /* used by CLI_TASK_adjTnSessCounter, plus 1 */
#define CLI_TASK_DECREASE_COUNTER       1                                           /* used by CLI_TASK_adjTnSessCounter, minus 1 */

#define CLI_TASK_CONSOLE_STR            "console"
#define CLI_TASK_TELNET_STR             "VTY"
#define CLI_TASK_SSH_STR                "SSH"
#define CLI_TASK_TELNET_FORMAT_STR      CLI_TASK_TELNET_STR" %lu"
#define CLI_TASK_SSH_FORMAT_STR         CLI_TASK_SSH_STR" %lu"

#ifndef	MAX
#define MAX(x1,x2)	((x1) >= (x2) ? (x1) : (x2))
#endif

#ifndef	MIN
#define MIN(x1,x2)	((x1) <= (x2) ? (x1) : (x2))
#endif

/* MAX 99 session
 */
#define CLI_TASK_MAX_LINE_STR_LEN       (MAX(sizeof(CLI_TASK_CONSOLE_STR), MAX(sizeof(CLI_TASK_TELNET_STR" dd"), sizeof(CLI_TASK_SSH_STR" dd"))) -1)

#if (SYS_CPNT_IPV6 == TRUE)
#define CLI_TASK_LOCATION_STR_LEN   L_INET_MAX_IP6ADDR_STR_LEN
#else
#define CLI_TASK_LOCATION_STR_LEN   L_INET_MAX_IP4ADDR_STR_LEN
#endif

typedef struct
{
   UI32_T idel_time;
   char username[SYS_ADPT_MAX_USER_NAME_LEN + 1];
   char line[CLI_TASK_MAX_LINE_STR_LEN + 1];
   char location[CLI_TASK_LOCATION_STR_LEN + 1];
} CLI_TASK_OnlineUserInfo_T;


enum CLI_TASK_LINE_E
{
   CLI_TASK_LINE_CONSOLE = 0,
   CLI_TASK_LINE_VTY_0,
   CLI_TASK_LINE_VTY_1,
   CLI_TASK_LINE_VTY_2,
   CLI_TASK_LINE_VTY_3,
   CLI_TASK_LINE_VTY_4,
/*92.03.27 pttch add for disconnect session*/
   CLI_TASK_LINE_SSH_0,
   CLI_TASK_LINE_SSH_1,
   CLI_TASK_LINE_SSH_2,
   CLI_TASK_LINE_SSH_3,
   CLI_TASK_LINE_SSH_4
};

/*------------------------------------------------------------------------
 *                     DATA STRUCTURE DEFINITION
 *------------------------------------------------------------------------*/
/* cli working area structure
 */
typedef enum CLI_TASK_ProvisionEvents_E
{
    CLI_TASK_Prov = 0,

} CLI_TASK_ProvisionEvents_T;


typedef enum CLI_TASK_SessionType_E
{
   CLI_TASK_UART = 1,         		   /* cli connection type: uart */
   CLI_TASK_TELNET,			           /* cli connection type: telnet */
/*92.03.27 pttch add for disconnect session*/
   CLI_TASK_SSH,			           /* cli connection type: ssh */
   CLI_TASK_PROVISION,                 /* cli connection type: provision */
} CLI_TASK_SessionType_T;

UI32_T  CLI_MGR_ReadSystemConfig(char *xbuffer, UI32_T *xbuf_length);
/*-----------------------------------------------------------
 * ROUTINE NAME - Del
 *-----------------------------------------------------------
 * FUNCTION:Delete task
 * INPUT   :CLI_TASK_WorkingArea_T *WorkingSpace
 * OUTPUT  :None.
 * RETURN  :None.
 * NOTE    :None.
 *----------------------------------------------------------*/
void CLI_TASK_Del(CLI_TASK_WorkingArea_T *WorkingSpace);


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
void CLI_TASK_Init(void);


/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_SetTransitionMode();
 *-----------------------------------------------------------
 * FUNCTION:Set task to transition mode
 * INPUT   :None.
 * OUTPUT  :None.
 * RETURN  :None.
 * NOTE    :Must be called before any cli session task
 *----------------------------------------------------------*/
void CLI_TASK_SetTransitionMode();

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_EnterTransitionMode();
 *-----------------------------------------------------------
 * FUNCTION:Force task to transition mode
 * INPUT   :None.
 * OUTPUT  :None.
 * RETURN  :None.
 * NOTE    :Must be called before any cli session task
 *----------------------------------------------------------*/
void CLI_TASK_EnterTransitionMode();
/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_AllocatTelnetWorkingSpace
 *------------------------------------------------------------------------
 * PURPOSE :
 * INPUT   :
 * OUTPUT  :
 * RETURN  :
 * NOTE    :
 *------------------------------------------------------------------------*/
void *CLI_TASK_AllocatTelnetWorkingSpace(void);

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_FreeTelnetWorkingSpace
 *------------------------------------------------------------------------
 * PURPOSE :
 * INPUT   :
 * OUTPUT  :
 * RETURN  :
 * NOTE    :
 *------------------------------------------------------------------------*/
void CLI_TASK_FreeTelnetWorkingSpace(void *WorkingSpace);

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_SetTelnetSessionContext
 *------------------------------------------------------------------------
 * PURPOSE :
 * INPUT   :
 * OUTPUT  :
 * RETURN  :
 * NOTE    :
 *------------------------------------------------------------------------*/
BOOL_T  CLI_TASK_SetTelnetSessionContext(void *WorkingSpace, UI32_T tid, I32_T socket_id);

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_SessionMain
 *------------------------------------------------------------------------
 * PURPOSE :
 * INPUT   :
 * OUTPUT  :
 * RETURN  :
 * NOTE    :
 *------------------------------------------------------------------------*/
void CLI_TASK_TelnetSessionMain(void *ctrl_P);




/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_CreateTask
 *------------------------------------------------------------------------
 * PURPOSE: This function creates the CLI session task.
 * INPUT   : 1. session_type
 *           2. socket_id
 *           3. *buffer
 *           4. buffer_size
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    : 1.This function shall not be invoked before CLI_INIT_Init() is executed.
 *           2.socket_id is valid only if CLI_TYPE_SessionType_T != CLI_TYPE_UART
 *           3.*buffer and buffer_size is used for provision cli task only
 *
 *------------------------------------------------------------------------*/
BOOL_T CLI_TASK_CreateTask(UI32_T cli_session_type, I32_T  socket_id,UI32_T unit, char *prov_buffer, UI32_T prov_buffer_len);


/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetMyWorkingArea
 *-----------------------------------------------------------
 * FUNCTION: Get working buffer
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : addr - Currnet address
 *----------------------------------------------------------*/
CLI_TASK_WorkingArea_T *CLI_TASK_GetMyWorkingArea(void);
CLI_TASK_WorkingArea_T *CLI_TASK_GetProvisionArea(void);


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
void CLI_TASK_TnSessExit(CLI_TASK_WorkingArea_T *ctrl_P);


/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetMySessId
 *------------------------------------------------------------------------
 * FUNCTION: Get session id of calling cli session
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : cli seesion serial number (session id)
 * NOTE    : assume existence of cli_working_buffer[] structure
 *-----------------------------------------------------------------------*/
UI8_T CLI_TASK_GetMySessId(void);

/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetOnLineUserInfoByIndex
 *------------------------------------------------------------------------
 * FUNCTION: Get next information of online user
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  :
 * NOTE    :
 *-----------------------------------------------------------------------*/
BOOL_T CLI_TASK_GetOnLineUserInfoByIndex(UI32_T index, CLI_TASK_OnlineUserInfo_T *user_info);


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
BOOL_T CLI_TASK_SetKillWorkingSpaceFlag(UI32_T sessid);



/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetKillWorkingSpaceFlag
 *------------------------------------------------------------------------
 * FUNCTION: Get kill_itself flag of working space
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : BOOL_T : value of kill_itself
 * NOTE    :
 *-----------------------------------------------------------------------*/
BOOL_T CLI_TASK_GetKillWorkingSpaceFlag();

#if(SYS_CPNT_CLUSTER==TRUE)
/*------------------------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_SetTelnetRelayingFlag
 *------------------------------------------------------------------------
 * FUNCTION: Set responding telnet relaying status
 * INPUT   : tnpdTaskId, isRelaying
 * OUTPUT  : none
 * RETURN  : none
 * NOTE    :
 *-----------------------------------------------------------------------*/
void CLI_TASK_SetTelnetRelayingFlag(UI32_T tnpdTaskId, BOOL_T isRelaying);
#endif

void CLI_TASK_CloseTelnetSessions(void );
void CLI_TASK_TelnetIPFilter(void);

#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_TASK_GetWorkingAreaByIndex
 *-----------------------------------------------------------
 * FUNCTION: Get working buffer by index
 * INPUT   : UI32_T index
 * OUTPUT  : None
 * RETURN  :
 *----------------------------------------------------------*/
CLI_TASK_WorkingArea_T *CLI_TASK_GetWorkingAreaByIndex( UI32_T index);
#endif /* SYS_CPNT_SYSMGMT_DEFERRED_RELOAD */

BOOL_T CLI_TASK_GET_IPC_Handle( UI32_T * handler_p);

BOOL_T CLI_TASK_Reproduce_Config(char *filename);

BOOL_T  CLI_TASK_SetSessionContext(CLI_TASK_WorkingArea_T *WorkingSpace,
                                   UI32_T tid,
                                   I32_T  socket_id,
                                   UI32_T sess_type,
                                   UI32_T unit,
                                   char  *prov_buffer,
                                   UI32_T prov_buffer_len);


#endif
