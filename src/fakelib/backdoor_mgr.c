#include "config.h"

#include <stdarg.h>
#include "backdoor_mgr.h"

#if __WIN32__
#include "backdoor_mgr_win32.c"
#elif __OSX__
#include "backdoor_mgr_osx.c"
#endif

#if (RUN_UNIT_TEST == TRUE)
#include "utest.h"
#endif

/*-------------------------------------------------------------------------
 * FUNCTION NAME : BACKDOOR_MGR_InitiateSystemResources
 *-------------------------------------------------------------------------
 * PURPOSE:
 *      Initiate system resource for BACKDOOR_MGR
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      private om API
 *-------------------------------------------------------------------------
 */
void BACKDOOR_MGR_InitiateSystemResources(void)
{
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME : BACKDOOR_MGR_AttachSystemResources
 *-------------------------------------------------------------------------
 * PURPOSE:
 *      Attach system resource for BACKDOOR_MGR in the context of the
 *      calling process.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      private om API
 *-------------------------------------------------------------------------
 */
void BACKDOOR_MGR_AttachSystemResources(void)
{
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME : BACKDOOR_MGR_GetShMemInfo
 *-------------------------------------------------------------------------
 * PURPOSE:
 *      Provide shared memory information of BACKDOOR_MGR for SYSRSC.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      segid_p  --  shared memory segment id
 *      seglen_p --  length of the shared memroy segment
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      private om API
 *-------------------------------------------------------------------------
 */
void BACKDOOR_MGR_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p)
{
    *segid_p = SYSRSC_MGR_BACKDOOR_MGR_SHMEM_SEGID;
    *seglen_p = 100;
}

/*-----------------------------------------------------------------
 * ROUTINE NAME - BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack
 *-----------------------------------------------------------------
 * FUNCTION: To register which function wish to appear in backdoor
 *           of CLI.
 * INPUT   : csc_name   - The name of the csc (computer software compoment)
 *                        module that wish to hook to backdoor, and this
 *                        name will be appeared in the main menu of the
 *                        backdoor. The string length of the name should
 *                        <= BACKDOOR_MGR_MAX_CSC_NAME_STRING_LENTH
 *                        and >= BACKDOOR_MGR_MIN_CSC_NAME_STRING_LENTH.
 *                        If the length > BACKDOOR_MGR_MAX_CSC_NAME_STRING_LENTH
 *                        The name will be truncated.
 *                        If the length < BACKDOOR_MGR_MIN_CSC_NAME_STRING_LENTH
 *                        registering will fail.
 *                        The name should not be BACKDOOR_MGR_RESERVED_WORD because
 *                        this is reserved word and registering will fail.
 *           msgq_key   - The message queue that waits for backdoor message.
 *           func       - The functional pointer of the function
 *                        that wish to register.
 *
 * OUTPUT  : None.
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------------*/
void BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack(char *csc_name,
                                                       UI32_T msgq_key,
                                                       void (*func)(void))
{
#if (RUN_UNIT_TEST == TRUE)
    UT_ShellItem    si = {0};

    strcpy(si.name, csc_name);
    si.func = func;

    if (UT_RegisterShellItem(&si))
    {
        BACKDOOR_MGR_Printf("%s for %s failed\n", __FUNCTION__, csc_name);
    }
#endif /* RUN_UNIT_TEST */
}

/*-----------------------------------------------------------------
 * ROUTINE NAME - BACKDOOR_MGR_RequestKeyIn
 *-----------------------------------------------------------------
 * FUNCTION: To get the string until enter is pressed from UI.
 * INPUT   : key_in_string - The pointer to the buffer to store
 *                           the key in string.
 *                           The size of buffer must reserve additional
 *                           space for '\0' (null terminate).
 *           max_key_len   - Maxium character user can key in.
 * OUTPUT  : key_in_string - Got string.
 * RETURN  : TRUE  - Successful.
 *           FALSE - Failed. (due to invalid parameters/out of memory/IPC operation failure)
 * NOTE    : this function is only allowed to be used in backdoor threads.
 *----------------------------------------------------------------
 */
BOOL_T BACKDOOR_MGR_RequestKeyIn(void *key_in_string, UI32_T max_key_len)
{
#if __WIN32__
    return BACKDOOR_MGR_RequestKeyIn_Win32(key_in_string, max_key_len);
#elif __OSX__
    return BACKDOOR_MGR_RequestKeyIn_OSX(key_in_string, max_key_len);
#else
    return FALSE;
#endif
}

/*-----------------------------------------------------------------
 * ROUTINE NAME - BACKDOOR_MGR_Printf
 *-----------------------------------------------------------------
 * FUNCTION: To print a formatted string.
 * INPUT   : fmt_p - The format string.
 * OUTPUT  : None.
 * RETURN  : TRUE  - Successful.
 *           FALSE - Failed. (due to invalid parameters/out of memory/IPC operation failure)
 * NOTE    : it may spend at most BACKDOOR_MGR_PRINTF_DFLT_WAIT_TIME
 *           on IPC.
 *----------------------------------------------------------------*/
BOOL_T BACKDOOR_MGR_Printf(char *fmt, ...)
{
    va_list argp;
    BOOL_T ret;

    va_start(argp, fmt);

#if __WIN32__
    ret = BACKDOOR_MGR_Printf_Win32(fmt, argp);
#elif __OSX__
    ret = BACKDOOR_MGR_Printf_OSX(fmt, argp);
#else
    ret = FALSE;
#endif

    va_end(argp);

    return ret;
}

/* LOCAL SUBPROGRAM SPECIFICATIONS
 */
/*-----------------------------------------------------------
 * ROUTINE NAME - BACKDOOR_MGR_InitateProcessResource
 *-----------------------------------------------------------
 * FUNCTION: To initiate resource used in this process.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : TRUE   - Successful
 *           FALSE  - Failed
 * NOTE    : None.
 *----------------------------------------------------------*/
//static BOOL_T BACKDOOR_MGR_InitateProcessResource(void)
//{
//    return TRUE;
//}
