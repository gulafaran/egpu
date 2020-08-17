CC=gcc
VERSION=v1.0
ARCH=x86-64
RELEASE=waffles
EXTRA_CFLAGS=-march=$(ARCH) -mtune=native -g -Wall -D_GNU_SOURCE

PKGFLAGS=
CFLAGS=`pkg-config --cflags libpci` $(EXTRA_CFLAGS)
LINKER=`pkg-config --libs libpci`
INCLUDE=-Isrc/


SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
DEPS = $(wildcard src/*.h)

gpuctl: $(OBJ)
	mkdir -p bin/
	$(CC) $(OBJ) $(LINKER) -o bin/gpuctl

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

install: gpuctl
	install -D -m755 bin/gpuctl $(DESTDIR)/usr/bin/gpuctl

.PHONY: uninstall
uninstall:
	rm $(DESTDIR)/usr/bin/gpuctl

.PHONY: clean
clean:
	rm src/*.o bin/gpuctl
