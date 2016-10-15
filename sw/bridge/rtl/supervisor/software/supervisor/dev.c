#include <stdlib.h>
#include "dev.h"

static struct dev *find_dev(struct dev *d, uint16_t ioaddr)
{
	for (; d != NULL &&
		!(d->ioaddr <= ioaddr && ioaddr < d->ioaddr + d->size);
		d = d->next)
		continue;

	return d;
}

int dev_read(struct dev *d, uint16_t ioaddr, uint16_t *iodata)
{
	d = find_dev(d, ioaddr);
	if (d == NULL)
		return 1;
	*iodata = d->read(d, d->ioaddr - ioaddr);
	return 0;
}

int dev_write(struct dev *d, uint16_t ioaddr, uint16_t iodata)
{
	d = find_dev(d, ioaddr);
	if (d == NULL)
		return 1;
	d->write(d, d->ioaddr - ioaddr, iodata);
	return 0;

}

void dev_register(struct dev **devices, struct dev *d, void *data)
{
	d->next = *devices;
	*devices = d;
	d->init(d, data);
}
