#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ssfp.h"


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

/*
 *  Copies string from **str to dest until the first instance of '\r'.
 *  Increments *str. Returns 1 if string is empty, otherwise, 0;
 */
int
next_line(char **str, char *dest)
{
  char *index;
  int length;
  // Skip any empty lines
  while(**str == '\r') {
    *str += 1;
    if (**str == '\n') {
      *str += 1;
    }
  }
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
  if (**str == '\n') {
    *str += 1;
  }
  return 0;
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
