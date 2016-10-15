#ifndef COMMAND_H_
# define COMMAND_H_

# include <types.h>

# define COMMAND_MAX_ARGS	32

struct command {
	const char *name;
	const char *help;
	u8 min_args;
	u8 max_args;

	int (*callback)(const struct command *cmd, int argc,
			const char *argv[]);
};

const struct command *get_command(const char *name);

int help(const struct command *cmd, int argc, const char *argv[]);
int memory(const struct command *cmd, int argc, const char *argv[]);

#endif /* !COMMAND_H_ */
