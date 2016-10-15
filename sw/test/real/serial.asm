bits 16
cpu 386

%define COM1_PORT	0x3F8

%define THR	0
%define RHR	0
%define IER	1
%define DLL	0
%define DLH	1
%define IIR	2
%define FCR	2
%define LCR	3
%define MCR	4
%define LSR	5

%define RHR_READY	1
%define THR_READY	(1 << 5)

%define LED_PORT	0x10

org 0x8000
startup:
	call readb
	mov cl, al
	call writeb
	jmp startup

readb:
	; Check if the receiver is ready
	mov dx, COM1_PORT + LSR
	in al, dx
	and al, RHR_READY
	test al, al
	jz readb

	; Read the character
	mov dx, COM1_PORT + RHR
	in al, dx
	ret

writeb:
	; Check if the transmitter is ready
	mov dx, COM1_PORT + LSR
	in al, dx
	and al, THR_READY
	test al, al
	jz writeb

	; Write the character
	mov dx, COM1_PORT + THR
	mov al, cl
	out dx, al

	in al, LED_PORT
	xor al, 1
	out LED_PORT, al

	ret
