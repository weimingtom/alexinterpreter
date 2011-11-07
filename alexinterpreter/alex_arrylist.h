#ifndef  _ALEX_ARRYLIST_H_
#define  _ALEX_ARRYLIST_H_
#include "alex_sym.h"

#define DEF_AL_SIZE 256

alex_al* new_al();
r_value add_al(alex_al* al, r_value r_v);
void del_al(alex_al* al);
r_value* get_al(alex_al* al, int inx);
void free_value(r_value* rv);
void copy_value(r_value* l_v_p, r_value r_v);


#endif

