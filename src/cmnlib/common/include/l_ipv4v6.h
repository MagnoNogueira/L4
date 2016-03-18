/* MODULE NAME: l_ipv4v6.h
 * PURPOSE:
 * NOTES:
 * Modification History:
 *      Date                Modifier,           Reason
 *      ----------------------------------------------
 *      2006/10/31          peter_yu            create
 *
 * COPYRIGHT(C)         Accton Corporation, 2006
 */
#ifndef _L_IPV4V6_H
#define _L_IPV4V6_H



/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define L_IPV4V6_IP_AF_INET      2    /* IPv4 */
#define L_IPV4V6_IP_AF_LINK     18    /* Link layer address (used in route entries and ancillary data) */
#define L_IPV4V6_IP_AF_INET6    24    /* Default family value of IP version 6 */


#define IPV6_PREFIX_TEMP_BUFFER_LEN         64
#define IPV6_ADDREESS_STRING_MAX_LEN        (46 + 10)
/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */

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
BOOL_T L_IPV4V6_Is_Valid_IP_AddressString(char *str, UI32_T str_len);

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
BOOL_T L_IPV4V6_Is_Valid_IPv4_AddressString(char *str, UI32_T str_le);

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
BOOL_T L_IPV4V6_Is_Valid_IPv6_AddressString(char *str, UI32_T str_le);

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
BOOL_T L_IPV4V6_Is_Valid_IPv6_PrefixString(char *str, UI32_T str_len);


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
                                         BOOL_T      scope_id_required);

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
                                          BOOL_T      scope_id_required);

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
BOOL_T L_IPV4V6_Inet_Ntop(int family, void *addrptr, char *strptr, UI32_T len);

#endif /* _L_IPV4V6_H */
