CC     = gcc
CFLAGS = -g
RM     = rm -f

HEADERS = ssfp.h socket.h strarray.h
OBJECTS = ssfp.o socket.o strarray.o

LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto

default: all

all: ssfp-server ssfp-client

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

ssfp-server: ssfp-server.o $(OBJECTS)
	$(CC) ssfp-server.o $(OBJECTS) -o ssfp-server $(LDFLAGS) $(LDLIBS)

ssfp-client: ssfp-client.o $(OBJECTS)
	$(CC) ssfp-client.o $(OBJECTS) -o ssfp-client $(LDFLAGS) $(LDLIBS)

clean:
	-rm -f *.o
