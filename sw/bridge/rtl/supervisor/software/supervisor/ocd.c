#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "altera_avalon_pio_regs.h"
#include "sys/alt_stdio.h"
#include "system.h"

#include "ocd.h"

#define OCD_MEM		0x6000
#define OCD_MEM_SIZE	1024

#define OCD_REGS	((uint16_t *)(0x6200))
#define OCD_REGS_SIZE	(16 * 2)

static void ocd_reset(void)
{
	IOWR_ALTERA_AVALON_PIO_DATA(OCD_CTL_BASE, 1);
}

static void ocd_start(void)
{
	IOWR_ALTERA_AVALON_PIO_DATA(OCD_CTL_BASE, 0);
}

void ocd_set_reg(uint8_t id, uint16_t value)
{
	OCD_REGS[id] = value;
}

void ocd_set_reg32(uint8_t id, uint32_t value)
{
	OCD_REGS[id] = value;
	OCD_REGS[id + 1] = value >> 16;
}

uint16_t ocd_get_reg(uint8_t id)
{
	return OCD_REGS[id];
}

uint32_t ocd_get_reg32(uint8_t id)
{
	return OCD_REGS[id] | (OCD_REGS[id + 1] << 16);
}

void ocd_run(const uint8_t *prog, size_t size)
{
	if (size > OCD_MEM_SIZE)
		return;
	ocd_reset();
	memcpy((void *) OCD_MEM, prog, size);
	ocd_start();
}

int ocd_isrunning(void)
{
	return !IORD_ALTERA_AVALON_PIO_DATA(OCD_STATUS_BASE);
}

void ocd_busywait(void)
{
	while (ocd_isrunning())
		continue;
}
