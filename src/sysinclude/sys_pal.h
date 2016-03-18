/*---------------------------------------------------------------------
 * File_Name : sys_pal.h
 *
 * Purpose   : Provide defination of Platform Abstract Layer according
 *             to current OS
 *
 * History:
 *       Date       --  Modifier,   Reason
 *      2008.04.07  --  Vai Wang,   First Created.
 *
 *
 * Note    : The naming constant defined in this package shall be reused
 *           by original Mercury Platform and ZebOS.
 *---------------------------------------------------------------------
 */

#ifndef SYS_PAL_H
#define SYS_PAL_H
#include "sys/types.h"
/*
** These types are unsigned and signed variations of known fixed length ints.
** We call chars ints when we use them as ints.
*/
#if 0
typedef unsigned int        u_int32_t; /* 32 bit unsigned integer */
typedef unsigned short      u_int16_t; /* 16 bit unsigned integer */
typedef unsigned char       u_int8_t;  /* 8 bit unsigned integer */
#endif
typedef signed int          S_INT32_T;	/* 32 bit signed integer */
#ifndef s_int32_t
#define s_int32_t           S_INT32_T
#endif /* s_int32_t */

typedef signed short        S_INT16_T;	/* 16 bit signed integer */
#ifndef s_int16_t
#define s_int16_t           S_INT16_T
#endif /* s_int16_t */

typedef signed char         S_INT8_T;	/* 8 bit signed integer */
#ifndef s_int8_t
#define s_int8_t           S_INT8_T
#endif /* s_int8_t */


/*
** An IPv4 address.  Can probably #define on a real OS.
*/
/*
struct pal_in4_addr {
  u_int32_t s_addr;
};
*/
#define pal_in4_addr in_addr

/*
** An IPv6 address (myriad ways of accessing it).  Maybe #define on real OS.
*/
/*
struct pal_in6_addr {
  union {
    u_int8_t  u6_addr8[16];
    u_int16_t u6_addr16[8];
    u_int32_t u6_addr32[4];
  } in6_u;
};
#define s6_addr			in6_u.u6_addr8
#define s6_addr16		in6_u.u6_addr16
#define s6_addr32		in6_u.u6_addr32
*/
#define pal_in6_addr in6_addr


#ifndef IN_LOOPBACK
#define IN_LOOPBACK(a)	((((u_int32_t) (a)) & 0xff000000) == 0x7f000000)
#endif /* !IN_LOOPBACK */


#undef pal_mem_set
#define pal_mem_set memset

#undef pal_mem_cpy
#define pal_mem_cpy memcpy

#undef pal_mem_cmp
#define pal_mem_cmp memcmp

#undef pal_mem_move
#define pal_mem_move memmove


#undef pal_snprintf
#define pal_snprintf snprintf

#undef pal_vsnprintf
#define pal_vsnprintf vsnprintf

#undef pal_sscanf
#define pal_sscanf sscanf

#undef pal_strcpy
#define pal_strcpy strcpy

#undef pal_strncpy
#define pal_strncpy strncpy

/* pal_strdup implementation is in the pal_memory.c file */

#undef pal_strcat
#define pal_strcat strcat

#undef pal_strncat
#define pal_strncat strncat

#undef pal_strcmp
#define pal_strcmp strcmp

#undef pal_strncmp
#define pal_strncmp strncmp

#undef pal_strcasecmp
#define pal_strcasecmp strcasecmp

#undef pal_strncasecmp
#define pal_strncasecmp strncasecmp

#undef pal_strlen
#define pal_strlen strlen

#undef pal_strto32
#define pal_strtos32(x,y,z) strtol((char*)x,(char**)y,z)

#undef pal_strtou32
#define pal_strtou32(x,y,z) strtoul((char*)x,(char**)y,z)

#undef pal_strchr
#define pal_strchr strchr

#undef pal_strstr
#define pal_strstr strstr

#undef pal_strrchr
#define pal_strrchr strrchr

#undef pal_strspn
#define pal_strspn strspn

#undef pal_strerror
#define pal_strerror strerror

#undef pal_strtok
#define pal_strtok strtok

#undef pal_char_tolower
#undef pal_char_toupper
#define pal_char_tolower tolower
#define pal_char_toupper toupper

#undef pal_char_isspace
#undef pal_char_isdigit
#undef pal_char_isxdigit
#undef pal_char_isalpha
#undef pal_char_isalnum
#undef pal_char_isupper
#undef pal_char_islower
#define pal_char_isspace isspace
#define pal_char_isdigit isdigit
#define pal_char_isxdigit isxdigit
#define pal_char_isalpha isalpha
#define pal_char_isalnum isalnum
#define pal_char_isupper isupper
#define pal_char_islower islower
#define pal_char_isprint isprint


#undef pal_ntoh32
#undef pal_hton32
#undef pal_ntoh16
#undef pal_hton16
#define pal_ntoh32 ntohl
#define pal_hton32 htonl
#define pal_ntoh16 ntohs
#define pal_hton16 htons


/* Address family numbers from RFC1700.  */
#define AFI_IP                    1
#define AFI_IP6                   2
#define AFI_MAX                   3

 #endif /* SYS_PAL_H */
 
