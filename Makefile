CFLAGS=-std=c99 -O3 -Wall 
LIBS=-lmagic

all: archive-scanner

archive-scanner: archive-scanner.c
	$(LINK.c) $(LIBS) $< -o $@

clean:
	rm -f *.o archive-scanner

.PHONY: all clean
