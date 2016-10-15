bits 32
cpu 386

%define LED_PORT	0x10

org 0xFD000
entry_point:
	mov eax, 0x33221100
	mov ebx, 0x77665544
	mov ecx, 0xBBAA9988
	mov edx, 0xFFEEDDCC
	mov esi, 0x01234567
	mov edi, 0x89ABCDEF

led:
	in al, LED_PORT
	xor al, 1
	out LED_PORT, al

	jmp led

	hlt
