#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ssfp.h"
#include "parser.h"
#include "strarray.h"

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

void
SSFP_free_response(SSFPResponse *res)
{
  free(res->context);
  free(res->session);
  for (int i = 0; i < res->num_forms; i++) {
    //SSFPForm_destroy(res->forms[i]);
  }
  free(res->forms);  
  free(res);
}

SSFPForm*
SSFPForm_create() {
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

/*
 *  Copies string from **str to dest until the first instance of '\r'.
 *  Increments *str. Returns 1 if string is empty, otherwise, 0;
 */
 
int
next_line(char **str, char *dest)
{
  char *index;
  int length;
  
  // If at end of string, return 1
  if (**str == '\0') {
    return 1;
  }
  // Calculate length of line
  index = strchr(*str, '\r');
  if (index == NULL) {
    length = strlen(*str);
  } else {
    length = index - *str;
  }

  // Copy line to dest
  memcpy(dest, *str, length);
  dest[length] = '\0';
  //Increment **str
  *str += length;
  if (**str != '\0') {
    *str +=1;
  }
  if (**str == '\n') {
    *str += 1;
  }
  return 0;
}

/*
int
next_subline(char **str, char *dest)
{
  char *index;
  int length;

  // If at end of string, return 1
  if (**str == '\0') {
    return 1;
  }
  // Calculate length of line
  index = strchr(*str, '\n');
  if (index == NULL) {
    length = strlen(*str);
  } else {
    length = index - *str;
  }

  // Copy line to dest
  memcpy(dest, *str, length);
  dest[length] = '\0';
  //Increment **str
  *str += length;
  if (**str == '\n') {
    *str += 1;
  }
  return 0;
}
*/
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
    form->id = Parser_field(p, 0)+1;
    form->name = Parser_field(p, 1);
    return 0;
  }

  if (res->num_forms == 0) {
    return 1;
  }

  form = res->forms[res->num_forms - 1];
  
  if ((type = Parser_field(p,0)) == NULL) return 1;
  if ((id   = Parser_field(p,1)) == NULL) return 1;
  if ((name = Parser_field(p,2)) == NULL) return 1;

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

int SSFP_parse_response(SSFPResponse *response, char *str) {
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

void
print_many(char *str, int num)
{
  for (int i = 0; i < num; i++) printf("%s", str);
}

void
print_form(SSFPForm *form) {

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
SSFP_print_response(SSFPResponse *res)
{
  //printf("CONTEXT: %s\nSESSION: %s\n", res->context, res->session);

  int form_count = 0;
  int form_name_length = 0;

  int type;

  while (!IntArray_at_end(res->types)) {
    type = IntArray_next(res->types);
    switch (type) {
    case DTEXT:
      printf("\n");
      print_string_indent(StrArray_next(res->messages), "|");
      break;
    case DFORM:
      print_form(res->forms[form_count]);
      break;
    }
  }
  /*
  while (StrArray_cur(res->directive_types) != NULL) {

    if (StrArray_cmp(res->directive_types, "message")) {
      printf("message\n");
      print_string_indent(StrArray_next(res->messages), "|");
    }
    else if (StrArray_cmp(res->directive_types, "form")) {
      form_name_length = strlen(StrArray_cur(res->form_names));
      printf("\n====%s====\n\n", StrArray_next(res->form_names));
    }
    else if (StrArray_cmp(res->directive_types, "element")) {
      //element_type etype = res->element_types[element_count];
      // Field Elements
      if (StrArray_cmp(res->element_types, "field")) {
        printf("%s: %s\n", StrArray_next(res->element_names),StrArray_next(res->element_texts));
      // Area ELements
      } else if (StrArray_cmp(res->element_types, "area")) {
        printf("%s:\n", StrArray_cur(res->element_names));
        printf("--------------------\n");
        printf("%s\n", StrArray_cur(res->element_texts));
        printf("--------------------\n");
      // Radio and Check Elements
      } else if (StrArray_cmp(res->element_types, "radio")
              || StrArray_cmp(res->element_types, "check")) {
        printf("%s:\n", StrArray_next(res->element_names));
        for (int j = 0; j < res->element_num_options[element_count]; j++) {
          printf(" |%s: %s\n", StrArray_next(res->option_ids), StrArray_next(res->options));
        }
      // Submit elements
      } else if (StrArray_cmp(res->element_types, "submit")) {
          printf("\n");
          print_boxed(StrArray_next(res->element_names));
      }

      element_count++;
    }
  }
  print_many("=", form_name_length+8);
  printf("\n");
  */
}

/*
void
SSFP_begining(SSFPResponse *res)
{
  IntArray_start(res->directive_types);
  StrArray_begining(res->form_names);
  StrArray_begining(res->form_ids);
  StrArray_begining(res->messages);
  IntArray_start(res->element_types);
  StrArray_begining(res->element_names);
  StrArray_begining(res->element_ids);
  StrArray_begining(res->element_texts);
  StrArray_begining(res->options);
  StrArray_begining(res->option_ids);
  IntArray element_num_options;
}
*/


/*
void
SSFP_next(SSFPResponse *res) {
  DType cur_type;
  if (res->directive_index < res->num_directives) {
    cur_type = res->types[res->directive_index];

    if (cur_type == DTEXT) {
      StrArray_next(res->text);
    }

    if (cur_type == DFORM) {
      StrArray_next(res->names);
      StrArray_next(res->ids);
    }

    if (cur_type == DELEMENT) {
      StrArray_next(res->names);
      StrArray_next(res->ids);
      int options = res->element_num_options[res->element_index++];
      if (options == -1) {
        StrArray_next(res->text);
      }
      else {
        for(int i = 0; i < options; i++) {
          StrArray_next(res->options);
          StrArray_next(res->option_ids);
        }
      }
    }
    res->directive_index++;
  }
}
*/

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


form_element*
new_form_element()
{
  form_element *element = malloc(sizeof(form_element));
  element->id = NULL;
  element->num_options = 0;
  element->data = NULL;
  return element;
}

int
add_element_option(form_element *element, char *id, char *name)
{
  int count = element->num_options;
  element_option **option_arr;

  //printf("%d - %s\n", count, id);

  // Use malloc on initial allocation, otherwise use realloc.
  if(count == 0) {
    option_arr = malloc(sizeof(element_option*));
  } else {
    option_arr = realloc(element->options, sizeof(element_option*)*(count+1));
  }

  // Test return value of allocation.
  if (option_arr == NULL) {
    return 1;
  }

  // Allocate memory for the option.
  option_arr[count] = malloc(sizeof(element_option));
  if(option_arr[count] == NULL) {
    return 1;
  }

  option_arr[count]->id = malloc(strlen(id)+1);
  option_arr[count]->name = malloc(strlen(name)+1);

  strcpy(option_arr[count]->id, id);
  strcpy(option_arr[count]->name, name);
  
  element->num_options += 1;
  element->options = option_arr;
  return 0;
}

Form*
new_form()
{
  Form *form = malloc(sizeof(Form));
  form->id = NULL;
  form->num_elements = 0;
  form->elements = NULL;
  return form;
}

int
add_form_element(Form *form, form_element *element)
{
  int count = form->num_elements;
  form_element **element_array;
  
  if (count == 0) {
    element_array = malloc(sizeof(form_element*));
  } else {
    element_array = realloc(form->elements, sizeof(form_element*)*(count+1));
  }

  if (element_array == NULL) {
    return 1;
  }

  element_array[count] = element;
  form->elements = element_array;
  form->num_elements += 1;
  return 0;
}

response_data*
new_response_data() {
  response_data *rd = malloc(sizeof(response_data));
  rd->context_id = NULL;
  rd->session_id = NULL;
  rd->num_forms = 0;
  rd->forms = NULL;
  return rd;
}

int
add_form(response_data *rd, Form *f)
{
  int count = rd->num_forms;
  Form **form_array;

  if (count == 0) {
    form_array = malloc(sizeof(Form*));
  } else {
    form_array = realloc(rd->forms, sizeof(Form*)*(count+1));
  }

  if (form_array == NULL) {
    return 1;
  }

  form_array[count+1] = f;
  rd->num_forms += 1;
  return 0;
}


int
extract_until(char **str, char *target, char* sample)
{
  char *crlf_pos = strstr(*str, sample);
  int length;

  if (**str == '\0') {
    return 1;
  }

  if (crlf_pos == NULL) {
    sample = "";
    length = strlen(*str);
  } else {
    length = crlf_pos - *str;
  }


  strncpy(target, *str, length);
  target[length] = '\0';
  
  *str += length + strlen(sample);
  
  return 0;
}


request_data*
new_request()
{
  return malloc(sizeof(request_data));
}


element_type
classify_element_type(char *str)
{
  if (strcmp(str,"field") == 0) return FIELD;
  else if (strcmp(str, "area") == 0) return AREA;
  else if (strcmp(str, "radio") == 0) return RADIO;
  else if (strcmp(str, "check") == 0) return CHECK;
  return NONE;
}


int
next_segment(char **str, char *dest) {
  char *index;
  int length;
  // If at end of string, return 1
  if (**str == '\0') {
    return 1;
  }
  // Calculate length of line
  index = strchr(*str, '\n');
  if (index == NULL) {
    length = strlen(*str);
  } else {
    length = index - *str;
  }
  // Copy line to destination
  memcpy(dest, *str, length);
  dest[length] = '\0';
  // Increment string pointer
  *str += length;
  if (**str != '\0') {
    *str += 1;
  }
  return 0;
}

form_element*
parse_request_element(char *str)
{
   form_element *element;
   char **strptr;
   char buffer[100];

   // Allocate new form element
   element = new_form_element();
   // Initialize string pointer
   strptr = &str;
   // Get element id
   if (next_segment(strptr, buffer) == 1) {
     return NULL;
   }
   element->id = malloc(strlen(buffer)+1);
   strcpy(element->id,buffer);
   // Get options
   while(next_segment(strptr, buffer) == 0) {
     //printf("%s\n", buffer);
     add_element_option(element, buffer, "");
   }

   return element;
}

int
parse_request(request_data *data, char *str)
{
  char **strptr;
  char buffer[100];
  
  data->context_id = malloc(100);
  data->session_id = malloc(100);
  strptr = &str;
  // Get the context id  
  if(next_line(strptr, data->context_id) == 1) {
    return 1;
  }
  // Get the session id
  if(next_line(strptr, data->session_id) == 1) {
    return 1;
  }
  // Parse Form Elements
  data->form = new_form();
  form_element *cur;
  char *index;


  // Iterate over every element of form
  while (next_line(strptr, buffer) == 0) {
    //printf("%s\n",buffer);
    cur = parse_request_element(buffer);
    add_form_element(data->form, cur);
  }

  return 0;
 
}

Form*
parse_form(char **str) {
  Form *form = new_form();
  char line[2000];
  char *lineptr = (char *)line;
  char buff[100];

  if ((next_line(str, line))== 0) {
    return NULL;
  }

  form->id = malloc(100);
  if ((next_segment(&lineptr, form->id)) == 0) {
    return NULL;
  }

  while (next_line(str, line) == 0) {
    


    if(**str == '&') break;
  }

  return form;
}

int
parse_response_data(element **elements, char *str)
{
  char **strptr;
  char buffer[100];  
  strptr = &str;

  // Get the context id  
  if(next_line(strptr, buffer) == 1) {
    return 1;
  }
  // Get the session id
  if(next_line(strptr, buffer) == 1) {
    return 1;
  }

  elements = malloc(sizeof(element)*20);

  int index = 0;

  char curForm[100];
  /*
  while(next_line(strptr, buffer) == 0) {
    if(buffer[0] == '&') {
      memcpy(curForm, buffer[1]);
      continue;
    }
    char *ptr = strtok(buffer,"_");
    

    index++;
  }*/

  return 0;  
}


void
free_request_data(request_data *data)
{
  free(data->context_id);
  free(data->session_id);
  Form form = *data->form;
  free(data->form);

  free(form.id);
  for(int i = 0; i < form.num_elements; i++) {
    form_element element = *form.elements[i];
    free(element.id);
    
    for (int j = 0; j < element.num_options; j++) {
      free(element.options[j]->id);
      free(element.options[j]->name);
      free(element.options[j]);
    }
    free(element.options);

    free(form.elements[i]);  
  }
  free(form.elements);
  free(data);
}


void
free_form_element(form_element *element)
{
  
} 
