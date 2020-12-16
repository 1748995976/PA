#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = {.NO = -1, .next = NULL, .pre = NULL}, *free = {.NO = -1, .next = NULL, .pre = NULL};

void insert_wp_before(WP* old,WP* wp){
  WP *pre = old->pre;
	wp->next = old;
	wp->pre = pre ;
	pre->next = wp;
	old->pre = wp;
}

WP* remove_wp(WP* wp){
  WP* pre = wp->pre;
  WP* next = wp->next;
  pre->next = next;
  next->pre = pre;
  wp->next = wp->pre = NULL;
  return wp;
}

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].old_val = 0;
    memset(wp_pool[i].expression, 0, sizeof(wp_pool[i].expression));
    wp_pool[i].next = NULL;
    wp_pool[i].pre = NULL;
  }
  head->next = head->pre = head;
  free->next = free->pre = head;

  for (i = 0; i < NR_WP; i++)
  {
    insert_wp_before(free,&wp_pool[i]);
  }
  
}

/* TODO: Implement the functionality of watchpoint */
WP *new_wp(){

	assert(free->next != free);

	WP *wp = remove_wp(free->next);
	insert_wp_before(head, wp);
	return wp;
}

void free_wp(int index_delete){

	assert(index_delete >= 0 && index_delete < NR_WP);

  if(strlen(wp_pool[index_delete].expression) == 0){
    printf("Don't delete the watchpoint that has been deleted!");
    return;
  }
	WP *wp = remove_wp(&wp_pool[index_delete]);
	wp->old_val = 0;
	memset(wp->expression, 0, sizeof(wp->expression));
	insert_wp_before(free, wp);
  printf("Watchpoint %d deleted\n", index_delete);
}

WP *get_head(){
	return head;
}