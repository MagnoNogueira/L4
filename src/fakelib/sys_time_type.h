/* MODULE NAME:  sys_time_type.h
 * PURPOSE:
 *      Define definitions which are private to SYS_TIME_DRV.
 * NOTES:
 *      None.
 * HISTORY
 *    2007/08/07 - Echo Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2007
 */
#ifndef SYS_TIME_DRV_TYPE_H
#define SYS_TIME_DRV_TYPE_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sysfun.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
typedef struct
{
    /* text string buffer  is max text length + 1
     * Zhong Qiyao, 2004.07.26
     */
    UI8_T  zone_name[MAXSIZE_sysTimeZoneName + 1];
#if 0
    UI8_T  zone_name[MAX_ZONE_NAME];
#endif

    int    offset; /*In minutes */

} Timezone_T;

typedef struct
{

    UI32_T time;        /* seconds from 2001/01/01 00:00:00 */
    UI32_T sys_tick;    /* system ticks of time */

} Software_clock_T;

typedef struct 
{
    SYSFUN_DECLARE_CSC_ON_SHMEM

    // this 3 variablies for watchdog
    BOOL_T wd_enable         ;//= FALSE;
    BOOL_T wd_kick          ; //= FALSE;
    BOOL_T wd_kick_counter   ;//= 0;

    Timezone_T time_zone;
    Software_clock_T software_clock;
    UI32_T provision_complete_ticks;
    UI32_T is_provision_complete;
}  SYS_TIME_Shmem_Data_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

#endif    /* SYS_TIME_DRV_TYPE_H */

