/*-----------------------------------------------------------------------------
 * Module   : TIME_RANGE_OM.c
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

//ezio
#ifdef shmem_data_p
#define temp_shemem_p shmem_data_p
#endif

#define shmem_data_p time_range_shmem_data_p

#include <string.h>
#include "sys_adpt.h"
#include "sys_bld.h"
#include "sysfun.h"
#include "time_range_om.h"

/* MACRO FUNCTION DECLARATIONS
 */
#define TIME_RANGE_OM_EnterCriticalSection() SYSFUN_TakeSem(time_range_om_sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER)
#define TIME_RANGE_OM_LeaveCriticalSection() SYSFUN_GiveSem(time_range_om_sem_id)

/* STATIC VARIABLE DECLARATIONS
 */
static UI32_T time_range_om_sem_id;
static TIME_RANGE_OM_ShmemData_T *shmem_data_p;

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
void TIME_RANGE_OM_InitiateSystemResources(void)
{
    shmem_data_p = (TIME_RANGE_OM_ShmemData_T *)SYSRSC_MGR_GetShMem(SYSRSC_MGR_TIME_RANGE_OM_SHMEM_SEGID);
    SYSFUN_GetSem(SYS_BLD_SYS_SEMAPHORE_KEY_TIME_RANGE_OM, &time_range_om_sem_id);
    SYSFUN_INITIATE_CSC_ON_SHMEM(shmem_data_p);

    return;
}

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
void TIME_RANGE_OM_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p)
{
    *segid_p = SYSRSC_MGR_TIME_RANGE_OM_SHMEM_SEGID;
    *seglen_p = sizeof(TIME_RANGE_OM_ShmemData_T);
}

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
void TIME_RANGE_OM_AttachSystemResources(void)
{
    shmem_data_p = (TIME_RANGE_OM_ShmemData_T *)SYSRSC_MGR_GetShMem(SYSRSC_MGR_TIME_RANGE_OM_SHMEM_SEGID);
    SYSFUN_GetSem(SYS_BLD_SYS_SEMAPHORE_KEY_TIME_RANGE_OM, &time_range_om_sem_id);
}

/* LOCAL SUBPROGRAM DECLARATIONS
 */
/*------------------------------------------------------------------------------
 * FUNCTION NAME: TIME_RANGE_OM_LocalInitTimeRangeEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE: This function is used to initialize the time_range OM by specific index
 * INPUT:   index - index of the time-range
 * OUTPUT:  None
 * RETURN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
static void TIME_RANGE_OM_LocalInitTimeRangeEntryByIndex(UI32_T index)
{
    UI32_T i;

    memset(&shmem_data_p->entry_table[index], 0, sizeof(TIME_RANGE_TYPE_ENTRY_T));
    shmem_data_p->entry_table[index].valid = FALSE;
    shmem_data_p->entry_table[index].active = FALSE;
    shmem_data_p->entry_table[index].check_start_absolute = FALSE;
    shmem_data_p->entry_table[index].check_end_absolute = FALSE;
    for (i = 0; i < SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY; ++i)
    {
        shmem_data_p->entry_table[index].check_periodic[i] = FALSE;
    }
}

 /*------------------------------------------------------------------------------
 * FUNCTION NAME: TIME_RANGE_OM_LocalGetTimeRangeEntryIndexByName
 *------------------------------------------------------------------------------
 * PURPOSE: This function is used to get time-range index by name
 * INPUT:   name - time-range name
 * OUTPUT:  index - time-range index
 * RETURN   : TRUE  --  Success
 *            FALSE --  Error
 * NOTES: Call by time_range_mgr.c only.
 *------------------------------------------------------------------------------
 */
static BOOL_T TIME_RANGE_OM_LocalGetTimeRangeEntryIndexByName(UI8_T *name, UI32_T *index)
{
    UI32_T i;

    for (i = 0; i < SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY; ++i)
    {
        if (FALSE == shmem_data_p->entry_table[i].valid)  // if this entry is not in use
        {
            continue;
        }

        if (0 == strcmp((char *) name, (char *) shmem_data_p->entry_table[i].name))
        {
            *index = i;
            return TRUE;
        }
    }

    return FALSE;
}


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
BOOL_T TIME_RANGE_OM_Initiate_System_Resources(void)
{
    UI32_T i;

    /* create semaphore */
    if (SYSFUN_CreateSem(SYSFUN_SEMKEY_PRIVATE, 1, SYSFUN_SEM_FIFO, &time_range_om_sem_id) != SYSFUN_OK)
    {
        return FALSE;
    }

    /* init database */
    for (i = 0; i < SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY; ++i)
    {
        TIME_RANGE_OM_LocalInitTimeRangeEntryByIndex(i);
    }

    return TRUE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_CreateTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Create a time_range_entry with specific name.
 * INPUT    : name        -- The name of the new entry you want to create.\
 * OUTPUT : None
 * RETURN   : Error code
 * NOTES    : None.
 * HISTORY:
 *   mm/dd/yy
 *   03/06/13 -- Add member err_type, please refer struct TIME_RANGE_ERROR_TYPE_T.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_CreateTimeRangeEntry(UI8_T *name)
{
    UI32_T i;
    UI32_T empty_index = TIME_RANGE_TYPE_UNDEF_TIME_RANGE;

    TIME_RANGE_OM_EnterCriticalSection();

    for (i = 0; i < SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY; ++i)
    {
        /* If this entry is in use, then check its name.
         * Entries can not have same names.
         */
        if (TRUE == shmem_data_p->entry_table[i].valid)
        {
            if (0 == strcmp((char *) name, (char *) shmem_data_p->entry_table[i].name))
            {
                TIME_RANGE_OM_LeaveCriticalSection();
                return TIME_RANGE_ERROR_TYPE_SAME_DATA;
            }
        }
        else
        {
            if (empty_index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE)
            {
                break;
            }
            else
            {
                empty_index = i;
            }
        }
    }

    if (empty_index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE)  // still have free space
    {
        /* Init the entry */
        TIME_RANGE_OM_LocalInitTimeRangeEntryByIndex(empty_index);
        strncpy((char *) shmem_data_p->entry_table[empty_index].name, (char *) name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
        shmem_data_p->entry_table[empty_index].valid = TRUE;

        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_NONE;
    }

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_OUT_OF_TIME_RANGE_ENTRY;
}

 /*------------------------------------------------------------------------------
 * FUNCTION NAME: TIME_RANGE_OM_GetTimeRangeEntryIndexByName
 *------------------------------------------------------------------------------
 * PURPOSE: This function is used to get time-range index by name
 * INPUT:   name - time-range name
 * OUTPUT:  index - time-range index
 * RETURN   : Error code
 * NOTES: Call by time_range_mgr.c only.
 *------------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_GetTimeRangeEntryIndexByName(UI8_T *name, UI32_T *index)
{
    BOOL_T ret = TIME_RANGE_ERROR_TYPE_NONE;

    TIME_RANGE_OM_EnterCriticalSection();

    if(TRUE != TIME_RANGE_OM_LocalGetTimeRangeEntryIndexByName(name, index))
    {
        ret = TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    TIME_RANGE_OM_LeaveCriticalSection();

    return ret;	
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_IsTimeRangeEntryValid
 *-------------------------------------------------------------------------
 * PURPOSE  : check if the time range entry is valid by name
 * INPUT    : index --  time-range index.
 * OUTPUT   : none
 * RETURN   : TIME_RANGE_ERROR_TYPE_NONE: The time-range entry is valid
 *                  Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_IsTimeRangeEntryValid(UI32_T index)
{
    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    TIME_RANGE_OM_EnterCriticalSection();

    if (shmem_data_p->entry_table[index].valid == FALSE)
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_NONE;	
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_GetTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get time_range_entry's index according to the input name.
 * INPUT    : index  --  time-range index.
 * OUTPUT   : entry --  the entry you are looking for.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_GetTimeRangeEntry(UI32_T index, TIME_RANGE_TYPE_ENTRY_T *entry)
{
    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    TIME_RANGE_OM_EnterCriticalSection();

    if (shmem_data_p->entry_table[index].valid == FALSE)
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    memcpy(entry, &shmem_data_p->entry_table[index], sizeof(TIME_RANGE_TYPE_ENTRY_T));

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_NONE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_SetTimeRangeAbsolute
 *-------------------------------------------------------------------------
 * PURPOSE  : Set absolute time range for specific entry.
 * INPUT    : index  --  index of the time_range_entry.
 *            is_start_null
 *            is_end_null
 *            start --  start of the absolute time.
 *            end   --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_SetTimeRangeAbsolute(UI32_T index,
    BOOL_T is_start_null,
    BOOL_T is_end_null,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    TIME_RANGE_OM_EnterCriticalSection();

    if (shmem_data_p->entry_table[index].valid != TRUE)
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    /* Assign start and/or end datetime value
     */
    if (is_start_null != TRUE)
    {
        memcpy(&shmem_data_p->entry_table[index].start_time, start, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
        shmem_data_p->entry_table[index].check_start_absolute = TRUE;
    }
    else
    {
        shmem_data_p->entry_table[index].check_start_absolute = FALSE;
    }
	
    if (is_end_null != TRUE)
    {
        memcpy(&shmem_data_p->entry_table[index].end_time, end, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
        shmem_data_p->entry_table[index].check_end_absolute = TRUE;
    }
    else
    {
        shmem_data_p->entry_table[index].check_end_absolute = FALSE;
    }

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_NONE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_SetTimeRangePeriodic
 *-------------------------------------------------------------------------
 * PURPOSE  : Set periodic time range for specific entry.
 * INPUT    : index  --  index of the time_range_entry.
 *            entry  --  periodic time_range for the entry.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_SetTimeRangePeriodic(UI32_T index,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    I32_T i, entry_index=0;
    I32_T compare_result;

    TIME_RANGE_OM_EnterCriticalSection();

    if (shmem_data_p->entry_table[index].valid != TRUE)
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (TRUE == shmem_data_p->entry_table[index].check_periodic[SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY-1])
    {
        /*no free space*/
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_OUT_OF_PERIODIC_POLICY_ENTRY;            
    }	

    for(i=0; i<SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY; i++)
    {
        if (shmem_data_p->entry_table[index].check_periodic[i] == TRUE)
        {
            compare_result = memcmp(&shmem_data_p->entry_table[index].periodic_daily_time[i], entry, 
				sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));

	     if (compare_result == 0)
	     {
	         TIME_RANGE_OM_LeaveCriticalSection();
                return TIME_RANGE_ERROR_TYPE_SAME_DATA;
	     }
            else if (compare_result > 0)
            {
                entry_index = i;
                break;
            }
        }
        else
        {
            /* put the entry in last */
            memcpy(&shmem_data_p->entry_table[index].periodic_daily_time[i], entry, 
		                     sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));
            shmem_data_p->entry_table[index].check_periodic[i] = TRUE;

            TIME_RANGE_OM_LeaveCriticalSection();
            return TIME_RANGE_ERROR_TYPE_NONE;
        }
    }

    /* the entry must be inserted, shift from last */
    for (i = SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY-2; i >= entry_index; i--)
    {
        if (shmem_data_p->entry_table[index].check_periodic[i] == TRUE)
        {
            /*shift*/
            memcpy(&shmem_data_p->entry_table[index].periodic_daily_time[i+1], 
                          &shmem_data_p->entry_table[index].periodic_daily_time[i], sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));
            shmem_data_p->entry_table[index].check_periodic[i+1] = TRUE;
        }
    }

    memcpy(&shmem_data_p->entry_table[index].periodic_daily_time[entry_index], entry, 
		                     sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_NONE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_DeleteTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Delete a time_range_entry with specific index.
 * INPUT    : index        -- index of the entry to be deleted.
 * OUTPUT   : None
 * RETURN   : Error code
 * NOTES    : 1. The entry can not be deleted if other CSCs are bound to it.
 *            2. User can delete the entry without destroying absolute/periodic time range first.
 * HISTORY:
 *   mm/dd/yy
 *   04/19/13 -- Add member err_type, please refer struct TIME_RANGE_ERROR_TYPE_T.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_DeleteTimeRangeEntry(UI32_T index)
{
    TIME_RANGE_OM_EnterCriticalSection();

    if (shmem_data_p->entry_table[index].ref_counter > 0)
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_REFERENCE_IN_USING;
    }

    /* Set valid to FALSE to indicate that entry not being used.
     */
    shmem_data_p->entry_table[index].valid  = FALSE;
    shmem_data_p->entry_table[index].active = FALSE;

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_NONE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_DestroyTimeRangeAbsolute
 *-------------------------------------------------------------------------
 * PURPOSE  : Destroy absolute time range for specific entry.
 * INPUT    : index  --  index of the time_range_entry.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : It can not be deleted if other CSCs are bound to the entry.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_DestroyTimeRangeAbsolute(UI32_T index)
{
    TIME_RANGE_OM_EnterCriticalSection();

    shmem_data_p->entry_table[index].check_start_absolute = FALSE;
    shmem_data_p->entry_table[index].check_end_absolute   = FALSE;
    memset(&(shmem_data_p->entry_table[index].start_time), 0, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
    memset(&(shmem_data_p->entry_table[index].end_time),   0, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));

    TIME_RANGE_OM_LeaveCriticalSection();

    return TIME_RANGE_ERROR_TYPE_NONE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_DestroyTimeRangePeriodic
 *-------------------------------------------------------------------------
 * PURPOSE  : Destroy periodic time range for specific entry.
 * INPUT    : name  --  name of the time_range_entry.
 *            entry --  periodic time_range for the entry.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : It can not be deleted if other CSCs are bound to the entry.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_DestroyTimeRangePeriodic(UI32_T index,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    UI32_T i;
    BOOL_T find_and_delete = FALSE;

    TIME_RANGE_OM_EnterCriticalSection();

    for (i = 0; i < SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY; ++i)
    {
        if (shmem_data_p->entry_table[index].check_periodic[i] != TRUE)
        {
            break;
        }

        if (find_and_delete == FALSE)
        {
            if (0 == memcmp(&shmem_data_p->entry_table[index].periodic_daily_time[i], entry, sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T)))
            {
                shmem_data_p->entry_table[index].check_periodic[i] = FALSE;
                memset(&(shmem_data_p->entry_table[index].periodic_daily_time[i]), 0, sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));
                find_and_delete = TRUE;
            }
        }
        else
        {
            /*shift*/
            memcpy(&shmem_data_p->entry_table[index].periodic_daily_time[i-1], 
                          &shmem_data_p->entry_table[index].periodic_daily_time[i], sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));
            shmem_data_p->entry_table[index].check_periodic[i-1] = TRUE;
            shmem_data_p->entry_table[index].check_periodic[i] = FALSE;
            memset(&(shmem_data_p->entry_table[index].periodic_daily_time[i]), 0, sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));	
	 }
    }

    TIME_RANGE_OM_LeaveCriticalSection();

    return TIME_RANGE_ERROR_TYPE_NONE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_GetNextTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next time_range_entry according to current index.
 * INPUT    : index --  index for current time_range_entry.
 * OUTPUT   : index --  index for next time_range_entry.
 *            entry --  next time_range_entry.
 * RETURN   : Error code
 * NOTES    : If index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE,
 *                  first entry and first index will be returned.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_GetNextTimeRangeEntry(UI32_T *index,
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

    if (*index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE)
    {
        *index = 0; // get first
    }
    else
    {
        *index = *index + 1;    // get next
    }

    TIME_RANGE_OM_EnterCriticalSection();

    for (; *index < SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY; ++(*index))
    {
        if (TRUE != shmem_data_p->entry_table[*index].valid)
        {
            continue;
        }

        memset(entry, 0, sizeof(TIME_RANGE_TYPE_ENTRY_T));
        memcpy(entry, &shmem_data_p->entry_table[*index], sizeof(TIME_RANGE_TYPE_ENTRY_T));

        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_NONE;
    }

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_FAIL;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_RequestTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Bind specific entry to other CSC,
 *                  or unbind specific entry from other CSC.
 * INPUT    : name  --  name of the time_range_entry.
 *            bind  --  TRUE : bind the entry to CSC.
 *                      FALSE: unbind the entry from CSC.
 * OUTPUT   : index --  index of the entry.
 * RETURN   : Error code
 * NOTES    : bind is like lock the time-range entry, must be executed first
 *                 unbind is like unlock the time-range entry, must be executed last
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_RequestTimeRangeEntry(UI8_T *name, BOOL_T bind, UI32_T *index)
{
    TIME_RANGE_OM_EnterCriticalSection();

    if (FALSE == TIME_RANGE_OM_LocalGetTimeRangeEntryIndexByName(name, index))
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (shmem_data_p->entry_table[*index].valid != TRUE)
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (bind == TRUE)
    {
        shmem_data_p->entry_table[*index].ref_counter++;
    }
    else
    {
        shmem_data_p->entry_table[*index].ref_counter--;
    }

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_NONE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_IsTimeRangeEntryActive
 *-------------------------------------------------------------------------
 * PURPOSE  : Check if specific entry is active or not.
 * INPUT    : index  --  index of the time_range_entry
 * OUTPUT   : is_active -- TRUE: active
 *                        FALSE: inactive
 * RETURN   :  Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_IsTimeRangeEntryActive(UI32_T index, BOOL_T *is_active)
{
    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    TIME_RANGE_OM_EnterCriticalSection();

    if (shmem_data_p->entry_table[index].active == FALSE)
    {
        *is_active = FALSE;
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_NONE;
    }

    *is_active = TRUE;
    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_NONE;	
}

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
    UI8_T *old_status, UI8_T *new_status)
{
    UI32_T i, periodic_i;
    UI32_T day_of_week;
    UI32_T current_min, start_min, end_min;
    UI8_T  start_type_of_wk, end_type_of_wk;
    BOOL_T active, compare_min, check_absolute, check_periodic;
    TIME_RANGE_TYPE_ENTRY_T         *p_entry;
    TIME_RANGE_TYPE_PERIODIC_TIME_T *p_periodic_entry;

    TIME_RANGE_OM_EnterCriticalSection();

    memset(old_status, 0x0, SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST);
    memset(new_status, 0x0, SYS_ADPT_TIME_RANGE_TOTAL_NBR_OF_BYTE_FOR_ENTRY_LIST);

    current_min = 3600 * current_time->hour + 60 * current_time->minute + current_time->second;

    for (i = 0; i < SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY; ++i)
    {
        /* Init
         */
        p_entry = &shmem_data_p->entry_table[i];
        check_absolute = FALSE;
        check_periodic = FALSE;
        active         = FALSE;

        if (p_entry->valid == FALSE)
        {
            continue;
        }

        /* First, fill old_status
         */
        if (p_entry->active == TRUE)
            old_status[(UI32_T)(i>>3)] |= (1 << (7 - (i%8)));

        /* Then, update current status
         */
        /* check start absolute time
         */
        if (TRUE == p_entry->check_start_absolute)
        {
            if (current_time->time_by_sec < p_entry->start_time.time_by_sec)
            {
                p_entry->active = FALSE;
                continue;
            }
            check_absolute = TRUE;
        }
        /* check end absolute time
         */
        if (TRUE == p_entry->check_end_absolute)
        {
            if (current_time->time_by_sec > p_entry->end_time.time_by_sec)
            {
                p_entry->active = FALSE;
                continue;
            }
            check_absolute = TRUE;
        }

        /* check periodic time
         */
        for (periodic_i = 0; periodic_i < SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY; ++periodic_i)
        {
            if (FALSE == p_entry->check_periodic[periodic_i])
            {
                continue;
            }

            /* Init
             */
            check_periodic   = TRUE;
            compare_min      = FALSE;
            p_periodic_entry = &p_entry->periodic_daily_time[periodic_i];
            start_type_of_wk = p_periodic_entry->start_type_of_wk;
            end_type_of_wk   = p_periodic_entry->end_type_of_wk;
            start_min        = 3600 * p_periodic_entry->start_hour + 60 * p_periodic_entry->start_minute + p_periodic_entry->start_second;
            end_min          = 3600 * p_periodic_entry->end_hour + 60 * p_periodic_entry->end_minute + p_periodic_entry->end_second;
            day_of_week = current_time->day_of_week;

            switch (start_type_of_wk)
            {
                /* Daily : Monday through Sunday */
                case TIME_RANGE_TYPE_WEEK_DAILY:
                    compare_min = TRUE;
                    break;

                /* Weekdays : Monday through Friday */
                case TIME_RANGE_TYPE_WEEK_WEEKDAYS:
                    if ((day_of_week != TIME_RANGE_TYPE_WEEK_SUNDAY) &&
                        (day_of_week != TIME_RANGE_TYPE_WEEK_SATURDAY))
                    {
                        compare_min = TRUE;
                    }
                    break;

                /* WEEKEND : Saturday and Sunday */
                case TIME_RANGE_TYPE_WEEK_WEEKEND:
                    if ((day_of_week == TIME_RANGE_TYPE_WEEK_SUNDAY) ||
                        (day_of_week == TIME_RANGE_TYPE_WEEK_SATURDAY))
                    {
                        compare_min = TRUE;
                    }
                    break;

                case TIME_RANGE_TYPE_WEEK_SUNDAY:
                case TIME_RANGE_TYPE_WEEK_MONDAY:
                case TIME_RANGE_TYPE_WEEK_TUESDAY:
                case TIME_RANGE_TYPE_WEEK_WEDNESDAY:
                case TIME_RANGE_TYPE_WEEK_THURSDAY:
                case TIME_RANGE_TYPE_WEEK_FRIDAY:
                case TIME_RANGE_TYPE_WEEK_SATURDAY:
                    /* Condition 1: start and end are the same day
                     */
                    if (start_type_of_wk == end_type_of_wk)
                    {
                        /* Normal Case - monday 5:00 ~ monday 7:00
                         */
                        if (end_min > start_min)
                        {
                            if (day_of_week == start_type_of_wk)
                            {
                                compare_min = TRUE;
                            }
                        }
                        /* Whole Day - monday 5:00 ~ monday 5:00
                         */
                        else if (end_min == start_min)
                        {
                            if (day_of_week == start_type_of_wk) /* on the same day - pass */
                            {
                                active = TRUE;
                            }
                        }
                        /* Other - monday 5:00 ~ monday 2:00
                         * (means: only monday 2:01 ~ 4:59 is invalid)
                         */
                        else
                        {
                            if (day_of_week == start_type_of_wk) /* on the same day */
                            {
                                if ((current_min >= start_min) ||
                                    (current_min <= end_min))
                                {
                                    active = TRUE;
                                }
                            }
                            else /* on different day */
                            {
                                active = TRUE;
                            }
                        }
                    }
                    /*
                     Condition 2 : start and end are different day
                     */
                    else
                    {

                        /* Check if end is cross week compared with start, Yes: day += 7, No: none.
                         */
                        if (start_type_of_wk > end_type_of_wk)
                        {
                            end_type_of_wk += 7;

                            /* Check if day of week is cross week comparsion with start, Yes: day += 7, No: none.
                             */
                            if (day_of_week < start_type_of_wk)
                                day_of_week += 7;
                        }                      

                        /* check if today is in range between start and end */
                        if ((day_of_week > start_type_of_wk) && (day_of_week < end_type_of_wk))
                        {
                            active = TRUE;
                        }
                        else if (day_of_week == start_type_of_wk)
                        {
                            if (current_min >= start_min)
                            {
                                active = TRUE;
                            }
                        }
                        else if (day_of_week == end_type_of_wk)
                        {
                            if (current_min < end_min)
                            {
                                active = TRUE;
                            }
                        }
                    }
                    break;
            } /* end of switch (start_type_of_wk) */

            /* check hour and min */
            if (TRUE == compare_min)
            {
                if (current_min >= start_min && current_min < end_min)
                {
                    active = TRUE;
                }
            }

            if (active == TRUE)
            {
                break; /* break for loop */
            }
        } /* for periodic loop */

        if ((check_absolute == TRUE) && (check_periodic == FALSE)) /* if we only set absolute time */
        {
            active = TRUE;
        }

#if 0
        if (p_entry->active != active)
        {
            printf("\r\n%s(%d): Time Range entry %lu:\r\n  Status= %s, Current= %lu, Start= %lu, End= %lu.\r\n", __FUNCTION__, __LINE__, (UI32_T) i, (active == TRUE) ? "Inactive -> Active" : "Active -> Inactive", current_min, start_min, end_min);
            fflush(stdout);
        }
#endif

        p_entry->active = active;

        /* Last, fill new_status
         */
        if (active == TRUE)
        {
            new_status[(UI32_T)(i>>3)] |= (1 << (7 - (i%8)));
        }
    } /* for entry loop */

    TIME_RANGE_OM_LeaveCriticalSection();
    return TRUE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_CheckTimeRangeAbsoluteEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next absolute-entry
 * INPUT    : index   -- index of the time-range
 *            is_start_null
 *            is_end_null
 *            start  --  start of the absolute time. (NULL means don't care)
 *            end    --  end of the absolute time. (NULL means don't care)
 * OUTPUT   : None.
 * RETURN   : TIME_RANGE_ERROR_TYPE_NONE - Success (The absolute-entry exist)
 *                  Error code
 * NOTES    : None
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_CheckTimeRangeAbsoluteEntry(
	    UI32_T index,
	    BOOL_T is_start_null,
	    BOOL_T is_end_null,
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start, 
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *src;
    BOOL_T is_check_start, is_check_end;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((start == NULL) || (end == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    is_check_start = (!is_start_null);
    is_check_end = (!is_end_null);

    TIME_RANGE_OM_EnterCriticalSection();

     if ((index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY) ||
        (shmem_data_p->entry_table[index].valid == FALSE))
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((shmem_data_p->entry_table[index].check_start_absolute != is_check_start) ||
	  (shmem_data_p->entry_table[index].check_end_absolute != is_check_end))
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }	 

    if (TRUE == shmem_data_p->entry_table[index].check_start_absolute)
    {
        src = &shmem_data_p->entry_table[index].start_time;
        if (src->time_by_sec != start->time_by_sec)
        {            
            TIME_RANGE_OM_LeaveCriticalSection();
            return TIME_RANGE_ERROR_TYPE_INVALID_DATA;	            
        }	
    }

    if (TRUE == shmem_data_p->entry_table[index].check_end_absolute)
    {
        src = &shmem_data_p->entry_table[index].end_time;
        if (src->time_by_sec != end->time_by_sec)
        {            
            TIME_RANGE_OM_LeaveCriticalSection();
            return TIME_RANGE_ERROR_TYPE_INVALID_DATA;	            
        }	
    }	        

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_NONE;	
}

/***************************************
 * These APIs below are for SNMP only. *
 ***************************************/

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_MibCreateEmptyTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Create an empty time_range_entry with specific index
 * INPUT    : index --  index of the time_range_entry
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_MibCreateEmptyTimeRangeEntry(UI32_T index)
{
    UI32_T name_num;

    TIME_RANGE_OM_EnterCriticalSection();

    if (shmem_data_p->entry_table[index].valid == TRUE)
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    /* Init the entry */
    TIME_RANGE_OM_LocalInitTimeRangeEntryByIndex(index);

    /* Set default name
     */
    for (name_num = 1; name_num <= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY; ++name_num)
    {
        UI32_T temp_index;

        snprintf(shmem_data_p->entry_table[index].name, sizeof(shmem_data_p->entry_table[index].name), "%lu", name_num);

        if (FALSE == TIME_RANGE_OM_LocalGetTimeRangeEntryIndexByName((UI8_T *)shmem_data_p->entry_table[index].name,
                                                                  &temp_index))
        {
            shmem_data_p->entry_table[index].valid = TRUE;

            TIME_RANGE_OM_LeaveCriticalSection();
            return TIME_RANGE_ERROR_TYPE_NONE;
        }
    }

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_FAIL;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_OM_MibSetTimeRangeEntryName
 *-------------------------------------------------------------------------
 * PURPOSE  : Set time_range_entry name with specific index
 * INPUT    : index --  index of the time_range_entry
 *            name  --  name of the time_range_entry
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_OM_MibSetTimeRangeEntryName(UI32_T index, UI8_T *name)
{
    UI32_T temp_index;

    TIME_RANGE_OM_EnterCriticalSection();

    if (shmem_data_p->entry_table[index].valid == FALSE)
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (TRUE == TIME_RANGE_OM_LocalGetTimeRangeEntryIndexByName(name, &temp_index))
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return (temp_index == index) ? TIME_RANGE_ERROR_TYPE_NONE: TIME_RANGE_ERROR_TYPE_SAME_DATA;
    }

    if (shmem_data_p->entry_table[index].ref_counter != 0)
    {
        TIME_RANGE_OM_LeaveCriticalSection();
        return TIME_RANGE_ERROR_TYPE_REFERENCE_IN_USING;
    }

    strncpy((char *) shmem_data_p->entry_table[index].name, (char *) name, sizeof(shmem_data_p->entry_table[index].name) - 1);
    shmem_data_p->entry_table[index].name[sizeof(shmem_data_p->entry_table[index].name) - 1] = '\0';

    TIME_RANGE_OM_LeaveCriticalSection();
    return TIME_RANGE_ERROR_TYPE_NONE;
}

//ezio
#undef shmem_data_p
#ifdef temp_shemem_p
#define shmem_data_p temp_shemem_p
#endif
