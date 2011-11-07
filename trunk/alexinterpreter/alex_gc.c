#include "alex_gc.h"
#include "alex_sym.h"
#include "alex_symbol_table.h"


a_gc alex_gc = {0};


r_value new_string(char* str)
{
	r_value ret = {0};
	ret.r_t = sym_type_string;
	ret.r_v.str = alex_string(str);
	
	return ret;
}


