#include "alex_com.h"
#include "alex_vm.h"


com_env* com_env_p = NULL;



com_env* new_com_env()
{
	com_env* ret_c_e_p = (com_env*)malloc(sizeof(com_env));
	memset(ret_c_e_p, 0, sizeof(com_env));

	relloc_code(&(ret_c_e_p->com_inst));
	ret_c_e_p->var_table.g_table = &global_table;
	ret_c_e_p->var_table.g_top=0;

	ret_c_e_p->var_table.l_table = new_table();
	ret_c_e_p->var_table.l_top=0;

	relloc_s_addr(&ret_c_e_p->var_table.addr_ptr, TEMP_MEM_LEN);
	return ret_c_e_p;
} 

// ����code��� main_treeΪִ�е��ô��� func_treeΪ��������
void alex_com(com_env* com_p, tree_node* main_tree, tree_node* func_tree)
{
	if(main_tree == NULL)
		return;
	 
	while(func_tree)
	{
		com_func_def(com_p, func_tree);
		func_tree = func_tree->next;
	}

	while(main_tree)
	{
		switch(type_tree(main_tree))
		{
		case bnf_type_vardef:
			{
				com_g_vardef(com_p, main_tree);
			}
			break;
		case bnf_type_funccall:
			{
				com_func_call(com_p, main_tree);
			}
			break;
		default:
			{
				print("com[error line %d]: the g_code not allow!\n", main_tree->line);
			}
			return;
		}
		main_tree = main_tree->next;
	}
}


int com_func_def(com_env* com_p, tree_node* t_n)
{
	tree_node* arg_t_n = t_n->childs_p[0];
	tree_node* seg_t_n = t_n->childs_p[1];

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
	t_n = t_n->childs_p[0];
	
	if(t_n==NULL)
	{
		print("com[error line: %d] the seg is nil!\n", t_n->line);
		return COM_ERROR_NOT_EXP;
	}

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
	t_n = t_n->childs_p[0];
	if(t_n==NULL)			// not return value
	{
		com_return_assume(com_p, t_n);
	}
	else
	{
		check_com(com_exp(com_p, t_n));
		push_inst(&com_p->com_inst, new_inst(RET));
	}

	return COM_SUCCESS;
} 

// when return; or no return
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
	
	loop_begin_addr = now_inst_addr(com_p);		// ��õ�ǰwhileѭ�����׵�ַ
	push_s_addr(&com_p->var_table.addr_ptr, new_s_addr(loop_begin_addr));	//  ��temp��ջ��д��whileѭ�����׵�ַ
	
	check_com(com_exp(com_p, bool_t_n));		// push inst while bool
	now_addr = now_inst_addr(com_p);
	push_inst(&com_p->com_inst, new_inst(JFALSE, new_addr(NULL_ADDR)));	// while if
	check_com(com_seg(com_p, t_n->childs_p[1]));				// while seg
	push_inst(&com_p->com_inst, new_inst(JUMP, new_addr(loop_begin_addr)));		// jump LOOP_BEGIN
	com_p->com_inst.root_ptr[now_addr].inst_value.r_v.addr = now_inst_addr(com_p);	// back write JFALSE addr 
	
	// ��д break continue ��ַ
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
		push_inst(&com_p->com_inst, new_inst(CALL, r_a.gl, r_addr));
	}
	
	return COM_SUCCESS;
}


int com_arg(com_env* com_p, tree_node* t_n)
{
	tree_node* t_n = t_n->childs_p[0];

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
	//	push_inst(&com_p->com_inst, new_inst(PUSH, new_str));
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
	case bnf_type_value:
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

int	com_op(com_env* com_p, tree_node* t_n)
{
	
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

	check_com(com_exp(com_p, t_n->childs_p[0]));
	if(v_p==COM_VALUE)
		push_inst(&com_p->com_inst, new_inst(AL, r_a.gl, r_a.addr));
	else if(v_p==COM_POINT)
	{
		// push al address
		push_inst(&com_p->com_inst, new_inst(PUSH, new_addr(r_a.addr)));
		// push al global or local
		push_inst(&com_p->com_inst, new_inst(PUSH, new_number(r_a.gl)));
	}
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
				print("com[error line: %d] the ide \"%s\" not define! \n", t_n->line, t_n->b_v.name.s_ptr);
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
			push_inst(&com_p->com_inst, new_inst(MOVEAL));
		}
		break;
	default:
		print("com[error line: %d] the ide can not left value!\n", t_n->line);
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
		case bnf_type_ass:
			check_com(com_ass(com_p, t_n));
			push_inst(&com_p->com_inst, new_inst(POP));
			break;
		case bnf_type_var:
			var_name = t_n->b_v.name.s_ptr;
			if(look_table(a_table, var_name))
			{
				print("com[error line: %d] the var  \"%s\" ide is redefine!\n", t_n->line, var_name);
				return COM_ERROR_REDEF;
			}
			else
				add_new_table(a_table, var_name);
			break;
		default:
			print("inter[error line %d] at var def the  tree_node \"%s\" no allow!\n", t_n->line, string_bnf(t_n->b_t));
			return COM_ERROR_NOT_ALLOW;
		}
		t_n = t_n->next;
	}
}



void clear_local_addr(com_env* com_p)
{
	if(com_p==NULL)
		return;

	free_table(com_p->var_table.l_table);
	com_p->var_table.l_table = new_table();
	com_p->var_table.l_top = 0;
}


// ��ñ�������ɵĵ�ַ �����table
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
			r_st->st_addr = com_p->var_table.g_top++;
		}
		ret.addr = r_st->st_addr;	
	}
	else if(gl == COM_LOCAL)
	{
		r_st = look_table(com_p->var_table.l_table, name);
		if(r_st==NULL)
		{
			r_st = add_new_table(com_p->var_table.l_table, name);
			r_st->st_addr = com_p->var_table.l_top++;
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


// ���ҵ�ַ
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
			ret.gl = COM_LOCAL;
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
	case JUMP:
	case AL:
	case MOVE:
	case PUSHVAR:
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
	case PUSH:
		{
			a_i.inst_value = (r_value)va_arg(arg_list, r_value);
		}
		break;
	case JFALSE:
	case JTRUE:
		{
			a_i.inst_value = (r_value)va_arg(arg_list, r_value);
		}
		break;
	}
	va_end(a_i);

	return a_i;
}

addr_data* relloc_s_addr(addr_data* a_d, int a_len)
{
	if(a_d==NULL)
		return NULL;
	if(a_d->addr_len>=a_d->addr_size)
	{
		int n_len = (a_d->addr_size+a_len)*sizeof(s_addr);
		s_addr* n_a_i = (s_addr*)malloc(n_len);
		memset(n_a_i, 0, n_len);
		memcpy(n_a_i, a_d->root_ptr, a_d->addr_size);
		free(a_d->root_ptr);
		a_d->root_ptr = n_a_i;
		a_d->addr_size += a_len;
	}
	
	return a_d;
}

void push_s_addr(addr_data* a_d, s_addr s_a)
{
	if(a_d==NULL)
		return;
	
	relloc_s_addr(a_d);
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
		free(b_p);
		b_p = n_b_p;
	}
	
}

void add_back_addr(s_addr* s_a, int b_addr)
{
	b_addr_node* new_node = NULL;
	if(s_a==NULL)
		return;
	
	new_node = (b_addr_node*)malloc(sizeof(b_addr_node));
	memset(new_node, 0, sizeof(b_addr_node));
	new_node->back_addr = b_addr;
	
	new_node->next = s_a->back_addr_head;
	s_a->back_addr_head = new_node;
}
