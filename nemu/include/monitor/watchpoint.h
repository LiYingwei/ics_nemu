#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
<<<<<<< HEAD
	char *expr;
	uint32_t val;

} WP;

WP* new_wp(char *expr);

void free_wp(int N);

bool check_wp(bool show);
=======


} WP;
>>>>>>> 1dd62864fe9b4d0aafd2e0d81070f60c1e34058f

#endif
