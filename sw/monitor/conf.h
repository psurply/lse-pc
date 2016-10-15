#ifndef CONF_H
#define CONF_H

#include <stdbool.h>

struct config
{
	char *dev;
	char *input_file;
	char *cmd;
	bool interactive;
	FILE *setenv;
	char *rootfs;
};

extern struct config config;

#endif /* CONF_H */
