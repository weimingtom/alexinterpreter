#ifndef  _ALEX_VALUE_SYMBOL_TABLE_H_
#define  _ALEX_VALUE_SYMBOL_TABLE_H_
#include "alex_conf.h"

typedef union _sym_value_{
	ALEX_NUMBER num;
	ALEX_STRING str;
	ALEX_FUNC func;
}_sym_value;

enum _sym_type{
	sym_num,			// number
	sym_string,			// string
	sym_func			//function
};



typedef struct _sym
{
	a_string sym_name;			// ��������
	byte sym_type;				// ��������
	_sym_value sym_value;		// ����ֵ
}sym;


typedef struct _sym_node_{
	sym  sym_data;
	struct _sym_node_* next;
}_sym_node;


#endif