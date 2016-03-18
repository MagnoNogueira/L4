#ifndef CLI_RUNCFG_H
#define CLI_RUNCFG_H

enum CLI_RUNCFG_RETURN_TYPE_E
{
   CLI_RUNCFG_RETURN_OK = 0,
   CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY = 0xffffffff
};

/*system*/

/*syb-systems*/
#include "sys_type.h"

/*cli internal*/

/*macros*/

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_RUNCFG_Get_RunningCfg                 
 *-----------------------------------------------------------
 * FUNCTION: This is a running config generator. Use the function
 *           call to collect the configuration of the system, and
 *           store in the style of CLI command.
 * INPUT   : buffer      - The pointer to the starting address to
 *                         store the configuration.
 *         : buffer_size - The size of the buffer to store the 
 *                         configuration.
 * OUTPUT  : None.
 * RETURN  : CLI_RUNCFG_RETURN_OK - Collect configuration successfully.
 *           CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY - Buffer size is too 
 *                                  to collect all configuration.
 * NOTE    : None.
 *----------------------------------------------------------*/
UI32_T CLI_RUNCFG_Get_RunningCfg(char *buffer, UI32_T buffer_size);

/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_RUNCFG_Get_Interface_RunningCfg
 *-----------------------------------------------------------
 * FUNCTION: This is a running config generator. Use the function
 *           call to collect the configuration of specific interface,
 *           and store in the style of CLI command.
 * INPUT   : buffer      - The pointer to the starting address to
 *                         store the configuration.
 *         : buffer_size - The size of the buffer to store the
 *                         configuration.
 * OUTPUT  : None.
 * RETURN  : CLI_RUNCFG_RETURN_OK - Collect configuration successfully.
 *           CLI_RUNCFG_RETURN_NO_ENOUGH_MEMORY - Buffer size is too
 *                                  to collect all configuration.
 * NOTE    : None.
 *----------------------------------------------------------*/
UI32_T CLI_RUNCFG_Get_Interface_RunningCfg(char *buffer, UI32_T buffer_size, char *arg[]);

#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_RUNCFG_Get_DeviceRunningCfg                 
 *-----------------------------------------------------------
 * FUNCTION: This is a module running config generator. Use the function
 *           call to collect the configuration of the system, and
 *           store in the style of CLI command.
 * INPUT   : buffer      - The pointer to the starting address to
 *                         store the configuration.
 *         : buffer_size - The size of the buffer to store the 
 *                         configuration.
 *         : starting_port_ifindex - The start module port to generate start config index
 *         : number_of_port - number of module ports
 * OUTPUT  : None.
 * RETURN  : buffer count 
 * NOTE    : None.
 *----------------------------------------------------------*/
UI32_T CLI_RUNCFG_Get_DeviceRunningCfg(UI32_T starting_port_ifindex, UI32_T number_of_port, char *buffer, UI32_T buffer_size);
#else
/*-----------------------------------------------------------
 * ROUTINE NAME - CLI_RUNCFG_Get_ModuleRunningCfg                 
 *-----------------------------------------------------------
 * FUNCTION: This is a module running config generator. Use the function
 *           call to collect the configuration of the system, and
 *           store in the style of CLI command.
 * INPUT   : buffer      - The pointer to the starting address to
 *                         store the configuration.
 *         : buffer_size - The size of the buffer to store the 
 *                         configuration.
 *         : starting_port_ifindex - The start module port to generate start config index
 *         : number_of_port - number of module ports
 * OUTPUT  : None.
 * RETURN  : buffer count 
 * NOTE    : None.
 *----------------------------------------------------------*/
UI32_T CLI_RUNCFG_Get_ModuleRunningCfg(UI32_T starting_port_ifindex, UI32_T number_of_port, char *buffer, UI32_T buffer_size);
#endif
#endif
