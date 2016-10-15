#ifndef OCD_H
#define OCD_H

#include "ocd_prgm.h"

/* break/cont */
#define OCD_REG_CPU_MODE	1
#define OCD_REG_EFLAGS		2
#define OCD_REG_CS		4
#define OCD_REG_EIP		5

/* get_regs */
#define OCD_REG_EDI	0
#define OCD_REG_ESI	2
#define OCD_REG_EBP	4
#define OCD_REG_ESP	6
#define OCD_REG_EBX	8
#define OCD_REG_EDX	10
#define OCD_REG_ECX	12
#define OCD_REG_EAX	14
#define OCD_REG_SET	1
#define OCD_REG_SET_ESP	0xBC90
#define OCD_REG_RVALUE	2

/* IO */
#define OCD_REG_IOADDR	1
#define OCD_REG_IOWR	2
#define OCD_REG_IODATA	3

void ocd_run(const uint8_t *prog, size_t size);
void ocd_busywait(void);
void ocd_set_reg(uint8_t idx, uint16_t value);
void ocd_set_reg32(uint8_t idx, uint32_t value);
uint16_t ocd_get_reg(uint8_t idx);
uint32_t ocd_get_reg32(uint8_t idx);
int ocd_isrunning(void);

#endif /* OCD_H */
