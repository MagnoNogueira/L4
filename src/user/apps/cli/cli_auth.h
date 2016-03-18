#ifndef CLI_AUTH_H
#define CLI_AUTH_H

#include "sys_type.h"
#include "userauth.h"

BOOL_T CLI_AUTH_CheckLoginPassword(CLI_TASK_WorkingArea_T *ctrl_P, USERAUTH_AuthResult_T *auth_result_p);
BOOL_T CLI_AUTH_CheckEnablePassword(CLI_TASK_WorkingArea_T *ctrl_P, UI32_T *privilege);
#if (SYS_CPNT_3COM_CLI == TRUE)
BOOL_T CLI_AUTH_isDoingPasswordRecovery();
#endif
#endif
