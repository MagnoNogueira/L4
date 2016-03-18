/****************************************************************************/
/*   The following routine is coming from tomcat 						    */
/****************************************************************************/
#include <sys_type.h>
#include <string.h>
#include "cli_func.h"

 

/* ---------------------------------------------------------------------------------- */
/* local subroutines for test & set functions                                         */
/* ---------------------------------------------------------------------------------- */
void CLI_FUNC_reverse(UI8_T *buf)
{
   int	c, i, j;
   for (i = 0, j = strlen((char *)buf)-1; i < j; i++, j--)
   {
      c = buf[i];
      buf[i] = buf[j];
      buf[j] = c;
   }
}


/* -------------------------------------------------- */
/* Transform 0x???????? IP address to xxx.xxx.xxx.xxx */
/* -------------------------------------------------- */
UI8_T CLI_FUNC_ipHextoStr(UI32_T hip, UI8_T *buf)
{
   UI8_T     *ipp, *buf_ptr;
   UI16_T    i;
   UI8_T     len;

   ipp = (UI8_T *)&hip;
   buf_ptr = buf;
   for (i=0; i<4; i++)
   {
      len = CLI_FUNC_UI8_TtoStr(ipp[i], buf_ptr);
      buf_ptr += len;     /* advance buffer pointer */
      *buf_ptr = '.';
      buf_ptr++;
   }
   *(buf_ptr-1) = '\0';    /* delete the last '.' */
   return 0;
}



/* Transform integer n to a string */

UI8_T CLI_FUNC_UI32_TtoStr(UI32_T n, UI8_T *buf)
{
   int i = 0;
   do
   {
      buf[i++] = n % 10 + '0';
   } while ((n /= 10) > 0);

   buf[i] = '\0';
   CLI_FUNC_reverse(buf);
   return i;                   /* return value is the length of UI32_T integer */
}


UI8_T CLI_FUNC_UI8_TtoStr(UI8_T n, UI8_T *buf)
{
   int i = 0;
   do
   {
      buf[i++] = n % 10 + '0';
   } while ((n /= 10) > 0);

   buf[i] = '\0';
   CLI_FUNC_reverse(buf);
   return i;                   /* return value is the length of UI32_T integer */
}

