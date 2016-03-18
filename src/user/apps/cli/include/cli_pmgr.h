/* MODULE NAME:  cli_pmgr.h
 * PURPOSE:
 *    PMGR for cli 
 *
 * NOTES:
 *
 * REASON:
 * Description:
 * HISTORY
 *    06/02/2007 - rich Lee, Created for Linux platform.
 *
 * Copyright(C)      Accton Corporation, 2007
 */
#ifndef CLI_PMGR_H
#define CLI_PMGR_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_PMGR_InitiateProcessResource
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Initiate resource used in the calling process.
 * INPUT:
 *    None.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    TRUE  --  Sucess
 *    FALSE --  Error
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_InitiateProcessResource(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_PMGR_AllCscsKnowProvisionComplete
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_MGR_AllCscsKnowProvisionComplete through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    *state --  current ssh status.
 *
 * RETURN:
 *    TRUE   -- Success
 *    FALSE -- Fail
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void CLI_PMGR_AllCscsKnowProvisionComplete(void);

void CLI_PMGR_Notify_EnterTransitionMode(void *prov_buf);

UI32_T CLI_PMGR_Get_RunningCfg(void *buffer, UI32_T buffer_size);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_IncreaseRemoteSession
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will increase counter of remote seesion
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE  -- success.
 *                     FLASE -- fail.
 * NOTE     : Maximux of total session is 4 include telnet and ssh
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_IncreaseRemoteSession(void);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_DecreaseRemoteSession
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will decrease counter of remote seesion
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE  -- success.
 *                     FLASE -- fail.
 * NOTE     : Maximux of total session is 4 include telnet and ssh
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_DecreaseRemoteSession(void);

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_SetKillWorkingSpaceFlag
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will set the kill_itself flag to TRUE for
 *              specified sessid.
 * INPUT    : sessid
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE /FALSE
 * NOTE     : Maximux of total session is 4 include telnet and ssh
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_SetKillWorkingSpaceFlag(UI32_T sessid);

#if (SYS_CPNT_TELNET == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_CloseAllTelnetSessions
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will close all telnet sessions.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_CloseAllTelnetSessions();
#endif /* #if (SYS_CPNT_TELNET == TRUE) */

#if (SYS_CPNT_MGMT_IP_FLT == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_HandleChangedIpMgmtFilter
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will handle changed IP mgmt filter
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_HandleChangedIpMgmtFilter();
#endif /* #if (SYS_CPNT_MGMT_IP_FLT == TRUE) */

#endif    /* End of CSCA_PMGR_H */

