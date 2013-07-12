CFLAGS += -pthread

SRCS = $(wildcard *.c)

OBJS = $(SRCS:.c = .o)

CC = gcc

INCLUDES = -I./

CCFLAGS = -g -Wall -O0

all : $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES)
% : %.c
	$(CC) -c $< $(CCFLAGS)

clean:
	　　rm *.o
