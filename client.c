#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "socket.h"
#include "ssfp-client.h"

void delay(int milliseconds) {
  clock_t start_time = clock();
  while(clock() < start_time + milliseconds * CLOCKS_PER_SEC /1000);
}

void
print_form(SSFP_Form form)
{
  printf("====%s====\n", SSFP_Form_name(form));  
  int num_elements = SSFP_Form_num_elements(form);

  for(int i = 0; i < num_elements; i++) {
    printf("%s\n", SSFP_Form_element_name(form, i));
  }
}

void
print_response(SSFP_Client client)
{
  printf("Session: %s\n", SSFP_Client_session(client));
  printf("Context: %s\n", SSFP_Client_context(client));

  int num_directives = SSFP_Client_num_directives(client);
  int dir;
  int form_index = 0;
  int status_index = 0;
  for (int i = 0; i < num_directives; i++) {
    dir = SSFP_Client_get_directive(client, i);
    if (dir == SSFP_STATUS) {
      printf("\n%s\n\n", SSFP_Client_get_status(client, status_index++));
      continue;
    }
    print_form(SSFP_Client_get_form(client, form_index++));
  }

  
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

  SSFP_Client client = SSFP_Client_create();
  SSFP_Client_parse_response(client, data);
  print_response(client);
  //SSFPResponse_print(response);
  //SSFP_free_response(response);

  char in_buffer[200];
  /*
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
  
  */
  return 1;
}

