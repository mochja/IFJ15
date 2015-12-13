CC?=gcc
CFLAGS=-std=c11 -c -pedantic -Wall
LDFLAGS=

ifeq ($(DEBUG), 1)
	CFLAGS += -g -DDEBUG
endif

SOURCES=main.c scanner.c parser.c list.c hash.c token.h zval.h expr.c instruction.c vm.c ial.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ifj15

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE)  *.o
