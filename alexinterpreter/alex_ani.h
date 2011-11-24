#ifndef  _ALEX_ANI_H_
#define  _ALEX_ANI_H_
#include "alex_anic.h"

#define ani_pop_number(v_p)			alex_ani_p->_ani_pop_number(v_p)
#define ani_push_number(v_p, n)		alex_ani_p->_ani_push_number((v_p), (ALEX_NUMBER)(n))

#define ani_pop_string(v_p)			alex_ani_p->_ani_pop_string(v_p)
#define ani_push_string(v_p, s)		alex_ani_p->_ani_push_string((v_p), (char*)(s))

#define ani_pop_al(v_p)				alex_ani_p->_ani_pop_al(v_p)
#define ani_push_al(v_p, r_v)		alex_ani_p->_ani_push_al((v_p), (r_value)(r_v))

#define ani_pop_ptr(v_p)			alex_ani_p->_ani_pop_ptr(v_p)
#define ani_push_ptr(v_p, p)		alex_ani_p->_ani_push_ptr((v_p), (void*)(p))

#define ani_pop_func(v_p)			alex_ani_p->_ani_pop_func(v_p)
#define ani_reg_func(s, f)			alex_ani_p->_ani_reg_func((char*)(s), (void*)(f))


extern ani* alex_ani_p = NULL;
extern "C" _declspec(dllexport)
void alex_init_reg(ani* ani_p)
{
	alex_ani_p = ani_p;
}


#endif