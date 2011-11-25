#include "alex_sym.h"
#include "stdio.h"
#include "stdlib.h"
#include "alex_symbol_table.h"
#include "alex_log.h"
#include "alex_parsing.h"
#include "alex_arrylist.h"
#include "alex_com.h"
#include <memory.h>
// 符号表 操作


sym_table* global_table = NULL;		// only one global table

#define bkdr_hash(str)		_BKDRHash(str, SYM_TABLE_LEN)

sym_table* new_table()
{
	sym_table* r_st = (sym_table*)a_malloc(sizeof(sym_table));
	memset(r_st, 0, sizeof(sym_table));

	return r_st;
}


st* add_new_table(sym_table* s_t, char* name)
{
	st t_st = {0};
	t_st.name = alex_string(name);

	return add_table(s_t, t_st);
}

st* add_table(sym_table* s_t, st st_v)
{
	int inx = 0;
	st_node* st_p = NULL;
	st_node** st_w = NULL;

	if(s_t == NULL)
		return NULL;

	inx = bkdr_hash(st_v.name.s_ptr);
	st_p = s_t->st_l[inx];

	while (st_p)
	{
		if(alex_strcmp(st_p->st_v.name, st_v.name.s_ptr) == 0)
		{
			st_p->st_v = st_v;
			return &st_p->st_v;
		}

		if(st_p->next == NULL)
			break;
		st_p = st_p->next;
	}

	st_w = (st_p)?(&(st_p->next)):(&(s_t->st_l[inx]));
	*st_w = (st_node*)a_malloc(sizeof(st_node));
	memset(*st_w, 0, sizeof(st_node));

	//  进行编译地址赋值
	if (s_t==global_table)
	{
		r_value r_v = {0};
		if(st_v.s_t == sym_type_reg_func)
		{
			r_v.r_t = sym_type_reg_func;
			r_v.r_v.func = st_v.s_v.func;

		}
		else
		{
			r_v = new_number(0);
		}
		push_data(&com_env_p->var_table.global_ptr, r_v);
		st_v.st_addr = com_env_p->var_table.global_ptr.data_len - 1;
	}
	else
		st_v.st_addr = (com_env_p->var_table.l_top++);
	(*st_w)->st_v = st_v;
	
	return &((*st_w)->st_v);
}


st*	look_table(sym_table* s_t, char* s_str)
{
	int inx = 0;
	st_node* st_p = NULL;

	if(s_t == NULL || s_str == NULL)
		return NULL;

	inx = bkdr_hash(s_str);
	st_p = s_t->st_l[inx];

	while(st_p)
	{
		if(alex_strcmp(st_p->st_v.name, s_str)==0)
			return &st_p->st_v;
		st_p = st_p->next;
	}

	return NULL;
}


void free_table(sym_table* s_t)
{
	int i=0;
	if(s_t == NULL)
		return;

	for(i=0; i<SYM_TABLE_LEN; i++)
	{
		st_node* st_p = NULL;
		if(s_t->st_l[i]== NULL)
			continue;
		
		st_p = s_t->st_l[i];
		while(st_p)
		{
			st_node* t_st_p = st_p->next;
			free_st(&st_p->st_v);
			a_free(st_p);
			st_p = t_st_p;
		}
	}

	a_free(s_t);
}



void print_table(sym_table* s_t)
{
	int i=0;
	if(s_t==NULL)
		return;
	
	for(i=0; i<SYM_TABLE_LEN; i++)
	{
		st_node* st_p = s_t->st_l[i];
		while(st_p)
		{
			print("name %s, ", st_p->st_v.name.s_ptr);
			switch(st_p->st_v.s_t)
			{
			case sym_type_num:
					print(" value : %lf\n", st_p->st_v.s_v.num);
				break;
			case sym_type_string:
					print(" value : %s\n", st_p->st_v.s_v.str);
				break;
			case  sym_type_func:
				print(" value : %p\n", st_p->st_v.s_v.func);
				break;
			}
			st_p = st_p->next;
		}
	}
	
}


void free_st(st* st_p)
{
	if(st_p == NULL)
		return;
	
	free_string(&st_p->name);
	memset(st_p, 0, sizeof(st));
}



st* add_g_table(st  a_st)
{
	if(global_table == NULL)
		global_table = new_table();

	return add_table(global_table, a_st);
}


// get a func st
st new_func_st(char* name)
{
	st r_t = {0};

	if(name == NULL)
		return r_t;

	r_t.s_t = sym_type_func;
	r_t.name = alex_string(name);

	return r_t;
}

r_value new_addr(int addr)
{
	r_value ret = {0};
	ret.r_t = sym_type_addr;
	ret.r_v.addr = addr;

	return ret;
}



r_value new_number(ALEX_NUMBER num)
{
	r_value ret = {0};
	ret.r_t = sym_type_num;
	ret.r_v.num = num;

	return ret;
}