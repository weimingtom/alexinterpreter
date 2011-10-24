/*
	alex stack vm
*/

#include "alex_vm.h"
#include "stdlib.h"




vm_env init_vm_env()
{
	vm_env ret_vm_env = {0};

	// fill code segment

	relloc_code(&ret_vm_env.code_ptr);
	relloc_stack(&ret_vm_env.data_ptr);
	relloc_local(&ret_vm_env.local_ptr);

	ret_vm_env.local_top =0;
	ret_vm_env.pc=0;

	return ret_vm_env;
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


void push_inst(vm_env* vm_p, alex_inst a_i)
{
	if(vm_p==NULL)
		return;

	relloc_code(&vm_p->code_ptr);
	vm_p->code_ptr.root_ptr[(vm_p->code_ptr.inst_len)++] = a_i;
}


void push_stack(vm_env* vm_p, r_value r_v)
{
	if(vm_p==NULL)
		return;

	relloc_stack(&vm_p->data_ptr);
	vm_p->data_ptr.root_ptr[(vm_p->data_ptr.data_len)++] = r_v;

	vm_p->top = vm_p->data_ptr.data_len;
}


void push_local(vm_env* vm_p, r_value r_v)
{
	if(vm_p==NULL)
		return;

	relloc_local(&vm_p->local_ptr);
	vm_p->local_ptr.root_ptr[(vm_p->local_ptr.data_len)++] = r_v;
}