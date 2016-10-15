ELF=bios
BINARY=$(addsuffix .bin, $(ELF))
MIF=$(addsuffix .mif, $(ELF))

SRCDIR=src/
INCLUDEDIR=include/
TOOLSDIR=../tools/

CC=gcc
CPPFLAGS=-I$(INCLUDEDIR) -MMD
ASFLAGS=-march=i386 -m32
CFLAGS=$(ASFLAGS) -std=c11 -mregparm=3 -ffreestanding -ggdb3 \
       -fno-stack-protector -fno-delete-null-pointer-checks \
       -fno-asynchronous-unwind-tables

LSCRIPT=linker.ld
LDFLAGS=-nostdlib -static -Wl,--build-id=none,--script=$(LSCRIPT)

SRC=reset.S startup.S isr.S main.c uart.c io.c shell.c string.c idt.c \
    command.c paging.c printf.c frame_alloc.c fs.c
SRC:=$(addprefix $(SRCDIR), $(SRC))
OBJS_TMP=$(SRC:.c=.o)
OBJS=$(OBJS_TMP:.S=.o)
DEPS=$(OBJS:.o=.d)

BIN2MIF=$(addprefix $(TOOLSDIR), bin2mif)
BASECODE=0xF8000
OFFSET=0x8000

OBJCOPY=objcopy
OFLAGS=-O binary

SIZE=size
SZFLAGS=-A

QEMU=$(addprefix $(TOOLSDIR), qemu/i386-softmmu/qemu-system-i386)
QFLAGS=-nographic -monitor pty -serial stdio -serial pty

MEMORY=$(addprefix $(TOOLSDIR), memory)

.PHONY: all clean debug distclean flash run

all: $(MIF) rootfs.cpio

$(MIF): $(BINARY)
	$(BIN2MIF) -o $@ -b $(BASECODE) -i $< $(BASECODE) $(OFFSET)

$(BINARY): $(ELF)
	$(OBJCOPY) $(OFLAGS) $< $@

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(SIZE) $(SZFLAGS) $@

flash: $(MIF) rootfs.cpio
	$(MEMORY) flash $<

clean:
	$(RM) $(OBJS) rootfs.cpio

distclean: clean
	$(RM) $(ELF) $(BINARY) $(MIF)

run: $(BINARY) $(QEMU)
	$(QEMU) $(QFLAGS) -bios $<

debug: $(BINARY) $(QEMU)
	$(QEMU) $(QFLAGS) -s -S -bios $<

qemu: $(QEMU)

$(QEMU):
	$(TOOLSDIR)/setup_qemu.sh $(TOOLSDIR)

%.cpio: $(wildcard rootfs/*)
	cd rootfs && \ls | cpio -ov > ../$@

-include $(DEPS)
