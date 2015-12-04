CC?=gcc
CFLAGS=-std=c11 -c -g -pedantic -Wall
LDFLAGS=

SOURCES=main.c scanner.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ifj15

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE)  *.o
