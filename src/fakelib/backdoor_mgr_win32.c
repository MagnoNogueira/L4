#include <windows.h>
#include <strsafe.h>

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
BOOL_T BACKDOOR_MGR_RequestKeyIn_Win32(void *key_in_string, UI32_T max_key_len)
{
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    TCHAR *pszBuff;
    DWORD cchRead;
    BOOL bResult;

    pszBuff = (LPVOID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, max_key_len * sizeof(TCHAR));
    if (NULL == pszBuff)
    {
        return FALSE;
    }

    if (max_key_len <= 1)
    {
        return FALSE;
    }

    bResult = ReadConsole(hStdIn, (LPVOID)pszBuff, max_key_len-1, &cchRead, NULL);
    if (FALSE == bResult)
    {
        HeapFree(GetProcessHeap(), 0, pszBuff);
        return FALSE;
    }

    ZeroMemory(key_in_string, max_key_len*sizeof(CHAR));

    if (2 <= cchRead)
    {
#ifdef _UNICODE
        wcstombs(key_in_string, pszBuff, cchRead);
#else
        StringCchCopy(key_in_string, max_key_len, pszBuff);
#endif

        ((char*)key_in_string)[cchRead] = '\0';
    }

    HeapFree(GetProcessHeap(), 0, pszBuff);
    return TRUE;
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
BOOL_T BACKDOOR_MGR_Printf_Win32(char *fmt, va_list argp)
{
    enum {OUTPUT_MAX_CCH = 1*1024};
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CHAR *pszBuff;
    size_t cchBuff;
    DWORD cchWritten;

    pszBuff = (LPVOID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, OUTPUT_MAX_CCH * sizeof(CHAR));
    if (NULL == pszBuff)
    {
        return FALSE;
    }

    StringCchVPrintfA(pszBuff, OUTPUT_MAX_CCH, fmt, argp);

    StringCchLengthA(pszBuff, OUTPUT_MAX_CCH, &cchBuff);

    WriteConsoleA(hStdOut, pszBuff, cchBuff, &cchWritten, NULL);

    HeapFree(GetProcessHeap(), 0, pszBuff);
    return TRUE;
}

