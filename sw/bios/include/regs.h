#ifndef REGS_H
#define REGS_H

#include <types.h>

struct intr_regs {
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;

	u32 intr_nb;
	u32 error_code;

	u32 eip;
	u32 cs;
	u32 eflags;
	/* Undefined if the previous DPL was 0 */
	u32 esps;
	u32 ss;
	const char *panic_msg;
};


#endif /* REGS_H */
