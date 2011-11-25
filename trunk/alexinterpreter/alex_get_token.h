#ifndef _ALEX_GET_TOKEN_H_
#define _ALEX_GET_TOKEN_H_
#include "alex_conf.h"
#include "alex_symbol_table.h"
#include "alex_string.h"

// ö����������
enum _token_type{
	token_type_err,				// �����token����
	
	token_type_var,				// �ؼ�������
	token_type_func,			// function
	token_type_if,
	token_type_else,
	token_type_while,
	token_type_continue,
	token_type_break,
	token_type_sizeof,
	token_type_null,
	token_type_true,
	token_type_return,
	token_type_false,
	token_type_using,

	token_type_add,			// ���������	+
	token_type_sub,			//	-
	token_type_mul,			// *
	token_type_div,			// /
	token_type_mod,			// %
	token_type_ass,			// =
	token_type_lbra,		// (
	token_type_rbra,		// )
	token_type_sadd,		// ++
	token_type_ssub,		// --
	token_type_adds,		// +=
	token_type_subs,		// -=

	token_type_and,			// �߼������	&&
	token_type_or,			// ||
	token_type_big,			// >
	token_type_lit,			// <
	token_type_bige,		// >=
	token_type_lite,		// <=
	token_type_equ,			// ==
	token_type_nequ,		// !=
	token_type_comma,		// ,

	token_type_end,				// �ս������
	token_type_lseg,			// ���������	{
	token_type_rseg,			// ���������	}
	token_type_ide,				// ��ʶ������
	token_type_num,				// ����������
	token_type_string,			// �ַ�������
	token_type_lal,				// [ ����
	token_type_ral,				// ] ����
	token_type_count			// ���͸���
};


extern char *_token_type_str[];

#define alex_get_type(type_index)		( (type_index>=token_type_count || type_index<=0)?(_token_type_str[0]):(_token_type_str[type_index]) )



typedef union _u_value
{
	ALEX_NUMBER		number;
	ALEX_STRING		str;
	ALEX_NAME		name;
	byte			op_t;
}u_value;

// Token����
typedef  struct _token
{
	a_string token_name;				// �ֶε�����
	byte token_type;					// �ֶε�����
	u_value token_value;				// �ֶ�ֵ
	int token_line;						// token ���ڵ��к�
}token;

// ���ɵ�token�ڵ�
typedef struct _token_node
{
	token tk;
	struct _token_node* next;
}token_node;



typedef struct _token_list{
	token_node* token_head;		// token_listͷ���
	token_node* token_end;		// token_listβ���ڵ�
	token_node* token_read;		// token_list ��ȡָ��
}token_list;



extern token_list t_l;			// �ʷ��������ɵ�token list


#define token_list_head		(t_l.token_head)

int alex_read_source_code(const char* source_code);
void alex_get_token(code_buff c_bf);
void add_token();					// ����ǰ���ڽ�����token
token get_num_token(code_buff* c_bf);
token get_ide_token(code_buff* c_bf);
token  get_string_token(code_buff* c_bf);
token get_oper_token(code_buff* c_bf);
token get_seg_token(code_buff* c_bf);
token get_al_token(code_buff* c_bf);
token  get_end_token(code_buff* c_bf);
void free_token_list(token_list* t_lp);
void print_token();

#define alex_load_file(f)	alex_read_source_code(f)

#endif