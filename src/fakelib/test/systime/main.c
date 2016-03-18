#include <assert.h>
#include <stdio.h>
#include "sys_time.h"

int testGetRealtimeBySec()
{
    UI32_T second_val;

    int set_year, set_month, set_day, set_hour, set_minute, set_second;
    int get_year, get_month, get_day, get_hour, get_minute, get_second;

    set_year    = 1970;
    set_month   = 1;
    set_day     = 2;
    set_hour    = 0;
    set_minute  = 0;
    set_second  = 0;

    SYS_TIME_SetRealTimeClock(set_year,
                              set_month,
                              set_day,
                              set_hour,
                              set_minute,
                              set_second);
    SYS_TIME_GetRealTimeBySec(&second_val);
    SYS_TIME_ConvertSecondsToLocalDateTime(second_val,
                                           &get_year,
                                           &get_month,
                                           &get_day,
                                           &get_hour,
                                           &get_minute,
                                           &get_second);
    assert(set_year == get_year && set_month == get_month && set_day == get_day && set_hour == get_hour);

    set_year    = 2010;
    set_month   = 1;
    set_day     = 1;
    set_hour    = 0;
    set_minute  = 0;
    set_second  = 0;

    SYS_TIME_SetRealTimeClock(set_year,
                              set_month,
                              set_day,
                              set_hour,
                              set_minute,
                              set_second);
    SYS_TIME_GetRealTimeBySec(&second_val);
    SYS_TIME_ConvertSecondsToLocalDateTime(second_val,
                                           &get_year,
                                           &get_month,
                                           &get_day,
                                           &get_hour,
                                           &get_minute,
                                           &get_second);
    assert(set_year == get_year && set_month == get_month && set_day == get_day);

    return 0;
}

int testIsTimeModify()
{
    BOOL_T b_result;

    SYS_TIME_Init();

    b_result = SYS_TIME_IsTimeModify();

    assert(FALSE == b_result);

    SYS_TIME_SetRealTimeClock(2011, 1, 2, 3, 4, 5);
    b_result = SYS_TIME_IsTimeModify();

    assert(TRUE == b_result);

    return 0;
}


int main (int argc, const char * argv[])
{
    SYS_TIME_AttachSystemResources();

    SYS_TIME_InitiateSystemResources();
    SYS_TIME_EnterMasterMode();

    testGetRealtimeBySec();
    testIsTimeModify();

    printf("Hello, World!\n");
    return 0;
}
