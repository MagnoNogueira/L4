/*
 *  sys_time_osx.c
 *  systime
 *
 *  Created by yehjunying on 2011/7/9.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <time.h>

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
void SYS_TIME_ConvertSecondsToDateTime_OSX(UI32_T seconds, int *year, int *month, int *day,
                                           int *hour, int *minute, int *second)
{
    struct tm utc_tm = {0};
    struct tm base_tm = {0};

    time_t utc_seconds;
    time_t base_seconds;

    //
    // Base time
    //
    {
        base_tm.tm_year = 1970 - 1900;
        base_tm.tm_mon = 1 - 1;
        base_tm.tm_mday = 1;
        base_tm.tm_hour = 0;
        base_tm.tm_min = 0;
        base_tm.tm_sec = 0;

        base_seconds = mktime(&base_tm);
        base_seconds += base_tm.tm_gmtoff;

        assert(base_seconds == 0);
    }

    base_tm.tm_year = DFLT_YEAR - 1900;
    base_tm.tm_mon = DFLT_MONTH - 1;
    base_tm.tm_mday = DFLT_DAY;
    base_tm.tm_hour = DFLT_HOUR;
    base_tm.tm_min = DFLT_MINUTE;
    base_tm.tm_sec = DFLT_SECOND;

    base_seconds = mktime(&base_tm);
    base_seconds += base_tm.tm_gmtoff;

    {
        gmtime_r(&base_seconds, &base_tm);

        assert(base_tm.tm_year == DFLT_YEAR - 1900);
        assert(base_tm.tm_mon  == DFLT_MONTH - 1);
        assert(base_tm.tm_mday == DFLT_DAY);
        assert(base_tm.tm_hour == DFLT_HOUR);
        assert(base_tm.tm_min  == DFLT_MINUTE);
        assert(base_tm.tm_sec  == DFLT_SECOND);
    }

    utc_seconds = seconds;
    utc_seconds = utc_seconds + base_seconds;

    gmtime_r(&utc_seconds, &utc_tm);

    *year = utc_tm.tm_year + 1900;
    *month = utc_tm.tm_mon + 1;
    *day = utc_tm.tm_mday;
    *hour = utc_tm.tm_hour;
    *minute = utc_tm.tm_min;
    *second = utc_tm.tm_sec;
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
void SYS_TIME_ConvertDateTimeToSeconds_OSX(int year, int month, int day, int hour,
                                           int minute, int second, UI32_T *seconds)
{
    struct tm utc_tm = {0};
    struct tm base_tm = {0};

    time_t utc_seconds;
    time_t base_seconds;

    utc_tm.tm_year = year - 1900;
    utc_tm.tm_mon = month - 1;
    utc_tm.tm_mday = day;
    utc_tm.tm_hour = hour;
    utc_tm.tm_min = minute;
    utc_tm.tm_sec = second;

    base_tm.tm_year = DFLT_YEAR - 1900;
    base_tm.tm_mon = DFLT_MONTH - 1;
    base_tm.tm_mday = DFLT_DAY;
    base_tm.tm_hour = DFLT_HOUR;
    base_tm.tm_min = DFLT_MINUTE;
    base_tm.tm_sec = DFLT_SECOND;

    utc_seconds = mktime(&utc_tm);
    utc_seconds += utc_tm.tm_gmtoff;

    base_seconds = mktime(&base_tm);
    base_seconds += base_tm.tm_gmtoff;

    *seconds = (UI32_T) difftime(utc_seconds, base_seconds);
}
