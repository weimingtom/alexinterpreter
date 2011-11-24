// a_util.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include <stdio.h>
#include "alex_ani.h"



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


 
int dll_print_a(void* vm_p)
{
	ALEX_NUMBER a = ani_pop_number(vm_p);

	printf("here is dll_print_a a=[%lf]!\n", a);

	return 0;
}


extern "C" _declspec(dllexport)
void alex_dll_reg()
{
	ani_reg_func("print_a", dll_print_a);		
}

