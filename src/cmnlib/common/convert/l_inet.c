/* Package Name: l_inet.C
 * Purpose: This package provides the functions to covert ip to decimal ip-dot-string
 *          use ISAM structure.
 * Notes:
 * History:
 *    12/10/2001 -- Ryan
 * Copyright(C)      Accton Corporation, 2001
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h>
#if __WIN32__
#elif __APPLE__ && __MACH__
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#elif __LINUX__
#include <sys/socket.h>
#include <linux/in.h>
#include <linux/in6.h>
#endif

#include "sys_type.h"
#include "sys_cpnt.h"
#include "sys_adpt.h"

#include "l_inet.h"
#include "l_stdlib.h"

/* NAMING CONSTANT DECLARATIONS
 */
#ifndef L_INET_MAX
#define L_INET_MAX(x1,x2)   ((x1) >= (x2) ? (x1) : (x2))
#endif

#define L_INET_ADDR_MAX_ZONE_ID_STR_LEN     5 /* %9999 */

#define L_INET_ADDR_MAX_IPV4_PREFIX_STR_LEN 3 /* /32  */
#define L_INET_ADDR_MAX_IPV6_PREFIX_STR_LEN 4 /* /128  */
#define L_INET_ADDR_MAX_PREFIX_STR_LEN      L_INET_MAX(L_INET_ADDR_MAX_IPV6_PREFIX_STR_LEN, L_INET_ADDR_MAX_IPV4_PREFIX_STR_LEN)

#define L_INET_ADDR_DFLT_ZONE_ID            0
#define L_INET_ADDR_DFLT_PREFIX_LEN         0

#if (SYS_CPNT_CRAFT_PORT == TRUE)
#define L_INET_ADDR_MAX_ZONE_ID  (((SYS_ADPT_CRAFT_INTERFACE_IFINDEX-SYS_ADPT_VLAN_1_IF_INDEX_NUMBER+1)>SYS_ADPT_MAX_VLAN_ID)?\
                                     (SYS_ADPT_CRAFT_INTERFACE_IFINDEX-SYS_ADPT_VLAN_1_IF_INDEX_NUMBER+1):SYS_ADPT_MAX_VLAN_ID)
#else
#define L_INET_ADDR_MAX_ZONE_ID  SYS_ADPT_MAX_VLAN_ID
#endif

#define L_INET_ADDR_MIN_ZONE_ID  0

/* MACRO FUNCTION DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_LocalIsValidIpv4StringFormat
 *-------------------------------------------------------------------------
 * PURPOSE : Verify whether a string is a valid IPv4 string format
 * INPUT   : str_p  -- address string
 * OUTPUT  : None
 * RETURN  : L_INET_RETURN_SUCCESS  -- valid
 *           L_INET_RETURN_FAIL     -- invalid
 * NOTE    : The format of the address string is
 *           ipv4[%zone_id][/prefix_length]
 *-------------------------------------------------------------------------
 */
static L_INET_RETURN_VALUE_T
L_INET_LocalIsValidIpv4StringFormat(
    const char *str_p
);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_LocalIsValidIpv6StringFormat
 *-------------------------------------------------------------------------
 * PURPOSE : Verify whether a string is a valid IPv6 string format
 * INPUT   : str_p  -- address string
 * OUTPUT  : None
 * RETURN  : L_INET_RETURN_SUCCESS  -- valid
 *           L_INET_RETURN_FAIL     -- invalid
 * NOTE    : The format of the address string is
 *           ipv6[%zone_id][/prefix_length]
 *-------------------------------------------------------------------------
 */
static L_INET_RETURN_VALUE_T
L_INET_LocalIsValidIpv6StringFormat(
    const char *str_p
);

/* EXPORTED SUBPROGRAM BODIES
 */

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_FillInetAddrIp
 *-------------------------------------------------------------------------
 * PURPOSE : Fill a L_INET_AddrIp_T structure with suitable arguments
 * INPUT   : af         -- address family
 *           addr_p     -- binary IPv4/v6 address
 *           prefix_len -- prefix length
 * OUTPUT  : inaddr_p   -- output L_INET_AddrIp_T object
 * RETURN  : TRUE   - success
 *           FALSE  - fail
 * NOTE    : None
 *-------------------------------------------------------------------------
 */
BOOL_T
L_INET_FillInetAddrIp(
    UI32_T af,
    UI8_T *addr_p,
    UI16_T prefix_len,
    L_INET_AddrIp_T *ipaddr_p)
{
    if (!addr_p || !ipaddr_p)
        return FALSE;

    memset(ipaddr_p, 0, sizeof(L_INET_AddrIp_T));

    if(af == L_INET_AF_INET)
    {
        if (prefix_len > L_INET_MAX_IPV4_PREFIX_LEN)
            return FALSE;

        memcpy(ipaddr_p->addr, addr_p, SYS_ADPT_IPV4_ADDR_LEN);
        ipaddr_p->addrlen = SYS_ADPT_IPV4_ADDR_LEN;
        ipaddr_p->preflen = prefix_len;

        if (L_INET_ADDR_IS_IPV4_LINK_LOCAL(addr_p))
            ipaddr_p->type = L_INET_ADDR_TYPE_IPV4Z;
        else
            ipaddr_p->type = L_INET_ADDR_TYPE_IPV4;

        return TRUE;
    }
    else if(af == L_INET_AF_INET6)
    {
        if (prefix_len > L_INET_MAX_IPV6_PREFIX_LEN)
            return FALSE;

        memcpy(ipaddr_p->addr, addr_p, SYS_ADPT_IPV6_ADDR_LEN);
        ipaddr_p->addrlen = SYS_ADPT_IPV6_ADDR_LEN;
        ipaddr_p->preflen = prefix_len;

        if (L_INET_ADDR_IS_IPV6_LINK_LOCAL(addr_p))
            ipaddr_p->type = L_INET_ADDR_TYPE_IPV6Z;
        else
            ipaddr_p->type = L_INET_ADDR_TYPE_IPV6;

        return TRUE;
    }

    return FALSE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_Aton
 *-------------------------------------------------------------------------
 * PURPOSE : Check and convert a decimal ip-dot-string to a binary IPv4 address
 * INPUT   : cp   -- decimal ip-dot-string
 * OUTPUT  : ip   -- binary IPv4 address
 * RETURN  : TRUE   - success
 *           FALSE  - fail
 * NOTE    : None
 *-------------------------------------------------------------------------
 */
BOOL_T
L_INET_Aton(
    UI8_T *cp,
    UI32_T *ip)
{
    return (inet_pton(AF_INET, (char *)cp, (void*) ip) > 0);
}


/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_Ntoa
 *-------------------------------------------------------------------------
 * PURPOSE : Convert binary IPv4 address to decimal ip-dot-string(d.d.d.d) representation.
 * INPUT   : ip   -- binary IPv4 address
 * OUTPUT  : cp   -- decimal ip-dot-string
 * RETURN  : cp
 * NOTE    : None
 *-------------------------------------------------------------------------
 */
UI8_T *
L_INET_Ntoa(
    UI32_T ip,
    UI8_T cp[18])
{
   UI8_T *p;

   p = (UI8_T *)&ip;
   sprintf((char *)cp, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
   return (cp);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_Pton
 *-------------------------------------------------------------------------
 * PURPOSE : Check and convert a IPv4/v6 string to a binary IPv4/v6 address
 * INPUT   : af     -- address family
 *           str_p  -- IPv4/v6 address string
 * OUTPUT  : addr_p -- binary IPv4/v6 array
 * RETURN  : TRUE   - success
 *           FALSE  - fail
 * NOTE    : None
 *-------------------------------------------------------------------------
 */
BOOL_T
L_INET_Pton(
    int af,
    char *str_p,
    UI8_T *addr_p)
{
    return (inet_pton(af, str_p, (void *)addr_p) > 0);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_Ntop
 *-------------------------------------------------------------------------
 * PURPOSE : Convert a binary IPv4/v6 address to IPv4/v6 string
 * INPUT   : af      -- address family
 *           addr_p  -- binary IPv4/v6 array
 *           str_len -- str_p size (must include space for '\0')
 * OUTPUT  : str_p   -- output IPv4/v6 address string
 * RETURN  : TRUE   - success
 *           FALSE  - fail
 * NOTE    : None
 *-------------------------------------------------------------------------
 */
BOOL_T
L_INET_Ntop(
    int af, UI8_T *addr_p,
    char *str_p,
    UI32_T str_len)
{
    return (inet_ntop(af, (void *)addr_p, str_p, (size_t)str_len) != NULL);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_InaddrToString
 *-------------------------------------------------------------------------
 * PURPOSE : Convert a L_INET_Addr_T structure to IPv4/v6
 * INPUT   : inaddr_p   -- L_INET_Addr_T object
 *           cb_str     -- size of str_p (must include space for '\0')
 * OUTPUT  : str_p      -- IPv4/v6
 * RETURN  : L_INET_RETURN_SUCCESS  -- success
 *           L_INET_RETURN_FAIL     -- fail
 * NOTE    : The format of the ouput string is
 *           {ipv4|ipv6}[%zone_id][/prefix_length]
 *-------------------------------------------------------------------------
 */
L_INET_RETURN_VALUE_T
L_INET_InaddrToString(
    const L_INET_Addr_T *inaddr_p,
    char *str_p,
    UI32_T cb_str)
{
    char zoneid_str[L_INET_ADDR_MAX_ZONE_ID_STR_LEN+1] = {0};
    char prefix_str[L_INET_ADDR_MAX_PREFIX_STR_LEN+1] = {0};

    if (TRUE == L_INET_IS_IP_ADDR_TYPE(inaddr_p->type))
    {
        L_INET_AddrIp_T *ipaddr_p = (L_INET_AddrIp_T*)inaddr_p;

        if (TRUE == L_INET_IS_LINK_LOCAL_ADDR_TYPE(ipaddr_p->type))
        {
            if (ipaddr_p->zoneid > L_INET_ADDR_MAX_ZONE_ID)
            {
                return L_INET_RETURN_FAIL;
            }

            if (ipaddr_p->zoneid != L_INET_ADDR_DFLT_ZONE_ID)
            {
                /* ipv4 no need zoin id
                 */
                if (L_INET_ADDR_TYPE_IPV4Z == ipaddr_p->type)
                {
                    return L_INET_RETURN_FAIL;
                }

                snprintf(zoneid_str, sizeof(zoneid_str), "%%%lu", ipaddr_p->zoneid);
                zoneid_str[sizeof(zoneid_str)-1] = '\0';
            }
        }

        if (TRUE == L_INET_IS_IPV4_ADDR_TYPE(ipaddr_p->type))
        {
            if (ipaddr_p->addrlen != SYS_ADPT_IPV4_ADDR_LEN)
            {
                return L_INET_RETURN_FAIL;
            }

            if (((ipaddr_p->type == L_INET_ADDR_TYPE_IPV4) &&
                 (TRUE == L_INET_ADDR_IS_IPV4_LINK_LOCAL(ipaddr_p->addr)))
                ||
                ((ipaddr_p->type == L_INET_ADDR_TYPE_IPV4Z) &&
                 (TRUE != L_INET_ADDR_IS_IPV4_LINK_LOCAL(ipaddr_p->addr))))
            {
                return L_INET_RETURN_FAIL;
            }

            if (ipaddr_p->preflen > L_INET_MAX_IPV4_PREFIX_LEN)
            {
                return L_INET_RETURN_FAIL;
            }

            if (inet_ntop(AF_INET, (void*)ipaddr_p->addr, str_p, (size_t)cb_str) == NULL)
            {
                return L_INET_RETURN_FAIL;
            }
        }
        else if (TRUE == L_INET_IS_IPV6_ADDR_TYPE(ipaddr_p->type))
        {
            if (ipaddr_p->addrlen != SYS_ADPT_IPV6_ADDR_LEN)
            {
                return L_INET_RETURN_FAIL;
            }

            if (((ipaddr_p->type == L_INET_ADDR_TYPE_IPV6) &&
                 (TRUE == L_INET_ADDR_IS_IPV6_LINK_LOCAL(ipaddr_p->addr)))
                ||
                ((ipaddr_p->type == L_INET_ADDR_TYPE_IPV6Z) &&
                 (TRUE != L_INET_ADDR_IS_IPV6_LINK_LOCAL(ipaddr_p->addr))))
            {
                return L_INET_RETURN_FAIL;
            }

            if (ipaddr_p->preflen > L_INET_MAX_IPV6_PREFIX_LEN)
            {
                return L_INET_RETURN_FAIL;
            }

            if (inet_ntop(AF_INET6, (void*)ipaddr_p->addr, str_p, (size_t)cb_str) == NULL)
            {
                return L_INET_RETURN_FAIL;
            }
        }

        if (ipaddr_p->preflen != L_INET_ADDR_DFLT_PREFIX_LEN)
        {
            snprintf(prefix_str, sizeof(prefix_str), "/%u", ipaddr_p->preflen);
            prefix_str[sizeof(prefix_str)-1] = '\0';
        }

        if ((strlen(str_p) + strlen(zoneid_str) + strlen(prefix_str)) > (cb_str-1))
        {
            return L_INET_RETURN_FAIL;
        }

        strcat(str_p, zoneid_str);
        strcat(str_p, prefix_str);
        str_p[cb_str-1] = '\0';

        return L_INET_RETURN_SUCCESS;
    }

    return L_INET_RETURN_FAIL;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_InprefixToString
 *-------------------------------------------------------------------------
 * PURPOSE : Convert a L_INET_Addr_T structure to IPv4/v6 string
 *           (slightly different with L_INET_InaddrToString, see Note below)
 * INPUT   : in_p    -- L_INET_Addr_T object
 *           str_len -- str_p size (must include space for '\0')
 * OUTPUT  : str_p   -- output IPv4/v6 address string
 * RETURN  : TRUE   - success
 *           FALSE  - fail
 * NOTE    : For show route special, different with L_INET_InaddrToString() in:
 *           1. not support DNS & zoneid
 *           2. prefix string is added even if prefix len = 0
 *-------------------------------------------------------------------------
 */
BOOL_T
L_INET_InprefixToString(
    const L_INET_Addr_T *inaddr_p,
    UI32_T str_len,
    UI8_T *str_p)
{
    char prefix_str[L_INET_ADDR_MAX_PREFIX_STR_LEN+1] = {0};

    if (L_INET_IS_IP_ADDR_TYPE(inaddr_p->type))
    {
        L_INET_AddrIp_T *ipaddr_p = (L_INET_AddrIp_T*)inaddr_p;

        if (L_INET_IS_IPV4_ADDR_TYPE(ipaddr_p->type))
        {
            if (ipaddr_p->preflen > L_INET_MAX_IPV4_PREFIX_LEN)
                return FALSE;
            if (inet_ntop(AF_INET, (void*)ipaddr_p->addr, (char *) str_p, (size_t)str_len) == NULL)
                return FALSE;
        }
        else if (L_INET_IS_IPV6_ADDR_TYPE(ipaddr_p->type))
        {
            if (ipaddr_p->preflen > L_INET_MAX_IPV6_PREFIX_LEN)
                return FALSE;
            if (inet_ntop(AF_INET6, (void*)ipaddr_p->addr, (char *) str_p, (size_t)str_len) == NULL)
                return FALSE;
        }

        snprintf(prefix_str, sizeof(prefix_str), "/%u", ipaddr_p->preflen);

        if ((strlen((char*)str_p) + strlen(prefix_str)) > (str_len-1))
            return FALSE;

        strcat((char*)str_p, prefix_str);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_StringToInaddr
 *-------------------------------------------------------------------------
 * PURPOSE : Converts an address string to L_INET_Addr_T object
 * INPUT   : format     -- which format of string we want to convert
 *         : str_p      -- IPv4/v6
 *           cb_inaddr  -- size of inaddr_p
 * OUTPUT  : inaddr_p   -- L_INET_Addr_T object
 * RETURN  : L_INET_RETURN_SUCCESS  -- success
 *           L_INET_RETURN_FAIL     -- fail
 * NOTE    : None
 *-------------------------------------------------------------------------
 */
L_INET_RETURN_VALUE_T
L_INET_StringToInaddr(
    L_INET_FORMAT_T format,
    const char *str_p,
    L_INET_Addr_T *inaddr_p,
    UI32_T cb_inaddr)
{
    L_INET_AddrIp_T *ip_p = (L_INET_AddrIp_T*)inaddr_p;
    UI32_T str_len;
    UI32_T zoneid;
    UI32_T preflen;
    int    af;
    UI8_T  ipaddr[SYS_ADPT_IPV6_ADDR_LEN];
    char   buf[L_INET_MAX_IPADDR_STR_LEN+1];
    char   *zoneid_p, *preflen_p;

    if (NULL == str_p || NULL == inaddr_p)
    {
        return L_INET_RETURN_FAIL;
    }

    if (L_INET_RETURN_SUCCESS == L_INET_LocalIsValidIpv4StringFormat(str_p))
    {
        af = AF_INET;
    }
    else if (L_INET_RETURN_SUCCESS == L_INET_LocalIsValidIpv6StringFormat(str_p))
    {
        af = AF_INET6;
    }
    else
    {
        return L_INET_RETURN_FAIL;
    }

    if (cb_inaddr < sizeof(L_INET_AddrIp_T))
    {
        return L_INET_RETURN_FAIL;
    }

    str_len = strlen(str_p);
    memcpy(buf, str_p, str_len);
    buf[str_len] = '\0';
    zoneid = L_INET_ADDR_DFLT_ZONE_ID;
    preflen = L_INET_ADDR_DFLT_PREFIX_LEN;

    if (NULL != (preflen_p = strchr(buf, '/')))
    {
        *preflen_p = '\0';
        preflen = atoi(++preflen_p);
    }

    if (NULL != (zoneid_p = strchr(buf, '%')))
    {
        *zoneid_p = '\0';
        zoneid = atoi(++zoneid_p);
    }

    if (inet_pton(af, buf, (void*)ipaddr) <= 0)
    {
        return L_INET_RETURN_FAIL;
    }

    switch (format)
    {
        /* {ipv4|ipv6}[%zone_id][/prefix_length]
         */
        case L_INET_FORMAT_IP_UNSPEC:
            break;

        /* {ipv4|ipv6}[%zone_id]
         */
        case L_INET_FORMAT_IP_ADDR:
            if (NULL != preflen_p)
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* {ipv4|ipv6}[%zone_id]/prefix_length
         */
        case L_INET_FORMAT_IP_PREFIX:
            if (NULL == preflen_p)
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* ipv4[%zone_id][/prefix_length]
         */
        case L_INET_FORMAT_IPV4_UNSPEC:
            if (AF_INET != af)
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* ipv4[%zone_id]
         */
        case L_INET_FORMAT_IPV4_ADDR:
            if ((AF_INET != af) ||
                (NULL != preflen_p))
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* ipv4[%zone_id]/prefix_length
         */
        case L_INET_FORMAT_IPV4_PREFIX:
            if ((AF_INET != af) ||
                (NULL == preflen_p))
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* ipv6[%zone_id][/prefix_length]
         */
        case L_INET_FORMAT_IPV6_UNSPEC:
            if (AF_INET6 != af)
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /*  ipv6[%zone_id]
         */
        case L_INET_FORMAT_IPV6_ADDR:
            if ((AF_INET6 != af) ||
                (NULL != preflen_p))
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* ipv6[%zone_id]/prefix_length
         */
        case L_INET_FORMAT_IPV6_PREFIX:
            if ((AF_INET6 != af) ||
                (NULL == preflen_p))
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* ipv6
         */
        case L_INET_FORMAT_IPV6_GLOBAL:
            if ((AF_INET6 != af) ||
                (NULL != zoneid_p) ||
                (NULL != preflen_p))
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* ipv6/prefix_length
         */
        case L_INET_FORMAT_IPV6_GLOBAL_PREFIX:
            if ((AF_INET6 != af) ||
                (NULL != zoneid_p) ||
                (NULL == preflen_p))
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* ipv6%zone_id
         */
        case L_INET_FORMAT_IPV6_LINK_LOCAL:
            if ((AF_INET6 != af) ||
                (NULL == zoneid_p) ||
                (NULL != preflen_p))
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* ipv6(link-local)
         */
        case L_INET_FORMAT_IPV6_LINK_LOCAL_NO_SCOPE_ID:
            if ((TRUE != L_INET_ADDR_IS_IPV6_LINK_LOCAL(ipaddr)) ||
                (NULL != zoneid_p) ||
                (NULL != preflen_p))
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        /* ipv6(global | link-local)[/prifix]
         */
        case L_INET_FORMAT_IPV6_ADDR_RAW:
            if ((AF_INET6 != af) ||
                (NULL != zoneid_p))
            {
                return L_INET_RETURN_FAIL;
            }
            break;

        default:
            return L_INET_RETURN_FAIL;
    }

    memset(ip_p, 0, sizeof(*ip_p));

    switch (af)
    {
        case AF_INET:
            if (TRUE == L_INET_ADDR_IS_IPV4_LINK_LOCAL(ipaddr))
            {
                /* ipv4 link local no need zone id
                 */
                if (NULL != zoneid_p)
                {
                    return L_INET_RETURN_FAIL;
                }

                ip_p->type = L_INET_ADDR_TYPE_IPV4Z;
            }
            else
            {
                if (NULL != zoneid_p)
                {
                    return L_INET_RETURN_FAIL;
                }

                ip_p->type = L_INET_ADDR_TYPE_IPV4;
            }

            ip_p->addrlen = SYS_ADPT_IPV4_ADDR_LEN;
            break;

        case AF_INET6:
            if (TRUE == L_INET_ADDR_IS_IPV6_LINK_LOCAL(ipaddr))
            {
                if (format != L_INET_FORMAT_IPV6_LINK_LOCAL_NO_SCOPE_ID &&
                    format != L_INET_FORMAT_IPV6_ADDR_RAW &&
                    NULL == zoneid_p)
                {
                    return L_INET_RETURN_FAIL;
                }

                ip_p->type = L_INET_ADDR_TYPE_IPV6Z;
            }
            else
            {
                if (NULL != zoneid_p)
                {
                    return L_INET_RETURN_FAIL;
                }

                ip_p->type = L_INET_ADDR_TYPE_IPV6;
            }

            ip_p->addrlen = SYS_ADPT_IPV6_ADDR_LEN;
            break;

        default:
            return L_INET_RETURN_FAIL;
    }

    memcpy(ip_p->addr, ipaddr, ip_p->addrlen);
    ip_p->zoneid  = zoneid;
    ip_p->preflen = (UI16_T)preflen;

    return L_INET_RETURN_SUCCESS;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_SockaddrToInaddr
 *-------------------------------------------------------------------------
 * PURPOSE : Convert struct sockaddr to L_INET_AddrIp_T format
 * INPUT   : sock_addr -- struct sockaddr object
 * OUTPUT  : inet_addr -- L_INET_AddrIp_T object
 * RETURN  : TRUE   - success
 *           FALSE  - fail
 * NOTE    : None
 *-------------------------------------------------------------------------
 */
BOOL_T
L_INET_SockaddrToInaddr(
    const struct sockaddr *sockaddr_p,
    L_INET_AddrIp_T *ipaddr_p)
{
    if(AF_INET == sockaddr_p->sa_family)  /* IPv4 */
    {
        struct sockaddr_in *sock_addr4 = (struct sockaddr_in*)sockaddr_p;

        memset(ipaddr_p, 0, sizeof(L_INET_AddrIp_T));
        ipaddr_p->addrlen = SYS_ADPT_IPV4_ADDR_LEN;
        memcpy(ipaddr_p->addr, &sock_addr4->sin_addr, SYS_ADPT_IPV4_ADDR_LEN);
        ipaddr_p->type = (L_INET_ADDR_IS_IPV4_LINK_LOCAL(ipaddr_p->addr)) ?
                            L_INET_ADDR_TYPE_IPV4Z : L_INET_ADDR_TYPE_IPV4;
    }
#if (SYS_CPNT_IPV6 == TRUE)
    else if(AF_INET6 == sockaddr_p->sa_family)  /* IPv6 */
    {
        static UI8_T v4_prefix[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF};
        static int v4_prefix_len = sizeof(v4_prefix);
        struct sockaddr_in6 *sock_addr6 = (struct sockaddr_in6*)sockaddr_p;

        memset(ipaddr_p, 0, sizeof(L_INET_AddrIp_T));

        /* IPv4-Mapped IPv6 Address
         */
#if __linux__
        if(0 == memcmp(v4_prefix, &sock_addr6->sin6_addr.in6_u, v4_prefix_len))
        {
            ipaddr_p->addrlen = SYS_ADPT_IPV4_ADDR_LEN;
            memcpy(ipaddr_p->addr, &sock_addr6->sin6_addr.in6_u.u6_addr8[v4_prefix_len], SYS_ADPT_IPV4_ADDR_LEN);
            ipaddr_p->type = (L_INET_ADDR_IS_IPV4_LINK_LOCAL(ipaddr_p->addr)) ?
                                L_INET_ADDR_TYPE_IPV4Z : L_INET_ADDR_TYPE_IPV4;
        }
        else  /* IPv6 */
        {
            ipaddr_p->addrlen = SYS_ADPT_IPV6_ADDR_LEN;
            memcpy(ipaddr_p->addr, &sock_addr6->sin6_addr.in6_u, SYS_ADPT_IPV6_ADDR_LEN);
            ipaddr_p->type = (L_INET_ADDR_IS_IPV6_LINK_LOCAL(ipaddr_p->addr)) ?
                                L_INET_ADDR_TYPE_IPV6Z : L_INET_ADDR_TYPE_IPV6;
            ipaddr_p->zoneid = sock_addr6->sin6_scope_id - SYS_ADPT_VLAN_1_IF_INDEX_NUMBER + 1;
            if (ipaddr_p->zoneid > L_INET_ADDR_MAX_ZONE_ID)
                ipaddr_p->zoneid = 0;
        }
#elif __APPLE__ && __MACH__
        /* IPv4-Mapped IPv6 Address
         */
        if(0 == memcmp(v4_prefix, &sock_addr6->sin6_addr.__u6_addr, v4_prefix_len))
        {
            ipaddr_p->addrlen = SYS_ADPT_IPV4_ADDR_LEN;
            memcpy(ipaddr_p->addr, &sock_addr6->sin6_addr.__u6_addr.__u6_addr8[v4_prefix_len], SYS_ADPT_IPV4_ADDR_LEN);
            ipaddr_p->type = (L_INET_ADDR_IS_IPV4_LINK_LOCAL(ipaddr_p->addr)) ?
            L_INET_ADDR_TYPE_IPV4Z : L_INET_ADDR_TYPE_IPV4;
        }
        else  /* IPv6 */
        {
            ipaddr_p->addrlen = SYS_ADPT_IPV6_ADDR_LEN;
            memcpy(ipaddr_p->addr, &sock_addr6->sin6_addr.__u6_addr, SYS_ADPT_IPV6_ADDR_LEN);
            ipaddr_p->type = (L_INET_ADDR_IS_IPV6_LINK_LOCAL(ipaddr_p->addr)) ?
            L_INET_ADDR_TYPE_IPV6Z : L_INET_ADDR_TYPE_IPV6;
            ipaddr_p->zoneid = sock_addr6->sin6_scope_id - SYS_ADPT_VLAN_1_IF_INDEX_NUMBER + 1;
            if (ipaddr_p->zoneid > L_INET_ADDR_MAX_ZONE_ID)
                ipaddr_p->zoneid = 0;
        }
#endif
        
    }
#endif  /* #if (SYS_CPNT_IPV6 == TRUE) */
    else
    {
        return FALSE;
    }

    return TRUE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_SockaddrToInaddr
 *-------------------------------------------------------------------------
 * PURPOSE : Convert struct sockaddr to L_INET_AddrIp_T format
 * INPUT   : ipaddr_p     -- L_INET_AddrIp_T object
 *           port_number  -- ip socket port number
 *           sockaddr_len -- size of sockaddr_p
 * OUTPUT  : sockaddr_p   -- struct sockaddr object
 * RETURN  : TRUE   - success
 *           FALSE  - fail
 * NOTE    : None
 *-------------------------------------------------------------------------
 */
BOOL_T
L_INET_InaddrToSockaddr(
    const L_INET_AddrIp_T *ipaddr_p,
    UI16_T port_number,
    UI32_T sockaddr_len,
    struct sockaddr *sockaddr_p)
{
    if(NULL == ipaddr_p || NULL == sockaddr_p)
    {
        return FALSE;
    }

    if (L_INET_IS_IPV4_ADDR_TYPE(ipaddr_p->type))
    {
        if(sizeof(struct sockaddr_in) <= sockaddr_len)
        {
            struct sockaddr_in *sin4 =(struct sockaddr_in*)sockaddr_p;
            sin4->sin_family = AF_INET;
            sin4->sin_port   = port_number;
            memcpy(&sin4->sin_addr.s_addr, ipaddr_p->addr, SYS_ADPT_IPV4_ADDR_LEN);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

#if(SYS_CPNT_IPV6 == TRUE)
    else if (L_INET_IS_IPV6_ADDR_TYPE(ipaddr_p->type))
    {
        if(sizeof(struct sockaddr_in6) <= sockaddr_len)
        {
            struct sockaddr_in6 *sin6 =(struct sockaddr_in6*)sockaddr_p;
            sin6->sin6_family = AF_INET6;
            sin6->sin6_port   = port_number;
            memcpy(&sin6->sin6_addr, ipaddr_p->addr, SYS_ADPT_IPV6_ADDR_LEN);
            sin6->sin6_scope_id = ipaddr_p->zoneid + SYS_ADPT_VLAN_1_IF_INDEX_NUMBER - 1;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
#endif

    return FALSE;
}


/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_CompareInetAddr
 *-------------------------------------------------------------------------
 * PURPOSE : Compares two L_INET_Addr_T objects whether the first one is "less than",
 *           "equal to" or "greater than" the second one
 * INPUT   : in1_p                   -- First L_INET_Addr_T object to be compared
 *           in2_p                   -- Second L_INET_Addr_T object to be compared
 *           compare_flag            -- L_INET_COMPARE_FLAG_INCLUDE_ADDRRESS_LENGTH
 *                                      L_INET_COMPARE_FLAG_INCLUDE_PREFIX_LENGTH
 * OUTPUT  : None
 * RETURN  : < 0  -- in1_p is less (earlier) than in2_p
 *          == 0  -- in1_p is equal to in2_p
 *           > 0  -- in1_p is greater (later) than ip2_p
 * NOTE    : 1. L_INET_COMPARE_FLAG_INCLUDE_ADDRRESS_LENGTH
 *              is needed by SNMP when it is a length-prefixed (not IMPLIED) index
 *           2. L_INET_COMPARE_FLAG_INCLUDE_PREFIX_LENGTH
 *              is needed for self-addresses including prefix length, in the style
 *              PRIVATE-MIB.ipAddrInetTable
 *           3. compare order:
 *                1. type
 *                2. addrlen (if L_INET_COMPARE_FLAG_INCLUDE_ADDRRESS_LENGTH is set)
 *                3. addr
 *                4. zoneid (if they are link-local addresses)
 *                5. preflen (if L_INET_COMPARE_FLAG_INCLUDE_PREFIX_LENGTH is set)
 *-------------------------------------------------------------------------
 */
I32_T
L_INET_CompareInetAddr(
    const L_INET_Addr_T *in1_p,
    const L_INET_Addr_T *in2_p,
    UI32_T compare_flag)
{
    I32_T cmp = 0;          /* compare value */

    L_INET_AddrIp_T *ip1_p = (L_INET_AddrIp_T *) in1_p;  /* only for IPv4 and IPv6 */
    L_INET_AddrIp_T *ip2_p = (L_INET_AddrIp_T *) in2_p;  /* only for IPv4 and IPv6 */
    L_INET_AddrDns_T *dns1_p = (L_INET_AddrDns_T *) in1_p;  /* only for DNS */
    L_INET_AddrDns_T *dns2_p = (L_INET_AddrDns_T *) in2_p;  /* only for DNS */

    if (in1_p->type != in2_p->type)
    {
        return (in1_p->type - in2_p->type);
    }

    /* check special case for both types being "unknown"
     */
    if (in1_p->type == L_INET_ADDR_TYPE_UNKNOWN)
    {
        return 0;
    }

    /* compare address length (if needed)
     */
    if ((compare_flag & L_INET_COMPARE_FLAG_INCLUDE_ADDRRESS_LENGTH) &&
        (in1_p->addrlen != in2_p->addrlen))
    {
        return (in1_p->addrlen - in2_p->addrlen);
    }

    /* address length ignored or equal: compare address array (without zone ID)
     */
    switch (in1_p->type)
    {
        case L_INET_ADDR_TYPE_IPV4:
        case L_INET_ADDR_TYPE_IPV4Z:
        {
            cmp = memcmp(ip1_p->addr, ip2_p->addr, SYS_TYPE_IPV4_ADDR_LEN);

            /* if address array (without zone ID) unequal: return the "memcmp" result
             */
            if (cmp != 0)
            {
                return cmp;
            }
            break;
        }

        case L_INET_ADDR_TYPE_IPV6:
        case L_INET_ADDR_TYPE_IPV6Z:
        {
            cmp = memcmp(ip1_p->addr, ip2_p->addr, SYS_TYPE_IPV6_ADDR_LEN);

            /* if address array (without zone ID) unequal: return the "memcmp" result
             */
            if (cmp != 0)
            {
                return cmp;
            }
            break;
        }

        case L_INET_ADDR_TYPE_DNS:
        {
            /* compare has finished (whether equal or not): return the "strncmp" result
             */
            return (strncmp((char*)dns1_p->addr, (char*)dns2_p->addr, sizeof(dns1_p->addr) - 1));
        }

        default:
            return 0;
    }

    /* if type and address array (without zone ID) equal: compare zone ID
     */
    if (((in1_p->type == L_INET_ADDR_TYPE_IPV4Z)
         ||(in1_p->type == L_INET_ADDR_TYPE_IPV6Z))
        && (ip1_p->zoneid != ip2_p->zoneid))
    {
        /* Because a legal zone ID can range from 0 to maximum of a 32-bit integer
         * (although our switch should be using only VLAN ID's and craft port zone ID's),
         * zone ID subtraction (ip1_p->zoneid - ip2_p->zoneid) may exceed the range of I32_T
         * and so cannot be used here.
         */
        if (ip1_p->zoneid < ip2_p->zoneid)
        {
            return (-1);
        }
        else if (ip1_p->zoneid == ip2_p->zoneid)
        {
            return 0;
        }
        else  /* case for (ip1_p->zoneid > ip2_p->zoneid) */
        {
            return 1;
        }
    }

    /* if type and address array and zone ID are equal, compare prefix length (if needed)
     */
    if ((compare_flag & L_INET_COMPARE_FLAG_INCLUDE_PREFIX_LENGTH) &&
        (ip1_p->preflen != ip2_p->preflen))
    {
        return (ip1_p->preflen - ip2_p->preflen);
    }

    return 0;
}


/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_PrefixMatch
 *-------------------------------------------------------------------------
 * PURPOSE : Check whether a prefix includes another
 * INPUT   : n -- first L_INET_AddrIp_T object
 *           p -- second L_INET_AddrIp_T object
 * OUTPUT  : None
 * RETURN  : If n includes p prefix then return 1 else return 0
 * NOTE    : Refer from Quagga prefix_match()
 *-------------------------------------------------------------------------
 */
BOOL_T
L_INET_PrefixMatch(
    const L_INET_AddrIp_T *n,
    const L_INET_AddrIp_T *p)
{
    /* Utility mask array. */
    static UI8_T maskbit[] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};

    int offset;
    int shift;

    /* Set both prefix's head pointer. */
    const UI8_T *np = (const UI8_T *)n->addr;
    const UI8_T *pp = (const UI8_T *)p->addr;

    /* If n's prefix is longer than p's one return 0. */
    if (n->preflen> p->preflen)
        return FALSE;

    offset = n->preflen / 8;
    shift =  n->preflen % 8;

    if (shift)
        if (maskbit[shift] & (np[offset] ^ pp[offset]))
            return FALSE;

    while (offset--)
        if (np[offset] != pp[offset])
            return FALSE;

    return TRUE;
}


/* There is a bug in __cmsg_nxthdr() function defined in cmsg_nxthdr.c
 * within uClibc 0.9.27. This defect will cause the last cmsg couldn't
 * be retrieve from msghdr. Since this library function is declared as
 * libc_hidden_proto(__cmsg_nxthdr), which means it can be overwritten
 * by declaring a new and correct function with the same name.
 * If we use newer version of uClibc, this duplicated function can be removed then.
 * One of the places using this function is: pal_sock_in6_cmsg_lookup()
 * in pal_sock_raw.c (the CMSG_NXTHDR macro)
 */
#if 0
#ifndef __KERNEL__
struct cmsghdr *
__cmsg_nxthdr (struct msghdr *mhdr, struct cmsghdr *cmsg)
{
  if ((size_t) cmsg->cmsg_len < sizeof (struct cmsghdr))
    /* The kernel header does this so there may be a reason.  */
    return NULL;

  cmsg = (struct cmsghdr *) ((unsigned char *) cmsg
                 + CMSG_ALIGN (cmsg->cmsg_len));
  if ((unsigned char *) (cmsg + 1) > ((unsigned char *) mhdr->msg_control
                      + mhdr->msg_controllen)
      || ((unsigned char *) cmsg + CMSG_ALIGN (cmsg->cmsg_len)
      > ((unsigned char *) mhdr->msg_control + mhdr->msg_controllen)))
    /* No more entries.  */
    return NULL;
  return cmsg;
}
#endif
#endif /* 0 */
/* LOCAL SUBPROGRAM BODIES
 */
/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_LocalIsValidIpv4StringFormat
 *-------------------------------------------------------------------------
 * PURPOSE : Verify whether a string is a valid IPv4 string format
 * INPUT   : str_p  -- address string
 * OUTPUT  : None
 * RETURN  : L_INET_RETURN_SUCCESS  -- valid
 *           L_INET_RETURN_FAIL     -- invalid
 * NOTE    : The format of the address string is
 *           ipv4[%zone_id][/prefix_length]
 *-------------------------------------------------------------------------
 */
static L_INET_RETURN_VALUE_T
L_INET_LocalIsValidIpv4StringFormat(
    const char *str_p)
{
    long   zoneid, preflen;
    UI32_T str_len;
    u_int  val;
    int    saw_digit, octets, ch;
    char   *tmp_p, *zoneid_p, *preflen_p;
    char   buf[L_INET_MAX_IP4ADDR_STR_LEN+1];
    const  char *src = buf;

    if (NULL == str_p)
    {
        return L_INET_RETURN_FAIL;
    }

    str_len = strlen(str_p);

    if (str_len > L_INET_MAX_IP4ADDR_STR_LEN)
    {
        return L_INET_RETURN_FAIL;
    }

    memcpy(buf, str_p, str_len);
    buf[str_len] = '\0';

    /* check prifix length
     */
    if (NULL != (preflen_p = strchr(buf, '/')))
    {
        if (strlen(preflen_p) > L_INET_ADDR_MAX_IPV4_PREFIX_STR_LEN)
        {
            return L_INET_RETURN_FAIL;
        }

        *preflen_p = '\0';

        if (*(++preflen_p) == '\0')
        {
            return L_INET_RETURN_FAIL;
        }

        preflen = strtol(preflen_p, &tmp_p, 10);

        if (*tmp_p != '\0' ||
            preflen < L_INET_MIN_IPV4_PREFIX_LEN ||
            preflen > L_INET_MAX_IPV4_PREFIX_LEN)
        {
            return L_INET_RETURN_FAIL;
        }
    }

    /* check scope id
     */
    if (NULL != (zoneid_p = strchr(buf, '%')))
    {
        if (strlen(zoneid_p) > L_INET_ADDR_MAX_ZONE_ID_STR_LEN)
        {
            return L_INET_RETURN_FAIL;
        }

        *zoneid_p = '\0';

        if (*(++zoneid_p) == '\0')
        {
            return L_INET_RETURN_FAIL;
        }

        zoneid = strtol(zoneid_p, &tmp_p, 10);

        if (*tmp_p != '\0' ||
            zoneid < L_INET_ADDR_MIN_ZONE_ID ||
            zoneid > L_INET_ADDR_MAX_ZONE_ID)
        {
            return L_INET_RETURN_FAIL;
        }
    }

    /* check address, refered & modified from inet_pton4()
     */
    saw_digit = 0;
    octets    = 0;
    val       = 0;

    while ((ch = *src++) != '\0')
    {
        if (ch >= '0' && ch <= '9')
        {
            val = val * 10 + (ch - '0');

            if (val > 255)
            {
                return L_INET_RETURN_FAIL;
            }

            if (++saw_digit > 3)
            {
                return L_INET_RETURN_FAIL;
            }

            continue;
        }

        if (ch == '.')
        {
            if (++octets > SYS_ADPT_IPV4_ADDR_LEN)
            {
                return L_INET_RETURN_FAIL;
            }

            if (*src == '\0')
            {
                return L_INET_RETURN_FAIL;
            }

            val = 0;
            saw_digit = 0;
            continue;
        }

        return L_INET_RETURN_FAIL;
    }

    if (0 != saw_digit)
    {
        ++octets;
    }

    if (octets != SYS_ADPT_IPV4_ADDR_LEN)
    {
        return L_INET_RETURN_FAIL;
    }

    return L_INET_RETURN_SUCCESS;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_LocalIsValidIpv6StringFormat
 *-------------------------------------------------------------------------
 * PURPOSE : Verify whether a string is a valid IPv6 string format
 * INPUT   : str_p  -- address string
 * OUTPUT  : None
 * RETURN  : L_INET_RETURN_SUCCESS  -- valid
 *           L_INET_RETURN_FAIL     -- invalid
 * NOTE    : The format of the address string is
 *           ipv6[%zone_id][/prefix_length]
 *-------------------------------------------------------------------------
 */
static L_INET_RETURN_VALUE_T
L_INET_LocalIsValidIpv6StringFormat(
    const char *str_p)
{
    long   zoneid, preflen;
    UI32_T str_len;
    int    ch, tp, saw_xdigit, saw_dcolon;
    UI8_T  ipaddr[SYS_ADPT_IPV6_ADDR_LEN];
    char   *tmp_p, *zoneid_p, *preflen_p;
    char   buf[L_INET_MAX_IP6ADDR_STR_LEN+1];
    char   ipv4_buf[L_INET_MAX_IP4ADDR_STR_LEN+1];
    char   *curtok, *src = buf;
    BOOL_T has_ipv4_format;

    if (NULL == str_p)
    {
        return L_INET_RETURN_FAIL;
    }

    str_len = strlen(str_p);

    if (str_len > L_INET_MAX_IP6ADDR_STR_LEN)
    {
        return L_INET_RETURN_FAIL;
    }

    memcpy(buf, str_p, str_len);
    buf[str_len] = '\0';

    /* check prifix length
     */
    if (NULL != (preflen_p = strchr(buf, '/')))
    {
        if (strlen(preflen_p) > L_INET_ADDR_MAX_IPV6_PREFIX_STR_LEN)
        {
            return L_INET_RETURN_FAIL;
        }

        *preflen_p = '\0';

        if (*(++preflen_p) == '\0')
        {
            return L_INET_RETURN_FAIL;
        }

        preflen = strtol(preflen_p, &tmp_p, 10);

        if (*tmp_p != '\0' ||
            preflen < L_INET_MIN_IPV6_PREFIX_LEN ||
            preflen > L_INET_MAX_IPV6_PREFIX_LEN)
        {
            return L_INET_RETURN_FAIL;
        }
    }

    /* check scope id
     */
    if (NULL != (zoneid_p = strchr(buf, '%')))
    {
        if (strlen(zoneid_p) > L_INET_ADDR_MAX_ZONE_ID_STR_LEN)
        {
            return L_INET_RETURN_FAIL;
        }

        *zoneid_p = '\0';

        if (*(++zoneid_p) == '\0')
        {
            return L_INET_RETURN_FAIL;
        }

        zoneid = strtol(zoneid_p, &tmp_p, 10);

        if (*tmp_p != '\0' ||
            zoneid < L_INET_ADDR_MIN_ZONE_ID ||
            zoneid > L_INET_ADDR_MAX_ZONE_ID)
        {
            return L_INET_RETURN_FAIL;
        }
    }

    /* check address, refered & modified from inet_pton6()
     */

    /* leading :: requires some special handling.
     */
    if (*src == ':')
    {
        if (*++src != ':')
        {
            return L_INET_RETURN_FAIL;
        }
    }

    curtok     = src;
    tp         = 0;
    saw_xdigit = 0;
    saw_dcolon = 0;
    has_ipv4_format = FALSE;

    while ((ch = (*src++)) != '\0')
    {
        if (0 != isxdigit(ch))
        {
            if (++saw_xdigit > 4)
            {
                return L_INET_RETURN_FAIL;
            }

            continue;
        }

        if (ch == ':')
        {
            curtok = src;

            if (0 == saw_xdigit)
            {
                if (0 != saw_dcolon++)
                {
                    return L_INET_RETURN_FAIL;
                }

                continue;
            }
            else if (*src == '\0')
            {
                return L_INET_RETURN_FAIL;
            }

            if ((tp + 2) > SYS_ADPT_IPV6_ADDR_LEN)
            {
                return L_INET_RETURN_FAIL;
            }

            tp += 2;
            saw_xdigit = 0;
            continue;
        }

        if (ch == '.')
        {
            if ((tp + SYS_ADPT_IPV4_ADDR_LEN) > SYS_ADPT_IPV6_ADDR_LEN)
            {
                return L_INET_RETURN_FAIL;
            }

            if (NULL != zoneid_p)
            {
                snprintf(ipv4_buf, sizeof(ipv4_buf), "%s%%%s", curtok, zoneid_p);
            }
            else
            {
                memcpy(ipv4_buf, curtok, sizeof(ipv4_buf));
            }

            ipv4_buf[sizeof(ipv4_buf)-1] = '\0';

            if (L_INET_RETURN_SUCCESS != L_INET_LocalIsValidIpv4StringFormat(ipv4_buf))
            {
                return L_INET_RETURN_FAIL;
            }

            tp += SYS_ADPT_IPV4_ADDR_LEN;
            saw_xdigit = 0;
            has_ipv4_format = TRUE;
            break;  /* '\0' was seen by L_INET_IsValidIpv4AddressString(). */
        }

        return L_INET_RETURN_FAIL;
    }

    if (0 != saw_xdigit)
    {
        tp += 2;
    }

    if ((saw_dcolon == 0 && tp != SYS_ADPT_IPV6_ADDR_LEN) ||
        (saw_dcolon == 1 && tp >= SYS_ADPT_IPV6_ADDR_LEN))
    {
        return L_INET_RETURN_FAIL;
    }

    if (inet_pton(AF_INET6, buf, (void*)ipaddr) <= 0)
    {
        return L_INET_RETURN_FAIL;
    }

    if (TRUE != L_INET_ADDR_IS_IPV4_COMPATIBLE_IPV6_ADDRESS(ipaddr))
    {
        if (TRUE == has_ipv4_format)
        {
            return L_INET_RETURN_FAIL;
        }
    }

    return L_INET_RETURN_SUCCESS;
}

