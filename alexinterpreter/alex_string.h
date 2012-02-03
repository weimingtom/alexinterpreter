#ifndef  _ALEX_STRING_H_
#define _ALEX_STRING_H_
#include <string.h>

typedef struct _a_string{
	char*  s_ptr;			// 字符串指针
	int	s_len;				// 字符串已用长度
//	int s_size;				// 字符串缓存大小
}a_string;


char* cat_char(a_string* a_s, char one_char);
char* cat_string(a_string* a_s, char* add_str);
char* cat_a_string(a_string* a_s, a_string add_a_s);
int alex_strcmp(a_string a_s, char* str);
a_string alex_string(char* str);
a_string* relloc_string(a_string* a_s, int a_size);
void free_string(a_string* a_s);


#endif