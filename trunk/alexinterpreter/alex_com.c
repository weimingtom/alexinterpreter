#include "alex_com.h"
#include "alex_vm.h"


com_env* com_env_p = NULL;
com_env* new_com_env()
{
	com_env* ret_c_e_p = (com_env*)malloc(sizeof(com_env));
	memset(ret_c_e_p, 0, sizeof(com_env));

	relloc_code(&(ret_c_e_p->com_inst));
	ret_c_e_p->var_table.g_table = &global_table;
	ret_c_e_p->var_table.g_top=0;

	ret_c_e_p->var_table.l_table = new_table();
	ret_c_e_p->var_table.l_top=0;

	relloc_data(&(ret_c_e_p->var_table.temp_ptr), TEMP_MEM_LEN);
	return ret_c_e_p;
} 

// 编译code入口 main_tree为执行调用代码 func_tree为函数链表
void alex_com(com_env* com_p, tree_node* main_tree, tree_node* func_tree)
{
	if(main_tree == NULL)
		return;
	 
	while(func_tree)
	{
		com_func_def(com_p, func_tree);
		func_tree = func_tree->next;
	}

	while(main_tree)
	{
		switch(type_tree(main_tree))
		{
		case bnf_type_vardef:
			{
				com_g_var(com_p, main_tree);
			}
			break;
		case bnf_type_funccall:
			{
				com_func_call(com_p, main_tree);
			}
			break;
		default:
			{
				print("com[error line %d]: the g_code not allow!\n", main_tree->line);
			}
			return;
		}
		main_tree = main_tree->next;
	}
}


int com_func_def(com_env* com_p, tree_node* t_n)
{
	tree_node* arg_t_n = t_n->childs_p[0];
	tree_node* seg_t_n = t_n->childs_p[1];

	check_com(com_arg_def(com_p, arg_t_n));
	check_com(com_seg(com_p, seg_t_n));

	// if the function is not return, so assume is number 0
	check_com(com_return_assume(com_p, t_n));
	return COM_SUCCESS;
}


int com_arg_def(com_env* com_p, tree_node* t_n)
{
	int i;
	int arg_num = (int)t_n->b_v.number;
	t_n = t_n->childs_p[0];

	// set addr
	while(t_n)
	{
		l_com_addr(com_p, t_n->b_v.name.s_ptr);
		t_n = t_n->next;
	}
	
	// push inst 
	for(i=arg_num-1; i>=0; i--)
	{
		push_inst(&com_p->com_inst, new_inst(MOVE, COM_LOCAL, i));
		push_inst(&com_p->com_inst, new_inst(POP));
	}

	return COM_SUCCESS;
}

int com_seg(com_env* com_p, tree_node* t_n)
{
	t_n = t_n->childs_p[0];
	
	while(t_n)
	{
		switch(type_tree(t_n))
		{
		case bnf_type_seg:
			check_com(com_seg(com_p, t_n));
			break;
		case bnf_type_vardef:
			check_com(com_l_var(com_p, t_n));
			break;
		case bnf_type_return:
			check_com(com_return(com_p, t_n));
			break;
		case bnf_type_break:
			check_com(com_break(com_p, t_n));
			break;
		case bnf_type_continue:
			check_com(com_continue(com_p, t_n));
			break;
		case bnf_type_if:
			check_com(com_if(com_p, t_n));
			break;
		case bnf_type_while:
			check_com(com_while(com_p, t_n));
			break;
		default:
			check_com(com_exp_stmt(com_p, t_n));
			break;
		}
		t_n = t_n->next;
	}

	return COM_SUCCESS;
}

int com_return(com_env* com_p, tree_node* t_n)
{
	t_n = t_n->childs_p[0];
	if(t_n==NULL)			// not return value
	{
		com_return_assume(com_p, t_n);
	}
	else
	{
		check_com(com_exp(com_p, t_n));
		push_inst(&com_p->com_inst, new_inst(RET));
	}

	return COM_SUCCESS;
} 

// when return; or no return
int com_return_assume(com_env* com_p, tree_node* t_n)
{
	r_value r_v = new_number(0);

	push_inst(&com_p->com_inst, new_inst(PUSH, r_v));
	push_inst(&com_p->com_inst, new_inst(RET));
	
	return COM_SUCCESS;
}


int com_break(com_env* com_p, tree_node* t_n)
{
	return COM_SUCCESS;
}

int com_continue(com_env* com_p, tree_node* t_n)
{
	return COM_SUCCESS;
}

int com_if(com_env* com_p, tree_node* t_n)
{
	return COM_SUCCESS;
}

int com_while(com_env* com_p, tree_node* t_n)
{

	/* while seg*/

	
	return COM_SUCCESS;
}

int com_func_call(com_env* com_p, tree_node* t_n)
{
	r_addr r_a = search_addr(com_p, t_n->b_v.name.s_ptr);
	st* ret_st = NULL;
	if(r_a.gl == COM_ERROR)
	{
		print("com[error line :%d] the func %s is not find!\n", t_n->line, t_n->b_v.name.s_ptr);
		return COM_ERROR_NOT_FIND_IDE;
	}
	else
	{
		ret_st = look_com(com_p, t_n->b_v.name.s_ptr);
		check_com(com_arg(com_p, t_n));	
		push_inst(&com_p->com_inst, new_inst(CALL, r_a.gl, r_addr));
	}
	
	return COM_SUCCESS;
}


int com_arg(com_env* com_p, tree_node* t_n)
{
	tree_node* t_n = t_n->childs_p[0];

	while(t_n)
	{
		check_com(com_exp(com_p, t_n));
		t_n = t_n->next;
	}

	return COM_SUCCESS;
}


int com_exp(com_env* com_p, tree_node* t_n)
{
	return COM_SUCCESS;
}


int com_exp_stmt(com_env* com_p, tree_node* t_n)
{
	check_com(com_exp(com_p, t_n));
	push_inst(&com_p->com_inst, new_inst(POP));
	
	return COM_SUCCESS;
}

int com_ass(com_env* com_p, tree_node* t_n)
{
	tree_node* l_t_n = t_n->childs_p[0];
	switch(type_tree(l_t_n))
	{
	case bnf_type_var:
		break;
	case bnf_type_al:
		break;
	default:
		print("com[error line: %d] the ide can not left value!\n", t_n->line);
		return COM_ERROR_NOT_LEFT_VALUE;
	}

	return COM_SUCCESS;
}

int  com_var(com_env* com_p, tree_node* t_n, e_gl gl)
{
	sym_table* a_table = NULL;	
	t_n = t_n->childs_p[0];
	a_table = (gl==COM_GLOBAL)?(com_p->var_table.g_table):(com_p->var_table.l_table);
				
	while(t_n)
	{
		char* var_name = NULL;
		switch(type_tree(t_n))
		{
		case bnf_type_ass:
			var_name = t_n->childs_p[0]->b_v.name.s_ptr;
			goto CHECK_VAR;
		case bnf_type_var:
			var_name = t_n->b_v.name.s_ptr;
CHECK_VAR:
			if(look_table(a_table, var_name))
			{
				print("com[error line: %d] the var  \"%s\" ide is redefine!\n", t_n->line, var_name);
				return COM_ERROR_REDEF;
			}
			else
				add_new_table(a_table, var_name);

			if(type_tree(t_n)==bnf_type_ass)
			{
				r_addr r_a = {0};
				check_com(com_ass(com_p, t_n));
				r_a = search_addr(com_p, var_name);
				push_inst(&com_p->com_inst, new_inst(MOVE, r_a.gl, r_a.addr));
				push_inst(&com_p->com_inst, new_inst(POP));
			}
			break;
		default:
			print("inter[error line %d] at var def the  tree_node \"%s\" no allow!\n", t_n->line, string_bnf(t_n->b_t));
			return COM_ERROR_NOT_ALLOW;
		}
		t_n = t_n->next;
	}
}



void clear_local_addr(com_env* com_p)
{
	if(com_p==NULL)
		return;

	free_table(com_p->var_table.l_table);
	com_p->var_table.l_table = new_table();
	com_p->var_table.l_top = 0;
}


// 获得变量编译成的地址 如果在table
r_addr com_addr(com_env* com_p, char* name, e_gl gl)
{
	r_addr ret = {0};
	st* r_st = NULL;

	if(com_p==NULL || name==NULL)
		return ret;
	
	ret.gl = gl;

	if(gl== COM_GLOBAL)
	{
		r_st = look_table(com_p->var_table.g_table, name);
		if(r_st==NULL)
		{
			r_st = add_new_table(com_p->var_table.g_table, name);
			r_st->st_addr = com_p->var_table.g_top++;
		}
		ret.addr = r_st->st_addr;	
	}
	else if(gl == COM_LOCAL)
	{
		r_st = look_table(com_p->var_table.l_table, name);
		if(r_st==NULL)
		{
			r_st = add_new_table(com_p->var_table.l_table, name);
			r_st->st_addr = com_p->var_table.l_top++;
		}
		ret.addr = r_st->st_addr;
	}	

	return ret;
}


st*  look_com(com_env* com_p, char* str)
{
	st* ret_st = NULL;
	ret_st = look_table(com_p->var_table.l_table, str);
	if(ret_st)
		return ret_st;
	else
		return look_table(com_p->var_table.g_table, str);
}


// 查找地址
r_addr search_addr(com_env* com_p, char* name)
{
	r_addr ret = {0};
	st* r_st = NULL;
	
	if(com_p==NULL || name==NULL)
		return ret;
	
	r_st = look_table(com_p->var_table.l_table, name);
	if(r_st)
	{
		ret.gl = COM_LOCAL;
		ret.addr = r_st->st_addr;
	}
	else
	{
		r_st = look_table(com_p->var_table.g_table, name);
		if(r_st)
		{
			ret.gl = COM_LOCAL;
			ret.addr = r_st->st_addr;
		}
	}

	return ret;
}

alex_inst new_inst(e_alex_inst e_i, ...)
{
	alex_inst a_i = {0};
	va_list arg_list;

	va_start(arg_list, e_i);
	a_i.inst_type = e_i;
	switch(a_i.inst_type)
	{
	case CALL:
	case JUMP:
	case MOVE:
		{
			a_i.gl = (e_gl)va_arg(arg_list, e_gl);			// get gl
			a_i.inst_value.r_v.addr = va_arg(arg_list, int);// get addr
			a_i.inst_value.r_t = sym_type_addr;				// set type
		}	
		break;
	case PUSH:
		{
			a_i.inst_value = (r_value)va_arg(arg_list, r_value);
		}
		break;
	}
	va_end(a_i);

	return a_i;
}
