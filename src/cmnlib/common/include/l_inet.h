/* Package Name: l_inet.C
 * Purpose: This package provides the functions to covert ip to decimal ip-dot-string
 *          use ISAM structure.
 * Notes:
 * History:
 *    12/10/2001 -- Ryan
 * Copyright(C)      Accton Corporation, 2001
 */

#ifndef L_INET_H
#define L_INET_H

#ifdef WIN32
#include <winsock2.h>
#else
#ifdef __KERNEL__
#include <linux/socket.h>  /* located in Linux kernel */
#else
#include <sys/socket.h>    /* located in ELDK */
#endif
#endif

#include "sys_type.h"
#include "sys_cpnt.h"


/* NAMING CONSTANT DECLARACTIONS
 */

/* Address family
 */
#define L_INET_AF_INET      AF_INET
#define L_INET_AF_LINK      AF_LINK
#define L_INET_AF_INET6     AF_INET6

/* for IPv4 dotted-decimal string(15) + scope id(5) + prefix length(3)
 * e.g., 255.255.255.255%1234/32
 */
#define L_INET_MAX_IP4ADDR_STR_LEN    23

/* for IPv6 hex string(45) + scope id(5) + prefix length(4)
 * e.g., FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:255.255.255.255%1234/128
 */
#define L_INET_MAX_IP6ADDR_STR_LEN    54

#define L_INET_MAX_IPADDR_STR_LEN       L_INET_MAX_IP6ADDR_STR_LEN
#define L_INET_MAX_DNS_STR_LEN          255
#define L_INET_ADDR_DNS_MAX_STR_LEN     255
#define L_INET_MIN_IPV4_PREFIX_LEN              0
#define L_INET_MAX_IPV4_PREFIX_LEN              32
#define L_INET_MIN_IPV6_PREFIX_LEN              0
#define L_INET_MAX_IPV6_PREFIX_LEN              128

#define L_INET_COMPARE_FLAG_INCLUDE_ADDRRESS_LENGTH   0x1
#define L_INET_COMPARE_FLAG_INCLUDE_PREFIX_LENGTH     0x2

/* MACRO FUNCTION DECLARATIONS
 */
#define L_INET_ADDR_HEADER_SIZE   ((size_t)&((L_INET_Addr_T*)0)->addr)

#define L_INET_ADDR_IS_IPV4_COMPATIBLE_IPV6_ADDRESS(addr) \
    ((addr[0] == 0x00) && (addr[1] == 0x00) && \
     (addr[2] == 0x00) && (addr[3] == 0x00) && \
     (addr[4] == 0x00) && (addr[5] == 0x00) && \
     (addr[6] == 0x00) && (addr[7] == 0x00) && \
     (addr[8] == 0x00) && (addr[9] == 0x00) && \
     (addr[10] == 0xFF) && (addr[11] == 0xFF))

#define L_INET_ADDR_IS_IPV4_LINK_LOCAL_COMPATIBLE_IPV6_ADDRESS(addr) \
    ((addr[0] == 0x00) && (addr[1] == 0x00) && \
     (addr[2] == 0x00) && (addr[3] == 0x00) && \
     (addr[4] == 0x00) && (addr[5] == 0x00) && \
     (addr[6] == 0x00) && (addr[7] == 0x00) && \
     (addr[8] == 0x00) && (addr[9] == 0x00) && \
     (addr[10] == 0xFF) && (addr[11] == 0xFF) && \
     (addr[12] == 0xA9) && (addr[13] == 0xFE))

#define L_INET_ADDR_IS_IPV6_LINK_LOCAL(addr) \
    (L_INET_ADDR_IS_IPV6_LINK_LOCAL_UNICAST(addr)||L_INET_ADDR_IS_IPV6_LINK_LOCAL_MULTICAST(addr))

/* In RFC 4291, 2.4 Address Type Identification,
 * link-local unicast address should be FE80::/10
 */
#define L_INET_ADDR_IS_IPV6_LINK_LOCAL_UNICAST(addr) \
	((((addr[0] << 8) + addr[1]) & 0XFFC0 ) == 0XFE80 )

#define L_INET_ADDR_IS_VALID_IPV6_LINK_LOCAL(addr) \
    (L_INET_ADDR_IS_VALID_IPV6_LINK_LOCAL_UNICAST(addr)||L_INET_ADDR_IS_IPV6_LINK_LOCAL_MULTICAST(addr))

/* In RFC 4291, 2.5.6 Link-local IPv6 Unicast Addresses
 * valid link-local unicast address should be FE80::/64;
 * Configured(UI) IPv6 link-local address should be checked by this macro
 */
#define L_INET_ADDR_IS_VALID_IPV6_LINK_LOCAL_UNICAST(addr) \
    ((addr[0] == 0xFE) && (addr[1] == 0x80) && \
     (addr[2] == 0x00) && (addr[3] == 0x00) && \
     (addr[4] == 0x00) && (addr[5] == 0x00) && \
     (addr[6] == 0x00) && (addr[7] == 0x00))



#define L_INET_ADDR_IS_IPV6_LINK_LOCAL_MULTICAST(addr) \
    ( ( ((addr[0] << 8) + addr[1]) & 0XFF0F ) == 0XFF02 )

#define L_INET_ADDR_IS_IPV4_LINK_LOCAL(addr) \
    ((addr[0] == 169) && (addr[1] == 254))

#define L_INET_IS_IPV4_ADDR_TYPE(type) \
    (L_INET_ADDR_TYPE_IPV4 == type || L_INET_ADDR_TYPE_IPV4Z == type)

#define L_INET_IS_IPV6_ADDR_TYPE(type) \
    (L_INET_ADDR_TYPE_IPV6 == type || L_INET_ADDR_TYPE_IPV6Z == type)

#define L_INET_IS_IP_ADDR_TYPE(type) \
    (L_INET_IS_IPV4_ADDR_TYPE(type) || L_INET_IS_IPV6_ADDR_TYPE(type))

#define L_INET_IS_LINK_LOCAL_ADDR_TYPE(type) \
    (L_INET_ADDR_TYPE_IPV4Z == type || L_INET_ADDR_TYPE_IPV6Z == type)

/* DATA TYPE DECLARATIONS
 */

/* values for L_INET_Addr_T.type, same as in INET-ADDRESS-MIB
 */
typedef enum
{
    L_INET_ADDR_TYPE_UNKNOWN    = 0,
    L_INET_ADDR_TYPE_IPV4       = 1,
    L_INET_ADDR_TYPE_IPV6       = 2,
    L_INET_ADDR_TYPE_IPV4Z      = 3,
    L_INET_ADDR_TYPE_IPV6Z      = 4,
    L_INET_ADDR_TYPE_DNS        = 16
}L_INET_AddrType_T;

typedef enum
{
    L_INET_FORMAT_IP_UNSPEC,                   /* {ipv4|ipv6}[%zone_id][/prefix_length]     */
    L_INET_FORMAT_IP_ADDR,                     /* {ipv4|ipv6}[%zone_id]                     */
    L_INET_FORMAT_IP_PREFIX,                   /* {ipv4|ipv6}[%zone_id]/prefix_length       */
    L_INET_FORMAT_IPV4_UNSPEC,                 /* ipv4[%zone_id][/prefix_length]            */
    L_INET_FORMAT_IPV4_ADDR,                   /* ipv4[%zone_id]                            */
    L_INET_FORMAT_IPV4_PREFIX,                 /* ipv4[%zone_id]/prefix_length              */
    L_INET_FORMAT_IPV6_UNSPEC,                 /* ipv6[%zone_id][/prefix_length]            */
    L_INET_FORMAT_IPV6_ADDR,                   /* ipv6[%zone_id]                            */
    L_INET_FORMAT_IPV6_PREFIX,                 /* ipv6[%zone_id]/prefix_length              */
    L_INET_FORMAT_IPV6_GLOBAL,                 /* ipv6                                      */
    L_INET_FORMAT_IPV6_GLOBAL_PREFIX,          /* ipv6/prefix_length                        */
    L_INET_FORMAT_IPV6_LINK_LOCAL,             /* ipv6%zone_id                              */
    L_INET_FORMAT_IPV6_LINK_LOCAL_NO_SCOPE_ID, /* ipv6(link local)                          */
    L_INET_FORMAT_IPV6_ADDR_RAW,               /* ipv6(global | link local)[/prefix_length] */
}L_INET_FORMAT_T;

typedef enum
{
    L_INET_RETURN_SUCCESS = 1,
    L_INET_RETURN_FAIL,
}L_INET_RETURN_VALUE_T;

/* Declares for internet address parameter to pass between different components.
 */
typedef struct
{
    UI16_T type;
    UI16_T addrlen;

    UI8_T addr[4];
}L_INET_Addr_T;

typedef struct
{
    UI16_T type;
    UI16_T addrlen;
    UI8_T addr[16];
    UI32_T zoneid;
    UI16_T preflen;
}L_INET_AddrIp_T;

typedef struct
{
    UI16_T type;
    UI16_T addrlen;

    UI8_T addr[L_INET_ADDR_DNS_MAX_STR_LEN + 1];
}L_INET_AddrDns_T;

/* EXPORTED FUNCTION PROTOTYPE
 */
/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_INET_FillInetAddrIp
 *-------------------------------------------------------------------------
 * PURPOSE : Fill a L_INET_AddrIp_T structure with suitable arguments
 * INPUT   : af         -- address family
 *           addr_p     -- binary IPv4/v6 address
 *           prefix_len -- prefix length
 * OUTPUT  : ipaddr_p   -- output L_INET_AddrIp_T object
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
    L_INET_AddrIp_T *ipaddr_p
);

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
    UI32_T *ip
);

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
    UI8_T cp[18]
);

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
    UI8_T *addr_p
);

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
    int af,
    UI8_T *addr_p,
    char *str_p,
    UI32_T str_len
);

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
    UI32_T cb_str
);

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
 * NOTE    : Different with L_INET_InaddrToString()
 *           1. not support DNS & zoneid
 *           2. prefix string is added even if prefix len = 0
 *-------------------------------------------------------------------------
 */
BOOL_T
L_INET_InprefixToString(
    const L_INET_Addr_T *in_p,
    UI32_T str_len,
    UI8_T *str_p
);

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
    UI32_T cb_inaddr
);

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
    L_INET_AddrIp_T *ipaddr_p
);

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
    struct sockaddr *sockaddr_p
);

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
    UI32_T compare_flag
);

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
    const L_INET_AddrIp_T *p
);

/* There is a bug in __cmsg_nxthdr() function defined in cmsg_nxthdr.c
 * within uClibc 0.9.27. This defect will cause the last cmsg couldn't
 * be retrieve from msghdr. Since this library function is declared as
 * libc_hidden_proto(__cmsg_nxthdr), which means it can be overwritten
 * by declaring a new and correct function with the same name.
 * If we use newer version of uClibc, this duplicated function can be removed then.
 * One of the places using this function is: pal_sock_in6_cmsg_lookup()
 * in pal_sock_raw.c (the CMSG_NXTHDR macro)
 */
#ifndef __KERNEL__
struct cmsghdr *
__cmsg_nxthdr (struct msghdr *mhdr, struct cmsghdr *cmsg);
#endif

#endif  /* L_INET_H */

