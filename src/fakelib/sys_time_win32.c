/*
 *  sys_time_win32.c
 *  systime
 *
 *  Created by yehjunying on 2011/7/9.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <windows.h>

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
void SYS_TIME_ConvertSecondsToDateTime_Win32(UI32_T seconds, int *year, int *month, int *day,
                                             int *hour, int *minute, int *second)
{
    SYSTEMTIME      stLocalTime;
    SYSTEMTIME      stBaseTime;

    FILETIME        ftLocalTime;
    FILETIME        ftBaseTime;

    LARGE_INTEGER   lLocalTime;

    ZeroMemory(&stBaseTime, sizeof(stBaseTime));
    stBaseTime.wYear            = DFLT_YEAR;
    stBaseTime.wMonth           = DFLT_MONTH;
    stBaseTime.wDay             = DFLT_DAY;
    stBaseTime.wHour            = DFLT_HOUR;
    stBaseTime.wMinute          = DFLT_MINUTE;
    stBaseTime.wSecond          = DFLT_SECOND;
    stBaseTime.wMilliseconds    = DFLT_MSECOND;

    SystemTimeToFileTime(&stBaseTime, &ftBaseTime);

    lLocalTime.QuadPart = seconds;
    lLocalTime.QuadPart *= 10000000;

    lLocalTime.HighPart += ftBaseTime.dwHighDateTime;
    lLocalTime.LowPart  += ftBaseTime.dwLowDateTime;

    ftLocalTime.dwHighDateTime  = lLocalTime.HighPart;
    ftLocalTime.dwLowDateTime   = lLocalTime.LowPart;

    FileTimeToSystemTime(&ftLocalTime, &stLocalTime);

    *year   = stLocalTime.wYear;
    *month  = stLocalTime.wMonth;
    *day    = stLocalTime.wDay;
    *hour   = stLocalTime.wHour;
    *minute = stLocalTime.wMinute;
    *second = stLocalTime.wSecond;
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
void SYS_TIME_ConvertDateTimeToSeconds_Win32(int year, int month, int day, int hour,
                                             int minute, int second, UI32_T *seconds)
{
    SYSTEMTIME      stLocalTime;
    SYSTEMTIME      stBaseTime;

    FILETIME        ftLocalTime;
    FILETIME        ftBaseTime;

    LARGE_INTEGER   lLocalTime;
    LARGE_INTEGER   lBaseTime;

    ZeroMemory(&stLocalTime, sizeof(stLocalTime));
    stLocalTime.wYear   = year;
    stLocalTime.wMonth  = month;
    stLocalTime.wDay    = day;
    stLocalTime.wHour   = hour;
    stLocalTime.wMinute = minute;
    stLocalTime.wSecond = second;

    ZeroMemory(&stBaseTime, sizeof(stBaseTime));
    stBaseTime.wYear            = DFLT_YEAR;
    stBaseTime.wMonth           = DFLT_MONTH;
    stBaseTime.wDay             = DFLT_DAY;
    stBaseTime.wHour            = DFLT_HOUR;
    stBaseTime.wMinute          = DFLT_MINUTE;
    stBaseTime.wSecond          = DFLT_SECOND;
    stBaseTime.wMilliseconds    = DFLT_MSECOND;

    SystemTimeToFileTime(&stLocalTime, &ftLocalTime);
    SystemTimeToFileTime(&stBaseTime, &ftBaseTime);

    lLocalTime.HighPart = ftLocalTime.dwHighDateTime;
    lLocalTime.LowPart  = ftLocalTime.dwLowDateTime;

    lBaseTime.HighPart  = ftBaseTime.dwHighDateTime;
    lBaseTime.LowPart   = ftBaseTime.dwLowDateTime;

    *seconds = (UI32_T) ((lLocalTime.QuadPart - lBaseTime.QuadPart)/10000000);
}

