CC=gcc
CFLAGS=
LDFLAGS=

SOURCES=main.cc
OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=ifj15

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

