SOURCE = $(wildcard *.cc)
OBJS = $(patsubst %.cc, %, $(SOURCE))

CXX=g++

all:${OBJS}

.PHONY:clean
	clean:
	rm -f *.o *.d bts

%.o:%.cc
	$(CXX) -c $< -o $@

%.d: %.cc
	@set -e; rm -f $@; \
	$(CXX) -MM $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
