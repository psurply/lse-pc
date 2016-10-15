#include <bios.h>
#include <frame_alloc.h>
#include <fs.h>
#include <idt.h>
#include <memory.h>
#include <paging.h>
#include <shell.h>
#include <stdio.h>
#include <uart.h>

void __section(".text.main") main(void)
{
	init_frames();
	setup_minimal_idt();
	init_fs();
	shell_run();
}
