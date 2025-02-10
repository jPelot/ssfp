#include <stdio.h>
#include "ssfp.h"
#include "parser.h"

int main() {

  char str[] = "MYCONTEXT\r\nEPICSAUCE\r\n%epic comment\nHello, World!\r\n&newform My New Form\r\nfield my_text Epic Text Box\nSample Text\r\ncheck multichoice Multible Choice\none Option One\ntwo Option Two\r\n";

  Parser p = Parser_create(str);

  do {
    printf("%s\n-%s\n-%s\n", Parser_line(p), Parser_field(p, 0), Parser_field(p, 1));
    printf("Data:%s\n",Parser_data(p));
  } while (Parser_nextline(p));
  return 0;
}
