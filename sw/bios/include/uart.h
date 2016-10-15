#ifndef UART_H_
#define UART_H_

#include <io.h>
#include <types.h>

#define COM1	0x3F8
#define COM2	0x2F8

#define uart1_readb()			uart_readb(COM1)
#define uart1_writeb(C)			uart_writeb(COM1, C)
#define uart1_write(Str)		uart_write(COM1, Str)
#define uart1_write_newline(Str)	uart_write_newline(COM1, Str)

#define uart2_readb()			uart_readb(COM2)
#define uart2_writeb(C)			uart_writeb(COM2, C)
#define uart2_write(Str)		uart_write(COM2, Str)
#define uart2_write_newline(Str)	uart_write_newline(COM2, Str)


#define GENERAL_REG(Port)	(Port)
#define STATUS_REG(Port)	((Port) + 5)

#define RECEIVER_READY		1
#define TRANSMITTER_READY	(1 << 5)

#define CAN_RECEIVE(Port)	(inb(STATUS_REG(Port)) & RECEIVER_READY)
#define CAN_TRANSMIT(Port)	(inb(STATUS_REG(Port)) & TRANSMITTER_READY)

char uart_readb(u16 port);
void uart_writeb(u16 port, u8 c);
void uart_write(u16 port, const char *str);
void uart_write_newline(u16 port, const char *str);

#endif /* !UART_H_ */
