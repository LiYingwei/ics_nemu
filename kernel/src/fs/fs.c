#include <string.h>
#include "common.h"

typedef struct {
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

enum {SEEK_SET, SEEK_CUR, SEEK_END};

/* This is the information about all files in disk. */
static const file_info file_table[] = {
	{"1.rpg", 188864, 1048576}, {"2.rpg", 188864, 1237440},
	{"3.rpg", 188864, 1426304}, {"4.rpg", 188864, 1615168},
	{"5.rpg", 188864, 1804032}, {"abc.mkf", 1022564, 1992896},
	{"ball.mkf", 134704, 3015460}, {"data.mkf", 66418, 3150164},
	{"desc.dat", 16027, 3216582}, {"fbp.mkf", 1128064, 3232609},
	{"fire.mkf", 834728, 4360673}, {"f.mkf", 186966, 5195401},
	{"gop.mkf", 11530322, 5382367}, {"map.mkf", 1496578, 16912689},
	{"mgo.mkf", 1577442, 18409267}, {"m.msg", 188232, 19986709},
	{"mus.mkf", 331284, 20174941}, {"pat.mkf", 8488, 20506225},
	{"rgm.mkf", 453202, 20514713}, {"rng.mkf", 4546074, 20967915},
	{"sss.mkf", 557004, 25513989}, {"voc.mkf", 1997044, 26070993},
	{"wor16.asc", 5374, 28068037}, {"wor16.fon", 82306, 28073411},
	{"word.dat", 5650, 28155717},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);

/* TODO: implement a simplified file system here. */

struct {
	bool opened;
	uint32_t offset;
} Fstate[NR_FILES + 3];

int fs_open(const char *pathname, int flags) {
	int i;
	for(i = 0; i < NR_FILES; i++) {
		if(strcmp(pathname,file_table[i].name) == 0) {
			Fstate[i + 3].opened = true;
			Fstate[i + 3].offset = 0;
			printk("syscall: open(\"%s\", %d) = %d\n", pathname, flags, i);
			return i + 3;
		}
	}
	nemu_assert(0);
    return 0;
}

int fs_read(int fd, void *buf, int len) {
	//int len_before = len;
	if(Fstate[fd].offset + len > file_table[fd - 3].size) 
		len = file_table[fd - 3].size - Fstate[fd].offset;
	ide_read(buf, file_table[fd - 3].disk_offset + Fstate[fd].offset, len);
	Fstate[fd].offset += len;
	//printk("syscall: read( %d, \"%s\", %d) = %d\n", fd, buf, len_before, len);
	return len;
}

int fs_write(int fd, void *buf, int len) {
	//int len_before = len;
	if(Fstate[fd].offset + len > file_table[fd - 3].size)
		len = file_table[fd - 3].size - Fstate[fd].offset;
	ide_write(buf, file_table[fd - 3].disk_offset + Fstate[fd].offset, len);
	Fstate[fd].offset += len;
	//printk("syscall: write( %d, \"%s\", %d) = %d\n", fd, buf, len_before, len);
	return len;
}
	
int fs_lseek(int fd, int offset, int whence) {
	unsigned new_offset = 0;
	if(whence == SEEK_SET) new_offset = offset;
	else if(whence == SEEK_CUR) new_offset = file_table[fd - 3].disk_offset + offset;
	else if(whence == SEEK_END) new_offset = file_table[fd - 3].size + offset;
	else panic("unknown whence %d", whence);
	Fstate[fd].offset = new_offset;
	return new_offset;
}

int fs_close(int fd) {
	Fstate[fd].opened = false;
	printk("syscall: close(%d) [fn = %s]\n", fd, file_table[fd - 3].name);
	return 0;
}












