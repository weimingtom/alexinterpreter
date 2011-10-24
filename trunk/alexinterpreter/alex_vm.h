#ifndef _ALEX_VM_H_
#define _ALEX_VM_H_
#include "alex_sym.h"

#define CODE_MEM_LEN	512			// Ĭ�ϴ���γ���
#define DATA_MEM_LEN	64			// Ĭ�ϲ������ݶ�ջ�γ���
#define LOCAL_MEM_LEN	1280		// �ֲ���ջ���ݶ�

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
	ubyte inst_type;		// ָ������
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
	d_data		data_ptr;	// ���ݶ�ջ��ָ��	
	int			pc;			// ���������			
	int			top;		// ���ݶζ�ջָ��		

	d_data		local_ptr;	//�ֲ���ջ
	int			local_top;	//�ֲ���ջָ��
}vm_env;

#define relloc_stack(d_d)	relloc_data((d_d), DATA_MEM_LEN)
#define relloc_local(d_d)	relloc_data((d_d), LOCAL_MEM_LEN)
vm_env new_vm_env();
c_inst* relloc_code(c_inst* c_i);
d_data* relloc_data(d_data* d_d, int d_len);

#endif