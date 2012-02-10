#ifndef _LLP_H_
#define _LLP_H_
#include "lp_conf.h"

typedef struct _llp_env llp_env;
typedef struct _llp_mes llp_mes;

// --------env
llp_env* llp_new_env();
void llp_free_env(llp_env* p);

// --------regedit/delete .lpb files
int llp_reg_mes(llp_env* env, char* mes_name);
int llp_del_mes(llp_env* env, char* mes_name);

// --------new/delete a message object
llp_mes*  llp_message_new(llp_env* env, char* mes_name);
void llp_message_clr(llp_mes* in_mes);
void  llp_message_free(llp_mes* in_mes);

// --------write a message object
int llp_Wmes_int32(llp_mes* lm, char* filed_str, llp_int32 number);
int llp_Wmes_int64(llp_mes* lm, char* filed_str, llp_int64 number);
int llp_Wmes_float32(llp_mes* lm, char* filed_str, llp_float32 number);
int llp_Wmes_float64(llp_mes* lm, char* filed_str, llp_float64 number);
int llp_Wmes_string(llp_mes* lm, char* filed_str, char* str);
int llp_Wmes_stream(llp_mes* lm, char* filed_str, unsigned char* ptr, unsigned int len);
llp_mes* llp_Wmes_message(llp_mes* lm, char* filed_str);

// ---------read a message object
llp_int32 llp_Rmes_int32(llp_mes* lm, char* filed_str, unsigned int al_inx);
llp_int64 llp_Rmes_int64(llp_mes* lm, char* filed_str, unsigned int al_inx);
llp_float32 llp_Rmes_float32(llp_mes* lm, char* filed_str, unsigned int al_inx);
llp_float64 llp_Rmes_float64(llp_mes* lm, char* filed_str, unsigned int al_inx);
slice* llp_Rmes_stream(llp_mes* lm, char* filed_str, unsigned int al_inx);
char* llp_Rmes_string(llp_mes* lm, char* filed_str, unsigned int al_inx);
llp_mes* llp_Rmes_message(llp_mes* lm, char* filed_str, unsigned int al_inx);
unsigned int llp_Rmes_size(llp_mes* lm, char* filed_str);

// ------- out/in a message body
slice* llp_out_message(llp_mes* lms);
int llp_in_message(slice* in, llp_mes* lms);

#endif