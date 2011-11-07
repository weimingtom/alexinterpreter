#include "alex_gc.h"
#include "alex_sym.h"
#include "alex_symbol_table.h"
#include "alex_log.h"
#include <stdlib.h>

#define gc_hash(str) _BKDRHash((str), GC_STR_TABLE_LEN)
a_gc alex_gc = {0};


gc_node* gc_add(g_value g_v, e_gc_level gc_l);
g_value gc_new_g_v_str(char* str);

int gc_del_str(char* str)
{
	unsigned int index = 0;
	str_node* str_b = NULL;
	str_node* str_p = NULL;
	
	if(str==NULL)
		return 0;
	
	index = gc_hash(str);
	str_p = alex_gc.gc_str_table.str_ptr[index];
	str_b = str_p;

	while(str_p && str_p->next)
	{
		if(alex_strcmp(str_p->str, str)==0)
		{
			str_b->next = str_p->next;
			free(str_p);
			return 1;
		}

		str_b = str_p;
		str_p = str_p->next;
	}

	return 0;

}

gc_node*  gc_add_str_table(char* str, e_gc_level gc_l)
{
	unsigned int index =0;
	str_node* str_p = NULL;
	gc_node* gc_p = NULL;
	str_node* t_s_n = NULL;

	if(str==NULL)
		return NULL;

	index = gc_hash(str);

	str_p = alex_gc.gc_str_table.str_ptr[index];

	while(str_p && str_p->next)
	{
		if(alex_strcmp(str_p->str, str)== 0)
		{
			str_p->gc_p->gc_count++;
			return str_p->gc_p;
		}
		str_p = str_p->next;
	}
	
	t_s_n = (str_node*)malloc(sizeof(str_node));
	memset(t_s_n, 0, sizeof(str_node));
	gc_p = gc_add(gc_new_g_v_str(str), gc_l);
	t_s_n->str = gc_p->gc_value.sg_v.str;

	if(str_p==NULL)
	{	
		alex_gc.gc_str_table.str_ptr[index] = t_s_n;
	}
	else
	{
		str_p->next = t_s_n;	
	}

	return gc_p;
}


g_value gc_new_g_v_str(char* str)
{
	g_value ret = {0};

	ret.sg_t = sym_type_string;
	ret.sg_v.str = alex_string(str);

	return ret;
}

gc_node* gc_add(g_value g_v, e_gc_level gc_l)
{
	gc_node* t_gc_node = (gc_node*)malloc(sizeof(gc_node));
	memset(t_gc_node, 0, sizeof(gc_node));
	
	t_gc_node->gc_value = g_v;
	t_gc_node->gc_level = gc_l;

	t_gc_node->next = alex_gc.gc_head;
	alex_gc.gc_head = t_gc_node;

	return t_gc_node;
}

r_value gc_new_string(char* str, e_gc_level gc_l)
{
	gc_node* gc_p = NULL;
	r_value ret = {0};

	if(str==NULL || (gc_p=gc_add_str_table(str, gc_l))==0)
		return ret;

	ret.r_t = sym_type_string;
	ret.r_v.str = gc_p->gc_value.sg_v.str;
	ret.gc_p = gc_p;

	return ret;
}


int gc_back()
{
	gc_node* gc_p= alex_gc.gc_head;
	gc_node* gc_b = gc_p;

	if(alex_gc.gc_size < GC_CLEAR_LEN)
		return 0;

	while(gc_p)
	{
		if(gc_p->gc_level==GC_DEAD || gc_p->gc_count > 0)
		{
			gc_b = gc_p;
			gc_p = gc_p->next;
			continue;
		}
		
		switch(gc_p->gc_value.sg_t)
		{
		case sym_type_string:
			{
				gc_node* now_n = gc_p->next;
				gc_b->next = now_n;

				gc_del_str(gc_p->gc_value.sg_v.str.s_ptr);
				free_string(&gc_p->gc_value.sg_v.str);
				free(gc_p);
				gc_p = now_n;
			}
			break;
		case sym_type_al:
			break;
		default:
			print("gc[error], not konw gc_value!\n");
			return 1;
		}	
	}

	return 0;
}


