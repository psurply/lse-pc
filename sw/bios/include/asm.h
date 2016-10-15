#ifndef ASM_H_
# define ASM_H_

/* Align on 16-byte boundary and pad with `nop' instruction */
# define ALIGN	.p2align 4, 0x90

# define GLOBAL(Name)	\
	.globl Name;	\
	name:

# define ENTRY(Name)	\
	ALIGN;		\
	GLOBAL(Name)

#endif /* !ASM_H_ */
