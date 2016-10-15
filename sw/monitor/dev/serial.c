#include <err.h>
#include <fcntl.h>
#include <pty.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "../conf.h"
#include "dev.h"

#define RBR 0
#define THR 0
#define IER 1
#define IIR 2
#define LCR 3
#define MCR 4
#define LSR 5
#define MSR 6
#define SCR 7

#define THR_READY	(1 << 5)
#define DATA_AVL	(1 << 0)

#define QUEUE_SIZE 512

struct serial_data {
	size_t queue_size;
	size_t cursor;
	uint8_t queue[QUEUE_SIZE];
	int input_fd;
	int output_fd;
};

static struct serial_data com1_data = {
	.queue_size = 0,
	.cursor = 0
};

static struct serial_data com2_data = {
	.queue_size = 0,
	.cursor = 0
};

static uint16_t serial_read(struct dev *dev, uint16_t offset)
{
	struct serial_data *data = dev->data;
	uint16_t r;
	int ret;

	if (data->queue_size == 0) {
		if ((ret = read(data->input_fd, data->queue, QUEUE_SIZE)) > 0) {
			data->queue_size = ret;
			data->cursor = 0;
		}
	}

	switch (offset) {
	case RBR:
		r = 0;
		if (data->queue_size) {
			r = data->queue[data->cursor++];
			data->queue_size--;
		}
		return r;
	case LSR:
		return THR_READY | (data->queue_size > 0 ? DATA_AVL : 0);
	default:
		printf("[SERIAL] Invalid offset (Read): %x\n", offset);
		break;
	}
	return 0;
}

static void serial_write(struct dev *dev, uint16_t offset, uint16_t value)
{
	struct serial_data *data = dev->data;

	switch (offset) {
	case THR:
		write(data->output_fd, &value, 1);
		break;
	default:
		printf("[SERIAL] Invalid offset (Write): %x", offset);
		break;
	}
}

static void serial_init_pts(struct dev *dev, void *data)
{
	(void) data;
	(void) dev;

	int amaster;
	int aslave;
	char name[128];
	int flags;
	struct serial_data *serial_data = dev->data;

	if (openpty(&amaster, &aslave, name, NULL, NULL) < 0)
		err(EXIT_FAILURE, "serial_init");

	flags = fcntl(amaster, F_GETFL, 0);
	fcntl(amaster, F_SETFL, flags | O_NONBLOCK);
	serial_data->input_fd = amaster;
	serial_data->output_fd = amaster;

	printf("[SERIAL] %s bound to %s\n", dev->name, name);
	fprintf(config.setenv, "LSEPC_%s=%s\n", dev->name, name);
	fflush(config.setenv);
}


static void serial_init_file(struct dev *dev, void *data)
{
	(void) dev;

	int fd;
	int flags;
	struct serial_data *serial_data = dev->data;

	if ((fd = open((char *) data, O_RDONLY)) < 0)
		err(EXIT_FAILURE, data);

	flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	serial_data->input_fd = fd;
	serial_data->output_fd = STDOUT_FILENO;

	printf("[SERIAL] %s bound to file %s\n", dev->name, (char *) data);
}


struct dev com1_dev = {
	.name = "COM1",
	.ioaddr = 0x3f8,
	.size = 8,
	.init = serial_init_pts,
	.read = serial_read,
	.write = serial_write,
	.data = &com1_data
};

struct dev com2_dev = {
	.name = "COM2",
	.ioaddr = 0x2f8,
	.size = 8,
	.init = serial_init_file,
	.read = serial_read,
	.write = serial_write,
	.data = &com2_data
};
