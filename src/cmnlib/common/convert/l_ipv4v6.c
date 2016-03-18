/* MODULE NAME: l_ipv4v6.c
 * PURPOSE:
 *      This package provides functions for IPv4/v6 address syntax checking.
 *
 * NOTES:
 *      None.
 * History:
 *      Date         Modifier       Reason
 * --------------- ------------ -----------------------
 *      2006/11/14   peter_yu       create
 *
 * Copyright(C)      Accton Corporation, 2006
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "sys_type.h"
#include "sys_cpnt.h"
#include "sys_adpt.h"
#include "l_stdlib.h"
#include "l_ipv4v6.h"

/*
#define DEBUG                   0
*/

/* NAMING CONSTANT DECLARATIONS
 */
#define IP4_ADDR_MAXSTRLEN      16
#define IPV6_MAX_PREFIX_LEN     128
#define RIFNO_DIFF_IFINDEX      2   /* number of netifs - number of rifs */

/* MACRO FUNCTION DECLARATIONS
 */
#define L_IPV4V6_IS_ADDR_LINK_LOCAL(addr) \
    (((addr) & 0xffc0) == 0xfe80)

#define L_IPV4V6_IS_ADDR_V4COMPAT(addr0, addr1, addr2, addr3) \
    (addr0 == 0 && \
     addr1 == 0 && \
     addr2 == 0 && \
     addr3 != 0 && /* Must not return TRUE for "::" and "::1" */ \
     addr3 != L_STDLIB_Hton32(1))

#define L_IPV4V6_IS_ADDR_V4MAPPED(addr0, addr1, addr2) \
    (addr0 == 0 && \
     addr1 == 0 && \
     addr2 == L_STDLIB_Hton32(0x0000ffff))

#define L_IPV4V6_Ipv6_GetIpnetIfindex(rifno)  ((rifno) + RIFNO_DIFF_IFINDEX +SYS_ADPT_MAX_NBR_OF_RIF)

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static int    L_IPV4V6_Inet_Pton(int family, const UI8_T *strptr, UI8_T *addrptr);
static BOOL_T L_IPV4V6_Char_Append(char tkn, char **str, UI32_T *len);
static BOOL_T L_IPV4V6_Is_Dec_Digital_String(const char *string);

/* STATIC VARIABLE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM BODIES
 */

/* FUNCTION: L_IPV4V6_Is_Valid_IP_AddressString
 *
 * PURPOSE:
 *      Check if this is a valid IPv4/v6 address string.
 *
 * INPUT:
 *      str         -- an ipv4/v6 address string
 *      str_len     -- string length
 * OUTPUT:
 *      None.
 *
 *  RETURN:
 *      TRUE        -- Valid IP address.
 *      FALSE       -- Invalid IP address.
 *
 * NOTES:
 *      None.
 */
BOOL_T L_IPV4V6_Is_Valid_IP_AddressString(char *str, UI32_T str_len)
{
#ifdef DEBUG
    printf("[L_IPV4V6_Is_Valid_IP_AddressString]\n");
    printf("str: %s, str_len: %ld\n", str, str_len);
#endif /* DEBUG */

    if(L_IPV4V6_Is_Valid_IPv4_AddressString(str, str_len)==TRUE)
    {
        return TRUE;
    }
#if (SYS_CPNT_IPV6 == TRUE)
    else if(L_IPV4V6_Is_Valid_IPv6_AddressString(str, str_len)==TRUE)
    {
        return TRUE;
    }
#endif
    else
    {
        return FALSE;
    }
}

/* FUNCTION: L_IPV4V6_Is_Valid_IPv4_AddressString
 *
 * PURPOSE:
 *      Check if this is a valid IPv4 address string.
 *
 * INPUT:
 *      str         -- an ipv4 address string
 *      str_len     -- string length
 * OUTPUT:
 *      None.
 *
 *  RETURN:
 *      TRUE        -- Valid IP address.
 *      FALSE       -- Invalid IP address.
 *
 * NOTES:
 *      None.
 */
BOOL_T L_IPV4V6_Is_Valid_IPv4_AddressString(char *str, UI32_T str_len)
{
    int   i;
    char *s;
    char  buf[IP4_ADDR_MAXSTRLEN];

    /* length checking */
    if(str_len > IP4_ADDR_MAXSTRLEN)
    {
        return FALSE;
    }

    /* copy string to local buffer */
    memcpy(buf, str, str_len);
    buf[str_len]=0;

#ifdef DEBUG
    printf("[L_IPV4V6_Is_Valid_IPv4_AddressString]\n");
    printf("buf: %s\n", buf);
#endif /* DEBUG */


    s = buf;
    /* first pass, checking isdigit or '.'  */
    while(*s != '\0')
    {
        if ( !isdigit(*s) && *s != '.')
        {
            return FALSE;
        }
        s++;
    }

    /* second pass, checking value range */
    {
        char *p = buf;
        char *p1;

        for ( i=0; i<3; i++)
        {
            p1 = strstr ( p, "."); /* find first position of str2 in str1 */
            if ( p1==NULL)   /* (1)p1==NULL => no "." found */
            {
                return FALSE;
            }

            if (p1==p) /* (2)p1==p => The sub string is NULL */
            {
                return FALSE;
            }

            if ( atoi(p) > 255 )
            {
                return FALSE;
            }
            p=p1+1;
        }

       /* check 4th sub string*/

        if (*p == '\0') /*(4) p == NULL => The 4th sub string is NULL*/
        {
            return FALSE;
        }

        if (atoi(p) > 255)
        {
            return FALSE;
        }

        if(strstr(p,".") != NULL) /* checking redundant '.' */
        {
            return FALSE;
        }

        return TRUE;
    }
}

/* FUNCTION: L_IPV4V6_Is_Valid_IPv6_AddressString
 *
 * PURPOSE:
 *      Check if this is a valid IPv6 address string.
 *      Especially for Get_Token1 in cli_main.c
 *
 * INPUT:
 *      str         -- an ipv6 address string
 *      str_len     -- string length
 *
 * OUTPUT:
 *      None.
 *
 *  RETURN:
 *      TRUE        -- Valid IPv6 address.
 *      FALSE       -- Invalid IPv6 address.
 *
 * NOTES:
 *      Refence ipcom_inet_pton() first pass.
 */
BOOL_T L_IPV4V6_Is_Valid_IPv6_AddressString(char *str, UI32_T str_len)
{
    int         colon_count = 0;
    BOOL_T      found_double_colon = FALSE;
    int         xstart = 0;  /* first zero (double colon) */
    int         len = 7;     /* numbers of zero words the double colon represents */
    char       *sp, *scope_id_p;
    char        buf[IPV6_ADDREESS_STRING_MAX_LEN];

    UI32_T value1, value2;

    /* length checking */
    if(str_len > IPV6_ADDREESS_STRING_MAX_LEN)
    {
        return FALSE;
    }

    /* copy string to local buffer */
    memcpy(buf, str, str_len);
    buf[str_len]=0;

#ifdef DEBUG
    printf("[L_IPV4V6_Is_Valid_IPv6_AddressString]\n");
    printf("buf: %s\n", buf);
#endif /* DEBUG */

    /* Found scope_id for link-local address */
    if((scope_id_p = strchr(buf, '%')) != NULL)
    {
        *scope_id_p = '\0';
        scope_id_p++;

        /* check if scope_id format / range */
        value2 = atoi(scope_id_p);
        value1 = 0;
        while (isdigit(*scope_id_p))
        {
            value1 = value1 * 10 + (*scope_id_p++ - '0');
        }

        /* we can know that if the scope_id substring is all digit by this checking*/
        if(value1 != value2)
        {
            goto errout;
        }
    }

#ifdef DEBUG
    printf("buf again : %s\n", buf);
#endif /* DEBUG */

    sp = buf;

    for (;;)
    {
        int i = 0;
        while (isxdigit(*sp))
        {
            sp++;
            i++;
        }
        if (i > 4)
            goto errout;

        if (*sp == '\0')
            break;
        if (*sp != ':')
        {
            if (*sp == '.' && len >= 2)
            {
                while (sp != buf && *(sp-1) != ':')
                    --sp;
                if (L_IPV4V6_Is_Valid_IPv4_AddressString(sp, strlen(sp)) == TRUE)
                {
                    len -= 2;
                    break;
                }
            }
            goto errout;
        }
        if (sp == buf)
        {
            /* The address begins with a colon */
            if (*++sp != ':')
                /* Must start with a double colon or a number */
                goto errout;
        }
        else
        {
            /* if i =0, that means no digit between two colon
             * if double colon is already found, that means three consecutive colons
             */
            if(found_double_colon && (i==0))
                goto errout;

            sp++;
            if (found_double_colon)
                len--;
            else
                xstart++;
        }

        if (*sp == ':')
        {
            if (found_double_colon)
                /* Two double colons are not allowed */
                goto errout;
            found_double_colon = TRUE;
            len -= xstart;
            sp++;
        }

        if (++colon_count == 7){

            UI8_T *ptr = (UI8_T *) sp;
            int i = 0;
            while(*ptr != '\0'){
                   if(isxdigit(*ptr++))
                         i++;
                   else
                        goto errout;
            }
            /* Found all colons */
            if(i > 4)
                goto errout;

            break;
        }
    } /* for (;;) */

    if ((colon_count < 7) && !found_double_colon)
        goto errout;
    if (colon_count > 7)
        goto errout;

    if (*--sp == ':')
        len++;
#ifdef DEBUG

    printf("L_IPV4V6_Is_Valid_IPv6_AddressString return TRUE\n");
#endif /* DEBUG */
    return TRUE;

 errout:
    /* str is not a valid presentation format for the specified family */
#ifdef DEBUG
    printf("L_IPV4V6_Is_Valid_IPv6_AddressString return FALSE;\n");
#endif /* DEBUG */
    return FALSE;
}


/* FUNCTION: L_IPV4V6_Is_Valid_IPv6_PrefixString
 *
 * PURPOSE:
 *      Check if this is a valid IPv6 prefix string.
 *
 * INPUT:
 *      str         -- an ipv6 prefix string
 *      str_len     -- string length
 *
 * OUTPUT:
 *      None.
 *
 *  RETURN:
 *      TRUE        -- Valid IPv6 prefix.
 *      FALSE       -- Invalid IPv6 prefix.
 *
 * NOTES:
 *      Reference Is_Valid_IPv6_Prefix.
 */
BOOL_T L_IPV4V6_Is_Valid_IPv6_PrefixString(char *str, UI32_T str_len)
{
    int    i;
    int    buf_len;
    int    rc = FALSE; /* return code */
    UI32_T value1, value2;
    char   buf[IPV6_ADDREESS_STRING_MAX_LEN];

    /* length checking */
    if(str_len > IPV6_ADDREESS_STRING_MAX_LEN)
    {
        return FALSE;
    }
    memcpy(buf, str, str_len);
    buf[str_len]=0;


#ifdef DEBUG
    printf("[L_IPV4V6_Is_Valid_IPv6_PrefixString]\n");
    printf("buf: %s\n", buf);
#endif /* DEBUG */

    if(str == NULL)
    {
        rc = FALSE;
#ifdef DEBUG
        printf("return %d;\n", rc);
#endif /* DEBUG */
       return rc;
    }

    memcpy(buf, str, str_len);
    buf[str_len]=0;

    buf_len = strlen(buf);

    for(i=0;i<buf_len;i++)
    {
        if(buf[i] == '/')
            break;
    }
    if(i == buf_len)
    {   /* There is no char '/' in the prefix string */
        rc = L_IPV4V6_Is_Valid_IPv6_AddressString(buf, buf_len);
    }
    else if(i < buf_len -1 && i>1)
    { /* full xxxx/xx format */
            buf[i]='\0';
            if(L_IPV4V6_Is_Valid_IPv6_AddressString(buf,i) == TRUE)
            {

                /* check prefix format */
                i++;
                value2 = atoi(&buf[i]);
                value1 = 0;
                while (isdigit(buf[i]))
                {
                    value1 = value1 * 10 + (buf[i] - '0');
                    i++;
                }

                /* we can know that is the scope_id substring is all digit by this */
                if((value1 == value2) && (value1 <= IPV6_MAX_PREFIX_LEN))
                {
                   rc = TRUE;
                }
            }

    }
#ifdef DEBUG
    printf("return %d;\n", rc);
#endif /* DEBUG */

    return rc;
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - L_IPV4V6_Get_Ipv6_Prefix_ByString
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the ipv6-address/prefix-length like form string to the binary form.
 *            This function can be used to (1)check format of ipv6 prefix
 *                                         (2)convert to binary form.
 * INPUT    : ipv6_prefix_str   -- a null terminated ipv6 prefix string
 *            scope_id_required -- scope_id_required, only for link-local address.
 * OUTPUT   : ipv6_prefix       -- a pointer to save save the binary form of ipv6
 *                                   address if convert succsessfully.
 *            prefix_len        -- a pointer to save prefix length if convert succsessfully.
 *            scope_id          -- a pointer to save scope_id if convert succsessfully.
 * RETURN   : TRUE  -- the input string represent a valid ipv6 prefix
 *            FALSE -- can not convert
 * NOTES    : 1. If scope_id_required is true, a scope_id is required for link-local prefix.
 * ---------------------------------------------------------------------
 */
BOOL_T L_IPV4V6_Get_Ipv6_Prefix_ByString(const char *ipv6_prefix_str,
                                         char       *ipv6_prefix,
                                         UI32_T     *prefix_len,
                                         UI32_T     *scope_id,
                                         BOOL_T      scope_id_required)
{
    UI32_T  i, len;

    if(((ipv6_prefix_str == NULL || ipv6_prefix == NULL || prefix_len == NULL)) ||
       ((scope_id_required == TRUE) && (scope_id == NULL)))
        return FALSE;

    len = strlen(ipv6_prefix_str);

    if(len > IPV6_PREFIX_TEMP_BUFFER_LEN - 1) return FALSE;

    for(i=0;i<len;i++)
    {
        if(ipv6_prefix_str[i] == '/')
            break;
    }

    if(i == len)
    {
        /* char '/' do not exist inside the prefix string */
        if(L_IPV4V6_Get_Ipv6_Address_ByString(ipv6_prefix_str, ipv6_prefix, scope_id, scope_id_required))
        {
            *prefix_len = 64;   /* Peter.Guo, 2005/06/03, change the default prefix length to 64 */
            return TRUE;
        }
    }
    else if(i < len -1 && i>1)
    {
        /* full xxxx/xx format */
        char cli_tmp_buf[IPV6_PREFIX_TEMP_BUFFER_LEN];

        memcpy(cli_tmp_buf, ipv6_prefix_str, len); /* duplicate the orginal string avoiding modification to it*/
        cli_tmp_buf[len]='\0';
        cli_tmp_buf[i]='\0';

        if(TRUE == L_IPV4V6_Get_Ipv6_Address_ByString(cli_tmp_buf, ipv6_prefix, scope_id, scope_id_required))
        {
            if(TRUE == L_IPV4V6_Is_Dec_Digital_String(&cli_tmp_buf[i+1]))
            {
                *prefix_len = atoi(&cli_tmp_buf[i+1]);
                if(*prefix_len <= IPV6_MAX_PREFIX_LEN)
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - L_IPV4V6_Get_Ipv6_Address_ByString
 * ---------------------------------------------------------------------
 * PURPOSE  : Convert ipv6 address string form to the binary form, including
 *              link-local-ip-address with or without scope-id.
 *              This function can be used to (1)check format of ipv6 address
 *                                           (2)convert to binary form.
 * INPUT    : ipv6_addr_string  -- a null terminated ipv6 address string
 *            scope_id_required -- scope_id_required, only for link-local address.
 * OUTPUT   : ipv6_addr         -- a pointer to save the binary form of ipv6 address
 *                                   if convert succsessfully.
 *            scope_id          -- a pointer to save scope_id if convert succsessfully.
 * RETURN   : TRUE  -- the input string represent a valid ipv6 address
 *            FALSE -- can not convert
 * NOTES    : 1. Valid IPv6 address string is
 *             (1)unicast without scope_id
 *             (2)link-local with scope_id
 *             (3)link-local without scope_id (for acl ipv6 and interface link-local address assignment).
 *            2. Valid scope_id is the interface (VLAN) range: 1 ~ SYS_ADPT_MAX_VLAN_ID.
 * ---------------------------------------------------------------------
 */
BOOL_T L_IPV4V6_Get_Ipv6_Address_ByString(const char *ipv6_addr_string,
                                          char       *ipv6_addr,
                                          UI32_T     *scope_id,
                                          BOOL_T      scope_id_required)
{
    UI32_T  tmp_scope_id;
    char   *ptr_scope_id              = NULL;
    UI8_T   *ptr_address;
    UI8_T   buff[IPV6_ADDREESS_STRING_MAX_LEN]    = {0}; /* IPv6 address string after filtering scope id */
    UI8_T   tmp_ipaddr[SYS_ADPT_IPV6_ADDR_LEN];

    if((ipv6_addr == NULL) || (ipv6_addr_string == NULL))
    {
        goto errout;
    }

    memset(ipv6_addr, 0, SYS_ADPT_IPV6_ADDR_LEN);
    memset(&tmp_ipaddr, 0, SYS_ADPT_IPV6_ADDR_LEN);

    /* seperate scope_id from address if exist. */
    ptr_scope_id = strchr(ipv6_addr_string, '%');

    if(ptr_scope_id != NULL)
    {
        if(scope_id_required == FALSE)
        {
            goto errout;
        }

        /* copy sub string contains ipv6 address */
        if((UI32_T)(ptr_scope_id - ipv6_addr_string) >= IPV6_ADDREESS_STRING_MAX_LEN)
        {
            goto errout;
        }
        memcpy(buff, ipv6_addr_string, ptr_scope_id-ipv6_addr_string);
        ptr_address = buff;
    }
    else /* ptr_scope_id == NULL */
    {
        ptr_address = (UI8_T *)ipv6_addr_string;
    }

    if (L_IPV4V6_Inet_Pton(L_IPV4V6_IP_AF_INET6, ptr_address, tmp_ipaddr) == 1)
    {
        UI16_T  link_addr = (tmp_ipaddr[0] << 8) + tmp_ipaddr[1];

        if(L_IPV4V6_IS_ADDR_LINK_LOCAL(link_addr))
        {
            if( ptr_scope_id != NULL) /* scope_id_required is TRUE here */
            {
                /* process sub string contains scope_id */
                /* verify the legality of scope id */
                ptr_scope_id++;
                if(TRUE == L_IPV4V6_Is_Dec_Digital_String(ptr_scope_id))
                {
                    tmp_scope_id = atoi(ptr_scope_id);
                }
                else
                {
                    goto errout;
                }
                /* Check interface id (VLAN) range */
                if( (tmp_scope_id < 1 ) || (tmp_scope_id > SYS_ADPT_MAX_VLAN_ID))
                {
                    goto errout;
                }

                if(scope_id == NULL)
                {
                    goto errout;
                }
                else
                {
                    *scope_id = L_IPV4V6_Ipv6_GetIpnetIfindex(tmp_scope_id);
                }
            }
            else /* ptr_scope_id == NULL */
            {
                if (scope_id_required == TRUE)
                {
                    goto errout;
                }
                /* else is valid */
            }
        }
        else
        {
            /* Check invalid address string: Non-link-local with scope_id */
            if(ptr_scope_id != NULL)
            {
                goto errout;
            }
            /* else is valid */
        }
        /* valid address string and copy temp address to output */
        memcpy(ipv6_addr, tmp_ipaddr, sizeof(tmp_ipaddr));

        return TRUE;
    } /* if ipcom_inet_pton */
    else
    {
        goto errout;
    }

errout:
    return FALSE;
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - L_IPV4V6_Inet_Ntop
 * ---------------------------------------------------------------------
 * PURPOSE  : Converts the network address into a character string.
 * INPUT    : family  -- the address family for the address to convert.
 *            addrptr -- a pointer to the binary form of ipv6 address
 *            len     -- the length of the strptr buffer.
 * OUTPUT   : strptr  -- a pointer to the buffer that will receive the
 *                         string representation of the address.
 * RETURN   : TRUE    -- if the call was successful
 *            FALSE   -- if failed
 * NOTES    : None
 * ---------------------------------------------------------------------
 */
BOOL_T L_IPV4V6_Inet_Ntop(int family, void *addrptr, char *strptr, UI32_T len)
{
    char *str = strptr;

    if (family == L_IPV4V6_IP_AF_INET)
    {
        UI32_T  in_val;
        int     octet;
        int     b, s;

        memcpy (&in_val, addrptr, sizeof(in_val));
        in_val = L_STDLIB_Ntoh32 (in_val);

        for (b = 24; b >= 0; b -= 8)
        {
            octet = (in_val >> b) & 0xFF;
            for (s = 100; s > 0; s /= 10)
            {
                if (octet >= s || s == 1)
                {
                    if (FALSE == L_IPV4V6_Char_Append(
                        (char)('0' + (octet % (s * 10)) / s), &str, &len))
                        goto errout;
                }
            }

            if (b != 0 && L_IPV4V6_Char_Append('.', &str, &len) == FALSE)
                goto errout;
        }
        return TRUE;
    }
    else if (family == L_IPV4V6_IP_AF_INET6)
    {
        UI32_T  *in6_val32;
        int     n, w;
        UI16_T  word;
        UI8_T   in6_val [16];
        UI8_T   nibble;
        BOOL_T  dci = FALSE; /* is double colon inserted */
        BOOL_T  dcc = FALSE; /* is double colon beeing inserted */

        memcpy(in6_val, addrptr, sizeof(in6_val));
        in6_val32 = (UI32_T *) in6_val;

        if (L_IPV4V6_IS_ADDR_V4COMPAT(in6_val32[0], in6_val32[1],
                                      in6_val32[2], in6_val32[3]))
        {
            /* Compat addresses is formated like this ::x.y.z.w */
            if ((L_IPV4V6_Char_Append(':', &str, &len) == FALSE) ||
                (L_IPV4V6_Char_Append(':', &str, &len) == FALSE))
                goto errout;

            return (L_IPV4V6_Inet_Ntop(L_IPV4V6_IP_AF_INET, &in6_val[12], str, len));
        }

        if (L_IPV4V6_IS_ADDR_V4MAPPED(in6_val32[0], in6_val32[1], in6_val32[2]))
        {
            /* Ipv6 mapped Ipv4 addresses is formated like this ::ffff:x.y.z.w */
            if (8 >= len)
                goto errout;
            strcpy(strptr, "::FFFF:");

            return (L_IPV4V6_Inet_Ntop(L_IPV4V6_IP_AF_INET, &in6_val[12], strptr + 7, len - 7));
        }

        for (w = 0; w < 8; w++)
        {
            memcpy (&word, &in6_val [w*2], sizeof (word));
            word = L_STDLIB_Ntoh16 (word);

            if (word == 0 && dci == FALSE)
            {
                dcc = TRUE;
                if (w == 0 && L_IPV4V6_Char_Append(':', &str, &len) == FALSE)
                    goto errout;
                continue;
            }

            if (dci == FALSE && dcc == TRUE)
            {
                if (L_IPV4V6_Char_Append(':', &str, &len) == FALSE)
                    goto errout;
                dci = TRUE;
            }

            for (n = 12; n >= 0; n -= 4)
            {
                /* Skipp leading zeros */
                if (n == 0 || word >= (0x1 << n))
                {
                    nibble = (char) ((word >> n) & 0xF);
                    if (FALSE == L_IPV4V6_Char_Append(
                        (char)(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble), &str, &len))
                        goto errout;
                }
            }
            if (w != 7 && L_IPV4V6_Char_Append(':', &str, &len) == FALSE)
                goto errout;
        }
        if (dci == FALSE && dcc == TRUE)
            if (L_IPV4V6_Char_Append(':', &str, &len) == FALSE)
                goto errout;

        return TRUE;
    }

errout:
    return FALSE;
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - L_IPV4V6_Inet_Pton
 * ---------------------------------------------------------------------
 * PURPOSE  : Converts a string representation of a address to its binary form.
 * INPUT    : family  -- the address family for the address to convert.
 *                         (L_IPV4V6_IP_AF_INET or L_IPV4V6_IP_AF_INET6)
 *            strptr  -- a string representation of the address.
 * OUTPUT   : addrptr -- a pointer to the buffer that will receive the
 *                         binary form of the address.
 * RETURN   : 1       -- if the call was successful
 *            0       -- if failed
 * NOTES    : None
 * ---------------------------------------------------------------------
 */
static int L_IPV4V6_Inet_Pton(int family, const UI8_T *strptr, UI8_T *addrptr)
{
    switch(family)
    {
    case L_IPV4V6_IP_AF_INET:
        {
            UI32_T  octet, octval, addr = 0;

            for (octet = 1; octet <= 4; octet++)
            {
                octval = 0;
                while (isdigit(*strptr))
                    octval = octval * 10 + (*strptr++ - '0');

                if (octval > 255 || (*strptr++ != '.' && octet != 4))
                    goto errout;

                addr = (addr << 8) + octval;
            }

            addr = L_STDLIB_Hton32 (addr);
            memcpy (addrptr, &addr, sizeof (addr));

            return 1;
        }

    case L_IPV4V6_IP_AF_INET6:
        {
            UI32_T          i;
            UI32_T          colon_count = 0;
            UI32_T          xstart = 0;  /* first zero (double colon) */
            UI32_T          len = 7;     /* numbers of zero words the double colon represents */
            UI16_T          *in6_val = (UI16_T *) addrptr;
            const UI8_T     *s = strptr;
            BOOL_T          found_double_colon = FALSE;

            /* First pass, verify the syntax and locate the double colon if present */
            for (;;)
            {
                while (isxdigit(*s))
                    s++;
                if (*s == '\0')
                    break;
                if (*s != ':')
                {
                    if (*s == '.' && len >= 2)
                    {
                        while (s != strptr && *(s-1) != ':')
                            --s;
                        if (L_IPV4V6_Inet_Pton(L_IPV4V6_IP_AF_INET, s, addrptr) == 1)
                        {
                            len -= 2;
                            break;
                        }
                    }
                    /* This could be a valid address */
                    /* break; */ /* 2005-02-16, stj add + */
                    /* This could be an invalid address */
                    goto errout;       /* Peter.guo, 2005/02/28, modify to support strict address check*/
                }
                if (s == strptr)
                {
                    /* The address begins with a colon */
                    if (*++s != ':')
                        /* Must start with a double colon or a number */
                        goto errout;
                }
                else
                {
                    s++;
                    if (found_double_colon)
                        len--;
                    else
                        xstart++;
                }

                if (*s == ':')
                {
                    if (found_double_colon)
                        /* Two double colons are not allowed */
                        goto errout;
                    found_double_colon = TRUE;
                    len -= xstart;
                    s++;
                }

                if (++colon_count == 7)
                {
                    /* Peter.Guo, 2005/5/13, strictly check ipv6 string form.*/
                    UI32_T  i = 0;
                    UI8_T   *ptr = (UI8_T *) s;
                    while(*ptr != '\0')
                    {
                       if(isxdigit(*ptr++))
                             i++;
                       else
                            goto errout;
                    }
                    /* Found all colons */
                    if(i > 4)
                        goto errout;

                    break;
                }
            }

            if (colon_count == 0 || colon_count > 7)
                goto errout;

            if (*--s == ':')
                len++;

            /* Second pass, read the address */
            s = strptr;
            for (i = 0; i < 8; i++)
            {
                UI16_T val = 0;

                if (found_double_colon && i >= xstart && i < xstart + len)
                {
                    in6_val[i] = 0;
                    continue;
                }
                while (*s == ':')
                    s++;

                if (i == 6 && isdigit(*s)
                    && L_IPV4V6_Inet_Pton(L_IPV4V6_IP_AF_INET, s, (UI8_T *) &in6_val[i]) == 1)
                    /* Ending with :IPv4-address, which is valid */
                    break;

                while (isxdigit(*s))
                {
                    val <<= 4;
                    if (*s >= '0' && *s <= '9')
                        val += (*s - '0');
                    else if (*s >= 'A' && *s <= 'F')
                        val += (*s - 'A' + 10);
                    else
                        val += (*s - 'a' + 10);
                    s++;
                }
                in6_val[i] = L_STDLIB_Hton16(val);
            }
            return 1;
        }

    default:
        break;
    }

errout:
    /* strptr is not a valid presentation format for the specified family */
    return 0;
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - L_IPV4V6_Char_Append
 * ---------------------------------------------------------------------
 * PURPOSE  : Appends a character to a string and verifies that the string
 *              is long enough.
 * INPUT    : tkn   -- the character to append.
 *            len   -- must contain the current length of 'str' when calling,
 *                       will be updated with the new length.
 * OUTPUT   : str   -- the string to append to, the string will be point to
 *                       the next character beyond the appended one.
 * RETURN   : TRUE  -- if the call was successful
 *            FALSE -- if failed
 * NOTES    : None
 * ---------------------------------------------------------------------
 */
static BOOL_T L_IPV4V6_Char_Append(char tkn, char **str, UI32_T *len)
{
    if (*len <= 1)
        return FALSE;
    **str = tkn;
    (*str)++;
    **str = '\0';
    (*len)--;
    return TRUE;
}

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - L_IPV4V6_Is_Dec_Digital_String
 * ---------------------------------------------------------------------
 * PURPOSE  : tell whether it is a digital string
 * INPUT    : string -- a null terminated string
 * OUTPUT   : none
 * RETURN   : TRUE  -- it is a digital string
 *            FALSE -- it is not a digital string
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
static BOOL_T L_IPV4V6_Is_Dec_Digital_String(const char *string)
{
    I32_T i;

    if(string != NULL)
    {
        for(i=strlen(string)-1;i>=0;i--)
        {
            if(!isdigit(string[i]))
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}
