CC = gcc
CFLAGS = -W -Wall

BINARY = map-extractor
SOURCES = $(wildcard *.c)

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(BINARY)

clean:
	rm -f $(BINARY) $(BINARY).exe
