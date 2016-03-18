/*-----------------------------------------------------------------------------
 * Module   : TIME_RANGE_PMGR.h
 *-----------------------------------------------------------------------------
 * PURPOSE  : TIME_RANGE_PMGR provides all TIME_RANGE accessing functions,
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
#ifndef  _TIME_RANGE_PMGR_H
#define  _TIME_RANGE_PMGR_H

/* INCLUDE FILE DECLARATIONS
 */
#include "time_range_type.h"

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_InitiateProcessResource
 *
 * PURPOSE:
 *    Initiate resource used in the calling process, means the process that use
 *    this pmgr functions should call this init.
 *
 * INPUT:
 *    None.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    TRUE  --  Success
 *    FALSE --  Error
 *
 * NOTES:
 *    None.
 */
BOOL_T TIME_RANGE_PMGR_InitiateProcessResource(void);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_IsTimeRangeEntryValidByName
 *
 * PURPOSE:
 *    check if the time-range entry is valid by name
 *
 * INPUT:
 *    name: the name of the entry.
 *
 * OUTPUT:
 *    none
 *
 * RETURN:
 *    TIME_RANGE_ERROR_TYPE_NONE: The time-range entry is valid
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_IsTimeRangeEntryValidByName(UI8_T *name);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_GetTimeRangeEntryByName
 *
 * PURPOSE:
 *    Get time_range_entry according to the input name.
 *
 * INPUT:
 *    name: the name of the entry.
 *
 * OUTPUT:
 *    entry: the entry you are looking for.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_GetTimeRangeEntryByName (UI8_T *name, TIME_RANGE_TYPE_ENTRY_T *entry);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_CreateTimeRangeEntry
 *
 * PURPOSE:
 *    Create a time_range_entry with specific name.
 *
 * INPUT:
 *    name: the name of the new entry you want to create.
 *
 * OUTPUT:
 *    None
 *
 * RETURN   : 
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_CreateTimeRangeEntry(UI8_T *name);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_SetTimeRangeAbsolute
 *
 * PURPOSE:
 *    Set absolute time range for specific entry.
 *
 * INPUT:
 *    name:  name of the entry
 *    start: start of the absolute time.
 *    end:   end of the absolute time.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_SetTimeRangeAbsolute (UI8_T *name,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_SetTimeRangePeriodic
 *
 * PURPOSE:
 *    Set periodic time range for specific entry.
 *
 * INPUT:
 *    name:  name of the entry.
 *    entry: periodic time_range for the entry.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_SetTimeRangePeriodic (UI8_T *name,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_DeleteTimeRangeEntry
 *
 * PURPOSE:
 *    Delete a time_range_entry with specific name.
 *
 * INPUT:
 *    name
 *
 * OUTPUT:
 *    None
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    1. The entry can not be deleted if other CSCs are bound to it.
 *    2. User can delete the entry without destroying absolute/periodic time range first.
 *
 * HISTORY:
 *   mm/dd/yy
 *   04/19/13 -- Add member err_type, please refer struct TIME_RANGE_ERROR_TYPE_T.
 */
UI32_T TIME_RANGE_PMGR_DeleteTimeRangeEntry (UI8_T *name);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_DestroyTimeRangeAbsolute
 *
 * PURPOSE:
 *    Destroy absolute time range for specific entry.
 *
 * INPUT:
 *    name: name of the time_range_entry.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    It can not be deleted if other CSCs are bound to the entry.
 */
UI32_T TIME_RANGE_PMGR_DestroyTimeRangeAbsolute (UI8_T *name);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_DestroyTimeRangePeriodic
 *
 * PURPOSE:
 *    Destroy periodic time range for specific entry.
 *
 * INPUT:
 *    name : name of the time_range_entry.
 *    entry: periodic time_range for the entry.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    It can not be deleted if other CSCs are bound to the entry.
 */
UI32_T TIME_RANGE_PMGR_DestroyTimeRangePeriodic (UI8_T *name,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_GetNextTimeRangeEntry
 *
 * PURPOSE:
 *    Get next time_range_entry according to current index.
 *
 * INPUT:
 *    index: index for current time_range_entry
 *
 * OUTPUT:
 *    index: index for next time_range_entry
 *    entry: next time_range_entry
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    If index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE,
 *    first entry and first index will be returned.
 */
UI32_T TIME_RANGE_PMGR_GetNextTimeRangeEntry (UI32_T *index,
    TIME_RANGE_TYPE_ENTRY_T *entry);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_GetNextRunningTimeRangeEntry
 *
 * PURPOSE:
 *    Get next time_range_entry according to current index.
 *
 * INPUT:
 *    index: index for current time_range_entry
 *
 * OUTPUT:
 *    index: index for next time_range_entry
 *    entry: next time_range_entry
 *
 * RETURN:
 *    SYS_TYPE_GET_RUNNING_CFG_SUCCESS  --  Success
 *    SYS_TYPE_GET_RUNNING_CFG_FAIL --  Error
 *
 * NOTES:
 *    If index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE,
 *    first entry and first index will be returned.
 */
SYS_TYPE_Get_Running_Cfg_T TIME_RANGE_PMGR_GetNextRunningTimeRangeEntry (UI32_T *index,
    TIME_RANGE_TYPE_ENTRY_T *entry);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_RequestTimeRangeEntry
 *
 * PURPOSE:
 *    Bind specific entry to other CSC,
 *    or unbind specific entry from other CSC.
 *
 * INPUT:
 *    name: name of the time_range_entry.
 *    bind: TRUE : bind the entry to CSC.
 *          FALSE: unbind the entry from CSC.
 *
 * OUTPUT:
 *    index: index of the entry.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:  bind is like lock the time-range entry, must be executed first
 *              unbind is like unlock the time-range entry, must be executed last
 */
UI32_T TIME_RANGE_PMGR_RequestTimeRangeEntry (UI8_T *name, BOOL_T bind, UI32_T *index);

/***************************************
 * These APIs below are for SNMP only. *
 ***************************************/

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibCreateEmptyTimeRangeEntryById
 *
 * PURPOSE:
 *    Create an empty time_range_entry with specific index
 *
 * INPUT:
 *    index: index of the time_range_entry
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibCreateEmptyTimeRangeEntryById (UI32_T index);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibSetTimeRangeEntryNameById
 *
 * PURPOSE:
 *    Set time_range_entry name with specific index
 *
 * INPUT:
 *    index: index of the time_range_entry
 *    name : name of the time_range_entry
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibSetTimeRangeEntryNameById (UI32_T index, UI8_T *name);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibDeleteTimeRangeEntryById
 *
 * PURPOSE:
 *    Delete a time-range entry with specific time-range index
 *
 * INPUT:
 *    index   -- index of the time-range
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibDeleteTimeRangeEntryById (UI32_T index);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibSetTimeRangePeriodicById
 *
 * PURPOSE:
 *    Set periodic_entry with time-range index
 *
 * INPUT:
 *    index   -- index of the time-range
 *    entry   -- periodic_entry
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibSetTimeRangePeriodicById (UI32_T index,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibDestroyTimeRangePeriodicById
 *
 * PURPOSE:
 *    Destroy a periodic_entry with specific time-range index
 *
 * INPUT:
 *    index   -- index of the time-range
 *    entry   -- periodic_entry
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    TRUE  --  Success
 *    FALSE --  Error
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibDestroyTimeRangePeriodicById (UI32_T index, 
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/* FUNCTION NAME:
 *	TIME_RANGE_PMGR_MibSetTimeRangeAbsoluteById
 *
 * PURPOSE:
 *    Set absolute entry for specific time-range index.
 *
 * INPUT: 
 *    index  --  index of the time-range.
 *    start  --  start of the absolute time.
 *    end   --  end of the absolute time.
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *   Error code
 *
 * NOTES: 
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibSetTimeRangeAbsoluteById (UI32_T index,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

/* FUNCTION NAME:
 *	TIME_RANGE_PMGR_MibDestroyTimeRangeAbsoluteById
 *
 * PURPOSE:
 *    Destroy absolute entry for specific time-range index.
 *
 * INPUT: 
 *    index  --  index of the time-range.
 *    start  --  start of the absolute time.
 *    end   --  end of the absolute time.
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    Error code
 *
 * NOTES: 
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibDestroyTimeRangeAbsoluteById (UI32_T index,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibGetTimeRangeEntryById
 *
 * PURPOSE:
 *    Get time_range_entry according to the input index.
 *
 * INPUT:
 *    name: the index of the entry.
 *
 * OUTPUT:
 *    entry: the entry you are looking for.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibGetTimeRangeEntryById (UI32_T index, TIME_RANGE_TYPE_ENTRY_T *entry);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibGetNextTimeRangePeriodicEntry
 *
 * PURPOSE: 
 *    Get next periodic_entry
 *
 * INPUT: 
 *    index   -- index of the time-range
 *    entry   -- periodic_entry
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    Error code
 *
 * NOTES: 
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibGetNextTimeRangePeriodicEntry(UI32_T *index, TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibGetTimeRangePeriodicEntry
 *
 * PURPOSE: 
 *    Get periodic-entry
 *
 * INPUT: 
 *    index   -- index of the time-range
 *    entry   -- periodic-entry
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    TIME_RANGE_ERROR_TYPE_INVALID_DATA - Success (The periodic-entry exist)
 *    Error code
 *
 * NOTES: 
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibGetTimeRangePeriodicEntry(UI32_T index, TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/* FUNCTION NAME: 
 *    TIME_RANGE_PMGR_MibGetNextTimeRangeAbsoluteEntry
 *
 * PURPOSE: 
 *    Get next absolute-entry
 *
 * INPUT: 
 *    index   -- index of the time-range
 *    start  --  start of the absolute time.
 *    end   --  end of the absolute time.
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    Error code
 *
 * NOTES: 
 *    The MIB OID of undefined absolute-entry must be 65535.255.255.255.255
 */
 UI32_T TIME_RANGE_PMGR_MibGetNextTimeRangeAbsoluteEntry(
	UI32_T *index, 
	TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
	TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

/* FUNCTION NAME: 
 *    TIME_RANGE_PMGR_MibGetNextTimeRangeAbsoluteEntry
 *
 * PURPOSE: 
 *    Get next absolute-entry
 *
 * INPUT: 
 *    index   -- index of the time-range
 *    start  --  start of the absolute time.
 *    end   --  end of the absolute time.
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    TIME_RANGE_ERROR_TYPE_INVALID_DATA - Success (The absolute-entry exist)
 *    Error code
 *
 * NOTES: 
 *    The MIB OID of undefined absolute-entry must be 65535.255.255.255.255
 */
UI32_T TIME_RANGE_PMGR_MibGetTimeRangeAbsoluteEntry(
	UI32_T index, 
	TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
	TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

#endif
