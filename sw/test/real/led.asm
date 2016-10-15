bits 16
cpu 386

%define LED_PORT	0x10

org 0x8000
startup:
	in al, LED_PORT
	xor al, 1
	out LED_PORT, al
	hlt
