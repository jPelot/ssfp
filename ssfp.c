#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ssfp.h"

SSFPResponse *SSFP_new_response()
{
  SSFPResponse *response = malloc(sizeof(SSFPResponse));
  response->context = NULL;
  response->session = NULL;
  response->num_directives = 0;
  response->types = NULL;
  response->names = NULL;
  response->ids = NULL;
  response->text = NULL;
  response->options = NULL;
  response->element_types = NULL;

  response->name_count = 0;
  response->text_count = 0;
  response->options_count = 0;
  response->element_count = 0;
  response->element_num_options = 0;
  return response;
}

void SSFP_free_response(SSFPResponse *response)
{
  SSFPDirective *cur_directive;
  free(response->context);
  free(response->session);
  /*
  for(int i = 0; i < response->num_directives; i++) {
    cur_directive = response->directives + (i*sizeof(SSFPDirective));
    switch (cur_directive->type) {
    case DTEXT:
      free(cur_directive->text.string);
      break;  
    case DFORM:
      free(cur_directive->form.id);
      free(cur_directive->form.name);
      break;
    case DELEMENT:
      free(cur_directive->element.data);
      free(cur_directive->element.id);
      free(cur_directive->element.name);
      break;
    }
    free(cur_directive);
  }
  */
  free(response);
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

int parse_directive(SSFPResponse *res, char *str) {
  char **strptr = &str;
  char *index;
  char buffer[256];
  DType dir_type;

  if (str[0] == '%') {
    dir_type = DTEXT;
  } else if (str[0] == '&') {
    dir_type = DFORM;
  } else {
    dir_type = DELEMENT;
  }

  if (dir_type == DTEXT) {
    res->types[res->num_directives] = DTEXT;
    int len = strlen(str+1);
    res->text[res->text_count] = malloc(len+1);
    strcpy(res->text[res->text_count], str+1);
    res->num_directives++;
    res->text_count++;
    return 0;
  }
  
  next_subline(strptr, buffer);

  if (dir_type == DFORM) {
    res->types[res->num_directives] = DFORM;
    char *c = strchr(buffer, ' ');
    if(c == NULL) {
      return 1;
    }
    *c = '\0';
    res->ids[res->name_count] = malloc(strlen(buffer+1) + 1);
    strcpy(res->ids[res->name_count],buffer+1);

    int len = strlen(c+1);
    if (len < 1) {
      return 1;
    }
    res->names[res->name_count] = malloc(len+1);
    strcpy(res->names[res->name_count], c+1);

    res->num_directives++;
    res->name_count++;
    return 0;
  }

  char *c_index;
  char *type, *id, *name;

  if ((type = strtok(buffer, " ")) == NULL) {
    return 1;
  }
  if ((id = strtok(NULL, " ")) == NULL) {
    return 1;
  }
  if ((name = strtok(NULL, "\0")) == NULL) {
    return 1;
  }

  element_type e_type;
  if      (strcmp(type, "field") == 0) {e_type = FIELD;}
  else if (strcmp(type, "area") == 0)  {e_type = AREA;}
  else if (strcmp(type, "radio") == 0) {e_type = RADIO;}
  else if (strcmp(type, "check") == 0) {e_type = CHECK;}
  else if (strcmp(type, "submit") == 0){e_type = SUBMIT;}
  else {return 1;}
  res->types[res->num_directives] = DELEMENT;
  res->element_types[res->element_count] = e_type;
  res->names[res->name_count] = malloc(strlen(name)+1);
  strcpy(res->names[res->name_count], name);
  res->ids[res->name_count] = malloc(strlen(id) + 1);
  strcpy(res->ids[res->name_count], id);

  res->name_count++;
  res->num_directives++;

  if (e_type == FIELD || e_type == AREA) {
    int len = strlen(*strptr);
    res->text[res->text_count] = malloc(len+1);

    strcpy(res->text[res->text_count], *strptr);
    res->element_num_options[res->element_count] = -1;
    res->text_count++;
    res->element_count++;
  }
  else if (e_type == RADIO || e_type == CHECK) {
    int num_options = 0;

    while((next_subline(strptr, buffer)) == 0) {
      num_options++;
      char *c = strchr(buffer, ' ');
      if (c == NULL) {return 1;}
      *c = '\0';
      int len = strlen(buffer);
      res->option_ids[res->options_count] = malloc(len+1);
      strcpy(res->option_ids[res->options_count], buffer);

      len = strlen(c+1);
      res->options[res->options_count] = malloc(len+1);
      strcpy(res->options[res->options_count], c+1);

      res->options_count++;
    }
    res->element_num_options[res->element_count] = num_options;
    res->element_count++;
  } else {

    //res->element_num_options[res->element_count] = -1;
    //res->element_count++;
  }

  return 0;
}

int SSFP_parse_response(SSFPResponse *response, char *str) {
  char **strptr = &str;
  response->context = malloc(100);
  response->session = malloc(100);
  next_line(strptr, response->context);
  next_line(strptr, response->session);

  char line_buffer[256];

  int index = 0;
  int num_allocated = 10;

  response->types = malloc(sizeof(DType) * 100);
  response->names = malloc(sizeof(char*) * 100);
  response->ids = malloc(sizeof(char*) * 100);
  response->text = malloc(sizeof(char*) * 100);
  response->options = malloc(sizeof(char*) * 100);
  response->option_ids = malloc(sizeof(char*) * 100);
  response->element_types = malloc(sizeof(element_type*) * 100);
  response->element_num_options = malloc(sizeof(int)*100);

  while(next_line(strptr, line_buffer) == 0) {
    parse_directive(response, line_buffer);
  }
  return 0;
}

void
print_string_indent(char *str, char *indent) {
  char *strptr = strtok(str, "\n");
  printf("%s%s\n",indent,strptr);
  while ((strptr = strtok(NULL, "\n")) != NULL) {
    printf("%s%s\n",indent,strptr);
  }
}

void
print_boxed(char *str) {
  int len = strlen(str);
  printf("+");
  for(int i = 0; i < len + 2; i++) printf("-");
  printf("+\n| %s |\n+",str);
  for(int i = 0; i < len + 2; i++) printf("-");
  printf("+\n");
}

void
print_many(char *str, int num) {
  for (int i = 0; i < num; i++) printf("%s", str);
}


void
SSFP_print_response(SSFPResponse *res)
{
  //printf("CONTEXT: %s\nSESSION: %s\n", res->context, res->session);

  int name_counter = 0;
  int text_count = 0;
  int options_count = 0;
  int element_count = 0;
  int form_name_length = 0;
  
  for (int i = 0; i < res->num_directives; i++) {
    if (res->types[i] == DTEXT) {
      printf("\n");
      print_string_indent(res->text[text_count], "|");
      text_count++;
    }
    else if (res->types[i] == DFORM) {
      form_name_length = strlen(res->names[name_counter]);
      printf("\n====%s====\n\n", res->names[name_counter]);
      name_counter++;
    }
    else if (res->types[i] == DELEMENT) {
      element_type etype = res->element_types[element_count];
      if (res->element_types[element_count] == FIELD) {
        printf("%s: %s\n", res->names[name_counter],res->text[text_count]);
        text_count++;
      } else if (res->element_types[element_count] == AREA) {
        printf("%s:\n", res->names[name_counter]);
        printf("--------------------\n");
        printf("%s\n",res->text[text_count]);
        printf("--------------------\n");
        text_count++;
      } else if (etype == RADIO || etype == CHECK) {
        printf("%s:\n", res->names[name_counter]);
        for (int j = 0; j < res->element_num_options[element_count]; j++) {
          printf(" |%s: %s\n", res->option_ids[options_count], res->options[options_count]);
          options_count++;
        }
      } else if (etype == SUBMIT) {
          printf("\n");
          print_boxed(res->names[name_counter]);
      }

      element_count++;
      name_counter++;
    }
  }
  print_many("=", form_name_length+8);
  printf("\n");
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
