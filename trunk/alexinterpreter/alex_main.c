#include <stdio.h>
#include "alex_parsing.h"
#include "alex_interpret.h"
#include "alex_sym.h"
#include "alex_lib.h"
#include "alex_get_token.h"
#include "alex_log.h"
#include "alex_com.h"

void  alex_init()
{	
	alex_init_symbol_table();
	
	if(global_table)
		free_table(global_table);

	global_table = new_table();
	com_env_p = new_com_env();	
	alex_reg_lib(global_table);
}

int  alex_run(char* file)
{
	int ret = 0;
//	inter_env env = new_inter_env();
	tree_node* t_n = NULL;

	if (file==NULL) return 0;
	alex_load_file(file);
	if (alex_parsing(&t_l)==NULL) return 0;
	
	if(alex_com(com_env_p, m_tree.head, f_tree.head)==COM_SUCCESS)
	{
		if(com_print(com_env_p) == COM_SUCCESS)
		{
			com_to_vm(com_env_p);
			print("\n\n -----CONSEL-----\n");
			alex_vm(&alex_vm_env);

			vm_print(&alex_vm_env);
		}
	}
	
	
//	alex_interpret(env, m_tree.head);
	
	return 1;
}

void alex_free()
{
	free_table(global_table);
	free_tree(m_tree.head);
}


int main(int arg, char* arg_list[])
{
	alex_init();
	alex_run(".\\snake.alx");
	print("please any key to be continue!\n");
	getchar();
	return 0;
}