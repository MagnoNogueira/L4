/*---------------------------------------------------------------------
 * File_Name : l_prefix.h
 *
 * Purpose   : Provide defination of prefix family and prefix operations
 *
 * History:
 *       Date       --  Modifier,   Reason
 *      2008.01.17  --  Vai Wang,   First Created.
 *
 *
 * Note    : Ported from Zebra
 *---------------------------------------------------------------------
 */
#ifndef L_PREFIX_H
#define L_PREFIX_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_pal.h"
#include "sys_cpnt.h"
#include "sys_adpt.h"
#include <netinet/in.h>
#include <sys/socket.h>

/* NAMING CONSTANT DECLARATIONS
 */
/* Number of bits in prefix type. */
#ifndef PNBBY
#define PNBBY 8
#endif /* PNBBY */

/* Unix Domain Socket Name Length */
#define UDS_NAME_SPACE   9

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif /* INET_ADDRSTRLEN */

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif /* INET6_ADDRSTRLEN */

#ifndef INET6_BUFSIZ
#define INET6_BUFSIZ 51
#endif /* INET6_BUFSIZ */

#ifndef INET_NTOP_BUFSIZ
#define INET_NTOP_BUFSIZ 51
#endif /* INET_NTOP_BUFSIZ */

/* Max bit/byte length of IPv4 address. */
#define IPV4_MAX_BYTELEN    4
#define IPV4_MAX_BITLEN    32
#define IPV4_MAX_PREFIXLEN 32
#define IPV4_CLASS_C_PREFIXLEN 24
#define IPV4_ADDR_CMP(D,S)   (pal_mem_cmp ((D), (S), IPV4_MAX_BYTELEN))
#define IPV4_ADDR_SAME(D,S)  (pal_mem_cmp ((D), (S), IPV4_MAX_BYTELEN) == 0)
#define IPV4_ADDR_COPY(D,S)  (pal_mem_cpy ((D), (S), IPV4_MAX_BYTELEN))
#define IPV4_ADDR_MARTIAN(X)                                          \
  ((IN_CLASSA (X)                                                     \
    && ((((u_int32_t) (X)) & IN_CLASSA_NET) == 0x00000000L            \
        || (((u_int32_t) (X)) & IN_CLASSA_NET) == 0x7F000000L))       \
   || (IN_CLASSB (X)                                                  \
       && ((((u_int32_t) (X)) & IN_CLASSB_NET) == 0x80000000L         \
           || ((((u_int32_t) (X))) & IN_CLASSB_NET) == 0xBFFF0000L))  \
   || (IN_CLASSC (X)                                                  \
       && ((((u_int32_t) (X)) & IN_CLASSC_NET) == 0xC0000000L         \
           || ((((u_int32_t) (X))) & IN_CLASSC_NET) == 0xDFFFFF00L)))

#define IPV4_NET0(a)    ((((u_int32_t) (a)) & 0xff000000) == 0x00000000)
#define IPV4_NET127(a)  ((((u_int32_t) (a)) & 0xff000000) == 0x7f000000)
#define IPV6_LOOPBACK(a)  ((((u_int32_t) (a)) & 0xffffffff) == 0x00000001)

#define CLASS_A_BROADCAST(a)   ((((u_int32_t) (a)) & 0x00ffffff) == 0x00ffffff)
#define CLASS_B_BROADCAST(a)   ((((u_int32_t) (a)) & 0x0000ffff) == 0x0000ffff)
#define CLASS_C_BROADCAST(a)   ((((u_int32_t) (a)) & 0x000000ff) == 0x000000ff)

#define IN_CLASSA_PREFIXLEN    8
#define IN_CLASSB_PREFIXLEN    16
#define IN_CLASSC_PREFIXLEN    24

/* Max bit/byte length of IPv6 address. */
#define IPV6_MAX_BYTELEN    16
#define IPV6_MAX_BITLEN    128
#define IPV6_MAX_PREFIXLEN 128
#define IPV6_ADDR_CMP(D,S)   (pal_mem_cmp ((D), (S), IPV6_MAX_BYTELEN))
#define IPV6_ADDR_SAME(D,S)  (pal_mem_cmp ((D), (S), IPV6_MAX_BYTELEN) == 0)
#define IPV6_ADDR_COPY(D,S)  (pal_mem_cpy ((D), (S), IPV6_MAX_BYTELEN))
#define IPV6_ADDR_SET(addr) (pal_mem_set(addr, 0, IPV6_MAX_BYTELEN))
/* IPV4 prefix string maximum length */
#define IPV4_PREFIX_STR_MAX_LEN               19


#if (SYS_CPNT_INET6 == TRUE)
#define IS_PREFIX_MAX_BITLEN(P)                                               \
    ((P)->family == AF_INET ? (P)->prefixlen == IPV4_MAX_BITLEN :             \
     (P)->family == AF_INET6 ? (P)->prefixlen == IPV6_MAX_BITLEN : 0)
#else
#define IS_PREFIX_MAX_BITLEN(P)                                               \
    ((P)->family == AF_INET ? (P)->prefixlen == IPV4_MAX_BITLEN : 0)
#endif /* SYS_CPNT_INET6 */

#if (SYS_CPNT_INET6 == TRUE)
#define PREFIX_MAX_BITLEN(family)                                             \
    ((family) == AF_INET ? IPV4_MAX_BITLEN :                                  \
     (family) == AF_INET6 ? IPV6_MAX_BITLEN : 0)
#else
#define PREFIX_MAX_BITLEN(family)                                             \
    ((family) == AF_INET ? IPV4_MAX_BITLEN : 0)
#endif /* SYS_CPNT_INET6 */

#if (SYS_CPNT_INET6 == TRUE)
#define PREFIX_MAX_BYTELEN(family)                                             \
    ((family) == AF_INET ? IPV4_MAX_BYTELEN :                                  \
     (family) == AF_INET6 ? IPV6_MAX_BYTELEN : 0)
#else
#define PREFIX_MAX_BYTELEN(family)                                             \
    ((family) == AF_INET ? IPV4_MAX_BYTELEN : 0)
#endif /* SYS_CPNT_INET6 */


/* MACRO FUNCTION DECLARATIONS
 */
#undef PSIZE
#define PSIZE(a) (((a) + 7) >> (3))

/* Prefix's family member. */
#define PREFIX_FAMILY(p)  ((p)->family)

/* Shift and convert to network-byte order. */
#define PREP_FOR_NETWORK(val,v,l)               \
{                                               \
  val = v;                                      \
  val = pal_hton32 (((val) << (IPV4_MAX_BITLEN - l))); \
}

/* Shift and convert to host-byte order. */
#define PREP_FOR_HOST(val,v,l)                    \
{                                                 \
  val = v;                                        \
  val = ((pal_ntoh32 (val)) >> (IPV4_MAX_BITLEN - l)); \
}


/* DATA TYPE DECLARATIONS
 */
/* IPv4 and IPv6 unified prefix structure. */
struct prefix
{
  u_int8_t family;
  u_int8_t prefixlen;
  u_int8_t pad1;
  u_int8_t pad2;
  union 
  {
    u_int8_t prefix;
    struct pal_in4_addr prefix4;
#if (SYS_CPNT_INET6 == TRUE)
    struct pal_in6_addr prefix6;
#endif /* SYS_CPNT_INET6 */
    struct 
    {
      struct pal_in4_addr id;
      struct pal_in4_addr adv_router;
    } lp;
    u_int8_t val[UDS_NAME_SPACE];
  } u;
};

/* IPv4 prefix structure. */
struct prefix_ipv4
{
  u_int8_t family;
  u_int8_t prefixlen;
  u_int8_t pad1;
  u_int8_t pad2;
  struct pal_in4_addr prefix;
};

struct prefix_ipv6
{
  u_int8_t family;
  u_int8_t prefixlen;
  u_int8_t pad1;
  u_int8_t pad2;
  struct pal_in6_addr prefix;
};

struct prefix_ls
{
  u_int8_t family;
  u_int8_t prefixlen;
  u_int8_t pad1;
  u_int8_t pad2;
  struct pal_in4_addr id;
  struct pal_in4_addr adv_router;
};

typedef struct prefix L_PREFIX_T;
typedef struct prefix_ipv4 L_PREFIX_IPv4_T;
typedef struct prefix_ipv6 L_PREFIX_IPv6_T;


/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
s_int32_t L_PREFIX_Afi2Family (s_int32_t);
s_int32_t L_PREFIX_Family2Afi (s_int32_t);

s_int32_t L_PREFIX_Prefix2Str (L_PREFIX_T *, char *, s_int32_t);
s_int32_t L_PREFIX_Str2Prefix (const char *, L_PREFIX_T *);

int L_PREFIX_StrMask2IpStr (char *, char *,char *);

L_PREFIX_T *L_PREFIX_AllocPrefix(void);
void L_PREFIX_FreePrefix(L_PREFIX_T *p);

L_PREFIX_IPv4_T *L_PREFIX_AllocIPv4Prefix(void);
void L_PREFIX_FreeIPv4Prefix (L_PREFIX_IPv4_T *p);

L_PREFIX_IPv6_T *L_PREFIX_AllocIPv6Prefix(void);
void L_PREFIX_FreeIPv6Prefix (L_PREFIX_IPv6_T *p);

s_int32_t L_PREFIX_Str2IPv4Prefix (const char *, L_PREFIX_IPv4_T *);
s_int32_t L_PREFIX_Str2IPv6Prefix(const char *, L_PREFIX_IPv6_T *);
s_int32_t L_PREFIX_IPv4Prefix2Str(L_PREFIX_IPv4_T *, char *, s_int32_t);

u_int8_t L_PREFIX_IPv4MaskLen (struct pal_in4_addr);
void L_PREFIX_MaskLen2IPv4(s_int32_t, struct pal_in4_addr *);

void L_PREFIX_ApplyIPv4MaskClassful(struct prefix_ipv4 *);

const char *L_PREFIX_Family2Str (L_PREFIX_T *);
void L_PREFIX_Str2In6Addr(char *str, struct pal_in6_addr *addr);
void L_PREFIX_MaskLen2IPv6(s_int32_t masklen, struct pal_in6_addr *netmask);
s_int32_t L_PREFIX_IPv6MaskLen (struct pal_in6_addr);

void L_PREFIX_Copy(L_PREFIX_T *dst, L_PREFIX_T *src);
int L_PREFIX_Match (L_PREFIX_T *n, L_PREFIX_T *p);
int L_PREFIX_Cmp (L_PREFIX_T *p1, L_PREFIX_T *p2);
int L_PREFIX_PrefixSame (L_PREFIX_T *p1, L_PREFIX_T *p2);
int L_PREFIX_PrefixAddrSame (L_PREFIX_T *p1, L_PREFIX_T *p2);
void L_PREFIX_ApplyMask(L_PREFIX_T *p);
void L_PREFIX_ApplyMaskIPv4(L_PREFIX_IPv4_T *p);
void L_PREFIX_ApplyMaskIPv6(L_PREFIX_IPv6_T *p);
BOOL_T L_PREFIX_InetAddr2Prefix (UI32_T ipv4_addr, UI32_T ipv4_mask, L_PREFIX_T *p);
BOOL_T L_PREFIX_Inet6Addr2Prefix (UI8_T *ipv6_addr, UI16_T preflen, L_PREFIX_T *p);
UI8_T L_PREFIX_Ip2MaskLen (UI32_T ipv4_mask);
void L_PREFIX_MaskLen2Ip(UI8_T masklen, UI32_T *ipv4_mask);
void L_PREFIX_PrefixCommon (L_PREFIX_T *n, L_PREFIX_T *p, L_PREFIX_T *new);
s_int32_t L_PREFIX_IsAllDigit (char *str);
#endif /* L_PREFIX_H */

