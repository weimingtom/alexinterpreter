/*
	alex stack vm
*/
#include "alex_com.h"
#include "alex_vm.h"
#include "stdlib.h"
#include "alex_arrylist.h"
#include "alex_log.h"


vm_env alex_vm_env = {0};

int vm_push(vm_env* vm_p, alex_inst a_i);
int vm_tp(vm_env* vm_p, alex_inst a_i);
r_value vm_get_var(vm_env* vm_p, e_gl gl, int addr);
int push_call(vm_env* vm_p, r_value r_v);

/*

// 初始化虚拟机
vm_env* init_vm_env()
{
	free_vm_evn(&alex_vm);

	// fill code segment
	relloc_code(&alex_vm.code_ptr);
	relloc_stack(&alex_vm.data_ptr);
	relloc_local(&alex_vm.local_ptr);
	relloc_global(&alex_vm.global_ptr);
	relloc_call(&alex_vm.call_ptr);

	alex_vm.local_top =0;
	alex_vm.pc=0;
	
	return &alex_vm;
}
*/

// vm cpu
int alex_vm(vm_env* vm_p)
{
	alex_inst a_i = {0};

	if(vm_p==NULL)
		return VM_ERROR;
	
	for(;;)
	{
		if(vm_p->pc<0 || vm_p->pc>=vm_p->code_ptr.inst_len)
		{
			print("vm[error addr: %d] Attempts to access a error addr!\n", vm_p->pc);
			return	VM_ERROR_PC;
		}
		
		a_i = vm_p->code_ptr.root_ptr[vm_p->pc];
		//	parse inst 
		switch(vm_p->code_ptr.root_ptr[vm_p->pc].inst_type)
		{
		case END:
			{
				print("\n\n-----inst success end-----\n\n");
			}
			return VM_SUCCESS;
		case PUSH:
			vm_push(vm_p, a_i);
			break;
		case PUSHVAR:
			break;
		case POP:
			break;
		case NEWAL:
			break;
		case AL:
			break;
		case JFALSE:
			break;
		case JTRUE:
			break;
		case MOVE:
			break;
		case MOVEAL:
			break;
		case MOVEREG:
			break;
		case TABLE:		// ?
			break;

		case ADD:
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
			check_vm(vm_tp(vm_p, a_i));
			break;
		case CALL:
			break;
		case JUMP:
			break;
		case  RET:
			break;
		}

		vm_p->pc++;
	}

	return	VM_ERROR;
}

int vm_add(vm_env* vm_p)
{
	r_value l_r_v = {0};
	r_value r_r_v = {0};
	
	check_value(r_r_v=pop_data(&vm_p->data_ptr));
	check_value(l_r_v=pop_data(&vm_p->data_ptr));
	
	switch(l_r_v.r_t)
	{
	case sym_type_string:
		{
			switch(r_r_v.r_t)
			{
			case sym_type_string:
				{
				//	r_value r_v = ;
				}
				break;
			case sym_type_num:
				break;
			default:
				print("vm[error line: %d] the right op value is not allow!\n", vm_p->code_ptr.root_ptr[vm_p->pc].line);
				return VM_ERROR_ADD_OP;
			}
		}
		break;
	case sym_type_num:
		break;
	case sym_type_al:
		break;
	}
	return VM_SUCCESS;
}

int vm_pushvar(vm_env* vm_p, alex_inst a_i)
{
	r_value r_v = {0};
	check_value(r_v=vm_get_var(vm_p, a_i.gl, a_i.inst_value.r_v.addr));
	
	push_data(&vm_p->data_ptr, r_v);
	return VM_SUCCESS;
}


int vm_push(vm_env* vm_p, alex_inst a_i)
{
	push_data(&vm_p->data_ptr, a_i.inst_value);
	return VM_SUCCESS;
}


int vm_move(vm_env* vm_p, alex_inst a_i)
{
	r_value r_v = {0};
	
	check_value(r_v = top_data(&vm_p->data_ptr));
	check_vm(vm_set_var(vm_p, a_i.gl, a_i.inst_value.r_v.addr, r_v));
	return	VM_SUCCESS;
}


// 设置 gl addr 上地址上的值为 r_v
int vm_set_var(vm_env* vm_p, e_gl gl, int addr, r_value r_v)
{
	if(gl== COM_LOCAL)
	{
		if(addr>=0 && vm_p->local_top+addr < vm_p->local_ptr.data_len)
		{
			r_value* r_p = &vm_p->local_ptr.root_ptr[vm_p->local_top+addr];
		//	copy_value(r_p, r_v);
		}
		else
			return	VM_ERROR;
	}
	else if(gl == COM_GLOBAL)
	{
		if(addr<vm_p->global_ptr.data_len && addr >=0)
		{
			r_value* r_p = &vm_p->global_ptr.root_ptr[addr];
		//	copy_value(r_p->r_t r_v);
		}
		else
			return VM_ERROR;
	}

	return VM_SUCCESS;
}


r_value vm_get_var(vm_env* vm_p, e_gl gl, int addr)
{
	r_value ret = {0};
	if(gl== COM_LOCAL)
	{
		if(addr>=0 && vm_p->local_top+addr < vm_p->local_ptr.data_len)
			ret = vm_p->local_ptr.root_ptr[vm_p->local_top+addr];
	}
	else if(gl == COM_GLOBAL)
	{
		if(addr<vm_p->global_ptr.data_len && addr >=0)
			ret = vm_p->global_ptr.root_ptr[addr];
	}

	return ret;
}

int vm_call(vm_env* vm_p, alex_inst a_i)
{
	r_value r_v = {0};
	next_pc(vm_p);
	
	// get jump addr
	check_value(r_v=vm_get_var(vm_p, a_i.gl, a_i.inst_value.r_v.addr));
	
	if(r_v.r_t != sym_type_addr)
	{
		print("vm[error line: %d] the ide is not find func!\n", a_i.line);
		return VM_ERROR_NOT_IDE;
	}
	
	// record pc
	push_call(vm_p, new_addr(vm_p->pc));
	vm_p->pc = r_v.r_v.addr;

	// record local top
	push_call(vm_p, new_addr(vm_p->local_top));
	vm_p->local_top = vm_p->local_ptr.data_len;

	return VM_SUCCESS;
}


int vm_ret(vm_env* vm_p, alex_inst a_i)
{
	int i=0;
	r_value pc_value = {0};
	r_value local_top_value = {0};

	check_value(local_top_value=pop_data(&vm_p->call_ptr));
	check_value(pc_value=pop_data(&vm_p->call_ptr));

	// resume pc
	vm_p->pc = pc_value.r_v.addr;
	// free local stack memory  if var is string or al will free 
	for(i=vm_p->local_top; i<vm_p->local_ptr.data_len; i++)
	{
		r_value l_r_v = pop_data(&vm_p->local_ptr);
		free_value(&l_r_v);
	}

	vm_p->local_top = local_top_value.r_v.addr;
	return VM_SUCCESS;
}

int vm_tp(vm_env* vm_p, alex_inst a_i)
{
	r_value l_r_v ={0};
	r_value r_r_v ={0};
	r_value ret_value={0};

	check_value(r_r_v=pop_data(&vm_p->data_ptr));
	check_value(l_r_v=pop_data(&vm_p->data_ptr));

	if(l_r_v.r_t != sym_type_num || r_r_v.r_t != sym_type_num)
	{
		print("vm[error line: %d] the op value not number!\n", a_i.line);
		return VM_ERROR_OP_VALUE;
	}

	switch(a_i.inst_type)
	{
	case SUB:
		ret_value = new_number(l_r_v.r_v.num - r_r_v.r_v.num);
		break;
	case MUL:
		ret_value = new_number(l_r_v.r_v.num * r_r_v.r_v.num);
		break;
	case DEV:
		ret_value = new_number(l_r_v.r_v.num / r_r_v.r_v.num);
		break;
	case MOD:
		ret_value = new_number((int)(l_r_v.r_v.num) % (int)(r_r_v.r_v.num));
		break;
	case AND:
		ret_value = new_number( ((int)(l_r_v.r_v.num) && (int)(r_r_v.r_v.num))?(1):(0) );
		break;
	case OR:
		ret_value = new_number( ((int)(l_r_v.r_v.num) || (int)(r_r_v.r_v.num))?(1):(0) );
		break;
	case BIG:
		ret_value = new_number( ((int)(l_r_v.r_v.num) > (int)(r_r_v.r_v.num))?(1):(0) );
		break;
	case BIGE:
		ret_value = new_number( ((int)(l_r_v.r_v.num) >= (int)(r_r_v.r_v.num))?(1):(0) );
		break;
	case LIT:
		ret_value = new_number( ((int)(l_r_v.r_v.num) < (int)(r_r_v.r_v.num))?(1):(0) );
		break;
	case LITE:
		ret_value = new_number( ((int)(l_r_v.r_v.num) >= (int)(r_r_v.r_v.num))?(1):(0) );
		break;
	case EQU:
		ret_value = new_number( ((int)(l_r_v.r_v.num) == (int)(r_r_v.r_v.num))?(1):(0) );
		break;
	case NEQU:
		ret_value = new_number( ((int)(l_r_v.r_v.num) != (int)(r_r_v.r_v.num))?(1):(0) );
		break;
	default:
		print("vm[error line: %d] not op inst!\n", a_i.line);
		return VM_ERROR;
	}

	push_data(&vm_p->data_ptr,ret_value);

	return VM_SUCCESS;
}



// 编译到虚拟机的入口
vm_env* com_to_vm(com_env* com_p)
{
	free_vm_evn(&alex_vm_env);
	
	alex_vm_env.code_ptr = com_p->com_inst;			// 传递编译器传过来的汇编指令
	relloc_stack(&alex_vm_env.data_ptr);
	relloc_local(&alex_vm_env.data_ptr);
	relloc_call(&alex_vm_env.call_ptr);
	alex_vm_env.global_ptr = com_p->var_table.global_ptr;	// 获取编辑器解析的全局变量

	alex_vm_env.local_top = 0;
	alex_vm_env.pc = com_p->pc;

	return &alex_vm_env;
}


void free_vm_evn(vm_env* vm_p)
{
	if(vm_p==NULL)
		return;

	if(vm_p->code_ptr.root_ptr)
		free(vm_p->code_ptr.root_ptr);

	if(vm_p->data_ptr.root_ptr)
		free(vm_p->data_ptr.root_ptr);

	if(vm_p->local_ptr.root_ptr)
		free(vm_p->local_ptr.root_ptr);

	if(vm_p->global_ptr.root_ptr)
		free(vm_p->global_ptr.root_ptr);

	memset(vm_p, 0, sizeof(vm_env));
}


c_inst* relloc_code(c_inst* c_i)
{
	if(c_i==NULL)
		return NULL;
	if(c_i->inst_len>=c_i->inst_size)
	{
		int n_len = (c_i->inst_size+CODE_MEM_LEN)*sizeof(alex_inst);
		alex_inst* n_a_i = (alex_inst*)malloc(n_len);
		memset(n_a_i, 0, n_len);
		memcpy(n_a_i, c_i->root_ptr, c_i->inst_size);
		free(c_i->root_ptr);
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
		r_value* n_a_i = (r_value*)malloc(n_len);
		memset(n_a_i, 0, n_len);
		memcpy(n_a_i, d_d->root_ptr, d_d->data_size);
		free(d_d->root_ptr);
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


void push_data(d_data* d_ptr, r_value r_v)
{
	if(d_ptr==NULL)
		return;
	
	relloc_data(d_ptr, DATA_MEM_LEN);
	d_ptr->root_ptr[d_ptr->data_len++] = r_v;
}

r_value top_data(d_data* d_ptr)
{
	r_value ret = {0};
	
	if(d_ptr==NULL || d_ptr->data_len<=0)
		return ret;

	return d_ptr->root_ptr[d_ptr->data_len-1];
}

r_value pop_data(d_data* d_ptr)
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


// push local ide  return is offset local_top 
int push_local(vm_env* vm_p, r_value r_v)
{
	if(vm_p==NULL)
		return -1;
	
	relloc_local(&vm_p->local_ptr);
	vm_p->local_ptr.root_ptr[(vm_p->local_ptr.data_len)] = r_v;
	
	return ((vm_p->local_ptr.data_len++) -vm_p->local_top);
}


// push global ide, return is data_len, because global stack is only one
int push_global(vm_env* vm_p, r_value r_v)
{
	if(vm_p==NULL)
		return -1;
	
	relloc_global(&vm_p->global_ptr);
	vm_p->global_ptr.root_ptr[(vm_p->global_ptr.data_len)] = r_v;
	
	return (vm_p->global_ptr.data_len)++;
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
