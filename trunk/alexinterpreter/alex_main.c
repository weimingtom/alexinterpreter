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

	(file==NULL)?(return 0):(0);
	alex_load_file(file);
	(alex_parsing(&t_l)==NULL)?(return 0):(0);
	
	print("\n\n -----CONSEL-----\n");
	alex_interpret(env, m_tree.head);

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
	alex_run("F:\\code\\alex\\tt.alx");
	getchar();
	return 0;
}