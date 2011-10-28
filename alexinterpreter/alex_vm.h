#ifndef _ALEX_VM_H_
#define _ALEX_VM_H_
#include "alex_sym.h"

#define CODE_MEM_LEN	512			// Ĭ�ϴ���γ���
#define DATA_MEM_LEN	64			// Ĭ�ϲ������ݶ�ջ�γ���
#define LOCAL_MEM_LEN	1024		// �ֲ����ݶ�ջ���ݶ�
#define GLOBAL_MEM_LEN	64			// Ĭ��ȫ�ֶ�ջ���ݶ�
#define CALL_MEM_LEN	256			// Ĭ�Ϻ������ö�ջ���ݶ�

enum _inst
{
	END,							//����ָ��
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
	ubyte inst_type;		// ָ������
	ubyte gl;				// global or local?
	r_value inst_value;		// ָ�����ֵ
}alex_inst;


typedef struct _c_inst
{
	alex_inst* root_ptr;	// ����θ�ָ��
	int inst_len;			// �Ѿ�ʹ�õĴ���γ���
	int inst_size;			// ��ǰ�ܹ��Ĵ���γ���
}c_inst;

typedef struct _d_data
{
	r_value* root_ptr;		// ���ݶ�ջ�θ�ָ��
	int data_len;			// �Ѿ�ʹ�õ����ݶ�ջ�γ���
	int data_size;			// ��ǰ�ܹ������ݶ�ջ�γ���
}d_data;

typedef struct _vm_env
{
	c_inst		code_ptr;	// �����ָ��				
	int			pc;			// ���������

	d_data		data_ptr;	// ���ݶ�ջ��ָ��
	int			top;		// ���ݶζ�ջָ��		

	d_data		local_ptr;	//�ֲ�������ջ
	int			local_top;	//�ֲ�������ջָ��
	
	d_data		call_ptr;	// �������ö�ջ
	d_data		global_ptr;	//ȫ�ֱ�����ջ
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
