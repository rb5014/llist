CC = gcc
CFLAGS = -fPIC -Iinclude -Wall -Wextra
LDFLAGS = -shared -lpthread
TARGET = libllist.so
SRC = src/llist.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean