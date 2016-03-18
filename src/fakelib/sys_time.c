#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "sys_hwcfg.h"
#include "sys_type.h"
//#include "sys_adpt.h"
#include "sys_bld.h"
#include "sys_dflt.h"
//#include "sys_module.h"
#include "sys_cpnt.h"

#include "l_stdlib.h"

#include "sysfun.h"
//#ifndef SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM
//#define SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(shmem)   SYS_TYPE_STACKING_MASTER_MODE
//#endif

#include "sys_time.h"

//#ifdef SYS_CPNT_STACKING
//#undef SYS_CPNT_STACKING
//#define SYS_CPNT_STACKING FALSE
//#endif

//#if (SYS_CPNT_STACKING == TRUE)
//#include "isc_om.h"
//#include "sys_time_stk.h"
//#endif
//#include "phyaddr_access.h"
//#include "sysrsc_mgr.h"
//#include "sysdrv.h"
//#include "stktplg_pom.h"

//#if (SYS_HWCFG_SUPPORT_RTC == TRUE)
//#include "m41t11.h"
//#endif

//#include "syslog_pmgr.h"
//#include "backdoor_mgr.h"


//#if (SYS_CPNT_WATCHDOG_TIMER == TRUE)
//#define  WATCH_DOG_ENABLE   (SYS_HWCFG_WATCH_DOG_CONTROL_DIS_WDT)
//#define  WATCH_DOG_DISABLE  (SYS_HWCFG_WATCH_DOG_CONTROL_WDT_CLK)
//#define  WATCH_DOG_KICK_L   (SYS_HWCFG_WATCH_DOG_CONTROL_DIS_WDT)
//#define  WATCH_DOG_KICK_H   (SYS_HWCFG_WATCH_DOG_CONTROL_WDT_CLK | SYS_HWCFG_WATCH_DOG_CONTROL_DIS_WDT)
//#define  WATCH_DOG_PRE_ENABEL_L  (SYS_HWCFG_WATCH_DOG_PRE_ENABEL_KICK_L)
//#define  WATCH_DOG_PRE_ENABEL_H  (SYS_HWCFG_WATCH_DOG_PRE_ENABEL_KICK_H)
///*
//    this address need to modify , but no now .
//    WDT_ADDR
//*/
//#define WDT_ADDR (SYS_HWCFG_WATCH_DOG_CONTROL_ADDRESS)
//#endif /* SYS_CPNT_WATCHDOG_TIMER */

static const char *month_name[]=
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/*time zone range in minutes */
#if (SYS_CPNT_A3COM515_SNTP_MIB == TRUE)
#define MAXTIMEZONE 840
#else
#define MAXTIMEZONE 780
#endif

#define MINTIMEZONE (-720)
#define SYS_TIME_DEFAULT_TIME_ZONE_NAME "UTC"
#define DEFAULT_TIME_ZONE_OFFSET        0

#define is_leap_year(_y_) (((_y_) % 4 == 0 && (_y_) % 100 != 0) || (_y_) % 400 == 0)

#define DFLT_YEAR       2001
#define DFLT_MONTH      1       /* 1 ~ 12 */
#define DFLT_DAY        1
#define DFLT_HOUR       0
#define DFLT_MINUTE     0
#define DFLT_SECOND     0
#define DFLT_MSECOND    0

static SYS_TIME_Shmem_Data_T *sys_time_shmem_data_p;

static SYS_TIME_Shmem_Data_T sys_time_shmem_data;

//static BOOL_T SYS_TIME_GetUtcSoftwareClockBySec(UI32_T *sec);

static BOOL_T SYS_TIME_StrIsDigitEx ( char *str_p, char *except_ar_p );

//daniel
//extern void BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack(char *csc_name, UI32_T msgq_key, void (*func)(void));

//static UI32_T sys_time_om_sem_id;
//#define SYS_TIME_OM_ENTER_CRITICAL_SECTION() \
//    SYSFUN_TakeSem(sys_time_om_sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER)
//#define SYS_TIME_OM_LEAVE_CRITICAL_SECTION() \
//    SYSFUN_GiveSem(sys_time_om_sem_id)



#if __WIN32__
#include "sys_time_win32.c"
#elif __OSX__
#include "sys_time_osx.c"
#endif



/* FUNCTION NAME: SYS_TIME_Initiate_System_Resources
 *-----------------------------------------------------------------------------
 * PURPOSE: initializes all resources for SYS_TIME
 *-----------------------------------------------------------------------------
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 *-----------------------------------------------------------------------------
 * NOTES:
 */
BOOL_T SYS_TIME_InitiateSystemResources(void)
{
    sys_time_shmem_data_p = &sys_time_shmem_data;

    SYS_TIME_Init();

    return TRUE;
}  /* End of SYSDRV_Initiate_System_Resources() */

/*---------------------------------------------------------------------------------
 * FUNCTION NAME: SYSDRV_AttachSystemResources
 *---------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for SYS_TIME in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *---------------------------------------------------------------------------------*/
void SYS_TIME_AttachSystemResources(void)
{
    sys_time_shmem_data_p = &sys_time_shmem_data;
    return;
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_Init
 * ---------------------------------------------------------------------
 * PURPOSE: This function will init the system resource
 *
 * INPUT : None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES: 1. This routine will initialize the software clock by copy time
 *           information from hardware clock if RTC exists.
 *        2. If RTC doesn't exists, then set software clock to zero.
 *        3. Software clock always store UTC time in seconds point from
 *           01/01/1970 00:00:00
 * History:  S.K.Yang     06/7/2002      modified
 *           haiqiang.li  11/01/2008     rewrite
 * ---------------------------------------------------------------------
 */
BOOL_T SYS_TIME_Init(void)
{
    SYS_TIME_SetTimeZone((UI8_T *) "Hollo TZ", 0);

    SYS_TIME_SetRealTimeClock(DFLT_YEAR,
                              DFLT_MONTH,
                              DFLT_DAY,
                              DFLT_HOUR,
                              DFLT_MINUTE,
                              DFLT_SECOND);
    return TRUE;
}

/*--------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function initializes all function pointer registration operations.
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *--------------------------------------------------------------------------*/
void SYS_TIME_Create_InterCSC_Relation(void)
{
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_EnterMasterMode
 * ---------------------------------------------------------------------
 * PURPOSE: This function will enter master mode
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES:   None
 * ---------------------------------------------------------------------*/
void SYS_TIME_EnterMasterMode(void)
{
    /* set mgr in master mode */
    SYSFUN_ENTER_MASTER_MODE_ON_SHMEM(sys_time_shmem_data_p);
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_EnterSlaveMode
 * ---------------------------------------------------------------------
 * PURPOSE: This function will enter slave mode
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES:   None
 * ---------------------------------------------------------------------
 */
void SYS_TIME_EnterSlaveMode(void)
{
    SYSFUN_ENTER_SLAVE_MODE_ON_SHMEM(sys_time_shmem_data_p);
}

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
void SYS_TIME_SetTransitionMode(void)
{
    SYSFUN_ENTER_TRANSITION_MODE_ON_SHMEM(sys_time_shmem_data_p);
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_EnterTransitionMode
 * ---------------------------------------------------------------------
 * PURPOSE: This function will enter transition mode
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES:   None
 * ---------------------------------------------------------------------
 */
void SYS_TIME_EnterTransitionMode(void)
{
    /* wait other callers leave */
    SYSFUN_ENTER_TRANSITION_MODE_ON_SHMEM(sys_time_shmem_data_p);

    sys_time_shmem_data_p->is_provision_complete = FALSE;
}


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
void SYS_TIME_ProvisionComplete(void)
{
}

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
 * History  : S.K.Yang     06/10/2002      modified
 *            haiqiang.li  11/01/2008      modified
 * ---------------------------------------------------------------------
 */
void SYS_TIME_GetRealTimeClock(int *year, int *month, int *day, int *hour,
                               int *minute, int *second)
{
    UI32_T s = sys_time_shmem_data_p->software_clock.time +
              (sys_time_shmem_data_p->time_zone.offset * 60);

    SYS_TIME_ConvertSecondsToDateTime(s, year, month, day, hour, minute, second);
}

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
 *             haiqiang.li  11/01/2008     modified
 * ---------------------------------------------------------------------
 */
void SYS_TIME_GetRealTimeBySec(UI32_T *seconds)
{
    *seconds = sys_time_shmem_data_p->software_clock.time +
              (sys_time_shmem_data_p->time_zone.offset * 60);
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_ConvertSecondsToDateTime
 * ---------------------------------------------------------------------
 * PURPOSE  : Convert seconds to date time without time zone offset
 * INPUT    : seconds -- seconds
 * OUTPUT   : year    -- year
 *            month   -- month
 *            day     -- day
 *            hour    -- hour
 *            minute  -- minute
 *            second  -- second
 * RETURN   : None
 * NOTES    : The same as gmtime()
 * ---------------------------------------------------------------------
 */
void SYS_TIME_ConvertSecondsToDateTime(UI32_T seconds, int *year, int *month, int *day,
                                       int *hour, int *minute, int *second)
{
#if __WIN32__
    SYS_TIME_ConvertSecondsToDateTime_Win32(seconds, year, month, day, hour, minute, second);
#elif __OSX__
    SYS_TIME_ConvertSecondsToDateTime_OSX(seconds, year, month, day, hour, minute, second);
#elif
    assert(0);
#endif
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_ConvertSecondsToLocalDateTime
 * ---------------------------------------------------------------------
 * PURPOSE  : Convert seconds to date time with time zone offset
 * INPUT    : seconds -- seconds
 * OUTPUT   : year    -- year
 *            month   -- month
 *            day     -- day
 *            hour    -- hour
 *            minute  -- minute
 *            second  -- second
 * RETURN   : None
 * NOTES    : The same as localtime()
 * ---------------------------------------------------------------------
 */
void SYS_TIME_ConvertSecondsToLocalDateTime(UI32_T seconds, int *year, int *month, int *day,
                                            int *hour, int *minute, int *second)
{
    UI32_T s = seconds + (sys_time_shmem_data_p->time_zone.offset * 60);

    SYS_TIME_ConvertSecondsToDateTime(s, year, month, day, hour, minute, second);
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - SYS_TIME_ConvertDateTimeToSeconds
 * ---------------------------------------------------------------------
 * PURPOSE  : get real time in seconds based on the input
 *            -- Year,month,day,hour,min,second
 * INPUT    : TOD:Year,month,day,hour,min,second
 * OUTPUT   : Seconds from 01/01/2001 00:00:00 base on input
 * RETURN   : None
 * NOTES    : 1. Symmetry function of  SYS_TIME_ConvertSecondsToDateTime
 * History  : S.K.Yang     06/10/2002      new added
 *            haiqiang.li  11/01/2008      base on 01/01/1970 00:00:00
 * ---------------------------------------------------------------------
 */
void SYS_TIME_ConvertDateTimeToSeconds(int year, int month, int day, int hour,
                                       int minute, int second, UI32_T *seconds)
{
#if __WIN32__
    SYS_TIME_ConvertDateTimeToSeconds_Win32(year, month, day, hour, minute, second, seconds);
#elif __OSX__
    SYS_TIME_ConvertDateTimeToSeconds_OSX(year, month, day, hour, minute, second, seconds);
#elif
    assert(0);
#endif
}

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
                             int minute, int second)
{
    if (year < SYS_TIME_MIN_CONFIGURABLE_YEAR
        || year > SYS_TIME_MAX_CONFIGURABLE_YEAR
        || month > 12 || month < 1 || hour > 23 || hour < 0 || minute > 59
        || minute < 0 || second > 59 || second < 0 )
    {
        return FALSE;
    }

    if ((month == 2) && (day > 29))
    {
        return FALSE;
    }

    if (((month == 4) || (month == 6) || (month == 9) || (month == 11)) &&
        (day > 30))
    {
        return FALSE;
    }

    if (!is_leap_year(year) && (month == 2) && (day > 28))
    {
        return FALSE;
    }

    return TRUE;
}

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
 * History  :  S.K.Yang     06/7/2002      modified
 *             haiqiang.li  11/01/2008     modified
 * ---------------------------------------------------------------------
 */
BOOL_T SYS_TIME_SetRealTimeClock(int year, int month, int day,
                                 int hour, int minute, int second)
{
//    int    offset = 0;  /* time zone offset base on UTC */
    UI32_T seconds;     /* Total seconds from 2001/01/01 00:00:00 to your setting time */
    UI32_T utc_seconds; /* UTC time relative to your setting time in seconds */

    /*
    if (year < SYS_TIME_MIN_CONFIGURABLE_YEAR
        || year > SYS_TIME_MAX_CONFIGURABLE_YEAR
        || month > 12 || month < 1 || hour > 23 || hour < 0 || minute > 59
        || minute < 0 || second > 59 || second < 0 )
        return FALSE;

    if ((month == 2) && (day > 29))
        return FALSE;

    if (((month == 4) || (month == 6) || (month == 9) || (month == 11)) &&
        (day > 30))
        return FALSE;

    if (!is_leap_year(year) && (month == 2) && (day > 28))
        return FALSE;
     */
    if (FALSE == SYS_TIME_ValidateTime(year, month, day, hour, minute, second))
    {
        return FALSE;
    }

    SYS_TIME_ConvertDateTimeToSeconds(year, month, day, hour, minute, second, &seconds);

    utc_seconds = seconds + (sys_time_shmem_data_p->time_zone.offset * 60);

    sys_time_shmem_data_p->software_clock.time = seconds;

    return TRUE;
}

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
BOOL_T SYS_TIME_SetRealTimeClockByStr(UI8_T *clock_time)
{
    int year, day, hour, minute, second;
    UI8_T month_index,match=0;
    UI8_T month_str[4];

    if (clock_time != NULL)
    {
        /* Check the format of the colok_time
         * MMM  D HH:mm:SS YYYY
         * 012345
         */
        if ((strlen((char *)clock_time) < 5)
            || (SYS_TIME_StrIsDigitEx((char *)&clock_time[5], " :") != TRUE)
            || (sscanf((char *)clock_time, "%3s  %d %d:%d:%d %d", month_str, &day, &hour, &minute, &second, &year) != 6))
        {
            return FALSE;
        }

        /* Check the string of month
         */
        for (month_index = 0; month_index < 12; month_index++)
        {
            if (strcmp(month_name[month_index], (char *)month_str) == 0)
            {
                match = 1;
                break;
            }
        }
        if (0 == match)
            return FALSE;

        if (SYS_TIME_SetRealTimeClock(year, month_index + 1,
                day, hour, minute, second) != TRUE)
            return FALSE;

//#if (SYS_CPNT_STACKING == TRUE)
//        if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p)
//                                              == SYS_TYPE_STACKING_MASTER_MODE)
//        {
//            SYS_TIME_RemoteSyncDateTime();
//        }
//#endif

        return TRUE;
    }
    else
        return FALSE;

    return TRUE;
}

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
                              UI32_T   *miliseconds)
{
    UI32_T  ticks;
    UI32_T  tmp;

    SYS_TIME_GetSystemUpTimeByTick(&ticks);

    *miliseconds = ticks % SYS_BLD_TICKS_PER_SECOND;
    tmp = ticks / SYS_BLD_TICKS_PER_SECOND;      /* in seconds   */

    *seconds = tmp % 60;
    tmp = tmp / 60;         /* in minutes   */

    *minutes = tmp % 60;
    tmp = tmp / 60;         /* in hours     */

    *hours = tmp % 24;
    *days = tmp / 24;       /* in days      */
}

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
void SYS_TIME_GetSystemUpTimeByTick(UI32_T *ticks)
{
    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p)!= SYS_TYPE_STACKING_MASTER_MODE)
    {
        *ticks = 0;
        return ;
    }

    if (FALSE == sys_time_shmem_data_p->is_provision_complete)
    {
        *ticks = 0;
        return ;
    }

    //*ticks = SYS_TIME_GetSystemTicksBy10ms() - sys_time_shmem_data_p->provision_complete_ticks;
    *ticks = 100;
    return ;
}

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
UI32_T SYS_TIME_GetSystemTicksBy10ms(void)
{
    //return (SYSFUN_GetSysTick());
    return 100;
}


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
void SYS_TIME_SetSystemTicksBy10ms(UI32_T ticks)
{
    //SYSFUN_SetSysTick(ticks);
}

void SYS_TIME_EnableWatchDogTimer(void)
{
    return;
}
void SYS_TIME_DisableWatchDogTimer(void)
{
    return;
}
void SYS_TIME_KickWatchDogTimer(void)
{
    return;
}

/* -----------------------------------------------------------
 * Time information Process Routine, added for software clock
 * Add by S.K.Yang 2002/06/07 1:00 pm
 *-----------------------------------------------------------*/
/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_GetTimeZone
 *------------------------------------------------------------------------------
 * PURPOSE  : Get time zone and name of time zone
 * INPUT    : A pointer point to buffer
 * OUTPUT   : 1. zone_name 2. time offset in minute relative to GMT
 * RETURN   : TRUE/FALSE
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_GetTimeZone(UI8_T* zone_name,int *minute)
{
    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p) != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return FALSE;
    }
    else
    {
        if (zone_name != 0 && minute != 0)
        {
            /* Text string should be "strcpy", not "memcpy".
             * Zhong Qiyao, 2004.07.26
             */
            strcpy ((char *)zone_name, (char *)sys_time_shmem_data_p->time_zone.zone_name);
#if 0
            memcpy(zone_name,time_zone.zone_name,sizeof(UI8_T)*MAX_ZONE_NAME);
#endif

            *minute = sys_time_shmem_data_p->time_zone.offset;
            return TRUE;
        }
        else
        {
            //EH_MGR_Handle_Exception(SYS_MODULE_SYSMGMT, SYS_TIME_GetTimeZone_Fun_NO, EH_TYPE_MSG_NULL_POINTER, (EH_MGR_FOR_DEBUG_MSG_PURPOSE | SYSLOG_LEVEL_DEBUG));
            return FALSE;
        }
    }
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_GetTimeZoneByStr
 *------------------------------------------------------------------------------
 * PURPOSE  : Get time zone by string
 * INPUT    : A pointer point to buffer
 * OUTPUT   : 1. time offset relative to GMT e.q, "+08:00"  7 characters (include '\0')
 * RETURN   : TRUE/FALSE
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_GetTimeZoneByStr(UI8_T *time)
{
    int hour,min,abs_min,tmp;
    UI8_T hour_str[3], min_str[3];

    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p) != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return FALSE;
    }
    else
    {
        tmp = sys_time_shmem_data_p->time_zone.offset;
        if (time == NULL)
        {
            //EH_MGR_Handle_Exception(SYS_MODULE_SYSMGMT, SYS_TIME_GetTimeZoneByStr_Fun_NO, EH_TYPE_MSG_NULL_POINTER, (EH_MGR_FOR_DEBUG_MSG_PURPOSE | SYSLOG_LEVEL_DEBUG));
            return FALSE;
        }
        else if (tmp == 0) /* UTC time */
        {
            strcpy((char *)time,"00:00");
        }
        else
        {

            if ( tmp > 0)
            {
                time[0] = '+';
            }
            else
            {
                time[0] = '-';
            }
            abs_min = abs(tmp);
            hour = abs_min / 60;
            min = abs_min % 60;

            sprintf((char *)hour_str,"%d",hour);
            if (hour < 10)
            {
                time[1] = '0';
                time[2] = hour_str[0];
            }
            else
            {
            time[1] = hour_str[0];
            time[2] = hour_str[1];
            }

            time[3] = ':';

            sprintf((char *)min_str,"%d",min);
            if (min < 10)
            {
                time[4] = '0';
                time[5] = min_str[0];
            }
            else
            {
            time[4] = min_str[0];
            time[5] = min_str[1];
            }
            time[6] = '\0';
        }
        return TRUE;
    }
}

 /*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_SetTimeZoneByStr
 *------------------------------------------------------------------------------
 * PURPOSE  : Set time zone by string
 * INPUT    : time offset relative to GMT e.q, "+08:00"  7 characters (include '\0')
 * OUTPUT   :
 * RETURN   : TRUE/FALSE
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_SetTimeZoneByStr(UI8_T *time)
{
    int hour, min, offset;

    if (time != NULL)
    {
        /* nonzero means true ==>  means it is UTC */
        if (L_STDLIB_StrIsDigit((char*)time) != 0)
        {
            if (strcmp((char *)time, "00:00")!= 0)
                return FALSE;
            else
                sys_time_shmem_data_p->time_zone.offset = 0; /*UTC time */
        }
        else
        {
            if ((SYS_TIME_StrIsDigitEx((char *)&time[1], ":") != TRUE)
                || (sscanf((char *)&time[1],"%d:%d",&hour,&min) != 2)
                || (time[0] != '+' && time[0] != '-'))
                return FALSE;

            if (min < 0 || min > 59)
                return FALSE;

            offset = (time[0] == '+') ? (hour * 60 + min) : -(hour * 60 + min) ;

            if (offset > MAXTIMEZONE || offset < MINTIMEZONE)
                return FALSE;

            sys_time_shmem_data_p->time_zone.offset = offset;
        }
//#if (SYS_CPNT_STACKING == TRUE)
//        if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p)
//                                               == SYS_TYPE_STACKING_MASTER_MODE)
//        {
//            SYS_TIME_RemoteSyncDateTime();
//        }
//#endif
    }
    else
        return FALSE;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_SetTimeZone
 *------------------------------------------------------------------------------
 * PURPOSE  : Set time zone and name of time zone
 * INPUT    : 1.Time zone name 2.Total time different in minutes
 * OUTPUT   : none
 * RETURN   : TRUE if success,FALSE if time zone out of range -780 < zone <720 (min)
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_SetTimeZone(const UI8_T* zone_name, int minute)
{
    /* Time zone range from -12 hours to 13 hours */
    if (minute > MAXTIMEZONE || minute < MINTIMEZONE )
    {
        return FALSE;
    }

    if (zone_name != 0)
    {
        strncpy((char *)sys_time_shmem_data_p->time_zone.zone_name,
            (char *)zone_name, MAXSIZE_sysTimeZoneName);
        //sys_time_shmem_data_p->time_zone.zone_name[MAXSIZE_sysTimeZoneName] = '\0';
        sys_time_shmem_data_p->time_zone.offset = minute;

//#if (SYS_CPNT_STACKING == TRUE)
//        if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p)
//                                               == SYS_TYPE_STACKING_MASTER_MODE)
//        {
//            SYS_TIME_RemoteSyncDateTime();
//        }
//#endif

        return TRUE;
    }
    else
        return FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_GetTimeZoneNameByStr
 *------------------------------------------------------------------------------
 * PURPOSE  : Get name of time zone
 * INPUT    : 1.Time zone name
 * OUTPUT   : none
 * RETURN   : TRUE if success,FALSE
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_GetTimeZoneNameByStr(UI8_T* name)
{
    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p) != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return FALSE;
    }
    else
    {
        if (name != 0 )
        {
            /* Text string should be "strcpy", not "memcpy".
             * Zhong Qiyao, 2004.07.26
             */
            strcpy ((char *)name, (char *)sys_time_shmem_data_p->time_zone.zone_name);
#if 0
            memcpy(name,time_zone.zone_name,sizeof(UI8_T)*MAX_ZONE_NAME);
#endif
            return TRUE;
        }
        else
        {
            //EH_MGR_Handle_Exception(SYS_MODULE_SYSMGMT, SYS_TIME_GetTimeZoneNameByStr_Fun_NO, EH_TYPE_MSG_NULL_POINTER, (EH_MGR_FOR_DEBUG_MSG_PURPOSE | SYSLOG_LEVEL_DEBUG));
            return FALSE;
        }
    }
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_SetTimeZoneNameByStr
 *------------------------------------------------------------------------------
 * PURPOSE  : Set  name of time zone
 * INPUT    : A pointer point to buffer
 * OUTPUT   : 1. zone_name
 * RETURN   : TRUE/FALSE
 * NOTES    :
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_SetTimeZoneNameByStr(UI8_T *name)
{
    if (name != 0)
    {
        strncpy ((char *)sys_time_shmem_data_p->time_zone.zone_name,
            (char *)name, MAXSIZE_sysTimeZoneName);
        //sys_time_shmem_data_p->time_zone.zone_name[MAXSIZE_sysTimeZoneName] = '\0';

//#if (SYS_CPNT_STACKING == TRUE)
//        if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p)
//            == SYS_TYPE_STACKING_MASTER_MODE)
//        {
//            SYS_TIME_RemoteSyncDateTime();
//        }
//#endif

        return TRUE;
    }
    else
        return FALSE;
}

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
SYS_TYPE_Get_Running_Cfg_T SYS_TIME_GetRunningTimeZone(int *minute)
{
    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p) != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }
    else
    {
        *minute =sys_time_shmem_data_p->time_zone.offset;

        if ( sys_time_shmem_data_p->time_zone.offset == DEFAULT_TIME_ZONE_OFFSET)
            return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
        else
            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;

    }
}

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
SYS_TYPE_Get_Running_Cfg_T SYS_TIME_GetRunningTimeZoneName(UI8_T* zone_name)
{
    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p) != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }
    else
    {
        /* Text string should be "strcmp", not "memcmp".
         * Zhong Qiyao, 2004.07.26
         */
        if (strcmp ((char *)sys_time_shmem_data_p->time_zone.zone_name, SYS_TIME_DEFAULT_TIME_ZONE_NAME) == 0)
#if 0
        if ( memcmp( time_zone.zone_name,DEFAULT_TIME_ZONE_NAME,sizeof(UI8_T)*MAX_ZONE_NAME)==0)
#endif

        {
            return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
        }
        else
        {
            /* Text string should be "strcmp", not "memcmp".
             * Zhong Qiyao, 2004.07.26
             */
            strcpy ((char *)zone_name,(char *)sys_time_shmem_data_p->time_zone.zone_name);
#if 0
            memcpy(zone_name, time_zone.zone_name,sizeof(UI8_T)*MAX_ZONE_NAME);
#endif
            return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
        }
    }
}

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

BOOL_T SYS_TIME_GetDayAndTime(UI8_T *day_and_time, UI32_T *day_and_time_len)
{
    int time_zone_minutes,utc_hour,utc_minute;
    int year,month,day,hour,minute,second;
    UI16_T *ui16_year;
    /* BODY
     */

    if ((day_and_time == NULL) || (day_and_time_len == NULL))
        return FALSE;

    memset(day_and_time, 0, sizeof(UI8_T)*11);
    SYS_TIME_GetRunningTimeZone(&time_zone_minutes);
    utc_hour = (time_zone_minutes / 60);
    utc_minute = time_zone_minutes - (utc_hour * 60);
    SYS_TIME_GetSoftwareClock(&year,&month,&day,&hour,&minute,&second);

    ui16_year = (UI16_T *)day_and_time;
    *ui16_year = (UI16_T)year;

    day_and_time[2] = month;
    day_and_time[3] = day;
    day_and_time[4] = hour;
    day_and_time[5] = minute;
    day_and_time[6] = second;
    day_and_time[7] = 0;

    if(time_zone_minutes > 0)
    {
        day_and_time[8] = '+';
        day_and_time[9] = utc_hour;
        day_and_time[10] = utc_minute;
        *day_and_time_len = 11;
    }
    else if (time_zone_minutes < 0)
    {
        day_and_time[8] = '-';
        day_and_time[9] = utc_hour;
        day_and_time[10] = utc_minute;
        *day_and_time_len = 11;
    }
    else
    {
        *day_and_time_len = 8;
    }
    return TRUE;
}

/*----------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_GetSoftwareClock
 *---------------------------------------------------------------------------
 * PURPOSE:  Get software clock
 * INPUT:    Buffer of year, month, day, hour, minute, second
 * OUTPUT:   year,month,day,hour,minute of local time.
 * RETURN:   TRUE/FALSE
 * NOTE:     1. This routine will give you local time by year, month,day,hour,
 *              minute,second."year" may be 2 if 2002 , 3 if 2003, etc,.
 *           2. This routine provide you local time.
 *           3. The software clock may be setted by SNTP(when SNTP is enabled)
 *               or RTC(when it exists, or setted by user)
 *           4. Even RTC or SNTP doesn't exists, software clock always has a base time
 *              1970/01/01 00:00:00
 *           5. It will be drifted away during polling interval. Or if sntp is not available
 *           6. If SNTP doesn't exists and sys_tick is very samll, "before-UTC" will
 *              casue the problem of negative value. So modify beginning time to
 *              2000/12/31 00:00:00
 *---------------------------------------------------------------------------*/
BOOL_T  SYS_TIME_GetSoftwareClock(int *year, int *month, int *day,
                                  int *hour, int *minute, int *second)
{
    int    offset;      /* time zone offset in minutes */
    UI32_T sys_tick;    /* In 0.01 sec */
    UI32_T delta_tick;  /* Count of difference tick*/
    UI32_T time_in_sec = 0; /* Current time in Seconds */

    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p) != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return FALSE;
    }
    else
    {
        /* Get current time zone offset */
        offset = sys_time_shmem_data_p->time_zone.offset;

        /* Get the current system tick */
        sys_tick = SYS_TIME_GetSystemTicksBy10ms();

        /* Calculate the difference of current system tick and last system tick */
        delta_tick = sys_tick - sys_time_shmem_data_p->software_clock.sys_tick;

        /* Calculate the currnet time */
        time_in_sec = sys_time_shmem_data_p->software_clock.time + delta_tick / SYS_BLD_TICKS_PER_SECOND + offset * 60;

        SYS_TIME_ConvertSecondsToDateTime(time_in_sec, year, month, day, hour, minute, second);

        return TRUE;
    }
}

/*----------------------------------------------------------------------------
 * ROUTINE NAME - SYS_TIME_GetSoftwareClockBySec
 *---------------------------------------------------------------------------
 * PURPOSE:  Get software clock by seconds
 * INPUT:    buffer of seconds
 * OUTPUT:   Local time in seconds from 2001/01/01 00:00:00
 * RETURN:   TRUE/FALSE
 * NOTE:     1. This routine get time in seconds
 *           2. This routine provide you local time.
 *           3. It will be drifted away during polling interval. Or if sntp is not available
 *---------------------------------------------------------------------------*/
BOOL_T SYS_TIME_GetSoftwareClockBySec(UI32_T *sec)
{
    int    offset; /* time zone offset in minutes */
    UI32_T sys_tick, delta_tick;

    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p) != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return FALSE;
    }
    else
    {
        /* Get current time zone offset */
        offset =sys_time_shmem_data_p->time_zone.offset;

        sys_tick = SYS_TIME_GetSystemTicksBy10ms();

        if (sec != 0) /* Non null buffer */
        {   /* Protect software clock, and calculate current time use system tick */
            delta_tick = sys_tick - sys_time_shmem_data_p->software_clock.sys_tick;
            *sec = sys_time_shmem_data_p->software_clock.time + delta_tick / SYS_BLD_TICKS_PER_SECOND + offset * 60;
            return TRUE;
        }
        else
        {
            //EH_MGR_Handle_Exception(SYS_MODULE_SYSMGMT, SYS_TIME_GetSoftwareClockBySec_Fun_NO, EH_TYPE_MSG_NULL_POINTER, (EH_MGR_FOR_DEBUG_MSG_PURPOSE | SYSLOG_LEVEL_DEBUG));
            return FALSE;
        }
    }
}

#if 0
static BOOL_T SYS_TIME_GetUtcSoftwareClockBySec(UI32_T *sec)
{
    UI32_T sys_tick, delta_tick;

    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p) != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return FALSE;
    }
    else
    {
        sys_tick = SYS_TIME_GetSystemTicksBy10ms();

        if (sec != 0) /* Non null buffer */
        {
            delta_tick = sys_tick - sys_time_shmem_data_p->software_clock.sys_tick;
            *sec = sys_time_shmem_data_p->software_clock.time + delta_tick / SYS_BLD_TICKS_PER_SECOND;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
}
#endif /* 0 */

/*------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_ConvertTime
 *------------------------------------------------------------------------------
 * PURPOSE  : Make  time information into format as May 21 10:11:11 2002
 * INPUT    : 1.Time in seconds from 2001/01/01 00:00:00
 *            2.Buffer pointer stored time information, length: SIZE_sysCurrentTime + 1
 * OUTPUT   : "May 21 10:11:11 2002" format
 * RETURN   : TRUE : SUCCESS, FALSE: FAIL.
 * NOTES    : Return is as the form :
 *            Date-time string :      J|a|n| | |3| |1|5|:| 1| 4| :| 1| 3|  | 2| 0| 0| 1|\0|
 *            Character Position :    0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|
 *            so 21 bytes needed
 *------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_ConvertTime(UI32_T time, UI8_T *date_time)
{
    int     year, month, day, hour, minute, second;
    char    *mon_tag_ar[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    /* check valididy of output pointer
     */
    if (date_time == NULL)
    {
        return FALSE;
    }

    /* Call this function to get date_time format from seconds */
    SYS_TIME_ConvertSecondsToDateTime(time, &year, &month, &day, &hour, &minute, &second);

    /* "2000" means SYS_TIME_ConvertSecondsToDateTime count year from 2000 */
    /* year = year + 2000; */
    /* Not every library have itoa function, so use sprintf to convert int to string */
    sprintf((char *)date_time, "%s %2d %02d:%02d:%02d %4d",
                       mon_tag_ar[month-1], day, hour, minute, second, year);

    return TRUE;
}


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
 *                V                                V
 *              system up time                   sntp packet received
 *                                               and get system tick number on this point
 *           2. This routine is called by SNTP_MGR when sntp is enabled
 *              and received time packet.
 *           3. Once this routine is called, it will update software clock and hardware clock.
 *           4. Called by SNTP_MGR
 *-------------------------------------------------------------------------------------------*/
BOOL_T SYS_TIME_SetRealTimeClockBySeconds(UI32_T seconds, UI32_T systick)
{
//#if (SYS_HWCFG_SUPPORT_RTC == TRUE)
//    int year, month, day, hour, minute, second;
//#endif /* #if (SYS_HWCFG_SUPPORT_RTC == TRUE) */

    if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p) != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return FALSE;
    }
    else
    {
    /*Update hardware clock */
//#if (SYS_HWCFG_SUPPORT_RTC == TRUE)
//        SYS_TIME_ConvertSecondsToDateTime(seconds, &year, &month, &day, &hour, &minute, &second);
//        /* Set TOD to hardware , now, it is UTC */
//        SYSDRV_RTC_SetDateTime(year, month, day, hour, minute, second);
//#endif

        /* sync software clock */
        //SYS_TIME_ConvertDateTimeToSeconds(year, month, day, hour, minute, second, (UI32_T *)&seconds);
        sys_time_shmem_data_p->software_clock.time = seconds;
        if (systick == 0)
            systick = SYS_TIME_GetSystemTicksBy10ms();
        sys_time_shmem_data_p->software_clock.sys_tick = systick;

        /* sync kernel time */
        //SYSFUN_SetSysClock(&seconds);

//#if (SYS_CPNT_STACKING == TRUE)
//        if (SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(sys_time_shmem_data_p)
//            == SYS_TYPE_STACKING_MASTER_MODE)
//        {
//            SYS_TIME_RemoteSyncDateTime();
//        }
//#endif

        return TRUE;
    }
}

/*-------------------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_StrIsDigitEx
 *-------------------------------------------------------------------------------------------
 * PURPOSE: Return true if str_p is a particular representation of a decimal
 *          character string ('0'-'9') exception same specific characters.
 * INPUT:   str_p       - pointer of input string (ASCIZ format)
 *          execpt_ar_p - pointer of the specific characters
 * OUTPUT:  none
 * RETURN:  return TRUE if str_p is a decimal character string
 *-------------------------------------------------------------------------------------------*/
static BOOL_T SYS_TIME_StrIsDigitEx(char *str_p, char *except_ar_p )
{
    char c;

    for (; (c = *str_p) != 0; str_p++)
    {
        if (strchr(except_ar_p, c) != NULL)
            continue;

        if (L_STDLIB_IsDigit(c) == FALSE)
            return FALSE;
    }

    return TRUE;
}

#define SYSTIME_BACKDOOR_Printf(fmt_p, ...) printf((fmt_p), ##__VA_ARGS__)

void SYS_TIME_Main (void)
{
#define MAXLINE 255
    UI8_T select_value;

    while(1)
    {
        SYSTIME_BACKDOOR_Printf("\r\nPress <enter> to continue.");
        SYSTIME_BACKDOOR_Printf("\r\n===========================================");
        SYSTIME_BACKDOOR_Printf("\r\n  SYSTIME Engineer Menu 2008/04/22  ");
        SYSTIME_BACKDOOR_Printf("\r\n===========================================");

        SYSTIME_BACKDOOR_Printf("\r\n 1 Get Real time by sec test.");
        SYSTIME_BACKDOOR_Printf("\r\n 2 Software clock test.");
        SYSTIME_BACKDOOR_Printf("\r\n 3 Hardware clock test.");

        SYSTIME_BACKDOOR_Printf("\r\n 0 Exit Stack Topology Engineer Menu!!");
        SYSTIME_BACKDOOR_Printf("\r\n Enter Selection: ");

        select_value = getchar();
        if(select_value < '0' || select_value > '7')
            continue;

        select_value -= '0';
        printf("%d", select_value);

        switch(select_value)
        {
            case 1:
            {
                UI32_T seconds;
                UI8_T UTC[22]={0};
                SYS_TIME_GetRealTimeBySec(&seconds);
                SYS_TIME_ConvertTime(seconds, UTC);

                SYSTIME_BACKDOOR_Printf(" \r\n Get real time by sec is %s.\n\r", UTC);
            }
            break;

            case 2:
            {
                int year, month, day, hour, minute, second;

                SYS_TIME_GetSoftwareClock(&year, &month, &day, &hour, &minute, &second);

                SYSTIME_BACKDOOR_Printf(" \r\n Get software time is %d.%d.%d %d:%d:%d .\n\r", year, month, day, hour, minute, second);
            }
            break;

            case 3:
            {
                int year, month, day, hour, minute, second;


                SYS_TIME_GetRealTimeClock(&year, &month, &day, &hour, &minute, &second);

                SYSTIME_BACKDOOR_Printf(" \r\n Get software time is %d.%d.%d %d:%d:%d .\n\r", year, month, day, hour, minute, second);
            }
            break;

            case 0:
                SYSTIME_BACKDOOR_Printf("\r\n Exit Stack Topology Engineer Menu");
                return;
        }
    }

} /* End of SYS_BACKDOOR_Main */


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
BOOL_T SYS_TIME_GetDayOfWeek(UI32_T second, UI32_T *day_of_week)
{
    *day_of_week = 1;
    return TRUE;
}

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
BOOL_T SYS_TIME_IsTimeModify(void)
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;

    SYS_TIME_ConvertSecondsToDateTime(sys_time_shmem_data_p->software_clock.time,
        &year, &month, &day, &hour, &minute, &second);

    return (year == DFLT_YEAR && month == DFLT_MONTH && day == DFLT_DAY &&
            hour == DFLT_HOUR && minute == DFLT_MINUTE && second == DFLT_SECOND) ?
        FALSE : TRUE;
}

/*-------------------------------------------------------------------------------------------
 * FUNCTION NAME - SYS_TIME_IsLeapYear
 *-------------------------------------------------------------------------------------------
 * PURPOSE : check if the input is a leap year
 * INPUT   : year
 * OUTPUT  : none
 * RETURN  : TRUE - input is a leap year
 *           FALSE - input is not a leap year
 *-------------------------------------------------------------------------------------------*/
static BOOL_T SYS_TIME_IsLeapYear(UI32_T year)
{
    if( (year % 4 == 0) && ( (year % 100 != 0) || (year % 400 == 0)) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static BOOL_T SYS_TIME_GetDaysOfMonth(UI32_T year, UI32_T month, UI32_T *days)
{
    static const UI32_T DAY_OF_MONTH[2][12] = { {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                                            {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} };
#define LEAP_YEAR           1
#define NORMAL_YEAR         0

    /* check if input buffer and value is valid
     */
    if((NULL == days) || (month > 12))
        return FALSE;

    /* check if the year is leap
     */
    if(TRUE == SYS_TIME_IsLeapYear(year))
    {
        *days = DAY_OF_MONTH[LEAP_YEAR][month - 1];
    }
    else
    {
        *days = DAY_OF_MONTH[NORMAL_YEAR][month - 1];
    }

    return TRUE;
}

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
BOOL_T SYS_TIME_GetNextProcessDate(UI32_T year, UI32_T month, UI32_T day, UI32_T *next_year, UI32_T *next_month)
{
    /* Find reload regularity time date
     */
    UI32_T  days_of_month ;
    UI32_T  suspense_year;
    UI32_T  suspense_month;

    if ( (next_year == NULL) || (next_month == NULL) )
    {
        return FALSE;
    }

    suspense_year   = year;
    suspense_month  = month;
    days_of_month   = day;

    do
    {
        if (suspense_month ==12)
        {
            suspense_year++;
            suspense_month = 1;
        }
        else
        {
            suspense_month++;
        }
        SYS_TIME_GetDaysOfMonth(suspense_year, suspense_month, &days_of_month);
    } while ( days_of_month < day );

    *next_year  = suspense_year;
    *next_month = suspense_month;

    return TRUE;
}
