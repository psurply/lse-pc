#ifndef IDT_H_
# define IDT_H_

#include <regs.h>

#define X(n, ec, str) \
void isr##n(void);
#include <idt.def>
#undef X

void setup_minimal_idt(void);
void exception_handler(struct intr_regs *regs);

#endif /* !IDT_H_ */
