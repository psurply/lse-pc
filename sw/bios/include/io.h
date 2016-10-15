#ifndef IO_H_
# define IO_H_

# include <types.h>

void outb(u16 port, u8 val);
u8 inb(u16 port);

#endif /* !IO_H_ */
