#include <stdio.h>
#include <stdlib.h>
#include "socket.h"

int
main(int argc, char *argv[])
{
  char *IP = "127.0.0.1";
  int PORT = 8080;
  if (argc == 0) {
    printf("Defaulting to 127.0.0.1:8080\n");
  } 
  if (argc > 1) {
    IP = argv[1];
  }
  if (argc > 2) {
    PORT = atoi(argv[2]);
  }

  if (argc > 3 || PORT == 0) {
    printf("Usage: ssfp-client [ip_address] [port]\n");
    exit(1);
  }

  printf("Connecting to %s:%d\n", IP, PORT);

  int fd = start_client(IP,PORT);

  client_send(fd, "id_one\r\nid_two\r\nele_1\noption1\noption2\n\rele_2\nnice\r\n");
  
  return 1;
}

