/*
	alex stack vm
*/
#include "alex_com.h"
#include "alex_vm.h"
#include "stdlib.h"
#include "alex_arrylist.h"
#include "alex_log.h"
#include "alex_gc.h"


typedef int (*vm_func)(vm_env*);

r_value error_v = {0};
vm_env alex_vm_env = {0};


int vm_tp(vm_env* vm_p, alex_inst* a_i_p);
int push_call(vm_env* vm_p, r_value r_v);
r_value _top_data(d_data* d_ptr);
int vm_set_var(vm_env* vm_p, e_gl gl, int addr, r_value* r_v);
int vm_add(vm_env* vm_p);
int vm_pushvar(vm_env* vm_p, alex_inst* a_i_p);
int vm_jfalse(vm_env* vm_p, alex_inst* a_i_p);
int vm_jtrue(vm_env* vm_p, alex_inst* a_i_p);
int vm_move(vm_env* vm_p, alex_inst* a_i_p);
int vm_movereg(vm_env* vm_p, alex_inst* a_i_p);
int vm_call(vm_env* vm_p, alex_inst* a_i_p);
int vm_jump(vm_env* vm_p, alex_inst* a_i_p);
int vm_ret(vm_env* vm_p);
int vm_newal(vm_env* vm_p, alex_inst* a_i_p);
int vm_al(vm_env* vm_p, alex_inst* a_i_p);
int vm_moveal(vm_env* vm_p, alex_inst* a_i_p);
int vm_addr(vm_env* vm_p, alex_inst* a_i_p);
int vm_s_b(vm_env* vm_p, alex_inst* a_i, byte tt);

// 
// #define vm_get_var_l(vm_p, addr)		(	\
// 											((addr)>=0 && ( ((vm_p)->local_top+(addr)) < (vm_p)->local_ptr.data_len))?	\
// 											(&((vm_p)->local_ptr.root_ptr[(vm_p)->local_top+(addr)])):	\
// 											(print("vm[error] get local is fail!\n"), &error_v) \
// 										)
// 
// #define vm_get_var_g(vm_p, addr)		( \
// 											( ((addr)<(vm_p)->global_ptr.data_len) && ((addr)>=0) )?	\
// 											(&((vm_p)->global_ptr.root_ptr[(addr)])):	\
// 											(print("vm[error] get global is fail!\n"), &error_v)	\
// 										)
// 
// #define vm_get_var_r(vm_p, addr)	( &((vm_p)->reg[(addr)]) )
// 
// #define vm_get_var(vm_p, gl, addr)		(  \
// 											((gl)==COM_LOCAL)?	\
// 											( vm_get_var_l((vm_p), (addr)) ):	\
// 											( \
// 												((gl)==COM_GLOBAL)?	\
// 												( vm_get_var_g((vm_p), (addr)) ):	\
// 												( \
// 													((gl)==COM_REG)? \
// 													( vm_get_var_r((vm_p), (addr)) ):	\
// 													( print("vm[error] not find gl!\n"), &error_v)	\
// 												)  \
// 											)	\
// 										) 
// 

#define vm_get_var_grl(vm_p, gl, addr)   (&( (*(vm_p->glr[gl]))[(addr)] ) )
#define vm_get_var(vm_p, gl, addr)	    ( (gl==COM_LOCAL)?(vm_get_var_grl(vm_p, gl, addr+vm_p->local_top)):(vm_get_var_grl(vm_p, gl, addr)) )

// vm 
int alex_vm(vm_env* vm_p)
{
	
	alex_inst* a_i_p = NULL;
	byte i_t = END;
	if(vm_p==NULL)
		return VM_ERROR;
	
	for(;;)
	{
		if(vm_p->pc<0 || vm_p->pc>=vm_p->code_ptr.inst_len)
		{
			print("vm[error addr: %d] Attempts to access a error addr!\n", vm_p->pc);
			return	VM_ERROR_PC;
		}
		
		a_i_p = &vm_p->code_ptr.root_ptr[vm_p->pc];

		//	parse inst 
		switch(i_t=vm_p->code_ptr.root_ptr[vm_p->pc].inst_type)
		{
		case END:
			{
				_gc_back(0);
				print("\n\n-----inst success end-----\n\n");
			}
			return VM_SUCCESS;
		case PUSH:
			push_data(&vm_p->data_ptr, a_i_p->inst_value);
			break;
		case VAR:
			push_data(&vm_p->local_ptr, new_number(0));
			break;
		case PUSHVAR:
			check_vm(vm_pushvar(vm_p, a_i_p));
			break;
		case POP:
			check_value(pop_data(&vm_p->data_ptr));
			break;
		case NEWAL:
			check_vm(vm_newal(vm_p, a_i_p));
			break;
		case AL:
			check_vm(vm_al(vm_p, a_i_p));
			break;
		case JFALSE:
			check_vm(vm_jfalse(vm_p, a_i_p));
			break;
		case JTRUE:
			check_vm(vm_jtrue(vm_p, a_i_p));
			break;
		case MOVE:
			check_vm(vm_move(vm_p, a_i_p));
			break;
		case MOVEAL:
			check_vm(vm_moveal(vm_p, a_i_p));
			break;
		case MOVEREG:
			check_vm(vm_movereg(vm_p, a_i_p));
			break;
		case TABLE:		// ?
			break;

		case SADD:
		case BSADD:
		case SSUB:
		case BSSUB:
			check_vm(vm_s_b(vm_p, a_i_p, i_t));
			break;
		case ADD:
			check_vm(vm_add(vm_p));
			break;

		case SUB:
		case MUL:
		case DEV:
		case MOD:
		case AND:
		case OR:
		case BIG:
		case BIGE:
		case LIT:
		case LITE:
		case EQU:
		case NEQU:
			check_vm(vm_tp(vm_p, a_i_p));
			break;
		case CALL:
			check_vm(vm_call(vm_p, a_i_p));
			continue;
			break;
		case JUMP:
			check_vm(vm_jump(vm_p, a_i_p));
			break;
		case  RET:
			check_vm(vm_ret(vm_p));
			continue;
			break;
		default:
			print("vm[error line: %d] not know inst!\n", a_i_p->line);
			return	VM_ERROR;
		}

		vm_p->pc++;
	}

	return	VM_ERROR;
}


int vm_s_b(vm_env* vm_p, alex_inst* a_i_p, byte tt)
{
	r_value* r_v = NULL;
	r_value r_r_v = {0};

	check_value(r_v=vm_get_var(vm_p, a_i_p->gl, a_i_p->inst_value.r_v.addr));
	if(r_v->r_t != sym_type_num)
	{
		print("vm[error line: %d] ide is not number type!\n", a_i_p->line);
		return VM_ERROR_ADD_OP;
	}
	
	switch(tt)
	{
	case SADD:
		r_r_v=new_number(r_v->r_v.num+1);
		check_vm(vm_set_var(vm_p, a_i_p->gl, a_i_p->inst_value.r_v.addr, &r_r_v));
		push_data(&vm_p->data_ptr, *r_v);
		break;
	case SSUB:
		r_r_v=new_number(r_v->r_v.num-1);
		check_vm(vm_set_var(vm_p, a_i_p->gl, a_i_p->inst_value.r_v.addr, &r_r_v));
		push_data(&vm_p->data_ptr, *r_v);
		break;
	case BSADD:
		r_r_v=new_number(r_v->r_v.num+1);
		check_vm(vm_set_var(vm_p, a_i_p->gl, a_i_p->inst_value.r_v.addr, &r_r_v));
		push_data(&vm_p->data_ptr, r_r_v);
		break;
	case BSSUB:
		r_r_v=new_number(r_v->r_v.num-1);
		check_vm(vm_set_var(vm_p, a_i_p->gl, a_i_p->inst_value.r_v.addr, &r_r_v));
		push_data(&vm_p->data_ptr, r_r_v);
		break;
	default:
		print("vm[error line: %d] error type at func: vm_S_b!\n", a_i_p->line);
		return VM_ERROR;
	}
	return VM_SUCCESS;
}

int vm_moveal(vm_env* vm_p, alex_inst* a_i_p)
{
	r_value* al_inx = NULL;
	r_value* al_r_v = NULL;
	r_value* al = NULL;
	
	r_value* al_l_p = NULL; 

	al_inx=pop_data(&vm_p->data_ptr);
	al=pop_data(&vm_p->data_ptr);
	check_value(al_r_v=top_data(&vm_p->data_ptr));
	
	if(al_inx->r_t != sym_type_num)		// index
	{
		print("vim[error line: %d], index is not number!\n", a_i_p->line);
		return VM_ERROR_AL;
	}

	if(al->r_t != sym_type_al)		// arraylist
	{
		print("vm[error line: %d], not al!\n", a_i_p->line);
		return VM_ERROR_AL;
	}
	
	if( (al_l_p=get_al(al->r_v.al, (int)al_inx->r_v.num))==NULL )		// value
	{
		print("vm[error line: %d], error al index[%d]!", a_i_p->line, (int)al_inx->r_v.num);
		return VM_ERROR_AL;
	}
	
	check_l_gc(al_l_p);
	check_r_gc(al_r_v);
	
	*al_l_p = *al_r_v;
	
	gc_back();
	return VM_SUCCESS;
}


int vm_al(vm_env* vm_p, alex_inst* a_i_p)
{
	r_value* at_al = NULL;
	r_value* r_index = NULL;
	r_value* al = NULL;
	r_index=pop_data(&vm_p->data_ptr);
	al = pop_data(&vm_p->data_ptr);		// arraylist

	if(al->r_t != sym_type_al)
	{
		print("vm[error line: %d] the ide is not al!\n", a_i_p->line);
		return VM_ERROR_AL;
	}

	if(r_index->r_t != sym_type_num)	// index
	{
		print("vm[error line: %d] the al index is not number!\n", a_i_p->line);
		return VM_ERROR_AL;
	}
	
	if( (at_al=get_al(al->r_v.al, (int)r_index->r_v.num))==NULL )	// value
	{
		print("vm[error line: %d] not find al index!\n", a_i_p->line);
		return VM_ERROR_AL;
	}
	
	push_data(&vm_p->data_ptr, *at_al);
	return VM_SUCCESS;
}


int vm_newal(vm_env* vm_p, alex_inst* a_i_p)
{
	int i=0;
	int count = 0;
	r_value* al_one = NULL;
	r_value new_al = {0};

	if(a_i_p->inst_value.r_t != sym_type_num)
	{
		print("vm[error line: %d] new al count is error !\n", a_i_p->line);
		return VM_ERROR_AL;
	}
	
	new_al = gc_new_al((int)a_i_p->inst_value.r_v.num);
	count = (int)a_i_p->inst_value.r_v.num;
	for(i=0; i<count; i++)
	{
		r_value* r_p = NULL;
		check_value(al_one=pop_data(&vm_p->data_ptr));
		check_r_gc(al_one);
		r_p = get_al(new_al.r_v.al, count-i-1);
		if(r_p==NULL)
			return VM_ERROR_AL;
		else
			*r_p = *al_one;
	}

	push_data(&vm_p->data_ptr, new_al);
	return VM_SUCCESS;
}

int vm_addr(vm_env* vm_p, alex_inst* a_i_p)
{
	if(a_i_p->inst_value.r_t != sym_type_addr)
	{
		print("vm[error line: %d] jump inst is error, not know jump addr!\n", a_i_p->line);
		return VM_ERROR;
	}
	else if(a_i_p->inst_value.r_v.addr <0 || a_i_p->inst_value.r_v.addr >= vm_p->code_ptr.inst_len)
	{
		print("vm[error line: %d] jump addr [%d] is error!\n", a_i_p->line, a_i_p->inst_value.r_v.addr);
		return VM_ERROR;
	}

	return VM_SUCCESS;
}

int vm_movereg(vm_env* vm_p, alex_inst* a_i_p)
{
	r_value* r_v = NULL;
	check_value(r_v = top_data(&vm_p->data_ptr));
	if(a_i_p->gl != COM_REG || a_i_p->inst_value.r_t != sym_type_addr || a_i_p->inst_value.r_v.addr <0 || a_i_p->inst_value.r_v.addr >=REG_LEN)
	{
		print("vm[error line: %d] is not reg!\n", a_i_p->line);
		return VM_ERROR;
	}

	vm_p->reg[a_i_p->inst_value.r_v.addr] = *r_v;

	return VM_SUCCESS;
}

int vm_jfalse(vm_env* vm_p, alex_inst* a_i_p)
{
	r_value* bool_v = NULL;

	check_value(bool_v=pop_data(&vm_p->data_ptr));
	if(bool_v->r_t != sym_type_num)
	{
		print("vm[error line: %d] the check exp is not bool!\n", a_i_p->line);
		return VM_ERROR;
	}

	if((int)(bool_v->r_v.num)==0)
	{
//		check_vm(vm_addr(vm_p, a_i));
		vm_p->pc = a_i_p->inst_value.r_v.addr;
		vm_p->pc--;
	}


	return VM_SUCCESS;
}

int vm_jtrue(vm_env* vm_p, alex_inst* a_i_p)
{
	r_value* bool_v = NULL;
	
	check_value(bool_v=pop_data(&vm_p->data_ptr));
	if(bool_v->r_t != sym_type_num)
	{
		print("vm[error line: %d] the check exp is not bool!\n", a_i_p->line);
		return VM_ERROR;
	}
	
	if((int)(bool_v->r_v.num))
	{
//		check_vm(vm_addr(vm_p, a_i));
		vm_p->pc = a_i_p->inst_value.r_v.addr;
		vm_p->pc--;
	}
	
	return VM_SUCCESS;
}

int vm_jump(vm_env* vm_p, alex_inst* a_i_p)
{
//	check_vm(vm_addr(vm_p, a_i));
	vm_p->pc = a_i_p->inst_value.r_v.addr;
	vm_p->pc--;

	return VM_SUCCESS;
}

int vm_add(vm_env* vm_p)
{
	r_value* l_r_v = NULL;
	r_value* r_r_v = NULL;
	
	check_value(r_r_v=pop_data(&vm_p->data_ptr));
	check_value(l_r_v=pop_data(&vm_p->data_ptr));
	
	switch(l_r_v->r_t)
	{
	case sym_type_string:
		{
			switch(r_r_v->r_t)
			{
			case sym_type_string:		// string + string
				{
					r_value r_v = {0};
					a_string a_s = alex_string(l_r_v->r_v.str.s_ptr);
					cat_string(&a_s, r_r_v->r_v.str.s_ptr);
					r_v = gc_new_string(a_s.s_ptr, GC_LIVE);
					free_string(&a_s);
					push_data(&vm_p->data_ptr, r_v);
				}
				break;
			case sym_type_num:
				{
					r_value r_v = {0};
					char t_num[256] = {0};
					a_string a_s = alex_string(l_r_v->r_v.str.s_ptr);
					gcvt(r_r_v->r_v.num, 10, t_num);
					cat_string(&a_s, t_num);
					r_v = gc_new_string(a_s.s_ptr, GC_LIVE);
					free_string(&a_s);
					push_data(&vm_p->data_ptr, r_v);
				}
				break;
			default:
				print("vm[error line: %d] the right op value is not allow!\n", vm_p->code_ptr.root_ptr[vm_p->pc].line);
				return VM_ERROR_ADD_OP;
			}
		}
		break;
	case sym_type_num:
		{
			switch(r_r_v->r_t)
			{
			case sym_type_string:
				{
					r_value r_v = {0};
					char t_num[256]={0};
					a_string a_s = alex_string(gcvt(l_r_v->r_v.num, 10, t_num));
					cat_string(&a_s, r_r_v->r_v.str.s_ptr);
					r_v = gc_new_string(a_s.s_ptr, GC_LIVE);
					free_string(&a_s);
					push_data(&vm_p->data_ptr, r_v);
				}
				break;
			case sym_type_num:
				{
					r_value r_v = new_number(0);
					r_v.r_v.num = l_r_v->r_v.num+r_r_v->r_v.num;
					push_data(&vm_p->data_ptr, r_v);
				}
				break;
			default:
				print("vm[error line: %d] the right op value is not allow!\n", vm_p->code_ptr.root_ptr[vm_p->pc].line);
				return VM_ERROR_ADD_OP;
			}
		}
		break;
	default:
		print("vm[error line: %d] the left op value is not allow!\n", vm_p->code_ptr.root_ptr[vm_p->pc].line);
		return VM_ERROR_ADD_OP;
	}
	return VM_SUCCESS;
}

int vm_pushvar(vm_env* vm_p, alex_inst* a_i_p)
{
	r_value* r_v = NULL;
	check_value(r_v=vm_get_var(vm_p, a_i_p->gl, a_i_p->inst_value.r_v.addr));
	
	push_data(&vm_p->data_ptr, *r_v);
	return VM_SUCCESS;
}



int vm_move(vm_env* vm_p, alex_inst* a_i_p)
{
	r_value* r_v = NULL;
	
	check_value(r_v = top_data(&vm_p->data_ptr));
	check_vm(vm_set_var(vm_p, a_i_p->gl, a_i_p->inst_value.r_v.addr, r_v));
	return	VM_SUCCESS;
}


//  gl addr ϵַϵֵΪ r_v
int vm_set_var(vm_env* vm_p, e_gl gl, int addr, r_value* r_v)
{
	if(r_v==NULL)
		return	VM_ERROR;

	switch(gl)
	{
	case COM_LOCAL:
		{
			if(addr>=0 && (vm_p->local_top+addr < vm_p->local_ptr.data_len))
			{
				r_value* r_p = &vm_p->local_ptr.root_ptr[vm_p->local_top+addr];
				check_l_gc(r_p);
				check_r_gc(r_v);
				*r_p = *r_v;
				
				gc_back();
			}
			else
			{
				print("vm[error] you are try access a error local var!\n");
				return	VM_ERROR;
			}
		}
		break;
	case COM_GLOBAL:
		{
			if(addr<vm_p->global_ptr.data_len && addr >=0)
			{
				r_value* r_p = &vm_p->global_ptr.root_ptr[addr];
				check_l_gc(r_p);
				check_r_gc(r_v);
				*r_p = *r_v;
			}
			else
			{
				print("vm[error] you are try access a error global var!\n");
				return VM_ERROR;
			}
		}
		break;
	}

	return VM_SUCCESS;
}

int vm_call(vm_env* vm_p, alex_inst* a_i_p)
{
	r_value* r_v = NULL;
	next_pc(vm_p);
	
	// get jump addr
	check_value(r_v=vm_get_var(vm_p, a_i_p->gl, a_i_p->inst_value.r_v.addr));
	check_vm(vm_p_call(vm_p, r_v));

	return VM_SUCCESS;
}

int vm_p_call(vm_env* vm_p, r_value* r_v_p)
{
	if(r_v_p==NULL || vm_p==NULL)
	{
		return VM_ERROR;
	}
	else if(r_v_p->r_t == sym_type_addr)		// alex func
	{
		// record pc
		push_call(vm_p, new_addr(vm_p->pc));
		vm_p->pc = r_v_p->r_v.addr;
		
		// record local top
		push_call(vm_p, new_addr(vm_p->local_top));
		
		vm_p->local_top = vm_p->local_ptr.data_len;
	}
	else if(r_v_p->r_t == sym_type_reg_func)		// reg func
	{
		vm_func func_p = (vm_func)r_v_p->r_v.func;	
		int ret = func_p(vm_p);
		if(ret==0)
		{
			push_data(&vm_p->data_ptr, new_number(0));
		}
		else if(ret != 1)
		{
			print("vm[error] reg func return is error ! ret= %d is error!", ret);
			return VM_ERROR_REG_FUNC;
		}
	}
	else
	{
		print("vm[error] the ide is not find func!\n");
		return VM_ERROR_NOT_IDE;
	}

	return VM_SUCCESS;
}

int vm_ret(vm_env* vm_p)
{
	int i=0, len =0;
	r_value* pc_value = NULL;
	r_value* local_top_value = NULL;

	check_value(local_top_value=pop_data(&vm_p->call_ptr));
	check_value(pc_value=pop_data(&vm_p->call_ptr));

	// resume pc
	vm_p->pc = pc_value->r_v.addr;
	len = vm_p->local_ptr.data_len;
	// free local stack memory  if var is string or al will free 
	for(i=vm_p->local_top; i<len; i++)
	{
		r_value* l_r_v = pop_data(&vm_p->local_ptr);
		check_l_gc(l_r_v);		// clear gc count
	}

	vm_p->local_top = local_top_value->r_v.addr;

	gc_back();
	return VM_SUCCESS;
}

int vm_tp(vm_env* vm_p, alex_inst* a_i_p)
{
	r_value* l_r_v =NULL;
	r_value* r_r_v =NULL;

	check_value(r_r_v=pop_data(&vm_p->data_ptr));
	check_value(l_r_v=pop_data(&vm_p->data_ptr));

	if(l_r_v->r_t != sym_type_num || r_r_v->r_t != sym_type_num)
	{
		print("vm[error line: %d] the op value not number!\n", a_i_p->line);
		return VM_ERROR_OP_VALUE;
	}

	switch(a_i_p->inst_type)
	{
	case SUB:
		push_data(&vm_p->data_ptr, new_number(l_r_v->r_v.num - r_r_v->r_v.num));
		break;
	case MUL:
		push_data(&vm_p->data_ptr, new_number(l_r_v->r_v.num * r_r_v->r_v.num));
		break;
	case DEV:
		push_data(&vm_p->data_ptr, new_number(l_r_v->r_v.num / r_r_v->r_v.num));
		break;
	case MOD:
		push_data(&vm_p->data_ptr, new_number((int)(l_r_v->r_v.num) % (int)(r_r_v->r_v.num)));
		break;
	case AND:
		push_data(&vm_p->data_ptr, new_number(((int)(l_r_v->r_v.num) && (int)(r_r_v->r_v.num))?(1):(0)));
		break;
	case OR:
		push_data(&vm_p->data_ptr, new_number(((int)(l_r_v->r_v.num) || (int)(r_r_v->r_v.num))?(1):(0)));
		break;
	case BIG:
		push_data(&vm_p->data_ptr, new_number(((int)(l_r_v->r_v.num) > (int)(r_r_v->r_v.num))?(1):(0)));
		break;
	case BIGE:
		push_data(&vm_p->data_ptr, new_number(((int)(l_r_v->r_v.num) >= (int)(r_r_v->r_v.num))?(1):(0)));
		break;
	case LIT:
		push_data(&vm_p->data_ptr, new_number(((int)(l_r_v->r_v.num) < (int)(r_r_v->r_v.num))?(1):(0)));
		break;
	case LITE:
		push_data(&vm_p->data_ptr, new_number(((int)(l_r_v->r_v.num) <= (int)(r_r_v->r_v.num))?(1):(0)));
		break;
	case EQU:
		push_data(&vm_p->data_ptr, new_number(((int)(l_r_v->r_v.num) == (int)(r_r_v->r_v.num))?(1):(0)));
		break;
	case NEQU:
		push_data(&vm_p->data_ptr, new_number(((int)(l_r_v->r_v.num) != (int)(r_r_v->r_v.num))?(1):(0)));
		break;
	default:
		print("vm[error line: %d] not op inst!\n", a_i_p->line);
		return VM_ERROR;
	}

	return VM_SUCCESS;
}



// 뵽
vm_env* com_to_vm(com_env* com_p)
{
	free_vm_evn(&alex_vm_env);
	
	alex_vm_env.code_ptr = com_p->com_inst;			// ݱĻָ
	relloc_stack(&alex_vm_env.data_ptr);
	relloc_local(&alex_vm_env.data_ptr);
	relloc_call(&alex_vm_env.call_ptr);
	alex_vm_env.global_ptr = com_p->var_table.global_ptr;	// ȡ༭ȫֱ

	alex_vm_env.local_top = 0;
	alex_vm_env.pc = com_p->pc;
	alex_vm_env.reg_p = alex_vm_env.reg;
	alex_vm_env.glr[COM_LOCAL] = &(alex_vm_env.local_ptr.root_ptr);
	alex_vm_env.glr[COM_GLOBAL] = &(alex_vm_env.global_ptr.root_ptr);
	alex_vm_env.glr[COM_REG]  = &(alex_vm_env.reg_p);

	return &alex_vm_env;
}


void free_vm_evn(vm_env* vm_p)
{
	if(vm_p==NULL)
		return;

	if(vm_p->code_ptr.root_ptr)
		a_free(vm_p->code_ptr.root_ptr);

	if(vm_p->data_ptr.root_ptr)
		a_free(vm_p->data_ptr.root_ptr);

	if(vm_p->local_ptr.root_ptr)
		a_free(vm_p->local_ptr.root_ptr);

	if(vm_p->call_ptr.root_ptr)
		a_free(vm_p->call_ptr.root_ptr);

	if(vm_p->global_ptr.root_ptr)
		a_free(vm_p->global_ptr.root_ptr);

	memset(vm_p, 0, sizeof(vm_env));
}


c_inst* relloc_code(c_inst* c_i)
{
	if(c_i==NULL)
		return NULL;
	if(c_i->inst_len>=c_i->inst_size)
	{
		int n_len = (c_i->inst_size+CODE_MEM_LEN)*sizeof(alex_inst);
		alex_inst* n_a_i = (alex_inst*)a_malloc(n_len);
		memset(n_a_i, 0, n_len);
		memcpy(n_a_i, c_i->root_ptr, c_i->inst_size*sizeof(alex_inst));
		a_free(c_i->root_ptr);
		c_i->root_ptr = n_a_i;
		c_i->inst_size += CODE_MEM_LEN;
	}

	return c_i;
}

d_data* relloc_data(d_data* d_d, int d_len)
{
	if(d_d==NULL)
		return NULL;
	if(d_d->data_len>=d_d->data_size)
	{
		int n_len = (d_d->data_size+d_len)*sizeof(r_value);
		r_value* n_a_i = (r_value*)a_malloc(n_len);
		memset(n_a_i, 0, n_len);
		memcpy(n_a_i, d_d->root_ptr, d_d->data_size*sizeof(r_value));
		a_free(d_d->root_ptr);
		d_d->root_ptr = n_a_i;
		d_d->data_size += d_len;
	}
	
	return d_d;
}


void push_inst(c_inst* code_ptr, alex_inst a_i)
{
	if(code_ptr==NULL)
		return;
	
	relloc_code(code_ptr);
	code_ptr->root_ptr[(code_ptr->inst_len)++] = a_i;
}


void _push_data(d_data* d_ptr, r_value r_v)
{
	if(d_ptr==NULL)
		return;

	relloc_data(d_ptr, DATA_MEM_LEN);
	d_ptr->root_ptr[d_ptr->data_len++] = r_v;
}


r_value _top_data(d_data* d_ptr)
{
	r_value ret = {0};
	
	if(d_ptr==NULL || d_ptr->data_len<=0)
		return ret;

	return d_ptr->root_ptr[d_ptr->data_len-1];
}


r_value _pop_data(d_data* d_ptr)
{
	r_value  r_v = {0};
	if(d_ptr==NULL)
	{
		r_v.r_t = sym_type_error;
		return r_v;
	}
	else if(d_ptr->data_len <=0)
	{
		print("pop[error: ]you are try a nil data at stack!\n");
		r_v.r_t = sym_type_error;

		return r_v;
	}
	else
	{
		r_v = d_ptr->root_ptr[(--d_ptr->data_len)];
		//d_ptr->root_ptr[d_ptr->data_len].gc_p = NULL;
		//memset(&d_ptr->root_ptr[d_ptr->data_len], 0, sizeof(r_value));
		return r_v;
	}

}

void push_stack(vm_env* vm_p, r_value r_v)
{
	if(vm_p==NULL)
		return;
	
	relloc_stack(&vm_p->data_ptr);
	vm_p->data_ptr.root_ptr[(vm_p->data_ptr.data_len)++] = r_v;
	
	vm_p->top = vm_p->data_ptr.data_len;
}

// when func call, push pc, local_top
int push_call(vm_env* vm_p, r_value r_v)
{
	if(vm_p==NULL)
		return -1;
	
	relloc_global(&vm_p->call_ptr);
	vm_p->call_ptr.root_ptr[(vm_p->call_ptr.data_len)] = r_v;
	
	return (vm_p->call_ptr.data_len)++;	
}


void vm_print(vm_env* vm_p)
{
	print("\n\n-------vm state----------\n\n");
	print("code stack:    len[%.5d], size[%.5d], pc[%d]\n", vm_p->code_ptr.inst_len, vm_p->code_ptr.inst_size, vm_p->pc);	
	print("local stack:   len[%.5d], size[%.5d]\n", vm_p->local_ptr.data_len, vm_p->local_ptr.data_size);
	print("data stack:    len[%.5d], size[%.5d]\n", vm_p->data_ptr.data_len, vm_p->data_ptr.data_size);
	print("call stack:    len[%.5d], size[%.5d]\n", vm_p->call_ptr.data_len, vm_p->call_ptr.data_size);
	print("global stack:  len[%.5d], size[%.5d]\n", vm_p->global_ptr.data_len, vm_p->global_ptr.data_size);
	print("gc:            size[%.5d]\n", alex_gc.gc_size);
	print("--------vm state end---------\n");
}

ALEX_NUMBER pop_number(vm_env* vm_p)
{
	ALEX_NUMBER ret = 0;
	if(vm_p)
	{
		r_value r_v = {0};
		r_v = _pop_data(&vm_p->data_ptr);

		if(r_v.r_t != sym_type_error && r_v.r_t == sym_type_num)
			ret = r_v.r_v.num;
	}

	return ret;
}

int push_number(vm_env* vm_p, ALEX_NUMBER num)
{
	if(vm_p)
	{
		push_data(&vm_p->data_ptr, new_number(num));
		return VM_SUCCESS;
	}

	return VM_ERROR;
}

ALEX_STRING pop_string(vm_env* vm_p)
{
	ALEX_STRING ret = {0};
	if(vm_p)
	{
		r_value r_v = {0};
		r_v = _pop_data(&vm_p->data_ptr);

		if(r_v.r_t != sym_type_error && r_v.r_t == sym_type_string)
			ret = r_v.r_v.str;
	}

	return ret;
}

int push_string(vm_env* vm_p, char* str)
{
	if(vm_p && str)
	{
		push_data(&vm_p->data_ptr, gc_new_string(str, GC_LIVE));
		return VM_SUCCESS;
	}

	return VM_ERROR;
}

alex_al* pop_al(vm_env* vm_p)
{
	alex_al* ret = NULL;
	if(vm_p)
	{
		r_value r_v = {0};
		r_v = _pop_data(&vm_p->data_ptr);
		
		if(r_v.r_t != sym_type_error && r_v.r_t == sym_type_al)
			ret = r_v.r_v.al;
	}
	
	return ret;
}

r_value pop_func(vm_env* vm_p)
{
	r_value ret = {0};
	if(vm_p)
	{
		r_value r_v = _pop_data(&vm_p->data_ptr);

		if(r_v.r_t != sym_type_error && (r_v.r_t == sym_type_addr || r_v.r_t == sym_type_reg_func))
			ret = r_v;
	}

	return ret;
}

int push_al(vm_env* vm_p, r_value al)
{
	if(vm_p && al.r_t==sym_type_al)
	{
		push_data(&vm_p->data_ptr, al);
		return VM_SUCCESS;
	}

	return VM_ERROR;
}

void* pop_ptr(vm_env* vm_p)
{
	if(vm_p)
	{
		r_value r_v = {0};
		r_v = _pop_data(&vm_p->data_ptr);

		if(r_v.r_t != sym_type_error && r_v.r_t == sym_type_pointer)
			return r_v.r_v.ptr;
	}

	return NULL;
}

int push_ptr(vm_env* vm_p, void* p)
{
	if(vm_p)
	{
		r_value r_v = {0};
		r_v.r_t = sym_type_pointer;
		r_v.r_v.ptr = p;

		push_data(&vm_p->data_ptr, r_v);
		return VM_SUCCESS;
	}
	return VM_ERROR;
}
