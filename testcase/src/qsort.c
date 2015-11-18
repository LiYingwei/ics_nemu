#include "trap.h"
#include<stdlib.h>
int a[100000];
int cmp(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}
int main()
{
	int i=0;
	for(i=0;i<100000;i++) a[i] = 100000-i;
	qsort(a,100000,sizeof(a[0]),cmp);
	for(i=0;i<100000;i++) nemu_assert(a[i]==i+1);
	HIT_GOOD_TRAP;
	return 0;
}
