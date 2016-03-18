/*
 *  table.h
 *  CLI_TBL_show
 *
 *  Created by yehjunying on 2009/5/16.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "cli_msg.h"

/*
    <col>

    <title, right aligned><title>               <title>
    ----------<seaperator>-----------<seaperator>-------  <line>
    <int,right            <text, left
      aligned>            aligned>

    default:
    seaperator = 1
    multi_line = 1
 */

#define MAX_NBR_OF_COL          10
#define MAX_EOL_STR_LEN         2
#define CLI_TBL_MAX_U32_STR_LEN 10

typedef enum {
    CLI_TBL_ALIGN_LEFT,
    CLI_TBL_ALIGN_RIGHT
}CLI_TBL_Align_T;

typedef struct {
    int col;
    int width;
    CLI_TBL_Align_T align;
}CLI_TBL_Temp_T;

typedef struct {
    int width;
    CLI_TBL_Align_T align;
}CLI_TBL_Column_T;

typedef struct {
    int multi_line;                     /* !0: multi lines, 0: single line */
    unsigned int seaperator;            /* seaperator width */
    int line_no_to_show;                /* current line no of one page */

    /* variables for internal use
     */
    int line_no;                        /* max line no of buffer */
    int buf_size;
    char *buf;
    int eol_len;
    char eol[MAX_EOL_STR_LEN+1];
    CLI_TBL_Column_T col[MAX_NBR_OF_COL];
}CLI_TBL_Object_T;


#define CLI_TBL_PRINT_RC_SUCCESS    (0)
#define CLI_TBL_PRINT_RC_BREAK      (-1)
#define CLI_TBL_PRINT_RC_ERROR      (-2)

#define CLI_TBL_PRINT_FAIL_RC_TO_CLI(rc)                            \
    ((rc == CLI_TBL_PRINT_RC_BREAK) ? CLI_NO_ERROR : CLI_EXIT_SESSION)

/* Init Functions
 */
int CLI_TBL_Init(CLI_TBL_Object_T *this);
int CLI_TBL_InitWithBuf(CLI_TBL_Object_T *this, char *buf, int buf_size);
int CLI_TBL_SetBuf(CLI_TBL_Object_T *this, char *buf, int buf_size);

/* Config Functions
 */
int CLI_TBL_SetSingleLine(CLI_TBL_Object_T *this);
int CLI_TBL_SetLineNum(CLI_TBL_Object_T *this, int line);
int CLI_TBL_GetLineNum(CLI_TBL_Object_T *this);
int CLI_TBL_SetSeaperator(CLI_TBL_Object_T *this, unsigned int seaperator);
int CLI_TBL_SetCol(CLI_TBL_Object_T *this, int col, int width, CLI_TBL_Align_T align);
int CLI_TBL_SetColIndirect(CLI_TBL_Object_T *this, CLI_TBL_Temp_T *temp, int col_size);

/* Set Context of Table Functions
 */
int CLI_TBL_SetColTitle(CLI_TBL_Object_T *this, int col, const char *text);
int CLI_TBL_SetColText(CLI_TBL_Object_T *this, int col, const char *text);
int CLI_TBL_SetColInt(CLI_TBL_Object_T *this, int col, unsigned long val);
void CLI_TBL_SetLine(CLI_TBL_Object_T *this);

/* Output Functions
 */
int CLI_TBL_Print(CLI_TBL_Object_T *this);
