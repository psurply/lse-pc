bits 16
cpu 386

%define FILL_VALUE	0x1337
%define FILL_LEN	8
%define FILL_ADDR	0x0000

org 0x8000
startup:
	call fill_mem
	call read_mem
	hlt

fill_mem:
	mov cx, FILL_LEN
	mov ax, FILL_VALUE
	mov di, FILL_ADDR
	cld
	rep stosw
	ret


read_mem:
	mov cx, FILL_LEN
	mov di, FILL_ADDR
	cld
	rep lodsb
	ret
