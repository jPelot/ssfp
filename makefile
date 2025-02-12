CC     = gcc
CFLAGS = -g
RM     = rm -f

HEADERS = ssfp-client.h socket.h strarray.h intarray.h parser.h
OBJECTS = ssfp-client.o socket.o strarray.o intarray.o parser.o

LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto

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
