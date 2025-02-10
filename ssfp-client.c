#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "socket.h"
#include "ssfp.h"

void delay(int milliseconds) {
  clock_t start_time = clock();
  while(clock() < start_time + milliseconds * CLOCKS_PER_SEC /1000);
}


void
interactive() {
  
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
  system("clear");
  printf("Connecting to %s:%s\n", IP, PORT);

  
  int fd = start_client(IP,PORT);

  client_send(" \r\n \r\n");

  delay(50);

  char* data = client_read();
  client_close(fd);

  SSFPResponse *response = SSFPResponse_create();
  SSFP_parse_response(response, data);
  SSFPResponse_print(response);
  //SSFP_free_response(response);

  char in_buffer[200];
  
  while(1) {

    char* message = SSFP_prompt(response);
    fd = start_client(IP,PORT);
    if (fd == 0) continue;
    
    client_send(message);
    delay(50);
    char* data = client_read();
    client_close(fd);

    response = SSFPResponse_create();
    SSFP_parse_response(response, data);
    SSFPResponse_print(response);
    //SSFP_free_response(response);
  }
  
  
  return 1;
}

