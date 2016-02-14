#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	return ((long long)a * b) >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	int sign = (a < 0 ? -1 : 1) * (b < 0 ? -1 : 1);
	if(a < 0) a = -a;
	if(b < 0) b = -b;
	int ret = a / b;
	unsigned r = a % b;
	int i = 0;
	for(i = 0; i < 16; i++)
	{
		ret *= 2;
		r *= 2;
		while(r >= b) ret++, r-=b;
	}
	return ret * sign;
}

FLOAT f2F(float a) {
	/*unsigned *A = (unsigned*)&a;
	unsigned sign = ((*A) & 0x80000000) ? -1 : 1;
	unsigned expo = ((*A) & 0x7F800000) >> 23;
	unsigned ret = (*A) & 0x7FFFFF;
	if(expo == 0) return (*A) & 0x80000000;
	else
	{
		ret += 0x800000;
		//ret <<= -23 + (expo - 127) + 16; (e.i. ret <<= expo - 134;
		if(expo >= 134) ret <<= expo - 134;
		else ret >>= 134 - expo;
		return (ret & 0x7FFFFFFF) * sign;
	}*/
	int ans = 0, mul = 0;
    union{
        float x;
        unsigned y;
    }pp;
    pp.x = a;
    mul = (pp.y >> 23) & 0x7f;
    mul -= 127;
    ans = (pp.y & 0x7fffff) + 0x800000;
    if (mul > 0) ans <<= mul;
    else ans >>= mul * - 1;
    ans >>= 7;
	return ans * (pp.y >> 31 ? - 1 : 1);
}

FLOAT Fabs(FLOAT a) {
	/*return a & 0x7FFFFFFF;*/
	return a > 0 ? a : -a;
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

