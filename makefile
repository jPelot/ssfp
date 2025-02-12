CC     = gcc
CFLAGS = -g
RM     = rm -f

HEADERS = ssfp-client.h socket.h strarray.h intarray.h parser.h
OBJECTS = ssfp-client.o socket.o strarray.o intarray.o parser.o

override LDFLAGS += -L/usr/local/ssl/lib
override LDLIBS += -lssl -lcrypto

default: all

all: server client

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

server: server.o $(OBJECTS)
	$(CC) server.o $(OBJECTS) -o server $(LDFLAGS) $(LDLIBS)

client: client.o $(OBJECTS)
	$(CC) client.o $(OBJECTS) -o client $(LDFLAGS) $(LDLIBS)

clean:
	-rm -f *.o
