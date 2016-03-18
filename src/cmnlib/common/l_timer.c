/* MODULE NAME: l_timer.c
* port from zebos\thread.c
*1)create (L_TIMER_SLOT+1) list, list 0... list (L_TIMER_SLOT -1) used as timer lists, list (L_TIMER_SLOT)
*   , ie lm_unuse, to collect unused timer;
*
*2)try to balance in each lists by lm_idx;
* Copyright(C)      Accton Corporation, 2008
*/

#include "l_timer.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <assert.h>

static void
l_timer_list_add_after (L_TIMTER_LIST_T *list, L_Timer_T *tt, L_Timer_T *t);

static void 
l_timer_list_add(L_TIMER_MASTER_T *m, L_Timer_T *t); 

static void
l_timer_list_unlink(L_TIMTER_LIST_T *list, L_Timer_T *t);

static void
l_timer_list_link(L_TIMTER_LIST_T *list, L_Timer_T *t);

/*add a timer to list, use m->lm_idx to balance every lists*/
static void 
l_timer_list_add(L_TIMER_MASTER_T *m, L_Timer_T *t)
{
  L_Timer_T *tt;

  /* Sort by timeval. */
  for (tt = m->lm_timer[m->lm_idx].tail; tt; tt = tt->prev)
  {
      if (t->lt_expt >= tt->lt_expt)
          break;
  }

  l_timer_list_add_after(&m->lm_timer[m->lm_idx], tt, t);

  /* Increment timer slot index.  */
  m->lm_idx++;
  m->lm_idx %= L_TIMER_SLOT;
}



/* Add a new timer just after the tt. If tt is NULL, add to head. */
static void
l_timer_list_add_after (L_TIMTER_LIST_T *list, L_Timer_T *tt, L_Timer_T *t)
{
  t->prev = tt;
  
  if (tt)
  {
      if (tt->next)
          tt->next->prev = t;
      else
          list->tail = t;
	  
      t->next = tt->next;
      tt->next = t;
  }
  else
  {
      if (list->head)
          list->head->prev = t;
      else
          list->tail = t;
	  
      t->next = list->head;
      list->head = t;
  }

  t->lt_type = L_TIMER_ONE_TIME;
  t->lt_idx = list->ll_idx;
  t->lt_list = list;

  list->ll_cnt++;
}


/*unlink a timer from the list. */
static void
l_timer_list_unlink(L_TIMTER_LIST_T *list, L_Timer_T *t)
{
  L_TIMER_ASSERT(list == t->lt_list);

  if (t->next)
      t->next->prev = t->prev;
  else
      list->tail = t->prev;
  
  if (t->prev)
      t->prev->next = t->next;
  else
      list->head = t->next;

  t->lt_idx = 0;
  t->lt_list = NULL;

  t->next = t->prev = NULL;
  list->ll_cnt--;
  
  return;
}


static void
l_timer_list_link(L_TIMTER_LIST_T *list, L_Timer_T *t)
{
  t->next = NULL;
  t->prev = list->tail;
  
  if (list->tail)
      list->tail->next = t;
  else
      list->head = t;

  t->lt_list = list;
  t->lt_idx = list->ll_idx;
  
  list->tail = t;
  list->ll_cnt++;
}

void 
l_timer_list_debug(L_TIMTER_LIST_T *list, UI8_T *str, UI32_T line)
{
  L_Timer_T *t;    
  UI32_T i = 0;

  t = list->head;

  while (t)
  {
      i++;
      t = t->next;
  }

  if (i != list->ll_cnt)
  {
      printf("$$$$$$$$$$$$$$$list[%lu] has error, %s[%lu]\n", list->ll_idx, str, line);
      l_timer_master_debug(list->ll_m);
  }
}


/*try to get from unused list, avoid to malloc*/
L_Timer_T* 
l_timer_get(L_TIMER_MASTER_T *m, funcPtr func, void *arg, UI32_T value)
{
  L_Timer_T *t;
  
  if ((t = m->lm_unuse->head) != NULL)
      l_timer_list_unlink(m->lm_unuse, t);
  else
      t = (L_Timer_T *)malloc(sizeof(L_Timer_T));		
  
  L_TIMER_ASSERT(t);
  
  t->lt_func = func;
  t->lt_arg = arg;
  t->lt_expt = m->lm_tick + value;
  t->lt_orig = value;
  t->lt_type = L_TIMER_ONE_TIME;
  
  return t ;
}


L_Timer_T* 
l_timer_add(L_TIMER_MASTER_T *m, funcPtr func, void *arg, UI32_T value)
{
  L_Timer_T *t;

  t = l_timer_get(m, func, arg, value);

  L_TIMER_ASSERT(t); 
  
  l_timer_list_add(m, t);
  
  /* Increment timer slot index.  */
  m->lm_idx++;
  m->lm_idx %= L_TIMER_SLOT;
  
  return t ;
}


/*link freed timer to unused list, used in next time*/
void 
l_timer_del(L_Timer_T *t)
{
  L_TIMER_MASTER_T *m = t->lt_list->ll_m;

  l_timer_list_unlink(t->lt_list, t);

  t->lt_type = L_TIMER_UNUSED;

  /*put in unused list, used in next time*/
  l_timer_list_link(m->lm_unuse, t);
}


void 
l_timer_update(L_Timer_T *t, UI32_T value)
{
  L_TIMER_MASTER_T *m = t->lt_list->ll_m;
  
  l_timer_list_unlink(t->lt_list, t);

  t->lt_expt = m->lm_tick + value;
  t->lt_orig = value;
  
  l_timer_list_add(m, t);
}

/**/
UI32_T 
l_timer_query(L_Timer_T *t)
{
  UI32_T ttl = 0;
  
  if (!t || !t->lt_list)
      goto EXIT;
  
  ttl = (UI32_T)(t->lt_expt - t->lt_list->ll_m->lm_tick);

EXIT:
  return ttl;
}

/*handle in each tick event*/
void 
l_timer_master_process(L_TIMER_MASTER_T *m)
{
  L_TIMTER_LIST_T *list;
  L_Timer_T *t;
  UI32_T i;
  register UI64_T curr;

  curr = ++m->lm_tick;

  for (i = 0; i < L_TIMER_SLOT; i++)
  {
      list = &m->lm_timer[i];
	  
      while ((t = list->head) != NULL)
      {
          if (curr < t->lt_expt)
              break;

          /*timer expire*/
          l_timer_list_unlink(list, t);

          t->lt_func(t->lt_arg);
          t->lt_type = L_TIMER_UNUSED;
          
          /*put in unused list, used in next time*/
          l_timer_list_link(m->lm_unuse, t);
      }
  }
}

void 
l_timer_master_debug(L_TIMER_MASTER_T *m)
{
  L_TIMTER_LIST_T *list;
  L_Timer_T *t;    
  UI32_T i, cnt;

  printf("tick %lu\n", (UI32_T)m->lm_tick);

  /*including the unused list*/
  for (i = 0; i <= L_TIMER_SLOT; i++)
  {
      list = &m->lm_timer[i];

      cnt = 0;
      printf("index %lu, list %p, head %p, tail %p, cnt %lu\n", 
                  list->ll_idx, list, list->head, list->tail, list->ll_cnt);
	  
      t = list->head;

      while (t)
      {
          printf("%p(%lu) ", t, (UI32_T)t->lt_expt);
          t = t->next;
          cnt++;
      }

      printf("\ncount:%lu\n", cnt);
  }
}

/*init timer master*/
void
l_timer_master_init(L_TIMER_MASTER_T *m)
{
  UI32_T i;

  memset(m, 0, sizeof(L_TIMER_MASTER_T));

  /*including the unused list*/
  for (i = 0; i <= L_TIMER_SLOT; i++)
  {
      m->lm_timer[i].ll_m = m;
      m->lm_timer[i].ll_idx = i;
  }

  m->lm_unuse = &m->lm_timer[L_TIMER_SLOT];
}

