#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#include "altera_avalon_pio_regs.h"
#include "altera_avalon_uart_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_stdio.h"
#include "system.h"

#include "ocd.h"
#include "proto.h"
#include "supervisor.h"

void led_init(struct sv *sv);

int main(void)
{
	int status;

	struct sv sv;

	sv_init(&sv);

	for (;;) {
		status = IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE);
		if (status & ALTERA_AVALON_UART_CONTROL_RRDY_MSK)
			proto_handle(&sv);
		if (!ocd_isrunning())
			sv_iobreak(&sv);
	}

	return 0;
}
