/* Module Name: STKTPLG_BOARD.H
 *
 * Purpose: 
 *
 * Notes:
 *
 * History:
 *  
 *
 * Copyright(C)      Accton Corporation, 2002 - 2005
 */
 
#ifndef  STKTPLG_BOARD_H
#define  STKTPLG_BOARD_H


/* INCLUDE FILE DECLARATIONS
 */

#include "sys_type.h"

/* DATA TYPE DECLARATIONS
 */

static void inline STKTPLG_BOARD_GetCraftPortInfo(UI8_T *device_id_p, UI8_T *device_port_id)
{
    *device_id_p = 0;
    *device_port_id = 26;
}

#endif /* STKTPLG_BOARD_H */

