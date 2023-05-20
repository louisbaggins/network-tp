CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=server.cpp client.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLES=server client

all: $(EXECUTABLES)

server: server.o
	$(CC) $(LDFLAGS) server.o -o server

client: client.o
	$(CC) $(LDFLAGS) client.o -o client

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES) $(OBJECTS)
