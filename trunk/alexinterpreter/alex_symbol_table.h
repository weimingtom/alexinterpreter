#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

		
// ASCII码表数据，定义
enum c_t{
	type_null,					// 不合法ascii
	type_space,					// 空格 换行 table
	type_char,					// a-z A-Z
	type_num,					// 0-9
	type_oper,					// +,-,*,/, & | 

	type_seg,
	type_al,					// 数组
	type_end,
	type_note					//  # 注释
};

extern char ascii_type[128];

#define get_ascii_type(one_char)	(ascii_type[one_char])		// 获得一个ascii的类型

#define ASCII_BEGIN_OFFSET				0 
#define ASCII_NULL_END_OFFSET			33

#define ASCII_OPER0_BEGIN_OFFSET		33
#define ASCII_OPER0_END_OFFSET			48

#define ASCII_NUM_BEGIN_OFFSET			48
#define ASCII_NUM_END_OFFSET			58

#define	ASCII_OPER1_BEGIN_OFFSET		58
#define ASCII_OPER1_END_OFFSET			65


#define  ASCII_CHAR0_BEGIN_OFFSET		65
#define  ASCII_CHAR0_END_OFFSET			91

#define ASCII_OPER2_BEGIN_OFFSET		91		
#define ASCII_OPER2_END_OFFSET			97

#define ASCII_CHAR1_BEGIN_OFFSET		97
#define ASCII_CHAR1_END_OFFSET			123

#define ASCII_OPER3_BEGIN_OFFSET		123
#define ASCII_OPER3_END_OFFSET			128


// 定义符号 hash table 表长度
#define SYMBOL_HASH_TABLE_LEN			48
// 定义关键字与操作运算符的字符长度
#define MAX_SYMBOL_LEN					24


//  参数char* 当前缓冲区, int 当前缓冲区长度, int当前缓冲区读取的索引
typedef int (*symbol_os)(char*, int, int);
typedef unsigned char byte;


// 符号表节点
typedef struct _sym_node
{
	char symbol_info[MAX_SYMBOL_LEN];				// 符号字符信息
	byte symbol_type;								// 符号类型
	struct _sym_node* next;
}sym_node;



void  alex_init_symbol_table();						// 初始化符号表函数
unsigned int _BKDRHash(char *str, int len);
unsigned int BKDRHash(char *str);					// HASH函数生成因子
int   alex_add_hash(sym_node* add_sym);				// 添加符号表函数接口(初始化使用)
sym_node* alex_search_sym(char* search_info);			// 查找符号表函数接口
void init_ascii_table();
void free_symbol_table();



#endif 