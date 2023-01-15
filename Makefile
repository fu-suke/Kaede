CFLAGS=-std=c11 -g -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

kaede: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): kaede.h

test: kaede
	./test.sh

clean:
	rm -f kaede *.o *~ tmp*

.PHONY: test clean