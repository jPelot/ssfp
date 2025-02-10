#include <stdio.h>
#include "ssfp.h"


int main() {

  char str[] = "MYCONTEXT\r\nEPICSAUCE\r\n%epic comment\nHello, World!\r\n&newform My New Form\r\nfield my_text Epic Text Box\nSample Text\r\ncheck multichoice Multible Choice\none Option One\ntwo Option Two\r\n";
  SSFPResponse *response = SSFP_new_response();

  SSFP_parse_response(response, str);
  SSFP_print_response(response);
  SSFP_free_response(response);

  return 0;
}
