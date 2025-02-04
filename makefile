CC     = gcc
CFLAGS = -g
RM     = rm -f

HEADERS = ssfp.h socket.h
OBJECTS = ssfp.o socket.o

default: all

all: ssfp-server ssfp-client

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

ssfp-server: ssfp-server.o $(OBJECTS)
	$(CC) ssfp-server.o $(OBJECTS) -o ssfp-server

ssfp-client: ssfp-client.o $(OBJECTS)
	$(CC) ssfp-client.o $(OBJECTS) -o ssfp-client

clean:
	-rm -f *.o
