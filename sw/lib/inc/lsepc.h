#ifndef LSEPC_H
#define LSEPC_H

#include <stdint.h>

#define LSEPC_ST_STOP	0
#define LSEPC_ST_RUN	1
#define LSEPC_ST_BRK16	2
#define LSEPC_ST_BRK32	3
#define LSEPC_ST_IOWR	4
#define LSEPC_ST_IORD	5

struct lsepc;

struct lsepc_regs
{
	uint8_t valid;

	uint32_t eflags;
	uint32_t eip;

	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;

	uint16_t cs;
} __attribute__((packed));

struct lsepc *lsepc_init(const char *dev);
void lsepc_delete(struct lsepc *lsepc);

/* Commands */
int lsepc_get_version(struct lsepc *lsepc, char *verison, int size);
int lsepc_start(struct lsepc *lsepc);
int lsepc_stop(struct lsepc *lsepc);
int lsepc_brk(struct lsepc *lsepc);
int lsepc_cont(struct lsepc *lsepc);
int lsepc_get_status(struct lsepc *lsepc);
int lsepc_get_regs(struct lsepc *lsepc, struct lsepc_regs *regs);
int lsepc_set_io_data(struct lsepc *lsepc, uint16_t data);
int lsepc_get_io_data(struct lsepc *lsepc);
int lsepc_get_io_addr(struct lsepc *lsepc);

#endif /* LSEPC_H */
