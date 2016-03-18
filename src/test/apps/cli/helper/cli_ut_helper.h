//
//  cli_ut_helper.h
//  L4
//
//  Created by Swei Chen on 2014/11/27.
//
//

#ifndef __L4__cli_ut_helper__
#define __L4__cli_ut_helper__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "sys_type.h"
#include "cli_def.h"

#ifndef _countof
#define _countof(ar)    (sizeof(ar)/sizeof(*ar))
#endif

void
CLI_UT_Proc_Run_Command(
    UI16_T cmd_idx,
    const char *cmd_line,
    CLI_TASK_WorkingArea_T *ctrl_p,
    UI32_T (*fn)(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
);

#endif /* defined(__L4__cli_ut_helper__) */
