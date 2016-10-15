#ifndef BIOS_H_
#define BIOS_H_

#define NULL	((void*)0)

#define TOSTRING(X)	# X

#define __attribute(...)	__attribute__((__VA_ARGS__))
#define __section(Name)	__attribute(section(Name))
#define __packed		__attribute(packed)

#define CODE16	__asm__(".code16")
#define CODE32	__asm__(".code32")

#define SECTION(Name)	__asm__(".section " Name)

typedef __builtin_va_list va_list;
#define va_start(ap, param)	__builtin_va_start(ap, param)
#define va_end(ap)		__builtin_va_end(ap)
#define va_arg(ap, type)	__builtin_va_arg(ap, type)

#define MIN(A, B)	((A) > (B) ? (B) : (A))

#define PANIC_INT 31
#define panic(msg) \
	__asm__ volatile ("push %0\n\t" \
			"push %0\n\t" \
			"push %0\n\t" \
			"int $31" \
			:: "r" (msg));

void main(void);

#endif /* !BIOS_H_ */
