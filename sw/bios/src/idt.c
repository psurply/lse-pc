#include <bios.h>
#include <frame_alloc.h>
#include <idt.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <uart.h>

struct idt_gate {
	u16 offset_low : 16;
	u16 segment_selector : 16;
	u8 always0 : 8;
	u8 type : 5;
	u8 dpl : 2;
	u8 present : 1;
	u16 offset_high : 16;
} __packed;

struct idtr {
	u16 limit : 16;
	u32 base : 32;
} __packed;

static const char *exceptions_str[] = {
#define X(n, ec, str) str,
#include <idt.def>
#undef X
};

void exception_handler(struct intr_regs *r)
{
	if (r->intr_nb == PANIC_INT)
		printf("BIOS Panic: %s\n", r->panic_msg);
	else
		printf("Exception: %s (#%d)\n",
			exceptions_str[r->intr_nb], r->intr_nb);
	printf("EAX: %p, EBX: %p, ECX: %p, EDX: %p\n",
		r->eax, r->ebx, r->ecx, r->edx);
	printf("ESI: %p, EDI: %p, EBP: %p, ESP: %p\n",
		r->esi, r->edi, r->ebp, r->esp);
	printf("CS: %x, EIP: %p, EFLAGS: %x\n",
		r->cs, r->eip, r->eflags);

	__asm__ volatile ("hlt");
}

typedef void (*isr_handler)(void);
static void add_isr(struct idt_gate *idt, u8 n, isr_handler handler)
{
#define KERNEL_CS_SEL	0x08
#define TRAP_GATE	0x0f
#define PRIVILEGE_LEVEL	0
	idt[n].offset_low = (u32)handler;
	idt[n].segment_selector = KERNEL_CS_SEL;
	idt[n].always0 = 0;
	idt[n].type = TRAP_GATE;
	idt[n].dpl = PRIVILEGE_LEVEL;
	idt[n].present = 1;
	idt[n].offset_high = ((u32)handler >> 16) & 0xffff;
}

static void load_idt(void *base, u16 limit)
{
	struct idtr idtr = {
		.base = (u32)base,
		.limit = limit,
	};

	__asm__ volatile ("lidt %0\n\t"
		: /* No output */
		: "m" (idtr)
		: "memory");
}

void setup_minimal_idt(void)
{
#define IDT_SIZE 32
	struct idt_gate *idt = frame_to_virt(alloc_frame());

	memset(idt, 0, sizeof(struct idt_gate) * IDT_SIZE);

#define X(i, ec, s) add_isr(idt, i, isr##i);
#include <idt.def>
#undef X

	load_idt(idt, sizeof(struct idt_gate) * IDT_SIZE);
	printf("IDT: %p\n", idt);
}
