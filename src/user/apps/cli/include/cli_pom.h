/* MODULE NAME:  CLI_pom.h
 * PURPOSE:
 *    POM implement for CLI.
 *
 * NOTES:
 *
 * REASON:
 * Description:
 * HISTORY
 *    07/30/2007 - Rich Lee, Created
 *
 * Copyright(C)      Accton Corporation, 2007
 */
#ifndef CLI_POM_H
#define CLI_POM_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "stktplg_om.h"
#include "sys_dflt.h"
#include "sys_adpt.h"
/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_POM_GetFacilityType
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_OM_GetFacilityType through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    port
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
UI8_T CLI_POM_GetStackingDB(STKTPLG_OM_StackingDB_T stacking_db[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK]);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_POM_GetConfiguartionModuleTypes
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_OM_GetConfiguartionModuleTypes through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    port
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T CLI_POM_GetConfiguartionModuleTypes(UI32_T  module_types[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK]);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_POM_GetUartHandler
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_OM_GetUartHandler through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    uart_handler_p.
 *
 * RETURN:
 *    TRUE/FALSE
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T CLI_POM_GetUartHandler(UI32_T *uart_handler_p);

/*-----------------------------------------------------------------------------
 * ROUTINE NAME : CLI_POM_InitiateProcessResource
 *-----------------------------------------------------------------------------
 * PURPOSE : Initiate resource for CLI_POM in the calling process.
 *
 * INPUT   : None.
 *
 * OUTPUT  : None.
 *
 * RETURN  : TRUE  --  Sucess
 *           FALSE --  Error
 *
 * NOTES   : None.
 *-----------------------------------------------------------------------------
 */
BOOL_T CLI_POM_InitiateProcessResource(void);

#endif


