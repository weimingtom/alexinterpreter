#include "alex_com.h"
#include "alex_vm.h"



com_env* init_com_env()
{
	com_env* ret_c_e_p = (com_env*)malloc(sizeof(com_env));
	memset(ret_c_e_p, 0, sizeof(com_env));

	relloc_code(&(ret_c_e_p->com_inst));
	ret_c_e_p->var_table.g_table = new_table();
	ret_c_e_p->var_table.g_top=0;

	ret_c_e_p->var_table.l_table = new_table();
	ret_c_e_p->var_table.l_top=0;
	return ret_c_e_p;
} 

void alex_com(com_env* com_p, tree_node* main_tree)
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


void free_local_addr(com_env* com_p)
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