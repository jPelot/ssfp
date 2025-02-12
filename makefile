SUBMODULE_DIR = ssfp-client
C_FILES = $(wildcard $(SUBMODULE_DIR)/*.c) $(wildcard *.c)
OBJECT_FILES = $(C_FILES:.c=.o) 
CC = gcc
CFLAGS = -Wall -g -Issfp-client

LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto

.PHONY: submodules update-submodules

submodules:
	git submodule init
	git submodule update

update-submodules:
	git submodule foreach 'git pull origin main'

all: client server

client: submodules $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o client $(OBJECT_FILES) $(LDLIBS)
	
$(SUBMODULE_DIR)/%.o: $(SUBMODULE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -f $(OBJECT_FILES)



