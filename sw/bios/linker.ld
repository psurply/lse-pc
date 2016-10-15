OUTPUT_ARCH(i386)
ENTRY(main)

MEMORY
{
	/* We will actually take only the last 32K for the BIOS code */
	INTERNAL_RAM (rwx) : ORIGIN = 0xC00F0000, LENGTH = 64K
	EXTERNAL_RAM (rwx) : ORIGIN = 0xC0100000, LENGTH = 1M
}

/* The stack must be aligned on a 16 byte boundary */
_stack = ORIGIN(INTERNAL_RAM) + LENGTH(INTERNAL_RAM) - 16;

SECTIONS
{
	/* BIOS startup code after CPU reset */
	.startup : AT(ADDR(.startup) - 0xC0000000)
	{
		. += LENGTH(INTERNAL_RAM) / 2;
		_begin_code = .;

		*(.startup)
		*(.startup.*)
	} >INTERNAL_RAM

	.text : AT(ADDR(.text) - 0xC0000000)
	{
		_main = .;
		*(.text.main)
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
	} >INTERNAL_RAM

	.data : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
		*(.data.*)
	}

	.bss : AT(ADDR(.bss) - 0xC0000000)
	{
		*(.bss)
		*(.bss.*)
	}

	.reset :  AT(ADDR(.reset) - 0xC0000000)
	{
		. = ORIGIN(INTERNAL_RAM) + LENGTH(INTERNAL_RAM) - 16;
		_begin_reset = .;

		*(.reset)

		. = ORIGIN(INTERNAL_RAM) + LENGTH(INTERNAL_RAM);
	} >INTERNAL_RAM
}
