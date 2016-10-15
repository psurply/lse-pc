#include <bios.h>
#include <command.h>
#include <uart.h>
#include <shell.h>
#include <string.h>

#define BUF_SIZE	1024

#define PROMPT	"$ "

#define BACKSPACE	'\b'
#define BLANK		' '
#define DEL		127
#define ENTER		'\r'
#define CTRL_C		3

static int exec_cmd(u8 *buffer)
{
	const char *name = strtok_c(buffer, ' ');
	const char *argv[COMMAND_MAX_ARGS + 1] = { name, NULL };
	const struct command *cmd = get_command(name);

	if (cmd != NULL) {
		u8 nb_args = 0;
		char *arg = NULL;

		do {
			arg = strtok_c(NULL, ' ');
			argv[++nb_args] = arg;
		} while (nb_args < COMMAND_MAX_ARGS && arg != NULL);

		if (nb_args != 1 && nb_args - 1 < cmd->min_args) {
			uart1_write(argv[0]);
			uart1_write_newline(": not enough arguments");
			uart1_write_newline(cmd->help);
		} else if (nb_args - 1> cmd->max_args) {
			uart1_write(argv[0]);
			uart1_write_newline(": too much arguments");
			uart1_write_newline(cmd->help);
		} else {
			argv[nb_args] = NULL;
			cmd->callback(cmd, nb_args, argv);
		}
	} else {
		uart1_write(buffer);
		uart1_write_newline(": command not found");
	}

	uart1_write_newline("");
}

static int ack_cmd(u8 *buffer, u16 *pos)
{
	u8 byte = buffer[*pos];

	if (byte == ENTER) {
		buffer[*pos] = '\0';
		uart1_write_newline("");

		if (*pos != 0)
			exec_cmd(buffer);

		uart1_write(PROMPT);
		*pos = 0;
	} else if (byte == DEL || byte == BACKSPACE) {
		if (*pos > 0) {
			uart1_writeb(BACKSPACE);
			uart1_writeb(BLANK);
			uart1_writeb(BACKSPACE);

			--*pos;
		}
	}
	else if (byte == CTRL_C) {
		*pos = 0;
		uart1_write_newline("");
		uart1_write(PROMPT);
	} else if (is_printable(byte)) {
		uart1_writeb(byte);
		++*pos;
	}

	return 0;
}

void shell_run(void)
{
	uart1_write(PROMPT);

	u8 buffer[BUF_SIZE] = { 0 };
	u16 pos = 0;

	do {
		u8 byte = uart1_readb();
		buffer[pos] = byte;
	} while (!ack_cmd(buffer, &pos));
}
