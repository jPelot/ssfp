#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ssfp.h"
#include "socket.h"

int
main()
{
  int conn = start_server();
  int sockfd;
  
  while(1) {
    char *buff = server_listen(conn);

    if(buff == NULL) {
      continue;
    }

    request_data *rd = new_request();

    parse_request(rd, buff);

    printf("Context: %s\nSession: %s\n",rd->context_id,rd->session_id);
    Form *form = rd->form;
    for(int i = 0; i < rd->form->num_elements; i++) {
      form_element *element = form->elements[i];
      printf("%s\n",element->id);
      for (int j = 0; j < element->num_options; j++) {
        printf("|%s\n",element->options[j]->id);
      }
    }

    char response[] = "calc\r\n \r\n&number Number\nfield num1 Number One\r\n";
    server_send(response);

    free(buff);
  }
}
