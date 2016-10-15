bits 32
cpu 386

org 0xFD000
entry_point:
    push   edi
    lea    edi,[esp+0x8]
    and    esp,0xfffffff0
    push   DWORD PTR [edi-0x4]
    push   ebp
    mov    ebp,esp
    push   edi
    sub    esp,0x14
    mov    DWORD PTR [ebp-0xc],0xf8458
    mov    eax,DWORD PTR [ebp-0xc]
    mov    al,BYTE PTR [eax]
    movzx  eax,al
    mov    edx,eax
    mov    eax,0x3f8
    call   outb
    jmp    main+0x19

outb:
    push   ebp
    mov    ebp,esp
    sub    esp,0x8
    mov    WORD PTR [ebp-0x4],ax
    mov    BYTE PTR [ebp-0x8],dl
    mov    al,BYTE PTR [ebp-0x8]
    mov    edx,DWORD PTR [ebp-0x4]
    out    dx,al
    nop
    leave
    ret
