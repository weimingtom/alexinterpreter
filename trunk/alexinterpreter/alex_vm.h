#ifndef _ALEX_VM_H_
#define _ALEX_VM_H_
#include "alex_sym.h"

#define CODE_MEM_LEN	512			// 默认代码段长度
#define DATA_MEM_LEN	64			// 默认操作数据堆栈段长度
#define LOCAL_MEM_LEN	1024		// 局部数据堆栈数据段
#define GLOBAL_MEM_LEN	64			// 默认全局堆栈数据段
#define CALL_MEM_LEN	256			// 默认函数调用堆栈数据段


enum _e_vm_ret{
	VM_SUCCESS,
	VM_ERROR,
	VM_ERROR_PC,
	VM_ERROR_POP,
	VM_ERROR_OP_VALUE,
	VM_ERROR_NOT_IDE,
	VM_ERROR_ADD_OP,
	VM_ERROR_REG_FUNC,
	VM_ERROR_AL
};

enum _e_register{
	REG_AX,
	REG_BX,
	REG_CX,
	REG_SP,

	REG_LEN
}e_register;

#define NULL_ADDR		(-1)

typedef enum _inst
{
	END,							// 结束指令
	PUSH,							// 向栈顶PUSH进去一个常量
	VAR,							// 向局部堆栈申请一个变量 默认为0
	PUSHVAR,						// 向栈顶写入一个变量
	POP,							// 弹出栈顶数据
	NEWAL,							// 创建一个数组
	AL,								// 获取一个数组中的某个组员
	JFALSE,							// 如果为假则跳转
	JTRUE,							// 如果为真则跳转
	MOVE,							// 将栈顶的数据写入变量中
	MOVEAL,							// 将栈顶的数据写入数组组员中
	MOVEREG,						// 将栈顶的数据写入寄存器中
	TABLE,							// 
	ADD,							// +
	SUB,							// -
	MUL,							// *
	DEV,							// /
	MOD,							// %
	AND,							// &&
	OR,								// ||
	BIG,							// >
	BIGE,							// >=
	LIT,							// <
	LITE,							// <=
	EQU,							// ==
	NEQU,							// !=
	CALL,							// 函数调用
	JUMP,							// 跳转
	RET,							// return
	SADD,
	BSADD,
	SSUB,
	BSSUB,
	INST_COUNT
}e_alex_inst;


typedef struct _alex_inst
{
	ubyte inst_type;		// 指令类型
	ubyte gl;				// global or local?
	r_value inst_value;		// 指令操作值
	int line;				// 指令对应的代码行
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

	r_value		reg[REG_LEN];// 寄存器
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
//#define check_value(r_v)	_check_value(r_v)
#define check_value(r_v)	do{r_value t_r_v=(r_v);if(t_r_v.r_t==sym_type_error) { print("vm[error pc: %d] you are try pop a error value!\n", alex_vm_env.pc); return VM_ERROR_POP;} }while(0)
#define check_at_value(r_v, tt)	do{r_value t_r_v=(r_v);if(t_r_v.r_t==sym_type_error || t_r_v.r_t!=(tt)) return VM_ERROR_POP;}while(0) 
//#define check_vm(rt)		do{int r=(rt); if(r) return r;}while(0)

#define check_vm(rt)	(rt)

#define next_pc(v_p)		((v_p)->pc++)
extern vm_env alex_vm_env;

c_inst* relloc_code(c_inst* c_i);
d_data* relloc_data(d_data* d_d, int d_len);
vm_env* init_vm_env();
void free_vm_evn(vm_env* vm_p);

void push_inst(c_inst* code_ptr, alex_inst a_i);
void push_stack(vm_env* vm_p, r_value r_v);
int push_local(vm_env* vm_p, r_value r_v);
int push_global(vm_env* vm_p, r_value r_v);
r_value pop_data(d_data* d_ptr);
void push_data(d_data* d_ptr, r_value r_v);
int alex_vm(vm_env* vm_p);
int _check_value(r_value r_v);
void vm_print(vm_env* vm_p);

#endif
