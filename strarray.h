#ifndef strarray_h
#define strarray_h

typedef struct string_array_t* StrArray;

StrArray StrArray_create();
void     StrArray_destroy(StrArray arr);
void     StrArray_add(StrArray arr, const char *str);
void     StrArray_set(StrArray arr, int index, const char *str);
int      StrArray_length(StrArray arr);
const char* StrArray_get(StrArray arr, int index);
const char* StrArray_last(StrArray arr);
void    StrArray_add_arr(StrArray arr, StrArray that);

// Iteration functions
void        StrArray_begining(StrArray arr);
const char* StrArray_cur(StrArray arr);
const char* StrArray_next(StrArray arr);
int         StrArray_cmp(StrArray arr, const char *str);

char* StrArray_combine(StrArray arr);

#endif // strarray_h
