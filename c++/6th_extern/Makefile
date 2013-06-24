test:tlib.o test.o

tlib.o:tlib.c

test.o:test.cc

TARGETS := test

latest: $(firstword $(TARGETS))

.PHONY: all clean

all: $(TARGETS)

clean:
	-@rm $(TARGETS)
