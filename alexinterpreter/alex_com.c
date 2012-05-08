#include "alex_com.h"
#include "alex_vm.h"
#include "alex_log.h"
#include "alex_gc.h"
#include "alex_lib.h"

#include <stdlib.h>


com_env* com_env_p = NULL;

char* sym_str[] = {
	"sym_type_error",
	"num",
	"string",
	"func",
	"reg_func",	// reg func
	"al",
	"alp",
	"pointer",
	"addr",
	"arg_num"
};

char* gl_str[] = {
	"COM_ERROR",	
	"COM_GLOBAL",
	"COM_LOCAL",	
	"COM_VALUE",
	"COM_POINT",
	"COM_REG"
};

char* com_str[] = {
	"END",		
	"PUSH",	
	"VAR",
	"PUSHVAR",	
	"POP",		
	"NEWAL",
	"AL",			
	"JFALSE",		
	"JTRUE",		
	"MOVE",		
	"MOVEAL",		
	"MOVEREG",	
	"TABLE",		
	"ADD",	
	"SUB",	
	"MUL",		
	"DEV",		
	"MOD",		
	"AND",		
	"OR",			
	"BIG",		
	"BIGE",		
	"LIT",		
	"LITE",		
	"EQU",		
	"NEQU",		
	"CALL",		
	"JUMP",		
	"RET",
	"SADD",
	"BSADD",
	"SSUB",
	"BSSUB"
};

s_addr* top_s_addr(addr_data* a_d);
addr_data* relloc_s_addr(addr_data* a_d, int a_len);
int com_func_def(com_env* com_p, tree_node* t_n);
int com_func_call(com_env* com_p, tree_node* t_n);
int com_arg_def(com_env* com_p, tree_node* t_n);
int com_seg(com_env* com_p, tree_node* t_n);
int com_return_assume(com_env* com_p, tree_node* t_n);
int com_return(com_env* com_p, tree_node* t_n);
int com_break(com_env* com_p, tree_node* t_n);
int com_continue(com_env* com_p, tree_node* t_n);
int com_if(com_env* com_p, tree_node* t_n);
int com_while(com_env* com_p, tree_node* t_n);
int com_exp_stmt(com_env* com_p, tree_node* t_n);
int com_exp(com_env* com_p, tree_node* t_n);
int com_arg(com_env* com_p, tree_node* t_n);
int com_ass(com_env* com_p, tree_node* t_n);
int	com_op_logic(com_env* com_p, tree_node* t_n);
int com_op_value(com_env* com_p, tree_node* t_n);
int com_op_one(com_env* com_p, tree_node* t_n);
int com_aldef(com_env* com_p, tree_node* t_n);
void add_back_addr(s_addr* s_a, int b_addr);
void push_s_addr(addr_data* a_d, s_addr s_a);
s_addr new_s_addr(int addr);
int pop_s_addr(addr_data* a_d);
void free_s_addr(s_addr* s_p);
void clear_local_addr(com_env* com_p);


com_env* new_com_env()
{
	com_env* ret_c_e_p = (com_env*)a_malloc(sizeof(com_env));
	memset(ret_c_e_p, 0, sizeof(com_env));

	relloc_code(&(ret_c_e_p->com_inst));
	ret_c_e_p->var_table.g_table = global_table;
	relloc_global(&ret_c_e_p->var_table.global_ptr);

	ret_c_e_p->var_table.g_top=0;

	ret_c_e_p->var_table.l_table = new_table();
	ret_c_e_p->var_table.l_top=0;

	relloc_s_addr(&ret_c_e_p->var_table.addr_ptr, TEMP_MEM_LEN);
	return ret_c_e_p;
} 

void free_com_env()
{
	if(com_env_p==NULL)
		return;
	
	free_table(com_env_p->var_table.l_table);
	if(com_env_p->var_table.addr_ptr.root_ptr)
	{
		int i=0;
		for(i=0; i<com_env_p->var_table.addr_ptr.addr_len; i++)
			free_s_addr(com_env_p->var_table.addr_ptr.root_ptr+i);
		a_free(com_env_p->var_table.addr_ptr.root_ptr);
	}

	a_free(com_env_p);
	com_env_p = NULL;
}


// 编译code入口 main_tree为执行调用代码 func_tree为函数链表
int alex_com(com_env* com_p, tree_node* main_tree, tree_node* func_tree)
{
	if(main_tree == NULL)
		return	COM_ERROR_OTHER;;
	 
	// write begin pc
	com_p->pc = now_inst_addr(com_p);
	while(main_tree)
	{
		switch(type_tree(main_tree))
		{
		case bnf_type_using:
			{
				check_com(alex_reg_dll(main_tree->b_v.str.s_ptr));
			}
			break;
		case bnf_type_vardef:
			{
				check_com(com_g_vardef(com_p, main_tree));
			}
			break;
		case bnf_type_funccall:
			{
				check_com(com_func_call(com_p, main_tree));
				push_inst(&com_p->com_inst, new_inst(POP));
			}
			break;
		default:
			{
				print("com[error line %d]: the g_code not allow!\n", main_tree->line);
			}
			return	COM_ERROR_OTHER;
		}
		main_tree = main_tree->next;
	}
	
	// write end inst
	push_inst(&com_p->com_inst, new_inst(END));
	
	while(func_tree)
	{
		check_com(com_func_def(com_p, func_tree));
		func_tree = func_tree->next;
	}

	return COM_SUCCESS;
}


int com_func_def(com_env* com_p, tree_node* t_n)
{
	tree_node* arg_t_n = t_n->childs_p[0];
	tree_node* seg_t_n = t_n->childs_p[1];
	int now_addr = now_inst_addr(com_p);
	r_addr r_a = {0};
	clear_local_addr(com_p);
	r_a = search_addr(com_p, t_n->b_v.name.s_ptr);
	if(r_a.gl == COM_ERROR)
	{
		print("com[error line: %d] not find func:\"%s\" at globle!\n", t_n->line, t_n->b_v.name.s_ptr);
		return COM_ERROR_NOT_FIND_IDE;
	}
	com_p->var_table.global_ptr.root_ptr[r_a.addr] = new_addr(now_addr);		// write the func addr!
	check_com(com_arg_def(com_p, arg_t_n));
	check_com(com_seg(com_p, seg_t_n));

	// if the function is not return, so assume is number 0
	check_com(com_return_assume(com_p, t_n));
	return COM_SUCCESS;
}


int com_arg_def(com_env* com_p, tree_node* t_n)
{
	int i;
	int arg_num = (int)t_n->b_v.number;
	t_n = t_n->childs_p[0];

	// set addr
	while(t_n)
	{
		l_com_addr(com_p, t_n->b_v.name.s_ptr);
		push_inst(&com_p->com_inst, new_inst(VAR));
		t_n = t_n->next;
	}
	
	// push inst  get arg 
	for(i=arg_num-1; i>=0; i--)
	{
		push_inst(&com_p->com_inst, new_inst(MOVE, COM_LOCAL, i));
		push_inst(&com_p->com_inst, new_inst(POP));
	}

	return COM_SUCCESS;
}

int com_seg(com_env* com_p, tree_node* t_n)
{
	if(t_n->childs_p[0]==NULL)
	{
		print("com[error line: %d] the seg is nil!\n", t_n->line);
		return COM_ERROR_NOT_EXP;
	}

	t_n = t_n->childs_p[0];

	while(t_n)
	{
		switch(type_tree(t_n))
		{
		case bnf_type_seg:
			check_com(com_seg(com_p, t_n));
			break;
		case bnf_type_vardef:
			check_com(com_l_vardef(com_p, t_n));
			break;
		case bnf_type_return:
			check_com(com_return(com_p, t_n));
			break;
		case bnf_type_break:
			check_com(com_break(com_p, t_n));
			break;
		case bnf_type_continue:
			check_com(com_continue(com_p, t_n));
			break;
		case bnf_type_if:
			check_com(com_if(com_p, t_n));
			break;
		case bnf_type_while:
			check_com(com_while(com_p, t_n));
			break;
		default:
			check_com(com_exp_stmt(com_p, t_n));
			break;
		}
		t_n = t_n->next;
	}

	return COM_SUCCESS;
}

int com_return(com_env* com_p, tree_node* t_n)
{
	if(t_n->childs_p[0]==NULL)			// not return value
	{
		com_return_assume(com_p, t_n);
	}
	else
	{
		t_n = t_n->childs_p[0];
		check_com(com_exp(com_p, t_n));
		push_inst(&com_p->com_inst, new_inst(RET));
	}
	
	com_p->var_table.temp_reg[COM_REG_RET] = 1;
	return COM_SUCCESS;
} 


int com_return_assume(com_env* com_p, tree_node* t_n)
{
	r_value r_v = new_number(0);

	push_inst(&com_p->com_inst, new_inst(PUSH, r_v));
	push_inst(&com_p->com_inst, new_inst(RET));
	
	return COM_SUCCESS;
}

int com_break(com_env* com_p, tree_node* t_n)
{
	s_addr* s_a = top_s_addr(&com_p->var_table.addr_ptr);
	
	if(s_a==NULL)
	{
		print("com[error line: %d] the keyword \"break\" is use error! not find while!\n", t_n->line);
		return	COM_ERROR_BREAK;
	}
	else
	{
		int now_addr = now_inst_addr(com_p);
		push_inst(&com_p->com_inst, new_inst(JUMP, new_addr(NULL_ADDR)));
		add_back_addr(s_a, now_addr);
	}

	return COM_SUCCESS;
}

int com_continue(com_env* com_p, tree_node* t_n)
{
	s_addr* s_a = top_s_addr(&com_p->var_table.addr_ptr);

	if(s_a==NULL)
	{
		print("com[error line: %d] the keyword \"continue\" is use error! not find while!\n", t_n->line);
		return	COM_ERROR_CONTINUE;
	}
	else
	{
		push_inst(&com_p->com_inst, new_inst(JUMP, new_addr(s_a->begin_addr)));
	}

	return COM_SUCCESS;
}

int com_if(com_env* com_p, tree_node* t_n)
{
	int if_addr,if_end_addr,else_addr,end_addr;
	
	if(t_n->childs_p[0]==NULL)
	{
		print("COM[error line: %d]the if exp is nil!\n", t_n->line);
		return COM_ERROR_NOT_EXP;
	}

	check_com(com_exp(com_p, t_n->childs_p[0]));
	if_addr = now_inst_addr(com_p);
	push_inst(&com_p->com_inst, new_inst(JFALSE, new_addr(NULL_ADDR)));
	check_com(com_seg(com_p, t_n->childs_p[1]));		// if seg
	if(t_n->childs_p[2])		// else is exit
	{
		if_end_addr = now_inst_addr(com_p);
		push_inst(&com_p->com_inst, new_inst(JUMP, new_addr(NULL_ADDR)));
		else_addr = now_inst_addr(com_p);
		check_com(com_seg(com_p, t_n->childs_p[2]));
		end_addr= now_inst_addr(com_p);
		com_p->com_inst.root_ptr[if_addr].inst_value.r_v.addr = else_addr;
		com_p->com_inst.root_ptr[if_end_addr].inst_value.r_v.addr = end_addr;
	}
	else
	{
		end_addr = now_inst_addr(com_p);
		com_p->com_inst.root_ptr[if_addr].inst_value.r_v.addr = end_addr;
	}

	return COM_SUCCESS;
}

int com_while(com_env* com_p, tree_node* t_n)
{
	s_addr* s_a = NULL;
	int now_addr = 0;
	int loop_begin_addr = 0;
	tree_node* bool_t_n = t_n->childs_p[0];

	if(bool_t_n==NULL)
	{
		print("com[error line: %d] the while exp is nil!\n", t_n->line);
		return COM_ERROR_NOT_EXP;
	}
	
	loop_begin_addr = now_inst_addr(com_p);		// 获得当前while循环的首地址
	push_s_addr(&com_p->var_table.addr_ptr, new_s_addr(loop_begin_addr));	//  向temp堆栈中写入while循环的首地址
	
	check_com(com_exp(com_p, bool_t_n));		// push inst while bool
	now_addr = now_inst_addr(com_p);
	push_inst(&com_p->com_inst, new_inst(JFALSE, new_addr(NULL_ADDR)));	// while if
	check_com(com_seg(com_p, t_n->childs_p[1]));				// while seg
	push_inst(&com_p->com_inst, new_inst(JUMP, new_addr(loop_begin_addr)));		// jump LOOP_BEGIN
	com_p->com_inst.root_ptr[now_addr].inst_value.r_v.addr = now_inst_addr(com_p);	// back write JFALSE addr 
	
	// 回写 break continue 地址
	s_a = top_s_addr(&com_p->var_table.addr_ptr);
	if(s_a)
	{
		b_addr_node* b_p = s_a->back_addr_head;
		while(b_p)
		{
			com_p->com_inst.root_ptr[b_p->back_addr].inst_value.r_v.addr = now_inst_addr(com_p);
			b_p = b_p->next;
		}
	}
	else
	{
		print("com[error line: %d] big error s_addr stack is pop -1!\n", t_n->line);
		return	COM_ERROR_POP;
	}
	pop_s_addr(&com_p->var_table.addr_ptr);

	return COM_SUCCESS;
}

int com_func_call(com_env* com_p, tree_node* t_n)
{
	r_addr r_a = search_addr(com_p, t_n->b_v.name.s_ptr);
	st* ret_st = NULL;
	if(r_a.gl == COM_ERROR)
	{
		print("com[error line :%d] the func %s is not find!\n", t_n->line, t_n->b_v.name.s_ptr);
		return COM_ERROR_NOT_FIND_IDE;
	}
	else
	{
		ret_st = look_com(com_p, t_n->b_v.name.s_ptr);
		check_com(com_arg(com_p, t_n));	
		push_inst(&com_p->com_inst, new_inst(CALL, r_a.gl, r_a.addr));
	}
	
	return COM_SUCCESS;
}


int com_arg(com_env* com_p, tree_node* t_n)
{
	t_n = t_n->childs_p[0];

	while(t_n)
	{
		check_com(com_exp(com_p, t_n));
		t_n = t_n->next;
	}

	return COM_SUCCESS;
}


int com_exp(com_env* com_p, tree_node* t_n)
{
	if(t_n==NULL)
	{
		print("com[error line: %d] not find exp!\n", t_n->line);
		return COM_ERROR_OTHER;
	}

	switch(type_tree(t_n))
	{
	case bnf_type_vardef:
		check_com(com_l_vardef(com_p, t_n));
		break;
	case bnf_type_const:
		push_inst(&com_p->com_inst, new_inst(PUSH, new_number(t_n->b_v.number)));
		break;
	case bnf_type_string:
		push_inst(&com_p->com_inst, new_inst(PUSH, gc_new_string(t_n->b_v.str.s_ptr, GC_DEAD)));
	//	push_inst(&com_p->com_inst, new_inst(PUSH, new_string(t_n->b_v.str.s_ptr)));
		break;
	case bnf_type_var:
		{
			r_addr r_a = search_addr(com_p, t_n->b_v.name.s_ptr);
			if(r_a.gl == COM_ERROR)
			{
				print("com[error line: %d] not find ide \"%s\"!\n", t_n->line, t_n->b_v.name.s_ptr);
				return COM_ERROR_NOT_FIND_IDE;
			}
			push_inst(&com_p->com_inst, new_inst(PUSHVAR, r_a.gl, r_a.addr));
		}
		break;
	case bnf_type_ass:
		check_com(com_ass(com_p, t_n));
		break;
	case bnf_type_logic:
		check_com(com_op_logic(com_p, t_n));
		break;
	case bnf_type_value:
		check_com(com_op_value(com_p, t_n));
		break;
	case bnf_type_al:
		check_com(com_al_v(com_p, t_n));
		break;
	case bnf_type_aldef:
		check_com(com_aldef(com_p, t_n));
		break;
	case bnf_type_funccall:
		check_com(com_func_call(com_p, t_n));
		break;
	default:
		print("com[error line: %d] not allow at exp!\n");
		return COM_ERROR_OTHER;
	}

	return COM_SUCCESS;
}

int com_op_value(com_env* com_p, tree_node* t_n)
{
	switch(t_n->b_v.op_t)
	{
	case token_type_mod:		// %
	case token_type_div:		// /
	case token_type_mul:		// *
	case token_type_sub:		// -
	case token_type_add:		// +
		{
			if(t_n->childs_p[0]==NULL)
			{
				print("com[error line: %d]  oper \"%s\" not find left value!\n", t_n->line, alex_get_type(t_n->b_v.op_t));
				return COM_ERROR_NOT_EXP;
			}
			else if(t_n->childs_p[1]==NULL)
			{
				print("com[error line: %d] oper \"%s\" not find left value!\n", t_n->line, alex_get_type(t_n->b_v.op_t));
				return COM_ERROR_NOT_EXP;
			}
			
			check_com(com_exp(com_p, t_n->childs_p[0]));
			check_com(com_exp(com_p, t_n->childs_p[1]));
			push_inst( &com_p->com_inst, new_inst(ADD+(t_n->b_v.op_t-token_type_add)) );
		}
		break;
	case token_type_sadd:		//++
	case token_type_ssub:		//--
		{
			check_com(com_op_one(com_p, t_n));
		}
		break;
	default:
		print("inter[error line %d]: oper \"%s\" is not exit!\n", t_n->line, alex_get_type(t_n->b_v.op_t));
		return COM_ERROR_NOT_EXP;
	}
	return	COM_SUCCESS;
}

// 一元操作符
int com_op_one(com_env* com_p, tree_node* t_n)
{
	e_alex_inst e_is = END;
	e_alex_inst e_i = END;
	tree_node* l_t_n = t_n->childs_p[0];
	tree_node* r_t_n = t_n->childs_p[1];
	tree_node* o_t_n = NULL;
	r_addr r_a = {0};

	if( (l_t_n==NULL && r_t_n==NULL)||(l_t_n!=NULL && r_t_n!=NULL) )
	{
		print("com[error line: %d] oper \"%s\" is not find operValue!\n", t_n->line, alex_get_type(t_n->b_v.op_t));
		return COM_ERROR_NOT_EXP;
	}
	
	o_t_n = (l_t_n==NULL)?(r_t_n):(l_t_n);
	if(type_tree(o_t_n)!= bnf_type_var && type_tree(o_t_n)!=bnf_type_al)
	{
		print("com[error line: %d] the oper \"%s\" value is not var!\n", t_n->line, alex_get_type(t_n->b_v.op_t));
		return COM_ERROR_NOT_EXP;
	}
	else
	{
		r_a = search_addr(com_p, o_t_n->b_v.name.s_ptr);
		if(r_a.gl== COM_ERROR)
		{
			print("com[error line: %d] not find var \"%s\" !\n", t_n->line, o_t_n->b_v.name.s_ptr);
			return COM_ERROR_NOT_FIND_IDE;
		}
	}

	switch(t_n->b_v.op_t)
	{
	case token_type_sadd:
		e_i = ADD;
		e_is = SADD;
		goto OP_ONE;
	case token_type_ssub:
		e_i = SUB;
		e_is = SSUB;
OP_ONE:
		{
			if(l_t_n)		// 前缀
			{
				if(type_tree(l_t_n)==bnf_type_var)
				{
					push_inst(&com_p->com_inst, new_inst(e_is, r_a.gl, r_a.addr));
				}
				else if(type_tree(l_t_n)==bnf_type_al)
				{
					push_inst(&com_p->com_inst, new_inst(PUSH, new_number(1)));
					check_com(com_al_v(com_p, l_t_n));		// push al value
					push_inst(&com_p->com_inst, new_inst(MOVEREG, new_addr(REG_AX)));		// al[inx] 的值移动到寄存器AX
					push_inst(&com_p->com_inst, new_inst(e_i));
					check_com(com_al_p(com_p, l_t_n));
					push_inst(&com_p->com_inst, new_inst(POP));
					push_inst(&com_p->com_inst, new_inst(PUSHVAR, COM_REG, new_addr(REG_AX)));
				}
			}
			else if(r_t_n)		// 后缀
			{
				e_is++;
				if(type_tree(r_t_n)==bnf_type_var)
				{
				/*	push_inst(&com_p->com_inst, new_inst(PUSH, new_number(1)));
					push_inst(&com_p->com_inst, new_inst(PUSHVAR, r_a.gl, r_a.addr));
					push_inst(&com_p->com_inst, new_inst(e_i));
					push_inst(&com_p->com_inst, new_inst(MOVE, r_a.gl, r_a.addr));
					*/
					push_inst(&com_p->com_inst, new_inst(e_is, r_a.gl, r_a.addr));
				}
				else if(type_tree(r_t_n)==bnf_type_al)
				{
					push_inst(&com_p->com_inst, new_inst(PUSH, new_number(1)));
					check_com(com_al_v(com_p, r_t_n));		// push al value
					push_inst(&com_p->com_inst, new_inst(e_i));
					check_com(com_al_p(com_p, r_t_n));
				}
			}
		}
		break;
	default:
		print("com[error line: %d] Serious error , the op not know!\n", t_n->line);
		return COM_ERROR_OTHER;
	}

	return COM_SUCCESS;
}



int	com_op_logic(com_env* com_p, tree_node* t_n)
{
	// check left and right value
	if(t_n->childs_p[0]==NULL)		// left exp
	{
		print("com[error line: %d] not find left value!\n", t_n->line);
		return COM_ERROR_NOT_EXP;
	}
	else if(t_n->childs_p[1]==NULL)
	{
		print("com[error line: %d] not find right value!\n", t_n->line);
		return COM_ERROR_NOT_EXP;
	}

	check_com(com_exp(com_p, t_n->childs_p[0]));
	check_com(com_exp(com_p, t_n->childs_p[1]));
	
	switch(t_n->b_v.op_t)
	{
	case token_type_and:
		push_inst(&com_p->com_inst, new_inst(AND));
		break;
	case token_type_or:
		push_inst(&com_p->com_inst, new_inst(OR));
		break;
	case token_type_big:
		push_inst(&com_p->com_inst, new_inst(BIG));
		break;
	case token_type_bige:
		push_inst(&com_p->com_inst, new_inst(BIGE));
		break;
	case token_type_lit:
		push_inst(&com_p->com_inst, new_inst(LIT));
		break;
	case token_type_lite:
		push_inst(&com_p->com_inst, new_inst(LITE));
		break;
	case token_type_equ:
		push_inst(&com_p->com_inst, new_inst(EQU));
		break;
	case token_type_nequ:
		push_inst(&com_p->com_inst, new_inst(NEQU));
		break;
	default:
		print("com[error line: %d] error op!\n", t_n->line);
		return COM_ERROR_OTHER;
	}

	return COM_SUCCESS;
}


int com_aldef(com_env* com_p, tree_node* t_n)
{
	int count = 0;
	t_n = t_n->childs_p[0];

	while(t_n)
	{
		check_com(com_exp(com_p, t_n));
		count++;
		t_n = t_n->next;
	}

	push_inst(&com_p->com_inst, new_inst(NEWAL, count));
	return COM_SUCCESS;
}

int com_exp_stmt(com_env* com_p, tree_node* t_n)
{
	check_com(com_exp(com_p, t_n));
	push_inst(&com_p->com_inst, new_inst(POP));
	
	return COM_SUCCESS;
}


int com_al(com_env* com_p, tree_node* t_n, ubyte v_p)
{
	tree_node* exp = NULL;
	r_addr r_a = search_addr(com_p, t_n->b_v.name.s_ptr);
	// check al is true
	if(r_a.gl== COM_ERROR)
	{
		print("com[error line: %d] not find arraylist \"%s\"!\n", t_n->line, t_n->b_v.name.s_ptr);
		return COM_ERROR_NOT_FIND_IDE;
	}
	
	// check al inx
	if(t_n->childs_p[0]==NULL)
	{
		print("com[error line: %d] not find arraglist \"%s\" index [?]!\n", t_n->line, t_n->b_v.name.s_ptr);
		return COM_ERROR_NOT_AL_INX;
	}

	exp = t_n->childs_p[0];
	push_inst(&com_p->com_inst, new_inst(PUSHVAR, r_a.gl, r_a.addr));
	
	while(exp->next)
	{
		check_com(com_exp(com_p, exp));
		push_inst(&com_p->com_inst, new_inst(AL));
		exp = exp->next;
	}

	check_com(com_exp(com_p, exp));
	if(v_p==COM_VALUE)
		push_inst(&com_p->com_inst, new_inst(AL));
	else if(v_p==COM_POINT)
		push_inst(&com_p->com_inst, new_inst(MOVEAL));
	else
		return COM_ERROR_OTHER;

	return COM_SUCCESS;
}

int com_ass(com_env* com_p, tree_node* t_n)
{
	tree_node* l_t_n = t_n->childs_p[0];
	tree_node* r_t_n = t_n->childs_p[1];

	switch(type_tree(l_t_n))
	{
	case bnf_type_var:
		{
			// check left value
			r_addr r_a = search_addr(com_p, l_t_n->b_v.name.s_ptr);
			if(r_a.gl == COM_ERROR)
			{
				print("com[error line: %d] the ide \"%s\" not define! \n", t_n->line, l_t_n->b_v.name.s_ptr);
				return COM_ERROR_NOT_FIND_IDE;
			}

			check_com(com_exp(com_p, r_t_n));		// get right value

			// move right value to left value
			push_inst(&com_p->com_inst, new_inst(MOVE, r_a.gl, r_a.addr));
		}
		break;
	case bnf_type_al:
		{
			check_com(com_exp(com_p, r_t_n));		// get right value
			check_com(com_al_p(com_p, l_t_n));		// get left al point
	//		push_inst(&com_p->com_inst, new_inst(MOVEAL));
		}
		break;
	default:
		print("com[error line: %d] the ide \"%s\" can not left value!\n", t_n->line, l_t_n->b_v.name.s_ptr);
		return COM_ERROR_NOT_LEFT_VALUE;
	}

	return COM_SUCCESS;
}

int  com_vardef(com_env* com_p, tree_node* t_n, e_gl gl)
{
	sym_table* a_table = NULL;	
	t_n = t_n->childs_p[0];
	a_table = (gl==COM_GLOBAL)?(com_p->var_table.g_table):(com_p->var_table.l_table);
				
	while(t_n)
	{
		char* var_name = NULL;
		switch(type_tree(t_n))
		{
		case bnf_type_var:
				var_name = t_n->b_v.name.s_ptr;
				if(look_table(a_table, var_name))
				{
					print("com[error line: %d] the var  \"%s\" ide is redefine!\n", t_n->line, var_name);
					return COM_ERROR_REDEF;
				}
				else
				{
					if(gl == COM_LOCAL)
						push_inst(&com_p->com_inst, new_inst(VAR));
					add_new_table(a_table, var_name);
				}
			break;
		case bnf_type_ass:
			{
				var_name = t_n->childs_p[0]->b_v.name.s_ptr;
				if(look_table(a_table, var_name))
				{
					print("com[error line: %d] the var  \"%s\" ide is redefine!\n", t_n->line, var_name);
					return COM_ERROR_REDEF;
				}
				else
				{
					if(gl == COM_LOCAL)
						push_inst(&com_p->com_inst, new_inst(VAR));
					add_new_table(a_table, var_name);
				}
				check_com(com_ass(com_p, t_n));
				push_inst(&com_p->com_inst, new_inst(POP));
			}
			break;
		default:
			print("inter[error line %d] at var def the  tree_node \"%s\" no allow!\n", t_n->line, string_bnf(t_n->b_t));
			return COM_ERROR_NOT_ALLOW;
		}
		t_n = t_n->next;
	}

	return COM_SUCCESS;
}


void clear_local_addr(com_env* com_p)
{
	if(com_p==NULL)
		return;

	free_table(com_p->var_table.l_table);
	com_p->var_table.l_table = new_table();
	com_p->var_table.l_top = 0;
}


// 获得变量编译成的地址 如果在table
r_addr com_addr(com_env* com_p, char* name, e_gl gl)
{
	r_addr ret = {0};
	st* r_st = NULL;

	if(com_p==NULL || name==NULL)
		return ret;
	
	ret.gl = gl;

	if(gl== COM_GLOBAL)
	{
		r_st = look_table(com_p->var_table.g_table, name);
		if(r_st==NULL)
		{
			r_st = add_new_table(com_p->var_table.g_table, name);
		}
		ret.addr = r_st->st_addr;	
	}
	else if(gl == COM_LOCAL)
	{
		r_st = look_table(com_p->var_table.l_table, name);
		if(r_st==NULL)
		{
			r_st = add_new_table(com_p->var_table.l_table, name);
		}
		ret.addr = r_st->st_addr;
	}	

	return ret;
}


st*  look_com(com_env* com_p, char* str)
{
	st* ret_st = NULL;
	ret_st = look_table(com_p->var_table.l_table, str);
	if(ret_st)
		return ret_st;
	else
		return look_table(com_p->var_table.g_table, str);
}


// 查找地址
r_addr search_addr(com_env* com_p, char* name)
{
	r_addr ret = {0};
	st* r_st = NULL;
	
	if(com_p==NULL || name==NULL)
		return ret;
	
	r_st = look_table(com_p->var_table.l_table, name);
	if(r_st)
	{
		ret.gl = COM_LOCAL;
		ret.addr = r_st->st_addr;
	}
	else
	{
		r_st = look_table(com_p->var_table.g_table, name);
		if(r_st)
		{
			ret.gl = COM_GLOBAL;
			ret.addr = r_st->st_addr;
		}
	}

	return ret;
}

alex_inst new_inst(e_alex_inst e_i, ...)
{
	alex_inst a_i = {0};
	va_list arg_list;

	va_start(arg_list, e_i);
	a_i.inst_type = e_i;
	switch(a_i.inst_type)
	{
	case CALL:
	case MOVE:
	case PUSHVAR:
	case SADD:
	case BSADD:
	case SSUB:
	case BSSUB:
		{
			a_i.gl = (e_gl)va_arg(arg_list, e_gl);			// get gl
			a_i.inst_value.r_v.addr = va_arg(arg_list, int);// get addr
			a_i.inst_value.r_t = sym_type_addr;				// set type
		}	
		break;
	case NEWAL:
		{
			a_i.inst_value.r_v.num = (ALEX_NUMBER)va_arg(arg_list, int);
			a_i.inst_value.r_t = sym_type_num;
		}
		break;
	case MOVEREG:
	case PUSH:
		{
			a_i.inst_value = va_arg(arg_list, r_value);
		}
		break;
	case JUMP:
	case JFALSE:
	case JTRUE:
		{
			a_i.inst_value = va_arg(arg_list, r_value);
		}
		break;
	}
	va_end(arg_list);

	return a_i;
}

int com_print_inst_type(alex_inst a_i, int pc)
{
	if(a_i.inst_type >= (ubyte)INST_COUNT)
	{
		print("\nasm[error pc: %d] the inst_type[%d] is over!\n", pc, a_i.inst_type);
		return COM_ERROR_OTHER;
	}
	
	print(" %10s  ", com_str[a_i.inst_type]);
	return	COM_SUCCESS;
}

int com_print_inst_gl(alex_inst a_i, int pc)
{
	if(a_i.gl >= (ubyte)COM_GL_COUNT)
	{
		print("\nasm[error pc: %d] the inst_gl[%d] is over!\n", pc, a_i.gl);
		return COM_ERROR_OTHER;
	}
	
	if(a_i.gl)
		print(" %10s  ", gl_str[a_i.gl]);
	return COM_SUCCESS;
}

int com_print_inst_value(alex_inst a_i, int pc)
{
	switch(a_i.inst_value.r_t)
	{
	case sym_type_num:
		print(" %10.14g	", a_i.inst_value.r_v.num);
		break;
	case sym_type_string:
		print("  \"%s\"	", a_i.inst_value.r_v.str.s_ptr);
		break;
	case sym_type_reg_func:
	case sym_type_func:
		print(" %10p	", a_i.inst_value.r_v.func);
		break;
	case sym_type_al:
		print(" %10p	", a_i.inst_value.r_v.al);
		break;
	case sym_type_alp:
		print(" %10p	", a_i.inst_value.r_v.ptr);
		break;
	case sym_type_pointer:
		print(" %10p	", a_i.inst_value.r_v.ptr);
		break;
	case sym_type_addr:
		print(" %10d	", a_i.inst_value.r_v.addr);
		if(a_i.inst_type == CALL)
		{
			if(a_i.inst_value.r_t == sym_type_addr)
			{
				r_value r_v = com_env_p->var_table.global_ptr.root_ptr[a_i.inst_value.r_v.addr];
				if(r_v.r_t == sym_type_addr)
					print("; func addr = [%d] ", r_v.r_v.addr);
				else if(r_v.r_t == sym_type_reg_func)
					print("; reg func addr = [%p] ", r_v.r_v.func);
				else
				{
					print(" attemp a not addr !\n");
					return COM_ERROR_OTHER;
				}
			}
			else
			{
				print(" attemp a not addr !\n");
				return COM_ERROR_OTHER;
			}
			return COM_SUCCESS;
		}
		break;
	case sym_type_arg_num:
		print(" %10d	", a_i.inst_value.r_v.arg_num);
		break;
	
	case sym_type_error:
		break;

	default:
		print("\nasm[error pc: %d] the sym type[%d] is over!\n", pc, a_i.inst_value.r_t);
		return COM_ERROR_OTHER;
	}

	return COM_SUCCESS;
}

int com_print_inst(alex_inst a_i, int pc)
{
	print("[%.3d]  ", pc);
	check_com(com_print_inst_type(a_i, pc));
	check_com(com_print_inst_gl(a_i, pc));
	check_com(com_print_inst_value(a_i, pc));

	print("\n");
	return COM_SUCCESS;
}

addr_data* relloc_s_addr(addr_data* a_d, int a_len)
{
	if(a_d==NULL)
		return NULL;
	if(a_d->addr_len>=a_d->addr_size)
	{
		int n_len = (a_d->addr_size+a_len)*sizeof(s_addr);
		s_addr* n_a_i = (s_addr*)a_malloc(n_len);
		memset(n_a_i, 0, n_len);
		memcpy(n_a_i, a_d->root_ptr, a_d->addr_size*sizeof(s_addr));
		a_free(a_d->root_ptr);
		a_d->root_ptr = n_a_i;
		a_d->addr_size += a_len;
	}
	
	return a_d;
}

void push_s_addr(addr_data* a_d, s_addr s_a)
{
	if(a_d==NULL)
		return;
	
	relloc_s_addr(a_d, CALL_MEM_LEN);
	a_d->root_ptr[a_d->addr_len++] = s_a;
}

s_addr* top_s_addr(addr_data* a_d)
{
	if(a_d==NULL)
		return NULL;
	
	if(a_d->addr_len>0)
		return &(a_d->root_ptr[(a_d->addr_len-1)]);
	else
		return NULL;
}

int pop_s_addr(addr_data* a_d)
{
	if(a_d==NULL)
		return COM_ERROR;
	
	if(a_d->addr_len>0)
	{
		s_addr* t_s = top_s_addr(a_d);
		free_s_addr(t_s);
		a_d->addr_len--;
		return COM_SUCCESS;
	}
	else
		return COM_ERROR;
}

s_addr new_s_addr(int addr)
{
	s_addr ret_s = {0};
	ret_s.begin_addr = addr;
	
	return ret_s;
}

void free_s_addr(s_addr* s_p)
{
	b_addr_node* b_p = NULL;
	
	if(s_p==NULL)
		return;
	
	b_p = s_p->back_addr_head;
	while(b_p)
	{
		b_addr_node* n_b_p= b_p->next;
		a_free(b_p);
		b_p = n_b_p;
	}
	
}

void add_back_addr(s_addr* s_a, int b_addr)
{
	b_addr_node* new_node = NULL;
	if(s_a==NULL)
		return;
	
	new_node = (b_addr_node*)a_malloc(sizeof(b_addr_node));
	memset(new_node, 0, sizeof(b_addr_node));
	new_node->back_addr = b_addr;
	
	new_node->next = s_a->back_addr_head;
	s_a->back_addr_head = new_node;
}




int com_print(com_env* com_p)
{
	int i=0;

	if(com_p==NULL)
	{
		print("com_p==NULL !\n");
		return COM_ERROR_OTHER;
	}

	print("; COM ASM CODE :\n");
	for(i=0; i<com_p->com_inst.inst_len; i++)
	{
		check_com(com_print_inst(com_p->com_inst.root_ptr[i], i));
	}
	return COM_SUCCESS;
}
