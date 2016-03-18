/*
 * $Id: io.h,v 1.12 Broadcom SDK $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File: 	io.h
 * Purpose: 	SAL I/O definitions
 */

#ifndef _SAL_IO_H
#define _SAL_IO_H

#include <sal/core/libc.h>


#ifdef NO_SAL_APPL

/* You must specify a stdarg */
#include <stdarg.h>

/*
 * The only things exported or required are console read/write
 * These must be provided by the target environment. 
 */
extern int sal_portable_vprintf(const char* fmt, va_list vargs); 
extern char* sal_portable_readline(char *prompt, char *buf, int bufsize);


/* These are provided internally using the above */
extern int sal_portable_printf(const char* fmt, ...); 
extern char* sal_portable_readline_internal(char* prompt, char* buf, int bufsize, char* defl); 

#define printk sal_portable_printf
#define vprintk sal_portable_vprintf
#define sal_readline sal_portable_readline_internal
#define sal_printf sal_portable_printf

/*
 * debugk
 *
 *  Debug printing facility (also synchronized with locks like printk).
 *  Specify one or more debug message classes in the call.  The message is
 *  displayed only if the message class is enabled via debugk_select().
 *
 *  Use debugk_select to activate selected debug message classes.
 *  Use debugk_check to check if specified class(es) are currently active.
 *
 *  NOTE!! When adding here, add matching entry to dk_map[] in shell.c.
 */


/* NOTE -- The actual debug flags have been moved to <soc/debug.h> */

extern uint32 sal_portable_debugk_select(uint32 dk);
extern uint32 sal_portable_debugk_enable(uint32 dk);
extern uint32 sal_portable_debugk_disable(uint32 dk);
extern int sal_portable_debugk_check(uint32 dk);
extern int sal_portable_debugk(uint32 dk, const char *, ...)
     COMPILER_ATTRIBUTE ((format (printf, 2, 3)));
extern int sal_portable_vdebugk(uint32 dk, const char* fmt, va_list varg);

#define debugk_select sal_portable_debugk_select
#define debugk_enable sal_portable_debugk_enable
#define debugk_disable sal_portable_debugk_disable
#define debugk_check sal_portable_debugk_check
#define debugk sal_portable_debugk
#define vdebugk sal_portable_vdebugk


#define SAL_VT100_SO		"\033[7m"	/* Stand-out begin */
#define SAL_VT100_SE		"\033[m"	/* Stand-out end */
#define SAL_VT100_CE		"\033[K"	/* Kill to end of line */

#define printk_cons_is_enabled() 1

#else



#ifdef NO_FILEIO
typedef void* FILE;	/* Dummy for declarations. Never used. */
#define EOF (-1)
#else
#include <stdio.h>
#endif

#include <stdarg.h>

#ifdef VA_COPY_ARRAY
#  define va_copy(to, from)	((to)[0] = (from)[0])
#endif

#ifdef VA_COPY_PTR
#  define va_copy(to, from)	((to) = (from))
#endif

#ifndef va_copy
# if defined(MOUSSE) || defined(BMW)
#  define va_copy(to, from)	((to)[0] = (from)[0])
# elif defined(UNIX) || defined(IDTRP334) || defined(__BORLAND__) || \
     defined(linux) || defined(MBZ) || defined(IDT438) || defined(NSX) || \
     defined(ROBO_4704) || defined(ROBO_4702) || defined(METROCORE)
#  define va_copy(to, from)	((to) = (from))
# else
#  error va_copy?
# endif
#endif

#include <sal/types.h>

#define SAL_NAME_MAX		256

#define SAL_VT100_SO		"\033[7m"	/* Stand-out begin */
#define SAL_VT100_SE		"\033[m"	/* Stand-out end */
#define SAL_VT100_CE		"\033[K"	/* Kill to end of line */

typedef struct sal_console_info_s {
    int cols;           /* characters per row */
    int rows;           /* number of rows */
} sal_console_info_t;

extern int sal_console_init(void);
extern int sal_console_info_get(sal_console_info_t *info);
extern int sal_console_write(const void *buf, int count);
extern int sal_console_read(void *buf, int count);
extern char *sal_console_gets(char *buf, int size);
extern int sal_printf(const char *fmt, ...) COMPILER_ATTRIBUTE ((format (printf, 1, 2)));
extern int sal_vprintf(const char *fmt, va_list varg);

extern int sal_open(char *name, int oflag, ...);
extern int sal_close(int fd);

extern FILE *sal_fopen(char *name, char *mode);

extern int sal_fclose(FILE *fp);

extern int sal_homedir_set(char *dir);
extern char *sal_homedir_get(char *buf, size_t size);

extern char *sal_getcwd(char *buf, size_t size);
extern int sal_ls(char *filename, char *flags);
extern int sal_cd(char *filename);
extern int sal_remove(char *filename);
extern int sal_rename(char *file_old, char *file_new);
extern int sal_mkdir(char *path);
extern int sal_rmdir(char *path);

#ifdef VXWORKS
int sal_expand_file(char *file, char *fname);
#endif

/*
 * Directory I/O
 */

struct sal_dirent {
    char	d_name[SAL_NAME_MAX];
};

typedef struct {
    int			is_netio;
    void		*dirp;		/* NETIO_DIR or DIR pointer */
    struct sal_dirent	de;
} SAL_DIR;

extern SAL_DIR *sal_opendir(char *dirName);
extern int sal_closedir(SAL_DIR *pDir);
extern struct sal_dirent *sal_readdir(SAL_DIR *pDir);
extern void sal_rewinddir(SAL_DIR *pDir);

/*
 * Command line editor I/O
 */

extern char* sal_readline(char *prompt, char *buf, int bufsize, char *defl);
extern void sal_readline_config(char *(*complete)(char *pathname, int *unique),
			    int (*list_possib)(char *pathname, char ***avp));
extern int sal_readchar(const char *prompt);

extern int sal_flash_init(int format);
extern int sal_flash_boot(char *file);
extern int sal_flash_sync(void);

/*
 * printk
 *
 *  Synchronized printing facility which allows multiplexing output
 *  to console and/or file.
 */

#ifdef __KERNEL__
/* unfortunately, these are already used :)
 * However, we need to make sure that the client
 * call's our printk, not the kernel's. 
 * 
 * Just redefine'em here for everyone...
 */
#if 0
#define printk sal_appl_printk
#define vprintk sal_appl_vprintk
#else
#include <linux/kernel.h>
#endif
#endif

extern int printk(const char *, ...)		/* To console and/or file */
		COMPILER_ATTRIBUTE ((format (printf, 1, 2)));
extern int vprintk(const char *, va_list ap);

extern int printk_cons_enable(int enable);		/* Set console output on/off */
extern int printk_cons_is_enabled(void);
extern int printk_cons(const char *, ...)		/* To console only */
		COMPILER_ATTRIBUTE ((format (printf, 1, 2)));

extern int printk_file_enable(int enable);		/* Set file output on/off */
extern int printk_file_is_enabled(void);
extern int printk_file_open(char *filename, int append);
						/* Initiate output to file */
extern int printk_file_close(void);		/* Terminate output to file */
extern int printk_file(const char *, ...)		/* To log file only */
		COMPILER_ATTRIBUTE ((format (printf, 1, 2)));
extern int vprintk_file(const char *, va_list ap);
extern char* printk_file_name(void);			/* Return current file name */

/*
 * debugk
 *
 *  Debug printing facility (also synchronized with locks like printk).
 *  Specify one or more debug message classes in the call.  The message is
 *  displayed only if the message class is enabled via debugk_select().
 *
 *  Use debugk_select to activate selected debug message classes.
 *  Use debugk_check to check if specified class(es) are currently active.
 *
 *  NOTE!! When adding here, add matching entry to dk_map[] in shell.c.
 */


/* NOTE -- The actual debug flags have been moved to <soc/debug.h> */

extern uint32 debugk_select(uint32 dk);
extern uint32 debugk_enable(uint32 dk);
extern uint32 debugk_disable(uint32 dk);
extern int debugk_check(uint32 dk);
extern int debugk(uint32 dk, const char *, ...) COMPILER_ATTRIBUTE ((format (printf, 2, 3)));
extern int vdebugk(uint32 dk, const char* fmt, va_list varg);

/*
 * Some of our BSPs have routines have a sysSerialPrintf routine that
 * allows the console serial port to be written directly in polled mode
 * with interrupts turned off.  When this is available, it is preferred
 * over logMsg.
 */

#if defined(MOUSSE) || defined(BMW) || defined(MBZ) || \
    defined(IDTRP334) || defined(IDT438) || defined(NSX) || \
    defined(ROBO_4704) || defined(ROBO_4702) || defined(METROCORE)
#define HAVE_DIRECT_SERIAL
#endif

#ifdef HAVE_DIRECT_SERIAL
extern void sysSerialPutc(int c);
extern int sysSerialGetc(void);
extern void sysSerialPrintString(char *s);
extern void sysSerialPrintHex(uint32 value, int cr);
#endif /* HAVE_DIRECT_SERIAL */

#endif /* NO_SAL_APPL */

#endif	/* !_SAL_IO_H */
