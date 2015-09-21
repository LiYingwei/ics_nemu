#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <cpu/reg.h>


#define CMD_STATUS_VALID 1
#define CMD_STATUS_NOT_VALID 0

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KRESET "\033[0m"

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
	printf("coming soon...\n");
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
	if(success) printf("%s = %d\n", args, ans);
	return 0;
}

static int cmd_x(char *args) {
	char *token, *saveptr;
	bool success;
	size_t len;
	swaddr_t addr;
	int i;

	token = strtok_r(args, " ", &saveptr);
	sscanf(token, "%lu",&len);
	addr = expr(saveptr, &success);
	for(i = 0 ;i < len; i++, addr++) {
		uint32_t ret;

		ret = swaddr_read(addr, 1);
		printf("%02X%c", ret, i % 5 == 4 ? '\n' : ' ');
	}
	return 0;
}

static int cmd_help(char *args);

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
	{ "x", "Scan memory", cmd_x },
	{ "p", "Calcuate the Value of EXPR", cmd_p },
	/* TODO: Add more commands */

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
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
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
