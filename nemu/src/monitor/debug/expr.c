#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KRESET "\033[0m"

enum {
	NOTYPE = 256, EQ, NUM

	/* TODO: Add more token types */

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
	{"\\+", '+', 6},					// plus
	{"-",   '-', 6},					// minus
	{"\\*", '*', 5},					// multiply
	{"/",   '/', 5},					// division
	{"\\(", '(', -1},
	{"\\)", ')', -1},
	{"[0-9]+", NUM, -1},				// 不知道为什么"\\d+"不行
	{"==", EQ, 9}						// equal
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
			Assert(ret != 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
	int precedence;
} Token;

Token tokens[32];
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

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain
				 * types of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE: break;
					case NUM:
						assert(substr_len < 32);
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
		Log("tokens[%d] = %d(%c)(%s)",i,tokens[i].type, tokens[i].type,tokens[i].str);
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
	Log("op = %d", tokens[op].type);
	return op;
}

int eval(int p, int q, bool *success) {
	Log("EVAL %d %d",p, q);
	if (p > q) {
		*success = false;
		printf("BAD EXPRESSION\n");
		return 0;
	}
	else if (p == q) {
		return atoi(tokens[p].str);
	}
	else if (check_parentheses(p, q)) {
		return eval(p + 1, q - 1, success);
	}
	else {
		int op, val1, val2;
		op = get_dominant_op(p, q);
		if(op <= 0) {
			*success = false;
			printf("BAD EXPRESSION or INNER ERROR\n");
			return 0;
		}
		val1 = eval(p, op - 1, success);
		val2 = eval(op + 1, q, success);
		switch (tokens[op].type) {
			case '+': return val1 + val2;
			case '-': return val1 - val2;
			case '*': return val1 * val2;
			case '/': return val1 / val2;
			default: assert(0);
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
	int ret = eval(0, nr_token -1, success);
	return ret;
}

