#include <malloc.h>
#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

WP* new_wp(char *expr) {
	assert(free_ != NULL);
	WP *wp = free_;
	free_ = wp->next;
	wp->next = head;
	head = wp;
	wp->expr = strdup(expr);
	return wp;
}

void free_wp(int N) {
	WP *wp;
	for(wp = head; wp->NO != N; wp = wp->next);
	assert(wp != NULL);
	free(wp->expr);
	WP *pre = head;
	while(pre->next != wp) pre = pre->next;
	pre->next = wp->next;
	wp->next = free_;
	free_ = wp;
}

bool check_wp(bool show) {
	WP *wp;
	char* buf[NR_WP];
	bool change[NR_WP];
	bool changed = false;
	bool success;
	int i;

	for(i = 0; i < NR_WP; i++) buf[i] = NULL;

	memset(change, 0, sizeof(change));

	for(wp = head; wp != NULL; wp = wp->next)
	{
		uint32_t val = expr(wp->expr, &success);
		if(val != wp->val) {
			changed = true;
			wp->val = val;
			change[wp->NO] = true;
		}
		buf[wp->NO] = malloc(30);
		sprintf(buf[wp->NO],"%d: %s = %u",wp->NO, wp->expr, wp->val);
	}
	if(changed || show)
	{
		for(i = 0; i < NR_WP; i++)
			if(buf[i] != NULL)
			{
				if(change[i]) printf(KRED);
				printf("%s\n", buf[i]);
				printf(KRESET);
				free(buf[i]);
			}
		return true;
	}
	return false;
}



