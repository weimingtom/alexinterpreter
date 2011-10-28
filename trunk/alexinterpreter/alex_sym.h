#ifndef	 _ALEX_SYM_H_
#define  _ALEX_SYM_H_
#include "alex_string.h"
#include "alex_conf.h"

#define SYM_TABLE_LEN	128

enum sym_type{
	sym_type_error,
	sym_type_num,
	sym_type_string,
	sym_type_func,
	sym_type_reg_func,	// reg func
	sym_type_al,
	sym_type_alp,
	sym_type_pointer,
	sym_type_addr,
	sym_type_arg_num
};


typedef struct _r_value;

typedef  struct _alex_al{
	struct _r_value* al_v;
	int al_size;
	int al_len;
	int count;			//  引用count值
}alex_al;


typedef union _s_value{
	ALEX_NUMBER num;
	ALEX_STRING str;
	ALEX_FUNC	func;
	alex_al*	al; 
	ALEX_P		ptr;
	int			addr;
	int			arg_num;		
}s_value;


typedef struct _st{
	a_string name;
	enum sym_type s_t;
	s_value s_v;
	int st_addr;			// vm虚拟机中的数据地址
}st;

typedef struct _st_node{
	st	st_v;
	struct _st_node* next;
}st_node;


//  sym table
typedef struct _sym_table{
	st_node* st_l[SYM_TABLE_LEN];
}sym_table;


typedef struct _r_value{
	s_value r_v;		// rect value 
	
	enum sym_type r_t;	// rect type
}r_value;



extern sym_table* global_table;

sym_table* new_table();
st*	look_table(sym_table* s_t, char* s_str);
st* add_new_table(sym_table* s_t, char* name);
st* add_table(sym_table* s_t, st st_v);
void free_table(sym_table* s_t);
st* add_g_table(st  a_st);
void print_table(sym_table* s_t);

void free_st(st* st_p);
st new_func_st(char* name,  ALEX_FUNC  tn_p);
st new_num_st(char* name, ALEX_NUMBER tn_v);
st new_str_st(char* name, ALEX_STRING tn_s);

r_value new_number(ALEX_NUMBER num);

#endif
