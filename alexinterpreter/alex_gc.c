#include "alex_gc.h"
#include "alex_sym.h"
#include "alex_symbol_table.h"
#include "alex_arrylist.h"
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

	while(str_p)
	{
		if(alex_strcmp(str_p->str, str)==0)
		{
			str_b->next = str_p->next;
			if(str_b==str_p)
				alex_gc.gc_str_table.str_ptr[index] = str_p->next;
			free(str_p);
			return 1;
		}

		str_b = str_p;
		str_p = str_p->next;
	}

	return 0;
}

int gc_del_al(alex_al* al_p)
{
	int i=0;
	if(al_p==NULL)
		return 0;
	
	for(i=0; i<al_p->al_len; i++)
		check_l_gc(&(al_p->al_v[i]));
	
	free(al_p->al_v);
	free(al_p);
	
	return 1;
}

gc_node*  gc_add_str_table(char* str, e_gc_level gc_l)
{
	unsigned int index =0;
	str_node* str_p = NULL;
	str_node* back_p = NULL;
	gc_node* gc_p = NULL;
	str_node* t_s_n = NULL;

	if(str==NULL)
		return NULL;

	index = gc_hash(str);
	str_p = alex_gc.gc_str_table.str_ptr[index];
	back_p = str_p;

	while(str_p)
	{
		if(alex_strcmp(str_p->str, str)== 0)
			return str_p->gc_p;
		
		back_p = str_p;
		str_p = str_p->next;
	}
	
	t_s_n = (str_node*)malloc(sizeof(str_node));
	memset(t_s_n, 0, sizeof(str_node));
	gc_p = gc_add(gc_new_g_v_str(str), gc_l);
	t_s_n->str = gc_p->gc_value.sg_v.str;
	t_s_n->gc_p = gc_p;

	if(back_p== NULL)
	{	
		alex_gc.gc_str_table.str_ptr[index] = t_s_n;
	}
	else
	{
		back_p->next = t_s_n;	
	}

	return gc_p;
}

g_value gc_new_g_v_al(int count)
{
	g_value ret = {0};

	ret.sg_t = sym_type_al;
	ret.sg_v.al = _new_al(count);

	return ret;
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
	
	alex_gc.gc_size++;
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

r_value gc_new_al(int count)
{
	gc_node* gc_p = NULL;
	r_value ret = {0};

	if( (gc_p=gc_add(gc_new_g_v_al(count), GC_LIVE))==0 )
		return ret;

	ret.r_t = sym_type_al;
	ret.r_v.al = gc_p->gc_value.sg_v.al;
	ret.gc_p = gc_p;

	return ret;
}


void gc_print()
{
	char* tt[] = {
		"dead",
		"live"
	};
	gc_node* gc_p = alex_gc.gc_head;

	print("\n------gc print------ \n");
	while(gc_p)
	{
		print("node:LEVE[%s] type[%d] count[%d]-> \"%s\"\n", tt[gc_p->gc_level], gc_p->gc_value.sg_t, gc_p->gc_count, (gc_p->gc_value.sg_t==sym_type_string)?(gc_p->gc_value.sg_v.str.s_ptr):(" al "));
		gc_p = gc_p->next;
	}
	print("\n------gc print end----\n");
}

int _gc_back_()
{
	gc_node* gc_p= alex_gc.gc_head;
	gc_node* gc_b = gc_p;


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
				if(gc_p==alex_gc.gc_head)
					alex_gc.gc_head = now_n;
				gc_p = now_n;
			}
			break;
		case sym_type_al:
			{
				gc_node* now_n = gc_p->next;
				gc_b->next = now_n;
				
				gc_del_al(gc_p->gc_value.sg_v.al);
				free(gc_p);
				if(gc_p==alex_gc.gc_head)
					alex_gc.gc_head = now_n;
				gc_p = now_n;
			}
			break;
		default:
			print("gc[error], not konw gc_value!\n");
			return 1;
		}
		
		alex_gc.gc_size--;
	}

	alex_gc.c_size = 0;
	return 0;
}


