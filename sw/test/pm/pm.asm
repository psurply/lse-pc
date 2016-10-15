bits 16
cpu 386

%define LJMP_OPCODE	0xEA66
%define CODE_START	0xF0000
%define ENTRY_POINT	0xFD000
%define CS		0x08
%define DS		0x10
%define LED_PORT	0x10

%define LINEAR_ADDR(Addr)	((CODE_START) + (Addr))

%macro INT_GATE 3
	dw	(%1) & 0xFFFF
	dw	(%2)
	db	0x0
	db	(10001110b | (((%3) & 0x3) << 5))
	dw	(%1) >> 16
%endmacro

%macro IRQ_HANDLER 1
	align 0x8
	irq_handler_%1:
		call led_on
		push %1
		hlt
%endmacro

org 0x8000
startup:
	mov ax, 0xF000
	mov ds, ax
	mov es, ax

	lgdt [gdtr]

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp reload_segs

reload_segs:
	mov ax, DS
	mov ds, ax
	mov fs, ax
	mov gs, ax
	mov es, ax
	mov ss, ax

	dw LJMP_OPCODE
	dd LINEAR_ADDR(startup32)
	dw CS

bits 32

align 0x10
startup32:
	mov esp, 0xFFFF0
	lidt [LINEAR_ADDR(idtr)]
	jmp CS:ENTRY_POINT

led_on:
	in al, LED_PORT
	xor al, 1
	out LED_PORT, al
	ret

%assign i 0
%rep 256
	IRQ_HANDLER i
	%assign i (i + 1)
%endrep

align 0x10
gdt:
	;; NULL
	dd 0
	dd 0

	;; Flat code
	dw 0xFFFF       ; limit low
	dw 0            ; base low
	db 0x00         ; base middle
	db 10011010b    ; access
	db 11001111b    ; granularity / limit high
	db 0x00         ; base high

	;; Flat data
	dw 0xFFFF       ; data descriptor
	dw 0            ; limit low
	db 0            ; base low
	db 10010010b    ; access
	db 11001111b    ; granularity / limit high
	db 0            ; base high

gdtr:
	dw gdtr - gdt - 1
	dd LINEAR_ADDR(gdt)

align 0x10
idt:
	%assign i 0
	%rep 256
		INT_GATE LINEAR_ADDR(irq_handler_0 + 0x8 * i), 0x8, 0x0
		%assign i (i + 1)
	%endrep

idtr:
	dw idtr - idt - 1
	dd LINEAR_ADDR(idt)
