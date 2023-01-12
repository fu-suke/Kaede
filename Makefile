CFLAGS=-std=c11 -g -static

kaede: kaede.c

test: kaede
	./test.sh

assemble:
	cc -o tmp tmp.s
	./tmp

clean:
	rm -f kaede *.o *~ tmp*

.PHONY: test clean assemble
