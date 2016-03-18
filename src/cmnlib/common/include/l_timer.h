/*-----------------------------------------------------------------------------
 * Module Name: cfm_engine.h
 *-----------------------------------------------------------------------------
 * PURPOSE: Definitions for the timer
 *-----------------------------------------------------------------------------
 * NOTES:
 *
 *-----------------------------------------------------------------------------
 * HISTORY:
 *    01/01/2007 - Macualey Cheng  , Created
 *
 *
 *-----------------------------------------------------------------------------
 * Copyright(C)                               Accton Corporation, 2006
 *-----------------------------------------------------------------------------
 */

#ifndef _L_TIMER_H
#define _L_TIMER_H

#include "sys_type.h"

#define L_TIMER_DEBUG

#ifdef L_TIMER_DEBUG
#define L_TIMER_ASSERT(x) assert(x)
#else
#define L_TIMER_ASSERT(x) 
#endif/*L_TIMER_DEBUG*/

enum L_TIMER_E
{
  L_TIMER_ONE_TIME=0,
  L_TIMER_UNUSED
};

typedef BOOL_T (*funcPtr)(void *);

typedef struct L_TIMER_S
{
  struct L_TIMER_S   *next;      
  struct L_TIMER_S   *prev;      
  
  /*the unit belong to list*/
  struct L_TIMTER_LIST_S *lt_list;
  
  /*callback function*/
  BOOL_T (*lt_func)(void *); 
  
  /*callback function argument*/
  void   *lt_arg;         
  
  /*the expire time*/
  UI64_T lt_expt;
  
  /*the orignial vlaue*/
  UI32_T lt_orig;      
  
  /*unit type, used or unuseds*/
  UI32_T lt_type;		           
  
  /*belong to which list*/
  UI32_T lt_idx;
}L_Timer_T;


typedef struct L_TIMTER_LIST_S
{
  L_Timer_T *head;        
  L_Timer_T *tail; 

  /*l_timer master*/
  struct L_TIMER_MASTER_S *ll_m;

  /*unit count*/
  UI32_T ll_cnt;

  /*list index*/
  UI32_T ll_idx;
}L_TIMTER_LIST_T;


#define L_TIMER_SLOT           7

typedef struct L_TIMER_MASTER_S
{
  /*ticks*/
  UI64_T lm_tick;

  /*multi lists, avoid the list to be too long*/
  L_TIMTER_LIST_T lm_timer[L_TIMER_SLOT + 1];

  /*collect unused unit, idx== L_TIMER_SLOT list*/
  L_TIMTER_LIST_T *lm_unuse;
  
  /*try to load balance the lists*/
  UI32_T lm_idx;
}L_TIMER_MASTER_T;



L_Timer_T* 
l_timer_add(L_TIMER_MASTER_T *m, funcPtr func, void *arg, UI32_T value);

void 
l_timer_del(L_Timer_T *t);

void 
l_timer_update(L_Timer_T *t, UI32_T value);

UI32_T 
l_timer_query(L_Timer_T *t);

void 
l_timer_master_process(L_TIMER_MASTER_T *m);

void
l_timer_master_init(L_TIMER_MASTER_T *m);

void 
l_timer_master_debug(L_TIMER_MASTER_T *m);

void 
l_timer_list_debug(L_TIMTER_LIST_T *list, UI8_T *str, UI32_T line);

#endif
