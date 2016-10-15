#ifndef MEMORY_H_
# define MEMORY_H_

/* Linear base address of the stack */
# define BASE_STACK	((1 << 20) - 16)

/* Linear base address of the code */
# define BASE_CODE	((1 << 20) - (32 << 10))

#define PD_ADDR		0x00010000
#define PT_ADDR		0x00011000
#define PAGE_SIZE	4096
#define PAGE_OFFSET	12

#define IDT_ADDR	0xC0012000

#define EXTERNAL_RAM	0x00100000

#define BIOS_BASE	0xC0000000

#endif /* !MEMORY_H_ */
