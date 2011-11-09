#include "alex_conf.h"
#include "alex_sym.h"
#include "alex_lib.h"
#include "alex_arrylist.h"
#include "alex_interpret.h"
#include "alex_window.h"
#include "alex_vm.h"
#include <time.h>



void reg_lib(sym_table* g_t, char* str, ALEX_FUNC a_f)
{
	st t_st = {0};

	t_st.name = alex_string(str);
	t_st.s_t = sym_type_reg_func;
	t_st.s_v.func = a_f;
	add_table(g_t, t_st);
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


int alex_a_print(vm_env* vm_p)
{
	r_value r_v = pop_data(&vm_p->data_ptr);
	print_value(r_v);
	
	return 0;
}


// ´òÓ¡¿âº¯Êý
ret_node* alex_print(ret_node* arg_list)
{
	while(arg_list)
	{
		print_value(arg_list->ret_value);
		arg_list = arg_list->next;
	}
	
	return NULL;
}


// sleepº¯Êý
ret_node* alex_sleep(ret_node* arg_list)
{
	int w_t = (int)pop_arg_num(arg_list);
	Sleep((DWORD)w_t);

	return NULL;
}

ret_node* alex_len(ret_node* arg_list)
{
	ret_node* ret = new_ret_node(sym_type_num);
	alex_al* al = pop_arg_al(arg_list);

	if(al)
		ret->ret_value.r_v.num = al->al_len;
	return ret;
}

ret_node* alex_add(ret_node* arg_list)
{
	alex_al* al = NULL;
	ret_node* r_al = pop_arg(arg_list);
	
	if(check_ret(r_al, sym_type_al))
	{	
		al = (r_al->ret_value.r_v.al);
		while(arg_list)
		{
			ret_node* n_ar = arg_list->next;
			(check_ret(arg_list, sym_type_al))?(arg_list->ret_value.r_v.al->count++):(0);
			arg_list = (check_ret(arg_list, sym_type_alp))?(arg_list->next=NULL, al_ptov(arg_list)):(arg_list);
			add_al(al, arg_list->ret_value);
			arg_list = n_ar;
		}
	}
	else if(check_ret(r_al, sym_type_alp))
	{
		r_value* r_p = r_al->ret_value.r_v.ptr;
		if(r_p->r_t == sym_type_al)
		{
			while(arg_list)
			{
				add_al(r_p->r_v.al, arg_list->ret_value);
				arg_list = arg_list->next;
			}	
		}
	}

	return NULL;
}


// rand  func
ret_node* alex_rand(ret_node* arg_list)
{
	ret_node* ret = new_ret_node(sym_type_num);
	int b_r = (int)pop_arg_num(arg_list);
	int e_r = (int)pop_arg_num(arg_list);
	int  ret_n = 0;

	
	ret_n = rand();
	
	if(e_r - b_r  <= 0)
		ret->ret_value.r_v.num = 0;
	else
	{
		ret_n = (ret_n % (e_r-b_r)) + b_r;
		ret->ret_value.r_v.num = ret_n;
	}
	return ret;
}



void alex_reg_lib(sym_table* g_t)
{
	srand((unsigned)time(0));
	//reg_lib(g_t, "print", alex_print);
	reg_lib(g_t, "print", alex_a_print);
	reg_lib(g_t, "create_window", alex_create_window);
	reg_lib(g_t, "message_box", alex_message_box);
	reg_lib(g_t, "sleep", alex_sleep);
	reg_lib(g_t, "add", alex_add);
	reg_lib(g_t, "reg_pen", alex_reg_pen);
	reg_lib(g_t, "reg_key", alex_reg_key);
	reg_lib(g_t, "rectangle", alex_rectangle);
	reg_lib(g_t, "rand", alex_rand);
	reg_lib(g_t, "len", alex_len);
	reg_lib(g_t, "t_time", alex_t_time);
}	


char* _pop_ret_str(ret_node** arg_list)
{
	char* ret = NULL;

	if(*arg_list==NULL || check_ret(*arg_list, sym_type_string)==0)
		return NULL;

	ret = (*arg_list)->ret_value.r_v.str.s_ptr;
	*arg_list = (*arg_list)->next;

	return ret;
}

alex_al* _pop_ret_al(ret_node** arg_list)
{
	alex_al* al = NULL;
	if(*arg_list==NULL || check_ret(*arg_list, sym_type_al)==0)
		return NULL;

	al = (*arg_list)->ret_value.r_v.al;
	*arg_list = (*arg_list)->next;

	return al;
}

void* _pop_ret_ptr(ret_node** arg_list)
{
	void* ret = NULL;

	if(*arg_list==NULL || (check_ret(*arg_list, sym_type_pointer)==0 && check_ret(*arg_list, sym_type_alp)==0))
		return NULL;
	ret = (*arg_list)->ret_value.r_v.ptr;
	*arg_list = (*arg_list)->next;
	
	return ret;

}

ALEX_NUMBER _pop_ret_num(ret_node** arg_list)
{
	ALEX_NUMBER ret = 0;

	if(*arg_list==NULL || check_ret(*arg_list, sym_type_num)==0)
		return 0;
	
	ret = (*arg_list)->ret_value.r_v.num;
	*arg_list = (*arg_list)->next;
	
	return ret;
}



ALEX_FUNC _pop_ret_func(ret_node** arg_list)
{
	ALEX_FUNC ret = 0;
	
	if(*arg_list==NULL || (check_ret(*arg_list, sym_type_func)==0 && check_ret(*arg_list, sym_type_reg_func)==0))
		return NULL;
	
	ret = (*arg_list)->ret_value.r_v.func;
	*arg_list = (*arg_list)->next;
	
	return ret;
}


ret_node* _pop_ret(ret_node** arg_list)
{
	ret_node* rt = *arg_list;
	*arg_list = (*arg_list)?((*arg_list)->next):(NULL);

	return rt;
}