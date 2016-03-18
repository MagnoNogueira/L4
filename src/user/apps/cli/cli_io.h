#ifndef CLI_IO_H
#define CLI_IO_H

#include "cli_def.h"

/* Key code definition
 */
enum KEY_CODE_E
{
    ENTER = 0,
    UP,
    DOWN,
    RIGHT,
    LEFT,
    ESC,
    CTRL_A,
    CTRL_B,
    CTRL_C,
    CTRL_D,
    CTRL_E,
    CTRL_F,
    CTRL_G,
    BS,
    TAB,
    CTRL_K,
    CTRL_L,
    CTRL_N,
    CTRL_P,
    CTRL_R,
    CTRL_T,
    CTRL_U,
    CTRL_V,
    CTRL_W,
    CTRL_X,
    CTRL_Y,
    CTRL_Z,
    DEL,
    ESC_DE,
    ESC_B,
    ESC_C,
    ESC_D,
    ESC_F,
    ESC_L,
    ESC_Q,
    ESC_U,
    ESC_Y,
    PRNTABLE,
    QUMARK,
    KEY_TIMEOUT,
    KEYIN_ERROR,
    CLI_IO_MENU_TIMEOUT,
    CLI_IO_MENU_FAIL,
    CLI_IO_MENU_OK,
    UNKNOWN
};


/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_OpenUart                           |
 *-----------------------------------------------------------|
 * PURPOSE : Open uart .             |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  :None.                                             |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
BOOL_T CLI_IO_OpenUart(void);

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_GetUartHandler                           |
 *-----------------------------------------------------------|
 * PURPOSE : Get uart Handler.             |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  :None.                                             |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
BOOL_T CLI_IO_GetUartHandler(UI32_T *uart_handler_p);

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_GetKey                              |
 *-----------------------------------------------------------|
 * PURPOSE : Get byte or escape sequence                     |
 * INPUT   : None.                                           |
 * OUTPUT  : if the return value is PRNTABLE, the *ch is     |
 *           meaning.                                        |
 * RETURN  : Key code definition                             |
 * NOTE    : Escape sequence define                          |
 *              up    : 0x1B 0x5B 0x41                       |
 *              down  : 0x1B 0x5B 0x42                       |
 *              right : 0x1B 0x5B 0x43                       |
 *              left  : 0x1B 0x5B 0x44                       |
 *           ASCII control set                               |
 *              BEL: 0x07                                    |
 *              BS : 0x08                                    |
 *              TAB: 0x09                                    |
 *              ..........                                   |
 *-----------------------------------------------------------*/
UI16_T CLI_IO_GetKey(char *ch);


/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_ReadACharNoWait                     |
 *-----------------------------------------------------------|
 * PURPOSE : Read a char from console or telnet without      |
 *           blocking.                                       |
 *           Wrap function for CLI_IO_ReadACharFromConsole   |
 *           CLI_IO_ReadACharFromTelnet.                     |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  : A byte.   - Read a byte success, or 0 for fail  |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
UI8_T CLI_IO_ReadACharNoWait();


/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_PrintOut                            |
 *-----------------------------------------------------------|
 * PURPOSE : Print out the string in the send_buf in working |
 *           space.                                          |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  : None.                                           |
 * NOTE    : This funuction just handle the way to print out,|
 *           the other service rutines are in CLI_LIB, and   |
 *           this function only service the function in the  |
 *           CLI_LIB.                                        |
 *-----------------------------------------------------------*/
void CLI_IO_PrintOut(CLI_TASK_WorkingArea_T *ctrl_P);

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_ReadACharFromConsole                |
 *-----------------------------------------------------------|
 * PURPOSE : Replace by CLI_IO_ReadACharNoWait               |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  : A byte.   - Read a byte success, or 0 for fail  |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
UI8_T CLI_IO_ReadACharFromTelnet(CLI_TASK_WorkingArea_T *ctrl_P);

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_ReadACharFromConsole                |
 *-----------------------------------------------------------|
 * PURPOSE : Replace by CLI_IO_ReadACharNoWait               |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  : A byte.   - Read a byte success, or 0 for fail  |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
UI8_T CLI_IO_ReadACharFromConsole(CLI_TASK_WorkingArea_T *ctrl_P);

void CLI_IPCIO_ParseKey(UI32_T msgq_handle, char ch, UI32_T *need_wait_key_p, char *buff);
BOOL_T CLI_IPCIO_HandleIPCReqMsg(SYSFUN_Msg_T *msg_p, UI32_T *need_wait_key_p);
#if (SYS_CPNT_3COM_CLI == TRUE)
UI16_T CLI_IO_Menu_GetKey(UI8_T *ch, UI32_T timeout);
#endif

/*------------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_DebugMode_GetChar                    |
 *------------------------------------------------------------|
 * PURPOSE : for backdoor mgr to get char                     |
 * INPUT   : cookie (sess_id)                                 |
 * OUTPUT  : none                                             |
 * RETURN  : a char or EOF if failed                          |
 *------------------------------------------------------------*/
int CLI_IO_DebugMode_GetChar(void *cookie);

/*------------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_DebugMode_Print                      |
 *------------------------------------------------------------|
 * PURPOSE : for backdoor mgr to print string                 |
 * INPUT   : cookie (sess_id)                                 |
 *           s - string                                       |
 * OUTPUT  : none                                             |
 * RETURN  : >= 0 - successful                                |
 *           < 0  - failed                                    |
 *------------------------------------------------------------*/
int CLI_IO_DebugMode_Print(void *cookie, const char *s);

#endif

