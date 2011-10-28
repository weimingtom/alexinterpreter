#include "alex_parsing.h"
#include "alex_get_token.h"
#include "alex_string.h"
#include "alex_log.h"
#include "stdlib.h"
#include "string.h"
#include "alex_sym.h"
#include "alex_com.h"


// 语法解析 根据BNF生成相应的TREE结构

main_tree m_tree = {0};		// 执行代码链表tree
main_tree f_tree = {0};		// 函数链表tree

byte error_flag =0;
char* bnf_type_str[] = {
	"bnf_type_ass",	
	"bnf_type_logic",	
	"bnf_type_value",	
	"bnf_type_var",	
	"bnf_type_const",	
	"bnf_type_string",
	
	"bnf_type_vardef",
	"bnf_type_funccall",
	"bnf_type_func",	
	"bnf_type_argdef",
	"bnf_type_argcal",
	"bnf_type_exp",	
	"bnf_type_seg",	
	"bnf_type_return",
	"bnf_type_break",
	"bnf_type_continue",
	"bnf_type_if",
	"bnf_type_else",
	"bnf_type_while"
};

tree_node* add_tree(main_tree* m_t, tree_node* t_n)
{
	if(t_n == NULL || m_t==NULL)
		return NULL;

	if(m_t->head == NULL)
	{
		m_t->head = t_n;
	}
	if(m_t->end == NULL)
	{
		m_t->end = t_n;
	}

	if(m_t->end != t_n)
	{
		m_t->end->next = t_n;
		m_t->end = t_n;
	}

	return t_n;
}



tree_node* alex_parsing(token_list* t_lt)
{
	token* tk_p = NULL;
	tree_node* r_tn = NULL;

	if(t_lt == NULL)
		return NULL;	
	
	while(tk_p=at_token(t_lt))
	{
		switch(tk_p->token_type)
		{
		case token_type_var:		// 定义变量
			{
				r_tn = syn_var_def(t_lt);
				if(r_tn)
					add_main(r_tn);
				else
					return NULL;
			}
			break;
		case token_type_ide:
			{
				r_tn = syn_exp_stmt(t_lt);
				if(r_tn)
					add_main(r_tn);
				else
					return NULL;
			}
			break;
		case token_type_func:		// 定义函数
			{
				r_tn = syn_func_def(t_lt);
				if(r_tn)
				{
					if(look_table(&global_table, r_tn->b_v.name))
					{
						st* r_st = add_g_table(new_func_st(r_tn->b_v.name.s_ptr, r_tn));
						add_func(r_tn);
					}
					else
					{
						free_tree(r_tn);
						print("parsing[erro line %d]: the function %s is redef!\n", tk_p->token_line, r_tn->b_v.name.s_ptr);
						return NULL;
					}
				}
				else
					return NULL;
			}
			break;
		default:
			{
				 print("parsing[error line %d]: not allow token \"%s\" at globle code seg!\n", tk_p->token_line, tk_p->token_name);
				 next_token(t_lt);
			}
			free_tree(r_tn);
			return NULL;
		}
	}
	
	return r_tn;
}


// 查看当前token name
int syn_watch(token_list* t_lt,  enum _token_type t_t)
{
	token* n_t = at_token(t_lt);
	if(t_lt==NULL ||  n_t == NULL || n_t->token_type == token_type_err)
	{
		print("syn_watch[error]: not find type[%d] syn!\n", t_t);
		error_flag = 1;
		return token_type_err;
	}

	if(n_t->token_type != t_t)
	{
		print("syn_watch[error line %d]: not same, look at \"%s\" \n", get_line(t_lt), n_t->token_name.s_ptr);
		error_flag = 1;
		return token_type_err;
	}
	else
	{
		next_token(t_lt);
		return n_t->token_type;
	}
}



// 定义变量
tree_node* syn_var_def(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_vardef);
	tree_node** t_rt_n = &(rt_n->childs_p[0]);
	syn_watch(t_lt, token_type_var);			// check key "var"
	
	do 										
	{
		tree_node* n_t = syn_vardef_def(t_lt);
		if(n_t)
		{
			*t_rt_n = n_t;
			t_rt_n = &((*t_rt_n)->next);
		}
		else
		{
			free_tree(rt_n);
			return NULL;
		}

		if(type_token(t_lt) != token_type_comma)
			break;

		next_token(t_lt);
	}while(1);	// if , while
	
	syn_watch(t_lt, token_type_end);

	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}
	return rt_n;
}


// 定义变量格式
tree_node* syn_vardef_def(token_list* t_lt)
{
	tree_node* rt_n = NULL;

	if(type_token(t_lt) != token_type_ide)	
	{
		token* n_t = at_token(t_lt);
		if(n_t)
			print("vardef[error line %d]: token \"%s\" is not var def!\n", n_t->token_line, n_t->token_name.s_ptr);
		else
			print("vardef[error line %d]: not find ide, error vardef format!\n", n_t->token_line);

		return  NULL;
	}

	switch(look_token(t_lt))
	{
	case token_type_ass:			// =
	case token_type_comma:			// ,
	case token_type_end:			// ;
		{
			rt_n = syn_exp_def(t_lt);
			if(rt_n == NULL)
				return NULL;
		}
		break;
	default:
		{
			token* nn_t = look_next_token(t_lt);
			if(nn_t)
				print("vardef[error line %d]: the token \"%s\" is not var def!\n", nn_t->token_line, nn_t->token_name);
			else
				print("vardef[error line %d]: not find \";\"!\n", nn_t->token_line);
		}
		return NULL;
	}

	return rt_n;
}


// function def 
tree_node* syn_func_def(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_func);
	
	syn_watch(t_lt, token_type_func);				// key function
	if(type_token(t_lt) == token_type_ide)			// ide
	{
		token* n_t = at_token(t_lt);
		if(look_table(global_table, n_t->token_name.s_ptr))
		{
			print("parsing[error line %d] the func redef!\n", n_t->token_line);
			free_tree(rt_n);
			return NULL;
		}
		else
		{
			rt_n->b_v.name = alex_string(n_t->token_name.s_ptr);
		}
	}
	else
	{
		print("parsing[error line %d] can not find function name!\n", get_line(t_lt));
		free_tree(rt_n);
		return NULL;
	}
	syn_watch(t_lt, token_type_ide);		// watch func name
	syn_watch(t_lt, token_type_lbra);		// watch (
	rt_n->childs_p[0] = syn_arg_def(t_lt);	// get arg list
	syn_watch(t_lt, token_type_rbra);		// watch )
	rt_n->childs_p[1] = syn_seg_def(t_lt);
											// rt_n->childs_p[2] return vale
	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}

	return rt_n;
}

// arg list def
tree_node* syn_arg_def(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_argdef);
	tree_node** nrt_n = &(rt_n->childs_p[0]);
	

	do 
	{
		token* n_t = NULL;
		if(type_token(t_lt) == token_type_var)
		{
			syn_watch(t_lt, token_type_var);
		}
		else
		{
			free(rt_n);
			rt_n = NULL;
			break;
		}

		n_t = at_token(t_lt);
		if(type_token(t_lt) == token_type_ide)
		{
			(rt_n->b_v.number)++;
			*nrt_n = new_tree_node(get_line(t_lt), bnf_type_var);
			(*nrt_n)->b_v.name = alex_string(n_t->token_name.s_ptr);
			nrt_n = &((*nrt_n)->next);
		}
		syn_watch(t_lt, token_type_ide);
		
		if(type_token(t_lt) != token_type_comma)
			break;
		else
			syn_watch(t_lt, token_type_comma);
	} while (1);

	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}

	return rt_n;
}


// segment def
tree_node* syn_seg_def(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_seg);
	tree_node** nrt_n = &rt_n->childs_p[0];
	byte seg_flag = 0;
	
	if(type_token(t_lt) == token_type_lseg)
	{
		seg_flag = 1;
		syn_watch(t_lt, token_type_lseg);
	}

	do{
		switch(type_token(t_lt))
		{
		case token_type_lseg:				// if {
			{
				syn_watch(t_lt, token_type_lseg);		// watch {
				*nrt_n = syn_seg_def(t_lt);
				 nrt_n = &((*nrt_n)->next);
				 syn_watch(t_lt, token_type_rseg);		// watch }
			}
			break;
		case token_type_var:
			{
				*nrt_n = syn_var_def(t_lt);
				nrt_n = &((*nrt_n)->next);
			}
			break;
		case token_type_if:
			{
				*nrt_n = syn_if_def(t_lt);
				nrt_n = &((*nrt_n)->next);
			}
			break;
		case token_type_while:
			{
				*nrt_n = syn_while_def(t_lt);
				nrt_n = &((*nrt_n)->next);
			}
			break;
		case token_type_continue:
			{
				*nrt_n = syn_continue_def(t_lt);
				nrt_n = &((*nrt_n)->next);
			}
			break;
		case token_type_break:
			{
				*nrt_n = syn_break_def(t_lt);
				nrt_n = &((*nrt_n)->next);
			}
			break;
		case token_type_return:
			{
				*nrt_n = syn_return_def(t_lt);
				nrt_n = &((*nrt_n)->next);
			}
			break;
		case token_type_rseg:
			break;
		default:
			{
				*nrt_n = syn_exp_stmt(t_lt);
				nrt_n = &((*nrt_n)->next);
			}
			break;
		}
	}while( seg_flag && type_token(t_lt) != token_type_rseg);			//  if not find { is loop

	if(seg_flag)
		syn_watch(t_lt, token_type_rseg);
	
	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}
	return rt_n;
}


// while
tree_node* syn_while_def(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_while);
	syn_watch(t_lt, token_type_while);		// watch while
	syn_watch(t_lt, token_type_lbra);		// watch (
	
	rt_n->childs_p[0] = syn_exp_def(t_lt);	// while 条件
	syn_watch(t_lt, token_type_rbra);		// watch )

	rt_n->childs_p[1] = syn_seg_def(t_lt);	// while seg
	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}
	return rt_n;
}


// if
tree_node* syn_if_def(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_if);
	syn_watch(t_lt, token_type_if);		// watch if
	syn_watch(t_lt, token_type_lbra);	// watch (
	
	rt_n->childs_p[0] = syn_exp_def(t_lt);
	syn_watch(t_lt, token_type_rbra);	// watch )
	rt_n->childs_p[1] = syn_seg_def(t_lt);			// if seg
	
	if(type_token(t_lt) == token_type_else)		// find else
	{
		syn_watch(t_lt, token_type_else);
		rt_n->childs_p[2] = syn_seg_def(t_lt);
	}

	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}
	return rt_n;
}


// break
tree_node* syn_break_def(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_break);
	
	syn_watch(t_lt, token_type_break);
	syn_watch(t_lt, token_type_end);

	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}
	return rt_n;
}


// continue
tree_node* syn_continue_def(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_continue);

	syn_watch(t_lt, token_type_continue);
	syn_watch(t_lt, token_type_end);

	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}
	return rt_n;
}

// return 
tree_node* syn_return_def(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_return);
	tree_node** nrt_n = &(rt_n->childs_p[0]);
	syn_watch(t_lt, token_type_return);

	if(type_token(t_lt)==token_type_end)
		goto RETURN_NED;
	do 
	{
		*nrt_n = syn_exp_def(t_lt);
		nrt_n = &((*nrt_n)->next);

		if(type_token(t_lt) != token_type_comma)
			break;
		else
			syn_watch(t_lt, token_type_comma);
	} while (1);

RETURN_NED:
	syn_watch(t_lt, token_type_end);
	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}

	return rt_n;
}

// 表达式	;结尾
tree_node* syn_exp_stmt(token_list* t_lt)
{
	tree_node* rt_n = syn_exp_def(t_lt);
	if(rt_n)
		syn_watch(t_lt, token_type_end);		// watch ;

	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}
	return rt_n;
}



// 表达式
tree_node* syn_exp_def(token_list* t_lt)
{
	tree_node* op_l = syn_logic_exp(t_lt);	// 左操作数
	tree_node* rt_n = op_l;

	if(op_l == NULL)
		return NULL;

	while(type_token(t_lt) == token_type_ass)		// 赋值
	{
		rt_n = new_tree_node(get_line(t_lt), bnf_type_ass);
		rt_n->childs_p[0] = op_l;
		rt_n->b_v.op_t = token_type_ass;

		next_token(t_lt);
		rt_n->childs_p[1] = syn_logic_exp(t_lt);
		if(rt_n->childs_p[1] == NULL)
		{
			free(rt_n);
			return NULL;
		}
		op_l=rt_n;					// 迭代解析
	}
	
	return rt_n;
}


// 逻辑表达式
tree_node* syn_logic_exp(token_list* t_lt)
{
	tree_node* op_l = syn_relop_exp(t_lt);
	tree_node* rt_n = op_l;

	if(op_l == NULL)
		return NULL;

	while( (type_token(t_lt)==token_type_or)  ||  (type_token(t_lt)==token_type_and) )		// watch || &&
	{
		rt_n = new_tree_node(get_line(t_lt), bnf_type_logic);
		rt_n->childs_p[0] = op_l;
		rt_n->b_v.op_t = type_token(t_lt);

		next_token(t_lt);
		rt_n->childs_p[1] = syn_relop_exp(t_lt);
		if(rt_n->childs_p[1]==NULL)
		{
			free_tree(rt_n);
			return NULL;
		}
		op_l = rt_n;
	}
	return rt_n;
}


// 
tree_node* syn_relop_exp(token_list* t_lt)
{
	tree_node* op_l = syn_add_exp(t_lt);
	tree_node* rt_n = op_l;

	if(op_l == NULL)
		return NULL;

	while( type_token(t_lt)==token_type_big || type_token(t_lt)==token_type_bige ||
		   type_token(t_lt)==token_type_lit || type_token(t_lt)==token_type_lite ||
		   type_token(t_lt)==token_type_equ || type_token(t_lt)==token_type_nequ
		 )
	{
		rt_n = new_tree_node(get_line(t_lt), bnf_type_logic);
		rt_n->childs_p[0]=op_l;
		rt_n->b_v.op_t = type_token(t_lt);

		next_token(t_lt);
		rt_n->childs_p[1] = syn_add_exp(t_lt);
		if(rt_n->childs_p[1] == NULL)
		{
			free_tree(rt_n);
			return NULL;
		}
		op_l = rt_n;
	}

	return rt_n;
}



// + - 运算
tree_node* syn_add_exp(token_list* t_lt)
{
	tree_node* op_l = syn_term_exp(t_lt);
	tree_node* rt_n = op_l;

	if(op_l == NULL)
		return NULL;

	while(type_token(t_lt)==token_type_add || type_token(t_lt)==token_type_sub)
	{
		rt_n = new_tree_node(get_line(t_lt), bnf_type_value);
		rt_n->childs_p[0]=op_l;
		rt_n->b_v.op_t = type_token(t_lt);

		next_token(t_lt);
		rt_n->childs_p[1] = syn_term_exp(t_lt);
		if(rt_n->childs_p[1] == NULL)
		{
			free_tree(rt_n);
			return NULL;
		}
		op_l = rt_n;
	}

	return rt_n;
}



// * / %
tree_node* syn_term_exp(token_list* t_lt)
{
	tree_node* op_l = syn_factor_exp(t_lt);
	tree_node* rt_n = op_l;

	if(op_l == NULL)
		return NULL;

	while(type_token(t_lt)==token_type_mul || type_token(t_lt)==token_type_div || 
		  type_token(t_lt)==token_type_mod
		 )
	{
		rt_n = new_tree_node(get_line(t_lt), bnf_type_value);
		rt_n->childs_p[0]=op_l;
		rt_n->b_v.op_t = type_token(t_lt);

		next_token(t_lt);
		rt_n->childs_p[1] = syn_factor_exp(t_lt);
		if(rt_n->childs_p[1] == NULL)
		{
			free_tree(rt_n);
			return NULL;
		}
		op_l = rt_n;
	}

	return rt_n;
}


// ( const value  var value
tree_node* syn_factor_exp(token_list* t_lt)
{
	tree_node* rt_n = NULL;
	token* n_t = NULL;

	switch(type_token(t_lt))
	{
	case token_type_lbra:		// (
		{
			syn_watch(t_lt, token_type_lbra);		// jump (
			rt_n = syn_exp_def(t_lt);
			syn_watch(t_lt, token_type_rbra);		// jump )
		}
		break;
	case token_type_lal:
		{
			syn_watch(t_lt, token_type_lal);		// jump [
			rt_n = syn_al_def(t_lt);
			syn_watch(t_lt, token_type_ral);		// jump ]
		}
		break;
	case token_type_ide:		// var ide
		{
			switch(look_token(t_lt))		// if token=='(' func call
			{
			case token_type_lbra:
				{
					rt_n = syn_func_call(t_lt);	
				}
				break;
			case token_type_lal:			// arraylist
				{
					rt_n = syn_at_al(t_lt);
				}
				break;
			default:										// var 
				{				
					token* n_t = at_token(t_lt);
					rt_n = new_tree_node(get_line(t_lt), bnf_type_var);
					rt_n->b_v.name = alex_string(n_t->token_name.s_ptr);
					next_token(t_lt);
				}
				break;
			}
			
		}
		break;
	case token_type_num:		// const number
		{
			token* n_t = at_token(t_lt);
			rt_n = new_tree_node(get_line(t_lt), bnf_type_const);
			rt_n->b_v.number = n_t->token_value.number;
			next_token(t_lt);
		}
		break;
	case token_type_string:		// const string
		{
			token* n_t = at_token(t_lt);
			rt_n = new_tree_node(get_line(t_lt), bnf_type_string);
			rt_n->b_v.str = n_t->token_value.str;
			next_token(t_lt);
		}
		break;
		
	case token_type_sadd:		// ++number
	case token_type_ssub:		// --number
	case token_type_sub:		// -
		{
			rt_n = new_tree_node(get_line(t_lt), bnf_type_value);
			rt_n->b_v.op_t = type_token(t_lt);
			next_token(t_lt);
			rt_n->childs_p[1] = syn_factor_exp(t_lt);
		}
		break;
	default:
		{
			token* n_t = at_token(t_lt);
			next_token(t_lt);
			print("factor[error line %d]: token \"%s\" is not allow!\n", n_t->token_line, n_t->token_name.s_ptr);
		}
		return NULL;
	}

	if(type_token(t_lt) == token_type_sadd || type_token(t_lt) == token_type_ssub)		// if number--  number++
	{
		tree_node* n_rt_n = new_tree_node(get_line(t_lt), bnf_type_value);
		n_rt_n->b_v.op_t = type_token(t_lt);
		n_rt_n->childs_p[0] = rt_n;
		rt_n = n_rt_n;
		next_token(t_lt);
	}

	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}
	return rt_n;
}


tree_node* syn_al_def(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_aldef);
	tree_node** nrt_n = &(rt_n->childs_p[0]);
	token* n_t = NULL;
	

	while(type_token(t_lt)!=token_type_ral)
	{
		switch(type_token(t_lt))
		{
		case token_type_lal:
			syn_watch(t_lt, token_type_lal);
			*nrt_n = syn_al_def(t_lt);
			syn_watch(t_lt, token_type_ral);
			break;
		default:
			*nrt_n = syn_exp_def(t_lt);
			break;
		}
		nrt_n = &((*nrt_n)->next);
		
		n_t = at_token(t_lt);
		if(n_t->token_type==token_type_comma)
			syn_watch(t_lt, token_type_comma);
		else if(n_t->token_type != token_type_ral)
		{
			free_tree(rt_n);
			return NULL;
		}
	}

	return rt_n;
}


// arraylist[]
tree_node* syn_at_al(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_al);
	tree_node** nrt_n = &(rt_n->childs_p[0]);
	token* n_t = at_token(t_lt);
	rt_n->b_v.name = alex_string(n_t->token_name.s_ptr);

	syn_watch(t_lt, token_type_ide);

	do 
	{
		syn_watch(t_lt, token_type_lal);
		*nrt_n = syn_exp_def(t_lt);
		nrt_n = &((*nrt_n)->next);
		syn_watch(t_lt, token_type_ral);
	} while (type_token(t_lt)== token_type_lal);

	return rt_n;
}


// func call
tree_node* syn_func_call(token_list* t_lt)
{
	tree_node* rt_n = new_tree_node(get_line(t_lt), bnf_type_funccall);
	tree_node** nrt_n = &(rt_n->childs_p[0]);
	token* n_t = at_token(t_lt);
	rt_n->b_v.name = alex_string(n_t->token_name.s_ptr);

	syn_watch(t_lt, token_type_ide);
	syn_watch(t_lt, token_type_lbra);
	
	while(type_token(t_lt) != token_type_rbra) 
	{
		*nrt_n = syn_exp_def(t_lt);
		nrt_n = &((*nrt_n)->next);

		if(type_token(t_lt)==token_type_comma)
			syn_watch(t_lt, token_type_comma);
		else
			break;
	}
	
	syn_watch(t_lt, token_type_rbra);
	if(error_flag)
	{
		free_tree(rt_n);
		return NULL;
	}
	return	rt_n;
}


// 获取下一个token,读指针不会指向下一个token
token* look_next_token(token_list* t_lt)
{
	token_node* t_p = t_lt->token_read;
	
	if(t_p==NULL)
		return NULL;

	return &t_p->next->tk;
};


// 生成一个bnf tree node
tree_node* new_tree_node(int line, bnf_type b_t)
{
	tree_node* n_t_n = (tree_node*)malloc(sizeof(tree_node));
	memset(n_t_n, 0, sizeof(tree_node));

	n_t_n->b_t = b_t;
	n_t_n->line = line;
	return n_t_n;
}

void free_tree_node(tree_node* t_n)
{
	if(t_n == NULL)
		return;
	switch(t_n->b_t)
	{
	case bnf_type_string:
		free_string(&t_n->b_v.str);
		break;
	case  bnf_type_var:
		free_string(&t_n->b_v.name);
		break;
	case bnf_type_funccall:
		free_string(&t_n->b_v.name);
		break;
	}

	free(t_n);
}

void free_tree(tree_node* tree_head)
{
	tree_node* next_t_n = NULL;
	if(tree_head == NULL)
		return;

	next_t_n = tree_head->next;
	free_tree(tree_head->childs_p[0]);
	free_tree(tree_head->childs_p[1]);
	free_tree(tree_head->childs_p[2]);
	free_tree_node(tree_head);
	free_tree(next_t_n);
}



void print_tree(tree_node* tree_head)
{

	if(tree_head == NULL)
		return ;

	print("tree_addr[%p] type[%d] chiled_l[%p] child_c[%p] next[%p]", tree_head, tree_head->b_t, tree_head->childs_p[0], tree_head->childs_p[1], tree_head->next);
	if(tree_head->b_t == bnf_type_var)
		print(" value[%s]\n", tree_head->b_v.name);
	else if(tree_head->b_t == bnf_type_const)
		print(" value[%d]\n", (int)tree_head->b_v.number);
	else if(tree_head->b_t == bnf_type_func)
		print(" value[%s]\n", tree_head->b_v.name.s_ptr);
	else
		print("value[%s]\n", string_bnf(tree_head->b_t));

	
	print_tree(tree_head->childs_p[0]);
	print_tree(tree_head->childs_p[1]);
	print_tree(tree_head->next);
}
