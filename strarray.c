#include <stdlib.h>
#include <string.h>
#include "strarray.h"

#define INITIAL_SIZE 10

struct string_array_t {
  int strptr_allocated;
  int length;
  int index;
  char **arr;
};

StrArray
StrArray_create()
{
  StrArray strarr = malloc(sizeof(StrArray));
  strarr->arr = malloc(INITIAL_SIZE);
  strarr->strptr_allocated = INITIAL_SIZE;
  strarr->length = 0;
  strarr->index  = 0;
  return strarr;
}

void
StrArray_destroy(StrArray arr)
{
  for(int i = 0; i < arr->length; i++) {
    free(arr->arr[i]);
  }
  free(arr->arr);
  free(arr);
}

void
StrArray_add(StrArray arr, const char *str)
{
  if(arr->length >= arr->strptr_allocated) {
    arr->arr = realloc(arr->arr, arr->strptr_allocated + INITIAL_SIZE);
    arr->strptr_allocated += INITIAL_SIZE;
  }  
  int len = strlen(str);
  arr->arr[arr->length] = malloc(len+1);
  strcpy(arr->arr[arr->length], str);
  arr->length++;
}

int
StrArray_length(StrArray arr)
{
  return arr->length;
}

void
StrArray_begining(StrArray arr)
{
  arr->index = 0;
}

const char*
StrArray_next(StrArray arr) {
  if (arr->index >= arr->length) {
    return NULL;
  }
  return arr->arr[arr->index++];
}
