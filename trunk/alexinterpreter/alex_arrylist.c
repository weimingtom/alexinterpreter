#include "alex_interpret.h"
#include "alex_arrylist.h"
#include <stdlib.h>


alex_al* relloc_al(alex_al* al)
{
	if(al==NULL)
		return NULL;

	if(al->al_v==NULL)
	{
		al->al_v = (r_value*)malloc(sizeof(r_value)*DEF_AL_SIZE);
		memset(al->al_v, 0, sizeof(r_value)*DEF_AL_SIZE);
		al->al_size = DEF_AL_SIZE;
		al->al_len=0;
	}
	else
	{
		if(al->al_size <= al->al_len)
		{
			int size = sizeof(r_value)*(al->al_size+DEF_AL_SIZE);
			r_value* n_al_v = (r_value*)malloc(size);
			memset(n_al_v, 0, size);
			memcpy(n_al_v, al->al_v, sizeof(r_value)*al->al_size);
			free(al->al_v);
			al->al_v = n_al_v;
			al->al_size +=DEF_AL_SIZE; 
		}
	}

	return al;
}

alex_al* new_al()
{
	alex_al* ret_al = (alex_al*)malloc(sizeof(alex_al));
	memset(ret_al, 0, sizeof(alex_al));
	relloc_al(ret_al);
//	ret_al->count = 1;
	return ret_al;
}

r_value* get_al(alex_al* al, int inx)
{
	r_value t_v = {0};
	int i=0;
	int size= sizeof(r_value);
	if(al==NULL || al->al_len < inx || inx <0 )
		return NULL;
	
	for(i=0; i<al->al_len; i++)
	{
		t_v = (al->al_v)[i];
	}

	return &((al->al_v)[inx]);
}


r_value add_al(alex_al* al, r_value r_v)
{
	relloc_al(al);
	if(r_v.r_t == sym_type_string)
	{
		(al->al_v[al->al_len]).r_v.str = alex_string(r_v.r_v.str.s_ptr);
		(al->al_v[al->al_len++]).r_t = r_v.r_t;
	}
	else
		al->al_v[al->al_len++] = r_v;

	return al->al_v[al->al_len-1];
}

void free_value(r_value* rv)
{
	if(rv == NULL)
		return ;

	switch( rv->r_t )
	{
	case sym_type_string:
		free_string( &(rv->r_v.str));
		break;
	case sym_type_al:
		del_al( (rv->r_v.al) );
		break;
	}

	memset(rv, 0, sizeof(r_value));
}

void del_al(alex_al* al)
{
	int i=0;
	if(al==NULL)
		return ;
	
	if((--al->count)>0)
	{
		return;
	}
	for(i=0; i<al->al_len; i++)
	{
		free_value( &(al->al_v[i]) );
	}

	free(al->al_v);
	memset(al, 0, sizeof(alex_al));
	free(al);
}


