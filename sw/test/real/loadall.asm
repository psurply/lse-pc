bits 16
cpu 386

%define LED_PORT	0x10
%define loadall		dw 0x070F

org 0x8000
startup:
	mov ax, 0xF000
	mov ds, ax
	mov es, ax
	mov di, loadall_struc
	loadall
	hlt

success:
	pusha
	in al, LED_PORT
	xor al, 1
	out LED_PORT, al
	hlt

loadall_struc:
	dd 0            ;; CR0
	dd 2            ;; EFLAGS
	dd success      ;; EIP
	dd 66666666h    ;; EDI
	dd 77777777h    ;; ESI
	dd 55555555h    ;; EBP
	dd 88888888h    ;; ESP
	dd 22222222h    ;; EBX
	dd 44444444h    ;; EDX
	dd 33333333h    ;; ECX
	dd 11111111h    ;; EAX
	dd 0            ;; DR6
	dd 0            ;; DR7
	dd 0            ;; TR
	dd 0            ;; LDT
	dd 5555h        ;; GS
	dd 4444h        ;; FS
	dd 2222h        ;; DS
	dd 6666h        ;; SS
	dd 1111h        ;; CS
	dd 3333h        ;; ES
	dd 00008900h,00070000h,00000800h    ;; TSS Desc Cache
	dd 00000000h,00000000h,000003ffh    ;; IDT Desc Cache
	dd 00000000h,00000000h,00000000h    ;; GDT Desc Cache
	dd 00008200h,00090000h,00000088h    ;; LDT Desc Cache
	dd 00009300h,000f0000h,0000ffffh    ;; GS Desc Cache
	dd 00009300h,000f0000h,0000ffffh    ;; FS Desc Cache
	dd 00009300h,000f0000h,0000ffffh    ;; DS Desc Cache
	dd 00009300h,000f0000h,0000ffffh    ;; SS Desc Cache
	dd 00009b00h,000f0000h,0000ffffh    ;; CS Desc Cache
	dd 00009300h,000f0000h,0000ffffh    ;; ES Desc Cache
