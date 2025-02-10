#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ssfp.h"
#include "socket.h"
#include "strarray.h"

int
main()
{
  int conn = start_server();
  int sockfd;
  char cbuf[20];
  
  while(1) {
    char *buff = server_listen(conn);

    if(buff == NULL) {
      continue;
    }

    SSFPRequest *req = SSFPRequest_create();

    parse_request(req, buff);
    
    int nums[] = {-1,-1};
    int count = 0;

    printf("Context: %s\nSession: %s\n",rd->context_id,rd->session_id);
    Form *form = rd->form;
    for(int i = 0; i < rd->form->num_elements; i++) {
      form_element *element = form->elements[i];
      printf("%s\n",element->id);
      for (int j = 0; j < element->num_options; j++) {
        printf("|%s\n",element->options[j]->id);
        nums[count++] = atoi(element->options[j]->id);
      }
    }
    StrArray arr = StrArray_create();
    StrArray_add(arr, "calc\r\n \r\n%Number adder:\nEnter in 2 numbers, and we'll add 'em!\r\n");
    StrArray_add(arr, "");
    StrArray_add(arr, "");
    StrArray_add(arr, "");
    StrArray_add(arr, "\r\n&calc Calculator\r\nfield num1 Number One\n");
    StrArray_add(arr, "");
    StrArray_add(arr, "\r\nfield num2 Number Two\n");
    StrArray_add(arr, "");
    StrArray_add(arr, "\r\nsubmit add Add!\r\n");

    if (count == 2) {
      StrArray_set(arr, 1, "%");
      StrArray_set(arr, 2, "Your sum is: ");
      sprintf(cbuf, "%d",nums[0]+nums[1]);
      StrArray_set(arr, 3, cbuf);
      
      sprintf(cbuf, "%d", nums[0]);
      StrArray_set(arr, 5, cbuf);
      
      sprintf(cbuf, "%d", nums[1]);
      StrArray_set(arr, 7, cbuf);
    }

    char* response = StrArray_combine(arr);
    server_send(response);
    free(response);
    StrArray_destroy(arr);
    free(buff);
  }
}
