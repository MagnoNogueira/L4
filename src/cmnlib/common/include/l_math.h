#ifndef _L_MATH_H
#define _L_MATH_H
#include "sys_type.h"

/* NAMING CONSTANT
 */

#define	INVALID_ACTIVE_PORT_NUMBER		0xffffffff

/* MACRO DECLARATIONS
 */

/* the following two MACRO definitions work for gap between timeout_tick and current_tick is less than half of 2**32 or 2**16
 */
#define L_MATH_TimeOut32(current_tick, timeout_tick) ((I32_T)((UI32_T)(timeout_tick) - (UI32_T)(current_tick)) <= 0 )
#define L_MATH_TimeOut16(current_tick, timeout_tick) ((I16_T)((UI16_T)(timeout_tick) - (UI16_T)(current_tick)) <= 0 )

/* EXPORTED FUNCTIONS DECLARATION
 */
/* Lyn Yeh, 12/11/01, 11:26:07 */
UI32_T L_MATH_SelectBalancePort(UI32_T key, UI32_T nbr_of_active_port, UI32_T algorithm);

UI32_T L_MATH_CheckSum(void *start_addr, UI32_T length);

UI16_T L_MATH_CheckSum16 (UI16_T *ptr, UI32_T nbytes);

UI32_T L_MATH_Crc32(UI32_T crc, const I8_T *buf, UI32_T length);

#endif
