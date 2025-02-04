#include <stdio.h>
#include "ssfp.h"
#include "socket.h"

int
main() {
  int fd = start_client("127.0.0.1",8080);

  client_send(fd, "id_one\r\nid_two\r\nele_1\noption1\noption2\n\rele_2\nnice\r\n");
  
  return 1;
}

