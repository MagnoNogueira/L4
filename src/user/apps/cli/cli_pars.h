#ifndef CLI_PARS_H
#define CLI_PARS_H

#include "sys_type.h"

#define CLI_PARS_ARG_MATCH       0
#define CLI_PARS_ARG_SUBMATCH    1
#define CLI_PARS_ARG_NOMATCH     2
#define CLI_PARS_ARG_NOIMPL     (-1)


UI8_T  CLI_PARS_GetKeyState(CLI_TASK_WorkingArea_T *ctrl_P);
void   CLI_PARS_ResetKeyState(CLI_TASK_WorkingArea_T *ctrl_P);
void   CLI_PARS_SetKeyState(UI8_T status, CLI_TASK_WorkingArea_T *ctrl_P);
UI16_T CLI_PARS_GetKey(UI8_T *ch);
UI16_T CLI_PARS_TransLowUpper(char *buf, I16_T si, UI16_T key_type);
UI16_T CLI_PARS_GetKeyForwardSize(char *buf, I16_T si);
UI16_T CLI_PARS_GetKeyBackwardSize(char *buf, I16_T si);
UI16_T CLI_PARS_ReadLine(char *buff, I16_T bufsize, BOOL_T Is_CtrlKey_Skipped, BOOL_T Is_Hidden);
UI16_T CLI_PARS_GetAWord(char *buff, UI16_T *str_idx, char *word_buf);
UI16_T CLI_PARS_GetWords(UI8_T *buff, UI8_T *words[], UI16_T widx[], UI16_T max);

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_PARS_GetNeiborCommandIndex
 *---------------------------------------------------------------------------
 * PURPOSE  : Get neibor command index from starting index
 * INPUT    : cmd_lst       - command list
 *            start_index   - starting command index
 * OUTPUT   : None
 * RETURN   : command index or NULL_CMD
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
UI16_T
CLI_PARS_GetNeiborCommandIndex(
    const CMDLIST * cmd_list,
    UI16_T start_idx
);

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_PARS_GetCommandIndex
 *---------------------------------------------------------------------------
 * PURPOSE  : Get command index from starting index
 * INPUT    : cmd_lst       - command list
 *            start_index   - starting command index
 * OUTPUT   : None
 * RETURN   : command index or NULL_CMD
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
UI16_T
CLI_PARS_GetCommandIndex(
    const CMDLIST * cmd_list,
    UI16_T start_idx
);

/************************************************************************/
/* CLI_PARS_LookupCmdWord                                               */
/*                                                                      */
/* Description:                                                         */
/*     Lookup a command word in the command words list                  */
/*                                                                      */
/* Arguments:                                                           */
/*     UI8_T    *word_buf : word pointer                                */
/*     UI16_T   start_idx : the start point that to be searched         */
/*     UI16_T   *near_idx : the nearest matched command word index      */
/*                                                                      */
/* Return:                                                              */
/*     CLI_NO_ERROR          => when exactly match one command word     */
/*     CLI_ERR_CMD_INVALID   => when not match any command word         */
/*     CLI_ERR_CMD_AMBIGUOUS => when found ambiguous words              */
/************************************************************************/
UI16_T
CLI_PARS_LookupCmdWord(
    char *word_buf,
    UI16_T start_idx,
    UI16_T *near_idx
);

UI8_T  *CLI_PARS_TOKEN_UTIL_Get_Token(UI8_T *s, UI8_T *Token);


/************************************************************************/
/* CLI_PARS_RegisterArgumentParseHandler                                */
/*                                                                      */
/* Description:                                                         */
/*     Register parse function for argument                             */
/*                                                                      */
/* Arguments:                                                           */
/*     None                                                             */
/*                                                                      */
/* Output:                                                              */
/*     None                                                             */
/*                                                                      */
/* Return:                                                              */
/*     None                                                             */
/*                                                                      */
/* Invoke this function before use CLI_PARS_ParseArgument and           */
/* CLI_PARS_GetArgumentHelpMessage.                                     */
/*                                                                      */
/************************************************************************/
void CLI_PARS_RegisterArgumentParseHandler();


/************************************************************************/
/* CLI_PARS_ParseArgument                                               */
/*                                                                      */
/* Description:                                                         */
/*     Parse an argument                                                */
/*                                                                      */
/* Arguments:                                                           */
/*     token     : Argument. The token may not a null-terminal string   */
/*     token_len : String of the argument                               */
/*     arg_list  : Agurment array list                                  */
/*     arg_idx   : Current argument index                               */
/*                                                                      */
/* Output:                                                              */
/*     match_len : How many character of token be read                  */
/*     name      : Help message for 1st column                          */
/*     help      : Help message for 2nd column                          */
/* Return:                                                              */
/*     ARG_MATCH             => when exactly match                      */
/*     ARG_SUBMATCH          => sub match                               */
/*     ARG_NOMATCH           => no match                                */
/************************************************************************/
int
CLI_PARS_ParseArgument(
    const char *token,
    UI32_T token_len,
    const ARGLIST arg_list[],
    UI16_T  arg_idx,
    UI16_T *match_len,
    UI16_T *err_kind
);


/************************************************************************/
/* CLI_PARS_GetArgumentHelpMessage                                      */
/*                                                                      */
/* Description:                                                         */
/*     Get help message of an argument                                  */
/*                                                                      */
/* Arguments:                                                           */
/*     token     : Argument. The token may not a null-terminal string   */
/*     token_len : String of the argument                               */
/*     arg_list  : Agurment array list                                  */
/*     arg_idx   : Current argument index                               */
/*                                                                      */
/* Output:                                                              */
/*     name      : Help message for 1st column                          */
/*     help      : Help message for 2nd column                          */
/* Return:                                                              */
/*     ARG_MATCH             => when exactly match                      */
/*     ARG_SUBMATCH          => sub match                               */
/*     ARG_NOMATCH           => no match                                */
/************************************************************************/
int
CLI_PARS_GetArgumentHelpMessage(
    const char *token,
    UI32_T token_len,
    const ARGLIST arg_list[],
    UI16_T  arg_idx,
    char *name,
    UI32_T name_len,
    char *help,
    UI32_T help_len
);

#endif
