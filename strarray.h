#ifndef strarray_h
#define strarray_h

typedef struct string_array_t* StrArray;

StrArray StrArray_create();
void     StrArray_destroy(StrArray arr);
void     StrArray_add(StrArray arr, const char *str);
int      StrArray_length(StrArray arr);

// Iteration functions
void        StrArray_begining(StrArray arr);
const char* StrArray_cur(StrArray arr);
const char* StrArray_next(StrArray arr);

#endif // strarray_h
