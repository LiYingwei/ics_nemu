#include "trap.h"






int fun20() {
	return 13;
}
int fun19() {
	return fun20() * fun20();
}
int fun18() {
	return fun19() * fun20();
}
int fun17() {
	return fun18() * fun18() * fun18();
}
int fun16() {
	return fun17() * fun18() * fun19() * fun20();
}
int fun15() {
	return fun16();
}
int fun14() {
	return fun15();
}
int fun13() {
	return fun14();
}
int fun12() {
	return fun13();
}
int fun11() {
	return fun12();
}
int fun10() {
	return fun11();
}
int fun9() {
	return fun10();
}
int fun8() {
	return fun9();
}
int fun7() {
	return fun8();
}
int fun6() {
	return fun7();
}
int fun5() {
	return fun6();
}
int fun4() {
	return fun5();
}
int fun3() {
	return fun4();
}
int fun2() {
	return fun3();
}
int fun1() {
	return fun2();
}

int main() {
	int ans = fun1();
	nemu_assert(ans != 0);
	HIT_GOOD_TRAP;
	return 0;
}

