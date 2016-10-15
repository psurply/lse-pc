#include <stdio.h>
#include <stdint.h>

#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_stdio.h"
#include "system.h"

#include "proto.h"

#define PROTO_CMD_SIZE		32

#define CMD_GETVERSION		1
#define CMD_ECHO		2
#define CMD_START		3
#define CMD_STOP		4
#define CMD_BRK			5
#define CMD_CONT		6
#define CMD_GETSTATUS		7
#define CMD_GETREGS		8
#define CMD_SETIODATA		9
#define CMD_GETIODATA		10
#define CMD_GETIOADDR		11

#define PROTO_SOH		0x01
#define PROTO_STX		0x02
#define PROTO_ETX		0x03
#define PROTO_EOT		0x04
#define PROTO_ENQ		0x05
#define PROTO_ACK		0x06
#define PROTO_NAK		0x15
#define PROTO_SYN		0x16

#define PROTO_VERSION		"LSE-PC Supervisor v0.1"

static inline void proto_nack(void)
{
	alt_putchar(PROTO_NAK);
}

static inline void proto_ack(const void *buff, uint8_t size)
{
	uint8_t i;
        const char *b = buff;

	alt_putchar(PROTO_ACK);
	alt_putchar((int) size);
	alt_putchar(PROTO_STX);
	for (i = 0; i < size; ++i)
		alt_putchar((int) b[i]);
	alt_putchar(PROTO_EOT);
}

static inline void proto_syn(void)
{
	alt_putchar(PROTO_SYN);
	alt_putchar(PROTO_ACK);
}

static int cb_get_version(struct sv *sv, uint8_t *args)
{
	proto_ack((uint8_t *) PROTO_VERSION, sizeof (PROTO_VERSION));
	return 0;
}

static int cb_echo(struct sv *sv, uint8_t *args)
{
	proto_ack(args, 1);
	return 0;
}

static int cb_stop(struct sv *sv, uint8_t *args)
{
	uint8_t ret = sv_stop(sv);
	proto_ack(&ret, 1);
	return 0;
}

static int cb_start(struct sv *sv, uint8_t *args)
{
	uint8_t ret = sv_start(sv);
	proto_ack(&ret, 1);
	return 0;
}

static int cb_brk(struct sv *sv, uint8_t *args)
{
	uint8_t ret = sv_brk(sv);
	proto_ack(&ret, 1);
	return 0;
}

static int cb_cont(struct sv *sv, uint8_t *args)
{
	uint8_t ret = sv_cont(sv);
	proto_ack(&ret, 1);
	return 0;
}

static int cb_get_status(struct sv *sv, uint8_t *args)
{
	proto_ack(&sv->status, 1);
	return 0;
}

static int cb_get_regs(struct sv *sv, uint8_t *args)
{
	sv->regs.valid = sv_get_regs(sv);
	proto_ack(&sv->regs, sizeof(struct sv_regs));
	return 0;
}

static int cb_set_io_data(struct sv *sv, uint8_t *args)
{
	sv->iodata = args[0] | (args[1] << 8);
	proto_ack(&sv->iodata, 2);
	return 0;
}

static int cb_get_io_data(struct sv *sv, uint8_t *args)
{
	proto_ack(&sv->iodata, 2);
	return 0;
}

static int cb_get_io_addr(struct sv *sv, uint8_t *args)
{
	proto_ack(&sv->ioaddr, 2);
	return 0;
}

struct proto_cmd
{
	uint8_t cmd;
	uint8_t arg_size;
	int (*callback)(struct sv *sv, uint8_t *args);
};

static struct proto_cmd proto_cmds[] =
{
	{CMD_GETVERSION, 0, cb_get_version},
	{CMD_ECHO, 1, cb_echo},
	{CMD_START, 0, cb_start},
	{CMD_STOP, 0, cb_stop},
	{CMD_BRK, 0, cb_brk},
	{CMD_CONT, 0, cb_cont},
	{CMD_GETSTATUS, 0, cb_get_status},
	{CMD_GETREGS, 0, cb_get_regs},
	{CMD_SETIODATA, 2, cb_set_io_data},
	{CMD_GETIODATA, 0, cb_get_io_data},
	{CMD_GETIOADDR, 0, cb_get_io_addr},
	{0, 0, NULL},
};

static int proto_recv(struct sv *sv)
{
	struct proto_cmd *pcmd;
	uint8_t args[PROTO_CMD_SIZE];
	uint8_t cmd;
	uint8_t i;

	if (alt_getchar() != PROTO_SOH)
		return 1;

	cmd = alt_getchar();

	if (alt_getchar() != PROTO_STX)
		return 1;

	for (pcmd = proto_cmds; pcmd->cmd && pcmd->cmd != cmd; ++pcmd)
		continue;

	if (!pcmd->callback)
		return 1;

	for (i = 0; i < pcmd->arg_size && i < PROTO_CMD_SIZE; ++i)
		args[i] = alt_getchar();

	if (alt_getchar() != PROTO_EOT)
		return 1;

	return pcmd->callback(sv, args);
}

void proto_handle(struct sv *sv)
{
	switch (alt_getchar()) {
		case PROTO_ENQ:
			if (proto_recv(sv))
				proto_nack();
			break;
		case PROTO_SYN:
			proto_syn();
			break;
		default:
			proto_nack();
	}
}
