#include <stdio.h>
#include "alex_parsing.h"
#include "alex_interpret.h"
#include "alex_sym.h"
#include "alex_lib.h"
#include "alex_get_token.h"
#include "alex_log.h"


void  alex_init()
{	
	alex_init_symbol_table();
	
	if(global_table)
		free_table(global_table);

	global_table = new_table();
	alex_reg_lib(global_table);
}

int  alex_run(char* file)
{
	int ret = 0;
	inter_env env = new_inter_env();
	tree_node* t_n = NULL;

	if(file==NULL)
		return 0;

	alex_load_file(file);
	
//	print_token();
	
	t_n = alex_parsing(&t_l);
	if(t_n == NULL)
		goto END;
//	print_tree(m_tree.head);
	
//	print("alex parsing end......\n");
	
	print("\n\n -----CONSEL-----\n");
	alex_interpret(env, m_tree.head);
	ret = 1;
	
END:

	return ret;
}

void alex_free()
{
	free_table(global_table);
	free_tree(m_tree.head);
}


int main(int arg, char* arg_list[])
{
	alex_init();
//	print("arg: %s", arg_list[1]);
	alex_run("F:\\code\\alex\\tt.alx");
	getchar();
	return 0;
}