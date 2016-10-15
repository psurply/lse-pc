#include <err.h>
#include <error.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "conf.h"
#include "monitor.h"

#define PROMPT		"[lsepc-monitor] "

struct config config =
{
	.dev = "/dev/ttyUSB0",
	.input_file = NULL,
	.cmd = NULL,
	.interactive = false,
	.setenv = NULL,
	.rootfs = "rootfs.cpio"
};

static struct monitor m;

const struct option long_options[] = {
	{"device", required_argument, 0, 'd'},
	{"command", required_argument, 0, 'c'},
	{"file", required_argument, 0, 'f'},
	{"interactive", no_argument, 0, 'i'},
	{"rootfs", required_argument, 0, 'r'},
	{0, 0, 0, 0},
};

static void usage(const char *exec)
{
	printf("USAGE: %s [-f FILE | -c CMD | -d DEV | -i]\n\n", exec);
	for (const struct option *opt = long_options; opt->name; ++opt)
		printf("  -%c, %s\n", opt->val, opt->name);
	exit(EXIT_FAILURE);
}

static int parse_args(int argc, char *argv[])
{
	int opt_idx;
	int c;

	while ((c = getopt_long(argc, argv, "hd:c:f:ir:",
			long_options, &opt_idx)) >= 0) {
		switch (c) {
			case 'h':
			case '?':
				usage(argv[0]);
				return 1;
			case 'c':
				config.cmd = optarg;
				break;
			case 'f':
				config.input_file = optarg;
				break;
			case 'd':
				config.dev = optarg;
				break;
			case 'i':
				config.interactive = true;
				break;
			case 'r':
				config.rootfs = optarg;
				break;
			default:
				return 1;
		}
	}

	return 0;
}

static void exec_cmd(struct monitor *m, char *cmd)
{
	if (cmd[0]) {
		monitor_exec(m, cmd);
		add_history(cmd);
	}
}

static void interactive(struct monitor *m)
{
	char *cmd;

	while ((cmd = readline(PROMPT))) {
		exec_cmd(m, cmd);
		free(cmd);
	}
	puts("");
}

static void exec_script(struct monitor *m, const char *f)
{
	FILE *stream;
	char *line = NULL;
	size_t len = 0;
	int read;

	if ((stream = fopen(f, "r")) == NULL)
		err(EXIT_FAILURE, "%s", f);
	
	while ((read = getline(&line, &len, stream)) != -1) {
		line[read - 1] = 0;
		exec_cmd(m, line);
	}

	free(line);
	fclose(stream);
}

int main(int argc, char *argv[])
{

	if (parse_args(argc, argv))
		return EXIT_FAILURE;

	config.setenv = fopen("/tmp/lsepc_setenv.sh", "w+");

	if (monitor_init(&m, config.dev))
		errx(EXIT_FAILURE, "Cannot initialize monitor (%s)", config.dev);

	if (config.input_file)
		exec_script(&m, config.input_file);

	if (config.cmd)
		exec_cmd(&m, config.cmd);

	if (config.interactive)
		interactive(&m);

	if (config.cmd == NULL
		&& config.interactive == false
		&& config.input_file == NULL)
		usage(argv[0]);

	monitor_delete(&m);

	return EXIT_SUCCESS;
}
