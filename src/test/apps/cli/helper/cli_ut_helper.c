//
//  cli_ut_helper.c
//  L4
//
//  Created by Swei Chen on 2014/11/27.
//
//

#include "cli_ut_helper.h"

void
CLI_UT_Proc_Run_Command(
    UI16_T cmd_idx,
    const char *cmd_line,
    CLI_TASK_WorkingArea_T *ctrl_p,
    UI32_T (*fn)(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P))
{
    CLI_TASK_WorkingArea_T ctrl;
    UI32_T i;

    char *arg[255] =
    {
        NULL
    };

    char *str;
    char *delim = " \t";

    assert(cmd_line != NULL);

    str = malloc((strlen(cmd_line) + 1) * sizeof(*cmd_line));
    assert(NULL != str);

    strcpy(str, cmd_line);

    for (i = 0; i < _countof(arg); ++ i)
    {
        char *p;
        char *token = strtok_r(str, delim, &p);
        char **cur_arg = &arg[i];

        if (token == NULL)
        {
            break;
        }

        *cur_arg = token;

        str = p;
    }

    if (NULL != ctrl_p)
    {
        fn(cmd_idx, arg, ctrl_p);
    }
    else
    {
        fn(cmd_idx, arg, &ctrl);
    }
    
    free(str);
}


