#include <bios.h>
#include <frame_alloc.h>
#include <fs.h>
#include <stdio.h>
#include <string.h>
#include <uart.h>

#define FS_NB_BLOCKS	128

#define CPIO_MAGIC0		0xC7
#define CPIO_MAGIC1		0x71

struct cpio_hdr {
	u16 c_magic;
	u16 c_dev;
	u16 c_ino;
	u16 c_mode;
	u16 c_uid;
	u16 c_gid;
	u16 c_nlink;
	u16 c_rdev;
	u16 c_mtime[2];
	u16 c_namesize;
	u16 c_filesize[2];
} __packed;

static u8 first_free_block;
static struct fs_block *fs;

static unsigned int fs_find(const char *path)
{
	for (unsigned int i = fs[0].next; i; i = fs[i].dnext) {
		if (strncmp(fs[i].name, path, FS_NAME_SIZE) == 0) {
			return i;
		}
	}

	return 0;
}

static void fs_read_file(unsigned int i)
{
	printf("%s\n", fs[i].data);
}

void fs_stats(void)
{
	unsigned int free = 0;

	for (u8 i = first_free_block; i; i = fs[i].next)
		++free;

	printf("File system blocks:\n\tUsed: %d\n\tFree: %d\n",
		FS_NB_BLOCKS - free, free);
}

static size_t fs_get_file_size(unsigned int i)
{
	if (fs[i].type != FS_TYPE_INODE)
		return 0;

	size_t s = 0;

	for (; i; i = fs[i].next)
		s += fs[i].size;

	return s;
}

void fs_list_dir(const char *path)
{
	unsigned int d = 0;

	if (path) {
		d = fs_find(path);

		if (d == 0) {
			printf("Cannot find %s\n", path);
			return;
		}
	}

	if (fs[d].type != FS_TYPE_DIR) {
		printf("%s is not a directory\n", path);
		return;
	}

	for (unsigned int i = fs[d].next; i; i = fs[i].dnext) {
		printf("%cr%c %d\t%s\n",
			fs[i].type == FS_TYPE_DIR ? 'd' : '-',
			fs[i].flags & FS_RW ? 'w' : '-',
			fs_get_file_size(i),
			fs[i].name);
	}
}

void fs_print_file(const char *path)
{
	unsigned int d = fs_find(path);

	if (d == 0) {
		printf("Cannot find %s\n", path);
		return;
	}

	if (fs[d].type != FS_TYPE_INODE) {
		printf("%s is not a regular file\n", path);
		return;
	}

	for (; d; d = fs[d].next)
		for (unsigned int i = 0; i < fs[d].size; ++i)
			printf("%c", fs[d].data[i]);
}

static u8 fs_find_free_block()
{
	if (first_free_block == 0)
		panic("Not enough free block");

	u8 b = first_free_block;
	first_free_block = fs[first_free_block].next;

	return b;
}

static int fs_recvb_cpio(u8 *c)
{
	for (;;) {
		if (CAN_RECEIVE(COM1)) {
			if (uart1_readb() == 'q')
				return 1;
			printf("Waiting for cpio archive on COM2 (press q to abort)\n");
		}
		if (CAN_RECEIVE(COM2)) {
			*c = uart2_readb();
			return 0;
		}
	}

	return 0;
}

void fs_recv_cpio(void)
{
	unsigned int cur;
	u8 *buff = frame_to_virt(alloc_frame());
	struct cpio_hdr *hdr = (struct cpio_hdr *) buff;

	char *filename;
	u32 filesize;

	for (;;) {
		if (fs_recvb_cpio(buff))
			goto free_buff;
		if (buff[0] != CPIO_MAGIC0)
			continue;

		if (fs_recvb_cpio(buff + 1))
			goto free_buff;
		if (buff[1] != CPIO_MAGIC1)
			continue;

		for (cur = 2; cur < sizeof (struct cpio_hdr); ++cur)
			if (fs_recvb_cpio(buff + cur))
				goto free_buff;

		filename = buff + cur;
		if (hdr->c_namesize + cur > PAGE_SIZE)
			panic("Filename too long");

		for (unsigned int i = 0; i < hdr->c_namesize; ++i, ++cur)
			if (fs_recvb_cpio(buff + cur))
				goto free_buff;

		if (strcmp(filename, "TRAILER!!!") == 0) {
			printf("cpio successfully loaded\n");
			goto free_buff;
		}

		filesize = (hdr->c_filesize[0] << 16) | hdr->c_filesize[1];
		printf("Receiving file: %s (%d bytes)...", filename, filesize);

		u8 b = fs_find_free_block();
		fs[b].type = FS_TYPE_INODE;
		fs[b].flags = FS_RW;
		fs[b].dnext = fs[0].next;
		fs[0].next = b;
		strncpy(fs[b].name, filename, MIN(FS_NAME_SIZE, hdr->c_namesize));

		cur = 0;
		while (cur < filesize) {
			fs[b].data = frame_to_virt(alloc_frame());
			fs[b].size = MIN(PAGE_SIZE, filesize - cur);

			for (unsigned int i = 0; i < PAGE_SIZE
				&& cur < filesize; ++i, ++cur)
				if (fs_recvb_cpio(fs[b].data + i))
					goto free_buff;

			if (cur < filesize) {
				u8 nb = fs_find_free_block();
				fs[b].next = nb;
				b = nb;
				fs[b].type = FS_TYPE_DBLK;
			}
		}

		fs[b].next = 0;
		printf("done\n");
	}

free_buff:
	free_frame(virt_to_frame(buff));
}

void init_fs(void)
{
	fs = frame_to_virt(alloc_frame());

	for (unsigned int i = 0; i < FS_NB_BLOCKS; ++i) {
		fs[i].type = FS_TYPE_FREE;
		fs[i].next = i + 1;
		fs[i].dnext = 0;
	}

	fs[FS_NB_BLOCKS - 1].next = 0;

	fs[0].type = FS_TYPE_DIR;
	fs[0].flags = FS_RO;
	fs[0].next = 0;
	fs[0].size = 0;
	fs[0].dnext = 0;

	first_free_block = 1;

	printf("FS: %p\n", fs);
}
