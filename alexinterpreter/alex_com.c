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
	
}


int com_func_call(com_env* com_p, tree_node* t_n)
{
	r_addr r_a = search_addr(com_p, t_n->b_v.name.s_ptr);
	if(r_a.gl == COM_ERROR)
	{
		print("com[error line :%d] the func %s is not find!\n", t_n->line, t_n->b_v.name.s_ptr);
		return COM_ERROR_NOT_FIND_IDE;
	}

	return COM_SUCCESS;
}


int com_ass(com_env* com_p, tree_node* t_n)
{

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
				push_inst(&com_p->com_inst, new_inst((r_a.gl==COM_LOCAL)?(MOVE):(GMOVE), r_a.addr));
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
	switch(e_i)
	{
	case CALL:
	case JUMP:
	case GMOVE:
	case MOVE:
		{
			int addr = va_arg(arg_list, int);
			a_i.inst_value.r_t = sym_type_addr;
			a_i.inst_value.r_v.addr = addr;
		}	
		break;
	}
	va_end(a_i);

	return a_i;
}
