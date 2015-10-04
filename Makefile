CC=gcc
CFLAGS=-std=c11
LDFLAGS=

SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ifj15

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) *.o
