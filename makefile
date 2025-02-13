SUBMODULE_DIR = ssfp-client
C_FILES = $(wildcard $(SUBMODULE_DIR)/*.c) $(wildcard *.c)
OBJECT_FILES = $(C_FILES:.c=.o) 
CC = gcc
CFLAGS = -Wall -g -Issfp-client
LDFLAGS = -L/usr/local/ssl/lib
override LDLIBS += -lssl -lcrypto 

.PHONY: submodules update-submodules windows-libs windows-flags windows

submodules:
	git submodule init
	git submodule update

update-submodules:
	git submodule foreach 'git pull origin main'

windows: LDLIBS+= -lgdi32 -ladvapi32 -lcrypt32 -luser32 -lwsock32 -lWs2_32 
windows: CFLAGS+= -static

all: client

client: submodules $(OBJECT_FILES)
	mkdir -p bin
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/client $(OBJECT_FILES) $(LDLIBS)
	
$(SUBMODULE_DIR)/%.o: $(SUBMODULE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -f $(OBJECT_FILES)
	
windows: all



