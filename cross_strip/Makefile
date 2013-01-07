
C=arm-hisiv200-linux-gcc

STRIP_OPT=-s --strip-program=arm-hisiv200-linux-strip
INSTALL=/usr/bin/install -c

test:test.o
	$(CC) -o test test.o
test.o:
	$(CC) -c test.c

install: install-files
install-files:
	$(INSTALL) -m 0755 $(STRIP_OPT) test /home/duli/nfsroot/teststrip
