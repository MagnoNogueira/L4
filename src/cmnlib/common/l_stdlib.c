#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sys_adpt.h"
#include "sys_type.h"
#include "l_stdlib.h"

/*******************************************************
 * Submodule: 64 bits binary serial
 *******************************************************/
/* NAMING CONSTANT
 */
#define X   0x10000L
#define Y   10000

enum
{
    UPPER_BOUND,
    LOWER_BOUND
};

/* LOCAL FUNCTION DECLARATION
 */
static void UI64_toa4 (UI32_T v, char s4[4], char leading);
static void UI64_Multi_Y ( UI32_T sum[5], UI32_T a[5], UI32_T b);
static void UI64_Add_Y( UI32_T sum[5], UI32_T a[5]);
static void UI64_to_Y ( UI32_T vh, UI32_T vl, UI32_T sum[5] );

/* These function use for 64 bit multi/devide */
static int UI64_Multi_B16 (UI16_T d[4], UI32_T m);
static int UI64_Add_B16 (UI16_T d[4], UI16_T s[4]);

#if 0 /* Penny,2004-1-9: Due to no one use this API, disable it for not showing warning */
static int UI64_Sub_B16 (UI16_T d[4], UI16_T s[4]);
#endif
/* removed using Mercury_V2-00185. Since they are defined in .h file to export
static BOOL_T L_STDLIB_IsAsciiPrint ( int c);
static BOOL_T L_STDLIB_IsDigit ( int c);
*/

static char* L_STDLIB_ExtractVersionNum(char* str_p, BOOL_T* is_wildcard, UI8_T *version_no_p);
static BOOL_T L_STDLIB_ValidateVerStr(char* str_p);
static BOOL_T L_STDLIB_VersionBoundaryCheck(char* boundary_ver_str, char* check_ver_str, UI8_T boundary_type);

/* EXPORTED FUNCTION BODY
 */
/* Purpose: convert 64 bit binary value to 10^4 value
 * Input: vh      --- hight word ( bit 32 - bit 63 )
 *        vl      --- Low word ( bit 0 - bit 31 )
 * Output:  str   --- decimal character string,
 * Note1: the output string is always 20 characters, it is a leading
 *        blank digit string.
 * Note2: do not append null value to str
 *
 */
void L_STDLIB_UI64toa ( UI32_T vh, UI32_T vl, char str[20] )
{
    /* LOCAL VARIABLES
     */
    UI32_T sum[5];
    int    i;
    char   leading=' ';      /* default: leading blank */
    char  *p;

    UI64_to_Y (vh, vl, sum);

    /* convert to string */

    for (p=str, i=4; i>=0; i--, p+=4)
    {
        UI64_toa4(sum[i], p, leading );
        if ( sum[i] )
            leading='0';
        /* end if */

    } /* end for */

    if ( leading == ' ')
        str[19]='0';
    /* end if */
} /* L_STDLIB_UI64toa */

void L_STDLIB_UI64toa_Extended ( UI64_T v, char str[20] )
{
    L_STDLIB_UI64toa (L_STDLIB_UI64_H32(v), L_STDLIB_UI64_L32(v), str);
}


/* Purpose: 64 bit unsigned binary addition
 * Input:  h1, l1, h2, l2
 * Output: h1, l1
 * Remark:  (h1, l1) += (h2, l2);
 */
void L_STDLIB_UI64_Add ( UI32_T *h1, UI32_T *l1,  UI32_T h2, UI32_T l2)
{
    *l1 += l2;
    if ( *l1 < l2 ) h2++;
    *h1 += h2;
} /* End of L_STDLIB_UI64_Add */



/* Purpose: 64 bit unsigned binary addition
 * Input:  h1, l1, h2, l2
 * Output: h1, l1
 * Remark:  (h1, l1) -= (h2, l2);
 */
void L_STDLIB_UI64_Sub ( UI32_T *h1, UI32_T *l1,  UI32_T h2, UI32_T l2)
{
    if ( *l1 < l2 ) h2++;
    *l1 -= l2;
    *h1 -= h2;
} /* End of L_STDLIB_UI64_Add */



/* Purpose: (h,l) *= m;
 * Input:   h, l        --- 64 bit binary
 *          m
 * Output:  (h,l)
 * Return: 0: not overflow,  else:overflow
 */
int L_STDLIB_UI64_Multi (UI32_T *h, UI32_T *l, UI32_T m)
{
    /* Local Variable
     */
    int         c;
    UI16_T d[4];     /* [0]:bit 0~15, [3]:bit 48~63 */

    /* Body
     */
    /* (h, l) -> d
     */
    d[0] = (UI16_T) (*l);
    d[1] = (UI16_T) ((*l) >> 16);
    d[2] = (UI16_T) (*h);
    d[3] = (UI16_T) ((*h) >> 16);
    c = UI64_Multi_B16 (d, m);

    *l = d[0] + (d[1] << 16);
    *h = d[2] + (d[3] << 16);

    return c;
} /* L_STDLIB_UI64_Multi */


/* Purpose: (h,l) = f*q + r
 * Input:   (h,l)            --- 64 bit binary, h: bit 32~63, l:bit 0~31
 *          f
 * Output:  (h,l)=q
 * Return: r
 */
UI32_T L_STDLIB_UI64_Div (UI32_T *h, UI32_T *l, I32_T f)
{
    /* Local Variable
     */
    UI32_T q[2];    /* 64 bit binary, m[0]: bit 32~63, m[1]:bit 0~31 */
    UI32_T areg = 0;
    I32_T  sl;
    UI32_T n, i, j, c;

    /* Body
     */
    if ( f == 0 ) return 0;       /* Divide Zero */

    /* Initial local variables
     */
    q[0]=q[1]=0;

    /* normalize f
     */
    for ( n=0; n<32; n++)
    {
       if ( f < 0 ) break;
       f<<=1;
    }

    /* f shift right i times
     */

    /* Covert to Signed Binary
     */

    areg = *h,  sl = (I32_T) *l;
    for ( c=j=0, i=1; ; i++)
    {
       if (i==n+2)  j=1;

       q[j] = q[j]<<1;

       if ( c || areg >= (UI32_T) f )
       {
          c=0;
          areg-=f;
          q[j] |= 1;
       }
       if ( areg >= 0x80000000 ) c=1;

       if ( i == (32+n+1) ) break;

       areg = (areg<<1) | (sl<0);
       sl = sl << 1;
    } /* end for */

    *h = q[0];
    *l = q[1];
    return (areg>>n);
} /* End of L_STDLIB_UI64_Div */

/*----------------------------------------------------------------------------------
 * Function name: L_STDLIB_StringN_To_Lower
 *----------------------------------------------------------------------------------
 * Purpose: Convert string to lower case
 * Input  : string_buffer -- the string to  be converted.
 *          converting_string_lenth -- the char. to be converted;
 *                                     n -- only n char. be scanned and convertted.
 * Output : string_buffer -- convertted string buffer
 * Return : the amount of char. scanned.
 * Note   : 1. Only convert printable char. to lower case.
 *
 */
int L_STDLIB_StringN_To_Lower(UI8_T *string_buffer,  UI32_T convert_string_length)
{
    UI32_T str_len = 0;
    UI32_T i = 0;
    str_len = strlen((char *)string_buffer);

    if(convert_string_length>str_len)
    {
        convert_string_length = str_len;
    }

    for(i=0;i<convert_string_length;i++)
    {
        string_buffer[i] = (UI8_T)tolower(string_buffer[i]);
    }

    return convert_string_length;
} /* L_STDLIB_StringN_To_Lower */

/* PURPOSE: Return true if str_p is a particular representation of a printable
 *          character string (0x20-0x7E, except some characters).
 * INPUT:   str_p   - pointer of input string (ASCIZ format)
 * OUTPUT:  none
 * RETURN:  return TRUE if str_p is a printable character string
 */

BOOL_T L_STDLIB_StrIsAsciiPrint ( char *str_p )
{
    /* This defines exception characters used in ASCII string check
     * in function "L_STDLIB_StrIsAsciiPrint".
     * It can be an empty string or one or more characters
     */
    /* LOCAL VARIABLES
     */
     int c;

    for ( ;(c=*str_p) !=0; str_p++)
    {
       if ( !L_STDLIB_IsAsciiPrint(c)
                 || (strchr(SYS_ADPT_CMNLIB_ASCII_PRINT_EXCEPT_CHAR,c) != NULL))
           return FALSE;
    }
    return TRUE;
} /* L_STDLIB_StrIsPrint */


/* PURPOSE: Return true if c is a printable character (0x20-0x7E).
 * INPUT:   c - input char (OCTET format)
 * OUTPUT:  none
 * RETURN:  return TRUE if c is a printable character
 */
BOOL_T L_STDLIB_IsAsciiPrint ( int c)
{
    return ((c >= 0x20) && (c <= 0x7e));
}

/* PURPOSE: Return true if c is a printable decimal character.
 * INPUT:   c   - input char (ASCIZ format)
 * OUTPUT:  none
 * RETURN:  return TRUE if c is a printable decimal character
 */
BOOL_T L_STDLIB_IsDigit ( int c)
{
    return ((c>='0') && (c <= '9'));
}

/* PURPOSE  : Return true if there is special char included in the name string
 * INPUT    :  *p -- which point to the name string
                     c--special char
 * OUTPUT   : BOOL value
 * RETURN   : TRUE/FALSEds
 * NOTES    : none
 *-----------------------------------------------------------------------------
*/
/*
  *EPR:ES4827G-FLF-ZZ-00233
   Part of EPR:ES4827G-FLF-22-00232
   EPR:ES4827G-FLF-ZZ-00096
  *Problem:if name string contain special char ':', " " or"?",the string will be cut
  *RootCause:when special char entered into the name string,token number will be added,which will course type check problem
  *Solution:we should do name checking at the stage of name setting,so here we add such function.
  *File:L_stdlib.c,L_stdlib..h,Cli_api_vlan.c,Rfc_2863.c,Es3626a_superset2.0.c
  *ccb_approve:Hard.Sun
*/
BOOL_T L_STDLIB_IsSpecialName(char *p,char c)
{
  if(p == NULL) return TRUE;
  while(*p != '\0')
  {
    if (*p == c)
      return TRUE;
    p++;
  }
  return FALSE;
}


/* PURPOSE: Return true if oct_p is a particular representation of a printable
 *          character string (0x20-0x7E).
 * INPUT:   oct_p   - pointer of input string (OCTET format)
 *          len     - length of the octet
 * OUTPUT:  none
 * RETURN:  return TRUE if oct_p is a printable character octet string
 */
BOOL_T L_STDLIB_StrIsAsciiPrintWithCount ( char *oct_p, int len )
{
    /* LOCAL VARIABLES
     */
     int i;

    for ( i=0; i<len; i++)
    {
       if ( !L_STDLIB_IsAsciiPrint((int)oct_p[i]))
           return FALSE;
    }
    return TRUE;
} /* L_STDLIB_StrIsAsciiPrintWithCount */


/* PURPOSE: Return true if str_p is a particular representation of a decimal
 *          character string ('0'-'9').
 * INPUT:   str_p   - pointer of input string (ASCIZ format)
 * OUTPUT:  none
 * RETURN:  return TRUE if str_p is a decimal character string
 */
BOOL_T L_STDLIB_StrIsDigit ( char *str_p )
{
    /* LOCAL VARIABLES
     */
     int c;

    for ( ;(c=*str_p) !=0; str_p++)
    {
       if ( !L_STDLIB_IsDigit(c) )
           return FALSE;
    }
    return TRUE;
} /* L_STDLIB_StrIsDigit */


/* Function Name: L_STDLIB_SnmpStrcmp
 * Purpose      :Compare strings.
 *               The return value for each of these functions
 *               indicates the lexicographic relation of str1 to str2.
 *               similar to "strcmp", However, the difference is:
 *               L_STDLIB_SnmpStrcmp compare the length first,
 *               and then compare the content of the string
 *               but strcmp not compare the length
 *               for example, compare "abc" and "z",
 *               strcmp will return -1, but L_STDLIB_SnmpStrcmp will return 1.
 *
 *               -1: str1 less than str2
 *               0: str1 identical to str2
 *               1: str1 greater than str2
 *
 * First Created: Phoebe, 2003/9/30
 */
I32_T L_STDLIB_SnmpStrcmp(char *str1, char *str2)
{
    if (strlen(str1) > strlen(str2))
        return 1;
    else if(strlen(str1) < strlen(str2))
        return -1;

    return strcmp(str1, str2);
}

/* LOCAL FUNCTION BODY
 */
/* Purpose: conver v to digital string(fix length:4).
 * Input    v        --- binary that < 10000
 *          leading  --- replace leading zero whith character leading.
 * Output   s[4]     --- result digit string
 */
static void UI64_toa4 (UI32_T v, char s4[4], char leading)
{
    int   len, lead_cnt;
    char  s[40];

    /* Convert an integer to a string. */

    if ( v==0 )
    {
       memset ( s4, leading, 4);
       return;
    }

    sprintf (s, "%d", (int)v);
    /*
     * itoa( (int)v, s, 10);
     */

    if ( (len=strlen(s))  > 4 ) return;
    lead_cnt = 4-len;

    memmove ( s4+4-len, s, len);

    if ( lead_cnt )
       memset ( s4, leading, lead_cnt);
    /* end if */
} /* UI64_toa4 */

/* Purpose: sum[i] = a[i] * b;
 * Input    a[]      --- Y base binary array
 *          b
 * Output   sum[5]   --- Y base binary array
 */
static void UI64_Multi_Y ( UI32_T sum[5], UI32_T a[5], UI32_T b)
{
    int i, c;
    UI32_T reg;

    for ( c=i=0; i<5; i++)
    {
        reg = c+a[i]*b;
        c = reg / Y;
        sum[i] = reg % Y;
    }
} /* UI64_Multi_Y */




/* Purpose: sum[i] += a[i];
 * Input    sum[], a[]        --- Y base binary array
 * Output   sum[5]            --- Y base binary array
 */
static void UI64_Add_Y( UI32_T sum[5], UI32_T a[5])
{
    int i, c;
    UI32_T reg;

    for ( c=i=0; i<5; i++)
    {
        reg = c+sum[i]+a[i];
        c = reg / Y;
        sum[i] = reg % Y;
    }
} /* UI64_Add_Y */



/* Purpose: convert 64 bit binary value to 10^4 value
 * Input: vh      --- hight word ( bit 32 - bit 63 )
 *        vl      --- Low word ( bit 0 - bit 31 )
 * Output:  sum   --- ouput = sum[5].Y^4 + ... +sum[1].Y + sum[0]
 *                --- where Y=10000
 */
static void UI64_to_Y ( UI32_T vh, UI32_T vl, UI32_T sum[5] )
{
    UI32_T xreg[5], wreg[5];
    UI32_T a[4];
    int      i;
    /* Let x = 2^16, y=10^8
     * v=(vh, vl) = a3.x^3 + a2.x^2 + a1.x + a0 = b2.y^2 + b1.y + b0
     *  (a3, a2, a1, a0)x = (b2, b1, b0)y
     */


    /* prepare a[i] */
    a[0] = vl & 0xFFFF;
    a[1] = vl >> 16;
    a[2] = vh & 0xFFFF;
    a[3] = vh >> 16;


    memset(xreg+1, 0, sizeof(UI32_T)*4);
    xreg [0] = 1;

    memset(sum, 0, sizeof(UI32_T)*5);

    for (i=0; i<=3; i++)
    {
        UI64_Multi_Y (wreg, xreg, a[i]);
        UI64_Add_Y (sum, wreg);
        UI64_Multi_Y (xreg, xreg, X);
    }

    return;

} /* UI64_to_Y */


/* These functions use for 64 bit multi/devide */

/* Purpose: d[] *= m;
 * Input:   d[]        --- 4 16-bit binary arrary
 *          m
 * Output:  d[]
 * Return: 0: not overflow,   else:overflow
 */
static int UI64_Multi_B16 (UI16_T d[4], UI32_T m)
{
    /* Local Variable
     */
    UI16_T  d1[4];   /* [0]:bit 0~15, [3]:bit 48~63 */
    UI32_T a, b, c;
    int i;

    /* Body
     */

    if ( m > 0x0FFFF )
    {
       /* let m = a.2^16 + b,
        *  d.m = d(a.2^16+b) = d.a.2^16 + d.b
        */
       a = m >> 16;
       b = m & 0x0FFFF;

       /* d1=d.2^16 */
       d1[1] = d[0];  d1[2] = d[1];  d1[3] = d[2];
       d1[0] =0;
       UI64_Multi_B16 (d1, a);          /* d1 = d.2^16.a */
       UI64_Multi_B16 (d, b);           /* d = d.b */
       return UI64_Add_B16 (d, d1);     /* d = d + d1 */
    }

    for ( c=i=0; i<4; i++)
    {
       a = (UI32_T)d[i] * m +c;
       c = a >> 16;
       d[i] = (UI16_T) a;
    }
    return c;
} /* End of UI64_Multi_B16 */



/* Purpose: d[] += s;
 * Input:   d[], s[]      --- 4 16-bit binary arrary
 * Output:  d[]
 * Return: 0: not overflow,   else:overflow
 */
static int UI64_Add_B16 (UI16_T d[4], UI16_T s[4])
{
    /* Local Variable
     */
    int a, c;
    int i;

    /* Body
     */

    for ( c=i=0; i<4; i++)
    {
        c = (a = (UI32_T)d[i] +(UI32_T)s[i] + c)  > 0x0FFFF;
        d[i] = (UI16_T) a;
    }
    return c;
} /* End of UI64_Add_B16 */



#if 0 /* Penny,2004-1-9: Due to no one use this API, disable it for not showing warning */
/* Purpose: d[] -= s;
 * Input:   d[], s[]      --- 4 16-bit binary arrary
 * Output:  d[]
 * Return: 0: not overflow,   else:overflow
 */
static int UI64_Sub_B16 (UI16_T d[4], UI16_T s[4])
{
    /* Local Variable */
    int  c, c1;
    int i;

    /* Body */

    for ( c=i=0; i<4; i++)
    {
        c1 = d[i] < s[i];
        d[i] -= (s[i] +c);
        c = c1;
    }
    return c;
} /* End of UI64_Sub_B16 */
#endif

/* FUNCTION NAME: L_STDLIB_ExtractVersionNum
 * PURPOSE: This function will extrac the version number from the input string
 *          and return the pointer to the delimiter or end of the string.
 *
 * INPUT:   str_p       --  version string
 * OUTPUT:  is_wildcard --  TRUE  if str_p is wildcard.
 *                          FALSE if str_p is a version number
 *          version_no_p--  The version number in str_p(when is_wildcard==FALSE)
 *
 * RETUEN:  The pointer to the delimiter or end of the str_p.
 * NOTES:   None.
 */
static char* L_STDLIB_ExtractVersionNum(char* str_p, BOOL_T* is_wildcard, UI8_T *version_no_p)
{
    char* cursor_p=str_p;
    char org;

    while((*cursor_p >= '0') && (*cursor_p <='9'))
        cursor_p++;

    if(*cursor_p=='*')
    {
        *is_wildcard=TRUE;
        cursor_p++;
    }
    else
    {
        *is_wildcard=FALSE;
        org=*cursor_p;
        *cursor_p=0;
        *version_no_p = (UI8_T)atoi(str_p);
        *cursor_p=org;
    }

    return cursor_p;
}

/* FUNCTION NAME: L_STDLIB_ValidateVerStr
 * PURPOSE: This function will check whether the input string is a valid version string.
 *
 * INPUT:   str_p  --  version string to be validated
 * OUTPUT:  None.
 * RETUEN:  TRUE   --  valid version string.
 *          FALSE  --  invalid version string.
 * NOTES:   1. This function is used by L_STDLIB_VersionRangeCheck() to validate version
 *             string.
 */
static BOOL_T L_STDLIB_ValidateVerStr(char* str_p)
{
    BOOL_T is_delimiter=FALSE;

    while(*str_p!=0)
    {
        /* wildcard char '*' can only be at the last character of the
         * version string
         */
        if(*str_p == '*')
        {
            if(str_p[1] == 0)
                return TRUE;
            else
                return FALSE;
        }

        if(L_STDLIB_IsDigit(*str_p)==FALSE)
        {
            if(*str_p=='.' || *str_p=='-')
            {
                if(is_delimiter==TRUE)
                {
                    /* no digits between two delimiters
                     */
                    return FALSE;
                }
                else
                {
                    is_delimiter=TRUE;
                }
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            is_delimiter=FALSE;
        }

        str_p++;
    }

    return TRUE;
}

/* FUNCTION NAME: L_STDLIB_VersionBoundaryCheck
 * PURPOSE: This function will compare the numbers in boundary_ver_str and
 *          check_ver_str to see whether the check_ver_str is within the
 *          boundary.
 *
 * INPUT:   boundary_ver_str --  The version string to specify boundary
 *          check_ver_str    --  The version string to be checked
 *          boundary_type    --  UPPER_BOUND: boundary_ver_str is an upper bound.
 *                               LOWER_BOUND: boundary_ver_str is an lower bound.
 *
 * OUTPUT:  None.
 * RETUEN:  TRUE  --  The check_ver_str is within the specified boundary
 *          FALSE --  The check_ver_str is out of the specified boundary
 * NOTES:
 *          1.boundary_ver_str is allowed to contain '*' to indicate that no
 *            boundary restriction thereafter.
 *          2.If the count of version number in check_ver_str is less than
 *            the one in boundary_ver_str, this function always return FALSE.
 *          3.If the count of version number in check_ver str is greater than
 *            the one in boundary_ver_str and boundary_ver_str does not end
 *            with '*', this function always return FALSE.
 */
static BOOL_T L_STDLIB_VersionBoundaryCheck(char* boundary_ver_str, char* check_ver_str, UI8_T boundary_type)
{
    char* delimiter_p;
    UI8_T boundary_version, check_version;
    BOOL_T is_wildcard;

    while(check_ver_str[0] != 0 && boundary_ver_str[0]!=0)
    {
        /* extract boundary_ver_str
         */
        delimiter_p = L_STDLIB_ExtractVersionNum(boundary_ver_str, &is_wildcard, &boundary_version);
        if(is_wildcard == TRUE)
        {
            /* wildcard, need not to check the version number behind
             */
            return TRUE;
        }
        /* move boundary_ver_str to the start of next number
         */
        boundary_ver_str=delimiter_p+1;

        /* extract check_ver_str
         */
        delimiter_p = L_STDLIB_ExtractVersionNum(check_ver_str, &is_wildcard, &check_version);
        if(is_wildcard == TRUE)
        {
            /* check_ver_str should not contain wildcard
             */
            return FALSE;
        }

        /* move check_ver_str to the start of next number
         */
        check_ver_str=delimiter_p+1;

        /* compare extracted number
         */
        if(boundary_type==LOWER_BOUND)
        {
            if(check_version>boundary_version)
            {
               /* larger than lower bound, need not to check the version number behind
                */
                return TRUE;
            }
            else if(check_version<boundary_version)
            {
                return FALSE;
            }
        }
        else /* UPPER_BOUND check */
        {
            if(check_version<boundary_version)
            {
               /* less than upper bound, need not to check the version number behind
                */
                return TRUE;
            }
            else if(check_version>boundary_version)
            {
                return FALSE;
            }
        }

    }

    /* boundary_ver_str which contains '*' would had been returned above
     * whether the check_ver_str and boundary_ver_str reaches the end of the string
     * at the same time? If not, that means the count of version number in
     * check_ver_str and boundary_ver_str doesn't match, FALSE will be returned in
     * this case.
     */
    return (check_ver_str[-1]==0 && boundary_ver_str[-1]==0) ? TRUE: FALSE;
}

/*******************************************************
 * Submodule: String
 *******************************************************/
/* Purpose: Trim the leading blank
 * return length of str
 */
int L_STDLIB_Trim_Left(char *str, int str_len)
{
    int i, len;
    char *p;

    /* Get 1st non-blank position */
    p = str;
    for(i=0; i<str_len; i++,p++)
    {
        if (*p != ' ') break;
    }

    len = str_len - i;
    memmove(str, p, len);

    if ( len < str_len)
        str[len] = 0;
    return   len;
} /* L_STDLIB_Trim_Left */

/* FUNCTION NAME: L_STDLIB_VersionRangeCheck
 * PURPOSE: This function is used to check that whether the given version falls in the range
 *          between the start version and the end version (start version and the end version
 *          are also included).
 *
 * INPUT:   start_ver_str  --  The start version string (lower bound)
 *          end_ver_str    --  The end version string   (upper bound)
 *          check_ver_str  --  The check version string
 *
 * OUTPUT:  None.
 * RETUEN:  TRUE  --  The check_ver_str falls in the range between start_ver_str and end_ver_str.
 *          FALSE --  The check_ver_str does not fall in the specified range.
 * NOTES:
 *       1. The version string must be all digits separated by "." or "-"
 *          (e.g. 1.2.1.1 or 1.2.1.1-1.0)
 *       2. The maximum value of the number, whcih is separated by "." or "-"
 *          is 99. The minimum value of the number is 0.
 *       3. The first number in the version string is the most significant
 *          number and the last number in the version string is the least
 *          significant number.
 *          For example, if "start_ver_str" is "1.1" and end_ver_str is "3.2",
 *          then the version within the range would be "1.1" to "1.99",
 *          "2.0" to "2.99" and "3.0" to "3.2".
 *       3. The version string in start_ver_str and end_ver_str may contain a single "*" to
 *          represent no restriction after the specified position. A "*" in start_ver_str
 *          means no lower bound. A "*" in end_ver_str means no upper bound. "*" can only appear
 *          at the end of the version string and it indicates no restriction from and after that
 *          position.
 *          For example, given that "start_ver_str" is "2.5" and "end_ver_str" is "2.*". The function
 *          will return TRUE when the first number in check_ver_str is larger or equal to 2.
 *          Given that "start_ver_str" is "2.*" and "end_ver_str" is "2.5" The version
 *          within the range would be "2.0" to "2.5". The count of the number in
 *          check_ver_str can be greater than the number appeared in start_ver_str and
 *          end_ver_str. Follow the previous example, "2.0.0" to "2.5.99" is also
 *          in the specified range.
 *       4. If the count of number in check_ver_str is less than those in start_ver_str or
 *          end_ver_str, this function will always return FALSE.
 */
BOOL_T L_STDLIB_VersionRangeCheck(char* start_ver_str, char* end_ver_str, char* check_ver_str)
{
    /* validate start_ver_str
     */
    if(L_STDLIB_ValidateVerStr(start_ver_str)==FALSE)
        return FALSE;

    /* validate end_ver_str
     */
    if(L_STDLIB_ValidateVerStr(end_ver_str)==FALSE)
        return FALSE;

    /* check start_ver_str (lower bound)
     */
    if(L_STDLIB_VersionBoundaryCheck(start_ver_str, check_ver_str, LOWER_BOUND)==FALSE)
        return FALSE;

    /* check end_ver_str (upper bound)
     */
    return L_STDLIB_VersionBoundaryCheck(end_ver_str, check_ver_str, UPPER_BOUND);
}

#ifdef STDLIB_DEBUG
#include <stdio.h>
main()
{
    UI32_T len;
    UI64_T v;
    static char   str[21];
    static char   in_str[80];

    while (1)
    {
        printf ("input v:");
        gets(in_str);
        if ( in_str[0] == '0' )
           break;
        v = _atoi64( in_str );
        L_STDLIB_UI64toa_1 ( v, str );
        len = L_STDLIB_Trim_Left(str, 20);
        printf("converted str: %s\n\n", str);
    }
}
#endif

