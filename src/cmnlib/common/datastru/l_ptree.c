/*---------------------------------------------------------------------
 * File_Name : l_ptree.c
 *
 * This generic Patricia Tree structure may be used for variable length
 * keys. Please note that key_len being passed to the routines is in bits,
 * and not in bytes.
 *
 * Also note that this tree may not work if both IPv4 and IPv6 prefixes are
 * stored in the table at the same time, since the prefix length values for
 * the IPv4 and IPv6 addresses can be non-unique, and so can the bit
 * representation of the address.
 * If only host addresses are being stored, then this table may be used to
 * store IPv4 and IPv6 addresses at the same time, since the prefix lengths
 * will be 32 bits for the former, and 128 bits for the latter.
 *
 * Note    : Ported from Zebos
 *---------------------------------------------------------------------
 */
#include "sys_type.h"
#include "l_ptree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#define MIN(a,b)    ((a) < (b) ? (a) : (b))

/* Initialize tree. max_key_len is in bits. */
struct L_ptree *
L_ptree_init (UI16_T max_key_len)
{
  struct L_ptree *tree;

  if (! max_key_len)
    return NULL;

  tree = malloc(sizeof (struct L_ptree));

  if (!tree)
    return NULL;
  
  memset(tree, 0, sizeof (struct L_ptree));
  
  tree->max_key_len = max_key_len;

  return tree;
}

/* Free route tree. */
void
L_ptree_free (struct L_ptree *rt)
{
  struct L_ptree_node *tmp_node;
  struct L_ptree_node *node;

  if (rt == NULL)
    return;

  node = rt->top;

  while (node)
    {
      if (node->p_left)
	{
	  node = node->p_left;
	  continue;
	}

      if (node->p_right)
	{
	  node = node->p_right;
	  continue;
	}

      tmp_node = node;
      node = node->parent;

      if (node != NULL)
	{
	  if (node->p_left == tmp_node)
	    node->p_left = NULL;
	  else
	    node->p_right = NULL;

	  L_ptree_node_free (tmp_node);
	}
      else
	{
	  L_ptree_node_free (tmp_node);
	  break;
	}
    }

  free(rt);
  return;
}

/* Remove route tree. */
void
L_ptree_finish (struct L_ptree *rt)
{
  L_ptree_free (rt);
}

static int
L_ptree_bit_to_octets (UI16_T key_len)
{
  return MAX ((key_len + 7) / 8, PTREE_KEY_MIN_LEN);
}

/* Set key in node. */
void
L_ptree_key_copy (struct L_ptree_node *node, UI8_T *key, UI16_T key_len)
{
  int octets;

  if (key_len == 0)
    return;

  octets = L_ptree_bit_to_octets (key_len);
  memcpy (PTREE_NODE_KEY (node), key, octets);
}

/* Allocate new route node. */
struct L_ptree_node *
L_ptree_node_create (UI16_T key_len)
{
  struct L_ptree_node *pn;
  int octets;

  octets = L_ptree_bit_to_octets (key_len);

  pn = malloc(sizeof (struct L_ptree_node) + octets);
  
  if (! pn)
    return NULL;
  
  memset(pn, 0, sizeof (struct L_ptree_node) + octets);
  
  pn->key_len = key_len;

  return pn;
}

/* Utility mask array. */
static const UI8_T maskbit[] =
{
  0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};

/* Match keys. If n includes p prefix then return TRUE else return FALSE. */
int
L_ptree_key_match (UI8_T *np, UI16_T n_len, UI8_T *pp, UI16_T p_len)
{
  int shift;
  int offset;

  offset = MIN (n_len, p_len) / 8;
  shift = MIN (n_len, p_len) % 8;

  if (shift)
    if (maskbit[shift] & (np[offset] ^ pp[offset]))
      return FALSE;

  while (offset--)
    if (np[offset] != pp[offset])
      return FALSE;

  return TRUE;
}

/* Allocate new route node with ptree_key set. */
struct L_ptree_node *
L_ptree_node_set (struct L_ptree *tree, UI8_T *key, UI16_T key_len)
{
  struct L_ptree_node *node;

  node = L_ptree_node_create (key_len);
  if (! node)
    return NULL;

  /* Copy over key. */
  L_ptree_key_copy (node, key, key_len);
  node->tree = tree;

  return node;
}

/* Free route node. */
void
L_ptree_node_free (struct L_ptree_node *node)
{
  free(node);
}

/* Common ptree_key route genaration. */
static struct L_ptree_node *
L_ptree_node_common (struct L_ptree_node *n, UI8_T *pp, UI16_T p_len)
{
  int i;
  int j;
  UI8_T diff;
  UI8_T mask;
  UI16_T key_len;
  struct L_ptree_node *new;
  UI8_T *np;
  UI8_T *newp;
  UI8_T boundary = 0;

  np = PTREE_NODE_KEY (n);

  for (i = 0; i < p_len / 8; i++)
    if (np[i] != pp[i])
      break;

  key_len = i * 8;

  if (key_len != p_len)
    {
      diff = np[i] ^ pp[i];
      mask = 0x80;
      while (key_len < p_len && !(mask & diff))
	{
          if (boundary == 0)
            boundary = 1;
	  mask >>= 1;
	  key_len++;
	}
    }

  /* Fill new key. */
  new = L_ptree_node_create (key_len);
  if (! new)
    return NULL;

  newp = PTREE_NODE_KEY (new);

  for (j = 0; j < i; j++)
    newp[j] = np[j];

  if (boundary)
    newp[j] = np[j] & maskbit[new->key_len % 8];

  return new;
}

/* Check bit of the ptree_key. */
static int
L_ptree_check_bit (struct L_ptree *tree, UI8_T *p, UI16_T key_len)
{
  int offset;
  int shift;

  //assert(tree->max_key_len >= key_len);

  offset = key_len / 8;
  shift = 7 - (key_len % 8);

  return (p[offset] >> shift & 1);
}

static void
L_ptree_set_link (struct L_ptree_node *node, struct L_ptree_node *new)
{
  int bit;

  bit = L_ptree_check_bit (new->tree, PTREE_NODE_KEY (new), node->key_len);

  //assert(bit == 0 || bit == 1);

  node->link[bit] = new;
  new->parent = node;
}

/* Lock node. */
struct L_ptree_node *
L_ptree_lock_node (struct L_ptree_node *node)
{
  node->lock++;
  return node;
}

/* Unlock node. */
void
L_ptree_unlock_node (struct L_ptree_node *node)
{
  node->lock--;

  if (node->lock == 0)
    L_ptree_node_delete (node);
}

/* Find matched ptree_key. */
struct L_ptree_node *
L_ptree_node_match (struct L_ptree *tree, UI8_T *key, UI16_T key_len)
{
  struct L_ptree_node *node;
  struct L_ptree_node *matched;

  if (key_len > tree->max_key_len)
    return NULL;

  matched = NULL;
  node = tree->top;

  /* Walk down tree.  If there is matched route then store it to
     matched. */
  while (node && L_ptree_key_match (PTREE_NODE_KEY (node),
                                  node->key_len, key, key_len))
    {
      matched = node;
      node = node->link[L_ptree_check_bit (tree, key, node->key_len)];

      if (matched->key_len >= key_len)
        break;
    }

  /* If matched route found, return it. */
  if (matched)
    return L_ptree_lock_node (matched);

  return NULL;
}


/* Lookup same ptree_key node.  Return NULL when we can't find the node. */
struct L_ptree_node *
L_ptree_node_lookup (struct L_ptree *tree, UI8_T *key, UI16_T key_len)
{
  struct L_ptree_node *node;

  if (key_len > tree->max_key_len)
    return NULL;

  node = tree->top;
  while (node && node->key_len <= key_len
         && L_ptree_key_match (PTREE_NODE_KEY (node),
                             node->key_len, key, key_len))
    {
      if (node->key_len == key_len && node->info)
        return L_ptree_lock_node (node);

      node = node->link[L_ptree_check_bit(tree, key, node->key_len)];
    }

  return NULL;
}

/*
 * Lookup same ptree_key node in sub-tree rooted at 'start_node'.
 * Return NULL when we can't find the node.
 */
struct L_ptree_node *
L_ptree_node_sub_tree_lookup (struct L_ptree *tree,
                            struct L_ptree_node *start_node,
                            UI8_T *key,
                            UI16_T key_len)
{
  struct L_ptree_node *node;

  if (! start_node || key_len > tree->max_key_len)
    return NULL;

  node = start_node;
  while (node && node->key_len <= key_len
         && L_ptree_key_match (PTREE_NODE_KEY (node),
                             node->key_len, key, key_len))
    {
      if (node->key_len == key_len && node->info)
        return L_ptree_lock_node (node);

      node = node->link[L_ptree_check_bit(tree, key, node->key_len)];
    }

  return NULL;
}

/* Add node to routing tree. */
struct L_ptree_node *
L_ptree_node_get (struct L_ptree *tree, UI8_T *key, UI16_T key_len)
{
  struct L_ptree_node *new, *node, *match;

  if (key_len > tree->max_key_len)
    return NULL;

  match = NULL;
  node = tree->top;
  while (node && node->key_len <= key_len && 
	 L_ptree_key_match (PTREE_NODE_KEY (node), node->key_len, key, key_len))
    {
      if (node->key_len == key_len)
	return L_ptree_lock_node (node);

      match = node;
      node = node->link[L_ptree_check_bit (tree, key, node->key_len)];
    }

  if (node == NULL)
    {
      new = L_ptree_node_set (tree, key, key_len);
      if (match)
	L_ptree_set_link (match, new);
      else
	tree->top = new;
    }
  else
    {
      new = L_ptree_node_common (node, key, key_len);
      if (! new)
	return NULL;
      new->tree = tree;
      L_ptree_set_link (new, node);

      if (match)
	L_ptree_set_link (match, new);
      else
	tree->top = new;

      if (new->key_len != key_len)
	{
	  match = new;
	  new = L_ptree_node_set (tree, key, key_len);
	  L_ptree_set_link (match, new);
	}
    }

  L_ptree_lock_node (new);
  return new;
}

/* Delete node from the routing tree. */
void
L_ptree_node_delete (struct L_ptree_node *node)
{
  struct L_ptree_node *child, *parent;

  //assert(node->lock == 0);
  //assert(node->info == NULL);

  if (node->p_left && node->p_right)
    return;

  if (node->p_left) 
    child = node->p_left;
  else
    child = node->p_right;

  parent = node->parent;

  if (child)
    child->parent = parent;

  if (parent)
    {
      if (parent->p_left == node)
	parent->p_left = child;
      else
	parent->p_right = child;
    }
  else
    node->tree->top = child;

  L_ptree_node_free (node);

  /* If parent node is stub then delete it also. */
  if (parent && parent->lock == 0)
    L_ptree_node_delete (parent);
}

/* Delete All Ptree Nodes */
void
L_ptree_node_delete_all (struct L_ptree *rt)
{
  struct L_ptree_node *tmp_node;
  struct L_ptree_node *node;

  if (rt == NULL)
    return;

  node = rt->top;

  while (node)
    {
      if (node->p_left)
        {
          node = node->p_left;
          continue;
        }

      if (node->p_right)
        {
          node = node->p_right;
          continue;
        }

      tmp_node = node;
      node = node->parent;

      if (node != NULL)
        {
          if (node->p_left == tmp_node)
            node->p_left = NULL;
          else
            node->p_right = NULL;

          L_ptree_node_free (tmp_node);
        }
      else
        {
          L_ptree_node_free (tmp_node);
          rt->top = NULL;
          break;
        }
    }

  return;
}

/* Get fist node and lock it.  This function is useful when one want
   to lookup all the node exist in the routing tree. */
struct L_ptree_node *
L_ptree_top (struct L_ptree *tree)
{
  /* If there is no node in the routing tree return NULL. */
  if (tree == NULL || tree->top == NULL)
    return NULL;

  /* Lock the top node and return it. */
  return L_ptree_lock_node (tree->top);
}

/* Unlock current node and lock next node then return it. */
struct L_ptree_node *
L_ptree_next (struct L_ptree_node *node)
{
  struct L_ptree_node *next, *start;

  /* Node may be deleted from L_ptree_unlock_node so we have to preserve
     next node's pointer. */

  if (node->p_left)
    {
      next = node->p_left;
      L_ptree_lock_node (next);
      L_ptree_unlock_node (node);
      return next;
    }
  if (node->p_right)
    {
      next = node->p_right;
      L_ptree_lock_node (next);
      L_ptree_unlock_node (node);
      return next;
    }

  start = node;
  while (node->parent)
    {
      if (node->parent->p_left == node && node->parent->p_right)
	{
	  next = node->parent->p_right;
	  L_ptree_lock_node (next);
	  L_ptree_unlock_node (start);
	  return next;
	}
      node = node->parent;
    }
  L_ptree_unlock_node (start);
  return NULL;
}

/* Unlock current node and lock next node until limit. */
struct L_ptree_node *
L_ptree_next_until (struct L_ptree_node *node, struct L_ptree_node *limit)
{
  struct L_ptree_node *next, *start;

  /* Node may be deleted from L_ptree_unlock_node so we have to preserve
     next node's pointer. */

  if (node->p_left)
    {
      next = node->p_left;
      L_ptree_lock_node (next);
      L_ptree_unlock_node (node);
      return next;
    }
  if (node->p_right)
    {
      next = node->p_right;
      L_ptree_lock_node (next);
      L_ptree_unlock_node (node);
      return next;
    }

  start = node;
  while (node->parent && node != limit)
    {
      if (node->parent->p_left == node && node->parent->p_right)
	{
	  next = node->parent->p_right;
	  L_ptree_lock_node (next);
	  L_ptree_unlock_node (start);
	  return next;
	}
      node = node->parent;
    }
  L_ptree_unlock_node (start);
  return NULL;
}

/* Check if the tree contains nodes with info set. */
int
L_ptree_has_info (struct L_ptree *tree)
{
  struct L_ptree_node *node;

  if (tree == NULL)
    return 0;

  node = tree->top;
  while (node)
    {
      if (node->info)
        return 1;

      if (node->p_left)
        {
          node = node->p_left;
          continue;
        }

      if (node->p_right)
        {
          node = node->p_right;
          continue;
        }

      while (node->parent)
        {
          if (node->parent->p_left == node && node->parent->p_right)
            {
              node = node->parent->p_right;
              break;
            }
          node = node->parent;
        }

      if (node->parent == NULL)
        break;
    }

  return 0;
}
