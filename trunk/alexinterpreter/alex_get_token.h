#ifndef _ALEX_GET_TOKEN_H_
#define _ALEX_GET_TOKEN_H_
#include "alex_conf.h"
#include "alex_symbol_table.h"
#include "alex_string.h"

// 枚举类型数据
enum _token_type{
	token_type_err,				// 错误的token类型
	
	token_type_var,				// 关键字类型
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

	token_type_add,			// 运算符类型	+
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

	token_type_and,			// 逻辑运算符	&&
	token_type_or,			// ||
	token_type_big,			// >
	token_type_lit,			// <
	token_type_bige,		// >=
	token_type_lite,		// <=
	token_type_equ,			// ==
	token_type_nequ,		// !=
	token_type_comma,		// ,

	token_type_end,				// 终结符类型
	token_type_lseg,			// 区域运算符	{
	token_type_rseg,			// 区域运算符	}
	token_type_ide,				// 标识符类型
	token_type_num,				// 立即数类型
	token_type_string,			// 字符串类型
	token_type_lal,				// [ 数组
	token_type_ral,				// ] 数组
	token_type_count			// 类型个数
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

// Token数据
typedef  struct _token
{
	a_string token_name;				// 字段的名称
	byte token_type;					// 字段的类型
	u_value token_value;				// 字段值
	int token_line;						// token 所在的行号
}token;

// 生成的token节点
typedef struct _token_node
{
	token tk;
	struct _token_node* next;
}token_node;



typedef struct _token_list{
	token_node* token_head;		// token_list头结点
	token_node* token_end;		// token_list尾部节点
	token_node* token_read;		// token_list 读取指针
}token_list;



extern token_list t_l;			// 词法解析生成的token list


#define token_list_head		(t_l.token_head)

int alex_read_source_code(const char* source_code);
void alex_get_token(code_buff c_bf);
void add_token();					// 将当前正在解析的token
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