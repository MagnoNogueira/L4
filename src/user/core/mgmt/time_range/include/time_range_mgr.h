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
#ifndef  _TIME_RANGE_MGR_H
#define  _TIME_RANGE_MGR_H

#include "sys_adpt.h"
#include "sys_type.h"
#include "sysfun.h"
#include "time_range_type.h"

/* MACRO FUNCTION DECLARATIONS
 */
#define TIME_RANGE_MGR_MSGBUF_TYPE_SIZE sizeof(union TIME_RANGE_MGR_IPCMsg_Type_U)
#define TIME_RANGE_MGR_GET_MSG_SIZE(field_name)                       \
                    (TIME_RANGE_MGR_MSGBUF_TYPE_SIZE +                \
                    sizeof(((TIME_RANGE_MGR_IPCMsg_T*)0)->data.field_name))

/* DATA TYPE DECLARATIONS
 */

/* structure for the request/response ipc message in csc pmgr and mgr
 */
typedef struct TIME_RANGE_MGR_IPCMsg_S
{
    union TIME_RANGE_MGR_IPCMsg_Type_U
    {
        UI32_T cmd;         /* for sending IPC request. CSCA_MGR_IPC_CMD1 ... */
        UI32_T result_ui32;
        SYS_TYPE_Get_Running_Cfg_T result_running_cfg; /* For running config API */
    } type;

    union
    {
        UI32_T index;
        UI8_T  name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH + 1];

        struct
        {
            UI8_T  name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH + 1];
            UI32_T index;
            TIME_RANGE_TYPE_ENTRY_T entry;
        } entry;

        struct
        {
            UI32_T index;
            TIME_RANGE_TYPE_ENTRY_T entry;
        } next;

        struct
        {
            UI8_T  name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH + 1];
            BOOL_T is_start_null;
            BOOL_T is_end_null;
            TIME_RANGE_TYPE_ABSOLUTE_TIME_T start;
            TIME_RANGE_TYPE_ABSOLUTE_TIME_T end;
        } absolute;

        struct
        {
            UI8_T  name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH + 1];
            TIME_RANGE_TYPE_PERIODIC_TIME_T entry;
        } periodic;

        struct
        {
            UI8_T  name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH + 1];
            BOOL_T bind;
            UI32_T index;
        } request;

        struct
        {
            UI8_T  new_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH + 1];
            UI8_T  old_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH + 1];
            UI32_T new_index;
        } request_for_modify;  		

        struct
        {
            UI32_T index;
            TIME_RANGE_TYPE_ABSOLUTE_TIME_T start;
            TIME_RANGE_TYPE_ABSOLUTE_TIME_T end;
        } absolute_id;

	struct
        {
            UI32_T index;
            TIME_RANGE_TYPE_PERIODIC_TIME_T entry;
        } periodic_id;
    } data; /* contains the supplemental data for the corresponding cmd */
} TIME_RANGE_MGR_IPCMsg_T;

typedef enum TIME_RANGE_MGR_IPCCMD_E
{
    TIME_RANGE_MGR_IPCCMD_IS_TIME_RANGE_ENTRY_VALID_BY_NAME,
    TIME_RANGE_MGR_IPCCMD_GET_TIME_RANGE_ENTRY_BY_NAME,
    TIME_RANGE_MGR_IPCCMD_CREATE_TIME_RANGE_ENTRY,
    TIME_RANGE_MGR_IPCCMD_SET_TIME_RANGE_ABSOLUTE,
    TIME_RANGE_MGR_IPCCMD_SET_TIME_RANGE_PERIODIC,
    TIME_RANGE_MGR_IPCCMD_DELETE_TIME_RANGE_ENTRY,
    TIME_RANGE_MGR_IPCCMD_DESTROY_TIME_RANGE_ABSOLUTE,
    TIME_RANGE_MGR_IPCCMD_DESTROY_TIME_RANGE_PERIODIC,
    TIME_RANGE_MGR_IPCCMD_GET_NEXT_TIME_RANGE_ENTRY,
    TIME_RANGE_MGR_IPCCMD_GET_NEXT_RUNNING_TIME_RANGE_ENTRY,
    TIME_RANGE_MGR_IPCCMD_REQUEST_TIME_RANGE_ENTRY,
    TIME_RANGE_MGR_IPCCMD_MIB_CREATE_EMPTY_TIME_RANGE_ENTRY_BY_ID,
    TIME_RANGE_MGR_IPCCMD_MIB_SET_TIME_RANGE_ENTRY_NAME_BY_ID,
    TIME_RANGE_MGR_IPCCMD_MIB_DELETE_TIME_RANGE_ENTRY_BY_ID,
    TIME_RANGE_MGR_IPCCMD_MIB_SET_TIME_RANGE_PERIODIC_BY_ID,
    TIME_RANGE_MGR_IPCCMD_MIB_DESTROY_TIME_RANGE_PERIODIC_BY_ID,
    TIME_RANGE_MGR_IPCCMD_MIB_SET_TIME_RANGE_ABSOLUTE_BY_ID,
    TIME_RANGE_MGR_IPCCMD_MIB_DESTROY_TIME_RANGE_ABSOLUTE_BY_ID,
    TIME_RANGE_MGR_IPCCMD_MIB_GET_TIME_RANGE_ENTRY_BY_ID,
    TIME_RANGE_MGR_IPCCMD_MIB_GET_NEXT_TIME_RANGE_PERIODIC_ENTRY,
    TIME_RANGE_MGR_IPCCMD_MIB_GET_TIME_RANGE_PERIODIC_ENTRY,
    TIME_RANGE_MGR_IPCCMD_MIB_GET_NEXT_TIME_RANGE_ABSOLUTE_ENTRY,
    TIME_RANGE_MGR_IPCCMD_MIB_GET_TIME_RANGE_ABSOLUTE_ENTRY
} TIME_RANGE_MGR_IPCCMD_T;

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_Initiate_System_Resources
 *--------------------------------------------------------------------------
 * PURPOSE  : This function is used to initialize the time_range module.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *                  FALSE   -- unspecified failure, system error.
 * NOTES    : Call by TIME_RANGE_TASK_Initiate_System_Resources() only.
 *--------------------------------------------------------------------------*/
BOOL_T TIME_RANGE_MGR_Initiate_System_Resources(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_Create_InterCSC_Relation
 *--------------------------------------------------------------------------
 * PURPOSE  : This function is used to create internal CSC relation.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *                  FALSE   -- unspecified failure, system error.
 * NOTES    : Call by UTILITY_GROUP_Create_InterCSC_Relation() only.
 *--------------------------------------------------------------------------*/
BOOL_T TIME_RANGE_MGR_Create_InterCSC_Relation(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_EnterMasterMode
 *--------------------------------------------------------------------------
 * PURPOSE  : It is used to enable the time_range operation while in master mode.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *                  FALSE   -- unspecified failure, system error.
 * NOTES    : None
 *--------------------------------------------------------------------------*/
BOOL_T TIME_RANGE_MGR_EnterMasterMode(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_EnterTransitionMode
 *--------------------------------------------------------------------------
 * PURPOSE  : It's the temporary transition mode between system into master
 *                   mode.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *                  FALSE   -- unspecified failure, system error.
 * NOTES    : None
 *--------------------------------------------------------------------------*/
BOOL_T TIME_RANGE_MGR_EnterTransitionMode(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_EnterSlaveMode
 *--------------------------------------------------------------------------
 * PURPOSE  : Set SFLOW op_state to "SYS_TYPE_STACKING_SLAVE_MODE"
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *                  FALSE   -- unspecified failure, system error.
 * NOTES    : None
 *-------------------------------------------------------------------------- */
BOOL_T TIME_RANGE_MGR_EnterSlaveMode(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_SetTransitionMode
 *--------------------------------------------------------------------------
 * PURPOSE  : This function sets the component to temporary transition mode
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *            F      ALSE   -- unspecified failure, system error.
 * NOTES    : None
 *--------------------------------------------------------------------------
 */
BOOL_T TIME_RANGE_MGR_SetTransitionMode(void);

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_GetOperationMode
 *--------------------------------------------------------------------------
 * PURPOSE  : This functions returns the current operation mode of this component
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE    -- successful.
 *                  FALSE   -- unspecified failure, system error.
 * NOTES    : None
 *--------------------------------------------------------------------------
 */
SYS_TYPE_Stacking_Mode_T TIME_RANGE_MGR_GetOperationMode(void);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_HandleTimerEvent
 *-------------------------------------------------------------------------
 * PURPOSE  : Process the timer event
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : Call by TIME_RANGE_TASK_TaskMain() only.
 * ------------------------------------------------------------------------
 */
void TIME_RANGE_MGR_HandleTimerEvent(void);

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
UI32_T TIME_RANGE_MGR_IsTimeRangeEntryValidByName(UI8_T *name);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_GetTimeRangeEntryByName
 *-------------------------------------------------------------------------
 * PURPOSE  : Get time-range entry according to the input name.
 * INPUT    : name  --  the name of the entry.
 * OUTPUT   : entry --  the entry you are looking for.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_GetTimeRangeEntryByName(UI8_T *name, TIME_RANGE_TYPE_ENTRY_T *entry);

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
UI32_T TIME_RANGE_MGR_CreateTimeRangeEntry(UI8_T *name);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_SetTimeRangeAbsolute
 *-------------------------------------------------------------------------
 * PURPOSE  : Set absolute time range for specific entry.
 * INPUT    : name  --  name of the time_range_entry.
 *                is_start_null
 *                is_end_null
 *                start --  start of the absolute time.
 *                end   --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_SetTimeRangeAbsolute(UI8_T *name,
    BOOL_T is_start_null, BOOL_T is_end_null,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_SetTimeRangePeriodic
 *-------------------------------------------------------------------------
 * PURPOSE  : Set periodic time range for specific entry.
 * INPUT    : name  --  name of the time_range_entry.
 *                 entry --  periodic time_range for the entry.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_SetTimeRangePeriodic(UI8_T *name,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

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
UI32_T TIME_RANGE_MGR_DeleteTimeRangeEntry(UI8_T *name);

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
UI32_T TIME_RANGE_MGR_DestroyTimeRangeAbsolute(UI8_T *name);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_DestroyTimeRangePeriodic
 *-------------------------------------------------------------------------
 * PURPOSE  : Destroy periodic time range for specific entry.
 * INPUT    : name  --  name of the time_range_entry.
 *                  entry --  periodic time_range for the entry.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : It can not be deleted if other CSCs are bound to the entry.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_DestroyTimeRangePeriodic(UI8_T *name,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_GetNextTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next time_range_entry according to current index.
 * INPUT    : index --  index for current time_range_entry.
 * OUTPUT   : index --  index for next time_range_entry.
 *                  entry --  next time_range_entry.
 * RETURN   : Error code
 * NOTES    : If index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE,
 *            first entry and first index will be returned.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_GetNextTimeRangeEntry(UI32_T *index,
    TIME_RANGE_TYPE_ENTRY_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_GetNextRunningTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next time_range_entry according to current index.
 * INPUT    : index --  index for current time_range_entry.
 * OUTPUT   : index --  index for next time_range_entry.
 *                  entry --  next time_range_entry.
 * RETURN   : Error code
 * NOTES    : If index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE,
 *                 first entry and first index will be returned.
 * ------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T TIME_RANGE_MGR_GetNextRunningTimeRangeEntry(UI32_T *index,
    TIME_RANGE_TYPE_ENTRY_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_RequestTimeRangeEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Bind specific entry to other CSC,
 *            or unbind specific entry from other CSC.
 * INPUT    : name  --  name of the time_range_entry.
 *                 bind  --  TRUE : bind the entry to CSC.
 *                              FALSE: unbind the entry from CSC.
 * OUTPUT   : index --  index of the entry.
 * RETURN   : Error code
 * NOTES    : bind is like lock the time-range entry, must be executed first
 *                 unbind is like unlock the time-range entry, must be executed last
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_RequestTimeRangeEntry(UI8_T *name, BOOL_T bind, UI32_T *index);

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
UI32_T TIME_RANGE_MGR_MibCreateEmptyTimeRangeEntryById(UI32_T index);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibSetTimeRangeEntryNameById
 *-------------------------------------------------------------------------
 * PURPOSE  : Set time_range_entry name with specific index
 * INPUT    : index --  index of the time_range_entry
 *               name  --  name of the time_range_entry
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibSetTimeRangeEntryNameById(UI32_T index, UI8_T *name);

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
UI32_T TIME_RANGE_MGR_MibDeleteTimeRangeEntryById(UI32_T index);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibSetTimeRangePeriodicById
 *-------------------------------------------------------------------------
 * PURPOSE  : Set periodic_entry with time-range index
 * INPUT    : index   -- index of the time-range
 *                entry   -- periodic_entry
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibSetTimeRangePeriodicById(UI32_T index,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

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
UI32_T TIME_RANGE_MGR_MibDestroyTimeRangePeriodicById(UI32_T index, TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibSetTimeRangeAbsoluteById
 *-------------------------------------------------------------------------
 * PURPOSE  : Set absolute entry for specific time-range index.
 * INPUT    : index  --  index of the time-range.
 *                 start  --  start of the absolute time.
 *                 end   --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibSetTimeRangeAbsoluteById(UI32_T index,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibDestroyTimeRangeAbsoluteById
 *-------------------------------------------------------------------------
 * PURPOSE  : Destroy absolute entry for specific time-range index.
 * INPUT    : index  --  index of the time-range.
 *                start  --  start of the absolute time.
 *                end   --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibDestroyTimeRangeAbsoluteById(UI32_T index,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

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
UI32_T TIME_RANGE_MGR_MibGetTimeRangeEntryById(UI32_T index, TIME_RANGE_TYPE_ENTRY_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibGetNextTimeRangePeriodicEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next periodic_entry
 * INPUT    : index   -- index of the time-range
 *                entry   -- periodic_entry
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibGetNextTimeRangePeriodicEntry(UI32_T *index, TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibGetTimeRangePeriodicEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get periodic-entry
 * INPUT    : index   -- index of the time-range
 *                entry   -- periodic-entry
 * OUTPUT   : None.
 * RETURN   : TIME_RANGE_ERROR_TYPE_INVALID_DATA - Success (The periodic-entry exist)
 *                  Error code
 * NOTES    : None.
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibGetTimeRangePeriodicEntry(UI32_T index, TIME_RANGE_TYPE_PERIODIC_TIME_T *entry);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibGetNextTimeRangeAbsoluteEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next absolute-entry
 * INPUT    : index   -- index of the time-range
 *                start  --  start of the absolute time.
 *                end   --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : Error code
 * NOTES    : Get the absolute entry of the next time-range
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibGetNextTimeRangeAbsoluteEntry(
	    UI32_T *index, 
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start, 
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_MGR_MibGetNextTimeRangeAbsoluteEntry
 *-------------------------------------------------------------------------
 * PURPOSE  : Get next absolute-entry
 * INPUT    : index   -- index of the time-range
 *                  start  --  start of the absolute time.
 *                  end   --  end of the absolute time.
 * OUTPUT   : None.
 * RETURN   : TIME_RANGE_ERROR_TYPE_INVALID_DATA - Success (The absolute entry exist)
 *                  Error code
 * NOTES    : The MIB OID of undefined absolute-entry must be 65535.255.255.255.255
 * ------------------------------------------------------------------------
 */
UI32_T TIME_RANGE_MGR_MibGetTimeRangeAbsoluteEntry(
	    UI32_T index, 
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start, 
	    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end);

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
BOOL_T TIME_RANGE_MGR_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p);

#endif
