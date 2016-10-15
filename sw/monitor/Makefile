CFLAGS = -Wall -Wextra -Werror -std=c99
CPPFLAGS = -MMD -I../lib/inc -D_POSIX_C_SOURCE=200809L
LDFLAGS = -L../lib -llsepc -lreadline -lutil

PREFIX ?= /usr

SRC = main.c \
      monitor.c \
      dev/dev.c \
      dev/serial.c

OBJ = $(SRC:.c=.o)
DEPS = $(SRC:.c=.d)
ELF = lsepc-monitor

OUT = /tmp/lsepc-monitor.out

NIOS_ELF = ../bridge/supervisor/software/supervisor/supervisor.elf

.PHONY: all run stop sysemu clean install
all: $(ELF)

run: $(ELF)
	LD_LIBRARY_PATH=../lib/ ./$< -i

sysemu: $(ELF)
	LD_LIBRARY_PATH=../lib/ ./$< -c "sysemu" -r ../bios/rootfs.cpio

stop: $(ELF)
	LD_LIBRARY_PATH=../lib/ ./$< -c "stop"

$(ELF): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) $(ELF) $(DEPS)

install:
	install $(ELF) $(PREFIX)/bin

-include $(DEPS)
