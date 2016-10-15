#include <bios.h>
#include <frame_alloc.h>
#include <fs.h>
#include <command.h>
#include <stdio.h>
#include <string.h>
#include <uart.h>

static int ls(const struct command *cmd, int argc, const char *argv[])
{
	const char *path = NULL;

	if (argc > 1)
		path = argv[1];

	fs_list_dir(path);
}

static int print_file(const struct command *cmd, int argc, const char *argv[])
{
	if (argc == 1)
		return 1;

	fs_print_file(argv[1]);
	return 0;
}

static int recvfs(const struct command *cmd, int argc, const char *argv[])
{
	fs_recv_cpio();
	return 0;
}

static int stats(const struct command *cmd, int argc, const char *argv[])
{
	frame_stats();
	fs_stats();
	return 0;
}

const static struct command commands[] = {
	{
		.name = "help",
		.min_args = 0,
		.max_args = 1,
		.callback = help,
		.help = "usage: help [command]\r\n"
			"\r\nDisplay the help of the given command\r\n"
	},
	{
		.name = "memory",
		.min_args = 2,
		.max_args = 2,
		.callback = memory,
		.help = "usage: memory address length\r\n"
			"\r\nDump the content of the memory\r\n"
			"\r\npositional arguments:\r\n"
			"\taddress\tstart address\r\n"
			"\tlength\tbyte length\r\n"
	},
	{
		.name = "ls",
		.min_args = 0,
		.max_args = 1,
		.callback = ls,
		.help = "usage: ls [dir]\r\n"
			"\r\nList directory\r\n"
	},
	{
		.name = "p",
		.min_args = 1,
		.max_args = 1,
		.callback = print_file,
		.help = "usage: p [file]\r\n"
			"\r\nPrint file content\r\n"
	},
	{
		.name = "recvfs",
		.min_args = 0,
		.max_args = 0,
		.callback = recvfs,
		.help = "usage: recv\r\n"
			"\r\nReceive CPIO archive on COM2\r\n"
	},
	{
		.name = "stats",
		.min_args = 0,
		.max_args = 0,
		.callback = stats,
		.help = "usage: stats\r\n"
			"\r\nPrint memory usage stats\r\n"
	},

	{ NULL, NULL, 0, 0, NULL }
};

const struct command *get_command(const char *name)
{
	const struct command *cmd = NULL;

	for (int i = 0; commands[i].name; ++i) {
		if (!strcmp(commands[i].name, name)) {
			cmd = commands + i;
			break;
		}
	}

	return cmd;
}

int help(const struct command *cmd, int argc, const char *argv[])
{
	if (argc == 1) {
		printf("%s\nCommands:\n", cmd->help);
		for (unsigned int i = 0; commands[i].name; ++i)
			printf("\t\t%s\n", commands[i].name);
	} else {
		int exist = 0;

		for (int i = 0; commands[i].name; ++i) {
			if (!strcmp(commands[i].name, argv[1])) {
				uart1_write(commands[i].help);

				exist = 1;
				break;
			}
		}

		if (!exist) {
			uart1_write(cmd->name);
			uart1_write(": command `");
			uart1_write(argv[1]);
			uart1_write_newline("' does not exist");
		}
	}

	return 0;
}

int memory(const struct command *cmd, int argc, const char *argv[])
{
	if (argc == 1) {
		uart1_write(cmd->help);
	} else {
		int err = 0;

		u32 address;
		if (atoi(argv[1], &address)) {
			uart1_write(cmd->name);
			uart1_write_newline(": `address' must be an integer");
			err = 1;
		}

		u32 length;
		if (atoi(argv[2], &length)) {
			uart1_write(cmd->name);
			uart1_write_newline(": `length' must be an integer");
			err = 1;
		}

		if (err)
			return 1;

		u8 *addr = (u8 *)address;
		for (u32 i = 0; i < length; ++i) {
			char buffer[3] = { 0 };
			itoa(addr[i], 16, buffer);

			if (buffer[1] == '\0')
			{
				buffer[1] = buffer[0];
				buffer[0] = '0';
				buffer[2] = '\0';
			}

			uart1_write(buffer);
			uart1_writeb(' ');
		}

		uart1_write_newline("");
	}

	return 0;
}
