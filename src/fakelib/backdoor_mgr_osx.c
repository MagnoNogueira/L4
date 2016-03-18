#include <stdio.h>
#include <sgtty.h>
#include <unistd.h> // read()

static struct sgttyb savemodes;
static int havemodes = 0;

int tty_break()
{
	struct sgttyb modmodes;
	if(ioctl(fileno(stdin), TIOCGETP, &savemodes) < 0)
		return -1;
	havemodes = 1;
	modmodes = savemodes;
	modmodes.sg_flags |= CBREAK;
	return ioctl(fileno(stdin), TIOCSETN, &modmodes);
}

int tty_getchar()
{
	return getchar();
}

// read a single character from the keyboard without echo
int tty_getch()
{
    unsigned char ch;
    while (read(fileno(stdin), &ch, 1) != 1);
    
    return ch;
}

int tty_fix()
{
	if(!havemodes)
		return 0;
	return ioctl(fileno(stdin), TIOCSETN, &savemodes);
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
BOOL_T BACKDOOR_MGR_RequestKeyIn_OSX(char *key_in_string, UI32_T max_key_len)
{
    if (0 == max_key_len)
    {
        return TRUE;
    }

    if (NULL == key_in_string)
    {
        return FALSE;
    }

    if (1 == max_key_len)
    {
        key_in_string[0] = '\0';
        return TRUE;
    }
    
    if( fgets( key_in_string, max_key_len, stdin ) == NULL )
    {
        return FALSE;
    }
    fflush(stdin);
    key_in_string[max_key_len-1] = '\0';
    
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
BOOL_T BACKDOOR_MGR_Printf_OSX(char *fmt, va_list args)
{
    vprintf(fmt, args);
    return TRUE;
}

