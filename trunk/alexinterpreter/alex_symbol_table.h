#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

		
// ASCII������ݣ�����
enum c_t{
	type_null,					// ���Ϸ�ascii
	type_space,					// �ո� ���� table
	type_char,					// a-z A-Z
	type_num,					// 0-9
	type_oper,					// +,-,*,/, & | 

	type_seg,
	type_al,					// ����
	type_end,
	type_note					//  # ע��
};

extern char ascii_type[128];

#define get_ascii_type(one_char)	(ascii_type[one_char])		// ���һ��ascii������

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


// ������� hash table ����
#define SYMBOL_HASH_TABLE_LEN			48
// ����ؼ����������������ַ�����
#define MAX_SYMBOL_LEN					24


//  ����char* ��ǰ������, int ��ǰ����������, int��ǰ��������ȡ������
typedef int (*symbol_os)(char*, int, int);
typedef unsigned char byte;


// ���ű�ڵ�
typedef struct _sym_node
{
	char symbol_info[MAX_SYMBOL_LEN];				// �����ַ���Ϣ
	byte symbol_type;								// ��������
	struct _sym_node* next;
}sym_node;



void  alex_init_symbol_table();						// ��ʼ�����ű���
unsigned int _BKDRHash(char *str, int len);
unsigned int BKDRHash(char *str);					// HASH������������
int   alex_add_hash(sym_node* add_sym);				// ��ӷ��ű����ӿ�(��ʼ��ʹ��)
sym_node* alex_search_sym(char* search_info);			// ���ҷ��ű����ӿ�
void init_ascii_table();
void free_symbol_table();



#endif 