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
  DTEXT,
  DFORM,
  DELEMENT,
} DType;

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
  char *form_id;
  char *submit_id;
  
  StrArray ids;
  IntArray num_options;
  StrArray option_ids;
  StrArray text_data;

} SSFPRequest;


SSFPResponse* SSFPResponse_create();
void          SSFPResponse_destroy(SSFPResponse *response);
int           SSFPResponse_parse(SSFPResponse *SSFPResponse, char *str);
void          SSFPResponse_print(SSFPResponse *res);
SSFPRequest*  SSFPResponse_prompt(SSFPResponse *res);

SSFPRequest*  SSFPRequest_create();
void          SSFPRequest_destroy(SSFPRequest *req);
int           SSFPRequest_parse(SSFPRequest *req, char *str);
char*         SSFPRequest_to_string(SSFPRequest *req);

#endif //ssfp_h
