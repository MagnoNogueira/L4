#ifndef L_STDLIB_H
#define L_STDLIB_H


/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_hwcfg.h"

/* EXPORTED MACRO FUNCTIONS DECLARATION
 */
#define L_STDLIB_Swap32(addr)     ( (((UI32_T)(addr) & 0x000000FF)<<24) | \
                                   (((UI32_T)(addr) & 0x0000FF00)<<8)  | \
                                   (((UI32_T)(addr) & 0x00FF0000)>>8)  | \
                                   (((UI32_T)(addr) & 0xFF000000)>>24))

#define L_STDLIB_Swap16(addr)     ( (((UI16_T)(addr) & 0x00FF)<<8)  | \
                                   (((UI16_T)(addr) & 0xFF00)>>8))

#define L_STDLIB_Swap64(addr)     ( (((UI64_T)(addr) & 0x000000FFULL)<<56) | \
                                  (((UI64_T)(addr) & 0x0000FF00ULL)<<40)  | \
                                  (((UI64_T)(addr) & 0x00FF0000ULL)<<24)  | \
                                  (((UI64_T)(addr) & 0xFF000000ULL)<<8)   | \
                                  (((UI64_T)(addr) & 0xFF00000000ULL)>>8) | \
                                  (((UI64_T)(addr) & 0xFF0000000000ULL)>>24) | \
                                  (((UI64_T)(addr) & 0xFF000000000000ULL)>>40) | \
                                  (((UI64_T)(addr) & 0xFF00000000000000ULL)>>56))

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/* Purpose: convert 64 bit integer to string
 * Input: v       --- 64 bit unsigned integer
 * Output:  str   --- decimal character string,
 * Note1: the output string is always 20 characters, it is a leading
 *        blank digit string.
 * Note2: do not append null value to str
 *
 */
void L_STDLIB_UI64toa_Extended ( UI64_T v, char str[20] );


/* Purpose: convert 64 bit integer to string
 * Input: vh      --- hight word ( bit 32 - bit 63 )
 *        vl      --- Low word ( bit 0 - bit 31 )
 * Output:  str   --- decimal character string,
 * Note1: the output string is always 20 characters, it is a leading
 *        blank digit string.
 * Note2: do not append null value to str
 *
 */
void L_STDLIB_UI64toa ( UI32_T vh, UI32_T vl, char str[20] );


/* Purpose: 64 bit unsigned binary addition
 * Input:  h1, l1, h2, l2
 * Output: h1, l1
 * Remark:  (h1, l1) += (h2, l2);
 */
void L_STDLIB_UI64_Add ( UI32_T *h1, UI32_T *l1,  UI32_T h2, UI32_T l2);


/* Purpose: 64 bit unsigned binary addition
 * Input:  h1, l1, h2, l2
 * Output: h1, l1
 * Remark:  (h1, l1) -= (h2, l2);
 */
void L_STDLIB_UI64_Sub ( UI32_T *h1, UI32_T *l1,  UI32_T h2, UI32_T l2);




/* Purpose: (h,l) *= m;
 * Input:   h, l        --- 64 bit binary
 *          m
 * Output:  (h,l)
 * Return: 0: not overflow,  else:overflow
 */
int L_STDLIB_UI64_Multi (UI32_T *h, UI32_T *l, UI32_T m);


/* Purpose: (h,l) = f*q + r
 * Input:   (h,l)            --- 64 bit binary, h: bit 32~63, l:bit 0~31
 *          f
 * Output:  (h,l)=q
 * Return: r
 */
UI32_T L_STDLIB_UI64_Div (UI32_T *h, UI32_T *l, I32_T f);


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
int L_STDLIB_StringN_To_Lower(UI8_T *string_buffer,  UI32_T convert_string_length);


/* PURPOSE: Return true if str_p is a particular representation of a printable
 *          character string (0x20-0x7E, except some characters).
 * INPUT:   str_p   - pointer of input string (ASCIZ format)
 * OUTPUT:  none
 * RETURN:  return TRUE if str_p is a printable character string
 */

BOOL_T L_STDLIB_StrIsAsciiPrint ( char *str_p );

/* PURPOSE: Return true if oct_p is a particular representation of a printable
 *          character string (0x20-0x7E).
 * INPUT:   oct_p   - pointer of input string (OCTET format)
 *          len     - length of the octet
 * OUTPUT:  none
 * RETURN:  return TRUE if oct_p is a printable character octet string
 */

BOOL_T L_STDLIB_StrIsAsciiPrintWithCount ( char *oct_p, int len );

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
I32_T L_STDLIB_SnmpStrcmp(char *str1, char *str2);

/*******************************************************
 * Submodule: String
 *******************************************************/
/* Purpose: Trim left blanks & result an ASCIZ string
 * Input:   str
 * Output:  str
 * Return length of str
 * Note: if return length equal len then no '\0' append to
 *       the output string.
 */
int L_STDLIB_Trim_Left(char *str, int len);



#if (SYS_HWCFG_LITTLE_ENDIAN_CPU==TRUE)

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Hton16
 *------------------------------------------------------------------------
 * FUNCTION: Convert 16-bit integer from host byte order to network byte order
 * INPUT   : 16-bit integer in host byte order (usable as integer)
 * OUTPUT  : None
 * RETURN  : 16-bit integer in network byte order (big-endian)
 * NOTE    :
 *------------------------------------------------------------------------*/

#define  L_STDLIB_Hton16(host_int)  (L_STDLIB_Swap16((UI16_T)host_int))

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Ntoh16
 *------------------------------------------------------------------------
 * FUNCTION: Convert 16-bit integer from network byte order to host byte order
 * INPUT   : 16-bit integer in network byte order (big-endian)
 * OUTPUT  : None
 * RETURN  : 16-bit integer in host byte order (usable as integer)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_Ntoh16(network_int)  (L_STDLIB_Swap16((UI16_T)network_int))

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Hton32
 *------------------------------------------------------------------------
 * FUNCTION: Convert 32-bit integer from host byte order to network byte order
 * INPUT   : 32-bit integer in host byte order (usable as integer)
 * OUTPUT  : None
 * RETURN  : 32-bit integer in network byte order (big-endian)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_Hton32(host_int) (L_STDLIB_Swap32((UI32_T)host_int))

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Ntoh32
 *------------------------------------------------------------------------
 * FUNCTION: Convert 16-bit integer from network byte order to host byte order
 * INPUT   : 32-bit integer in network byte order (big-endian)
 * OUTPUT  : None
 * RETURN  : 32-bit integer in host byte order (usable as integer)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_Ntoh32(network_int) (L_STDLIB_Swap32((UI32_T)network_int))

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Hton64
 *------------------------------------------------------------------------
 * FUNCTION: Convert 64-bit integer from host byte order to network byte order
 * INPUT   : 64-bit integer in host byte order (usable as integer)
 * OUTPUT  : None
 * RETURN  : 64-bit integer in network byte order (big-endian)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_Hton64(host_int) (L_STDLIB_Swap64((UI64_T)host_int))

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Ntoh64
 *------------------------------------------------------------------------
 * FUNCTION: Convert 64-bit integer from network byte order to host byte order
 * INPUT   : 64-bit integer in network byte order (big-endian)
 * OUTPUT  : None
 * RETURN  : 64-bit integer in host byte order (usable as integer)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_Ntoh64(network_int) (L_STDLIB_Swap64((UI64_T)network_int))

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_UI64_L32
 *------------------------------------------------------------------------
 * FUNCTION: Get lower 32-bit value from a UI64_T variable
 * INPUT   : UI64_T value (big-endian)
 * OUTPUT  : None
 * RETURN  : 32-bit integer in host byte order(usable as integer)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_UI64_L32(val) (((UI32_T*)(void*)&(val))[0])

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_UI64_H32
 *------------------------------------------------------------------------
 * FUNCTION: Get higher 32-bit value from a UI64_T variable
 * INPUT   : UI64_T value (big-endian)
 * OUTPUT  : None
 * RETURN  : 32-bit integer in host byte order (usable as integer)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_UI64_H32(val) (((UI32_T*)(void*)&(val))[1])

#else
/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Hton16
 *------------------------------------------------------------------------
 * FUNCTION: Convert 16-bit integer from host byte order to network byte order
 * INPUT   : 16-bit integer in host byte order (usable as integer)
 * OUTPUT  : None
 * RETURN  : 16-bit integer in network byte order (big-endian)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define  L_STDLIB_Hton16(host_int)  ((UI16_T)(host_int))

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Ntoh16
 *------------------------------------------------------------------------
 * FUNCTION: Convert 16-bit integer from network byte order to host byte order
 * INPUT   : 16-bit integer in network byte order (big-endian)
 * OUTPUT  : None
 * RETURN  : 16-bit integer in host byte order (usable as integer)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_Ntoh16(network_int)  ((UI16_T)(network_int))


/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Hton32
 *------------------------------------------------------------------------
 * FUNCTION: Convert 32-bit integer from host byte order to network byte order
 * INPUT   : 32-bit integer in host byte order (usable as integer)
 * OUTPUT  : None
 * RETURN  : 32-bit integer in network byte order (big-endian)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_Hton32(host_int) ((UI32_T)(host_int))

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Ntoh32
 *------------------------------------------------------------------------
 * FUNCTION: Convert 16-bit integer from network byte order to host byte order
 * INPUT   : 32-bit integer in network byte order (big-endian)
 * OUTPUT  : None
 * RETURN  : 32-bit integer in host byte order (usable as integer)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_Ntoh32(network_int) ((UI32_T)(network_int))

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Hton64
 *------------------------------------------------------------------------
 * FUNCTION: Convert 64-bit integer from host byte order to network byte order
 * INPUT   : 64-bit integer in host byte order (usable as integer)
 * OUTPUT  : None
 * RETURN  : 64-bit integer in network byte order (big-endian)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_Hton64(host_int) ((UI64_T)host_int)

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_Ntoh64
 *------------------------------------------------------------------------
 * FUNCTION: Convert 64-bit integer from network byte order to host byte order
 * INPUT   : 64-bit integer in network byte order (big-endian)
 * OUTPUT  : None
 * RETURN  : 64-bit integer in host byte order (usable as integer)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_Ntoh64(network_int) ((UI64_T)network_int)

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_UI64_L32
 *------------------------------------------------------------------------
 * FUNCTION: Get lower 32-bit value from a UI64_T variable
 * INPUT   : UI64_T value (big-endian)
 * OUTPUT  : None
 * RETURN  : 32-bit integer in host byte order(usable as integer)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_UI64_L32(val) (((UI32_T*)(void*)&(val))[1])

/*------------------------------------------------------------------------
 * ROUTINE NAME - L_STDLIB_UI64_H32
 *------------------------------------------------------------------------
 * FUNCTION: Get higher 32-bit value from a UI64_T variable
 * INPUT   : UI64_T value (big-endian)
 * OUTPUT  : None
 * RETURN  : 32-bit integer in host byte order (usable as integer)
 * NOTE    :
 *------------------------------------------------------------------------*/
#define L_STDLIB_UI64_H32(val) (((UI32_T*)(void*)&(val))[0])

#endif /*SYS_HWCFG_LITTLE_ENDIAN_CPU*/
/* EPR: Mercury_V2-00185 */

/* PURPOSE: Return true if c is a printable character (0x20-0x7E).
 * INPUT:   c - input char (OCTET format)
 * OUTPUT:  none
 * RETURN:  return TRUE if c is a printable character
 */
BOOL_T L_STDLIB_IsAsciiPrint ( int c);

/* PURPOSE: Return true if c is a printable decimal character.
 * INPUT:   c   - input char (ASCIZ format)
 * OUTPUT:  none
 * RETURN:  return TRUE if c is a printable decimal character
 */
BOOL_T L_STDLIB_IsDigit ( int c);

/* PURPOSE  : Return true if there is special char included in the name string
 * INPUT    :  *p -- which point to the name string
                     c--special char
 * OUTPUT   : BOOL value   
 * RETURN   : TRUE/FALSE
*/
BOOL_T L_STDLIB_IsSpecialName(char *p, char c);


/* PURPOSE: Return true if oct_p is a particular representation of a printable
 *          character string (0x20-0x7E).
 * INPUT:   oct_p   - pointer of input string (OCTET format)
 *          len     - length of the octet
 * OUTPUT:  none
 * RETURN:  return TRUE if oct_p is a printable character octet string
 */
 
BOOL_T L_STDLIB_StrIsAsciiPrint(char *str_p);

/* PURPOSE: Return true if str_p is a particular representation of a decimal
 *          character string ('0'-'9').
 * INPUT:   str_p   - pointer of input string (ASCIZ format)
 * OUTPUT:  none
 * RETURN:  return TRUE if str_p is a decimal character string
 */
BOOL_T L_STDLIB_StrIsDigit(char *str_p);

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
BOOL_T L_STDLIB_VersionRangeCheck(char* start_ver_str, char* end_ver_str, char* check_ver_str);

#endif
