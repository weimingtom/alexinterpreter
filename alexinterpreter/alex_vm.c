/*
	alex stack vm
*/

#include "alex_vm.h"
#include "stdlib.h"

vm_env alex_vm = {0};

// ³õÊ¼»¯ÐéÄâ»ú
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
		r_v = d_ptr->root_ptr[d_ptr->data_len--];
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
		return;
	
	relloc_local(&vm_p->local_ptr);
	vm_p->local_ptr.root_ptr[(vm_p->local_ptr.data_len)] = r_v;
	
	return ((vm_p->local_ptr.data_len++) -vm_p->local_top);
}


// push global ide, return is data_len, because global stack is only one
int push_global(vm_env* vm_p, r_value r_v)
{
	if(vm_p==NULL)
		return;
	
	relloc_global(&vm_p->global_ptr);
	vm_p->global_ptr.root_ptr[(vm_p->global_ptr.data_len)] = r_v;
	
	return (vm_p->global_ptr.data_len)++;
}

// when func call, push pc, local_top and arg_num
int push_call(vm_env* vm_p, r_value r_v)
{
	if(vm_p==NULL)
		return;
	
	relloc_global(&vm_p->call_ptr);
	vm_p->call_ptr.root_ptr[(vm_p->call_ptr.data_len)] = r_v;
	
	return (vm_p->call_ptr.data_len)++;	
}
