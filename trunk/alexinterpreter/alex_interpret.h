#ifndef  _ALEX_INTERPRET_H_
#define  _ALEX_INTERPRET_H_
#include "alex_sym.h"
#include "alex_parsing.h"


typedef struct _inter_env{
	sym_table* g_table;		// global table
	sym_table* l_table;		// local table
}inter_env;


// ret type
typedef enum _ret_type{
	ret_normal,
	ret_break,
	ret_continue,
	ret_return
}ret_type;


// tree return value
typedef struct _ret_node{
	r_value ret_value;
	ret_type rt;
	struct _ret_node* next;
}ret_node;

typedef int (*logic_op_func)(ret_node*, ret_node*);
typedef ret_node* (*reg_func)(ret_node*);

inter_env new_inter_env();
void free_inter_env(inter_env env);
void alex_interpret(inter_env env, tree_node* main_tree);
int  ret_to_bool(ret_node* rt_n);
ret_node* inter_seg(inter_env env, tree_node* t_n);
ret_node* new_ret_node(enum sym_type s_t);
ret_node* inter_p_call(inter_env env, tree_node* t_n, ret_node* arg_list);
ret_node* al_ptov(ret_node* al_p);
#endif