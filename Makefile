EXECUTABLE=crawler
CC=g++
CFLAGS=-c -Wall -std=c++11 -pthread
LDFLAGS=
LIBS=-lcurl -lpthread
DEPS=$(wildcard *.h)
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean run

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

run:
	./$(EXECUTABLE)
