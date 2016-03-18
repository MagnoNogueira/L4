/*-----------------------------------------------------------------------------
 * Module   : TIME_RANGE_OM.h
 *-----------------------------------------------------------------------------
 * PURPOSE  : TIME_RANGE_OM serves as a database to store time_range_entry.
 *
 *-----------------------------------------------------------------------------
 * HISTORY  : 2010.02 - Nelson Dai, Create
 *
 *
 *-----------------------------------------------------------------------------
 * Copyright(C)                               Accton Corporation, 2010
 *-----------------------------------------------------------------------------
 */
#ifndef  _TIME_RANGE_OM_H
#define  _TIME_RANGE_OM_H

#include "sys_type.h"
#include "sysfun.h"
#include "sysrsc_mgr.h"
#include "time_range_type.h"

typedef struct
{
    SYSFUN_DECLARE_CSC_ON_SHMEM

    TIME_RANGE_TYPE_ENTRY_T entry_table[SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY];

} TIME_RANGE_OM_ShmemData_T;

/*------------------------------------------------------------------------------
 * FUNCTION NAME: TIME_RANGE_OM_InitiateSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Initialize system resources.
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void TIME_RANGE_OM_InitiateSystemResources(void);

/* FUNCTION NAME: TIME_RANGE_OM_GetShMemInfo
 *------------------------------------------------------------------------------
 * PURPOSE: Get share memory info
 *------------------------------------------------------------------------------
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None
 *------------------------------------------------------------------------------
 * NOTES:
 */
void TIME_RANGE_OM_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p);

/*------------------------------------------------------------------------------
 * FUNCTION NAME: TIME_RANGE_OM_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for TIME_RANGE_OM in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void TIME_RANGE_OM_AttachSystemResources(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_Initiate_System_Resources
 *--------------------------------------------------------------------------
 * PURPOSE  : This function is used to initialize the time_range OM module.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *            FALSE   -- unspecified failure, system error.
 * NOTES    : Call by TIME_RANGE_TASK_Initiate_System_Resources() only.
 *--------------------------------------------------------------------------*/
BOOL_T TIME_RANGE_OM_Initiate_System_Resources(void);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_CreateTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Create a time_range_entry with specific name.
 * INPUT    : name        -- The name of the new entry you want to create.
 * OUTPUT : None
 * RETURN   : TIME_RANGE_ERROR_TYPE_SAME_DATA
 *                  TIME_RANGE_ERROR_TYPE_OUT_OF_TIME_RANGE_ENTRY
 *                  TIME_RANGE_ERROR_TYPE_NONE
 * NOTES    : None.
 * HISTORY:
 *   mm/dd/yy
 *   03/06/13 -- Add member err_type, please refer struct TIME_RANGE_ERROR_TYPE_T.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_CreateTimeRangeEntry(UI8_T *name);

 /*------------------------------------------------------------------------------
 * FUNCTION NAME: TIME_RANGE_OM_GetTimeRangeEntryIndexByName
 *------------------------------------------------------------------------------
 * PURPOSE: This function is used to get time-range index by name
 * INPUT:   name - time-range name
 * OUTPUT:  index - time-range index
 * RETURN   : TRUE  --  Success
 *                  FALSE --  Error
 * NOTES: Call by time_range_mgr.c only.
 *------------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_GetTimeRangeEntryIndexByName(UI8_T *name, UI32_T *index);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_IsTimeRangeEntryValidByName
 *-------------------------------------------------------------------------
 * PURPOSE  : check if the time range entry is valid by name
 * INPUT    : index  --  the index of the entry.
 * OUTPUT   : none
 * RETURN   : TRUE  --  Valid
 *                  FALSE --  Invalid
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_IsTimeRangeEntryValid(UI32_T index);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_GetTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get time_range_entry's index according to the input name.
 * INPUT    : index  --  time-range index.
 * OUTPUT   : entry --  the entry you are looking for.
 * RETURN   : TRUE  --  Success
 *                  FALSE --  Error
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_GetTimeRangeEntry(UI32_T index, TIME_RANGE_TYPE_ENTRY_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_SetTimeRangeAbsolute
 *-------------------------------------------------------------------------
 * PURPOSE  : Set absolute time range for specific entry.
 * INPUT    : index  --  index of the time_range_entry.
 *            start --  start of the absolute time.
 *            end   --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : TRUE  --  Success
 *            FALSE --  Error
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_SetTimeRangeAbsolute(UI32_T index,
    BOOL_T is_start_null,
    BOOL_T is_end_null,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_SetTimeRangePeriodic
 *-------------------------------------------------------------------------
 * PURPOSE  : Set periodic time range for specific entry.
 * INPUT    : index  --  name of the time_range_entry.
 *                entry --  periodic time_range for the entry.
 * OUTPUT   : None.
 * RETURN   : TRUE  --  Success
 *            FALSE --  Error
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_SetTimeRangePeriodic(UI32_T index,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_DeleteTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Delete a time_range_entry with specific index.
 * INPUT    : index        -- index of the entry to be deleted.
 * OUTPUT   : None
 * RETURN   : TIME_RANGE_ERROR_TYPE_REFERENCE_IN_USING
 *            TIME_RANGE_ERROR_TYPE_NONE
 * NOTES    : 1. The entry can not be deleted if other CSCs are bound to it.
 *            2. User can delete the entry without destroying absolute/periodic time range first.
 * HISTORY:
 *   mm/dd/yy
 *   04/19/13 -- Add member err_type, please refer struct TIME_RANGE_ERROR_TYPE_T.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_DeleteTimeRangeEntry(UI32_T index);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_DestroyTimeRangeAbsolute
 *-------------------------------------------------------------------------
 * PURPOSE  : Destroy absolute time range for specific entry.
 * INPUT    : index  --  index of the time_range_entry.
 * OUTPUT   : None.
 * RETURN   : TRUE  --  Success
 *            FALSE --  Error
 * NOTES    : It can not be deleted if other CSCs are bound to the entry.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_DestroyTimeRangeAbsolute(UI32_T index);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_DestroyTimeRangePeriodic
 *-------------------------------------------------------------------------
 * PURPOSE  : Destroy periodic time range for specific entry.
 * INPUT    : index  --  index of the time_range_entry.
 *                entry --  periodic time_range for the entry.
 * OUTPUT   : None.
 * RETURN   : TRUE  --  Success
 *                  FALSE --  Error
 * NOTES    : It can not be deleted if other CSCs are bound to the entry.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_DestroyTimeRangePeriodic(UI32_T index,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_GetNextTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next time_range_entry according to current index.
 * INPUT    : index --  index for current time_range_entry.
 * OUTPUT   : index --  index for next time_range_entry.
 *            entry --  next time_range_entry.
 * RETURN   : TRUE  --  Success
 *            FALSE --  Error
 * NOTES    : If index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE,
 *            first entry and first index will be returned.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_GetNextTimeRangeEntry(UI32_T *index,
    TIME_RANGE_TYPE_ENTRY_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_RequestTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Bind specific entry to other CSC,
 *            or unbind specific entry from other CSC.
 * INPUT    : name  --  name of the time_range_entry.
 *            bind  --  bind to other CSC or not.
 * OUTPUT   : index --  index of the entry.
 * RETURN   : TRUE  --  Success
 *            FALSE --  Error
 * NOTES    : bind == TRUE : bind the entry to CSC.
 *            bind == FALSE: unbind the entry from CSC.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_RequestTimeRangeEntry(UI8_T *name, BOOL_T bind, UI32_T *index);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_IsTimeRangeEntryActive
 *-------------------------------------------------------------------------
 * PURPOSE  : Check if specific entry is active or not.
 * INPUT    : index  --  index of the time_range_entry
 * OUTPUT   : is_active -- TRUE: active
 *                        FALSE: inactive
 * RETURN   :  TRUE  --  Success
 *            FALSE --  Error
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_IsTimeRangeEntryActive(UI32_T index, BOOL_T *is_active);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_UpdateEntryStatus
 *-------------------------------------------------------------------------
 * PURPOSE  : Update every entry's status.
 * INPUT    : current_time.
 * OUTPUT   : old_status --  bitmap to store entries' old status.
 *            new_status --  bitmap to store entries' new status.
 * RETURN   : TRUE  --  Success
 *            FALSE --  Error
 * NOTES    : bitmap size is SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST
 * ------------------------------------------------------------------------
 */
BOOL_T TIME_RANGE_OM_UpdateEntryStatus(TIME_RANGE_TYPE_CONVERT_TIME_T *current_time,
    UI8_T *old_status, UI8_T *new_status);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_CheckTimeRangeAbsoluteEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next absolute-entry
 * INPUT    : index   -- index of the time-range
 *            is_start_null
 *            is_end_null
 *            start  --  start of the absolute time.
 *            end    --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : TRUE  --  Success (The absolute-entry exist)
 *            FALSE --  Error
 * NOTES    : None
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_CheckTimeRangeAbsoluteEntry(
	    UI32_T index,
	    BOOL_T is_start_null,
	    BOOL_T is_end_null,
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start, 
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

/***************************************
 * These APIs below are for SNMP only. *
 ***************************************/

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_MibCreateEmptyTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Create an empty time_range_entry with specific index
 * INPUT    : index --  index of the time_range_entry
 * OUTPUT   : None.
 * RETURN   : TRUE  --  Success
 *            FALSE --  Error
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_MibCreateEmptyTimeRangeEntry(UI32_T index);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_MibSetTimeRangeEntryName
 *-------------------------------------------------------------------------
 * PURPOSE  : Set time_range_entry name with specific index
 * INPUT    : index --  index of the time_range_entry
 *            name  --  name of the time_range_entry
 * OUTPUT   : None.
 * RETURN   : TRUE  --  Success
 *            FALSE --  Error
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_MibSetTimeRangeEntryName(UI32_T index, UI8_T *name);

#endif
