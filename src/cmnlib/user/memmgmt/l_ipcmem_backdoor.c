/* MODULE NAME:  L_IPCMEM_BACKDOOR.c
 * PURPOSE:
 *     User space front end executable binary for L_Mm_Backdoor
 *
 * NOTES:
 *     BACKDOOR_OPEN in kernel L_IPCMEM.c must be defined as TRUE when using this
 *     module in user space.
 *     Value of _IN_TARGET, _TMZ must be consistent with definied values in
 *     kernel space L_IPCMEM.
 *
 * HISTORY
 *    2006/11/1 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2006
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>

/* accton include files */

#include "sys_type.h"
#include "l_ipcmem_type.h"
#include "backdoor_mgr.h"

/* NAMING CONSTANT DECLARATIONS
 */
/*  <<< Define some directory for special purpose >>>
 *  Following definition must be consistent with those in L_Mem kernel
 *  _IN_TARGET    : active tracking counter of each memory pool, this mechanism waste
 *                  some calculation time.
 *                  1 : active, 0 : inactive.
 *  _TMZ : the space to separate over range.
 */

#define _IN_TARGET                      1
#define _TMZ                            1

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
#if(_IN_TARGET==1)
static void L_IPCMEM_Backdoor_DisplayLMEMCounter(void);
#else
#define L_IPCMEM_Backdoor_DisplayLMEMCounter() BACKDOOR_MGR_Printf("\r\nNot support")
#endif

#if(_TMZ==1)
static void L_IPCMEM_Backdoor_VerifyTmz(void);
#else
#define L_IPCMEM_Backdoor_VerifyTmz() BACKDOOR_MGR_Printf("\r\nNot support")
#endif

/* STATIC VARIABLE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM BODIES
 */

/* LOCAL SUBPROGRAM BODIES
 */

#if(_TMZ==1)
/* FUNCTION NAME : L_IPCMEM_Backdoor_VerifyTmz
 * PURPOSE:
 *      Detect any space not same as MEM_TMZ_PATTERN
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
 *
 */
static void L_IPCMEM_Backdoor_VerifyTmz(void)
{
    /*  LOCAL VARIABLE DECLARATION
     */
    int     i, count;
    char    tmz[MEM_TMZ_SIZE];

    /*  BODY
     */
    if(FALSE==(BOOL_T)SYSFUN_Syscall(SYSFUN_SYSCALL_L_IPCMEM, L_IPCMEM_CMD_BACKDOOR, L_IPCMEM_BACKDOOR_CMD_GETTMZINFO, TRUE, (UI32_T)tmz, NULL))
    {
        BACKDOOR_MGR_Printf("\r\nsys_L_IPCMEM fails");
        return;
    }

    BACKDOOR_MGR_Printf ("\r\n L_IPCMEM Front :\r\n");
    count = 0;
    for (i=0; i<MEM_TMZ_SIZE; i++)
    {
        if (MEM_TMZ_PATTERN != tmz[i])
        {
            BACKDOOR_MGR_Printf (" %4d-%2x", i, tmz[i]);
            count++;
            if ((count&0x00000007)==0)
                BACKDOOR_MGR_Printf (" ");
            if (count>=16)
            {
                BACKDOOR_MGR_Printf("\r\n");
                count=0;
            }
        }
    }

    if(FALSE==(BOOL_T)SYSFUN_Syscall(SYSFUN_SYSCALL_L_IPCMEM, L_IPCMEM_CMD_BACKDOOR, L_IPCMEM_BACKDOOR_CMD_GETTMZINFO, FALSE, (UI32_T)tmz, NULL))
    {
        BACKDOOR_MGR_Printf("\r\nsys_L_IPCMEM fails");
        return;
    }

    BACKDOOR_MGR_Printf ("\r\n L_IPCMEM Rear :\r\n");
    count = 0;
    for (i=0; i<MEM_TMZ_SIZE; i++)
    {
        if (MEM_TMZ_PATTERN != tmz[i])
        {
            BACKDOOR_MGR_Printf (" %4d-%2x", i, tmz[i]);
            count++;
            if ((count&0x00000007)==0)
                BACKDOOR_MGR_Printf (" ");
            if (count>=16)
            {
                BACKDOOR_MGR_Printf("\r\n");
                count=0;
            }
        }
    }

}   /*  end of L_IPCMEM_Backdoor_VerifyTmz  */
#endif

#if(_IN_TARGET==1)
static void L_IPCMEM_Backdoor_DisplayLMEMCounter(void)
{
    L_Mem_BackdoorInfoEntry_T backdoor_info[L_IPCMEM_NUMBER_OF_BUF_TYPE];
    UI32_T buf_type, allocate_fail_counter;

    if(FALSE == (BOOL_T)(SYSFUN_Syscall(SYSFUN_SYSCALL_L_IPCMEM, L_IPCMEM_CMD_BACKDOOR, L_IPCMEM_BACKDOOR_CMD_GETBACKDOORINFO, (UI32_T)backdoor_info, (UI32_T)&allocate_fail_counter, NULL)))
    {
        BACKDOOR_MGR_Printf("\r\nsys_L_IPCMEM fails");
        return;
    }

    BACKDOOR_MGR_Printf ("\r\n L_IPCMEM (size/all ;  alloc /  free  ;used/peak/fail)\r\n");

    for (buf_type=0; buf_type<L_IPCMEM_NUMBER_OF_BUF_TYPE; buf_type++)
    {
        BACKDOOR_MGR_Printf ("       (%4d/%4d;%8lu/%8lu;%4ld/%4ld/%4ld)\r\n",
                    backdoor_info[buf_type].partition_len,
                    backdoor_info[buf_type].num_of_block,
                    backdoor_info[buf_type].alloc_counter,
                    backdoor_info[buf_type].free_counter,
                    backdoor_info[buf_type].used_count,
                    backdoor_info[buf_type].peak_count,
                    backdoor_info[buf_type].fail_count);
    }
    BACKDOOR_MGR_Printf("Allocate fail counter=%lu\r\n", allocate_fail_counter);
}   /*  end of L_IPCMEM_Backdoor_DisplayLMEMCounter */
#endif

/* change main() to L_IPCMEM_BackDoor_Menu() when done
 */
int L_IPCMEM_BACKDOOR_BackDoorMenu(void)
{
    int ch;
    BOOL_T  eof=FALSE;
    /*  BODY
     */

    while (!eof)
    {
        BACKDOOR_MGR_Printf("\r\n 0. Exit\r\n");
#if(_IN_TARGET==1)
        BACKDOOR_MGR_Printf(" 1. Display counters of each size memory pool.\r\n");
#endif
        BACKDOOR_MGR_Printf(" 2. Check TMZ corrupt.\r\n");
        BACKDOOR_MGR_Printf("     select =");
        ch = BACKDOOR_MGR_GetChar();
        if ((ch < 0x30)||(ch>0x39))
            continue;
        ch -= 0x30;
        BACKDOOR_MGR_Printf ("%d",ch);
        switch (ch)
        {
            case 0 :
                eof = TRUE;
                break;
            case 1 :
                L_IPCMEM_Backdoor_DisplayLMEMCounter();
                break;
            case 2 :
                L_IPCMEM_Backdoor_VerifyTmz();
                break;
            default :
                ch = 0;
                break;
        }
    }   /*  end of while    */

    return 0;
}   /*  L_IPCMEM_BackDoor_Menu */

