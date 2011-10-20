#include "alex_interpret.h"
#include "alex_sym.h"
#include "alex_parsing.h"
#include "alex_log.h"
#include "alex_arrylist.h"
#include "alex_string.h"
#include "stdlib.h"
#include "alex_lib.h"
#include <memory.h>

// 语义分析执行, 将生成的tree 解析执行

ret_node* inter_vardef(sym_table* s_t, tree_node* t_n);
ret_node* inter_funccall(inter_env env, tree_node* t_n);
ret_node* inter_exp(inter_env env, tree_node* t_n);
ret_node* inter_op(inter_env env, tree_node* t_n);
st*  look_env(inter_env env, char* str);
ret_node* inter_arg(inter_env env, tree_node* t_n);
ret_node* inter_var(inter_env env, tree_node* n_t);
st ret_to_st(char* name, ret_node* rt_n);
ret_node* st_to_ret(st* l_st);
ret_node* inter_op_ass(inter_env env, tree_node* t_n);
ret_node* inter_op_value(inter_env env, tree_node* t_n);
ret_node* inter_op_logic(inter_env env, tree_node* t_n);
void free_ret(ret_node* rt_n);
ret_node* inter_aldef(sym_table* s_t, tree_node* t_n);
int tree_watch(tree_node* t_n, enum bnf_type  b_t);
ret_node* inter_return(inter_env env, tree_node* t_n);
ret_node* inter_if(inter_env env, tree_node* t_n);
ret_node* inter_while(inter_env env, tree_node* t_n);
ret_node* inter_al(inter_env env, tree_node* t_n);



void alex_interpret(inter_env env, tree_node* main_tree)
{
	if(main_tree == NULL)
		return;

	switch(type_tree(main_tree))
	{
	case bnf_type_vardef:
		{
			inter_vardef(env.g_table, main_tree);		// add to global_table
		}
		break;
	case bnf_type_funccall:
		{
			inter_funccall(env, main_tree);
		}
		break;
	default:
		{
			print("inter[error line %d]: the g_code not allow!\n", main_tree->line);
		}
		return;
	}

	alex_interpret(env, main_tree->next);
}


// get a new env
inter_env new_inter_env()
{
	inter_env rt_ie = {0};
	
	if(global_table == NULL)
		global_table = new_table();

	rt_ie.g_table = global_table;
	rt_ie.l_table = new_table();

	return rt_ie;
}

void free_inter_env(inter_env env)
{
	if(env.l_table)
		free_table(env.l_table);
}


ret_node* inter_funccall(inter_env env, tree_node* t_n)
{
	inter_env n_env = {0};

	ret_node* rt_n = NULL;
	st* ret_st = NULL;

	if( (ret_st=look_env(env, t_n->b_v.name.s_ptr)) && (ret_st->s_t==sym_type_func || ret_st->s_t==sym_type_reg_func) )
	{
		if(ret_st->s_t == sym_type_reg_func)
		{
			reg_func r_func = (reg_func)ret_st->s_v.func;
			ret_node* arg_list = inter_arg(env, t_n);
			rt_n = r_func(arg_list);
			free_ret(arg_list);
		}
		else
		{
			ret_node* arg_list  = inter_arg(env, t_n);
			n_env = new_inter_env();
			rt_n = inter_p_call(n_env, ret_st->s_v.func, arg_list);
			free_ret(arg_list);
		}
	}
	else
	{
		print("inter[error line %d]: not find func \"%s\"!\n", t_n->line, t_n->b_v.name.s_ptr);
	}

	free_inter_env(n_env);
	return rt_n;
}


ret_node* inter_arg(inter_env env, tree_node* t_n)
{
	ret_node* rt_n = NULL;
	ret_node** t_rt_n = &rt_n;
	tree_node* tn_p = t_n->childs_p[0];

	while(tn_p)
	{
		*t_rt_n = inter_exp(env, tn_p);
		if(check_ret(*t_rt_n, sym_type_alp))
		{
			*t_rt_n = al_ptov(*t_rt_n);
		}
		t_rt_n = &((*t_rt_n)->next);
		tn_p = tn_p->next;
	}
	return rt_n;
}



ret_node* inter_exp(inter_env env, tree_node* t_n)
{
	ret_node* rt_n = NULL;
	
	if(t_n==NULL)
		return NULL;

	switch(type_tree(t_n))
	{
	case bnf_type_vardef:
		{
			rt_n = inter_vardef(env.l_table, t_n);
		}
		break;
	case  bnf_type_const:
		{
			rt_n = (ret_node*)malloc(sizeof(ret_node));
			memset(rt_n, 0, sizeof(ret_node));
			rt_n->ret_value.r_t = sym_type_num;
			rt_n->ret_value.r_v.num = t_n->b_v.number;
		}
		break;
	case bnf_type_string:
		{
			rt_n = (ret_node*)malloc(sizeof(ret_node));
			memset(rt_n, 0, sizeof(ret_node));
			rt_n->ret_value.r_t = sym_type_string;
			rt_n->ret_value.r_v.str = alex_string(t_n->b_v.str.s_ptr);
		}
		break;
	case bnf_type_var:
		{
			rt_n = inter_var(env, t_n);
		}
		break;
	case  bnf_type_ass:
	case bnf_type_logic:
	case bnf_type_value:
		{
			rt_n = inter_op(env, t_n);
		}
		break;
	case bnf_type_aldef:
		{
			rt_n = inter_aldef(env.l_table, t_n);
		}
		break;
	case bnf_type_al:
		{
			rt_n = inter_al(env, t_n);
		}
		break;
	case  bnf_type_funccall:
		{
			rt_n = inter_funccall(env, t_n);
		}
		break;
	default:
		print("inter[error line %d]: tree_node \"%s\" not allow at exp!\n", t_n->line, string_bnf(t_n->b_t));
		return NULL;
	}

	return rt_n;
}


ret_node* inter_var(inter_env env, tree_node* n_t)
{
	st* r_st = NULL;

	if( (r_st=look_env(env, n_t->b_v.name.s_ptr))==0 )
	{
		print("inter[error line %d]: can not find ide \"%s\" !\n", n_t->line, n_t->b_v.name.s_ptr);
		return NULL;
	}
	else
	{
		ret_node* rt_n = (ret_node*)malloc(sizeof(ret_node));
		memset(rt_n, 0, sizeof(ret_node));
		rt_n->ret_value.r_t = r_st->s_t;
		if(rt_n->ret_value.r_t == sym_type_string)
			rt_n->ret_value.r_v.str = alex_string(r_st->s_v.str.s_ptr);
		else
			rt_n->ret_value.r_v = r_st->s_v; 
		
		return rt_n;
	}
}


// at  al
ret_node* inter_al(inter_env env, tree_node* t_n)
{
	r_value* r_v_p= NULL;
	ret_node* rt_n = NULL;
	tree_node* n_tn = NULL;
	st* at_al = look_env(env, t_n->b_v.name.s_ptr);
	if(at_al == NULL || at_al->s_t != sym_type_al)
	{
		print("inter[error line: %d] the %s al not find it!\n", t_n->line, t_n->b_v.name.s_ptr);
		return NULL;
	}

	rt_n = inter_exp(env, t_n->childs_p[0]);
	if(rt_n == NULL || check_ret(rt_n, sym_type_num)==0)
	{
		print("inter[error line: %d] the al index is error!\n", t_n->line);
		return NULL;
	}
	
	n_tn = t_n->childs_p[0]->next;
	r_v_p = get_al((at_al->s_v.al), (int)(rt_n->ret_value.r_v.num));
	free_ret(rt_n);

	do{
		if(r_v_p== NULL || r_v_p->r_t!= sym_type_al || n_tn == NULL)
			break;
		if(r_v_p->r_t == sym_type_al)
		{
			rt_n = inter_exp(env, n_tn);
			if(rt_n == NULL || check_ret(rt_n, sym_type_num)==0)
			{
				print("inter[error line: %d] the al index is error!\n", t_n->line);
				free_ret(rt_n);
				return NULL;
			}
			r_v_p = get_al(r_v_p->r_v.al, (int)(rt_n->ret_value.r_v.num));
			free_ret(rt_n);
		}
		else
		{
			print("inter[error line: %d] the al one is not al!\n", t_n->line);
			return NULL;
		}
		n_tn = n_tn->next;
	}while(n_tn);
	
	rt_n = new_ret_node(sym_type_alp);
	rt_n->ret_value.r_v.ptr = r_v_p;

	return rt_n;
}


// def  arraylist
ret_node* inter_aldef(sym_table* s_t, tree_node* t_n)
{
	ret_node* rt_n = new_ret_node(sym_type_al);
	ret_node* tt_n = NULL;
	alex_al* al = new_al();
	
	t_n = t_n->childs_p[0];
	
	while(t_n)
	{
		inter_env env = {0};
		env.g_table = global_table;
		env.l_table = s_t;

		tt_n = inter_exp(env, t_n);
			
		if(tt_n==NULL)
		{
			del_al(al);
			free_ret(rt_n);
			return NULL;
		}
		(check_ret(tt_n, sym_type_al))?(tt_n->ret_value.r_v.al->count++):(0);
		tt_n = (check_ret(tt_n, sym_type_alp))?(al_ptov(tt_n)):(tt_n);
		add_al(al, tt_n->ret_value);
		free_ret(tt_n);
		t_n = t_n->next;
	}

	rt_n->ret_value.r_v.al = al;
	return rt_n;
}



// 定义变量解析
ret_node* inter_vardef(sym_table* s_t, tree_node* t_n)
{
	t_n = t_n->childs_p[0];

	while(t_n)
	{
		switch(type_tree(t_n))
		{
		case bnf_type_var:
			add_table(s_t, new_num_st(t_n->b_v.name.s_ptr, 0));
			break;
		case bnf_type_ass:
			{
				inter_env env = {0};
				ret_node* rt_n = NULL;
				env.g_table = global_table;
				env.l_table = s_t;

				add_new_table(s_t, t_n->childs_p[0]->b_v.name.s_ptr);
				rt_n = inter_exp(env, t_n);
				
				if(rt_n == NULL)
					return NULL;

	//			add_table(s_t, ret_to_st(t_n->childs_p[0]->b_v.name.s_ptr, rt_n));
				free_ret(rt_n);
			}
			break;
		default:
			print("inter[error line %d] at var def the  tree_node \"%s\" no allow!\n", t_n->line, string_bnf(t_n->b_t));
			return NULL;
		}

		t_n = t_n->next;
	}

	return NULL;
}



ret_node* inter_op(inter_env env, tree_node* t_n)
{
	ret_node* rt_n = NULL;
	switch(type_tree(t_n))
	{
	case bnf_type_ass:
		rt_n = inter_op_ass(env, t_n);
		break;
	case bnf_type_value:
		rt_n = inter_op_value(env, t_n);
		break;
	case bnf_type_logic:
		rt_n = inter_op_logic(env, t_n);
		break;
	default:
		print("inter[error line %d] the tree node \"\" is not op sym!\n", t_n->line, string_bnf(t_n->b_t));
		break;
	}

	return rt_n;
}


// 二元操作符
ret_node* inter_op_tp(inter_env env, tree_node* t_n, byte t_t)
{
	ret_node* rt_n = NULL;
	ret_node* l_rt = inter_exp(env, t_n->childs_p[0]);
	ret_node* r_rt = inter_exp(env, t_n->childs_p[1]);

	if(l_rt == NULL || r_rt== NULL)
		return NULL;

	if( 
		check_ret(l_rt, sym_type_func) || check_ret(r_rt, sym_type_func) ||
		check_ret(l_rt, sym_type_al)   || check_ret(r_rt, sym_type_al)
	  )
	{
		print("inter[error line %d]: the type can not is oper!\n", t_n->line);
		return NULL;
	}

	l_rt = (check_ret(l_rt, sym_type_alp))?(al_ptov(l_rt)):(l_rt);
	r_rt = (check_ret(r_rt, sym_type_alp))?(al_ptov(r_rt)):(r_rt);

	switch(t_t)
	{
	case  token_type_add:
		{
			if(l_rt->ret_value.r_t == sym_type_string)
			{
				rt_n = new_ret_node(sym_type_string);
				rt_n->ret_value.r_v.str = alex_string(l_rt->ret_value.r_v.str.s_ptr);
				if(r_rt->ret_value.r_t == sym_type_string)
				{	
					cat_a_string(&rt_n->ret_value.r_v.str, r_rt->ret_value.r_v.str);
				}
				else
				{
					char temp[256] = {0};
					gcvt(r_rt->ret_value.r_v.num, 10, temp);
					cat_string(&rt_n->ret_value.r_v.str, temp);
				}
			}
			else
			{
				if(r_rt->ret_value.r_t == sym_type_string)
				{
					char temp[256] = {0};
					gcvt(l_rt->ret_value.r_v.num, 10, temp);
					rt_n = new_ret_node(sym_type_string);
					rt_n->ret_value.r_v.str = alex_string(temp);
					cat_a_string(&rt_n->ret_value.r_v.str, r_rt->ret_value.r_v.str);
				}
				else
				{
					rt_n = new_ret_node(sym_type_num);
					rt_n->ret_value.r_v.num = l_rt->ret_value.r_v.num + r_rt->ret_value.r_v.num;
				}
			}
		}
		break;
	case token_type_mod:
		{
			if(l_rt->ret_value.r_t==sym_type_num && r_rt->ret_value.r_t==sym_type_num)
			{
				rt_n = new_ret_node(sym_type_num);
				rt_n->ret_value.r_v.num = (int)(l_rt->ret_value.r_v.num) % (int)(r_rt->ret_value.r_v.num);
			}
			else
				print("inter[error line %d] op value not allow!\n", t_n->line);
		}
		break;
	case token_type_sub:
		{
			if(l_rt->ret_value.r_t==sym_type_num && r_rt->ret_value.r_t==sym_type_num)
			{
				rt_n = new_ret_node(sym_type_num);
				rt_n->ret_value.r_v.num = l_rt->ret_value.r_v.num - r_rt->ret_value.r_v.num;
			}
			else
				print("inter[error line %d] op value not allow!\n", t_n->line);
		}
		break;
	case token_type_mul:
		{
			if(l_rt->ret_value.r_t==sym_type_num && r_rt->ret_value.r_t==sym_type_num)
			{
				rt_n = new_ret_node(sym_type_num);
				rt_n->ret_value.r_v.num = l_rt->ret_value.r_v.num * r_rt->ret_value.r_v.num;
			}
			else
				print("inter[error line %d] op value not allow!\n", t_n->line);
		}
		break;
	case token_type_div:
		{
			if(l_rt->ret_value.r_t==sym_type_num && r_rt->ret_value.r_t==sym_type_num)
			{
				rt_n = new_ret_node(sym_type_num);
				rt_n->ret_value.r_v.num = l_rt->ret_value.r_v.num / r_rt->ret_value.r_v.num;
			}
			else
				print("inter[error line %d] op value not allow!\n", t_n->line);
		}
		break;
	}

	free_ret(l_rt);
	free_ret(r_rt);

	return rt_n;

}

// 一元运算符
ret_node* inter_op_op(inter_env env, tree_node* t_n, byte t_t)
{
	ret_node* rt_n = NULL;
	ret_node* l_rt = (t_n->childs_p[0])?(inter_exp(env, t_n->childs_p[0])):(NULL);		// 前置 
	ret_node* r_rt = (t_n->childs_p[1])?(inter_exp(env, t_n->childs_p[1])):(NULL);		// 后置

	if(l_rt==NULL && r_rt==NULL)
	{
		print("inter[error]: not find one oper!\n");
		return NULL;
	}

	switch(t_t)
	{
	case token_type_sadd:
		{
			if(l_rt==NULL)
			{
				if(t_n->childs_p[1]->b_t == bnf_type_var)
				{
					st* p_st = look_env(env, t_n->childs_p[1]->b_v.name.s_ptr);
					if(p_st->s_t == sym_type_num)
					{
						p_st->s_v.num +=1;
						rt_n = new_ret_node(sym_type_num);
						rt_n->ret_value.r_v = p_st->s_v;
					}
					else
						print("inter[error line %d]: the oper value is not num!\n", t_n->line);
				}
				else if(t_n->childs_p[1]->b_t == bnf_type_const)
				{
					rt_n = new_ret_node(sym_type_num);
					rt_n->ret_value.r_v.num = t_n->childs_p[1]->b_v.number+1;
				}


				if(check_ret(r_rt, sym_type_alp))
				{
					r_value* r_p = r_rt->ret_value.r_v.ptr;
					if(r_p->r_t != sym_type_num)
						print("inter[error line %d]: the oper value is not num!\n", t_n->line);
					else
						(r_p->r_v.num)++;
				}

			}
			else
			{
				if(t_n->childs_p[0]->b_t == bnf_type_var)
				{
					st* p_st = look_env(env, t_n->childs_p[0]->b_v.name.s_ptr);
					if(p_st->s_t == sym_type_num)
					{
						rt_n = new_ret_node(sym_type_num);
						rt_n->ret_value.r_v = p_st->s_v;
						p_st->s_v.num +=1;
					}
					else
						print("inter[error line %d]: the oper value is not num!\n", t_n->line);
				}
				else if(t_n->childs_p[0]->b_t == bnf_type_const)
				{
					rt_n = new_ret_node(sym_type_num);
					rt_n->ret_value.r_v.num = t_n->childs_p[0]->b_v.number;
				}

				
				if(check_ret(l_rt, sym_type_alp))
				{
					r_value* r_p = l_rt->ret_value.r_v.ptr;
					if(r_p->r_t != sym_type_num)
						print("inter[error line %d]: the oper value is not num!\n", t_n->line);
					else
						(r_p->r_v.num)++;
				}
			}
		}
		break;
	case  token_type_ssub:
		{
			if(l_rt==NULL)
			{
				if(t_n->childs_p[1]->b_t == bnf_type_var)
				{
					st* p_st = look_env(env, t_n->childs_p[1]->b_v.name.s_ptr);
					if(p_st->s_t == sym_type_num)
					{
						p_st->s_v.num -=1;
						rt_n = new_ret_node(sym_type_num);
						rt_n->ret_value.r_v = p_st->s_v;
					}
					else
						print("inter[error line %d]: the oper value is not num!\n", t_n->line);
				}
				else if(t_n->childs_p[1]->b_t == bnf_type_const)
				{
					rt_n = new_ret_node(sym_type_num);
					rt_n->ret_value.r_v.num = t_n->childs_p[1]->b_v.number-1;
				}

				if(check_ret(r_rt, sym_type_alp))
				{
					r_value* r_p = r_rt->ret_value.r_v.ptr;
					if(r_p->r_t != sym_type_num)
						print("inter[error line %d]: the oper value is not num!\n", t_n->line);
					else
						(r_p->r_v.num)--;
				}

			}
			else
			{
				if(t_n->childs_p[0]->b_t == bnf_type_var)
				{
					st* p_st = look_env(env, t_n->childs_p[0]->b_v.name.s_ptr);
					if(p_st->s_t == sym_type_num)
					{
						rt_n = new_ret_node(sym_type_num);
						rt_n->ret_value.r_v = p_st->s_v;
						p_st->s_v.num -=1;
					}
					else
						print("inter[error line %d]: the oper value is not num!\n", t_n->line);
				}
				else if(t_n->childs_p[0]->b_t == bnf_type_const)
				{
					rt_n = new_ret_node(sym_type_num);
					rt_n->ret_value.r_v.num = t_n->childs_p[0]->b_v.number;
				}

				if(check_ret(l_rt, sym_type_alp))
				{
					r_value* r_p = l_rt->ret_value.r_v.ptr;
					if(r_p->r_t != sym_type_num)
						print("inter[error line %d]: the oper value is not num!\n", t_n->line);
					else
						(r_p->r_v.num)--;
				}
			}
		}
		break;
	}

	free_ret(l_rt);
	free_ret(r_rt);

	return rt_n;
}


ret_node* inter_op_value(inter_env env, tree_node* t_n)
{
	ret_node* rt_n = NULL;
	switch(t_n->b_v.op_t)
	{
	case token_type_mod:		// %
	case token_type_div:		// /
	case token_type_mul:		// *
	case token_type_sub:		// -
	case token_type_add:		// +
		{
			rt_n = inter_op_tp(env, t_n, t_n->b_v.op_t);
		}
		break;
	case token_type_sadd:		//++
	case token_type_ssub:		//--
		{
			rt_n = inter_op_op(env, t_n, t_n->b_v.op_t);
		}
		break;
	default:
		{
			print("inter[error line %d]: oper \"%s\" is not exit!\n", t_n->line, alex_get_type(t_n->b_v.op_t));
			return NULL;
		}
	}

	return rt_n;
}


int logic_op_big(ret_node* l_ret, ret_node* r_ret)
{
	switch(l_ret->ret_value.r_t)
	{
		case sym_type_string:
			return strcmp(l_ret->ret_value.r_v.str.s_ptr, r_ret->ret_value.r_v.str.s_ptr);
		case sym_type_num:
			return ((l_ret->ret_value.r_v.num) > (r_ret->ret_value.r_v.num)) ;
	}
	return 0;
}

int logic_op_bige(ret_node* l_ret, ret_node* r_ret)
{
	switch(l_ret->ret_value.r_t)
	{
		case sym_type_string:
			return (strcmp(l_ret->ret_value.r_v.str.s_ptr, r_ret->ret_value.r_v.str.s_ptr)>=0);
		case sym_type_num:
			return ((l_ret->ret_value.r_v.num) >= (r_ret->ret_value.r_v.num)) ;
	}
	return 0;
}

int logic_op_lit(ret_node* l_ret, ret_node* r_ret)
{
	switch(l_ret->ret_value.r_t)
	{
		case sym_type_string:
			return (strcmp(l_ret->ret_value.r_v.str.s_ptr, r_ret->ret_value.r_v.str.s_ptr)<0)?(1):(0) ;
		case sym_type_num:
			return ((l_ret->ret_value.r_v.num) < (r_ret->ret_value.r_v.num)) ;
	}
	return 0;
}

int logic_op_lite(ret_node* l_ret, ret_node* r_ret)
{
	switch(l_ret->ret_value.r_t)
	{
		case sym_type_string:
			return (strcmp(l_ret->ret_value.r_v.str.s_ptr, r_ret->ret_value.r_v.str.s_ptr)<=0)?(1):(0) ;
		case sym_type_num:
			return ((l_ret->ret_value.r_v.num) <= (r_ret->ret_value.r_v.num)) ;
	}
	return 0;
}

int logic_op_equ(ret_node* l_ret, ret_node* r_ret)
{
	switch(l_ret->ret_value.r_t)
	{
		case sym_type_string:
			return (strcmp(l_ret->ret_value.r_v.str.s_ptr, r_ret->ret_value.r_v.str.s_ptr)==0)?(1):(0) ;
		case sym_type_num:
			return ((l_ret->ret_value.r_v.num) == (r_ret->ret_value.r_v.num)) ;
	}
	return 0;
}

int logic_op_nequ(ret_node* l_ret, ret_node* r_ret)
{
	switch(l_ret->ret_value.r_t)
	{
		case sym_type_string:
			return (strcmp(l_ret->ret_value.r_v.str.s_ptr, r_ret->ret_value.r_v.str.s_ptr)!=0)?(1):(0) ;
		case sym_type_num:
			return ((l_ret->ret_value.r_v.num) != (r_ret->ret_value.r_v.num)) ;
	}
	return 0;
}


int inter_logic_op(ret_node* l_ret, ret_node* r_ret, logic_op_func l_op_func)
{
	if(l_ret->ret_value.r_t != r_ret->ret_value.r_t)
	{
		print("inter[error] logic is error! l_op and r_op is not same type!\n");
		return 0;
	}
	else
	{
		switch(l_ret->ret_value.r_t)
		{
		case sym_type_func:
			{
				print("inter[error]: func is not can be oper!\n");
				return 0;
			}
			break;
		case sym_type_num:
		case sym_type_string:
			{
				return l_op_func(l_ret, r_ret);
			}
			break;
		}
	}

	return 0;
}



ret_node* inter_op_logic(inter_env env, tree_node* t_n)
{
	ret_node* rt_n = new_ret_node(sym_type_num);
	ret_node* l_ret = inter_exp(env, t_n->childs_p[0]);
	ret_node* r_ret = inter_exp(env, t_n->childs_p[1]);

	l_ret = (check_ret(l_ret, sym_type_alp))?(al_ptov(l_ret)):(l_ret);
	r_ret = (check_ret(r_ret, sym_type_alp))?(al_ptov(r_ret)):(r_ret);

	switch(t_n->b_v.op_t)
	{
	case token_type_and:
		{
			rt_n->ret_value.r_v.num = (ret_to_bool(l_ret) && ret_to_bool(r_ret) );	
		}
		break;
	case token_type_or:
		rt_n->ret_value.r_v.num = (ret_to_bool(l_ret) || ret_to_bool(r_ret) );
		break;
	case token_type_big:
		rt_n->ret_value.r_v.num = inter_logic_op(l_ret, r_ret, logic_op_big);
		break;
	case  token_type_bige:
		rt_n->ret_value.r_v.num = inter_logic_op(l_ret, r_ret, logic_op_bige);
		break;
	case token_type_lit:
		rt_n->ret_value.r_v.num = inter_logic_op(l_ret, r_ret, logic_op_lit);
		break;
	case token_type_lite:
		rt_n->ret_value.r_v.num = inter_logic_op(l_ret, r_ret, logic_op_lite);
		break;
	case token_type_equ:
		rt_n->ret_value.r_v.num = inter_logic_op(l_ret, r_ret, logic_op_equ);
		break;
	case token_type_nequ:
		rt_n->ret_value.r_v.num = inter_logic_op(l_ret, r_ret, logic_op_nequ);
		break;
	}

	free_ret(l_ret);
	free_ret(r_ret);
	return rt_n;
}

// = 赋值
ret_node* inter_op_ass(inter_env env, tree_node* t_n)
{
	ret_node* rt_n = NULL;
	st* l_st = NULL;
	st  r_st = {0};


	switch(t_n->childs_p[0]->b_t)
	{
	case bnf_type_var:
		{
			// search ide 
			if( (l_st=look_env(env, t_n->childs_p[0]->b_v.name.s_ptr))==NULL )
			{
				print("inter[error line %d]: the ide \"%s\" not def!\n", t_n->line, t_n->childs_p[0]->b_v.name.s_ptr);
				return NULL;
			}
			
			// find it
			rt_n = inter_exp(env, t_n->childs_p[1]);
			if(check_ret(rt_n, sym_type_alp))
				rt_n = al_ptov(rt_n);
			r_st = ret_to_st(t_n->childs_p[0]->b_v.name.s_ptr, rt_n);
			free_st(l_st);
			if(r_st.s_t == sym_type_al)
				r_st.s_v.al->count++;
			*l_st = r_st;			// re value
		}
		break;
	case bnf_type_al:
		{
			ret_node* l_rt = NULL;
			l_rt = inter_al(env, t_n->childs_p[0]);
			rt_n = inter_exp(env, t_n->childs_p[1]);

			if(l_rt==NULL || rt_n==NULL)
			{
				print("inter[error line: %d] left value or right value error!\n", t_n->line);
				free_ret(l_rt);
				free_ret(rt_n);
				return NULL;
			}
			
			free_value((r_value*)(l_rt->ret_value.r_v.ptr));

			switch(rt_n->ret_value.r_t)
			{
			case  sym_type_string:
				((r_value*)(l_rt->ret_value.r_v.ptr))->r_t = sym_type_string;
				((r_value*)(l_rt->ret_value.r_v.ptr))->r_v.str = alex_string(rt_n->ret_value.r_v.str.s_ptr);
				break;
			case sym_type_alp:
				rt_n = al_ptov(rt_n);
				(check_ret(rt_n, sym_type_al))?(rt_n->ret_value.r_v.al->count++):(0);
				*((r_value*)(l_rt->ret_value.r_v.ptr)) = rt_n->ret_value;
				break;
			case sym_type_al:
				rt_n->ret_value.r_v.al->count++;
				*((r_value*)(l_rt->ret_value.r_v.ptr)) = rt_n->ret_value;
				break;
			default:
				*((r_value*)(l_rt->ret_value.r_v.ptr)) = rt_n->ret_value;
				break;
			}
			free_ret(l_rt);
		}
		break;
	default:
		{
			print("inter[error line %d]: the tree_node \"%s\" is not left value!\n", t_n->line, string_bnf(t_n->childs_p[0]->b_t));
		}
		return NULL;
	}

	return rt_n;
}


void free_ret(ret_node* rt_n)
{
	if (rt_n == NULL)
	{
		return;
	}

	while(rt_n)
	{
		ret_node* n_rn = rt_n->next;
		if(check_ret(rt_n, sym_type_string))
			free_string( &(rt_n->ret_value.r_v.str));
		free(rt_n);
		rt_n = n_rn;
	}
}


ret_node* st_to_ret(st* l_st)
{
	ret_node* rt_n = NULL;
	if(l_st)
	{
		rt_n = new_ret_node(l_st->s_t);
		switch(l_st->s_t)
		{
		case sym_type_string:
			rt_n->ret_value.r_v.str = alex_string(l_st->s_v.str.s_ptr);
			break;
		default:
			rt_n->ret_value.r_v = l_st->s_v;
			break;
		}
	}

	return rt_n;
}


ret_node* al_ptov(ret_node* al_p)
{
	ret_node* rt_n = NULL;
	r_value* r_p = NULL;

	if(al_p==NULL || check_ret(al_p, sym_type_alp)==0)
	{
		return NULL;
	}
	
	r_p = (r_value*)(al_p->ret_value.r_v.ptr);
	rt_n = new_ret_node(r_p->r_t);
	if(r_p->r_t == sym_type_string)
		rt_n->ret_value.r_v.str = alex_string(r_p->r_v.str.s_ptr);
	else
		rt_n->ret_value.r_v = r_p->r_v;
	
	free_ret(al_p);
	return rt_n;
}


st ret_to_st(char* name, ret_node* rt_n)
{
	st ret_st = {0};

	if(rt_n)
	{
		ret_st.s_t = rt_n->ret_value.r_t;
		ret_st.name = alex_string(name);
		switch(rt_n->ret_value.r_t)
		{
		case sym_type_func:
			ret_st.s_v.func = rt_n->ret_value.r_v.func;
			break;
		case sym_type_num:
			ret_st.s_v.num = rt_n->ret_value.r_v.num;
			break;
		case  sym_type_string:
			ret_st.s_v.str = alex_string(rt_n->ret_value.r_v.str.s_ptr);
			break;
		case sym_type_al:
			ret_st.s_v.al = rt_n->ret_value.r_v.al;
			break;
		}
	}
	return ret_st;
}


ret_node* inter_p_call(inter_env env, tree_node* t_n, ret_node* arg_list)
{
	ret_node* rt_n = NULL;
	tree_node* arg_t_n = t_n->childs_p[0];
	
	// get arg tree node
	if(arg_t_n)
		arg_t_n = arg_t_n->childs_p[0];
	
	if(tree_watch(t_n, bnf_type_func)==0)
		goto P_CALL_END;

	// get arg
	while( (arg_t_n) && (type_tree(arg_t_n)==bnf_type_var) )
	{
		st a_st = {0};
		if(arg_list== NULL)
		{
			print("inter[error line %d] the arg list not same for the func def!\n", t_n->line);
			return NULL;
		}
		a_st.name = alex_string(arg_t_n->b_v.name.s_ptr);
		a_st.s_t = arg_list->ret_value.r_t;
		a_st.s_v = arg_list->ret_value.r_v;

		add_table(env.l_table, a_st);			// add arg from local table
		if(arg_list == NULL)
		{
			print("inter[error line %d]: func's arg list is error!\n", t_n->line);
			return NULL;
		}

		arg_list = arg_list->next;	
		arg_t_n = arg_t_n->next;
	}

	rt_n = inter_seg(env, t_n->childs_p[1]);		// run func

P_CALL_END:
	return rt_n;
}


ret_node* inter_seg(inter_env env, tree_node* t_n)
{
	ret_node* rt_n = NULL;
	t_n = t_n->childs_p[0];

	while(t_n)
	{
		switch(type_tree(t_n))
		{
		case bnf_type_seg:
			rt_n = inter_seg(env, t_n);
			break;
		case  bnf_type_vardef:
			rt_n = inter_vardef(env.l_table, t_n);
			break;
		case  bnf_type_return:
			{
				rt_n = inter_return(env, t_n);
				return rt_n;
			}
			break;
		case bnf_type_break:
			{
				rt_n = new_ret_node(sym_type_num);
				rt_n->rt = ret_break;

				return rt_n;
			}
			break;
		case  bnf_type_continue:
			{
				rt_n = new_ret_node(sym_type_num);
				rt_n->rt = ret_continue;

				return rt_n;
			}
			break;
		case  bnf_type_if:
			{
				rt_n = inter_if(env , t_n);
				if(rt_n)
					return rt_n;
			}
			break;
		case bnf_type_while:
			{
				rt_n = inter_while(env, t_n);
				if(rt_n)
					return rt_n;
			}
			break;
		default:
			rt_n = inter_exp(env, t_n);
			break;
//			print("inter[error]: seg is  not allow \"%s\" !\n", string_bnf(type_tree(t_n)));
		}

		free_ret(rt_n);
		rt_n = NULL;
		t_n = t_n->next;
	}

	return rt_n;
}


ret_node* inter_if(inter_env env, tree_node* t_n)
{
	ret_node* rt_n = NULL;
	ret_node* if_rt_n = NULL;

	tree_watch(t_n, bnf_type_if);
	if_rt_n = inter_exp(env, t_n->childs_p[0]);

	if(ret_to_bool(if_rt_n))
	{
		rt_n = inter_seg(env, t_n->childs_p[1]);
	}
	else if(t_n->childs_p[2])
		rt_n = inter_seg(env, t_n->childs_p[2]);

	free_ret(if_rt_n);
	return rt_n;
}



ret_node* inter_while(inter_env env, tree_node* t_n)
{
	ret_node* rt_n = NULL;
	ret_node* while_rt_n = NULL;

	while(ret_to_bool(while_rt_n = inter_exp(env, t_n->childs_p[0])))
	{
		rt_n = 	inter_seg(env, t_n->childs_p[1]);
		if(rt_n)
		{
			switch(rt_n->rt)
			{
			case ret_break:
			case ret_return:
				goto WHILE_END;
			case ret_continue:
			case ret_normal:
				rt_n = NULL;
				break;
			}
		}
		free_ret(rt_n);
		rt_n = NULL;
		free_ret(while_rt_n);
		while_rt_n = NULL;
	}

WHILE_END:
	free_ret(while_rt_n);
	while_rt_n = NULL;
	return rt_n;
}


ret_node* inter_return(inter_env env, tree_node* t_n)
{
	ret_node* rt_n= NULL;
	t_n = t_n->childs_p[0];

	if(t_n == NULL)
	{
		rt_n = new_ret_node(sym_type_num);
		rt_n->rt = ret_return;
		rt_n->ret_value.r_t = sym_type_num;
		rt_n->ret_value.r_v.num =0;
	}
	else
	{
		ret_node** n_rt_n = NULL;
		rt_n = inter_exp(env, t_n);
		rt_n->rt = ret_return;
		n_rt_n = &rt_n->next;
		t_n = t_n->next;

		while(t_n)
		{
			*n_rt_n = inter_exp(env, t_n);
			(*n_rt_n)->rt = ret_return;
			n_rt_n = &((*n_rt_n)->next);
			t_n = t_n->next;
		}
	}

	return rt_n;
}



int tree_watch(tree_node* t_n, enum bnf_type  b_t)
{
	if(t_n)
	{
		if(t_n->b_t == b_t)
			return 1;
		else
		{
			print("tree_watch[error line %d]: error tree node \"%s\"!\n", t_n->line, string_bnf(t_n->b_t));
			return 0;
		}
	}
	else
		return 0;
}


ret_node* new_ret_node(enum sym_type s_t)
{
	ret_node* rt = (ret_node*)malloc(sizeof(ret_node));
	memset(rt, 0, sizeof(ret_node));
	
	rt->ret_value.r_t = s_t;
	return rt;
}

st*  look_env(inter_env env, char* str)
{
	st* ret_st = NULL;
	ret_st = look_table(env.l_table, str);
	if(ret_st)
		return ret_st;
	else
		return look_table(env.g_table, str);
}


int  ret_to_bool(ret_node* rt_n)
{
	if( (rt_n == NULL) || (rt_n->rt != ret_normal) )
		return 0;

	switch(rt_n->ret_value.r_t)
	{
	case  sym_type_func:
	case sym_type_string:
		return 1;
	case sym_type_num:
		return (int)rt_n->ret_value.r_v.num;
	}

	return 0;
}


