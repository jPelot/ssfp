#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "socket.h"

void delay(int milliseconds) {
  clock_t start_time = clock();
  while(clock() < start_time + milliseconds * CLOCKS_PER_SEC /1000);
}

int
main(int argc, char *argv[])
{
  char *IP = "127.0.0.1";
  char *PORT = "8080";
  if (argc == 0) {
    printf("Defaulting to 127.0.0.1:8080\n");
  } 
  if (argc > 1) {
    IP = argv[1];
  }
  if (argc > 2) {
    PORT = argv[2];
  }

  if (argc > 3 || PORT == 0) {
    printf("Usage: ssfp-client [ip_address] [port]\n");
    exit(1);
  }

  printf("Connecting to %s:%s\n", IP, PORT);

  int fd = start_client(IP,PORT);

  client_send(" \r\n \r\n");

  delay(500);

  char* response = client_read();

  printf("Response:\n%s\n",response);  
  
  return 1;
}

