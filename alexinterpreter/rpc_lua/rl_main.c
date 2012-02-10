#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <windows.h>
#include <stdio.h>
#include "llp.h"

static int rpc_value_in(lua_State* L, int inx, llp_mes* rpc_lua_data);
static int rpc_out_value(lua_State* L, llp_mes* rpc_lua_data);

enum {
	e_null,
	e_string,
	e_number,
	e_bool,
	e_table
};

static llp_mes* rpc_lua  = NULL;
static llp_mes* r_rpc_lua = NULL;
static int rpc_write_table(lua_State* L, int inx, llp_mes* rpc_table)
{
	if(lua_type(L, inx)!= LUA_TTABLE)
		return LP_FAIL;
	
	lua_pushnil(L);			// the nill as table default key
	while(lua_next(L, inx))
	{
		int top = lua_gettop(L);
		llp_mes* table_value = llp_Wmes_message(rpc_table, "table_value");
		rpc_value_in(L, top, llp_Wmes_message(table_value, "value"));		// write value
		
		top--;
		switch(lua_type(L, top))
		{
		case LUA_TNUMBER:		// if key is number
			llp_Wmes_int32(table_value, "n_key", (llp_int32)lua_tonumber(L, top));
			break;
		case LUA_TSTRING:		// if key is string
			llp_Wmes_string(table_value, "s_key", (char*)lua_tostring(L, top));
			break;
		default: 
			return LP_FAIL;
		}
		lua_pop(L, 1);
	}
	return LP_TRUE;
}

static int rpc_value_in(lua_State* L, int inx, llp_mes* rpc_lua_data)
{
	switch(lua_type(L, inx))
	{
	case LUA_TSTRING:
		{
			llp_Wmes_int32(rpc_lua_data,  "rpc_data_type", e_string);
			llp_Wmes_string(rpc_lua_data, "rpc_str", (char*)lua_tostring(L, inx));
		}
		break;
	case LUA_TNUMBER:
		{
			llp_Wmes_int32(rpc_lua_data, "rpc_data_type", e_number);
			llp_Wmes_int32(rpc_lua_data, "rpc_number", (llp_int32)lua_tonumber(L, inx));
		}
		break;
	case LUA_TBOOLEAN:
		{
			llp_Wmes_int32(rpc_lua_data, "rpc_data_type", e_bool);
			llp_Wmes_int32(rpc_lua_data, "rpc_bool", lua_toboolean(L, inx));
		}
		break;
	case LUA_TTABLE:
		{
			llp_Wmes_int32(rpc_lua_data, "rpc_data_type", e_table);
			rpc_write_table(L, inx, llp_Wmes_message(rpc_lua_data, "rpc_table"));
		}
		break;
	default:
		break;
	}

	return LP_TRUE;
}

static slice* rpc_in(lua_State* L, int arg_len)
{
	int i=0;
	llp_message_clr(rpc_lua);		// clear rpc_mes data
	
	for(i=1; i<=arg_len; i++)
	{
		rpc_value_in(L, lua_gettop(L), llp_Wmes_message(rpc_lua, "rpc_ret"));
		lua_pop(L, 1);
	}

	return llp_out_message(rpc_lua);
	// send to server
} 

static int rpc_out_table(lua_State* L, llp_mes* rpc_table)
{
	int i=0;
	int len = llp_Rmes_size(rpc_table, "table_value");
	int top = -1;

	lua_createtable(L, len, len);
	top = lua_gettop(L);
	for(i=0; i<len; i++)
	{
		llp_mes* table_value = llp_Rmes_message(rpc_table, "table_value", i);
		int n_key = llp_Rmes_int32(table_value, "n_key", 0);
		char* s_key = llp_Rmes_string(table_value, "s_key", 0);

		if(s_key)
		{
			rpc_out_value(L, llp_Rmes_message(table_value, "value", 0));		// set value
			lua_setfield(L, top, s_key);										// set key
		}
		else
		{
			lua_pushnumber(L, n_key);
			rpc_out_value(L, llp_Rmes_message(table_value, "value", 0));		// set value
			lua_settable(L, top);
		}
	}
	return 1;
}

static int rpc_out_value(lua_State* L, llp_mes* rpc_lua_data)
{
	switch(llp_Rmes_int32(rpc_lua_data, "rpc_data_type", 0))
	{
	case e_string:
		{
			lua_pushstring(L, llp_Rmes_string(rpc_lua_data, "rpc_str", 0));
		}
		break;
	case e_bool:
		{
			lua_pushboolean(L, llp_Rmes_int32(rpc_lua_data, "rpc_bool", 0));
		}
		break;
	case e_number:
		{
			lua_pushnumber(L, llp_Rmes_int32(rpc_lua_data, "rpc_number", 0));
		}
		break;
	case e_table:
		{
			rpc_out_table(L, llp_Rmes_message(rpc_lua_data, "rpc_table", 0));
		}
		break;
	default:
		return 0;
	}
	return 1;
}

static int rpc_out(lua_State* L, slice* in, llp_mes* lm)
{
	int i=0, len=0;
	llp_in_message(in, lm);		// get ms
	len = llp_Rmes_size(lm, "rpc_ret");
	for(i=len-1; i>=0; i--)
	{
		llp_mes* rpc_ret = llp_Rmes_message(lm, "rpc_ret", i);
		rpc_out_value(L, rpc_ret);
	}

	return len;
}

  
static int lua_rpc_call(lua_State* L)
{
	int t_len =0;
	slice* in = NULL;
	int ret_len = 0;
	int b_top = -1;
	int i=0;
	int len = 0;
	char* func = NULL;
	llp_mes* rpc_ret = NULL;
	
	// send to server 
	llp_in_message(rpc_in(L, lua_gettop(L)), r_rpc_lua);			// 获得client 传送过来的rpc包，进行反序列化生成message obj
	len = llp_Rmes_size(r_rpc_lua, "rpc_ret");
	rpc_ret = llp_Rmes_message(r_rpc_lua, "rpc_ret", len-1);	// 获得rpc_call函数名
	func = llp_Rmes_string(rpc_ret, "rpc_str", 0);
	
	b_top = lua_gettop(L);
	lua_getglobal(L, func);
	for(i=len-2; i>=0; i--)
	{
		rpc_ret = llp_Rmes_message(r_rpc_lua, "rpc_ret", i);		// 获得参数
		rpc_out_value(L, rpc_ret);		// 将参数展开到stack上
	}
	
	if(lua_pcall(L, len-1, LUA_MULTRET, 0))
	{
		const char* error = lua_tostring(L, -1);
	}
	ret_len = lua_gettop(L)-b_top;		// 获得返回值个数
	
	// back to client
	in = rpc_in(L, ret_len);			// 从栈上获取返回值，同时将其序列化
	lua_settop(L, b_top);
	llp_message_clr(r_rpc_lua);
	len = rpc_out(L, in, r_rpc_lua);

	return len;
} 


int main()
{
	int ret = 0;
	lua_State* L = lua_open();
	llp_env* env = llp_new_env();
	luaopen_base(L);
	luaopen_table(L);
 
	ret = llp_reg_mes(env, "F:\\code\\alex\\rpc_lua\\rpc_lua.mes.lpb");
	rpc_lua = llp_message_new(env, "rpc_lua");
	r_rpc_lua = llp_message_new(env, "rpc_lua");

	lua_register(L, "rpc_call", lua_rpc_call);
	if(luaL_dofile(L, "F:\\code\\alex\\rpc_lua\\rpc_lua.lua") != 0)
	{
		printf("[error]: %s", lua_tostring(L, -1));
	}

	lua_close(L);
	llp_message_free(rpc_lua);
	llp_message_free(r_rpc_lua);
	llp_free_env(env);
	return 0;
}
