/* MODULE NAME:  SYSFUN_BACKDOOR.c
 * PURPOSE:
 *     Backdoor functions of SYSFUN.
 *
 * NOTES:
 *     None.
 *
 * HISTORY
 *    2009/11/23 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2009
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

/* accton include files */

#include "sys_type.h"
#include "backdoor_mgr.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define UPLOAD_EXCEPTION_LOG_SCRIPT_FILE "/etc/upload_exception_log.sh"
#define EXCEPTION_LOG_DIRNAME "/flash/exception_log"
#define EXCEPTION_LOG_FILENAME_PREFIX "exception_log"

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */

/* STATIC VARIABLE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM BODIES
 */

/* FUNCTION NAME : SYSFUN_BACKDOOR_BackDoorMenu
 * PURPOSE:
 *      Backdoor menu of sysfun backdoor.
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
 *      None.
 */

void SYSFUN_BACKDOOR_BackDoorMenu(void)
{
    int ch;
    BOOL_T  eof=FALSE;
    /*  BODY
     */

    while (!eof)
    {
        BACKDOOR_MGR_Printf("\r\n 0. Exit\r\n");
        BACKDOOR_MGR_Printf(" 1. Upload exception log file by TFTP.\r\n");
        BACKDOOR_MGR_Printf("     select =");
        ch = BACKDOOR_MGR_GetChar();
        if ((ch < 0x30)||(ch>0x39))
            continue;
        ch -= 0x30;
        BACKDOOR_MGR_Printf("%d\r\n",ch);
        switch (ch)
        {
            case 0 :
                eof = TRUE;
                break;
            case 1 :
            {
                struct stat file_stat;
                int    rc;
                char   ip_buffer[28];
                char   sys_buffer[64];
                DIR   *dir;
                struct dirent *dirent_p;
                BOOL_T is_found;

                rc = stat(UPLOAD_EXCEPTION_LOG_SCRIPT_FILE, &file_stat);
                if(rc!=0)
                {
                    BACKDOOR_MGR_Print("Do not support yet.\r\n");
                    break;
                }

                /* check whether exception log exists
                 */
                is_found=FALSE;
                dir = opendir(EXCEPTION_LOG_DIRNAME);
                if(dir==NULL)
                {
                    goto DIRECTORY_NOT_EXIST;
                }

                is_found=FALSE;
                while((dirent_p = readdir(dir))!=NULL)
                {
                    if(!strncmp(dirent_p->d_name, EXCEPTION_LOG_FILENAME_PREFIX, sizeof(EXCEPTION_LOG_FILENAME_PREFIX)-1))
                    {
                        is_found=TRUE;
                        break;
                    }
                }

                closedir(dir);

DIRECTORY_NOT_EXIST:
                /* do upload exception log files if they exist
                 */
                if(is_found==TRUE)
                {
                    BACKDOOR_MGR_Print("Please ensure that IP of this device had been configured properly\r\n");
                    BACKDOOR_MGR_Print("TFTP server ip: ");
                    BACKDOOR_MGR_RequestKeyIn(ip_buffer, sizeof(ip_buffer)-1);
                    BACKDOOR_MGR_Print("\r\n");
                    sprintf(sys_buffer, "%s %s", UPLOAD_EXCEPTION_LOG_SCRIPT_FILE, ip_buffer);
                    system(sys_buffer);
                    BACKDOOR_MGR_Print("Done.\r\n");
                }
                else
                    BACKDOOR_MGR_Print("No exception log on the device\r\n");

            }
                break;
            default :
                ch = 0;
                break;
        }
    }   /*  end of while    */

    return;
}   /*  SYSFUN_BackDoor_Menu */

/* LOCAL SUBPROGRAM BODIES
 */
