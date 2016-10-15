#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <stdint.h>

#define ST_STOP		0
#define ST_RUNNING	1
#define ST_BRK16	2
#define ST_BRK32	3
#define ST_IOWR		4
#define ST_IORD		5

struct sv_regs
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

struct sv
{
	uint8_t status;
	uint16_t iodata;
	uint16_t ioaddr;
	struct dev *dev;

	struct sv_regs regs;
};

void sv_init(struct sv *sv);
int sv_stop(struct sv *sv);
int sv_start(struct sv *sv);
int sv_brk(struct sv *sv);
int sv_cont(struct sv *sv);
int sv_get_regs(struct sv *sv);
int sv_iobreak(struct sv *sv);

#endif /* SUPERVISOR_H */
