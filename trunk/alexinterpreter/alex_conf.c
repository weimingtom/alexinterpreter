#include "alex_conf.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

code_buff c_b = {0};
int mem_count=0;

// 获取文件字节大小
long fsize( FILE *fp)
{
    long int save_pos;
    long size_of_file;
	
	if (fp == NULL)
		return 0;

    save_pos = ftell( fp );			// 保存当前文件指针地址

    fseek( fp, 0L, SEEK_END );		// 跳转到文件末尾
    size_of_file = ftell( fp );		// 获取文件开始末尾文件地址
    fseek( fp, save_pos, SEEK_SET ); // 恢复当前的文件地址
	
    return( size_of_file);
}



// 获得code_buff
code_buff get_code_buff(long  code_size)
{
	if(c_b.code_size >= (code_size-2) )
	{
		c_b.read_code_ptr = c_b.code_ptr;
	}
	else
	{
		if(c_b.code_ptr != NULL)
			a_free(c_b.code_ptr);

		c_b.code_size = code_size+2;
		c_b.code_ptr = (char*)a_malloc(c_b.code_size);
		memset(c_b.code_ptr, 0, c_b.code_size);
		c_b.read_code_ptr = c_b.code_ptr;
	}
	
	c_b.line = 1;
	return c_b;
}


void free_code_buff()
{
	if(c_b.code_ptr != NULL)
		a_free(c_b.code_ptr);
	
	memset(&c_b, 0, sizeof(c_b));
}


void a_free(void* p)
{
	(p!=NULL)?(mem_count--, free(p)):(0);
}

void* a_malloc(size_t s_t)
{
	return (s_t!=0)?(mem_count++, malloc(s_t)):(NULL);
}