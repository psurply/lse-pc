.macro ISR n ec
.global isr\n
isr\n:
.if \ec
	push $0
.endif
	push $\n
	jmp isr_common
.endm

#define X(n, ec, str) ISR n, ec
#include <idt.def>
#undef X

.extern panic
isr_common:
	pusha
	mov %esp, %eax
	call exception_handler
	popa
	iret
