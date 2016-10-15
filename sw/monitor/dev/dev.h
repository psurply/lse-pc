#ifndef DEV_H
#define DEV_H

#include <stdint.h>

struct dev {
	struct dev *next;

	const char *name;

	uint16_t ioaddr;
	uint16_t size;

	void (*init)(struct dev *d, void *data);
	uint16_t (*read)(struct dev *d, uint16_t offset);
	void (*write)(struct dev *d, uint16_t offset, uint16_t value);

	void *data;
};

int dev_read(struct dev *d, uint16_t ioaddr, uint16_t *iodata);
int dev_write(struct dev *d, uint16_t ioaddr, uint16_t iodata);
void dev_register(struct dev **devices, struct dev *d, void *data);

extern struct dev com1_dev;
extern struct dev com2_dev;

#endif /* DEV_H */
