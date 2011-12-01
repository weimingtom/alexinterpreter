#ifndef _ALEX_ANIC_H_
#define _ALEX_ANIC_H_

#include "alex_conf.h"
#include "alex_sym.h"
//#include "alex_vm.h"
		

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

#endif