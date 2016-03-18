/* Module Name:	TIME_RANGE_TYPE.H
 * Purpose:
 *		This module declares all structures used in TIME_RANGE, structure name and
 *		field name do not be changed.
 *
 * Notes:
 *
 * History:
 *		 Date		-- Modifier,   Reason
 *  2010.02.22      -- Nelson Dai, Created
 *
 * Copyright(C)		 Accton	Corporation, 2010.
 */

#ifndef _TIME_RANGE_TYPE_H
#define _TIME_RANGE_TYPE_H

#define TIME_RANGE_TYPE_UNDEF_TIME_RANGE    (-1)
#define TIME_RANGE_TYPE_ABSOLUTE_UNDEF_VALUE        (-1)
#define  TIME_RANGE_TYPE_TIMER_INTERVAL_SEC  (50 * SYS_BLD_TICKS_PER_SECOND)

typedef enum
{
    TIME_RANGE_TYPE_WEEK_SUNDAY    = 0,
    TIME_RANGE_TYPE_WEEK_MONDAY    = 1,
    TIME_RANGE_TYPE_WEEK_TUESDAY   = 2,
    TIME_RANGE_TYPE_WEEK_WEDNESDAY = 3,
    TIME_RANGE_TYPE_WEEK_THURSDAY  = 4,
    TIME_RANGE_TYPE_WEEK_FRIDAY    = 5,
    TIME_RANGE_TYPE_WEEK_SATURDAY  = 6,
    TIME_RANGE_TYPE_WEEK_DAILY     = 7,
    TIME_RANGE_TYPE_WEEK_WEEKDAYS  = 8,
    TIME_RANGE_TYPE_WEEK_WEEKEND   = 9,
    TIME_RANGE_TYPE_WEEK_MAX = 10,
} TIME_RANGE_TYPE_WEEK_T;

typedef enum
{
    TIME_RANGE_ERROR_TYPE_NONE                             = 0, /* No error */
    TIME_RANGE_ERROR_TYPE_INVALID_DATA                   , /* Invalid data */
    TIME_RANGE_ERROR_TYPE_SAME_DATA                        , /* Same data */
    TIME_RANGE_ERROR_TYPE_DIFFERENT_DATA               , /* Different data */
    TIME_RANGE_ERROR_TYPE_OUT_OF_TIME_RANGE_ENTRY              , /* Out of time range entry */
    TIME_RANGE_ERROR_TYPE_OUT_OF_ABSOLUTE_POLICY_ENTRY     , /* Out of absolute policy entry */
    TIME_RANGE_ERROR_TYPE_OUT_OF_PERIODIC_POLICY_ENTRY      , /* Out of periodic policy entry */
    TIME_RANGE_ERROR_TYPE_OUT_OF_NAME_LENGTH             , /* Exceed name length of time range entry */
    TIME_RANGE_ERROR_TYPE_REFERENCE_IN_USING               , /* Time range entry binds with other CSC(s). */
    TIME_RANGE_ERROR_TYPE_FAIL                         , /* internal error */
} TIME_RANGE_ERROR_TYPE_T;

#define TIME_RANGE_ERROR_STRING                      \
	ERROR_STRING(TIME_RANGE_ERROR_TYPE_NONE, "no error") \
	ERROR_STRING(TIME_RANGE_ERROR_TYPE_INVALID_DATA, "the invalid data") \
	ERROR_STRING(TIME_RANGE_ERROR_TYPE_SAME_DATA, "the same data") \
	ERROR_STRING(TIME_RANGE_ERROR_TYPE_DIFFERENT_DATA, "the different data") \
	ERROR_STRING(TIME_RANGE_ERROR_TYPE_OUT_OF_TIME_RANGE_ENTRY, "out of the maximum time-range number") \
	ERROR_STRING(TIME_RANGE_ERROR_TYPE_OUT_OF_ABSOLUTE_POLICY_ENTRY, "out of the maximum absolute time-range number") \
	ERROR_STRING(TIME_RANGE_ERROR_TYPE_OUT_OF_PERIODIC_POLICY_ENTRY, "out of the maximum periodic time-range number") \
	ERROR_STRING(TIME_RANGE_ERROR_TYPE_OUT_OF_NAME_LENGTH, "exceed the maximun length of time-range name") \
	ERROR_STRING(TIME_RANGE_ERROR_TYPE_REFERENCE_IN_USING, "time-range entry is binding with other CSCs") \
	ERROR_STRING(TIME_RANGE_ERROR_TYPE_FAIL, "internal error")

typedef struct
{
    UI32_T  year;
    UI32_T  month;
    UI32_T  day;
    UI32_T  day_of_week;
    UI32_T  hour;
    UI32_T  minute;
    UI32_T  second;
    UI32_T  time_by_sec;
} TIME_RANGE_TYPE_CONVERT_TIME_T;

typedef struct
{
    UI32_T  time_by_sec;
    UI16_T  year;   /* 2009-2039 */
    UI8_T   month;  /* 01-12 */
    UI8_T   day;    /* 01-31 */
    UI8_T   hour;   /* 00-23 */
    UI8_T   minute; /* 00-59 */
    UI8_T   second; /* 00-59 */
} TIME_RANGE_TYPE_ABSOLUTE_TIME_T;

typedef struct
{
    UI8_T   start_type_of_wk;   /* TIME_RANGE_TYPE_WEEK_T */
    UI8_T   start_hour;         /* 00-23 */
    UI8_T   start_minute;       /* 00-59 */
    UI8_T   start_second;       /* 00-59 */
    UI8_T   end_type_of_wk;     /* TIME_RANGE_TYPE_WEEK_T */
    UI8_T   end_hour;           /* 00-23 */
    UI8_T   end_minute;         /* 00-59 */
    UI8_T   end_second;         /* 00-59 */
} TIME_RANGE_TYPE_PERIODIC_TIME_T;

typedef struct
{
    UI32_T                          ref_counter;
    BOOL_T                          valid;  /* indicate this entry is in use or not */
    char                            name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH + 1];
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T start_time;
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T end_time;
    TIME_RANGE_TYPE_PERIODIC_TIME_T periodic_daily_time[SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY];
    BOOL_T                          check_start_absolute;
    BOOL_T                          check_end_absolute;
    BOOL_T                          check_periodic[SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY];
    BOOL_T                          active;
} TIME_RANGE_TYPE_ENTRY_T;

#endif
