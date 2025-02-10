#ifndef ssfp_h
#define ssfp_h

#include "strarray.h"
#include "intarray.h"

typedef enum {
  NONE,
  FIELD,
  AREA,
  RADIO,
  CHECK,
  SUBMIT
} element_type;

// Element Option
typedef struct {
  char *id;
  char *name;
} element_option;

// Form Element
typedef struct {
  element_type type;
  char *id;
  int num_options;
  union {
    char *data;
    element_option **options;
  };
} form_element;

// Form
typedef struct {
  char *id;
  int num_elements;
  form_element **elements;
} Form;

// Response_data
typedef struct {
  char *context_id;
  char *session_id;
  int num_forms;
  Form **forms;
} response_data;

// Request Data
typedef struct {
  char *context_id;
  char *session_id;
  Form *form;
} request_data;



typedef enum {
  FORM,
  TEXT,
} e_type;

typedef struct {
  char *form_id;
  char *id;
  char *name;
  char *data;  
} f_element;

typedef struct {
  char* str;  
} t_element;

typedef struct {
  e_type type;
  
  union {
    f_element form;
    t_element str;
  };

} element;



typedef enum {
  SSFPField,
  SSFPArea,
  SSFPRadio,
  SSFPCheck,
  SSFPSubmit
} SSFPElementType;

typedef struct {
  char *string;  
} SSFPText;

/*
typedef struct {
  char *id;
  char *name;
} SSFPForm;
*/
typedef struct {
  char *id;
  char *name;
} SSFPOption;

typedef struct {
  int num_options;
  SSFPOption *options;
} SSFPOptionSet;

typedef struct {
  char *id;
  char *name;
  char *data;
} SSFPElement;

typedef enum {
  DTEXT,
  DFORM,
  DELEMENT,
} DType;

typedef struct {
  DType type;
  
} SSFPDirective;


typedef struct {
  char *id;
  char *name;

  IntArray element_types;
  IntArray num_options;
  IntArray options_index;

  StrArray element_names;
  StrArray element_ids;
  StrArray element_texts;
  StrArray option_names;
  StrArray option_ids;

  int num_submits;
  StrArray submit_ids;
  StrArray submit_names;
} SSFPForm;


typedef struct {
  // Header
  char *context;
  char *session;
  // Data
  int num_forms;
  SSFPForm **forms;

  IntArray types;

  StrArray messages;

} SSFPResponse;





SSFPResponse *SSFPResponse_create();
void SSFPResponse_destroy(SSFPResponse *response);
int SSFP_parse_response(SSFPResponse *SSFPResponse, char *str);
void SSFP_print_response(SSFPResponse *res);
char *SSFP_prompt(SSFPResponse *res);


request_data *new_request();
int parse_request(request_data *data, char *str);
void free_request_data(request_data *data);
void free_request_element(form_element *element); 

int parse_response_data(element **elements, char *str);

#endif //ssfp_h
