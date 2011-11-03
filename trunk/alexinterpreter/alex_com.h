#ifndef _ALEX_COM_H_
#define _ALEX_COM_H_

#include "alex_vm.h"
#include "alex_sym.h"
#include "alex_parsing.h"

/*
	alex interpret compile
*/

#define TEMP_MEM_LEN 64

typedef enum _e_e{
	COM_SUCCESS,
	COM_ERROR_NOT_ALLOW,
	COM_ERROR_NOT_FIND_IDE,
	COM_ERROR_REDEF,
	COM_ERROR_NOT_LEFT_VALUE,
	COM_ERROR_POP,
	COM_ERROR_NOT_EXP,
	COM_ERROR_BREAK,
	COM_ERROR_CONTINUE,
	COM_ERROR_NOT_AL_INX,
	COM_ERROR_OTHER
}e_e;

typedef enum _e_reg_{
	COM_REG_RET,

	COM_RET_LEN
}e_reg;

typedef enum _e_gl{
	COM_ERROR,

	COM_GLOBAL,
	COM_LOCAL,

	COM_VALUE,
	COM_POINT,

	COM_REG,

	COM_GL_COUNT
}e_gl;

typedef struct _r_addr{
	e_gl gl;
	int addr;
}r_addr;

// 回写地址链表节点
typedef struct _b_addr_node{
	int back_addr;

	struct _b_addr_node* next;
}b_addr_node;

// 地址结构
typedef struct _s_addr{
	int begin_addr;
	int end_addr;
	b_addr_node* back_addr_head;		// 回写地址指令序列		
}s_addr;

typedef struct _addr_data{
	s_addr*	root_ptr;
	int addr_len;
	int addr_size;
}addr_data;

typedef struct _var_addr{
	sym_table* g_table;
	d_data	global_ptr;
	int g_top;
	
	sym_table* l_table;
	int l_top;
	
	int temp_reg[COM_RET_LEN];	// 临时编译寄存器

	addr_data addr_ptr;		//临时地址堆栈
}var_addr;

typedef struct _com_env{
	c_inst	   com_inst;	// compile inst
	int		   pc;			// 程序入口地址
	var_addr	var_table;	
}com_env;

#define g_com_addr(c_p,s) com_addr((c_p),(s), COM_GLOBAL)
#define l_com_addr(c_p,s) com_addr((c_p),(s), COM_LOCAL)
#define check_com(s)	  do{int r_s=(s); if(r_s) return r_s;}while(0) 
#define check_pop(r_v)	 do{(r_v.r_t==sym_type_error)?(return COM_ERROE_POP):(0)}while(0)
#define now_inst_addr(c_p)	((c_p)->com_inst.inst_len)
#define com_al_p(c_p, t_n)	com_al((c_p), (t_n), COM_POINT)
#define com_al_v(c_p, t_n)	com_al((c_p), (t_n), COM_VALUE)

extern com_env* com_env_p;

com_env* new_com_env();
r_addr com_addr(com_env* com_p, char* name, e_gl gl);
r_addr search_addr(com_env* com_p, char* name);
int  com_vardef(com_env* com_p, tree_node* t_n, e_gl gl);
#define com_g_vardef(c_p, t_n)		com_vardef((c_p), (t_n), COM_GLOBAL)
#define com_l_vardef(c_p, t_n)		com_vardef((c_p), (t_n), COM_LOCAL)
st*  look_com(com_env* com_p, char* str);
alex_inst new_inst(e_alex_inst e_i, ...);
int com_al(com_env* com_p, tree_node* t_n, ubyte v_p);
int com_print(com_env* com_p);
int alex_com(com_env* com_p, tree_node* main_tree, tree_node* func_tree);

#endif
