#ifndef  _ALEX_ANI_H_
#define  _ALEX_ANI_H_

#include "alex_conf.h"
#include "alex_vm.h"

typedef void (*FUNC_REG_FUNC)(char*, void*);

typedef ALEX_NUMBER (*FUNC_POP_NUMBER)(void*);
typedef int (*FUNC_PUSH_NUMBER)(void*, ALEX_NUMBER);

typedef ALEX_STRING (*FUNC_POP_STRING)(void*);
typedef int (*FUNC_PUSH_STRING)(void*, char*);

typedef alex_al* (*FUNC_POP_AL)(void*);
typedef int	(*FUNC_PUSH_AL)(void*, r_value);

typedef void* (*FUNC_POP_PTR)(void*);
typedef int  (*FUNC_PUSH_PTR)(void*, void*);

typedef r_value (*FUNC_POP_FUNC)(void*);

typedef struct _ani
{
	FUNC_POP_NUMBER _ani_pop_number;
	FUNC_PUSH_NUMBER _ani_push_number;

	FUNC_POP_STRING _ani_pop_string;
	FUNC_PUSH_STRING _ani_push_string;

	FUNC_POP_AL	_ani_pop_al;
	FUNC_PUSH_AL _ani_push_al;

	FUNC_POP_PTR  _ani_pop_ptr;
	FUNC_PUSH_PTR _ani_push_ptr;

	FUNC_POP_FUNC  _ani_pop_func;
	FUNC_REG_FUNC  _ani_reg_func;
}ani;
		

#define ani_pop_number(v_p)			alex_ani._ani_pop_number(v_p)
#define ani_push_number(v_p, n)		alex_ani._ani_push_number((v_p), (ALEX_NUMBER)(n))

#define ani_pop_string(v_p)			alex_ani._ani_pop_string(v_p)
#define ani_push_string(v_p, s)		alex_ani._ani_push_string((v_p), (char*)(s))

#define ani_pop_al(v_p)				alex_ani._ani_pop_al(v_p)
#define ani_push_al(v_p, r_v)		alex_ani._ani_push_al((v_p), (r_value)(r_v))

#define ani_pop_ptr(v_p)			alex_ani._ani_pop_ptr(v_p)
#define ani_push_ptr(v_p, p)		alex_ani._ani_push_ptr((v_p), (void*)(p))

#define ani_pop_func(v_p)			alex_ani._ani_pop_func(v_p)
#define ani_reg_func(s, f)		alex_ani._ani_reg_func((char*)(s), (void*)(f))
#endif