#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <cpu/reg.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KRESET "\033[0m"
extern uint32_t findsym(char* var);
enum {
	NOTYPE = 256, NUM, HEXNUM, REG, VAR,
	UNARY_PLUS = 33, UNARY_MINUS = 34, LNOT = 35, BNOT = 36, IND = 38,

	PLUS = 43, MINUS = 45, MULTIPLY = 42, DIVISION = 47, LEFT_PARENTHESES = 40, RIGHT_PARENTHESES = 41,
	MOD = 53,
	BLSHIFT = 71, BRSHIFT = 72,
	LESSTHAN = 81 ,LESSOREQ = 82, GREATOREQ = 84, GREATTHAN = 83,
	EQ = 91, NEQ = 92,
	BITAND = 101, BITOR = 111, BITXOR = 121, LAND = 131, LOR = 141,

	//DIRASS = 161, ASSSUM = 162, ASSDIF = 163, ASSPRO = 164, ASSQUO = 165, ASSREM = 166,
	//ASSLS = 167, ASSRS = 168,ASSAND = 169, ASSXOR = 170, ASSOR = 171,

};

static struct rule {
	char *regex;
	int token_type;
	int precedence;  // -1为不适用 优先级参考资料：https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE, -1},				// spaces

	{"0x[0-9a-fA-F]+", HEXNUM, -1},
	{"\\$[a-zA-Z]+", REG, -1},
	{"[0-9]+", NUM, -1},				// 不知道为什么"\\d+"不行

	//{"<<=", ASSLS, 16},
	//{">>=", ASSRS, 16},

	{"<<", BLSHIFT, 7},
	{">>", BRSHIFT, 7},
	{"<=", LESSOREQ, 8},
	{">=", GREATOREQ, 8},
	{"==", EQ, 9},						// equal
	{"!=",NEQ, 9},
	{"&&", LAND, 13},
	{"\\|\\|", LOR,  14},

	/*{"\\+=", ASSSUM, 16},
	{"-=", ASSDIF, 16},
	{"*=", ASSPRO, 16},
	{"/=", ASSQUO, 16},
	{"%=", ASSREM, 16},
	{"&=", ASSAND, 16},
	{"\\^=", ASSXOR, 16},
	{"\\|=", ASSOR, 16},
	 */

	{"\\+", '+', 6},					// plus  '+' = 75
	{"-",   '-', 6},					// minus '-' = 77
	{"!", LNOT, 3},
	{"~", BNOT, 3},
	{"\\*", '*', 5},					// multiply '*' = 74
	{"/",   '/', 5},					// division '/' = 79
	{"%", MOD, 5},
	{">", GREATTHAN, 8},
	{"<", LESSTHAN, 8},
	{"\\(", '(', -1},                   //          '(' = 72
	{"\\)", ')', -1},                   //          ')' = 73
	{"&", BITAND, 10},
	{"\\^", BITXOR, 11},
	{"\\|", BITOR, 12},
	//{"=", DIRASS, 16},


	{"\\*", IND, 3},
	{"[^0-9\\)]-", UNARY_MINUS, 3},
	{"[^0-9\\)]\\+", UNARY_PLUS,  3},
    {"[A-Za-z_][A-Za-z0-9_]*", VAR, -1}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
	int precedence;
} Token;

Token tokens[128];

int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain
				 * types of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE: break;
                    case VAR :
					case NUM :
					case HEXNUM:
					case REG:
						assert(substr_len < 32);
                        memset(tokens[nr_token].str, 0, sizeof(tokens[nr_token]));
						memcpy(tokens[nr_token].str, substr_start, (size_t) substr_len);
					default:
						tokens[nr_token].type = rules[i].token_type;
						tokens[nr_token++].precedence = rules[i].precedence;
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	for(i = 0; i < nr_token; i++)
	{
		if(tokens[i].type == MINUS || tokens[i].type == PLUS || tokens[i].type == MULTIPLY) // HERE MAY BE HAVE SOME BUG!
		{
			if (i == 0 || (tokens[i-1].type != ')' &&
                    tokens[i-1].type != NUM && tokens[i-1].type != REG &&
                    tokens[i-1].type != HEXNUM && tokens[i-1].type != VAR) ) {
				switch (tokens[i].type) {
					case PLUS: tokens[i].type = UNARY_PLUS; break;
					case MINUS:tokens[i].type = UNARY_MINUS;break;
					case MULTIPLY:tokens[i].type = IND; break;
					default: assert(0);
				}
				tokens[i].precedence = 3;
			}
		}
		//Log("tokens[%d] = %d(%d)(%s)",i,tokens[i].type, tokens[i].type,tokens[i].str);
	}

	return true; 
}

bool check_parentheses(int p, int q) {
	if(tokens[p].type != '(' || tokens[q].type != ')') return false;
	int i, cnt = 0;
	for(i = p; i <= q; i++) {
		if(tokens[i].type == '(') cnt ++;
		if(tokens[i].type == ')') {
			cnt --;
			if(cnt == 0) return (bool) (i == q);
		}
	}
	return false;
	//return (bool) (tokens[p].type == '(' && tokens[q].type == ')'); // 为什么要强制类型转换？ch
}

int get_dominant_op(int p,int q) {
	int op = -1, op_pre = 0 , i, cnt_parentheses = 0;
	for(i = q; i >= p; i--) {
		if(cnt_parentheses == 0 && op_pre < tokens[i].precedence)
		{
			op = i;
			op_pre = tokens[i].precedence;
		}
		else if(tokens[i].type == ')') cnt_parentheses --;
		else if(tokens[i].type == '(') cnt_parentheses ++;
	}
	//Log("op = %d", tokens[op].type);
	return op;
}

uint32_t eval(int p, int q, bool *success) {
	//Log("EVAL %d %d",p, q);
	if (p > q) {
		//*success = false;
		//printf("BAD EXPRESSION\n");
		return 0;
	}
	else if (p == q) {
		uint32_t ret;
		char buf[32];
		int i;
		switch (tokens[p].type) {
			case NUM:
				sscanf(tokens[p].str, "%u", &ret);
				return ret;
			case HEXNUM:
				sscanf(tokens[p].str, "%X", &ret);
				return ret;
            case VAR:
                return findsym(tokens[p].str);
			case REG:
                memset(buf, 0, sizeof(buf));
				sscanf(tokens[p].str + 1, "%s", buf);
				for (i = R_EAX; i <= R_EDI; i++) {
					if (strcmp(buf, regsl[i]) == 0)
						return cpu.gpr[i]._32;
				}
				for (i = R_AX; i <= R_DI; i++) {
					if (strcmp(buf, regsw[i]) == 0)
						return cpu.gpr[i]._16;
				}
				for (i = R_AL; i <= R_BH; i++) {
					if (strcmp(buf, regsb[i]) == 0)
						return cpu.gpr[i % 4]._8[i / 4];
				}
                for(i = E_CF; i <= E_VM; i++)
                {
                    if(strcmp(buf, regeflags[i]) == 0)
                        return (cpu.EFLAGS & (1 << i)) >> i;
                }
                for(i = R_ES; i <= R_GS; i++) {
                    if(strcmp(buf, segsw[i]) == 0)
                        return seg_w(i);
                }
                if(strcmp(buf, "eip") == 0)
                    return cpu.eip;
                if(strcmp(buf, "eflags") == 0)
                    return cpu.EFLAGS;
                if(strcmp(buf, "cro") == 0)
                    return cpu.CR0;
                if(strcmp(buf, "crE") == 0)
                    return cpu.CR3;

			default:
				Assert(0,"tokens[%d].type = %d",p,tokens[p].type);
		}
	}
	else if (check_parentheses(p, q)) {
		return eval(p + 1, q - 1, success);
	}
	else {
		uint32_t val1, val2;
		int op;
		op = get_dominant_op(p, q);
		if(op < 0) {
			*success = false;
			printf("BAD EXPRESSION or INNER ERROR\n");
			return 0;
		}
		val1 = eval(p, op - 1, success);
		val2 = eval(op + 1, q, success);
		//Log("[%d,%d] val1=%u val2=%u\n", p, q, val1, val2);
		switch (tokens[op].type) {
			case UNARY_MINUS: return (uint32_t) -val2;
			case UNARY_PLUS: return val2;
			case IND: return swaddr_read(val2, 4, R_DS);
			case LNOT: return (uint32_t) !val2;
			case BNOT: return ~val2;

			case EQ: return (uint32_t) (val1 == val2);
			case NEQ: return (uint32_t) (val1 != val2);
			case '+': return val1 + val2;
			case '-': return val1 - val2;
			case '*': return val1 * val2;
			case '/': return val1 / val2;
			case MOD: return val1 % val2;
			case BLSHIFT: return val1 << val2;
			case BRSHIFT: return val1 >> val2;
			case LESSTHAN: return (uint32_t) (val1 < val2);
			case LESSOREQ: return (uint32_t) (val1 <= val2);
			case GREATOREQ: return (uint32_t) (val1 >= val2);
			case GREATTHAN: return (uint32_t) (val1 > val2);
			case BITAND: return val1 & val2;
			case BITOR: return val1 | val2;
			case BITXOR: return val1 ^ val2;
			case LAND: return (uint32_t) (val1 && val2);
			case LOR: return (uint32_t) (val1 || val2);

			default:
				*success = false;
				printf("BAD EXPRESSION or INNER ERROR\n");
				return 0;
		}
	}

}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	/* TODO: Insert codes to evaluate the expression. */
	*success = true;
	uint32_t ret = (uint32_t) eval(0, nr_token -1, success);
	return ret;
}

