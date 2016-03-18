#ifndef CLIS_H
#define CLIS_H

#include "sys_type.h"


// cjchu, form IAD sys_bld.h temp use
#define SYS_BLD_FACTORY_DEFAULT_CFG     "/etc/.fact.cfg"
#define SYS_BLD_SYSTEM_CFG_PATH         "/var/sys_cfg/"
#define SYS_BLD_DEFAULT_BOOT_FILE       "/var/sys_cfg/.def_boot_filename"


/* ------------------------------------------------------------------------- */
/* Escape sequence header                                                    */
/* ------------------------------------------------------------------------- */
#define E_HEAD      "\x1B\x5B"


/* ------------------------------------------------------------------------- */
/* Graphics mode define                                                      */
/* ------------------------------------------------------------------------- */
/* Attribute */
#define ALL_OFF          "0"//"0;37;40"
#define BOLD             "1"
#define UNDERLINE        "4"
#define BLINK            "5"
#define REVERSE          "7"
#define INVISIBLE        "8"

#define BOLD_UNDERLINE    "1;4"
#define BOLD_BLINK        "1;5"
#define BOLD_REVERSE      "1;7"
#define UNDERLINE_BLINK   "4;5"
#define UNDERLINE_REVERSE "4;7"
#define BLINK_REVERSE     "5;7"

#define BOLD_UNDERLINE_BLINK    "1;4;5"
#define BOLD_UNDERLINE_REVERSE  "1;4;7"
#define BOLD_BLINK_REVERSE      "1;5;7"
#define UNDERLINE_BLINK_REVERSE "4;5;7"

#define BOLD_UNDERLINE_BLINK_REVERSE "1;4;5;7"

/* Color */
#define FORE_BLACK       "30"
#define FORE_RED         "31"
#define FORE_GREEN       "32"
#define FORE_YELLOW      "33"
#define FORE_BLUE        "34"
#define FORE_MAGENTA     "35"
#define FORE_CYAN        "36"
#define FORE_WHITE       "37"
#define BACK_BLACK       "40"
#define BACK_RED         "41"
#define BACK_GREEN       "42"
#define BACK_YELLOW      "43"
#define BACK_BLUE        "44"
#define BACK_MAGENTA     "45"
#define BACK_CYAN        "46"
#define BACK_WHITE       "47"
	



/* the following block from tomcat, dralee */
/* ------------------------------------------------------------------------- */
/* State in UIS task (control & menu task)                                   */
/* ------------------------------------------------------------------------- */
enum UIS_CTRL_STATE_E
{
    UIS_INIT_STATE = 0,
    UIS_RESTACK_STATE,
    UIS_NORMAL_MAS_STATE,
    UIS_NORMAL_SLA_STATE,
    UIS_DOWNLOAD_STATE,
    UIS_WARM_START_STATE,
    UIS_COLD_START_STATE
};

enum UIS_MENU_STATE_E   /* uis menu task state */
{
    UIS_MENU_NOTCREATE_STATE = 0,
    UIS_MENU_INIT_STATE,
    UIS_MENU_TFTP_DNLD_STATE,  /* used in uis menu task */
    UIS_MENU_SET_EEDEF_STATE,
    UIS_MENU_RESTART_STATE,
    UIS_MENU_EXIT_STATE
};


/* ------------------------------------------------------------------------- */
/* definition for UIS active exit parameter                                  */
/* ------------------------------------------------------------------------- */
enum UIS_ACTIVE_EXIT_REASON_E
{
    EXIT_USER_REQUEST = 1,
    EXIT_READ_TIMEOUT,
    EXIT_LOGIN_ERR_TOO_MANY,
    EXIT_FAIL_CONNECT,
    EXIT_IP_CHANGED,
    EXIT_NO_DISPLAY
};


#if 0
void ui_PrintStr(UI8_T *str);
void ui_PutChar(UI8_T ch);
void ui_EraseDisp(void);
void ui_Send(void);
#endif

#endif
