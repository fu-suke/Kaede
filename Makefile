CFLAGS:=-std=c11 -g -fno-common
SRCS:=tokenize.c $(wildcard parser/*.c) codegen.c main.c func_def.c error.c

OBJS:=$(SRCS:.c=.o)

kaede: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): kaede.h parser/parse.h

unit_test: unit_test.o tokenize.o parser/*.o error.o

test: kaede
	./test.sh

test-tokenize: unit_test
	./test.sh tokenize

test-parse: unit_test
	./test.sh parse

clean:
	rm -f kaede unit_test *.o *~ tmp*

.PHONY: test clean test-tokenize test-parse

