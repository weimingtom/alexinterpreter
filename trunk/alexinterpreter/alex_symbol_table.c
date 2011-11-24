#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alex_symbol_table.h"
#include "alex_get_token.h"


// 全局符号表
static sym_node*  symbol_table[SYMBOL_HASH_TABLE_LEN] = {0};
char ascii_type[128] = {0};
// 初始化符号表
void alex_init_symbol_table()
{
	int i;
	sym_node* tmp = NULL;
	int st=0;
	char* symbol_list[]={

		// 关键字
		"var",					// 变量声明类型关键字		
		"function",				// 函数声明关键字				
		"if",					// if语句						
		"else",					// else语句						
		"while",				// while语句					
		"continue",				// continue语句					
		"break",				// break语句					
		"sizeof",				// sizeof运算符					
		"null",					// 空
		"true",
		"return",
		"false",
		"using",				// 引入dll库函数

		"+",					
		"-",
		"*",
		"/",
		"%",
		"=",
		"(",
		")",
		"++",
		"--",
		"+=",
		"-=",
		

		"&&",
		"||",
		">",
		"<",
		">=",
		"<=",
		"==",
		"!=",
		",",
		"#"
	};


	for(i=0; i<(sizeof(symbol_list)/sizeof(char*)); i++)
	{
		tmp = (sym_node *)malloc(sizeof(sym_node));
		tmp->next=NULL;

		strcpy(tmp->symbol_info, symbol_list[i]);
		tmp->symbol_type = i+token_type_var;
		alex_add_hash(tmp);
	}

	init_ascii_table();
}


// 初始化ascii 类型表
void init_ascii_table()
{
	int i=0;

	ascii_type[' '] = type_space;
	ascii_type['\n'] = type_space;
	ascii_type['\t'] = type_space;

	for(i=ASCII_CHAR0_BEGIN_OFFSET; i<ASCII_CHAR0_END_OFFSET; i++)
		ascii_type[i] = type_char;

	for(i=ASCII_CHAR1_BEGIN_OFFSET; i<ASCII_CHAR1_END_OFFSET; i++)
		ascii_type[i] = type_char;

	for (i='0'; i<='9'; i++)
		ascii_type[i] = type_num;

	ascii_type['_'] = type_char;

	ascii_type['+'] = type_oper;
	ascii_type['-'] = type_oper;
	ascii_type['*'] = type_oper;
	ascii_type['/']	= type_oper;
	ascii_type['%'] = type_oper;
	ascii_type['='] = type_oper;
	ascii_type['&'] = type_oper;
	ascii_type['|'] = type_oper;
	ascii_type['>'] = type_oper;
	ascii_type['<'] = type_oper;
	ascii_type['!'] = type_oper;
	ascii_type['('] = type_oper;
	ascii_type[')'] = type_oper;
	ascii_type[','] = type_oper;
	
	ascii_type['{'] = type_seg;
	ascii_type['}'] = type_seg;
	ascii_type['['] = type_al;
	ascii_type[']'] = type_al;
	ascii_type['\"'] = type_seg;
	ascii_type[';'] = type_end;
	ascii_type['#'] = type_note;
	
}


unsigned int _BKDRHash(char *str, int len)
{
	unsigned int seed = 131; 
	unsigned int hash = 0;
	
	while (*str)
	{
		hash = hash * seed + (*str++);
	}
	
	return (hash & 0x7FFFFFFF)%(len);
}



// BKDR hash函数
unsigned int BKDRHash(char *str)
{
	return _BKDRHash(str, SYMBOL_HASH_TABLE_LEN);
}


int  alex_add_hash(sym_node* add_sym)
{
	sym_node* head=NULL;

	// 获取hash table索引 
	unsigned int index = BKDRHash(add_sym->symbol_info);

	if(index<0 || index>=SYMBOL_HASH_TABLE_LEN)
		return 0;


	head= symbol_table[index];
	if(symbol_table[index] == 0)
	{
		symbol_table[index]=add_sym;
		return 1;
	}
	
	while(head->next != NULL)
	{
		head=head->next;
	}
	head->next=add_sym;
	return 1;
}

sym_node* alex_search_sym(char* search_info)
{
	sym_node* head=NULL;
	// 获取hash table索引 
	unsigned int index = BKDRHash(search_info);
	
	if(index<0 || index>=SYMBOL_HASH_TABLE_LEN)
		return NULL;
	
	
	head= symbol_table[index];
	if(symbol_table[index] == 0)
	{
		return NULL;
	}
	
	while(head != NULL)
	{
		if(strcmp(head->symbol_info, search_info)==0)
			return head;

		head=head->next;
	}

	return NULL;
}


/*
void print_table()
{
int i;
sym_node* head = NULL;
for(i=0;i<SYMBOL_HASH_TABLE_LEN;i++)
{

		if( symbol_table[i]!=0 )
		{
		head=symbol_table[i];
		while(head !=NULL)
		{
		printf("index %d sym: %s\n", i, head->data.symbol_info);
		head= head->next;
		}
		}
		}
		}
		
		  int main(void)
		  {
		  alex_init_symbol_table();
		  print_table();
		  return 0;
		  }
*/
