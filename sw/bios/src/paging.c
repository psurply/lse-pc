#include <bios.h>
#include <memory.h>
#include <string.h>
#include <types.h>

struct pde {
	u8 p : 1;
	u8 rw : 1;
	u8 us : 1;
	u8 : 2;
	u8 a : 1;
	u8 d : 1;
	u8 : 1;
	u8 : 1;
	u8 avl : 3;
	u32 pt_addr : 20;
} __packed;

struct pte {
	u8 p : 1;
	u8 rw : 1;
	u8 us : 1;
	u8 : 2;
	u8 a : 1;
	u8 d : 1;
	u8 : 1;
	u8 : 1;
	u8 avl : 3;
	u32 frame_addr : 20;
} __packed;

#define PD_SIZE		1024
#define PT_SIZE		1024

#define USER_ENTRY	0
#define SYSTEM_ENTRY	1

#define RO_ENTRY	0
#define RW_ENTRY	1

#define ADDR_TO_PDE(X)	(((X) >> 22) & 0x03FF)
#define ADDR_TO_PTE(X)	(((X) >> 12) & 0x03FF)

static void set_pde(struct pde *pd, unsigned int idx, int us, struct pte *pt)
{
	pd[idx].p = 1;
	pd[idx].rw = RW_ENTRY;
	pd[idx].us = us;
	pd[idx].pt_addr = (u32) pt >> 12;
}

static void set_pte(struct pte *pt, unsigned int idx, int us, int rw, void *frame)
{
	pt[idx].p = 1;
	pt[idx].rw = rw;
	pt[idx].us = us;
	pt[idx].frame_addr = (u32) frame >> 12;
}

static inline void set_cr3(struct pde *pd)
{
	__asm__ volatile ("mov %0, %%cr3":: "a" (pd));
}

static inline void enable_paging(void)
{
	__asm__ volatile ("mov %%cr0, %%eax \n\t"
			"orl $0x80000000, %%eax \n\t"
			"movl %%eax, %%cr0":::"eax");
}

struct gdtr {
	u16 limit;
	u32 base;
} __packed;

extern struct gdtr gdtr;

void __section(".startup.setup_paging") setup_paging(void)
{
	struct pde *bios_pd = (struct pde *) PD_ADDR;
	struct pte *bios_pt = (struct pte *) PT_ADDR;

	memset(bios_pd, 0, PD_SIZE * sizeof (struct pde));
	memset(bios_pt, 0, PT_SIZE * sizeof (struct pte));

	for (unsigned long i = 0; i < PT_SIZE; ++i)
		set_pte(bios_pt, i, SYSTEM_ENTRY, RW_ENTRY, (void *) (i << 12));

	set_pde(bios_pd, ADDR_TO_PDE(0x00000000), SYSTEM_ENTRY, bios_pt);
	set_pde(bios_pd, ADDR_TO_PDE(0xC0000000), SYSTEM_ENTRY, bios_pt);

	set_cr3(bios_pd);
	enable_paging();

	__asm__ volatile ("orl $0xC0000000, %%esp\n\t"
			"movl $1f, %%eax\n\t"
			"orl $0xC0000000, %%eax\n\t"
			"jmp *%%eax\n\t"
			"1:"::: "eax");

	gdtr.base |= 0xC0000000;
	__asm__ volatile ("lgdt %0":: "m" (gdtr));

	bios_pd[ADDR_TO_PDE(0x00000000)].p = 0;
	set_cr3(bios_pd);

	main();
}
