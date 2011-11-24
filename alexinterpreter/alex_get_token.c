/**
进行词法分析， 生成token_list t_l 传送到语法分析进行处理

*/
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include "alex_get_token.h"
#include "alex_log.h"
#include "alex_conf.h"
#include "alex_string.h"

char *_token_type_str[]={
	"token_type_err",				// 错误的token类型
	
	"token_type_var",				// 关键字类型
	"token_type_func",
	"token_type_if",
	"token_type_else",
	"token_type_while",
	"token_type_continue",
	"token_type_break",
	"token_type_sizeof",
	"token_type_null",
	"token_type_true",
	"token_type_return",
	"token_type_false",
	"token_type_using",
	
	"token_type_add",			// 运算符类型
	"token_type_sub",
	"token_type_mul",
	"token_type_div",
	"token_type_mod",
	"token_type_ass",
	"token_type_lbra",
	"token_type_rbra",
	"token_type_sadd",
	"token_type_ssub",
	"token_type_adds",
	"token_type_subs",
	
	"token_type_and",			// 逻辑运算符
	"token_type_or",
	"token_type_big",
	"token_type_lit",
	"token_type_bige",
	"token_type_lite",
	"token_type_equ",
	"token_type_nequ",
	"token_type_comma",
	
	"token_type_end",				// 终结符类型
	"token_type_lseg",				// 区域运算符
	"token_type_rseg",
	"token_type_ide",				// 标识符类型
	"token_type_num",				// 立即数类型
	"token_type_string"			// 字符串类型
		
};
token_list t_l = {0};



int alex_read_source_code(const char* source_code_string)
{
	long code_size = 0;
	int read_ret = 0;
	FILE*  source_fd = NULL;
	source_fd = fopen(source_code_string, "r");
	
	if(source_fd==NULL)
	{
		print("open %s is error!\n", source_code_string);
		return 0;
	}

	print("begin read source....\n");

	code_size = fsize(source_fd);		// 获得当前load文件的大小
	c_b = get_code_buff(code_size);

	read_ret = fread(c_b.read_code_ptr, sizeof(char), c_b.code_size, source_fd);
	if(read_ret <0)
	{
		print("read the file is error!\n");
		free_code_buff();
		return 0;
	}
	else
	{
		alex_get_token(c_b);
	}
	

	if(source_fd!=NULL)
	{
		fclose(source_fd);
		source_fd=NULL;
	}

	free_code_buff();
	return 1;
}


//  从line_buf中解析出每一个token
void alex_get_token(code_buff c_bf)
{
	char one_char = 0;

	while(one_char = at_char(c_bf))
	{
		switch(get_ascii_type(one_char))
		{
		case type_space:		// 过滤 空格换行
			{
				switch(one_char)
				{
				case '\n':
					c_bf.line++;
				default:
					next_char(c_bf);
				}
				
			}
			break;
		case type_note:
			while(at_char(c_bf) && at_char(c_bf)!='\n')
			{
				next_char(c_bf);
			}
			break;
		case type_oper:
			{
				add_token(get_oper_token(&c_bf));
			}
			// 解析操作运算符
			break;
		case type_num:
			{
				add_token(get_num_token(&c_bf));
			}
			// 解析数字
			break;
			// 解析数组
		case type_al:
			{
				add_token(get_al_token(&c_bf));
			}
			break;
		case  type_seg:
			{
				switch(one_char)
				{
				case '\"':
					add_token(get_string_token(&c_bf));
					break;
				case '{':
				case '}':
					add_token(get_seg_token(&c_bf));
					break;
				}
			}
			// 解析代码块
			break;
		case type_end:
			{
				add_token(get_end_token(&c_bf));
			}
			break;
		case type_char:
			{
				add_token(get_ide_token(&c_bf));
			}
			break;
		default:
			next_char(c_bf);
			break;
		}
	}
}


void add_token(token tk)
{
	token_node* t_n = (token_node*)malloc(sizeof(token_node));
	t_n->next = NULL;

	t_n->tk = tk;

	if(t_l.token_head == NULL)
	{
		t_l.token_head = t_n;
		t_l.token_read = t_l.token_head;		// 记录read指针
	}

	if(t_l.token_end == NULL)
		t_l.token_end = t_n;

	if(t_l.token_end != t_n)
	{
		t_l.token_end->next = t_n;
		t_l.token_end = t_n;
	}
 
}



// 生成 num token
token get_num_token(code_buff* c_bf)
{
	char one_char = 0;
	token  new_token = {0};
	
	new_token.token_line = c_bf->line;
	new_token.token_type = token_type_num;
	while(one_char=at_char(*c_bf))
	{
		if(get_ascii_type(one_char) != type_num && one_char !='.')
		{
			new_token.token_value.number = atof(new_token.token_name.s_ptr);
			return new_token;
		}
		
		cat_char(&new_token.token_name, one_char);
		next_char(*c_bf);
	}

	return new_token;
}

// 生成 标识符 token
token get_ide_token(code_buff* c_bf)
{
	char one_char = 0;
	token  new_token = {0};
	sym_node* s_n = NULL;
	
	new_token.token_line = c_bf->line;

	while(one_char=at_char(*c_bf))
	{
		if(get_ascii_type(one_char) != type_num && get_ascii_type(one_char) != type_char)
		{
CHECK_IDE:
			s_n = alex_search_sym(new_token.token_name.s_ptr);
			if(s_n)
			{
				new_token.token_type = s_n->symbol_type;
				new_token.token_value.name = alex_string(new_token.token_name.s_ptr);
			}
			else
			{
				new_token.token_type = token_type_ide;
				new_token.token_value.name = alex_string(new_token.token_name.s_ptr);
			}
			return new_token;
		}

		cat_char(&new_token.token_name, one_char);
		next_char(*c_bf);
	}

	goto CHECK_IDE;
}


// 生成字符串token
token  get_string_token(code_buff* c_bf)
{
	char one_char = 0;
	token  new_token = {0};
	
	new_token.token_line = c_bf->line;

	if(at_char(*c_bf)!='\"')
		return new_token;

	next_char(*c_bf);
	while(one_char=at_char(*c_bf))
	{
		if(one_char=='\"')
		{
			new_token.token_type = token_type_string;
			new_token.token_value.str = alex_string(new_token.token_name.s_ptr);
			next_char(*c_bf);
			return new_token;
		}
		else if (one_char == '\\')
		{
			next_char(*c_bf);
			switch(at_char(*c_bf))
			{
			case 'n':
				cat_char(&new_token.token_name, '\n');
				break;
			case 't':
				cat_char(&new_token.token_name, '\t');
				break;
			case '\"':
				cat_char(&new_token.token_name, '\"');
				break;
			case '\\':
				cat_char(&new_token.token_name, '\\');
				break;
			default:
				goto END_STRING;
			}
			next_char(*c_bf);
			continue;
		}
		else if (one_char == '\n')
		{
			goto END_STRING;
		}
		cat_char(&new_token.token_name, one_char);
		next_char(*c_bf);
	}
	
END_STRING:
	// 无法找到最后的" 错误的token数据
	new_token.token_type = token_type_err;
	cat_string(&new_token.token_name, "   can not find string end \" ");
	return new_token;
}


// 生成运算符
token get_oper_token(code_buff* c_bf)
{
	char t_oper[4] = {0};
	int i=0;
	char one_char = 0;
	token  new_token = {0};
	sym_node* s_n = NULL;

	
	cat_char(&new_token.token_name, one_char=at_char(*c_bf));
	t_oper[0] = one_char;
	s_n = alex_search_sym(t_oper);
	if(s_n)
		new_token.token_type =s_n->symbol_type;
	one_char=next_char(*c_bf);
	
	
	if( get_ascii_type(one_char)== type_oper )
	{
		t_oper[1] = one_char;
		if(s_n = alex_search_sym(t_oper))
		{
			cat_char(&new_token.token_name, one_char);
			new_token.token_type =s_n->symbol_type;
			next_char(*c_bf);
		}
	}
	
	new_token.token_line = c_bf->line;
	return new_token;
}

// 生成数组运算符
token get_al_token(code_buff* c_bf)
{
	char one_char = 0;
	token  new_token = {0};
	
	cat_char(&new_token.token_name, at_char(*c_bf));
	if(at_char(*c_bf) == '[')
		new_token.token_type = token_type_lal;
	else if(at_char(*c_bf) == ']')
		new_token.token_type = token_type_ral;
	new_token.token_value.name = alex_string(new_token.token_name.s_ptr);
	next_char(*c_bf);
	
	new_token.token_line = c_bf->line;
	return new_token;
}

// 生成域运算符
token get_seg_token(code_buff* c_bf)
{
	char one_char = 0;
	token  new_token = {0};

	cat_char(&new_token.token_name, at_char(*c_bf));
	if(at_char(*c_bf) == '{')
		new_token.token_type = token_type_lseg;
	else if(at_char(*c_bf) == '}')
		new_token.token_type = token_type_rseg;
	new_token.token_value.name = alex_string(new_token.token_name.s_ptr);
	next_char(*c_bf);

	new_token.token_line = c_bf->line;
	return new_token;
}

token  get_end_token(code_buff* c_bf)
{
	char one_char = 0;
	token  new_token = {0};
	
	cat_char(&new_token.token_name, at_char(*c_bf));
	new_token.token_type = token_type_end;
	new_token.token_value.name = alex_string(new_token.token_name.s_ptr);
	next_char(*c_bf);

	new_token.token_line = c_bf->line;
	return new_token;
}



void print_token()
{
	print("-----BEGIN PRINT TOKEN------\n");
	if(t_l.token_head == NULL)
		return;
	
	while(t_l.token_read)
	{
		print("line: %d   %s     ", t_l.token_read->tk.token_line, alex_get_type(t_l.token_read->tk.token_type));
		print(": %s\n", t_l.token_read->tk.token_name);
		t_l.token_read = t_l.token_read->next;
	}
	
	print("------END PRINT TOKEN-------\n");
	t_l.token_read = t_l.token_head;

}


 


