#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ssfp.h"
#include "parser.h"
#include "strarray.h"

void
print_many(char *str, int num)
{
  for (int i = 0; i < num; i++) printf("%s", str);
}

void
print_string_indent(const char *str, char *indent)
{
  char *buff = malloc(strlen(str)+1);
  strcpy(buff, str);
  char *strptr = strtok(buff, "\n");
  printf("%s%s\n", indent, strptr);
  while ((strptr = strtok(NULL, "\n")) != NULL) {
    printf("%s%s\n",indent,strptr);
  }
}

void
print_boxed(const char *str)
{
  int len = strlen(str);
  printf("+");
  for(int i = 0; i < len + 2; i++) printf("-");
  printf("+\n| %s |\n+",str);
  for(int i = 0; i < len + 2; i++) printf("-");
  printf("+\n");
}

SSFPForm*
SSFPForm_create()
{
  SSFPForm *form = malloc(sizeof(SSFPForm));
  form->element_types = IntArray_create();
  form->num_options   = IntArray_create();
  form->options_index = IntArray_create();
  form->element_names = StrArray_create();
  form->element_ids   = StrArray_create();
  form->element_texts = StrArray_create();
  form->option_names  = StrArray_create();
  form->option_ids    = StrArray_create();
  form->submit_ids    = StrArray_create();
  form->submit_names  = StrArray_create();
  return form;
}

SSFPResponse*
SSFPResponse_create()
{
  SSFPResponse *res = malloc(sizeof(SSFPResponse));
  res->context = NULL;
  res->session = NULL;
  res->forms = malloc(sizeof(SSFPForm*) * 100);
  res->num_forms = 0;
  res->types = IntArray_create();
  res->messages = StrArray_create();
  return res;
}

SSFPRequest*
SSFPRequest_create()
{
  SSFPRequest *req = malloc(sizeof(SSFPRequest));
  req->context = NULL;
  req->session = NULL;
  req->submit_id = NULL;
  req->form = SSFPForm_create();
  return req;
}

void
SSFPResponse_destroy(SSFPResponse *res)
{
  free(res->context);
  free(res->session);
  for (int i = 0; i < res->num_forms; i++) {
    //SSFPForm_destroy(res->forms[i]);
  }
  free(res->forms);  
  free(res);
}

int
parse_directive(SSFPResponse *res, Parser p)
{ 
  char *type, *id, *name;
  StrArray option_ids, option_names;
  DType dir_type;
  element_type e_type;
  SSFPForm *form; 

  switch (Parser_first_char(p)) {
  case '%':
    dir_type = DTEXT;
    break;
  case '&':
    dir_type = DFORM;
    break;
  default:
    dir_type = DELEMENT;
    break;
  }

  if (dir_type == DTEXT) {
    IntArray_add(res->types, DTEXT);
    StrArray_add(res->messages, Parser_line(p)+1);
    return 0;
  }
  
  if (dir_type == DFORM) {
    form = SSFPForm_create();
    res->forms[res->num_forms++] = form;
    IntArray_add(res->types, DFORM);
    form->id = Parser_field(p, 0, 0)+1;
    form->name = Parser_field(p, 1, 1);
    return 0;
  }

  if (res->num_forms == 0) {
    return 1;
  }

  form = res->forms[res->num_forms - 1];
  
  if ((type = Parser_field(p,0,0)) == NULL) return 1;
  if ((id   = Parser_field(p,1,0)) == NULL) return 1;
  if ((name = Parser_field(p,2,1)) == NULL) return 1;

  if      (strcmp(type, "field")  == 0) e_type = FIELD;
  else if (strcmp(type, "area")   == 0) e_type = AREA;
  else if (strcmp(type, "radio")  == 0) e_type = RADIO;
  else if (strcmp(type, "check")  == 0) e_type = CHECK;
  else if (strcmp(type, "submit") == 0) e_type = SUBMIT;
  else return 1;

  switch(e_type) {
  case SUBMIT:
    StrArray_add(form->submit_names, name);
    StrArray_add(form->submit_ids, id);
    break;
  case FIELD:
  case AREA:
    IntArray_add(form->element_types, e_type);
    StrArray_add(form->element_names, name);
    StrArray_add(form->element_ids, id);
    StrArray_add(form->element_texts, Parser_data(p));
    IntArray_add(form->num_options, -1);
    break;
  case RADIO:
  case CHECK:
    IntArray_add(form->element_types, e_type);
    StrArray_add(form->element_names, name);
    StrArray_add(form->element_ids, id);
    option_ids = Parser_option_ids(p);
    option_names = Parser_option_names(p);
    StrArray_add_arr(form->option_ids, option_ids);
    StrArray_add_arr(form->option_names, option_names);
    IntArray_add(form->num_options, StrArray_length(option_ids));
    StrArray_destroy(option_ids);
    StrArray_destroy(option_names);
    break;
  default:
    break;
  }
  return 0;
}

int SSFP_parse_response(SSFPResponse *response, char *str)
{
  Parser p = Parser_create(str);
  response->context = malloc(100);
  response->session = malloc(100);
  strcpy(response->context, Parser_line(p));
  Parser_nextline(p);
  strcpy(response->session, Parser_line(p));
  
  while (Parser_nextline(p)) {
    parse_directive(response, p);
  }
  return 0;
}

void
SSFPForm_print(SSFPForm *form)
{
  int form_name_length = strlen(form->name);
  printf("\n====%s====\n\n", form->name);

  int type;
  for (int i = 0; i < IntArray_length(form->element_types); i++) {
    type = IntArray_get(form->element_types, i);
    switch (type) {
    case FIELD:
      printf("%s: %s\n", StrArray_next(form->element_names),
                         StrArray_next(form->element_texts));
      break;    
    }
  }
  printf("\n");
  for (int i = 0; i < StrArray_length(form->submit_ids); i++) {
    print_boxed(StrArray_get(form->submit_names, i));
  }

  print_many("=", form_name_length + 8);
  printf("\n");
}

void
SSFPResponse_print(SSFPResponse *res)
{
  int form_counter = 0;
  StrArray_begining(res->messages);

  for(int i = 0; i < IntArray_length(res->types); i++) {
    if (IntArray_get(res->types, i) == DTEXT) {
      printf("\n");
      print_string_indent(StrArray_next(res->messages), "|");
    } else {
      SSFPForm_print(res->forms[form_counter++]);
    }
  }
}

char*
SSFP_prompt(SSFPResponse *res)
{
  StrArray arr = StrArray_create();
  StrArray_add(arr, " ");   //Context 0
  StrArray_add(arr, "\r\n");
  StrArray_add(arr, " ");   //Session 2
  StrArray_add(arr, "\r\n");
  StrArray_add(arr, " ");   //Form id 3
  StrArray_add(arr, "\r\n");
  StrArray_add(arr, " ");  // Submit id 4
  StrArray_add(arr, "\r\n");

  int form_index = 0;

  char inputbuffer[200];

  while (form_index < 1 || form_index > res->num_forms) {
    printf("Select form to enter (1-%d): ", res->num_forms);
    scanf("%d", &form_index);
  }
  form_index -= 1;

  SSFPForm * form = res->forms[form_index];  

  StrArray_set(arr,3,form->id);

  system("clear");  
  printf("====%s====\n",form->name);
  scanf("%*c");
  const char* id;
  const char* name;
 
  for (int i = 0; i < StrArray_length(form->element_names); i++) {

    id = StrArray_get(form->element_ids, i);
    name = StrArray_get(form->element_names, i);

    StrArray_add(arr, id);
    StrArray_add(arr,"\n");
    printf("%s: ",name);
    int chars;
    scanf("%s%n",inputbuffer, &chars);

    StrArray_add(arr, inputbuffer);
    StrArray_add(arr, "\r\n");  
  }

  system("clear");
  return StrArray_combine(arr);
}

int
parse_request_element(SSFPForm *form, Parser p, SSFPForm *res_form)
{
  char **strptr;
  char buffer[100];
  char *id;
  int e_type = NONE;
  StrArray option_ids, option_names;
  
  if ((id   = Parser_field(p,0,0)) == NULL) return 1;

  for(int i = 0; i < StrArray_length(res_form->element_ids); i++) {
    if (strcmp(StrArray_get(res_form->element_ids, i), id) == 0) {
      e_type = IntArray_get(res_form->element_types, i);
      break;
    }
  }

  switch(e_type) {
  case FIELD:
  case AREA:
    IntArray_add(form->element_types, e_type);
    StrArray_add(form->element_ids, id);
    StrArray_add(form->element_texts, Parser_data(p));
    IntArray_add(form->num_options, -1);
    break;
  case RADIO:
  case CHECK:
    IntArray_add(form->element_types, e_type);
    StrArray_add(form->element_ids, id);
    option_ids = Parser_option_ids(p);
    option_names = Parser_option_names(p);
    StrArray_add_arr(form->option_ids, option_ids);
    StrArray_add_arr(form->option_names, option_names);
    IntArray_add(form->num_options, StrArray_length(option_ids));
    StrArray_destroy(option_ids);
    StrArray_destroy(option_names);
    break;
  default:
    return 1;
    break;
  }
  return 0;  
}

int
parse_request(SSFPRequest *req, char *str, SSFPResponse *res)
{
  char **strptr;
  char buffer[100];
  SSFPForm *res_form = NULL;

  Parser p = Parser_create(str);
  
  req->context = malloc(100);
  req->session = malloc(100);
  req->submit_id = malloc(100);
  req->form_id = malloc(100);

  strcpy(req->context, Parser_line(p));
  Parser_nextline(p);
  strcpy(req->session, Parser_line(p));
  Parser_nextline(p);
  strcpy(req->form_id, Parser_line(p));
  Parser_nextline(p);
  strcpy(req->submit_id, Parser_line(p));

  for(int i = i; i < res->num_forms; i++) {
    if (strcmp(res->forms[i]->id, req->form_id) == 0) {
      res_form = res->forms[i];
    }
  }
  if (res_form == NULL) {return 1;}
  
  while (Parser_nextline(p)) {
    parse_request_element(req->form, p, res_form);  
  }

  return 0;
}
