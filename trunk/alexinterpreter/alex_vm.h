#ifndef _ALEX_VM_H_
#define _ALEX_VM_H_
#include "alex_sym.h"

#define CODE_MEM_LEN	512			// 默认代码段长度
#define DATA_MEM_LEN	64			// 默认操作数据堆栈段长度
#define LOCAL_MEM_LEN	1280		// 局部堆栈数据段

enum _inst
{
	PUSH,
	POP,
	MOVE,
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
	d_data		data_ptr;	// 数据堆栈段指针	
	int			pc;			// 程序计数器			
	int			top;		// 数据段堆栈指针		

	d_data		local_ptr;	//局部堆栈
	int			local_top;	//局部堆栈指针
}vm_env;

#define relloc_stack(d_d)	relloc_data((d_d), DATA_MEM_LEN)
#define relloc_local(d_d)	relloc_data((d_d), LOCAL_MEM_LEN)
vm_env new_vm_env();
c_inst* relloc_code(c_inst* c_i);
d_data* relloc_data(d_data* d_d, int d_len);

#endif