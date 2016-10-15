bits 16
cpu 386

%define LED_PORT	0x10

org 0x8000
startup:
	mov al, 'X'
	mov ah, 'A'
	mov bl, 'X'
	mov bh, 'B'
	mov cl, 'X'
	mov ch, 'C'
	mov dl, 'X'
	mov dh, 'D'

	pusha
	pushf
	popf
	popa

	hlt
