#include <err.h>
#include <error.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "monitor.h"

#include "conf.h"
#include "dev/dev.h"

#define DELIM		" "
#define CMD_SIZE	128

static const char *str_status[] = {
	"STOP",
	"RUN",
	"BREAK (Real mode)",
	"BREAK (Protected mode)",
	"IO WRITE",
	"IO READ"
};

static int cmd_help(struct monitor *m);
static int cmd_list(struct monitor *m);

static int cmd_echo(struct monitor *m)
{
	(void) m;
	char *s;

	while ((s = strtok(NULL, DELIM)))
		printf("%s ", s);
	putchar('\n');

	return 0;
}

static int cmd_info(struct monitor *m)
{
	char version[32];
	int ret;

	if ((ret = lsepc_get_version(m->lsepc, version, sizeof(version))) < 0)
		return ret;

	version[sizeof (version) - 1] = 0;

	printf("Version: %s\n", version);
	return 0;
}

static int cmd_start(struct monitor *m)
{
	return lsepc_start(m->lsepc);
}

static int cmd_stop(struct monitor *m)
{
	return lsepc_stop(m->lsepc);
}

static int cmd_brk(struct monitor *m)
{
	return lsepc_brk(m->lsepc);
}

static int cmd_cont(struct monitor *m)
{
	return lsepc_cont(m->lsepc);
}

static int cmd_status(struct monitor *m)
{
	int status = lsepc_get_status(m->lsepc);

	if (status < 0)
		return status;

	if ((size_t) status > (sizeof(str_status) / sizeof(str_status[0])) - 1)
	{
		printf("Invalid CPU status: %d\n", status);
		return -EBADMSG;
	}

	printf("CPU Status: %s\n", str_status[status]);
	return 0;
}

static int cmd_getregs(struct monitor *m)
{
	struct lsepc_regs regs;
	int r;

	if ((r = lsepc_get_regs(m->lsepc, &regs)))
		return r;

	printf("EFLAGS:\t%08x\n", regs.eflags);
	printf("EIP:\t%08x\n", regs.eip);
	printf("ESP:\t%08x\n", regs.esp);
	printf("EBP:\t%08x\n", regs.ebp);
	printf("EAX:\t%08x\n", regs.eax);
	printf("EBX:\t%08x\n", regs.ebx);
	printf("ECX:\t%08x\n", regs.ecx);
	printf("EDX:\t%08x\n", regs.edx);
	printf("ESI:\t%08x\n", regs.esi);
	printf("EDI:\t%08x\n", regs.edi);
	printf("CS:\t%04x\n", regs.cs);
	return 0;
}

static int cmd_sysemu(struct monitor *m)
{
	int status;
	uint16_t ioaddr;
	uint16_t iodata;

	dev_register(&m->dev, &com1_dev, NULL);
	dev_register(&m->dev, &com2_dev, config.rootfs);

	printf("Emulating full system\n");
	monitor_exec(m, "info");
	monitor_exec(m, "stop");
	monitor_exec(m, "start");

	while ((status = lsepc_get_status(m->lsepc)) != LSEPC_ST_STOP) {
		switch (status) {
		case LSEPC_ST_RUN:
			break;
		case LSEPC_ST_IORD:
			ioaddr = lsepc_get_io_addr(m->lsepc);
			if (dev_read(m->dev, ioaddr, &iodata))
				printf("[SYS] IO READ(%x)\n", ioaddr);
			lsepc_set_io_data(m->lsepc, iodata);
			lsepc_cont(m->lsepc);
			break;
		case LSEPC_ST_IOWR:
			ioaddr = lsepc_get_io_addr(m->lsepc);
			iodata = lsepc_get_io_data(m->lsepc);
			if (dev_write(m->dev, ioaddr, iodata))
				printf("[SYS] IO WRITE(%x, %x)\n",
					ioaddr, iodata);
			lsepc_cont(m->lsepc);
			break;
		default:
			printf("Invalid CPU status: %d\n", status);
			return -EBADMSG;
		}
	}

	return 0;
}

struct monitor_cmd
{
	const char *name;
	const char *usage;
	const char *help;
	int (*cmd)(struct monitor *m);
};

struct monitor_cmd monitor_cmds[] =
{
	{
		"help",
		"[CMD]",
		"Show help",
		cmd_help
	},
	{
		"list",
		"",
		"List commands",
		cmd_list
	},
	{
		"echo",
		"[STRING]",
		"Print STRING",
		cmd_echo
	},
	{
		"info",
		"",
		"Print LSE-PC information",
		cmd_info
	},
	{
		"start",
		"",
		"Start LSE-PC CPU",
		cmd_start
	},
	{
		"stop",
		"",
		"Stop LSE-PC CPU",
		cmd_stop
	},
	{
		"break",
		"",
		"Break LSE-PC CPU execution",
		cmd_brk
	},
	{
		"continue",
		"",
		"Continue LSE-PC CPU execution",
		cmd_cont
	},
	{
		"status",
		"",
		"Print LSE-PC CPU status",
		cmd_status
	},
	{
		"getregs",
		"",
		"Print LSE-PC CPU registers",
		cmd_getregs
	},
	{
		"sysemu",
		"",
		"Emulate full system",
		cmd_sysemu
	},
	{
		NULL,
		NULL,
		NULL,
		NULL
	}
};

static int cmd_list(struct monitor *m)
{
	(void) m;
	for (size_t i = 0; monitor_cmds[i].name; ++i)
		printf("%s\t%s\n", monitor_cmds[i].name, monitor_cmds[i].help);

	return 0;
}

static int cmd_help(struct monitor *m)
{
	(void) m;
	int no_args = 1;
	int cmd_found = 0;
	char *s;

	while ((s = strtok(NULL, DELIM)))
	{
		no_args = 0;
		cmd_found = 0;
		for (size_t i = 0; monitor_cmds[i].name; ++i) {
			if (!strcmp(monitor_cmds[i].name, s)) {
				cmd_found = 1;
				printf("USAGE: %s %s\n",
					s, monitor_cmds[i].usage);
				printf("DESCR: %s\n\n", monitor_cmds[i].help);
				break;
			}
		}

		if (!cmd_found) {
			warnx("%s: command not found", s);
			return 1;
		}
	}

	if (no_args)
		cmd_list(m);

	return 0;
}

int monitor_init(struct monitor *m, const char *dev)
{
	m->lsepc = lsepc_init(dev);
	if (!m->lsepc)
		return 1;
	m->dev = NULL;

	return 0;
}

void monitor_exec(struct monitor *m, const char *cmd)
{
	char ccmd[CMD_SIZE];
	char *cmd_name;
	int ret;

	strncpy(ccmd, cmd, CMD_SIZE);
	cmd_name = strtok(ccmd, DELIM);

	if (cmd_name[0] == '#')
		return;

	for (size_t i = 0; monitor_cmds[i].name; ++i) {
		if (!strcmp(monitor_cmds[i].name, cmd_name)) {
			if ((ret = monitor_cmds[i].cmd(m)))
				error(0, -ret, "%s failed", cmd_name);
			return;
		}
	}
	warnx("%s: command not found", cmd_name);
}

void monitor_delete(struct monitor *m)
{
	lsepc_delete(m->lsepc);
}
