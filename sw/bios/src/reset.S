#include <reset.h>

.code16

/* CPU reset entry point */
.section .reset, "a"
reset:
	mov $SEGMENT, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov $BASE_SEG_STACK, %sp
	jmp $SEGMENT, $BASE_SEG_CODE
