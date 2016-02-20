#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <cpu/reg.h>
#include <memory/cache.h>


#define CMD_STATUS_VALID 1
#define CMD_STATUS_NOT_VALID 0


void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args) {
	int N, i;
	bool status = CMD_STATUS_VALID;
	char *token, *saveptr;
	if(args == NULL) {
		cpu_exec(1);
		return 0;
	}
	token = strtok_r(args, " ", &saveptr);
	N = atoi(token);
	// to check the token is a number
	for(i = 0; token[i]; i++)
		if(token[i] < '0' || token[i] > '9')
		{
			status = CMD_STATUS_NOT_VALID;
			break;
		}
	token = strtok_r(NULL, " ", &saveptr);
	if(token != NULL) status = 1;
	if(status == CMD_STATUS_NOT_VALID) {
		printf("args is not valid\nsi [N]\n");
		return 0;
	}
	cpu_exec((uint32_t) N);
	return 0;
}

static int cmd_info_r() {
	static CPU_state last_cpu_state; // to highlight the changed register
	char R[8][4] = {"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"};
	int i;
	for(i = R_EAX; i <= R_EDI; i++) {
		printf("%s\t", R[i]);
		if(last_cpu_state.gpr[i]._32 != cpu.gpr[i]._32) printf(KRED);
		printf("0x%08X\t", cpu.gpr[i]._32);
		printf(KRESET);
		if(i % 4 == 3) printf("\n");
	}
	printf("EIP\t");
	if(last_cpu_state.eip != cpu.eip) printf(KRED);
	printf("0x%08X\n", cpu.eip);
	printf(KRESET);
	last_cpu_state = cpu;
	return 0;
}

static int cmd_info_w() {
	check_wp(true);
	return 0;
}

static int cmd_info(char *args) {
	bool status = CMD_STATUS_VALID;
	char *token1 = NULL, *token2 = NULL, *saveptr;
	if(args == NULL) status = CMD_STATUS_NOT_VALID;
	if(status) token1 = strtok_r(args, " ", &saveptr);
	if(status) token2 = strtok_r(NULL, "", &saveptr);
	if(status && token1 == NULL) status = CMD_STATUS_NOT_VALID;
	if(status && token2 != NULL) status = CMD_STATUS_NOT_VALID;
	if(status && (token1[0] != 'r' && token1[0] != 'w')) status = CMD_STATUS_NOT_VALID;
	if(status == CMD_STATUS_NOT_VALID) {
		printf("args is not valid\ninfo [r|w]\n");
		return 0;
	}
	if(token1[0] == 'r') return cmd_info_r();
	else return cmd_info_w();
}

static int cmd_p(char *args) {
	bool success;
	if(args == NULL)
	{
		printf("args can't be empty\np EXPR\n");
		return 0;
	}
	int ans = expr(args, &success);
	if(success) printf("ans = %08X(%d)\n", ans, ans);
	return 0;
}

static int cmd_x(char *args) {
	char *token, *saveptr, *exprstring;
	bool success;
	size_t len;
	swaddr_t addr;
	int i;

	token = strtok_r(args, " ", &saveptr);
	if (token == NULL) {
		printf("args is not valid\nx [N] expr\n");
		return 0;
	}

	exprstring = strtok_r(NULL, "", &saveptr);
	if (exprstring == NULL) {
		printf("args is not valid\nx [N] expr\n");
		return 0;
	}

	sscanf(token, "%lu", &len);
	addr = expr(exprstring, &success);
	for (i = 0; i < len; i++, addr++) {
		uint32_t ret;
		if(addr >= (1 << (10 + 10 + 3 + (27 - 10 - 10 - 3)))) {
			printf("address is out of boundary, maybe expr's inner error or input error\n");
			return 0;
		}
		ret = swaddr_read(addr, 1, R_DS);
		printf("%02X%c", ret, (i % 5 == 4 || i == len - 1) ? '\n' : ' ');
	}
	return 0;
}

static int cmd_w(char *args) {
	new_wp(args);
	return 0;
}

static int cmd_d(char *args) {
	int N;
	sscanf( args, "%d", &N);
	free_wp(N);
	return 0;
}

typedef struct {
    swaddr_t prev_ebp;
    swaddr_t ret_addr;
    uint32_t args[4];
} PartOfStackFrame;
extern char* getname(swaddr_t addr);

static int cmd_bt(char *args) {
    swaddr_t EBP = cpu.ebp;
    PartOfStackFrame psf;
    int i,index=0;
    char *name = getname(cpu.eip);
    printf("#%d\t0x%x in %s ()\n", index++, cpu.eip, name);
    while(EBP)
    {
        psf.prev_ebp = swaddr_read(EBP,4, R_SS);
        //if(EBP + 4 < HW_MEM_SIZE)
            psf.ret_addr = swaddr_read(EBP+4,4, R_SS);
        for(i=0;i<4;i++)
            //if(EBP + 8 + i * 4 < HW_MEM_SIZE)
                psf.args[i] = swaddr_read(EBP+8+i*4,4, R_SS);
        name = getname(psf.ret_addr);
        EBP = psf.prev_ebp;
        if(EBP)printf("#%d\t0x%x in %s ()\n", index++, psf.ret_addr, name);
    }
    return 0;
}

static int cmd_help(char *args);

extern cache_type cache;
static int cmd_cache(char *args)
{
    uint32_t addr;
    sscanf(args, "%x", &addr);
    uint32_t tag = (addr >> (BLOCK_WIDTH + SET_WIDTH)) & TAG_MASK;
    uint32_t index = (addr >> BLOCK_WIDTH) & SET_MASK;
    uint32_t offset = addr & BLOCK_MASK;
    int hit = cache_check_hit(index, tag);
    if(hit == -1)
    {
        printf("not hit\n");
        return 0;
    }
    printf("hit!\n");
    printf("tag = %0X\nindex = %X\noffset = %X\n", tag, index, offset);
    printf("blockid = %u\n", hit);
    printf("datum: %X\n", cache.set[index].block[hit].data[offset]);
    return 0;
}
static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Step into", cmd_si },
	{ "info", "Print program status", cmd_info },
	{ "p", "Calcuate the Value of EXPR", cmd_p },
	{ "x", "Scan memory", cmd_x },
	{ "w", "Set watch point", cmd_w },
	{ "d", "Delete watch point", cmd_d },
    { "bt", "Print backtrace of all stack frames", cmd_bt},
    { "cache", "Search cache by ADDR", cmd_cache}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	char *last_cmd = NULL, *last_args = NULL;
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);
		char *args = NULL;

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) {
			if(last_cmd == NULL)continue;
			cmd = last_cmd;
			args = last_args;
		}
		else {
			/* treat the remaining string as the arguments,
			 * which may need further parsing
			 */
			args = cmd + strlen(cmd) + 1;
			if (args >= str_end) {
				args = NULL;
			}
			if(last_args != NULL) free(last_args);
			if(last_cmd != NULL) free(last_cmd);
			if(args != NULL) last_args = strdup(args);
			else last_args = NULL;
			last_cmd = strdup(cmd);
		}
#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
