#include "altera_avalon_pio_regs.h"
#include "system.h"

#include "dev.h"
#include "supervisor.h"

struct led_data {
	uint8_t reg;
} led_data = {
	.reg = 0
};

static uint16_t led_read(struct dev *dev, uint16_t offset)
{
	if (offset == 0)
		return ((struct led_data *) dev->data)->reg;
	return 0;
}

static void led_write(struct dev *dev, uint16_t offset, uint16_t value)
{
	if (offset == 0) {
		((struct led_data *) dev->data)->reg = value;
		IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, value);
	}
}

static void led_init(struct dev *dev, void *data)
{
	(void) data;
}

struct dev dev_led = {
	.ioaddr = 0x10,
	.size = 1,
	.init = led_init,
	.read = led_read,
	.write = led_write,
	.data = &led_data
};
