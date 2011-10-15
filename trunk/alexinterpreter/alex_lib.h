#ifndef  _ALEX_LIB_H_
#define _ALEX_LIB_H_
#include "alex_interpret.h"
#include "alex_sym.h"

#define pop_arg_num(a_l)		_pop_ret_num(&(a_l))
#define pop_arg_func(a_l)		_pop_ret_func(&(a_l))
#define pop_arg_str(a_l)		_pop_ret_str(&(a_l))
#define pop_arg_ptr(a_l)		_pop_ret_ptr(&(a_l))
#define pop_arg_al(a_l)			_pop_ret_al(&(a_l))

#define pop_arg(a_l)			_pop_ret(&(a_l))

#define check_ret(a_l, a_t)	( ((a_l)==NULL || (a_l)->ret_value.r_t != a_t )?(0):(1) )

ret_node* _pop_ret(ret_node** arg_list);
ret_node* alex_print(ret_node* arg_list);
void alex_reg_lib(sym_table* g_t);
char* _pop_ret_str(ret_node** arg_list);
ALEX_NUMBER _pop_ret_num(ret_node** arg_list);
ALEX_FUNC _pop_ret_func(ret_node** arg_list);
void* _pop_ret_ptr(ret_node** arg_list);
alex_al* _pop_ret_al(ret_node** arg_list);


#endif