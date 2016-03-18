/*---------------------------------------------------------------------
 * File_Name : l_ls_prefix.h
 *
 * Purpose   : Provide ls_prefix operations
 *
 * History:
 *       Date       --  Modifier,   Reason
 *      2008.12.04  --  Yu Xiong,   First Created.
 *
 *
 * Note    : Ported from Zebra
 *---------------------------------------------------------------------
 */


#ifndef L_LS_PREFIX_H
#define L_LS_PREFIX_H

#include <sys/types.h>
#include <stdarg.h>
#include <assert.h>
#include "sys_type.h"
#include "sys_pal.h"
#include "l_prefix.h"


#define L_LS_INDEX_NONE    '\0'
#define L_LS_INDEX_INT8    'c'
#define L_LS_INDEX_INT16   's'
#define L_LS_INDEX_INT32   'i'
#define L_LS_INDEX_INADDR  'a'
#define L_LS_INDEX_OSIADDR 'o'
#define L_LS_INDEX_LSPID   'l'
#define L_LS_INDEX_INADDR6 'p'

#define L_LS_IPV4_ROUTE_TABLE_DEPTH  4
#define L_LS_IPV6_ROUTE_TABLE_DEPTH  16

/* Link state prefix structure. */
#define L_LS_PREFIX_MIN_SIZE	4
struct L_ls_prefix
{
  u_int8_t family;
  u_int8_t prefixlen;
  u_int8_t prefixsize;
  u_int8_t pad;
  u_int8_t prefix[L_LS_PREFIX_MIN_SIZE];
};

#define L_LS_PREFIX_SIZE(P)                                                     \
    (sizeof (struct L_ls_prefix) + (P)->prefixsize - L_LS_PREFIX_MIN_SIZE)

/* Some useful structure. */
struct L_ls_prefix6
{
  u_int8_t family;
  u_int8_t prefixlen;
  u_int8_t prefixsize;
  u_int8_t pad1;
  u_int8_t prefix[6];
};

struct L_ls_prefix8
{
  u_int8_t family;
  u_int8_t prefixlen;
  u_int8_t prefixsize;
  u_int8_t pad;
  u_int8_t prefix[8];
};

struct L_ls_prefix12
{
  u_int8_t family;
  u_int8_t prefixlen;
  u_int8_t prefixsize;
  u_int8_t pad;
  u_int8_t prefix[12];
};

struct L_ls_prefix16
{
  u_int8_t family;
  u_int8_t prefixlen;
  u_int8_t prefixsize;
  u_int8_t pad;
  u_int8_t prefix[16];
};

struct L_ls_prefix_nh
{
  u_int8_t family;
  u_int8_t prefixlen;
  u_int8_t prefixsize;
  u_int8_t pad;
  struct pal_in4_addr if_id;
  struct pal_in4_addr nbr_id;
};

/* Prototypes. */
struct L_ls_prefix *L_ls_prefix_new (u_int8_t);
void L_ls_prefix_free (struct L_ls_prefix *);
void L_ls_prefix_copy (struct L_ls_prefix *, struct L_ls_prefix *);
int L_ls_prefix_match (struct L_ls_prefix *, struct L_ls_prefix *);
void apply_mask_ls (struct L_ls_prefix *);
void L_ls_prefix_ipv4_set (struct L_ls_prefix *, int, struct pal_in4_addr);
#ifdef HAVE_IPV6
void L_ls_prefix_ipv6_set (struct L_ls_prefix *, int, struct pal_in6_addr);
#endif /* HAVE_IPV6 */
void L_ls_prefix_set_args (struct L_ls_prefix *, char *, ...);
void L_ls_prefix_set_va_list (struct L_ls_prefix *, char *, va_list);

#endif /* _ZEBOS_LS_PREFIX_H */
