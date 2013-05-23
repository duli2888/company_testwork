#$(OBJNAME):$(objects)
#	$(CC) -o $(OBJNAME) $(obj)   $(LIB_PATHS) $(LDFLAGS)
CFLAGS	+=-lrt

ping:ping.o
	$(CC) ping.o -o ping 

ping.o:


clean:
	rm ping.o ping
