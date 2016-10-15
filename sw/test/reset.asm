bits 16
cpu 386

org 0xFFF0
reset:
	mov ax, 0xF000
	mov ds, ax
	mov ss, ax
	mov sp, 0xFFF0
	jmp 0xF000:0x8000
