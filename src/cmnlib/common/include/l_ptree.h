/*---------------------------------------------------------------------
 * File_Name : l_ptree.h
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

#ifndef L_PTREE_H
#define L_PTREE_H

#include "sys_type.h"

/* Patricia tree top structure. */
struct L_ptree
{
  /* Top node. */
  struct L_ptree_node *top;

  /* Maximum key size allowed (in bits). */
  UI32_T max_key_len;
};

/* Patricia tree node structure. */
struct L_ptree_node
{
  struct L_ptree_node *link[2];
#define  p_left      link[0]
#define  p_right     link[1]

  /* Tree link. */
  struct L_ptree *tree;
  struct L_ptree_node *parent;

  /* Lock of this radix. */
  UI32_T lock;

  /* Each node of route. */
  void *info;

  /* Key len (in bits). */
  UI32_T key_len;

  /* Key begins here. */
  UI8_T key [1];
};

#define PTREE_KEY_MIN_LEN       1 
#define PTREE_NODE_KEY(n)       (& (n)->key [0])



struct L_ptree *
L_ptree_init (UI16_T max_key_len);

struct L_ptree_node *
L_ptree_top (struct L_ptree *tree);

struct L_ptree_node *
L_ptree_next (struct L_ptree_node *node);

struct L_ptree_node *
L_ptree_next_until (struct L_ptree_node *node1, struct L_ptree_node *node2);

struct L_ptree_node *
L_ptree_node_get (struct L_ptree *tree, UI8_T *key, UI16_T key_len);

struct L_ptree_node *
L_ptree_node_lookup (struct L_ptree *tree, UI8_T *key, UI16_T key_len);

struct L_ptree_node *
L_ptree_lock_node (struct L_ptree_node *node);

struct L_ptree_node *
L_ptree_node_match (struct L_ptree *tree, UI8_T *key, UI16_T key_len);

void 
L_ptree_node_free (struct L_ptree_node *node);

void 
L_ptree_finish (struct L_ptree *tree);

void 
L_ptree_unlock_node (struct L_ptree_node *node);

void 
L_ptree_node_delete (struct L_ptree_node *node);

void 
L_ptree_node_delete_all (struct L_ptree *tree);

int 
L_ptree_has_info (struct L_ptree *tree);

#endif /* L_PTREE_H */

