/*-----------------------------------------------------------------------------
 * Module   : TIME_RANGE_MGR.h
 *-----------------------------------------------------------------------------
 * PURPOSE  : TIME_RANGE_MGR provides all TIME_RANGE accessing functions,
 *            including Create, Delete, and GetNext.
 *
 *-----------------------------------------------------------------------------
 * HISTORY  : 2010.02 - Nelson Dai, Create
 *
 *
 *-----------------------------------------------------------------------------
 * Copyright(C)                               Accton Corporation, 2010
 *-----------------------------------------------------------------------------
 */

#include "string.h"
#include "stdio.h"
#include "l_charset.h"
#include "time_range_mgr.h"
#include "time_range_om.h"
#include "time_range_backdoor.h"
#include "sys_time.h"
#include "sys_callback_mgr.h"
#include "sys_module.h"
#include "backdoor_mgr.h"

SYSFUN_DECLARE_CSC

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_LocalMibAbsoluteUndefSetting
 *-------------------------------------------------------------------------
 * PURPOSE  : The function is used to set the value to mean 
 *                   don't care start or end absolute time
 * INPUT    : entry --  the absolute time.
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
static void TIME_RANGE_MGR_LocalMibAbsoluteUndefSetting(
        TIME_RANGE_TYPE_ABSOLUTE_TIME_T *entry)
{
    memset(entry, 0, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));

    entry->year = TIME_RANGE_TYPE_ABSOLUTE_UNDEF_VALUE;
    entry->month = TIME_RANGE_TYPE_ABSOLUTE_UNDEF_VALUE;
    entry->day = TIME_RANGE_TYPE_ABSOLUTE_UNDEF_VALUE;
    entry->hour = TIME_RANGE_TYPE_ABSOLUTE_UNDEF_VALUE;
    entry->minute = TIME_RANGE_TYPE_ABSOLUTE_UNDEF_VALUE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_LocalCheckPeriodicTime
 *-------------------------------------------------------------------------
 * PURPOSE  : The function is used to check if the periodic time-range entry is legal
 * INPUT    : entry -- the periodic time.
 * OUTPUT   : None.
 * RETURN   : TRUE / FALSE
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
static BOOL_T TIME_RANGE_MGR_LocalCheckPeriodicTime(
        TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    if (entry == NULL)
    {
        return FALSE;
    }

    if ((entry->start_type_of_wk >= TIME_RANGE_TYPE_WEEK_MAX) || 
	   (entry->end_type_of_wk >= TIME_RANGE_TYPE_WEEK_MAX) ||
	   (entry->start_hour >= 24) || (entry->end_hour >= 24) ||
	   (entry->start_minute >= 60) || (entry->end_minute >= 60) ||
	   (entry->start_second >= 60) || (entry->end_second >= 60))
    {
        return FALSE;
    }    

    switch (entry->start_type_of_wk)
    {
        case TIME_RANGE_TYPE_WEEK_DAILY:
        case TIME_RANGE_TYPE_WEEK_WEEKDAYS:
        case TIME_RANGE_TYPE_WEEK_WEEKEND:
            /* must be the same day type */
            if (entry->start_type_of_wk != entry->end_type_of_wk)
            {
                return FALSE;
            }

            /* check time (start time < end time) */
            if (entry->start_hour > entry->end_hour)
            {
                return FALSE;
            }
            else if (entry->start_hour == entry->end_hour)
            {
                if (entry->start_minute > entry->end_minute)
                {
                    return FALSE;
                }
                else if (entry->start_minute == entry->end_minute)
                {
                    if (entry->start_second >= entry->end_second)
                    {
                        return FALSE;
                    }
                }
            }
            break;

        default:
            break;
    	}

    return TRUE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_LocalCheckAbsoluteTime
 *-------------------------------------------------------------------------
 * PURPOSE  : The function is used to check if the Absolute time-range entry is legal
 * INPUT    : is_start_null   -- no set start absolute time
 *            is_end_null     -- no set end absolute time
 *            start  --  start of the absolute time.
 *            end    --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : TRUE / FALSE
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
static BOOL_T TIME_RANGE_MGR_LocalCheckAbsoluteTime(
        BOOL_T is_start_null, BOOL_T is_end_null,
        TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start, 
        TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    if ((start == NULL) || (end == NULL))
    {
        return FALSE;
    }	

    if ((is_start_null == TRUE) && (is_end_null == TRUE))
    {
        return FALSE;
    }

    /* convert */
    if (is_start_null != TRUE)
    {		
        if ( FALSE == SYS_TIME_ValidateTime(start->year,
                                          start->month,
                                          start->day,
                                          start->hour,
                                          start->minute, 
                                          start->second) )
            return FALSE;

        SYS_TIME_ConvertDateTimeToSeconds(start->year,
                                          start->month,
                                          start->day,
                                          start->hour,
                                          start->minute,
                                          start->second,
                                          &start->time_by_sec);
    }

    if (is_end_null != TRUE)
    {
        if ( FALSE == SYS_TIME_ValidateTime(end->year,
                                          end->month,
                                          end->day,
                                          end->hour,
                                          end->minute,
                                          end->second) )
            return FALSE;
		
        SYS_TIME_ConvertDateTimeToSeconds(end->year,
                                          end->month,
                                          end->day,
                                          end->hour,
                                          end->minute,
                                          end->second,
                                          &end->time_by_sec);
    }

    /* check if it is valid */
    if ((is_start_null != TRUE) && (is_end_null != TRUE))
    {
        if (start->time_by_sec > end->time_by_sec)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_LocalConvertMibAbsoluteTime
 *-------------------------------------------------------------------------
 * PURPOSE  : The function is used to conver MIB setting 
 * INPUT    :  is_start_null   -- no set start absolute time
 *             is_end_null     -- no set end absolute time
 *             start_time -- start absolute time setting from MIB
 *             end_time   -- end absolute time setting from MIB
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
static BOOL_T TIME_RANGE_MGR_LocalConvertAndCheckMibAbsoluteTime(
        BOOL_T *is_start_null,
        BOOL_T *is_end_null,
        TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
        TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T mib_undef;

    TIME_RANGE_MGR_LocalMibAbsoluteUndefSetting(&mib_undef);

    if (start->year == mib_undef.year)
    {
        if (0 != memcmp(start, &mib_undef, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T)))
        {
            return FALSE;
        }
        *is_start_null = TRUE;
    }
    else 
    {
        *is_start_null = FALSE;
    }

    if (end->year == mib_undef.year)
    {
        if (0 != memcmp(end, &mib_undef, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T)))
        {
            return FALSE;
        }
        *is_end_null = TRUE;
    }
    else
    {
        *is_end_null = FALSE;
    }

    if (FALSE == TIME_RANGE_MGR_LocalCheckAbsoluteTime(
		                        *is_start_null, 
		                        *is_end_null, 
		                        start, end))
    {
        return FALSE;
    }		                     	

    return TRUE;
}

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_Initiate_System_Resources
 *--------------------------------------------------------------------------
 * PURPOSE  : This function is used to initialize the time_range module.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *            FALSE   -- unspecified failure, system error.
 * NOTES    : Call by TIME_RANGE_TASK_Initiate_System_Resources() only.
 *--------------------------------------------------------------------------*/
BOOL_T TIME_RANGE_MGR_Initiate_System_Resources(void)
{
    return TIME_RANGE_OM_Initiate_System_Resources();
}

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function is used to create internal CSC relation.
 * INPUT    : None
 * OUTPUT   : None
 * RETUEN   : TRUE    -- successful.
 *            FALSE   -- unspecified failure, system error.
 * NOTES    : Call by UTILITY_GROUP_Create_InterCSC_Relation() only.
 *--------------------------------------------------------------------------*/
BOOL_T TIME_RANGE_MGR_Create_InterCSC_Relation(void)
{
    BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack("TIME_RANGE",
        SYS_BLD_UTILITY_GROUP_IPCMSGQ_KEY, TIME_RANGE_BACKDOOR_Main);

    return TRUE;
}

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_EnterMasterMode
 *--------------------------------------------------------------------------
 * PURPOSE  : It is used to enable the time_range operation while in master mode.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *            FALSE   -- unspecified failure, system error.
 * NOTES    : None
 *--------------------------------------------------------------------------*/
BOOL_T TIME_RANGE_MGR_EnterMasterMode(void)
{
    SYSFUN_ENTER_MASTER_MODE();
    return TRUE;
}

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_EnterTransitionMode
 *--------------------------------------------------------------------------
 * PURPOSE  : It's the temporary transition mode between system into master mode.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *            FALSE   -- unspecified failure, system error.
 * NOTES    : None
 *--------------------------------------------------------------------------*/
BOOL_T TIME_RANGE_MGR_EnterTransitionMode(void)
{
    SYSFUN_ENTER_TRANSITION_MODE();
    return TRUE;
}

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_EnterSlaveMode
 *--------------------------------------------------------------------------
 * PURPOSE  : Set SFLOW op_state to "SYS_TYPE_STACKING_SLAVE_MODE"
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *            FALSE   -- unspecified failure, system error.
 * NOTES    : None
 *-------------------------------------------------------------------------- */
BOOL_T TIME_RANGE_MGR_EnterSlaveMode(void)
{
    SYSFUN_ENTER_SLAVE_MODE();
    return TRUE;
}

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_SetTransitionMode
 *--------------------------------------------------------------------------
 * PURPOSE  : This function sets the component to temporary transition mode
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *            FALSE   -- unspecified failure, system error.
 * NOTES    : None
 *--------------------------------------------------------------------------
 */
BOOL_T TIME_RANGE_MGR_SetTransitionMode(void)
{
    SYSFUN_SET_TRANSITION_MODE();
    return TRUE;
}

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_GetOperationMode
 *--------------------------------------------------------------------------
 * PURPOSE  : This functions returns the current operation mode of this component
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *            FALSE   -- unspecified failure, system error.
 * NOTES    : None
 *--------------------------------------------------------------------------
 */
SYS_TYPE_Stacking_Mode_T TIME_RANGE_MGR_GetOperationMode(void)
{
    return SYSFUN_GET_CSC_OPERATING_MODE();
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_HandleTimerEvent
 *-------------------------------------------------------------------------
 * PURPOSE  : Process the timer event
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : Call by UTILITY_GROUP_Mgr_Thread_Function_Entry() only.
 * ------------------------------------------------------------------------
 */
void TIME_RANGE_MGR_HandleTimerEvent(void)
{
    TIME_RANGE_TYPE_CONVERT_TIME_T current_time;
    UI32_T i, day_of_week;
    UI8_T  mapping_table_day_of_week[7] = {
            TIME_RANGE_TYPE_WEEK_SUNDAY,
            TIME_RANGE_TYPE_WEEK_MONDAY,
            TIME_RANGE_TYPE_WEEK_TUESDAY,
            TIME_RANGE_TYPE_WEEK_WEDNESDAY,
            TIME_RANGE_TYPE_WEEK_THURSDAY,
            TIME_RANGE_TYPE_WEEK_FRIDAY,
            TIME_RANGE_TYPE_WEEK_SATURDAY
           };
    UI8_T isChanged [SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST],
          old_status[SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST],
          new_status[SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST];

    /* convert current time to structure time */
    SYS_TIME_GetRealTimeClock(
            (int *)&(current_time.year),   (int *)&(current_time.month),
            (int *)&(current_time.day),    (int *)&(current_time.hour),
            (int *)&(current_time.minute), (int *)&(current_time.second));

    SYS_TIME_ConvertDateTimeToSeconds(
            current_time.year,   current_time.month,
            current_time.day,    current_time.hour,
            current_time.minute, current_time.second,
            &(current_time.time_by_sec));

    SYS_TIME_GetDayOfWeek(current_time.time_by_sec, &day_of_week);
    current_time.day_of_week = mapping_table_day_of_week[day_of_week];

    /* update status */
    if (TIME_RANGE_OM_UpdateEntryStatus(&current_time, old_status, new_status) == TRUE)
    {
        if (0 == memcmp(old_status, new_status, SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST))
        {
            return;
        }

        memset(isChanged, 0x0, sizeof(isChanged));
        for (i = 0; i < SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST; ++i)
        {
            isChanged[i] |= (old_status[i] ^ new_status[i]);
        }
        SYS_CALLBACK_MGR_TIME_RANGE_StatusChange(SYS_MODULE_TIME_RANGE, isChanged, new_status);
    }

    return;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_IsTimeRangeEntryValidByName
 *-------------------------------------------------------------------------
 * PURPOSE  : check if the time range entry is valid by name
 * INPUT    : name  --  the name of the entry.
 * OUTPUT   : none
 * RETURN   : TIME_RANGE_ERROR_TYPE_NONE: The time-range entry is valid
 *                  Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_IsTimeRangeEntryValidByName(UI8_T *name)
{
    UI32_T index;

    if(name == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_OM_GetTimeRangeEntryIndexByName(name, &index))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }
	
    return TIME_RANGE_OM_IsTimeRangeEntryValid(index);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_GetTimeRangeEntryIndexByName
 *-------------------------------------------------------------------------
 * PURPOSE  : Get time-range entry according to the input name.
 * INPUT    : name  --  the name of the entry.
 * OUTPUT   : entry --  the entry you are looking for.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_GetTimeRangeEntryByName(UI8_T *name, TIME_RANGE_TYPE_ENTRY_T *entry)
{
    UI32_T index;

    if ((name == NULL) || (entry == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_OM_GetTimeRangeEntryIndexByName(name, &index))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_GetTimeRangeEntry(index, entry);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_CreateTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Create a time_range_entry with specific name.
 * INPUT    : name        -- the name of the new entry you want to create.
 * OUTPUT : None
 * RETURN   : Error code
 * NOTES    : None.
 * HISTORY:
 *   mm/dd/yy
 *   03/06/13 -- Add member err_type, please refer struct TIME_RANGE_ERROR_TYPE_T.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_CreateTimeRangeEntry(UI8_T *name)
{
    if (name == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (strlen((char *)name) > SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH)
    {
        return TIME_RANGE_ERROR_TYPE_OUT_OF_NAME_LENGTH;
    }

    return TIME_RANGE_OM_CreateTimeRangeEntry(name);  
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_SetTimeRangeAbsolute
 *-------------------------------------------------------------------------
 * PURPOSE  : Set absolute time range for specific entry.
 * INPUT    : name  --  name of the time_range_entry.
 *            is_start_null
 *            is_end_null
 *            start --  start of the absolute time.
 *            end   --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_SetTimeRangeAbsolute(UI8_T *name,
    BOOL_T is_start_null, BOOL_T is_end_null,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    UI32_T index;

    if ((name == NULL) || (start == NULL) || (end == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_OM_GetTimeRangeEntryIndexByName(name, &index))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (FALSE == TIME_RANGE_MGR_LocalCheckAbsoluteTime(is_start_null, is_end_null, 
		                        start, end))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }		                     

    return TIME_RANGE_OM_SetTimeRangeAbsolute(index, is_start_null, is_end_null, start, end);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_SetTimeRangePeriodic
 *-------------------------------------------------------------------------
 * PURPOSE  : Set periodic time range for specific entry.
 * INPUT    : name  --  name of the time_range_entry.
 *            entry --  periodic time_range for the entry.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_SetTimeRangePeriodic(UI8_T *name, 
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    UI32_T index;

    if ((name == NULL) || (entry == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_OM_GetTimeRangeEntryIndexByName(name, &index))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (FALSE == TIME_RANGE_MGR_LocalCheckPeriodicTime(entry))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_SetTimeRangePeriodic(index, entry);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_DeleteTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Delete a time_range_entry with specific name.
 * INPUT    : name        -- Name of the entry to be deleted.
 * OUTPUT   : None
 * RETURN   : Error code
 * NOTES    : None.
 * HISTORY:
 *   mm/dd/yy
 *   04/18/13 -- Add member err_type, please refer struct TIME_RANGE_ERROR_TYPE_T.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_DeleteTimeRangeEntry(UI8_T *name)
{
    UI32_T index;

    if (name == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    /*It is not error case to delete non-existed time-range*/
    if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_OM_GetTimeRangeEntryIndexByName(name, &index))
    {
        return TIME_RANGE_ERROR_TYPE_NONE;
    }

    return TIME_RANGE_OM_DeleteTimeRangeEntry(index);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_DestroyTimeRangeAbsolute
 *-------------------------------------------------------------------------
 * PURPOSE  : Destroy absolute time range for specific entry.
 * INPUT    : name  --  name of the time_range_entry.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : It can not be deleted if other CSCs are bound to the entry.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_DestroyTimeRangeAbsolute(UI8_T *name)
{
    UI32_T index;

    if (name == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_OM_GetTimeRangeEntryIndexByName(name, &index))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_DestroyTimeRangeAbsolute(index);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_DestroyTimeRangePeriodic
 *-------------------------------------------------------------------------
 * PURPOSE  : Destroy periodic time range for specific entry.
 * INPUT    : name  --  name of the time_range_entry.
 *            entry --  periodic time_range for the entry.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : It can not be deleted if other CSCs are bound to the entry.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_DestroyTimeRangePeriodic(UI8_T *name,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    UI32_T index;

    if ((name == NULL) || (entry == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_OM_GetTimeRangeEntryIndexByName(name, &index))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_DestroyTimeRangePeriodic(index, entry);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_GetNextTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next time_range_entry according to current index.
 * INPUT    : index --  index for current time_range_entry.
 * OUTPUT   : index --  index for next time_range_entry.
 *            entry --  next time_range_entry.
 * RETURN   : Error code
 * NOTES    : If index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE,
 *            first entry and first index will be returned.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_GetNextTimeRangeEntry(UI32_T *index,
    TIME_RANGE_TYPE_ENTRY_T *entry)
{
    if ((*index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE) && (*index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_GetNextTimeRangeEntry(index, entry);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_GetNextRunningTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next time_range_entry according to current index.
 * INPUT    : index --  index for current time_range_entry.
 * OUTPUT   : index --  index for next time_range_entry.
 *                  entry --  next time_range_entry.
 * RETURN   : SYS_TYPE_GET_RUNNING_CFG_SUCCESS  --  Success
 *            SYS_TYPE_GET_RUNNING_CFG_FAIL --  Error
 * NOTES    : If index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE,
 *            first entry and first index will be returned.
 * ------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T TIME_RANGE_MGR_GetNextRunningTimeRangeEntry(UI32_T *index,
    TIME_RANGE_TYPE_ENTRY_T *entry)
{
    if ((*index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE) && (*index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY))
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (entry == NULL)
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }

    if (TIME_RANGE_ERROR_TYPE_NONE == TIME_RANGE_OM_GetNextTimeRangeEntry(index, entry))
    {
        return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_RequestTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Bind specific entry to other CSC,
 *                   or unbind specific entry from other CSC.
 * INPUT    : name  --  name of the time_range_entry.
 *            bind  --  TRUE : bind the entry to CSC.
 *                      FALSE: unbind the entry from CSC.
 * OUTPUT   : index --  index of the entry.
 * RETURN   : Error code
 * NOTES    : bind is like lock the time-range entry, must be executed first
 *            unbind is like unlock the time-range entry, must be executed last
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_RequestTimeRangeEntry(UI8_T *name, BOOL_T bind, UI32_T *index)
{
    if ((name == NULL) || (index == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_RequestTimeRangeEntry(name, bind, index);
}

/***************************************
 * These APIs below are for SNMP only. *
 ***************************************/

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibCreateEmptyTimeRangeEntryById
 *-------------------------------------------------------------------------
 * PURPOSE  : Create an empty time_range_entry with specific index
 * INPUT    : index --  index of the time_range_entry
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibCreateEmptyTimeRangeEntryById(UI32_T index)
{
    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_MibCreateEmptyTimeRangeEntry(index);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibSetTimeRangeEntryNameById
 *-------------------------------------------------------------------------
 * PURPOSE  : Set time_range_entry name with specific index
 * INPUT    : index --  index of the time_range_entry
 *            name  --  name of the time_range_entry
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibSetTimeRangeEntryNameById(UI32_T index, UI8_T *name)
{
    int i = 0;
    char *c = (char *) &name[0];

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (name == NULL)
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;

    while (*(c + i) != '\0')
    {
        if (SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH <= i)
        {
            return TIME_RANGE_ERROR_TYPE_OUT_OF_NAME_LENGTH;
        }

        if (L_CHARSET_IsValidGenericChar(*(c + i)) == 0)
        {
            return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
        }

        i++;
    }    

    return TIME_RANGE_OM_MibSetTimeRangeEntryName(index, name);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibDeleteTimeRangeEntryById
 *-------------------------------------------------------------------------
 * PURPOSE  : Delete a time_range_entry with specific index.
 * INPUT    : index        -- index of the entry to be deleted.
 * OUTPUT   : none
 * RETURN   : Error code
 * NOTES    : None.
 * HISTORY:
 *   mm/dd/yy
 *   04/18/13 -- Add member err_type, please refer struct TIME_RANGE_ERROR_TYPE_T.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibDeleteTimeRangeEntryById(UI32_T index)
{
    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_DeleteTimeRangeEntry(index);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibSetTimeRangePeriodicById
 *-------------------------------------------------------------------------
 * PURPOSE  : Set periodic_entry with time-range index
 * INPUT    : index   -- index of the time-range
 *            entry   -- periodic_entry
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibSetTimeRangePeriodicById(UI32_T index,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }
		
    if (FALSE == TIME_RANGE_MGR_LocalCheckPeriodicTime(entry))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_SetTimeRangePeriodic(index, entry);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibDestroyTimeRangePeriodicById
 *-------------------------------------------------------------------------
 * PURPOSE  : Destroy a periodic_entry with specific time-range index
 * INPUT    : index   -- index of the time-range
 *                entry   -- periodic_entry
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibDestroyTimeRangePeriodicById(UI32_T index, TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_DestroyTimeRangePeriodic(index, entry);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibSetTimeRangeAbsoluteById
 *-------------------------------------------------------------------------
 * PURPOSE  : Set absolute entry for specific time-range index.
 * INPUT    : index  --  index of the time-range.
 *            start  --  start of the absolute time.
 *            end    --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibSetTimeRangeAbsoluteById(UI32_T index,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    BOOL_T is_start_null, is_end_null;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((start == NULL) || (end == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (FALSE == TIME_RANGE_MGR_LocalConvertAndCheckMibAbsoluteTime(
		             &is_start_null, &is_end_null, start, end))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_SetTimeRangeAbsolute(index, is_start_null, is_end_null, start, end);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibDestroyTimeRangeAbsoluteById
 *-------------------------------------------------------------------------
 * PURPOSE  : Destroy absolute entry for specific time-range index.
 * INPUT    : index  --  index of the time-range.
 *            start  --  start of the absolute time.
 *            end    --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibDestroyTimeRangeAbsoluteById(UI32_T index,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    BOOL_T is_start_null, is_end_null;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((start == NULL) || (end == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (FALSE == TIME_RANGE_MGR_LocalConvertAndCheckMibAbsoluteTime(
		               &is_start_null, &is_end_null, start, end))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_OM_CheckTimeRangeAbsoluteEntry(
		               index, is_start_null, is_end_null, start, end))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    TIME_RANGE_OM_DestroyTimeRangeAbsolute(index);

    return TIME_RANGE_ERROR_TYPE_NONE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibGetTimeRangeEntryById
 *-------------------------------------------------------------------------
 * PURPOSE  : Get time-range entry according to the input index.
 * INPUT    : index  --  the index of the entry.
 * OUTPUT   : entry --  the entry you are looking for.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibGetTimeRangeEntryById(UI32_T index, TIME_RANGE_TYPE_ENTRY_T *entry)
{
    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    return TIME_RANGE_OM_GetTimeRangeEntry(index, entry);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibGetNextTimeRangePeriodicEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next periodic_entry
 * INPUT    : index   -- index of the time-range
 *            entry   -- periodic_entry
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibGetNextTimeRangePeriodicEntry(UI32_T *index, TIME_RANGE_TYPE_PERIODIC_TIME_T *p_entry)
{
    TIME_RANGE_TYPE_ENTRY_T entry;
    I32_T i;

    if ((*index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE) && (*index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (p_entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (*index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE)
    {
        if (TIME_RANGE_ERROR_TYPE_NONE == TIME_RANGE_OM_GetTimeRangeEntry(*index, &entry))
        {
            for(i = 0; i < (SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY-1); i++)
            {
                if (memcmp(p_entry, &entry.periodic_daily_time[i], sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T)) == 0)
                {
                    if (entry.check_periodic[i+1] == TRUE)
                    {
                        memcpy(p_entry, &entry.periodic_daily_time[i+1], sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));
                        return TIME_RANGE_ERROR_TYPE_NONE;
                    }
                    else
                    {
                        break;
                    }
                }
            }         
        }
    }

    while(TIME_RANGE_ERROR_TYPE_NONE == TIME_RANGE_OM_GetNextTimeRangeEntry(index, &entry))
    {
        if(entry.check_periodic[0] == TRUE)
        {
            memcpy(p_entry, &entry.periodic_daily_time[0], sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));
            return TIME_RANGE_ERROR_TYPE_NONE;
        }
    }

    return TIME_RANGE_ERROR_TYPE_FAIL; 
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibGetTimeRangePeriodicEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get periodic-entry
 * INPUT    : index   -- index of the time-range
 *            entry   -- periodic-entry
 * OUTPUT   : None.
 * RETURN   : TIME_RANGE_ERROR_TYPE_INVALID_DATA - Success (The periodic-entry exist)
 *                  Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibGetTimeRangePeriodicEntry(UI32_T index, TIME_RANGE_TYPE_PERIODIC_TIME_T *p_entry)
{
    TIME_RANGE_TYPE_ENTRY_T   entry;
    UI32_T i;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (p_entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if(TIME_RANGE_ERROR_TYPE_NONE == TIME_RANGE_OM_GetTimeRangeEntry(index, &entry))
    {
        for(i=0; i<SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY; i++)
        {
            if((entry.check_periodic[i] == TRUE) && 
		  (0 == memcmp(p_entry, &entry.periodic_daily_time[i], sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T))))
            {
                return TIME_RANGE_ERROR_TYPE_NONE;
            }
        }
    }

    return TIME_RANGE_ERROR_TYPE_FAIL;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibGetNextTimeRangeAbsoluteEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next absolute-entry
 * INPUT    : index -- index of the time-range
 *            start --  start of the absolute time.
 *            end   --  end of the absolute time.
 * OUTPUT   : start --  start of the absolute time.
 *            end   --  end of the absolute time.
 * RETURN   : Error code
 * NOTES    : Get the absolute entry of the next time-range
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibGetNextTimeRangeAbsoluteEntry(
	    UI32_T *index, 
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start, 
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    TIME_RANGE_TYPE_ENTRY_T entry;

    if ((*index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE) && (*index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((start == NULL) || (end == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    while (TIME_RANGE_ERROR_TYPE_NONE == TIME_RANGE_OM_GetNextTimeRangeEntry(index, &entry))
    {
        if ((entry.check_start_absolute == TRUE) || (entry.check_end_absolute == TRUE))
        {
            if (entry.check_start_absolute == TRUE)
            {
                memcpy(start, &entry.start_time, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
            }
            else
            {
                TIME_RANGE_MGR_LocalMibAbsoluteUndefSetting(start);
            }

            if (entry.check_end_absolute == TRUE)
            {
                memcpy(end, &entry.end_time, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
            }
            else
            {
                TIME_RANGE_MGR_LocalMibAbsoluteUndefSetting(end);
            }

            return TIME_RANGE_ERROR_TYPE_NONE;
        }
    }

    return TIME_RANGE_ERROR_TYPE_FAIL;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibGetNextTimeRangeAbsoluteEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next absolute-entry
 * INPUT    : index   -- index of the time-range
 *            start   --  start of the absolute time.
 *            end     --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : TIME_RANGE_ERROR_TYPE_INVALID_DATA - Success (The absolute entry exist)
 *                  Error code
 * NOTES    : The MIB OID of undefined absolute-entry must be 65535.255.255.255.255
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibGetTimeRangeAbsoluteEntry(
	    UI32_T index, 
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start, 
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    BOOL_T is_start_null, is_end_null;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((start == NULL) || (end == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (FALSE == TIME_RANGE_MGR_LocalConvertAndCheckMibAbsoluteTime(
		                  &is_start_null, &is_end_null, start, end))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (TIME_RANGE_ERROR_TYPE_NONE == TIME_RANGE_OM_CheckTimeRangeAbsoluteEntry(
		                  index, is_start_null, is_end_null, start, end))
    {
        return TIME_RANGE_ERROR_TYPE_NONE;
    }
    else
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_HandleIPCReqMsg
 *-------------------------------------------------------------------------
 * PURPOSE  : Handle the ipc request message for SFLOW MGR.
 * INPUT    : ipcmsg_p -- input request ipc message buffer
 * OUTPUT   : ipcmsg_p -- output response ipc message buffer
 * RETURN   : TRUE  - There is a response need to send,
 *                 FALSE - There is no response to send.
 * NOTES    : None
 * ------------------------------------------------------------------------
 */
BOOL_T TIME_RANGE_MGR_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p;

    if (ipcmsg_p == NULL)
    {
        return FALSE;
    }

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)ipcmsg_p->msg_buf;

    if (SYSFUN_GET_CSC_OPERATING_MODE() == SYS_TYPE_STACKING_TRANSITION_MODE)
    {
        SYSFUN_Debug_Printf("%s(): In transition mode.\r\n", __FUNCTION__);
        msg_p->type.result_ui32 = TIME_RANGE_ERROR_TYPE_FAIL;
        ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
        return TRUE;
    }

    switch (msg_p->type.cmd)
    {
        case TIME_RANGE_MGR_IPCCMD_IS_TIME_RANGE_ENTRY_VALID_BY_NAME:
            msg_p->type.result_ui32 = 
	         TIME_RANGE_MGR_IsTimeRangeEntryValidByName(msg_p->data.name);
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(name);
            break;

        case TIME_RANGE_MGR_IPCCMD_GET_TIME_RANGE_ENTRY_BY_NAME:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_GetTimeRangeEntryByName(msg_p->data.entry.name,
                                                        &(msg_p->data.entry.entry));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(entry);
            break;

        case TIME_RANGE_MGR_IPCCMD_CREATE_TIME_RANGE_ENTRY:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_CreateTimeRangeEntry(msg_p->data.entry.name);
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(entry);
            break;

        case TIME_RANGE_MGR_IPCCMD_SET_TIME_RANGE_ABSOLUTE:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_SetTimeRangeAbsolute(msg_p->data.absolute.name,
                                                    msg_p->data.absolute.is_start_null,
                                                    msg_p->data.absolute.is_end_null,
                                                    &(msg_p->data.absolute.start),
                                                    &(msg_p->data.absolute.end));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;

        case TIME_RANGE_MGR_IPCCMD_SET_TIME_RANGE_PERIODIC:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_SetTimeRangePeriodic(msg_p->data.periodic.name,
                                               &(msg_p->data.periodic.entry));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;

        case TIME_RANGE_MGR_IPCCMD_DELETE_TIME_RANGE_ENTRY:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_DeleteTimeRangeEntry(msg_p->data.entry.name);
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(entry);
            break;

        case TIME_RANGE_MGR_IPCCMD_DESTROY_TIME_RANGE_ABSOLUTE:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_DestroyTimeRangeAbsolute(msg_p->data.name);
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;

        case TIME_RANGE_MGR_IPCCMD_DESTROY_TIME_RANGE_PERIODIC:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_DestroyTimeRangePeriodic(msg_p->data.periodic.name,
                                                    &(msg_p->data.periodic.entry));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;

        case TIME_RANGE_MGR_IPCCMD_GET_NEXT_TIME_RANGE_ENTRY:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_GetNextTimeRangeEntry(&(msg_p->data.next.index),
                                                    &(msg_p->data.next.entry));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(next);
            break;

        case TIME_RANGE_MGR_IPCCMD_GET_NEXT_RUNNING_TIME_RANGE_ENTRY:
            msg_p->type.result_running_cfg =
                TIME_RANGE_MGR_GetNextRunningTimeRangeEntry(&(msg_p->data.next.index),
                                                            &(msg_p->data.next.entry));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(next);
            break;

        case TIME_RANGE_MGR_IPCCMD_REQUEST_TIME_RANGE_ENTRY:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_RequestTimeRangeEntry(msg_p->data.request.name,
                                                     msg_p->data.request.bind,
                                                   &(msg_p->data.request.index));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(request);
            break;

        /* APIs for SNMP - start */
        case TIME_RANGE_MGR_IPCCMD_MIB_CREATE_EMPTY_TIME_RANGE_ENTRY_BY_ID:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_MibCreateEmptyTimeRangeEntryById(msg_p->data.index);
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;

        case TIME_RANGE_MGR_IPCCMD_MIB_SET_TIME_RANGE_ENTRY_NAME_BY_ID:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_MibSetTimeRangeEntryNameById(msg_p->data.entry.index,
                                                         msg_p->data.entry.name);
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;

        case TIME_RANGE_MGR_IPCCMD_MIB_DELETE_TIME_RANGE_ENTRY_BY_ID:
            msg_p->type.result_ui32 = 
                TIME_RANGE_MGR_MibDeleteTimeRangeEntryById(msg_p->data.index);
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;

        case TIME_RANGE_MGR_IPCCMD_MIB_SET_TIME_RANGE_PERIODIC_BY_ID:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_MibSetTimeRangePeriodicById(msg_p->data.periodic_id.index,
                                                        &(msg_p->data.periodic_id.entry));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;

        case TIME_RANGE_MGR_IPCCMD_MIB_DESTROY_TIME_RANGE_PERIODIC_BY_ID:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_MibDestroyTimeRangePeriodicById(msg_p->data.periodic_id.index,
                                                            &(msg_p->data.periodic_id.entry));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;

        case TIME_RANGE_MGR_IPCCMD_MIB_SET_TIME_RANGE_ABSOLUTE_BY_ID:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_MibSetTimeRangeAbsoluteById(msg_p->data.absolute_id.index,
                                                        &(msg_p->data.absolute_id.start),
                                                        &(msg_p->data.absolute_id.end));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;

        case TIME_RANGE_MGR_IPCCMD_MIB_DESTROY_TIME_RANGE_ABSOLUTE_BY_ID:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_MibDestroyTimeRangeAbsoluteById(msg_p->data.absolute_id.index,
                                                            &(msg_p->data.absolute_id.start),
                                                            &(msg_p->data.absolute_id.end));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;			

        case TIME_RANGE_MGR_IPCCMD_MIB_GET_TIME_RANGE_ENTRY_BY_ID:
            msg_p->type.result_ui32 = 
                TIME_RANGE_MGR_MibGetTimeRangeEntryById(msg_p->data.entry.index, 
                                                          &(msg_p->data.entry.entry));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(entry);
            break;

        case TIME_RANGE_MGR_IPCCMD_MIB_GET_NEXT_TIME_RANGE_PERIODIC_ENTRY:
            msg_p->type.result_ui32 = 
                TIME_RANGE_MGR_MibGetNextTimeRangePeriodicEntry(&(msg_p->data.periodic_id.index), 
                                                          &(msg_p->data.periodic_id.entry));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(periodic_id);
            break;

        case TIME_RANGE_MGR_IPCCMD_MIB_GET_TIME_RANGE_PERIODIC_ENTRY:
            msg_p->type.result_ui32 = 
                TIME_RANGE_MGR_MibGetTimeRangePeriodicEntry(msg_p->data.periodic_id.index, 
                                                          &(msg_p->data.periodic_id.entry));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(periodic_id);
            break;	

        case TIME_RANGE_MGR_IPCCMD_MIB_GET_NEXT_TIME_RANGE_ABSOLUTE_ENTRY:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_MibGetNextTimeRangeAbsoluteEntry(
                                                          &(msg_p->data.absolute_id.index),
                                                          &(msg_p->data.absolute_id.start), 
                                                          &(msg_p->data.absolute_id.end));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(absolute_id);
            break;

        case TIME_RANGE_MGR_IPCCMD_MIB_GET_TIME_RANGE_ABSOLUTE_ENTRY:
            msg_p->type.result_ui32 =
                TIME_RANGE_MGR_MibGetTimeRangeAbsoluteEntry(
                                                          msg_p->data.absolute_id.index,
                                                          &(msg_p->data.absolute_id.start), 
                                                          &(msg_p->data.absolute_id.end));
            ipcmsg_p->msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(absolute_id);
            break;
        /* APIs for SNMP - end */

        default:
            SYSFUN_Debug_Printf("%s(): Invalid cmd(%d).\n", __FUNCTION__, (int)(ipcmsg_p->cmd));
            msg_p->type.result_ui32 = TIME_RANGE_ERROR_TYPE_FAIL;
            ipcmsg_p->msg_size = TIME_RANGE_MGR_MSGBUF_TYPE_SIZE;
            break;
    }

    return TRUE;
}
