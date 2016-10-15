#include <pm.h>
#include <reset.h>

#define BEGIN_CODE	0xF8000

#define GDT_ENTRY_NULL	0
#define GDT_ENTRY_CODE	GDT_ENTRY((1 << 20) - 1, 0, CODE_TYPE, 1, 0, 0, 1, 1)
#define GDT_ENTRY_DATA	GDT_ENTRY((1 << 20) - 1, 0, DATA_TYPE, 1, 0, 0, 1, 1)

#define LJMP_OPCODE	0xEA66

.code16

.section .startup, "a"

/* GDT for flat protected mode */
pm:
	lgdt BASE_SEG_CODE + gdtr - pm

	mov %cr0, %eax
	or $CR0_PE, %eax
	mov %eax, %cr0

	/* Clear the prefetcher to avoid reloading segments for 16-bit mode */
	jmp reload_segs - 0xC0000000

reload_segs:
	mov $DATA_SELECTOR, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	mov $BASE_STACK, %esp

	/*
	 * As gas still consider we are in 16-bit mode but we did not reload the
	 * code segment yet, trick gas to be able to reference 32-bit address.
	 */
	.word LJMP_OPCODE
	.long setup_paging - 0xC0000000
	.word CODE_SELECTOR

gdt:
	.quad GDT_ENTRY_NULL
	.quad GDT_ENTRY_CODE
	.quad GDT_ENTRY_DATA

.global gdtr
gdtr:
	.word gdtr - gdt - 1
	.long BEGIN_CODE + gdt - pm
