#include "trap.h"
#include<stdlib.h>
#define N 5
int a[N];
int cmp(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}
int main()
{
	int i=0;
	for(i=0;i<N;i++) a[i] = N-i;
	qsort(a,N,sizeof(a[0]),cmp);
	for(i=0;i<N;i++) nemu_assert(a[i]==i+1);
	HIT_GOOD_TRAP;
	return 0;
}
