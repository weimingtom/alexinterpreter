#include "alex_string.h"
#include "string.h"
#include "stdlib.h"
#include "alex_conf.h"

/*
	ALEX 字符串处理
*/

// 拼接字符串
char* cat_char(a_string* a_s, char one_char)
{
	if(a_s == NULL)
		return NULL;

	relloc_string(a_s, sizeof(one_char));
	a_s->s_ptr[a_s->s_len++] = one_char;

	return a_s->s_ptr;
}

// 
char* cat_string(a_string* a_s, char* add_str)
{
	char* t_a_s_p= NULL;

	if(a_s == NULL)
		return NULL;

	relloc_string(a_s, strlen(add_str));
	t_a_s_p = &a_s->s_ptr[a_s->s_len];

	while(*add_str)
		*t_a_s_p++ = *add_str++, a_s->s_len++;

	return a_s->s_ptr;
}

char* cat_a_string(a_string* a_s, a_string add_a_s)
{
	char* t_a_s_p= NULL;
	
	if(a_s == NULL)
		return NULL;
	
	relloc_string(a_s, add_a_s.s_len);
	t_a_s_p = &a_s->s_ptr[a_s->s_len];
	
	while(*add_a_s.s_ptr)
		*t_a_s_p++ = *add_a_s.s_ptr++, a_s->s_len++;
	
	return a_s->s_ptr;
}


a_string* relloc_string(a_string* a_s, int a_size)
{
	if(a_s == NULL)
		return NULL;
	
	if((a_s->s_size - a_s->s_len-1)<a_size )
	{
		char* new_ptr = NULL;

		a_s->s_size += 64*(1+ (a_size- a_s->s_size +a_s->s_len+1)/64 );
		new_ptr = (char*)a_malloc(a_s->s_size);
		memset(new_ptr, 0, a_s->s_size);
		strncpy(new_ptr, a_s->s_ptr, a_s->s_len); 
		a_free(a_s->s_ptr);
		a_s->s_ptr = new_ptr;
	}

	return a_s;
}

// 生成 a_string
a_string alex_string(char* str)
{
	a_string a_s = {0};
	char* t_a_s_ptr = NULL;

	if (str == NULL)
		return a_s;

	relloc_string(&a_s, strlen(str));
	t_a_s_ptr = a_s.s_ptr;

	while(*str)
		*t_a_s_ptr++ = *str++, a_s.s_len++;

	return a_s;
}

int alex_strcmp(a_string a_s, char* str)
{
	if(a_s.s_ptr == NULL || str == NULL)
		return 1;

	return strcmp(a_s.s_ptr, str);
}

//
void free_string(a_string* a_s)
{
	if(a_s == NULL)
		return;
	
	if(a_s->s_ptr)
		a_free(a_s->s_ptr);

	memset(a_s, 0, sizeof(a_string));
}