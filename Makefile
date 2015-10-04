CC=gcc
CFLAGS=-std=c11
LDFLAGS=

SOURCES=main.c
OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=ifj15

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

