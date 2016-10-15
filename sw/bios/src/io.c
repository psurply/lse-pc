#include <io.h>

void outb(u16 port, u8 val)
{
	__asm__ volatile("outb %0, %1\n\t"
			 : /* No output */
			 : "a" (val), "d" (port)
			 : /* No clobber */);
}

u8 inb(u16 port)
{
	u8 res = 0;

	__asm__ volatile("inb %1, %0\n\t"
			 : "=&a" (res)
			 : "d" (port)
			 : /* No clobber */);

	return res;
}
