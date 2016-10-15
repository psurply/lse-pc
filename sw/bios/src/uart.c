#include <io.h>
#include <uart.h>
# define LED_PORT		0x10

char uart_readb(u16 port)
{
	while (!CAN_RECEIVE(port))
		continue;

	return inb(port);
}

void uart_writeb(u16 port, u8 c)
{
	while (!CAN_TRANSMIT(port))
		continue;

	outb(LED_PORT, inb(LED_PORT) ^ 1);
	outb(port, c);
}

void uart_write(u16 port, const char *str)
{
	for (int i = 0; str[i] != '\0'; ++i) {
		if (str[i] == '\n')
			uart_writeb(port, '\r');
		uart_writeb(port, str[i]);
	}
}

void uart_write_newline(u16 port, const char *str)
{
	uart_write(port, str);
	uart_write(port, "\n");
}
