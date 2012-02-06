#include "alex_anic.h"
#include "alex_vm.h"
#include "alex_lib.h"
#include "alex_log.h"
#include "alex_com.h"
#include <windows.h>

typedef void (*FUNC_DELL_REG)();
typedef void (*FUNC_INIT_REG)(ani*);
// alex reg function table
static ani alex_ani = {0};

void alex_ani_init()
{
	alex_ani._ani_pop_number = pop_number;
	alex_ani._ani_push_number = push_number;
	alex_ani._ani_pop_string = pop_string;
	alex_ani._ani_push_string = push_string;
	alex_ani._ani_pop_al = pop_al;
	alex_ani._ani_push_al = push_al;
	alex_ani._ani_pop_ptr = pop_ptr;
	alex_ani._ani_push_ptr = push_ptr;
	alex_ani._ani_pop_func = pop_func;
	alex_ani._ani_reg_func = reg_dll;
}


int alex_reg_dll(char* dll)
{
	FUNC_INIT_REG init_reg = NULL;
	FUNC_DELL_REG dll_reg = NULL;
	HINSTANCE hinstance=LoadLibraryEx(dll, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	
	if(hinstance && dll)
	{
		// reg dell function 
		init_reg = (FUNC_INIT_REG)GetProcAddress(hinstance,"alex_init_reg");
		if(init_reg)	
			init_reg(&alex_ani);
		else			
		{
			print("using[error]: can not find alex_init_reg at  \"%s\"\n", dll);
			return COM_ERROR_USING;
		}
		// get dll reg function 
		dll_reg = (FUNC_DELL_REG)GetProcAddress(hinstance, "alex_dll_reg");
		if(dll_reg)		
			dll_reg();
		else
		{
			print("using[error]: can not find alex_dll_reg at  \"%s\"\n", dll);
			return COM_ERROR_USING;
		}
	}
	else
	{
		print("using[error]: not find \"%s\"\n", dll);
		return	0;
	}
	return COM_SUCCESS;
}