/*---------------------------------------------------------------------
 * File_Name : l_ls_table.h
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


#ifndef L_LS_TABLE_H
#define L_LS_TABLE_H

#include <sys/types.h>
#include <assert.h>
#include "sys_type.h"
#include "sys_pal.h"
#include "l_prefix.h"
#include "l_ls_prefix.h"



/* Routing table top structure. */
struct L_ls_table
{
  /* Description for this table. */
  char *desc;

  /* Top node of route table. */
  struct L_ls_node *top;

  /* Prefix size of depth. */
  u_int8_t prefixsize;

  /* Number of info slot. */
  u_int8_t slotsize;

  /* Counter for vinfo. */
  u_int32_t count[1];
};

#define L_LS_TABLE_SIZE(S)                                                      \
  (sizeof (struct L_ls_table) + ((S) - 1) * sizeof (u_int32_t))

#define L_LS_NODE_SIZE(T)                                                       \
    (sizeof (struct L_ls_node) + ((T)->slotsize - 1) * sizeof (void *))

/* Each routing entry. */
struct L_ls_node
{
  /* DO NOT MOVE the first 2 pointers are used by the memory manager as well */
  struct L_ls_node *link[2];
#define l_left   link[0]
#define l_right  link[1]

  /* Pointer to variable length prefix of this radix. */
  struct L_ls_prefix *p;

  /* Tree link. */
  struct L_ls_table *table;
  struct L_ls_node *parent;

  /* Lock of this radix. */
  unsigned int lock;

  /* Routing information. */
  void *vinfo[1];
#define vinfo0   vinfo[0]
#define vinfo1   vinfo[1]
#define vinfo2   vinfo[2]

  /* Convenient macro. */
#define ri_cur   vinfo[0]
#define ri_new   vinfo[1]
#define ri_lsas  vinfo[2]
};

#define RN_INFO_SET(N,I,D)     L_ls_node_info_set ((N), (I), (D))
#define RN_INFO_UNSET(N,I)     L_ls_node_info_unset ((N), (I))
#define RN_INFO(N,I)                                                          \
    (assert ((N)->table->slotsize > (I)), (N)->vinfo[(I)])

/* Route node info index. */
#define RNI_DEFAULT        0
#define RNI_CURRENT        0
#define RNI_NEW            1
#define RNI_LSAS           2

/* Prototypes. */
struct L_ls_table *L_ls_table_init (u_int8_t, u_int8_t);
void L_ls_table_finish (struct L_ls_table *);
void L_ls_unlock_node (struct L_ls_node *);
void L_ls_node_delete (struct L_ls_node *);
struct L_ls_node *L_ls_table_top (struct L_ls_table *);
struct L_ls_node *L_ls_route_next (struct L_ls_node *);
struct L_ls_node *L_ls_route_next_until (struct L_ls_node *, struct L_ls_node *);
struct L_ls_node *L_ls_node_get (struct L_ls_table *, struct L_ls_prefix *);
struct L_ls_node *L_ls_node_lookup (struct L_ls_table *, struct L_ls_prefix *);
struct L_ls_node *L_ls_node_lookup_first (struct L_ls_table *);
struct L_ls_node *L_ls_lock_node (struct L_ls_node *);
struct L_ls_node *L_ls_node_match (struct L_ls_table *, struct L_ls_prefix *);
//struct L_ls_node *L_ls_node_match_ipv4 (struct L_ls_table *, struct pal_in4_addr *);
struct L_ls_node *L_ls_node_info_lookup (struct L_ls_table *, struct L_ls_prefix *, int);
void *L_ls_node_info_set (struct L_ls_node *, int, void *);
void *L_ls_node_info_unset (struct L_ls_node *, int);

#endif /* _ZEBOS_LS_TABLE_H */
