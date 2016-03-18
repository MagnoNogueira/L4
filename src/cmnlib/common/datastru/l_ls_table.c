/*---------------------------------------------------------------------
 * File_Name : l_ls_table.c
 *
 * Purpose   : Provide ls_table operations
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
#include "sys_type.h"
#include "l_ls_table.h"

void L_ls_table_free (struct L_ls_table *);
int L_ls_node_info_exist (struct L_ls_node *);

/* Utility mask array. */
static u_int8_t maskbit[] = 
{
  0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};

struct L_ls_table *
L_ls_table_init (u_int8_t prefixsize, u_int8_t slotsize)
{
  struct L_ls_table *rt;

  rt = (struct L_ls_table *)malloc (L_LS_TABLE_SIZE (slotsize));
  memset(rt, 0, L_LS_TABLE_SIZE (slotsize));
  rt->prefixsize = prefixsize;
  rt->slotsize = slotsize;
  
  return rt;
}

void
L_ls_table_finish (struct L_ls_table *rt)
{
  L_ls_table_free (rt);
}

/* Allocate new route node. */
struct L_ls_node *
L_ls_node_create (struct L_ls_table *table)
{
  struct L_ls_node *node = 0;
  node = (struct L_ls_node *) malloc (L_LS_NODE_SIZE (table));
  if (node)
    memset(node, 0, L_LS_NODE_SIZE (table));
  return node;
}

/* Allocate new route node with allocating new ls_prefix. */
struct L_ls_node *
L_ls_node_set (struct L_ls_table *table, struct L_ls_prefix *prefix)
{
  struct L_ls_node *node;
  
  assert (table->prefixsize == prefix->prefixsize);

  node = (struct L_ls_node *)malloc (L_LS_NODE_SIZE (table));
  memset(node, 0, L_LS_NODE_SIZE (table));
  node->p = L_ls_prefix_new (prefix->prefixsize);
  L_ls_prefix_copy (node->p, prefix);
  node->table = table;

  return node;
}

/* Free route node. */
void
L_ls_node_free (struct L_ls_node *node)
{
  L_ls_prefix_free (node->p);
  free(node);
}

/* Free route table. */
void
L_ls_table_free (struct L_ls_table *rt)
{
  struct L_ls_node *tmp_node;
  struct L_ls_node *node;
 
  if (rt == NULL)
    return;

  node = rt->top;

  while (node)
    {
      if (node->l_left)
	{
	  node = node->l_left;
	  continue;
	}

      if (node->l_right)
	{
	  node = node->l_right;
	  continue;
	}

      tmp_node = node;
      node = node->parent;

      if (node != NULL)
	{
	  if (node->l_left == tmp_node)
	    node->l_left = NULL;
	  else
	    node->l_right = NULL;

	  L_ls_node_free (tmp_node);
	}
      else
	{
	  L_ls_node_free (tmp_node);
	  break;
	}
    }
 
  free(rt);
  return;
}

/* Common prefix route generation. */
struct L_ls_prefix *
L_ls_route_common (struct L_ls_prefix *n, struct L_ls_prefix *p)
{
  int i;
  u_int8_t diff;
  u_int8_t mask;

  struct L_ls_prefix *new;

  u_int8_t *np = n->prefix;
  u_int8_t *pp = p->prefix;
  u_int8_t *newp;

  assert (n->prefixsize == p->prefixsize);

  new = L_ls_prefix_new (n->prefixsize);
  newp = new->prefix;

  for (i = 0; i < p->prefixlen / 8; i++)
    {
      if (np[i] == pp[i])
	newp[i] = np[i];
      else
	break;
    }

  new->prefixlen = i * 8;

  if (new->prefixlen != p->prefixlen)
    {
      diff = np[i] ^ pp[i];
      mask = 0x80;
      while (new->prefixlen < p->prefixlen && !(mask & diff))
	{
	  mask >>= 1;
	  new->prefixlen++;
	}
      newp[i] = np[i] & maskbit[new->prefixlen % 8];
    }

  return new;
}

/* Macro version of check_bit (). */
#define CHECK_BIT(X,P) ((((u_int8_t *)(X))[(P) / 8]) >> (7 - ((P) % 8)) & 1)

/* Check bit of the prefix. */
static int
check_bit (u_int8_t *prefix, u_int8_t prefixlen)
{
  int offset;
  int shift;
  u_int8_t *p = (u_int8_t *)prefix;

  /*  assert (prefixlen <= 128); */

  offset = prefixlen / 8;
  shift = 7 - (prefixlen % 8);
  
  return (p[offset] >> shift & 1);
}

/* Macro version of set_link (). */
#define SET_LINK(X,Y) (X)->link[CHECK_BIT(&(Y)->prefix,(X)->prefixlen)] = (Y);\
                      (Y)->parent = (X)

static void
set_link (struct L_ls_node *node, struct L_ls_node *new)
{
  int bit;
    
  bit = check_bit (new->p->prefix, node->p->prefixlen);

  assert (bit == 0 || bit == 1);

  node->link[bit] = new;
  new->parent = node;
}

/* Lock node. */
struct L_ls_node *
L_ls_lock_node (struct L_ls_node *node)
{
  node->lock++;
  return node;
}

/* Unlock node. */
void
L_ls_unlock_node (struct L_ls_node *node)
{
  node->lock--;

  if (node->lock == 0)
    L_ls_node_delete (node);
}

/* Find matched prefix. */
struct L_ls_node *
L_ls_node_match (struct L_ls_table *table, struct L_ls_prefix *p)
{
  struct L_ls_node *node;
  struct L_ls_node *matched;

  matched = NULL;
  node = table->top;

  /* Walk down tree.  If there is matched route then store it to
     matched. */
  while (node && node->p->prefixlen <= p->prefixlen && 
	 L_ls_prefix_match (node->p, p))
  {
      if (node->vinfo0)
	matched = node;
      node = node->link[check_bit (p->prefix, node->p->prefixlen)];
  }

  /* If matched route found, return it. */
  if (matched)
    return L_ls_lock_node (matched);

  return NULL;
}

/* Lookup same prefix node.  Return NULL when we can't find route. */
struct L_ls_node *
L_ls_node_lookup (struct L_ls_table *table, struct L_ls_prefix *p)
{
  struct L_ls_node *node;

  node = table->top;

  while (node && node->p->prefixlen <= p->prefixlen && 
	 L_ls_prefix_match (node->p, p))
    {
      if (node->p->prefixlen == p->prefixlen && L_ls_node_info_exist (node))
	return L_ls_lock_node (node);

      node = node->link[check_bit (p->prefix, node->p->prefixlen)];
    }

  return NULL;
}

struct L_ls_node *
L_ls_node_lookup_first (struct L_ls_table *table)
{
  struct L_ls_node *node;

  for (node = L_ls_table_top (table); node; node = L_ls_route_next (node))
    if (node->vinfo0)
      return node;

  return NULL;
}

/* Add node to routing table. */
struct L_ls_node *
L_ls_node_get (struct L_ls_table *table, struct L_ls_prefix *p)
{
  struct L_ls_node *new;
  struct L_ls_node *node;
  struct L_ls_node *match;

  match = NULL;
  node = table->top;
  while (node && node->p->prefixlen <= p->prefixlen && 
	 L_ls_prefix_match (node->p, p))
    {
      if (node->p->prefixlen == p->prefixlen)
	{
	  L_ls_lock_node (node);
	  return node;
	}
      match = node;
      node = node->link[check_bit (p->prefix, node->p->prefixlen)];
    }

  if (node == NULL)
    {
      new = L_ls_node_set (table, p);
      if (match)
	set_link (match, new);
      else
	table->top = new;
    }
  else
    {
      new = L_ls_node_create (table);
      new->p = L_ls_route_common (node->p, p);
      new->p->family = p->family;
      new->table = table;
      set_link (new, node);

      if (match)
	set_link (match, new);
      else
	table->top = new;

      if (new->p->prefixlen != p->prefixlen)
	{
	  match = new;
	  new = L_ls_node_set (table, p);
	  set_link (match, new);
	}
    }
  L_ls_lock_node (new);
  
  return new;
}

/* Delete node from the routing table. */
void
L_ls_node_delete (struct L_ls_node *node)
{
  struct L_ls_node *child;
  struct L_ls_node *parent;

  assert (node->lock == 0);
  assert (node->vinfo0 == NULL);
  /*  assert (node->vinfo1 == NULL); */

  if (node->l_left && node->l_right)
    return;

  if (node->l_left)
    child = node->l_left;
  else
    child = node->l_right;

  parent = node->parent;

  if (child)
    child->parent = parent;

  if (parent)
    {
      if (parent->l_left == node)
	parent->l_left = child;
      else
	parent->l_right = child;
    }
  else
    node->table->top = child;

  L_ls_node_free (node);

  /* If parent node is stub then delete it also. */
  if (parent && parent->lock == 0)
    L_ls_node_delete (parent);
}

/* Get first node and lock it.  This function is useful when one want
   to lookup all the node exist in the routing table. */
struct L_ls_node *
L_ls_table_top (struct L_ls_table *table)
{
  /* If there is no node in the routing table return NULL. */
  if (table == NULL || table->top == NULL)
    return NULL;

  /* Lock the top node and return it. */
  L_ls_lock_node (table->top);
  return table->top;
}

/* Unlock current node and lock next node then return it. */
struct L_ls_node *
L_ls_route_next (struct L_ls_node *node)
{
  struct L_ls_node *next;
  struct L_ls_node *start;

  /* Node may be deleted from ls_unlock_node so we have to preserve
     next node's pointer. */

  if (node->l_left)
    {
      next = node->l_left;
      L_ls_lock_node (next);
      L_ls_unlock_node (node);
      return next;
    }
  if (node->l_right)
    {
      next = node->l_right;
      L_ls_lock_node (next);
      L_ls_unlock_node (node);
      return next;
    }

  start = node;
  while (node->parent)
    {
      if (node->parent->l_left == node && node->parent->l_right)
	{
	  next = node->parent->l_right;
	  L_ls_lock_node (next);
	  L_ls_unlock_node (start);
	  return next;
	}
      node = node->parent;
    }
  L_ls_unlock_node (start);
  return NULL;
}

/* Unlock current node and lock next node until limit. */
struct L_ls_node *
L_ls_route_next_until (struct L_ls_node *node, struct L_ls_node *limit)
{
  struct L_ls_node *next;
  struct L_ls_node *start;

  /* Node may be deleted from ls_unlock_node so we have to preserve
     next node's pointer. */

  if (node->l_left)
    {
      next = node->l_left;
      L_ls_lock_node (next);
      L_ls_unlock_node (node);
      return next;
    }
  if (node->l_right)
    {
      next = node->l_right;
      L_ls_lock_node (next);
      L_ls_unlock_node (node);
      return next;
    }

  start = node;
  while (node->parent && node != limit)
    {
      if (node->parent->l_left == node && node->parent->l_right)
	{
	  next = node->parent->l_right;
	  L_ls_lock_node (next);
	  L_ls_unlock_node (start);
	  return next;
	}
      node = node->parent;
    }
  L_ls_unlock_node (start);
  return NULL;
}

int
L_ls_node_info_exist (struct L_ls_node *node)
{
  int index;

  for (index = 0; index < node->table->slotsize; index++)
    if (node->vinfo[index])
      return 1;

  return 0;
}

void *
L_ls_node_info_set (struct L_ls_node *node, int index, void *info)
{
  void *old = node->vinfo[index];

  if (old == NULL)
    {
      L_ls_lock_node (node);
      node->table->count[index]++;
    }

  node->vinfo[index] = info;

  if (info == NULL)
    {
      node->table->count[index]--;
      L_ls_unlock_node (node);
    }

  return old;
}

void *
L_ls_node_info_unset (struct L_ls_node *node, int index)
{
  void *old = node->vinfo[index];

  if (node->vinfo[index] != NULL)
    {
      node->vinfo[index] = NULL;
      node->table->count[index]--;
      L_ls_unlock_node (node);
    }

  return old;
}

/* Return ls_node only if specified info exists. */
struct L_ls_node *
L_ls_node_info_lookup (struct L_ls_table *table, struct L_ls_prefix *p, int index)
{
  struct L_ls_node *node;
  assert (table->slotsize > index);

  node = L_ls_node_lookup (table, p);
  if (node)
    {
      if (node->vinfo[index])
	return node;

      L_ls_unlock_node (node);
    }
  return NULL;
}
