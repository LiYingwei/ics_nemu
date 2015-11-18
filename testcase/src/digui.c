#include "trap.h"






int fun20() {
	set_bp();
	return 13;
}
int fun19() {
	set_bp();
	return fun20();
}
int fun18() {
	set_bp();
	return fun19();
}
int fun17() {
	set_bp();
	return fun18();
}
int fun16() {
	set_bp();
	return fun17();
}
int fun15() {
	set_bp();
	return fun16();
}
int fun14() {
	set_bp();
	return fun15();
}
int fun13() {
	set_bp();
	return fun14();
}
int fun12() {
	set_bp();
	return fun13();
}
int fun11() {
	set_bp();
	return fun12();
}
int fun10() {
	set_bp();
	return fun11();
}
int fun9() {
	set_bp();
	return fun10();
}
int fun8() {
	set_bp();
	return fun9();
}
int fun7() {
	set_bp();
	return fun8();
}
int fun6() {
	set_bp();
	return fun7();
}
int fun5() {
	set_bp();
	return fun6();
}
int fun4() {
	set_bp();
	return fun5();
}
int fun3() {
	set_bp();
	return fun4();
}
int fun2() {
	set_bp();
	return fun3();
}
int fun1() {
	set_bp();
	return fun2();
}

int main() {
	int ans = fun1();
	nemu_assert(ans != 0);
	HIT_GOOD_TRAP;
	return 0;
}

