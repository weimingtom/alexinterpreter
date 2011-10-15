#ifndef		_ALEX_WINDOW_H_
#define		_ALEX_WINDOW_H_
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <tchar.h>
#include "alex_interpret.h"


enum mes{
	mes_pen=0x11234,
	mes_key
};

ret_node* alex_create_window(ret_node* arg_list);
ret_node* alex_message_box(ret_node* arg_list);
ret_node* alex_rectangle(ret_node* arg_list);
int alex_window(ret_node* arg_list);
ret_node* alex_reg_pen(ret_node* arg_list);
ret_node* alex_reg_key(ret_node* arg_list);

#endif