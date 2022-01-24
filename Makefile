OVPNSUP_SRCS = $(sort $(wildcard *.c) $(wildcard nk/*.c))
OVPNSUP_OBJS = $(OVPNSUP_SRCS:.c=.o)
OVPNSUP_DEP = $(OVPNSUP_SRCS:.c=.d)
INCL = -I.

CC ?= gcc
CFLAGS = -MMD -O2 -s -std=gnu99 -pedantic -Wall -Wextra -Wimplicit-fallthrough=0 -Wformat=2 -Wformat-nonliteral -Wformat-security -Wshadow -Wpointer-arith -Wmissing-prototypes -Wunused-const-variable=0 -Wcast-qual -Wsign-conversion -D_GNU_SOURCE -Wno-discarded-qualifiers
CPPFLAGS += $(INCL)

all: ovpnsup

ovpnsup: $(OVPNSUP_OBJS)
	$(CC) $(CFLAGS) $(INCL) -o $@ $^

-include $(OVPNSUP_DEP)

clean:
	rm -f $(OVPNSUP_OBJS) $(OVPNSUP_DEP) ovpnsup

.PHONY: all clean

