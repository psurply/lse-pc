#ifndef FS_H
#define FS_H

#include <types.h>

#define FS_NAME_SIZE 22

#define FS_TYPE_FREE	0
#define FS_TYPE_DIR	1
#define FS_TYPE_INODE	2
#define FS_TYPE_DBLK	2

#define FS_RO		0
#define FS_RW		1

struct fs_block {
	u8 type;
	u8 flags;
	u8 dnext;
	u8 next;
	u16 size;
	u8 *data;
	char name[FS_NAME_SIZE];
} __packed;

void init_fs(void);
void fs_recv_cpio(void);
void fs_list_dir(const char *path);
void fs_print_file(const char *path);
void fs_stats(void);

#endif /* FS_H */
