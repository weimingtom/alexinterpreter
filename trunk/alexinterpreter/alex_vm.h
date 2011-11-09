#ifndef _ALEX_VM_H_
#define _ALEX_VM_H_
#include "alex_sym.h"

#define CODE_MEM_LEN	512			// Ĭ�ϴ���γ���
#define DATA_MEM_LEN	64			// Ĭ�ϲ������ݶ�ջ�γ���
#define LOCAL_MEM_LEN	1024		// �ֲ����ݶ�ջ���ݶ�
#define GLOBAL_MEM_LEN	64			// Ĭ��ȫ�ֶ�ջ���ݶ�
#define CALL_MEM_LEN	256			// Ĭ�Ϻ������ö�ջ���ݶ�


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
	END,							// ����ָ��
	PUSH,							// ��ջ��PUSH��ȥһ������
	VAR,							// ��ֲ���ջ����һ������ Ĭ��Ϊ0
	PUSHVAR,						// ��ջ��д��һ������
	POP,							// ����ջ������
	NEWAL,							// ����һ������
	AL,								// ��ȡһ�������е�ĳ����Ա
	JFALSE,							// ���Ϊ������ת
	JTRUE,							// ���Ϊ������ת
	MOVE,							// ��ջ��������д�������
	MOVEAL,							// ��ջ��������д��������Ա��
	MOVEREG,						// ��ջ��������д��Ĵ�����
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
	CALL,							// ��������
	JUMP,							// ��ת
	RET,							// return
	
	INST_COUNT
}e_alex_inst;


typedef struct _alex_inst
{
	ubyte inst_type;		// ָ������
	ubyte gl;				// global or local?
	r_value inst_value;		// ָ�����ֵ
	int line;				// ָ���Ӧ�Ĵ�����
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

	r_value		reg[REG_LEN];// �Ĵ���
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
#define check_value(r_v)	do{r_value t_r_v=(r_v);if(t_r_v.r_t==sym_type_error) return VM_ERROR_POP;}while(0)
#define check_at_value(r_v, tt)	do{r_value t_r_v=(r_v);if(t_r_v.r_t==sym_type_error || t_r_v.r_t!=(tt)) return VM_ERROR_POP;}while(0) 
#define check_vm(rt)		do{int r=(rt); if(r) return r;}while(0)
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

#endif