#ifndef ssfp_h
#define ssfp_h

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

request_data *new_request();
int parse_request(request_data *data, char *str);
void free_request_data(request_data *data);
void free_request_element(form_element *element); 

#endif //ssfp_h
