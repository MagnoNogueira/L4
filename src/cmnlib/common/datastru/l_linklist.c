/*---------------------------------------------------------------------
 * File_Name : l_linklist.c
 *
 * Purpose   : Generic linked list routine
 *
 * Note    : Ported from Zebra
 *---------------------------------------------------------------------
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include "sys_type.h"
#include "l_linklist.h"

/* Allocate new list. */
struct L_list *
L_list_new (void)
{
  struct L_list *list;

  list = malloc(sizeof (struct L_list));

  if (!list)
  {
      printf("%s[%d] fail to malloc", __FUNCTION__, __LINE__);
      goto EXIT;
  }
  
  memset(list, 0, sizeof (struct L_list));

EXIT:
  return list;
}

/* Free list. */
void
L_list_free (struct L_list *list)
{
  free(list);
}

/* Allocate new L_listnode.  Internal use only. */
static struct L_listnode *
L_listnode_new (void)
{
  struct L_listnode *node;

  node = malloc(sizeof (struct L_listnode));

  if (!node)
  {
      printf("%s[%d] fail to malloc", __FUNCTION__, __LINE__);
      goto EXIT;
  }
  
  memset(node, 0, sizeof (struct L_listnode));

EXIT:
  return node;
}

/* Free L_listnode. */
static void
L_listnode_free (struct L_listnode *node)
{
  free(node);
}

/* Add new data to the list. */
struct L_listnode *
L_listnode_add (struct L_list *list, void *val)
{
  struct L_listnode *node;

  if ( (!list) || (!val) )
    return NULL;

  node = L_listnode_new ();
  if ( !node )
      return NULL;

  node->prev = list->tail;
  node->data = val;

  if (list->head == NULL)
      list->head = node;
  else
      list->tail->next = node;
  
  list->tail = node;

  list->count++;

  return node;
}

/* Add new node with sort function. */
struct L_listnode *
L_listnode_add_sort (struct L_list *list, void *val)
{
  struct L_listnode *n;
  struct L_listnode *new;

  new = L_listnode_new ();
  if (! new)
      return NULL;

  new->data = val;

  if (list->cmp)
  {
      for (n = list->head; n; n = n->next)
      {
          if ((*list->cmp) (val, n->data) < 0)
          {	    
              new->next = n;
              new->prev = n->prev;
              
              if (n->prev)
                  n->prev->next = new;
              else
                  list->head = new;
              
              n->prev = new;
              list->count++;
			  
              return new;
          }
      }
  }

  new->prev = list->tail;

  if (list->tail)
      list->tail->next = new;
  else
      list->head = new;

  list->tail = new;
  list->count++;
  
  return new;
}

/* Add a unique new node with sort function. */
/* Returns 0 if a new node has been added */
/* Returns -1 if the node is duplicate */
int
L_listnode_add_sort_nodup (struct L_list *list, void *val)
{
  struct L_listnode *n;
  struct L_listnode *new;
  int ret;

  new = L_listnode_new ();
  if (! new)
      return 0;
  
  new->data = val;

  if (list->cmp)
  {
      for (n = list->head; n; n = n->next)
      {
          ret = (*list->cmp) (val, n->data);
		  
          if (ret < 0)
          {	    
              new->next = n;
              new->prev = n->prev;
              
              if (n->prev)
                  n->prev->next = new;
              else
                  list->head = new;
			  
              n->prev = new;
              list->count++;
			  
              return 0;
          }
          else if (ret == 0)/* Duplicate node */
          {
              L_listnode_free (new);
              return 1;
          }
      }
  }

  new->prev = list->tail;

  if (list->tail)
      list->tail->next = new;
  else
      list->head = new;

  list->tail = new;
  list->count++;
  
  return 0;
}

struct L_listnode *
L_listnode_add_before (struct L_list *list, struct L_listnode *pp, void *val)
{
  struct L_listnode *new;
                                                                                
  new = L_listnode_new ();
  if (! new)
      return NULL;
                                                                                
  if (pp == NULL)
  {
      if (list->head)
          list->head->prev = new;
      else
          list->tail = new;
                                      
      new->next = list->head;
      new->prev = pp;
                                      
      list->head = new;
  }
  else
  {
      new->data = val;
      new->next = pp;
      new->prev = pp->prev;
                                                  
      if (pp->prev)
          pp->prev->next  = new;
      else
          list->head = new;
                                                  
      pp->prev = new;
  }
                                                                                
  list->count++;
  return new;
}



struct L_listnode *
L_listnode_add_after (struct L_list *list, struct L_listnode *pp, void *val)
{
  struct L_listnode *nn;

  nn = L_listnode_new ();
  if (! nn)
      return NULL;
  
  nn->data = val;

  if (pp == NULL)
  {
      if (list->head)
          list->head->prev = nn;
      else
          list->tail = nn;
      
      nn->next = list->head;
      nn->prev = pp;
      
      list->head = nn;
  }
  else
  {
      if (pp->next)
          pp->next->prev = nn;
      else
          list->tail = nn;
      
      nn->next = pp->next;
      nn->prev = pp;
      
      pp->next = nn;
  }
  
  list->count++;

  return nn;
}

/* Delete specific date pointer from the list. */
void
L_listnode_delete (struct L_list *list, void *val)
{
  struct L_listnode *node;
  
  if ( (!list) || (!val) )
      return;

  for (node = list->head; node; node = node->next)
  {
      if (node->data == val)
      {
          if (node->prev)
              node->prev->next = node->next;
          else
              list->head = node->next;
          
          if (node->next)
              node->next->prev = node->prev;
          else
              list->tail = node->prev;
          
          list->count--;
          L_listnode_free (node);
		  
          return;
      }
  }
}

/* Return first node's data if it is there.  */
void *
L_listnode_head (struct L_list *list)
{
  struct L_listnode *node;

  node = list->head;

  if (node)
      return node->data;
  
  return NULL;
}

/* Delete all L_listnode from the list. */
void
L_list_delete_all_node (struct L_list *list)
{
  struct L_listnode *node;
  struct L_listnode *next;

  for (node = list->head; node; node = next)
  {
      next = node->next;
	  
      if (list->del)
          (*list->del) (node->data);
	  
      L_listnode_free (node);
  }
  
  list->head = list->tail = NULL;
  list->count = 0;
}

/* Delete all L_listnode then free list itself. */
void
L_list_delete (struct L_list *list)
{
  struct L_listnode *node;
  struct L_listnode *next;

  for (node = list->head; node; node = next)
  {
      next = node->next;
	  
      if (list->del)
          (*list->del) (node->data);
	  
      L_listnode_free (node);
  }
  
  L_list_free (list);
}

/* Lookup the node which has given data. */
struct L_listnode *
L_listnode_lookup (struct L_list *list, void *data)
{
  struct L_listnode *node;

  for (node = list->head; node; L_NEXTNODE (node))
      if (data == L_GETDATA (node))
          return node;
  
  return NULL;
}

/* Lookup the next of the node which has given data. */
struct L_listnode *
L_listnode_lookup_next (struct L_list *list, void *data)
{
  struct L_listnode *node;

  for (node = list->head; node; L_NEXTNODE (node))
      if (data == L_GETDATA (node))
          return L_NEXTNODE (node);
  
  return NULL;
}


/* Delete the node from list.  For ospfd and ospf6d. */
void
L_list_delete_node (struct L_list *list, struct L_listnode *node)
{
  if (node->prev)
      node->prev->next = node->next;
  else
      list->head = node->next;
  
  if (node->next)
      node->next->prev = node->prev;
  else
      list->tail = node->prev;
  
  list->count--;
  L_listnode_free (node);
}


void
L_list_add_node_prev (struct L_list *list, struct L_listnode *current, void *val)
{
  struct L_listnode *node;

  node = L_listnode_new ();
  if (!node)
      return;
  
  node->next = current;
  node->data = val;

  if (current->prev == NULL)
      list->head = node;
  else
      current->prev->next = node;

  node->prev = current->prev;
  current->prev = node;

  list->count++;
}

void
L_list_add_node_next (struct L_list *list, struct L_listnode *current, void *val)
{
  struct L_listnode *node;

  node = L_listnode_new ();
  if (!node)
      return;
  
  node->prev = current;
  node->data = val;

  if (current->next == NULL)
      list->tail = node;
  else
      current->next->prev = node;

  node->next = current->next;
  current->next = node;

  list->count++;
}

void
L_list_add_list (struct L_list *l, struct L_list *m)
{
  struct L_listnode *n;

  for (n = L_LISTHEAD (m); n; L_NEXTNODE (n))
      L_listnode_add (l, n->data);
}

void *
L_list_pop(struct L_list *list)
{
  struct L_listnode *node;
  void *data = NULL;

  if (!(node = list->head))
      goto EXIT;

  data = node->data;

  list->head = node->next;
  
  if (node->next)
      node->next->prev = NULL;
  else
      list->tail = NULL;
  
  list->count--;
  L_listnode_free (node);

EXIT:
  return data;
}

