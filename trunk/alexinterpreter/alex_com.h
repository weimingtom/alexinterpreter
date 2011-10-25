#ifndef _ALEX_COM_H_
#define _ALEX_COM_H_

#include "alex_vm.h"
#include "alex_sym.h"

/*
	alex interpret compile
*/


typedef enum _e_e{
	COM_SUCCESS,
	COM_ERROR_NOT_ALLOW
}e_e;

typedef enum _e_gl{
	COM_ERROR,
	COM_GLOBAL,
	COM_LOCAL
}e_gl;

typedef struct _r_addr{
	e_gl gl;
	int addr;
}r_addr;

typedef struct _var_addr{
	sym_table* g_table;
	int g_top;

	sym_table* l_table;
	int l_top;
}var_addr;

typedef struct _com_env{
	c_inst	   com_inst;	// compile inst
	
	var_addr	var_table;	
}com_env;

#define g_com_addr(c_p,s) com_addr((c_p),(s), COM_GLOBAL)
#define l_com_addr(c_p,s) com_addr((c_p),(s), COM_LOCAL)
#define check_com(s)	  do{int r_s=(s); (r_s)?(return r_s):(0)}while(0) 


r_addr com_addr(com_env* com_p, char* name, e_gl gl);
r_addr search_addr(com_env* com_p, char* name);

#endif