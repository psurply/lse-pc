#include <stdlib.h>
#include <stdint.h>

#include "altera_avalon_pio_regs.h"
#include "sys/alt_stdio.h"
#include "system.h"

#include "dev.h"
#include "ocd.h"
#include "supervisor.h"

#define ESP_RUN(X)	((X) + 12)
#define ESP_BRK(X)	((X) - 12)

static void sv_set_status(struct sv *sv, int st)
{
	sv->status = st;
	IOWR_ALTERA_AVALON_PIO_DATA(STATUS_BASE, sv->status);
}

void sv_init(struct sv *sv)
{
	sv->dev = NULL;
	sv->ioaddr = 0;
	sv->iodata = 0;
	dev_register(&sv->dev, &dev_led, NULL);
	ocd_run(ocd_prgm_reset, sizeof (ocd_prgm_reset));
	ocd_busywait();
	sv_set_status(sv, ST_STOP);
}

int sv_stop(struct sv *sv)
{
	ocd_run(ocd_prgm_reset, sizeof (ocd_prgm_reset));
	ocd_busywait();
	sv_set_status(sv, ST_STOP);
	return 0;
}

int sv_brk(struct sv *sv)
{
	if (sv->status == ST_RUNNING) {
		ocd_run(ocd_prgm_break, sizeof (ocd_prgm_break));

		ocd_busywait();
		sv->regs.eflags = ocd_get_reg32(OCD_REG_EFLAGS);
		sv->regs.cs = ocd_get_reg(OCD_REG_CS);
		sv->regs.eip = ocd_get_reg32(OCD_REG_EIP);

		if (ocd_get_reg(OCD_REG_CPU_MODE))
			sv_set_status(sv, ST_BRK32);
		else
			sv_set_status(sv, ST_BRK16);

		return 0;
	}
	return 1;
}

int sv_iobreak(struct sv *sv)
{
	if (sv->status == ST_RUNNING) {
		sv->ioaddr = ocd_get_reg(OCD_REG_IOADDR);
		uint16_t iowr = ocd_get_reg(OCD_REG_IOWR);

		if (iowr) {
			ocd_run(ocd_prgm_read_io, sizeof (ocd_prgm_read_io));
			ocd_busywait();
			sv->iodata = ocd_get_reg(OCD_REG_IODATA);
			sv_set_status(sv, ST_IOWR);
			if (dev_write(sv->dev, sv->ioaddr, sv->iodata) == 0)
				sv_cont(sv);
		} else {
			sv_set_status(sv, ST_IORD);
			if (dev_read(sv->dev, sv->ioaddr, &sv->iodata) == 0)
				sv_cont(sv);
		}

		return 0;
	}
	return 1;
}

int sv_cont(struct sv *sv)
{
	if (sv->status == ST_BRK32) {
		ocd_set_reg32(OCD_REG_EFLAGS, sv->regs.eflags);
		ocd_set_reg(OCD_REG_CS, sv->regs.cs);
		ocd_set_reg32(OCD_REG_EIP, sv->regs.eip);

		ocd_run(ocd_prgm_cont, sizeof (ocd_prgm_cont));
		sv_set_status(sv, ST_RUNNING);
		return 0;
	} else if (sv->status == ST_IORD) {
		ocd_set_reg(OCD_REG_IODATA, sv->iodata);
		ocd_run(ocd_prgm_write_io, sizeof (ocd_prgm_write_io));
		sv_set_status(sv, ST_RUNNING);
		return 0;
	} else if (sv->status == ST_IOWR) {
		ocd_run(ocd_prgm_start, sizeof (ocd_prgm_start));
		sv_set_status(sv, ST_RUNNING);
		return 0;
	}
	return 1;
}

int sv_start(struct sv *sv)
{
	if (sv->status == ST_STOP) {
		ocd_run(ocd_prgm_start, sizeof (ocd_prgm_start));
		sv_set_status(sv, ST_RUNNING);
		return 0;
	}
	return 1;
}

int sv_get_regs(struct sv *sv)
{
	if (sv->status == ST_BRK32) {
		ocd_run(ocd_prgm_get_regs, sizeof (ocd_prgm_get_regs));

		ocd_busywait();
		sv->regs.edi = ocd_get_reg32(OCD_REG_EDI);
		sv->regs.esi = ocd_get_reg32(OCD_REG_ESI);
		sv->regs.ebp = ocd_get_reg32(OCD_REG_EBP);
		sv->regs.esp = ESP_RUN(ocd_get_reg32(OCD_REG_ESP));
		sv->regs.ebx = ocd_get_reg32(OCD_REG_EBX);
		sv->regs.edx = ocd_get_reg32(OCD_REG_EDX);
		sv->regs.ecx = ocd_get_reg32(OCD_REG_ECX);
		sv->regs.eax = ocd_get_reg32(OCD_REG_EAX);

		// Restore ESP
		ocd_set_reg(OCD_REG_SET, OCD_REG_SET_ESP);
		ocd_set_reg32(OCD_REG_RVALUE, ESP_BRK(sv->regs.esp));
		ocd_run(ocd_prgm_set_reg, sizeof (ocd_prgm_set_reg));
		ocd_busywait();

		return 0;
	}
	return 1;
}
