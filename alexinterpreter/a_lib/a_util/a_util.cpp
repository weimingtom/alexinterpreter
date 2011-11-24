// a_util.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include <stdio.h>
#include "alex_ani.h"

static ani alex_ani = {0};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" _declspec(dllexport)
void alex_init_reg(
	void* pop_number_p,
	void* push_number_p,
	
	void* pop_string_p,
	void* push_string_p,
	
	void* pop_al_p,
	void* push_al_p,

	void* pop_ptr_p,
	void* push_ptr_p,

	void* pop_func_p,
	void* reg_func_p
				   )
{

	alex_ani._ani_pop_number = (FUNC_POP_NUMBER)pop_number_p;
	alex_ani._ani_push_number = (FUNC_PUSH_NUMBER)push_number_p;

	alex_ani._ani_pop_string = (FUNC_POP_STRING)pop_string_p;
	alex_ani._ani_push_string = (FUNC_PUSH_STRING)push_string_p;

	alex_ani._ani_pop_al = (FUNC_POP_AL)pop_al_p;
	alex_ani._ani_push_al = (FUNC_PUSH_AL)push_al_p;

	alex_ani._ani_pop_ptr = (FUNC_POP_PTR)pop_ptr_p;
	alex_ani._ani_push_ptr = (FUNC_PUSH_PTR)push_ptr_p;

	alex_ani._ani_pop_func = (FUNC_POP_FUNC)pop_func_p;

	alex_ani._ani_reg_func = (FUNC_REG_FUNC)reg_func_p;
}

 
int dll_print_a(void* vm_p)
{
	ALEX_NUMBER a = ani_pop_number(vm_p);

	printf("here is dll_print_a a=[%d]!\n", a);

	return 0;
}


extern "C" _declspec(dllexport)
void alex_dll_reg()
{
	ani_reg_func("print_a", dll_print_a);		
}
