#ifndef SYS_TIME_H
#define SYS_TIME_H

/* ------------------------------------------------------------------------
 *  FILE NAME  -  SYS_TIME.H
 * ------------------------------------------------------------------------
 * PURPOSE: This package provides the services to handle the system timer
 *          functions
 *
 * Notes: This module will support all the system timer functions includes
 *        1) System Ticks
 *        2) RTC timer setting/getting if the H/W support RTC on the board
 *
 *  History
 *
 *   Jason Hsue     11/19/2001      new created
 *   S.K.Yang       05/25/2002      modified
 *   1.Modified SYS_TIME_GetRealTimeBySec,SYS_TIME_GetRealTimeClock,SYS_TIME_SetRealTimeClock
 *   2.Add software clock for situation of non-RTC machine.
 *
 * ------------------------------------------------------------------------
 * Copyright(C)  ACCTON Technology Corp. , 2001
 * ------------------------------------------------------------------------
 */
#include "sysfun.h"
#include "sys_type.h"
#include "sys_cpnt.h"

#ifdef EPON
#include "leaf_eltbox.h"
#else
#include "leaf_es3626a.h"
#endif

#define SYS_TIME_MIN_CONFIGURABLE_YEAR  1970  /* for linux system */
#define SYS_TIME_MAX_CONFIGURABLE_YEAR  2037  /* for linux system */

typedef struct
{
    UI8_T zone_name[MAXSIZE_sysTimeZoneName + 1];
    int   offset; /* minutes */
} Timezone_T;

typedef struct
{
    UI32_T time;     /* seconds from 2001/01/01 00:00:00 */
    UI32_T sys_tick; /* system ticks of time */
} Software_clock_T;

typedef struct
{
    SYSFUN_DECLARE_CSC_ON_SHMEM

    // this 3 variablies for watchdog
    BOOL_T wd_enable;//= FALSE;
    BOOL_T wd_kick;  //= FALSE;
    BOOL_T wd_kick_counter;//= 0;

    Timezone_T time_zone;
    Software_clock_T software_clock;
    UI32_T provision_complete_ticks;
    UI32_T is_provision_complete;
    UI32_T seq_no;
} SYS_TIME_Shmem_Data_T;

#if (SYS_CPNT_EH == TRUE)
typedef enum
{
    SYS_TIME_None_Fun_NO                       = 0,
    SYS_TIME_SetRealTimeClock_Fun_NO           = 1,
    SYS_TIME_SetRealTimeClockByStr_Fun_NO      = 2,
    SYS_TIME_GetTimeZone_Fun_NO                = 3,
    SYS_TIME_GetTimeZoneByStr_Fun_NO           = 4,
    SYS_TIME_SetTimeZoneByStr_Fun_NO           = 5,
    SYS_TIME_SetTimeZone_Fun_NO                = 6,
    SYS_TIME_GetTimeZoneNameByStr_Fun_NO       = 7,
    SYS_TIME_SetTimeZoneNameByStr_Fun_NO       = 8,
    SYS_TIME_GetSoftwareClockBySec_Fun_NO      = 9,
    SYS_MGR_Init_Fun_NO                        = 10,
    SYS_MGR_SetPasswordThreShold_Fun_NO        = 11,
    SYS_MGR_SetConsoleInActiveTimeOut_Fun_NO   = 12,
    SYS_MGR_SetConsoleSilentTime_Fun_NO        = 13,
    SYS_MGR_SetTelnetPasswordThreshold_Fun_NO  = 14,
    SYS_MGR_SetTelnetInActiveTimeOut_Fun_NO    = 15,
    SYS_MGR_SetUartParity_Fun_NO               = 16,
    SYS_MGR_SetUartDataBits_Fun_NO             = 17,
    SYS_MGR_SetUartStopBits_Fun_NO             = 18,
    SYS_MGR_GetSysInfo_Fun_NO                  = 19
} SYS_MGR_UI_MESSAGE_FUNC_NO_T;
#endif /* SYS_CPNT_EH */


/* FUNCTION NAME: SYS_TIME_Initiate_System_Resources
 *-----------------------------------------------------------------------------
 * PURPOSE: initializes all resources for SYSDRV
 *-----------------------------------------------------------------------------
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 *-----------------------------------------------------------------------------
 * NOTES:
 */
BOOL_T SYS_TIME_InitiateSystemResources(void);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_Init
 * ---------------------------------------------------------------------
 * PURPOSE: This function will init the system resource
 *
 * INPUT: None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES: 1. This routine will initialize the software clock by copy time
 *           information from hardware clock if RTC exists.
 *        2. If RTC doesn't exists, then set software clock to zero.
 *        3. Software clock always store UTC time in seconds point from
 *           1970/01/01 00:00:00
 * ---------------------------------------------------------------------
 */
BOOL_T SYS_TIME_Init(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/
void SYS_TIME_Create_InterCSC_Relation(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_EnterMasterMode
 * ---------------------------------------------------------------------
 * PURPOSE: This function will enter master mode
 * INPUT : None
 * OUTPUT: None
 * RETURN: None
 * NOTES : None
 * ---------------------------------------------------------------------
 */
void SYS_TIME_EnterMasterMode(void);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_EnterSlaveMode
 * ---------------------------------------------------------------------
 * PURPOSE: This function will enter slave
 * INPUT : None
 * OUTPUT: None
 * RETURN: None
 * NOTES : None
 * ---------------------------------------------------------------------
 */
void SYS_TIME_EnterSlaveMode(void);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_EnterTransitionMode
 * ---------------------------------------------------------------------
 * PURPOSE: This function will enter transition mode
 * INPUT : None
 * OUTPUT: None
 * RETURN: None
 * NOTES : None
 * ---------------------------------------------------------------------
 */
void SYS_TIME_EnterTransitionMode(void);

/*--------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_SetTransitionMode
 *---------------------------------------------------------------------------
 * PURPOSE:  This function will set the SYS_TIME into the transition mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *---------------------------------------------------------------------------
 */
void SYS_TIME_SetTransitionMode(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_ProvisionComplete
 * ---------------------------------------------------------------------
 * PURPOSE: This function is used to notify provision complete.
 * INPUT: None
 * OUTPUT:
 * RETURN: none
 * NOTES: None
 * ---------------------------------------------------------------------
 */
void SYS_TIME_ProvisionComplete(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_GetRealTimeClock
 * ---------------------------------------------------------------------
 * PURPOSE  : get real TOD(Time-Of-Day)
 * INPUT    : None
 * OUTPUT   : year, month, day, hour, minute, second
 * RETURN   : None
 * NOTES    : 1.If there is no RTC in the system, then then get time
 *              from S/W clock. (Local time)
 *            2.If there is no RTC in system, then this routine is same
 *              as SYS_TIME_GetSoftwareClock
 * History  :  S.K.Yang     06/10/2002      modified
 * ---------------------------------------------------------------------
 */
void SYS_TIME_GetRealTimeClock(int   *year,      /* 2001-2100 */
                               int   *month,     /* 01-12 */
                               int   *day,       /* 01-31 */
                               int   *hour,      /* 00-23 */
                               int   *minute,    /* 00-59 */
                               int   *second);   /* 00-59 */

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_GetRealTimeBySec
 * ---------------------------------------------------------------------
 * PURPOSE  : get real time in seconds
 * INPUT    : None
 * OUTPUT   : seconds
 * RETURN   : None
 * NOTES    : 1. This API is used for Sys_Log. When the system log try to
 *               log event, he will call this API to get current time by
 *               how many seconds.
 *            2. This API will always based on 1/1/2001 00:00:00 as 0 second
 *               to count how many seconds pass away from that time.
 *            3. If there is no RTC in the system, then then get seconds
 *                from S/W clock (Local Time)
 *            4. If no RTC exists, this rountine behave like SYS_TIME_GetSoftwareClockBySec
 * History  :  S.K.Yang     06/7/2002      modified
 * ---------------------------------------------------------------------
 */
void SYS_TIME_GetRealTimeBySec(UI32_T *seconds);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_ConvertSecondsToDateTime
 * ---------------------------------------------------------------------
 * PURPOSE  : convert real time based on the input -- how many seconds pass away
 *            since the time 1/1/1970 00:00:00 to UTC date time
 * INPUT    : None
 * OUTPUT   : year, month, day, hour, minute, second
 * RETURN   : None
 * ---------------------------------------------------------------------
 */
void SYS_TIME_ConvertSecondsToDateTime(UI32_T seconds, int *year, int *month, int *day,
                                       int *hour, int *minute, int *second);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_ConvertSecondsToLocalDateTime
 * ---------------------------------------------------------------------
 * PURPOSE  : convert real time based on the input -- how many seconds pass away
 *            since the time 1/1/1970 00:00:00 to local date time
 * INPUT    : None
 * OUTPUT   : year, month, day, hour, minute, second
 * ---------------------------------------------------------------------
 */
void SYS_TIME_ConvertSecondsToLocalDateTime(UI32_T seconds, int *year, int *month, int *day,
                                            int *hour, int *minute, int *second);

/* please do not use "SYS_TIME_GetRealTimeFromSeconds" from now on */
#define SYS_TIME_GetRealTimeFromSeconds SYS_TIME_ConvertSecondsToLocalDateTime

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_ConvertDateTimeToSeconds
 * ---------------------------------------------------------------------
 * PURPOSE  : convert real time in seconds based on the input
 *            -- Year,month,day,hour,min,second to seconds
 * INPUT    : TOD:Year,month,day,hour,min,second
 * OUTPUT   : Seconds from 1970/01/01 00:00:00 base on input
 * RETURN   : None
 * NOTES    : 1. Symmetry function of SYS_TIME_ConvertDateTimeToSeconds
 * History  : S.K.Yang     06/10/2002      new added
 * ---------------------------------------------------------------------
 */
void SYS_TIME_ConvertDateTimeToSeconds(int year, int month, int day, int hour,
                                       int minute, int second, UI32_T *seconds);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_ValidateTime
 * ---------------------------------------------------------------------
 * PURPOSE  : Validates time
 * INPUT    : year, month, day, hour, minute, second
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 * ---------------------------------------------------------------------
 */
BOOL_T SYS_TIME_ValidateTime(int year, int month, int day, int hour,
                             int minute, int second);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_SetRealTimeClock
 * ---------------------------------------------------------------------
 * PURPOSE: set real TOD(Time-Of-Day)
 * INPUT    : year, month, day, hour, minute, second
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : 1. This API is the only entrance used to set up local time.
 *            2. Set to RTC  if it exists, and set it to Software clock,
 *               so h/w clock is synchronous with s/w clock.
 *            3. When set time to software/hardware clock, this routine will trans-
 *               late local time to UTC format.
 * ---------------------------------------------------------------------
 */
BOOL_T SYS_TIME_SetRealTimeClock(int   year,      /* 2001-2099 */
                                 int   month,     /* 01-12 */
                                 int   day,       /* 01-31 */
                                 int   hour,      /* 00-23 */
                                 int   minute,    /* 00-59 */
                                 int   second);   /* 00-59 */

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_SetRealTimeClockByStr
 * ---------------------------------------------------------------------
 * PURPOSE: set real time clock by string
 * INPUT    : String like below :
 *                   Date-time string :   J|a|n| | |3| |1|5|:| 1| 4| :| 1| 3|  | 1| 9| 8| 8|\0|
 *               Character Position :     0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    :
 *
 * ---------------------------------------------------------------------
 */
BOOL_T SYS_TIME_SetRealTimeClockByStr(UI8_T *clock_time);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_GetSystemUpTime
 * ---------------------------------------------------------------------
 * PURPOSE  : This is used by CLI and WEB to tell how many days, hours,
 *            minutes, seconds and miliseconds this box has run after
 *            system running.
 * INPUT    : None
 * OUTPUT   : days, hours, minutes, seconds, miliseconos
 * RETURN   : None
 * NOTES    : None
 * ---------------------------------------------------------------------
 */
void SYS_TIME_GetSystemUpTime(UI32_T   *days,
                              UI32_T   *hours,
                              UI32_T   *minutes,
                              UI32_T   *seconds,
                              UI32_T   *miliseconds);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_GetSystemUpTimeByTick
 * ---------------------------------------------------------------------
 * PURPOSE  : This is used to get system up time by tick.
 * INPUT    : None
 * OUTPUT   : Ticks
 * RETURN   : None
 * NOTES    : None
 * ---------------------------------------------------------------------
 */
void SYS_TIME_GetSystemUpTimeByTick(UI32_T *ticks);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_GetSystemTicksBy10ms
 * ---------------------------------------------------------------------
 * PURPOSE  : This is used for SNMP or those functions which needs to
 *            get system tick count.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : The most tickSet() value, plus all tickAnnounce() calls since.
 * NOTES    : VxWorks kernel also supports tickGet() to get systemUpTicks,
 *            but for maintainance and generic purpose.  Our core layer
 *            and application layer had better use this API instead of
 *            call tickGet().
 * ---------------------------------------------------------------------
 */
UI32_T SYS_TIME_GetSystemTicksBy10ms(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_SetSystemTicksBy10ms
 * ---------------------------------------------------------------------
 * PURPOSE  : This is used for SNMP or those functions which needs to
 *            set system tick count.
 * INPUT    : ticks -- new time in ticks
 * OUTPUT   : None
 * RETURN   : None.
 * NOTES    : VxWorks kernel also supports tickSet() to set systemUpTicks,
 *            but for maintainance and generic purpose.  Our core layer
 *            and application layer had better use this API instead of
 *            call tickSet().
 * ---------------------------------------------------------------------
 */
void SYS_TIME_SetSystemTicksBy10ms(UI32_T ticks);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_EnableWatchDogTimer
 * ---------------------------------------------------------------------
 * PURPOSE  : This is used to enable watch dog timer.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None.
 * NOTES    : This function is hard ware dependent.  If the agent board
 *            H/W watch dog timer design changes, the function might change
 *            as well.
 * ---------------------------------------------------------------------
 */
void SYS_TIME_EnableWatchDogTimer(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_DisableWatchDogTimer
 * ---------------------------------------------------------------------
 * PURPOSE  : This is used to disable watch dog timer.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None.
 * NOTES    : This function is hard ware dependent.  If the agent board
 *            H/W watch dog timer design changes, the function might change
 *            as well.
 * ---------------------------------------------------------------------
 */
void SYS_TIME_DisableWatchDogTimer(void);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_KickWatchDogTimer
 * ---------------------------------------------------------------------
 * PURPOSE  : This is used to kick watch dog timer to prevent that
 *            watch dog timer time out.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None.
 * NOTES    : This function is hard ware dependent.  If the agent board
 *            H/W watch dog timer design changes, the function might change
 *            as well.
 * ---------------------------------------------------------------------
 */
void SYS_TIME_KickWatchDogTimer(void);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_GetTimeZone
 *------------------------------------------------------------------------------
 * PURPOSE  : Get time zone and name of time zone
 * INPUT    : A pointer point to buffer
 * OUTPUT   : 1. zone_name 2. time offset in minute relative to GMT
 * RETURN   : TRUE/FALSE
 * NOTES    :
 *------------------------------------------------------------------------------
 */
BOOL_T SYS_TIME_GetTimeZone(UI8_T* zone_name, int *minute);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_GetTimeZoneByStr
 *------------------------------------------------------------------------------
 * PURPOSE  : Get time zone by string
 * INPUT    : A pointer point to buffer
 * OUTPUT   : 1. time offset relative to GMT e.q, +8:00 characters
 * RETURN   : TRUE/FALSE
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_GetTimeZoneByStr(UI8_T *time);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_SetTimeZone
 *------------------------------------------------------------------------------
 * PURPOSE  : Set time zone and name of time zone
 * INPUT    : 1.Time zone name 2.Total time different in minutes
 * OUTPUT   : none
 * RETURN   : TRUE if success,FALSE if time zone out of range -780 < zone <720 (min)
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_SetTimeZone(const UI8_T *zone_name, int minute);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_GetTimeZoneNameByStr
 *------------------------------------------------------------------------------
 * PURPOSE  : Get name of time zone
 * INPUT    : 1.Time zone name
 * OUTPUT   : none
 * RETURN   : TRUE if success,FALSE
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_GetTimeZoneNameByStr(UI8_T *name);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_SetTimeZoneNameByStr
 *------------------------------------------------------------------------------
 * PURPOSE  : Set  name of time zone
 * INPUT    : A pointer point to buffer
 * OUTPUT   : 1. zone_name
 * RETURN   : TRUE/FALSE
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_SetTimeZoneNameByStr(UI8_T *name);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_SetTimeZoneByStr
 *------------------------------------------------------------------------------
 * PURPOSE  : Set time zone by string
 * INPUT    : A pointer point to buffer
 * OUTPUT   :
 * RETURN   : TRUE/FALSE
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_SetTimeZoneByStr(UI8_T *time);

/*--------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_GetRunningTimeZone
 *---------------------------------------------------------------------------
 * PURPOSE:  This function will get the time zon of system
 * INPUT:    None
 * OUTPUT:   In minutes
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: Default value is 0 (UTC time)
 *---------------------------------------------------------------------------*/
SYS_TYPE_Get_Running_Cfg_T SYS_TIME_GetRunningTimeZone(int *minute);

/*--------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_GetRunningTimeZoneName
 *---------------------------------------------------------------------------
 * PURPOSE:  This function will get the time zone name of the system
 * INPUT:    Constant string of time zone name
 * OUTPUT:   In minutes
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE  :   1.Default value is "GMT" (UTC time)
 *           2.The length of zone_name is limited to 56 characters
 *---------------------------------------------------------------------------*/
SYS_TYPE_Get_Running_Cfg_T SYS_TIME_GetRunningTimeZoneName(UI8_T *zone_name);

/*--------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_GetDayAndTime
 *---------------------------------------------------------------------------
 * PURPOSE:  Get the DayAndTime in RFC_2579 for SNMP usage.
 * INPUT:    None.
 * OUTPUT:   day_and_time     - The current local time or the UTC time.
 *           day_and_time_len - The length of the time.
 * RETURN:   TRUE/FALSE
 * NOTE  :   1.Refer to RFC2579
 *             DateAndTime ::= TEXTUAL-CONVENTION
 *              field  octets  contents                  range
 *              -----  ------  --------                  -----
 *                1      1-2   year*                     0..65536
 *                2       3    month                     1..12
 *                3       4    day                       1..31
 *                4       5    hour                      0..23
 *                5       6    minutes                   0..59
 *                6       7    seconds                   0..60
 *                             (use 60 for leap-second)
 *                7       8    deci-seconds              0..9
 *                8       9    direction from UTC        '+' / '-'
 *                9      10    hours from UTC*           0..13
 *               10      11    minutes from UTC          0..59
 *---------------------------------------------------------------------------*/

BOOL_T SYS_TIME_GetDayAndTime(UI8_T *day_and_time, UI32_T *day_and_time_len);

/*----------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_GetSoftwareClock
 *---------------------------------------------------------------------------
 * PURPOSE:  Get software clock
 * INPUT:    Buffer of year, month, day, hour, minute, second
 * OUTPUT:   year,month,day,hour,minute of local time.
 * RETURN:   TRUE/FALSE
 * NOTE:     1. This routine will give you local time by year, month,day,hour,
 *              minute,second."year" may be 2 if 2002 , 3 if 2003,etc,.
 *           2. This routine provide you local time.
 *           3. The software clock may be setted by SNTP(when SNTP is enabled)
 *               or RTC(when it exists, or setted by user)
 *           4. Even RTC or SNTP doesn't exists, software clock always has a base time
 *              2001/01/01 00:00:00
 *           5. It will be drifted away during polling interval. Or if sntp is not available
 *---------------------------------------------------------------------------*/
BOOL_T SYS_TIME_GetSoftwareClock(int *year, int *month, int *day,
                                 int *hour, int *minute, int *second);

/*----------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_GetSoftwareClockBySec
 *---------------------------------------------------------------------------
 * PURPOSE:  Get software clock by seconds
 * INPUT:    buffer of seconds
 * OUTPUT:   Local time in seconds from 1970/01/01 00:00:00
 * RETURN:   TRUE/FALSE
 * NOTE:     1. This routine get time in seconds
 *           2. This routine provide you local time
 *           3. It will be drifted away during polling interval. Or if sntp is not available
 *---------------------------------------------------------------------------*/
 BOOL_T  SYS_TIME_GetSoftwareClockBySec(UI32_T *sec);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_ConvertTime
 *------------------------------------------------------------------------------
 * PURPOSE  : Make  time information into format as May 21 10:11:11 2002
 * INPUT    : 1.Time in seconds from 1970/01/01/,00:0:00:00
 *            2.Buffer pointer stored time information , 22 characters buffer needed
 * OUTPUT   : "May 21 10:11:11 2002" format
 * RETURN   : TRUE : SUCCESS, FALSE: FAIL.
 * NOTES    : Return is as the form :
 *            Date-time string :      J|a|n| | |3| |1|5|:| 1| 4| :| 1| 3|  | 1| 9| 8| 8|\0|
 *            Character Position :    0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|
 *            so 22 characters needed
 *------------------------------------------------------------------------------*/
  BOOL_T    SYS_TIME_ConvertTime(UI32_T time, UI8_T *date_time);

/*-------------------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_SetRealTimeClockBySeconds
 *-------------------------------------------------------------------------------------------
 * PURPOSE  : Get time information from sntp to calculate system up time
 *            when RTC is not available.
 * INPUT    : 1. seconds : seconds from 1970/01/01 00:00:00
 *            2. systick : system tick number when  time packet is received
 * OUTPUT   : none
 * RETURN   : TRUE : SUCCESS, FALSE: FAIL
 *            never get time from network.
 * NOTES    :1. --|--------------------------------|---------> time coordinate
 *                V                              V
 *              system up time                   sntp packet received
 *                                               and get system tick number on this point
 *           2. This routine is called by SNTP_MGR when sntp is enabled
 *              and received time packet.
 *           3. Once this routine is called, it will update software clock and hardware clock.
 *           4. Called by SNTP_MGR
 *-------------------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_SetRealTimeClockBySeconds(UI32_T seconds, UI32_T systick);

#if (SYS_CPNT_WATCHDOG_TIMER == TRUE)
/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_SetKick
 * ---------------------------------------------------------------------
 * PURPOSE  : This function set kick on/off.
 * INPUT    : setting -- turn on/off kick
 * OUTPUT   : None
 * RETURN   : None.
 * NOTES    : 1.For debug issue, we need support this function.
 *            2.We need turn on kick after enable watch dog timer.
 * ---------------------------------------------------------------------
 */
void SYS_TIME_SetKick(BOOL_T setting);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_GetKickCounter
 * ---------------------------------------------------------------------
 * PURPOSE  : This function get kick counter.
 * INPUT    : None.
 * OUTPUT   : counter - kick number.
 * RETURN   : None.
 * NOTES    : 1.For debug issue, we need support this function.
 * ---------------------------------------------------------------------
 */
void SYS_TIME_GetKickCounter(UI32_T *tcounter);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_ClearWatchdogIntStatus
 * ---------------------------------------------------------------------
 * PURPOSE  : This function clear watchdog int status.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None.
 * NOTES    :
 * EPR00057114,sunnyt 2005/07/29
 * in 8248, watchdog timeout is trigger by irq5.
 * We need to clear watchdog irq status
 * So, need to add one function in sys_time to clear irq status
 * ---------------------------------------------------------------------
 */
void SYS_TIME_ClearWatchdogIntStatus();

#endif /* SYS_CPNT_WATCHDOG_TIMER */

/*---------------------------------------------------------------------------------
 * FUNCTION NAME: SYS_TIME_AttachSystemResources
 *---------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for SYS_TIME in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *---------------------------------------------------------------------------------*/
void SYS_TIME_AttachSystemResources(void);

/**/
void SYS_TIME_GetUtcRealTimeBySec(UI32_T *seconds);

void SYS_TIME_Main(void);

/*--------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_GetDayOfWeek
 *---------------------------------------------------------------------------
 * PURPOSE  : This function will get day-of-week
 * INPUT    : UI32_T year
 *            UI32_T month
 *            UI32_T day
 * OUTPUT   : UI32_T *day_of_week
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *---------------------------------------------------------------------------
 */
BOOL_T SYS_TIME_GetDayOfWeek(UI32_T second, UI32_T *day_of_week);

/*--------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_IsTimeModify
 *---------------------------------------------------------------------------
 * PURPOSE:  This function will be used to check sys_time is changed by sntp or admininstrator
 * INPUT:    NONE
 * OUTPUT:   NONE
 * RETURN:   TRUE/FALSE
 * NOTE: 	 none
 *---------------------------------------------------------------------------
 */
BOOL_T SYS_TIME_IsTimeModify(void);

/*--------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_GetNextProcessDate
 *---------------------------------------------------------------------------
 * PURPOSE:  This function will be used to get next process date which days of month is greater than input date
 * INPUT:    NONE
 * OUTPUT:   NONE
 * RETURN:   TRUE/FALSE
 * NOTE:
 *---------------------------------------------------------------------------
 */
BOOL_T SYS_TIME_GetNextProcessDate(UI32_T year, UI32_T month, UI32_T day, UI32_T *next_year, UI32_T *next_month);

#endif /* End of SYS_TIME_H */

