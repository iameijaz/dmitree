CC=gcc
CFLAGS=-Wall -O2
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

dmitree: dmitree.c
	$(CC) $(CFLAGS) -o dmitree dmitree.c

install: dmitree
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 dmitree $(DESTDIR)$(BINDIR)

clean:
	rm -f dmitree

.PHONY: install clean
