/*--------------------------------------------------------------------------
 * Project : Mercury (BCM solution)
 * Creator : Chin-Ju Chu
 * File    : cli_main.h
 * Abstract: cli_main.c header file
 *
 * Note    : Inherit from Hornet & Poppadom
 *------------------------------------------------------------------------*/

#ifndef CLI_MAIN_H
#define CLI_MAIN_H

#include "sys_type.h"
#include "sysfun.h"

/* ------------------------------------------------------------------------- */
/* Read a byte from UART or TELNET socket                                    */
/* ------------------------------------------------------------------------- */
UI8_T   CLI_MAIN_Read_Byte(void);


/* ------------------------------------------------------------------------- */
/* Called in uis menu task for user active exit, or .....                    */
/* ------------------------------------------------------------------------- */
void    CLI_MAIN_Active_Exit(UI16_T reason, CLI_TASK_WorkingArea_T *ctrl_P);

/* ------------------------------------------------------------------------- */
/* Initial CLI menu table                                                    */
/* ------------------------------------------------------------------------- */
void    CLI_MAIN_Init_Menu_Table(CLI_TASK_WorkingArea_T * ctrl_P);

void CLI_MAIN_Enter_Main_Routine(CLI_TASK_WorkingArea_T *ctrl_P);

void CLI_MAIN_Enter_DebugMode();

/*****************************************************************************/
/* ROUTINE NAME - CLI_MAIN_HandleMGR_Thread()                                                     */
/* ------------------------------------------------------------------------- */
/* FUNCTION: Main MGR function.                                                  */
/*                                                                           */
/* RETURN  : none.                                                           */
/* INPUT   : sleep ticks                                                     */
/* OUTPUT  :                                                                 */
/* NOTE    : This is the main mgr thread for this module.                    */
/*****************************************************************************/

/* this function will work like mgr thread */
void CLI_MAIN_HandleMGR_Thread(UI32_T ticks, UI8_T ch, UI8_T *buff);
void CLI_IPCIO_PRINT(char *str);
void CLI_MAIN_SET_WAIT_KEY(UI32_T key);
void CLI_MAIN_GET_WAIT_KEY(UI32_T *key);

#if (SYS_CPNT_UNIT_HOT_SWAP == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_MAIN_HandleHotInertion
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This function will invoke a new dut insertion in CSCGroup1.
 *
 * INPUT:
 *    starting_port_ifindex.
      number_of_port
      use_default
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    None.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void CLI_MAIN_HandleHotInertion(SYSFUN_Msg_T* msgbuf_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_MAIN_ HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This function will invoke a new dut removal in CSCGroup1.
 *
 * INPUT:
 *    starting_port_ifindex.
      number_of_port
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    None.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void CLI_MAIN_HandleHotRemoval(SYSFUN_Msg_T* msgbuf_p);
#endif

#endif

