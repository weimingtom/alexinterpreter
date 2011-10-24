#ifndef  _ALEX_CONF_H_
#define _ALEX_CONF_H_
#include <stdio.h>
#include "alex_symbol_table.h"
#include "stdio.h"
#include "alex_string.h"

// ����code����
typedef struct _code_buff{
	char* code_ptr;				// �����ļ�������ʼ��ַ
	long code_size;				// �ļ��ߴ��С
	char* read_code_ptr;		// ��ȡ�ļ�����ָ��
	int line;					// ��ǰ����
}code_buff;

extern code_buff c_b;		

#define at_char(_c_b_)		(*((_c_b_).read_code_ptr) )
#define next_char(_c_b_)   ( ((_c_b_).read_code_ptr)?(*(++(_c_b_).read_code_ptr) ):(0) )
#define back_char(_c_b_)	( ((_c_b_).read_code_ptr)?(*(--(_c_b_).read_code_ptr) ):(0) )
#define check_next_type(_c_b_)	( ((_c_b_).read_code_ptr)?( ascii_type[*((_c_b_).read_code_ptr+1)] ):(type_null) )


typedef  double			ALEX_NUMBER;
typedef  a_string		ALEX_STRING;
typedef  a_string		ALEX_NAME;
typedef  void*			ALEX_FUNC;
typedef void*			ALEX_P;
#define  a_print	   printf	


typedef unsigned char ubyte;

long fsize( FILE *fp);			// ��õ�ǰ�ļ���С
code_buff get_code_buff(long  code_size);
void free_code_buff();

#endif