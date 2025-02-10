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

typedef enum {
  SSFPField,
  SSFPArea,
  SSFPRadio,
  SSFPCheck,
  SSFPSubmit
} SSFPElementType;

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

typedef struct {
  char *context;
  char *session;
  char *submit_id;
  char *form_id;
  SSFPForm *form;
} SSFPRequest;


SSFPResponse *SSFPResponse_create();
void SSFPResponse_destroy(SSFPResponse *response);
int SSFP_parse_response(SSFPResponse *SSFPResponse, char *str);
void SSFPResponse_print(SSFPResponse *res);
char *SSFP_prompt(SSFPResponse *res);

SSFPRequest *SSFPRequest_create();
int parse_request(SSFPRequest *req, char *str, SSFPResponse *res);

#endif //ssfp_h
