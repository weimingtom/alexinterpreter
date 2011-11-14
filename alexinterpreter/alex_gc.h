#ifndef  _ALEX_GC_H_
#define  _ALEX_GC_H_

#include "alex_sym.h"
#include "alex_conf.h"

#define GC_STR_TABLE_LEN		(128)
#define GC_CLEAR_LEN			(256)

// gc level
typedef enum _e_gc_level{
	GC_DEAD,			// 常量
	GC_LIVE				// 变量
}e_gc_level;

typedef union _sg_value{
	alex_al* al;
	ALEX_STRING str;
}sg_value;

typedef struct _g_value{
	sg_value sg_v;
	byte sg_t;
}g_value; 

typedef struct _gc_node{
	g_value gc_value;
	int		gc_count;
	e_gc_level gc_level;

	struct _gc_node* next;
}gc_node;

typedef struct _str_node{
	ALEX_STRING str;
	gc_node*   gc_p;
	struct _str_node* next;
}str_node;

typedef struct _str_table{
	str_node* str_ptr[GC_STR_TABLE_LEN];
}str_table;

typedef struct _a_gc{
	gc_node* gc_head;
	int		 gc_size;
	
	str_table gc_str_table;
}a_gc;

extern a_gc alex_gc;
r_value gc_new_string(char* str, e_gc_level gc_l);
#define check_l_gc(p)  do{ if((p)->gc_p) ((gc_node*)((p)->gc_p))->gc_count--; }while(0)
#define check_r_gc(p)  do{ if((p)->gc_p) ((gc_node*)((p)->gc_p))->gc_count++; }while(0)
#define _gc_back(gs)	   (alex_gc.gc_size < (gs))?(0):(_gc_back_());
#define gc_back()	    _gc_back(GC_CLEAR_LEN)

r_value gc_new_al(int count);
int _gc_back_();

#endif