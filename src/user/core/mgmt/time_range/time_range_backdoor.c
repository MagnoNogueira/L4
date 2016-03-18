/*-----------------------------------------------------------------------------
 * FILE NAME: time_range_backdoor.c
 *-----------------------------------------------------------------------------
 * PURPOSE:
 *    Implementations for the Time_Range backdoor
 *
 * NOTES:
 *    None.
 *
 * HISTORY:
 *    2010/03/08 - Nelson Dai, created for Time_Range backdoor.
 *
 * Copyright(C)      Accton Corporation, 2010
 *-----------------------------------------------------------------------------
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <string.h>
#include "time_range_mgr.h"
#include "backdoor_mgr.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define MAXLINE 255

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static void TIME_RANGE_BACKDOOR_ShowMenu();
static void TIME_RANGE_BACKDOOR_ShowInfo();

/* STATIC VARIABLE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM BODIES
 */

/*--------------------------------------------------------------------------
 * FUNCTION NAME - TIME_RANGE_BACKDOOR_Main
 *--------------------------------------------------------------------------
 * PURPOSE  : This is backdoor main function.
 * INPUT    : None
 * OUTPUT   : None
 * RETUEN   : None
 * NOTES    : Registered in TIME_RANGE_MGR_Create_InterCSC_Relation().
 *--------------------------------------------------------------------------*/
void TIME_RANGE_BACKDOOR_Main(void)
{
    //char cmd_buf[MAXLINE+1] = {0};
    char c;

    //uart_handler = SYSFUN_OpenUART(SYSFUN_UART_CHANNEL1);

    TIME_RANGE_BACKDOOR_ShowMenu();

    while (1)
    {
        /* Due to the defect of BACKDOOR design in Simba platform,
         * we cannot use BACKDOOR_PMGR_Xxx() (because we are in the same group).
         * Workaround: use printf() and getchar().
         *
         * This piece of code should be modified after BACKDOOR is enhanced.
         * Ask Wakka_Tu for more information.
         */
        //BACKDOOR_PMGR_Printf("\r\n\r\nEnter Selection (0 for menu page): ");
        //BACKDOOR_PMGR_RequestKeyIn(cmd_buf, MAXLINE);
        printf("\r\n\r\nEnter Selection (0 for menu page): ");
        c = getchar();

        //ch = atoi(cmd_buf);
        switch (c)
        {
            case '1':
                TIME_RANGE_BACKDOOR_ShowInfo();
                break;

            case '9':
                return;

            case '0':
            default:
                TIME_RANGE_BACKDOOR_ShowMenu();
                break;
        }
    }
}

static void TIME_RANGE_BACKDOOR_ShowMenu()
{
    printf("\n========= Time Range Backdoor =========\n");
    printf("    0: Show Menu\n");
    printf("    1: Show Time Range Entry Info\n");
    printf("    9: Quit\n");
}

static void TIME_RANGE_BACKDOOR_ShowInfo()
{
    UI32_T p_index;
    UI32_T  index = TIME_RANGE_TYPE_UNDEF_TIME_RANGE;
    TIME_RANGE_TYPE_ENTRY_T entry;

    char month[13][5] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    char day[10][10] = {"Sun.", "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat.",
                               "Daily", "Weekdays", "Weekend"};

    memset(&entry, 0, sizeof(TIME_RANGE_TYPE_ENTRY_T));

    while (TIME_RANGE_ERROR_TYPE_NONE == TIME_RANGE_MGR_GetNextTimeRangeEntry(&index, &entry))
    {
        printf("\r\n--------------------------------------------------");
        printf("\r\n time range name(index=%lu) (active=%u) (ref_count=%lu): %s",
            index, (entry.active == TRUE ? 1 : 0), entry.ref_counter, entry.name);

        if (TRUE == entry.check_start_absolute)
            printf("\r\n     start absolute : %4u.%s.%02u %2u:%02u ",
                entry.start_time.year, month[entry.start_time.month],
                entry.start_time.day, entry.start_time.hour,
                entry.start_time.minute);

        if (TRUE == entry.check_end_absolute)
            printf("\r\n     end absolute   : %4u.%s.%02u %2u:%02u",
                entry.end_time.year, month[entry.end_time.month],
                entry.end_time.day, entry.end_time.hour,
                entry.end_time.minute);

        for (p_index = 0; p_index < SYS_ADPT_TIME_RANGE_MAX_NBR_OF_PERIODIC_ENTRY; ++p_index)
        {
            if (TRUE == entry.check_periodic[p_index])
            {
                printf("\r\n     periodic[%lu] : %8s %2u:%02u -- %8s %2u:%02u", p_index,
                    day[entry.periodic_daily_time[p_index].start_type_of_wk],
                        entry.periodic_daily_time[p_index].start_hour,
                        entry.periodic_daily_time[p_index].start_minute,
                    day[entry.periodic_daily_time[p_index].end_type_of_wk],
                        entry.periodic_daily_time[p_index].end_hour,
                        entry.periodic_daily_time[p_index].end_minute);
            }
        }
    }

    printf("\r\n---------------------- DONE ----------------------");
}
