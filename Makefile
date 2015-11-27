CC?=gcc
CFLAGS=-std=c11 -c -g -pedantic -Wall
LDFLAGS=

SOURCES=main.c ial.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ifj15

CHECK_SOURCES=check_main.c ial.c hash.c
CHECK_OBJECTS=$(CHECK_SOURCES:.c=.o)
CHECK_EXECUTABLE=check_ifj15

CHECK_LDFLAGS=-lcheck -lm -lrt -pthread

all: $(SOURCES) $(EXECUTABLE)
check: $(CHECK_SOURCES) $(CHECK_EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(CHECK_EXECUTABLE): $(CHECK_OBJECTS)
	$(CC) $(CHECK_OBJECTS) $(CHECK_LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(CHECK_EXECUTABLE) *.o
