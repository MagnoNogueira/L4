#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "sysfun.h"
#include "sys_type.h"
#include "sys_cpnt.h"

#include "cli_type.h"
#include "cli_def.h"
#include "cli_task.h"
#include "cli_pars.h"
#include "cli_api.h"
#include "cli_main.h"
#include "cli_lib.h"
#include "cli_msg.h"
#include "cli_io.h"
#if (SYS_CPNT_STP == SYS_CPNT_STP_TYPE_MSTP)
//#include "xstp_pmgr.h"
//#include "xstp_type.h"
#endif
//#include "sys_mgr.h"
#include "l_ipv4v6.h" /* common lib for IPv4/v6 address syntax checking */

#if (SYS_CPNT_MOTD == TRUE)
#include "sys_bnr_pmgr.h"
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
#include "cli_api_vlan.h"
#endif
#include "l_ipcio.h"
#if (SYS_CPNT_MOTD == TRUE)
/*--------------------- extern ---------------------*/
/* cli_main.c */
extern BOOL_T operate_sys_banner_msg_status;
extern UI8_T  operate_sys_banner_msg_by_session_id;
extern UI8_T  operate_sys_banner_msg_delimiting_character;
extern UI8_T  cli_sys_bnr_msg[SYS_ADPT_MAX_MOTD_LENGTH + 1];
extern UI32_T cli_sys_bnr_msg_position;
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/
//#include "cli_api_tunnel.h"

extern BOOL_T L_IPV4V6_Is_Valid_IPv6_PrefixString(char *str, UI32_T str_len);
extern BOOL_T L_IPV4V6_Is_Valid_IPv6_AddressString(char *str, UI32_T str_len);

/************************************************************************/
/* CLI_PARS_ParseArgumentDispatch                                       */
/*                                                                      */
/* Description:                                                         */
/*     Parse an argument                                                */
/*                                                                      */
/* Arguments:                                                           */
/*     token     : Argument. The token may not a null-terminal string   */
/*     token_len : String of the argument                               */
/*     arg_list  : Agurment array list                                  */
/*     arg_idx   : Current argument index                               */
/*                                                                      */
/* Output:                                                              */
/*     match_len : How many character of token be read                  */
/*     name      : Help message for 1st column                          */
/*     help      : Help message for 2nd column                          */
/* Return:                                                              */
/*     ARG_MATCH             => when exactly match                      */
/*     ARG_SUBMATCH          => sub match                               */
/*     ARG_NOMATCH           => no match                                */
/************************************************************************/
static int
CLI_PARS_ParseArgumentDispatch(
    const char *token,
    UI32_T token_len,
    const ARGLIST arg_list[],
    UI16_T  arg_idx,
    UI16_T *match_len,
    UI16_T *err_kind,
    char *name,
    UI32_T name_len,
    char *help,
    UI32_T help_len
);

typedef int (*CLI_PARS_ArgParseFunc)(
    const char *token,
    UI16_T token_len,
    const ARGLIST arg_list[],
    UI16_T  arg_idx,
    UI16_T *match_len,
    UI16_T *err_kind,
    char *name,
    UI32_T name_len,
    char *help,
    UI32_T help_len
);

static int
CLI_BGP_COMMUNITY_DATA_Parse(
    const char *token,
    UI16_T token_len,
    const ARGLIST arg_list[],
    UI16_T  arg_idx,
    UI16_T *match_len,
    UI16_T *err_kind,
    char *name,
    UI32_T name_len,
    char *help,
    UI32_T help_len
);

typedef struct
{
    CLI_PARS_ArgParseFunc    parse;
} CLI_PARS_Handler;

static CLI_PARS_Handler      cli_pars_arg_handler[MAX_DATA_TYPE];
extern I8_T                  *ui_HelpMsg[];

/**************************************************************************/
/* Process ui_KeyState variables                                          */
/*         ui_KeyState:   0 => Normal state                               */
/*                        1 => the last pressed key is Ctrl_V             */
/*                        2 => the last pressed key is Ctrl_Y             */
/**************************************************************************/
UI8_T  CLI_PARS_GetKeyState(CLI_TASK_WorkingArea_T *ctrl_P)
{
    assert(ctrl_P != NULL);
    return (ctrl_P->CMenu.KeyState);
}


void CLI_PARS_ResetKeyState(CLI_TASK_WorkingArea_T *ctrl_P)
{
    assert(ctrl_P != NULL);
    ctrl_P->CMenu.KeyState = 0;
}


void CLI_PARS_SetKeyState(UI8_T status, CLI_TASK_WorkingArea_T *ctrl_P)
{
    assert(ctrl_P != NULL);
    ctrl_P->CMenu.KeyState = status;
}


UI16_T CLI_PARS_TransLowUpper(char *buf, I16_T si, UI16_T key_type)
{
    UI16_T cnt;
    UI16_T buf_cnt;
    char  ch;

    buf_cnt = strlen((char *)buf);
    cnt = 0;
    ch = *(buf+si);
    while ( ((ch == ' ') || (ch == '-')
          || (ch == 0x09)) && (si+cnt < buf_cnt) )
    {
        cnt++;
        ch = *(buf+si+cnt);
    }

    if (key_type == 1)   /* ESC_C */
    {
        /* change the first char to upper case */
        if (si+cnt < buf_cnt)
        {
            if ((ch >= 'a') && (ch <= 'z'))
            {
                ch = *(buf+si+cnt) - 0x20;
                *(buf+si+cnt) = ch;
            }
            cnt++;
            if ((ch >= 0x01) && (ch <= 0x1B))
                cnt++;
            ch = *(buf+si+cnt);
        }
    }

    while ((ch != ' ') && (ch != '-') && (ch != 0x09) && (si+cnt < buf_cnt))
    {
        if ((key_type == 1) || (key_type == 2))  /* ESC_C || ESC_L */
        {
            /* Change the char to lowercase */
            if ((ch >= 'A') && (ch <= 'Z'))
            {
                ch = *(buf+si+cnt) + 0x20;
                *(buf+si+cnt) = ch;
            }
        }
        else if (key_type == 3)         /* ESC_U */
        {
            /* change the char of this word to upper case */
            if ((ch >= 'a') && (ch <= 'z'))
            {
                ch = *(buf+si+cnt) - 0x20;
                *(buf+si+cnt) = ch;
            }
        }
        cnt++;
        if ((ch >= 0x01) && (ch <= 0x1B))
            cnt++;
        ch = *(buf+si+cnt);

    }  /* while */

    return cnt;
}


UI16_T CLI_PARS_GetKeyForwardSize(char *buf, I16_T si)
{
    UI16_T cnt;
    char   ch;

    cnt = 1;
    ch = *(buf+si);
    if ((ch >= 0x01) && (ch <= 0x1B))
    {
        if (ch == 0x09)
            cnt = 8;
        else
            cnt++;
    }

    return cnt;
}


/* for some special key from Ctrl_V function */

UI16_T CLI_PARS_GetKeyBackwardSize(char *buf, I16_T si)
{
    UI16_T  cnt;
    char    ch;

    cnt = 1;

    if (*(buf+si-1) == 0x09) /* TAB key */
        cnt = 8;
    else if( si-2 >= 0 ) /* dralee 98.9.7 */
    {
        ch = *(buf+si-2);
        if ((ch >= 0x01) && (ch <= 0x1B) && (ch != 0x09))
            cnt++;
    }

    return cnt;
}

static char  cli_pars_ipcio_msg_buf[SYSFUN_SIZE_OF_MSG(sizeof(L_IPCIO_Msg_T))];


/* PURPOSE: Get a input line from: (1) The user (2) Configuration file
 *          (3) Authentication verification, and leave it in the specified
 *          buffer. ENTER, TAB and QUMARK can terminate the input session.
 * INPUT:   buff  : Input buffer
 *          bufsize: Input buffer size in bytes
 *          Is_CtrlKey_Skipped: TRUE -- Skip control keys
 *                              e.g. TAB, Question Mark, CTRL-G...
 *          Is_Hidden: TRUE -- The input characters are invisible
 *                     e.g. Password
 * OUTPUT:  buff  : Output buffer
 * RETURN:  Type of End-line, 0->ENTER, 1->TAB, 2->QUMARK 3->ERROR
 * NOTE:    1. Print the content of the *buff, if it doesn't have NULL string
 *          2. Change si and buf_cnt number, if *buff doesn't have NULL string
 *          3. Don't let the buffer overflow.
 */
UI16_T CLI_PARS_ReadLine(char *buff, I16_T bufsize, BOOL_T Is_CtrlKey_Skipped, BOOL_T Is_Hidden)
{
    static UI16_T ipcio_count=0;
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();
    char    *edit_buf; /* point to current edit buffer */
    //char    tmpbuf[CLI_DEF_MAX_BUFSIZE+1]; /* for history command editing  */
    char    tmpchar;
    UI16_T  c;
    char   ch;
    UI16_T  exit_flag, ret_val;
    I16_T   si, shift_num, buf_cnt;
    UI16_T  disp_width;
    char   *old_buf;

    char   *hist_buf;
    UI16_T  histIdx;
    UI16_T  histUpFlag; /* if it has touch history top or bottom */
    UI16_T  histDnFlag;

    char   *del_buf;
    UI16_T  delIdx;
    UI16_T  delFlag; /* for (Ctrl_Y, ESC_Y) pair operation */
    UI16_T  used_len, prev_len = 0;
    I16_T   diff_len;
    I16_T i, j;
    UI16_T  eidx, sidx;
    UI16_T  cnt, snum;
    UI8_T   arrow_type;
    UI32_T wait_key;
#if (SYS_CPNT_MOTD == TRUE)
    char ch_str[2] = {0};

    cli_sys_bnr_msg_position = 0;
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

    if( ctrl_P == NULL )
    {
        SYSFUN_Debug_Printf("\r\n%sFailed to get working area\r\n", __FUNCTION__);
        /*jingyan zheng  ES3628BT-FLF-ZZ-00231*/
        return 3;
    }

   // tmpbuf[0] = '\0';
    disp_width = CLI_LIB_GetTermDispWidth(ctrl_P);

    //printf("\n disp_width is %lu", disp_width);
    /* Initialize si, buf_cnt, and *edit_buf */

    shift_num = 0;
    si = strlen((char *)buff);                  /* cursor index */
    buf_cnt = strlen((char *)buff);             /* buffer count */

    //printf("buff is %s, si is %d\n", buff, si);
    if ( buf_cnt != 0 && Is_CtrlKey_Skipped == FALSE)
    {
       cnt = CLI_LIB_RedisplayCmd(buff, buf_cnt, disp_width, 0, ctrl_P);
       shift_num = buf_cnt - cnt;
    }
    edit_buf = buff;
    old_buf = ctrl_P->CMenu.OldEditBuf;    /* for process scroll compare */
    *old_buf = '\0';

    /* Initialize history and delete buffer variables */

    histIdx = CLI_LIB_GetHistTailIdx(ctrl_P);
    histUpFlag = 0;
    histDnFlag = 1;

    CLI_PARS_ResetKeyState(ctrl_P);  /* Set key state to normal state */
    delFlag = 0;               /* not yet process Ctrl-Y key */

    ret_val = 0;
    exit_flag = 0;

    while (!exit_flag)
    {

            arrow_type = 0;

        c = CLI_IO_GetKey(&ch);

        if(c == KEYIN_ERROR) /*error*/
           return 3;

#if (SYS_CPNT_MOTD == TRUE)
        if( (operate_sys_banner_msg_status == TRUE) &&
        (operate_sys_banner_msg_by_session_id == CLI_TASK_GetMySessId()))
        {
            if(ch == operate_sys_banner_msg_delimiting_character) /*users end the input*/
            {
                cli_sys_bnr_msg[cli_sys_bnr_msg_position] = '\0';

                if (SYS_BNR_PMGR_SetSysBnrMsgDelimitingChar(SYS_BNR_MGR_MOTD_TYPE, operate_sys_banner_msg_delimiting_character) != TRUE)
                {
                    CLI_LIB_PrintStr("\r\nFailed to set system banner message.\r\n");
                }
                else if (SYS_BNR_PMGR_SetSysBnrMsg(SYS_BNR_MGR_MOTD_TYPE, cli_sys_bnr_msg) != TRUE)
                {
                    CLI_LIB_PrintStr("\r\nFailed to set system banner message.\r\n");
                }
                else
                {
                    CLI_LIB_PrintStr("\r\n");
                }

                return (4);
            }
            else if (c == BS) /*del*/
            {
                if(cli_sys_bnr_msg_position == 0)
                {

                }
                else if(cli_sys_bnr_msg[cli_sys_bnr_msg_position-2] == '\r' &&
                   cli_sys_bnr_msg[cli_sys_bnr_msg_position-1] == '\n')
                {

                }
                else
                {
                    cli_sys_bnr_msg_position--;
                    cli_sys_bnr_msg[cli_sys_bnr_msg_position] = '\0';
                    CLI_LIB_PrintStr(BACKSPACE);
                }
            }
            else if(cli_sys_bnr_msg_position >= SYS_ADPT_MAX_MOTD_LENGTH) /*exceed range*/
            {
                cli_sys_bnr_msg[cli_sys_bnr_msg_position] = '\0';
                CLI_LIB_PrintStr("\r\nInput string length exceeds 1024. Auto-save!!\r\n");

                if (SYS_BNR_PMGR_SetSysBnrMsgDelimitingChar(SYS_BNR_MGR_MOTD_TYPE, operate_sys_banner_msg_delimiting_character) != TRUE)
                {
                    CLI_LIB_PrintStr("\r\nFailed to set system banner message.\r\n");
                }
                else if (SYS_BNR_PMGR_SetSysBnrMsg(SYS_BNR_MGR_MOTD_TYPE, cli_sys_bnr_msg) != TRUE)
                {
                    CLI_LIB_PrintStr("\r\nFailed to auto-save!!\r\n");
                }

                return (4);
            }
            else if (c == ENTER) /*enter*/
            {
                cli_sys_bnr_msg[cli_sys_bnr_msg_position++] = '\r';
                cli_sys_bnr_msg[cli_sys_bnr_msg_position++] = '\n';
                CLI_LIB_PrintStr("\r\n");

            }
            else if(c == PRNTABLE) /*printable character*/
            {
                cli_sys_bnr_msg[cli_sys_bnr_msg_position++] = ch;
                ch_str[0] = ch;
                CLI_LIB_PrintStr(ch_str);
            }
            else /*non-printable character*/
            {

            }
            continue;
        }

#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

        if (c != ESC_Y)  /* if it is not ESC_Y key, reset Ctrl_Y flag */
            delFlag = 0;

        if (CLI_PARS_GetKeyState(ctrl_P) == UI_KEY_CTRL_V_ON)
        {
            if ((c == UP) || (c == DOWN) || (c == RIGHT) || (c == LEFT))
                arrow_type = c;
            c = PRNTABLE;
        }

        switch (c)
        {
        case (UP):
        case (CTRL_P):
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_IsHistBufEmpty(ctrl_P))   /* buffer empty */
                   break;

               histUpFlag = CLI_LIB_GetHistPrevIdx(&histIdx, ctrl_P);
               histDnFlag = 0;

               if (histUpFlag == 0)
               {
                   hist_buf  = CLI_LIB_GetHistBuf(histIdx, ctrl_P);
                   shift_num = CLI_LIB_ScrollToEnd(edit_buf, hist_buf,
                            si, shift_num, disp_width);
                   si = strlen(hist_buf);
                   buf_cnt = strlen(hist_buf);
                   #if 0
                   strcpy(tmpbuf, hist_buf);
                   edit_buf = tmpbuf;

                   strcpy(buff, edit_buf);
                   #endif
                   strncpy(edit_buf,hist_buf,buf_cnt);
                   edit_buf[buf_cnt] = '\0';
               }
               else                /* touch the history top */
               {
                   CLI_LIB_PrintStr(BEL);
               }
            }
            break;

        case (DOWN):
        case (CTRL_N):
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_IsHistBufEmpty(ctrl_P))   /* buffer empty */
                   break;

               if (histDnFlag == 0)
               {
                   if (CLI_LIB_GetHistNextIdx(&histIdx, ctrl_P) == 0)
                   {
                       hist_buf  = CLI_LIB_GetHistBuf(histIdx, ctrl_P);
                       shift_num = CLI_LIB_ScrollToEnd(edit_buf, hist_buf,
                              si, shift_num, disp_width);

                       si = strlen(hist_buf);
                       buf_cnt = strlen(hist_buf);
                      #if 0
                       strcpy(tmpbuf, hist_buf);
                       edit_buf = tmpbuf;
                      #endif
                       strncpy(edit_buf, hist_buf,buf_cnt);
                       edit_buf[buf_cnt] = '\0';
                   }
                   else
                   {
                       shift_num = CLI_LIB_ScrollToEnd(edit_buf, buff, si,
                                   shift_num, disp_width);

                       si = strlen(buff);
                       buf_cnt = strlen(buff);

                       edit_buf = buff;
                       histDnFlag = 1;
                   }

                   strcpy(buff, edit_buf);

               }
               else            /* touch the history bottom */
               {
                   CLI_LIB_PrintStr(BEL);
               }
            }
            break;

        case (RIGHT):
        case (CTRL_F):
            if ((CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED) && (c == CTRL_F))
                break;

            if ((si+1) <= buf_cnt)
            {
                cnt = CLI_PARS_GetKeyForwardSize(edit_buf, si);
                si += cnt;

                if (si-shift_num >= disp_width)
                {
                    shift_num = CLI_LIB_ScrollToLeft(edit_buf, edit_buf,
                                si-cnt, si, shift_num, disp_width);
                }
                else
                    CLI_LIB_PrintFStr(edit_buf, si-cnt, si);
            }
            break;

        case (LEFT):
        case (CTRL_B):
            if ((CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED) && (c == CTRL_B))
                break;
            if ((si-1) >= 0)
            {
                cnt = CLI_PARS_GetKeyBackwardSize(edit_buf, si);
                si -= cnt;

                if ((si <= shift_num) && (si > 0))
                {
                    shift_num = CLI_LIB_ScrollToRight(edit_buf, edit_buf,
                                si+cnt, si, shift_num, disp_width);
                }
                else
                {
                   if (!Is_Hidden)
                   {
                      for (i=0; i<cnt; i++)
                         CLI_LIB_PrintStr(BACK);
                   }
                }
            }
            break;

        case (DEL):      /* dralee 99.1.18 to match cisco-1900 */
        case (BS):
            /*------------------------------------------------*/
            /* Erases the character to the left of the cursor */
            /*------------------------------------------------*/

            /* Charles: And Don't delete the character shown in terminal */
            CLI_MAIN_GET_WAIT_KEY(&wait_key);
            if(wait_key == 1)
            {
             //do nothing
             break;
             }
            else if(wait_key == 2)
            {
                //store key
                if (ipcio_count >0)
                {
                    char bs_str[] = "\x08\x20\x08";
                    ipcio_count --;
                    printf(bs_str); fflush(stdout);/*perform backspace on screen*/
                 }
              }
            else
            {
                if ( (si-1) >= 0 )
                {
                    cnt = CLI_PARS_GetKeyForwardSize(edit_buf, si);

                    si -= cnt;
                    buf_cnt -= cnt;
                    strcpy((char *)old_buf, (char *)edit_buf);

                    for (j=si; j<buf_cnt; j++)
                        *(edit_buf+j) = *(edit_buf+(j+cnt));
                    *(edit_buf+j) = '\0';

                    if ((si <= shift_num) && (si > 0))
                    {
                        shift_num = CLI_LIB_ScrollToRight(old_buf, edit_buf,
                                    si+cnt, si, shift_num, disp_width);
                    }
                    else
                    {
                        if (!Is_Hidden)
                        {
                           for (i=0; i<cnt; i++)
                              CLI_LIB_PrintStr(BACK);
                        }

                        shift_num = CLI_LIB_ScrollForBS(old_buf, edit_buf, si, si, shift_num,
                                    shift_num, disp_width);
                    }
                }
            }
            break;

        case (CTRL_D):     /* Delete the character at the cursor */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if ((CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED) && (c == CTRL_D))
                   break;
               if (si < buf_cnt)
               {
                   cnt = CLI_PARS_GetKeyForwardSize(edit_buf, si);
                   buf_cnt -= cnt;
                   strcpy((char *)old_buf, (char *)edit_buf);

                   for (j=si; j<buf_cnt; j++)
                       *(edit_buf+j) = *(edit_buf+(j+cnt));
                   *(edit_buf+j) = '\0';

                   CLI_LIB_ScrollAt(old_buf, edit_buf, si, si, shift_num,
                               shift_num, disp_width);
               }
            }
            break;

        case (CTRL_A):  /* Move the cursor to the beginning of the command line */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;

               for (i=shift_num; i<si; i++)
                   CLI_LIB_PrintStr(BACK);

               if (shift_num > 0)     /* need to scroll to head */
                   CLI_LIB_ScrollAt(edit_buf, edit_buf, 0, 0, shift_num, 0,
                                         disp_width);
               si = 0;
               shift_num = 0;
            }
            break;

        case (CTRL_E): /* Move the cursor to the end of the command line */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;

               if (buf_cnt-shift_num < disp_width)
               {
                   CLI_LIB_PrintFStr(edit_buf, si, buf_cnt);
               }
               else    /* need to process scroll to end */
               {
                   shift_num = CLI_LIB_ScrollToEnd(edit_buf, edit_buf, si,
                                              shift_num, disp_width);
               }
               si = buf_cnt;
            }
            break;

        case (CTRL_K):/* Delete all chars from the cursor to the end of cmd line */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;

               eidx = si;
               while ((eidx < buf_cnt) && (eidx-shift_num <= disp_width))
                   eidx++;

               for (i=si; i<eidx; i++)
                   CLI_LIB_PrintStr(SPACE);

               for (i=si; i<eidx; i++)
                   CLI_LIB_PrintStr(BACK);

               CLI_LIB_AddDelBuf(edit_buf, si, buf_cnt, ctrl_P);
               buf_cnt = si;
               *(edit_buf + buf_cnt) = '\0';
            }
            break;

        case (CTRL_U):
        case (CTRL_X): /* Delete all chars from the cursor to the begin of cmd line */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;

               /* Find same character values that need not to be re-print */
               if (shift_num > 0)
                   cnt = 0;
               else
                   cnt = CLI_LIB_StrNCmp(edit_buf, edit_buf, shift_num, si,
                                            si, buf_cnt);

               CLI_LIB_AddDelBuf(edit_buf, 0, si, ctrl_P);
               strcpy((char *)old_buf, (char *)edit_buf);

               /* shift *edit_buf buffer contents */
               for (i=si; i<buf_cnt; i++)
                   *(edit_buf+i-si) = *(edit_buf+i);
               *(edit_buf+i-si) = '\0';

               /* Move cursor back */
               for (i=shift_num+cnt; i<si; i++)
                   CLI_LIB_PrintStr(BACK);

               /* Print out this line within terminal width */
               CLI_LIB_ScrollAt(old_buf, edit_buf, cnt, 0, shift_num, 0,
                                            disp_width);
               buf_cnt -= si;
               si = 0;
               shift_num = 0;
            }
            break;

        case (CTRL_W):   /* Delete the word to the left of the cursor */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;

               if (si == 0)    /* cursor already in the line head */
                   break;

               cnt = 0;        /* the char count that will be deleted */
               tmpchar = *(edit_buf+si-1);
               while ( ((tmpchar==' ') || (tmpchar=='-') || (tmpchar==0x09))
                                                            && (si-cnt > 0) )
               {
                   cnt++;
                   tmpchar = *(edit_buf+(si-1-cnt));
               }

               while ( (tmpchar!=' ') && (tmpchar!='-') && (tmpchar!=0x09)
                                                           && (si-cnt > 0) )
               {
                   cnt++;
                   tmpchar = *(edit_buf+(si-1-cnt));
               }

               strcpy(old_buf, edit_buf);
               CLI_LIB_AddDelBuf(edit_buf, si-cnt, si, ctrl_P);

               /* correct edit_buf, buf_cnt, and si values */
               for (i=si; i<buf_cnt; i++)
                   *(edit_buf+i-cnt) = *(edit_buf+i);
               *(edit_buf+i-cnt) = '\0';

               buf_cnt -= cnt;
               si -= cnt;

               /* Redisplay the line */
               if ((si <= shift_num) && (shift_num > 0))
               {
                   shift_num = CLI_LIB_ScrollToRight(old_buf, edit_buf,
                                 si+cnt, si, shift_num, disp_width);
               }
               else
               {
                   snum = CLI_LIB_StrNCmp(old_buf,edit_buf, si,si+cnt, si,buf_cnt);
                   for (i=snum; i<cnt; i++)
                       CLI_LIB_PrintStr(BACK);

                   CLI_LIB_ScrollAt(old_buf, edit_buf, si+snum, si, shift_num,
                               shift_num, disp_width);
               }
            }
            break;

        case (CTRL_L):
            #if 1//rich for test to enter linux kernel
            SYSFUN_InvokeLinuxShell();
            #else
            if (CLI_LIB_GetEditModeStatus() == EDIT_DISABLED || Is_CtrlKey_Skipped == TRUE )
                break;
            #endif

        case (CTRL_R): /* Redisplay the current command line */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               CLI_LIB_PrintNullStr(1);
               CLI_LIB_ShowPromptMsg(ctrl_P);

               sidx = shift_num;
               if (shift_num > 0)
               {
                   CLI_LIB_PrintStr("$");
                   sidx++;
               }
               eidx = si;
               while ((eidx<buf_cnt) && (eidx-shift_num < disp_width))
                   eidx++;

               if (buf_cnt-shift_num == disp_width+1)
                   eidx++;

               CLI_LIB_PrintFStr(edit_buf, sidx, eidx);
               if (buf_cnt-shift_num > disp_width+1)
               {
                   CLI_LIB_PrintStr("$");
                   eidx++;
               }
               for (i=si; i<eidx; i++)
                   CLI_LIB_PrintStr(BACK);
            }
            break;

        case (CTRL_T): /* Transpose the char to the left of the cursor */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;

               if ((si == 0) || (buf_cnt <= 1))  break;

               strcpy((char *)old_buf, (char *)edit_buf);

               sidx = si;
               if (si == buf_cnt)
               {
                   cnt = CLI_PARS_GetKeyBackwardSize(edit_buf, si);
                   sidx -= cnt;
               }
               else
                   cnt = CLI_PARS_GetKeyForwardSize(edit_buf, si);

               eidx = sidx - CLI_PARS_GetKeyBackwardSize(edit_buf, sidx);

               for (i=0; i<cnt; i++)
                   *(edit_buf+eidx+i) = *(old_buf+sidx+i);

               for (i=eidx; i<sidx; i++)
                   *(edit_buf+cnt+i) = *(old_buf+i);

               if ((sidx <= shift_num+1) && (shift_num > 0))
               {
                   shift_num = CLI_LIB_ScrollToRight(old_buf, edit_buf,
                               si, sidx+cnt, shift_num, disp_width);
               }
               else if (sidx-shift_num >= disp_width-1)
               {
                   shift_num = CLI_LIB_ScrollToLeft(old_buf, edit_buf,
                               si, sidx+cnt, shift_num, disp_width);
               }
               else
               {
                   for (i=eidx; i<si; i++)
                       CLI_LIB_PrintStr(BACK);
                   CLI_LIB_PrintFStr(edit_buf, eidx, sidx+cnt);
               }
               si = sidx + cnt;
            }
            break;

        case (CTRL_V):
        case (ESC_Q):
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;
               CLI_PARS_SetKeyState(UI_KEY_CTRL_V_ON, ctrl_P);
            }
            break;

        case (CTRL_Y): /* Recall the most recent entry in the delete buffer */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;

               if (buf_cnt >= bufsize-1)    /* if exceed the buffer */
               {
                   CLI_LIB_PrintStr(BEL);
                   break;
               }

               if (CLI_LIB_IsDelBufEmpty(ctrl_P))
                   break;

               delFlag = CLI_LIB_GetDelBufFinalIdx(&delIdx, ctrl_P);
               del_buf = CLI_LIB_GetDelBuf(delIdx, ctrl_P);

               used_len =  strlen((char *)del_buf);
               if (buf_cnt+used_len > bufsize-1)
               {
                   used_len = bufsize-1-buf_cnt;
               }

               strcpy((char *)old_buf, (char *)edit_buf);
               for (j=buf_cnt; j>si; j--)  /* move buffer */
               {
                   *(edit_buf+(j-1)+used_len) = *(edit_buf+(j-1));
               }
               for (j=0; j<used_len; j++)
                   *(edit_buf+si+j) = *(del_buf+j);

               prev_len = used_len;
               si += used_len;
               buf_cnt += used_len;
               *(edit_buf+buf_cnt) = '\0';

               if (si-shift_num >= disp_width)
               {
                   shift_num = CLI_LIB_ScrollToLeft(old_buf, edit_buf, si-used_len,
                               si, shift_num, disp_width);
               }
               else
               {
                   CLI_LIB_ScrollAt(old_buf, edit_buf, si-used_len, si, shift_num,
                               shift_num, disp_width);
               }
            }
            break;

        case (TAB):
            //printf("%s %d\n",__FUNCTION__,__LINE__);
            if (Is_CtrlKey_Skipped == FALSE)
            {
                //CLI_LIB_PrintNullStr(1);
                {
                   /* Move cursor back */
                    // char  prompt[25] = {0};
                    char  prompt[SYS_ADPT_MAX_PROMPT_STRING_LEN+1] = {0}; /*wenfang modify for tab */
                    prompt[0]='\0';
#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
                    UI32_T vlan_id;
                    char  vlan_string_temp[25]={0};
#endif
                    SYS_PMGR_GetPromptString((UI8_T *)prompt);
                    //printf(" ret is %lu\n", ret);
#if (defined(STRAWMAN) || defined(STRAWMANHD))
                    char  Name[SYS_ADPT_MAX_SYSTEM_NAME_STR_LEN + 1]     = {0};

                    MIB2_POM_GetSysName(Name);
                    if (strlen(Name) >=16 )
                        memcpy(prompt,Name,sizeof(UI8_T)*16);
                    else
                        strcpy(prompt, (char *)Name);
#else
                    switch(ctrl_P->sess_type)
                    {
                    case CLI_TYPE_UART:

                        if(strlen(prompt)==0)
                        {
                             sprintf(prompt, "Console");
                        }
                        break;

                    case CLI_TYPE_TELNET:
                          if(strlen(prompt)!=0)
                          {
                             sprintf(prompt, "%s-%d", prompt, CLI_TASK_GetMySessId() - CLI_TASK_MIN_TELNET_SESSION_ID);
                          }
                          else
                          {
                             sprintf(prompt, "Vty-%d", CLI_TASK_GetMySessId() - CLI_TASK_MIN_TELNET_SESSION_ID);
                          }
                          break;

                    default:
                          ;
                          break;
                    }
#endif

                    if(ctrl_P->sess_type != CLI_TYPE_PROVISION)
                    {
                        switch(ctrl_P->CMenu.AccMode)
                        {
                        case NORMAL_EXEC_MODE:
                        default:
                            strcat(prompt, NORMAL_EXEC_MODE_PROMPT);
                            break;

                        case PRIVILEGE_EXEC_MODE:
                            strcat(prompt, PRIVILEGE_EXEC_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_GLOBAL_MODE:
                            strcat(prompt, PRIVILEGE_CFG_GLOBAL_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
                            strcat(prompt, PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
                            strcat(prompt, PRIVILEGE_CFG_INTERFACE_ETH_MODE_PROMPT);
                            break;
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
                        case PRIVILEGE_CFG_INTERFACE_ETH0_MODE:   /*wenfang add 20090106*/
                            strcat(prompt, PRIVILEGE_CFG_INTERFACE_ETH0_MODE_PROMPT);
                            break;
#endif
#if (SYS_CPNT_CRAFT_PORT == TRUE)
                        case PRIVILEGE_CFG_INTERFACE_CRAFT_MODE:
                            strcat(prompt, PRIVILEGE_CFG_INTERFACE_CRAFT_MODE_PROMPT);
                            break;
#endif
#if (SYS_CPNT_FILTER_THROOTTLE == TRUE)
                        case PRIVILEGE_CFG_IGMP_PROFILE_MODE:
                            strcat(prompt, PRIVILEGE_CFG_INTERFACE_IGMP_PROFILE_PROMPT);
                            break;
#endif
                        case PRIVILEGE_CFG_INTERFACE_VLAN_MODE:
                            strcat(prompt, PRIVILEGE_CFG_INTERFACE_VLAN_MODE_PROMPT);
                            break;
#if (SYS_CPNT_IP_TUNNEL == TRUE)
                        case PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE:
                            strcat(prompt, PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE_PROMPT);
                            break;
#endif /*(SYS_CPNT_IP_TUNNEL == TRUE)*/
                        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE:
                            strcat(prompt, PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_VLAN_DATABASE_MODE:
#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
                            CLI_API_GET_VLAN_DATABASE_VLANID(&vlan_id);
                            sprintf(vlan_string_temp,"%s%lu)#", PRIVILEGE_CFG_VLAN_DATABASE_MODE_PROMPT,vlan_id);
                            strcat(prompt, vlan_string_temp);
#else
                            strcat(prompt, PRIVILEGE_CFG_VLAN_DATABASE_MODE_PROMPT);
#endif
                            break;

                        case PRIVILEGE_CFG_LINE_MODE:
                            strcat(prompt, PRIVILEGE_CFG_LINE_MODE_PROMPT);
                            break;

                 case PRIVILEGE_CFG_DOMAIN_MODE:
                            strcat(prompt, PRIVILEGE_CFG_DOMAIN_MODE_PROMPT);
                            break;
                        case PRIVILEGE_CFG_MSTP_MODE:
                            strcat(prompt, PRIVILEGE_CFG_MSTP_MODE_PROMPT);
                            break;

                        case DEBUG_MODE:
                            strcat(prompt, DEBUG_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_ROUTER_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ROUTER_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ROUTERDVMRP_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_ROUTEROSPF_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ROUTEROSPF_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ROUTEROSPF_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_DHCPPOOL_MODE:
                            strcat(prompt, PRIVILEGE_CFG_DHCPPOOL_MODE_PROMPT);
                            break;
#if (SYS_CPNT_BGP == TRUE)
                        case PRIVILEGE_CFG_ROUTER_BGP_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ROUTER_BGP_MODE_PROMPT);
                            break;
#endif
#if (SYS_CPNT_NSM_POLICY == TRUE)
                        case PRIVILEGE_CFG_ROUTE_MAP_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ROUTE_MAP_MODE_PROMPT);
                            break;
#endif
#if (SYS_CPNT_ACL == TRUE)
                        case PRIVILEGE_CFG_ACL_MAC_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ACL_MAC_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_ACL_STANDARD_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ACL_STANDARD_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_ACL_EXTENDED_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ACL_EXTENDED_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_ACL_IP_MASK_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ACL_IP_MASK_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_ACL_MAC_MASK_MODE:
                            strcat(prompt, PRIVILEGE_CFG_ACL_MAC_MASK_MODE_PROMPT);
                            break;
#if (SYS_CPNT_DAI == TRUE)
                        case PRIVILEGE_CFG_ACL_ARP_MODE:
                            strcat((char *)prompt, PRIVILEGE_CFG_ACL_ARP_MODE_PROMPT);
                            break;
#endif
#if (SYS_CPNT_ACL_IPV6 == TRUE)
                        case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
                            strcat((char *)prompt, PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE_PROMPT);
                            break;

                        case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
                            strcat((char *)prompt, PRIVILEGE_CFG_ACL_EXTEND_IPV6_MODE_PROMPT);
                            break;
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
                       case PRIVILEGE_CFG_CLASS_MAP_MODE:
                          strcat(prompt, PRIVILEGE_CFG_CLASS_MAP_MODE_PROMPT);
                          break;

                       case PRIVILEGE_CFG_POLICY_MAP_MODE:
                          strcat(prompt, PRIVILEGE_CFG_POLICY_MAP_MODE_PROMPT);
                          break;

                       case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
                          strcat(prompt, PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE_PROMPT);
                          break;
#endif

#if (SYS_CPNT_AAA == TRUE)
                       case PRIVILEGE_CFG_AAA_SG_MODE:
                          strcat(prompt, PRIVILEGE_CFG_AAA_SG_TACACS_PLUS_MODE_PROMPT);
                          break;

                       case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
                          strcat((char *)prompt, PRIVILEGE_CFG_AAA_SG_RADIUS_MODE_PROMPT);
                          break;

#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
                        case PRIVILEGE_CFG_TIME_RANGE_MODE:
                           strcat(prompt, PRIVILEGE_CFG_TIME_RANGE_MODE_PROMPT);
                           break;
#endif

#if (SYS_CPNT_ERPS == TRUE)
                       case PRIVILEGE_CFG_ERPS_MODE:
                          strcat((char *)prompt, PRIVILEGE_CFG_ERPS_MODE_PROMPT);
                          break;
#endif

                        }
                    }
                    else
                    {
                        prompt[0] = 0;
                    }
    //printf("%s %d, si is %d,  prompt is %s, length is %lu\n",__FUNCTION__,__LINE__, si,prompt, strlen(prompt));

    //printf("$$%s", prompt);
    //fflush(stdout);

                    for (i=0; i<si+strlen(prompt); i++)
                        CLI_LIB_PrintStr(BACK);
    //printf("^^^^^\n");
    //fflush(stdout);

                   si = 0;
                   shift_num = 0;
               }
               ret_val = 1;
               exit_flag = 1;
            }

            //edit_buf =  buff; /*maggie liu, ES4827G-FLE-ZZ-00125*/ /*wenfang modify for tab */
            break;

        case (ESC_B): /* Move the cursor back one word */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;

               if (si == 0)    /* cursor already in the line head */
                   break;

               cnt = 0;        /* cursor shift left count */
               /* Skip leading white space */
               tmpchar = *(edit_buf+si-1);
               while ( ((tmpchar == ' ') || (tmpchar == '-')
                         || (tmpchar == 0x09)) && (si-cnt>0) )
               {
                   cnt++;
                   tmpchar = *(edit_buf+(si-1-cnt));
               }

               while ( (tmpchar != ' ') && (tmpchar != '-')
                         && (tmpchar != 0x09) && (si-cnt > 0) )
               {
                   cnt++;
                   tmpchar = *(edit_buf+(si-1-cnt));
               }

               si -= cnt;
               /* Move cursor back */
               if ((si <= shift_num) && (shift_num > 0))
               {
                   shift_num = CLI_LIB_ScrollToRight(edit_buf, edit_buf,
                               si+cnt, si, shift_num, disp_width);
               }
               else
                   for (i=0; i<cnt; i++)
                       CLI_LIB_PrintStr(BACK);
            }
            break;

        case (ESC_C): /* Capitalize the word at the cursor */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;
               if (si == buf_cnt) break;

               strcpy((char *)old_buf, (char *)edit_buf);
               cnt = CLI_PARS_TransLowUpper(edit_buf, si, 1);

               si += cnt;
               /* Print out this word */
               if (si-shift_num >= disp_width)
               {
                   shift_num = CLI_LIB_ScrollToLeft(old_buf, edit_buf,
                               si-cnt, si, shift_num, disp_width);
               }
               else
                   CLI_LIB_PrintFStr(edit_buf, si-cnt, si);
            }
            break;

        case (ESC_D): /* Delete from the cursor to the end of the word */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;
               if (si == buf_cnt)  /* cursor already in the end */
                   break;

               cnt = 0;      /* char count that will be deleted */
               tmpchar = *(edit_buf+si);
               while ( ((tmpchar == ' ') || (tmpchar == '-')
                        || (tmpchar == 0x09)) && (si+cnt<buf_cnt) )
               {
                   cnt++;
                   tmpchar = *(edit_buf+si+cnt);
               }
               while ((tmpchar != ' ') && (tmpchar != '-')
                        && (tmpchar != 0x09) && (si+cnt < buf_cnt) )
               {
                   cnt++;
                   tmpchar = *(edit_buf+si+cnt);
               }
               CLI_LIB_AddDelBuf(edit_buf, si, si+cnt, ctrl_P);
               strcpy((char *)old_buf, (char *)edit_buf);

               /* correct edit_buf, and buf_cnt values */
               for (i=si+cnt; i<buf_cnt; i++)
               *(edit_buf+i-cnt) = *(edit_buf+i);
               *(edit_buf+i-cnt) = '\0';
               buf_cnt -= cnt;

               CLI_LIB_ScrollAt(old_buf, edit_buf, si, si, shift_num,
                           shift_num, disp_width);
            }
            break;

        case (ESC_F):        /* Move the cursor forward one word */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;
               if (si == buf_cnt)    /* cursor already in the end */
                   break;

               cnt = 0;        /* cursor shift right count */
               tmpchar = *(edit_buf+si);
               while ( ((tmpchar == ' ') || (tmpchar == '-')
                        || (tmpchar == 0x09)) && (si+cnt < buf_cnt))
               {
                   cnt++;
                   tmpchar = *(edit_buf+si+cnt);
               }

               while ( (tmpchar != ' ') && (tmpchar != '-')
                        && (tmpchar != 0x09) && (si+cnt < buf_cnt) )
               {
                   cnt++;
                   tmpchar = *(edit_buf+si+cnt);
               }

               si += cnt;
               /* Move cursor forward */
               if (si-shift_num >= disp_width)
               {
                   shift_num = CLI_LIB_ScrollToLeft(edit_buf, edit_buf,
                               si-cnt, si, shift_num, disp_width);
               }
               else
                   CLI_LIB_PrintFStr(edit_buf, si-cnt, si);
            }
            break;

        case (ESC_L):    /* Change the word at the cursor to lowercase */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;
               if (si == buf_cnt) break;

               strcpy((char *)old_buf, (char *)edit_buf);
               cnt = CLI_PARS_TransLowUpper(edit_buf, si, 2);

               si += cnt;
               /* Print out this word */
               if (si-shift_num >= disp_width)
               {
                   shift_num = CLI_LIB_ScrollToLeft(old_buf, edit_buf,
                               si-cnt, si, shift_num, disp_width);
               }
               else
                   CLI_LIB_PrintFStr(edit_buf, si-cnt, si);
            }
            break;

        case (ESC_U): /* Capitalize letters from the cursor to the word end */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;
               if (si == buf_cnt) break;

               strcpy((char *)old_buf, (char *)edit_buf);
               cnt = CLI_PARS_TransLowUpper(edit_buf, si, 3);

               si += cnt;
               /* Print out this word */
               if (si-shift_num >= disp_width)
               {
                   shift_num = CLI_LIB_ScrollToLeft(old_buf, edit_buf,
                               si-cnt, si, shift_num, disp_width);
               }
               else
                   CLI_LIB_PrintFStr(edit_buf, si-cnt, si);
            }
            break;

        case (ESC_Y):/* Recall the next buffer entry in delete buffer */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (CLI_LIB_GetEditModeStatus(ctrl_P) == EDIT_DISABLED)
                   break;
               if (delFlag == 0) /* not yet press Ctrl-Y or only one del buffer */
                   break;

               if (buf_cnt >= bufsize-1)    /* if exceed the buffer */
               {
                   CLI_LIB_PrintStr(BEL);
                   break;
               }
               delIdx  = CLI_LIB_GetDelBufPrevIdx(delIdx, ctrl_P);
               del_buf = CLI_LIB_GetDelBuf(delIdx, ctrl_P);
               strcpy((char *)old_buf, (char *)edit_buf);

               used_len = strlen((char *)del_buf);
               diff_len = used_len - prev_len;
               if (buf_cnt+diff_len > bufsize-1)
               {
                   used_len = bufsize-1-buf_cnt-prev_len;
                   diff_len = used_len - prev_len;
               }

               /* Move buffer and fill in correct contents */
               if (diff_len < 0)
               {
                   for (j=si; j<buf_cnt; j++)
                       *(edit_buf+j+diff_len) = *(edit_buf+j);
               }
               else if (diff_len > 0)
               {
                   for (j=buf_cnt; j>si; j--)
                       *(edit_buf+(j-1)+diff_len) = *(edit_buf+(j-1));
               }
               for (j=0; j<used_len; j++)
                   *(edit_buf+si-prev_len+j) = *(del_buf+j);

               *(edit_buf+buf_cnt+diff_len) = '\0';

               si = si + diff_len;
               buf_cnt = buf_cnt + diff_len;

               if ((si <= shift_num) && (shift_num > 0))
               {
                   shift_num = CLI_LIB_ScrollToRight(old_buf, edit_buf,
                               si-diff_len, si, shift_num, disp_width);
               }
               else if (si-shift_num >= disp_width)
               {
                   shift_num = CLI_LIB_ScrollToLeft(old_buf, edit_buf,
                               si-diff_len, si, shift_num, disp_width);
               }
               else
               {
                   sidx = si - diff_len - prev_len;
                   if ((sidx <= shift_num) && (shift_num > 0))
                       sidx = 1;

                   for (i=sidx; i<si-diff_len; i++)
                       CLI_LIB_PrintStr(BACK);
                   CLI_LIB_ScrollAt(old_buf, edit_buf, sidx, si,
                               shift_num, shift_num, disp_width);
               }
               prev_len = used_len;
            }
            break;

        case (PRNTABLE):
            //printf("%s %d \n", __FUNCTION__,__LINE__);
            CLI_MAIN_GET_WAIT_KEY(&wait_key);
            //printf("%s %d , wait_key is %lu \n", __FUNCTION__,__LINE__, wait_key);
            /*
            if( wait_key == 1)//get char
            {
                printf("%s %d \n", __FUNCTION__,__LINE__);
                CLI_MAIN_HandleMGR_Thread(SYSFUN_TIMEOUT_NOWAIT, ch, NULL);
                printf("%s %d \n", __FUNCTION__,__LINE__);
                break;
            }*/

          // printf(" wait key is %lu\n",wait_key);
           if(wait_key == 1)
           {
            UI32_T msgq_handle;

            CLI_TASK_GET_IPC_Handle(&msgq_handle);
#pragma mark -
#pragma mark mark for passing compile
            // CLI_IPCIO_ParseKey(msgq_handle,ch, &wait_key, NULL);
#pragma mark -
            CLI_MAIN_SET_WAIT_KEY(0);
            break;
            }
           else if(wait_key == 2)
           {
           //store key
               cli_pars_ipcio_msg_buf[ipcio_count]=ch;
               printf("%c",ch);
               fflush(stdout);
               ipcio_count++;
             }
            else
            {
            /*---------------------------------*/
            /* Print the printable characters  */
            /*---------------------------------*/
            if (buf_cnt >= bufsize-1)      /* check if exceed the buffer*/
            {
                /*jingyan zheng add to fix defect ES3628BT-FLF-ZZ-00044*/
                if (CLI_PARS_GetKeyState(ctrl_P) == UI_KEY_CTRL_V_ON)
                {
                    CLI_PARS_ResetKeyState(ctrl_P);
                }
                CLI_LIB_PrintStr(BEL);
                break;
            }
            strcpy((char *)old_buf, (char *)edit_buf);

            cnt = 1;          /* insert char count */
            if (CLI_PARS_GetKeyState(ctrl_P) == UI_KEY_CTRL_V_ON)
            {
                CLI_PARS_ResetKeyState(ctrl_P);
                if ((ch >= 0x01) && (ch <= 0x1B))
                {
                    if (ch == 0x09)     /* TAB key */
                        cnt = 8;
                    else
                        cnt++;
                }
                if (arrow_type != 0)
                    cnt = 2;
            }

            for (j=buf_cnt-1; j>=si; j--)  /* move buffer */
                *(edit_buf+j+cnt) = *(edit_buf+j);

            *(edit_buf+si) = ch;
            if (cnt == 2)
            {
                if (arrow_type == 0)
                    *(edit_buf+si+1) = ch + 0x40;
                else
                    *(edit_buf+si+1) = arrow_type + 0x40;
            }
            else if (cnt == 8)
            {
                for (i=1; i<7; i++)
                    *(edit_buf+si+i) = 0x20;
                *(edit_buf+si+7) = ch;
            }
            si += cnt;
            buf_cnt += cnt;
            *(edit_buf+buf_cnt) = '\0';


            if (! Is_Hidden)
            {
               if (si-shift_num >= disp_width)
               {
                  shift_num = CLI_LIB_ScrollToLeft(old_buf, edit_buf, si-cnt,
                                              si, shift_num, disp_width);
               }
               else
               {
                  CLI_LIB_ScrollAt(old_buf, edit_buf, si-cnt, si, shift_num,
                              shift_num, disp_width);
               }
            }
            }
            break;

        case (QUMARK):
            if (Is_CtrlKey_Skipped == FALSE)
            {
               if (buf_cnt >= bufsize-1)      /* check if exceed the buffer*/
               {
                  CLI_LIB_PrintStr(BEL);
                  break;
               }

               CLI_LIB_PutChar(ch);
               CLI_LIB_PrintNullStr(1);

               ret_val = 2;
               exit_flag = 1;
            }
            break;

        case (ENTER):

           //printf("%s %d \n", __FUNCTION__,__LINE__);
           CLI_MAIN_GET_WAIT_KEY(&wait_key);
           //printf(" wait key is %lu\n",wait_key);
           if(wait_key == 2)
           {
            UI32_T msgq_handle;

            CLI_TASK_GET_IPC_Handle(&msgq_handle);
            cli_pars_ipcio_msg_buf[ipcio_count]='\0';
#pragma mark -
#pragma mark mark for passing compile
            //CLI_IPCIO_ParseKey(msgq_handle, 0, &wait_key,cli_pars_ipcio_msg_buf);
#pragma mark -
            CLI_MAIN_SET_WAIT_KEY(wait_key);
            cli_pars_ipcio_msg_buf[0]='\0';
            ipcio_count=0;
            break;
            }
            else
            {
           //printf(" wait key is %lu\n",wait_key);
           /*
           if( wait_key == 2)//get char
           {
               // printf("%s %d \n", __FUNCTION__,__LINE__);
                CLI_MAIN_HandleMGR_Thread(SYSFUN_TIMEOUT_NOWAIT, ch, edit_buf);
               // printf("%s %d \n", __FUNCTION__,__LINE__);
                break;
           }
            */
           ret_val = 0;
           exit_flag = 1;
           }
           break;

        case (CTRL_C):
            if (Is_CtrlKey_Skipped == FALSE)
            {
               *edit_buf = '\0';
               ret_val = 0;
               exit_flag = 1;
            }
            break;

        /*Debug mode*/
        case (CTRL_G):
            if (Is_CtrlKey_Skipped == FALSE)
            {
               *edit_buf = '\0';
               ret_val = 0;
               exit_flag = 1;

               CLI_MAIN_Enter_DebugMode();
            }
            break;

        case (CTRL_Z): /* Exit the global configuration mode and go to privilege EXEC mode */
            if (Is_CtrlKey_Skipped == FALSE)
            {
               switch(ctrl_P->CMenu.AccMode)
               {
               case PRIVILEGE_CFG_GLOBAL_MODE:
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;

               case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
               ctrl_P->CMenu.loopback_id = 0;
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;

               case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
               memset(ctrl_P->CMenu.port_id_list, 0, sizeof(ctrl_P->CMenu.port_id_list));
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
               case PRIVILEGE_CFG_INTERFACE_ETH0_MODE:/*wf 20090106*/
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#endif
#if (SYS_CPNT_CRAFT_PORT == TRUE)
               case PRIVILEGE_CFG_INTERFACE_CRAFT_MODE:
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#endif
#if (SYS_CPNT_FILTER_THROOTTLE == TRUE)
               case PRIVILEGE_CFG_IGMP_PROFILE_MODE:
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#endif
               case PRIVILEGE_CFG_INTERFACE_VLAN_MODE:
               ctrl_P->CMenu.vlan_ifindex = 0;
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#if (SYS_CPNT_IP_TUNNEL == TRUE)
               case PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE:
               ctrl_P->CMenu.vlan_ifindex = 0;
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#endif /*(SYS_CPNT_IP_TUNNEL == TRUE)*/
               case PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE:
               ctrl_P->CMenu.pchannel_id = 0;
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;

               case PRIVILEGE_CFG_VLAN_DATABASE_MODE:
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;

               case PRIVILEGE_CFG_LINE_MODE:
               ctrl_P->CMenu.line_type = 0;
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;

            case PRIVILEGE_CFG_DOMAIN_MODE:
               ctrl_P->CMenu.domain_index = 0;
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;

#if (SYS_CPNT_STP == SYS_CPNT_STP_TYPE_MSTP)
               case PRIVILEGE_CFG_MSTP_MODE:
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;
#endif

               case PRIVILEGE_CFG_ROUTER_MODE:
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;

               case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;

               case PRIVILEGE_CFG_ROUTEROSPF_MODE:
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;

               case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#if (SYS_CPNT_BGP == TRUE)
               case PRIVILEGE_CFG_ROUTER_BGP_MODE:
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#endif
#if (SYS_CPNT_NSM_POLICY == TRUE)
               case PRIVILEGE_CFG_ROUTE_MAP_MODE:
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#endif
#if (CLI_SUPPORT_L3_FEATURE == 1)
               case PRIVILEGE_CFG_DHCPPOOL_MODE:
               memset(ctrl_P->CMenu.Pool_name, 0, sizeof(ctrl_P->CMenu.Pool_name));
               ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#endif

#if (SYS_CPNT_ACL == TRUE)
               case PRIVILEGE_CFG_ACL_MAC_MODE:
                  ctrl_P->CMenu.acl_type = 0;
                  ctrl_P->CMenu.acl_name[0] = 0;
                  ctrl_P->CMenu.mask_mode = 0;
                  ctrl_P->CMenu.mask_type = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;

               case PRIVILEGE_CFG_ACL_STANDARD_MODE:
                  ctrl_P->CMenu.acl_type = 0;
                  ctrl_P->CMenu.acl_name[0] = 0;
                  ctrl_P->CMenu.mask_mode = 0;
                  ctrl_P->CMenu.mask_type = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;

               case PRIVILEGE_CFG_ACL_EXTENDED_MODE:
                  ctrl_P->CMenu.acl_type = 0;
                  ctrl_P->CMenu.acl_name[0] = 0;
                  ctrl_P->CMenu.mask_mode = 0;
                  ctrl_P->CMenu.mask_type = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;

               case PRIVILEGE_CFG_ACL_IP_MASK_MODE:
                  ctrl_P->CMenu.acl_type = 0;
                  ctrl_P->CMenu.acl_name[0] = 0;
                  ctrl_P->CMenu.mask_mode = 0;
                  ctrl_P->CMenu.mask_type = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;

               case PRIVILEGE_CFG_ACL_MAC_MASK_MODE:
                  ctrl_P->CMenu.acl_type = 0;
                  ctrl_P->CMenu.acl_name[0] = 0;
                  ctrl_P->CMenu.mask_mode = 0;
                  ctrl_P->CMenu.mask_type = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;
#if (SYS_CPNT_DAI == TRUE)
               case PRIVILEGE_CFG_ACL_ARP_MODE:
                  ctrl_P->CMenu.acl_type = 0;
                  ctrl_P->CMenu.acl_name[0] = 0;
                  ctrl_P->CMenu.mask_mode = 0;
                  ctrl_P->CMenu.mask_type = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;
#endif
#if (SYS_CPNT_ACL_IPV6 == TRUE)
               case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
               case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
                  ctrl_P->CMenu.acl_type = 0;
                  ctrl_P->CMenu.acl_name[0] = 0;
                  ctrl_P->CMenu.mask_mode = 0;
                  ctrl_P->CMenu.mask_type = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
               case PRIVILEGE_CFG_AAA_SG_MODE:
               case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
                  ctrl_P->CMenu.aaa_sg_name[0] = 0;
                  ctrl_P->CMenu.aaa_sg_type = 0;
                  ctrl_P->CMenu.aaa_sg_group_index = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
               break;
#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
               case PRIVILEGE_CFG_CLASS_MAP_MODE:
                  ctrl_P->CMenu.cmap_name[0] = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;

               case PRIVILEGE_CFG_POLICY_MAP_MODE:
                  ctrl_P->CMenu.pmap_name[0] = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;

               case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
                  ctrl_P->CMenu.cmap_name[0] = 0;
                  ctrl_P->CMenu.pmap_name[0] = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
               case PRIVILEGE_CFG_TIME_RANGE_MODE:
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;
#endif

#if (SYS_CPNT_ERPS == TRUE)
               case PRIVILEGE_CFG_ERPS_MODE:
                  ctrl_P->CMenu.erps_domain_id = 0;
                  ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
                  break;
#endif
               default:
                  ;
               }

               *edit_buf = '\0';
               ret_val = 0;
               exit_flag = 1;
            }

        /* No return */
        default:
            break;
        } /* switch */

        if (exit_flag == 1)
        {
            /*pttch 2002.08.15 must check length of buff size or it will violation*/
            if (strlen((char *)edit_buf) > (bufsize-1))/* check if exceed the buffer*/
            {
                memcpy(buff, edit_buf, sizeof(UI8_T)*(bufsize-1));
                buff[bufsize-1] = '\0';

            }

            else
               strcpy((char *)buff, (char *)edit_buf);
        }

    } /* while */

    return (ret_val);
}


/*****************************************************************************/
/* CLI_PARS_GetAWord                                                         */
/*                                                                           */
/* Description:                                                              */
/*     Get a word from index pointer, and return the last char index.        */
/*                                                                           */
/* Arguments:                                                                */
/*     UI8_T   *buff : Input buffer                                          */
/*     I16_T s_idx   : The first index that start to search a word from      */
/*                      this buffer.                                         */
/*     UI8_T   *word_buf:The specified char that end this word search        */
/*                                                                           */
/* Return:                                                                   */
/*     Return the final char index                                           */
/*                                                                           */
/* Note:                                                                     */
/*     1.if this is the null value word, it return 0.                        */
/*****************************************************************************/

UI16_T CLI_PARS_GetAWord(char *buff, UI16_T *str_idx, char *word_buf)
{
    I16_T cnt, w_cnt;
    UI16_T  s_idx;

    cnt = w_cnt = 0;
    word_buf[0] = '\0';
    s_idx = *str_idx;

    /* skip whitespace char */

    while (*(buff + s_idx + cnt) == ' ')
    {
        cnt++;
    }

    *str_idx = s_idx + cnt;


    /* Check if end of line */

    if (*(buff+s_idx+cnt) == '\0')
        return 0;


/* dralee 99.2.1 to fix user press "/ + tab", then system crash */
    /* if first char is '/', process it as a command token */
/*
    if( s_idx == 0 && *(buff+s_idx+cnt) == '/')
    {
        *(word_buf+w_cnt) = *(buff+s_idx+cnt);
        w_cnt++;
        cnt++;
        *(word_buf+w_cnt) = '\0';
        return (s_idx+cnt);
    }
*/
/*
pttch 91.12.03 mark it, because "/" is not delimeter and will not crash
when user press "/ + tab"
*/

    /* Find end of word */

/*    while ( (*(buff+s_idx+cnt) != ' ') && (*(buff+s_idx+cnt) != '\0'))
 * dralee 98.8.14 for TOM. '/' and ':' be regaarded as a token sparator
 */

    while ((*(buff+s_idx+cnt) != ' ' /*&& *(buff+s_idx+cnt) != '/'*/)
            && (*(buff+s_idx+cnt) != '\0'))
    {
        *(word_buf+w_cnt) = *(buff+s_idx+cnt);
        w_cnt++;
        cnt++;

        /*  comment by Peter.guo at 2005/02/17.
         *  Cause: char ':' can not be regarded as command line delimiter any more.
         *  Side effect. Colon Data can only be looked upon as keyword.
         */

        /* peter comment
         *   if(*(word_buf+w_cnt-1) == ':')
         *       break;
         */

        /*
         * peter.guo, 2005/03/01, try to modify to fit original colon data processing.
         * Note: if new data type that contains ':' is added to CLI, here should be changed again!
         */
        {
            char *ptr_b, *ptr_e;
            ptr_b = ptr_e = word_buf+w_cnt-1;
            if(*ptr_b == ':')
            {
                /* this colon did not belong to part of ipv6 address or prefix
                 */
                while(*ptr_e != 0 && *ptr_e != ' ') ptr_e++;

                if((!L_IPV4V6_Is_Valid_IPv6_PrefixString (word_buf, w_cnt+(ptr_e-ptr_b))) &&
                   (!L_IPV4V6_Is_Valid_IPv6_AddressString(word_buf, w_cnt+(ptr_e-ptr_b))))
                {
          break;
    }
            }
        }
    }

    *(word_buf+w_cnt) = '\0';

    return (s_idx+cnt);
}


/************************************************************************/
/* CLI_PARS_GetWords()                                                        */
/*                                                                      */
/* Description:                                                         */
/*     Get pointers of words from buff                                  */
/*                                                                      */
/* Arguments:                                                           */
/*     UI8_T  *buff    : Input buffer                                    */
/*     UI8_T  *words[] : the word starting pointer                       */
/*     UI16_T widx[]   : the word starting index in the buff.            */
/*     UI16_T max      : max word number that need to be got            */
/*                                                                      */
/* Return:                                                              */
/*     Return the word number that is found                             */
/************************************************************************/
UI16_T CLI_PARS_GetWords(UI8_T *buff, UI8_T *words[], UI16_T widx[], UI16_T max)
{
    UI16_T num, i, idx;
    UI16_T quat_flag;
    UI16_T bypass_flag=0; /* dralee 99.1.14 for copy tftp filename support path name. */

    for (i=0; i<max; i++)
        widx[i] = 0;

    num = 0;
    idx = 0;

    while (*buff != '\0')
    {
#if 1
       quat_flag = 0;
#endif

       /* Skip lead white space */

       while (*buff == ' ')
       {
          idx++;
          buff++;
       }

       if (*buff == '\0')
            break;

#if 1
       if( *buff == '"')
           quat_flag = 1;
#endif

       if (num < max)
       {
           widx[num] = idx;
           words[num] = buff;
           num++;
           idx++;
           buff++;
/* dralee 99.1.14 don't regard '/' as a token after 3 '/' is got */
/* dralee 98.10.6 */
           if( *(buff-1) == '/' )
           {
                 if( (++bypass_flag) <= 3 )
               continue;
           }
       }
/* dralee 99.11.10
 *       while(*buff != ' ' && *buff != '/' && *buff != '\0')
 *       {
 *            idx++;
 *            buff++;
 *       }
 */

/* dralee 98.11.8 for solving quot mark */

#if 1

/* dralee 99.1.14
 *       while(*buff != '/' && *buff != '\0')
 */
       while((*buff != '/' || (*buff == '/' && bypass_flag >=3)) && *buff != '\0')
       {
           if( *buff == ' ' && quat_flag == 0)
                  break;
           else if( *buff == ' ' && quat_flag == 1)
           {
              idx++;
              buff++;
           }
           else if( *buff == '"' && *(buff-1) == '\\')
           {
              idx++;
              buff++;
           }
           else if( *buff == '"' && quat_flag == 1)
           {
              idx++;
              buff++;
              quat_flag = 0;
           }
           else
           {
             idx++;
             buff++;
           }
       }
#endif

        switch(*buff)
                {
                  case ' ':
                      idx++;
                          *buff++ = '\0';
                          break;

                  case '/':
/* dralee 99.1.14 don't regards '/' as a token after 3 '/' are got
 *                    if(num < max)
 */
                      if(num < max && (++bypass_flag <=3) )
                          {
                            widx[num] = idx;
                            words[num] = buff;
                num++;
                          }
                          idx++;
              buff++;

                }


    } /* while */

#ifdef CLI_DEBUG
        for(i=0;i<num;i++)
        {
          CLI_LIB_PrintNullStr(1);
          CLI_LIB_PrintStr(words[i]);
          CLI_LIB_PrintNullStr(1);
        }
#endif

    return num;
}

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_PARS_GetNeiborCommandIndex
 *---------------------------------------------------------------------------
 * PURPOSE  : Get neibor command index from starting index
 * INPUT    : cmd_lst       - command list
 *            start_index   - starting command index
 * OUTPUT   : None
 * RETURN   : command index or NULL_CMD
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
UI16_T
CLI_PARS_GetNeiborCommandIndex(
    const CMDLIST * cmd_list,
    UI16_T start_idx)
{
#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    CLI_TASK_WorkingArea_T *ctrl_P =
        (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();
#endif

    if (NULL_CMD == start_idx)
    {
        return NULL_CMD;
    }

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    while (1)
    {
        UI16_T neibor_idx = cmd_list[start_idx].neibor_idx;

        if (NULL_CMD == neibor_idx)
        {
            return NULL_CMD;
        }

        if (ctrl_P->CMenu.AccessPrivilege < cmd_list[neibor_idx].current_privilege)
        {
            start_idx = neibor_idx;
            continue;
        }

        return neibor_idx;
    }
#else
    return cmd_list[start_idx].neibor_idx;
#endif
}

/*---------------------------------------------------------------------------
 * ROUTINE NAME - CLI_PARS_GetCommandIndex
 *---------------------------------------------------------------------------
 * PURPOSE  : Get command index from starting index
 * INPUT    : cmd_lst       - command list
 *            start_index   - starting command index
 * OUTPUT   : None
 * RETURN   : command index or NULL_CMD
 * NOTE     : None
 *---------------------------------------------------------------------------
 */
UI16_T
CLI_PARS_GetCommandIndex(
    const CMDLIST * cmd_list,
    UI16_T start_idx)
{
#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    CLI_TASK_WorkingArea_T *ctrl_P =
        (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    if (NULL_CMD == start_idx)
    {
        return NULL_CMD;
    }

    if (ctrl_P->CMenu.AccessPrivilege < cmd_list[start_idx].current_privilege)
    {
        return CLI_PARS_GetNeiborCommandIndex(cmd_list, start_idx);
    }
#endif

    return start_idx;
}

/************************************************************************/
/* CLI_PARS_LookupCmdWord                                               */
/*                                                                      */
/* Description:                                                         */
/*     Lookup a command word in the command words list                  */
/*                                                                      */
/* Arguments:                                                           */
/*     UI8_T    *word_buf : word pointer                                */
/*     UI16_T   start_idx : the start point that to be searched         */
/*     UI16_T   *near_idx : the nearest matched command word index      */
/*                                                                      */
/* Return:                                                              */
/*     CLI_NO_ERROR          => when exactly match one command word     */
/*     CLI_ERR_CMD_INVALID   => when not match any command word         */
/*     CLI_ERR_CMD_AMBIGUOUS => when found ambiguous words              */
/************************************************************************/
UI16_T
CLI_PARS_LookupCmdWord(char *word_buf, UI16_T start_idx, UI16_T *near_idx)
{
    UI16_T len1;
    UI16_T curr_idx, prev_idx, next_idx;
    I16_T cmp_flag;
    UI16_T  match_num, match_num1;
    const CMDLIST *ui_cmd_lst;

    curr_idx = start_idx;
    prev_idx = NULL_CMD;
    len1 = strlen((char *)word_buf);

    ui_cmd_lst = CLI_LIB_Get_Cmd_List();

    while (1)
    {
        *near_idx = 0;

        /*
         * Check user Access Mode
         */
        curr_idx = CLI_PARS_GetCommandIndex(ui_cmd_lst, curr_idx);
        if(curr_idx == NULL_CMD)
        {
             *near_idx = curr_idx;
             return CLI_ERR_CMD_INVALID;
        }

        cmp_flag = CLI_LIB_StrCmp((char *)word_buf, ui_cmd_lst[curr_idx].name, &match_num);

        if (cmp_flag == 0)      /* the same string */
        {
            *near_idx = curr_idx;
            return CLI_NO_ERROR;
        }
        else if (cmp_flag < 0)
        {
            /*
             * word_buff shorter or lower than current command word
             */
            if (match_num == len1)
            {
                /*
                 * Shorter than current and matched with it,
                 * check the neighbor to see if match again -> ambiguous
                 */
                next_idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, curr_idx);

                if (next_idx != NULL_CMD)
                {
                    CLI_LIB_StrCmp((char *)word_buf, ui_cmd_lst[next_idx].name, &match_num);
                    if (match_num == len1)
                    {
                        /*
                         * Ambiguous
                         */
                        *near_idx = curr_idx;
                        return CLI_ERR_CMD_AMBIGUOUS;
                    }
                }

                /*
                 * Found abbreviation word
                 */
                *near_idx = curr_idx;
                return CLI_NO_ERROR;

            }  /* if */

            /*
             * Lower than current command word,
             * check with previous command word
             */
            if (prev_idx == NULL_CMD)
            {
                /*
                 * String in word_buf is lower than the first command word,
                 * not found any matched command word
                 */
                *near_idx = curr_idx;
                return CLI_ERR_CMD_INVALID;
            }
            else
            {
                /*
                 * Check with previous command word
                 * to see which word is the nearest word
                 */
                CLI_LIB_StrCmp((char *)word_buf, ui_cmd_lst[prev_idx].name, &match_num);
                CLI_LIB_StrCmp((char *)word_buf, ui_cmd_lst[curr_idx].name, &match_num1);

                if (match_num >= match_num1)
                    *near_idx = prev_idx;
                else
                    *near_idx = curr_idx;

               return CLI_ERR_CMD_INVALID;
            }

        }
        else if (cmp_flag > 0)
        {
            /*
             * Longer than or larger than current command word
             */
            if (ui_cmd_lst[curr_idx].neibor_idx != NULL_CMD)
            {
                prev_idx = curr_idx;
                curr_idx = ui_cmd_lst[curr_idx].neibor_idx;
            }
            else
            {
                *near_idx = curr_idx;
                return CLI_ERR_CMD_INVALID;
            }
        }

    }  /* while */

}



UI8_T *CLI_PARS_TOKEN_UTIL_Get_Token(UI8_T *s, UI8_T *Token)
{
   int  state = 0;

   for( ; *s !=0; s++)
   {
      switch ( state )
      {
         case 0:   /* skip while space */
         if ( isspace (*s) || (*s == 0x0a) ||  (*s == 0x0d) )
         {
            continue;
         }
         else
         {
            state = 1;
            *Token++ = *s;
            break;
         }

         case 1:   /* character occures */
         if ( isspace(*s) )
         {
            *Token = '\0';
            return s;
         }
         else
         {
            *Token++ = *s;
            break;
         }
      }
   }
   return 0; /* if (*s == 0) occures */
}


/************************************************************************/
/* CLI_PARS_RegisterArgumentParseHandler                                */
/*                                                                      */
/* Description:                                                         */
/*     Register parse function for argument                             */
/*                                                                      */
/* Arguments:                                                           */
/*     None                                                             */
/*                                                                      */
/* Output:                                                              */
/*     None                                                             */
/*                                                                      */
/* Return:                                                              */
/*     None                                                             */
/*                                                                      */
/* Invoke this function before use CLI_PARS_ParseArgument and           */
/* CLI_PARS_GetArgumentHelpMessage.                                     */
/*                                                                      */
/************************************************************************/
void CLI_PARS_RegisterArgumentParseHandler()
{
    cli_pars_arg_handler[BGP_COMMUNITY_DATA].parse = CLI_BGP_COMMUNITY_DATA_Parse;
}

/************************************************************************/
/* CLI_PARS_ParseArgument                                               */
/*                                                                      */
/* Description:                                                         */
/*     Parse an argument                                                */
/*                                                                      */
/* Arguments:                                                           */
/*     token     : Argument. The token may not a null-terminal string   */
/*     token_len : String of the argument                               */
/*     arg_list  : Agurment array list                                  */
/*     arg_idx   : Current argument index                               */
/*                                                                      */
/* Output:                                                              */
/*     match_len : How many character of token be read                  */
/*     name      : Help message for 1st column                          */
/*     help      : Help message for 2nd column                          */
/* Return:                                                              */
/*     ARG_MATCH             => when exactly match                      */
/*     ARG_SUBMATCH          => sub match                               */
/*     ARG_NOMATCH           => no match                                */
/************************************************************************/
int
CLI_PARS_ParseArgument(
    const char *token,
    UI32_T token_len,
    const ARGLIST arg_list[],
    UI16_T  arg_idx,
    UI16_T *match_len,
    UI16_T *err_kind)
{
    return CLI_PARS_ParseArgumentDispatch(token, token_len, arg_list, arg_idx,
                                          match_len, err_kind,
                                          NULL, 0, NULL, 0);
}

/************************************************************************/
/* CLI_PARS_GetArgumentHelpMessage                                      */
/*                                                                      */
/* Description:                                                         */
/*     Get help message of an argument                                  */
/*                                                                      */
/* Arguments:                                                           */
/*     token     : Argument. The token may not a null-terminal string   */
/*     token_len : String of the argument                               */
/*     arg_list  : Agurment array list                                  */
/*     arg_idx   : Current argument index                               */
/*                                                                      */
/* Output:                                                              */
/*     name      : Help message for 1st column                          */
/*     help      : Help message for 2nd column                          */
/* Return:                                                              */
/*     ARG_MATCH             => when exactly match                      */
/*     ARG_SUBMATCH          => sub match                               */
/*     ARG_NOMATCH           => no match                                */
/************************************************************************/
int
CLI_PARS_GetArgumentHelpMessage(
    const char *token,
    UI32_T token_len,
    const ARGLIST arg_list[],
    UI16_T  arg_idx,
    char *name,
    UI32_T name_len,
    char *help,
    UI32_T help_len)
{
    UI16_T match_len;
    UI16_T err_kind;

    return CLI_PARS_ParseArgumentDispatch(token, token_len,
                                          arg_list, arg_idx,
                               	          &match_len, &err_kind,
                                          name, name_len, help, help_len);
}


/************************************************************************/
/* CLI_PARS_ParseArgumentDispatch                                       */
/*                                                                      */
/* Description:                                                         */
/*     Parse an argument                                                */
/*                                                                      */
/* Arguments:                                                           */
/*     token     : Argument. The token may not a null-terminal string   */
/*     token_len : String of the argument                               */
/*     arg_list  : Agurment array list                                  */
/*     arg_idx   : Current argument index                               */
/*                                                                      */
/* Output:                                                              */
/*     match_len : How many character of token be read                  */
/*     name      : Help message for 1st column                          */
/*     help      : Help message for 2nd column                          */
/* Return:                                                              */
/*     ARG_MATCH             => when exactly match                      */
/*     ARG_SUBMATCH          => sub match                               */
/*     ARG_NOMATCH           => no match                                */
/************************************************************************/
static int
CLI_PARS_ParseArgumentDispatch(
    const char *token,
    UI32_T token_len,
    const ARGLIST arg_list[],
    UI16_T  arg_idx,
    UI16_T *match_len,
    UI16_T *err_kind,
    char *name,
    UI32_T name_len,
    char *help,
    UI32_T help_len)
{
    const ARGLIST *arg;

    if (NULL_ARG == arg_idx)
    {
        return CLI_PARS_ARG_NOIMPL;
    }

    arg = &arg_list[arg_idx];

    if (MAX_DATA_TYPE <= arg->type)
    {
        return CLI_PARS_ARG_NOIMPL;
    }

    if (NULL == cli_pars_arg_handler[arg->type].parse)
    {
        return CLI_PARS_ARG_NOIMPL;
    }

    return cli_pars_arg_handler[arg->type].parse(token, token_len,
                                                 arg_list, arg_idx,
                                                 match_len, err_kind,
                                                 name, name_len,
                                                 help, help_len);
}


/************************************************************************/
/* CLI_BGP_COMMUNITY_DATA_Parse                                         */
/*                                                                      */
/* Description:                                                         */
/*     Parse a BGP community data                                       */
/*                                                                      */
/* Arguments:                                                           */
/*     token     : Argument. The token may not a null-terminal string   */
/*     token_len : String of the argument                               */
/*     arg_list  : Agurment array list                                  */
/*     arg_idx   : Current argument index                               */
/*                                                                      */
/* Output:                                                              */
/*     name      : Help message for 1st column                          */
/*     help      : Help message for 2nd column                          */
/* Return:                                                              */
/*     ARG_MATCH             => when exactly match                      */
/*     ARG_SUBMATCH          => sub match                               */
/*     ARG_NOMATCH           => no match                                */
/************************************************************************/
static int
CLI_BGP_COMMUNITY_DATA_Parse(
	const char *token,
    UI16_T token_len,
    const ARGLIST arg_list[],
    UI16_T  arg_idx,
    UI16_T *match_len,
    UI16_T *err_kind,
    char *name,
    UI32_T name_len,
    char *help,
    UI32_T help_len)
{
#define CLI_MAX_COMMUNITY_AA    65535L
#define CLI_MIN_COMMUNITY_AA    1L
#define CLI_MAX_COMMUNITY_NN    65535L
#define CLI_MIN_COMMUNITY_NN    1L

    const ARGLIST *arg = &arg_list[arg_idx];
    const char *p;
    char val[10];
    char *pv;

    assert(arg_list != NULL);
    assert(arg_idx != NULL_ARG);
    assert(arg->type == BGP_COMMUNITY_DATA);

    (*match_len) = 0;
    (*err_kind) = CLI_NO_ERROR;

    if (token == NULL)
    {
        if (name && 1 < name_len)
        {
            SYSFUN_Snprintf(name,
                            name_len,
                            "%s", arg->name);
            name[name_len-1] = '\0';
        }

        if (help && 1 < help_len)
        {
            SYSFUN_Snprintf(help,
                            help_len,
                            "%s", ui_HelpMsg[ arg->help_idx ]);
            help[help_len-1] = '\0';
        }

        *err_kind = CLI_ERR_CMD_INVALID;
        return CLI_PARS_ARG_NOMATCH;
    }

    *match_len = 0;
    memset(val, 0, sizeof(val));

    /* 12345:67890
     * ^^^^^
     */
    for (p=token, pv=val; *p != '\0'; )
    {
        if (!isdigit(*p))
        {
            if (*p == ':')
            {
                break;
            }

            *err_kind = CLI_ERR_CMD_INVALID;
            return CLI_PARS_ARG_NOMATCH;
        }

        *pv++ = *p++;
        ++ *match_len;

        if (sizeof(val) <= (pv - val))
        {
            *err_kind = CLI_ERR_CMD_INVALID_RANGE;
            return CLI_PARS_ARG_NOMATCH;
        }
    }

    if (atol(val) < CLI_MIN_COMMUNITY_AA ||
        CLI_MAX_COMMUNITY_AA < atol(val))
    {
        *err_kind = CLI_ERR_CMD_INVALID_RANGE;
        return CLI_PARS_ARG_NOMATCH;
    }

    if (*p == '\0')
    {
        if (name && 1 < name_len)
        {
            SYSFUN_Snprintf(name,
                            name_len,
                            ":");
        }

        *err_kind = CLI_ERR_CMD_INVALID;
        return CLI_PARS_ARG_NOMATCH;
    }

    /* 12345:67890
     *      ^
     */
    if (*p != ':')
    {
        *err_kind = CLI_ERR_CMD_INVALID;
        return CLI_PARS_ARG_NOMATCH;
    }

    ++p; ++ *match_len;

    if (*p == '\0')
    {
        if (name && 1 < name_len)
        {
            SYSFUN_Snprintf(name,
                            name_len,
                            "<%lu-%lu>",
                            CLI_MIN_COMMUNITY_NN,
                            CLI_MAX_COMMUNITY_NN);
        }

        *err_kind = CLI_ERR_CMD_INVALID;
        return CLI_PARS_ARG_NOMATCH;
    }

    memset(val, 0, sizeof(val));

    /* 12345:67890
     *       ^^^^^
     */
    for (pv=val; *p != '\0'; )
    {
        if (!isdigit(*p))
        {
            if (*p == ' ' || *p == '\t')
            {
                break;
            }

            *err_kind = CLI_ERR_CMD_INVALID;
            return CLI_PARS_ARG_NOMATCH;
        }

        *pv++ = *p++;
        ++ *match_len;

        if (sizeof(val) <= (pv - val))
        {
            *err_kind = CLI_ERR_CMD_INVALID_RANGE;
            return CLI_PARS_ARG_NOMATCH;
        }
    }

    if (atol(val) < CLI_MIN_COMMUNITY_NN ||
        CLI_MAX_COMMUNITY_NN < atol(val))
    {
        *err_kind = CLI_ERR_CMD_INVALID_RANGE;
        return CLI_PARS_ARG_NOMATCH;
    }

    if (name && 1 < name_len)
    {
        SYSFUN_Snprintf(name,
                        name_len,
                        "%s", arg->name);
        name[name_len-1] = '\0';
    }

    return CLI_PARS_ARG_MATCH;

#undef CLI_MAX_COMMUNITY_AA
#undef CLI_MIN_COMMUNITY_AA
#undef CLI_MAX_COMMUNITY_NN
#undef CLI_MIN_COMMUNITY_NN
}

