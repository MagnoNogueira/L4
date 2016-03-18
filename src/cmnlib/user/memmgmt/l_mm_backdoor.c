/* MODULE NAME:  l_mm_backdoor.c
 * PURPOSE:
 *     User space l_mm backdoor
 *
 * NOTES:
 *     BACKDOOR_OPEN in kernel L_MM.c must be defined as TRUE when using this
 *     module in user space.
 *     Cares should be taken the definition of L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER
 *     must be consistent with the definition in kernel mode L_MM.c.
 *
 * HISTORY
 *    2006/10/26 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2006
 */
 
/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h> /* for strtoul() */

/* accton include files */
#include "sys_type.h"
#include "l_mm.h"
#include "l_mm_type.h"
#include "sysfun.h"
#include "backdoor_mgr.h"

/* NAMING CONSTANT DECLARATIONS
 */
/* L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER should be FALSE for
 * release build to avoid hindering system performance.
 */
#define L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER FALSE

#define L_MM_BACKDOOR_MAX_DISPLAY_TASKNAME_LEN 11

#define L_MM_SHOW_ALL_ID               0xFFFFFFFF

/* For L_MM_Backdoor user id display mode definition
 */
enum {
    L_MM_BACKDOOR_DISPLAY_POOL_AND_TRACE = 0,
    L_MM_BACKDOOR_DISPLAY_EXT_TRACE,
    L_MM_BACKDOOR_MAX_DISPLAY_MODE
};

/* MACRO FUNCTION DECLARATIONS
 */
#if (L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER==TRUE)
/* for showing backdoor menu item
 */
#define L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER_SHOW_BACKDOOR_MENU_ITEM() \
    BACKDOOR_MGR_Print("\r\n 8. Toggle malloc buffer elapsed time check flag");

/* for toggling the flag for checking elapsed time of malloc buffer
 */
#define L_MM_DEBUG_TOGGLE_FLAG_OF_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER() \
{\
    BOOL_T check_elapsed_time_flag=(BOOL_T)SYSFUN_Syscall(SYSFUN_SYSCALL_L_MM, L_MM_CMD_BACKDOOR, L_MM_BACKDOOR_CMD_TOGGLECHECKELAPSEDTIMEOFMALLOCBUFFERFLAG, NULL, NULL, NULL);\
    BACKDOOR_MGR_Printf("\r\nFlag %s", check_elapsed_time_flag ? "On" : "Off");\
}

#else
#define L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER_SHOW_BACKDOOR_MENU_ITEM()
#define L_MM_DEBUG_TOGGLE_FLAG_OF_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER()
#endif

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static void L_MM_BACKDOOR_ShowBufferInfo(UI32_T task_id, UI32_T module_id, UI32_T elapsed_time);
static void L_MM_BACKDOOR_ShowMrefInfo(UI32_T task_id, UI32_T module_id, UI32_T elapsed_time);
static BOOL_T L_MM_BACKDOOR_GetTaskIdFromConsole(UI32_T *value_p);
static BOOL_T L_MM_BACKDOOR_GetModuleIdFromConsole(UI32_T *value_p);
static BOOL_T L_MM_BACKDOOR_GetElapsedTimeFromConsole(UI32_T *value_p);

/* STATIC VARIABLE DECLARATIONS
 */
/* for table header of different display modes
 */
static char*  header_str[L_MM_BACKDOOR_MAX_DISPLAY_MODE] = {"PL_ID TR_ID", "EXT_TR_ID  "};
static UI8_T   uid_display_mode=L_MM_BACKDOOR_DISPLAY_POOL_AND_TRACE;

/* EXPORTED SUBPROGRAM BODIES
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_BACKDOOR_BackDoorMenu
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    L_MM kernel space backdoor menu entry funtion.
 *
 * INPUT:
 *    None.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    None.
 *
 * NOTES:
 *    This function should be called directly by backdoor_mgr which handles backdoor
 *    transactions.
 *------------------------------------------------------------------------------
 */
void L_MM_BACKDOOR_BackDoorMenu(void)
{
    UI32_T task_id, module_id, elapsed_time;
    char   str_buf[3];
    BOOL_T is_exit=FALSE;

    /*  BODY
     */

    while(is_exit==FALSE)
    {
        BACKDOOR_MGR_Print("\r\n==========L_MM BackDoor Menu================\n");
        BACKDOOR_MGR_Print("\r\n 0. Exit");
        BACKDOOR_MGR_Print("\r\n 1. Show ALL L_MM_Monitor_T info");
        BACKDOOR_MGR_Print("\r\n 2. Show L_MM_Monitor_T info by a specific taskID, moduleID, and elpased time > a given value");
        BACKDOOR_MGR_Print("\r\n 3. Show L_MM_Mref_T info by a specific taskID, moduleID, and elpased time > a given value");
        BACKDOOR_MGR_Print("\r\n 4. Show total number of allocated buffer");
        BACKDOOR_MGR_Print("\r\n 5. Toggle debug message flag");
        BACKDOOR_MGR_Print("\r\n 6. Toggle validate free flag");
        BACKDOOR_MGR_Print("\r\n 7. Toggle user id display mode");
        /* item 8
         */
        L_MM_DEBUG_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER_SHOW_BACKDOOR_MENU_ITEM();
        BACKDOOR_MGR_Print("\r\n=================================================");
        BACKDOOR_MGR_Print("\r\nselect =");

        BACKDOOR_MGR_RequestKeyIn(str_buf, sizeof(str_buf)-1);

        switch (str_buf[0])
        {
            case '0' :
                is_exit = TRUE;
                break;
            case '1': /* Show ALL L_MM_Monitor_T info */
                L_MM_BACKDOOR_ShowBufferInfo(L_MM_SHOW_ALL_ID, L_MM_SHOW_ALL_ID, 0);
                break;
            case '2': /* Show L_MM_Monitor_T info by a specific taskID, moduleID, and elpased time > a given value */
                BACKDOOR_MGR_Print("\r\nTask id(input hex value, '*' means show all):0x");
                if(FALSE==L_MM_BACKDOOR_GetTaskIdFromConsole(&task_id))
                {
                    BACKDOOR_MGR_Print("\r\nInvalid value for task id.");
                    break;
                }
                BACKDOOR_MGR_Print("\r\nModule id(input dec value, '*' means show all):");
                if(FALSE==L_MM_BACKDOOR_GetModuleIdFromConsole(&module_id))
                {
                    BACKDOOR_MGR_Print("\r\nInvalid value for module id.");
                    break;
                }
                BACKDOOR_MGR_Print("\r\nElapsed time(input dec value, '0' means show all):");
                if(FALSE==L_MM_BACKDOOR_GetElapsedTimeFromConsole(&elapsed_time))
                {
                    BACKDOOR_MGR_Print("\r\nInvalid value for elapsed time.\n");
                    break;
                }
                L_MM_BACKDOOR_ShowBufferInfo(task_id, module_id, elapsed_time);
                break;
            case '3': /* Show L_MM_Mref_T info by a specific taskID, moduleID, and elpased time > a given value */
                BACKDOOR_MGR_Print("\r\nTask id(input hex value, '*' means show all):0x");
                if(FALSE==L_MM_BACKDOOR_GetTaskIdFromConsole(&task_id))
                {
                    printf("\r\nInvalid value for task id.");
                    break;
                }
                BACKDOOR_MGR_Print("\r\nModule id(input dec value, '*' means show all):");
                if(FALSE==L_MM_BACKDOOR_GetModuleIdFromConsole(&module_id))
                {
                    printf("\r\nInvalid value for module id.");
                    break;
                }
                BACKDOOR_MGR_Print("\r\nElapsed time(input dec value, '0' means show all):");
                if(FALSE==L_MM_BACKDOOR_GetElapsedTimeFromConsole(&elapsed_time))
                {
                    printf("\r\nInvalid value for elapsed time.\n");
                    break;
                }
                L_MM_BACKDOOR_ShowMrefInfo(task_id, module_id, elapsed_time);
                break;
            case '4': /* Show total number of allocated buffer */
                BACKDOOR_MGR_Printf("\r\nTotal number of allocated buffer = %ld",
                       SYSFUN_Syscall(SYSFUN_SYSCALL_L_MM, L_MM_CMD_BACKDOOR, L_MM_BACKDOOR_CMD_GETTOTALNBROFALLOCATEDBUFFER, NULL, NULL, NULL));
                break;
            case '5': /* Toggle debug message flag */
            {
                BOOL_T dbg_msg_flag = (UI8_T)SYSFUN_Syscall(SYSFUN_SYSCALL_L_MM, L_MM_CMD_BACKDOOR, L_MM_BACKDOOR_CMD_TOGGLEDBGMSGFLAG, NULL, NULL, NULL);
                BACKDOOR_MGR_Printf("\r\nDebug message flag = %s", (dbg_msg_flag==TRUE)?"TRUE":"FALSE");
            }
                break;
            case '6': /* Toggle validation free flag */
            {
                BOOL_T validate_free_flag = (UI8_T)SYSFUN_Syscall(SYSFUN_SYSCALL_L_MM, L_MM_CMD_BACKDOOR, L_MM_BACKDOOR_CMD_TOGGLEVALIDATEFREEFLAG, NULL, NULL, NULL);
                BACKDOOR_MGR_Printf("\r\nValidate free flag = %s", (validate_free_flag==TRUE)?"TRUE":"FALSE");
            }
                break;
            case '7': /* Toogle uid display mode */
                if(uid_display_mode==L_MM_BACKDOOR_DISPLAY_POOL_AND_TRACE)
                {
                    uid_display_mode=L_MM_BACKDOOR_DISPLAY_EXT_TRACE;
                    BACKDOOR_MGR_Print("\r\nDisplay ext trace id.");
                }
                else
                {
                    uid_display_mode=L_MM_BACKDOOR_DISPLAY_POOL_AND_TRACE;
                    BACKDOOR_MGR_Print("\r\nDisplay pool id and trace id.");
                }
                break;
            case '8': /* Toggle malloc buffer elapsed time check flag */
                L_MM_DEBUG_TOGGLE_FLAG_OF_CHECK_ELAPSED_TIME_OF_MALLOC_BUFFER();
                break;
            default :
                break;
        }
    }   /*  end of while    */

    return 0;
}


/* LOCAL SUBPROGRAM BODIES
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_BACKDOOR_ShowBufferInfo
 *------------------------------------------------------------------------------
 * PURPOSE:
 *  Display Monitored Buffer information by given conditions
 *
 * INPUT:
 *  task_id      -- specify the task_id to display (or L_MM_SHOW_ALL_ID  to display all tasks)
 *  module_id    -- specify the module_id to display (or L_MM_SHOW_ALL_ID to display all modules)
 *  elapsed_time -- for those buffer with (current system time -allocation_time > elapsed_time)
 *                  are the condition to display (or 0 to display all)
 * OUTPUT:
 *  None.
 *
 * RETURN:
 *  None.
 *
 * NOTES:
 *   1.The given conditions (task_id, module_id, elapsed_time) will all be
 *     checked for display (They are ANDed, not ORed)
 *   2.The buffer info is retrieved from kernel one by one. So there are chances
 *     that the retrieved monitored buffer is freed so that there is no way to
 *     proceed to next monitored buffer.
 *------------------------------------------------------------------------------
 */
static void L_MM_BACKDOOR_ShowBufferInfo(UI32_T task_id, UI32_T module_id, UI32_T elapsed_time)
{
    L_MM_Backdoor_BufferInfo_T buffer_info;
    UI32_T current_time = SYSFUN_GetSysTick();
    UI32_T check_time = current_time - elapsed_time;
    UI32_T calc_elapsed_time;
    char *taskname_p;
    char taskname[SYSFUN_TASK_NAME_LENGTH+1];
    BOOL_T is_normal_scenario = (current_time>check_time) ? TRUE : FALSE;

    /* There are two scenarios should be considered for elapsed_time checking:
     * The first scenario is Normal Scenario:
     *     If the range of timestamp we're going to check is not
     *     across the maximum value of UI32_T(i.e. 0xFFFFFFFF).
     * The other scenario is Start-over Scenario:
     *     If the range of timestamp we're going to check is across the boundary of the maximum value of
     *     UI32_T.
     *
     * Normal Scenario is illustrated as the chart shown below:
     *  |***********|xxxxxxxxxxxxxxxxx|------------------>
     *  0           ^                 ^
     *              |<--------------->|
     *              |   elapsed_time  |
     *              +->check_time     |
     *                                +->current_time
     * Assume sys tick starts from 0, and the time stamp meet the condition should fall within the range
     * marked as '*'. If the time stamp fall in the range marked as 'x', that means life time of the buffer
     * is smaller than elpased_time. Thus the checking condition for Normal Scenario would be:
     *              time_stamp < check_time
     * Further more, if the sys tick has been across 0xFFFFFFFF, there might be some buffer with timestamp greater
     * than current_time, those buffer should also be included. At last, the checking condition would be:
     *              (time_stamp < check_time) || (time_stamp>current_time)
     * However, if the timestamp is assigned before sys tick was across 0xFFFFFFFF and the timestamp happens to
     * fall in 'x' range, we would consider the buffer doesn't meet the condition but actually it does meet.
     *
     * Start-over Scenario is illustrated as the chart shown below:
     *  |xxxxxx|**************** ... *******************|xxxxxxxxxxxxxxxxxx|
     *  0      ^                                        ^                  ^(0xFFFFFFFF)
     *  |<---->|                                        |<---------------->|
     *     a   |                                        |        b
     *         +->current_time                          +->check_time
     * The meaning of legends '*' and 'x' is the same as in Normal Condition.
     * Assume a + b == elapsed_time.
     * The checking condition for this scenario would be:
     *              (time_stamp < check_time) && (time_stamp > current_time)
     */

    BACKDOOR_MGR_Printf("\r\n   TASK_ID TASK_NAME    ELAPSEDTM MAG BUF_T M_ID %s   BUF_SZ BUF_ADDR", header_str[uid_display_mode]);
    BACKDOOR_MGR_Print("\r\n------------------------------------------------------------------------------");

    buffer_info.last_ref_monitor_p=NULL;

    while(((BOOL_T)SYSFUN_Syscall(SYSFUN_SYSCALL_L_MM, L_MM_CMD_BACKDOOR,
        L_MM_BACKDOOR_CMD_GETBUFFERINFO, (UI32_T)&buffer_info, 0, 0))
        == TRUE)
    {
        /* check whether task_id mached */
        if ( (task_id != L_MM_SHOW_ALL_ID) && (task_id != buffer_info.task_id) )
            continue;

        /* check whether module_id matched */
        if ( (module_id != L_MM_SHOW_ALL_ID) && (module_id != (L_MM_GET_MODULEID_FROM_USERID(buffer_info.uid)) ) )
            continue;

        /* Check whether elapsed time matched */
        if(elapsed_time!=0)
        {
            if ( is_normal_scenario == TRUE )
            {
                if ( (buffer_info.allocation_time> check_time) &&
                     (buffer_info.allocation_time <= current_time)
                   )
                    continue;
            } /* end of if( is_normal_scenario == TRUE ) */
            else /* is_normal_scenario == */
            {
                if ( (buffer_info.allocation_time > check_time) ||
                     (buffer_info.allocation_time <= current_time)
                   )
                   continue;
            } /* end of else */
        } /* end of if(elapsed_time!=0) */

        /* Display buffer info for matched buffer */
        /* Sample print out format:
         *   TASK_ID TASK_NAME    ELAPSEDTM MAG BUF_T M_ID PL_ID TR_ID   BUF_SZ BUF_ADDR
         *------------------------------------------------------------------------------
         *0x12345678 Sample_Task 1234567890 R,R  0,63   123   1     12 12345678 001A0000
         */

        if(SYSFUN_OK != SYSFUN_TaskIDToName(buffer_info.task_id, &taskname, sizeof(taskname)))
            taskname_p = "Unknown";
        else
            taskname_p=&taskname;

        if(buffer_info.allocation_time<=current_time)
            calc_elapsed_time = current_time - buffer_info.allocation_time;
        else
            calc_elapsed_time = 0xFFFFFFFF - (buffer_info.allocation_time - current_time);

        BACKDOOR_MGR_Printf("\r\n0x%8lx %-11s %10lu %1s,%1s  %1hu,%1hu   %3hu",
            buffer_info.task_id, /* TASK_ID */
            taskname_p,          /* TASK_NAME */
            calc_elapsed_time,   /* ELAPSEDTM */
            (buffer_info.is_valid_leading_magic_byte==TRUE) ? "R":"W", /* leading magic byte check */
            (buffer_info.is_valid_trail_magic_byte==TRUE) ? "R":"W", /* trail magic byte check */
            (UI8_T)L_MM_GET_INTERNAL_BUFTYPE(buffer_info.buffer_type), /* BUF_TYPE(Internal) */
            (UI8_T)L_MM_GET_APP_BUFTYPE(buffer_info.buffer_type), /* BUF_TYPE(Application defined) */
            (UI8_T)L_MM_GET_MODULEID_FROM_USERID(buffer_info.uid) /* MOD_ID */);

        switch(uid_display_mode)
        {
            case L_MM_BACKDOOR_DISPLAY_POOL_AND_TRACE:
            default:
                BACKDOOR_MGR_Printf("     %1hu    %2hu ", 
                    (UI8_T)L_MM_GET_POOLID_FROM_USERID(buffer_info.uid), /* POOL_ID */
                    (UI8_T)L_MM_GET_TRACEID_FROM_USERID(buffer_info.uid) /* TRACE_ID */);
                break;
            case L_MM_BACKDOOR_DISPLAY_EXT_TRACE:
                BACKDOOR_MGR_Printf("    %3hu      ", (UI8_T)L_MM_GET_EXT_TRACEID_FROM_USERID(buffer_info.uid) /* TRACE_ID */);
                break;
        }

        BACKDOOR_MGR_Printf("%8hu %8X",
               buffer_info.buf_len /* buffer length */,
               (int)buffer_info.buffer_addr /* buffer address */);
    } /* end of while((BOOL_T)... */
} /* end of L_MM_BACKDOOR_ShowBufferInfo() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_BACKDOOR_ShowMrefInfo
 *------------------------------------------------------------------------------
 * PURPOSE:
 *  Display multiple reference information by given conditions
 *------------------------------------------------------------------------------
 * INPUT:
 *  task_id      -- specify the task_id to display (or L_MM_SHOW_ALL_ID  to display all tasks)
 *  module_id    -- specify the module_id to display (or L_MM_SHOW_ALL_ID to display all modules)
 *  elapsed_time -- for those mref with (current system time -allocation_time > elapsed_time)
 *                  are the condition to display (or 0 to display all)
 *
 * OUTPUT:
 *  None.
 *
 * RETURN:
 *  None.
 *
 * NOTES:
 *   1.The given conditions (task_id, module_id, elapsed_time) will all be
 *     checked for display (They are ANDed, not ORed)
 *   2.SYSFUN_InterruptLock/SYSFUN_InterruptUnlock are needed in this procedure
 */
static void L_MM_BACKDOOR_ShowMrefInfo(UI32_T task_id, UI32_T module_id, UI32_T elapsed_time)
{
    L_MM_Backdoor_MrefInfo_T mref_info;
    char*  taskname_p;
    char taskname[SYSFUN_TASK_NAME_LENGTH+1];
    UI32_T current_time = SYSFUN_GetSysTick();
    UI32_T check_time = current_time - elapsed_time;
    UI32_T calc_elapsed_time;
    BOOL_T is_normal_scenario = (current_time>check_time) ? TRUE : FALSE;

    /* See comment in L_MM_BACKDOOR_ShowBufferInfo() for description of Normal Scenario and other scenario. */

    BACKDOOR_MGR_Printf("\r\nM_ID %s CUR_USR NXT_USR REF_CNT BUF_LEN PDU_OFFSET PDU_LEN BUF_ADDR", header_str[uid_display_mode]);
    BACKDOOR_MGR_Print("\r\n----------------------------------------------------------------------------");

    mref_info.buffer_info.last_ref_monitor_p=NULL;
    while(((BOOL_T)SYSFUN_Syscall(SYSFUN_SYSCALL_L_MM, L_MM_CMD_BACKDOOR,
        L_MM_BACKDOOR_CMD_GETMREFINFO,
        (UI32_T)&mref_info, NULL, NULL)) == TRUE)
    {
        /* check whether buffer type matched */
        if(0==(L_MM_GET_INTERNAL_BUFTYPE(mref_info.buffer_info.buffer_type) & L_MM_MREF_BUFFER_TYPE))
            continue;

        /* check whether task_id matched */
        if ( (task_id != L_MM_SHOW_ALL_ID) && (task_id != mref_info.buffer_info.task_id) )
            continue;

        /* check whether module_id matched */
        if ( (module_id != L_MM_SHOW_ALL_ID) && (module_id != (L_MM_GET_MODULEID_FROM_USERID(mref_info.buffer_info.uid)) ) )
            continue;

        /* Check whether elapsed time matched */
        if(elapsed_time!=0)
        {
            if ( is_normal_scenario == TRUE )
            {
                if ( (mref_info.buffer_info.allocation_time > check_time) &&
                     (mref_info.buffer_info.allocation_time <= current_time)
                   )
                    continue;
            } /* end of if( is_normal_scenario == TRUE ) */
            else /* is_normal_scenario == */
            {
                if ( (mref_info.buffer_info.allocation_time > check_time) ||
                     (mref_info.buffer_info.allocation_time <= current_time)
                   )
                   continue;
            } /* end of else */
        } /* end of if(elapsed_time!=0) */

        /* Display mref info for matched mref */
        /* Sample print out format:
         *M_ID PL_ID TR_ID CUR_USR NXT_USR REF_CNT BUF_LEN PDU_OFFSET PDU_LEN BUF_ADDR
         *----------------------------------------------------------------------------
         * 123   1     12   0x1234  0x1234   12345   12345    12345     12345 010A0000
         */

        if(SYSFUN_OK != SYSFUN_TaskIDToName(mref_info.buffer_info.task_id, &taskname, sizeof(taskname)))
            taskname_p = "Unknown";
        else
            taskname_p = &taskname;

        if(mref_info.buffer_info.allocation_time<=current_time)
            calc_elapsed_time = current_time - mref_info.buffer_info.allocation_time;
        else
            calc_elapsed_time = 0xFFFFFFFF - (mref_info.buffer_info.allocation_time - current_time);

        BACKDOOR_MGR_Printf("\r\n %3hu ", (UI8_T)L_MM_GET_MODULEID_FROM_USERID(mref_info.buffer_info.uid) /* M_ID(module id) */);

        switch(uid_display_mode)
        {
            case L_MM_BACKDOOR_DISPLAY_POOL_AND_TRACE:
            default:
                BACKDOOR_MGR_Printf("  %1hu    %2hu", 
                    (UI8_T)L_MM_GET_POOLID_FROM_USERID(mref_info.buffer_info.uid), /* POOL_ID */
                    (UI8_T)L_MM_GET_TRACEID_FROM_USERID(mref_info.buffer_info.uid) /* TRACE_ID */);
                break;
            case L_MM_BACKDOOR_DISPLAY_EXT_TRACE:
                BACKDOOR_MGR_Printf("      %3hu",
		(UI8_T)L_MM_GET_EXT_TRACEID_FROM_USERID(mref_info.buffer_info.uid) /* TRACE_ID */);
                break;
        }

        BACKDOOR_MGR_Printf("    0x%.4x  0x%.4x   %5u   %5u      %5u   %5u %8x",
            mref_info.current_usr_id, /* CUR_USR(current_usr_id) */
            mref_info.next_usr_id, /* NXT_USR(next_usr_id) */
            mref_info.ref_count, /* REF_CNT(reference count) */
            mref_info.buffer_info.buf_len, /* BUF_LEN(buffer length) */
            mref_info.pdu_offset, /* PDU_OFFSET */
            mref_info.pdu_len /* PDU_LEN */,
            (int)mref_info.buffer_info.buffer_addr);
    } /* end of while((BOOL_T)... */
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_BACKDOOR_GetTaskIdFromConsole
 *------------------------------------------------------------------------------
 * PURPOSE:
 *  Get task id(hex value) from console.
 * INPUT:
 *  None.
 * OUTPUT:
 *  value_p  -- task id which is input from console will be stored in this variable.
 *
 * RETURN:
 *  TRUE  -- Get valid task id from console.
 *  FALSE -- Get invalid task id from console.
 *
 * NOTES:
 *  None.
 *------------------------------------------------------------------------------
 */
static BOOL_T L_MM_BACKDOOR_GetTaskIdFromConsole(UI32_T *value_p)
{
    char str_buf[16], *ch_p;

    BACKDOOR_MGR_RequestKeyIn(str_buf, sizeof(str_buf)-1);

    if(str_buf[0] == '*')
    {
        *value_p = L_MM_SHOW_ALL_ID;
        return TRUE;
    }

    ch_p = str_buf;
    while( (*ch_p) != 0 )
    {
        /* validate input value */
        if(((*ch_p < '0') || (*ch_p > '9')) &&
           ((*ch_p < 'a') || (*ch_p > 'f')) &&
           ((*ch_p < 'A') || (*ch_p > 'F'))
          )
            return FALSE;
        ch_p++;
    }

    *value_p = (UI32_T)strtoul(str_buf, NULL, 16);
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_BACKDOOR_GetModuleIdFromConsole
 *------------------------------------------------------------------------------
 * PURPOSE:
 *  Get module id(decimal value) from console.
 * INPUT:
 *  None.
 * OUTPUT:
 *  value_p  -- module id which is input from console will be stored in this variable.
 *
 * RETURN:
 *  TRUE  -- Get valid module id from console.
 *  FALSE -- Get invalid module id from console.
 *
 * NOTES:
 *  None.
 *------------------------------------------------------------------------------
 */
static BOOL_T L_MM_BACKDOOR_GetModuleIdFromConsole(UI32_T *value_p)
{
    char str_buf[16], *ch_p;

    BACKDOOR_MGR_RequestKeyIn(str_buf, sizeof(str_buf)-1);

    if(str_buf[0] == '*')
    {
        *value_p = L_MM_SHOW_ALL_ID;
        return TRUE;
    }

    ch_p = str_buf;
    while( (*ch_p) != 0 )
    {
        /* validate input value */
        if((*ch_p < '0') || (*ch_p > '9'))
            return FALSE;
        ch_p++;
    }

    *value_p = (UI32_T)atol(str_buf);
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L_MM_BACKDOOR_GetElapsedTimeFromConsole
 *------------------------------------------------------------------------------
 * PURPOSE:
 *  Get elapsed time from console.
 * INPUT:
 *  None.
 * OUTPUT:
 *  value_p  -- elapsed time from console will be stored in this variable.
 *
 * RETURN:
 *  TRUE  -- Get valid value from console.
 *  FALSE -- Get invalid value from console.
 *
 * NOTES:
 *  None.
 *------------------------------------------------------------------------------
 */
static BOOL_T L_MM_BACKDOOR_GetElapsedTimeFromConsole(UI32_T *value_p)
{
    char str_buf[16], *ch_p;

    BACKDOOR_MGR_RequestKeyIn(str_buf, sizeof(str_buf)-1);

    ch_p = str_buf;
    while( (*ch_p) != 0 )
    {
        /* validate input value */
        if((*ch_p < '0') || (*ch_p > '9'))
            return FALSE;
        ch_p++;
    }

    *value_p = (UI32_T)atol(str_buf);
    return TRUE;
}

