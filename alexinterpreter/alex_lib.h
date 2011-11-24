#ifndef  _ALEX_LIB_H_
#define _ALEX_LIB_H_
#include "alex_conf.h"
#include "alex_sym.h"

void alex_reg_lib(sym_table* g_t);
void reg_dll(char* str, ALEX_FUNC a_f);

void alex_ani_init();
int alex_reg_dll(char* dll);
#endif