#ifndef _ALEX_VM_H_
#define _ALEX_VM_H_
#include "alex_sym.h"

#define CODE_MEM_LEN	512			// 默认代码段长度
#define DATA_MEM_LEN	64			// 默认操作数据堆栈段长度
#define LOCAL_MEM_LEN	1024		// 局部数据堆栈数据段
#define GLOBAL_MEM_LEN	64			// 默认全局堆栈数据段
#define CALL_MEM_LEN	256			// 默认函数调用堆栈数据段

enum _inst
{
	END,							//结束指令
	PUSH,
	POP,
	MOVE,
	TABLE,
	ADD,
	SUB,
	MUL,
	DEV,
	MOD,
	AND,
	OR,
	NOT,
	EQU,
	CALL,
	JUMP,
	RET
}e_alex_inst;


typedef struct _alex_inst
{
	ubyte inst_type;		// 指令类型
	ubyte gl;				// global or local?
	r_value inst_value;		// 指令操作值
}alex_inst;


typedef struct _c_inst
{
	alex_inst* root_ptr;	// 代码段根指针
	int inst_len;			// 已经使用的代码段长度
	int inst_size;			// 当前总共的代码段长度
}c_inst;

typedef struct _d_data
{
	r_value* root_ptr;		// 数据堆栈段根指针
	int data_len;			// 已经使用的数据堆栈段长度
	int data_size;			// 当前总共的数据堆栈段长度
}d_data;

typedef struct _vm_env
{
	c_inst		code_ptr;	// 代码段指针				
	int			pc;			// 程序计数器

	d_data		data_ptr;	// 数据堆栈段指针
	int			top;		// 数据段堆栈指针		

	d_data		local_ptr;	//局部变量堆栈
	int			local_top;	//局部变量堆栈指针
	
	d_data		call_ptr;	// 函数调用堆栈
	d_data		global_ptr;	//全局变量堆栈
}vm_env;

#define relloc_stack(d_d)	relloc_data((d_d), DATA_MEM_LEN)
#define relloc_local(d_d)	relloc_data((d_d), LOCAL_MEM_LEN)
#define relloc_global(d_d)	relloc_data((d_d), GLOBAL_MEM_LEN)
#define relloc_call(d_d)	relloc_data((d_d), CALL_MEM_LEN)

extern vm_env alex_vm;

c_inst* relloc_code(c_inst* c_i);
d_data* relloc_data(d_data* d_d, int d_len);
vm_env* init_vm_env();
void free_vm_evn(vm_env* vm_p);

void push_inst(c_inst* code_ptr, alex_inst a_i);
void push_stack(vm_env* vm_p, r_value r_v);
void push_local(vm_env* vm_p, r_value r_v);
void push_global(vm_env* vm_p, r_value r_v);
r_value pop_data(d_data* d_ptr);

#endif
