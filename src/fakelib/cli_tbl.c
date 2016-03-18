/*
 *  cli_tbl.c
 *  CLI_TBL_show
 *
 *  Created by yehjunying on 2009/5/16.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "cli_tbl.h"
#ifndef HAVE_NO_CLI_LIB
#include "cli_api.h"
#include "cli_lib.h"
#endif

static int CLI_TBL_IsOutOfRang(int col);
static int CLI_TBL_GetColStartPosition(CLI_TBL_Object_T *this, int col);
static int CLI_TBL_GetWidth(CLI_TBL_Object_T *this);
static int CLI_TBL_NewLine(CLI_TBL_Object_T *this);
static int CLI_TBL_ResetBuf(CLI_TBL_Object_T *this);
static int CLI_TBL_GetLineLen(const char *text);
static int CLI_TBL_More(CLI_TBL_Object_T *this, const char *str);
static int CLI_TBL_SetText(CLI_TBL_Object_T *this, int col, const char *text, CLI_TBL_Align_T align);

int CLI_TBL_Init(CLI_TBL_Object_T *this) {
    int i;

    for (i=0; i<MAX_NBR_OF_COL; ++i) {
        this->col[i].width = 0;
        this->col[i].align = CLI_TBL_ALIGN_LEFT;
    }

    this->eol[0] = '\r';
    this->eol[1] = '\n';
    this->eol[2] = '\0';
    this->eol_len = 2;
    this->buf = 0;
    this->buf_size = 0;
    this->seaperator = 1;
    this->multi_line = 1;
    this->line_no_to_show = 0;
    return 0;
}

int CLI_TBL_InitWithBuf(CLI_TBL_Object_T *this, char *buf, int buf_size) {
    CLI_TBL_Init(this);
    CLI_TBL_SetBuf(this, buf, buf_size);
    return 0;
}

int CLI_TBL_SetBuf(CLI_TBL_Object_T *this, char *buf, int buf_size) {
    this->buf = buf;
    this->buf_size = buf_size;

    return CLI_TBL_ResetBuf(this);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME:  CLI_TBL_SetSingleLine
 *-------------------------------------------------------------------------
 * PURPOSE  : Set to single line mode
 * INPUT    : this  - tbl object
 * OUTPUT   : None
 * RETURN   : Always returns 0
 * NOTES    : None
 *-------------------------------------------------------------------------
 */
int CLI_TBL_SetSingleLine(CLI_TBL_Object_T *this) {
    this->multi_line = 0;
    return 0;
}

int CLI_TBL_SetLineNum(CLI_TBL_Object_T *this, int line) {
    this->line_no_to_show = line;
    return 0;
}

int CLI_TBL_GetLineNum(CLI_TBL_Object_T *this) {
    return this->line_no_to_show;
}

int CLI_TBL_SetSeaperator(CLI_TBL_Object_T *this, int seaperator) {
    int available_size = this->buf_size - this->eol_len;
    int new_size;
    int old_seaperator = this->seaperator;

    this->seaperator = seaperator;
    new_size = CLI_TBL_GetWidth(this);

    if (available_size < new_size) {
        this->seaperator = old_seaperator;
        return -1;
    }

    return 0;
}

int CLI_TBL_SetColIndirect(CLI_TBL_Object_T *this, CLI_TBL_Temp_T *temp, int col_size) {
    int i;

    for (i=0; i<col_size; ++i) {
        CLI_TBL_SetCol(this, temp[i].col, temp[i].width, temp[i].align);
    }

    return 1;
}

int CLI_TBL_SetCol(CLI_TBL_Object_T *this, int col, int width, CLI_TBL_Align_T align) {
    int available_size = this->buf_size - this->eol_len;
    int new_size;

    if (CLI_TBL_IsOutOfRang(col) || width < 0) {
        return -1;
    }

    /* check the width of table whether is larger than size of buffer
     */
    new_size = CLI_TBL_GetWidth(this) - this->col[col].width + width + this->seaperator;
    if (available_size < new_size) {
        return -1;
    }

    this->col[col].width = width;
    this->col[col].align = align;

    CLI_TBL_ResetBuf(this);
    return 0;
}

int CLI_TBL_SetColTitle(CLI_TBL_Object_T *this, int col, const char *text)
{
    int len;
    int off;

    if (CLI_TBL_IsOutOfRang(col)) {
        return -1;
    }

    if (this->col[col].width <= 0) {
    return -1;
  }

    len = (this->col[col].width < strlen(text)) ? this->col[col].width:strlen(text);
    off = CLI_TBL_GetColStartPosition(this, col);

    memcpy(this->buf + off, text, len);

    return 0;
}

int CLI_TBL_SetColInt(CLI_TBL_Object_T *this, int col, unsigned long val) {
    char text[CLI_TBL_MAX_U32_STR_LEN + 1] = {0};

    if (CLI_TBL_IsOutOfRang(col)) {
        return -1;
    }

    sprintf(text, "%lu", val);
    return CLI_TBL_SetText(this, col, text, this->col[col].align);
}

int CLI_TBL_SetColText(CLI_TBL_Object_T *this, int col, const char *text) {

    if (CLI_TBL_IsOutOfRang(col)) {
        return -1;
    }

    return CLI_TBL_SetText(this, col, text, this->col[col].align);
}

void CLI_TBL_SetLine(CLI_TBL_Object_T *this) {
    int i;
    int offset = 0;

    memset(this->buf, 0, this->buf_size);

    for (i=0; i<MAX_NBR_OF_COL; ++i) {
        if (0 < this->col[i].width) {
            memset(this->buf + offset, '-', this->col[i].width);
            offset += this->col[i].width;

            memset(this->buf + offset, ' ', this->seaperator);
            offset += this->seaperator;
        }
    }

    this->buf[offset+1] = '\0';
}

int CLI_TBL_Print(CLI_TBL_Object_T *this)
{
    int rc;
    int eol = strcspn(this->buf, this->eol);

    while (memcmp(this->buf + eol, this->eol, this->eol_len) == 0) {
        int from = eol + this->eol_len;

        this->buf[eol] = '\0';

        rc = CLI_TBL_More(this, this->buf);
        if (rc != CLI_TBL_PRINT_RC_SUCCESS) {
            CLI_TBL_ResetBuf(this);
            return rc;
        }

        strcpy(this->buf, this->buf + from);
        eol = strcspn(this->buf, this->eol);
    }

    rc = CLI_TBL_More(this, this->buf);

    CLI_TBL_ResetBuf(this);
    return rc;
}

static int CLI_TBL_IsOutOfRang(int col) {
    return (col < 0 || MAX_NBR_OF_COL <= col) ? 1 : 0;
}

static int CLI_TBL_SetText(CLI_TBL_Object_T *this, int col, const char *text, CLI_TBL_Align_T align) {
    int len;
    int off;
    int line_no;
    int line_len;
    const char *src = text;

    if (this->col[col].width <= 0) {
        return -1;
    }

    len = strlen(src);
    if (len == 0)
        return 0;

    off = CLI_TBL_GetColStartPosition(this, col);

    line_no = 0;
    while(0 < len) {
        /* append new line if need
         */
        line_no ++;
        if (this->line_no < line_no) {
            if (CLI_TBL_NewLine(this) < 0) {
                break;
            }
        }

        line_len = CLI_TBL_GetLineLen(src);

        if (this->col[col].width < line_len) //{
            line_len = this->col[col].width;
            /*
            memcpy(this->buf + off, src, this->col[col].width);
            len -= this->col[col].width;
            src += this->col[col].width;
            off += CLI_TBL_GetWidth(this) + this->eol_len;
        }
        else {*/
        off += (align == CLI_TBL_ALIGN_RIGHT)?(this->col[col].width-line_len):0;
        memcpy(this->buf + off, src, line_len);
        len -= line_len;
        src += line_len;
        off += CLI_TBL_GetWidth(this) + this->eol_len;
        /*}*/

        /* skip CR
         */
        if (*src == '\n') {
            ++src;
            --len;
        }
        if (*src == '\r' && *(src+1) == '\n') {
            src += 2;
            len -= 2;
        }

        if (!this->multi_line) {
            break;
        }
    }

    return 0;
}

static int CLI_TBL_GetColStartPosition(CLI_TBL_Object_T *this, int col) {
    int pos = 0;
    int i;

    for (i=0; i<MAX_NBR_OF_COL; ++i) {
        if (i == col) {
            break;
        }

        if (0 < this->col[i].width) {
            pos += this->col[i].width + this->seaperator;
        }
        else {
            break;
        }
    }

    return pos;
}

static int CLI_TBL_GetWidth(CLI_TBL_Object_T *this)
{
    int width = 0;
    int i;

    for (i=0; i<MAX_NBR_OF_COL; ++i) {
        if (0 < this->col[i].width) {
            width += this->col[i].width + this->seaperator;
        }
    }

    if (width) {
        width -= this->seaperator;
    }

    return width;
}

static int CLI_TBL_NewLine(CLI_TBL_Object_T *this) {
    if (this->buf) {
        int start = ((CLI_TBL_GetWidth(this) + this->eol_len) * this->line_no);

        if (this->buf_size < (start + (CLI_TBL_GetWidth(this) + this->eol_len))) {
            return -1;
        }

        /* clear end-of-line of last line
         */
        if (0 < this->line_no) {
            memcpy(this->buf + start - this->eol_len, this->eol, this->eol_len);
        }

        /* append a new line
         */
        memset(this->buf + start, ' ', CLI_TBL_GetWidth(this));
        this->buf[start + CLI_TBL_GetWidth(this)] = '\0';

        ++this->line_no;
        return 0;
    }
    else {
        return -1;
    }
}

static int CLI_TBL_ResetBuf(CLI_TBL_Object_T *this) {
    this->line_no = 0;

    if (this->buf) {
        CLI_TBL_NewLine(this);
    }

    return 0;
}

static int CLI_TBL_GetLineLen(const char *text)
{
    const char *p = text;
    int len = 0;

    while (*p)
    {
        if (*p == '\n')
            return len;
        else if (*p == '\r' && *(p+1) == '\n')
            return len;

        ++p;
        ++len;

        if (!*p) break;
    }

    return len;
}

#ifdef HAVE_NO_CLI_LIB
int CLI_LIB_MaxLineNumPerScreen() {
    return 200;
}

int CLI_API_Get_Print_Interactive_Mode() {
    return 1;
}

void CLI_LIB_Printf(const char *fmt, const char *s1, const char *s2) {
    printf(fmt, s1, s2);
}

void CLI_LIB_PrintStr1(const char *s1, const char *s2) {
    printf("%s%s", s1, s2);
}

int CLI_LIB_ProcMoreFeature() {
    CLI_LIB_PrintStr1("--more--", "\r\n");
    return CLI_EXIT_SESSION;
}
#endif

static int CLI_TBL_More(CLI_TBL_Object_T *this, const char *str) {
    int rc;

    CLI_LIB_Printf("%s%s", str, this->eol);

    ++this->line_no_to_show;

    if (!CLI_API_Get_Print_Interactive_Mode())
        return CLI_TBL_PRINT_RC_SUCCESS;

    if (this->line_no_to_show < CLI_LIB_MaxLineNumPerScreen()-1)
        return CLI_TBL_PRINT_RC_SUCCESS;

    rc = CLI_LIB_ProcMoreFeature();
    switch(rc)
    {
        /* press SPACE, print one page */
        case 1:
        this->line_no_to_show = 0;
        break;

        /* press ENTER, print one line */
        case 2:
        this->line_no_to_show --;
        break;

        case CLI_EXIT_SESSION:
        return CLI_TBL_PRINT_RC_ERROR;

        /* press any key, exit */
        case 0:
        /* press ^C, exit */
        case 3:
        default:
        return CLI_TBL_PRINT_RC_BREAK;
    }

    return CLI_TBL_PRINT_RC_SUCCESS;
}



