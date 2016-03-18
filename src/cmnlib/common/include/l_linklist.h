/*---------------------------------------------------------------------
 * File_Name : l_linklist.h
 *
 * Purpose   : Generic linked list routine
 *
 * Note    : Ported from Zebra
 *---------------------------------------------------------------------
 */

#ifndef L_LINKLIST_H
#define L_LINKLIST_H

#include "sys_type.h"

struct L_listnode 
{
  struct L_listnode *next;
  struct L_listnode *prev;
  void *data;
};

struct L_list 
{
  struct L_listnode *head;
  struct L_listnode *tail;
  UI32_T count;
  I32_T (*cmp) (void *val1, void *val2);
  void (*del) (void *val);
};

#define L_NEXTNODE(X) ((X) = (X)->next)

#define L_LISTHEAD(X) ((X)->head)

#define L_LISTCOUNT(X) (((X) != NULL) ? ((X)->count) : 0)

#define L_LIST_ISEMPTY(X) ((X)->head == NULL && (X)->tail == NULL)

#define L_GETDATA(X) ((X)->data)

/* List iteration macro. */
#define L_LIST_LOOP(L,V,N) \
  if (L) \
    for ((N) = (L)->head; (N); (N) = (N)->next) \
      if (((V) = (N)->data) != NULL)

/* List node add macro.  */
#define L_LISTNODE_ADD(L,N) \
  do { \
    (N)->next = NULL; \
    (N)->prev = (L)->tail; \
    if ((L)->head == NULL) \
      (L)->head = (N); \
    else \
      (L)->tail->next = (N); \
    (L)->tail = (N); \
  } while (0)

/* List node delete macro.  */
#define L_LISTNODE_DELETE(L,N) \
  do { \
    if ((N)->prev) \
      (N)->prev->next = (N)->next; \
    else \
      (L)->head = (N)->next; \
    if ((N)->next) \
      (N)->next->prev = (N)->prev; \
    else \
      (L)->tail = (N)->prev; \
  } while (0)



/* Prototypes. */
struct L_list *
L_list_new(void);

void 
L_list_free (struct L_list *);

struct L_listnode *
L_listnode_add (struct L_list *, void *);

struct L_listnode *
L_listnode_add_sort (struct L_list *, void *);

struct L_listnode *
L_listnode_add_before (struct L_list *, struct L_listnode *,void *);

int 
L_listnode_add_sort_nodup (struct L_list *, void *);

struct L_listnode *
L_listnode_add_after (struct L_list *, struct L_listnode *, void *);

void 
L_listnode_delete (struct L_list *, void *);

struct L_listnode *
L_listnode_lookup (struct L_list *, void *);

struct L_listnode *
L_listnode_lookup_next (struct L_list *list, void *data);

void *
L_listnode_head (struct L_list *);

void 
L_list_delete (struct L_list *);

void 
L_list_delete_all_node (struct L_list *);

void 
L_list_delete_node (struct L_list *, struct L_listnode *);

void 
L_list_add_node_prev (struct L_list *, struct L_listnode *, void *);

void 
L_list_add_node_next (struct L_list *, struct L_listnode *, void *);

void 
L_list_add_list (struct L_list *, struct L_list *);

void *
L_list_pop(struct L_list *list);

#endif /* L_LINKLIST_H */

