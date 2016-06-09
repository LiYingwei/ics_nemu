#include "irq.h"

#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
void mm_brk(uint32_t);

extern int fs_open(const char *pathname, int flags);
extern int fs_read(int fd, void *buf, int len);
extern int fs_write(int fd, void *buf, int len);
extern int fs_lseek(int fd, int offset, int whence);
extern int fs_close(int fd);

static void sys_brk(TrapFrame *tf) {
#ifdef IA32_PAGE
	mm_brk(tf->ebx);
#endif
	tf->eax = 0;
}
void serial_printc(char ch);

static void sys_write(TrapFrame *tf) {
    int i;
	int fd = tf->ebx;
	uint32_t buf = tf->ecx;
	size_t len = tf->edx;
	
	if(fd == 1 || fd == 2) 
	{
		//asm volatile (".byte 0xd6" : : "a"(2), "c"(buf), "d"(len));
        for(i = 0; i < len; i++) {
            char *output = (char *)(buf + i);
            serial_printc(*output);
        }
        if(len == 0) tf->eax = (uint32_t) -1;
        else tf->eax = 0;
	} else {
        tf->eax = fs_write(tf->ebx, (void *)tf->ecx, tf->edx);
    }
	/*if((fd == 1 || fd == 2) && (len == 0)) tf->eax = -1;
	else tf->eax = 0;*/
}

static void sys_open(TrapFrame *tf) {
    tf->eax = fs_open((void *) tf->ebx, tf->ecx);
}

static void sys_close(TrapFrame *tf) {
    tf->eax = fs_close(tf->ebx);
}

static void sys_read(TrapFrame *tf) {
    tf->eax = fs_read(tf->ebx, (void *) tf->ecx, tf->edx);
}

static void sys_lseek(TrapFrame *tf) {
    tf->eax = fs_lseek(tf->ebx, tf->ecx, tf->edx);
}

void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The ``add_irq_handle'' system call is artificial. We use it to 
		 * let user program register its interrupt handlers. But this is 
		 * very dangerous in a real operating system. Therefore such a 
		 * system call never exists in GNU/Linux.
		 */
		case 0: 
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk: sys_brk(tf); break;
		
		case SYS_write: sys_write(tf); break;
		
        case SYS_open: sys_open(tf); break;

        case SYS_read: sys_read(tf); break;

        case SYS_lseek: sys_lseek(tf); break;

        case SYS_close: sys_close(tf); break;

		/* TODO: Add more system calls. */

		default: panic("Unhandled system call: id = %d", tf->eax);
	}
}

