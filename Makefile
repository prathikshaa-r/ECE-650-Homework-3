TARGETS = ringmaster player
CFLAGS=-std=gnu99 -ggdb3 -Wall -Werror -pedantic

all: $(TARGETS)

.PHONY: clean clobber

clean:
	rm -f *~ *.o $(TARGETS)

ringmaster: ringmaster.c
	g++ $(CFLAGS) -o $@ $<
