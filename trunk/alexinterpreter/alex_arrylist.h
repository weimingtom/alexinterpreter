#ifndef  _ALEX_ARRYLIST_H_
#define  _ALEX_ARRYLIST_H_
#include "alex_sym.h"

#define DEF_AL_SIZE 256

#define new_al() _new_al(0)
#define get_al(al, inx)	( ((al)==NULL || (al)->al_len < (inx) || (inx) <0 )?(NULL):(&(((al)->al_v)[(inx)])) )

alex_al* _new_al(int def_count);
r_value add_al(alex_al* al, r_value r_v);
r_value* _get_al(alex_al* al, int inx);


#endif

