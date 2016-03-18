/*---------------------------------------------------------------------
 * File_Name : l_ls_prefix.c
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

#include "l_ls_prefix.h"
 
/* Utility mask array. */
static u_int8_t maskbit[] = 
{
  0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};

/* Number of bits in prefix type. */
#ifndef PNBBY
#define PNBBY 8
#endif /* PNBBY */

struct L_ls_prefix *
L_ls_prefix_new (u_int8_t size)
{
  struct L_ls_prefix *p = 0;

  p = malloc(sizeof (struct L_ls_prefix) + size - L_LS_PREFIX_MIN_SIZE);
  if (p)
  {
    memset( p, 0, sizeof (struct L_ls_prefix) + size - L_LS_PREFIX_MIN_SIZE );
    p->prefixsize = size;
  }
  return p;
}

void
L_ls_prefix_free (struct L_ls_prefix *p)
{
  free(p);
}

void
L_ls_prefix_copy (struct L_ls_prefix *dest, struct L_ls_prefix *src)
{
  memcpy (dest, src, L_LS_PREFIX_SIZE (src));
}

/* If n includes p prefix then return else return 0. */
int
L_ls_prefix_match (struct L_ls_prefix *n, struct L_ls_prefix *p)
{
  int offset;
  int shift;

  /* Set both prefix's head pointer. */
  u_int8_t *np = n->prefix;
  u_int8_t *pp = p->prefix;

  assert (n->prefixsize == p->prefixsize);

  /* If n's prefix is longer than p's one return 0. */
  if (n->prefixlen > p->prefixlen)
    return 0;

  offset = n->prefixlen / PNBBY;
  shift =  n->prefixlen % PNBBY;

  if (shift)
    if (maskbit[shift] & (np[offset] ^ pp[offset]))
      return 0;
  
  while (offset--)
    if (np[offset] != pp[offset])
      return 0;

  return 1;
}

void
apply_mask_ls (struct L_ls_prefix *p)
{
  u_int8_t *pnt;
  int index;
  int offset;

  index = p->prefixlen / 8;

  if (index < p->prefixsize)
    {
      pnt = (u_int8_t *) &p->prefix;
      offset = p->prefixlen % 8;

      pnt[index] &= maskbit[offset];
      index++;

      while (index < p->prefixsize)
	pnt[index++] = 0;
    }
}

void
L_ls_prefix_ipv4_set (struct L_ls_prefix *p, int prefixlen,
		    struct pal_in4_addr addr)
{
  p->family = AF_INET;
  p->prefixlen = prefixlen;
  p->prefixsize = L_LS_IPV4_ROUTE_TABLE_DEPTH;
  memcpy (p->prefix, &addr, sizeof (struct pal_in4_addr));
  apply_mask_ls (p);
}

#ifdef HAVE_IPV6
void
L_ls_prefix_ipv6_set (struct L_ls_prefix *p, int prefixlen,
		    struct pal_in6_addr addr)
{
  p->family = AF_INET6;
  p->prefixlen = prefixlen;
  p->prefixsize = L_LS_IPV6_ROUTE_TABLE_DEPTH;
  memcpy (p->prefix, &addr, sizeof (struct pal_in6_addr));
  apply_mask_ls (p);
}
#endif /* HAVE_IPV6 */

/* This function just pass the arguments to ls_prefix_set_va_list. */
void
L_ls_prefix_set_args (struct L_ls_prefix *p, char *types, ...)
{
  va_list args;

  va_start (args, types);
  L_ls_prefix_set_va_list (p, types, args);
  va_end (args);
}

/* Suppose all arguments are passed as a pointer. */
void
L_ls_prefix_set_va_list (struct L_ls_prefix *p, char *types, va_list args)
{
  char type;
  unsigned int index = 0;

  while ((type = *types++) != L_LS_INDEX_NONE)
    {
      unsigned int *valp;
      unsigned int val = 0;
      unsigned int addrval = 0;
      int i, size = 0;

      valp = (unsigned int *)va_arg (args, unsigned int *);
      switch (type)
        {
        case L_LS_INDEX_INT8:
          val = (*valp & 0xFF);
          size = 1;
          break;
        case L_LS_INDEX_INT16:
          val = (*valp & 0xFFFF);
          size = 2;
          break;
        case L_LS_INDEX_INT32:
          val = (*valp & 0xFFFFFFFF);
          size = 4;
          break;
        case L_LS_INDEX_INADDR:
          addrval = (*valp & 0xFFFFFFFF);
          val = pal_ntoh32 (addrval);
          size = 4;
          break;
        }

      for (i = 1; i <= size; i++)
	p->prefix[index++] = (u_int8_t)((val >> ((size - i) * 8)) & 0xff);
    }
}

