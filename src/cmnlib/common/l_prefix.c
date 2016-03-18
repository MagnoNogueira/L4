/*---------------------------------------------------------------------
 * File_Name : l_prefix.c
 *
 * Purpose   : Provide prefix operations
 *
 * History:
 *       Date       --  Modifier,   Reason
 *      2008.01.17  --  Vai Wang,   First Created.
 *
 *
 * Note    : Ported from Zebra
 *---------------------------------------------------------------------
 */
/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "sys_type.h"
#include "sys_cpnt.h"
#include "l_stdlib.h"
#include "sys_pal.h"
#include "l_prefix.h"
#include "l_inet.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* STATIC VARIABLE DECLARATIONS
 */
/* Utility mask array. */
static UI8_T maskbit[] = 
{
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */


UI8_T *L_PREFIX_Ntoa(UI8_T addr[16], UI8_T *ip_str)
{
    //int family;
    UI8_T *cp;

      //cp = (UI8_T *)&ip_str;
    cp = ip_str;
       //sprintf((char *)cp, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",cp[0],cp[1],cp[2],cp[3],cp[4],cp[5],cp[6],cp[7],cp[8],cp[9],cp[10],cp[11],cp[12],cp[13],cp[14],cp[15]);
    sprintf((char *)cp, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5],addr[6],addr[7],addr[8],addr[9],addr[10],addr[11],addr[12],addr[13],addr[14],addr[15]);

        return cp;

    //return pal_inet_ntop(family, &(address->addr), ip_str, 40);
}

// Convert Hexidecimal ASCII digits to integer
int L_PREFIX_AHtoI(char *token)
{
  int result=0, value_added=0, i=0;

   do {
   if((*(token+i) >= '0') && (*(token+i) <= '9'))
    value_added = (int) (*(token+i) - 48);
   else if((*(token+i) >= 'a') && (*(token+i) <= 'f'))
    value_added = (int) (*(token+i) - 87);
   else if((*(token+i) >= 'A') && (*(token+i) <= 'F'))
    value_added = (int) (*(token+i) - 55);
   else
    return -1;
   result = result * 16 + value_added;
   i++;
  } while(*(token+i) != '\0');

   if(result < 0 || result > 255)
    return -1;
   return result;
}

#if 0
int L_PREFIX_AtoIPV6(UI8_T *s, UI8_T *ip)
{
        UI8_T token[50];
        int   i,j;  /* i for s[]; j for token[] */
        int   k,l;  /* k for ip[]; l for copying coutner */
    
        UI8_T temp[20];
    
        i = 0;
        j = 0;
        k = 0;
        l = 0;

        while (s[i] != '\0')
        {
            if ((s[i] == ':') || (j == 2))
            {
                 token[j] = '\0';
        
                if (strlen((char *)token) < 1 || strlen((char *)token) > 2 ||
                    L_PREFIX_AHtoI((char *)token) < 0 || L_PREFIX_AHtoI((char *)token) > 255) // Invalid Token
                    return 0;
                else if (k >= 16)  // Too many digits
                    return 0;
                else // token is ready
                {
                    temp[k++] =(UI8_T)L_PREFIX_AHtoI((char *)token);
		    if(s[i] == ':')
                     i++; 
		    j = 0;
                }
            }
            else if ((s[i] < '0' || s[i] > '9') && (s[i] < 'a' || s[i] > 'f') && (s[i] < 'A' || s[i] > 'F'))
                return 0;
            else
                token[j++] = s[i++];
        } /* while */

        token[j] = '\0';

        if (strlen((char *)token) < 1 || strlen((char *)token) > 2 ||
            L_PREFIX_AHtoI((char *)token) < 0 || L_PREFIX_AHtoI((char *)token) > 255) // Invalid Token
            return 0;
        else if (k >= 16)  // Too many digits
            return 0;
 
   
        temp[k]=(UI8_T)L_PREFIX_AHtoI((char *)token);
        
        for(l=0;l<16;l++)
         ip[l] = temp[l];
    
        return 1;

}
#endif
//static BOOL_T L_INET_Pton6(const UI8_T *strptr, UI8_T *addrptr)
int L_PREFIX_AtoIPV6(const UI8_T *strptr, UI8_T *addrptr)
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
                /*if (L_IPV4V6_Inet_Pton(L_IPV4V6_IP_AF_INET, s, addrptr) == 1)*/
                if (L_INET_Aton(s, addrptr) == 1)
                {
                    len -= 2;
                    break;
                }
            }
            /* This could be a valid address */
            /* break; */ /* 2005-02-16, stj add + */
            /* This could be an invalid address */
            //goto errout;       /* Peter.guo, 2005/02/28, modify to support strict address check*/
            return FALSE;
        }
        if (s == strptr)
        {
            /* The address begins with a colon */
            if (*++s != ':')
                /* Must start with a double colon or a number */
                //goto errout;
                return FALSE;
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
                //goto errout;
                return FALSE;
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
               if(isdigit(*ptr++))
                     i++;
               else
                    //goto errout;
                    return FALSE;
            }
            /* Found all colons */
            if(i > 4)
                /*goto errout;*/
                return FALSE;

            break;
        }
    }

    if (colon_count == 0 || colon_count > 7)
        /*goto errout;*/
        return FALSE;

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
            /*&& L_IPV4V6_Inet_Pton(L_IPV4V6_IP_AF_INET, s, (UI8_T *) &in6_val[i]) == 1)*/
            && L_INET_Aton(s, (UI8_T *) &in6_val[i]) == TRUE)
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



/* Purpose: Convert Address Famiy Identifier to Address Family
 * Input:  afi -- Address Family Identifier
 * Output: None
 * Return: Address Family (usually indentified in TCP/IP stack)
 * Notes:  None
 */
s_int32_t L_PREFIX_Afi2Family (s_int32_t afi)
{
    if (afi == AFI_IP)
        return AF_INET;
#if (SYS_CPNT_INET6 == TRUE)
    else if (afi == AFI_IP6)
        return AF_INET6;
#endif /* SYS_CPNT_INET6 */
    return 0;
}


/* Purpose: Convert Address Address Family to Famiy Identifier
 * Input:  family -- Address Family (usually indentified in TCP/IP stack)
 * Output: None
 * Return: Address Family Identifier
 * Notes:  None
 */
s_int32_t L_PREFIX_Family2Afi (s_int32_t family)
{
    if (family == AF_INET)
        return AFI_IP;
#if (SYS_CPNT_INET6 == TRUE)
    else if (family == AF_INET6)
        return AFI_IP6;
#endif /* SYS_CPNT_INET6 */
    return 0;
}



/* Purpose: Generic function for conversion string to L_PREFIX_T
 * Input:  str
 * Output: p
 * Return: 0 -- converstion fail
 *         1 -- converstion success
 * Notes:  None
 */
s_int32_t L_PREFIX_Str2Prefix (const char *str, L_PREFIX_T *p)
{
    s_int32_t ret;

    /* First we try to convert string to struct prefix_ipv4. */
    ret = L_PREFIX_Str2IPv4Prefix(str, (L_PREFIX_IPv4_T *) p);
    if (ret)
        return ret;

#if (SYS_CPNT_INET6 == TRUE)
    /* Next we try to convert string to struct prefix_ipv6. */
    ret = L_PREFIX_Str2IPv6Prefix (str, (L_PREFIX_IPv6_T *) p);
    if (ret)
        return ret;
#endif /* SYS_CPNT_INET6 */

    return 0;
}


/* Purpose: Generic function for conversion L_PREFIX_T to string
 * Input:  p
 *         size
 * Output: str
 * Return: 0 -- converstion success
 * Notes:  None
 */
s_int32_t L_PREFIX_Prefix2Str (L_PREFIX_T *p, char *str, s_int32_t size)
{
    char buf[INET6_ADDRSTRLEN];

    inet_ntop (p->family, &p->u.prefix, buf, INET6_ADDRSTRLEN);
    pal_snprintf (str, size, "%s/%d", buf, p->prefixlen);
    return 0;
}


/* Purpose: Conversion A.B.C.D A.B.C.D(mask) to A.B.C.D/M format
 * Input:  prefix_str -- A.B.C.D
 *         mask_str -- A.B.C.D
 * Output: ip_str -- A.B.C.D/M
 * Return: 0 -- converstion fail
 *         1 -- converstion success
 * Notes:  None
 */
int L_PREFIX_StrMask2IpStr(char *prefix_str, char *mask_str,
               char *ip_str)
{
    struct pal_in4_addr mask;
    u_int8_t masklen = 0;
    int ret = 0;

    pal_mem_set (&mask, 0, sizeof (struct pal_in4_addr));
    pal_mem_set (ip_str, 0, IPV4_PREFIX_STR_MAX_LEN);

    /* Convert string to prefix. */
    ret = inet_pton(AF_INET, mask_str, &mask);
    if (ret <= 0)
        return 0;

    masklen = L_PREFIX_IPv4MaskLen (mask);
    if (masklen > IPV4_MAX_BITLEN)
        return 0;

    pal_snprintf (ip_str, IPV4_PREFIX_STR_MAX_LEN, "%s/%d",
            prefix_str, masklen);

    return 1;
}


/* Purpose: Allocate memory to create a L_PREFIX_T
 * Input:  None
 * Output: None
 * Return: prefix
 * Notes:  None
 */
L_PREFIX_T *L_PREFIX_AllocPrefix(void)
{
    L_PREFIX_T *prefix = NULL;

    prefix = (L_PREFIX_T *)malloc(sizeof(L_PREFIX_T));
    if (NULL != prefix)
        memset(prefix, 0, sizeof(L_PREFIX_T));
    
    return prefix;
}


/* Purpose: Free memory space of a L_PREFIX_T
 * Input:  p
 * Output: None
 * Return: None
 * Notes:  None
 */
void L_PREFIX_FreePrefix(L_PREFIX_T *p)
{
    if (NULL != p)
        free(p);

    return;
}


/* Purpose: Allocate memory to create a L_PREFIX_IPv4_T
 * Input:  None
 * Output: None
 * Return: prefix_ipv4
 * Notes:  None
 */
L_PREFIX_IPv4_T *L_PREFIX_AllocIPv4Prefix(void)
{
    L_PREFIX_IPv4_T *prefix_ipv4 = NULL;

    prefix_ipv4 = (L_PREFIX_IPv4_T *)malloc(sizeof(L_PREFIX_IPv4_T));
    if (NULL != prefix_ipv4)
    {
        memset(prefix_ipv4, 0, sizeof (L_PREFIX_IPv4_T));
        prefix_ipv4->family = AF_INET;
    }

    return prefix_ipv4;
}


/* Purpose: Free memory space of a L_PREFIX_IPv4_T
 * Input:  p
 * Output: None
 * Return: None
 * Notes:  None
 */
void L_PREFIX_FreeIPv4Prefix (L_PREFIX_IPv4_T *p)
{
    if (NULL != p)
        free(p);

    return;
}


/* Purpose: Allocate memory to create a L_PREFIX_IPv6_T
 * Input:  None
 * Output: None
 * Return: prefix_ipv6
 * Notes:  None
 */
L_PREFIX_IPv6_T *L_PREFIX_AllocIPv6Prefix(void)
{
    L_PREFIX_IPv6_T *prefix_ipv6 = NULL;

    prefix_ipv6 = (L_PREFIX_IPv6_T *)malloc(sizeof(L_PREFIX_IPv6_T));
    if (NULL != prefix_ipv6)
    {
        memset(prefix_ipv6, 0, sizeof (L_PREFIX_IPv6_T));
        prefix_ipv6->family = AF_INET;
    }

    return prefix_ipv6;
}


/* Purpose: Free memory space of a L_PREFIX_IPv6_T
 * Input:  p
 * Output: None
 * Return: None
 * Notes:  None
 */
void L_PREFIX_FreeIPv6Prefix (L_PREFIX_IPv6_T *p)
{
    if (NULL != p)
        free(p);

    return;
}


/* Purpose: Generic function for conversion string to L_PREFIX_IPv4_T
 * Input:  str
 * Output: p
 * Return: 0 -- converstion fail
 *         1 -- converstion success
 * Notes:  None
 */
s_int32_t L_PREFIX_Str2IPv4Prefix (const char *str, L_PREFIX_IPv4_T *p)
{
    u_int32_t network;
    s_int32_t plen;
    s_int32_t ret;
    char *pnt;
    char cp[INET_ADDRSTRLEN];

    /* Find slash inside string. */
    pnt = pal_strchr (str, '/');

    /* String doesn't contail slash. */
    if (pnt == NULL) 
    {
        /* Convert string to prefix. */
        ret = inet_pton(AF_INET,str,&p->prefix);
        if (ret <= 0)
            return 0;

        p->family = AF_INET;

        /* Natural Mask is derived for network without a mask */
        network = pal_ntoh32 (p->prefix.s_addr);

        if (IN_CLASSA (network))
        {
            if ((network & IN_CLASSA_NET) == network)
                p->prefixlen = IN_CLASSA_PREFIXLEN;
            else
                p->prefixlen = IPV4_MAX_PREFIXLEN;
        }
        else if (IN_CLASSB (network))
        {
            if ((network & IN_CLASSB_NET) == network)
                p->prefixlen = IN_CLASSB_PREFIXLEN;
            else
                p->prefixlen = IPV4_MAX_PREFIXLEN;
        }
        else if (IN_CLASSC (network))
        {
            if ((network & IN_CLASSC_NET) == network)
                p->prefixlen = IN_CLASSC_PREFIXLEN;
            else
                p->prefixlen = IPV4_MAX_PREFIXLEN;
        }
        else if (IN_CLASSD (network))
            p->prefixlen = IPV4_MAX_PREFIXLEN;
        else if (IN_BADCLASS (network))
            return 0;
        else
            return 0;

        return 1;
    }
    else
    {
        pal_strncpy (cp, str, pnt - str);
        *(cp + (pnt - str)) = '\0';
        ret = inet_pton (AF_INET,cp, &p->prefix);

        if (ret <= 0)
            return 0;

        /* Get prefix length. */
        plen = (u_int8_t) pal_strtou32 (++pnt,NULL,10);
        if (plen > IPV4_MAX_PREFIXLEN)
            return 0;

        p->family = AF_INET;
        p->prefixlen = plen;
    }

    return 1;
}


/* Purpose: Generic function for conversion string to L_PREFIX_IPv6_T
 * Input:  str
 * Output: p
 * Return: 0 -- converstion fail
 *         1 -- converstion success
 * Notes:  None
 */
 
s_int32_t L_PREFIX_Str2IPv6Prefix (const char *str, L_PREFIX_IPv6_T *p)
{
    char *pnt;
    char cp[INET6_ADDRSTRLEN];
    s_int32_t ret;

    pnt = pal_strchr (str, '/');

    /* If string doesn't contain `/' treat it as host route. */
    if (pnt == NULL) 
    {
        ret = L_PREFIX_AtoIPV6 ( str, (void*)&p->prefix);
        if (ret <= 0)
            return 0;
        p->prefixlen = IPV6_MAX_BITLEN;
    }
    else 
    {
        s_int32_t plen;
        
        pal_strncpy (cp, str, pnt - str);
        *(cp + (pnt - str)) = '\0';
        ret = L_PREFIX_AtoIPV6(cp, (void*)&p->prefix);
        if (ret <= 0)
            return 0;
        plen = (u_int8_t) pal_strtou32 (++pnt,NULL,10);
        if (plen > IPV6_MAX_BITLEN)
            return 0;
        p->prefixlen = plen;
    }
    p->family = AF_INET6;

    return 1;
}


/* Purpose: Generic function for conversion L_PREFIX_IPv4_T to string
 * Input:  p
 *         size
 * Output: str
 * Return: 0 -- converstion success
 * Notes:  None
 */
s_int32_t L_PREFIX_IPv4Prefix2Str(L_PREFIX_IPv4_T *p, char *str, s_int32_t size)
{
    char buf[INET_ADDRSTRLEN];

    inet_ntop (p->family, &p->prefix, buf, INET_ADDRSTRLEN);
    pal_snprintf (str, size, "%s/%d", buf, p->prefixlen);
    return 0;
}


/* Purpose: Convert IP address's netmask into integer
 * Input:  netmask
 * Output: None
 * Return: mask length
 * Notes:  We assume netmask is sequential one. 
 *         Argument netmask should be network byte order.
 */
u_int8_t L_PREFIX_IPv4MaskLen (struct pal_in4_addr netmask)
{
    u_int8_t len;
    u_int8_t *pnt;
    u_int8_t *end;
    u_int8_t val;

    len = 0;
    pnt = (u_int8_t *) &netmask;
    end = pnt + IPV4_MAX_BYTELEN;

    while ((*pnt == 0xff) && pnt < end)
    {
        len+= PNBBY;
        pnt++;
    } 

    if (pnt < end)
    {
        val = *pnt;
        while (val)
        {
            len++;
            val <<= 1;
        }
    }
    return len;
}


/* Purpose: Convert struct pal_in6_addr netmask into integer
 * Input:  netmask
 * Output: None
 * Return: mask length
 * Notes:  We assume netmask is sequential one. 
 *         Argument netmask should be network byte order.
 */
s_int32_t L_PREFIX_IPv6MaskLen (struct pal_in6_addr netmask)
{
    s_int32_t len = 0;
    char val;
    char *pnt;

    pnt = (char *) & netmask;

    while ((*pnt == 0xff) && len < IPV6_MAX_PREFIXLEN) 
    {
        len += PNBBY;
        pnt++;
    } 

    if (len < IPV6_MAX_PREFIXLEN) 
    {
        val = *pnt;
        while (val) 
        {
            len++;
            val <<= 1;
        }
    }
    return len;
}


/* Purpose: Convert masklen into IP address's netmask.
 * Input:  masklen
 * Output: netmask
 * Return: None
 * Notes:  None
 */
void L_PREFIX_MaskLen2IPv4(s_int32_t masklen, struct pal_in4_addr *netmask)
{
    u_int8_t *pnt;
    s_int32_t bit;
    s_int32_t offset;

    pal_mem_set (netmask, 0, sizeof (struct pal_in4_addr));
    pnt = (u_int8_t *) netmask;

    offset = masklen / PNBBY;
    bit = masklen % PNBBY;

    while (offset--)
        *pnt++ = 0xff;

    if (bit)
        *pnt = maskbit[bit];
}


/* Purpose: Convert masklen into IPv6 address's netmask.
 * Input:  masklen
 * Output: netmask
 * Return: None
 * Notes:  None
 */
void L_PREFIX_MaskLen2IPv6(s_int32_t masklen, struct pal_in6_addr *netmask)
{
    char *pnt;
    int bit;
    int offset;

    pal_mem_set (netmask, 0, sizeof (struct pal_in6_addr));
    pnt = (char *) netmask;

    offset = masklen / PNBBY;
    bit = masklen % PNBBY;

    while (offset--)
        *pnt++ = 0xff;

    if (bit)
    *pnt = maskbit[bit];
}


/* Purpose: Utility function to convert ipv4 prefixes to Classful prefixes.
 * Input:  p
 * Output: p
 * Return: None
 * Notes:  None
 */
void L_PREFIX_ApplyIPv4MaskClassful (L_PREFIX_IPv4_T *p)
{
    u_int32_t destination;

    destination = pal_ntoh32 (p->prefix.s_addr);

    if (p->prefixlen == IPV4_MAX_PREFIXLEN);
    /* do nothing for host routes */
    else if (IN_CLASSC (destination)) 
    {
        p->prefixlen = IN_CLASSC_PREFIXLEN;
        L_PREFIX_ApplyMaskIPv4(p);
    }
    else if (IN_CLASSB(destination)) 
    {
        p->prefixlen = IN_CLASSB_PREFIXLEN;
        L_PREFIX_ApplyMaskIPv4(p);
    }
    else 
    {
        p->prefixlen = IN_CLASSA_PREFIXLEN;
        L_PREFIX_ApplyMaskIPv4(p);
    }
}


/* Purpose: Return prefix family type string.
 * Input:  p
 * Output: None
 * Return: Family type string
 * Notes:  None
 */
const char *L_PREFIX_Family2Str (L_PREFIX_T *p)
{
  if (p->family == AF_INET)
    return "inet";
#if (SYS_CPNT_INET6 == TRUE)
  if (p->family == AF_INET6)
    return "inet6";
#endif /* SYS_CPNT_INET6 */
  return "unspec";
}


/* Purpose: Generic function for conversion string to pal_in6_addr
 * Input:  str
 * Output: addr
 * Return: None
 * Notes:  None
 */
void L_PREFIX_Str2In6Addr(char *str, struct pal_in6_addr *addr)
{
    s_int32_t i;
    u_int32_t x;

    /* %x must point to unsinged int */
    for (i = 0; i < IPV6_MAX_BYTELEN; i++)
    {
        pal_sscanf (str + (i * 2), "%02x", &x);
        addr->s6_addr[i] = x & 0xff;
    }
}

 
/* Purpose: copy prefix from src to dst
 * Input:  dst, src
 * Output: dst
 * Notes:  None
 */
void L_PREFIX_Copy(L_PREFIX_T *dst, L_PREFIX_T *src)
{
    dst->family = src->family;
    dst->prefixlen = src->prefixlen;

    if (src->family == AF_INET)
        dst->u.prefix4 = src->u.prefix4;
#if (SYS_CPNT_INET6 == TRUE)
    else if (src->family == AF_INET6)
        dst->u.prefix6 = src->u.prefix6;
#endif /* SYS_CPNT_INET6 */
    else if (src->family == AF_UNSPEC)
    {
        dst->u.lp.id = src->u.lp.id;
        dst->u.lp.adv_router = src->u.lp.adv_router;
    }
#ifdef AF_LOCAL
    else if (src->family == AF_LOCAL)
        pal_mem_cpy (dst->u.val, src->u.val, 9);
#endif /* AF_LOCAL */

    return;
}


/* Purpose: Check if n includes p
 * Input:  n, p
 * Output: None
 * Return: 1 -- n includes p
 *         0 -- n does not include p
 * Notes:  None
 */
int L_PREFIX_Match (L_PREFIX_T *n, L_PREFIX_T *p)
{
    s_int32_t offset;
    s_int32_t shift;
    u_int8_t *np, *pp; 

    if(n->family != p->family)
    return 0;
    // printf("%s[%d]\n", __FUNCTION__, __LINE__);

    np = (u_int8_t *)&n->u.prefix;
    pp = (u_int8_t *)&p->u.prefix;

    if (n->prefixlen > p->prefixlen)
        return 0;

    offset = n->prefixlen / PNBBY;
    shift = n->prefixlen % PNBBY;

    if (shift)
        if (maskbit[shift] & (np[offset] ^ pp[offset]))
            return 0;

    while (offset--)
        if (np[offset] != pp[offset])
            return 0;

    return 1;
}


/* Purpose: Compare two prefix
 * Input:  p1, p2
 * Output: None
 * Return: 0 -- same
 *         1 -- different
 * Notes:  When both prefix structure is not same, but will be same after
 *         applying mask, return 0. otherwise, return 1
 */
int L_PREFIX_Cmp (L_PREFIX_T *p1, L_PREFIX_T *p2)
{
    UI8_T offset;
    UI8_T shift;

    /* Set both prefix's head pointer. */
    UI8_T *pp1 = (UI8_T *)&p1->u.prefix;
    UI8_T *pp2 = (UI8_T *)&p2->u.prefix;

    if (p1->family != p2->family || p1->prefixlen != p2->prefixlen)
        return 1;

    offset = p1->prefixlen / PNBBY;
    shift = p1->prefixlen % PNBBY;

    if (shift)
        if (maskbit[shift] & (pp1[offset] ^ pp2[offset]))
            return 1;

    while (offset--)
        if (pp1[offset] != pp2[offset])
            return 1;

    return 0;
}


/* Purpose: Check two prefix structure
 * Input:  p1, p2
 * Output: None
 * Return: 1 -- same
 *         0 -- different
 * Notes:  None
 */
int L_PREFIX_PrefixSame (L_PREFIX_T *p1, L_PREFIX_T *p2)
{
    if (p1->family == p2->family && p1->prefixlen == p2->prefixlen)
    {
        if (p1->family == AF_INET)
            if (IPV4_ADDR_SAME (&p1->u.prefix, &p2->u.prefix))
                return 1;
#if (SYS_CPNT_INET6 == TRUE)
        if (p1->family == AF_INET6 )
            if (IPV6_ADDR_SAME (&p1->u.prefix, &p2->u.prefix))
                return 1;
#endif /* SYS_CPNT_INET6 */
    }
    return 0;
}


/* Purpose: Check two prefix address
 * Input:  p1, p2
 * Output: None
 * Return: 1 -- same
 *         0 -- different
 * Notes:  None
 */
int L_PREFIX_PrefixAddrSame (L_PREFIX_T *p1, L_PREFIX_T *p2)
{
    if (p1->family == p2->family)
    {
        if (p1->family == AF_INET)
            if (IPV4_ADDR_SAME (&p1->u.prefix, &p2->u.prefix))
                return 1;
#if (SYS_CPNT_INET6 == TRUE)
        if (p1->family == AF_INET6 )
            if (IPV6_ADDR_SAME (&p1->u.prefix, &p2->u.prefix))
                return 1;
#endif /* SYS_CPNT_INET6 */
    }
    return 0;
}


/* Purpose: Creates a common prefix of n and p in new.
 * Input:  n, p
 * Output: new
 * Return: None
 * Notes:  n and new can be the same pointers since p is always
 *         used in the function for comparision
 */
void L_PREFIX_PrefixCommon (L_PREFIX_T *n, L_PREFIX_T *p, L_PREFIX_T *new)
{
    int i;
    u_char diff;
    u_char mask;

    u_char *np = (u_char *)&n->u.prefix;
    u_char *pp = (u_char *)&p->u.prefix;
    u_char *newp = (u_char *)&new->u.prefix;

    for (i = 0; i < p->prefixlen / PNBBY; i++)
    {
        if (np[i] == pp[i])
            newp[i] = np[i];
        else
            break;
    }

    new->prefixlen = i * PNBBY;

    if (new->prefixlen != p->prefixlen)
    {
        diff = np[i] ^ pp[i];
        mask = 0x80;
        while (new->prefixlen < p->prefixlen && !(mask & diff))
        {
            mask >>= 1;
            new->prefixlen++;
        }
        newp[i] = np[i] & maskbit[new->prefixlen % PNBBY];
    }
}


/* Purpose: Apply mask
 * Input:  p
 * Output: None
 * Return: None
 * Notes:  None
 */
void L_PREFIX_ApplyMask(L_PREFIX_T *p)
{
    switch (p->family)
    {
        case AF_INET:
            L_PREFIX_ApplyMaskIPv4 ((struct prefix_ipv4 *)p);
            break;
#if (SYS_CPNT_INET6 == TRUE)
        case AF_INET6:
            L_PREFIX_ApplyMaskIPv6 ((struct prefix_ipv6 *)p);
            break;
#endif /* SYS_CPNT_INET6 */
        default:
            break;
    }

    return;
}


/* Purpose: Apply IPv4 mask
 * Input:  p
 * Output: None
 * Return: None
 * Notes:  None
 */
void L_PREFIX_ApplyMaskIPv4(L_PREFIX_IPv4_T *p)
{
    UI8_T *pnt;
    UI8_T index;
    UI8_T offset;

    index = p->prefixlen / PNBBY;

    if (index < IPV4_MAX_BYTELEN)
    {
        pnt = (UI8_T *)&p->prefix;
        offset = p->prefixlen % PNBBY;

        pnt[index] &= maskbit[offset];
        index++;

        while (index < IPV4_MAX_BYTELEN)
            pnt[index++] = 0;
    }

    return;
}


/* Purpose: Apply IPv6 mask
 * Input:  p
 * Output: None
 * Return: None
 * Notes:  None
 */
void L_PREFIX_ApplyMaskIPv6 (L_PREFIX_IPv6_T *p)
{
    u_int8_t *pnt;
    s_int32_t index;
    s_int32_t offset;

    index = p->prefixlen / PNBBY;

    if (index < IPV6_MAX_BYTELEN)
    {
        pnt = (u_int8_t *) &p->prefix;
        offset = p->prefixlen % PNBBY;

        pnt[index] &= maskbit[offset];
        index++;

        while (index < IPV6_MAX_BYTELEN)
            pnt[index++] = 0;
    }
}


/* Purpose: Convert ip & mask to prefix
 * Input:  ipv4_addr, ipv4_mask, p
 * Output: p
 * Return: None
 * Notes:  None
 */
BOOL_T L_PREFIX_InetAddr2Prefix (UI32_T ipv4_addr, UI32_T ipv4_mask, L_PREFIX_T *p)
{
    L_PREFIX_IPv4_T *inet_p;

    if (NULL == p)
        return FALSE;

    inet_p = (L_PREFIX_IPv4_T *)p;

    inet_p->family = AF_INET;
    inet_p->prefixlen = L_PREFIX_Ip2MaskLen(ipv4_mask);
    inet_p->prefix.s_addr = ipv4_addr;

    return TRUE;
}

/* Purpose: Convert ip & mask to prefix
 * Input:  ipv4_addr, ipv4_mask, p
 * Output: p
 * Return: None
 * Notes:  None
 */
BOOL_T L_PREFIX_Inet6Addr2Prefix (UI8_T ipv6_addr[SYS_ADPT_IPV6_ADDR_LEN], UI16_T preflen, L_PREFIX_T *p)
{
    L_PREFIX_IPv6_T *inet6_p;

    if (NULL == p)
        return FALSE;

    inet6_p = (L_PREFIX_IPv6_T *)p;

    inet6_p->family = AF_INET6;
    inet6_p->prefixlen = (UI8_T)preflen;
    memcpy(inet6_p->prefix.s6_addr, ipv6_addr, SYS_ADPT_IPV6_ADDR_LEN*sizeof(char));

//    UI8_T ip_str[50] = {0};
//    printf("%s[%d]: preflen =%d\n", __FUNCTION__, __LINE__, inet6_p->prefixlen);
//    L_PREFIX_Ntoa(inet6_p->prefix.s6_addr, ip_str);
//    printf("%s[%d]: ipv6 addr=%s\n", __FUNCTION__, __LINE__, ip_str);
//    L_PREFIX_Ntoa(ipv6_addr, ip_str);
//    printf("%s[%d]: ipv6_addr=%s\n", __FUNCTION__, __LINE__, ip_str);

    return TRUE;
}



/* Purpose: Convert mask to mask length
 * Input:  ipv4_mask
 * Output: None
 * Return: Mask length
 * Notes:  None
 */
UI8_T L_PREFIX_Ip2MaskLen (UI32_T ipv4_mask)
{
    UI8_T len;
    UI8_T *pnt;
    UI8_T *end;
    UI8_T val;

    len = 0;
    pnt = (UI8_T *)&ipv4_mask;
    end = pnt + IPV4_MAX_BYTELEN;

    while ((*pnt == 0xff) && pnt < end)
    {
        len += PNBBY;
        pnt++;
    }

    if (pnt < end)
    {
        val = *pnt;
        while (val)
        {
            len++;
            val <<= 1;
        }
    }

    return len;
}


/* Purpose: Convert mask length to ip mask
 * Input:  Mask length
 * Output: ipv4_mask
 * Return: None
 * Notes:  None
 */
void L_PREFIX_MaskLen2Ip(UI8_T masklen, UI32_T *ipv4_mask)
{
  UI8_T *pnt;
  UI32_T bit;
  UI32_T offset;

  memset (ipv4_mask, 0, sizeof (UI32_T));
  pnt = (UI8_T *) ipv4_mask;

  offset = masklen / PNBBY;
  bit = masklen % PNBBY;
  
  while (offset--)
    *pnt++ = 0xff;

  if (bit)
    *pnt = maskbit[bit];
}


/* Purpose: Check the string only contains digit character
 * Input:  str
 * Output: None
 * Return: 1 -- digit characters
 *         0 -- contain non-digit character
 * Notes:  None
 */
s_int32_t L_PREFIX_IsAllDigit (char *str)
{
    for (; *str != '\0'; str++)
        if (!pal_char_isdigit ((s_int32_t) *str))
            return 0;
        
    return 1;
}

