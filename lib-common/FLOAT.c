#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	return ((long long)a * b) / (1 << 16);
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	return (a / b) * (1 << 16);
}

FLOAT f2F(float a) {
	unsigned *A = (unsigned*)&a;
	unsigned sign = (*A) & 0x80000000;
	unsigned expo = ((*A) & 0x7F800000) >> 23;
	unsigned ret = (*A) & 0x7FFFFF;
	if(expo == 0) return sign;
	else
	{
		ret += 0x800000;
		//ret <<= -23 + (expo - 127) + 16; (e.i. ret <<= expo - 134;
		if(expo >= 134) ret <<= expo - 134;
		else ret >>= 134 - expo;
		return (ret & 0x7FFFFFFF) | sign;
	}
	//return (FLOAT)(a * (1 << 16));
}

FLOAT Fabs(FLOAT a) {
	return a & 0x7FFFFFFF;
}

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

