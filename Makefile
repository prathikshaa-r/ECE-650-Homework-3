TARGETS=ringmaster player
CFLAGS=-std=gnu99 -ggdb3 -Wall -Werror -pedantic

all: $(TARGETS)

.PHONY: clean clobber

clean:
	rm -f *~ *.o $(TARGETS)

clobber:
	rm *~

ringmaster: ringmaster.c potato.h
	gcc $(CFLAGS) -o $@ $<

player: player.c
	gcc $(CFLAGS) -o $@ $<


# GNU Makefile manual: https://www.gnu.org/software/make/manual/html_node/Implicit-Rules.html#Implicit-Rules
