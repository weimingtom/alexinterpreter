#ifndef  _ALEX_GC_H_
#define  _ALEX_GC_H_

#include "alex_conf.h"

#define GC_STR_TABLE_LEN		(128)


// gc level
typedef enum _e_gc_level{
	GC_DEAD,			// 常量
	GC_LIVE				// 变量
}e_gc_level;

typedef struct _str_node{
	ALEX_STRING str;

	struct _str_node* next;
}str_node;

typedef struct _str_table{
	str_node str_ptr[GC_STR_TABLE_LEN];
}str_table;

typedef struct 

typedef struct _gc_node{
	r_value gc_value;
	int		gc_count;
	e_gc_level gc_level;

	struct _gc_node* next;
}gc_node;


typedef struct _a_gc{
	gc_node* gc_head;
	int		 gc_size;
	
	str_table gc_str_table;
}a_gc;

extern a_gc alex_gc;

#endif