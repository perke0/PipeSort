CC ?= gcc
AR ?= ar
RANLIB ?= ranlib

CFLAGS ?= -O3 -std=c11 -Wall -Wextra -Wshadow -Wconversion -Wpedantic -fPIC
INCS   := -Iinclude -Iinternal

# Public API wrapper sources
SRC_API := \
  src/psort_u.c \
  src/psort_u128.c \
  src/psort_u256.c \
  src/psort_u512.c

# Internal algorithm sources (copied into internal/)
SRC_INTERNAL := \
  internal/pipe_sort_u128.c \
  internal/pipe_sort_u256_idx_radix8.c \
  internal/pipe_sort_u512_idx_radix8.c

OBJ := $(SRC_API:.c=.o) $(SRC_INTERNAL:.c=.o)

LIB_STATIC := libpipesort.a
LIB_SHARED := libpipesort.so

PREFIX ?= /usr/local

.PHONY: all static shared clean install uninstall

all: static

static: $(LIB_STATIC)
shared: $(LIB_SHARED)

$(LIB_STATIC): $(OBJ)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(LIB_SHARED): $(OBJ)
	$(CC) -shared -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -f $(OBJ) $(LIB_STATIC) $(LIB_SHARED)

install: static
	install -d $(DESTDIR)$(PREFIX)/include/pipesort
	install -m 644 include/pipesort/*.h $(DESTDIR)$(PREFIX)/include/pipesort/
	install -d $(DESTDIR)$(PREFIX)/lib
	install -m 644 $(LIB_STATIC) $(DESTDIR)$(PREFIX)/lib/

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/include/pipesort
	rm -f $(DESTDIR)$(PREFIX)/lib/$(LIB_STATIC)
