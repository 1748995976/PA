#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  struct watchpoint *pre;
  char expression[128];
  uint32_t old_val;

} WP;

WP *new_wp();
void free_wp(int nr_to_delete);
WP *get_head();

#endif
