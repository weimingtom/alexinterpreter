#include "alex_sym.h"
#include "stdio.h"
#include "stdlib.h"
#include "alex_symbol_table.h"
#include "alex_log.h"
#include "alex_parsing.h"
#include "alex_arrylist.h"
#include <memory.h>
// 符号表 操作


sym_table* global_table = NULL;		// only one global table

#define bkdr_hash(str)		_BKDRHash(str, SYM_TABLE_LEN)

sym_table* new_table()
{
	sym_table* r_st = (sym_table*)malloc(sizeof(sym_table));
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
			free_st(&st_p->st_v); 
			st_p->st_v = st_v;
			return &st_p->st_v;
		}

		if(st_p->next == NULL)
			break;
		st_p = st_p->next;
	}

	if(st_p == NULL)
		st_w = &(s_t->st_l[inx]);
	else
		st_w = &(st_p->next);

	*st_w = (st_node*)malloc(sizeof(st_node));
	memset(*st_w, 0, sizeof(st_node));
	
	if(st_v.s_t == sym_type_al)
		st_v.s_v.al->count++;

	//  进行编译地址赋值
	st_v.st_addr = (s_t==global_table)?(com_env_p->var_table.g_top++):(com_env_p->var_table.l_top++);
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
			free(st_p);
			st_p = t_st_p;
		}
	}
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
	switch(st_p->s_t)
	{
	case sym_type_string:
		free_string(&(st_p->s_v.str));
		break;
	case sym_type_func:
		free_tree(st_p->s_v.func);
		break;
	case sym_type_al:
		del_al(st_p->s_v.al);
		break;
	}
	memset(st_p, 0, sizeof(st));
}



st* add_g_table(st  a_st)
{
	if(global_table == NULL)
		global_table = new_table();

	return add_table(global_table, a_st);
}


// get a func st
st new_func_st(char* name,  ALEX_FUNC tn_p)
{
	st r_t = {0};

	if(name == NULL|| tn_p == NULL)
		return r_t;

	r_t.s_t = sym_type_func;
	r_t.s_v.func = tn_p;
	r_t.name = alex_string(name);

	return r_t;
}


st new_num_st(char* name, ALEX_NUMBER tn_v)
{
	st r_t = {0};
	
	if(name == NULL)
		return r_t;

	r_t.s_t = sym_type_num;
	r_t.s_v.num = tn_v;
	r_t.name = alex_string(name);

	return r_t;
}


st new_str_st(char* name, ALEX_STRING tn_s)
{
	st r_t = {0};
	
	if(name == NULL)
		return r_t;

	r_t.s_t = sym_type_string;
	r_t.s_v.str = alex_string(tn_s.s_ptr);
	r_t.name = alex_string(name);

	return r_t;
}