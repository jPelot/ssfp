#include <stdlib.h>
#include <stdio.h>
#include "ssfp.h"
#include "socket.h"

int
main()
{
  int conn = start_server();
  int sockfd;
  
  while(1) {
    sockfd = server_listen(conn);
    char *buff = read_fd(sockfd);

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

    free(buff);
  }
}
