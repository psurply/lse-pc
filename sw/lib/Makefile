CFLAGS = -Wall -Wextra -Werror -std=c99 -fpic
CPPFLAGS = -MMD -I./inc
LDFLAGS =

PREFIX ?= /usr

SRC = lsepc.c

OBJ = $(SRC:.c=.o)
DEPS = $(SRC:.c=.d)
LIB = liblsepc.so

.PHONY: all clean install

all: $(LIB)

$(LIB): $(OBJ)
	$(CC) -shared $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) $(LIB) $(DEPS)

install:
	install $(LIB) $(PREFIX)/lib/

-include $(DEPS)
