#include "alex_conf.h"
#include "alex_sym.h"
#include "alex_lib.h"
#include "alex_arrylist.h"
#include "alex_interpret.h"
#include "alex_window.h"
#include "alex_vm.h"
#include "alex_gc.h"
#include <time.h>

void reg_lib(sym_table* g_t, char* str, ALEX_FUNC a_f)
{
	st t_st = {0};

	t_st.name = alex_string(str);
	t_st.s_t = sym_type_reg_func;
	t_st.s_v.func = a_f;
	add_table(g_t, t_st);
}

void reg_dll(char* str, ALEX_FUNC a_f)
{
	reg_lib(global_table, str, a_f);
}

void print_value(r_value  val)
{
	switch(val.r_t)
	{
	case sym_type_num:
		a_print("%lf", val.r_v.num);
		break;
	case sym_type_string:
		a_print("%s", val.r_v.str.s_ptr);
		break;
	case  sym_type_alp:
		{
			r_value r_ptr = *((r_value*)(val.r_v.ptr));
			print_value(r_ptr);
		}
		break;
	case sym_type_al:
		{
			int i=0;
			a_print("[");
			for(i=0; i<val.r_v.al->al_len; i++)
			{
				print_value((val.r_v.al->al_v)[i]);
				if(i <val.r_v.al->al_len-1)
					a_print(", ");
			}
			a_print("]");
		}
		break;
	case sym_type_func:
		a_print("%p", val.r_v.func);
		break;
	}
}



// ´òÓ¡¿âº¯Êý
int alex_print(vm_env* vm_p)
{
	r_value r_v = pop_data(&vm_p->data_ptr);
	print_value(r_v);
	
	return 0;
}




// sleepº¯Êý
int alex_sleep(vm_env* vm_p)
{
	int w_t = (int)pop_number(vm_p);
	Sleep((DWORD)w_t);

	return 0;
}

int alex_len(vm_env* vm_p)
{
	r_value al = pop_data(&vm_p->data_ptr);

	push_data(&vm_p->data_ptr, new_number(al.r_v.al->al_len));
	return 1;
}



int alex_add(vm_env* vm_p)
{
	r_value r_v = pop_data(&vm_p->data_ptr);
	r_value al =  pop_data(&vm_p->data_ptr);

	push_data(&vm_p->data_ptr, add_al(al.r_v.al, r_v));

	return 1;
}


// rand  func
int alex_rand(vm_env* vm_p)
{
	int e_r = (int)pop_number(vm_p);
	int b_r = (int)pop_number(vm_p);
	int ret_n =0;
	
	ret_n = rand();
	if(e_r - b_r  <= 0)
		ret_n = 0;
	else
		ret_n = (ret_n % (e_r-b_r)) + b_r;

	push_number(vm_p, (ALEX_NUMBER)ret_n);
	return 1;
}


void alex_reg_lib(sym_table* g_t)
{
	srand((unsigned)time(0));
	reg_lib(g_t, "print", alex_print);
	reg_lib(g_t, "create_window", alex_create_window);
	reg_lib(g_t, "message_box", alex_message_box);
	reg_lib(g_t, "sleep", alex_sleep);
	reg_lib(g_t, "add", alex_add);;
	reg_lib(g_t, "get_key", alex_get_key);
	reg_lib(g_t, "rectangle", alex_rectangle);
	reg_lib(g_t, "rand", alex_rand);
	reg_lib(g_t, "len", alex_len);
	reg_lib(g_t, "t_time", alex_t_time);
	reg_lib(g_t, "clear", alex_clear);
}	
