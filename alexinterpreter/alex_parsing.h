#ifndef	_ALEX_PARSING_H_
#define _ALEX_PARSING_H_
#include "alex_get_token.h"

typedef enum _bnf_type{
	bnf_type_ass,		// ��ֵ
	bnf_type_logic,		// �߼�
	bnf_type_value,		// ��ֵ
	bnf_type_var,		// ����
	bnf_type_const,		// ������ֵ
	bnf_type_string,	// �����ַ���
	bnf_type_al,		// ����

	bnf_type_aldef,		// ���鶨��
	bnf_type_vardef,	// �������
	bnf_type_funccall,	// ��������
	bnf_type_func,		// ����
	bnf_type_argdef,	// �����б�����
	bnf_type_argcal,	// �����б����
	bnf_type_exp,		// ���ʽ
	bnf_type_seg,		// ��
	bnf_type_return,	// return
	bnf_type_using,
	bnf_type_break,
	bnf_type_continue,
	bnf_type_if,
	bnf_type_else,
	bnf_type_while,

	bnf_type_count
}bnf_type;


extern char* bnf_type_str[];


// BNF tree
typedef struct _tree_node{
	bnf_type b_t;		// tree_node ����
	u_value  b_v;		// value
	int line;			// line

	struct _tree_node* next;		//  next p
	struct _tree_node* childs_p[3];	// child  p
}tree_node;


// ִ��tree���
typedef struct _main_tree{
	tree_node* head;
	tree_node* end;
}main_tree;

extern byte error_flag;
extern main_tree m_tree;
extern main_tree f_tree;


#define string_bnf(inx)		((inx>=bnf_type_count || inx <0)?("bnf_type_error"):(bnf_type_str[inx]) )

// ��ȡ��ǰtoken
#define at_token(t_lt)	( ( (t_lt)==NULL)?(NULL):(&((t_lt)->token_read->tk)) )
// ��õ�ǰ�к�
#define get_line(t_lt)	( ( (t_lt)==NULL)?(0):((int)((t_lt)->token_read->tk.token_line)) )

// ��ȡ��һ��token,ͬʱ����ָ��ָ����һ��token
#define next_token(t_lt)	((t_lt)->token_read=(t_lt)->token_read->next,  at_token(t_lt))
// �鿴��һ��token����
#define look_token(t_lt)	( ((t_lt)->token_read->next)?((t_lt)->token_read->next->tk.token_type):(0) )
// �鿴��һ��token����
#define look2_token(t_lt)	( ((t_lt)->token_read->next && ((t_lt)->token_read->next->next)?((t_lt)->token_read->next->->next->tk.token_type):(0) )
// ��õ�ǰtoken����
#define type_token(t_lt)	( ((t_lt)->token_read)?((t_lt)->token_read->tk.token_type):(0) )
#define end_token(t_lt)		(!((t_lt)->token_read))

// ��õ�ǰtree����
#define  type_tree(t_n)		(t_n->b_t)
#define  add_main(t_n)		add_tree(&m_tree, (t_n))
#define  add_func(t_n)		add_tree(&f_tree, (t_n))

int syn_watch(token_list* t_lt,  enum _token_type t_t);
tree_node* add_tree(main_tree* m_t, tree_node* t_n); 
tree_node* syn_exp_stmt(token_list* t_lt);
tree_node* syn_exp_def(token_list* t_lt);
tree_node* syn_logic_exp(token_list* t_lt);
tree_node* syn_relop_exp(token_list* t_lt);
tree_node* syn_add_exp(token_list* t_lt);
tree_node* syn_term_exp(token_list* t_lt);
tree_node* syn_factor_exp(token_list* t_lt);
tree_node* syn_func_call(token_list* t_lt);
token* look_next_token(token_list* t_lt);
tree_node* new_tree_node(int line, bnf_type b_t);
void free_tree(tree_node* tree_head);
tree_node* alex_parsing(token_list* t_lt);
void print_tree(tree_node* tree_head);
tree_node* syn_vardef_def(token_list* t_lt);
tree_node* syn_var_def(token_list* t_lt);
tree_node* syn_func_def(token_list* t_lt);
tree_node* syn_arg_def(token_list* t_lt);
tree_node* syn_seg_def(token_list* t_lt);
tree_node* syn_if_def(token_list* t_lt);
tree_node* syn_return_def(token_list* t_lt);
tree_node* syn_while_def(token_list* t_lt);
tree_node* syn_continue_def(token_list* t_lt);
tree_node* syn_break_def(token_list* t_lt);
tree_node* syn_al_def(token_list* t_lt);
tree_node* syn_at_al(token_list* t_lt);
tree_node* syn_using(token_list* t_lt);

#endif