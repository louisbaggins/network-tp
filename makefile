CC = g++
CFLAGS = -c -Wall
LDFLAGS =
CLIENT_DIR = client
SERVER_DIR = server
CLIENT_SOURCES = client.cpp
SERVER_SOURCES = $(wildcard $(SERVER_DIR)/*.cpp)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.cpp=.o)
SERVER_OBJECTS = $(SERVER_SOURCES:.cpp=.o)
CLIENT_EXECUTABLE = client
SERVER_EXECUTABLE = ./server/server

all: $(CLIENT_EXECUTABLE) $(SERVER_EXECUTABLE)

$(CLIENT_EXECUTABLE): $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(SERVER_EXECUTABLE): $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(CLIENT_DIR)/%.o: $(CLIENT_DIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

$(SERVER_DIR)/%.o: $(SERVER_DIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(CLIENT_EXECUTABLE) $(SERVER_EXECUTABLE) $(CLIENT_OBJECTS) $(SERVER_OBJECTS)
