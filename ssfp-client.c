#include <stdio.h>
#include "ssfp.h"
#include "socket.h"

int
main() {
  int fd = start_client("64.23.255.27",8080);

  client_send(fd, "id_one\r\nid_two\r\nele_1\noption1\noption2\n\rele_2\nnice\r\n");
  
  return 1;
}

